#ifndef BASE_CONTEXT_CRACKING_H
#define BASE_CONTEXT_CRACKING_H

#if defined(__clang__)
	#define COMPILER_CLANG 1

	#if defined(__APPLE__) && defined (__MACH__)
		#define OS_MAC 1
	#elif defined (_WIN32)
		#define OS_WINDOWS 1
	#else
		#error Unsupported OS
	#endif

	#if defined(__aarch64__)
		#define ARCH_ARM64 1
	#elif defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
		#define ARCH_X64 1
	#else
		#error Unsupported architecture compiler combo
	#endif

#elif defined(_MSC_VER) // defined(__clang__)
	#define COMPILER_MSVC 1

	#if _MSC_VER >= 1920
		#define COMPILER_MSVC_YEAR 2019
	#elif _MSC_VER >= 1910
		#define COMPILER_MSVC_YEAR 2017
	#elif _MSC_VER >= 1900
		#define COMPILER_MSVC_YEAR 2015
	#elif _MSC_VER >= 1800
		#define COMPILER_MSVC_YEAR 2013
	#elif _MSC_VER >= 1700
		#define COMPILER_MSVC_YEAR 2012
	#elif _MSC_VER >= 1600
		#define COMPILER_MSVC_YEAR 2010
	#elif _MSC_VER >= 1500
		#define COMPILER_MSVC_YEAR 2008
	#elif _MSC_VER >= 1400
		#define COMPILER_MSVC_YEAR 2005
	#else
		#define COMPILER_MSVC_YEAR 0
	#endif

        #if defined(_WIN32)
		    #define OS_WINDOWS 1
	#else
		#error MSVC on not windows?!?!
	#endif

	#if defined (_M_AMD64)
		#define ARCH_X64 1
	#else
		#error MSVC supported only on windows x64
	#endif
#else // defined(__clang__)
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
	#define BUILD_ENTRY_DEFINING_UNIT !BUILD_SUPPLEMENTARY_UNIT
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
