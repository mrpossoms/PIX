#ifndef PIX_IMP_OPENGL_GLSL_PARSER
#define PIX_IMP_OPENGL_GLSL_PARSER

enum PixGLSLParamType{
	PIX_GLSL_ATTRIB,
	PIX_GLSL_UNI,
};

struct PixGLSLParameter{
	char                  name[64];
	unsigned int          bytes;
	unsigned int          elements;
	enum PixGLSLParamType type;
};

struct PixGLSLParseState{
	// variables for scanning the source
	const char*  src;
	unsigned int srcIndex;

	struct PixGLSLParameter  attributes[64];
	unsigned int             attributeCount;

	struct PixGLSLParameter* uniforms[64];
	unsigned int             uniformCount;
};

int PixGLSLParseSource(
	struct PixGLSLParseState* state,
	const char* src
);

#endif