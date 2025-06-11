#include <glb_loader.h>
#include <logger/logger.h>
#include <stdio.h>
#include <string.h>

int main(){
	log_info("testing glb_loader...");
	float *vertices;
	uint32_t *indices;
	GLenum mode;
	model_load("assets/cube.glb", &vertices, &indices, &mode);
}
