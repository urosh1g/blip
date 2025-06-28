#include <cglm/cglm.h>
typedef struct light_t{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float ambient_strength;
} light_t;

light_t light_create_default();
