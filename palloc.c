#include <unistd.h>
#include <pthread.h>

typedef char ALIGN[16];

union header{
    struct header_t{
        size_t size;
        unsigned is_free;
        struct header_t *next;
    } s;
    /* force the header to wrap to 16 bytes */
    ALIGN stub;
};

typedef union header header_t;

header_t *head, *tail;
pthread_mutex_t global_malloc_lock;

void *palloc(size_t size){
    /**
     * Allocates a block of memory of the specified size and returns a pointer to the beginning of the block.
     * The content of the newly allocated memory is not initialized, it contains garbage values.
     *
     * @param size The size of the memory block to allocate in bytes.
     * @return A pointer to the allocated memory block, or NULL if the allocation fails.
     *
     * Example usage:
     * @code
     * int *array = (int *)malloc(10 * sizeof(int));
     * if (array == NULL) {
     *     // Handle memory allocation failure
     * }
     * @endcode
     *
     * Note:
     * - Always check if the returned pointer is NULL to ensure that the memory allocation was successful.
     * - The allocated memory should be freed using the free() function to avoid memory leaks.
     */
    size_t total_size;
    void *block;
    header_t *header;
    if(!size)
        return NULL;
    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);
    if(header){
        header->s.is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock);
        return (void*)(header + 1); //return the address after the header (so that the header remains hidden to end user)
    }
    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);
    if(block == (void*)-1){
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }
    header = block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;
    if (!head)
        head = header;
    if (tail)
        tail->s.next = header;
    tail = header;
    pthread_mutex_unlock(&global_malloc_lock);
    return (void*)(header + 1);
}

header_t *get_free_block(size_t size){
    header_t *curr = head;
    while(curr){
        if(curr->s.is_free && curr->s.size >= size)
            return curr;
        curr = curr->s.next;
    }
    return NULL;
}

void pfree(void *block){
    /**
     * @brief Frees the memory space pointed to by ptr, which must have been returned by a previous call to malloc(), calloc(), or realloc().
     * 
     * This function deallocates the memory previously allocated by a memory allocation function. If ptr is NULL, no operation is performed.
     * 
     * @param block Pointer to the memory block to be freed.
     */
    header_t *header, *tmp;
    void *prgbreak;

    if(!block)
        return;
    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t*)block - 1; //obtain the header of the block

    prgbreak = sbrk(0);
    if((char*)block + header->s.size == prgbreak){
        if(head == tail){
            head = tail = NULL;
        } else {
            tmp = head;
            while(tmp){
                if(tmp->s.next == tail){
                    tmp->s.next = NULL;
                    tail = tmp;
                }
                tmp = tmp->s.next;
            }
        }
        sbrk(0 - sizeof(header_t) - header->s.size); //sbrk(-total_size) to free the block
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    header->s.is_free = 1;
    pthread_mutex_unlock(&global_malloc_lock);
}

void *calloc(size_t num, size_t nsize){
    /**
     * Allocates memory for an array of `num` elements of `size` bytes each and 
     * initializes all bytes in the allocated storage to zero.
     *
     * @param num The number of elements to allocate.
     * @param nsize The size of each element in bytes.
     * @return A pointer to the allocated memory, or NULL if the allocation fails.
     *
     * The `calloc` function allocates memory for an array of `num` elements, each 
     * of `size` bytes, and initializes all bytes in the allocated memory to zero. 
     * If the allocation fails, it returns NULL.
     *
     * Example usage:
     * @code
     * int *arr = (int *)calloc(10, sizeof(int));
     * if (arr == NULL) {
     *     // Handle allocation failure
     * }
     * @endcode
     */
     size_t size;
     void *block;
     if(!num || !nsize)
        return NULL;
    size = num * nsize;

    /* check for integer overflow*/
    if(nsize != size / num)
        return NULL;
    block = malloc(size);
    
    if(!block)
        return NULL;
    
    memset(block, 0, size); //init all bytes to 0

    return block;
}

void *realloc(void *block, size_t size){
    /**
     * Reallocates memory for an array of elements.
     *
     * @param block A pointer to the memory block previously allocated with malloc, calloc, or realloc.
     *            If this is NULL, the function behaves like malloc.
     * @param size The new size for the memory block, in bytes.
     *             If this is 0 and ptr is not NULL, the memory block is freed.
     * @return A pointer to the newly allocated memory, which may be different from ptr.
     *         If the function fails to allocate the requested block of memory, a NULL pointer is returned,
     *         and the original block of memory is left untouched.
     *
     * @note The contents of the memory block are unchanged up to the lesser of the new and old sizes.
     *       If the new size is larger, the value of the newly allocated portion is indeterminate.
     */
    header_t *header;
    void *ret;
    if(!block || !size)
        return malloc(size);
    header = (header_t*)block - 1;
    if(header->s.size >= size)
        return block;
    ret = malloc(size);
    if(ret){
        memcpy(ret, block, header->s.size);
        free(block);
    }
    return ret;
}