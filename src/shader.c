#include <shader.h>
#include <stdio.h>
const char* file_load_text(const char* filename){
	FILE* f=fopen(filename,"r");
	if(!f){
		fprintf(stderr,"Error opening file %s\n",filename);
		exit(EXIT_FAILURE);
	}

	fseek(f,0,SEEK_END);
	size_t filesize=ftell(f);

	char* text=malloc(filesize+1);

	fseek(f,0,SEEK_SET);
	fread(text,64,filesize,f);
	text[filesize]='\0';
	return text;
}

GLuint shader_load(const char* filename, GLenum shader_type){
	const char* shader_code=file_load_text(filename);
	
	GLuint shader_id=glCreateShader(shader_type);
	if(!shader_id){
		fprintf(stderr,"Error creating shader\n");
	}
	glShaderSource(shader_id,1,&shader_code,NULL);
	GLenum err=glGetError();
	if(err!=GL_NO_ERROR){
		printf("Couldn't set shader source, error code: %d\n",err);
	}
	glCompileShader(shader_id);
	free((char*)shader_code);
	GLint success;
	glGetShaderiv(shader_id,GL_COMPILE_STATUS,&success);
	if(success==GL_FALSE){
		GLsizei log_size;
		glGetShaderiv(shader_id,GL_INFO_LOG_LENGTH,&log_size);					                GLchar *log=malloc(1024);
		glGetShaderInfoLog(shader_id,1024,&log_size,log);
		fprintf(stderr,"Error compiling shader %s\n",filename);
		fprintf(stderr,"%s\n",log);
		free(log);
		glDeleteShader(shader_id);
		exit(EXIT_FAILURE);
	}
	return shader_id;
}

void shader_destroy(GLuint shader_id){
	glDeleteShader(shader_id);
}

void program_use(GLuint program_id){
	glUseProgram(program_id);
}

GLuint program_link(GLuint* shaders, size_t num){
	GLuint program_id = glCreateProgram();
	for(size_t i=0;i<num;i++){
		glAttachShader(program_id,shaders[i]);
	}
	glLinkProgram(program_id);
	GLenum err=glGetError();
	if(err!=GL_NO_ERROR){
		printf("Program didn't link successfully.\n");
		exit(EXIT_FAILURE);
	}
	for(size_t i=0;i<num;i++){
		glDetachShader(program_id,shaders[i]);
	}
	return program_id;
}
