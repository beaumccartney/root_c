#ifndef OS_CORE_WINDOWS_H
#define OS_CORE_WINDOWS_H

#include <windows.h>

typedef struct OS_WINDOWS_FileIter OS_WINDOWS_FileIter;
struct OS_WINDOWS_FileIter
{
	HANDLE search_handle;
	WIN32_FIND_DATAW data;
};

internal FilePropertyFlags os_windows_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes);

#endif // OS_CORE_WINDOWS_H
