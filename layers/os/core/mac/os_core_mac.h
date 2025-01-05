#ifndef OS_CORE_MAC_H
#define OS_CORE_MAC_H

#pragma push_macro("internal")
#undef internal
#include <mach/mach.h>
#pragma pop_macro("internal")

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syslimits.h>

#endif // OS_CORE_MAC_H
