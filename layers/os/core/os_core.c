function B32 os_handle_match(OS_Handle a, OS_Handle b)
{
	B32 result = a.u[0] == b.u[0];
	return result;
}
