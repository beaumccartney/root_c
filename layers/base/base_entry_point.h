#ifndef BASE_ENTRY_POINT_H
#define BASE_ENTRY_POINT_H

#if BUILD_ENTRY_DEFINING_UNIT
	internal void main_thread_base_entry_point(int argc, char *argv[]);

	internal void entry_point(void); // application-defined entry point i.e. the program
#endif

internal void thread_base_entry_point(void (*entry_point)(void *params), void *params);

#endif // BASE_ENTRY_POINT_H
