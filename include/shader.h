#include <stdlib.h>
#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <string.h>

GLuint shader_load(const char* filename, GLenum shader_type);
void shader_destroy(GLuint shader_id);
GLuint program_link(GLuint* shaders, size_t num);
void program_use(GLuint program_id);
#endif
