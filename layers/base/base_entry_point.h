#ifndef BASE_ENTRY_POINT_H
#define BASE_ENTRY_POINT_H

internal void main_thread_base_entry_point(String8Array cmdline_args);

internal void entry_point(void); // application-defined entry point i.e. the program

internal void thread_base_entry_point(void (*entry_point)(void *params), void *params);

#endif // BASE_ENTRY_POINT_H
