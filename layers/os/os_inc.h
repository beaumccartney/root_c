#ifndef OS_INC_H
#define OS_INC_H

#ifndef OS_FEATURE_GRAPHICAL
	#define OS_FEATURE_GRAPHICAL 0
#endif

#include "core/os_core.h"
#if OS_MAC
	#include "core/mac/os_core_mac.h"
#elif OS_WINDOWS
	#include "core/windows/os_core_windows.h"
#endif

#if OS_FEATURE_GRAPHICAL
	#include "gfx/generated/os_gfx.meta.h"
	#include "gfx/os_gfx.h"
	#if OS_MAC
		#include "gfx/mac/os_gfx_mac.h"
	#elif OS_WINDOWS
		#include "gfx/windows/os_gfx_windows.h"
	#else
		#error os_gfx not implemented for the current os target
	#endif
#endif

#endif // OS_INC_H
