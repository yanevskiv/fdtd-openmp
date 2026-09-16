// Author: Ivan Janevski (C) 2026

#ifndef _FDTD_EZ_COEFFS_H_
#define _FDTD_EZ_COEFFS_H_

#include <stddef.h>
#include <fdtd/fdtd_types.h>
#include <fdtd/Ez/fdtd_ez_state.h>
#include <fdtd/Ez/fdtd_ez_material.h>

// Fdtd_EzCoeffs - Update coefficients for the 1D simulation equations
typedef struct {
    size_t fc_width;           // Number of cells
    Fdtd_FloatType *fc_ch1;    // Coefficient multiplying H_y in the H_y equation
    Fdtd_FloatType *fc_ch2;    // Coefficient multiplying the curl of E_z
    Fdtd_FloatType *fc_ce1;    // Coefficient multiplying E_z in the E_z equation
    Fdtd_FloatType *fc_ce2;    // Coefficient multiplying the curl of H_y
} Fdtd_EzCoeffs;

// Create coefficients for a 1D simulation of the given width
Fdtd_EzCoeffs *Fdtd_EzCoeffsCreate(size_t width);

// Compute coefficients from the material and simulation parameters
void Fdtd_EzCoeffsUpdate(Fdtd_EzCoeffs *coeffs, const Fdtd_EzMaterial *material, const Fdtd_EzState *state);

// Free coefficient memory
void Fdtd_EzCoeffsDestroy(Fdtd_EzCoeffs *coeffs);

#endif
