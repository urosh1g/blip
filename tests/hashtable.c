#include <assert.h>
#include <stdio.h>

#include <datastructs/data_structures.h>

htable_define_for(int, i32);

size_t hash(const char* key) {
    size_t s = 0;
    while (*key) {
        s += *key;
        ++key;
    }
    return s;
}

int main() {
    htable_i32_t table;
    htable_i32_init(&table, NULL);
    htable_i32_insert(&table, "first", 6);
    int* value = htable_i32_get(&table, "first");
    assert(*value == 6);
    htable_i32_insert(&table, "ifrst", 12);
    value = htable_i32_get(&table, "ifrst");
    assert(*value == 12);
    assert(htable_i32_get(&table, "non existent key") == NULL);
    htable_i32_destroy(&table);
    fprintf(stdout, "SUCCESS: %s passed\n", __FILE__);
    return 0;
}
