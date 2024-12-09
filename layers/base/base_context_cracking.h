#ifndef BASE_CONTEXT_CRACKING_H
#define BASE_CONTEXT_CRACKING_H

#if defined(__clang__)
	#define COMPILER_CLANG 1

	#if defined(__APPLE__) && defined (__MACH__)
		#define OS_MAC 1
	#else
		#error Unsupported OS
	#endif

	#if defined(__aarch64__)
		#define ARCH_ARM64 1
	#else
		#error Unsupported architecture
	#endif

#else
	#error Compiler not supported
#endif // defined(__clang__)

#if defined(__cplusplus)
	#define LANG_CPP 1
#else
	#define LANG_C 1
#endif

#if !defined(BUILD_DEBUG)
	#define BUILD_DEBUG 1
#endif

#if !defined(BUILD_SUPPLEMENTARY_UNIT)
	#define BUILD_SUPPLEMENTARY_UNIT 0
#endif


#if !defined(BUILD_ENTRY_DEFINING_UNIT)
	#define BUILD_ENTRY_DEFINING_UNIT 1
#endif

#if !defined(ARCH_X64)
	#define ARCH_X64 0
#endif

#if !defined(ARCH_ARM64)
	#define ARCH_ARM64 0
#endif

#if !defined(COMPILER_MSVC)
	#define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_GCC)
	#define COMPILER_GCC 0
#endif

#if !defined(COMPILER_CLANG)
	#define COMPILER_CLANG 0
#endif

#if !defined(OS_WINDOWS)
	#define OS_WINDOWS 0
#endif

#if !defined(OS_MAC)
	#define OS_MAC 0
#endif

#if !defined(OS_LINUX)
	#define OS_LINUX 0
#endif

#endif // BASE_CONTEXT_CRACKING
