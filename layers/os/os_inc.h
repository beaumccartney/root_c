#ifndef OS_INC_H
#define OS_INC_H

#ifndef OS_FEATURE_GRAPHICAL
	#define OS_FEATURE_GRAPHICAL 0
#endif

#include "core/os_core.h"
#if OS_MAC
	#include "core/mac/os_core_mac.h"
#endif

#if OS_FEATURE_GRAPHICAL
	#include "gfx/os_gfx.h"
#endif

#endif // OS_INC_H
