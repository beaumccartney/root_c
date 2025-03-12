#ifndef OS_GFX_MAC_H
#define OS_GFX_MAC_H

#pragma push_macro("global")
#undef global
#include <AppKit/AppKit.h>
#pragma pop_macro("global")

#include <Carbon/Carbon.h>

// NOTE(beau): methods are callbacks for menu items - it was either this or add
// methods to something at runtime
@interface MyApp : NSApplication
- (void)quit_render_loop:(id) sender;
@end

// NOTE(beau): ignore keydown events so they can be caught in nextEventMatchingMask
@interface MyWindow : NSWindow
- (void)keyDown:(NSEvent *) event;
@end

// NOTE(beau): the NSApplication object is stored in the global NSApp so no need
// to store it here
typedef struct OS_MAC_GFX_State OS_MAC_GFX_State;
struct OS_MAC_GFX_State
{
	MyWindow *window;

	// NOTE HACK(beau): should only be used from the event polling thread,
	// by the event polling code
	B8 private_command_q_should_quit_flag; // HACK(beau): this is terrible
};

#endif // OS_GFX_MAC_H
