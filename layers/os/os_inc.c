#include "core/os_core.c"
#if OS_MAC
	#include "core/mac/os_core_mac.c"
#elif OS_WINDOWS
	#include "core/windows/os_core_windows.c"
#endif // OS_MAC

#if OS_FEATURE_GRAPHICAL
	#include "gfx/generated/os_gfx.meta.c"
	#include "gfx/os_gfx.c"
	#if OS_MAC
		#include "gfx/mac/os_gfx_mac.m"
	#elif OS_WINDOWS
		#include "gfx/windows/os_gfx_windows.c"
	#else
		#error os_gfx not implemented for the current os target
	#endif
#endif
