#ifndef OS_CORE_H
#define OS_CORE_H

typedef struct OS_SystemInfo OS_SystemInfo;
struct OS_SystemInfo
{
	U64 page_size, large_page_size;
};

typedef struct OS_Handle OS_Handle;
struct OS_Handle
{
	U64 u[1];
};

// TODO(beau): initialize in the platform entry point and either return a pointer to that here or just define a global exposed by each header here
internal OS_SystemInfo os_get_system_info(void);

internal void *os_reserve(U64 size);
internal B32   os_commit(void *ptr, U64 size);
internal void  os_decommit(void *ptr, U64 size);
internal void  os_release(void *ptr, U64 size);

internal void *os_reserve_large(U64 size);
internal B32   os_commit_large(void *ptr, U64 size);

internal B32 os_handle_match(OS_Handle a, OS_Handle b);

internal void os_set_thread_name(String8 name);

#endif // OS_CORE_H
