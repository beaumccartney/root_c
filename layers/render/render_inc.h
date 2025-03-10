#ifndef RENDER_INC_H
#define RENDER_INC_H

#define R_BACKEND_D3D11  1
#define R_BACKEND_OPENGL 2
#define R_BACKEND_METAL  3

#if !defined(R_BACKEND)
	#if OS_WINDOWS
		#define R_BACKEND R_BACKEND_D3D11
	#elif OS_MAC
		#define R_BACKEND R_BACKEND_METAL
	#else
		#error no render backend for this os
	#endif
#endif


#include "render.h"

#if R_BACKEND == R_BACKEND_D3D11
	#include "d3d11/render_d3d11.h"
#elif R_BACKEND == R_BACKEND_METAL
	#include "metal/render_metal.h"
#elif R_BACKEND == R_BACKEND_OPENGL
	#if OS_WINDOWS
		#include "opengl/windows/render_opengl_windows.h"
	#else
		#error No opengl backend for this OS
	#endif
	#include "opengl/render_opengl_common.h"
#else
	#error no selected render backend
#endif

#endif // RENDER_INC_H
