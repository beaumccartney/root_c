#ifndef OS_CORE_WINDOWS_H
#define OS_CORE_WINDOWS_H

#include <windows.h>

typedef struct OS_WINDOWS_State OS_WINDOWS_State;
struct OS_WINDOWS_State
{
	U64 queryperformancefrequency_resolution;
};

global OS_WINDOWS_State g_os_windows_state = zero_struct;

// NOTE(beau): info for a directory being traversed, basically a node in a
// stack to allow depth-first traversal
typedef struct OS_WINDOWS_FileIterSearchFolder OS_WINDOWS_FileIterSearchFolder;
struct OS_WINDOWS_FileIterSearchFolder
{
	OS_WINDOWS_FileIterSearchFolder *next;
	HANDLE search_handle;
	WIN32_FIND_DATAW find_data;
	U64 dirname_length; // length of OS_WINDOWS_FileIter.working_path for this search folder
};

typedef struct OS_WINDOWS_FileIter OS_WINDOWS_FileIter;
struct OS_WINDOWS_FileIter
{
	Arena *search_dirs_arena;
	OS_WINDOWS_FileIterSearchFolder *search_dirs;
	WCHAR working_path[MAX_PATH + 3]; // room for '\\', '*', and '\0' to search the folder
};
StaticAssert(
	sizeof(OS_WINDOWS_FileIter) <= sizeof(Member(OS_FileIter, memory)),
	os_windows_file_iter_size_check
);
StaticAssert(
	AlignOf(OS_WINDOWS_FileIter) <= AlignOf(OS_FileIter), // HACK(beau): to check the alignment against OS_FileIter->memory
	os_windows_file_iter_alignment_check
);

internal FilePropertyFlags
os_windows_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes);

internal String16
os_full_path_from_path_windows_inner(Arena *arena, String16 path16);

internal void
os_windows_file_iter_push(OS_FileIter *iter, U64 path_length);

#endif // OS_CORE_WINDOWS_H
