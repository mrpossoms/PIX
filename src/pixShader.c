#include "pixShader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef PIX_GRAPHICS_API_GL
#include "GL/GLSLparser.h"
#include <GLFW/glfw3.h>
#endif

static int _pixGLSLshaderStatus(GLuint shader)
{
	GLint status;
	GLint logLength;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)malloc(logLength);
		glGetShaderInfoLog(shader, logLength, &logLength, log);
		printf("Shader compile log:\n%s\n", log);
		free(log);
	}

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == 0)
	{
		int i;
		
		printf("Failed to compile shader:\n");
		// for (i = 0; i < count; i++)
		// 	printf("%s", sources[i]);	
	}

	return status;
}

static void _pixGLSLabort(struct PixShader* shader, GLuint vert, GLuint frag)
{
	glDeleteProgram(shader->program);

	glDeleteShader(vert);
	glDeleteShader(frag);
}

int pixShaderFromSource(const char* src[], struct PixShader* shader)
{
	int ret = 0;
#ifdef PIX_GRAPHICS_API_GL
	const size_t valSize = sizeof(PixShaderVal);
	struct PixGLSLParseState vertexState   = {};
	struct PixGLSLParseState fragmentState = {};
	int vertRet = PixGLSLParseSource(&vertexState, src[0]);
	int fragRet = PixGLSLParseSource(&fragmentState, src[1]);

	// copy the parameters
	shader->parameters = malloc(valSize * (vertexState.uniformCount + fragmentState.uniformCount));
	memcpy(shader->parameters, vertexState.uniforms, valSize * vertexState.uniformCount);
	memcpy(
		shader->parameters + vertexState.uniformCount,
		fragmentState.uniforms,
		valSize * fragmentState.uniformCount
	);

	// copy the attributes
	shader->attributes = malloc(valSize * vertexState.attributeCount);
	memcpy(shader->attributes, vertexState.attributes, valSize * vertexState.attributeCount);

	// create the program
	shader->program = glCreateProgram();
	glUseProgram(shader->program);

	// compile and link the shaders
	{
		GLint lengths[2] = {
			strlen(src[0]), // vertex
			strlen(src[1])  // fragment
		};

		GLuint prog = shader->program;
		GLuint vert = glCreateShader(GL_VERTEX_SHADER);
		GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

		// attach the vertex and fragment to the program
		glAttachShader(prog, vert);
		glAttachShader(prog, frag);

		// set the source for each
		glShaderSource(vert, 1,	src + 0, lengths + 0);
		glShaderSource(frag, 1,	src + 1, lengths + 1);

		// compile the shaders, and check their status
		glCompileShader(vert);
		if((ret = _pixGLSLshaderStatus(vert))){
			_pixGLSLabort(shader, vert, frag);
			return ret;
		}

		glCompileShader(frag);
		if((ret = _pixGLSLshaderStatus(frag))){
			_pixGLSLabort(shader, vert, frag);
			return ret;
		}
		
		// Everything is ok at this point set up the attributes
		for(unsigned int i = 0; i < shader->attributeCount; ++i){
			struct PixGLSLParameter attr = shader->attributes[i];

			// make sure it exists
			GLint loc = glGetAttribLocation(shader->program, attr.name);
			if(loc == -1){
				printf("Attribute: '%s' not found\n", attr.name);
				continue;
			}

			// bind the location
			glBindAttribLocation(shader->program, i, attr.name);
		}

		// clean up
		glDeleteShader(vert);
		glDeleteShader(frag);
	}

	return 0;
#endif
	
}
