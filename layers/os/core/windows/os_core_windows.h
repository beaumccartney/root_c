#ifndef OS_CORE_WINDOWS_H
#define OS_CORE_WINDOWS_H

#pragma push_macro("function")
#undef function
#include <windows.h>
#pragma pop_macro("function")

internal FilePropertyFlags os_windows_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes);

#endif // OS_CORE_WINDOWS_H
