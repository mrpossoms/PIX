#include "GLSLparser.h"

static const char* GLSL_TYPE_NAMES = {
	// vectors are suffixed with a number
	// to indicate number of elements
	"vec",
	"dvec",
	"ivec",
	"uvec",
	"bvec",

	// matricies are suffixed with a number
	// to indicate a square matrix of a given
	// side length, or explicit dimensions nxm
	"mat",

	"float",
	"double",
	"bool",
	"int",
	"uint",
};

// Side
// : 2
// : 3
// : 4

// Width x Height
// | Side |x| Side |

// Dimension
// (Width x Height | Side )

// Precision
// : lowp
// : mediump
// : highp

// Decl type
// : attribute
// : uniform

// Base type
// | Type name |[ Dimension ]

// Data type
// | Base type |[ Array dimensions ]
 
// Statement
// | Decl type |[ Precision ]| data type || identifier |;


char* _glsl_genNextToken(struct PixGLSLParseState* state)
{

}

int PixGLSLParseSource(
	struct PixGLSLParseState* state,
	const char* src)
{
	state->src      = src;
	state->srcIndex = 0;



	return 0;
}