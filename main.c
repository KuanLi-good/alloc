#include "virtual_alloc.h"
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
    virtual_heap = malloc(72000);
    init_allocator(virtual_heap, 18, 10);
    void *p1 = virtual_malloc(virtual_heap, pow(2, 12));
    void *p2 = virtual_malloc(virtual_heap, pow(2, 11));
    void *p3 = virtual_malloc(virtual_heap, pow(2, 13));
    virtual_free(virtual_heap, p1);

    free(virtual_heap);
    return 0;
}
