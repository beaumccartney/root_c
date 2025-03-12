#if !BUILD_SUPPLEMENTARY_UNIT
	global R_METAL_State g_r_metal_state = zero_struct;
#endif

internal void r_init(void)
{
	Arena *arena           = arena_default;
	g_r_metal_state.arena = arena;

	g_r_metal_state.device    = MTLCreateSystemDefaultDevice();
	g_r_metal_state.swapchain = CAMetalLayer.layer;

	g_r_metal_state.swapchain.device          = g_r_metal_state.device;
	g_r_metal_state.swapchain.pixelFormat     = MTLPixelFormatBGRA8Unorm_sRGB;
	g_r_metal_state.swapchain.framebufferOnly = YES;
	g_r_metal_state.swapchain.frame           = g_os_mac_gfx_state.window.frame;

	{
		NSView *view    = g_os_mac_gfx_state.window.contentView;
		view.wantsLayer = YES;
		view.layer      = g_r_metal_state.swapchain;
	}

	g_r_metal_state.command_queue = g_r_metal_state.device.newCommandQueue;
}
