#ifndef _blip_model_loader
#define _blip_model_loader
#include <model_loader/glb_parser.h>
#include <glad/glad.h>
typedef struct geometry_data_t{
	void *data;
	uint32_t count;
	uint32_t component_type;
	uint32_t component_size;
	bool normalized;
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
	dynarr_gltfscene_t* scenes;
	dynarr_gltfnode_t* nodes;
	uint32_t default_scene;
}model_t;
void model_destroy(model_t* m);
model_t* model_load(char* filename);
uint32_t** model_get_VAOs(model_t* m);
void model_draw(model_t* m, mat4 model, uint32_t model_uniform_id, uint32_t** VAO);
#endif
