#ifndef OS_GFX_MAC_H
#define OS_GFX_MAC_H

#pragma push_macro("global")
#undef global
#include <AppKit/AppKit.h>
#pragma pop_macro("global")

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
	Arena *arena;
	MyWindow *window;
	B8 private_command_q_should_quit_flag; // HACK(beau): this is terrible
};

// NOTE(beau): from some events thing in Carbon.framework
typedef enum
{
	OS_MAC_kVK_ANSI_A              = 0x00,
	OS_MAC_kVK_ANSI_S              = 0x01,
	OS_MAC_kVK_ANSI_D              = 0x02,
	OS_MAC_kVK_ANSI_F              = 0x03,
	OS_MAC_kVK_ANSI_H              = 0x04,
	OS_MAC_kVK_ANSI_G              = 0x05,
	OS_MAC_kVK_ANSI_Z              = 0x06,
	OS_MAC_kVK_ANSI_X              = 0x07,
	OS_MAC_kVK_ANSI_C              = 0x08,
	OS_MAC_kVK_ANSI_V              = 0x09,
	OS_MAC_kVK_ANSI_B              = 0x0B,
	OS_MAC_kVK_ANSI_Q              = 0x0C,
	OS_MAC_kVK_ANSI_W              = 0x0D,
	OS_MAC_kVK_ANSI_E              = 0x0E,
	OS_MAC_kVK_ANSI_R              = 0x0F,
	OS_MAC_kVK_ANSI_Y              = 0x10,
	OS_MAC_kVK_ANSI_T              = 0x11,
	OS_MAC_kVK_ANSI_1              = 0x12,
	OS_MAC_kVK_ANSI_2              = 0x13,
	OS_MAC_kVK_ANSI_3              = 0x14,
	OS_MAC_kVK_ANSI_4              = 0x15,
	OS_MAC_kVK_ANSI_6              = 0x16,
	OS_MAC_kVK_ANSI_5              = 0x17,
	OS_MAC_kVK_ANSI_Equal          = 0x18,
	OS_MAC_kVK_ANSI_9              = 0x19,
	OS_MAC_kVK_ANSI_7              = 0x1A,
	OS_MAC_kVK_ANSI_Minus          = 0x1B,
	OS_MAC_kVK_ANSI_8              = 0x1C,
	OS_MAC_kVK_ANSI_0              = 0x1D,
	OS_MAC_kVK_ANSI_RightBracket   = 0x1E,
	OS_MAC_kVK_ANSI_O              = 0x1F,
	OS_MAC_kVK_ANSI_U              = 0x20,
	OS_MAC_kVK_ANSI_LeftBracket    = 0x21,
	OS_MAC_kVK_ANSI_I              = 0x22,
	OS_MAC_kVK_ANSI_P              = 0x23,
	OS_MAC_kVK_Return              = 0x24,
	OS_MAC_kVK_ANSI_L              = 0x25,
	OS_MAC_kVK_ANSI_J              = 0x26,
	OS_MAC_kVK_ANSI_Quote          = 0x27,
	OS_MAC_kVK_ANSI_K              = 0x28,
	OS_MAC_kVK_ANSI_Semicolon      = 0x29,
	OS_MAC_kVK_ANSI_Backslash      = 0x2A,
	OS_MAC_kVK_ANSI_Comma          = 0x2B,
	OS_MAC_kVK_ANSI_Slash          = 0x2C,
	OS_MAC_kVK_ANSI_N              = 0x2D,
	OS_MAC_kVK_ANSI_M              = 0x2E,
	OS_MAC_kVK_ANSI_Period         = 0x2F,
	OS_MAC_kVK_Tab                 = 0x30,
	OS_MAC_kVK_Space               = 0x31,
	OS_MAC_kVK_ANSI_Grave          = 0x32,
	OS_MAC_kVK_Delete              = 0x33,
	OS_MAC_kVK_Escape              = 0x35,
	OS_MAC_kVK_RightCommand        = 0x36,
	OS_MAC_kVK_Command             = 0x37,
	OS_MAC_kVK_Shift               = 0x38,
	OS_MAC_kVK_CapsLock            = 0x39,
	OS_MAC_kVK_Option              = 0x3A,
	OS_MAC_kVK_Control             = 0x3B,
	OS_MAC_kVK_RightShift          = 0x3C,
	OS_MAC_kVK_RightOption         = 0x3D,
	OS_MAC_kVK_RightControl        = 0x3E,
	OS_MAC_kVK_Function            = 0x3F,
	OS_MAC_kVK_F17                 = 0x40,
	OS_MAC_kVK_ANSI_KeypadDecimal  = 0x41,
	OS_MAC_kVK_ANSI_KeypadMultiply = 0x43,
	OS_MAC_kVK_ANSI_KeypadPlus     = 0x45,
	OS_MAC_kVK_ANSI_KeypadClear    = 0x47,
	OS_MAC_kVK_VolumeUp            = 0x48,
	OS_MAC_kVK_VolumeDown          = 0x49,
	OS_MAC_kVK_Mute                = 0x4A,
	OS_MAC_kVK_ANSI_KeypadDivide   = 0x4B,
	OS_MAC_kVK_ANSI_KeypadEnter    = 0x4C,
	OS_MAC_kVK_ANSI_KeypadMinus    = 0x4E,
	OS_MAC_kVK_F18                 = 0x4F,
	OS_MAC_kVK_F19                 = 0x50,
	OS_MAC_kVK_ANSI_KeypadEquals   = 0x51,
	OS_MAC_kVK_ANSI_Keypad0        = 0x52,
	OS_MAC_kVK_ANSI_Keypad1        = 0x53,
	OS_MAC_kVK_ANSI_Keypad2        = 0x54,
	OS_MAC_kVK_ANSI_Keypad3        = 0x55,
	OS_MAC_kVK_ANSI_Keypad4        = 0x56,
	OS_MAC_kVK_ANSI_Keypad5        = 0x57,
	OS_MAC_kVK_ANSI_Keypad6        = 0x58,
	OS_MAC_kVK_ANSI_Keypad7        = 0x59,
	OS_MAC_kVK_F20                 = 0x5A,
	OS_MAC_kVK_ANSI_Keypad8        = 0x5B,
	OS_MAC_kVK_ANSI_Keypad9        = 0x5C,
	OS_MAC_kVK_F5                  = 0x60,
	OS_MAC_kVK_F6                  = 0x61,
	OS_MAC_kVK_F7                  = 0x62,
	OS_MAC_kVK_F3                  = 0x63,
	OS_MAC_kVK_F8                  = 0x64,
	OS_MAC_kVK_F9                  = 0x65,
	OS_MAC_kVK_F11                 = 0x67,
	OS_MAC_kVK_F13                 = 0x69,
	OS_MAC_kVK_F16                 = 0x6A,
	OS_MAC_kVK_F14                 = 0x6B,
	OS_MAC_kVK_F10                 = 0x6D,
	OS_MAC_kVK_ContextualMenu      = 0x6E,
	OS_MAC_kVK_F12                 = 0x6F,
	OS_MAC_kVK_F15                 = 0x71,
	OS_MAC_kVK_Help                = 0x72,
	OS_MAC_kVK_Home                = 0x73,
	OS_MAC_kVK_PageUp              = 0x74,
	OS_MAC_kVK_ForwardDelete       = 0x75,
	OS_MAC_kVK_F4                  = 0x76,
	OS_MAC_kVK_End                 = 0x77,
	OS_MAC_kVK_F2                  = 0x78,
	OS_MAC_kVK_PageDown            = 0x79,
	OS_MAC_kVK_F1                  = 0x7A,
	OS_MAC_kVK_LeftArrow           = 0x7B,
	OS_MAC_kVK_RightArrow          = 0x7C,
	OS_MAC_kVK_DownArrow           = 0x7D,
	OS_MAC_kVK_UpArrow             = 0x7E,
	OS_MAC_kVK_COUNT,
}
OS_GFX_Mac_kVK;

#endif // OS_GFX_MAC_H
