// Author: Ivan Janevski (C) 2026

#include <fdtd.h>

// Create fields for a 1D simulation of the given width
Fdtd_EzFields *Fdtd_EzFieldsCreate(size_t width)
{
    // Create the structure
    Fdtd_EzFields *fields = (Fdtd_EzFields *) Fdtd_Alloc(sizeof(Fdtd_EzFields));

    // Electromagnetic fields (aligned memory for vectorization)
    fields->ff_width = width;
    fields->ff_hy = Fdtd_AllocArray(width, sizeof(Fdtd_FloatType));
    fields->ff_ez = Fdtd_AllocArray(width, sizeof(Fdtd_FloatType));
    return fields;
}

// Reset E_z and H_y fields to zero
void Fdtd_EzFieldsReset(Fdtd_EzFields *fields)
{
    size_t width = fields->ff_width;
    for (size_t k = 0; k < width; k++) {
        fields->ff_hy[k] = 0.0;
        fields->ff_ez[k] = 0.0;
    }
}

// Inject a value into the electric field E_z in cell kc (soft source)
void Fdtd_EzFieldsInjectEz(Fdtd_EzFields *fields, size_t kc, Fdtd_FloatType value)
{
    // Soft source: add the value to the current electric field
    fields->ff_ez[kc] += value;
}

// Free field memory
void Fdtd_EzFieldsDestroy(Fdtd_EzFields *fields)
{
    Fdtd_FreeArray(fields->ff_hy);
    Fdtd_FreeArray(fields->ff_ez);
    Fdtd_Free(fields);
}
