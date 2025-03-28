#ifndef BASE_COMMAND_LINE_H
#define BASE_COMMAND_LINE_H

#define CMD_LINE_LOG2_OPT_COUNT 10
#define CMD_LINE_OPT_COUNT      (1 << CMD_LINE_LOG2_OPT_COUNT)

#define CMD_LINE_LOAD_FACTOR 0.7f

typedef struct CmdLineOpt CmdLineOpt;
struct CmdLineOpt
{
	String8 string, value_string;
	String8Array value_strings;
};

typedef struct CmdLine CmdLine;
struct CmdLine
{
	CmdLineOpt option_table[CMD_LINE_OPT_COUNT]; // msi hash table
	String8Array passthrough_inputs,
		     args;
	String8 exe_name;
	S16 options[CMD_LINE_OPT_COUNT], // indices of option_table in insertion order
	    opt_count; // number of options
};
global CmdLine *g_cmdline = 0;

StaticAssert(CMD_LINE_LOG2_OPT_COUNT < 16, cmd_line_keep_16_ix_size);

internal U64          cmd_line_hash_from_string(String8 string);
internal CmdLineOpt*  cmd_line_opt_from_string(Arena *arena, CmdLine *cmd, String8 string);
internal String8Array cmd_line_strings(CmdLine *cmd_line, String8 name);
internal String8      cmd_line_string(CmdLine *cmd_line, String8 name);
internal B32          cmd_line_has_flag(CmdLine *cmd_line, String8 name);
internal B32          cmd_line_has_argument(CmdLine *cmd_line, String8 name);
internal CmdLine*     cmd_line_from_args(Arena *arena, String8Array args);

#endif // BASE_COMMAND_LINE_H
