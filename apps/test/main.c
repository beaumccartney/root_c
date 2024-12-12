#include "base/base_inc.h"
#include "os/os_inc.h"

#include "base/base_inc.c"
#include "os/os_inc.c"

#include <stdio.h>

typedef struct Node Node;
struct Node
{
	U64 datum;
	Node *next, *prev;
};

int main(void)
{
	TCTX tctx;
	tctx_init_and_equip(&tctx);
	tctx_write_this_srcloc();

	Arena * scratch1 = tctx_get_scratch(0, 0);
	Arena * scratch2 = tctx_get_scratch(&scratch1, 1);

	Arena * arena = scratch2;

	Node * base = 0;

	for (U64 i = 0; i < KB(1); i++)
	{
		Node * new = push_array(arena, Node, 1);

		new->datum = i;

		SLLStackPush(base, new);
	}

	for (Node * n = base; n != 0; n = n->next)
	{
		printf("datum: %llu\n", n->datum);
	}

	arena_release(arena);

	return 0;
}
