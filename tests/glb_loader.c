#include <glb_loader.h>
#include <logger/logger.h>
#include <stdio.h>
#include <string.h>

int main() {
    log_info("testing glb_loader...");
    float* vertices;
    uint32_t* indices;
    GLenum mode;
    model_load("assets/Duck.glb", &vertices, &indices, &mode);
    char* bb=malloc(1000);
    (void)bb;
    free(vertices);
    free(indices);
}
