#include <GLFW/glfw3.h>
#include "GLSLparser.h"

#define PIX_GLSL_DEBUG

#ifdef PIX_GLSL_DEBUG
#define LOG(str, ...) printf((str), __VA_ARGS__)
#else
#define LOG(str, ...)
#endif

struct PixGLSLshader{
	struct PixGLSLParameter* parameters;
	struct PixGLSLParameter* attributes;
	int          program;
	unsigned int parameterCount;
	unsigned int attributeCount;
	unsigned int attributeSizes;
};

int PixGLSLshaderFromSource(const char* src[], struct PixGLSLshader** shaderRef);
void PixGLSLconsumeParams(struct PixGLSLshader* shader, void* params);