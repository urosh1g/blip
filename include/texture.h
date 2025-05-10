#include <stb_image.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdbool.h>

unsigned int tex_load(const char* filename, bool flip_vertically);
void tex_bind(unsigned int tex_id);
