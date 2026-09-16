// Author: Ivan Janevski (C) 2026

#ifndef _FDTD_ALLOC_H_
#define _FDTD_ALLOC_H_

#include <stddef.h>

// Array memory alignment (in bytes) for efficient vectorization
#define FDTD_ALIGN 64

// Allocate zeroed memory of size bytes (a replacement for calloc)
void *Fdtd_Alloc(size_t size);

// Free memory allocated with Fdtd_Alloc (a replacement for free)
void Fdtd_Free(void *ptr);

// Allocate aligned and zeroed memory for an array of count elements of size size
void *Fdtd_AllocArray(size_t count, size_t size);

// Free an array allocated with Fdtd_AllocArray
void Fdtd_FreeArray(void *array);

#endif
