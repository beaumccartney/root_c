// REVIEW:
//  nil ast node?
//  nil symbol table node?

#ifndef MDESK_H
#define MDESK_H

typedef enum
{
	MD_TokenKind_NULL = 0, // also denotes illegal characters
	MD_TokenKind_Underscore,
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

	union
	{
		U64 tokens_ix;   // md_tokens_from_source() populates this
		MD_Token *token; // everything else uses this
	};
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
	       *parent; // REVIEW: needed?
	U64 children_count;
	MD_Token *token;
	MD_ASTKind kind;
};

typedef struct MD_SymbolTableEntry MD_SymbolTableEntry;
struct MD_SymbolTableEntry
{
	String8 key;
	MD_AST *ast;
	MD_SymbolTableEntry *slots[4];
	union
	{
		struct
		{
			MD_SymbolTableEntry *cols_stab; // symbol table for @table's columns
			U64 num_cols, num_rows;
			String8 *elem_matrix; // array of strings pointing into the source of each of the tables elements, of length num_columns * num_rows i.e. one string per table element
		} table_record;

		struct
		{
			U64 col; // this ident names the nth column of the table
		} col_record;
	};
};

typedef struct MD_ParseResult MD_ParseResult;
struct MD_ParseResult
{
	MD_AST *root;
	MD_MessageList messages;
	MD_SymbolTableEntry *global_stab;
};

// REVIEW:
//  should I pass any of this separately?
//  remove if parsing is done in one function
typedef struct MD_ParseState MD_ParseState;
struct MD_ParseState
{
	Arena *arena;
	MD_MessageList *messages;
	String8 source;
	MD_Token *tokens_first, // REVIEW: store token array and index instead?
		 *token,
		 *tokens_one_past_last;
	MD_SymbolTableEntry **global_stab;
};

internal MD_TokenizeResult
md_tokens_from_source(Arena *arena, String8 source);

internal MD_ParseResult
md_parse_from_tokens_source(Arena *arena, MD_TokenArray tokens, String8 source);

// NOTE: recursive descent parsing apis only used for generating the AST
internal MD_AST*
md_parse_root(MD_ParseState *parser);

// TODO: change param name stab to stab_root

internal MD_MessageList
md_check_parsed(Arena *arena, MD_AST *root, MD_SymbolTableEntry *stab, String8 source);

internal MD_AST*
md_ast_push_child(Arena *arena, MD_AST *parent, MD_ASTKind kind);

internal inline MD_Message*
md_messagelist_push_inner(Arena *arena, MD_MessageList *messages, String8 source, U8* source_loc, MD_MessageKind kind, String8 string);

// TODO:
//  return void
//  take U8 * param to location in source and add line and column to message
internal void
md_messagelist_push(Arena *arena, MD_MessageList *messages, String8 source, U8 *source_loc, MD_MessageKind kind, String8 string, MD_Token *token, MD_AST *ast);

internal U64 md_hash_ident(String8 ident);

internal MD_SymbolTableEntry*
md_symbol_from_ident(Arena *arena, MD_SymbolTableEntry** stab, String8 ident);

#endif // MDESK_H
