#include <assert.h>

#define DEBUG
#include <ecs/entity.h>

/* this value should be at least 10 in this test case,
 * other values rely on it
 * */
#define ENT_COUNT 10

int main() {
    entity_mgr_t em;
    entity_mgr_create(&em);

    for (size_t i = 0; i < ENT_COUNT; i++) {
        entity_t entity = entity_create(&em);
        assert(entity == (entity_t)i);
    }

    entity_destroy(&em, 2);
    entity_destroy(&em, 5);
    entity_destroy(&em, 1);
    entity_destroy(&em, 7);

    assert(entity_create(&em) == (entity_t)2);
    assert(entity_create(&em) == (entity_t)5);
    assert(entity_create(&em) == (entity_t)1);
    assert(entity_create(&em) == (entity_t)7);

    assert(entity_create(&em) == ENT_COUNT);

    entity_mgr_destroy(&em);
    return 0;
}
