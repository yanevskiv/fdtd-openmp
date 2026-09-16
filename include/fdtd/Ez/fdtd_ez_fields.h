// Author: Ivan Janevski (C) 2026

#ifndef _FDTD_EZ_FIELDS_H_
#define _FDTD_EZ_FIELDS_H_

#include <stddef.h>
#include <fdtd/fdtd_types.h>

// Fdtd_EzFields - Electromagnetic fields for a 1D simulation
typedef struct {
    size_t ff_width;       // Number of cells
    Fdtd_FloatType *ff_hy; // Magnetic field H_y
    Fdtd_FloatType *ff_ez; // Electric field E_z
} Fdtd_EzFields;

// Create fields for a 1D simulation of the given width
Fdtd_EzFields *Fdtd_EzFieldsCreate(size_t width);

// Reset E_z and H_y fields to zero
void Fdtd_EzFieldsReset(Fdtd_EzFields *fields);

// Inject a value into the electric field E_z in cell kc (soft source)
void Fdtd_EzFieldsInjectEz(Fdtd_EzFields *fields, size_t kc, Fdtd_FloatType value);

// Free field memory
void Fdtd_EzFieldsDestroy(Fdtd_EzFields *fields);

#endif
