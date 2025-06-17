#include <glad/glad.h>
#include <model_loader/model_loader.h>
#include <logger/logger.h>
void geometry_data_destroy(geometry_data_t* gd){
	free(gd->data);
}

void primitive_destroy(primitive_t* p){
	geometry_data_destroy(p->vertices);
	geometry_data_destroy(p->indices);
	free(p->vertices);
	free(p->indices);
}

void mesh_destroy(mesh_t* m){
	for(size_t i=0;i<m->primitives->length;i++)
	{
		primitive_destroy(&m->primitives->elems[i]);
		free(&m->primitives->elems[i]);
	}
	free(m->primitives);
}

void model_destroy(model_t* m){
	if(m->meshes){
	for(size_t i=0;i<m->meshes->length;i++)
	{
		mesh_destroy(&m->meshes->elems[i]);
	}
	dynarr_mesh_destroy(m->meshes);
	free(m->meshes);
	}
	if(m->scenes){
    	for (size_t i = 0; i < m->scenes->length; i++) {
    	    gltfscene_destroy(&m->scenes->elems[i]);
    	}
    	dynarr_gltfscene_destroy(m->scenes);
    	free(m->scenes);
	}
    	
	if(m->nodes){
	for (size_t i = 0; i < m->nodes->length; i++) {
	 gltfnode_destroy(&m->nodes->elems[i]);
	}
    	dynarr_gltfnode_destroy(m->nodes);
    	free(m->nodes);
	}
}

static geometry_data_t* indices_load(glb_t* glb, gltfprimitive_t* primitive,
                  dynarr_accessor_t* accessors,
                  dynarr_bufferView_t* bufferViews) {
    uint32_t* accessor_pos = htable_attributes_get(
        &primitive->attributes, "indices");
    if (!accessor_pos) {
        log_error("no indices found");
        return NULL;
    }
    accessor_t* a=&accessors->elems[*accessor_pos];
    uint32_t indices_count = a->count;
    GLenum component_type = a->componentType;
    if (component_type != GL_UNSIGNED_INT && component_type != GL_UNSIGNED_SHORT && component_type!=GL_UNSIGNED_BYTE) {
        log_error("indices ComponentType not UNSIGNED_INT");
        return NULL;
    }
    uint32_t component_size = component_type == GL_UNSIGNED_INT ? sizeof(uint32_t) : component_type==GL_UNSIGNED_SHORT?sizeof(uint16_t):sizeof(uint8_t);
    type_t type = a->type;
    uint32_t buffView_ind = a->bufferView;
    bufferView_t* buffView = dynarr_bufferView_get(bufferViews, buffView_ind);
    uint32_t buff_indx = buffView->buffer + 1; // first chunk is gltf
    uint32_t startOffset = buffView->byteOffset;

    uint32_t index = 0;
    uint32_t size = component_size * type;
    
    geometry_data_t *indices=malloc(sizeof(geometry_data_t));
    indices->data = malloc(size * indices_count);
    uint32_t byteStride = buffView->byteStride;
    byteStride=byteStride==0?size:byteStride;
    indices->count=indices_count;
    indices->component_type=component_size;
    indices->component_size=type;
    indices->GL_component_type=component_type; 
    indices->normalized=false;
    char* buff=glb->chunks.elems[buff_indx].chunkData;
    while (index < indices_count) {
        uint32_t offset = startOffset + index * byteStride;
        memcpy(&((char*)indices->data)[index*size],
               &buff[offset], component_size);
	//if(index<100)
	//log_info("offset:%d, i%d=%d",offset, index, ((uint16_t*)indices->data)[index]);
        index++;
    }
    //log_info("indices_count=%d",indices_count);
    return indices;
}

static geometry_data_t* position_load(glb_t* glb, gltfprimitive_t *primitive,
                   dynarr_accessor_t* accessors,
                   dynarr_bufferView_t* bufferViews) {
    uint32_t accessor_pos = *htable_attributes_get(
       & primitive->attributes, "POSITION");
    log_info("POSITION accessor index is %d",accessor_pos);
    accessor_t* a=&accessors->elems[accessor_pos];
    uint32_t vertices_count = a->count;
    GLenum component_type = a->componentType;
    uint32_t a_byteOffset = a->byteOffset;
    if (component_type != GL_FLOAT) {
        log_error("POSITION ComponentType not FLOAT");
        return NULL;
    }
    uint32_t component_size = sizeof(float);
    type_t type = a->type;
    uint32_t vertices_buffView_ind = a->bufferView;
    bufferView_t* vertices_buffView =
        dynarr_bufferView_get(bufferViews, vertices_buffView_ind);
    uint32_t buff_indx = vertices_buffView->buffer + 1; // first chunk is gltf
    uint32_t startOffset = vertices_buffView->byteOffset;

    uint32_t index = 0;
    uint32_t size = component_size * type;
    uint32_t byteStride = vertices_buffView->byteStride;
    byteStride=byteStride==0?size:byteStride;
    geometry_data_t* vertices=malloc(sizeof(geometry_data_t)); 
    vertices->data=malloc(size * vertices_count);
    vertices->count=vertices_count;
    vertices->component_type=component_size;
    vertices->component_size=type;
    vertices->GL_component_type=component_type;
    vertices->normalized=a->normalized; 
    char* buff=glb->chunks.elems[buff_indx].chunkData;
    while (index < vertices_count) {
        uint32_t offset = startOffset + a_byteOffset + index * byteStride ;
        memcpy(&((char*)vertices->data)[index*size],
               &buff[offset], component_size);
        memcpy(&((char*)vertices->data)[index*size + component_size],
               &buff[offset + component_size],
               component_size);
        memcpy(&((char*)vertices->data)[index*size + 2*component_size],
               &buff[offset + 2 * component_size],
               component_size);
        //log_info("%d. x=%f y=%f z=%f", index, ((float*)vertices->data)[3*index],
        //         ((float*)vertices->data)[3*index+1],((float*)vertices->data)[3*index+2]);
       
	index++;
    }
    log_info("vert_count=%d", vertices_count);
    return vertices;
}

model_t* model_load(char* filename) {
    glb_t* glb=glb_parse(filename);
    if(!glb) return false;
    if (strcmp((glb->chunks).elems[0].chunkType, "JSON") != 0) {
        log_error("first chunk not JSON");
        return NULL;
    }

    log_debug((glb->chunks).elems[0].chunkData);
    gltf_t* gltf=gltf_parse((glb->chunks).elems[0].chunkData);
    dynarr_gltfmesh_t* meshes=meshes_parse(gltf->meshes);
    dynarr_accessor_t* accessors=accessors_parse(gltf->accessors);
    dynarr_bufferView_t* bufferViews=bufferViews_parse(gltf->bufferViews);
    dynarr_gltfbuff_t* buffs=gltfbuffs_parse(gltf->buffers);
    dynarr_gltfscene_t* scenes=gltfscenes_parse(gltf->scenes);
    dynarr_gltfnode_t* nodes=gltfnodes_parse(gltf->nodes);
    
    model_t *model=malloc(sizeof(model_t));
    model->scenes=scenes;
    model->nodes=nodes; 
    model->meshes=malloc(sizeof(dynarr_mesh_t));
    model->default_scene=gltf->default_scene;
    dynarr_mesh_init(model->meshes);

    for(size_t i=0;i<meshes->length;i++){
	mesh_t m;	
	m.primitives=malloc(sizeof(dynarr_primitive_t));
    	dynarr_primitive_init(m.primitives);
	for(size_t j=0;j<meshes->elems[i].primitives.length;j++){
    		geometry_data_t* vertices=position_load(glb, &meshes->elems[i].primitives.elems[j], accessors, bufferViews);
    		geometry_data_t* indices =indices_load(glb, &meshes->elems[i].primitives.elems[j], accessors, bufferViews);
    		primitive_t primitive;
    		primitive.vertices=vertices;
    		primitive.indices=indices;
		log_info("mesh:%d,prim:%d,vertcount=%d indcount=%d",i,j,vertices->count,indices->count);
    		primitive.rendermode = meshes->elems[i].primitives.elems[j].mode;
		dynarr_primitive_push(m.primitives,primitive);
	}
	dynarr_mesh_push(model->meshes, m);
    }

    // cleanup
    // free buffs
    dynarr_gltfbuff_destroy(buffs);
    free(buffs);

    // free bufferViews
    dynarr_bufferView_destroy(bufferViews);
    free(bufferViews);

    // free accessors
    dynarr_accessor_destroy(accessors);
    free(accessors);

    // free meshes
    for (size_t i = 0; i < meshes->length; i++) {
        gltfmesh_destroy(&meshes->elems[i]);
    }
    dynarr_gltfmesh_destroy(meshes);
    free(meshes);

    // free gltf
    gltf_destroy(gltf);
    free(gltf);

    // free glb
    glb_destroy(glb);
    free(glb);
    return model;
}


void primitive_draw(uint32_t VAO, primitive_t* p){
   glBindVertexArray(VAO);
   glDrawElements(p->rendermode, p->indices->count, p->indices->GL_component_type, 0);
   glBindVertexArray(0);
} 

void gltfnode_draw(gltfnode_t* root, mat4 model, uint32_t model_uniform_id, model_t* loadedmodel, uint32_t** VAO){
	if(!root) return;
	//glm_mat4_mul(model,*root->matrix, model);
	(void) model;
	dynarr_uint32_t *children=root->children;
	if(children){
		for(size_t ind=0;ind<children->length;ind++){	
			uint32_t c=children->elems[ind];
			gltfnode_t* node=&loadedmodel->nodes->elems[c];
			gltfnode_draw(node, model, model_uniform_id, loadedmodel, VAO);
		}
	}
	if(root->mesh)
	{
		uint32_t mesh_index=*root->mesh;
		size_t primitives_count=loadedmodel->meshes->elems[mesh_index].primitives->length;
		log_info("for mesh_index=%d prim_count is %d",mesh_index, primitives_count);
		for(size_t j=0;j<primitives_count;j++){
   			primitive_t* p = &loadedmodel->meshes->elems[mesh_index].primitives->elems[j];
   			glUniformMatrix4fv(model_uniform_id, 1, GL_FALSE, (const float*)model);
			primitive_draw(VAO[mesh_index][j], p);
		}
	}
	mat4 inv;
	glm_mat4_inv(*root->matrix,inv);
	//glm_mat4_mul(model,inv,model);	
}

void model_draw(model_t* m, mat4 model, uint32_t model_uniform_id, uint32_t** VAO){
        //size_t mesh_count=m->meshes->length;
	gltfscene_t* scene=dynarr_gltfscene_get(m->scenes,m->default_scene);
	size_t rootnodes_count=scene->nodes->length;
	log_info("ROOTNODES_COUNT:%d",rootnodes_count);	
	for(size_t rootnode=0;rootnode<rootnodes_count;rootnode++)
	{
		gltfnode_t* root=&m->nodes->elems[rootnode];
		log_info("%d. %s",rootnode,root->name);
		gltfnode_draw(root, model, model_uniform_id, m, VAO);
	}
}

GLuint** model_get_VAOs(model_t* loadedmodel){
    GLuint mesh_count=loadedmodel->meshes->length;
    GLuint** VAO=malloc(sizeof(GLuint*)*mesh_count);
    for(size_t i=0; i<mesh_count;i++){
	    log_info("i=%d, len=%d",i,mesh_count);
	    size_t primitive_count=loadedmodel->meshes->elems[i].primitives->length; 
	    VAO[i]=malloc(sizeof(GLuint)*primitive_count);
	    glGenVertexArrays(primitive_count, VAO[i]);
    	for(size_t j=0; j<primitive_count;j++){
	    log_info("j=%d, len=%d",j,primitive_count);
	    primitive_t* primitive = &loadedmodel->meshes->elems[i].primitives->elems[j];
	    unsigned int VBO, EBO;
	    glGenBuffers(1, &VBO);
	    glGenBuffers(1, &EBO);
	    glBindVertexArray(VAO[i][j]);
	  
	    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	    glBufferData(GL_ARRAY_BUFFER, primitive->vertices->count * primitive->vertices->component_size * primitive->vertices->component_type, primitive->vertices->data,
	                 GL_STATIC_DRAW);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive->indices->count * primitive->indices->component_size * primitive->indices->component_type, primitive->indices->data, GL_STATIC_DRAW);
	    
	    glVertexAttribPointer(0, primitive->vertices->component_size, primitive->vertices->GL_component_type, primitive->vertices->normalized, 0, 0);
	    glEnableVertexAttribArray(0);
	    
	    log_info("count:%d",primitive->vertices->count);
	    log_info("ctype:%d",primitive->vertices->component_type);
	    log_info("csize:%d",primitive->vertices->component_size);
	    log_info("GLctype:%d",primitive->vertices->GL_component_type);
	    log_info("normalized:%s",primitive->vertices->normalized?"true":"false");
	    for(size_t ind=0;ind<primitive->vertices->count;ind++)
	    	log_info("%f %f %f",((float*)primitive->vertices->data)[3*ind],((float*)primitive->vertices->data)[3*ind+1],((float*)primitive->vertices->data)[3*ind+2]);
	    log_info("count:%d",primitive->indices->count);
	    log_info("ctype:%d",primitive->indices->component_type);
	    log_info("csize:%d",primitive->indices->component_size);
	    log_info("GLctype:%d",primitive->indices->GL_component_type);
	    log_info("normalized:%s",primitive->indices->normalized?"true":"false");
	    for(size_t ind=0;ind<primitive->indices->count;ind++)
	    	log_info("%d",((uint16_t*)primitive->indices->data)[ind]);
	    
	    glBindVertexArray(0);
    	}
    }

    return VAO;
}

