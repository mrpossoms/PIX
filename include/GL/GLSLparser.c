#include "GLSLparser.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static const char* GLSL_TYPE_NAMES[] = {
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

	"sampler",
};

static const enum PixGLSLPrimitiveType GLSL_TYPE_DATA_TYPES[] = {
	PIX_GLSL_FLOAT, //"vec",
	PIX_GLSL_FLOAT, //"dvec",
	PIX_GLSL_INT,   //"ivec",
	PIX_GLSL_UINT,  //"uvec",
	PIX_GLSL_INT,   //"bvec",

	PIX_GLSL_FLOAT, //"mat",

	PIX_GLSL_FLOAT, //"float",
	PIX_GLSL_FLOAT, //"double",
	PIX_GLSL_INT,   //"bool",
	PIX_GLSL_INT,   //"int",
	PIX_GLSL_UINT,  //"uint",

	PIX_GLSL_INT,   //"sampler",
};

static const char* GLSL_PRECISIONS[] = {
	"lowp",
	"highp",
	"mediump",
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
// | Base type |
 
// Statement
// | Decl type |[ Precision ]| data type || identifier |[ Array dimensions ];


char* _glsl_genNextToken(struct PixGLSLParseState* state)
{
	char* str = NULL;
	if(!state->parsingBegan){
		str = state->src;
		state->parsingBegan = 1;
	}

	return strtok(str, " \t\n[]");
}

int _glsl_char2elements(char c)
{
	switch(c){
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		default:
			return 0;
	}
}

int _glsl_parseDataType(char* token, struct PixGLSLParameter* param)
{
	int i, prefixLen = 0, noPrefix = 1;

	// determine the prefix of the type
	for(i = 0; i < sizeof(GLSL_TYPE_NAMES) / sizeof(char*); ++i){
		prefixLen = strlen(GLSL_TYPE_NAMES[i]);

		if(!strncmp(GLSL_TYPE_NAMES[i], token, prefixLen)){
			noPrefix = 0;
			break; // we found a corresponding type prefix
		}
	}

	param->dataType = GLSL_TYPE_DATA_TYPES[i];

	// a type is being defined that will have a dimension term
	if(i < 5){ // VECTOR
		param->width    = 1;
		param->height   = _glsl_char2elements(token[prefixLen]);
	}
	else if(i == 5){ // MATRIX
		param->width    = _glsl_char2elements(token[prefixLen]);
		param->height   = _glsl_char2elements(token[prefixLen + 2]);
	}
	else{
		return 0;
	}

	// make sure elements were returned
	if(!param->width * param->height){
		return PIX_GLSL_MSG_ERR_BAD_DIMENSION;
	}

	return 0;
}

int _glsl_parseIdentifier(struct PixGLSLParseState* state, char* token)
{
	struct PixGLSLParameter* current = &state->currentParameter;	
	int i = 0, tokenLen = strlen(token);

	// non array?
	if(token[tokenLen] == ';'){
		memcpy(current->name, token, tokenLen);
		return 0;
	}

	// the statement hasn't been terminated in the identifier
	// an array declaration must have taken place
	do{
		token = _glsl_genNextToken(state);
		sscanf(token, "%d", &current->arrayLens[i++]);
		if(i >= 3) return PIX_GLSL_MSG_ERR_ARRAY_TOO_MANY_DIMS;
	}while(token[0] != ';');

	return 0;
}

int _glsl_parseDecl(struct PixGLSLParseState* state)
{
	struct PixGLSLParameter* current = &state->currentParameter;
	int hasDeclType   = current->type;
	int hasPrecision  = current->precision;
	int hasDataType   = current->width * current->height;
	int hasIdentifier = strlen(current->name);

	if(hasIdentifier) return PIX_GLSL_MSG_DONE_PARSING_DECL;

	char* token = _glsl_genNextToken(state);

	if(!token) return PIX_GLSL_MSG_DONE_PARSING;

	if(!hasDeclType){
		if(!strcmp(token, "attribute")) current->type = PIX_GLSL_ATTRIB;
		if(!strcmp(token, "uniform"))   current->type = PIX_GLSL_UNI;

		if(current->type == PIX_GLSL_NONE){
			return PIX_GLSL_MSG_DONE_PARSING;
		}
	}
	else if(!hasPrecision && !hasDataType){
		int i = 0;

		// iterate over all precicion strings, try to find one that matches
		current->precision = PIX_GLSL_NOP; // otherwise, indicate that none was set
		for(i = 0; i < sizeof(GLSL_PRECISIONS) / sizeof(char*); ++i){
			if(!strcmp(token, GLSL_PRECISIONS[i])){
				current->precision = (enum PixGLSLPrecision)i;
				break;
			}
		}
	}
	else if(!hasDataType){
		int ret = _glsl_parseDataType(token, current);
		if(ret) return ret;
	}
	else if(!hasIdentifier){
		int ret = _glsl_parseIdentifier(state, token);
		if(ret) return ret;
	}

	return 0;
}

void _glsl_initParameter(struct PixGLSLParameter* param)
{
	bzero(param, sizeof(struct PixGLSLParameter));
	param->precision = PIX_GLSL_UNKNOWNP;
}

void PixGLSLPrintParam(struct PixGLSLParameter* param)
{
	printf("Parameter - '%s'\n\tSize: %d\n\tDims: %d x %d\n\tData Type: %d", 
		param->name,
		param->bytes,
		param->width,
		param->height,
		param->dataType
	);
}

int PixGLSLParseSource(
	struct PixGLSLParseState* state,
	const char* src)
{
	int ret = 0;

	state->src          = malloc(strlen(src) + 1);
	state->parsingBegan = 0;

	strcpy(state->src, src);
	_glsl_initParameter(&state->currentParameter);

	do
	{
		ret = _glsl_parseDecl(state);

		if(ret == PIX_GLSL_MSG_DONE_PARSING_DECL){
			if(state->currentParameter.type == PIX_GLSL_ATTRIB){
				state->attributes[state->attributeCount++] = state->currentParameter;
			}
			else if(state->currentParameter.type == PIX_GLSL_UNI){
				state->uniforms[state->uniformCount++] = state->currentParameter;
			}
		}
	}
	while(ret != PIX_GLSL_MSG_DONE_PARSING);

	free(state->src);

	return 0;
}