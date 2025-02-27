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

// REVIEW XXX: enforce ordering of prints to stdout and stderr relative to each other? fflush()
internal void entry_point(void)
{
	Arena *iter_arena = arena_default,
	      *work_arena = arena_default;

	// <project root>/build/.. i.e. project root
	String8 project_root = str8_chop_last_slash(g_os_state.process_info.exe_folder);
	printf("searching '%.*s'\n\n", str8_varg(project_root));
	OS_FileIter *iter = os_file_iter_begin(
		iter_arena,
		project_root,
		OS_FileIterFlag_SkipFolders
		| OS_FileIterFlag_RecurseFolders
		| OS_FileIterFlag_SkipHidden
	);
	OS_FileInfo info = zero_struct;
	while (os_file_iter_next(iter_arena, iter, &info))
	{
		String8 extension = str8_skip_last_char(info.name, '.');
		B32 is_c_lang = 0
		     || str8_match(extension, str8_lit("c"), 0)
		     || str8_match(extension, str8_lit("h"), 0)
		     || str8_match(extension, str8_lit("m"), 0),
		    is_meta = str8_match(extension, str8_lit("meta"), 0);

		if (is_meta || is_c_lang)
		{
			if (info.name.length == extension.length + 1) // file has no name before .?
			{
				fprintf(stderr, "warning: file '%.*s' has no name before '.' - skipping\n", str8_varg(info.name));
				goto work_cleanup;
			}

			String8 source = os_data_from_file_path(work_arena, info.name);
			if (is_meta)
			{
				#define mg_print_message(f, m) fprintf(stderr, "%.*s%.*s\n", str8_varg(f), str8_varg((m)->string))
				printf("processing mdesk file '%.*s'\n", str8_varg(info.name));

				MD_TokenizeResult tokenize = md_tokens_from_source(work_arena, source);

				for (MD_Message *m = tokenize.messages.first; m != 0; m = m->next)
				{
					m->token = &tokenize.tokens.tokens[m->tokens_ix]; // XXX: nothing is done with the token ix
					mg_print_message(info.name, m);
				}

				if (tokenize.messages.worst_message >= MD_MessageKind_Error)
					goto meta_fail;

				MD_ParseResult parsed = md_parse_from_tokens_source(
					work_arena,
					tokenize.tokens,
					source
				);

				for (MD_Message *m = parsed.messages.first; m != 0; m = m->next)
					mg_print_message(info.name, m);

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
					mg_print_message(info.name, m);

				if (check_messages.worst_message >= MD_MessageKind_Error)
					goto meta_fail;

				MG_GenResult generated = mg_generate_from_checked(
					work_arena,
					parsed.root,
					parsed.global_stab,
					str8(project_root.buffer, project_root.length + 1), // HACK: just giving the slash back rn
					source
				);

				for (MD_Message *m = generated.messages.first; m != 0; m = m->next)
					mg_print_message(info.name, m);

				if (generated.messages.worst_message >= MD_MessageKind_Error)
					goto meta_fail;

				// TODO:
				//  message saying generation was successful and output will happen

				Assert(extension.length > 0 && extension.buffer[-1] == '.');
				String8 mdesk              = str8_skip_last_slash(info.name), // name of mdesk file
				        folder             = str8_region(info.name.buffer, mdesk.buffer), // folder of source file including last slash
				        mdesk_no_extension = str8_region(mdesk.buffer, extension.buffer), // filename without extension but including last '.' XXX var name
					gen_file_upper     = upper_from_str8(
						work_arena,
						str8(mdesk_no_extension.buffer, mdesk_no_extension.length - 1)
					);

				Assert(folder.length > 0 && char_is_slash(folder.buffer[folder.length - 1]) && mdesk_no_extension.length > 0 && mdesk_no_extension.buffer[mdesk_no_extension.length - 1] == '.');

				U8 path_slash = folder.buffer[folder.length - 1];
				// REVIEW: with above?
				String8 gen_folder_name = str8_lit("generated"),
					gen_file = push_str8f(
						work_arena,
						"%S%S%c%Smeta.h",
						folder,
						gen_folder_name,
						path_slash,
						mdesk_no_extension
					),
					generated_folder = str8(
						gen_file.buffer,
						folder.length + gen_folder_name.length
					);

				os_create_folder(generated_folder); // REVIEW: if fails?

				{
					// XXX: remove dependency on stdio
					String8 gen_info = str8_lit("/* GENERATED */\n\n");
					// write header file
					FILE *fd = fopen((char *)gen_file.buffer, "w");
					fwrite(gen_info.buffer, sizeof(*gen_info.buffer), gen_info.length, fd);
					String8 include_guard = push_str8f(
						work_arena,
						"#ifndef %S_META_H\n#define %S_META_H\n\n",
						gen_file_upper,
						gen_file_upper
					);
					fwrite(include_guard.buffer, sizeof(*include_guard.buffer), include_guard.length, fd);
					fwrite(generated.h_file.buffer, sizeof(*generated.h_file.buffer), generated.h_file.length, fd);
					String8 include_guard_end = push_str8f(
						work_arena,
						"\n\n#endif // %S_META_H",
						gen_file_upper
					);
					fwrite(include_guard_end.buffer, sizeof(*include_guard_end.buffer), include_guard_end.length, fd);
					Assert(!fclose(fd));

					gen_file.buffer[gen_file.length - 1] = 'c'; // XXX
					fd = fopen((char *)gen_file.buffer, "w");
					fwrite(gen_info.buffer, sizeof(*gen_info.buffer), gen_info.length, fd);
					fwrite(generated.c_file.buffer, sizeof(*generated.c_file.buffer), generated.c_file.length, fd);
					Assert(!fclose(fd));
				}

				goto work_cleanup; // skip fail message
				meta_fail:;
				fprintf(stderr, "%.*s: failed to process mdesk file, skipping", str8_varg(info.name));
				#undef mg_print_message
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

	arena_release(iter_arena);
	arena_release(work_arena);
}
