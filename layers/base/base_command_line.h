#ifndef BASE_COMMAND_LINE_H
#define BASE_COMMAND_LINE_H

#define CMD_LINE_LOG2_OPT_COUNT 10
#define CMD_LINE_OPT_COUNT      (1 << CMD_LINE_LOG2_OPT_COUNT)

#define CMD_LINE_LOAD_FACTOR 0.7f

typedef struct CmdLineOpt CmdLineOpt;
struct CmdLineOpt
{
	String8 string, value_string;
	String8List value_strings;
};

typedef struct CmdLine CmdLine;
struct CmdLine
{
	CmdLineOpt option_table[CMD_LINE_OPT_COUNT]; // msi hash table
	String8List passthrough_inputs;
	String8 exe_name;
	U64 argc;
	char **argv;
	U16 options[CMD_LINE_OPT_COUNT], // indices of option_table in insertion order
	    opt_count; // number of options
};
StaticAssert(CMD_LINE_LOG2_OPT_COUNT < 16, cmd_line_keep_16_ix_size);

internal U64         cmd_line_hash_from_string(String8 string);
internal CmdLineOpt* cmd_line_opt_from_string(Arena *arena, CmdLine *cmd, String8 string);
internal String8List cmd_line_strings(CmdLine *cmd_line, String8 name);
internal String8     cmd_line_string(CmdLine *cmd_line, String8 name);
internal B32         cmd_line_has_flag(CmdLine *cmd_line, String8 name);
internal B32         cmd_line_has_argument(CmdLine *cmd_line, String8 name);
internal CmdLine*    cmd_line_from_argcv(Arena *arena, int argc, char *argv[]);

#endif // BASE_COMMAND_LINE_H
