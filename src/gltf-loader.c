#include "gltf-loader.h"
#include <stdio.h>
#include <stdlib.h>
void gltf_load(char* filename, data** obj){

	FILE* f=fopen(filename,"r");
	if(!f){
		fprintf(stderr,"Couldn't open %s.\n",filename);
		exit(EXIT_FAILURE);
	}
	
	*obj=malloc(sizeof(data));
	char BUFF[64];
	unsigned int vert_size;
	fscanf(f, "%s %d",BUFF,&vert_size);
	(*obj)->vert_size=vert_size;
	(*obj)->vertices=malloc(vert_size*sizeof(float)*3);
	for(unsigned int i=0;i<vert_size;i++){
		float x,y,z;
		fscanf(f, "%f %f %f",&x,&y,&z);
		(*obj)->vertices[i*3]  =	x;
		(*obj)->vertices[i*3+1]=	y;
		(*obj)->vertices[i*3+2]=	z;
	}
	unsigned int ind_size;
	fscanf(f, "%s %d",BUFF,&ind_size);
	(*obj)->ind_size=ind_size;
	(*obj)->indices=malloc(ind_size*sizeof(unsigned int));
	for(unsigned int i=0;i<ind_size;i++){
		int ind;
		fscanf(f,"%d",&ind);
		(*obj)->indices[i]=(unsigned int)ind;
	}
	fclose(f);

	printf("vert_size=%d\n",(*obj)->vert_size);
	for(unsigned int i=0;i<vert_size;i++){
		printf("%f %f %f\n",(*obj)->vertices[i*3], (*obj)->vertices[i*3+1], (*obj)->vertices[i*3+2]);
	}
	printf("ind_size=%d\n",(*obj)->ind_size);
	for(unsigned int i=0;i<ind_size;i++){
		printf("%d\n",(*obj)->indices[i]);
	}
}
