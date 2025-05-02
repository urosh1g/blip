#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cglm/mat4.h>
#include <cglm/vec3.h>
#include <cglm/cam.h>

#include <camera.h>

#define CAMERA_DEFAULT_PERSPECTIVE_FOV 45.0f
#define CAMERA_DEFAULT_YAW 45.0f
#define CAMERA_DEFAULT_PITCH 60.0f

camera_t* camera_create(const vec3 position, const vec3 world_up,
                        const float aspect, const float near, const float far,
                        const float fov, camera_projection_type type) {
    camera_t* camera = (camera_t*)malloc(sizeof(camera_t));
    memset(camera, 0, sizeof(camera_t));

    camera->yaw = CAMERA_DEFAULT_YAW;
    camera->pitch = CAMERA_DEFAULT_PITCH;
    camera->fov = fov;

    glm_mat4_identity(camera->view);
    glm_mat4_identity(camera->projection);

    glm_vec3_copy((float*)position, camera->position);
    glm_vec3_copy((float*)world_up, camera->world_up);
    glm_vec3_normalize(camera->world_up);

    if (type == CAMERA_PERSPECTIVE) {
        camera->fov = CAMERA_DEFAULT_PERSPECTIVE_FOV;
        glm_perspective(camera->fov, aspect, near, far, camera->projection);
    } else {
        glm_ortho_default(aspect, camera->projection);
    }

    camera_update_view(camera);

    return camera;
}

/*
 *  For smooth camera movements use
 *  deltaTime * amount
 */
void camera_move(camera_t* camera, direction_t direction, float amount) {
    vec3 delta;
    switch(direction) {
        case DIRECTION_FRONT:
            glm_vec3_scale(camera->front, amount, delta);
            break;
        case DIRECTION_BACK:
            glm_vec3_scale(camera->front, -amount, delta);
            break;
        case DIRECTION_LEFT:
            glm_vec3_scale(camera->right, -amount, delta);
            break;
        case DIRECTION_RIGHT:
            glm_vec3_scale(camera->right, amount, delta);
            break;
        default:
            fprintf(stderr, "ERROR: %s: Invalid direction\n", __FUNCTION__);
            return;
    }
    glm_vec3_add(camera->position, delta, camera->position);
    camera_update_view(camera);
}

void camera_rotate(camera_t* camera, float delta_yaw, float delta_pitch) {
    camera->yaw += delta_yaw;
    camera->pitch += delta_pitch;

    if(camera->pitch > 89.0f) camera->pitch = 89.0f;
    if(camera->pitch < -89.0f) camera->pitch = -89.0f;

    camera_update_view(camera);
}

/*
 *  Update the cameras view matrix
 *  after changing any of it's properties.
 *  TODO: Support **roll**
 */
void camera_update_view(camera_t* camera) {
    vec3 right, up, target;

    float yaw_rad = glm_rad(camera->yaw);
    float pitch_rad = glm_rad(camera->pitch);

    float cos_yaw = cos(yaw_rad);
    float cos_pitch = cos(pitch_rad);
    float sin_yaw = sin(yaw_rad);
    float sin_pitch = sin(pitch_rad);

    vec3 front = {
        cos_yaw * cos_pitch,
        sin_pitch,
        sin_yaw * cos_pitch,
    };

    glm_vec3_normalize_to(front, camera->front);

    glm_cross(camera->front, camera->world_up, right);
    glm_vec3_normalize_to(right, camera->right);

    glm_cross(camera->right, camera->front, up);
    glm_vec3_normalize_to(up, camera->up);

    glm_vec3_add(camera->position, camera->front, target);
    glm_lookat(camera->position, target, camera->up, camera->view);
}

void camera_destroy(void* camera_s) {
    camera_t* camera = (camera_t*)camera_s;
    free(camera);
}
