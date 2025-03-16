#if !BUILD_SUPPLEMENTARY_UNIT
	global CmdLine *g_cmdline = 0;
#endif

internal U64
cmd_line_hash_from_string(String8 string)
{
	U64 result = chibihash64(string.buffer, (ptrdiff_t)string.length, 0);
	return result;
}
internal CmdLineOpt*
cmd_line_opt_from_string(Arena *arena, CmdLine *cmd_line, String8 string)
{
	U64 hash = cmd_line_hash_from_string(string);
	CmdLineOpt *result = 0;
	for (S16 table_idx = (S16)hash;;)
	{
		table_idx = msi_lookup16(hash, CMD_LINE_LOG2_OPT_COUNT, table_idx);
		CmdLineOpt *opt = &cmd_line->option_table[table_idx];
		if (opt->string.length == 0) // empty slot found
		{
			// look up failed and out of memory for insert
			AssertAlways(cmd_line->opt_count < CMD_LINE_OPT_COUNT * CMD_LINE_LOAD_FACTOR);

			if (arena != 0)
			{
				result = opt;
				result->string = push_str8_copy(arena, string);
				cmd_line->options[cmd_line->opt_count] = table_idx;
				cmd_line->opt_count++;
			}
			break;
		}
		else if (str8_match(string, opt->string, 0))
		{
			result = opt;
			break;
		}
	}
	return result;
}
internal String8Array
cmd_line_strings(CmdLine *cmd_line, String8 name)
{
	CmdLineOpt *opt = cmd_line_opt_from_string(0, cmd_line, name);
	String8Array result = opt ? opt->value_strings : (String8Array)zero_struct;
	return result;
}
internal String8
cmd_line_string(CmdLine *cmd_line, String8 name)
{
	CmdLineOpt *opt = cmd_line_opt_from_string(0, cmd_line, name);
	String8 result = zero_struct;
	if (opt != 0) result = opt->value_string;
	return result;
}
internal B32
cmd_line_has_flag(CmdLine *cmd_line, String8 name)
{
	CmdLineOpt *opt = cmd_line_opt_from_string(0, cmd_line, name);
	B32 result = opt != 0;
	return result;
}
internal B32
cmd_line_has_argument(CmdLine *cmd_line, String8 name)
{
	CmdLineOpt *opt = cmd_line_opt_from_string(0, cmd_line, name);
	B32 result = opt != 0 && opt->value_strings.count != 0;
	return result;
}
internal CmdLine* cmd_line_from_args(Arena *arena, String8Array args)
{
	CmdLine *result = push_array(arena, CmdLine, 1);
	result->args    = args;
	Temp scratch    = scratch_begin(&arena, 1);

	Assert(args.count >= 1); // one for the exe name
	String8 *arg           = args.v,
		*one_past_last = args.v + args.count;
	result->exe_name = *arg++;

	// parse options and arguments - next loop will parse passthrough inputs
	for (; arg < one_past_last; arg++)
	{
		String8 option_name = zero_struct;
		if (str8_match(str8_prefix(*arg, 2), str8_lit("--"), 0) &&
			arg->length > 2)
		{
			option_name = str8_skip(*arg, 2);
		}
		else if (str8_match(str8_prefix(*arg, 1), str8_lit("-"), 0) &&
			arg->length > 2)
		{
			option_name = str8_skip(*arg, 1);
		}
		else if (g_os_from_context == OperatingSystem_Windows &&
			str8_match(str8_prefix(*arg, 1), str8_lit("/"), 0) &&
			arg->length > 1)
		{
			option_name = str8_skip(*arg, 1);
		}
		// when not an option, its a passthrough option. break to get to the passthrough loop
		else
		{
			// -- is passthrough, not an actual flag. don't push it to command line structure
			if (str8_match(*arg, str8_lit("--"), 0)) arg++;
			break;
		}

		S64 arg_signifier_position = Min(
			str8_find_needle(option_name, 0, str8_lit(":"), 0),
			str8_find_needle(option_name, 0, str8_lit("="), 0)
		);
		B32 has_arguments = arg_signifier_position < option_name.length;

		String8List option_args = zero_struct;
		if (has_arguments)
		{
			String8 option_args_portion = str8_skip(option_name, arg_signifier_position+1);
			option_name = str8_prefix(option_name, arg_signifier_position);

			U8 splits[] = { ',' };
			option_args = str8_split(scratch.arena, option_args_portion, splits, ArrayCount(splits), 0);
		}

		// insert option
		// REVIEW(beau): what if the option is already populated?
		CmdLineOpt *opt = cmd_line_opt_from_string(arena, result, option_name);
		opt->value_strings = str8_array_from_list(arena, option_args);
		opt->value_string = str8_list_join(arena, option_args, &(StringJoin){.sep = str8_lit(",")});
	}
	// passthrough option loop, starts at where option loop left off
	result->passthrough_inputs.count = one_past_last - arg;
	String8 *pass = result->passthrough_inputs.v = push_array_no_zero(arena, String8, result->passthrough_inputs.count);
	for (; arg < one_past_last; arg++, pass++)
		*pass = *arg;
	Assert(pass == result->passthrough_inputs.v + result->passthrough_inputs.count);

	scratch_end(scratch);
	return result;
}
