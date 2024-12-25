#ifndef BASE_COMMAND_LINE_H
#define BASE_COMMAND_LINE_H

typedef struct CmdLineOpt CmdLineOpt;
struct CmdLineOpt
{
	CmdLineOpt *next, *hash_next;
	U64 hash;
	String8 string, value_string;
	String8List value_strings;
};

typedef struct CmdLineOptList CmdLineOptList;
struct CmdLineOptList
{
	U64 count;
	CmdLineOpt *first, *last;
};

typedef struct CmdLine CmdLine;
struct CmdLine
{
	String8 exe_name;
	CmdLineOptList options;
	String8List inputs;
	U64 option_table_size;
	CmdLineOpt **option_table;
	U64 argc;
	char **argv;
};

function U64              cmd_line_hash_from_string(String8 string);
function CmdLineOpt**     cmd_line_slot_from_string(CmdLine *cmd_line, String8 string);
function CmdLineOpt*      cmd_line_opt_from_slot(CmdLineOpt **slot, String8 string);
function void             cmd_line_push_opt(CmdLineOptList *list, CmdLineOpt *var);
function CmdLineOpt*      cmd_line_insert_opt(Arena *arena, CmdLine *cmd_line, String8 string, String8List values);
function CmdLine          cmd_line_from_string_list(Arena *arena, String8List arguments);
function CmdLineOpt*      cmd_line_opt_from_string(CmdLine *cmd_line, String8 name);
function String8List      cmd_line_strings(CmdLine *cmd_line, String8 name);
function String8          cmd_line_string(CmdLine *cmd_line, String8 name);
function B32              cmd_line_has_flag(CmdLine *cmd_line, String8 name);
function B32              cmd_line_has_argument(CmdLine *cmd_line, String8 name);

#endif // BASE_COMMAND_LINE_H
