#include <GLFW/glfw3.h>

static int _pixGLSLshaderLink(GLuint shader)
{
    GLint status;
    glLinkProgram(shader);
    
    GLint logLength;
    glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(shader, logLength, &logLength, log);
        printf("Program link log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(shader, GL_LINK_STATUS, &status);
    if (status == 0) {
        return -1;
    }

    printf("Shader program %d compiled!\n", shader);

    return 0;
}

static int _pixGLSLshaderStatus(GLuint shader, const char* path)
{
	GLint status;
	GLint logLength;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = (GLchar *)malloc(logLength);
		glGetShaderInfoLog(shader, logLength, &logLength, log);
		printf("'%s' compile log:\n%s\n", path, log);
		free(log);
	}

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
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
	const size_t valSize = sizeof(PixShaderVal);
	struct PixGLSLParseState vertexState   = {};
	struct PixGLSLParseState fragmentState = {};
	int vertRet = PixGLSLParseSource(&vertexState, src[0]);
	int fragRet = PixGLSLParseSource(&fragmentState, src[1]);

	// clear out the shader structure
	bzero(shader, sizeof (struct PixShader));

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
		if((ret = _pixGLSLshaderStatus(vert, "Vertex")) == GL_FALSE){
			_pixGLSLabort(shader, vert, frag);
			printf("%s\n", src[0]);
			printf("Vertex shader: Aborting compilation of program\n");
			return ret;
		}

		glCompileShader(frag);
		if((ret = _pixGLSLshaderStatus(frag, "Fragment")) == GL_FALSE){
			_pixGLSLabort(shader, vert, frag);
			printf("%s\n", src[1]);
			printf("Fragment shader: Aborting compilation of program\n");
			return ret;
		}
		
		// Everything is ok at this point set up the attributes
		for(unsigned int i = 0; i < shader->attributeCount; ++i){
			struct PixGLSLParameter attr = shader->attributes[i];

			// make sure it exists
			GLint loc = glGetAttribLocation(shader->program, attr.name);
			if(loc == -1){
				printf("program: %d\nShader count: %d\n", shader->program, shader->attributeCount);
				printf("Attribute: '%s' not found\n", attr.name);
				assert(loc >=0);
				continue;
			}

			// bind the location
			glBindAttribLocation(shader->program, i, attr.name);
		}

		// link the program
		if((ret = _pixGLSLshaderLink(shader->program))){
			_pixGLSLabort(shader, vert, frag);
			return ret;
		}

		// clean up
		glDeleteShader(vert);
		glDeleteShader(frag);
	}

	return ret;
}