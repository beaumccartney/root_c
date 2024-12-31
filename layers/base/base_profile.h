#ifndef BASE_PROFILE_H
#define BASE_PROFILE_H

#ifndef PROFILE_SUPERLUMINAL
	#define PROFILE_SUPERLUMINAL 0
#endif

#ifndef PROFILE_TRACY
	#define PROFILE_TRACY 0
#endif

#ifndef PROFILE_SPALL
	#define PROFILE_SPALL 0
#endif

#if PROFILE_SUPERLUMINAL
	#if !OS_WINDOWS
		#error Superluminal profiling only supported on windows
	#endif
	#include "Superluminal/PerformanceAPI_capi.h"
	#pragma comment(lib, "PerformanceAPI_MT.lib")

	#define ProfBegin(name_cstr_lit)  PerformanceAPI_BeginEvent((name_cstr_lit), 0, PERFORMANCEAPI_DEFAULT_COLOR)
	#define ProfEnd()                 PerformanceAPI_EndEvent()
	#define ProfThreadName(name_cstr) PerformanceAPI_SetCurrentThreadName((name_cstr))
#elif PROFILE_TRACY
	#if OS_WINDOWS
	#pragma comment(lib, "..\\thirdparty\\tracy\\windows\\TracyClient.lib")
	#endif
	#define TRACY_ENABLE

	#pragma push_macro("function")
	#undef function
	#include "tracy/TracyC.h" // REVIEW(beau): include always and rely on TRACY_ENABLE?
	#pragma pop_macro("function")

#elif PROFILE_SPALL
	#if COMPILER_MSVC
		#pragma warning (push, 0)
        #elif COMPILER_CLANG
		#pragma clang diagnostic ignored "-Weverything"
	#endif
	#include "spall/spall.h"
	#if COMPILER_MSVC
		#pragma warning (pop)
        #elif COMPILER_CLANG
		#pragma clang diagnostic pop
	#endif
#endif // PROFILE_SUPERLUMINAL

#ifndef ProfBegin
	#define ProfBegin(name_cstr_lit)  (0)
	#define ProfEnd()                 (0)
	#define ProfThreadName(name_cstr) (0)
#endif

#endif // BASE_PROFILE_H
