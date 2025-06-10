#include <glb_loader.h>
#include <logger/logger.h>
#include <stdio.h>
#include <string.h>
int main(){
	glb_t *glb;
	log_info("testing glb_loader...");
	glb_parse("assets/Duck.glb",&glb);
	if(strcmp((glb->chunks).elems[0].chunkType,"JSON")!=0){
		log_error("first chunk not JSON");
		return -1;
	}
	
	log_debug((glb->chunks).elems[0].chunkData);
	gltf_t *gltf;
	gltf_parse((glb->chunks).elems[0].chunkData, &gltf);
	dynarr_mesh_t *meshes;
	meshes_parse(gltf->meshes, &meshes);
	
	
	
	free(meshes);
	free(gltf);
	free(glb);
	return 0;
}
