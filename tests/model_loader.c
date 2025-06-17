
#include <model_loader/model_loader.h>
#include <logger/logger.h>
#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h>
int main() {
    log_info("testing model_loader...");
  
    glfwInit(); 
    GLFWwindow* window = glfwCreateWindow(640, 480, "Test", NULL, NULL);
    glfwMakeContextCurrent(window); 
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    model_t *m=model_load("assets/cube.glb");
    model_get_VAOs(m);
    log_info("Vertices count:%d",m->meshes->elems[0].primitives->elems[0].vertices->count);
    log_info(" Indices count:%d",m->meshes->elems[0].primitives->elems[0].indices->count);
    model_destroy(m);
    free(m);

}
