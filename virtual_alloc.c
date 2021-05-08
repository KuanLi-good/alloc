#include "virtual_alloc.h"
#include "virtual_sbrk.h"
#include <stdio.h>
#include <stdlib.h>

// submission 36 has all info

void error_check(void *program_break){
    if(program_break == (void *) -1){
        fprintf(stderr, "unsuccessful virtual_sbrk() call\n");
        exit(1);
    }
}

int power_of_2(int n){
    if(n == 1) return 0;
    if(n && !(n & (n - 1))) return 1;
    return 0;
}

int parent(uint16_t index) {
    if(index == 0) return index;
    if(!index%2) return index/2-1;
    else return (index+1)/2-1;
    return -1;
}

int left_leaf(uint16_t index) {
    return index*2+1;
}

int right_leaf(uint16_t index) {
    return index*2+2;
}

int ceil_power_2(uint32_t size, uint8_t initial_size, uint8_t min_size) {
    if(size == 1) return 0;
    while(initial_size >= min_size){
        if(pow(2, initial_size) < size && pow(2, initial_size+1)>= size) break;
        initial_size--;
    }
    return initial_size+1;
}

// data strcuture stores a VIRTUAL_HEAP struct
void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) {
    // printf("virtual start: %p\n", heapstart);
    if (initial_size < 1 || min_size > initial_size)
        return;
    // printf("initial_size: %d\n",initial_size);
    // printf("min_size: %d\n",min_size);
    error_check(virtual_sbrk(sizeof(VIRTUAL_HEAP)+ pow(2, (initial_size-min_size))*2-1 + pow(2, initial_size)));
    ((VIRTUAL_HEAP*)heapstart)->heap_length = sizeof(VIRTUAL_HEAP)+ (pow(2, (initial_size-min_size))*2-1) + pow(2, initial_size);
    ((VIRTUAL_HEAP*)heapstart)->data_start = sizeof(VIRTUAL_HEAP) + (pow(2, (initial_size-min_size))*2-1);
    ((VIRTUAL_HEAP*)heapstart)->tree_length = pow(2, (initial_size-min_size))*2-1;
    ((VIRTUAL_HEAP*)heapstart)->initial_size = initial_size;
    ((VIRTUAL_HEAP*)heapstart)->min_size = min_size;
    ((VIRTUAL_HEAP*)heapstart)->tree_1D = heapstart + sizeof(VIRTUAL_HEAP);
    
    // printf("virtual break: %p\n", heapstart + ((VIRTUAL_HEAP*)heapstart)->heap_length);

    // puts("in init: ");
    // printf("heap_length: %lu\n", ((VIRTUAL_HEAP*)heapstart)->heap_length);
    // printf("tree_length: %d\n", ((VIRTUAL_HEAP*)heapstart)->tree_length);
    // printf("data_start: %d\n", ((VIRTUAL_HEAP*)heapstart)->data_start);
    // printf("tree_1D: %td\n", (ptrdiff_t)((char*)((VIRTUAL_HEAP*)heapstart)->tree_1D - (char*)(VIRTUAL_HEAP*)heapstart));
    // an array of intial available size of each level
    for(int i = 0; i < ((VIRTUAL_HEAP*)heapstart)->tree_length; i++){
        if (power_of_2(i+1)){
            // puts("");
            initial_size -= 1;
    }   
        ((VIRTUAL_HEAP*)heapstart)->tree_1D[i] = initial_size;
        // printf("%d ", ((VIRTUAL_HEAP*)heapstart)->tree_1D[i]);
    }
    error_check(virtual_sbrk(1));
    return;
}

void reassign_value(void *heapstart, uint16_t index) {
    ((VIRTUAL_HEAP*)heapstart)->tree_1D[index] = 0;
    // printf("index: %d\n", ((VIRTUAL_HEAP*)heapstart)->index);
    // printf("index holds: %d\n", ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index]);

    // printf("offset: %d\n", (int)((VIRTUAL_HEAP*)heapstart)->node_size * (((VIRTUAL_HEAP*)heapstart)->index + 1) - (int)pow(2, ((VIRTUAL_HEAP*)heapstart)->initial_size));
    // re assigning available size of each node
    while (index) {
        index = parent(index);
        if(((VIRTUAL_HEAP*)heapstart)->tree_1D[left_leaf(index)] < ((VIRTUAL_HEAP*)heapstart)->tree_1D[right_leaf(index)])
            ((VIRTUAL_HEAP*)heapstart)->tree_1D[index] = ((VIRTUAL_HEAP*)heapstart)->tree_1D[right_leaf(index)];
        else
            ((VIRTUAL_HEAP*)heapstart)->tree_1D[index] = ((VIRTUAL_HEAP*)heapstart)->tree_1D[left_leaf(index)];
    }
}

void * virtual_malloc(void * heapstart, uint32_t size) {
    
    if (heapstart == NULL || size == 0 ) {
        // puts("heapstart == NULL, size == 0");
        return NULL;
    }
        
    if(!((VIRTUAL_HEAP*)heapstart)->tree_length) {
        // puts("tree_length == 0");
        return NULL;
    }
        
    // uint32_t* index = virtual_sbrk(sizeof(uint32_t));
    // uint32_t* node_size = virtual_sbrk(sizeof(uint32_t));
    // void* ptr = virtual_sbrk(sizeof(void*));
    ((VIRTUAL_HEAP*)heapstart)->index = 0;
    ((VIRTUAL_HEAP*)heapstart)->node_size = ((VIRTUAL_HEAP*)heapstart)->initial_size;
    // printf("node_size: %d\n", ((VIRTUAL_HEAP*)heapstart)->node_size);

     if(size > pow(2, ((VIRTUAL_HEAP*)heapstart)->tree_1D[0])) {
        // puts("size > tree[0]");
        return NULL;
    }
    // convert to size of power
    size = ceil_power_2(size, ((VIRTUAL_HEAP*)heapstart)->initial_size, ((VIRTUAL_HEAP*)heapstart)->min_size);
    // printf("ceil_size: %d\n", size);
   
    // find the node with correspoinding size from top to bottom
    while(((VIRTUAL_HEAP*)heapstart)->node_size > size) {
        // printf("index: %d\n", ((VIRTUAL_HEAP*)heapstart)->index);
       // if both leaves have available space, choose the smaller one
        if(((VIRTUAL_HEAP*)heapstart)->tree_1D[left_leaf(((VIRTUAL_HEAP*)heapstart)->index)] >= size 
                    && ((VIRTUAL_HEAP*)heapstart)->tree_1D[right_leaf(((VIRTUAL_HEAP*)heapstart)->index)] >= size) {
            if (((VIRTUAL_HEAP*)heapstart)->tree_1D[left_leaf(((VIRTUAL_HEAP*)heapstart)->index)] <= ((VIRTUAL_HEAP*)heapstart)->tree_1D[right_leaf(((VIRTUAL_HEAP*)heapstart)->index)])
                    ((VIRTUAL_HEAP*)heapstart)->index = left_leaf(((VIRTUAL_HEAP*)heapstart)->index);        
            else
                    ((VIRTUAL_HEAP*)heapstart)->index = right_leaf(((VIRTUAL_HEAP*)heapstart)->index); 
        }
        // if only one leaf has enough space
        else if(((VIRTUAL_HEAP*)heapstart)->tree_1D[left_leaf(((VIRTUAL_HEAP*)heapstart)->index)] >= size) 
            ((VIRTUAL_HEAP*)heapstart)->index = left_leaf(((VIRTUAL_HEAP*)heapstart)->index);
        else 
            ((VIRTUAL_HEAP*)heapstart)->index = right_leaf(((VIRTUAL_HEAP*)heapstart)->index);
        ((VIRTUAL_HEAP*)heapstart)->node_size -= 1;               
    }

    // beginning of buddy allocator block + offset
    // ( offset = (index + 1) * block_size - total_size)
    ((VIRTUAL_HEAP*)heapstart)->offset = (int)pow(2, ((VIRTUAL_HEAP*)heapstart)->node_size) * (((VIRTUAL_HEAP*)heapstart)->index + 1) - (int)pow(2, ((VIRTUAL_HEAP*)heapstart)->initial_size);

    reassign_value(heapstart, ((VIRTUAL_HEAP*)heapstart)->index);
    return heapstart + ((VIRTUAL_HEAP*)heapstart)->data_start + ((VIRTUAL_HEAP*)heapstart)->offset;

}

int find_index(void* heapstart, void* ptr) {

     if(ptr == NULL || heapstart == NULL || ((VIRTUAL_HEAP*)heapstart)->offset < 0) { 
        // puts("wrong 1");
        return 1;
    }

    ((VIRTUAL_HEAP*)heapstart)->offset = (char*)ptr - (char*)heapstart - (int)((VIRTUAL_HEAP*)heapstart)->data_start;
        // error occurs when ptr out of bound, e.g. before or after data block of virtual heap
   
    
    if( ((VIRTUAL_HEAP*)heapstart)->offset > pow(2, ((VIRTUAL_HEAP*)heapstart)->initial_size)) {
        // puts("wrong 2");
        return 1;
    }

    // uint32_t* index = virtual_sbrk(sizeof(uint32_t));
    // uint32_t* node_size = virtual_sbrk(sizeof(uint32_t));
    
    // printf("offset: %d\n", ((VIRTUAL_HEAP*)heapstart)->offset);
    // assign index to the bottom of the tree
    ((VIRTUAL_HEAP*)heapstart)->node_size = ((VIRTUAL_HEAP*)heapstart)->min_size;
    // printf("node_size: %d\n", ((VIRTUAL_HEAP*)heapstart)->node_size);

    // index = offset... here we assure the smallest block
    // ( offset = (index + 1) * block_size - total_size)
    ((VIRTUAL_HEAP*)heapstart)->index = ((VIRTUAL_HEAP*)heapstart)->offset / (int)pow(2, ((VIRTUAL_HEAP*)heapstart)->node_size) + (((VIRTUAL_HEAP*)heapstart)->tree_length + 1)/2 - 1;
    // printf("index: %d\n", ((VIRTUAL_HEAP*)heapstart)->index);
    // printf("index holds: %d\n", ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index]);
        // and trace back till we find 0 (occupied) node
    for (; ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index] ; ((VIRTUAL_HEAP*)heapstart)->index = parent(((VIRTUAL_HEAP*)heapstart)->index)) {
            ((VIRTUAL_HEAP*)heapstart)->node_size += 1;
        // ptr is not a pointer to a block of memory that was previously allocated, return non-zero
        if (!((VIRTUAL_HEAP*)heapstart)->index && ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index]) {
            // printf("index: %d\n", ((VIRTUAL_HEAP*)heapstart)->index);
            // printf("index holds: %d\n", ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index]);
            // puts("wrong 3");
            return 1;
        }
    }
    // set to the original value of this block
    ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index] = ((VIRTUAL_HEAP*)heapstart)->node_size;
    // set the current offset for realloc, since we are modifying index, we need offset & node_size to find index
    ((VIRTUAL_HEAP*)heapstart)->offset = (int)pow(2, ((VIRTUAL_HEAP*)heapstart)->node_size) * (((VIRTUAL_HEAP*)heapstart)->index + 1) - (int)pow(2, ((VIRTUAL_HEAP*)heapstart)->initial_size);
    // printf("index now holds: %d\n", ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index]);
    // printf("free %d: \n", ((VIRTUAL_HEAP*)heapstart)->node_size);
    return 0;
}

void merge_block(void* heapstart) {
    while (((VIRTUAL_HEAP*)heapstart)->index) {
        ((VIRTUAL_HEAP*)heapstart)->node_size += 1;
        ((VIRTUAL_HEAP*)heapstart)->index = parent(((VIRTUAL_HEAP*)heapstart)->index);
       // check whether can be merged
        if(((VIRTUAL_HEAP*)heapstart)->tree_1D[left_leaf(((VIRTUAL_HEAP*)heapstart)->index)] == ((VIRTUAL_HEAP*)heapstart)->node_size-1 
            && ((VIRTUAL_HEAP*)heapstart)->tree_1D[right_leaf(((VIRTUAL_HEAP*)heapstart)->index)] == ((VIRTUAL_HEAP*)heapstart)->node_size-1 )
                ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index] = ((VIRTUAL_HEAP*)heapstart)->node_size;
        else {
        // set the available to larger one of the leaves
            if(((VIRTUAL_HEAP*)heapstart)->tree_1D[left_leaf(((VIRTUAL_HEAP*)heapstart)->index)] >= ((VIRTUAL_HEAP*)heapstart)->tree_1D[right_leaf(((VIRTUAL_HEAP*)heapstart)->index)])
                ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index] = ((VIRTUAL_HEAP*)heapstart)->tree_1D[left_leaf(((VIRTUAL_HEAP*)heapstart)->index)];
            else
                ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index] = ((VIRTUAL_HEAP*)heapstart)->tree_1D[right_leaf(((VIRTUAL_HEAP*)heapstart)->index)];
        }
        // printf("index %d now holds: %d\n", ((VIRTUAL_HEAP*)heapstart)->index, ((VIRTUAL_HEAP*)heapstart)->tree_1D[((VIRTUAL_HEAP*)heapstart)->index]);
    }
}

int virtual_free(void * heapstart, void * ptr) {

    if(find_index(heapstart, ptr))
        return 1;
    merge_block(heapstart);
    return 0;
}

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size) {
    // printf("realloc %d: \n", size);
    if(!size) {
        virtual_free(heapstart, ptr);
        return NULL;
    }
    if(ptr == NULL) {
        return virtual_malloc(heapstart, size);
    }
    if(!find_index(heapstart, ptr)) {

        error_check(virtual_sbrk(sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t)));
        uint8_t *node_size = heapstart + ((VIRTUAL_HEAP*)heapstart)->heap_length;
        uint16_t *index = heapstart + ((VIRTUAL_HEAP*)heapstart)->heap_length + sizeof(uint8_t);
        uint32_t *offset = heapstart + ((VIRTUAL_HEAP*)heapstart)->heap_length + sizeof(uint8_t) + sizeof(uint16_t);

        *offset =  ((VIRTUAL_HEAP*)heapstart)->offset;
        *index = ((VIRTUAL_HEAP*)heapstart)->index;
        *node_size = ((VIRTUAL_HEAP*)heapstart)->node_size;  
        merge_block(heapstart);
        void *ptr_temp = virtual_malloc(heapstart, size); 
        // find a suitable size block
        if(ptr_temp != NULL) {
            size = ceil_power_2(size, ((VIRTUAL_HEAP*)heapstart)->initial_size, ((VIRTUAL_HEAP*)heapstart)->min_size);  
            if(size > *node_size) {
                memmove(ptr_temp, heapstart + ((VIRTUAL_HEAP*)heapstart)->data_start + *offset, pow(2, *node_size));
                // printf("memcmp: %d\n", memcmp(ptr_temp, heapstart + ((VIRTUAL_HEAP*)heapstart)->data_start + *offset, *node_size));
            }
            else {
                memmove(ptr_temp, heapstart + ((VIRTUAL_HEAP*)heapstart)->data_start + *offset, pow(2, size));
                // printf("memcmp: %d\n", memcmp(ptr_temp, heapstart + ((VIRTUAL_HEAP*)heapstart)->data_start + *offset, size));
            }
            
            error_check(virtual_sbrk(-(int)(sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t))));
        }
        // not find, revert to original block
        else {
            reassign_value(heapstart, *index);
            ((VIRTUAL_HEAP*)heapstart)->offset = *offset;
            error_check(virtual_sbrk(-(int)(sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t))));
            return NULL;
        }
        
    }
    else 
        return NULL;
    return heapstart + ((VIRTUAL_HEAP*)heapstart)->data_start + ((VIRTUAL_HEAP*)heapstart)->offset;
    
}

int find_node_size(uint8_t node_size, uint16_t index, uint8_t initial_size, uint8_t min_size) {
    node_size = initial_size;
    while(index) {
        // printf("index: %d node_size: %d\n", index, node_size);
        if(power_of_2(index+1)) 
            node_size -= 1;
        index -= 1;    
        // printf("node_size: %d ", node_size);
        // printf("index: %d\n", index);
    }
    if(power_of_2(index+1)) 
        node_size -= 1;
    return node_size;
}

void offset_traversal(void *heapstart, uint16_t index, uint8_t node_size) {
    // printf("index: %d\n", index);
    node_size = find_node_size(node_size, index, ((VIRTUAL_HEAP*)heapstart)->initial_size, ((VIRTUAL_HEAP*)heapstart)->min_size);
    // when both children are empty, parent should not print duplicate info
    if(index < ((((VIRTUAL_HEAP*)heapstart)->tree_length-1))/2 && 
        !((VIRTUAL_HEAP*)heapstart)->tree_1D[left_leaf(index)] && !((VIRTUAL_HEAP*)heapstart)->tree_1D[right_leaf(index)]) {
        return;
    }
        // printf("node_size: %d\n", node_size);

    // when index holds zero
    if(!((VIRTUAL_HEAP*)heapstart)->tree_1D[index]){
        // printf("node size: %d\n", node_size);
        // printf("sum before: %d\n", *sum);
        // sum = new offset - previous sum
        // *sum = (uint32_t)node_size * (index + 1) - (uint32_t)pow(2, ((VIRTUAL_HEAP*)heapstart)->initial_size) - *sum;
        // if(*sum) 
        //     printf("free %d\n", *sum);
        printf("allocated %d\n", (int)pow(2, node_size));
    // when index remains untouched
    }else if(index && ((VIRTUAL_HEAP*)heapstart)->tree_1D[index] ==  find_node_size(node_size, index, ((VIRTUAL_HEAP*)heapstart)->initial_size, ((VIRTUAL_HEAP*)heapstart)->min_size)){
        if (((VIRTUAL_HEAP*)heapstart)->tree_1D[parent(index)]){ 
            // node on the left
            if(index%2 && ((VIRTUAL_HEAP*)heapstart)->tree_1D[index+1] !=  find_node_size(node_size, index+1, ((VIRTUAL_HEAP*)heapstart)->initial_size, ((VIRTUAL_HEAP*)heapstart)->min_size))
                printf("free %d\n", (int)pow(2, find_node_size(node_size, index, ((VIRTUAL_HEAP*)heapstart)->initial_size, ((VIRTUAL_HEAP*)heapstart)->min_size)));
            // node on the right
            else if (!(index%2) && ((VIRTUAL_HEAP*)heapstart)->tree_1D[index-1] !=  find_node_size(node_size, index-1, ((VIRTUAL_HEAP*)heapstart)->initial_size, ((VIRTUAL_HEAP*)heapstart)->min_size))
                printf("free %d\n", (int)pow(2, find_node_size(node_size, index, ((VIRTUAL_HEAP*)heapstart)->initial_size, ((VIRTUAL_HEAP*)heapstart)->min_size)));
        }
    }
    {
        
        // printf("parent size: %d\n", !((VIRTUAL_HEAP*)heapstart)->tree_1D[parent(index)]);
        // printf("%d %d %d\n", !index, !(index%2), ((VIRTUAL_HEAP*)heapstart)->tree_1D[index-1] != find_node_size(node_size, index-1, ((VIRTUAL_HEAP*)heapstart)->initial_size, ((VIRTUAL_HEAP*)heapstart)->min_size));
        // *sum += node_size;
    }
    // printf("sum after: %d\n", *sum);
    // when reach top or last node
    if(!index || index >= (((VIRTUAL_HEAP*)heapstart)->tree_length-1))
        return;
    // when index is on the left
    if(index%2) {
        offset_traversal(heapstart, parent(index), node_size);
    }
        
    // bottom of tree, has no leaf, move right
    if(index >= ((((VIRTUAL_HEAP*)heapstart)->tree_length-1))/2) 
        offset_traversal(heapstart, index+1, node_size);
}

void virtual_info(void * heapstart) {
    // puts("info:");
    //  for(int i = 0; i < ((VIRTUAL_HEAP*)heapstart)->tree_length; i++){
    //     if (power_of_2(i+1)){
    //         puts("");
    //     }   
    //     printf("%d ", ((VIRTUAL_HEAP*)heapstart)->tree_1D[i]);
    // }
    // puts("");
    if(((VIRTUAL_HEAP*)heapstart)->tree_1D[0] == ((VIRTUAL_HEAP*)heapstart)->initial_size) {
        printf("free %d\n", (int)pow(2, ((VIRTUAL_HEAP*)heapstart)->initial_size));
        return;
    }
    ((VIRTUAL_HEAP*)heapstart)->index = (((VIRTUAL_HEAP*)heapstart)->tree_length-1)/2;
    ((VIRTUAL_HEAP*)heapstart)->node_size = ((VIRTUAL_HEAP*)heapstart)->min_size;
    // starts from left most smallest block
    offset_traversal(heapstart, ((VIRTUAL_HEAP*)heapstart)->index, ((VIRTUAL_HEAP*)heapstart)->node_size);
    // if(*sum != (int)pow(2, ((VIRTUAL_HEAP*)heapstart)->initial_size)) 
    //     printf("free %d\n", (int)pow(2, ((VIRTUAL_HEAP*)heapstart)->initial_size) - *sum);
    
    // virtual_sbrk(-(int32_t)(2*sizeof(uint32_t)));
    // Your code here
}