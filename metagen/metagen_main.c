#include "base/base_inc.h"
#include "os/os_inc.h"
#include "mdesk/mdesk.h"

#include "base/base_inc.c"
#include "os/os_inc.c"
#include "mdesk/mdesk.c"

#include <stdio.h>

internal void entry_point(void)
{
	// REVIEW: right now all the memory for all the worked on files stays
	// alive until this is released, is it worth doing an arena per file
	// (i.e. loop iteration) and just releasing once the work in the file
	// is done?
	Temp scratch = scratch_begin(0, 0);

	// TODO(beau): make os file iters work on multiple folders
	OS_FileIter *iter = os_file_iter_begin(
		scratch.arena,
		str8_lit("."),
		OS_FileIterFlag_SkipFolders
		| OS_FileIterFlag_RecurseFolders
		| OS_FileIterFlag_SkipHidden
	);
	OS_FileInfo info = zero_struct;
	while (os_file_iter_next(scratch.arena, iter, &info))
	{
		String8 extension = str8_skip_last_dot(info.name);
		B32 is_c_lang = 0
		     || str8_match(extension, str8_lit("c"), 0)
		     || str8_match(extension, str8_lit("h"), 0)
		     || str8_match(extension, str8_lit("m"), 0),
		    is_meta = str8_match(extension, str8_lit("meta"), 0);

		if (is_meta || is_c_lang)
		{
			String8 source = os_data_from_file_path(scratch.arena, info.name);
			if (is_meta)
			{
				MD_TokenizeResult tokenize = md_tokens_from_source(scratch.arena, source);

				for (U64 ix = 0; ix < tokenize.tokens.count; ix++)
				{
					MD_Token tok = tokenize.tokens.tokens[ix];
					printf("lexeme: '%.*s'\n", str8_varg(tok.source));
				}

				for (MD_Message *m = tokenize.messages.first; m != 0; m = m->next)
				{
					const local_persist
					char *msg_kind_prefix_table[MD_MessageKind_COUNT] = {
						[MD_MessageKind_Warning   ] = "warning",
						[MD_MessageKind_Error     ] = "error",
						[MD_MessageKind_FatalError] = "error",
					};
					const char *prefix = msg_kind_prefix_table[m->kind];
					AssertAlways(prefix != 0);
					MD_Token token = tokenize.tokens.tokens[m->tokens_ix];
					Vec2U64 pos = str8_pos_from_offset(source, token.source.buffer - source.buffer);
					printf(
						"%.*s(%llu, %llu): %s: %.*s\n",
						str8_varg(info.name),
						pos.line,
						pos.column,
						prefix,
						str8_varg(m->string)
					);
				}

				if (tokenize.messages.worst_message >= MD_MessageKind_Error)
				{
					// TODO: indicate that parsing won't happen
					continue;
				}

				MD_ParseResult parsed = md_parse_from_tokens_source(
					scratch.arena,
					tokenize.tokens,
					source
				);
				#if 0
				for (MD_Message *m = parsed.messages.first; m != 0; m = m->next)
				{
					// TODO:
				}
				if (parse.messages.worst_message >= MD_MessageKind_Error)
				{
					// TODO: indicate that further processing won't happen
					continue;
				}
				#endif
			}
			else // is_c_lang
			{
				// generate from c markup
			}
		}
	}
	os_file_iter_end(iter);

	scratch_end(scratch);
}
