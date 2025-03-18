#ifndef OS_GFX_MAC_H
#define OS_GFX_MAC_H

#pragma push_macro("global")
#undef global
#include <AppKit/AppKit.h>
#pragma pop_macro("global")

#include <Carbon/Carbon.h>

// NOTE(beau): methods are callbacks for menu items - it was either this or add
// methods to something at runtime
@interface OS_MAC_NSApplication : NSApplication
- (void)quit_render_loop:(id) sender;
@end

// NOTE(beau): ignore keydown events so they can be caught in nextEventMatchingMask
@interface OS_MAC_NSWindow : NSWindow
- (void)keyDown:(NSEvent *) event;
@end

typedef struct OS_MAC_Window OS_MAC_Window;
struct OS_MAC_Window
{
	OS_MAC_NSWindow *nswindow;
	S64 generation, next_free_plus_one; // plus one to keep 0 free
};

typedef struct OS_MAC_GFX_State OS_MAC_GFX_State;
struct OS_MAC_GFX_State
{
	// windows pool
	OS_MAC_Window windows[64];
	S64 free_plus_one, // plus one to keep 0 free
	   lowest_unused;

	// NOTE HACK(beau): should only be used from the event polling thread,
	// by the event polling code
	B8 private_command_q_should_quit_flag; // HACK(beau): this is terrible
};
global OS_MAC_GFX_State g_os_mac_gfx_state = zero_struct;

inline internal OS_Window os_mac_handle_from_window(OS_MAC_Window *gfxwindow);
inline internal OS_MAC_Window *os_mac_window_from_handle(OS_Window handle);
internal NSString *os_mac_nsstring_from_str8(String8 string8);

#endif // OS_GFX_MAC_H
