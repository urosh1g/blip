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
	dynarr_accessor_t *accessors;
	accessors_parse(gltf->accessors,&accessors);	
	dynarr_bufferView_t *bufferViews;
	bufferViews_parse(gltf->bufferViews,&bufferViews);	
	dynarr_gltfbuff_t *buffs;
	gltfbuffs_parse(gltf->buffers,&buffs);



	//free buffs
	dynarr_gltfbuff_destroy(buffs);
	free(buffs);


	//free bufferViews
	dynarr_bufferView_destroy(bufferViews);
	free(bufferViews);

	//free accessors
	dynarr_accessor_destroy(accessors);
	free(accessors);	

	//free meshes
	for(size_t i=0;i<meshes->length;i++){
		mesh_destroy(&meshes->elems[i]);
	}
	dynarr_mesh_destroy(meshes);
	free(meshes);
	
	//free gltf
	gltf_destroy(gltf);
	free(gltf);

	//free glb
	glb_destroy(glb);
	free(glb);
	return 0;
}
