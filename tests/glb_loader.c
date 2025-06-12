#include <glb_loader.h>
#include <logger/logger.h>
#include <stdio.h>
#include <string.h>

int main() {
    log_info("testing glb_loader...");
    
    primitive_t *p=model_load("assets/Duck.glb");
    (void) p;

}
