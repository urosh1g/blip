#ifndef GLTF_LOADER
#define GLTF_LOADER

typedef struct data{
	int vert_size;
	float* vertices;
	int ind_size;
	int* indices;
} data;

void gltf_load(char* filename, data** obj);

#endif
