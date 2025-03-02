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
	MD_TokenKind_RawStringLit,

	MD_TokenKind_DirectiveHFile,
	MD_TokenKind_DirectiveCFile,

	MD_TokenKind_DirectiveTop,
	MD_TokenKind_DirectiveEnums, // REVIEW: do I need enums and structs in header and c files?
	MD_TokenKind_DirectiveStructs,
	MD_TokenKind_DirectiveFunctions,
	MD_TokenKind_DirectiveArrays,
	MD_TokenKind_DirectiveEmbeddedStrings,
	MD_TokenKind_DirectiveEmbeddedFiles,
	MD_TokenKind_DirectiveBottom,

	MD_TokenKind_DirectiveTable,

	MD_TokenKind_DirectiveGen,
	MD_TokenKind_DirectiveEnum,
	MD_TokenKind_DirectiveStruct,
	MD_TokenKind_DirectiveArray,

	MD_TokenKind_DirectiveExpand,

	MD_TokenKind_DirectiveEmbedString,
	MD_TokenKind_DirectiveEmbedFile,

	MD_TokenKind_Ident,
	MD_TokenKind_COUNT,
} MD_TokenKind;

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
	MD_Token *v;
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
	MD_ASTKind_RawStringLit,
	MD_ASTKind_Ident,

	MD_ASTKind_DirectiveTable,

	MD_ASTKind_DirectiveGen,
	MD_ASTKind_DirectiveEnum,
	MD_ASTKind_DirectiveStruct,
	MD_ASTKind_DirectiveArray,

	MD_ASTKind_DirectiveExpand,

	MD_ASTKind_DirectiveEmbedString,
	MD_ASTKind_DirectiveEmbedFile,

	MD_ASTKind_List, // container for children, with the children count

	MD_ASTKind_COUNT,
} MD_ASTKind;

// REVIEW: in c layer or something else? this should be common
typedef enum
{
	MD_GenFile_NULL, // REVIEW: just set to COUNT by default?
	MD_GenFile_H,
	MD_GenFile_C,
	MD_GenFile_COUNT,
} MD_GenFile;

typedef enum
{
	MD_GenLocation_NULL, // REVIEW: just set to COUNT by default?
	MD_GenLocation_Top,
	MD_GenLocation_Enums,
	MD_GenLocation_Structs,
	MD_GenLocation_Functions,
	MD_GenLocation_Arrays,
	MD_GenLocation_EmbeddedStrings,
	MD_GenLocation_EmbeddedFiles,
	MD_GenLocation_Default,
	MD_GenLocation_Bottom,
	MD_GenLocation_COUNT,
} MD_GenLocation;

typedef struct MD_AST MD_AST;
struct MD_AST
{
	// REVIEW: doubly link siblings?
	MD_AST *next, // sibling
	       *first, // children
	       *last, // children
	       *parent; // to traverse the tree iteratively (i.e. without procedure recursion)
	U64 children_count;
	MD_Token *token; // REVIEW: just the token string instead? or the string containing the entire tree?
	MD_ASTKind kind;
	MD_GenFile gen_file;
	MD_GenLocation gen_loc;
};

typedef struct MD_SymbolTableEntry MD_SymbolTableEntry;
struct MD_SymbolTableEntry
{
	String8 ident;
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

		// NOTE:
		//  @enum         - points to  the type specifier
		//  @embed_file   - points to the filename string
		//  @embed_string - points to the embedded string
		//  @array        - holds the type specifier and an optional length specifier
		struct
		{
			MD_Token *token1, *token2;
		} named_gen_record;
	};
};

typedef struct MD_ParseResult MD_ParseResult;
struct MD_ParseResult
{
	MD_AST *root;
	MD_MessageList messages;
	MD_SymbolTableEntry *global_stab;
};

internal MD_TokenizeResult
md_tokens_from_source(Arena *arena, String8 source);

internal MD_ParseResult
md_parse_from_tokens(Arena *arena, MD_TokenArray tokens, String8 source);

// TODO: change param name stab to stab_root

// TODO: rename to something more clear, and possibly factor operations that don't belong here to parsing
internal MD_MessageList
md_check_parsed(Arena *arena, MD_AST *root, MD_SymbolTableEntry *stab_root, String8 source);

internal MD_AST*
md_ast_push_child(Arena *arena, MD_AST *parent, MD_ASTKind kind, MD_Token *token);

internal inline MD_Message*
md_messagelist_push_inner(Arena *arena, MD_MessageList *messages, String8 source, U8* source_loc, MD_MessageKind kind, String8 string);

internal void
md_messagelist_push(Arena *arena, MD_MessageList *messages, String8 source, U8 *source_loc, MD_Token *token, MD_AST *ast, MD_MessageKind kind, String8 string);

internal void
md_messagelist_pushf(Arena *arena, MD_MessageList *messages, String8 source, U8 *source_loc, MD_Token *token, MD_AST *ast, MD_MessageKind kind, char *fmt, ...);

internal U64 md_hash_ident(String8 ident);

// REVIEW: there will be other string hash tables, if they're all backed by the same scheme then factor this into a macro
internal MD_SymbolTableEntry*
md_symbol_from_ident(Arena *arena, MD_SymbolTableEntry** stab_root, String8 ident);

#endif // MDESK_H
