#ifndef OS_GFX_H
#define OS_GFX_H

typedef enum
{
	OS_Key_Null,

	// NOTE(beau): ascii encoded

	OS_Key_Backspace         = 8,               // '\b'
	OS_Key_Tab               = 9,               // '\t'

	OS_Key_Return            = 13,              // '\n'
	OS_Key_Enter             = OS_Key_Return,

	OS_Key_Esc               = 27,              // '\e'
	OS_Key_Escape            = OS_Key_Esc,      // '\e'

	OS_Key_Space             = 32,              // ' '

	OS_Key_Quote             = 39,              // '\''

	OS_Key_Comma             = 44,              // ','
	OS_Key_Minus,
	OS_Key_Hyphen            = OS_Key_Minus,
	OS_Key_Period,
	OS_Key_Slash,
	OS_Key_Forwardslash      = OS_Key_Slash,

	OS_Key_0                 = 48,              // '0'
	OS_Key_1,
	OS_Key_2,
	OS_Key_3,
	OS_Key_4,
	OS_Key_5,
	OS_Key_6,
	OS_Key_7,
	OS_Key_8,
	OS_Key_9,

	OS_Key_Semicolon         = 59,              // ';'
	OS_Key_Equals            = 61,              // '= ',

	OS_Key_A                 = 65,              // 'a'
	OS_Key_B,
	OS_Key_C,
	OS_Key_D,
	OS_Key_E,
	OS_Key_F,
	OS_Key_G,
	OS_Key_H,
	OS_Key_I,
	OS_Key_J,
	OS_Key_K,
	OS_Key_L,
	OS_Key_M,
	OS_Key_N,
	OS_Key_O,
	OS_Key_P,
	OS_Key_Q,
	OS_Key_R,
	OS_Key_S,
	OS_Key_T,
	OS_Key_U,
	OS_Key_V,
	OS_Key_W,
	OS_Key_X,
	OS_Key_Y,
	OS_Key_Z,
	OS_Key_LBracket,
	OS_Key_LeftBracket       = OS_Key_LBracket,
	OS_Key_Backslash,
	OS_Key_RBracket,
	OS_Key_RightBracket      = OS_Key_RBracket,

	OS_Key_Backtick          = 96,              // '`'
	OS_Key_GraveAccent      = OS_Key_Backtick,

	// NOTE(beau): END ascii encoded

	OS_Key_LShift,
	OS_Key_LeftShift         = OS_Key_LShift,
	OS_Key_RShift,
	OS_Key_RightShift        = OS_Key_RShift,
	OS_Key_LControl,
	OS_Key_LeftControl       = OS_Key_LControl,
	OS_Key_RControl,
	OS_Key_RightControl      = OS_Key_RControl,
	OS_Key_LAlt,
	OS_Key_LeftAlt           = OS_Key_LAlt,
	OS_Key_RAlt,
	OS_Key_RightAlt          = OS_Key_RAlt,

	OS_Key_Right,
	OS_Key_Left,
	OS_Key_Down,
	OS_Key_Up,

	OS_Key_F1,
	OS_Key_F2,
	OS_Key_F3,
	OS_Key_F4,
	OS_Key_F5,
	OS_Key_F6,
	OS_Key_F7,
	OS_Key_F8,
	OS_Key_F9,
	OS_Key_F10,
	OS_Key_F11,
	OS_Key_F12,

	OS_Key_M1,
	OS_Key_LeftMouseButton   = OS_Key_M1,
	OS_Key_M2,
	OS_Key_RightMouseButton  = OS_Key_M2,
	OS_Key_M3,
	OS_Key_MiddleMouseButton = OS_Key_M3,
	OS_Key_M4,
	OS_Key_M5,

	OS_Key_COUNT, // REVIEW(beau): cause keys are trying to line up with ascii where possible this isn't actually the count. Perhaps metaprogram could generate this?
} OS_Key;

// REVIEW(beau): left and right?
typedef U32 OS_Modifiers;
enum
{
	OS_Modifier_Shift = (1<<0),
	OS_Modifier_Ctrl  = (1<<1),
	OS_Modifier_Alt   = (1<<2),
};

typedef enum
{
	OS_EventKind_Null,
	OS_EventKind_Press,
	OS_EventKind_Release,
	Os_EventKind_MouseMove,
	OS_EventKind_Quit,
	OS_EventKind_COUNT,
} OS_EventKind;

typedef struct OS_Event OS_Event;
struct OS_Event
{
	U64 timestamp_us;
	OS_EventKind kind;
	OS_Modifiers modifiers;
	OS_Key key;
	Vec2F32 pos;
	Side side;
};

typedef struct OS_EventNode OS_EventNode;
struct OS_EventNode
{
	OS_EventNode *next, *prev;
	OS_Event event;
};

typedef struct OS_EventList OS_EventList;
struct OS_EventList
{
	OS_EventNode *first, *last;
	U64 count;
};

internal void os_gfx_init(void);

internal OS_EventList os_gfx_get_events(Arena *arena);

internal OS_Event *os_eventlist_push_new(Arena *arena, OS_EventList *list);

#endif // OS_GFX_H
