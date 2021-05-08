#ifndef TESTS_H
#define TESTS_H

#include "virtual_alloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void * virtual_heap = NULL;
void *program_break = NULL;
FILE *fp;

int compareFiles(FILE *fp1, FILE *fp2);
void * virtual_sbrk(int32_t increment);
void init_allocator_test();
void sbrk_error_test();
void heap_null_test();
void virtual_malloc_zero_test();
void virtual_malloc_max_test();
void virtual_malloc_test();
void virtual_free_null_test();
void virtual_free_not_match_test();
void virtual_free_test();
void virtual_realloc_zero_test();
void virtual_realloc_null_test();
void virtual_realloc_test();

#endif