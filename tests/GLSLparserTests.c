#include "helper.h"
#include <GL/GLSLparser.h>
#include <stdarg.h>

TEST_INIT

static const char* realProgram = "attribute vec4 position;\n\nuniform mat4 MVP;\nuniform lowp vec4 color;\n\nvarying lowp vec4  tintColor;\n\nvoid main()\n{\n    gl_PointSize = 150.0;\n    gl_Position = MVP * position;\n    \n    tintColor = color;// * opacity;\n}";

//-----------------------------------------------------------------------------
//    _______        _     ______                      
//   |__   __|      | |   |  ____|                     
//      | | ___  ___| |_  | |__ _   _ _ __   ___ ___   
//      | |/ _ \/ __| __| |  __| | | | '_ \ / __/ __|  
//      | |  __/\__ \ |_  | |  | |_| | | | | (__\__ \_ 
//      |_|\___||___/\__| |_|   \__,_|_| |_|\___|___(_)                                               
int shouldReturnSingleAttribute(void* params)
{
	struct PixGLSLParseState state = {};
	PixGLSLParseSource(&state, "attribute vec3 aPosition;");
	return 0;
}

int shouldReturnSingleAttributeArray(void* params)
{
	struct PixGLSLParseState state = {};
	PixGLSLParseSource(&state, "attribute vec3 aPosition;\nattribute   float  aWeights[10];");
	return 0;
}

int shouldReturnSingleUniform(void* params)
{
	struct PixGLSLParseState state = {};
	PixGLSLParseSource(&state, "uniform mat3x2 VP;\nvoid main(void){ }");
	return 0;
}

int shouldParseParametersOfProgram(void* params)
{
	struct PixGLSLParseState state = {};
	PixGLSLParseSource(&state, realProgram);
	
	if(state.attributeCount != 1){
		return -1;
	}

	if(state.uniformCount != 2){
		return -2;
	}

	return 0;
}

/*
//-----------------------------------------------------------------------------
//    _______        _     _____                             
//   |__   __|      | |   |  __ \                            
//      | | ___  ___| |_  | |__) |   _ _ __  _ __   ___ _ __ 
//      | |/ _ \/ __| __| |  _  / | | | '_ \| '_ \ / _ \ '__|
//      | |  __/\__ \ |_  | | \ \ |_| | | | | | | |  __/ |   
//      |_|\___||___/\__| |_|  \_\__,_|_| |_|_| |_|\___|_|   
*/
int main(void){
	int result = 0;

	// Run sequence of test function invocations here
	result |= TEST(shouldReturnSingleAttribute, "Parse Single Attribute", NULL);
	result |= TEST(shouldReturnSingleAttributeArray, "Parse Array", NULL);
	result |= TEST(shouldReturnSingleUniform, "Parse Uniform", NULL);
	result |= TEST(shouldParseParametersOfProgram, "Parse Program", NULL);

	return result; // value to be read by test script
}