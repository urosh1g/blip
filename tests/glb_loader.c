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
	

	//free accessors
	dynarr_accessor_destroy(accessors);
	free(accessors);	

	//free meshes
	for(size_t i=0;i<meshes->length;i++){
		for(size_t j=0;j<meshes->elems[i].primitives.length;j++)
			htable_attributes_destroy(&meshes->elems[i].primitives.elems[j].attributes);
		dynarr_primitive_destroy(&meshes->elems[i].primitives);
	}
	dynarr_mesh_destroy(meshes);
	free(meshes);
	
	//free gltf
	free(gltf->meshes);
	free(gltf->accessors);
	free(gltf->bufferViews);	
	free(gltf);

	//free glb
	for(size_t i=0;i<glb->chunks.length;i++){
		free(glb->chunks.elems[i].chunkData);	
	}
	dynarr_chunk_destroy(&glb->chunks);
	free(glb);
	return 0;
}
