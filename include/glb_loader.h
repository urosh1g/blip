#ifndef _blip_glb_laoder
#define _blip_glb_loader

#include <stdint.h>
#include <stdbool.h>
#include <datastructs/data_structures.h>

typedef enum render_mode_t{
	POINTS,
	LINES,
	LINE_LOOP,
	LINE_STRIP,
	TRIANGLES,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
	DEFAULT=4
}render_mode_t;

typedef enum componentType_t{
	BYTE=5120,
	UNSIGNED_BYTE,
	SHORT,
	UNSIGNED_SHORT,
	UNSIGNED_INT=5125,
	FLOAT
} componentType_t;

typedef enum type_t{
	SCALAR=1,
	VEC2,
	VEC3,
	VEC4,
	MAT2=4,
	MAT3=9,
	MAT4=16
}type_t;

typedef struct accessor_t {
  /*uint32_t*/ char *index;
  /*uint32_t*/ char *count;				//required
  /*componentType_t*/ char *componentType;		//required
  /*type_t*/ char *type;					//required
  /*uint32_t*/ char *bufferView;
  /*uint32_t*/ char *byteOffset;
} accessor_t;

typedef struct bufferView_t {
  /*uint32_t*/char * index;				//required
  /*uint32_t*/char * byteOffset;				//default 0
  /*uint32_t*/char * byteLength;				//requred
} bufferView_t;


htable_define_for(uint32_t,uint32);

typedef struct primitive_t {
   	render_mode_t mode;
}primitive_t;

dynarr_define_for(primitive_t, primitive);






typedef struct mesh_t{
	dynarr_primitive_t primitives;
}mesh_t;








typedef struct gltf_t{
  char *meshes;
  char *accessors;
  char *bufferViews;
} gltf_t;

typedef struct chunk_t{
	uint32_t chunkLength;
	char *chunkType;
	char *chunkData;

} chunk_t;
dynarr_define_for(chunk_t, chunk);

typedef struct glb_t{
	uint32_t magic;
	uint32_t version;
	uint32_t length;
	dynarr_chunk_t chunks;
	uint32_t chunks_count;
} glb_t;

void meshes_parse(char* meshes);
bool glb_parse(char *filename, glb_t **glb);
bool gltf_parse(char* chunkData, gltf_t **gltf);
#endif
