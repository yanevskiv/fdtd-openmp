// Author: Ivan Janevski (C) 2026

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fdtd.h>

// Allocate zeroed memory of size bytes (a replacement for calloc)
void *Fdtd_Alloc(size_t size)
{
    void *ptr = calloc(1, size);
    assert(ptr != NULL);
    return ptr;
}

// Free memory allocated with Fdtd_Alloc (a replacement for free)
void Fdtd_Free(void *ptr)
{
    free(ptr);
}

// Allocate aligned and zeroed memory for an array of count elements of size size
void *Fdtd_AllocArray(size_t count, size_t size)
{
    // Round the size up to an alignment multiple (required by aligned_alloc)
    size_t bytes = ((count * size + FDTD_ALIGN - 1) / FDTD_ALIGN) * FDTD_ALIGN;
    void *ptr = aligned_alloc(FDTD_ALIGN, bytes);
    assert(ptr != NULL);

    // Initialize to zero
    memset(ptr, 0, bytes);
    return ptr;
}

// Free an array allocated with Fdtd_AllocArray
void Fdtd_FreeArray(void *array)
{
    free(array);
}
