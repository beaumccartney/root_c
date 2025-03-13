// TODO: refactor to just messagelist_push where stuff comes up. this way its
// possible to keep scanning even if there's errors to help report more errors
internal MD_TokenizeResult
md_tokens_from_source(Arena *arena, String8 source)
{
	Assert(0 <= source.length);
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
						S64 comment_stack = 1;
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
					{str8_lit_comp("h_file"),          MD_TokenKind_DirectiveHFile          },
					{str8_lit_comp("c_file"),          MD_TokenKind_DirectiveCFile          },

					{str8_lit_comp("top"),             MD_TokenKind_DirectiveTop            },
					{str8_lit_comp("enums"),           MD_TokenKind_DirectiveEnums          },
					{str8_lit_comp("structs"),         MD_TokenKind_DirectiveStructs        },
					{str8_lit_comp("functions"),       MD_TokenKind_DirectiveFunctions      },
					{str8_lit_comp("arrays"),          MD_TokenKind_DirectiveArrays         },
					{str8_lit_comp("embedded_strings"), MD_TokenKind_DirectiveEmbeddedStrings},
					{str8_lit_comp("embedded_files"),   MD_TokenKind_DirectiveEmbeddedFiles  },
					{str8_lit_comp("bottom"),          MD_TokenKind_DirectiveBottom         },

					{str8_lit_comp("table"),           MD_TokenKind_DirectiveTable          },
					{str8_lit_comp("gen"),             MD_TokenKind_DirectiveGen            },
					{str8_lit_comp("enum"),            MD_TokenKind_DirectiveEnum           },
					{str8_lit_comp("struct"),          MD_TokenKind_DirectiveStruct         },
					{str8_lit_comp("array"),           MD_TokenKind_DirectiveArray          },
					{str8_lit_comp("expand"),          MD_TokenKind_DirectiveExpand         },

					{str8_lit_comp("embed_file"),      MD_TokenKind_DirectiveEmbedFile      },
					{str8_lit_comp("embed_string"),    MD_TokenKind_DirectiveEmbedString    },
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
				String8 end_delim = str8_lit("\"");
				if ((c + 1) != one_past_last && c[1] == '"' && (c + 2) != one_past_last && c[2] == '"')
				{
					token_kind = MD_TokenKind_RawStringLit;
					end_delim = str8_lit("\"\"\"");
					c += 2;
				}
				while (++c <= one_past_last - end_delim.length && !MemoryMatch(c, end_delim.buffer, end_delim.length))
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
									scratch.arena,
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
				{
					Assert(token_kind == MD_TokenKind_StringLit || token_kind == MD_TokenKind_RawStringLit);
					c++;
					if (token_kind == MD_TokenKind_RawStringLit)
						c += 2; // skip the ending """
				}
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
				const local_persist
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
		.tokens.v     = push_array_no_zero(arena, MD_Token, toklist.count + 1),
		.tokens.count = toklist.count,
		.messages     = messages,
	};
	result.tokens.v[result.tokens.count] = (MD_Token) zero_struct; // null terminate to catch after end bugs
	MD_Token *dst = result.tokens.v;
	for (MD_TokenNode *n = toklist.first; n != 0; n = n->next, dst++)
	{
		MD_Token src = n->token;
		*dst = src;
	}
	Assert(dst == result.tokens.v + result.tokens.count);
	scratch_end(scratch);
	return result;
}

internal MD_ParseResult
md_parse_from_tokens(Arena *arena, MD_TokenArray tokens, String8 source)
{
	Assert(0 <= tokens.count && 0 <= source.length);
	MD_ParseResult result = zero_struct;

	MD_Token *token = tokens.v,
		 *tokens_one_past_last = tokens.v + tokens.count;

	result.root = push_array(arena, MD_AST, 1);
	result.root->kind = MD_ASTKind_Root;

	U8 *source_last = source.buffer + source.length - 1;

	// accumulators for directives specifying where code is generated
	// remember to clear when its used
	MD_GenFile gen_file    = MD_GenFile_NULL;
	MD_GenLocation gen_loc = MD_GenLocation_NULL;
	while (token < tokens_one_past_last)
	{
		const local_persist MD_ASTKind
		md_token_to_ast_kind_table[MD_TokenKind_COUNT] = {
			[MD_TokenKind_IntLit              ] = MD_ASTKind_IntLit,
			[MD_TokenKind_FloatLit            ] = MD_ASTKind_FloatLit,
			[MD_TokenKind_StringLit           ] = MD_ASTKind_StringLit,
			[MD_TokenKind_RawStringLit        ] = MD_ASTKind_RawStringLit,

			[MD_TokenKind_DirectiveTable      ] = MD_ASTKind_DirectiveTable,

			[MD_TokenKind_DirectiveGen        ] = MD_ASTKind_DirectiveGen,
			[MD_TokenKind_DirectiveEnum       ] = MD_ASTKind_DirectiveEnum,
			[MD_TokenKind_DirectiveStruct     ] = MD_ASTKind_DirectiveStruct,
			[MD_TokenKind_DirectiveArray      ] = MD_ASTKind_DirectiveArray,
			[MD_TokenKind_DirectiveExpand     ] = MD_ASTKind_DirectiveExpand,

			[MD_TokenKind_DirectiveEmbedString] = MD_ASTKind_DirectiveEmbedString,
			[MD_TokenKind_DirectiveEmbedFile  ] = MD_ASTKind_DirectiveEmbedFile,

			[MD_TokenKind_Ident               ] = MD_ASTKind_Ident,
		};

		switch (token->kind)
		{
			case MD_TokenKind_DirectiveHFile:
			case MD_TokenKind_DirectiveCFile:
			case MD_TokenKind_DirectiveTop:
			case MD_TokenKind_DirectiveEnums:
			case MD_TokenKind_DirectiveStructs:
			case MD_TokenKind_DirectiveFunctions:
			case MD_TokenKind_DirectiveArrays:
			case MD_TokenKind_DirectiveEmbeddedStrings:
			case MD_TokenKind_DirectiveEmbeddedFiles:
			case MD_TokenKind_DirectiveBottom: {
				U64 val = 0, null = 0;
				String8 message = zero_struct;
				if (token->kind == MD_TokenKind_DirectiveHFile || token->kind == MD_TokenKind_DirectiveCFile)
				{
					val  = gen_file;
					null = MD_GenFile_NULL;
					message = str8_lit("overriding previously specified generation file");
					gen_file = token->kind == MD_TokenKind_DirectiveHFile ? MD_GenFile_H : MD_GenFile_C;
				}
				else
				{
					val  = gen_loc;
					switch (token->kind)
					{
						case MD_TokenKind_DirectiveTop:             gen_loc = MD_GenLocation_Top;             break;
						case MD_TokenKind_DirectiveEnums:           gen_loc = MD_GenLocation_Enums;           break;
						case MD_TokenKind_DirectiveStructs:         gen_loc = MD_GenLocation_Structs;         break;
						case MD_TokenKind_DirectiveFunctions:       gen_loc = MD_GenLocation_Functions;       break;
						case MD_TokenKind_DirectiveArrays:          gen_loc = MD_GenLocation_Arrays;          break;
						case MD_TokenKind_DirectiveEmbeddedStrings: gen_loc = MD_GenLocation_EmbeddedStrings; break;
						case MD_TokenKind_DirectiveEmbeddedFiles:   gen_loc = MD_GenLocation_EmbeddedFiles;   break;
						case MD_TokenKind_DirectiveBottom:          gen_loc = MD_GenLocation_Bottom;          break;
						default: {
							Unreachable;
						} break;
					}
					null = MD_GenLocation_NULL;
					message = str8_lit("overriding previously specified generation location");
				}
				Assert(message.length > 0);
				if (val != null)
				{
					md_messagelist_push(
						arena,
						&result.messages,
						source,
						token->source.buffer,
						token,
						0,
						MD_MessageKind_Warning,
						message
					);
				}
				token++;
				continue;
			} break;
			case MD_TokenKind_DirectiveEmbedString:
			case MD_TokenKind_DirectiveEmbedFile:
			case MD_TokenKind_DirectiveGen:
			case MD_TokenKind_DirectiveTable:
			case MD_TokenKind_DirectiveEnum:
			case MD_TokenKind_DirectiveStruct:
			case MD_TokenKind_DirectiveArray: {
				MD_AST *global_directive = md_ast_push_child(arena, result.root, md_token_to_ast_kind_table[token->kind], token);
				global_directive->gen_file = gen_file;
				global_directive->gen_loc  = gen_loc;
				gen_file                   = MD_GenFile_NULL;
				gen_loc                    = MD_GenLocation_NULL;
				token++;

				MD_SymbolTableEntry *directive_symbol = 0;
				switch (global_directive->kind) // check for name on directive
				{
					case MD_ASTKind_DirectiveEmbedString:
					case MD_ASTKind_DirectiveEmbedFile:
					case MD_ASTKind_DirectiveTable:
					case MD_ASTKind_DirectiveEnum:
					case MD_ASTKind_DirectiveStruct:
					case MD_ASTKind_DirectiveArray: {
						if (token == tokens_one_past_last || token->kind != MD_TokenKind_Ident)
						{
							md_messagelist_pushf(
								arena,
								&result.messages,
								source,
								token == tokens_one_past_last
									? source_last
									: token->source.buffer,
								token,
								global_directive,
								MD_MessageKind_FatalError,
								"missing name for %S directive",
								global_directive->token->source
							);
							goto break_parse_outer_loop;
						}

						Assert(token->kind == MD_TokenKind_Ident);
						directive_symbol = md_symbol_from_ident(
							arena,
							&result.global_stab,
							token->source
						);
						if (directive_symbol->ast != 0)
						{
							// TODO: directive_symbol->key points
							// to the original declaration of the
							// symbol, use it to find the position
							// of the original declaration and
							// report in the error message
							md_messagelist_pushf(
								arena,
								&result.messages,
								source,
								token->source.buffer,
								token,
								global_directive, // REVIEW: the directive_name instead?
								MD_MessageKind_FatalError, // NOTE: because the symbol table entry holds table columns attempting to parse this directive could add columns to a different directive's column table
								"redeclaration of '%S'",
								directive_symbol->ident
							);
							goto break_parse_outer_loop; // REVIEW: fatal only for @table (either on original or duplicate)? i.e. circumstances where its guaranteed that no symbol taint can happen
						}
						directive_symbol->ast = global_directive;
						md_ast_push_child(arena, global_directive, MD_ASTKind_Ident, token);
						token++;
					} break;
					case MD_ASTKind_DirectiveGen: break; // no name or symbol for @gen
					default: {
						Unreachable;
					} break;
				}

				if (global_directive->kind == MD_ASTKind_DirectiveTable || global_directive->kind == MD_ASTKind_DirectiveArray || global_directive->kind == MD_ASTKind_DirectiveEnum)
				{
					if (token == tokens_one_past_last)
					{
						md_messagelist_push(
							arena,
							&result.messages,
							source,
							source_last,
							0,
							global_directive,
							MD_MessageKind_FatalError,
							str8_lit("unexpected end of input")
						);
						goto break_parse_outer_loop;
					}
					B8 directive_has_paren_args = 1;
					if (token->kind != MD_TokenKind_OpenParen)
					{
						if (global_directive->kind == MD_ASTKind_DirectiveTable || global_directive->kind == MD_ASTKind_DirectiveArray)
						{
							md_messagelist_pushf(
								arena,
								&result.messages,
								source,
								token == tokens_one_past_last
								? source_last
								: token->source.buffer,
								token,
								global_directive,
								MD_MessageKind_FatalError,
								"expected '(' to open %S param list", // REVIEW
								global_directive->token->source
							);
							goto break_parse_outer_loop;
						}
						directive_has_paren_args = 0;
					}
					if (directive_has_paren_args)
					{
						token++;
					}
					else
					{
						Assert(global_directive->kind == MD_ASTKind_DirectiveEnum);
					}
					if (global_directive->kind == MD_ASTKind_DirectiveTable)
					{
						if (global_directive->gen_file != MD_GenFile_NULL || global_directive->gen_loc != MD_GenLocation_NULL)
						{
							md_messagelist_push(
								arena,
								&result.messages,
								source,
								global_directive->token->source.buffer,
								global_directive->token,
								global_directive,
								MD_MessageKind_Warning,
								str8_lit("location or file specifier(s) used on @table - tables are not generated directly so these have no effect") // REVIEW
							);
						}
						S64 col_num = 0;
						for (; token < tokens_one_past_last && token->kind != MD_TokenKind_CloseParen; col_num++, token++)
						{
							if (token->kind != MD_TokenKind_Ident)
							{
								md_messagelist_pushf(
									arena,
									&result.messages,
									source,
									token->source.buffer,
									token,
									global_directive,
									MD_MessageKind_FatalError,
									"non-ident argument to %S directive: '%S' - perhaps a closing ')' is missing?", // REVIEW: better message
									global_directive->token->source,
									token->source
								);
								goto break_parse_outer_loop;
							}

							MD_SymbolTableEntry *column_symbol = md_symbol_from_ident(
								arena,
								&directive_symbol->table_record.cols_stab,
								token->source
							);
							if (column_symbol->ast != 0)
							{
								md_messagelist_pushf(
									arena,
									&result.messages,
									source,
									token->source.buffer,
									token,
									global_directive, // REVIEW: ast of the name itself?
									MD_MessageKind_Error,
									"duplicate column name '%S' in @table '%S",
									token->source,
									directive_symbol->ident
								);
							}
							else
							{
								column_symbol->col_record.col = col_num;
								// REVIEW: pointer to the token that names the column?
							}
						}
						if ((directive_symbol->table_record.num_cols = col_num) < 1)
						{
							md_messagelist_pushf(
								arena,
								&result.messages,
								source,
								global_directive->token->source.buffer,
								0, // REVIEW
								global_directive,
								MD_MessageKind_Warning,
								"@table directive '%S' has no named columns",
								directive_symbol->ident
							);
						}
					}
					else if (directive_has_paren_args)
					{
						Assert(global_directive->kind == MD_ASTKind_DirectiveArray || global_directive->kind == MD_ASTKind_DirectiveEnum);
						if (token == tokens_one_past_last || (token->kind != MD_TokenKind_Ident && token->kind != MD_TokenKind_StringLit))
						{
							md_messagelist_pushf(
								arena,
								&result.messages,
								source,
								token->source.buffer,
								token,
								global_directive,
								MD_MessageKind_FatalError,
								"expected type specifier for %S", // REVIEW
								global_directive->token->source
							);
							goto break_parse_outer_loop;
						}
						else if (token->kind == MD_TokenKind_StringLit && token->source.length == 2)
						{
							Assert(token->source.buffer[0] == '"' && token->source.buffer[1] == '"');
							md_messagelist_pushf(
								arena,
								&result.messages,
								source,
								token->source.buffer,
								token,
								global_directive,
								MD_MessageKind_Error,
								"empty type specifier for %S %S",
								global_directive->token->source,
								directive_symbol->ident
							);
						}
						MD_ASTKind kind = md_token_to_ast_kind_table[token->kind];
						Assert(kind == MD_ASTKind_StringLit || kind == MD_ASTKind_Ident);
						directive_symbol->named_gen_record.token1 = token;
						token++;
						if (global_directive->kind == MD_ASTKind_DirectiveArray && token < tokens_one_past_last && token->kind != MD_TokenKind_CloseParen)
						{
							if (token == tokens_one_past_last || (token->kind != MD_TokenKind_Ident && token->kind != MD_TokenKind_StringLit))
							{
								md_messagelist_push(
									arena,
									&result.messages,
									source,
									token->source.buffer,
									token,
									global_directive,
									MD_MessageKind_FatalError,
									str8_lit("expected length specifier for @array") // REVIEW:
								);
								goto break_parse_outer_loop;
							}
							else if (token->kind == MD_TokenKind_StringLit && token->source.length == 2)
							{
								Assert(token->source.buffer[0] == '"' && token->source.buffer[1] == '"');
								md_messagelist_pushf(
									arena,
									&result.messages,
									source,
									token->source.buffer,
									token,
									global_directive,
									MD_MessageKind_Error,
									"empty length specifier for @array %S",
									directive_symbol->ident
								);
							}
							kind = md_token_to_ast_kind_table[token->kind];
							Assert(kind == MD_ASTKind_StringLit || kind == MD_ASTKind_Ident);
							directive_symbol->named_gen_record.token2 = token;
							token++;
						}
					}
					if (directive_has_paren_args)
					{
						if (token == tokens_one_past_last || token->kind != MD_TokenKind_CloseParen)
						{
							md_messagelist_pushf(
								arena,
								&result.messages,
								source,
								source_last,
								token,
								global_directive,
								MD_MessageKind_FatalError,
								"unclosed parameter list for %S - missing ')'",
								global_directive->token->source
							);
							goto break_parse_outer_loop;
						}
						token++;
					}
					// TODO: compress
					if (global_directive->kind == MD_ASTKind_DirectiveArray || (global_directive->kind == MD_ASTKind_DirectiveEnum && directive_has_paren_args))
						Assert(directive_symbol->named_gen_record.token1);
					Assert(global_directive->kind != MD_ASTKind_DirectiveEnum || !directive_symbol->named_gen_record.token2);
				}
				if (global_directive->kind == MD_ASTKind_DirectiveEmbedString || global_directive->kind == MD_ASTKind_DirectiveEmbedFile)
				{
					MD_TokenKind string_kind = MD_TokenKind_StringLit;
					char *path_or_raw        = "path";
					if (global_directive->kind == MD_ASTKind_DirectiveEmbedString)
					{
						string_kind = MD_TokenKind_RawStringLit;
						path_or_raw = "raw";
					}
					if (token == tokens_one_past_last || token->kind != string_kind)
					{
						md_messagelist_pushf(
							arena,
							&result.messages,
							source,
							token == tokens_one_past_last
								? source_last
								: token->source.buffer,
							token,
							global_directive,
							MD_MessageKind_FatalError, // REVIEW: fatal?
							"%expected a %s string for %S directive",
							path_or_raw,
							global_directive->token->source
						);
						goto break_parse_outer_loop;
					}
					directive_symbol->named_gen_record.token1 = token;
					token++;
					break;
				}

				if (token == tokens_one_past_last || token->kind != MD_TokenKind_OpenBrace)
				{
					md_messagelist_pushf(
						arena,
						&result.messages,
						source,
						token == tokens_one_past_last
							? source_last
							: token->source.buffer,
						token,
						global_directive,
						MD_MessageKind_FatalError,
						"%S directive missing missing opening '{'",
						global_directive->token->source
					);
					goto break_parse_outer_loop;
				}
				S64 children_before_body = global_directive->children_count; // REVIEW: body ast to hold the children count?
				token++;
				switch (global_directive->kind)
				{
					case MD_ASTKind_DirectiveTable: {
						for (; token < tokens_one_past_last && token->kind != MD_TokenKind_CloseBrace; token++)
						{
							if (token->kind != MD_TokenKind_OpenBrace)
							{
								md_messagelist_push(
									arena,
									&result.messages,
									source,
									token->source.buffer,
									token,
									global_directive,
									MD_MessageKind_FatalError,
									str8_lit("expected '{' to open @table's row")
								);
								goto break_parse_outer_loop;
							}
							MD_AST *table_row = md_ast_push_child(arena, global_directive, MD_ASTKind_List, token);
							while (++token < tokens_one_past_last && token->kind != MD_TokenKind_CloseBrace)
							{
								MD_ASTKind kind = md_token_to_ast_kind_table[token->kind];
								switch (kind)
								{
									case MD_ASTKind_IntLit:
									case MD_ASTKind_FloatLit:
									case MD_ASTKind_StringLit:
									case MD_ASTKind_Ident: {
										MD_AST *table_entry = md_ast_push_child(arena, table_row, kind, token);
										Unused(table_entry);
									} break;
									default: {
										md_messagelist_pushf(
											arena,
											&result.messages,
											source,
											token->source.buffer,
											token,
											table_row,
											MD_MessageKind_FatalError, // REVIEW: fatal?
											"illegal @table entry: '%S' - perhaps closing '}' is missing?",
											token->source
										);
										goto break_parse_outer_loop;
									} break;
								}
							}
							if (token == tokens_one_past_last)
							{
								md_messagelist_push(
									arena,
									&result.messages,
									source,
									source_last,
									token,
									global_directive,
									MD_MessageKind_FatalError,
									str8_lit("expected '}' to close @table's row")
								);
								goto break_parse_outer_loop;
							}
							if (table_row->children_count == 0 || table_row->children_count != directive_symbol->table_record.num_cols)
							{
								if (table_row->children_count == 0 && directive_symbol->table_record.num_cols)
								{
									md_messagelist_push(
										arena,
										&result.messages,
										source,
										table_row->token->source.buffer, // row's opening '{' REVIEW: closing '}'
										0, // REVIEW: row's opening '{'?
										table_row,
										MD_MessageKind_Warning,
										str8_lit("empty @table row")
									);
								}
								else
								{
									md_messagelist_pushf(
										arena,
										&result.messages,
										source,
										table_row->token->source.buffer, // row's opening '{' REVIEW: closing '}'
										0, // REVIEW: row's opening '{'?
										table_row,
										MD_MessageKind_Error,
										"@table row element number mismatch: expected %d, got %d",
										directive_symbol->table_record.num_cols,
										table_row->children_count
									);
								}
							}
						}
					} break;
					case MD_ASTKind_DirectiveGen:
					case MD_ASTKind_DirectiveEnum:
					case MD_ASTKind_DirectiveStruct:
					case MD_ASTKind_DirectiveArray: {
						while (token < tokens_one_past_last && token->kind != MD_TokenKind_CloseBrace)
						{
							switch (token->kind)
							{
								case MD_TokenKind_StringLit: {
									MD_AST *string = md_ast_push_child(arena, global_directive, MD_ASTKind_StringLit, token);
									Unused(string);
									token++;
								} break;
								case MD_TokenKind_DirectiveExpand: {
									MD_AST *directive_expand = md_ast_push_child(
										arena,
										global_directive,
										MD_ASTKind_DirectiveExpand,
										token
									);
									if (++token == tokens_one_past_last || token->kind != MD_TokenKind_OpenParen)
									{
										md_messagelist_push(
											arena,
											&result.messages,
											source,
											token == tokens_one_past_last
												? source_last
												: token->source.buffer,
											token,
											directive_expand,
											MD_MessageKind_FatalError,
											str8_lit("expected '(' to open @expand's argument list")
										);
										goto break_parse_outer_loop;
									}
									if (++token == tokens_one_past_last || token->kind != MD_TokenKind_Ident)
									{
										md_messagelist_push(
											arena,
											&result.messages,
											source,
											token == tokens_one_past_last
												? source_last
												: token->source.buffer,
											token,
											directive_expand,
											MD_MessageKind_FatalError, // REVIEW: fatal?
											str8_lit("expected identifier for @expand's first argument")
										);
										goto break_parse_outer_loop;
									}
									{
										MD_AST *expand_target = md_ast_push_child(arena, directive_expand, MD_ASTKind_Ident, token);
										Unused(expand_target);
									}
									if (++token == tokens_one_past_last || token->kind != MD_TokenKind_StringLit)
									{
										md_messagelist_push(
											arena,
											&result.messages,
											source,
											token == tokens_one_past_last
												? source_last
												: token->source.buffer,
											token,
											directive_expand,
											MD_MessageKind_FatalError, // REVIEW: fatal?
											str8_lit("expected format string for @expand's second argument")
										);
										goto break_parse_outer_loop;
									}
									{
										MD_AST *format_string = md_ast_push_child(arena, directive_expand, MD_ASTKind_StringLit, token);
										Unused(format_string);
									}

									while (++token < tokens_one_past_last && token->kind != MD_TokenKind_CloseParen)
									{
										if (token->kind != MD_TokenKind_Ident)
										{
											md_messagelist_pushf(
												arena,
												&result.messages,
												source,
												token->source.buffer,
												token,
												directive_expand,
												MD_MessageKind_Error, // REVIEW: fatal?
												"illegal format string argument: '%S' - only identifiers are allowed",
												token->source
											);
											continue;
										}
										MD_AST *format_arg = md_ast_push_child(arena, directive_expand, MD_ASTKind_Ident, token);
										Unused(format_arg);
									}

									if (token == tokens_one_past_last)
									{
										md_messagelist_push(
											arena,
											&result.messages,
											source,
											source_last,
											token,
											directive_expand,
											MD_MessageKind_FatalError,
											str8_lit("expected ')' to close @expand")
										);
										goto break_parse_outer_loop;
									}
									token++;
								} break;
								default: {
									md_messagelist_pushf(
										arena,
										&result.messages,
										source,
										token->source.buffer,
										token,
										global_directive, // REVIEW: push the token instead of the AST?
										MD_MessageKind_FatalError, // REVIEW: fatal?
										"expected string literal or @expand for entry in %S, got '%S'",
										global_directive->token->source,
										token->source
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
				if (token == tokens_one_past_last || token->kind != MD_TokenKind_CloseBrace)
				{
					md_messagelist_pushf(
						arena,
						&result.messages,
						source,
						token == tokens_one_past_last
							? source_last
							: token->source.buffer,
						token,
						global_directive,
						MD_MessageKind_FatalError,
						"%S directive missing closing '}'",
						token->source
					);
					goto break_parse_outer_loop;
				}
				Assert(global_directive->children_count >= children_before_body);

				if (global_directive->kind == MD_ASTKind_DirectiveTable)
					directive_symbol->table_record.num_rows = global_directive->children_count - children_before_body;

				if (children_before_body == global_directive->children_count)
				{
					md_messagelist_pushf(
						arena,
						&result.messages,
						source,
						global_directive->token->source.buffer,
						0, // REVIEW
						global_directive,
						MD_MessageKind_Warning,
						"empty %S directive",
						global_directive->token->source
					);
				}
				token++;
			} break;
			default: {
				md_messagelist_pushf(
					arena,
					&result.messages,
					source,
					token->source.buffer,
					token,
					0,
					MD_MessageKind_FatalError,
					"only @embed_string, @embed_file, @table, @gen_h, @gen_c, @enum, @struct, or @array allowed at global scope, got '%S'", // REVIEW
					token->source
				);
			} break;
		}
	}

	break_parse_outer_loop:;

	return result;
}

internal MD_MessageList
md_check_parsed(Arena *arena, MD_AST *root, MD_SymbolTableEntry *stab_root, String8 source)
{
	Assert(0 <= source.length);
	MD_MessageList result = zero_struct;
	Temp scratch = scratch_begin(&arena, 1);

	for (MD_AST *global_directive = root->first; global_directive != 0; global_directive = global_directive->next)
	{
		switch (global_directive->kind)
		{
			case MD_ASTKind_DirectiveTable: {
				MD_AST *table_child = global_directive->first;
				Assert(table_child->kind == MD_ASTKind_Ident);
				MD_SymbolTableEntry *table_symbol = md_symbol_from_ident(0, &stab_root, table_child->token->source);
				String8List table_elems = zero_struct;
				for (table_child = table_child->next; table_child != 0; table_child = table_child->next)
				{
					Assert(table_child->kind == MD_ASTKind_List);
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
			case MD_ASTKind_DirectiveGen:
			case MD_ASTKind_DirectiveEnum:
			case MD_ASTKind_DirectiveArray:
			case MD_ASTKind_DirectiveStruct: {
				MD_AST *global_directive_child = global_directive->first;
				switch (global_directive->kind)
				{
					case MD_ASTKind_DirectiveArray:
					case MD_ASTKind_DirectiveEnum:
					case MD_ASTKind_DirectiveStruct: {
						global_directive_child = global_directive_child->next; // skip the name
					} break;
					case MD_ASTKind_DirectiveGen: break; // already at the gen children
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
					MD_SymbolTableEntry *target_symbol = md_symbol_from_ident(0, &stab_root, expand_arg->token->source);
					if (!target_symbol || target_symbol->ast->kind != MD_ASTKind_DirectiveTable)
					{
						Assert(!target_symbol
						     || target_symbol->ast->kind == MD_ASTKind_DirectiveArray
						     || target_symbol->ast->kind == MD_ASTKind_DirectiveEnum
						     || target_symbol->ast->kind == MD_ASTKind_DirectiveStruct
						     || target_symbol->ast->kind == MD_ASTKind_DirectiveEmbedString
						     || target_symbol->ast->kind == MD_ASTKind_DirectiveEmbedFile
						);
						char* format = target_symbol
							? "@expand-ing a non-@table symbol '%S'"
							: "@expand-ing an undefined symbol '%S'";
						md_messagelist_pushf(
							arena,
							&result,
							source,
							expand_arg->token->source.buffer,
							expand_arg->token,
							global_directive_child,
							MD_MessageKind_Error,
							format,
							expand_arg->token->source
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
							md_messagelist_pushf(
								arena,
								&result,
								source,
								expand_arg->token->source.buffer,
								expand_arg->token,
								global_directive_child,
								MD_MessageKind_Error,
								"undefined format arg to @expand - '%S' is not a column of %S",
								expand_arg->token->source,
								target_symbol->ident
							);
						}
					}
				}
			} break;
			case MD_ASTKind_DirectiveEmbedString:
			case MD_ASTKind_DirectiveEmbedFile: break; // nothing to check
			default: {
				Unreachable; // REVIEW: print an internal error?
			} break;
		}
	}

	scratch_end(scratch);
	return result;
}

internal MD_AST*
md_ast_push_child(Arena *arena, MD_AST *parent, MD_ASTKind kind, MD_Token *token)
{
	MD_AST *result = push_array_no_zero(arena, MD_AST, 1);
	*result = (MD_AST) {
		.kind  = kind,
		.token = token,
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
	char *const msg_kind_prefix_table[MD_MessageKind_COUNT] = {
		[MD_MessageKind_Warning   ] = "warning",
		[MD_MessageKind_Error     ] = "error",
		[MD_MessageKind_FatalError] = "error",
	};

	const char *prefix = msg_kind_prefix_table[kind];
	Assert(prefix != 0);

	// REVIEW: speed?
	Vec2S64 pos = str8_pos_from_offset(source, source_loc - source.buffer);

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
md_messagelist_push(Arena *arena, MD_MessageList *messages, String8 source, U8 *source_loc, MD_Token *token, MD_AST *ast, MD_MessageKind kind, String8 string)
{
	MD_Message *result = md_messagelist_push_inner(arena, messages, source, source_loc, kind, string);
	result->token = token;
	result->ast = ast;
}

internal void
md_messagelist_pushf(Arena *arena, MD_MessageList *messages, String8 source, U8 *source_loc, MD_Token *token, MD_AST *ast, MD_MessageKind kind, char *fmt, ...)
{
	Temp scratch = scratch_begin(&arena, 1);
	va_list args;
	va_start(args, fmt);
	String8 formatted = push_str8fv(scratch.arena, fmt, args);
	md_messagelist_push(arena, messages, source, source_loc, token, ast, kind, formatted);
	va_end(args);
	scratch_end(scratch);
}

internal U64 md_hash_ident(String8 ident)
{
	U64 result = chibihash64(ident.buffer, (ptrdiff_t)ident.length, 0);
	return result;
}

// TODO: check that its not just a linked list degredation every time
internal MD_SymbolTableEntry*
md_symbol_from_ident(Arena *arena, MD_SymbolTableEntry** stab_root, String8 ident)
{
	Assert(0 <= ident.length);
	MD_SymbolTableEntry **stab = stab_root;
	Assert(ident.length > 0);
	for (U64 hash = md_hash_ident(ident); *stab != 0; hash <<=2)
	{
		if (str8_match((*stab)->ident, ident, 0))
			goto finish;
		stab = &(*stab)->slots[hash >> 62];
	}
	if (arena != 0)
	{
		*stab = push_array_no_zero(arena, MD_SymbolTableEntry, 1);
		**stab = (MD_SymbolTableEntry) {
			.ident = ident,
		};
	}

	finish:;
	return *stab;
}
