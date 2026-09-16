// Author: Ivan Janevski (C) 2026

#ifndef _FDTD_EZ_MATERIAL_H_
#define _FDTD_EZ_MATERIAL_H_

#include <stddef.h>
#include <fdtd/fdtd_types.h>

// Left boundary
#define FDTD_EZ_MATERIAL_START ((size_t) 0)

// Right boundary
#define FDTD_EZ_MATERIAL_END   ((size_t) -1)

// Material
typedef struct {
    size_t fm_width;    // Number of cells
    Fdtd_FloatType *fm_mu;       // Permeability mu (H/m)
    Fdtd_FloatType *fm_eps;      // Permittivity eps (F/m)
    Fdtd_FloatType *fm_sigma_m;  // Magnetic conductivity sigma* (Ohm/m)
    Fdtd_FloatType *fm_sigma_e;  // Electric conductivity sigma (S/m)
} Fdtd_EzMaterial;

// Create a material (initialized to vacuum)
Fdtd_EzMaterial *Fdtd_EzMaterialCreate(size_t width);

// Set permittivity `eps`
void Fdtd_EzMaterialSetEps(Fdtd_EzMaterial *material, Fdtd_FloatType eps, size_t kstart, size_t kend);

// Set permeability `mu`
void Fdtd_EzMaterialSetMu(Fdtd_EzMaterial *material, Fdtd_FloatType mu, size_t kstart, size_t kend);

// Set the PML absorbing layer (both boundaries)
void Fdtd_EzMaterialSetPML(Fdtd_EzMaterial *material, size_t pml_width, Fdtd_FloatType dx);

// Set the PEC reflecting boundary (both boundaries)
void Fdtd_EzMaterialSetPEC(Fdtd_EzMaterial *material, size_t pec_width, Fdtd_FloatType dx);

// Free memory
void Fdtd_EzMaterialDestroy(Fdtd_EzMaterial *material);

#endif
