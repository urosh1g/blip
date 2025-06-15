#include <model_loader/model_loader.h>
#include <logger/logger.h>
#include <stdio.h>
#include <string.h>

int main() {
    log_info("testing model_loader...");
    
    model_t *m=model_load("assets/Duck.glb");
    log_info("Vertices count:%d",m->meshes->elems[0].primitives->elems[0].vertices->count);
    log_info(" Indices count:%d",m->meshes->elems[0].primitives->elems[0].indices->count);
    model_destroy(m);
    free(m);

}
