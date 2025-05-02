#include <stdlib.h>
#include "dequeue.h"
#include "assert.h"
#include "stdio.h"
dequeue_define_for(int,int);


void test_int_dequeue();

void main(){
	test_int_dequeue();
}

void test_int_dequeue(){
	printf("testing int dequeue...\n");
	dequeue_int_t d;
	
	//dequeue_int_init(&d);
	//assert(d.capacity == DEQUEUE_DEFAULT_CAPACITY && "dequeue_init sets wrong capacity");
	
	dequeue_int_init_capacity(&d,128);
	assert(d.capacity == 128 && "dequeue_init sets wrong capacity");
	
	printf("dequeue capacity is: %ld\n",d.capacity);
	
	int length=25;
	for(int i=0;i<length;i++){
		dequeue_int_add_back(&d, i);	
	}
	assert(d.length == length && "dequeue_add_back not good");
	int i=5;
	assert(d.elems[i]==i && "dequeue_add_back not good");
	printf("elems value at index %d is %d\n",i,dequeue_int_get_back(&d));

	int el_back=dequeue_int_remove_back(&d);
	printf("back now is: %d\n",el_back);
	el_back=dequeue_int_remove_back(&d);
	printf("back now is: %d\n",el_back);
	
	for(int i=0;i<length;i++){
		dequeue_int_add_front(&d, i);	
	}
	assert(d.length == 2*length-2 && "dequeue_add_front not good");
	assert(d.elems[d.head]==24 && "dequeue_add_front not good");
	printf("elems value at index %ld is %d\n",d.head,dequeue_int_get_front(&d));
	int el_front=dequeue_int_remove_front(&d);
	printf("front now is: %d\n",el_front);
	el_front=dequeue_int_remove_front(&d);
	printf("front now is: %d\n",el_front);
}
