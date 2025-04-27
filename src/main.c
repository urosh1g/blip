#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    (void)window;
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    (void)window;
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
	//add necessary key checks here
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods){
    (void)window;
    (void)button;
    (void)action;
    (void)mods;
	//add necessary mouse checks here
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    (void)window;
    (void)xoffset;
    (void)yoffset;
	//add necessary onscroll actions here
}

void error_callback(int error_code, const char* description){
	fprintf(stderr,"glfw error code: %d\n",error_code);
	fprintf(stderr,"%s\n",description);
	exit(EXIT_FAILURE);
}

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
	
	glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window,mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetErrorCallback(error_callback);
	
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		glfwDestroyWindow(window);
		glfwTerminate();
		fprintf(stderr,"Failed to init glad\n");
		exit(EXIT_FAILURE);
	}
	return window;
}

int main(){
	GLFWwindow* window=init_glad_glfw();
	
	while(!glfwWindowShouldClose(window)){	
		glClear(GL_COLOR_BUFFER_BIT);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

    return 0;
}
