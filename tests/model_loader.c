#include <gltf_loader.h>
#include <stdio.h>
int main(){
	glb_t glb;
	(void) glb;
	gltf_parse("assets/cube.glb");
	printf("hi from model_loading.o\n");
	return 0;
}
