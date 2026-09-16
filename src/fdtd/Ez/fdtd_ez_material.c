// Author: Ivan Janevski (C) 2026

#include <tgmath.h>
#include <fdtd.h>

// Normalized depth of point x within the PML layer (0 inside, 1 at the edge)
static Fdtd_FloatType Fdtd_PmlDepth(Fdtd_FloatType x, size_t pml_width, size_t width)
{
    Fdtd_FloatType n = (Fdtd_FloatType) pml_width;

    // Left PML layer
    if (x < n) {
        return (n - x) / n;
    }

    // Right PML layer
    if (x >= (Fdtd_FloatType) (width - pml_width)) {
        return (x - (Fdtd_FloatType) (width - pml_width) + 1.0) / n;
    }

    // Lossless interior
    return 0.0;
}

// Create the material for a 1D simulation (initialized to vacuum)
Fdtd_EzMaterial *Fdtd_EzMaterialCreate(size_t width)
{
    // Create the structure
    Fdtd_EzMaterial *material = (Fdtd_EzMaterial *) Fdtd_Alloc(sizeof(Fdtd_EzMaterial));

    // Allocate material arrays (aligned memory for vectorization)
    material->fm_width = width;
    material->fm_mu = Fdtd_AllocArray(width, sizeof(Fdtd_FloatType));
    material->fm_eps = Fdtd_AllocArray(width, sizeof(Fdtd_FloatType));
    material->fm_sigma_m = Fdtd_AllocArray(width, sizeof(Fdtd_FloatType));
    material->fm_sigma_e = Fdtd_AllocArray(width, sizeof(Fdtd_FloatType));

    // Initialize the material to free space (vacuum)
    for (size_t k = 0; k < width; k++) {
        material->fm_mu[k] = FDTD_MU0;
        material->fm_eps[k] = FDTD_EPS0;
        material->fm_sigma_m[k] = 0.0;
        material->fm_sigma_e[k] = 0.0;
    }
    return material;
}

// Set permittivity eps
void Fdtd_EzMaterialSetEps(Fdtd_EzMaterial *material, Fdtd_FloatType eps, size_t kstart, size_t kend)
{
    size_t width = material->fm_width;
    for (size_t k = kstart; k < kend && k < width; k++) {
        material->fm_eps[k] = eps;
    }
}

// Set permeability mu
void Fdtd_EzMaterialSetMu(Fdtd_EzMaterial *material, Fdtd_FloatType mu, size_t kstart, size_t kend)
{
    size_t width = material->fm_width;
    for (size_t k = kstart; k < kend && k < width; k++) {
        material->fm_mu[k] = mu;
    }
}

// Set conductivity sigma_e, sigma_m (absorbing PML boundary)
void Fdtd_EzMaterialSetPML(Fdtd_EzMaterial *material, size_t pml_width, Fdtd_FloatType dx)
{
    // PML-layer parameters
    size_t width = material->fm_width;
    int order = 3;
    Fdtd_FloatType refl = 1.0e-6;
    Fdtd_FloatType thickness = pml_width * dx;

    // Grade conductivity in the left and right PML layers
    for (size_t k = 0; k < width; k++) {
        if (k > pml_width && k < width - pml_width)
            continue;

        // Wave impedance and maximum conductivity for the given reflection
        Fdtd_FloatType mu = material->fm_mu[k];
        Fdtd_FloatType eps = material->fm_eps[k];
        Fdtd_FloatType eta = sqrt(mu / eps);
        Fdtd_FloatType sigma_max = -(Fdtd_FloatType) (order + 1) * log(refl) / (2.0 * eta * thickness);

        // Magnetic conductivity at the H point (x = k + 1/2), condition sigma*/mu = sigma/eps
        Fdtd_FloatType dh = Fdtd_PmlDepth((Fdtd_FloatType) k + 0.5, pml_width, width);
        if (dh > 0.0) {
            material->fm_sigma_m[k] = sigma_max * pow(dh, (Fdtd_FloatType) order) * mu / eps;
        }

        // Electric conductivity at the E point (x = k)
        Fdtd_FloatType de = Fdtd_PmlDepth((Fdtd_FloatType) k, pml_width, width);
        if (de > 0.0) {
            material->fm_sigma_e[k] = sigma_max * pow(de, (Fdtd_FloatType) order);
        }
    }
}

// Set conductivity sigma_e, sigma_m (reflecting PEC boundary)
void Fdtd_EzMaterialSetPEC(Fdtd_EzMaterial *material, size_t pec_width, Fdtd_FloatType dx)
{
    (void) dx;

    // Zero magnetic and electric conductivity in every cell
    size_t width = material->fm_width;
    for (size_t k = 0; k < width; k++) {
        if (k > pec_width && k < width - pec_width)
            continue;
        material->fm_sigma_m[k] = 0.0;
        material->fm_sigma_e[k] = 0.0;
    }
}

// Free material memory
void Fdtd_EzMaterialDestroy(Fdtd_EzMaterial *material)
{
    Fdtd_FreeArray(material->fm_mu);
    Fdtd_FreeArray(material->fm_eps);
    Fdtd_FreeArray(material->fm_sigma_m);
    Fdtd_FreeArray(material->fm_sigma_e);
    Fdtd_Free(material);
}
