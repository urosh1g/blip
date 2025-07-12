#ifndef _blip_ecs_component_h
#define _blip_ecs_component_h

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include <datastructs/dynarr.h>
#include <datastructs/hashtable.h>

#define MAX_COMPONENTS UINT64_MAX

typedef uint64_t component_id_t;

typedef struct {
    component_id_t id;
    void* instances;
    size_t instances_capacity;
    size_t instances_length;
    size_t instance_size;
} component_t;

dynarr_define_for(component_t, components);
htable_define_for(uint64_t, component_id);

typedef struct {
    uint8_t next_component_id;
    dynarr_components_t component_store;
    htable_component_id_t component_name_map;
} component_mgr_t;

void ecs_component_mgr_create(component_mgr_t* component_mgr);
void ecs_component_mgr_destroy(component_mgr_t* component_mgr);
component_id_t* _ecs_component_id(component_mgr_t* mgr,
                                  const char* component_name);
void _ecs_component_register(component_mgr_t* mgr, const char* component_name,
                             size_t component_size);

#define ecs_component_id(pmgr, typename) _ecs_component_id((pmgr), #typename)

#define ecs_component_register(pmgr, typename)                                 \
    do {                                                                       \
        _ecs_component_register((pmgr), #typename, sizeof(typename));          \
    } while (0)

#endif
