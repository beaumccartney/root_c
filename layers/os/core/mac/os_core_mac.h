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
#include <sys/stat.h>
#include <copyfile.h>

internal FileProperties os_mac_file_properties_from_stat(struct stat *s);

#endif // OS_CORE_MAC_H
