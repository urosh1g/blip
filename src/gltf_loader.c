#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "gltf_loader.h"
#define COMPTYPE_FLOAT 5126
#define COMPTYPE_USHORT 5123


void extract_section_between_tags(char **extracted_section, char *start_tag,
                                  char *end_tag, char *chunk,
                                  char *field_name) {
  char *start = strstr(chunk, start_tag);
  if (!start)
    return;
  char *end = strstr(start, end_tag);
  if (!end)
    end_tag = "}";
  end = strstr(start, end_tag);
  if (!end)
    return;

  *extracted_section = malloc(end - start + strlen(end_tag) + 1);
  strncpy((*extracted_section), start, end - start + strlen(end_tag));
  (*extracted_section)[end - start + strlen(end_tag)] = '\0';
  (void) field_name;
  // fprintf(stdout,"\nfound %s\n",field_name);
  // fprintf(stdout,"%s\n",(*extracted_section));
}

void extract_field_value(char **subchunk, char *startstr, char *chunk,
                         char *field_name) {
  char *dummy;
  extract_section_between_tags(&dummy, startstr, ",", chunk, field_name);
  char *start = strchr(dummy, ':');
  if (!start)
    return;
  start++;
  char *end = strchr(start, ',');
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

bool find_section_by_index(char **result, char *chunk, uint32_t target_index) {
  bool found = false;
  uint32_t offset = 0;
  uint32_t i = 0;
  uint32_t max_length = strlen(chunk);
  // fprintf(stdout, "Searching for section[%d]...\n",target_index);
  while (!found && offset < max_length) {
    char section_s[20];
    sprintf(section_s, "section[%d]", i);
    extract_section_between_tags(result, "{", "}", &chunk[offset], section_s);
    if (i == target_index) {
      // fprintf(stdout,"FOUND section[%s]\n",target_index);
      found = true;
    } else {
      offset += strlen(*result);
      free(*result);
      i++;
    }
  }
  return found;
}

void parse_chunk(char *chunkData) {
  gltf_t chunk;
  extract_section_between_tags(&chunk.meshes, "meshes", "}]", chunkData,
                               "meshes");
  extract_section_between_tags(&chunk.bufferViews, "bufferViews", "}]",
                               chunkData, "bufferViews");
  extract_section_between_tags(&chunk.accessors, "accessors", "}]", chunkData,
                               "accessors");

  fprintf(stdout, "\nExtracting vertices accessor...\n");
  char *POSITION;
  extract_field_value(&POSITION, "POSITION", chunk.meshes, "POSITION");

  char *buffView_indx;
  bool found =
      find_section_by_index(&buffView_indx, chunk.accessors, atoi(POSITION));
  if (!found)
    return;

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

  char *buff_indx;
  found = find_section_by_index(&buff_indx, chunk.bufferViews,
                                atoi(vertices.bufferView));
  if (!found)
    return;

  bufferView_t vertices_buffview;
  extract_field_value(&vertices_buffview.byteOffset, "byteOffset", buff_indx,
                      "vertices.byteOffset");
  extract_field_value(&vertices_buffview.byteLength, "byteLength", buff_indx,
                      "vertices byteLength");

  char *buffers;
  extract_section_between_tags(&buffers, "buffers", "}]", chunkData, "buffers");
}


bool gltf_parse(char *filename){
  FILE *f = fopen(filename, "r");
  if (!f) {
    fprintf(stderr, "Error reading file %s", filename);
    return -1;
  }

  uint32_t magic, version, length;
  fread(&magic, sizeof(uint32_t), 1, f);
  if (magic != 0x46546C67) {
    fprintf(stderr, "Not a gltf file\n");
    fclose(f);
    return false;
  }
  fread(&version, sizeof(uint32_t), 1, f);
  if (version != 2) {
    fprintf(stderr, ".gltf not version 2.0\n");
    fclose(f);
    return false;
  }
  fread(&length, sizeof(uint32_t), 1, f);

  printf("version=%d length=%d\n", version, length);

  uint32_t chunk_len, chunk_type;
  size_t i = 0;
  uint32_t length_read = 3*sizeof(uint32_t);
  while (length_read < length) {
    fread(&chunk_len, sizeof(uint32_t), 1, f);
    fread(&chunk_type, sizeof(uint32_t), 1, f);
    length_read+=2*sizeof(uint32_t);
    char *type;
    type = chunk_type == 0x4E4F534A   ? "JSON"
           : chunk_type == 0x004E4942 ? "BIN"
                                      : "unknown";
    fprintf(stdout, "\n%ld. chunk, chunk_len=%d, chunk_type=%s\n", i, chunk_len,
            type);
    char *chunkData = malloc(sizeof(char) * chunk_len);
    fread(chunkData, chunk_len, 1, f);
    fprintf(stdout, "%s\n", chunkData);
    if (strcmp(type,"JSON")==0 )
    {
    parse_chunk(chunkData);
    }
    free(chunkData);
    i++;
    length_read += chunk_len;

  }
  if(length!=length_read){
  	fprintf(stderr,"Error reading bytes. Read %d expected %d\n",length_read,length);
  }
  fprintf(stdout, "Finished reading %d bytes.\n",length_read);
  fclose(f);
  return true;
}

