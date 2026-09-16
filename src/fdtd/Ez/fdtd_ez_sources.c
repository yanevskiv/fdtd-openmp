// Author: Ivan Janevski (C) 2026

#include <assert.h>
#include <fdtd.h>

// Create a source list with the given capacity
Fdtd_EzSources *Fdtd_EzSourcesCreate(size_t capacity)
{
    // Create the structure
    Fdtd_EzSources *sources = (Fdtd_EzSources *) Fdtd_Alloc(sizeof(Fdtd_EzSources));

    // Fixed-capacity source array
    sources->fsrc_items = (Fdtd_EzSource *) Fdtd_AllocArray(capacity, sizeof(Fdtd_EzSource));
    sources->fsrc_size = capacity;
    sources->fsrc_count = 0;
    return sources;
}

// Free source-list memory
void Fdtd_EzSourcesDestroy(Fdtd_EzSources *sources)
{
    // Validate pointers
    if (sources == NULL) {
        return;
    }

    // Free the array and structure
    Fdtd_FreeArray(sources->fsrc_items);
    Fdtd_Free(sources);
}

// Register a source (data is copied by value)
void Fdtd_EzSourcesAdd(Fdtd_EzSources *sources, Fdtd_EzSourceFn fn, Fdtd_EzSourceData data)
{
    // Check for capacity overflow
    assert(sources->fsrc_count < sources->fsrc_size);

    // Append the source to the list
    Fdtd_EzSource *slot = &sources->fsrc_items[sources->fsrc_count];
    slot->fsrc_fn = fn;
    slot->fsrc_data = data;
    sources->fsrc_count++;
}

// Apply all sources to the simulation (call every registered callback)
void Fdtd_EzSourcesApply(Fdtd_EzSources *sources, Fdtd_EzSim *sim)
{
    for (size_t i = 0; i < sources->fsrc_count; i++) {
        Fdtd_EzSource *src = &sources->fsrc_items[i];
        src->fsrc_fn(sim, &src->fsrc_data);
    }
}
