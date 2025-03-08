#ifndef OS_GFX_WINDOWS_H
#define OS_GFX_WINDOWS_H

#include <Windowsx.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "Gdi32.lib")

typedef struct OS_WINDOWS_GFX_State OS_WINDOWS_GFX_State;
struct OS_WINDOWS_GFX_State
{
	HINSTANCE hInstance;
	HWND window;
	Arena *events_arena;
	OS_EventList events;
};

global OS_WINDOWS_GFX_State g_os_gfx_windows_state = zero_struct;

internal Rng2
os_gfx_windows_rng2f32_from_rect(RECT rect);

#endif // OS_GFX_WINDOWS_H
