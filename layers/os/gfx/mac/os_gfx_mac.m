#if !BUILD_SUPPLEMENTARY_UNIT
	global OS_MAC_GFX_State *os_mac_gfx_state = 0;
#endif

@implementation MyApp : NSApplication
- (void)quit_render_loop:(id) sender {
	os_mac_gfx_state->private_command_q_should_quit_flag = 1;
}
@end

@implementation MyWindow : NSWindow
- (void)keyDown:(NSEvent *) event {}
@end


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
		NSEvent *ns_event = [os_mac_gfx_state->app nextEventMatchingMask:NSEventMaskAny
			untilDate:[NSDate distantFuture]
			inMode:NSDefaultRunLoopMode
			dequeue:YES];
		for (; ns_event != 0; ns_event = [os_mac_gfx_state->app
				nextEventMatchingMask:NSEventMaskAny
				untilDate:0
				inMode:NSDefaultRunLoopMode
				dequeue:YES])
		{
			// TODO(beau): populate and push OS_Event
			switch ([ns_event type])
			{
				default: break;
				case NSEventTypeKeyDown:
				case NSEventTypeKeyUp:
				{
				} break;
			}
			[os_mac_gfx_state->app sendEvent:ns_event];
		}

	}
	return result;
}
