#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"

typedef struct model{
	uint32_t vertices_index;
	uint32_t indices_index;
}model_t;


void extract_subchunk(char** subchunk, char* startstr, char* endstr, char* chunk, char* found_str){
	char *start = strstr(chunk,startstr);
	if(!start) return;
	char *end=strstr(start,endstr);
	if(!end) endstr="}";
	end=strstr(start,endstr);
	if(!end) return;


	*subchunk=malloc(end-start+strlen(endstr)+1);
	strncpy((*subchunk),start,end-start+strlen(endstr));
	(*subchunk)[end-start+strlen(endstr)]='\0';
	//fprintf(stdout,"\nfound %s\n",found_str);
	//fprintf(stdout,"%s\n",(*subchunk));
}

void extract_value(char** subchunk, char startchr, char endchar, char* chunk){
	char *start = strchr(chunk,startchr);
	if(!start) return;
	start++;
	char *end=strchr(start,endchar);
	if(!end) endchar='}';
	end=strchr(start,endchar);
	if(!end) return;
	*subchunk=malloc(end-start+1);
	strncpy((*subchunk),start,end-start);
	(*subchunk)[end-start]='\0';
	//fprintf(stdout,"FOUND VALUE:%s\n",(*subchunk));
}
void extract_data(char* chunkData){
	char* meshes;
	extract_subchunk(&meshes, "meshes","}]",chunkData, "meshes");
	char* buffer_views;
	extract_subchunk(&buffer_views, "bufferViews","}]",chunkData,"bufferViews");	
	char* accessors;
	extract_subchunk(&accessors, "accessors","}]",chunkData, "accessors");	
	

	fprintf(stdout,"\nExtracting vertices data...\n");
	char* dummy;
	char* POSITION;	
	extract_subchunk(&dummy, "POSITION",",",meshes,"position");
	extract_value(&POSITION, ':',',',dummy);
	free(dummy);

	bool found=false;
	char* buffView_indx;
	uint32_t indx=0;
	uint32_t i=0;
	//fprintf(stdout, "Searching for accessor[%s]...\n",POSITION);
	while(!found&&indx<strlen(accessors)){
		char accessor_s[20];
		sprintf(accessor_s,"accessor[%d]",i);
       		extract_subchunk(&buffView_indx, "{","}",&accessors[indx],accessor_s);
		if(atoi(POSITION)==i){
			//fprintf(stdout,"FOUND accessor[%s]\n",POSITION);
			found=true;
		}
		else
		{
			indx+=strlen(buffView_indx);
			free(buffView_indx);
			i++;
		}
	}
     char* vertices_count;
     extract_subchunk(&dummy,"count",",",buffView_indx,"vertices count");	
     extract_value(&vertices_count,':',',',dummy);	
     fprintf(stdout,"vertices.count=%s\n",vertices_count);
     free(dummy);

	char* vertices_componentType;
	extract_subchunk(&dummy,"componentType",",",buffView_indx,"vertices component type");	
	extract_value(&vertices_componentType,':',',',dummy);	
	fprintf(stdout,"vertices.componentType=%s\n",vertices_componentType);
	free(dummy);

	char* vertices_type;
	extract_subchunk(&dummy,"type",",",buffView_indx,"vertices type");	
	extract_value(&vertices_type,':',',',dummy);	
	fprintf(stdout,"vertices.type=%s\n",vertices_type);
	free(dummy);

	char* vertices_bufferView;
	extract_subchunk(&dummy,"bufferView",",",buffView_indx,"vertices bufferView indx");	
	extract_value(&vertices_bufferView,':',',',dummy);	
	fprintf(stdout,"vertices.bufferView=%s\n",vertices_bufferView);
	free(dummy);

	char* vertices_bufferViewOffset;
	extract_subchunk(&dummy,"byteOffset",",",buffView_indx,"vertices byteOffset");	
	extract_value(&vertices_bufferViewOffset,':',',',dummy);	
	fprintf(stdout,"vertices.bufferViewOffset=%s\n",vertices_bufferViewOffset);
	free(dummy);
				
	
	found=false;
	char *buff_indx;
	indx=0;
	i=0;
	//fprintf(stdout, "Searching for buffer[%s]...\n",vertices_bufferView);
	while(!found&&indx<strlen(buffer_views)){
		char buffer_s[20];
		sprintf(buffer_s,"bufferView[%d]",i);
		extract_subchunk(&buff_indx, "{","}",&buffer_views[indx],buffer_s);
		if(atoi(vertices_bufferView)==i){
			//fprintf(stdout,"FOUND buffer[%s]\n",vertices_bufferView);
			found=true;
		}
		else
		{
			indx+=strlen(buff_indx);
			free(buff_indx);
			i++;
		}
	}
	
	char* vertices_byteOffset;
     extract_subchunk(&dummy,"byteOffset",",",buff_indx,"vertices byteOffset");	
     extract_value(&vertices_byteOffset,':',',',dummy);	
     fprintf(stdout,"vertices.byteOffset=%s\n",vertices_byteOffset);
     free(dummy);


	char* vertices_byteLength;
     extract_subchunk(&dummy,"byteLength",",",buff_indx,"vertices byteLength");	
     extract_value(&vertices_byteLength,':',',',dummy);	
     fprintf(stdout,"vertices.byteLength=%s\n",vertices_byteLength);
     free(dummy);





	char* buffers;
	extract_subchunk(&buffers, "buffers","}]",chunkData,"buffers");	


}	


int main()
{
	char* filename="Duck.glb";
	FILE* f=fopen(filename,"r");
	if(!f){
		fprintf(stderr,"Error reading file %s", filename);
		return -1;
	}

	uint32_t magic, version, length;
	fread(&magic,sizeof(uint32_t),1,f);
	if(magic!=0x46546C67){
		fprintf(stderr,"Not a gltf file\n");
		fclose(f);
		return -1;
	}
	fread(&version,sizeof(uint32_t),1,f);
	if(version!=2){
		fprintf(stderr, ".gltf not version 2.0\n");
		fclose(f);
		return -1;
	}
	fread(&length,sizeof(uint32_t),1,f);
	
	printf("version=%d length=%d\n",version,length);

	uint32_t chunk_len, chunk_type;
	size_t i=0;
	uint32_t length_read=0;
	while(length_read<length && i==0){
		fread(&chunk_len,sizeof(uint32_t),1,f);
		fread(&chunk_type,sizeof(uint32_t),1,f);
		char* type;
		type=chunk_type==0x4E4F534A?"JSON":chunk_type==0x004E4942?"BIN":"unknown";
		fprintf(stdout,"\n%ld. chunk, chunk_len=%d, chunk_type=%s\n",i,chunk_len,type);
		if(type=="unknown") continue;
		char *chunkData=malloc(sizeof(char)*chunk_len);
		fread(chunkData,chunk_len,1,f);
		fprintf(stdout,"%s\n",chunkData);
		extract_data(chunkData);
		i++;
		length_read+=chunk_len;
		
		free(chunkData);
	}
	fclose(f);
	return 0;
}
