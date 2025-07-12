#include <ecs/world.h>

void ecs_world_init(ecs_world_t* world) {
    entity_mgr_create(&world->entity_manager);
}

entity_t ecs_world_create_entity(ecs_world_t* world) {
    return entity_create(&world->entity_manager);
}

void ecs_world_destroy_entity(ecs_world_t* world, entity_t entity) {
    entity_destroy(&world->entity_manager, entity);
}

void ecs_world_destroy(ecs_world_t* world) {
    entity_mgr_destroy(&world->entity_manager);
}

