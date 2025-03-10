#ifndef RENDER_OPENGL_H
#define RENDER_OPENGL_H

#include <GL/gl.h>

#pragma comment(lib, "opengl32.lib")

typedef struct R_OPENGL_WINDOWS_State R_OPENGL_WINDOWS_State;
struct R_OPENGL_WINDOWS_State
{
	HDC hdc;
};

global R_OPENGL_WINDOWS_State g_r_opengl_windows_state = zero_struct;

#endif // RENDER_OPENGL_H
