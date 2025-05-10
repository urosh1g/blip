#include <ecs/entity.h>

#ifdef DEBUG
#define ENT_ASSERT(expr) assert(expr)
#else
#define ENT_ASSERT(expr) ((void)0)
#endif

void entity_mgr_create(entity_mgr_t* emgr) {
    emgr->next_avail = 0;
    dequeue_entity_init(&emgr->free_ids);
}

void entity_mgr_destroy(entity_mgr_t* emgr) {
    dequeue_entity_destroy(&emgr->free_ids);
}

entity_t entity_create(entity_mgr_t* emgr) {
    if (!dequeue_entity_is_empty(&emgr->free_ids)) {
        return dequeue_entity_remove_front(&emgr->free_ids);
    }
    return emgr->next_avail++;
}

void entity_destroy(entity_mgr_t* emgr, entity_t entity) {
    ENT_ASSERT(entity < emgr->next_avail &&
               "Given entity was not created by this manager\n");
    dequeue_entity_add_back(&emgr->free_ids, entity);
}
