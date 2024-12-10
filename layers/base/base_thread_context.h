#ifndef BASE_THREAD_CONTEXT_H
#define BASE_THREAD_CONTEXT_H

typedef struct TCTX TCTX;
struct TCTX
{
	Arena * arenas[2];
};

function void   tctx_init_and_equip(TCTX *tctx);
function void   tctx_release(void);
function TCTX*  tctx_get_equipped(void);

function Arena* tctx_get_scratch(Arena **conflicts, U64 count);

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts)), (count))
#define scratch_end(scratch) temp_end(scratch)

#endif // BASE_THREAD_CONTEXT_H
