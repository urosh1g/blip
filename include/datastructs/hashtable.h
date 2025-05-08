#ifndef _blip_hashtable_h
#define _blip_hashtable_h

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define HTABLE_DEFAULT_CAPACITY 1024
#define HTABLE_MAX_LOAD_FACTOR 0.7

#ifdef DEBUG
#include <assert.h>
#define HTABLE_ASSERT(expr) assert(expr)
#else
#define HTABLE_ASSERT(expr) ((void)0)
#endif

/*
 *  NOTE: This only works for gcc / clangd.
 */
#ifndef ALLOWUNUSED
#define ALLOWUNUSED __attribute__((unused))
#endif

/*
 *  This default hashing function (djb2) will be used if none is provided to
 *  htable_init.
 */
ALLOWUNUSED static inline size_t default_hasher(const char* key) {
    size_t hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

typedef size_t (*hash_fn)(const char*);

/*
 *  This hash table implementation uses open addressing with linear probing
 * in case of collisions.
 *
 */

#define htable_define_for(type, name)                                          \
                                                                               \
    typedef struct {                                                           \
        const char* key;                                                       \
        type value;                                                            \
        bool valid;                                                            \
    } htable_entry_##name##_t;                                                 \
                                                                               \
    typedef struct {                                                           \
        htable_entry_##name##_t* items;                                        \
        size_t capacity;                                                       \
        size_t size;                                                           \
        hash_fn hash;                                                          \
    } htable_##name##_t;                                                       \
                                                                               \
    ALLOWUNUSED static inline void htable_##name##_init(                       \
        htable_##name##_t* table, hash_fn hash) {                              \
        HTABLE_ASSERT(table);                                                  \
        table->capacity = HTABLE_DEFAULT_CAPACITY;                             \
        table->size = 0;                                                       \
        if (!hash) {                                                           \
            table->hash = default_hasher;                                      \
        } else {                                                               \
            table->hash = hash;                                                \
        }                                                                      \
        table->items = (htable_entry_##name##_t*)calloc(                       \
            table->capacity, sizeof(htable_entry_##name##_t));                 \
    }                                                                          \
                                                                               \
    ALLOWUNUSED static inline void htable_##name##_destroy(                    \
        htable_##name##_t* table) {                                            \
        HTABLE_ASSERT(table);                                                  \
        free(table->items);                                                    \
        table->items = NULL;                                                   \
        table->capacity = 0;                                                   \
        table->size = 0;                                                       \
        table->hash = NULL;                                                    \
    }                                                                          \
                                                                               \
    ALLOWUNUSED static inline void htable_##name##_resize(                     \
        htable_##name##_t* table, size_t new_capacity);                        \
                                                                               \
    ALLOWUNUSED static inline void htable_##name##_insert(                     \
        htable_##name##_t* table, const char* key, type value) {               \
        if ((double)(table->size + 1) / table->capacity >                      \
            HTABLE_MAX_LOAD_FACTOR) {                                          \
            htable_##name##_resize(table, table->capacity * 2);                \
        }                                                                      \
        size_t index = table->hash(key) % table->capacity;                     \
        for (size_t i = 0; i < table->capacity; i++) {                         \
            size_t idx = (index + i) % table->capacity;                        \
            if (!table->items[idx].valid ||                                    \
                strcmp(table->items[idx].key, key) == 0) {                     \
                if (!table->items[idx].valid)                                  \
                    table->size++;                                             \
                table->items[idx].key = key;                                   \
                table->items[idx].value = value;                               \
                table->items[idx].valid = true;                                \
                return;                                                        \
            }                                                                  \
        }                                                                      \
    }                                                                          \
                                                                               \
    ALLOWUNUSED static inline void htable_##name##_resize(                     \
        htable_##name##_t* table, size_t new_capacity) {                       \
        htable_entry_##name##_t* old_items = table->items;                     \
        size_t old_capacity = table->capacity;                                 \
                                                                               \
        table->items = (htable_entry_##name##_t*)calloc(                       \
            new_capacity, sizeof(htable_entry_##name##_t));                    \
        table->capacity = new_capacity;                                        \
        table->size = 0;                                                       \
                                                                               \
        for (size_t i = 0; i < old_capacity; i++) {                            \
            if (old_items[i].valid) {                                          \
                htable_##name##_insert(table, old_items[i].key,                \
                                       old_items[i].value);                    \
            }                                                                  \
        }                                                                      \
        free(old_items);                                                       \
    }                                                                          \
                                                                               \
    ALLOWUNUSED static inline type* htable_##name##_get(                       \
        htable_##name##_t* table, const char* key) {                           \
        size_t index = table->hash(key) % table->capacity;                     \
        for (size_t i = 0; i < table->capacity; i++) {                         \
            size_t idx = (index + i) % table->capacity;                        \
            if (table->items[idx].valid &&                                     \
                strcmp(table->items[idx].key, key) == 0) {                     \
                return &table->items[idx].value;                               \
            }                                                                  \
        }                                                                      \
        return NULL;                                                           \
    }                                                                          \
                                                                               \
    ALLOWUNUSED static inline bool htable_##name##_remove(                     \
        htable_##name##_t* table, const char* key) {                           \
        size_t index = table->hash(key) % table->capacity;                     \
        for (size_t i = 0; i < table->capacity; i++) {                         \
            size_t idx = (index + i) % table->capacity;                        \
            if (!table->items[idx].valid)                                      \
                return false;                                                  \
            if (strcmp(table->items[idx].key, key) == 0) {                     \
                table->items[idx].valid = false;                               \
                table->items[idx].key = NULL;                                  \
                table->size--;                                                 \
                                                                               \
                size_t j = (idx + 1) % table->capacity;                        \
                while (table->items[j].valid) {                                \
                    const char* rehash_key = table->items[j].key;              \
                    type rehash_value = table->items[j].value;                 \
                    table->items[j].valid = false;                             \
                    table->size--;                                             \
                    htable_##name##_insert(table, rehash_key, rehash_value);   \
                    j = (j + 1) % table->capacity;                             \
                }                                                              \
                return true;                                                   \
            }                                                                  \
        }                                                                      \
        return false;                                                          \
    }

#endif
