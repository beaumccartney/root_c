internal MD_TokenizeResult
md_tokens_from_string(Arena *arena, String8 source)
{
	MD_TokenList toklist = zero_struct;
	Temp scratch = scratch_begin(&arena, 1);
	U8 *one_past_last = source.buffer + source.length;
	for (U8 *c = source.buffer; c < one_past_last;)
	{
		MD_TokenKind kind = MD_TokenKind_NULL;
		U8 *tok_start = c;
		switch (*c)
		{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
			case '\f':
			case '\v':
				goto spin;
			case '_':
			case ',':
			case '.':
			case '(':
			case ')':
			case '{':
			case '}': {
				const local_persist
				MD_TokenKind single_char_tok_table[UINT8_MAX] = {
					['_'] = MD_TokenKind_Underscore,
					[','] = MD_TokenKind_Commma,
					['.'] = MD_TokenKind_Period,
					['('] = MD_TokenKind_OpenParen,
					[')'] = MD_TokenKind_CloseParen,
					['{'] = MD_TokenKind_OpenBrace,
					['}'] = MD_TokenKind_CloseBrace,
				};
				kind = single_char_tok_table[*c];
				Assert(kind != 0);
				c++;
			} break;
			case '/': {
				if (c >= one_past_last - 1) // trailing '/'
				{
					// TODO: trailing '/' error
					NotImplemented
				}
				if (*++c == '/' || *c == '*') // comment
				{
					// REVIEW: allowed characters?
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
							// unclosed multi line comment
							NotImplemented
							break;
						}
					}
					continue;
				}
				else
				{
					// TODO: either division or trailing slash operator
					NotImplemented
				}
			} break;
			case '@': {
				const local_persist struct MD_Directive_StringToken_Map {
					String8 string;
					MD_TokenKind kind;
				} directive_table[] = {
					{str8_lit_comp("table"),  MD_TokenKind_DirectiveTable },
					{str8_lit_comp("enum"),   MD_TokenKind_DirectiveEnum  },
					{str8_lit_comp("expand"), MD_TokenKind_DirectiveExpand},
					{str8_lit_comp("format"), MD_TokenKind_DirectiveFormat},
					{str8_lit_comp("exists"), MD_TokenKind_DirectiveExists},
					{str8_lit_comp("data"),   MD_TokenKind_DirectiveData  },
				};
				String8 remaining_source = str8_region(++c, one_past_last);
				for EachElement(i, directive_table)
				{
					struct MD_Directive_StringToken_Map entry = directive_table[i];
					if (str8_match(entry.string, remaining_source, StringMatchFlag_RightSideSloppy))
					{
						kind = entry.kind;
						c += entry.string.length;
						goto directive_success;
					}
				}
				// TODO: unknown directive error
				NotImplemented
				directive_success:; // skips unknown directive error
			} break;
			case '"': {
				// REVIEW: allowed characters?
				kind = MD_TokenKind_StringLit;
				while (++c < one_past_last && *c != '"')
					if (*c == '\\') // just skip escaped characters
						c++;
				if (c == one_past_last)
				{
					// TODO: broken string literal error
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
				kind = MD_TokenKind_Ident;
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
				kind = MD_TokenKind_IntLit;
				U32 radix = 10;
				if (*c++ == '0' && c != one_past_last)
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
							// TODO: no radix on leading zero error
							NotImplemented
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
						// TODO: incomplete 0x literal
						NotImplemented
					}
				}
				while (c < one_past_last && char_is_digit(*c, radix))
					c++;
				integral_zero:;
				if (c < one_past_last && *c == '.')
				{
					if (radix != 10)
					{
						// TODO: error - only decimal floating point literals are supported
						NotImplemented
					}
					kind = MD_TokenKind_FloatLit;
					// REVIEW: error for digits in a too-large radix?
					do c++; while (c < one_past_last && char_is_digit(*c, radix));
				}
			} break;
			default: {
				// REVIEW: allowed characters?
				//  what abt unused punctuation? perhaps split into weird stuff and normal stuff
				// TODO: unknown character warning
				NotImplemented
				spin:; // jump here for characters that are allowed and ignored in the syntax
				c++;
				continue;
			} break;
		}

		Assert(c > tok_start);
		MD_TokenNode *node = push_array_no_zero(scratch.arena, MD_TokenNode, 1);
		*node = (MD_TokenNode) {
			.token.kind = kind,
			.token.source_range = r1u64(
				tok_start - source.buffer,
				c - source.buffer
			),
		};
		SLLQueuePush(toklist.first, toklist.last, node);
		toklist.count++;
	}

	MD_TokenizeResult result = {
		.tokens.tokens = push_array_no_zero(arena, MD_Token, toklist.count),
		.tokens.count = toklist.count,
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
