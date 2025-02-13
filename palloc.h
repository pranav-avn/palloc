#ifndef PALLOC_H
#define PALLOC_H

#include <stddef.h>

// Function to allocate memory
void* palloc(size_t size);

// Function to free allocated memory
void pfree(void* block);

// Function to allocate memory for an array of elements
void* pcalloc(size_t num, size_t nsize);

// Function to reallocate memory
void* prealloc(void* block, size_t size);

#endif // PALLOC_H