// TODO:
//  large pages

#ifndef OS_CORE_H
#define OS_CORE_H

typedef struct OS_State OS_State; // REVIEW: name
struct OS_State
{
	Arena *arena; // REVIEW
	struct
	{
		S64 page_size;
	} system_info;
	struct
	{
		// TODO:
		//  pid
		//  env vars
		String8 exe_folder, // REVIEW: include exe name?
			initial_working_directory;
	} process_info;
};

global OS_State g_os_state = zero_struct; // REVIEW: other compile units? perhaps extern and !BUILD_SUPPLEMENTARY_UNIT decl in os_core.c file

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
	OS_FileIterFlag_SkipFolders    = (1 << 0),
	OS_FileIterFlag_SkipFiles      = (1 << 1),
	OS_FileIterFlag_SkipHidden     = (1 << 2),
	OS_FileIterFlag_RecurseFolders = (1 << 3),
};

typedef struct OS_FileIter OS_FileIter;
struct OS_FileIter
{
	OS_FileIterFlags flags;
	U64 memory[256]; // 8-byte aligned
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
	S64 size;
	FilePropertyFlags flags;
};

// TODO(beau): unify with FileProperties
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

internal void *os_vmem_reserve(S64 size);
internal B32   os_vmem_commit(void *ptr, S64 size);
internal void  os_vmem_decommit(void *ptr, S64 size);
internal void  os_vmem_release(void *ptr, S64 size);

#define os_handle_zero ((OS_Handle) zero_struct)
internal B32 os_handle_match(OS_Handle a, OS_Handle b);

no_return internal void os_abort(S32 exit_code);

internal String8   os_data_from_file_path(Arena *arena, String8 path);
internal B32       os_write_data_to_file_path(String8 path, String8 data);
internal B32       os_write_data_list_to_file_path(String8 path, String8List list);
internal B32       os_append_data_to_file_path(String8 path, String8 data);
internal String8   os_string_from_file_range(Arena *arena, OS_Handle file, Rng1S64 range); // REVIEW: name better

internal OS_Handle      os_file_open(OS_AccessFlags flags, String8 path);
internal void           os_file_close(OS_Handle file);
internal S64            os_file_read(OS_Handle file, Rng1S64 rng, void *out_data);
internal S64            os_file_write(OS_Handle file, Rng1S64 rng, void *data);
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
internal String8 os_get_current_folder(Arena *arena);

internal S64 os_now_microseconds(void);

internal void os_set_thread_name(String8 name);

internal OS_Handle os_thread_launch(OS_ThreadFunctionType *func, void *params);
internal B32       os_thread_join(OS_Handle handle, S64 endt_us);
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

#define OS_MutexScope(mutex) DeferLoop(os_mutex_take(mutex), os_mutex_drop(mutex))
#define OS_MutexScopeR(mutex) DeferLoop(os_rw_mutex_take_r(mutex), os_rw_mutex_drop_r(mutex))
#define OS_MutexScopeW(mutex) DeferLoop(os_rw_mutex_take_w(mutex), os_rw_mutex_drop_w(mutex))
#define OS_MutexScopeRWPromote(mutex) DeferLoop((os_rw_mutex_drop_r(mutex), os_rw_mutex_take_w(mutex)), (os_rw_mutex_drop_w(mutex), os_rw_mutex_take_r(mutex)))

#endif // OS_CORE_H
