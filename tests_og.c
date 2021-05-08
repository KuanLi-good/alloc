#include "og_1.h"
#include "virtual_sbrk.h"
#include <stdlib.h>
#include <stdio.h>

void * virtual_heap = NULL;
void *program_break = NULL;

void * virtual_sbrk(int32_t increment) {
    // virtual_sbrk returns the previous program break of your virtual heap. 
    if(program_break == NULL) {
        program_break = virtual_heap + increment;
    }
    else {
        program_break = program_break + increment;
    }
    // Your implementation here (for your testing only)
    return program_break - increment;
}

int main() {
    // Your own testing code here
    // virtual_heap = malloc(72000);
    // init_allocator(virtual_heap, 4, 0);
    // void *p1 = virtual_malloc(virtual_heap, 3);
    // // printf("p1 has address: %p back to main\n\n", (char*)p1);
    // void *p2 = virtual_malloc(virtual_heap, 1);
    // void *p3 = virtual_malloc(virtual_heap, 2);
    // void *p4 = virtual_malloc(virtual_heap, 5);

    // // void *p5 = virtual_malloc(virtual_heap, 1);
    // // printf("p5 has address: %p back to main\n\n", (char*)p5);
    // virtual_info(virtual_heap);
    // virtual_free(virtual_heap, p2);
    // virtual_info(virtual_heap);
    // virtual_free(virtual_heap, p1);
    // virtual_info(virtual_heap);
    // virtual_free(virtual_heap, p3);
    // virtual_info(virtual_heap);
    // virtual_free(virtual_heap, p4);
    // virtual_info(virtual_heap);
    // free(virtual_heap);


    virtual_heap = malloc(17000000);
    init_allocator(virtual_heap, 14, 10);
    // printf("heap_length: %llu\n", ((VIRTUAL_HEAP*)virtual_heap)->heap_length);
    // printf("tree_length: %d\n", ((VIRTUAL_HEAP*)virtual_heap)->tree_length);
    // printf("data_start: %d\n", ((VIRTUAL_HEAP*)virtual_heap)->data_start);
    // printf("tree_1D: %td\n", (ptrdiff_t)((char*)((VIRTUAL_HEAP*)virtual_heap)->tree_1D - (char*)(VIRTUAL_HEAP*)virtual_heap));
    puts("malloc p1 2^13: ");
    void *p1 = virtual_malloc(virtual_heap, pow(2, 10));
    // printf("p1: %d\n", *((int*)p1));
    virtual_info(virtual_heap);
    // printf("index: %d\n", 0);
    // printf("index holds: %d\n", ((VIRTUAL_HEAP*)virtual_heap)->tree_1D[0]);
    puts("\nmalloc p2 2^13: ");
    void *p2 = virtual_malloc(virtual_heap, pow(2, 10));
    // printf("p2: %d\n", *((int*)p2));
    virtual_info(virtual_heap);

    puts("\nrealloc p1 to 2^22: ");
    p1 = virtual_realloc(virtual_heap, p1, pow(2, 13));
    // printf("p1: %d\n", *((int*)p1));
    virtual_info(virtual_heap);
    // printf("index: %d\n", 0);
    // printf("index holds: %d\n", ((VIRTUAL_HEAP*)virtual_heap)->tree_1D[0]);
    // virtual_info(virtual_heap);
    


    // virtual_info(virtual_heap);
    // puts("\nmalloc p3 10000: ");
    // void *p3 = virtual_malloc(virtual_heap, 20000);
    // virtual_info(virtual_heap);
    puts("\nfree p2: ");
    virtual_free(virtual_heap, p2);
    // printf("p2: %d\n", *((int*)p2));
    virtual_info(virtual_heap);
    // puts("\nmalloc p4 2048: ");
    // void *p4 = virtual_malloc(virtual_heap, pow(2, 11));
    // virtual_info(virtual_heap);
    // puts("\nfree p3: ");
    // virtual_free(virtual_heap, p3);
    // virtual_info(virtual_heap);
    puts("\nfree p1: ");
    virtual_free(virtual_heap, p1);
    // printf("p1: %d\n", *((int*)p1));
    // virtual_info(virtual_heap);
    // puts("\nfree p4: ");
    // virtual_free(virtual_heap, p4);
    // virtual_info(virtual_heap);
    
    free(virtual_heap);


    // void *ptr = malloc(126);
    // void* p1 = virtual_malloc(ptr, 2);
    // virtual_free(ptr, p1);
    return 0;
}
