#include <unistd.h>
#include <stdio.h> //debug printf

typedef char ALIGN[16];

union header{
    struct header_t{
        size_t size;
        unsigned is_free;
        struct header_t *next;
    } s;
    ALIGN stub;
};

typedef union header header_t;

void *malloc(size_t size){
    void *block;
    block = sbrk(size);
    if(block == (void*) -1) return NULL;
    return block;
}


int main(){
    printf("Hello World\n");
    return 0;
}