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

const global OperatingSystem g_os_from_context =
#if OS_WINDOWS
	OperatingSystem_Windows
#elif OS_MAC
	OperatingSystem_Mac
#elif OS_LINUX
	OperatingSystem_Linux
#endif
;

const global Arch g_arch_from_context =
#if ARCH_X64
	Arch_x64
#elif ARCH_ARM64
	Arch_arm64
#endif
;

const global Compiler g_compiler_from_context =
#if COMPILER_MSVC
	Compiler_msvc
#elif COMPILER_CLANG
	Compiler_clang
#elif COMPILER_GCC
	Compiler_gcc
#endif
;

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
