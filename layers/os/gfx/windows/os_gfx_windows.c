internal LRESULT
os_gfx_windows_window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam) // REVIEW: name
{
	// REVIEW: push one event at the end?
	LRESULT result = 0;

	OS_Event event = zero_struct;

	switch (message)
	{
		// TODO: raw input for multiple keyboard support
		// REVIEW: WM_SYSKEY[DOWN|UP]?
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			// TODO: move to metagen
			const local_persist OS_Key virtual_to_os_key_table[max_U8] =
			{
				[VK_LBUTTON ] = OS_Key_LeftMouseButton,
				[VK_RBUTTON ] = OS_Key_RightMouseButton,
				[VK_MBUTTON ] = OS_Key_MiddleMouseButton,
				[VK_XBUTTON1] = OS_Key_M4,
				[VK_XBUTTON2] = OS_Key_M5,
				[VK_BACK    ] = OS_Key_Backspace,
				[VK_TAB     ] = OS_Key_Tab,
				[VK_RETURN  ] = OS_Key_Enter,
				[VK_CAPITAL ] = OS_Key_CapsLock,
				[VK_ESCAPE  ] = OS_Key_Esc,
				[VK_SPACE   ] = OS_Key_Space,
				[VK_LEFT    ] = OS_Key_Left,
				[VK_UP      ] = OS_Key_Up,
				[VK_RIGHT   ] = OS_Key_Right,
				[VK_DOWN    ] = OS_Key_Down,

				// ascii
				['0'] = OS_Key_0,
				['1'] = OS_Key_1,
				['2'] = OS_Key_2,
				['3'] = OS_Key_3,
				['4'] = OS_Key_4,
				['5'] = OS_Key_5,
				['6'] = OS_Key_6,
				['7'] = OS_Key_7,
				['8'] = OS_Key_8,
				['9'] = OS_Key_9,
				['a'] = OS_Key_A,
				['b'] = OS_Key_B,
				['c'] = OS_Key_C,
				['d'] = OS_Key_D,
				['e'] = OS_Key_E,
				['f'] = OS_Key_F,
				['g'] = OS_Key_G,
				['h'] = OS_Key_H,
				['i'] = OS_Key_I,
				['j'] = OS_Key_J,
				['k'] = OS_Key_K,
				['l'] = OS_Key_L,
				['m'] = OS_Key_M,
				['n'] = OS_Key_N,
				['o'] = OS_Key_O,
				['p'] = OS_Key_P,
				['q'] = OS_Key_Q,
				['r'] = OS_Key_R,
				['s'] = OS_Key_S,
				['t'] = OS_Key_T,
				['u'] = OS_Key_U,
				['v'] = OS_Key_V,
				['w'] = OS_Key_W,
				['x'] = OS_Key_X,
				['y'] = OS_Key_Y,
				['z'] = OS_Key_Z,
				// end ascii

				[VK_F1        ] = OS_Key_F1,
				[VK_F2        ] = OS_Key_F2,
				[VK_F3        ] = OS_Key_F3,
				[VK_F4        ] = OS_Key_F4,
				[VK_F5        ] = OS_Key_F5,
				[VK_F6        ] = OS_Key_F6,
				[VK_F7        ] = OS_Key_F7,
				[VK_F8        ] = OS_Key_F8,
				[VK_F9        ] = OS_Key_F9,
				[VK_F10       ] = OS_Key_F10,
				[VK_F11       ] = OS_Key_F11,
				[VK_F12       ] = OS_Key_F12,

				[VK_LSHIFT    ] = OS_Key_LeftShift,
				[VK_RSHIFT    ] = OS_Key_RightShift,
				[VK_LCONTROL  ] = OS_Key_LeftControl,
				[VK_RCONTROL  ] = OS_Key_RightControl,
				[VK_LMENU     ] = OS_Key_LeftAlt,
				[VK_RMENU     ] = OS_Key_RightAlt,
				[VK_OEM_PLUS  ] = OS_Key_Equals,
				[VK_OEM_COMMA ] = OS_Key_Comma,
				[VK_OEM_MINUS ] = OS_Key_Minus,
				[VK_OEM_PERIOD] = OS_Key_Period,
			};
			U8 vkeycode       = safe_cast_u8(wparam);
			U32 keyinfo       = safe_cast_u32(lparam);
			U32 repeat_count  = keyinfo & bitmask16;
			B32 wasdown       = (keyinfo & bit31) != 0,
			    isdown        = (keyinfo & bit32) == 0;
			Unused(repeat_count); // TODO: repeats
			if (wasdown != isdown)
			{
				event.kind = isdown ? OS_EventKind_Press : OS_EventKind_Release;
				event.key  = virtual_to_os_key_table[vkeycode];
			}
		} break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		{
			// REVIEW: coordinate conventions
			event.pos.x = (F32)GET_X_LPARAM(lparam);
			event.pos.y = (F32)GET_Y_LPARAM(lparam);
			switch (message)
			{
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				{
					event.key = OS_Key_M1;
				} break;
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
				{
					event.key = OS_Key_M2;
				} break;
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
				{
					event.key = OS_Key_M3;
				} break;
				case WM_XBUTTONDOWN:
				case WM_XBUTTONUP:
				{
					event.key = GET_XBUTTON_WPARAM(wparam) == 1 ? OS_Key_M4 : OS_Key_M5;
				} break;
				default:
				{
					Unreachable;
				} break;
			}
			switch (message)
			{
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_MBUTTONDOWN:
				case WM_XBUTTONDOWN:
				{
					event.kind = OS_EventKind_Press;
				} break;
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
				case WM_MBUTTONUP:
				case WM_XBUTTONUP:
				{
					event.kind = OS_EventKind_Release;
				} break;
				default:
				{
					Unreachable;
				} break;
			}
		} break;
		case WM_MOUSEMOVE:
		{
			event.kind  = OS_EventKind_MouseMove;
			// REVIEW: coordinate system conversion
			event.pos.x = (F32)GET_X_LPARAM(lparam);
			event.pos.y = (F32)GET_Y_LPARAM(lparam);

			// REVIEW: mouse mods for every event with GetKeyState?
			U8 mods = safe_cast_u8(wparam);
			if (mods & MK_LBUTTON)
				event.modifiers |= OS_Modifier_M1;
			if (mods & MK_RBUTTON)
				event.modifiers |= OS_Modifier_M2;
			if (mods & MK_MBUTTON)
				event.modifiers |= OS_Modifier_M3;
			if (mods & MK_XBUTTON1)
				event.modifiers |= OS_Modifier_M4;
			if (mods & MK_XBUTTON2)
				event.modifiers |= OS_Modifier_M5;
		} break;
		case WM_SIZE:
		case WM_PAINT:
		{
			// URGENT REVIEW
			PAINTSTRUCT ps = zero_struct;
			BeginPaint(window, &ps);
			EndPaint(window, &ps);
		} break;
		case WM_CLOSE:
		case WM_DESTROY:
		{
			event.kind = OS_EventKind_Quit;
		} break;

		// NOTE(beau): stuff we do nothing with, but don't want to pass to DefWindowProc
		case WM_ACTIVATEAPP: break;

		default:
		{
			result = DefWindowProcW(window,message, wparam, lparam);
		} break;
	}

	if (event.kind != OS_EventKind_NULL)
	{
		// TODO: put event position in some coordinate system where 0, 0 is the center of the window
		if (GetKeyState(VK_SHIFT) & bit16)
			event.modifiers |= OS_Modifier_Shift;
		if (GetKeyState(VK_CONTROL) & bit16)
			event.modifiers |= OS_Modifier_Ctrl;
		if (GetKeyState(VK_MENU) & bit16)
			event.modifiers |= OS_Modifier_Alt;
		if (GetKeyState(VK_CAPITAL) & bit1)
			event.modifiers |= OS_Modifier_CapsLock;

		for (OS_WINDOWS_Window *gfxwindow = g_os_gfx_windows_state.first_window; gfxwindow; gfxwindow = gfxwindow->next)
		{
			if (gfxwindow->hwnd == window)
			{
				event.window = os_windows_handle_from_window(gfxwindow);
				break;
			}
		}
		Assert(event.window.bits);
		OS_Event *pushed = os_eventlist_push_new(
			g_os_gfx_windows_state.events_arena,
			&g_os_gfx_windows_state.events,
			OS_EventKind_NULL
		);
		*pushed = event;
		Assert(pushed->kind != OS_EventKind_NULL);
	}

	return result;
}

internal void os_gfx_init(void)
{
	g_os_mac_gfx_state.arena = arena_alloc(MB(32), MB(1));
	g_os_gfx_windows_state.hInstance = GetModuleHandleW(0);
	WNDCLASSW window_class = {
		.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc   = os_gfx_windows_window_callback,
		.hInstance     = g_os_gfx_windows_state.hInstance,
		.lpszClassName = L"mywindowclass",
	};
	ATOM atom = RegisterClassW(&window_class);
	Assert(atom);
}

internal OS_Window os_window_open(Vec2S32 resolution, String8 title)
{
	Assert(0 <= title.length);
	Temp scratch = scratch_begin(0, 0);
	OS_WINDOWS_Window *gfxwindow = 0;
	if (g_os_gfx_windows_state.free_window)
	{
		gfxwindow = g_os_gfx_windows_state.free_window;
		SLLStackPop(g_os_gfx_windows_state.free_window);
	}
	else
	{
		gfxwindow = push_array_no_zero(g_os_gfx_windows_state.arena, OS_WINDOWS_Window, 1);
		gfxwindow->generation = 1; // zero generation is never a valid reference
	}

	String16 title16 = str16_from_8(scratch.arena, title);

	*gfxwindow = (OS_WINDOWS_Window) {
		.hwnd = CreateWindowExW(
			0,
			L"mywindowclass", // REVIEW: deduplicate?
			title16.buffer,
			WS_OVERLAPPEDWINDOW|WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			resolution.x,
			resolution.y,
			0,
			0,
			g_os_gfx_windows_state.hInstance,
			0
		),
		.generation = gfxwindow->generation;
	};
	Assert(gfxwindow.hwnd);

	DLLPushFront(
		g_os_gfx_windows_state.first_window,
		g_os_gfx_windows_state.last_window,
		gfxwindow
	);

	scratch_end(scratch);

	OS_Window result = os_windows_handle_from_window(gfxwindow);
	return result;
}
internal void os_window_close(OS_Window window)
{
	OS_WINDOWS_Window *gfxwindow = os_windows_window_from_handle(window);
	if (gfxwindow)
	{
		Assert(DestroyWindow(gfxwindow->hwnd));
		gfxwindow->generation++;
		DLLRemove(
			g_os_gfx_windows_state.first_window,
			g_os_gfx_windows_state.last_window,
			gfxwindow
		);
		SLLStackPop(g_os_gfx_windows_state.free_window, gfxwindow);
	}
}

internal OS_EventList os_gfx_get_events(Arena *arena)
{
	g_os_gfx_windows_state.events_arena = arena;

	for (MSG message = zero_struct; PeekMessageW(&message, 0, 0, 0, PM_REMOVE); )
	{
		TranslateMessage(&message);
		DispatchMessageW(&message);
		if (message.message == WM_QUIT)
		{
			OS_Event *event = os_eventlist_push_new(
				g_os_gfx_windows_state.events_arena,
				&g_os_gfx_windows_state.events,
				OS_EventKind_Quit
			);
			Unused(event);
		}
	}

	OS_EventList result = g_os_gfx_windows_state.events;
	g_os_gfx_windows_state.events = (OS_EventList)zero_struct;
	return result;
}

internal Rng2
os_gfx_windows_rng2f32_from_rect(RECT rect)
{
	// REVIEW: start at bottom left?
	Rng2 result = {
		.x0 = (F32)rect.left,
		.y0 = (F32)rect.top,
		.x1 = (F32)rect.right,
		.y1 = (F32)rect.bottom,
	};
	return result;
}

inline internal OS_Window os_windows_handle_from_window(OS_WINDOWS_Window *window)
{
	OS_Window result = { .bits = (U64)window, .generation = window->generation };
	return result;
}
inline internal OS_WINDOWS_Window *os_windows_window_from_handle(OS_Window handle)
{
	OS_WINDOWS_Window *result = (OS_WINDOWS_Window*)handle.bits;
	if (!result || result->generation != handle.generation)
		result = 0;
	return result;
}
