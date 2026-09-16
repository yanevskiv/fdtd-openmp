// Author: Ivan Janevski (C) 2026

#ifndef _FDTD_EZ_SOURCES_H_
#define _FDTD_EZ_SOURCES_H_

#include <stddef.h>
#include <fdtd/fdtd_types.h>

// Default source-list capacity
#define FDTD_EZ_SOURCES_DEFAULT_CAP 16

// Simulation
typedef struct Fdtd_EzSim Fdtd_EzSim;

// Source data
typedef struct {
    void  *data;
    Fdtd_FloatType f1; 
    Fdtd_FloatType f2;
    Fdtd_FloatType f3;
    Fdtd_FloatType f4;
    Fdtd_FloatType f5;
    Fdtd_FloatType f6;
} Fdtd_EzSourceData;

// Source callback function
typedef void (*Fdtd_EzSourceFn)(Fdtd_EzSim *sim, Fdtd_EzSourceData *data);

// Source
typedef struct {
    Fdtd_EzSourceFn   fsrc_fn;    // Callback that injects the source
    Fdtd_EzSourceData fsrc_data;  // Parameters passed to the callback
} Fdtd_EzSource;

// Source list
typedef struct {
    Fdtd_EzSource *fsrc_items;   // Array of registered sources
    size_t       fsrc_size;    // Array capacity
    size_t       fsrc_count;   // Number of registered sources
} Fdtd_EzSources;

// Create a source list with the given capacity
Fdtd_EzSources *Fdtd_EzSourcesCreate(size_t capacity);

// Free source-list memory
void Fdtd_EzSourcesDestroy(Fdtd_EzSources *sources);

// Register a source (data is copied by value)
void Fdtd_EzSourcesAdd(Fdtd_EzSources *sources, Fdtd_EzSourceFn fn, Fdtd_EzSourceData data);

// Apply all sources to the simulation (call every registered callback)
void Fdtd_EzSourcesApply(Fdtd_EzSources *sources, Fdtd_EzSim *sim);

#endif
