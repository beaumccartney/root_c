// REVIEW:
//  deduplicate generating strings and arrays, and possibly everything
internal MG_GenResult
mg_generate_from_checked(Arena *arena, MD_AST *root, MD_SymbolTableEntry *stab_root, String8 gen_folder, String8 source)
{
	Assert(0 <= gen_folder.length && 0 <= source.length);
	MG_GenResult result = zero_struct;

	String8List gen_lists[MD_GenFile_COUNT][MD_GenLocation_COUNT] = zero_struct;

	Temp scratch = scratch_begin(&arena, 1);

	for (MD_AST *global_directive = root->first; global_directive != 0; global_directive = global_directive->next)
	{
		const local_persist MD_GenLocation md_ast_to_gen_info_table[MD_ASTKind_COUNT] = {
			[MD_ASTKind_DirectiveGen        ] = MD_GenLocation_Default,
			[MD_ASTKind_DirectiveEnum       ] = MD_GenLocation_Enums,
			[MD_ASTKind_DirectiveStruct     ] = MD_GenLocation_Structs,
			[MD_ASTKind_DirectiveArray      ] = MD_GenLocation_Arrays,
			[MD_ASTKind_DirectiveEmbedString] = MD_GenLocation_EmbeddedStrings,
			[MD_ASTKind_DirectiveEmbedFile  ] = MD_GenLocation_EmbeddedFiles,
		};

		MD_GenFile     gen_file = global_directive->gen_file;
		MD_GenLocation gen_loc  = global_directive->gen_loc;

		if (gen_file == MD_GenFile_NULL)
			gen_file = MD_GenFile_H;

		if (gen_loc == MD_GenLocation_NULL)
			gen_loc = md_ast_to_gen_info_table[global_directive->kind];

		Assert(global_directive->kind == MD_ASTKind_DirectiveTable || (gen_file != MD_GenFile_NULL && gen_loc != MD_GenLocation_NULL));
		String8List *gen_target = &gen_lists[gen_file][gen_loc];
		MD_AST *directive_child = global_directive->first;
		MD_SymbolTableEntry *directive_symbol = 0;
		switch (global_directive->kind)
		{
			case MD_ASTKind_DirectiveStruct:
			case MD_ASTKind_DirectiveEnum:
			case MD_ASTKind_DirectiveArray:
			case MD_ASTKind_DirectiveEmbedString:
			case MD_ASTKind_DirectiveEmbedFile: {
				Assert(directive_child->kind == MD_ASTKind_Ident && directive_child->token->kind == MD_TokenKind_Ident);
				directive_symbol = md_symbol_from_ident(0, &stab_root, directive_child->token->source);
				directive_child = directive_child->next;
			} break;
			case MD_ASTKind_DirectiveGen:
			case MD_ASTKind_DirectiveTable: break;
			default: {
				Unreachable;
			} break;
		}

		switch (global_directive->kind)
		{
			case MD_ASTKind_DirectiveEmbedString:
			case MD_ASTKind_DirectiveEmbedFile: {
				MD_Token *gen_string_token = directive_symbol->named_gen_record.token1;
				String8 embedded_varname   = directive_symbol->ident,
					gen_string         = gen_string_token->source;
				if (global_directive->kind == MD_ASTKind_DirectiveEmbedFile)
				{
					Assert(gen_string_token->kind == MD_TokenKind_StringLit);
					gen_string = mg_trim_quotes(gen_string);
				}
				else
				{
					Assert(global_directive->kind == MD_ASTKind_DirectiveEmbedString
					    && gen_string_token->kind == MD_TokenKind_RawStringLit
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
						gen_string_token,
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
					OS_File file = os_file_open(OS_AccessFlag_Read, filepath);

					MD_MessageKind message_kind = MD_MessageKind_NULL;
					char *message_format = 0; // if set MUST HAVE ONE FORMAT SPECIFIER %S
					if (file.bits == 0)
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
							gen_string_token->source.buffer, gen_string_token,
							global_directive,
							message_kind,
							message_format,
							filepath
						);
						if (message_kind >= MD_MessageKind_Error) // still generate data for empty files
							break; // REVIEW: needed?
					}
					Assert(!(props.flags & FilePropertyFlag_IsFolder));
					gen_string = os_string_from_file_range(scratch.arena, file, rng_1s64(0, props.size));

					String8 bytes_varname = push_str8_cat(
						scratch.arena,
						embedded_varname,
						str8_lit("__bytes")
					);
					str8_list_pushf(
						scratch.arena,
						gen_target,
						"const global U8 %S[] =\n{\n",
						bytes_varname
					);
					if (props.size > 0)
					{
						#define MG_BYTELIT_STRLEN 5 // strlen("0xXX,")
						#define MG_BYTELITS_PER_LINE 40
						Assert(props.size > 0);
						S64 num_newlines = (props.size - 1) / MG_BYTELITS_PER_LINE + 1;
						String8 c_byte_lits = push_str8_nt(scratch.arena, props.size * MG_BYTELIT_STRLEN + num_newlines);
						U8 *copy_target = c_byte_lits.buffer;
						for (U8 *c = gen_string.buffer, *one_past_last = gen_string.buffer + gen_string.length; c < one_past_last; c++)
						{
							*copy_target++ = '0';
							*copy_target++ = 'x';
							*copy_target++ = g_integer_symbols[*c / 16];
							*copy_target++ = g_integer_symbols[*c % 16];
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
							gen_target,
							c_byte_lits
						);
					}
					str8_list_pushf(
						scratch.arena,
						gen_target,
						"};\nconst global String8 %S = {(U8*)%S, sizeof(%S)};\n\n",
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
						gen_target,
						"const global String8 %S = str8_lit_comp(\n",
						embedded_varname
					);
					U8 *one_past_last = gen_string.buffer + gen_string.length;
					for (U8 *line_begin = gen_string.buffer; line_begin < one_past_last; line_begin++)
					{
						str8_list_push(
							scratch.arena,
							gen_target,
							str8_lit("\"")
						);
						U8 *line_end = line_begin;
						for (; line_end < one_past_last && *line_end != '\n'; line_end++);
						String8 line = str8_region(line_begin, line_end);
						String8Node *line_node = str8_list_pushf(
							scratch.arena,
							gen_target,
							"%S\\n\"\n",
							line
						);
						Unused(line_node); // inspect in debugger
						line_begin = line_end;
					}
					str8_list_push(
						scratch.arena,
						gen_target,
						str8_lit(");\n\n")
					);
				}
			} break;
			case MD_ASTKind_DirectiveGen:
			case MD_ASTKind_DirectiveEnum:
			case MD_ASTKind_DirectiveStruct:
			case MD_ASTKind_DirectiveArray: {
				String8 enum_name = zero_struct; // 0 if not @enum // TODO: type name for enum and struct
				switch (global_directive->kind)
				{
					case MD_ASTKind_DirectiveEnum:
					case MD_ASTKind_DirectiveStruct: {
						String8Node *decl = 0;
						str8_list_push(scratch.arena, gen_target, str8_lit("typedef "));
						if (global_directive->kind == MD_ASTKind_DirectiveEnum)
						{
							char *enum_name_format = " %S"; // MUST HAVE ONE FORMAT SPECIFIER OF %S
							MD_Token *type_spec = directive_symbol->named_gen_record.token1;
							if (type_spec)
							{
								Assert(type_spec->kind == MD_TokenKind_StringLit || type_spec->kind == MD_TokenKind_Ident);
								String8 type = type_spec->source;
								if (type_spec->kind == MD_TokenKind_StringLit)
									mg_trim_quotes(type);
								str8_list_pushf(
									scratch.arena,
									gen_target,
									"%S %S;\ntypedef ",
									type,
									directive_symbol->ident
								);
								enum_name_format = " %S__Enum";
							}
							decl = str8_list_push(scratch.arena, gen_target, str8_lit("enum"));
							enum_name = push_str8f(
								scratch.arena,
								enum_name_format,
								directive_symbol->ident
							);
						}
						else
						{
							Assert(global_directive->kind == MD_ASTKind_DirectiveStruct);
							decl = str8_list_pushf(
								scratch.arena,
								gen_target,
								"struct %S %S;\nstruct %S",
								directive_symbol->ident,
								directive_symbol->ident,
								directive_symbol->ident
							);
						}
						Assert(decl);
					} break;
					case MD_ASTKind_DirectiveArray: {
						MD_Token *array_type_tok  = directive_symbol->named_gen_record.token1,
							 *array_count_tok = directive_symbol->named_gen_record.token2;
						Assert(array_type_tok->kind == MD_TokenKind_Ident || array_type_tok->kind == MD_TokenKind_StringLit);
						String8 array_type = directive_symbol->named_gen_record.token1->source;
						if (array_type_tok->kind == MD_TokenKind_StringLit)
						{
							Assert(array_type.length > 2);
							array_type = mg_trim_quotes(array_type);
						}
						Assert(array_type.length > 0);

						String8 array_count = zero_struct;
						if (array_count_tok)
						{
							Assert(array_count_tok->kind == MD_TokenKind_Ident || array_count_tok->kind == MD_TokenKind_StringLit);
							array_count = array_count_tok->source;
							if (array_count_tok->kind == MD_TokenKind_StringLit)
							{
								Assert(array_count.length > 2);
								array_count = mg_trim_quotes(array_count);
							}
							Assert(array_count.length > 0);
						}

						str8_list_pushf(
							scratch.arena,
							gen_target,
							"const global %S %S[%S] =",
							array_type,
							directive_symbol->ident,
							array_count
						);
					} break;
					case MD_ASTKind_DirectiveGen: break; // no symbol
					default: {
						Unreachable;
					} break;
				}
				if (global_directive->kind != MD_ASTKind_DirectiveGen)
				{
					Assert(global_directive->kind == MD_ASTKind_DirectiveEnum
					    || global_directive->kind == MD_ASTKind_DirectiveStruct
					    || global_directive->kind == MD_ASTKind_DirectiveArray);
					str8_list_push(
						scratch.arena,
						gen_target,
						str8_lit("\n{\n")
					);
				}

				for (; directive_child != 0; directive_child = directive_child->next)
				{
					Assert(directive_child->kind == MD_ASTKind_StringLit || directive_child->kind == MD_ASTKind_DirectiveExpand);

					if (directive_child->kind == MD_ASTKind_StringLit)
					{
						Assert(directive_child->token->kind == MD_TokenKind_StringLit);
						String8Node *expand_stringlit = str8_list_pushf(
							scratch.arena,
							gen_target,
							"%S\n",
							mg_trim_quotes(directive_child->token->source)
						);
						Unused(expand_stringlit);
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
						    && expand_child->token->kind == MD_TokenKind_StringLit); // is a format string
						MD_Token *format_string_token = expand_child->token;
						String8 format_string = mg_trim_quotes(format_string_token->source);
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
						S64 num_format_specifiers = 0;
						for (U8 *c = format_string.buffer, *one_past_last = format_string.buffer+format_string.length; c < one_past_last; c++)
						{
							if (*c == '\\')
								c++;
							else if (*c == '%')
								num_format_specifiers++;
						}
						struct MD_FormatSpecInfo
						{
							Rng1S64 range; // .min is index of after last specifier, .max is index of current specifier
							S64 col; // table column of the current specifier
						} *specifiers               = push_array_no_zero(arena, struct MD_FormatSpecInfo, num_format_specifiers),
						  *specifier                = specifiers,
						  *specifiers_one_past_last = specifiers + num_format_specifiers;
						for (S64 prefix_ix = 0, spec_ix = 0; spec_ix < format_string.length && specifier < specifiers_one_past_last; spec_ix++)
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
								specifier->range = r1s64(prefix_ix, spec_ix);
								specifier->col = col_record->col_record.col;
								prefix_ix = spec_ix + 1; // non-format specifier can't be on this current index
								specifier++;
								expand_child = expand_child->next;
							}
						}
						Assert(specifier == specifiers + num_format_specifiers);

						Assert(directive_child->children_count >= 2); // @expand should always have a gen_target table and a format string as children
						S64 num_format_args = directive_child->children_count - 2; // first two children of @expand are the table and the format string
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
						S64 after_last_spec_ix = specifiers < specifiers_one_past_last
							? (specifiers_one_past_last - 1)->range.max + 1
							: 0;
						Assert(after_last_spec_ix <= format_string.length);
						String8 *elem_row = table_symbol->table_record.elem_matrix,
							 after_last_spec_string = push_str8_cat(
							scratch.arena,
							str8_substr(format_string, r1s64(after_last_spec_ix, format_string.length)),
							str8_lit("\n")

						);
						for (S64 row = 0; row < table_symbol->table_record.num_rows; row++, elem_row += table_symbol->table_record.num_cols)
						{
							for (struct MD_FormatSpecInfo *pref_spec = specifiers; pref_spec < specifiers_one_past_last; pref_spec++)
							{
								String8Node *format_pushed = str8_list_push(
									scratch.arena,
									gen_target,
									str8_substr(format_string, pref_spec->range)
								);
								Unused(format_pushed); // to inspect in debugger
							String8 *elem_string = elem_row + pref_spec->col;
								Assert(elem_string < table_symbol->table_record.elem_matrix + (table_symbol->table_record.num_cols*table_symbol->table_record.num_rows)
								    && elem_string->length > 0);
								String8Node *elem_pushed = str8_list_push(
									scratch.arena,
									gen_target,
									*(elem_row + pref_spec->col)
								);
								Unused(elem_pushed); // to inspect in debugger
							}

							// push part of format string that comes after last format specifier
							String8Node *after_last_specifier = str8_list_push(
								scratch.arena,
								gen_target,
								after_last_spec_string
							);
							Unused(after_last_specifier); // inspect of debugger
						}
					}
				}

				switch (global_directive->kind)
				{
					case MD_ASTKind_DirectiveStruct:
					case MD_ASTKind_DirectiveArray:
					case MD_ASTKind_DirectiveEnum: {
						str8_list_pushf(
							scratch.arena,
							gen_target,
							"}%S;\n\n",
							enum_name
						);
					} break;
					case MD_ASTKind_DirectiveGen: break; // do nothing
					default: {
						Unreachable;
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

	for EachNonZeroEnumVal(MD_GenLocation, i)
	{
		str8_list_concat_in_place(&gen_lists[MD_GenFile_H][MD_GenFile_NULL], &gen_lists[MD_GenFile_H][i]);
		str8_list_concat_in_place(&gen_lists[MD_GenFile_C][MD_GenFile_NULL], &gen_lists[MD_GenFile_C][i]);
	}

	result.h_file = str8_list_join(arena, gen_lists[MD_GenFile_H][MD_GenFile_NULL], 0);
	result.c_file = str8_list_join(arena, gen_lists[MD_GenFile_C][MD_GenFile_NULL], 0);

	scratch_end(scratch);

	return result;
}

internal inline String8
mg_trim_quotes(String8 string)
{
	Assert(string.length >= 2 && *string.buffer == '"' && string.buffer[string.length - 1] == '"'); // is the literal delimited by ""?
	String8 result = str8(string.buffer + 1, string.length - 2);
	return result;
}
