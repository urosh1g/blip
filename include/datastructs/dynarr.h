#ifndef _blip_dynarr_t
#define _blip_dynarr_t

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#ifndef DYNARR_DEFAULT_CAPACITY
#define DYNARR_DEFAULT_CAPACITY 64
#endif

#define dynarr_define_for(type, name) \
    typedef struct { \
        type* elems; \
        size_t capacity; \
        size_t length; \
    } dynarr_##name##_t; \
    \
    static inline void dynarr_##name##_init(dynarr_##name##_t* arr) { \
        arr->elems = (type*)malloc(DYNARR_DEFAULT_CAPACITY * sizeof(type)); \
        arr->length = 0; \
        arr->capacity = DYNARR_DEFAULT_CAPACITY; \
    } \
    \
    static inline void dynarr_##name##_init_capacity(dynarr_##name##_t* arr, size_t capacity) { \
        arr->elems = (type*)malloc(capacity * sizeof(type)); \
        arr->length = 0; \
        arr->capacity = capacity; \
    } \
    \
    static inline void dynarr_##name##_push(dynarr_##name##_t* arr, type value) { \
        if (arr->length >= arr->capacity) { \
            arr->capacity *= 2; \
            arr->elems = realloc(arr->elems, arr->capacity * sizeof(type)); \
        } \
        arr->elems[arr->length++] = value; \
    } \
    \
    static inline void dynarr_##name##_destroy(dynarr_##name##_t* arr) { \
        free(arr->elems); \
        arr->elems = NULL; \
        arr->length = 0; \
        arr->capacity = 0; \
    } \
    \

#endif

