#if R_BACKEND == R_BACKEND_D3D11
	#include "d3d11/render_d3d11.c"
#elif R_BACKEND == R_BACKEND_METAL
	#include "metal/render_metal.m"
#elif R_BACKEND == R_BACKEND_OPENGL
	#if OS_WINDOWS
		#include "opengl/windows/render_opengl_windows.c"
	#else
		#error No opengl backend for this OS
	#endif
	#include "opengl/render_opengl_common.c"
#else
	#error no selected render backend
#endif
