#include <model_loader/glb_parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logger/logger.h>
static char* extract_section(char* start_tag, char* chunk,
                     char* field_name) {
    char* start = strstr(chunk, start_tag);
    if (!start)
        return NULL;
    uint32_t parenthesis = 1;
    if (start_tag[0] != '{' && start_tag[0] != '[') {
        start = strstr(start, "\":");
        start += 2;
    }
    char* p = start;
    while (strlen(p) > 0 && parenthesis != 0) {
        p++;
        if (*p == '{' || *p == '[')
            parenthesis++;
        if (*p == '}' || *p == ']')
            parenthesis--;
    }
    if (parenthesis != 0)
        return NULL;
    char* result = malloc(p - start + 2);
    strncpy(result, start, p - start + 1);
    result[p - start + 1] = '\0';
    (void)field_name;
    log_debug("found %s\n%s",field_name,result);
    return result;
}

static char* extract_section_between_tags(char* start_tag, char* end_tag,
                                  char* chunk, char* field_name) {
    char* start = strstr(chunk, start_tag);
    if (!start) {
        log_debug("Couldn't find %s", start_tag);
        return NULL;
    }
    char* end = strstr(start, end_tag);
    if (!end) {
        end = strstr(start, "}");
    }
    char* result = malloc(end - start + 2);
    strncpy(result, start, end - start + 1);
    result[end - start + 1] = '\0';
    (void)field_name;
    //log_debug("found %s\n%s",field_name,result);
    return result;
}

static char* extract_field_value(char* startstr, char* chunk,
                         char* field_name) {
    char* dummy=extract_section_between_tags(startstr, ",", chunk, field_name);
    if (!dummy)
        return NULL;
    char* start = strchr(dummy, ':');
    if (!start)
        return NULL;
    start++;
    char* end = strchr(start, ',');
    if(!end)
    end = strstr(start, "}");
    if (!end)
        return NULL;
    char *subchunk = malloc(end - start + 1);
    strncpy(subchunk, start, end - start);
    subchunk[end - start] = '\0';
    log_debug("%s=%s", field_name, subchunk);
    free(dummy);
    return subchunk;
}

static gltfnode_t* gltfnode_parse(char* node_s) {
    char* name=extract_field_value("name", node_s, "node.name");
    char* mesh=extract_field_value("mesh", node_s, "node.mesh");
    char* translation=extract_section("translation", node_s, "node.translation");
    char* rotation=extract_section("rotation", node_s, "node.rotation");
    char* scale=extract_section("scale", node_s, "node.scale");
    char* matrix=extract_section("matrix", node_s, "node.matrix");
    char* children=extract_section("children", node_s, "node.children");
    
    mat4 m;
    if(matrix)
    {
	float gltfm[16];
	uint32_t fields_read=sscanf(matrix,"[%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]",
			&gltfm[0],&gltfm[1],&gltfm[2],&gltfm[3],
			&gltfm[4],&gltfm[5],&gltfm[6],&gltfm[7],
			&gltfm[8],&gltfm[9],&gltfm[10],&gltfm[11],
			&gltfm[12],&gltfm[13],&gltfm[14],&gltfm[15]);
	if(fields_read!=16)
		log_error("Couldn't read matrix");
    	glm_mat4_make(gltfm,m);
	//glm_mat4_transpose_to(m,m);
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++)
			log_info("m[%d,%d]=%f",i,j,m[i][j]);
	}
	free(matrix);
    }
    
    else
    {
	glm_mat4_identity(m);
    }
    
    if(translation){
    	vec3 vec;
    	sscanf(translation,"[%f,%f,%f]",&vec[0],&vec[1],&vec[2]);
	glm_translate(m,vec);
    	free(translation);
    }
    if(rotation){
    	versor quat;
    	sscanf(rotation,"[%f,%f,%f,%f]",&quat[0],&quat[1],&quat[2],&quat[3]);
	glm_quat_rotate(m,quat,m);
    	free(rotation);
    }
    if(scale){
    	vec3 vec;
    	sscanf(scale,"[%f,%f,%f]",&vec[0],&vec[1],&vec[2]);
	glm_scale(m,vec);
    	free(scale);
    }
    
    gltfnode_t* n=malloc(sizeof(gltfnode_t));
    n->name=name;
    n->matrix=malloc(sizeof(mat4));
    glm_mat4_copy(m,*n->matrix);
    
    if(mesh){
    	    n->mesh=malloc(sizeof(uint32_t));
    	    *n->mesh=atoi(mesh);
   	    free(mesh);
    }
    
    if(children && strlen(children)>0){
    	n->children=malloc(sizeof(dynarr_uint32_t));	
    	dynarr_uint32_init(n->children);
    	char* node=strtok(children,"[");
    	node=strtok(node,"]");
    	node=strtok(node,",");
   
    	while(node){
		//log_error("-------------------node%s",node);
    		dynarr_uint32_push(n->children,atoi(node));
		node=strtok(NULL,",");
    	}
	free(children);
    }
    else
	    n->children=NULL;
    return n;
}

dynarr_gltfnode_t* gltfnodes_parse(char* nodes_s) {
    dynarr_gltfnode_t *nodes = malloc(sizeof(dynarr_gltfnode_t));
    dynarr_gltfnode_init(nodes);

    uint32_t i = 0, offset = 0;
    uint32_t max_length = strlen(nodes_s) - 2;
    log_info("Parsing nodes...");
    while (offset + i < max_length) {
        char* gltfnode_s=extract_section("{",&nodes_s[offset], "gltfnode");

        gltfnode_t* n = gltfnode_parse(gltfnode_s);
        dynarr_gltfnode_push(nodes, *n);

        offset += strlen(gltfnode_s);
        i++;
        free(gltfnode_s);
	free(n);
    }
    return nodes;
}

static gltfscene_t* gltfscene_parse(char* scene_s) {
    char* nodes_s=extract_section("nodes", scene_s, "scene.nodes");
    
    char* node=strtok(nodes_s,"[");
    node=strtok(node,"]");
    node=strtok(node,",");
    
    gltfscene_t *s=malloc(sizeof(gltfscene_t));
    if(!node){
    	s->nodes=NULL;
	free(nodes_s);
	return s;
    }
    dynarr_uint32_t *nodes=malloc(sizeof(dynarr_uint32_t));
    dynarr_uint32_init(nodes);
    while(node){
    	dynarr_uint32_push(nodes,atoi(node));
	node=strtok(NULL,",");
    }
    s->nodes=nodes;
    free(nodes_s);
    return s;
}

dynarr_gltfscene_t* gltfscenes_parse(char* scenes_s) {
    dynarr_gltfscene_t *scenes = malloc(sizeof(dynarr_gltfscene_t));
    dynarr_gltfscene_init(scenes);

    uint32_t i = 0, offset = 0;
    uint32_t max_length = strlen(scenes_s) - 2;
    log_info("Parsing scenes...");
    while (offset + i < max_length) {
        char* gltfscene_s=extract_section("{",&scenes_s[offset], "gltfscene");

        gltfscene_t* s = gltfscene_parse(gltfscene_s);
        dynarr_gltfscene_push(scenes, *s);
	
        offset += strlen(gltfscene_s);
        i++;
        free(gltfscene_s);
	free(s);
    }
    return scenes;
}

static gltfbuff_t gltfbuff_parse(char* buff_s) {
    char* byteLength=extract_field_value("byteLength", buff_s, "buffer.byteLength");
    
    gltfbuff_t b;
    b.byteLength = atoi(byteLength);
    free(byteLength); 
    return b;
}

dynarr_gltfbuff_t* gltfbuffs_parse(char* gltfbuffs_s) {
    dynarr_gltfbuff_t *buffs = malloc(sizeof(dynarr_gltfbuff_t));
    dynarr_gltfbuff_init(buffs);

    uint32_t i = 0, offset = 0;
    uint32_t max_length = strlen(gltfbuffs_s) - 2;
    log_info("Parsing buffers...");
    while (offset + i < max_length) {
        char* gltfbuff_s=extract_section("{", &gltfbuffs_s[offset], "gltfbuff");

        gltfbuff_t b = gltfbuff_parse(gltfbuff_s);
        dynarr_gltfbuff_push(buffs, b);

        offset += strlen(gltfbuff_s);
        i++;
        free(gltfbuff_s);
    }
    return buffs;
}

static bufferView_t bufferView_parse(char* bufferView_s) {
    char* buffer=extract_field_value("buffer", bufferView_s, "bufferView.buffer");
    char* byteOffset=extract_field_value("byteOffset", bufferView_s,
                        "bufferView.byteOffset");
    char* byteLength=extract_field_value("byteLength", bufferView_s,
                        "bufferView.byteLength");
    char* target = extract_field_value("target", bufferView_s,
                                       "bufferView.target");
    char* byteStride=extract_field_value("byteStride", bufferView_s, "bufferView.byteStride");
    bufferView_t b;
    b.buffer = atoi(buffer);
    b.byteOffset = atoi(byteOffset);
    b.byteLength = atoi(byteLength);
    b.target = target ? atoi(target) : 0;
    b.byteStride = byteStride?atoi(byteStride):0;

    free(buffer);
    free(byteOffset);
    free(byteLength);
    free(byteStride);
    if (target) {
        free(target);
    }
    return b;
}

dynarr_bufferView_t* bufferViews_parse(char* bufferViews_s) {
    dynarr_bufferView_t *buffViews = malloc(sizeof(dynarr_bufferView_t));
    dynarr_bufferView_init(buffViews);

    uint32_t i = 0, offset = 0;
    uint32_t max_length = strlen(bufferViews_s) - 2;
    log_info("Parsing bufferViews...");
    while (offset + i < max_length) {
        char* bufferView_s=extract_section("{", &bufferViews_s[offset],
                        "bufferView");

        bufferView_t b = bufferView_parse(bufferView_s);
        dynarr_bufferView_push(buffViews, b);

        offset += strlen(bufferView_s);
        i++;
        free(bufferView_s);
    }
    return buffViews;
}

static uint32_t accessor_get_type(char* type_s) {
    if (strcmp(type_s, "\"SCALAR\"") == 0)
        return SCALAR;
    if (strcmp(type_s, "\"VEC2\"") == 0)
        return VEC2;
    if (strcmp(type_s, "\"VEC3\"") == 0)
        return VEC3;
    if (strcmp(type_s, "\"VEC4\"") == 0)
        return VEC4;
    if (strcmp(type_s, "\"MAT2\"") == 0)
        return MAT2;
    if (strcmp(type_s, "\"MAT3\"") == 0)
        return MAT3;
    if (strcmp(type_s, "\"MAT4\"") == 0)
        return MAT4;
    return 0;
}

static accessor_t accessor_parse(char* accessor_s) {
    char* bufferView=extract_field_value("bufferView", accessor_s,
                        "accessor.bufferView");
    char* byteOffset=extract_field_value("byteOffset", accessor_s,
                        "accessor.byteOffset");
    char* componentType=extract_field_value("componentType", accessor_s,
                        "accessor.componentType");
    char* count=extract_field_value("count", accessor_s, "accessor.count");
    char* type=extract_field_value("type", accessor_s, "accessor.type");
    char* normalized=extract_field_value("normalized", accessor_s, "accessor.normalized");
    char* sparse=extract_field_value("sparse", accessor_s, "accessor.sparse");
    (void) sparse;
    accessor_t a;
    a.bufferView = atoi(bufferView);
    a.byteOffset = byteOffset?atoi(byteOffset):0;
    a.componentType = atoi(componentType);
    a.count = atoi(count);
    a.type = accessor_get_type(type);
    a.normalized=normalized?strcmp(normalized,"true"):false;
    free(bufferView);
    free(byteOffset);
    free(componentType);
    free(count);
    free(type);
    return a;
}

dynarr_accessor_t* accessors_parse(char* accessors_s) {
    dynarr_accessor_t *accessors = malloc(sizeof(dynarr_accessor_t));
    dynarr_accessor_init(accessors);
    uint32_t i = 0, offset = 0;
    uint32_t max_length = strlen(accessors_s) - 2;
    log_info("Parsing accessors...");
    while (offset + i < max_length) {
        char* accessor_s=extract_section("{", &accessors_s[offset], "accessor");

        accessor_t a = accessor_parse(accessor_s);
        a.index = i;
	dynarr_accessor_push(accessors, a);

        offset += strlen(accessor_s);
        i++;
        free(accessor_s);
    }
    return accessors;
}
static gltfmesh_t mesh_parse(char* mesh_str) {
    log_debug("Parsing mesh...");

    char* primitives=extract_section("primitives", mesh_str, "primitives");

    gltfmesh_t mesh;
    dynarr_gltfprimitive_init(&mesh.primitives);
    uint32_t offset = 0;
    uint32_t i = 0;
    uint32_t max_length = strlen(primitives) - 2;
    while (offset < max_length) {
        log_debug("Getting primitive[%d]...", i);
	char* primitive_str=extract_section("{", primitives, "primitive");

	char* mode=extract_field_value("mode", primitive_str, "mode");
        char* attributes=extract_section("attributes", primitive_str, "attributes");
	char* POSITION=extract_field_value("POSITION", attributes, "POSITION");
        char* indices=extract_field_value("indices", primitive_str, "indices");
	
       gltfprimitive_t p;
        htable_attributes_init(&p.attributes, NULL);
        p.mode = mode ? atoi(mode) : GL_TRIANGLES;
        htable_attributes_insert(&p.attributes, "POSITION", atoi(POSITION));
        htable_attributes_insert(&p.attributes, "indices", atoi(indices));

        dynarr_gltfprimitive_push(&mesh.primitives, p);
        offset += strlen(primitive_str);
        i++;

        free(indices);
        free(POSITION);
        free(mode);
        free(primitive_str);
	free(attributes);
    }
    free(primitives);
    return mesh;
}
dynarr_gltfmesh_t* meshes_parse(char* chunk) {
    dynarr_gltfmesh_t *meshes = malloc(sizeof(dynarr_gltfmesh_t));
    dynarr_gltfmesh_init(meshes);
    uint32_t i = 0, offset = 0;
    uint32_t max_length = strlen(chunk) - 2;
    log_info("Parsing meshes...");
    while (offset + i < max_length) {
        char* mesh=extract_section("{", &chunk[offset], "mesh");
        //log_info(mesh);
	gltfmesh_t m = mesh_parse(mesh);
        dynarr_gltfmesh_push(meshes, m);
        offset += strlen(mesh);
        i++;
        free(mesh);
    }
    return meshes;
}

gltf_t* gltf_parse(char* chunkData) {
    gltf_t *gltf = malloc(sizeof(gltf_t));
    gltf->meshes=extract_section("meshes", chunkData, "meshes");
    gltf->bufferViews=extract_section("bufferViews", chunkData,
                    "bufferViews");
    gltf->accessors=extract_section("accessors", chunkData, "accessors");
    gltf->buffers=extract_section("buffers", chunkData, "buffers");
    gltf->scenes=extract_section("scenes", chunkData, "scenes");
    gltf->nodes=extract_section("nodes\":[{", chunkData, "nodes");
    char* default_scene=extract_section("scene", chunkData, "scene");
    if(default_scene){
    	gltf->default_scene=atoi(default_scene);
    }
    else
	    gltf->default_scene=0;
    free(default_scene);
    return gltf;
}

glb_t* glb_parse(char *filename) {
    FILE* glb_fp = fopen(filename, "r");
    uint32_t length_read = 0;

    if (!glb_fp) {
        log_error("Error reading file %s", filename);
        return NULL;
    }

    glb_t *glb = malloc(sizeof(glb_t));
    length_read += fread(&glb->magic, sizeof(uint32_t), 1, glb_fp)*sizeof(uint32_t);
    if (glb->magic != 0x46546C67) {
        log_error("%s not a gltf file", filename);
        fclose(glb_fp);
        return NULL;
    }
    length_read+=fread(&glb->version, sizeof(uint32_t), 1, glb_fp)*sizeof(uint32_t);
    if (glb->version != 2) {
        log_error("%s is not version .gltf 2.0", filename);
        fclose(glb_fp);
        return NULL;
    }

    length_read+=fread(&glb->length, sizeof(uint32_t), 1, glb_fp)*sizeof(uint32_t);

    log_info("version=%d length=%d", glb->version, glb->length);

    dynarr_chunk_init(&glb->chunks);
    size_t i = 0;
    uint32_t chunk_type;
    while (length_read < glb->length) {
        chunk_t chunk;

        fread(&chunk.chunkLength, sizeof(uint32_t), 1, glb_fp);
        fread(&chunk_type, sizeof(uint32_t), 1, glb_fp);
        chunk.chunkType = chunk_type == 0x4E4F534A   ? "JSON"
                          : chunk_type == 0x004E4942 ? "BIN"
                                                     : "unknown";
        log_info("%ld. chunk, chunkLen=%d, chunkType=%s", i, chunk.chunkLength,
                 chunk.chunkType);
        length_read += 2 * sizeof(uint32_t);

        chunk.chunkData = malloc(sizeof(char) * chunk.chunkLength);
        fread(chunk.chunkData, chunk.chunkLength, 1, glb_fp);

        i++;
        length_read += chunk.chunkLength;
        dynarr_chunk_push(&glb->chunks, chunk);
    }

    if (glb->length != length_read) {
        log_error("Error reading bytes. Read %d expected %d\n", length_read,
                  glb->length);
    }
    glb->chunks_count = i;

    log_info("Finished reading %d bytes.", length_read);
    fclose(glb_fp);
    return glb;
}


void gltfnode_destroy(gltfnode_t* n){
	if(n->name)free(n->name);
	free(n->matrix);
	if(n->mesh)	
		free(n->mesh);
	if(n->children)
	{
		dynarr_uint32_destroy(n->children);
		free(n->children);
	}
}

void gltfscene_destroy(gltfscene_t* s){
	dynarr_uint32_destroy(s->nodes);
	free(s->nodes);
}

void gltfmesh_destroy(gltfmesh_t* mesh) {
    for (size_t j = 0; j < mesh->primitives.length; j++)
        htable_attributes_destroy(&mesh->primitives.elems[j].attributes);
    dynarr_gltfprimitive_destroy(&mesh->primitives);
}

void gltf_destroy(gltf_t* gltf) {
    free(gltf->meshes);
    free(gltf->accessors);
    free(gltf->bufferViews);
    free(gltf->buffers);
    free(gltf->scenes);
    free(gltf->nodes);
}

static void chunk_destroy(chunk_t* c){
	free(c->chunkData);
} 

void glb_destroy(glb_t* glb) {
    for (size_t i = 0; i < glb->chunks.length; i++) {
         chunk_destroy(dynarr_chunk_get(&glb->chunks,i));
    }
    dynarr_chunk_destroy(&glb->chunks);
}
