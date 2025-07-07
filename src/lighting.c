#include <lighting.h>

light_t light_create_default() {
    light_t l;
    glm_vec3_make((float[]){1, 1, 1}, l.ambient);
    glm_vec3_make((float[]){1, 1, 1}, l.diffuse);
    glm_vec3_make((float[]){1, 1, 1}, l.specular);
    glm_vec3_make((float[]){100, 200, 100}, l.position);
    l.ambient_strength = 0.4;
    return l;
}
