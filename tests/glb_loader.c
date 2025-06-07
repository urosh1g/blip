#include <glb_loader.h>
#include <stdio.h>
int main(){
	glb_t glb;
	(void) glb;
	glb_parse("assets/cube.glb");
	printf("testing glb_loading...\n");
	return 0;
}
