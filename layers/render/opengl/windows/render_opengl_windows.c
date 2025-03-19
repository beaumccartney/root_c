internal R_InitReceipt r_init(OS_GFX_InitReceipt gfxinit)
{
	g_r_opengl_windows_state.hdc = GetDC(g_os_gfx_windows_state.window);
	PIXELFORMATDESCRIPTOR desiredpixelformat = {
		.nSize           = sizeof(desiredpixelformat),
		.nVersion        = 1,
		.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.iPixelType      = PFD_TYPE_RGBA,
		.cColorBits      = 32, // REVIEW: including or excluding alpha color bits?
		.cAlphaBits      = 8,
		.cDepthBits      = 24,
		.cStencilBits    = 8,
		// .cAccumBits      = ,
		// .cAuxBuffers     = ,
		.iLayerType      = PFD_MAIN_PLANE,
	};
	int suggestedpixelformatindex = ChoosePixelFormat(g_r_opengl_windows_state.hdc, &desiredpixelformat);
	Assert(suggestedpixelformatindex);

	PIXELFORMATDESCRIPTOR suggestedpixelformat;
	Assert(DescribePixelFormat(
		g_r_opengl_windows_state.hdc,
		suggestedpixelformatindex,
		sizeof(suggestedpixelformat),
		&suggestedpixelformat
	));

	Assert(SetPixelFormat(g_r_opengl_windows_state.hdc, suggestedpixelformatindex, &suggestedpixelformat));

	HGLRC openglrc = wglCreateContext(g_r_opengl_windows_state.hdc);
	Assert(wglMakeCurrent(g_r_opengl_windows_state.hdc, openglrc));

	return (R_InitReceipt) {};
}
