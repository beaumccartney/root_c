#if !BUILD_SUPPLEMENTARY_UNIT
	global R_METAL_State *r_metal_state = 0;
#endif

internal void r_init(void)
{
	Arena *arena         = arena_default;
	r_metal_state        = push_array(arena, R_METAL_State, 1);
	r_metal_state->arena = arena;

	r_metal_state->device    = MTLCreateSystemDefaultDevice();
	r_metal_state->swapchain = CAMetalLayer.layer;

	r_metal_state->swapchain.device          = r_metal_state->device;
	r_metal_state->swapchain.pixelFormat     = MTLPixelFormatRGBA8Unorm_sRGB;
	r_metal_state->swapchain.framebufferOnly = YES;
	r_metal_state->swapchain.frame           = os_mac_gfx_state->window.frame;

	{
		NSView *view    = os_mac_gfx_state->window.contentView;
		view.wantsLayer = YES;
		view.layer      = r_metal_state->swapchain;
	}

	r_metal_state->command_queue = r_metal_state->device.newCommandQueue;
}
