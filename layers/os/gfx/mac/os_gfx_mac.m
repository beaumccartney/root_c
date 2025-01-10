#if !BUILD_SUPPLEMENTARY_UNIT
	global OS_MAC_GFX_State *os_mac_gfx_state = 0;
#endif

@implementation MyApp : NSApplication
- (void)quit_render_loop:(id) sender {
	// HACK(beau): this is terrible
	os_mac_gfx_state->private_command_q_should_quit_flag = 1;
}
@end

@implementation MyWindow : NSWindow
- (void)keyDown:(NSEvent *) event {}
@end

const global
OS_Key os_mac_kvk_table[OS_MAC_kVK_COUNT] = {
	[OS_MAC_kVK_ANSI_A]              = OS_Key_A,
	[OS_MAC_kVK_ANSI_S]              = OS_Key_S,
	[OS_MAC_kVK_ANSI_D]              = OS_Key_D,
	[OS_MAC_kVK_ANSI_F]              = OS_Key_F,
	[OS_MAC_kVK_ANSI_H]              = OS_Key_H,
	[OS_MAC_kVK_ANSI_G]              = OS_Key_G,
	[OS_MAC_kVK_ANSI_Z]              = OS_Key_Z,
	[OS_MAC_kVK_ANSI_X]              = OS_Key_X,
	[OS_MAC_kVK_ANSI_C]              = OS_Key_C,
	[OS_MAC_kVK_ANSI_V]              = OS_Key_V,
	[OS_MAC_kVK_ANSI_B]              = OS_Key_B,
	[OS_MAC_kVK_ANSI_Q]              = OS_Key_Q,
	[OS_MAC_kVK_ANSI_W]              = OS_Key_W,
	[OS_MAC_kVK_ANSI_E]              = OS_Key_E,
	[OS_MAC_kVK_ANSI_R]              = OS_Key_R,
	[OS_MAC_kVK_ANSI_Y]              = OS_Key_Y,
	[OS_MAC_kVK_ANSI_T]              = OS_Key_T,
	[OS_MAC_kVK_ANSI_1]              = OS_Key_1,
	[OS_MAC_kVK_ANSI_2]              = OS_Key_2,
	[OS_MAC_kVK_ANSI_3]              = OS_Key_3,
	[OS_MAC_kVK_ANSI_4]              = OS_Key_4,
	[OS_MAC_kVK_ANSI_6]              = OS_Key_6,
	[OS_MAC_kVK_ANSI_5]              = OS_Key_5,
	[OS_MAC_kVK_ANSI_Equal]          = OS_Key_Equals,
	[OS_MAC_kVK_ANSI_9]              = OS_Key_9,
	[OS_MAC_kVK_ANSI_7]              = OS_Key_7,
	[OS_MAC_kVK_ANSI_Minus]          = OS_Key_Minus,
	[OS_MAC_kVK_ANSI_8]              = OS_Key_8,
	[OS_MAC_kVK_ANSI_0]              = OS_Key_0,
	[OS_MAC_kVK_ANSI_RightBracket]   = OS_Key_RightBracket,
	[OS_MAC_kVK_ANSI_O]              = OS_Key_O,
	[OS_MAC_kVK_ANSI_U]              = OS_Key_U,
	[OS_MAC_kVK_ANSI_LeftBracket]    = OS_Key_LeftBracket,
	[OS_MAC_kVK_ANSI_I]              = OS_Key_I,
	[OS_MAC_kVK_ANSI_P]              = OS_Key_P,
	[OS_MAC_kVK_Return]              = OS_Key_Return,
	[OS_MAC_kVK_ANSI_L]              = OS_Key_L,
	[OS_MAC_kVK_ANSI_J]              = OS_Key_J,
	[OS_MAC_kVK_ANSI_Quote]          = OS_Key_Quote,
	[OS_MAC_kVK_ANSI_K]              = OS_Key_K,
	[OS_MAC_kVK_ANSI_Semicolon]      = OS_Key_Semicolon,
	[OS_MAC_kVK_ANSI_Backslash]      = OS_Key_Backslash,
	[OS_MAC_kVK_ANSI_Comma]          = OS_Key_Comma,
	[OS_MAC_kVK_ANSI_Slash]          = OS_Key_Slash,
	[OS_MAC_kVK_ANSI_N]              = OS_Key_N,
	[OS_MAC_kVK_ANSI_M]              = OS_Key_M,
	[OS_MAC_kVK_ANSI_Period]         = OS_Key_Period,
	[OS_MAC_kVK_Tab]                 = OS_Key_Tab,
	[OS_MAC_kVK_Space]               = OS_Key_Space,
	[OS_MAC_kVK_ANSI_Grave]          = OS_Key_GraveAccent,
	[OS_MAC_kVK_Delete]              = OS_Key_Null,
	[OS_MAC_kVK_Escape]              = OS_Key_Escape,
	[OS_MAC_kVK_RightCommand]        = OS_Key_Null,
	[OS_MAC_kVK_Command]             = OS_Key_Null,
	[OS_MAC_kVK_Shift]               = OS_Key_LeftShift,
	[OS_MAC_kVK_CapsLock]            = OS_Key_Null,
	[OS_MAC_kVK_Option]              = OS_Key_LeftAlt,
	[OS_MAC_kVK_Control]             = OS_Key_LeftControl,
	[OS_MAC_kVK_RightShift]          = OS_Key_RightShift,
	[OS_MAC_kVK_RightOption]         = OS_Key_RightAlt,
	[OS_MAC_kVK_RightControl]        = OS_Key_Null,
	[OS_MAC_kVK_Function]            = OS_Key_Null,
	[OS_MAC_kVK_F17]                 = OS_Key_Null,
	[OS_MAC_kVK_ANSI_KeypadDecimal]  = OS_Key_Null,
	[OS_MAC_kVK_ANSI_KeypadMultiply] = OS_Key_Null,
	[OS_MAC_kVK_ANSI_KeypadPlus]     = OS_Key_Null,
	[OS_MAC_kVK_ANSI_KeypadClear]    = OS_Key_Null,
	[OS_MAC_kVK_VolumeUp]            = OS_Key_Null,
	[OS_MAC_kVK_VolumeDown]          = OS_Key_Null,
	[OS_MAC_kVK_Mute]                = OS_Key_Null,
	[OS_MAC_kVK_ANSI_KeypadDivide]   = OS_Key_Null,
	[OS_MAC_kVK_ANSI_KeypadEnter]    = OS_Key_Null,
	[OS_MAC_kVK_ANSI_KeypadMinus]    = OS_Key_Null,
	[OS_MAC_kVK_F18]                 = OS_Key_Null,
	[OS_MAC_kVK_F19]                 = OS_Key_Null,
	[OS_MAC_kVK_ANSI_KeypadEquals]   = OS_Key_Null,
	[OS_MAC_kVK_ANSI_Keypad0]        = OS_Key_Null,
	[OS_MAC_kVK_ANSI_Keypad1]        = OS_Key_Null,
	[OS_MAC_kVK_ANSI_Keypad2]        = OS_Key_Null,
	[OS_MAC_kVK_ANSI_Keypad3]        = OS_Key_Null,
	[OS_MAC_kVK_ANSI_Keypad4]        = OS_Key_Null,
	[OS_MAC_kVK_ANSI_Keypad5]        = OS_Key_Null,
	[OS_MAC_kVK_ANSI_Keypad6]        = OS_Key_Null,
	[OS_MAC_kVK_ANSI_Keypad7]        = OS_Key_Null,
	[OS_MAC_kVK_F20]                 = OS_Key_Null,
	[OS_MAC_kVK_ANSI_Keypad8]        = OS_Key_Null,
	[OS_MAC_kVK_ANSI_Keypad9]        = OS_Key_F5,
	[OS_MAC_kVK_F5]                  = OS_Key_F6,
	[OS_MAC_kVK_F6]                  = OS_Key_F7,
	[OS_MAC_kVK_F7]                  = OS_Key_F8,
	[OS_MAC_kVK_F3]                  = OS_Key_F3,
	[OS_MAC_kVK_F8]                  = OS_Key_F8,
	[OS_MAC_kVK_F9]                  = OS_Key_F9,
	[OS_MAC_kVK_F11]                 = OS_Key_F11,
	[OS_MAC_kVK_F13]                 = OS_Key_Null,
	[OS_MAC_kVK_F16]                 = OS_Key_Null,
	[OS_MAC_kVK_F14]                 = OS_Key_Null,
	[OS_MAC_kVK_F10]                 = OS_Key_F10,
	[OS_MAC_kVK_ContextualMenu]      = OS_Key_Null,
	[OS_MAC_kVK_F12]                 = OS_Key_F12,
	[OS_MAC_kVK_F15]                 = OS_Key_Null,
	[OS_MAC_kVK_Help]                = OS_Key_Null,
	[OS_MAC_kVK_Home]                = OS_Key_Null,
	[OS_MAC_kVK_PageUp]              = OS_Key_Null,
	[OS_MAC_kVK_ForwardDelete]       = OS_Key_Null,
	[OS_MAC_kVK_F4]                  = OS_Key_F4,
	[OS_MAC_kVK_End]                 = OS_Key_Null,
	[OS_MAC_kVK_F2]                  = OS_Key_F2,
	[OS_MAC_kVK_PageDown]            = OS_Key_Null,
	[OS_MAC_kVK_F1]                  = OS_Key_F1,
	[OS_MAC_kVK_LeftArrow]           = OS_Key_Left,
	[OS_MAC_kVK_RightArrow]          = OS_Key_Right,
	[OS_MAC_kVK_DownArrow]           = OS_Key_Down,
	[OS_MAC_kVK_UpArrow]             = OS_Key_Up,
};

internal void os_gfx_init(void)
{
	{
		Arena *arena = arena_default;
		os_mac_gfx_state = push_array(arena, OS_MAC_GFX_State, 1);
		os_mac_gfx_state->arena = arena;
	}
	os_mac_gfx_state->app = [MyApp sharedApplication];
	[os_mac_gfx_state->app setActivationPolicy:NSApplicationActivationPolicyRegular];
	[os_mac_gfx_state->app finishLaunching];

	@autoreleasepool
	{
		NSMenu *main_menu = [[NSMenu alloc] init];
		NSMenuItem *main_menu_app_item = [main_menu addItemWithTitle: @"Appname" action:0 keyEquivalent:@""];
		NSMenu *app_menu = [[NSMenu alloc] init];
		NSMenuItem *quit_item = [[NSMenuItem alloc] initWithTitle: @"Quit"
			action:@selector(quit_render_loop:)
			keyEquivalent:@"q"];
		[app_menu addItem: quit_item];
		[main_menu_app_item setSubmenu: app_menu];

		[os_mac_gfx_state->app setMainMenu: main_menu];
	}

	{
		NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
		NSSize window_size = {800, 600};
		NSPoint window_origin = {
			((screen_rect.size.width - window_size.width) / 2),
			((screen_rect.size.height - window_size.height) / 2)
		};
		os_mac_gfx_state->window = [[MyWindow alloc] initWithContentRect:
			(NSRect) {window_origin, window_size}
			styleMask: NSWindowStyleMaskClosable | NSWindowStyleMaskTitled
			backing:NSBackingStoreBuffered
			defer:NO ];

		[os_mac_gfx_state->window setTitle:@"Window title"];
		[os_mac_gfx_state->window setIsVisible:YES];
	}
	[os_mac_gfx_state->app activateIgnoringOtherApps:YES];

	[os_mac_gfx_state->window setOpaque:YES];
	[os_mac_gfx_state->window setBackgroundColor:0];
}

internal OS_EventList os_gfx_get_events(Arena *arena)
{
	OS_EventList result = zero_struct;
	@autoreleasepool
		{
		NSEvent *ns_event = [os_mac_gfx_state->window nextEventMatchingMask:NSEventMaskAny
			untilDate:[NSDate distantFuture]
			inMode:NSDefaultRunLoopMode
			dequeue:YES];
		for (; ns_event != 0; ns_event = [os_mac_gfx_state->window
				nextEventMatchingMask:NSEventMaskAny
				untilDate:0
				inMode:NSDefaultRunLoopMode
				dequeue:YES])
		{
			NSEventType ns_event_type = [ns_event type];
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
					OS_Key os_key = os_mac_kvk_table[[ns_event keyCode]];
					OS_Event *os_event = os_eventlist_push_new(arena, &result);
					if (os_key != OS_Key_Null)
					{
						os_event->key = os_key;
						os_event->kind = ns_event_type == NSEventTypeKeyDown
									? OS_EventKind_Press
									: OS_EventKind_Release;
						NSEventModifierFlags ns_mods = [ns_event modifierFlags];
						if (ns_mods & NSEventModifierFlagShift)
							os_event->modifiers |= OS_Modifier_Shift;
						if (ns_mods & NSEventModifierFlagControl)
							os_event->modifiers |= OS_Modifier_Ctrl;
						if (ns_mods & NSEventModifierFlagOption)
							os_event->modifiers |= OS_Modifier_Alt;
					}
				} break;
			}

			[os_mac_gfx_state->app sendEvent:ns_event];

			// HACK(beau): this is terrible
			if (os_mac_gfx_state->private_command_q_should_quit_flag)
			{
				OS_Event *quitev = os_eventlist_push_new(arena, &result);
				quitev->kind = OS_EventKind_Quit;
				os_mac_gfx_state->private_command_q_should_quit_flag = 0;
			}
		}
	}
	return result;
}
