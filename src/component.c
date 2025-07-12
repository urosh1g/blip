#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <datastructs/data_structures.h>
#include <ecs/component.h>

#define DEFAULT_INSTANCES_CAPACITY 64

void ecs_component_mgr_create(component_mgr_t* mgr) {
    mgr->next_component_id = 0;
    dynarr_components_init(&mgr->component_store);
    htable_component_id_init(&mgr->component_name_map, NULL);
}

void ecs_component_mgr_destroy(component_mgr_t* mgr) {
    mgr->next_component_id = 0;
    dynarr_components_destroy(&mgr->component_store);
    htable_component_id_destroy(&mgr->component_name_map);
}

component_id_t* _ecs_component_id(component_mgr_t* mgr,
                                  const char* component_name) {
    return htable_component_id_get(&mgr->component_name_map, component_name);
}

void _ecs_component_register(component_mgr_t* mgr, const char* component_name,
                             size_t component_size) {
    assert(mgr->next_component_id < 64 && "Maximum components reached");
    component_t component = {0};
    component.id = 1 << mgr->next_component_id++;
    component.instance_size = component_size;
    component.instances_length = 0;
    component.instances_capacity = DEFAULT_INSTANCES_CAPACITY;
    component.instances = malloc(component.instances_capacity * component_size);
    dynarr_components_push(&mgr->component_store, component);
    htable_component_id_insert(&mgr->component_name_map, component_name,
                               component.id);
}
