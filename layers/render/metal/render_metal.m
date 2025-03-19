internal R_InitReceipt r_init(OS_GFX_InitReceipt gfxinit)
{
	g_r_metal_state.device        = MTLCreateSystemDefaultDevice();
	g_r_metal_state.command_queue = g_r_metal_state.device.newCommandQueue;

	return (R_InitReceipt) {};
}

internal void r_window_equip(OS_Window os_window)
{
	OS_MAC_Window *macwindow                   = os_mac_window_from_handle(os_window);
	macwindow->nswindow.contentView.wantsLayer = YES;
	CAMetalLayer *metallayer                   = [CAMetalLayer layer];
	macwindow->nswindow.contentView.layer      = metallayer;
	metallayer.device                          = g_r_metal_state.device;
	metallayer.pixelFormat                     = MTLPixelFormatBGRA8Unorm_sRGB;
	metallayer.framebufferOnly                 = YES;
	metallayer.frame                           = macwindow->nswindow.frame;
}

internal void r_window_unequip(OS_Window os_window)
{
	OS_MAC_Window *macwindow = os_mac_window_from_handle(os_window);
	if (macwindow)
	{
		CAMetalLayer *layer = (CAMetalLayer*)macwindow->nswindow.contentView.layer;
		if (layer)
		{
			[(CAMetalLayer*)macwindow->nswindow.contentView.layer release];
			macwindow->nswindow.contentView.wantsLayer = NO;
			macwindow->nswindow.contentView.layer      = 0;
		}
	}
}
