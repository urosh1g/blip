#ifndef _blip_dynarr_t
#define _blip_dynarr_t

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#ifndef DYNARR_DEFAULT_CAPACITY
#define DYNARR_DEFAULT_CAPACITY 64
#endif

#ifdef DYNARR_DEBUG
#include <assert.h>
#define DYNARR_ASSERT(expr) assert(expr)
#else
#define DYNARR_ASSERT(expr) ((void)0)
#endif

#define dynarr_define_for(type, name)                                          \
    typedef struct {                                                           \
        type* elems;                                                           \
        size_t capacity;                                                       \
        size_t length;                                                         \
    } dynarr_##name##_t;                                                       \
                                                                               \
    static inline void dynarr_##name##_init(dynarr_##name##_t* arr) {          \
        arr->elems = (type*)malloc(DYNARR_DEFAULT_CAPACITY * sizeof(type));    \
        arr->length = 0;                                                       \
        arr->capacity = DYNARR_DEFAULT_CAPACITY;                               \
    }                                                                          \
                                                                               \
    static inline void dynarr_##name##_init_capacity(dynarr_##name##_t* arr,   \
                                                     size_t capacity) {        \
        arr->elems = (type*)malloc(capacity * sizeof(type));                   \
        arr->length = 0;                                                       \
        arr->capacity = capacity;                                              \
    }                                                                          \
                                                                               \
    static inline void dynarr_##name##_push(dynarr_##name##_t* arr,            \
                                            type value) {                      \
        if (arr->length >= arr->capacity) {                                    \
            arr->capacity *= 2;                                                \
            arr->elems = realloc(arr->elems, arr->capacity * sizeof(type));    \
        }                                                                      \
        arr->elems[arr->length++] = value;                                     \
    }                                                                          \
                                                                               \
    static inline void dynarr_##name##_destroy(dynarr_##name##_t* arr) {       \
        DYNARR_ASSERT(arr);                                                    \
        free(arr->elems);                                                      \
        arr->elems = NULL;                                                     \
        arr->length = 0;                                                       \
        arr->capacity = 0;                                                     \
    }                                                                          \
                                                                               \
    static inline type* dynarr_##name##_get(dynarr_##name##_t* arr,            \
                                            size_t index) {                    \
        DYNARR_ASSERT(index < arr->length &&                                   \
                      "dynarr_get: index out of bounds");                      \
        return &arr->elems[index];                                             \
    }                                                                          \
                                                                               \
    static inline type dynarr_##name##_remove(dynarr_##name##_t* arr,          \
                                              size_t index) {                  \
        DYNARR_ASSERT(index < arr->length &&                                   \
                      "dynarr_remove: index out of bounds");                   \
        type elem = arr->elems[index];                                         \
        for (size_t i = index; i < arr->length - 1; i++) {                     \
            arr->elems[i] = arr->elems[i + 1];                                 \
        }                                                                      \
        --arr->length;                                                         \
        return elem;                                                           \
    }                                                                          \
                                                                               \
    static inline type dynarr_##name##_remove_unordered(                       \
        dynarr_##name##_t* arr, size_t index) {                                \
        DYNARR_ASSERT(index < arr->length &&                                   \
                      "dynarr_remove_unordered: index out of bounds");         \
        type elem = arr->elems[index];                                         \
        arr->elems[index] = arr->elems[--arr->length];                         \
        return elem;                                                           \
    }                                                                          \
                                                                               \
    static inline void dynarr_##name##_clear(dynarr_##name##_t* arr) {         \
        arr->length = 0;                                                       \
    }                                                                          \
                                                                               \
    static inline void dynarr_##name##_sort(                                   \
        dynarr_##name##_t* arr, int (*cmp)(const void*, const void*)) {        \
        qsort(arr->elems, arr->length, sizeof(*arr->elems), cmp);              \
    }

#endif
