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

#define dequeue_define_for(type, name)                                          \
    typedef struct {                                                           \
	 type* elems;\
	 size_t head;\
	 size_t length;\
	 size_t capacity;\
    } dequeue_##name##_t;\
\
	static inline void dequeue_##name##_init(dequeue_##name##_t* dequeue){\
		dequeue->capacity=DEQUEUE_DEFAULT_CAPACITY;\
	       	dequeue->elems=(type*)malloc(DEQUEUE_DEFAULT_CAPACITY * sizeof(type));\
		dequeue->head=0;\
		dequeue->length=0;\
	}	\
\
	static inline void dequeue_##name##_init_capacity(dequeue_##name##_t* dequeue, size_t capacity){\
		dequeue->capacity=capacity;\
	       	dequeue->elems=(type*)malloc(capacity * sizeof(type));\
		dequeue->head=0;\
		dequeue->length=0;\
	}\
\
	static inline void dequeue_##name##_add_back(dequeue_##name##_t* dequeue, type el){\
		DEQUEUE_ASSERT(dequeue->length<dequeue->capacity && "DEQUEUE IS FULL");\
		\
		size_t tail=dequeue->head+dequeue->length;\
		if(tail>=dequeue->capacity) tail-=dequeue->capacity;\
		dequeue->elems[tail]=el;\
		dequeue->length++;\
	}\
\
	static inline type dequeue_##name##_remove_back(dequeue_##name##_t* dequeue){\
		DEQUEUE_ASSERT(dequeue->length>0 && "DEQUEUE IS EMPTY");\
		size_t tail=dequeue->head+dequeue->length;\
		if(tail>dequeue->capacity) tail-=dequeue->capacity;\
		type el=dequeue->elems[tail-1];	\
		dequeue->length--;\
		return el;\
	}\
	\
	static inline type dequeue_##name##_get_back(dequeue_##name##_t* dequeue){\
		DEQUEUE_ASSERT(dequeue->length>0 && "DEQUEUE IS EMPTY");\
		size_t tail=dequeue->head+dequeue->length;\
		if(tail>dequeue->capacity) tail-=dequeue->capacity;\
		type el=dequeue->elems[tail-1];	\
		return el;\
	}\
\
	static inline void dequeue_##name##_add_front(dequeue_##name##_t* dequeue, type el){\
		DEQUEUE_ASSERT(dequeue->length<dequeue->capacity && "DEQUEUE IS FULL");\
		if(dequeue->head==0) dequeue->head+=dequeue->capacity;\
		dequeue->head--;\
		dequeue->elems[dequeue->head]=el;\
		dequeue->length++;\
	}\
\
	static inline type dequeue_##name##_remove_front(dequeue_##name##_t* dequeue){\
		DEQUEUE_ASSERT(dequeue->length>0 && "DEQUEUE IS EMPTY");\
		type el = dequeue->elems[dequeue->head];\
		dequeue->head++;\
		dequeue->length--;\
		return el;\
	}\
	\
	static inline type dequeue_##name##_get_front(dequeue_##name##_t* dequeue){\
		DEQUEUE_ASSERT(dequeue->length>0 && "DEQUEUE IS EMPTY");\
		type el = dequeue->elems[dequeue->head];\
		return el;\
	}\
\
	static inline bool dequeue_##name##_is_empty(dequeue_##name##_t* dequeue){\
		return dequeue->length==0;\
	}\
	\
	static inline bool dequeue_##name##_is_full(dequeue_##name##_t* dequeue){\
		return dequeue->length==dequeue->capacity;\
	}\

#endif
