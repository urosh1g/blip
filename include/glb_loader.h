#ifndef _blip_glb_laoder
#define _blip_glb_loader

#include <stdint.h>
#include <stdbool.h>
typedef struct gltf_t{
  char *meshes;
  char *accessors;
  char *bufferViews;
} gltf_t;

typedef struct accessor_t {
  char *index;
  char *count;			//required
  char *componentType;		//required
  char *type;			//required
  char *bufferView;
  char *byteOffset;
} accessor_t;

typedef struct bufferView_t {
  char *index;
  char *byteOffset;
  char *byteLength;
} bufferView_t;

typedef struct chunk_t{
	uint32_t chunkLength;
	uint32_t chunkType;
	char *chunkData;
} chunk_t;

typedef struct glb_t{
	uint32_t magic;
	uint32_t version;
	uint32_t length;
	chunk_t *chunks;
	uint32_t chunks_count;
} glb_t;

bool glb_parse(char *filename);
#endif
