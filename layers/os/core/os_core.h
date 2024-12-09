#ifndef OS_CORE_H
#define OS_CORE_H

typedef struct OS_SystemInfo OS_SystemInfo;
struct OS_SystemInfo
{
	U64 page_size, large_page_size;
};

// TODO(beau): initialize in the platform entry point and either return a pointer to that here or just define a global exposed by each header here
function OS_SystemInfo os_get_system_info(void);

function void *os_reserve(U64 size);
function B32   os_commit(void *ptr, U64 size);
function void  os_decommit(void *ptr, U64 size);
function void  os_release(void *ptr, U64 size);

function void *os_reserve_large(U64 size);
function B32   os_commit_large(void *ptr, U64 size);

#endif // OS_CORE_H
