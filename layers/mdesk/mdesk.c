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
		MD_Message message      = zero_struct;

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
					message.kind   = MD_MessageKind_Error;
					message.string = str8_lit("trailing / - comments begin with // or /*");
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
							message.kind   = MD_MessageKind_Error;
							message.string = str8_lit("unclosed block comment");
							break;
						}
					}
					continue;
				}
			} break;
			case '@': {
				// REVIEW: have directive name just be an
				// identifier and check in parsing?
				const local_persist struct MD_Directive_StringToken_Map {
					String8 string;
					MD_TokenKind token_kind;
				} directive_table[] = {
					{str8_lit_comp("table"),  MD_TokenKind_DirectiveTable },
					{str8_lit_comp("enum"),   MD_TokenKind_DirectiveEnum  },
					{str8_lit_comp("expand"), MD_TokenKind_DirectiveExpand},
					{str8_lit_comp("format"), MD_TokenKind_DirectiveFormat},
					{str8_lit_comp("exists"), MD_TokenKind_DirectiveExists},
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
						c += entry.string.length;
						goto break_lex_switch; // skip unknown directive error
					}
				}
				message.kind   = MD_MessageKind_Error;
				message.string = str8_lit("unknown directive");
			} break;
			case '"': {
				// REVIEW: allowed characters?
				token_kind = MD_TokenKind_StringLit;
				while (++c < one_past_last && *c != '"')
					if (*c == '\\') // just skip escaped characters
						c++;
				if (c == one_past_last)
				{
					message.kind   = MD_MessageKind_Error;
					message.string = str8_lit("broken string literal");
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
							message.kind   = MD_MessageKind_Error;
							message.string = str8_lit("no radix on leading zero - please use 0x, 0b, or 0o");
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
						message.kind   = MD_MessageKind_Error;
						message.string = str8_lit("incomplete 0x literal");
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
					[','] = MD_TokenKind_Commma,
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
					message.kind   = MD_MessageKind_Warning;
					// TODO: include an escaped version of the character in the message
					message.string = str8_lit("unknown character - ignoring");
				}
			} break;
		}
		break_lex_switch:;

		Assert(c > tok_start);
		Rng1U64 range = r1u64((U64)(tok_start - source.buffer), (U64)(c - source.buffer));
		MD_TokenNode *node = push_array_no_zero(scratch.arena, MD_TokenNode, 1);
		*node = (MD_TokenNode) {
			.token.kind         = token_kind,
			.token.source_range = range,
		};
		SLLQueuePush(toklist.first, toklist.last, node);
		toklist.count++;
		if (message.kind != MD_MessageKind_NULL)
		{
			MD_Message *pushed_message = push_array_no_zero(arena, MD_Message, 1);
			pushed_message = push_array_no_zero(arena, MD_Message, 1);
			*pushed_message            = message;
			SLLQueuePush(messages.first, messages.last, pushed_message);
			pushed_message->tokens_ix = toklist.count - 1;;
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
md_parse_from_tokens(Arena *arena, MD_TokenArray tokens)
{
	MD_ParseResult result = zero_struct;
	return result;
}
