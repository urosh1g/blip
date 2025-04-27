#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


GLFWwindow* init_glad_glfw(){
	if(!glfwInit()){
		fprintf(stderr,"Failed to init glfw\n");
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"New window",NULL,NULL);
	if(!window){
		glfwTerminate();
		fprintf(stderr,"Failed to create window\n");
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
		
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		fprintf(stderr,"Failed to init glad\n");
		exit(EXIT_FAILURE);
	}
	return window;
}

GLFWwindow* window_create_and_make_current(){
}

void main(){
	GLFWwindow* window=init_glad_glfw();
	
	while(!glfwWindowShouldClose(window)){	
		glClear(GL_COLOR_BUFFER_BIT);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}
