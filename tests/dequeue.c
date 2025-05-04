#include <stdlib.h>
#include "datastructs/dequeue.h"
#include "assert.h"
#include "stdio.h"

typedef struct color {
    float r;
    float g;
    float b;
} color;

dequeue_define_for(int, int);
dequeue_define_for(color, clr);

void test_int_dequeue();
void test_clr_dequeue();

int main() {
    test_int_dequeue();
    test_clr_dequeue();
    return 0;
}

void test_int_dequeue() {
    printf("\n\ntesting int dequeue...\n");
    dequeue_int_t d;

    // dequeue_int_init(&d);
    // assert(d.capacity == DEQUEUE_DEFAULT_CAPACITY && "dequeue_init sets wrong
    // capacity");

    dequeue_int_init_capacity(&d, 40);
    assert(d.capacity == 40 && "dequeue_init sets wrong capacity");

    printf("dequeue capacity is: %ld\n", d.capacity);

    int length = 25;
    for (int i = 0; i < length; i++) {
        dequeue_int_add_back(&d, i);
    }
    assert(d.length == (size_t)length && "dequeue_add_back not good");
    printf("\nadded %d elements from front\n", length);
    ;
    int i = 5;
    assert(d.elems[i] == i && "dequeue_add_back not good");
    printf("got from the back. got el=%d\n", dequeue_int_get_back(&d));

    int el_back = dequeue_int_remove_back(&d);
    printf("removed from the back. got el: %d\n", el_back);
    el_back = dequeue_int_remove_back(&d);
    printf("removed from the back. got el: %d\n", el_back);

    printf("capacity is: %ld length is: %ld\n", d.capacity, d.length);
    for (int i = 0; i < length; i++) {
        dequeue_int_add_front(&d, i);
    }
    assert(d.length == (size_t)2 * length - 2 && "dequeue_add_front not good");
    printf("\nadded %d elements from front\n", length);
    ;
    assert(d.elems[d.head] == 24 && "dequeue_add_front not good");
    printf("got from the front. got el: %d\n", dequeue_int_get_front(&d));
    int el_front = dequeue_int_remove_front(&d);
    printf("removed from the front. got el: %d\n", el_front);
    el_front = dequeue_int_remove_front(&d);
    printf("removed from the front. got el: %d\n", el_front);
    printf("capacity is: %ld length is: %ld\n", d.capacity, d.length);
    dequeue_int_destroy(&d);
    printf("all tests passed.\n");
}

void test_clr_dequeue() {
    printf("\n\ntesting clr dequeue...\n");
    dequeue_clr_t d;

    // dequeue_clr_init(&d);
    // assert(d.capacity == DEQUEUE_DEFAULT_CAPACITY && "dequeue_init sets wrong
    // capacity");

    dequeue_clr_init_capacity(&d, 40);
    assert(d.capacity == 40 && "dequeue_init sets wrong capacity");

    printf("dequeue capacity is: %ld\n", d.capacity);

    int length = 25;
    for (int i = 0; i < length; i++) {
        color c;
        c.r = (float)i;
        c.g = (float)i;
        c.b = (float)i;
        dequeue_clr_add_back(&d, c);
    }
    assert(d.length == (size_t)length && "dequeue_add_back not good");
    printf("\nadded %d elements from front\n", length);
    ;
    int i = 5;
    assert(d.elems[i].r == d.elems[i].g && d.elems[i].b == (float)i &&
           "dequeue_add_back not good");
    color c = dequeue_clr_get_back(&d);
    printf("got from the back. got clr=(%f,%f,%f)\n", c.r, c.g, c.b);

    color el_back = dequeue_clr_remove_back(&d);
    printf("removed from the back. got clr=(%f,%f,%f)\n", el_back.r, el_back.g,
           el_back.b);
    el_back = dequeue_clr_remove_back(&d);
    printf("removed from the back. got clr=(%f,%f,%f)\n", el_back.r, el_back.g,
           el_back.b);

    printf("capacity is: %ld length is: %ld\n", d.capacity, d.length);
    for (int i = 0; i < length; i++) {
        color c;
        c.r = (float)i;
        c.g = (float)i;
        c.b = (float)i;
        dequeue_clr_add_front(&d, c);
    }
    assert(d.length == (size_t)2 * length - 2 && "dequeue_add_front not good");
    printf("\nadded %d elements from front\n", length);
    assert(d.elems[d.head].r == d.elems[d.head].g &&
           d.elems[d.head].b == (float)24 && "dequeue_add_front not good");
    c = dequeue_clr_get_front(&d);
    printf("got from the front. got clr=(%f,%f,%f)\n", c.r, c.g, c.b);
    color el_front = dequeue_clr_remove_front(&d);
    printf("removed from the front. clr=(%f,%f,%f)\n", el_front.r, el_front.g,
           el_front.b);
    el_front = dequeue_clr_remove_front(&d);
    printf("removed from the front. clr=(%f,%f,%f)\n", el_front.r, el_front.g,
           el_front.b);
    printf("capacity is: %ld length is: %ld\n", d.capacity, d.length);
    dequeue_clr_destroy(&d);
    printf("all tests passed.\n");
}
