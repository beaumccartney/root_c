// TODO: refactor to just messagelist_push where stuff comes up. this way its
// possible to keep scanning even if there's errors to help report more errors
internal MD_TokenizeResult
md_tokens_from_source(Arena *arena, String8 source)
{
	MD_TokenList toklist    = zero_struct;
	MD_MessageList messages = zero_struct;
	Temp scratch            = scratch_begin(&arena, 1);
	U8 *one_past_last       = source.buffer + source.length;
	for (U8 *c = source.buffer; c < one_past_last;)
	{
		MD_TokenKind token_kind = MD_TokenKind_NULL;
		MD_MessageKind message_kind = 0;
		String8 message_string = zero_struct;

		U8 *tok_start = c;
		switch (*c)
		{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
			case '\f':
			case '\v': {
				c++;
				continue;
			} break;
			case '/': {
				if (++c == one_past_last || (*c != '/' && *c != '*')) // trailing '/'
				{
					message_kind   = MD_MessageKind_FatalError;
					message_string = str8_lit("trailing / - comments begin with // or /*");
				}
				else // comment
				{
					// TODO: allowed characters
					if (*c++ == '/')
					{
						while (c < one_past_last && *c != '\n')
							c++;
					}
					else
					{
						U64 comment_stack = 1;
						for (; comment_stack > 0 && c < one_past_last - 1; c++)
						{
							if (c[0] == '/' && c[1] == '*')
							{
								comment_stack++;
								c++;
							}
							else if (c[0] == '*' && c[1] == '/')
							{
								comment_stack--;
								c++;
							}
						}
						if (comment_stack > 0)
						{
							message_kind   = MD_MessageKind_Error;
							message_string = str8_lit("unclosed block comment");
							break;
						}
					}
					continue;
				}
			} break;
			case '@': {
				// REVIEW:
				//  have directive name just be an identifier and check in parsing?
				//  name of the struct
				typedef struct MD_Directive_StringToken_Map MD_Directive_StringToken_Map;
				struct MD_Directive_StringToken_Map
				{
					String8 string;
					MD_TokenKind token_kind;
				};
				// REVIEW: if this gets large, some kind of hash table?
				const local_persist MD_Directive_StringToken_Map directive_table[] = {
					{str8_lit_comp("table"),        MD_TokenKind_DirectiveTable      },
					{str8_lit_comp("gen_c"),        MD_TokenKind_DirectiveGenC       },
					{str8_lit_comp("gen_h"),        MD_TokenKind_DirectiveGenH       },
					{str8_lit_comp("enum"),         MD_TokenKind_DirectiveEnum       },
					{str8_lit_comp("struct"),       MD_TokenKind_DirectiveStruct     },
					{str8_lit_comp("array"),        MD_TokenKind_DirectiveArray      },
					{str8_lit_comp("expand"),       MD_TokenKind_DirectiveExpand     },

					{str8_lit_comp("embed_file"),   MD_TokenKind_DirectiveEmbedFile  },
					{str8_lit_comp("embed_string"), MD_TokenKind_DirectiveEmbedString},
				};
				do c++; while (c < one_past_last && (*c == '_' || char_is_alpha(*c)));
				String8 directive_name = str8_region(tok_start + 1, c);
				for EachElement(i, directive_table)
				{
					struct MD_Directive_StringToken_Map entry = directive_table[i];
					if (str8_match(entry.string, directive_name, 0))
					{
						token_kind = entry.token_kind;
						goto break_lex_switch; // skip unknown directive error
					}
				}
				message_kind   = MD_MessageKind_Error;
				message_string = str8_lit("unknown directive");
			} break;
			case '"': {
				// REVIEW: allowed characters?
				token_kind = MD_TokenKind_StringLit;
				while (++c < one_past_last && *c != '"')
					if (*c == '\\') // just skip escaped characters
					{
						switch (*++c)
						{
							// NOTE: escape sequences allowed in c spec
							case 'a':
							case 'b':
							case 'f':
							case 'n':
							case 'r':
							case 't':
							case 'v':
							case '\\':
							case '\'':
							case '"':
							case '?':

							// octal digits
							case '0':
							case '1':
							case '2':
							case '3':
							case '4':
							case '5':
							case '6':
							case '7':

							// hex prefix
							case 'x':

							// unicode prefix
							case 'u':
							case 'U':

							// escaped format specifier
							case '%': {
								// do nothing
							} break;
							default: {
								message_kind = MD_MessageKind_FatalError;
								message_string = push_str8f(
									arena,
									"unrecognized escape sequence - '\\x%x'",
									*c // TODO: print not in hex and make it nice
								);
								goto break_lex_switch; // REVIEW: keep lexing and push an error per bad escape?
							} break;
						}
					}
				if (c == one_past_last)
				{
					message_kind   = MD_MessageKind_Error;
					message_string = str8_lit("broken string literal");
				}
				else
					c++;
			} break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z': {
				token_kind = MD_TokenKind_Ident;
				do c++; while (c < one_past_last && (*c == '_' || char_is_alpha(*c) || char_is_digit(*c, 10)));
			} break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				token_kind = MD_TokenKind_IntLit;
				U32 radix = 10;
				if (*c++ == '0' && c < one_past_last)
				{
					switch (*c)
					{
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9': {
							message_kind   = MD_MessageKind_FatalError; // REVIEW: assume a radix and keep going?
							message_string = str8_lit("no radix on leading zero - please use 0x, 0b, or 0o");
							c++;
							goto break_lex_switch;
						} break;
						case 'b':
						case 'B': {
							radix = 2;
						} break;
						case 'o':
						case 'O': {
							radix = 8;
						} break;
						case 'x':
						case 'X': {
							radix = 16;
						} break;
						default: {
							goto integral_zero;
						} break;
					}
					if (++c == one_past_last || !char_is_digit(*c, radix))
					{
						message_kind   = MD_MessageKind_Error;
						message_string = str8_lit("incomplete 0x literal");
						if (c < one_past_last)
							c++;
						goto break_lex_switch;
					}
				}
				while (c < one_past_last && char_is_digit(*c, radix))
					c++;
				integral_zero:;
				if (c < one_past_last && *c == '.')
				{
					// REVIEW:
					//  floating point literals with radix != 10?
					//  error for digits in a too-large radix?
					token_kind = MD_TokenKind_FloatLit;
					do c++; while (c < one_past_last && char_is_digit(*c, radix));
				}
			} break;
			default: {
				const global
				MD_TokenKind single_char_tok_table[UINT8_MAX] = {
					['_'] = MD_TokenKind_Underscore,
					['.'] = MD_TokenKind_Period,
					['('] = MD_TokenKind_OpenParen,
					[')'] = MD_TokenKind_CloseParen,
					['{'] = MD_TokenKind_OpenBrace,
					['}'] = MD_TokenKind_CloseBrace,
				};
				token_kind = single_char_tok_table[*c++];
				// REVIEW: allowed characters?
				//  what abt unused punctuation? perhaps split into weird stuff and normal stuff
				if (token_kind == 0)
				{
					message_kind   = MD_MessageKind_Warning;
					// TODO: include an escaped version of the character in the message
					message_string = str8_lit("unknown character - ignoring");
				}
			} break;
		}
		break_lex_switch:;

		Assert(c > tok_start);
		MD_TokenNode *node = push_array_no_zero(scratch.arena, MD_TokenNode, 1);
		*node = (MD_TokenNode) {
			.token.kind   = token_kind,
			.token.source = str8_region(tok_start, c),
		};
		SLLQueuePush(toklist.first, toklist.last, node);
		toklist.count++;
		if (message_kind != MD_MessageKind_NULL)
		{
			MD_Message *message = md_messagelist_push_inner(
				arena,
				&messages,
				source,
				tok_start,
				message_kind,
				message_string
			);
			message->tokens_ix = toklist.count - 1;
			if (messages.worst_message >= MD_MessageKind_FatalError)
				break;
		}
	}

	MD_TokenizeResult result = {
		.tokens.tokens = push_array_no_zero(arena, MD_Token, toklist.count),
		.tokens.count  = toklist.count,
		.messages      = messages,
	};
	MD_Token *dst = result.tokens.tokens;
	for (MD_TokenNode *n = toklist.first; n != 0; n = n->next, dst++)
	{
		MD_Token src = n->token;
		*dst = src;
	}
	Assert(dst == result.tokens.tokens + result.tokens.count);
	scratch_end(scratch);
	return result;
}

internal MD_ParseResult
md_parse_from_tokens_source(Arena *arena, MD_TokenArray tokens, String8 source)
{
	MD_ParseResult result = zero_struct;

	MD_ParseState parser = {
		.arena                = arena,
		.messages             = &result.messages,
		.source               = source,
		.tokens_first         = tokens.tokens,
		.token                = tokens.tokens,
		.tokens_one_past_last = tokens.tokens + tokens.count,
		.global_stab           = &result.global_stab,
	};

	result.root = md_parse_root(&parser); // REVIEW: inline?

	return result;
}

// REVIEW: for unclosed delimiter errors, set location to the opening delimiter?
internal MD_AST*
md_parse_root(MD_ParseState *parser)
{
	MD_AST *root = push_array(parser->arena, MD_AST, 1);

	root->kind = MD_ASTKind_Root;

	U8 *source_last = parser->source.buffer + parser->source.length - 1;

	while (parser->token < parser->tokens_one_past_last)
	{
		switch (parser->token->kind)
		{
			case MD_TokenKind_DirectiveGenH:
			case MD_TokenKind_DirectiveGenC:
			case MD_TokenKind_DirectiveTable:
			case MD_TokenKind_DirectiveEnum:
			case MD_TokenKind_DirectiveStruct:
			case MD_TokenKind_DirectiveArray: {
				Assert(parser->token->kind != MD_TokenKind_DirectiveStruct); // not implemented
				const local_persist MD_ASTKind
				md_token_to_ast_kind_table[MD_TokenKind_COUNT] = {
					[MD_TokenKind_IntLit         ] = MD_ASTKind_IntLit,
					[MD_TokenKind_FloatLit       ] = MD_ASTKind_FloatLit,
					[MD_TokenKind_StringLit      ] = MD_ASTKind_StringLit,

					[MD_TokenKind_DirectiveTable ] = MD_ASTKind_DirectiveTable,

					[MD_TokenKind_DirectiveGenH  ] = MD_ASTKind_DirectiveGenH,
					[MD_TokenKind_DirectiveGenC  ] = MD_ASTKind_DirectiveGenC,
					[MD_TokenKind_DirectiveEnum  ] = MD_ASTKind_DirectiveEnum,
					[MD_TokenKind_DirectiveStruct] = MD_ASTKind_DirectiveStruct,
					[MD_TokenKind_DirectiveArray ] = MD_ASTKind_DirectiveArray,
					[MD_TokenKind_DirectiveExpand] = MD_ASTKind_DirectiveExpand,

					[MD_TokenKind_Ident          ] = MD_ASTKind_Ident,
				};

				MD_AST *global_directive_node = md_ast_push_child(parser->arena, root, md_token_to_ast_kind_table[parser->token->kind]), // XXX rename to global_directive
				       *directive_ident_params = 0; // ident list for @table and @array
				global_directive_node->token = parser->token;
				// REVIEW: allow integer lengths for @array?
				if (global_directive_node->kind == MD_ASTKind_DirectiveTable || global_directive_node->kind == MD_ASTKind_DirectiveArray)
				{
					if (++parser->token == parser->tokens_one_past_last || parser->token->kind != MD_TokenKind_OpenParen)
					{
						md_messagelist_push(
							parser->arena,
							parser->messages,
							parser->source,
							parser->token == parser->tokens_one_past_last
								? source_last
								: parser->token->source.buffer,
							MD_MessageKind_FatalError,
							push_str8f(
								parser->arena,
								"expected '(' after %S",
								global_directive_node->token->source
							),
							parser->token,
							global_directive_node
						);
						goto break_parse_outer_loop;
					}
					directive_ident_params = md_ast_push_child(
						parser->arena,
						global_directive_node,
						MD_ASTKind_IdentList
					);
					while (++parser->token != parser->tokens_one_past_last && parser->token->kind != MD_TokenKind_CloseParen)
					{
						if (parser->token->kind != MD_TokenKind_Ident)
						{
							md_messagelist_push(
								parser->arena,
								parser->messages,
								parser->source,
								parser->token->source.buffer,
								MD_MessageKind_FatalError,
								push_str8f(
									parser->arena,
									"non-ident argument to %S directive: '%S' - perhaps a closing ')' is missing?", // REVIEW: better message
									global_directive_node->token->source,
									parser->token->source
								),
								parser->token,
								global_directive_node
							);
							goto break_parse_outer_loop;
						}
						MD_AST *ident_node = md_ast_push_child(
							parser->arena,
							directive_ident_params,
							MD_ASTKind_Ident
						);
						ident_node->token = parser->token;
					}
					if (parser->token == parser->tokens_one_past_last)
					{
						md_messagelist_push(
							parser->arena,
							parser->messages,
							parser->source,
							source_last,
							MD_MessageKind_FatalError,
							push_str8f(
								parser->arena,
								"unclosed argument list for %S - missing ')'",
								global_directive_node->token->source
							),
							parser->token,
							global_directive_node
						);
						goto break_parse_outer_loop;
					}
				}

				// REVIEW: in scope where its guaranteed n
				// for directives with name onlyamed?
				MD_SymbolTableEntry *directive_symbol = 0;
				MD_AST *directive_name = 0;
				switch (global_directive_node->kind) // check for name on directive
				{
					case MD_ASTKind_DirectiveTable:
					case MD_ASTKind_DirectiveEnum:
					case MD_ASTKind_DirectiveStruct:
					case MD_ASTKind_DirectiveArray: {
						Assert(global_directive_node->kind != MD_ASTKind_DirectiveStruct); // not implemented
						if (++parser->token == parser->tokens_one_past_last || parser->token->kind != MD_TokenKind_Ident)
						{
							String8 message_str = push_str8f(
								parser->arena,
								"missing name for %S directive",
								global_directive_node->token->source
							);
							md_messagelist_push(
								parser->arena,
								parser->messages,
								parser->source,
								parser->token == parser->tokens_one_past_last
								? source_last
								: parser->token->source.buffer,
								MD_MessageKind_FatalError,
								message_str,
								parser->token,
								global_directive_node
							);
							goto break_parse_outer_loop;
						}

						directive_name = md_ast_push_child(
							parser->arena,
							global_directive_node,
							MD_ASTKind_Ident
						);
						directive_name->token = parser->token;
						directive_symbol = md_symbol_from_ident(
							parser->arena,
							parser->global_stab,
							directive_name->token->source
						);
						if (directive_symbol->ast != 0)
						{
							// TODO: directive_symbol->key points
							// to the original declaration of the
							// symbol, use it to find the position
							// of the original declaration and
							// report in the error message
							md_messagelist_push(
								parser->arena,
								parser->messages,
								parser->source,
								directive_name->token->source.buffer,
								MD_MessageKind_FatalError, // NOTE: because the symbol table entry holds table columns attempting to parse this directive could add columns to a different directive's column table
								push_str8f(
									parser->arena,
									"redeclaration of '%S'",
									directive_name->token->source
								),
								directive_name->token,
								global_directive_node // REVIEW: the directive_name instead?
							);
							goto break_parse_outer_loop; // REVIEW: fatal only for @table (either on original or duplicate)? i.e. circumstances where its guaranteed that no symbol taint can happen
						}
						directive_symbol->ast = global_directive_node;

					} break;
				}
				if (global_directive_node->kind == MD_ASTKind_DirectiveTable)
				{
					directive_symbol->table_record.num_cols = directive_ident_params->children_count;
					if (directive_symbol->table_record.num_cols < 1)
					{
						md_messagelist_push(
							parser->arena,
							parser->messages,
							parser->source,
							global_directive_node->token->source.buffer,
							MD_MessageKind_Warning,
							push_str8f(
								parser->arena,
								"@table directive '%S' has no named columns",
								directive_name->token->source
							),
							0, // REVIEW
							global_directive_node
						);
					}
					else
					{
						// REVIEW XXX: just insert these into the symbol table as the identlist is being built
						U64 column_number = 0;
						for (MD_AST *column = directive_ident_params->first; column != 0; column = column->next, column_number++)
						{
							MD_SymbolTableEntry *column_symbol = md_symbol_from_ident(
								parser->arena,
								&directive_symbol->table_record.cols_stab,
								column->token->source
							);
							if (column_symbol->ast != 0)
							{
								md_messagelist_push(
									parser->arena,
									parser->messages,
									parser->source,
									column->token->source.buffer,
									MD_MessageKind_Error,
									push_str8f(
										parser->arena,
										"duplicate column name '%S' in @table '%S",
										column->token->source,
										directive_name->token->source
									),
									column->token,
									global_directive_node // REVIEW: ast of the name itself?
								);
							}
							else
							{
								column_symbol->col_record.col = column_number;
								column_symbol->ast = column;
							}
						}
					}
				}
				else if (global_directive_node->kind == MD_ASTKind_DirectiveArray && (directive_ident_params->children_count < 1 || directive_ident_params->children_count > 2))
				{
					md_messagelist_push(
						parser->arena,
						parser->messages,
						parser->source,
						global_directive_node->token->source.buffer,
						MD_MessageKind_Error,
						push_str8f(
							parser->arena,
							"incorrect number of parameters for @array directive '%S' - at least one identifier is required for the type of the array, and one more identifier is allowed to refer to the length of the array",
							directive_name->token->source
						),
						0, // REVIEW
						global_directive_node
					);
				}

				if (++parser->token == parser->tokens_one_past_last || parser->token->kind != MD_TokenKind_OpenBrace)
				{
					md_messagelist_push(
						parser->arena,
						parser->messages,
						parser->source,
						parser->token == parser->tokens_one_past_last
							? source_last
							: parser->token->source.buffer,
						MD_MessageKind_FatalError,
						push_str8f(
							parser->arena,
							"%S directive missing missing opening '{'",
							global_directive_node->token->source
						),
						parser->token,
						global_directive_node
					);
					goto break_parse_outer_loop;
				}
				U64 children_before_body = global_directive_node->children_count; // REVIEW: body ast to hold the children count?
				parser->token++;
				switch (global_directive_node->kind)
				{
					case MD_ASTKind_DirectiveTable: {
						for (; parser->token != parser->tokens_one_past_last && parser->token->kind != MD_TokenKind_CloseBrace; parser->token++)
						{
							if (parser->token->kind != MD_TokenKind_OpenBrace)
							{
								md_messagelist_push(
									parser->arena,
									parser->messages,
									parser->source,
									parser->token->source.buffer,
									MD_MessageKind_FatalError,
									str8_lit("expected '{' to open @table's row"),
									parser->token,
									global_directive_node
								);
								goto break_parse_outer_loop;
							}
							MD_AST *table_row = md_ast_push_child(parser->arena, global_directive_node, MD_ASTKind_TableRow);
							table_row->token = parser->token; // location if there's an error
							while (++parser->token != parser->tokens_one_past_last && parser->token->kind != MD_TokenKind_CloseBrace)
							{
								MD_ASTKind kind = md_token_to_ast_kind_table[parser->token->kind];
								switch (kind)
								{
									case MD_ASTKind_IntLit:
									case MD_ASTKind_FloatLit:
									case MD_ASTKind_StringLit:
									case MD_ASTKind_Ident: {
										MD_AST *table_entry = md_ast_push_child(parser->arena, table_row, kind);
										table_entry->token = parser->token;
									} break;
									default: {
										md_messagelist_push(
											parser->arena,
											parser->messages,
											parser->source,
											parser->token->source.buffer,
											MD_MessageKind_FatalError, // REVIEW: fatal?
											push_str8f(
												parser->arena,
												"illegal @table entry: '%S' - perhaps closing '}' is missing?",
												parser->token->source
											),
											parser->token,
											table_row
										);
										goto break_parse_outer_loop;
									} break;
								}
							}
							if (parser->token == parser->tokens_one_past_last)
							{
								md_messagelist_push(
									parser->arena,
									parser->messages,
									parser->source,
									source_last,
									MD_MessageKind_FatalError,
									str8_lit("expected '}' to close @table's row"),
									parser->token,
									global_directive_node
								);
								goto break_parse_outer_loop;
							}
							if (table_row->children_count == 0 || table_row->children_count != directive_ident_params->children_count)
							{
								MD_MessageKind message_kind = 0;
								String8 message = zero_struct;
								if (table_row->children_count == 0 && directive_ident_params->children_count == 0)
								{
									message_kind = MD_MessageKind_Warning;
									message = str8_lit("empty @table row");
								}
								else
								{
									message_kind = MD_MessageKind_Error;
									message = push_str8f(
										parser->arena,
										"@table row element number mismatch: expected %d, got %d",
										directive_ident_params->children_count,
										table_row->children_count
									);
								}
								md_messagelist_push(
									parser->arena,
									parser->messages,
									parser->source,
									table_row->token->source.buffer, // row's opening '{' REVIEW: closing '}'
									message_kind,
									message,
									0, // REVIEW: row's opening '{'?
									table_row
								);
							}
						}
					} break;
					case MD_ASTKind_DirectiveGenH:
					case MD_ASTKind_DirectiveGenC:
					case MD_ASTKind_DirectiveEnum:
					case MD_ASTKind_DirectiveStruct:
					case MD_ASTKind_DirectiveArray: {
						Assert(global_directive_node->kind != MD_ASTKind_DirectiveStruct); // not implemented
						while (parser->token != parser->tokens_one_past_last && parser->token->kind != MD_TokenKind_CloseBrace)
						{
							switch (parser->token->kind)
							{
								case MD_TokenKind_StringLit: {
									MD_AST *string = md_ast_push_child(parser->arena, global_directive_node, MD_ASTKind_StringLit);
									string->token = parser->token++;
								} break;
								case MD_TokenKind_DirectiveExpand: {
									MD_AST *directive_expand = md_ast_push_child(
										parser->arena,
										global_directive_node,
										MD_ASTKind_DirectiveExpand
									);
									directive_expand->token = parser->token;
									if (++parser->token == parser->tokens_one_past_last || parser->token->kind != MD_TokenKind_OpenParen)
									{
										md_messagelist_push(
											parser->arena,
											parser->messages,
											parser->source,
											parser->token == parser->tokens_one_past_last
												? source_last
												: parser->token->source.buffer,
											MD_MessageKind_FatalError,
											str8_lit("expected '(' to open @expand's argument list"),
											parser->token,
											directive_expand
										);
										goto break_parse_outer_loop;
									}
									if (++parser->token == parser->tokens_one_past_last || parser->token->kind != MD_TokenKind_Ident)
									{
										md_messagelist_push(
											parser->arena,
											parser->messages,
											parser->source,
											parser->token == parser->tokens_one_past_last
												? source_last
												: parser->token->source.buffer,
											MD_MessageKind_FatalError, // REVIEW: fatal?
											str8_lit("expected identifier for @expand's first argument"),
											parser->token,
											directive_expand
										);
										goto break_parse_outer_loop;
									}
									{
										MD_AST *expand_target = md_ast_push_child(parser->arena, directive_expand, MD_ASTKind_Ident);
										expand_target->token = parser->token;
									}
									if (++parser->token == parser->tokens_one_past_last || parser->token->kind != MD_TokenKind_StringLit)
									{
										md_messagelist_push(
											parser->arena,
											parser->messages,
											parser->source,
											parser->token == parser->tokens_one_past_last
												? source_last
												: parser->token->source.buffer,
											MD_MessageKind_FatalError, // REVIEW: fatal?
											str8_lit("expected format string for @expand's second argument"),
											parser->token,
											directive_expand
										);
										goto break_parse_outer_loop;
									}
									{
										MD_AST *format_string = md_ast_push_child(parser->arena, directive_expand, MD_ASTKind_StringLit);
										format_string->token = parser->token;
									}

									while (++parser->token != parser->tokens_one_past_last && parser->token->kind != MD_TokenKind_CloseParen)
									{
										if (parser->token->kind != MD_TokenKind_Ident)
										{
											md_messagelist_push(
												parser->arena,
												parser->messages,
												parser->source,
												parser->token->source.buffer,
												MD_MessageKind_Error, // REVIEW: fatal?
												push_str8f(
													parser->arena,
													"illegal format string argument: '%S' - only identifiers are allowed",
													parser->token->source
												),
												parser->token,
												directive_expand
											);
											continue;
										}
										MD_AST *format_arg = md_ast_push_child(parser->arena, directive_expand, MD_ASTKind_Ident);
										format_arg->token = parser->token;
									}

									if (parser->token == parser->tokens_one_past_last)
									{
										md_messagelist_push(
											parser->arena,
											parser->messages,
											parser->source,
											source_last,
											MD_MessageKind_FatalError,
											str8_lit("expected ')' to close @expand"),
											parser->token,
											directive_expand
										);
										goto break_parse_outer_loop;
									}
									parser->token++;
								} break;
								default: {
									md_messagelist_push(
										parser->arena,
										parser->messages,
										parser->source,
										parser->token->source.buffer,
										MD_MessageKind_FatalError, // REVIEW: fatal?
										push_str8f(
											parser->arena,
											"expected string literal or @expand for entry in %S, got '%S'",
											global_directive_node->token->source,
											parser->token->source
										),
										parser->token,
										global_directive_node // REVIEW: push the token instead of the AST?
									);
									goto break_parse_outer_loop;
								} break;
							}
						}
					} break;
					default: {
						Unreachable
					} break;
				}
				if (parser->token == parser->tokens_one_past_last || parser->token->kind != MD_TokenKind_CloseBrace)
				{
					md_messagelist_push(
						parser->arena,
						parser->messages,
						parser->source,
						parser->token == parser->tokens_one_past_last
							? source_last
							: parser->token->source.buffer,
						MD_MessageKind_FatalError,
						push_str8f(parser->arena, "%S directive missing closing '}'", parser->token->source),
						parser->token,
						global_directive_node
					);
					goto break_parse_outer_loop;
				}
				Assert(global_directive_node->children_count >= children_before_body);

				if (global_directive_node->kind == MD_ASTKind_DirectiveTable)
					directive_symbol->table_record.num_rows = global_directive_node->children_count - children_before_body;

				if (children_before_body == global_directive_node->children_count)
				{
					md_messagelist_push(
						parser->arena,
						parser->messages,
						parser->source,
						global_directive_node->token->source.buffer,
						MD_MessageKind_Warning,
						push_str8f(
							parser->arena,
							"empty %S directive",
							global_directive_node->token->source
						),
						0, // REVIEW
						global_directive_node
					);
				}
				parser->token++;
			} break;
			default: {
				md_messagelist_push(
					parser->arena,
					parser->messages,
					parser->source,
					parser->token->source.buffer,
					MD_MessageKind_FatalError,
					push_str8f(parser->arena, "only @enum, @table, or @Array allowed at global scope, got '%S'", parser->token->source),
					parser->token,
					0
				);
			} break;
		}
	}

	break_parse_outer_loop:;

	return root;
}

internal MD_MessageList
md_check_parsed(Arena *arena, MD_AST *root, MD_SymbolTableEntry *stab, String8 source)
{
	MD_MessageList result = zero_struct;
	Temp scratch = scratch_begin(&arena, 1);

	for (MD_AST *global_directive = root->first; global_directive != 0; global_directive = global_directive->next)
	{
		switch (global_directive->kind)
		{
			case MD_ASTKind_DirectiveTable: {
				MD_AST *table_child = global_directive->first->next;
				MD_SymbolTableEntry *table_symbol = md_symbol_from_ident(0, &stab, table_child->token->source);
				String8List table_elems = zero_struct;
				for (table_child = table_child->next; table_child != 0; table_child = table_child->next)
				{
					Assert(table_child->kind == MD_ASTKind_TableRow);
					for (MD_AST *table_elem = table_child->first; table_elem != 0; table_elem = table_elem->next)
					{
						Assert(
							   table_elem->kind == MD_ASTKind_Ident
							|| table_elem->kind == MD_ASTKind_IntLit
							|| table_elem->kind == MD_ASTKind_FloatLit
							|| table_elem->kind == MD_ASTKind_StringLit
						);
						str8_list_push(scratch.arena, &table_elems, table_elem->token->source);
					}
				}
				Assert(table_elems.node_count == table_symbol->table_record.num_cols * table_symbol->table_record.num_rows);
				String8Array table_matrix = str8_array_from_list(arena, table_elems);
				table_symbol->table_record.elem_matrix = table_matrix.v;
			} break;
			case MD_ASTKind_DirectiveGenH:
			case MD_ASTKind_DirectiveGenC:
			case MD_ASTKind_DirectiveEnum:
			case MD_ASTKind_DirectiveArray:
			case MD_ASTKind_DirectiveStruct: {
				Assert(global_directive->kind != MD_ASTKind_DirectiveStruct); // not implemented
				MD_AST *global_directive_child = global_directive->first;
				switch (global_directive->kind)
				{
					case MD_ASTKind_DirectiveArray: {
						global_directive_child = global_directive_child->next; // skip the identlist and the name
					} // fallthrough
					case MD_ASTKind_DirectiveEnum:
					case MD_ASTKind_DirectiveStruct: {
						global_directive_child = global_directive_child->next; // skip the enum name
					} // fallthrough
					case MD_ASTKind_DirectiveGenH:
					case MD_ASTKind_DirectiveGenC: break; // already at the gen children
					default: {
						Unreachable;
					} break;
				}

				for (; global_directive_child != 0; global_directive_child = global_directive_child->next)
				{
					Assert(global_directive_child->kind == MD_ASTKind_StringLit || global_directive_child->kind == MD_ASTKind_DirectiveExpand);
					if (global_directive_child->kind == MD_ASTKind_StringLit)
						continue;
					Assert(global_directive_child->kind == MD_ASTKind_DirectiveExpand && global_directive_child->children_count >= 2);
					MD_AST *expand_arg = global_directive_child->first;
					MD_SymbolTableEntry *target_symbol = md_symbol_from_ident(0, &stab, expand_arg->token->source);
					if (!target_symbol || target_symbol->ast->kind != MD_ASTKind_DirectiveTable)
					{
						Assert(!target_symbol || target_symbol->ast->kind == MD_ASTKind_DirectiveArray || target_symbol->ast->kind == MD_ASTKind_DirectiveEnum);
						char* format = target_symbol
							? "@expand-ing a non-@table symbol '%S'"
							: "@expand-ing an undefined symbol '%S'";
						String8 message = push_str8f(arena, format, expand_arg->token->source);
						md_messagelist_push(
							arena,
							&result,
							source,
							expand_arg->token->source.buffer,
							MD_MessageKind_Error,
							message,
							expand_arg->token,
							global_directive_child
						);
						continue;
					}
					expand_arg = expand_arg->next;
					Assert(expand_arg->kind == MD_ASTKind_StringLit);

					for (expand_arg = expand_arg->next; expand_arg != 0; expand_arg = expand_arg->next)
					{
						Assert(expand_arg->kind = MD_ASTKind_Ident);
						MD_SymbolTableEntry *format_arg_symbol = md_symbol_from_ident(
							0,
							&target_symbol->table_record.cols_stab,
							expand_arg->token->source
						);
						if (!format_arg_symbol)
						{
							md_messagelist_push(
								arena,
								&result,
								source,
								expand_arg->token->source.buffer,
								MD_MessageKind_Error,
								push_str8f(
									arena,
									"undefined format arg to @expand - '%S' is not a column of %S",
									expand_arg->token->source,
									target_symbol->key
								),
								expand_arg->token,
								global_directive_child
							);
						}
					}
				}
			} break;
			default: {
				Unreachable; // REVIEW: print an internal error?
			} break;
		}
	}

	scratch_end(scratch);
	return result;
}

internal MD_AST*
md_ast_push_child(Arena *arena, MD_AST *parent, MD_ASTKind kind)
{
	MD_AST *result = push_array_no_zero(arena, MD_AST, 1);
	*result = (MD_AST) {
		.kind = kind,
	};
	SLLQueuePush(parent->first, parent->last, result);
	result->parent = parent;
	parent->children_count++;
	return result;
}

internal inline MD_Message*
md_messagelist_push_inner(Arena *arena, MD_MessageList *messages, String8 source, U8* source_loc, MD_MessageKind kind, String8 string)
{
	const local_persist
	char *msg_kind_prefix_table[MD_MessageKind_COUNT] = {
		[MD_MessageKind_Warning   ] = "warning",
		[MD_MessageKind_Error     ] = "error",
		[MD_MessageKind_FatalError] = "error",
	};

	const char *prefix = msg_kind_prefix_table[kind];
	Assert(prefix != 0);

	// REVIEW: speed?
	Vec2U64 pos = str8_pos_from_offset(source, (U64)(source_loc - source.buffer));

	MD_Message *message = push_array_no_zero(arena, MD_Message, 1);
	*message = (MD_Message) {
		.kind      = kind,
		.string    = push_str8f(
			arena,
			"(%llu, %llu): %s: %S",
			pos.line,
			pos.column,
			prefix,
			string
		),
	};
	SLLQueuePush(messages->first, messages->last, message);
	messages->count++;
	if (kind > messages->worst_message)
		messages->worst_message = kind;
	return message;
}

internal void
md_messagelist_push(Arena *arena, MD_MessageList *messages, String8 source, U8 *source_loc, MD_MessageKind kind, String8 string, MD_Token *token, MD_AST *ast)
{
	MD_Message *result = md_messagelist_push_inner(arena, messages, source, source_loc, kind, string);
	result->token = token;
	result->ast = ast;
}

internal U64 md_hash_ident(String8 ident)
{
	U64 result = chibihash64(ident.buffer, (ptrdiff_t)ident.length, 0);
	return result;
}

// TODO: check that its not just a linked list degredation every time
internal MD_SymbolTableEntry*
md_symbol_from_ident(Arena *arena, MD_SymbolTableEntry** stab, String8 ident)
{
	Assert(ident.length > 0);
	for (U64 hash = md_hash_ident(ident); *stab != 0; hash <<=2)
	{
		if (str8_match((*stab)->key, ident, 0))
			goto finish;
		stab = &(*stab)->slots[hash >> 62];
	}
	if (arena != 0)
	{
		*stab = push_array_no_zero(arena, MD_SymbolTableEntry, 1);
		**stab = (MD_SymbolTableEntry) {
			.key = ident,
		};
	}

	finish:;
	return *stab;
}
