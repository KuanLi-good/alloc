#include "tests.h"
#include "virtual_sbrk.h"

int compareFiles(FILE *fp1, FILE *fp2) {
    char ch1 = getc(fp1);
    char ch2 = getc(fp2);

    int line = 1;
  
    // iterate loop till end of file
    while (ch1 != EOF && ch2 != EOF)
    {
        if (ch1 == '\n' && ch2 == '\n')
            line++;

        if (ch1 != ch2)
            return line;
  
        // fetching character until end of file
        ch1 = getc(fp1);
        ch2 = getc(fp2);
    }
    return 0;
}

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
    init_allocator_test();
    sbrk_error_test();
    heap_null_test();
    virtual_malloc_zero_test();
    virtual_malloc_max_test();
    virtual_malloc_test();
    virtual_free_null_test();
    virtual_free_not_match_test();
    virtual_free_test();
    virtual_realloc_zero_test();
    virtual_realloc_null_test();
    virtual_realloc_test();

    fp = freopen ("/dev/tty", "a", stdout);
    FILE *fp1 = fopen("tests.txt", "r");
    FILE *fp2 = fopen("tests_expected.txt", "r");
    int i = compareFiles(fp1, fp2);
    
    if(i)
        printf("Files dont match, check from line %d\n", i);
    else
        printf("Passed\n");
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    return 0;
}

void init_allocator_test() {
    virtual_heap = malloc(17000000);
    fp = freopen("tests.txt", "w", stdout);
    puts("init_allocator_test");
    init_allocator(virtual_heap, 23, 10);
    // printf("heap length: %d, sbrk-heapstart: %ld\n", ((VIRTUAL_HEAP*)virtual_heap)->heap_length, (char*)virtual_sbrk(0) - (char*)virtual_heap-1);
    assert(((VIRTUAL_HEAP*)virtual_heap)->heap_length == (int)((char*)virtual_sbrk(0) -(char*)virtual_heap -1));
    assert(((VIRTUAL_HEAP*)virtual_heap)->tree_1D != NULL);
    assert(((VIRTUAL_HEAP*)virtual_heap)->tree_length > 0);
    assert(((VIRTUAL_HEAP*)virtual_heap)->data_start > 0);

    puts("passed");
    fclose(fp);
    free(virtual_heap);
    program_break = NULL;

}

void sbrk_error_test() {
    virtual_heap = malloc(100);
    fp = freopen("tests.txt", "a", stdout);
    puts("\nsbrk_error_test");
    init_allocator(virtual_heap, 14, 5);
    
    fclose(fp);
    free(virtual_heap);
    program_break = NULL;
}

void heap_null_test() {
    virtual_heap = malloc(700000);
    init_allocator(virtual_heap, 12, 5);
    void *p1 = virtual_malloc(NULL, 300);
    assert(p1 == NULL);
    void *p2 = virtual_malloc(virtual_heap, 300);
    int i = virtual_free(NULL, p2);
    assert(i);
    p1 = virtual_realloc(NULL, p2, 3000);
    assert(p1 == NULL);
    free(virtual_heap);
    program_break = NULL;
}

void virtual_malloc_zero_test() {
    virtual_heap = malloc(700000);
    fp = freopen("tests.txt", "a", stdout);
    puts("virtual_malloc_zero_test");
    init_allocator(virtual_heap, 12, 5);
    void *p1 = virtual_malloc(virtual_heap, 0);
    assert(p1 == NULL);
    puts("passed");
    
    fclose(fp);
    free(virtual_heap);
    program_break = NULL;
}

void virtual_malloc_max_test() {
    virtual_heap = malloc(700000);
    fp = freopen("tests.txt", "a", stdout);
    puts("\nvirtual_malloc_max_test");
    init_allocator(virtual_heap, 10, 5);
    void *p1 = virtual_malloc(virtual_heap, 200000);
    virtual_info(virtual_heap);
    assert(!p1);
    puts("passed");
    
    fclose(fp);
    free(virtual_heap);
    program_break = NULL;
}

void virtual_malloc_test() {
    // return the leftmost ceiled unallocated block.
    virtual_heap = malloc(700000);
    fp = freopen("tests.txt", "w", stdout);
    puts("\nvirtual_malloc_test");
    init_allocator(virtual_heap, 12, 5);
    puts("mallloc 2000: ");
    virtual_malloc(virtual_heap, 2000);
    virtual_info(virtual_heap);
    puts("\nmallloc 2^7: ");
    virtual_malloc(virtual_heap, pow(2, 7));
    virtual_info(virtual_heap);

    puts("\nmallloc 30: ");
    virtual_malloc(virtual_heap, 30);
    virtual_info(virtual_heap);

    puts("\nmallloc 2^5: ");
    virtual_malloc(virtual_heap, pow(2, 7));
    virtual_info(virtual_heap);
    
    fclose(fp);
    free(virtual_heap);
    program_break = NULL;
}

void virtual_free_null_test() {
    virtual_heap = malloc(700000);
    fp = freopen("tests.txt", "a", stdout);
    puts("\nvirtual_free_null_test");
    init_allocator(virtual_heap, 14, 5);
        
    puts("\nfree NULL: ");
    int i = virtual_free(virtual_heap, NULL);
    assert(i);

    fclose(fp);
    free(virtual_heap);
    program_break = NULL;
}

void virtual_free_not_match_test() {
    virtual_heap = malloc(700000);
    fp = freopen("tests.txt", "a", stdout);
    puts("\nvirtual_free_test");
    init_allocator(virtual_heap, 14, 5);
    puts("virtual sbrk p1 2000: ");
    void *p1 = virtual_sbrk(2000);

    puts("\nfree p1: ");
    int i = virtual_free(virtual_heap, p1);
    assert(i);

    fclose(fp);
    free(virtual_heap);
    program_break = NULL;
}

void virtual_free_test() {
    virtual_heap = malloc(700000);
    fp = freopen("tests.txt", "a", stdout);
    puts("\nvirtual_free_test");
    init_allocator(virtual_heap, 14, 5);
    puts("mallloc p1 2000: ");
    void *p1 = virtual_malloc(virtual_heap, 2000);
    puts("\nmallloc p2 2^7: ");
    void *p2 = virtual_malloc(virtual_heap, pow(2, 7));
        
    puts("\nfree p2: ");
    virtual_free(virtual_heap, p2);
    virtual_info(virtual_heap);
    
    puts("\nfree p1: ");
    virtual_free(virtual_heap, p1);
    virtual_info(virtual_heap);
    
    fclose(fp);
    free(virtual_heap);
    program_break = NULL;
}

void virtual_realloc_zero_test() {
    virtual_heap = malloc(700000);
    fp = freopen("tests.txt", "a", stdout);
    puts("\nvirtual_free_test");
    init_allocator(virtual_heap, 14, 5);
    puts("mallloc p1 2000: ");
    void *p1 = virtual_malloc(virtual_heap, 2000);
    virtual_info(virtual_heap);
    puts("realloc p1 30: ");
    p1 = virtual_realloc(virtual_heap, p1, 0);
    virtual_info(virtual_heap);
    assert(p1 == NULL);

    fclose(fp);
    free(virtual_heap);
    program_break = NULL;
}

void virtual_realloc_null_test() {
    virtual_heap = malloc(700000);
    fp = freopen("tests.txt", "a", stdout);
    puts("\nvirtual_free_null_test");
    init_allocator(virtual_heap, 14, 5);
    void *p1 = NULL;
    puts("\nrealloc null p1 30: ");
    p1 = virtual_realloc(virtual_heap, p1, 30);
    virtual_info(virtual_heap);
    assert(p1 != NULL);

    fclose(fp);
    free(virtual_heap);
    program_break = NULL;
}

void virtual_realloc_test() {
    virtual_heap = malloc(700000);
    fp = freopen("tests.txt", "a", stdout);
    puts("\nvirtual_free_test");
    init_allocator(virtual_heap, 14, 5);
    puts("\nmallloc p1 2000: ");
    void *p1 = virtual_malloc(virtual_heap, 2000);
    puts("\nrealloc p1 30: ");
    p1 = virtual_realloc(virtual_heap, p1, 30);
    puts("\nmallloc p2 2^7: ");
    void *p2 = virtual_malloc(virtual_heap, pow(2, 7));
        
    puts("\nfree p2: ");
    virtual_free(virtual_heap, p2);
    virtual_info(virtual_heap);
    
    puts("\nfree p1: ");
    virtual_free(virtual_heap, p1);
    virtual_info(virtual_heap);

    fclose(fp);
    free(virtual_heap);
    program_break = NULL;
}
