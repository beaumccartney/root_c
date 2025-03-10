internal void r_init(void)
{
	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };
	DXGI_SWAP_CHAIN_DESC swapchaindesc = {
		.BufferDesc     = {
			.Width  = 0,
			.Height = 0,
			.Format = DXGI_FORMAT_B8G8R8A8_UNORM, // REVIEW: srgb!
		},
		.SampleDesc.Count = 1,
		.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount      = 2,
		.OutputWindow     = g_os_gfx_windows_state.window,
		.Windowed         = TRUE,
		.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD,
	};

	IDXGISwapChain      *swapchain;
	ID3D11Device        *device;
	ID3D11DeviceContext *devicecontext;

	D3D11CreateDeviceAndSwapChain(
		0,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
		featurelevels,
		ArrayCount(featurelevels),
		D3D11_SDK_VERSION,
		&swapchaindesc,
		&swapchain,
		&device,
		0,
		&devicecontext
	);

	IDXGISwapChain_GetDesc(swapchain, &swapchaindesc);

	ID3D11Texture2D *framebuffer;

	IDXGISwapChain_GetBuffer(swapchain, 0, &IID_ID3D11Texture2D, &framebuffer);
}
