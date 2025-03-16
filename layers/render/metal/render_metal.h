#ifndef RENDER_METAL_H
#define RENDER_METAL_H

#include <Metal/Metal.h>
#include <QuartzCore/QuartzCore.h>

typedef struct R_METAL_State R_METAL_State;
struct R_METAL_State
{
	id<MTLDevice> device;
	id<MTLCommandQueue> command_queue;
};

global R_METAL_State g_r_metal_state = zero_struct;

#endif // RENDER_METAL_H
