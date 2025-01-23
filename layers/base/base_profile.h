#ifndef BASE_PROFILE_H
#define BASE_PROFILE_H

#ifndef PROFILE_SUPERLUMINAL
	#define PROFILE_SUPERLUMINAL 0
#endif

#ifndef PROFILE_TRACY
	#define PROFILE_TRACY 0
#endif

#if PROFILE_SUPERLUMINAL
	#if !OS_WINDOWS
		#error Superluminal profiling only supported on windows
	#endif
	#include "Superluminal/PerformanceAPI_capi.h"
	#pragma comment( "PerformanceAPI_MT.lib")

	#define ProfThreadName(name_cstr) PerformanceAPI_SetCurrentThreadName((name_cstr))
#elif PROFILE_TRACY
	#if OS_WINDOWS
	#pragma comment(lib, "..\\thirdparty\\tracy\\windows\\TracyClient.lib")
	#elif OS_MAC
	#pragma comment(lib, "c++")
	#endif

	#define TRACY_ENABLE

	#define ProfThreadName(name_cstr) TracyCSetThreadName((name_cstr))
#endif // PROFILE_SUPERLUMINAL

// without TRACY_ENABLE tracy apis don't do anything
#include "tracy/TracyC.h"

#ifndef ProfThreadName
	#define ProfThreadName(name_cstr) (0)
#endif

#endif // BASE_PROFILE_H
