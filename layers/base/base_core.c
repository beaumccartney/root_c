function OperatingSystem operating_system_from_context(void)
{
	OperatingSystem os = OperatingSystem_Null;
	#if OS_WINDOWS
		os = OperatingSystem_Windows;
	#elif OS_MAC
		os = OperatingSystem_Mac;
	#elif OS_LINUX
		os = OperatingSystem_Linux;
	#endif
	return os;
}
function Arch arch_from_context(void)
{
	Arch arch = Arch_Null;
	#if ARCH_X64
		arch = Arch_x64;
	#elif ARCH_ARM64
		arch = Arch_arm64;
	#endif
	return arch;
}
function Compiler compiler_from_context(void)
{
	Compiler compiler = Compiler_Null;
	#if COMPILER_MSVC
		compiler = Compiler_msvc;
	#elif COMPILER_CLANG
		compiler = Compiler_clang;
	#elif COMPILER_GCC
		compiler = Compiler_gcc;
	#endif
	return compiler;
}
