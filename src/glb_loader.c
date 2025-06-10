#include <glb_loader.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logger/logger.h>

#define COMPTYPE_FLOAT 5126
#define COMPTYPE_USHORT 5123



void extract_section(char **extracted_section, 
				  char *start_tag,
                                  char *chunk,
                                  char *field_name) {
  char *start = strstr(chunk, start_tag);
  if(!start) return; 
  uint32_t parenthesis=1;
  if(start_tag[0]!='{'&&start_tag[0]!='[')
  {
	  start=strstr(start,"\":"); 
  	  start+=2;
  } 
  char *p=start;
  while(strlen(p)>0&&parenthesis!=0){
  	p++;
	if(*p=='{'||*p=='[') parenthesis++;
	if(*p=='}'||*p==']') parenthesis--;
  }
  *extracted_section = malloc(p - start + 2);
  strncpy((*extracted_section), start, p - start+1);
  (*extracted_section)[p - start+1] = '\0';
  log_debug("found %s\n%s",field_name,(*extracted_section));
}


void extract_section_between_tags(char **result, char* start_tag, char* end_tag, char* chunk, char* field_name){
  char *start = strstr(chunk, start_tag);
  if(!start) {
	  log_debug("Couldn't find %s",start_tag);
	  return;
  }
  log_info("start is %s",start);
  char *end =strstr(start,end_tag);
  if(!end) {
    end =strstr(start,"}");
  }
  *result = malloc(end - start + 2);
  strncpy((*result), start, end - start+1);
  (*result)[end - start+1] = '\0';
  log_debug("found %s\n%s",field_name,(*result));

}

void extract_field_value(char **subchunk, char *startstr, char *chunk,
                         char *field_name) {
  char *dummy;
  extract_section_between_tags(&dummy, startstr, ",", chunk, field_name);
  if(!dummy) return;
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
  log_debug("%s=%s", field_name, (*subchunk));
  free(dummy);
}

bool find_section_by_index(char **result, char *chunk, uint32_t target_index) {
  bool found = false;
  uint32_t offset = 0;
  uint32_t i = 0;
  uint32_t max_length = strlen(chunk);
  log_debug("Searching for section[%d]...",target_index);
  while (!found && offset < max_length) {
    char section_s[20];
    sprintf(section_s, "section[%d]", i);
    extract_section(result, "{", &chunk[offset], section_s);
    if (i == target_index) {
      log_debug("FOUND section[%d]",target_index);
      found = true;
    } else {
      offset += strlen(*result);
      free(*result);
      i++;
    }
  }
  return found;
}

void bufferViews_parse(char* chunkData){
/*
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

 *
 * */
 (void) chunkData;
}

accessor_t accessor_parse(char* accessor_s){
  log_info("%s",accessor_s);

  accessor_t a;
  char *bufferView, *byteOffset, *componentType, *count, *type;
  extract_field_value(&bufferView, "bufferView", accessor_s,
                      "accessor.bufferView");
  extract_field_value(&byteOffset, "byteOffset", accessor_s,
                      "accessor.byteOffset");
  extract_field_value(&componentType, "componentType", accessor_s,
                      "accessor.componentType");
  extract_field_value(&count, "count", accessor_s,
                      "accessor.count");
  extract_field_value(&type, "type", accessor_s, "accessor.type"); 
  
  a.bufferView=atoi(bufferView);
  a.byteOffset=atoi(byteOffset);
  a.componentType=atoi(componentType);
  a.count=atoi(count);
  a.type=atoi(type);
  
  
  free(bufferView);
  free(byteOffset);
  free(componentType);
  free(count);
  free(type);
  return a;
}

void accessors_parse(char* accessors_s, dynarr_accessor_t **accessors){
  (*accessors)=malloc(sizeof(dynarr_accessor_t));
  dynarr_accessor_init(*accessors);
  uint32_t i = 0, offset=0;
  uint32_t max_length = strlen(accessors_s)-2;
  log_info("Parsing accessors...");
  while (offset < max_length && i==0) { 
  	char *accessor_s;
 	extract_section(&accessor_s, "{", &accessors_s[offset], "accessor");

	accessor_t a=accessor_parse(accessor_s);
	a.index=i;
	dynarr_accessor_push(*accessors, a);

	offset += strlen(accessor_s);
      	i++;
	free(accessor_s);
  }
}
mesh_t mesh_parse(char* mesh_str){
  log_debug("Parsing mesh...");
 
  char* primitives;
  extract_section(&primitives, "primitives", mesh_str, "primitives");
  
  mesh_t mesh; 
  dynarr_primitive_init(&mesh.primitives); 
  uint32_t offset = 0;
  uint32_t i = 0;
  uint32_t max_length = strlen(primitives)-2;
  while (offset < max_length) {
    log_debug("Getting primitive[%d]...",i);
    char* primitive_str;
    extract_section(&primitive_str, "{", primitives, "primitive");
    
    primitive_t p; 
    htable_attributes_init(&p.attributes,NULL);
    
    char *mode;
    extract_field_value(&mode, "mode", primitive_str, "mode");
    p.mode=mode?atoi(mode):RENDERMODE_DEFAULT;
    
    char *POSITION;
    extract_field_value(&POSITION, "POSITION", primitive_str, "POSITION");
    htable_attributes_insert(&p.attributes,"POSITION",atoi(POSITION));
    
    char *indices;
    extract_field_value(&indices, "indices", primitive_str, "indices");
    htable_attributes_insert(&p.attributes,"indices",atoi(indices));
    
    dynarr_primitive_push(&mesh.primitives,p);
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
void meshes_parse(char* chunk, dynarr_mesh_t** meshes){
  (*meshes)=malloc(sizeof(meshes));
  dynarr_mesh_init(*meshes);
  uint32_t i = 0, offset=0;
  uint32_t max_length = strlen(chunk)-2;
  log_info("Parsing meshes...");
  while (offset < max_length) { 
  	char *mesh;
 	extract_section(&mesh, "{", &chunk[offset], "mesh");
  	mesh_t m=mesh_parse(mesh);
	dynarr_mesh_push(*meshes,m);
	offset += strlen(mesh);
      	i++;
	free(mesh);
  }
}

bool gltf_parse(char *chunkData, gltf_t **gltf) {
  (*gltf)=malloc(sizeof(gltf_t));
  extract_section(&(*gltf)->meshes, "meshes", chunkData,
                           "meshes");
  extract_section(&(*gltf)->bufferViews, "bufferViews",
                               chunkData, "bufferViews");
  extract_section(&(*gltf)->accessors, "accessors", chunkData,
                               "accessors");
  return true;
}


bool glb_parse(char *filename, glb_t **glb){
  FILE *glb_fp = fopen(filename, "r");
  uint32_t length_read = 0;
  
  if (!glb_fp) {
    log_error("Error reading file %s", filename);
    return false;
  }
  
  (*glb)=malloc(sizeof(glb_t));
  fread(&(*glb)->magic, sizeof(uint32_t), 1, glb_fp);
  if ((*glb)->magic != 0x46546C67) {
    log_error("%s not a gltf file",filename);
    fclose(glb_fp);
    return false;
  }
  length_read+=sizeof(uint32_t);
  
  fread(&(*glb)->version, sizeof(uint32_t), 1, glb_fp);
  if ((*glb)->version != 2) {
    log_error("%s is not version .gltf 2.0",filename);
    fclose(glb_fp);
    return false;
  }
  length_read+=sizeof(uint32_t);
  
  fread(&(*glb)->length, sizeof(uint32_t), 1, glb_fp);
  length_read+=sizeof(uint32_t);
  
  log_info("version=%d length=%d", (*glb)->version, (*glb)->length);
 
  dynarr_chunk_init(&(*glb)->chunks);
  size_t i = 0;
  uint32_t chunk_type;
  while (length_read < (*glb)->length) {
    chunk_t chunk;
    
    fread(&chunk.chunkLength, sizeof(uint32_t), 1, glb_fp);
    fread(&chunk_type, sizeof(uint32_t), 1, glb_fp);
    chunk.chunkType = chunk_type ==  0x4E4F534A ? "JSON"
           : chunk_type == 0x004E4942 ? "BIN"
                                      : "unknown";
    log_info("%ld. chunk, chunkLen=%d, chunkType=%s", i, chunk.chunkLength,
            chunk.chunkType);
    length_read+=2*sizeof(uint32_t);
    
    chunk.chunkData = malloc(sizeof(char) * chunk.chunkLength);
    fread(chunk.chunkData, chunk.chunkLength, 1, glb_fp);
    
    i++;
    length_read += chunk.chunkLength;
    dynarr_chunk_push(&(*glb)->chunks,chunk);
  }

  if((*glb)->length!=length_read){
  	log_error("Error reading bytes. Read %d expected %d\n",length_read,(*glb)->length);
  }
  (*glb)->chunks_count=i;
  
  log_info("Finished reading %d bytes.",length_read);
  fclose(glb_fp);
  return true;
}
