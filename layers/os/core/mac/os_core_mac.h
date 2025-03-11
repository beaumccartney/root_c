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
#include <pthread.h>
#include <fts.h>
#include <sys/types.h>

#include <time.h>

typedef enum
{
	OS_MAC_EntityKind_Thread,
	OS_MAC_EntityKind_Mutex,
	OS_MAC_EntityKind_RWMutex,
}
OS_MAC_EntityKind;

typedef struct OS_MAC_Entity OS_MAC_Entity;
struct OS_MAC_Entity
{
	OS_MAC_Entity *next;
	OS_MAC_EntityKind kind;
	union
	{
		struct // OS_MAC_EntityKind_Thread
		{
			void *params;
			OS_ThreadFunctionType *func;
			pthread_t pthread_handle;
		} thread;
		pthread_mutex_t pthread_mutex_handle; // OS_MAC_EntityKind_Mutex
		pthread_rwlock_t pthread_rwlock_handle;
	};
};

typedef struct OS_MAC_State OS_MAC_State;
struct OS_MAC_State
{
	pthread_mutex_t entity_mutex;
	Arena *entity_arena;
	OS_MAC_Entity *entity_free;
};

typedef struct OS_MAC_FileIter OS_MAC_FileIter;
struct OS_MAC_FileIter { FTS *fts; };
StaticAssert(
	sizeof(OS_MAC_FileIter) <= sizeof(Member(OS_FileIter, memory)),
	os_mac_file_iter_size_check
);
StaticAssert(
	AlignOf(OS_MAC_FileIter) <= AlignOf(Member(OS_FileIter, memory)),
	os_mac_file_iter_alignment_check
);

internal FileProperties os_mac_file_properties_from_stat(struct stat *s);
internal void * os_mac_thread_entry_point(void *params);

internal OS_MAC_Entity *os_mac_entity_alloc(OS_MAC_EntityKind kind);
internal void           os_mac_entity_release(OS_MAC_Entity *entity);

#endif // OS_CORE_MAC_H
