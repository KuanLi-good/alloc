#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

typedef struct{
    uint64_t heap_length;
    uint8_t initial_size;
    uint8_t min_size;

    uint32_t tree_length;
    uint32_t* tree_1D;
    uint32_t data_start;

    uint32_t node_size;
    uint32_t index;
    uint64_t offset;
}VIRTUAL_HEAP;

void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size);

void * virtual_malloc(void * heapstart, uint32_t size);

int virtual_free(void * heapstart, void * ptr);

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size);

void virtual_info(void * heapstart);

int power_of_2(int n);

void error_check(void *program_break);

int parent(uint32_t index);

int left_leaf(uint32_t index);

int right_leaf(uint32_t index);

int ceil_power_2(uint32_t size, uint8_t initial_size, uint8_t min_size);

int find_node_size(uint32_t node_size, uint32_t index, uint8_t initial_size);

void offset_traversal(void *heapstart, uint32_t index, uint32_t node_size);

void reassign_value(void *heapstart, uint32_t index);

int find_index(void* heapstart, void* ptr);

void merge_block(void* heapstart);


