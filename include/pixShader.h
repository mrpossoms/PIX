#ifndef PIX_SHADER
#define PIX_SHADER

#include <GLFW/glfw3.h>

struct PixShader{
	GLint program;
	unsigned int parameterCount;
	unsigned int attributeCount;
	unsigned int attributeSizes;
};

#endif