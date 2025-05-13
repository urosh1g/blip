#ifndef _blip_ecs_entity_h
#define _blip_ecs_entity_h

#include <stdint.h>
#include <datastructs/dequeue.h>

typedef uint64_t entity_t;

dequeue_define_for(entity_t, entity);

typedef struct {
    entity_t next_avail;
    dequeue_entity_t free_ids;
} entity_mgr_t;

void entity_mgr_create(entity_mgr_t* emgr);
void entity_mgr_destroy(entity_mgr_t* emgr);

entity_t entity_create(entity_mgr_t* emgr);
void entity_destroy(entity_mgr_t* emgr, entity_t entity);

#endif
