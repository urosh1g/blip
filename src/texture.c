#include <texture.h>

unsigned int tex_load(const char* filename, TEX_ENUM flip_vertically) {
    if (flip_vertically == TEX_FLIP_VERT_ON)
        stbi_set_flip_vertically_on_load(1);
    else
        stbi_set_flip_vertically_on_load(0);

    int width, height, channels_num;
    unsigned char* img = stbi_load(filename, &width, &height, &channels_num, 0);
    if (!img) {

        fprintf(stderr, "Failed to load img %s\n", filename);
        fprintf(stderr, "%s\n", stbi_failure_reason());
        exit(EXIT_FAILURE);
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 channels_num == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(img);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

void tex_bind(unsigned int texture_id) {
    glBindTexture(GL_TEXTURE_2D, texture_id);
}
