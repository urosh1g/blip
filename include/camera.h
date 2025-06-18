#ifndef _blip_camera_h
#define _blip_camera_h

#include <cglm/types.h>

#include <direction.h>

typedef enum {
    CAMERA_ORTHO = 0,
    CAMERA_PERSPECTIVE,
} camera_projection_type;

typedef struct {
    // TODO: add **roll** component
    float yaw, pitch, fov;

    vec3 front, right, up;
    vec3 position, world_up;

    mat4 projection;
    mat4 view;
} camera_t;

void camera_create(camera_t* camera, const vec3 position, const vec3 world_up,
                   const float aspect, const float near, const float far,
                   const float fov, camera_projection_type type);

void camera_move(camera_t* camera, direction_t direction, float amount);
void camera_rotate(camera_t* camera, float delta_yaw, float delta_pitch);

/*
 *  Update the cameras view matrix
 *  after changing any of it's properties.
 */
void camera_update_view(camera_t* camera);

#endif
