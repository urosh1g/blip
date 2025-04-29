#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "datastructs/dynarr.h"

typedef struct {
    double x, y;
} position_t;

int pos_cmp(const void* a, const void* b) {
    position_t* pos_a = (position_t*)a;
    position_t* pos_b = (position_t*)b;
    if (pos_a->x == pos_b->x) {
        return (pos_a->y > pos_b->y) - (pos_a->y < pos_b->y);
    }
    return (pos_a->x > pos_b->x) - (pos_a->x < pos_b->x);
}

dynarr_define_for(int32_t, i32);
dynarr_define_for(const char*, str);
dynarr_define_for(position_t, pos);

void test_dynamic_string_array();
void test_dynamic_int_array();
void test_dynamic_pos_array();

int main() {
    test_dynamic_string_array();
    test_dynamic_int_array();
    test_dynamic_pos_array();

    fprintf(stdout, "SUCCESS: %s passed\n", __FILE__);

    return 0;
}

void test_dynamic_pos_array() {
    dynarr_pos_t positions;
    dynarr_pos_init(&positions);
    dynarr_pos_push(&positions, (position_t){.x = 0, .y = 1});
    dynarr_pos_push(&positions, (position_t){.x = 5, .y = 2});
    dynarr_pos_push(&positions, (position_t){.x = 1, .y = 3});
    dynarr_pos_push(&positions, (position_t){.x = 3, .y = -1});
    dynarr_pos_push(&positions, (position_t){.x = 2, .y = 0});

    dynarr_pos_sort(&positions, pos_cmp);
    for(size_t i = 0; i < positions.length - 1; i++) {
        if(positions.elems[i].x == positions.elems[i+1].x) {
            assert(positions.elems[i].y <= positions.elems[i+1].y);
        }
        else {
            assert(positions.elems[i].x < positions.elems[i+1].x);
        }
    }

    dynarr_pos_destroy(&positions);
}

void test_dynamic_int_array() {
    dynarr_i32_t arr;

    dynarr_i32_init(&arr);
    assert(arr.capacity == DYNARR_DEFAULT_CAPACITY &&
           "dynarr_init sets invalid capacity\n");

    const size_t num_elems = 20;
    for (size_t i = 0; i < num_elems; i++) {
        dynarr_i32_push(&arr, rand() % INT32_MAX);
    }

    const int32_t* elem = dynarr_i32_get(&arr, 4);
    assert(arr.elems[4] == *elem &&
           "dynarr_get should return the same element as indexing\n");

    size_t len = arr.length;
    dynarr_i32_remove(&arr, 1);
    assert(arr.length + 1 == len &&
           "dynarr_remove* should decrement the length\n");

    len = arr.length;
    dynarr_i32_remove_unordered(&arr, 128);
    assert(len == arr.length &&
           "dynarr_remove* should do nothing if out of bounds\n");

    dynarr_i32_clear(&arr);
    assert(arr.length == 0 && "dynarr_clear should reset the length\n");

    dynarr_i32_destroy(&arr);
}

void test_dynamic_string_array() {
    dynarr_str_t dynamic_string_array;

    dynarr_str_init(&dynamic_string_array);
    assert(dynamic_string_array.capacity == DYNARR_DEFAULT_CAPACITY &&
           "default initializer does not set expected capacity.\n");
    assert(dynamic_string_array.elems != NULL &&
           "default initializer does not provide an `elems` pointer.\n");

    dynarr_str_push(&dynamic_string_array, "asd");
    dynarr_str_push(&dynamic_string_array, "qwe");

    assert(dynamic_string_array.length == 2 &&
           "expecting 2 items in dynamic array\n");
    assert(!strcmp(dynamic_string_array.elems[0], "asd") &&
           "expecting first item to be `asd`\n");
    assert(!strcmp(dynamic_string_array.elems[1], "qwe") &&
           "expecting first item to be `qwe`\n");

    dynarr_str_destroy(&dynamic_string_array);
    assert(dynamic_string_array.elems == NULL &&
           "dynarr_destroy should invalidate the `elems` pointer.\n");
}
