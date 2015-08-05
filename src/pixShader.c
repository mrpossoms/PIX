#include "pixShader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef PIX_GRAPHICS_API_GL
#include "GL/GLSLshader.h"
#endif

int pixShaderFromSource(const char* src[], struct PixShader* shader)
{
#ifdef PIX_GRAPHICS_API_GL
	return PixGLSLshaderFromSource(src, (struct PixGLSLshader**)&shader->tag);
	
#endif

	return 0;
}