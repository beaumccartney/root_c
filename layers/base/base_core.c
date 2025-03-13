inline internal void *MemoryCopy(void *dst, const void* src, S64 length)
{
	Assert(0 <= length);
	return memmove(dst, src, (size_t)length);
}
inline internal void *MemorySet(void *dst, int byte, S64 length)
{
	Assert(0 <= length);
	return memset(dst, byte, (size_t)length);
}
inline internal int MemoryCompare(const void *a, const void *b, S64 length)
{
	Assert(0 <= length);
	return memcmp(a, b, (size_t)length);
}

internal OperatingSystem operating_system_from_context(void)
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
internal Arch arch_from_context(void)
{
	Arch arch = Arch_Null;
	#if ARCH_X64
		arch = Arch_x64;
	#elif ARCH_ARM64
		arch = Arch_arm64;
	#endif
	return arch;
}
internal Compiler compiler_from_context(void)
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
internal U8
safe_cast_u8(U64 x)
{
	AssertAlways(x <= max_U8);
	U8 result = (U8)x;
	return result;
}

internal U16
safe_cast_u16(U64 x)
{
	AssertAlways(x <= max_U16);
	U16 result = (U16)x;
	return result;
}

internal U32
safe_cast_u32(U64 x)
{
	AssertAlways(x <= max_U32);
	U32 result = (U32)x;
	return result;
}

internal S8 safe_cast_s8(S64 x)
{
	AssertAlways(min_S8 <= x && x <= max_S8);
	S8 result = (S8)x;
	return result;
}
internal S16 safe_cast_s16(S64 x)
{
	AssertAlways(min_S16 <= x && x <= max_S16);
	S16 result = (S16)x;
	return result;
}

internal S32
safe_cast_s32(S64 x)
{
	AssertAlways(min_S32 <= x && x <= max_S32);
	S32 result = (S32)x;
	return result;
}

internal U32
u32_from_u64_saturate(U64 x)
{
	U32 result = (U32)ClampTop(x, max_U32);
	return result;
}
