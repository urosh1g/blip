#include <stb_image.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include <glad/glad.h>

typedef enum TEX_ENUM { TEX_FLIP_VERT_OFF, TEX_FLIP_VERT_ON } TEX_ENUM;

unsigned int tex_load(const char* filename, TEX_ENUM flip_vertically);
void tex_bind(unsigned int tex_id);
