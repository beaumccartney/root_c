internal inline void
mg_print_message_push(Arena *arena, String8List *print_messages, String8 message_string, String8 filename)
{
	String8Node *pushed = str8_list_pushf(
		arena,
		print_messages,
		"%S%S",
		filename,
		message_string
	);
	Unused(pushed);
}

// REVIEW: move to mdesk - useful functionality to include with the file format in general
internal MG_GenResult
mg_generate_from_checked(Arena *arena, MD_AST *root, MD_SymbolTableEntry *stab_root, String8 source)
{
	MG_GenResult result = zero_struct;

	String8List h_file_enums  = zero_struct,
	            h_file_arrays = zero_struct, // arrays will always be put after enums so any ident lengths in the arrays are already declared
	            c_file        = zero_struct;

	Temp scratch = scratch_begin(&arena, 1);

	for (MD_AST *global_directive = root->first; global_directive != 0; global_directive = global_directive->next)
	{
		switch (global_directive->kind)
		{
			case MD_ASTKind_DirectiveData:
			case MD_ASTKind_DirectiveEnum: {
				MD_AST *directive_child = global_directive->first;
				String8List *target_file = 0;
				String8 enum_name = zero_struct; // 0 if not @enum
				if (global_directive->kind == MD_ASTKind_DirectiveEnum)
				{
					target_file = &h_file_enums;
					Assert(directive_child->kind == MD_ASTKind_Ident);
					enum_name = push_str8f(
						scratch.arena,
						" %S",
						directive_child->token->source
					);
					str8_list_push(scratch.arena, target_file, str8_lit("typedef enum"));
				}
				else
				{
					target_file = &c_file;
					Assert(
						  global_directive->kind == MD_ASTKind_DirectiveData
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
						"%S %S[%S]",
						type,
						name,
						array_count
					);

					str8_list_pushf(
						scratch.arena,
						&h_file_arrays,
						"extern %S;",
						common_arr_decl
					);

					str8_list_push(
						scratch.arena,
						&c_file,
						push_str8f(
							scratch.arena,
							"%S =",
							common_arr_decl
						)
					);
				}
				str8_list_push(
					scratch.arena,
					target_file,
					str8_lit("{")
				);

				for (directive_child = directive_child->next; directive_child != 0; directive_child = directive_child->next)
				{
					Assert(directive_child->kind == MD_ASTKind_StringLit || directive_child->kind == MD_ASTKind_DirectiveExpand);

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
						str8_list_push(
							scratch.arena,
							target_file,
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
								MD_MessageKind_Warning,
								str8_lit("empty @expand format string"),
								format_string_token,
								directive_child
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
							  *specifiers_one_past_last = specifiers + num_format_specifiers;
						struct MD_FormatSpecInfo *spec_info = specifiers;
						for (U64 prefix_ix = 0, spec_ix = 0; spec_ix < format_string.length && spec_info < specifiers_one_past_last; spec_ix++)
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
								spec_info->range = r1u64(prefix_ix, spec_ix);
								spec_info->col = col_record->col_record.col;
								prefix_ix = spec_ix + 1; // non-format specifier can't be on this current index
								spec_info++;
								expand_child = expand_child->next;
							}
						}
						Assert(spec_info == specifiers + num_format_specifiers && expand_child == 0);

						Assert(directive_child->children_count >= 2); // @expand should always have a target table and a format string as children
						U64 num_format_args = directive_child->children_count - 2; // first two children of @expand are the table and the format string
						if (num_format_specifiers != num_format_args)
						{
							md_messagelist_push(
								arena,
								&result.messages,
								source,
								format_string.buffer, // REVIEW
								MD_MessageKind_FatalError,
								push_str8f(
									arena,
									"@expand format-arg mismatch - %llu format specifiers and %llu format arguments", // REVIEW: make better
									num_format_specifiers,
									num_format_args
								),
								format_string_token,
								directive_child
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
								MD_MessageKind_Warning,
								str8_lit("@expand with no formatting"), // REVIEW
								format_string_token,
								directive_child
							);
						}

						Assert(num_format_specifiers == num_format_args);
						String8List expand_list = zero_struct; // instead of pushing directly to target_file for debugging purposes

						// REVIEW: number of specifiers is checked elsewhere, initialize there?
						U64 after_last_spec_ix = specifiers_one_past_last > specifiers
							? (specifiers_one_past_last - 1)->range.max + 1
							: 0;
						Assert(after_last_spec_ix <= format_string.length);
						String8 *elem_row = table_symbol->table_record.elem_matrix,
							 after_last_spec_string = push_str8f(
							scratch.arena,
							"%S\n",
							str8_substr(format_string, r1u64(after_last_spec_ix, format_string.length))

						);
						for (U64 row = 0; row < table_symbol->table_record.num_rows; row++, elem_row += table_symbol->table_record.num_cols)
						{
							for (struct MD_FormatSpecInfo *pref_spec = specifiers; pref_spec < specifiers_one_past_last; pref_spec++)
							{
								String8Node *format_pushed = str8_list_push(
									scratch.arena,
									&expand_list,
									str8_substr(format_string, pref_spec->range)
								);
								Unused(format_pushed); // to inspect in debugger
								String8 *elem_string = elem_row + pref_spec->col;
								Assert(elem_string < table_symbol->table_record.elem_matrix + (table_symbol->table_record.num_cols*table_symbol->table_record.num_rows)
								    && elem_string->length > 0);
								String8Node *elem_pushed = str8_list_push(
									scratch.arena,
									&expand_list,
									*(elem_row + pref_spec->col)
								);
								Unused(elem_pushed); // to inspect in debugger
							}

							// push part of format string that comes after last format specifier
							String8Node *after_last_specifier = str8_list_push(
								scratch.arena,
								&expand_list,
								after_last_spec_string
							);
							Unused(after_last_specifier); // inspect of debugger
						}

						// XXX: use string array instead of list?
						// REVIEW: use join param to add comma at the end?
						String8 expanded = str8_list_join(scratch.arena, expand_list, 0); // instead of concat because not all nodes want a newline after
						str8_list_push(scratch.arena, target_file, expanded);
					}
				}

				str8_list_pushf(
					scratch.arena,
					target_file,
					"}%S;\n\n",
					enum_name
				);
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

	str8_list_concat_in_place(&h_file_enums, &h_file_arrays);
	// REVIEW: right now there's two newlines after @expand contents, because of this, should I remove it?
	StringJoin join = {
		.sep = str8_lit("\n"),
	};
	result.h_file = str8_list_join(arena, h_file_enums, &join);
	result.c_file = str8_list_join(arena, c_file, &join);

	scratch_end(scratch);

	return result;
}
