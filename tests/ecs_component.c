#include <ecs/component.h>
#include <logger/logger.h>

typedef struct {
    float x, y, z;
} velocity_t;

typedef struct {
    float x, y, z;
} position_t;

typedef struct {
    float x, y, z;
} point3_t;

typedef struct {
    point3_t* points;
    size_t points_length;
    uint16_t* indices;
    size_t indices_length;
} mesh_t;

int main() {
    component_mgr_t mgr;
    ecs_component_mgr_create(&mgr);
    ecs_component_register(&mgr, velocity_t);
    ecs_component_register(&mgr, position_t);
    ecs_component_register(&mgr, mesh_t);
    log_debug("velocity_id = %d\n", *ecs_component_id(&mgr, velocity_t));
    log_debug("position_id = %d\n", *ecs_component_id(&mgr, position_t));
    log_debug("mesh_id = %d\n", *ecs_component_id(&mgr, mesh_t));
    ecs_component_mgr_destroy(&mgr);
    return 0;
}
