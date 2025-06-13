#ifndef _blip_glb_loader
#define _blip_glb_loader

#include <stdint.h>
#include <stdbool.h>
#include <datastructs/dynarr.h>
#include <datastructs/hashtable.h>
#include <glad/glad.h>
#include <cglm/cglm.h>
typedef enum bufferView_target_t {
    ARRAY_BUFFER = 34962,
    ELEMENT_ARRAY_BUFFER
} bufferView_target_t;

typedef enum render_mode_t {
    RENDERMODE_POINTS,
    RENDERMODE_LINES,
    RENDERMODE_LINE_LOOP,
    RENDERMODE_LINE_STRIP,
    RENDERMODE_TRIANGLES,
    RENDERMODE_TRIANGLE_STRIP,
    RENDERMODE_TRIANGLE_FAN,
    RENDERMODE_DEFAULT = 4
} render_mode_t;

typedef enum componentType_t {
    BYTE = 5120,
    UNSIGNED_BYTE,
    SHORT,
    UNSIGNED_SHORT,
    UNSIGNED_INT = 5125,
    FLOAT
} componentType_t;

typedef enum type_t {
    SCALAR = 1,
    VEC2,
    VEC3,
    VEC4,
    MAT2 = 4,
    MAT3 = 9,
    MAT4 = 16
} type_t;

typedef struct gltfnode_t{
	char* name;
	uint32_t* mesh;
	mat4 matrix;
	char* children;
}gltfnode_t;

typedef struct gltfscene_t{
	char* nodes; 	
} gltfscene_t;


typedef struct gltfbuff_t {
    uint32_t byteLength;
} gltfbuff_t;

typedef struct bufferView_t {
    uint32_t buffer;     // required
    uint32_t byteLength; // requred
    uint32_t byteOffset; // default 0
    uint32_t target;
} bufferView_t;

typedef struct accessor_t {
    uint32_t index;
    uint32_t count;                // required
    componentType_t componentType; // required
    type_t type;                   // required
    uint32_t bufferView;
    uint32_t byteOffset;
} accessor_t;

htable_define_for(uint32_t, attributes);

typedef struct gltfprimitive_t {
    render_mode_t mode;
    htable_attributes_t attributes;
} gltfprimitive_t;

dynarr_define_for(gltfprimitive_t, gltfprimitive);

typedef struct gltfmesh_t {
    dynarr_gltfprimitive_t primitives;
} gltfmesh_t;

typedef struct gltf_t {
    char* meshes;
    char* accessors;
    char* bufferViews;
    char* buffers;
    char* scene;
    char* scenes;
    char* nodes;
} gltf_t;

typedef struct chunk_t {
    uint32_t chunkLength;
    char* chunkType;
    char* chunkData;
} chunk_t;
dynarr_define_for(chunk_t, chunk);

typedef struct glb_t {
    uint32_t magic;
    uint32_t version;
    uint32_t length;
    dynarr_chunk_t chunks;
    uint32_t chunks_count;
} glb_t;

dynarr_define_for(gltfmesh_t, gltfmesh);
dynarr_define_for(accessor_t, accessor);
dynarr_define_for(gltfbuff_t, gltfbuff);
dynarr_define_for(bufferView_t, bufferView);
dynarr_define_for(gltfnode_t, gltfnode);
dynarr_define_for(gltfscene_t, gltfscene);

dynarr_gltfnode_t* gltfnodes_parse(char* chunk);
dynarr_gltfscene_t* gltfscenes_parse(char* chunk);

dynarr_gltfbuff_t* gltfbuffs_parse(char* chunk);
dynarr_bufferView_t* bufferViews_parse(char* chunk);
dynarr_accessor_t* accessors_parse(char* chunk);
dynarr_gltfmesh_t* meshes_parse(char* chunk);
gltf_t* gltf_parse(char* chunkData);
glb_t* glb_parse(char* filename);

typedef struct geometry_data_t{
	void *data;
	uint32_t count;
	uint32_t component_type;
	uint32_t component_size;
	GLenum GL_component_type;
} geometry_data_t;

typedef struct primitive_t{
	geometry_data_t *vertices;
	geometry_data_t *indices;
	GLenum rendermode;
}primitive_t;

dynarr_define_for(primitive_t, primitive);

typedef struct mesh_t{
	dynarr_primitive_t* primitives;	
}mesh_t;
dynarr_define_for(mesh_t, mesh);

typedef struct model_t{
	dynarr_mesh_t* meshes;
}model_t;
void model_destroy(model_t* m);
model_t* model_load(char* filename);
#endif
