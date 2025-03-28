#ifndef OS_GFX_H
#define OS_GFX_H

// REVIEW(beau): left and right?
typedef U32 OS_Modifiers;
enum
{
	OS_Modifier_Shift    = (1<<0),
	OS_Modifier_Ctrl     = (1<<1),
	OS_Modifier_Alt      = (1<<2),
	OS_Modifier_CapsLock = (1<<3),
	OS_Modifier_M1       = (1<<4),
	OS_Modifier_M2       = (1<<5),
	OS_Modifier_M3       = (1<<6),
	OS_Modifier_M4       = (1<<7),
	OS_Modifier_M5       = (1<<8),
};

typedef enum
{
	OS_EventKind_NULL,
	OS_EventKind_Press,
	OS_EventKind_Release,
	OS_EventKind_MouseMove,
	OS_EventKind_WindowClose,
	OS_EventKind_Quit,
	OS_EventKind_COUNT,
} OS_EventKind;

typedef struct OS_Window OS_Window;
struct OS_Window
{
	U64 bits;
	S64 generation;
};

typedef struct OS_Event OS_Event;
struct OS_Event
{
	S64 timestamp_us;
	OS_Window window;
	OS_EventKind kind;
	OS_Modifiers modifiers;
	OS_Key key;
	Vec2F32 pos;
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

// typesafety for other layers
typedef struct { Byte ignoreme; } OS_GFX_InitReceipt;

internal OS_GFX_InitReceipt os_gfx_init(void);

internal OS_Window os_window_open(Vec2S32 resolution, String8 title);
internal void      os_window_close(OS_Window window);

internal OS_EventList os_get_events(Arena *arena);

internal OS_Event* os_eventlist_push_new(Arena *arena, OS_EventList *list, OS_EventKind kind);

#endif // OS_GFX_H
