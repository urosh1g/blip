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
	int vert_size;
	fscanf(f, "%s %d",BUFF,&vert_size);
	(*obj)->vert_size=vert_size;
	(*obj)->vertices=malloc(vert_size*sizeof(float)*3);
	for(int i=0;i<vert_size;i++){
		float x,y,z;
		fscanf(f, "%f %f %f",&x,&y,&z);
		(*obj)->vertices[i*3]  =	x;
		(*obj)->vertices[i*3+1]=	y;
		(*obj)->vertices[i*3+2]=	z;
	}
	int ind_size;
	fscanf(f, "%s %d",BUFF,&ind_size);
	(*obj)->ind_size=ind_size;
	(*obj)->indices=malloc(ind_size*sizeof(int));
	for(int i=0;i<ind_size;i++){
		int ind;
		fscanf(f,"%d",&ind);
		(*obj)->indices[i]=ind;
	}
	
	fclose(f);
}
