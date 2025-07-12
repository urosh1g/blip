#ifndef _blip_ecs_world_h
#define _blip_ecs_world_h

#include <ecs/entity.h>

typedef struct {
    entity_mgr_t entity_manager;
} ecs_world_t;

void ecs_world_init(ecs_world_t* world);
entity_t ecs_world_create_entity(ecs_world_t* world);
void ecs_world_destroy_entity(ecs_world_t* world, entity_t entity);
void ecs_world_destroy(ecs_world_t* world);

#endif

