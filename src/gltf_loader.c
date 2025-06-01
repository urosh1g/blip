#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct gltf {
    char* meshes;
    char* accessors;
    char* bufferViews;
} gltf_t;

typedef struct model {
    char* vertices;
    char* indices;
    char* vertices_count;
    char* indices_count;
} model_t;

typedef struct accessor {
    char* index;
    char* count;
    char* componentType;
    char* type;
    char* bufferView;
    char* byteOffset;
} accessor_t;

typedef struct bufferView {
    char* index;
    char* byteOffset;
    char* byteLength;
} bufferView_t;

void extract_section_between_tags(char** extracted_section, char* start_tag,
                                  char* end_tag, char* chunk,
                                  char* field_name) {
    char* start = strstr(chunk, start_tag);
    if (!start)
        return;
    char* end = strstr(start, end_tag);
    if (!end)
        end_tag = "}";
    end = strstr(start, end_tag);
    if (!end)
        return;

    *extracted_section = malloc(end - start + strlen(end_tag) + 1);
    strncpy((*extracted_section), start, end - start + strlen(end_tag));
    (*extracted_section)[end - start + strlen(end_tag)] = '\0';
    // fprintf(stdout,"\nfound %s\n",field_name);
    // fprintf(stdout,"%s\n",(*extracted_section));
}

void extract_field_value(char** subchunk, char* startstr, char* chunk,
                         char* field_name) {
    char* dummy;
    extract_section_between_tags(&dummy, startstr, ",", chunk, field_name);
    char* start = strchr(dummy, ':');
    if (!start)
        return;
    start++;
    char* end = strchr(start, ',');
    if (!end)
        end = strchr(start, '}');
    if (!end)
        return;
    *subchunk = malloc(end - start + 1);
    strncpy((*subchunk), start, end - start);
    (*subchunk)[end - start] = '\0';
    // fprintf(stdout,"FOUND VALUE:%s\n",(*subchunk));
    fprintf(stdout, "%s=%s\n", field_name, (*subchunk));
    free(dummy);
}
void parse_chunk(char* chunkData) {
    gltf_t chunk;
    extract_section_between_tags(&chunk.meshes, "meshes", "}]", chunkData,
                                 "meshes");
    extract_section_between_tags(&chunk.bufferViews, "bufferViews", "}]",
                                 chunkData, "bufferViews");
    extract_section_between_tags(&chunk.accessors, "accessors", "}]", chunkData,
                                 "accessors");

    fprintf(stdout, "\nExtracting vertices accessor...\n");
    char* POSITION;
    extract_field_value(&POSITION, "POSITION", chunk.meshes, "POSITION");

    bool found = false;
    char* buffView_indx;
    uint32_t indx = 0;
    uint32_t i = 0;
    // fprintf(stdout, "Searching for accessor[%s]...\n",POSITION);
    while (!found && indx < strlen(chunk.accessors)) {
        char accessor_s[20];
        sprintf(accessor_s, "accessor[%d]", i);
        extract_section_between_tags(&buffView_indx, "{", "}",
                                     &chunk.accessors[indx], accessor_s);
        if (atoi(POSITION) == i) {
            // fprintf(stdout,"FOUND accessor[%s]\n",POSITION);
            found = true;
        } else {
            indx += strlen(buffView_indx);
            free(buffView_indx);
            i++;
        }
    }
    accessor_t vertices;
    extract_field_value(&vertices.bufferView, "bufferView", buffView_indx,
                        "vertices.bufferView");
    extract_field_value(&vertices.byteOffset, "byteOffset", buffView_indx,
                        "vertices.bufferViewOffset");
    extract_field_value(&vertices.componentType, "componentType", buffView_indx,
                        "vertices.componentType");
    extract_field_value(&vertices.count, "count", buffView_indx,
                        "vertices.count");
    extract_field_value(&vertices.type, "type", buffView_indx, "vertices.type");

    found = false;
    char* buff_indx;
    indx = 0;
    i = 0;
    // fprintf(stdout, "Searching for buffer[%s]...\n",vertices_bufferView);
    while (!found && indx < strlen(chunk.bufferViews)) {
        char buffer_s[20];
        sprintf(buffer_s, "bufferView[%d]", i);
        extract_section_between_tags(&buff_indx, "{", "}",
                                     &chunk.bufferViews[indx], buffer_s);
        if (atoi(vertices.bufferView) == i) {
            // fprintf(stdout,"FOUND buffer[%s]\n",vertices_bufferView);
            found = true;
        } else {
            indx += strlen(buff_indx);
            free(buff_indx);
            i++;
        }
    }
    bufferView_t vertices_buffview;
    extract_field_value(&vertices_buffview.byteOffset, "byteOffset", buff_indx,
                        "vertices.byteOffset");
    extract_field_value(&vertices_buffview.byteLength, "byteLength", buff_indx,
                        "vertices byteLength");

    char* buffers;
    extract_section_between_tags(&buffers, "buffers", "}]", chunkData,
                                 "buffers");
}

int main() {
    char* filename = "cube.glb";
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error reading file %s", filename);
        return -1;
    }

    uint32_t magic, version, length;
    fread(&magic, sizeof(uint32_t), 1, f);
    if (magic != 0x46546C67) {
        fprintf(stderr, "Not a gltf file\n");
        fclose(f);
        return -1;
    }
    fread(&version, sizeof(uint32_t), 1, f);
    if (version != 2) {
        fprintf(stderr, ".gltf not version 2.0\n");
        fclose(f);
        return -1;
    }
    fread(&length, sizeof(uint32_t), 1, f);

    printf("version=%d length=%d\n", version, length);

    uint32_t chunk_len, chunk_type;
    size_t i = 0;
    uint32_t length_read = 0;
    while (length_read < length && i == 0) {
        fread(&chunk_len, sizeof(uint32_t), 1, f);
        fread(&chunk_type, sizeof(uint32_t), 1, f);
        char* type;
        type = chunk_type == 0x4E4F534A   ? "JSON"
               : chunk_type == 0x004E4942 ? "BIN"
                                          : "unknown";
        fprintf(stdout, "\n%ld. chunk, chunk_len=%d, chunk_type=%s\n", i,
                chunk_len, type);
        if (type == "unknown")
            continue;
        char* chunkData = malloc(sizeof(char) * chunk_len);
        fread(chunkData, chunk_len, 1, f);
        fprintf(stdout, "%s\n", chunkData);
        parse_chunk(chunkData);
        i++;
        length_read += chunk_len;

        free(chunkData);
    }
    fclose(f);
    return 0;
}
