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

camera_t* camera_create(const vec3 position, const vec3 world_up,
                        const float aspect, const float near, const float far,
                        const float fov, camera_projection_type type);

void camera_move(camera_t* camera, direction_t direction, float amount);
void camera_rotate(camera_t* camera, float delta_yaw, float delta_pitch);

/*
 *  Update the cameras view matrix
 *  after changing any of it's properties.
 */
void camera_update_view(camera_t* camera);

/*
 *   Param: camera
 *   TODO: Should be of type camera_t*.
 *   Keeping it as void* for now,
 *   might be used as a destructor function
 *   in the near future.
 */
void camera_destroy(void* camera);

#endif
