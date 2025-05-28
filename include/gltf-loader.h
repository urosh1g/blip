#ifndef GLTF_LOADER
#define GLTF_LOADER

typedef struct data{
	unsigned int vert_size;
	float* vertices;
	unsigned int ind_size;
	unsigned int* indices;
} data;

void gltf_load(char* filename, data** obj);

#endif
