#ifndef BASE_THREAD_CONTEXT_H
#define BASE_THREAD_CONTEXT_H

// REVIEW(beau): raddbg uses c strings and lengths for string members, is there a reason?
typedef struct TCTX TCTX;
struct TCTX
{
	Arena * arenas[2];

	String8 thread_name;

	String8 file_name;
	U64 line_number;
};

function void   tctx_init_and_equip(TCTX *tctx);
function void   tctx_release(void);

function Arena* tctx_get_scratch(Arena **conflicts, U64 count);

#define tctx_write_this_srcloc() {tctx_thread_local->file_name=str8_lit(__FILE__);tctx_thread_local->line_number=__LINE__;}

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts)), (count))
#define scratch_end(scratch) temp_end(scratch)

#endif // BASE_THREAD_CONTEXT_H
