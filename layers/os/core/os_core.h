#ifndef OS_CORE_H
#define OS_CORE_H

typedef struct OS_SystemInfo OS_SystemInfo;
struct OS_SystemInfo
{
	U64 page_size, large_page_size;
};

typedef U32 OS_AccessFlags;
enum
{
	OS_AccessFlag_Read       = (1<<0),
	OS_AccessFlag_Write      = (1<<1),
	OS_AccessFlag_Append     = (1<<2),
};

typedef U32 OS_FileIterFlags;
enum
{
	OS_FileIterFlag_SkipFolders     = (1 << 0),
	OS_FileIterFlag_SkipFiles       = (1 << 1),
	OS_FileIterFlag_SkipHiddenFiles = (1 << 2),
	OS_FileIterFlag_Done            = (1 << 31),
};

typedef struct OS_FileIter OS_FileIter;
struct OS_FileIter
{
	OS_FileIterFlags flags;
	U8 memory[800];
};

typedef U32 FilePropertyFlags;
enum
{
	FilePropertyFlag_IsFolder = (1 << 0),
};

// TODO(beau): created time, last modified time
typedef struct FileProperties FileProperties;
struct FileProperties
{
	U64 size;
	FilePropertyFlags flags;
};

typedef struct OS_FileInfo OS_FileInfo;
struct OS_FileInfo
{
  String8 name;
  FileProperties props;
};

typedef struct OS_Handle OS_Handle;
struct OS_Handle
{
	U64 u[1];
};

typedef void OS_ThreadFunctionType(void *ptr);

// TODO(beau): initialize in the platform entry point and either return a pointer to that here or just define a global exposed by each header here
internal OS_SystemInfo os_get_system_info(void);

internal void *os_reserve(U64 size);
internal B32   os_commit(void *ptr, U64 size);
internal void  os_decommit(void *ptr, U64 size);
internal void  os_release(void *ptr, U64 size);

internal void *os_reserve_large(U64 size);
internal B32   os_commit_large(void *ptr, U64 size);

#define os_handle_zero ((OS_Handle) zero_struct)
internal B32 os_handle_match(OS_Handle a, OS_Handle b);

internal void os_abort(S32 exit_code);

internal void os_set_thread_name(String8 name);

internal String8   os_data_from_file_path(Arena *arena, String8 path);
internal B32       os_write_data_to_file_path(String8 path, String8 data);
internal B32       os_write_data_list_to_file_path(String8 path, String8List list);
internal B32       os_append_data_to_file_path(String8 path, String8 data);
internal String8   os_string_from_file_range(Arena *arena, OS_Handle file, Rng1U64 range);

internal OS_Handle      os_file_open(OS_AccessFlags flags, String8 path);
internal void           os_file_close(OS_Handle file);
internal U64            os_file_read(OS_Handle file, Rng1U64 rng, void *out_data);
internal U64            os_file_write(OS_Handle file, Rng1U64 rng, void *data);
internal FileProperties os_properties_from_file(OS_Handle file);
internal B32            os_delete_file_at_path(String8 path);
internal B32            os_remove_folder_at_path(String8 path);
internal B32            os_copy_file_path(String8 dst, String8 src);
internal String8        os_full_path_from_path(Arena *arena, String8 path);
internal B32            os_file_path_exists(String8 path);
internal B32            os_folder_path_exists(String8 path);
internal FileProperties os_properties_from_file_path(String8 path);

internal OS_FileIter *os_file_iter_begin(Arena *arena, String8 path, OS_FileIterFlags flags);
internal B32          os_file_iter_next(Arena *arena, OS_FileIter *iter, OS_FileInfo *info_out);
internal void         os_file_iter_end(OS_FileIter *iter);

internal B32 os_create_folder(String8 path);

internal void os_set_thread_name(String8 name);

internal OS_Handle os_thread_launch(OS_ThreadFunctionType *func, void *ptr, void *params);
internal B32       os_thread_join(OS_Handle handle, U64 endt_us);
internal void      os_thread_detach(OS_Handle handle);

internal OS_Handle os_mutex_alloc(void);
internal void      os_mutex_release(OS_Handle mutex);
internal void      os_mutex_take(OS_Handle mutex);
internal void      os_mutex_drop(OS_Handle mutex);

internal OS_Handle os_rw_mutex_alloc(void);
internal void      os_rw_mutex_release(OS_Handle rw_mutex);
internal void      os_rw_mutex_take_r(OS_Handle mutex);
internal void      os_rw_mutex_drop_r(OS_Handle mutex);
internal void      os_rw_mutex_take_w(OS_Handle mutex);
internal void      os_rw_mutex_drop_w(OS_Handle mutex);

internal OS_Handle os_condition_variable_alloc(void);
internal void      os_condition_variable_release(OS_Handle cv);
// returns false on timeout, true on signal, (max_wait_ms = max_U64) -> no timeout
internal B32       os_condition_variable_wait(OS_Handle cv, OS_Handle mutex, U64 endt_us);
internal B32       os_condition_variable_wait_rw_r(OS_Handle cv, OS_Handle mutex_rw, U64 endt_us);
internal B32       os_condition_variable_wait_rw_w(OS_Handle cv, OS_Handle mutex_rw, U64 endt_us);
internal void      os_condition_variable_signal(OS_Handle cv);
internal void      os_condition_variable_broadcast(OS_Handle cv);

internal OS_Handle os_semaphore_alloc(U32 initial_count, U32 max_count, String8 name);
internal void      os_semaphore_release(OS_Handle semaphore);
internal OS_Handle os_semaphore_open(String8 name);
internal void      os_semaphore_close(OS_Handle semaphore);
internal B32       os_semaphore_take(OS_Handle semaphore, U64 endt_us);
internal void      os_semaphore_drop(OS_Handle semaphore);

#define OS_MutexScope(mutex) DeferLoop(os_mutex_take(mutex), os_mutex_drop(mutex))
#define OS_MutexScopeR(mutex) DeferLoop(os_rw_mutex_take_r(mutex), os_rw_mutex_drop_r(mutex))
#define OS_MutexScopeW(mutex) DeferLoop(os_rw_mutex_take_w(mutex), os_rw_mutex_drop_w(mutex))
#define OS_MutexScopeRWPromote(mutex) DeferLoop((os_rw_mutex_drop_r(mutex), os_rw_mutex_take_w(mutex)), (os_rw_mutex_drop_w(mutex), os_rw_mutex_take_r(mutex)))

#endif // OS_CORE_H
