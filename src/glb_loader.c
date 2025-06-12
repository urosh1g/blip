#include <glb_loader.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logger/logger.h>

static bool extract_section(char** extracted_section, char* start_tag, char* chunk,
                     char* field_name) {
    char* start = strstr(chunk, start_tag);
    if (!start)
        return false;
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
        return false;
    *extracted_section = malloc(p - start + 2);
    strncpy((*extracted_section), start, p - start + 1);
    (*extracted_section)[p - start + 1] = '\0';
    (void)field_name;
    // log_debug("found %s\n%s",field_name,(*extracted_section));
    return true;
}

static bool extract_section_between_tags(char** result, char* start_tag, char* end_tag,
                                  char* chunk, char* field_name) {
    char* start = strstr(chunk, start_tag);
    if (!start) {
        log_debug("Couldn't find %s", start_tag);
        return false;
    }
    char* end = strstr(start, end_tag);
    if (!end) {
        end = strstr(start, "}");
    }
    *result = malloc(end - start + 2);
    strncpy((*result), start, end - start + 1);
    (*result)[end - start + 1] = '\0';
    (void)field_name;
    // log_debug("found %s\n%s",field_name,(*result));
    return true;
}

static bool extract_field_value(char** subchunk, char* startstr, char* chunk,
                         char* field_name) {
    char* dummy;
    bool success =
        extract_section_between_tags(&dummy, startstr, ",", chunk, field_name);
    if (!success)
        return false;
    char* start = strchr(dummy, ':');
    if (!start)
        return false;
    start++;
    char* end = strchr(start, ',');
    if (!end)
        end = strchr(start, '}');
    if (!end)
        return false;
    *subchunk = malloc(end - start + 1);
    strncpy((*subchunk), start, end - start);
    (*subchunk)[end - start] = '\0';
    // log_debug("%s=%s", field_name, (*subchunk));
    free(dummy);
    return true;
}

static gltfbuff_t gltfbuff_parse(char* buff_s) {
    gltfbuff_t b;
    char* byteLength;
    extract_field_value(&byteLength, "byteLength", buff_s, "buffer.byteLength");
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
        char* gltfbuff_s;
        extract_section(&gltfbuff_s, "{", &gltfbuffs_s[offset], "gltfbuff");

        gltfbuff_t b = gltfbuff_parse(gltfbuff_s);
        dynarr_gltfbuff_push(buffs, b);

        offset += strlen(gltfbuff_s);
        i++;
        free(gltfbuff_s);
    }
    return buffs;
}

static bufferView_t bufferView_parse(char* bufferView_s) {
    bufferView_t b;
    char *buffer, *byteOffset, *byteLength, *target;
    extract_field_value(&buffer, "buffer", bufferView_s, "bufferView.buffer");
    extract_field_value(&byteOffset, "byteOffset", bufferView_s,
                        "bufferView.byteOffset");
    extract_field_value(&byteLength, "byteLength", bufferView_s,
                        "bufferView.byteLength");
    bool success = extract_field_value(&target, "target", bufferView_s,
                                       "bufferView.target");

    b.buffer = atoi(buffer);
    b.byteOffset = atoi(byteOffset);
    b.byteLength = atoi(byteLength);
    b.target = success ? atoi(target) : 0;

    free(buffer);
    free(byteOffset);
    free(byteLength);
    if (success) {
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
        char* bufferView_s;
        extract_section(&bufferView_s, "{", &bufferViews_s[offset],
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
    accessor_t a;
    char *bufferView, *byteOffset, *componentType, *count, *type;
    extract_field_value(&bufferView, "bufferView", accessor_s,
                        "accessor.bufferView");
    extract_field_value(&byteOffset, "byteOffset", accessor_s,
                        "accessor.byteOffset");
    extract_field_value(&componentType, "componentType", accessor_s,
                        "accessor.componentType");
    extract_field_value(&count, "count", accessor_s, "accessor.count");
    extract_field_value(&type, "type", accessor_s, "accessor.type");

    a.bufferView = atoi(bufferView);
    a.byteOffset = atoi(byteOffset);
    a.componentType = atoi(componentType);
    a.count = atoi(count);
    a.type = accessor_get_type(type);

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
        char* accessor_s;
        extract_section(&accessor_s, "{", &accessors_s[offset], "accessor");

        accessor_t a = accessor_parse(accessor_s);
        a.index = i;
        dynarr_accessor_push(accessors, a);

        offset += strlen(accessor_s);
        i++;
        free(accessor_s);
    }
    return accessors;
}
static mesh_t mesh_parse(char* mesh_str) {
    log_debug("Parsing mesh...");

    char* primitives;
    extract_section(&primitives, "primitives", mesh_str, "primitives");

    mesh_t mesh;
    dynarr_primitive_init(&mesh.primitives);
    uint32_t offset = 0;
    uint32_t i = 0;
    uint32_t max_length = strlen(primitives) - 2;
    while (offset < max_length) {
        log_debug("Getting primitive[%d]...", i);
        char* primitive_str;
        extract_section(&primitive_str, "{", primitives, "primitive");

        primitive_t p;
        htable_attributes_init(&p.attributes, NULL);

        char* mode;
        extract_field_value(&mode, "mode", primitive_str, "mode");
        p.mode = mode ? atoi(mode) : RENDERMODE_DEFAULT;

        char* POSITION;
        extract_field_value(&POSITION, "POSITION", primitive_str, "POSITION");
        htable_attributes_insert(&p.attributes, "POSITION", atoi(POSITION));

        char* indices;
        extract_field_value(&indices, "indices", primitive_str, "indices");
        htable_attributes_insert(&p.attributes, "indices", atoi(indices));

        dynarr_primitive_push(&mesh.primitives, p);
        offset += strlen(primitive_str);
        i++;

        free(indices);
        free(POSITION);
        free(mode);
        free(primitive_str);
    }
    free(primitives);
    return mesh;
}
dynarr_mesh_t* meshes_parse(char* chunk) {
    dynarr_mesh_t *meshes = malloc(sizeof(dynarr_mesh_t));
    dynarr_mesh_init(meshes);
    uint32_t i = 0, offset = 0;
    uint32_t max_length = strlen(chunk) - 2;
    log_info("Parsing meshes...");
    while (offset + i < max_length) {
        char* mesh;
        extract_section(&mesh, "{", &chunk[offset], "mesh");
        mesh_t m = mesh_parse(mesh);
        dynarr_mesh_push(meshes, m);
        offset += strlen(mesh);
        i++;
        free(mesh);
    }
    return meshes;
}

gltf_t* gltf_parse(char* chunkData) {
    gltf_t *gltf = malloc(sizeof(gltf_t));
    extract_section(&gltf->meshes, "meshes", chunkData, "meshes");
    extract_section(&gltf->bufferViews, "bufferViews", chunkData,
                    "bufferViews");
    extract_section(&gltf->accessors, "accessors", chunkData, "accessors");
    extract_section(&gltf->buffers, "buffers", chunkData, "buffers");

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
    fread(&glb->magic, sizeof(uint32_t), 1, glb_fp);
    if (glb->magic != 0x46546C67) {
        log_error("%s not a gltf file", filename);
        fclose(glb_fp);
        return NULL;
    }
    length_read += sizeof(uint32_t);

    fread(&glb->version, sizeof(uint32_t), 1, glb_fp);
    if (glb->version != 2) {
        log_error("%s is not version .gltf 2.0", filename);
        fclose(glb_fp);
        return NULL;
    }
    length_read += sizeof(uint32_t);

    fread(&glb->length, sizeof(uint32_t), 1, glb_fp);
    length_read += sizeof(uint32_t);

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

static void mesh_destroy(mesh_t* mesh) {
    for (size_t j = 0; j < mesh->primitives.length; j++)
        htable_attributes_destroy(&mesh->primitives.elems[j].attributes);
    dynarr_primitive_destroy(&mesh->primitives);
}
static void glb_destroy(glb_t* glb) {
    for (size_t i = 0; i < glb->chunks.length; i++) {
        free(glb->chunks.elems[i].chunkData);
    }
    dynarr_chunk_destroy(&glb->chunks);
}
static void gltf_destroy(gltf_t* gltf) {
    free(gltf->meshes);
    free(gltf->accessors);
    free(gltf->bufferViews);
    free(gltf->buffers);
}

static bool indices_load(glb_t* glb, dynarr_mesh_t* meshes,
                  dynarr_accessor_t* accessors,
                  dynarr_bufferView_t* bufferViews, unsigned int** indices) {
    uint32_t* accessor = htable_attributes_get(
        &meshes->elems[0].primitives.elems[0].attributes, "indices");
    if (!accessor) {
        log_error("no indices found");
        return false;
    }
    uint32_t indices_count = accessors->elems[*accessor].count;
    componentType_t component_type = accessors->elems[*accessor].componentType;
    if (component_type != UNSIGNED_INT && component_type != UNSIGNED_SHORT) {
        log_error("indices ComponentType not UNSIGNED_INT/UNSIGNED_SHORT");
        return false;
    }
    uint32_t component_size = component_type == UNSIGNED_INT ? sizeof(uint32_t) : sizeof(uint16_t);
    type_t type = accessors->elems[*accessor].type;
    uint32_t buffView_ind = accessors->elems[*accessor].bufferView;
    bufferView_t* buffView = dynarr_bufferView_get(bufferViews, buffView_ind);
    uint32_t buff_indx = buffView->buffer + 1; // first chunk is gltf
    uint32_t startOffset = buffView->byteOffset;

    uint32_t index = 0;
    uint32_t size = component_size * type;
    *indices = malloc(4 * indices_count);
    while (index < indices_count) {
        uint32_t offset = startOffset + index * size;
	uint16_t dummy;
        memcpy(&dummy,
               &glb->chunks.elems[buff_indx].chunkData[offset], component_size);
        (*indices)[index]=(uint32_t)dummy;
	//log_info("i%d=%d", index, (*indices)[index]);
        index++;
    }
    log_info("indices_count=%d",indices_count);
    return true;
}

static bool position_load(glb_t* glb, dynarr_mesh_t* meshes,
                   dynarr_accessor_t* accessors,
                   dynarr_bufferView_t* bufferViews, float** vertices) {
    uint32_t accessor = *htable_attributes_get(
        &meshes->elems[0].primitives.elems[0].attributes, "POSITION");
    uint32_t vertices_count = accessors->elems[accessor].count;
    componentType_t component_type = accessors->elems[accessor].componentType;
    if (component_type != FLOAT) {
        log_error("POSITION ComponentType not FLOAT");
        return false;
    }
    uint32_t component_size = 4;
    type_t vertices_type = accessors->elems[accessor].type;
    uint32_t vertices_buffView_ind = accessors->elems[accessor].bufferView;
    bufferView_t* vertices_buffView =
        dynarr_bufferView_get(bufferViews, vertices_buffView_ind);
    uint32_t buff_indx = vertices_buffView->buffer + 1; // first chunk is gltf
    uint32_t startOffset = vertices_buffView->byteOffset;

    uint32_t index = 0;
    uint32_t size = component_size * vertices_type;
    *vertices = malloc(size * vertices_count);
    while (index < vertices_count) {
        uint32_t offset = startOffset + index * size;

        memcpy(&(*vertices)[3 * index],
               &glb->chunks.elems[buff_indx].chunkData[offset], component_size);
        memcpy(&(*vertices)[3 * index + 1],
               &glb->chunks.elems[buff_indx].chunkData[offset + component_size],
               component_size);
        memcpy(&(*vertices)[3 * index + 2],
               &glb->chunks.elems[buff_indx]
                    .chunkData[offset + 2 * component_size],
               component_size);
        //log_info("%d. x=%f y=%f z=%f", index, (*vertices)[3 * index],
        //         (*vertices)[3 * index + 1], (*vertices)[3 * index + 2]);
        index++;
    }
    log_info("vert_count=%d", vertices_count);
    return true;
}


bool model_load(char* filename, float** vertices, uint32_t** indices,
                GLenum* mode) {
    glb_t* glb=glb_parse(filename);
    if(!glb) return false;
    if (strcmp((glb->chunks).elems[0].chunkType, "JSON") != 0) {
        log_error("first chunk not JSON");
        return false;
    }

    log_debug((glb->chunks).elems[0].chunkData);
    gltf_t* gltf=gltf_parse((glb->chunks).elems[0].chunkData);
    dynarr_mesh_t* meshes=meshes_parse(gltf->meshes);
    dynarr_accessor_t* accessors=accessors_parse(gltf->accessors);
    dynarr_bufferView_t* bufferViews=bufferViews_parse(gltf->bufferViews);
    dynarr_gltfbuff_t* buffs=gltfbuffs_parse(gltf->buffers);

    // loading data
    position_load(glb, meshes, accessors, bufferViews, vertices);
    indices_load(glb, meshes, accessors, bufferViews, indices);
    *mode = meshes->elems[0].primitives.elems[0].mode;

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
        mesh_destroy(&meshes->elems[i]);
    }
    dynarr_mesh_destroy(meshes);
    free(meshes);

    // free gltf
    gltf_destroy(gltf);
    free(gltf);

    // free glb
    glb_destroy(glb);
    free(glb);
    return true;
}
