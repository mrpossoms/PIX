#ifndef PIX_SHADER
#define PIX_SHADER

#define PIX_GRAPHICS_API_GL

#ifdef PIX_GRAPHICS_API_GL
#include "GL/GLSLParser.h"
#endif

struct PixShader{
	int           program;
	PixShaderVal* parameters;
	unsigned int  parameterCount;
	PixShaderVal* attributes;
	unsigned int  attributeCount;
	unsigned int  attributeSizes;
};

int pixShaderFromSource(const char* src[], struct PixShader* shader);

#endif