#ifndef MDESK_H
#define MDESK_H

typedef enum
{
	MD_TokenKind_NULL = 0, // also denotes illegal characters
	MD_TokenKind_Underscore,
	MD_TokenKind_Commma,
	MD_TokenKind_Period,
	MD_TokenKind_OpenParen,
	MD_TokenKind_CloseParen,
	MD_TokenKind_OpenBrace,
	MD_TokenKind_CloseBrace,
	MD_TokenKind_IntLit,
	MD_TokenKind_FloatLit,
	MD_TokenKind_StringLit,
	MD_TokenKind_DirectiveTable,
	MD_TokenKind_DirectiveEnum,
	MD_TokenKind_DirectiveExpand,
	MD_TokenKind_DirectiveFormat,
	MD_TokenKind_DirectiveExists,
	MD_TokenKind_DirectiveData,
	MD_TokenKind_Ident,
	MD_TokenKind_COUNT,
} MD_TokenKind;

// REVIEW: broken flag for things that weren't terminated correctly?
// will broken and the kind be enough to know what went wrong?
typedef struct MD_Token MD_Token;
struct MD_Token
{
	MD_TokenKind kind;
	Rng1U64 source_range;
};

typedef struct MD_TokenNode MD_TokenNode;
struct MD_TokenNode
{
	MD_TokenNode *next;
	MD_Token token;
};

typedef struct MD_TokenList MD_TokenList;
struct MD_TokenList
{
	MD_TokenNode *first, *last;
	U64 count;
};

typedef struct MD_TokenArray MD_TokenArray;
struct MD_TokenArray
{
	MD_Token *tokens;
	U64 count;
};

typedef enum
{
	MD_MessageKind_NULL,
	MD_MessageKind_Warning,
	MD_MessageKind_Error,
	MD_MessageKind_COUNT,
} MD_MessageKind;

typedef struct MD_Message MD_Message;
struct MD_Message
{
	MD_Message *next;
	String8 string;
	union // infer which from if the message is in a parse or tokenize result
	{
		U64 tokens_ix;
		struct MD_AST *ast;
	};
	MD_MessageKind kind;
};

typedef struct MD_MessageList MD_MessageList;
struct MD_MessageList
{
	MD_Message *first, *last;
	U64 count;
};

typedef struct MD_TokenizeResult MD_TokenizeResult;
struct MD_TokenizeResult
{
	MD_TokenArray tokens;
	MD_MessageList messages;
};

typedef enum
{
	MD_ASTKind_COUNT,
} MD_ASTKind;

typedef struct MD_AST MD_AST;
struct MD_AST
{
	MD_AST **children,
	       *parent;
	U64 children_count;
	MD_ASTKind kind;
};

typedef struct MD_ParseResult MD_ParseResult;
struct MD_ParseResult
{
	MD_AST *root;
	MD_Message *messages;
};

internal MD_TokenizeResult
md_tokens_from_source(Arena *arena, String8 source);

internal MD_ParseResult
md_parse_from_tokens(Arena *arena, MD_TokenArray tokens);

#endif // MDESK_H
