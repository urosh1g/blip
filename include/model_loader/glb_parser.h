#ifndef _blip_glb_parser
#define _blip_glb_parser

#include <stdint.h>
#include <stdbool.h>
#include <datastructs/dynarr.h>
#include <datastructs/hashtable.h>
#include <glad/glad.h>
#include <cglm/cglm.h>

dynarr_define_for(uint32_t, uint32);

typedef enum type_t {
    SCALAR = 1,
    VEC2,
    VEC3,
    VEC4,
    MAT2 = 4,
    MAT3 = 9,
    MAT4 = 16
} type_t;

typedef struct gltfnode_t {
    char* name;
    uint32_t* mesh;
    mat4* matrix;
    dynarr_uint32_t* children;
} gltfnode_t;

typedef struct gltfscene_t {
    dynarr_uint32_t* nodes;
} gltfscene_t;

typedef struct gltfbuff_t {
    uint32_t byteLength;
} gltfbuff_t;

typedef struct bufferView_t {
    uint32_t buffer;     // required
    uint32_t byteLength; // requred
    uint32_t byteOffset; // default 0
    uint32_t byteStride; // default 0
    uint32_t target;
} bufferView_t;

typedef struct accessor_t {
    uint32_t index;
    uint32_t count;       // required
    GLenum componentType; // required
    type_t type;          // required
    uint32_t bufferView;
    uint32_t byteOffset;
    bool normalized;
} accessor_t;

htable_define_for(uint32_t, attributes);

typedef struct gltfprimitive_t {
    GLenum mode;
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
    uint32_t default_scene;
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

void gltfnode_destroy(gltfnode_t* n);
void gltfscene_destroy(gltfscene_t* s);
void gltfmesh_destroy(gltfmesh_t* m);
void gltf_destroy(gltf_t* gltf);
void glb_destroy(glb_t* glb);
#endif
