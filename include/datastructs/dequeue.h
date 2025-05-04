#ifndef _blip_dequeue_t
#define _blip_dequeue_t

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef DEQUEUE_DEFAULT_CAPACITY
#define DEQUEUE_DEFAULT_CAPACITY 64
#endif

#ifdef DEBUG
#include <assert.h>
#define DEQUEUE_ASSERT(expr) assert(expr)
#else
#define DEQUEUE_ASSERT(expr) ((void)0)
#endif

#define dequeue_define_for(type, name)                                         \
    typedef struct {                                                           \
        type* elems;                                                           \
        size_t head;                                                           \
        size_t length;                                                         \
        size_t capacity;                                                       \
    } dequeue_##name##_t;                                                      \
                                                                               \
    static inline void dequeue_##name##_init(dequeue_##name##_t* dequeue) {    \
        dequeue->capacity = DEQUEUE_DEFAULT_CAPACITY;                          \
        dequeue->elems =                                                       \
            (type*)malloc(DEQUEUE_DEFAULT_CAPACITY * sizeof(type));            \
        dequeue->head = 0;                                                     \
        dequeue->length = 0;                                                   \
    }                                                                          \
                                                                               \
    static inline void dequeue_##name##_init_capacity(                         \
        dequeue_##name##_t* dequeue, size_t capacity) {                        \
        dequeue->capacity = capacity;                                          \
        dequeue->elems = (type*)malloc(capacity * sizeof(type));               \
        dequeue->head = 0;                                                     \
        dequeue->length = 0;                                                   \
    }                                                                          \
                                                                               \
    static inline void dequeue_##name##_add_back(dequeue_##name##_t* dequeue,  \
                                                 type el) {                    \
        if(dequeue->length>=dequeue->capacity){\
		size_t new_capacity=dequeue->capacity*2;\
		size_t head_size=dequeue->head+dequeue->length;\
		head_size-=head_size>dequeue->capacity?dequeue->capacity:0;\
		dequeue->elems=realloc(dequeue->elems, new_capacity*sizeof(type));\
		for(size_t i=0; i<head_size;i++){\
			size_t old_index=dequeue->capacity-i;\
			size_t new_index=new_capacity-i;\
			dequeue->elems[new_index]=dequeue->elems[old_index];\
		}\
		dequeue->head=new_capacity-head_size;\
		dequeue->capacity=new_capacity;\
	}\
                                                                               \
        size_t tail = dequeue->head + dequeue->length;                         \
        if (tail >= dequeue->capacity)                                         \
            tail -= dequeue->capacity;                                         \
        dequeue->elems[tail] = el;                                             \
        dequeue->length++;                                                     \
    }                                                                          \
                                                                               \
    static inline type dequeue_##name##_remove_back(                           \
        dequeue_##name##_t* dequeue) {                                         \
        DEQUEUE_ASSERT(dequeue->length > 0 && "DEQUEUE IS EMPTY");             \
        size_t tail = dequeue->head + dequeue->length;                         \
        if (tail > dequeue->capacity)                                          \
            tail -= dequeue->capacity;                                         \
        type el = dequeue->elems[tail - 1];                                    \
        dequeue->length--;                                                     \
        return el;                                                             \
    }                                                                          \
                                                                               \
    static inline type dequeue_##name##_get_back(                              \
        dequeue_##name##_t* dequeue) {                                         \
        DEQUEUE_ASSERT(dequeue->length > 0 && "DEQUEUE IS EMPTY");             \
        size_t tail = dequeue->head + dequeue->length;                         \
        if (tail > dequeue->capacity)                                          \
            tail -= dequeue->capacity;                                         \
        type el = dequeue->elems[tail - 1];                                    \
        return el;                                                             \
    }                                                                          \
                                                                               \
    static inline void dequeue_##name##_add_front(dequeue_##name##_t* dequeue, \
                                                  type el) {                   \
        if(dequeue->length>=dequeue->capacity){\
		size_t new_capacity=dequeue->capacity*2;\
		size_t head_size=dequeue->head+dequeue->length;\
		head_size-=head_size>dequeue->capacity?dequeue->capacity:0;\
		dequeue->elems=realloc(dequeue->elems, new_capacity*sizeof(type));\
		for(size_t i=0; i<head_size;i++){\
			size_t old_index=dequeue->capacity-i;\
			size_t new_index=new_capacity-i;\
			dequeue->elems[new_index]=dequeue->elems[old_index];\
		}\
		dequeue->head=new_capacity-head_size;\
		dequeue->capacity=new_capacity;\
	}\
	if (dequeue->head == 0)                                                \
            dequeue->head += dequeue->capacity;                                \
        dequeue->head--;                                                       \
        dequeue->elems[dequeue->head] = el;                                    \
        dequeue->length++;                                                     \
    }                                                                          \
                                                                               \
    static inline type dequeue_##name##_remove_front(                          \
        dequeue_##name##_t* dequeue) {                                         \
        DEQUEUE_ASSERT(dequeue->length > 0 && "DEQUEUE IS EMPTY");             \
        type el = dequeue->elems[dequeue->head];                               \
        dequeue->head++;                                                       \
        dequeue->length--;                                                     \
        return el;                                                             \
    }                                                                          \
                                                                               \
    static inline type dequeue_##name##_get_front(                             \
        dequeue_##name##_t* dequeue) {                                         \
        DEQUEUE_ASSERT(dequeue->length > 0 && "DEQUEUE IS EMPTY");             \
        type el = dequeue->elems[dequeue->head];                               \
        return el;                                                             \
    }                                                                          \
                                                                               \
    static inline bool dequeue_##name##_is_empty(                              \
        dequeue_##name##_t* dequeue) {                                         \
        return dequeue->length == 0;                                           \
    }                                                                          \
                                                                               \
    static inline bool dequeue_##name##_is_full(dequeue_##name##_t* dequeue) { \
        return dequeue->length == dequeue->capacity;                           \
    }\
	static inline void dequeue_##name##_destroy(dequeue_##name##_t* dequeue){\
		DEQUEUE_ASSERT(dequeue);\
		free(dequeue->elems);\
	}

#endif
