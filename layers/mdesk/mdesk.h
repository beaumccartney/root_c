#ifndef MDESK_H
#define MDESK_H

typedef enum
{
	MD_TokenKind_NULL = 0, // also denotes illegal characters
	MD_TokenKind_Underscore,
	MD_TokenKind_Comma, // REVIEW: is this even useful?
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
	String8 source;
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
	MD_Token *tokens; // REVIEW: rename to something better
	U64 count;
};

typedef enum
{
	MD_MessageKind_NULL,
	MD_MessageKind_Warning,
	MD_MessageKind_Error,
	MD_MessageKind_FatalError, // REVIEW: remove and use goto if all parsing is done inline
	MD_MessageKind_COUNT,
} MD_MessageKind;

typedef struct MD_Message MD_Message;
struct MD_Message
{
	MD_Message *next;
	String8 string;

	// REVIEW: union of these? is there ever a case where I benefit from storing both in one message?
	U64 tokens_ix; // REVIEW: can I make this a pointer somehow?
	struct MD_AST *ast;

	MD_MessageKind kind;
};

typedef struct MD_MessageList MD_MessageList;
struct MD_MessageList
{
	MD_Message *first, *last;
	U64 count;
	MD_MessageKind worst_message;
};

typedef struct MD_TokenizeResult MD_TokenizeResult;
struct MD_TokenizeResult
{
	MD_TokenArray tokens;
	MD_MessageList messages;
};

typedef enum
{
	MD_ASTKind_NULL = 0,
	MD_ASTKind_Root,

	MD_ASTKind_IntLit,
	MD_ASTKind_FloatLit,
	MD_ASTKind_StringLit,
	MD_ASTKind_Ident,

	MD_ASTKind_Expression, // REVIEW: needed?
	MD_ASTKind_DirectiveTable,
	MD_ASTKind_DirectiveEnum,
	MD_ASTKind_DirectiveExpand,
	MD_ASTKind_DirectiveFormat,
	MD_ASTKind_DirectiveExists,
	MD_ASTKind_DirectiveData,

	// REVIEW: one list kind with flags for allowed node kinds?
	MD_ASTKind_IdentList,
	MD_ASTKind_TableRow,

	MD_ASTKind_COUNT,
} MD_ASTKind;

typedef struct MD_AST MD_AST;
struct MD_AST
{
	// REVIEW: doubly link siblings?
	MD_AST *next, // sibling
	       *first, // children
	       *last, // children
	       *parent;
	// REVIEW: children count?
	U64 children_count;
	MD_Token *token;
	MD_ASTKind kind;
};

typedef struct MD_ParseResult MD_ParseResult;
struct MD_ParseResult
{
	MD_AST *root;
	MD_MessageList messages;
};

// REVIEW:
//  should I pass any of this separately?
//  if all parsing is done in one function then remove
typedef struct MD_ParseState MD_ParseState;
struct MD_ParseState
{
	Arena *arena;
	MD_MessageList *messages;
	String8 source;
	MD_Token *tokens_first, // REVIEW: store token array and index instead?
		 *token,
		 *tokens_one_past_last;
};

internal MD_TokenizeResult
md_tokens_from_source(Arena *arena, String8 source);

internal MD_ParseResult
md_parse_from_tokens_source(Arena *arena, MD_TokenArray tokens, String8 source);

// NOTE: recursive descent parsing apis only used for generating the AST
internal MD_AST*
md_parse_root(MD_ParseState *parser);

internal MD_AST*
md_parse_exprlist(MD_ParseState *parser);

internal void
md_ast_add_child(MD_AST *parent, MD_AST *child);

internal MD_AST*
md_ast_push_child(Arena *arena, MD_AST *parent, MD_ASTKind kind);

internal MD_Message*
md_messagelist_push(Arena *arena, MD_MessageList *messages, MD_MessageKind kind, String8 string, U64 tokens_ix, MD_AST *ast);

#endif // MDESK_H
