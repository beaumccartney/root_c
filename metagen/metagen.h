#ifndef METAGEN_H
#define METAGEN_H

typedef struct MG_GenResult MG_GenResult;
struct MG_GenResult
{
	String8 h_file,
		c_file;
	MD_MessageList messages;
};

internal MG_GenResult
mg_generate_from_checked(Arena *arena, MD_AST *root, MD_SymbolTableEntry *stab_root, String8 source);

#endif // METAGEN_H
