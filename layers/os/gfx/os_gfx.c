internal OS_Event*
os_eventlist_push_new(Arena *arena, OS_EventList *list, OS_EventKind kind)
{
	OS_EventNode *node = push_array(arena, OS_EventNode, 1);
	SLLQueuePush(list->first, list->last, node);
	list->count += 1;
	OS_Event *result = &node->event;
	result->kind = kind;
	result->timestamp_us = os_now_microseconds();
	return result;
}
