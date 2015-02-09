#include "pix.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

GLFWwindow* PIX_GLFW_WINDOW;

int pixInit(struct PixDesc settings)
{
	if (!glfwInit()) exit(EXIT_FAILURE);
	PIX_GLFW_WINDOW = glfwCreateWindow(
		settings.width,
		settings.height,
		settings.name, 
		NULL,
		NULL
	);

	if(!PIX_GLFW_WINDOW){
		pixDestroy();
		return PIX_ERROR_WINDOW_FAILED_TO_OPEN;
	}

    glfwMakeContextCurrent(PIX_GLFW_WINDOW);

	return 0;
}

int  pixShouldBegin()
{
	pixClear();
	return !glfwWindowShouldClose(PIX_GLFW_WINDOW);
}

void pixDestroy()
{
    glfwDestroyWindow(PIX_GLFW_WINDOW);
    glfwTerminate();   
}

void pixPresent()
{
	glfwSwapBuffers(PIX_GLFW_WINDOW);
	glfwPollEvents();
}

void pixClear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
