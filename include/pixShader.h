#ifndef PIX_SHADER
#define PIX_SHADER

#define PIX_GRAPHICS_API_GL

#ifdef PIX_GRAPHICS_API_GL
#include "GL/GLSLParser.h"
#endif

struct PixShader{
	void* tag;
	void* params;
};

int pixShaderFromSource(const char* src[], struct PixShader* shader);

#endif