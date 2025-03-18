@implementation OS_MAC_NSApplication : NSApplication
- (void)quit_render_loop:(id) sender {
	OS_Event *event = os_eventlist_push_new(
		g_os_mac_gfx_state.events_arena,
		&g_os_mac_gfx_state.events,
		OS_EventKind_Quit
	);
	Unused(event);
}
@end

@implementation OS_MAC_NSWindow : NSWindow
- (void)keyDown:(NSEvent *) event {}
- (BOOL) windowShouldClose:(OS_MAC_NSWindow *) sender
{
	OS_Event *event = os_eventlist_push_new(
		g_os_mac_gfx_state.events_arena,
		&g_os_mac_gfx_state.events,
		OS_EventKind_WindowClose
	);
	event->window = os_mac_handle_from_nswindow(sender);
	Assert(event->window.generation > 0);

	return false;
}
@end

internal void os_gfx_init(void)
{
	[OS_MAC_NSApplication sharedApplication];
	[OS_MAC_NSApplication sharedApplication].activationPolicy = NSApplicationActivationPolicyRegular;
	[[OS_MAC_NSApplication sharedApplication] finishLaunching];

	@autoreleasepool
	{
		NSMenu *main_menu = [[NSMenu alloc] init];
		NSMenuItem *main_menu_app_item = [main_menu addItemWithTitle: @"Appname" action:0 keyEquivalent:@""];
		NSMenu *app_menu = [[NSMenu alloc] init];
		NSMenuItem *quit_item = [[NSMenuItem alloc] initWithTitle: @"Quit"
			action:@selector(quit_render_loop:)
			keyEquivalent:@"q"];
		[app_menu addItem: quit_item];
		main_menu_app_item.submenu = app_menu;

		[OS_MAC_NSApplication sharedApplication].mainMenu = main_menu;
	}

	[[OS_MAC_NSApplication sharedApplication] activate];
}

internal OS_EventList os_gfx_get_events(Arena *arena)
{
	g_os_mac_gfx_state.events_arena = arena;
	@autoreleasepool
	{
		NSEvent *ns_event = [[OS_MAC_NSApplication sharedApplication] nextEventMatchingMask:NSEventMaskAny
			untilDate:[NSDate distantFuture]
			inMode:NSDefaultRunLoopMode
			dequeue:YES];
		for (; ns_event != 0; ns_event = [[OS_MAC_NSApplication sharedApplication]
				nextEventMatchingMask:NSEventMaskAny
				untilDate:0
				inMode:NSDefaultRunLoopMode
				dequeue:YES])
		{
			NSEventType ns_event_type = ns_event.type;
			OS_Event event = zero_struct;
			switch (ns_event_type)
			{
				case NSEventTypeLeftMouseDown:
				case NSEventTypeLeftMouseUp:
				case NSEventTypeRightMouseDown:
				case NSEventTypeRightMouseUp:
				case NSEventTypeMouseMoved:
				case NSEventTypeLeftMouseDragged:
				case NSEventTypeRightMouseDragged:
				case NSEventTypeFlagsChanged:
				case NSEventTypeOtherMouseDown:
				case NSEventTypeOtherMouseUp:
				case NSEventTypeOtherMouseDragged:
				default: break;
				case NSEventTypeKeyDown:
				case NSEventTypeKeyUp:
				{
					const local_persist
					OS_Key os_mac_kvk_table[] = {
						[kVK_ANSI_A]            = OS_Key_A,
						[kVK_ANSI_S]            = OS_Key_S,
						[kVK_ANSI_D]            = OS_Key_D,
						[kVK_ANSI_F]            = OS_Key_F,
						[kVK_ANSI_H]            = OS_Key_H,
						[kVK_ANSI_G]            = OS_Key_G,
						[kVK_ANSI_Z]            = OS_Key_Z,
						[kVK_ANSI_X]            = OS_Key_X,
						[kVK_ANSI_C]            = OS_Key_C,
						[kVK_ANSI_V]            = OS_Key_V,
						[kVK_ANSI_B]            = OS_Key_B,
						[kVK_ANSI_Q]            = OS_Key_Q,
						[kVK_ANSI_W]            = OS_Key_W,
						[kVK_ANSI_E]            = OS_Key_E,
						[kVK_ANSI_R]            = OS_Key_R,
						[kVK_ANSI_Y]            = OS_Key_Y,
						[kVK_ANSI_T]            = OS_Key_T,
						[kVK_ANSI_1]            = OS_Key_1,
						[kVK_ANSI_2]            = OS_Key_2,
						[kVK_ANSI_3]            = OS_Key_3,
						[kVK_ANSI_4]            = OS_Key_4,
						[kVK_ANSI_6]            = OS_Key_6,
						[kVK_ANSI_5]            = OS_Key_5,
						[kVK_ANSI_Equal]        = OS_Key_Equals,
						[kVK_ANSI_9]            = OS_Key_9,
						[kVK_ANSI_7]            = OS_Key_7,
						[kVK_ANSI_Minus]        = OS_Key_Minus,
						[kVK_ANSI_8]            = OS_Key_8,
						[kVK_ANSI_0]            = OS_Key_0,
						[kVK_ANSI_RightBracket] = OS_Key_RightBracket,
						[kVK_ANSI_O]            = OS_Key_O,
						[kVK_ANSI_U]            = OS_Key_U,
						[kVK_ANSI_LeftBracket]  = OS_Key_LeftBracket,
						[kVK_ANSI_I]            = OS_Key_I,
						[kVK_ANSI_P]            = OS_Key_P,
						[kVK_Return]            = OS_Key_Return,
						[kVK_ANSI_L]            = OS_Key_L,
						[kVK_ANSI_J]            = OS_Key_J,
						[kVK_ANSI_Quote]        = OS_Key_Quote,
						[kVK_ANSI_K]            = OS_Key_K,
						[kVK_ANSI_Semicolon]    = OS_Key_Semicolon,
						[kVK_ANSI_Backslash]    = OS_Key_Backslash,
						[kVK_ANSI_Comma]        = OS_Key_Comma,
						[kVK_ANSI_Slash]        = OS_Key_Slash,
						[kVK_ANSI_N]            = OS_Key_N,
						[kVK_ANSI_M]            = OS_Key_M,
						[kVK_ANSI_Period]       = OS_Key_Period,
						[kVK_Tab]               = OS_Key_Tab,
						[kVK_Space]             = OS_Key_Space,
						[kVK_ANSI_Grave]        = OS_Key_GraveAccent,
						[kVK_Delete]            = OS_Key_Backspace,
						[kVK_Escape]            = OS_Key_Escape,
						[kVK_Shift]             = OS_Key_LeftShift,
						[kVK_CapsLock]          = OS_Key_CapsLock,
						[kVK_Option]            = OS_Key_LeftAlt,
						[kVK_Control]           = OS_Key_LeftControl,
						[kVK_RightShift]        = OS_Key_RightShift,
						[kVK_RightOption]       = OS_Key_RightAlt,
						[kVK_ANSI_Keypad9]      = OS_Key_F5,
						[kVK_F5]                = OS_Key_F6,
						[kVK_F6]                = OS_Key_F7,
						[kVK_F7]                = OS_Key_F8,
						[kVK_F3]                = OS_Key_F3,
						[kVK_F8]                = OS_Key_F8,
						[kVK_F9]                = OS_Key_F9,
						[kVK_F11]               = OS_Key_F11,
						[kVK_F10]               = OS_Key_F10,
						[kVK_F12]               = OS_Key_F12,
						[kVK_F4]                = OS_Key_F4,
						[kVK_F2]                = OS_Key_F2,
						[kVK_F1]                = OS_Key_F1,
						[kVK_LeftArrow]         = OS_Key_Left,
						[kVK_RightArrow]        = OS_Key_Right,
						[kVK_DownArrow]         = OS_Key_Down,
						[kVK_UpArrow]           = OS_Key_Up,
					};

					OS_Key os_key = os_mac_kvk_table[ns_event.keyCode];
					if (os_key != OS_Key_NULL)
					{
						event.kind = ns_event_type == NSEventTypeKeyDown ? OS_EventKind_Press : OS_EventKind_Release;
						event.key = os_key;
					}
				} break;
			}

			[[OS_MAC_NSApplication sharedApplication] sendEvent:ns_event];

			if (event.kind != OS_EventKind_NULL)
			{
				{
					NSEventModifierFlags ns_mods = ns_event.modifierFlags;
					if (ns_mods & NSEventModifierFlagShift)
						event.modifiers |= OS_Modifier_Shift;
					if (ns_mods & NSEventModifierFlagControl)
						event.modifiers |= OS_Modifier_Ctrl;
					if (ns_mods & NSEventModifierFlagOption)
						event.modifiers |= OS_Modifier_Alt;
					if (ns_mods & NSEventModifierFlagCapsLock)
						event.modifiers |= OS_Modifier_CapsLock;

					NSUInteger ns_mouse = NSEvent.pressedMouseButtons;
					if (ns_mouse & (1 << 0))
						event.modifiers |= OS_Modifier_M1;
					if (ns_mouse & (1 << 1))
						event.modifiers |= OS_Modifier_M2;
					if (ns_mouse & (1 << 2))
						event.modifiers |= OS_Modifier_M3;
					if (ns_mouse & (1 << 3))
						event.modifiers |= OS_Modifier_M4;
					if (ns_mouse & (1 << 4))
						event.modifiers |= OS_Modifier_M5;
				}


				event.window = os_mac_handle_from_nswindow((OS_MAC_NSWindow *)ns_event.window);
				OS_Event *pushed = os_eventlist_push_new(
					arena,
					&g_os_mac_gfx_state.events,
					OS_EventKind_NULL
				);
				*pushed = event;
				Assert(pushed->kind != OS_EventKind_NULL);
			}
		}
	}
	OS_EventList result = g_os_mac_gfx_state.events;
	g_os_mac_gfx_state.events = (OS_EventList)zero_struct;
	return result;
}

internal OS_Window os_window_open(Vec2S32 resolution, String8 title)
{
	Assert(0 < title.length && 0 <= resolution.x && 0 <= resolution.y);

	OS_MAC_Window *macwindow = 0;

	if (0 < g_os_mac_gfx_state.free_plus_one && g_os_mac_gfx_state.free_plus_one < ArrayCount(g_os_mac_gfx_state.windows))
	{
		macwindow = &g_os_mac_gfx_state.windows[g_os_mac_gfx_state.free_plus_one - 1];
		Assert(macwindow->generation > 0);
	}
	else if (0 <= g_os_mac_gfx_state.lowest_unused && g_os_mac_gfx_state.lowest_unused < ArrayCount(g_os_mac_gfx_state.windows))
	{
		macwindow = &g_os_mac_gfx_state.windows[g_os_mac_gfx_state.lowest_unused++];
		Assert(macwindow->generation == 0);
		macwindow->generation = 1;
	}

	if (macwindow)
	{
		Assert(!macwindow->nswindow);
		g_os_mac_gfx_state.free_plus_one = macwindow->next_free_plus_one;

		NSRect screen_rect = NSScreen.mainScreen.visibleFrame;
		NSSize window_size = {
			(CGFloat)resolution.x,
			(CGFloat)resolution.y,
		};
		NSPoint window_origin = { // REVIEW:
			((screen_rect.size.width - window_size.width)   / 2),
			((screen_rect.size.height - window_size.height) / 2)
		};
		*macwindow = (OS_MAC_Window) {
			.nswindow = [[OS_MAC_NSWindow alloc] initWithContentRect:
				(NSRect) {window_origin, window_size}
				styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
				backing:NSBackingStoreBuffered
				defer:NO],
			.generation = macwindow->generation,
		};

		macwindow->nswindow.title = os_mac_nsstring_from_str8(title);
		macwindow->nswindow.isVisible = YES;

		macwindow->nswindow.opaque = YES;
		macwindow->nswindow.backgroundColor = 0;
		macwindow->nswindow.releasedWhenClosed = false; // XXX REVIEW: how can I release a window when its closed?
	}

	OS_Window result = os_mac_handle_from_window(macwindow);
	return result;
}

internal void os_window_close(OS_Window window)
{
	OS_MAC_Window *macwindow = os_mac_window_from_handle(window);
	if (macwindow && macwindow->nswindow)
	{
		[macwindow->nswindow close];
		[macwindow->nswindow release];
		*macwindow = (OS_MAC_Window) {
			.generation         = macwindow->generation + 1,
			.next_free_plus_one = g_os_mac_gfx_state.free_plus_one,
		};
		g_os_mac_gfx_state.free_plus_one = (S64)window.bits + 1;
	}
}

inline internal OS_Window os_mac_handle_from_window(OS_MAC_Window *macwindow)
{
	OS_Window result = zero_struct;
	if (macwindow)
	{
		S64 index = macwindow - g_os_mac_gfx_state.windows;
		Assert(0 <= index && index < ArrayCount(g_os_mac_gfx_state.windows));
		if (macwindow->nswindow)
		{
			result.bits       = (U64)index;
			result.generation = macwindow->generation;
		}
	}
	return result;
}

inline internal OS_MAC_Window *os_mac_window_from_handle(OS_Window handle)
{
	S64 index = (S64)handle.bits;
	Assert(0 <= index && index < ArrayCount(g_os_mac_gfx_state.windows));
	OS_MAC_Window *result = &g_os_mac_gfx_state.windows[index];
	if (!result->nswindow || result->generation != handle.generation)
		result = 0;
	return result;
}
internal OS_Window os_mac_handle_from_nswindow(OS_MAC_NSWindow *nswindow)
{
	OS_MAC_Window *macwindow = 0;

	if (nswindow)
	{
		OS_MAC_Window *w = g_os_mac_gfx_state.windows, *opl = w + ArrayCount(g_os_mac_gfx_state.windows);
		for (; w < opl; w++)
		{
			if (w->nswindow == nswindow)
			{
				macwindow = w;
				break;
			}
		}
		Assert(macwindow);
	}
	OS_Window result = os_mac_handle_from_window(macwindow);
	return result;
}

internal NSString *os_mac_nsstring_from_str8(String8 string8)
{
	Assert(0 <= string8.length);
	NSString *result = [[NSString alloc] initWithBytes: string8.buffer // REVIEW: free the string on release?
		length:(NSUInteger)string8.length
		encoding:NSUTF8StringEncoding];
	return result;
}
