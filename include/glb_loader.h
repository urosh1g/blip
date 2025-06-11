#ifndef _blip_glb_loader
#define _blip_glb_loader

#include <stdint.h>
#include <stdbool.h>
#include <datastructs/data_structures.h>
#include <glad/glad.h>
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

dynarr_define_for(accessor_t, accessor);
htable_define_for(uint32_t, attributes);

typedef struct primitive_t {
    render_mode_t mode;
    htable_attributes_t attributes;
} primitive_t;

dynarr_define_for(primitive_t, primitive);

typedef struct mesh_t {
    dynarr_primitive_t primitives;
} mesh_t;
dynarr_define_for(mesh_t, mesh);

typedef struct gltf_t {
    char* meshes;
    char* accessors;
    char* bufferViews;
    char* buffers;
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

dynarr_define_for(gltfbuff_t, gltfbuff);
dynarr_define_for(bufferView_t, bufferView);

void gltfbuffs_parse(char* chunk, dynarr_gltfbuff_t** buffs);
void bufferViews_parse(char* chunk, dynarr_bufferView_t** buffViews);
void accessors_parse(char* chunk, dynarr_accessor_t** accessors);
void meshes_parse(char* chunk, dynarr_mesh_t** meshes);
bool glb_parse(char* filename, glb_t** glb);
bool gltf_parse(char* chunkData, gltf_t** gltf);

void mesh_destroy(mesh_t* mesh);
void glb_destroy(glb_t* glb);
void gltf_destroy(gltf_t* gltf);
bool model_load(char* filename, float** vertices, uint32_t** indices,
                GLenum* mode);
#endif
