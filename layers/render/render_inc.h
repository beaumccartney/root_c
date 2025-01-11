#ifndef RENDER_INC_H
#define RENDER_INC_H

#define R_BACKEND_METAL 1

#if !defined(R_BACKEND) && OS_MAC
	#define R_BACKEND R_BACKEND_METAL
#endif

#include "render_core.h"

#if R_BACKEND == R_BACKEND_METAL
	#include "metal/render_metal.h"
#endif

#endif // RENDER_INC_H
