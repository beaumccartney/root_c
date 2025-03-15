#ifndef OS_GFX_WINDOWS_H
#define OS_GFX_WINDOWS_H

#include <Windowsx.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "Gdi32.lib")


typedef struct OS_WINDOWS_Window OS_WINDOWS_Window;
struct OS_WINDOWS_Window
{
	OS_WINDOWS_Window *next, *prev;
	HWND hwnd;
};

typedef struct OS_WINDOWS_GFX_State OS_WINDOWS_GFX_State;
struct OS_WINDOWS_GFX_State
{
	Arena *arena;
	OS_WINDOWS_Window *first_window, *last_window, *free_window;
	HINSTANCE hInstance;
	Arena *events_arena;
	OS_EventList events;
};

global OS_WINDOWS_GFX_State g_os_gfx_windows_state = zero_struct;

internal Rng2
os_gfx_windows_rng2f32_from_rect(RECT rect);

inline internal OS_Window os_windows_handle_from_window(OS_WINDOWS Window *window);
inline internal OS_WINDOWS_Window *os_windows_window_from_handle(OS_Window handle);

#endif // OS_GFX_WINDOWS_H
