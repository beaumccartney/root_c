#ifndef RENDER_H
#define RENDER_H

typedef struct R_InitReceipt {} R_InitReceipt;

internal R_InitReceipt r_init(OS_GFX_InitReceipt gfx_init);

internal void r_window_equip(OS_Window os_window);
internal void r_window_unequip(OS_Window os_window);

internal void r_window_close(OS_Window os_window);

#endif // RENDER_H
