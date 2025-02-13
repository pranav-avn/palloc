#ifndef PALLOC_H
#define PALLOC_H

#include <stddef.h>

// Function to allocate memory
void* palloc(size_t size);

// Function to free allocated memory
void pfree(void* ptr);

#endif // PALLOC_H