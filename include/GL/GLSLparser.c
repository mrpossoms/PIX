/******************************************************************************
	GLSLparser.c
	Kirk Roerig

	This source file provides functionality to parse OpenGL ES 2.0 shader
	programs and can automatically produce a set of vertex attributes. These
	attributes can then be bound more easily bound for rendering geometry.
******************************************************************************/
	
#include "GLSLparser.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

// #define PIX_GLSL_DEBUG

#ifdef PIX_GLSL_DEBUG
#define LOG(str, ...) printf((str), __VA_ARGS__)
#else
#define LOG(str, ...)
#endif

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
// : varying

// Base type
// | Type name |[ Dimension ]

// Data type
// | Base type |
 
// Statement
// | Decl type |[ Precision ]| data type || identifier |[ Array dimensions ];


char* _glsl_genNextToken(struct PixGLSLParseState* state)
{
	char* str = NULL, *out;
	if(!state->parsingBegan){
		str = state->src;
		state->parsingBegan = 1;
	}

	out = strtok(str, " \t\n[]");

	LOG("Token: '%s' %x\n", out, (unsigned int)out);

	return out;
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
		LOG("It's a vector!\n");
		param->width    = 1;
		param->height   = _glsl_char2elements(token[prefixLen]);
	}
	else if(i == 5){ // MATRIX
		LOG("It's a matrix\n");
		int suffixLen = strlen(token) - prefixLen;

		if(suffixLen > 1){
			param->width  = _glsl_char2elements(token[prefixLen]);
			param->height = _glsl_char2elements(token[prefixLen + 2]);			
		}
		else{
			param->height = param->width = _glsl_char2elements(token[prefixLen]);
		}

		LOG("Dims %dx%d\n", param->width, param->height);

	}
	else{
		LOG("It's a scalar\n");
		param->width = param->height = 1;
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
	if(token[tokenLen - 1] == ';'){
		memcpy(current->name, token, tokenLen - 1);
		LOG("identifier: '%s'\n", current->name);
		return 0;
	}
	else{ // it's probably an array!
		memcpy(current->name, token, tokenLen - 1);
	}

	// the statement hasn't been terminated in the identifier
	// an array declaration must have taken place
	do{
		token = _glsl_genNextToken(state);
		if(!token) break;
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

	if(!token || !strcmp(token, "void")){
		LOG("Token is null, or 'void'! done parsing!\n");

		return PIX_GLSL_MSG_DONE_PARSING;
	}

	if(!hasDeclType){
		LOG("GETTING DECL TYPE\n");
		if(!strcmp(token, "attribute")) current->type = PIX_GLSL_ATTRIB;
		if(!strcmp(token, "uniform"))   current->type = PIX_GLSL_UNI;
		if(!strcmp(token, "varying"))   current->type = PIX_GLSL_VARYING;

		if(current->type == PIX_GLSL_NONE){
			return PIX_GLSL_MSG_DONE_PARSING;
		}
	}
	else if(!hasPrecision && !hasDataType){
		int i = 0;

		LOG("GETTING PRECISION\n");

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
		LOG("GETTING DATA TYPE\n");
		int ret = _glsl_parseDataType(token, current);
		if(ret) return ret;
	}
	else if(!hasIdentifier){
		LOG("GETTING IDENTIFIER\n");
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
	int i;

	if(param->type == PIX_GLSL_ATTRIB){
		LOG("attribute ");
	}
	else if(param->type == PIX_GLSL_UNI){
		LOG("uniform ");
	}
	else{
		return;
	}

	switch(param->dataType){
		case PIX_GLSL_FLOAT:
			LOG("float");
			break;
		case PIX_GLSL_INT:
			LOG("int");
			break;
		case PIX_GLSL_UINT:
			LOG("uint");
			break;
	}

	LOG("%dx%d ", param->width, param->height);

	LOG("%s", param->name);

	for(i = 0; i < 3; ++i){
		if(!param->arrayLens[i]) break;
		LOG("[%d]", param->arrayLens[i]);
	}

	LOG("\n");
}

int PixGLSLParseSource(
	struct PixGLSLParseState* state,
	const char* src)
{
	int ret = 0;

	assert(state->attributeCount == 0);
	assert(state->uniformCount   == 0);

	state->src          = malloc(strlen(src) + 1);
	state->parsingBegan = 0;

	strcpy(state->src, src);
	_glsl_initParameter(&state->currentParameter);

	LOG("Initialized ready to go!\n");

	do
	{
		ret = _glsl_parseDecl(state);

		if(ret == PIX_GLSL_MSG_DONE_PARSING_DECL || ret == PIX_GLSL_MSG_DONE_PARSING){
			LOG("Finished parsing decl\n");
			PixGLSLPrintParam(&state->currentParameter);
			LOG("\n");

			int dims = 1;
			struct PixGLSLParameter* current = &state->currentParameter;
			current->arrayLens[0] = !current->arrayLens[0] ? 1 : current->arrayLens[0];
			for(int i = 0; i < 3 && current->arrayLens[i]; i++)
				dims *= current->arrayLens[i];
			current->bytes = current->width * current->height * dims * 4;

			if((*current).type == PIX_GLSL_ATTRIB){
				LOG("Parsed an attribute, current count %u\n", state->attributeCount);
				state->attributes[state->attributeCount++] = (*current);
			}
			else if((*current).type == PIX_GLSL_UNI){
				LOG("Parsed a uniforms\n");
				state->uniforms[state->uniformCount++] = (*current);
			}

			// we're done parsing the param, reset
			_glsl_initParameter(&state->currentParameter);
		}
	}
	while(ret != PIX_GLSL_MSG_DONE_PARSING);

	free(state->src);

	return 0;
}