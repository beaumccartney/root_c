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
				const local_persist MD_Directive_StringToken_Map directive_table[] = {
					{str8_lit_comp("table"),  MD_TokenKind_DirectiveTable },
					{str8_lit_comp("enum"),   MD_TokenKind_DirectiveEnum  },
					{str8_lit_comp("expand"), MD_TokenKind_DirectiveExpand},
					{str8_lit_comp("data"),   MD_TokenKind_DirectiveData  },
				};
				do c++; while (c < one_past_last && (*c == '_' || char_is_alpha(*c)));
				String8 lexeme = str8_region(tok_start + 1, c);
				for EachElement(i, directive_table)
				{
					struct MD_Directive_StringToken_Map entry = directive_table[i];
					if (str8_match(entry.string, lexeme, 0))
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
						c++;
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
				do c++; while (c < one_past_last && (*c == '_' || char_is_alpha(*c)));
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
					[','] = MD_TokenKind_Comma,
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
			md_messagelist_push_token(
				arena,
				&messages,
				message_kind,
				message_string,
				toklist.count - 1
			);
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
		arena,
		&result.messages,
		source,
		tokens.tokens,
		tokens.tokens + tokens.count,
	};

	result.root = md_parse_root(&parser);

	return result;
}

const global MD_ASTKind
md_token_to_ast_kind_table[MD_TokenKind_COUNT] = {
	[MD_TokenKind_IntLit         ] = MD_ASTKind_IntLit,
	[MD_TokenKind_FloatLit       ] = MD_ASTKind_FloatLit,
	[MD_TokenKind_StringLit      ] = MD_ASTKind_StringLit,
	[MD_TokenKind_DirectiveTable ] = MD_ASTKind_DirectiveTable,
	[MD_TokenKind_DirectiveEnum  ] = MD_ASTKind_DirectiveEnum,
	[MD_TokenKind_DirectiveExpand] = MD_ASTKind_DirectiveExpand,
	[MD_TokenKind_DirectiveData  ] = MD_ASTKind_DirectiveData,
	[MD_TokenKind_Ident          ] = MD_ASTKind_Ident,
};

internal MD_AST*
md_parse_root(MD_ParseState *parser)
{
	MD_AST *root = push_array(parser->arena, MD_AST, 1);

	root->kind = MD_ASTKind_Root;

	// REVIEW: what should this loop be delimited by?
	while (parser->token < parser->tokens_one_past_last)
	{
		// REVIEW: only allocate once its known that a valid parse node
		// is present?
		MD_AST *global_directive_node = md_ast_push_child(parser->arena, root, MD_ASTKind_NULL);
		switch (parser->token->kind)
		{
			case MD_TokenKind_DirectiveTable:
			case MD_TokenKind_DirectiveData: {
				// REVIEW: initialize global_directive_node->kind here?
				global_directive_node->token = parser->token;
				if (++parser->token == parser->tokens_one_past_last || parser->token->kind != MD_TokenKind_OpenParen)
				{
					String8 message_str = push_str8f(
						parser->arena,
						"expected '(' after %S",
						parser->token->source
					);
					// TODO: error expected open paren
					md_messagelist_push_ast(
						parser->arena,
						parser->messages,
						MD_MessageKind_FatalError,
						message_str,
						global_directive_node
					);
					break;
				}
				MD_AST *identlist = md_ast_push_child(
					parser->arena,
					global_directive_node,
					MD_ASTKind_IdentList
				);
				while (++parser->token != parser->tokens_one_past_last && parser->token->kind != MD_TokenKind_CloseParen)
				{
					if (parser->token->kind != MD_TokenKind_Ident)
					{
						String8 message_str = push_str8f(
							parser->arena,
							"non-ident argument to %S",
							parser->token->source
						);
						md_messagelist_push_ast(
							parser->arena,
							parser->messages,
							MD_MessageKind_Error, // REVIEW: fatal?
							message_str,
							global_directive_node
						);
						// REVIEW: stop parsing?
					}
					else // ident arg
					{
						MD_AST *ident_node = md_ast_push_child(
							parser->arena,
							identlist,
							MD_ASTKind_Ident
						);
						ident_node->token = parser->token;
					}
				}
				if (parser->token == parser->tokens_one_past_last)
				{
					String8 message_str = push_str8f(
						parser->arena,
						"unclosed argument list for %S - missing ')'",
						parser->token->source
					);
					md_messagelist_push_ast(
						parser->arena,
						parser->messages,
						MD_MessageKind_FatalError,
						message_str,
						global_directive_node
					);
					break;
				}

				if ((++parser->token)->kind != MD_TokenKind_Ident)
				{
					String8 message_str = push_str8f(
						parser->arena,
						"missing name for %S directive",
						parser->token->source
					);
					md_messagelist_push_ast(
						parser->arena,
						parser->messages,
						MD_MessageKind_Error, // REVIEW: fatal?
						message_str,
						global_directive_node
					);
					parser->token++;
				}
				else
				{
					MD_AST *ident_node = md_ast_push_child(
						parser->arena,
						global_directive_node,
						MD_ASTKind_Ident
					);
					ident_node->token = parser->token;
				}
				if ((++parser->token)->kind != MD_TokenKind_OpenBrace)
				{
					String8 message_str = push_str8f(
						parser->arena,
						"missing name for %S directive",
						parser->token->source
					);
					md_messagelist_push_ast(
						parser->arena,
						parser->messages,
						MD_MessageKind_FatalError,
						message_str,
						global_directive_node
					);
					break;
				}
				// REVIEW: set function pointer for which thing
				// to parse inside {} based on @table or @data
				// at the beginning of the case? if that is
				// done, then everything else that depends on
				// which of these can also be set in the same
				// branch
				if (global_directive_node->token->kind == MD_TokenKind_DirectiveTable)
				{
					global_directive_node->kind = MD_ASTKind_DirectiveTable;

					while (++parser->token != parser->tokens_one_past_last && parser->token->kind != MD_TokenKind_CloseBrace)
					{
						if (parser->token->kind != MD_TokenKind_OpenBrace)
						{
							md_messagelist_push_ast(
								parser->arena,
								parser->messages,
								MD_MessageKind_FatalError,
								str8_lit("expected '{' to open @table's row"),
								global_directive_node
							);
							goto break_parse_switch;
						}
						// REVIEW: expect expr list num children is the same as directive ident
						parser->token++;
						MD_AST *table_row = md_parse_exprlist(parser);
						// REVIEW: tablerows ast node to parent each expression list?
						md_ast_add_child(global_directive_node, table_row);
						if (parser->token->kind != MD_TokenKind_CloseBrace)
						{
							md_messagelist_push_ast(
								parser->arena,
								parser->messages,
								MD_MessageKind_FatalError,
								str8_lit("expected '}' to close @table's row"),
								global_directive_node
							);
							goto break_parse_switch;
						}
					}
				}
				else
				{
					Assert(global_directive_node->token->kind == MD_TokenKind_DirectiveData);
					global_directive_node->kind = MD_ASTKind_DirectiveData;
					// TODO: parse children
				}
				if (parser->token == parser->tokens_one_past_last)
				{
					md_messagelist_push_ast(
						parser->arena,
						parser->messages,
						MD_MessageKind_FatalError,
						push_str8f(parser->arena, "%S directive missing closing '}'", parser->token->source),
						global_directive_node
					);
					goto break_parse_switch;
				}
				parser->token++;
			} break;
			case MD_TokenKind_DirectiveEnum: {
				global_directive_node->kind = MD_ASTKind_DirectiveEnum;
				// TODO: expect ident then openbrace
				if (++(parser->token)->kind != MD_TokenKind_Ident)
				{
					md_messagelist_push_ast(
						parser->arena,
						parser->messages,
						MD_MessageKind_FatalError, // REVIEW: fatal?
						str8_lit("expected name for @enum"), // REVIEW: print got?
						global_directive_node
					);
					goto break_parse_switch;
				}
				if (++(parser->token)->kind != MD_TokenKind_OpenBrace)
				{
					md_messagelist_push_ast(
						parser->arena,
						parser->messages,
						MD_MessageKind_FatalError, // REVIEW: fatal?
						str8_lit("expected '{' to open @enum"),
						global_directive_node
					);
					goto break_parse_switch;
				}
				while (++parser->token != parser->tokens_one_past_last && parser->token->kind != MD_TokenKind_CloseBrace)
				{
					MD_ASTKind kind = MD_ASTKind_NULL;
					switch (parser->token->kind)
					{
						case MD_TokenKind_StringLit: {
							// TODO: push string node as child of the enum
						} break;
						case MD_TokenKind_DirectiveExpand: {
							// TODO: push expand node as child of the enum
						} break;
						default: {
							// TODO: error invalid argument to @enum
						} break;
					}
				}
				if (parser->token == parser->tokens_one_past_last)
				{
					// TODO: error unfinished enum directive (i.e. missing '}'
					md_messagelist_push_ast(
						parser->arena,
						parser->messages,
						MD_MessageKind_FatalError,
						str8_lit("@enum directive missing closing '}'"),
						global_directive_node
					);
				}
			} break;
			default: {
				arena_pop(parser->arena, sizeof(MD_AST)); // the node we allocated won't be used
				global_directive_node = 0;
				// TODO: expected global directive
				//   not fatal error, just keep parsing for other directives
			} break;
		}
		break_parse_switch:;


		// TODO: error %S directive missing closing '}'

		if (global_directive_node != 0)
			SLLQueuePush(root->first, root->last, global_directive_node);
		if (parser->messages->worst_message >= MD_MessageKind_FatalError)
			break;
	}

	return root;
}

internal MD_AST*
md_parse_exprlist(MD_ParseState *parser)
{
	MD_AST *exprlist = push_array_no_zero(parser->arena, MD_AST, 1);
	*exprlist = (MD_AST) {
		.kind = MD_ASTKind_ExprList,
	};

	// REVIEW: mask to indicate allowed tokens?
	for (; parser->token != parser->tokens_one_past_last; parser->token++)
	{
		MD_ASTKind kind = md_token_to_ast_kind_table[parser->token->kind];
		switch (kind) {
			case MD_ASTKind_IntLit:
			case MD_ASTKind_FloatLit:
			case MD_ASTKind_StringLit:
			case MD_ASTKind_Ident: {
				MD_AST *expr = md_ast_push_child(parser->arena, exprlist, kind);
				expr->token = parser->token;
			} break;
			default: goto break_loop;
		}
	}
	break_loop:;

	return exprlist;
}

// REVIEW: can I get by without this?
internal void
md_ast_add_child(MD_AST *parent, MD_AST *child)
{
	SLLQueuePush(parent->first, parent->last, child);
	child->parent = parent;
	parent->children_count++;
}

internal MD_AST*
md_ast_push_child(Arena *arena, MD_AST *parent, MD_ASTKind kind)
{
	MD_AST *result = push_array_no_zero(arena, MD_AST, 1);
	*result = (MD_AST) {
		.kind = kind,
	};
	md_ast_add_child(parent, result);
	return result;
}

internal MD_Message*
md_messagelist_push_inner(Arena *arena, MD_MessageList *messages, MD_MessageKind kind, String8 string)
{
	MD_Message *message = push_array_no_zero(arena, MD_Message, 1);
	*message = (MD_Message) {
		.kind = kind,
		.string = string,
	};
	SLLQueuePush(messages->first, messages->last, message);
	if (kind > messages->worst_message)
		messages->worst_message = kind;
	return message;
}

internal void
md_messagelist_push_token(Arena *arena, MD_MessageList *messages, MD_MessageKind kind, String8 string, U64 tokens_ix)
{
	MD_Message *message = md_messagelist_push_inner(arena, messages, kind, string);
	message->tokens_ix = tokens_ix;
}

internal void
md_messagelist_push_ast(Arena *arena, MD_MessageList *messages, MD_MessageKind kind, String8 string, MD_AST *ast)
{
	MD_Message *message = md_messagelist_push_inner(arena, messages, kind, string);
	message->ast = ast;
}
