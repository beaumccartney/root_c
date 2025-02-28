// REVIEW: move to mdesk - useful functionality to include with the file format in general
internal MG_GenResult
mg_generate_from_checked(Arena *arena, MD_AST *root, MD_SymbolTableEntry *stab_root, String8 gen_folder, String8 source)
{
	MG_GenResult result = zero_struct;

	String8List h_file_enums           = zero_struct,
		    h_file_structs         = zero_struct,
		    h_file_functions       = zero_struct,
		    h_file_arrays          = zero_struct,
		    h_file_embeddedstrings = zero_struct,
		    h_file_embeddedfiles   = zero_struct,
		    h_file_default         = zero_struct,
		    c_file_functions       = zero_struct,
		    c_file_arrays          = zero_struct,
		    c_file_default         = zero_struct;

	Temp scratch = scratch_begin(&arena, 1);

	for (MD_AST *global_directive = root->first; global_directive != 0; global_directive = global_directive->next)
	{
		String8List *const md_ast_to_target_location_table[MD_ASTKind_COUNT] = {
			[MD_ASTKind_DirectiveGenH       ] = &h_file_default,
			[MD_ASTKind_DirectiveGenC       ] = &c_file_default,
			[MD_ASTKind_DirectiveEnum       ] = &h_file_enums,
			[MD_ASTKind_DirectiveStruct     ] = &h_file_structs,
			[MD_ASTKind_DirectiveArray      ] = &c_file_arrays,
			[MD_ASTKind_DirectiveEmbedString] = &h_file_embeddedstrings,
			[MD_ASTKind_DirectiveEmbedFile  ] = &h_file_embeddedfiles,
		};

		String8List *target_location = md_ast_to_target_location_table[global_directive->kind];
		Assert(global_directive->kind == MD_ASTKind_DirectiveTable || target_location);
		switch (global_directive->kind)
		{
			case MD_ASTKind_DirectiveEmbedString:
			case MD_ASTKind_DirectiveEmbedFile: {
				MD_AST *child = global_directive->first;
				Assert(child->kind == MD_ASTKind_Ident && child->token->kind == MD_TokenKind_Ident);
				String8 embedded_varname = child->token->source;

				child = child->next;
				String8 gen_string = child->token->source;
				if (global_directive->kind == MD_ASTKind_DirectiveEmbedFile)
				{
					Assert(child->kind        == MD_ASTKind_StringLit
					    && child->token->kind == MD_TokenKind_StringLit
					    && gen_string.length  >= 2); // string literals should have at least two quotes
					gen_string = str8(gen_string.buffer + 1, gen_string.length - 2); // trim delmiting quotes
				}
				else
				{
					Assert(global_directive->kind == MD_ASTKind_DirectiveEmbedString
					    && child->kind            == MD_ASTKind_RawStringLit
					    && child->token->kind     == MD_TokenKind_RawStringLit
					    && gen_string.length      >= 6 // room for delimiting triple-quotes
					);

					// REVIEW: don't trim whitespace? perhaps there's somewhere where leading/trailing whitespace is significant?
					gen_string = str8_trim_whitespace(str8(gen_string.buffer + 3, gen_string.length - 6)); // trim delimiting """ and any whitespace
				}

				if (gen_string.length == 0)
				{
					md_messagelist_pushf(
						arena,
						&result.messages,
						source,
						gen_string.buffer,
						child->token,
						global_directive,
						MD_MessageKind_Warning,
						"empty argument to %S",
						global_directive->token->source
					);
					break;
				}

				FileProperties props = zero_struct;
				if (global_directive->kind == MD_ASTKind_DirectiveEmbedFile)
				{
					Assert(gen_string.length > 2 && gen_folder.length > 0); // REVIEW
					// REVIEW: fix the slashes, possibly in a dedicated path_cat api
					//  fix in gen_folder, and gen_string. gen_string is important because its provided by the user, so one slash should work everywhere
					Assert(char_is_slash(gen_folder.buffer[gen_folder.length - 1]) && !char_is_slash(gen_string.buffer[0])); // folder ends in a slash and file doesn't begin with one
					String8 filepath = push_str8_cat(
						scratch.arena,
						gen_folder,
						gen_string
					);
					OS_Handle file = os_file_open(OS_AccessFlag_Read, filepath);

					MD_MessageKind message_kind = MD_MessageKind_NULL;
					char *message_format = 0; // if set MUST HAVE ONE FORMAT SPECIFIER %S
					if (os_handle_match(file, os_handle_zero))
					{
						message_kind   = MD_MessageKind_Error; // REVIEW: should this stop codegen? perhaps have errors still proceed and fatal errors actually stop. if so then I'll need to do a small refactor to do that
						message_format = "failed to open @embed_file target %S"; // REVIEW: better message
					}
					else
					{
						props = os_properties_from_file(file);
						if (props.flags & FilePropertyFlag_IsFolder)
						{
							message_kind   = MD_MessageKind_Error; // REVIEW: should this stop codegen? same as above
							message_format = "@embed_file target %S is a folder";
						}
						else if (props.size == 0)
						{
							message_kind   = MD_MessageKind_Warning;
							message_format = "@embed_file target %S is empty";
						}
					}
					if (message_kind != MD_MessageKind_NULL)
					{
						Assert(message_format);
						md_messagelist_pushf(
							arena,
							&result.messages,
							source,
							child->token->source.buffer, // opening '"' of the string literal
							child->token,
							global_directive,
							message_kind,
							message_format,
							filepath
						);
						if (message_kind >= MD_MessageKind_Error) // still generate data for empty files
							break; // REVIEW: needed?
					}
					Assert(!(props.flags & FilePropertyFlag_IsFolder));
					gen_string = os_string_from_file_range(scratch.arena, file, rng_1u64(0, props.size));

					String8 bytes_varname = push_str8_cat(
						scratch.arena,
						embedded_varname,
						str8_lit("__bytes")
					);
					str8_list_pushf(
						scratch.arena,
						target_location,
						"const global U8 %S[] =\n{\n",
						bytes_varname
					);
					if (props.size > 0)
					{
						#define MG_BYTELIT_STRLEN 5 // strlen("0xXX,")
						#define MG_BYTELITS_PER_LINE 40
						Assert(props.size > 0);
						U64 num_newlines = (props.size - 1) / MG_BYTELITS_PER_LINE + 1;
						String8 c_byte_lits = push_str8_nt(scratch.arena, props.size * MG_BYTELIT_STRLEN + num_newlines);
						U8 *copy_target = c_byte_lits.buffer;
						for (U8 *c = gen_string.buffer, *one_past_last = gen_string.buffer + gen_string.length; c < one_past_last; c++)
						{
							*copy_target++ = '0';
							*copy_target++ = 'x';
							*copy_target++ = integer_symbols[*c / 16];
							*copy_target++ = integer_symbols[*c % 16];
							*copy_target++ = ',';
							ptrdiff_t nth_char = (c - gen_string.buffer) + 1;
							Assert(nth_char > 0);
							if (nth_char % MG_BYTELITS_PER_LINE == 0)
							{
								*copy_target++ = '\n';
								num_newlines--;
							}
						}
						*copy_target++ = '\n';
						Assert(copy_target == c_byte_lits.buffer + c_byte_lits.length && --num_newlines == 0);
						#undef MG_BYTELIT_STRLEN
						#undef MG_BYTELITS_PER_LINE

						str8_list_push(
							scratch.arena,
							target_location,
							c_byte_lits
						);
					}
					str8_list_pushf(
						scratch.arena,
						target_location,
						"};\nconst global String8 %S = str8(%S, sizeof(%S));\n\n",
						embedded_varname,
						bytes_varname,
						bytes_varname
					);
				}
				else
				{
					Assert(global_directive->kind == MD_ASTKind_DirectiveEmbedString);
					str8_list_pushf(
						scratch.arena,
						target_location,
						"const global String8 %S = str8_lit(\n",
						embedded_varname
					);
					U8 *one_past_last = gen_string.buffer + gen_string.length;
					for (U8 *line_begin = gen_string.buffer; line_begin < one_past_last; line_begin++)
					{
						str8_list_push(
							scratch.arena,
							target_location,
							str8_lit("\"")
						);
						U8 *line_end = line_begin;
						for (; line_end < one_past_last && *line_end != '\n'; line_end++);
						String8 line = str8_region(line_begin, line_end);
						String8Node *line_node = str8_list_pushf(
							scratch.arena,
							target_location,
							"%S\\n\"\n",
							line
						);
						Unused(line_node); // inspect in debugger
						line_begin = line_end;
					}
					str8_list_push(
						scratch.arena,
						target_location,
						str8_lit(");\n\n")
					);
				}
			} break;
			case MD_ASTKind_DirectiveGenH:
			case MD_ASTKind_DirectiveGenC:
			case MD_ASTKind_DirectiveEnum:
			case MD_ASTKind_DirectiveStruct:
			case MD_ASTKind_DirectiveArray: {
				MD_AST *directive_child = global_directive->first;
				String8 enum_name = zero_struct; // 0 if not @enum // TODO: type name for enum and struct
				switch (global_directive->kind)
				{
					case MD_ASTKind_DirectiveGenH:
					case MD_ASTKind_DirectiveGenC: break;
					case MD_ASTKind_DirectiveEnum:
					case MD_ASTKind_DirectiveStruct: {
						Assert(directive_child->kind == MD_ASTKind_Ident);
						String8Node *decl = 0;
						str8_list_push(scratch.arena, target_location, str8_lit("typedef "));
						if (global_directive->kind == MD_ASTKind_DirectiveEnum)
						{
							enum_name = push_str8_cat(
								scratch.arena,
								str8_lit(" "),
								directive_child->token->source
							);
							decl = str8_list_push(scratch.arena, target_location, str8_lit("enum"));
						}
						else
						{
							Assert(global_directive->kind == MD_ASTKind_DirectiveStruct);
							decl = str8_list_pushf(
								scratch.arena,
								target_location,
								"typedef struct %S %S;\nstruct %S",
								directive_child->token->source,
								directive_child->token->source,
								directive_child->token->source
							);
						}
						Assert(decl);
						Unused(decl); // for debugger inspecting
					} break;
					case MD_ASTKind_DirectiveArray: {
						Assert(
							  global_directive->kind == MD_ASTKind_DirectiveArray
							&& directive_child->kind == MD_ASTKind_IdentList
							&& 0 < directive_child->children_count
							&& directive_child->children_count <= 2
							&& directive_child->first->kind == MD_ASTKind_Ident);
						String8 type = directive_child->first->token->source,
							array_count = zero_struct,
							name = zero_struct;

						if (directive_child->children_count == 2)
						{
							Assert(directive_child->last->kind == MD_ASTKind_Ident);
							array_count = directive_child->last->token->source;
						}

						directive_child = directive_child->next;
						Assert(directive_child->kind == MD_ASTKind_Ident);
						name = directive_child->token->source;

						String8 common_arr_decl = push_str8f(
							scratch.arena,
							"const %S %S[%S]",
							type,
							name,
							array_count
						);

						// REVIEW:
						//  is extern needed?
						//  static? put everything in header file?
						str8_list_pushf(
							scratch.arena,
							target_location,
							"extern %S;\n\n",
							common_arr_decl
						);

						str8_list_pushf(
							scratch.arena,
							target_location,
							"%S =",
							common_arr_decl
						);
					} break;
					default: {
						Unreachable;
					} break;
				}
				if (global_directive->kind != MD_ASTKind_DirectiveGenH && global_directive->kind != MD_ASTKind_DirectiveGenC)
				{
					Assert(global_directive->kind == MD_ASTKind_DirectiveEnum
					    || global_directive->kind == MD_ASTKind_DirectiveStruct
					    || global_directive->kind == MD_ASTKind_DirectiveArray);
					str8_list_push(
						scratch.arena,
						target_location,
						str8_lit("\n{\n")
					);
					Assert(directive_child->kind = MD_ASTKind_Ident);
					directive_child = directive_child->next; // advance past name
				}

				for (; directive_child != 0; directive_child = directive_child->next)
				{
					Assert(directive_child->kind == MD_ASTKind_StringLit || directive_child->kind == MD_ASTKind_DirectiveExpand);

					// TODO: unify codepath for removing quotes from string literal
					if (directive_child->kind == MD_ASTKind_StringLit)
					{
						Assert(directive_child->token->kind == MD_TokenKind_StringLit
						    && directive_child->token->source.length >= 2
						    && directive_child->token->source.buffer[0] == '"'
						    && directive_child->token->source.buffer[directive_child->token->source.length - 1] == '"'
						);
						// don't write delimiting quotes
						String8 to_write = str8(
							directive_child->token->source.buffer + 1,
							directive_child->token->source.length - 2
						);
						str8_list_pushf(
							scratch.arena,
							target_location,
							"%S\n",
							to_write
						);
					}
					else
					{
						Assert(directive_child->kind == MD_ASTKind_DirectiveExpand);
						MD_AST *expand_child = directive_child->first;
						Assert(expand_child->kind == MD_ASTKind_Ident);

						MD_SymbolTableEntry *table_symbol = md_symbol_from_ident(0, &stab_root, expand_child->token->source),
						                    *cols_stab    = table_symbol->table_record.cols_stab;

						expand_child = expand_child->next;
						Assert(expand_child->kind == MD_ASTKind_StringLit
						    && expand_child->token->kind == MD_TokenKind_StringLit
						    && expand_child->token->source.length >= 2
						    && expand_child->token->source.buffer[0] == '"'
						    && expand_child->token->source.buffer[expand_child->token->source.length - 1] == '"'
						); // is a format string
						MD_Token *format_string_token = expand_child->token;
						String8 format_string = str8(
							format_string_token->source.buffer + 1,
							format_string_token->source.length - 2
						);
						if (format_string.length < 1)
						{
							md_messagelist_push(
								arena,
								&result.messages,
								source,
								format_string.buffer, // REVIEW
								format_string_token,
								directive_child,
								MD_MessageKind_Warning,
								str8_lit("empty @expand format string")
							);
							continue;
						}
						expand_child = expand_child->next;
						U64 num_format_specifiers = 0;
						for (U8 *c = format_string.buffer, *one_past_last = format_string.buffer+format_string.length; c < one_past_last; c++)
						{
							if (*c == '\\')
								c++;
							else if (*c == '%')
								num_format_specifiers++;
						}
						struct MD_FormatSpecInfo
						{
							Rng1U64 range; // .min is index of after last specifier, .max is index of current specifier
							U64 col; // table column of the current specifier
						} *specifiers               = push_array_no_zero(arena, struct MD_FormatSpecInfo, num_format_specifiers),
						  *specifier                = specifiers,
						  *specifiers_one_past_last = specifiers + num_format_specifiers;
						for (U64 prefix_ix = 0, spec_ix = 0; spec_ix < format_string.length && specifier < specifiers_one_past_last; spec_ix++)
						{
							U8 c = format_string.buffer[spec_ix];
							// TODO: deduplicate next-format-specifier logic
							if (c == '\\')
								spec_ix++;
							else if (c == '%')
							{
								Assert(expand_child->kind == MD_ASTKind_Ident);
								// NOTE: we've already checked that all format arguments are valid
								MD_SymbolTableEntry *col_record = md_symbol_from_ident(0, &cols_stab, expand_child->token->source);
								specifier->range = r1u64(prefix_ix, spec_ix);
								specifier->col = col_record->col_record.col;
								prefix_ix = spec_ix + 1; // non-format specifier can't be on this current index
								specifier++;
								expand_child = expand_child->next;
							}
						}
						Assert(specifier == specifiers + num_format_specifiers);

						Assert(directive_child->children_count >= 2); // @expand should always have a target table and a format string as children
						U64 num_format_args = directive_child->children_count - 2; // first two children of @expand are the table and the format string
						if (num_format_specifiers != num_format_args)
						{
							md_messagelist_pushf(
								arena,
								&result.messages,
								source,
								format_string.buffer, // REVIEW
								format_string_token,
								directive_child,
								MD_MessageKind_FatalError,
								"@expand format-arg mismatch - %llu format specifiers and %llu format arguments", // REVIEW: make better
								num_format_specifiers,
								num_format_args
							);
							goto finish_generation;
						}
						else if (num_format_args < 1)
						{
							md_messagelist_push(
								arena,
								&result.messages,
								source,
								format_string.buffer, // REVIEW
								format_string_token,
								directive_child,
								MD_MessageKind_Warning,
								str8_lit("@expand with no formatting") // REVIEW
							);
						}

						Assert(num_format_specifiers == num_format_args && expand_child == 0); // is the number of format args correct and have we used each format arg

						// REVIEW: number of specifiers is checked elsewhere, initialize there?
						U64 after_last_spec_ix = specifiers < specifiers_one_past_last
							? (specifiers_one_past_last - 1)->range.max + 1
							: 0;
						Assert(after_last_spec_ix <= format_string.length);
						String8 *elem_row = table_symbol->table_record.elem_matrix,
							 after_last_spec_string = push_str8_cat(
							scratch.arena,
							str8_substr(format_string, r1u64(after_last_spec_ix, format_string.length)),
							str8_lit("\n")

						);
						for (U64 row = 0; row < table_symbol->table_record.num_rows; row++, elem_row += table_symbol->table_record.num_cols)
						{
							for (struct MD_FormatSpecInfo *pref_spec = specifiers; pref_spec < specifiers_one_past_last; pref_spec++)
							{
								String8Node *format_pushed = str8_list_push(
									scratch.arena,
									target_location,
									str8_substr(format_string, pref_spec->range)
								);
								Unused(format_pushed); // to inspect in debugger
								String8 *elem_string = elem_row + pref_spec->col;
								Assert(elem_string < table_symbol->table_record.elem_matrix + (table_symbol->table_record.num_cols*table_symbol->table_record.num_rows)
								    && elem_string->length > 0);
								String8Node *elem_pushed = str8_list_push(
									scratch.arena,
									target_location,
									*(elem_row + pref_spec->col)
								);
								Unused(elem_pushed); // to inspect in debugger
							}

							// push part of format string that comes after last format specifier
							String8Node *after_last_specifier = str8_list_push(
								scratch.arena,
								target_location,
								after_last_spec_string
							);
							Unused(after_last_specifier); // inspect of debugger
						}
					}
				}

				switch (global_directive->kind)
				{
					case MD_ASTKind_DirectiveGenH:
					case MD_ASTKind_DirectiveGenC: break; // do nothing
					case MD_ASTKind_DirectiveStruct:
					case MD_ASTKind_DirectiveArray:
					case MD_ASTKind_DirectiveEnum: {
						str8_list_pushf(
							scratch.arena,
							target_location,
							"}%S;\n\n",
							enum_name
						);
					} break;
				}

			} break;
			case MD_ASTKind_DirectiveTable: {
				// do nothing
			} break;
			default: {
				Unreachable; // REVIEW: internal error message?
			} break;
		}
	}

	finish_generation:;

	str8_list_concat_in_place(&h_file_enums,     &h_file_structs        );
	str8_list_concat_in_place(&h_file_enums,     &h_file_functions      );
	str8_list_concat_in_place(&h_file_enums,     &h_file_arrays         );
	str8_list_concat_in_place(&h_file_enums,     &h_file_embeddedstrings);
	str8_list_concat_in_place(&h_file_enums,     &h_file_embeddedfiles  );
	str8_list_concat_in_place(&h_file_enums,     &h_file_default        );
	str8_list_concat_in_place(&c_file_functions, &c_file_arrays         );
	str8_list_concat_in_place(&c_file_functions, &c_file_default        );

	result.h_file = str8_list_join(arena, h_file_enums,     0);
	result.c_file = str8_list_join(arena, c_file_functions, 0);

	scratch_end(scratch);

	return result;
}
