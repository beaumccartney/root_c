#ifndef BASE_THREAD_CONTEXT_H
#define BASE_THREAD_CONTEXT_H

typedef struct TCTX TCTX;
struct TCTX
{
	Arena * arenas[2];

	char *file_name;
	S64 line_number;

	U8 thread_name[32];
	S8 thread_name_length;
};

internal void tctx_init_and_equip(TCTX *tctx);
internal void tctx_release(void);

internal void    tctx_set_thread_name(String8 name);
internal String8 tctx_get_thread_name(void);

internal Arena* tctx_get_scratch(Arena *conflicts[], S64 count);

#define tctx_write_this_srcloc() {tctx_thread_local->file_name=__FILE__;tctx_thread_local->line_number=__LINE__;}

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)

#endif // BASE_THREAD_CONTEXT_H
