
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

    model_t* m = model_load("assets/Box.glb");
    GLuint** VAOs = model_get_VAOs(m);

    for (size_t i = 0; i < m->meshes->length; i++) {
        free(VAOs[i]);
    }
    free(VAOs);
    model_destroy(m);
    free(m);
    glfwDestroyWindow(window);
    glfwTerminate();
}
