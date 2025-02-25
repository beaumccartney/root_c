// REVIEW:
//  some kind of cache structure for byte positions to line and column positions
//  how to tab indents affect error positions? should I not allow tab indents?

#include "base/base_inc.h"
#include "os/os_inc.h"
#include "mdesk/mdesk.h"
#include "metagen.h"

#include "base/base_inc.c"
#include "os/os_inc.c"
#include "mdesk/mdesk.c"
#include "metagen.c"

#include <stdio.h>

internal void entry_point(void)
{
	// REVIEW: right now all the memory for all the worked on files stays
	// alive until this is released, is it worth doing an arena per file
	// (i.e. loop iteration) and just releasing once the work in the file
	// is done?
	Arena *iter_arena = arena_default,
	      *work_arena = arena_default;

	String8List print_messages = zero_struct;

	// TODO(beau): make os file iters work on multiple folders
	OS_FileIter *iter = os_file_iter_begin(
		iter_arena,
		str8_lit("."),
		OS_FileIterFlag_SkipFolders
		| OS_FileIterFlag_RecurseFolders
		| OS_FileIterFlag_SkipHidden
	);
	OS_FileInfo info = zero_struct;
	while (os_file_iter_next(iter_arena, iter, &info))
	{
		String8 extension = str8_skip_last_dot(info.name);
		B32 is_c_lang = 0
		     || str8_match(extension, str8_lit("c"), 0)
		     || str8_match(extension, str8_lit("h"), 0)
		     || str8_match(extension, str8_lit("m"), 0),
		    is_meta = str8_match(extension, str8_lit("meta"), 0);

		if (is_meta || is_c_lang)
		{
			if (info.name.length == extension.length + 1) // file has no name before .?
			{
				// TODO: warn about nameless file and that its being skipped
				goto work_cleanup;
			}

			String8 source = os_data_from_file_path(work_arena, info.name);
			if (is_meta)
			{
				MD_TokenizeResult tokenize = md_tokens_from_source(work_arena, source);

				for (MD_Message *m = tokenize.messages.first; m != 0; m = m->next)
				{
					m->token = &tokenize.tokens.tokens[m->tokens_ix]; // XXX: nothing is done with the token ix
					mg_print_message_push(iter_arena, &print_messages, m->string, info.name);
				}

				if (tokenize.messages.worst_message >= MD_MessageKind_Error)
					goto meta_fail;

				MD_ParseResult parsed = md_parse_from_tokens_source(
					work_arena,
					tokenize.tokens,
					source
				);

				for (MD_Message *m = parsed.messages.first; m != 0; m = m->next)
					mg_print_message_push(iter_arena, &print_messages, m->string, info.name);

				if (parsed.messages.worst_message >= MD_MessageKind_Error)
					goto meta_fail;

				// REVIEW: bundle root and global_stab into one struct?
				MD_MessageList check_messages = md_check_parsed(
					work_arena,
					parsed.root,
					parsed.global_stab,
					source
				);

				for (MD_Message *m = check_messages.first; m != 0; m = m->next)
					mg_print_message_push(iter_arena, &print_messages, m->string, info.name);

				if (check_messages.worst_message >= MD_MessageKind_Error)
					goto meta_fail;

				MG_GenResult generated = mg_generate_from_checked(work_arena, parsed.root, parsed.global_stab, source);

				for (MD_Message *m = generated.messages.first; m != 0; m = m->next)
					mg_print_message_push(iter_arena, &print_messages, m->string, info.name);

				if (generated.messages.worst_message >= MD_MessageKind_Error)
					goto meta_fail;

				goto work_cleanup; // skip fail message
				meta_fail:;
				str8_list_pushf(
					iter_arena,
					&print_messages,
					"%S: failed to process mdesk file, skipping",
					info.name
				);
			}
			else // is_c_lang
			{
				// generate from c markup
			}

			work_cleanup:;
			arena_clear(work_arena);
		}
	}
	os_file_iter_end(iter);

	StringJoin params = {
		.sep = str8_lit("\n"),
	};
	String8 print_string = str8_list_join(
		iter_arena,
		print_messages,
		&params
	);
	fprintf(stderr, "%.*s", str8_varg(print_string));

	arena_release(iter_arena);
	arena_release(work_arena);
}
