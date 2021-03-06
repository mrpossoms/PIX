#ifndef PIX_IMP_OPENGL_GLSL_PARSER
#define PIX_IMP_OPENGL_GLSL_PARSER

#define PIX_GLSL_MSG_DONE_PARSING            -1
#define PIX_GLSL_MSG_ERR_BAD_DIMENSION       -2
#define PIX_GLSL_MSG_ERR_ARRAY_TOO_MANY_DIMS -3
#define PIX_GLSL_MSG_DONE_PARSING_DECL       -4

enum PixGLSLParamType{
	PIX_GLSL_NONE = 0,
	PIX_GLSL_ATTRIB,
	PIX_GLSL_UNI,
	PIX_GLSL_VARYING,
};

enum PixGLSLPrecision{
	PIX_GLSL_UNKNOWNP = -2,
	PIX_GLSL_NOP      = -1,
	PIX_GLSL_LOWP     =  0,
	PIX_GLSL_HIGHP    =  1,
	PIX_GLSL_MEDIUMP  =  2,
};

enum PixGLSLPrimitiveType{
	PIX_GLSL_FLOAT,
	PIX_GLSL_INT,
	PIX_GLSL_UINT,
};

struct PixGLSLParameter{
	char                      name[64];
	unsigned int              bytes;
	unsigned int              width, height;
	unsigned int              arrayLens[3]; // dimensions in an array
	int                       location;
	enum PixGLSLPrimitiveType dataType;
	enum PixGLSLPrecision     precision;
	enum PixGLSLParamType     type;
	void (*assign)(int loc, ...);
};

struct PixGLSLParseState{
	// variables for scanning the source
	char*  src;
	unsigned int parsingBegan;

	unsigned int             attributeCount;
	struct PixGLSLParameter  attributes[64];

	unsigned int             uniformCount;
	struct PixGLSLParameter  uniforms[64];

	struct PixGLSLParameter  currentParameter;
};

void PixGLSLPrintParam(struct PixGLSLParameter* param);
int PixGLSLParseSource(
	struct PixGLSLParseState* state,
	const char* src
);
void PixGLSLcleanUp(struct PixGLSLParseState* state);

#endif