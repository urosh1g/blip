#include <glb_loader.h>
#include <logger/logger.h>
#include <stdio.h>
#include <string.h>

int main() {
    log_info("testing glb_loader...");
    
    model_t *m=model_load("assets/ToyCar.glb");
    log_error("vert count%d",m->meshes->elems[0].primitives->elems[0].vertices->count);
    log_error("vert count%d",m->meshes->elems[0].primitives->elems[0].indices->count);
    model_destroy(m);
    free(m);

}
