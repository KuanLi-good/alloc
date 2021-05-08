#ifndef VIRTUAL_ALLOC_H
#define VIRTUAL_ALLOC_H
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

typedef struct{
    uint32_t heap_length;
    uint8_t initial_size;
    uint8_t min_size;

    uint16_t tree_length;
    uint8_t* tree_1D;
    uint16_t data_start;

    uint8_t node_size;
    uint16_t index;
    uint32_t offset;
}VIRTUAL_HEAP;

void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size);

void * virtual_malloc(void * heapstart, uint32_t size);

int virtual_free(void * heapstart, void * ptr);

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size);

void virtual_info(void * heapstart);

int power_of_2(int n);

void error_check(void *program_break);

int parent(uint16_t index);

int left_leaf(uint16_t index);

int right_leaf(uint16_t index);

int ceil_power_2(uint32_t size, uint8_t initial_size, uint8_t min_size);

int find_node_size(uint8_t node_size, uint16_t index, uint8_t initial_size, uint8_t min_size);

void offset_traversal(void *heapstart, uint16_t index, uint8_t node_size);

void reassign_value(void *heapstart, uint16_t index);

int find_index(void* heapstart, void* ptr);

void merge_block(void* heapstart);

#endif
