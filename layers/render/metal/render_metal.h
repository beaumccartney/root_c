#ifndef RENDER_METAL_H
#define RENDER_METAL_H

#include <Metal/Metal.h>
#include <QuartzCore/QuartzCore.h>

typedef struct R_METAL_State R_METAL_State;
struct R_METAL_State
{
	Arena *arena;
	id<MTLDevice> device;
	CAMetalLayer *swapchain; // REVIEW(beau): use an MTKView instead?
	id<MTLCommandQueue> command_queue;
};

#endif // RENDER_METAL_H
