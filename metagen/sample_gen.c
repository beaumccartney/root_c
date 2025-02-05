enum OS_Key
{
	OS_Key_Null = 0,
	OS_Key_Z = 90,
	OS_Key_LBracket,
	OS_Key_LeftBracket = OS_Key_LBracket,
	OS_Key_Backslash,
	OS_Key_RBracket,
	OS_Key_RightBracket = OS_Key_RBracket,
	OS_Key_COUNT,
};

const static String8 os_key_strings[OS_Key_COUNT]
{
	[OS_Key_Null] = "null",
	[OS_Key_Z] = "Z",
	[OS_Key_LBracket] = "LBracket",
	[OS_Key_Backslash] = "Backslash",
	[OS_Key_RBracket] = "RBracket",
};
