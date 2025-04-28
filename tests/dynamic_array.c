#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "datastructs/dynarr.h"

dynarr_define_for(int, int);
dynarr_define_for(const char*, str);

int main() {
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

    fprintf(stdout, "SUCCESS: dynamic_array.c tests passed\n");

    return 0;
}
