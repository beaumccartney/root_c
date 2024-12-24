#include "core/os_core.c"
#if OS_FEATURE_GRAPHICAL
	#include "gfx/os_gfx.c"
#endif

#if OS_MAC
	#include "core/mac/os_core_mac.c"
#elif OS_WINDOWS
	#include "core/windows/os_core_windows.c"
#endif // OS_MAC
