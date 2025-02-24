internal inline void
mg_print_message_push(Arena *arena, String8List *print_messages, String8 message_string, String8 filename)
{
	String8Node *pushed = str8_list_pushf(
		arena,
		print_messages,
		"%S%S",
		filename,
		message_string
	);
	Unused(pushed);
}

internal MG_GenResult
mg_generate_from_checked(Arena *arena, MD_AST *root, MD_SymbolTableEntry *stab_root)
{
	MG_GenResult result = zero_struct;

	String8List h_file_builder = zero_struct,
	            c_file_builder = zero_struct;

	Temp scratch = scratch_begin(&arena, 1);

	for (MD_AST *global_directive = root->first; global_directive != 0; global_directive = global_directive->next)
	{
		if (global_directive->kind == MD_ASTKind_DirectiveData || global_directive->kind == MD_ASTKind_DirectiveEnum)
		{
		}
	}

	StringJoin join = {
		.sep = str8_lit("\n"),
	};
	result.h_file = str8_list_join(arena, h_file_builder, &join);
	result.c_file = str8_list_join(arena, c_file_builder, &join);

	scratch_end(scratch);

	return result;
}
