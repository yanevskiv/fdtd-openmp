// Author: Ivan Janevski (C) 2026

#include <fdtd.h>

// Create coefficients for a 1D simulation of the given width
Fdtd_EzCoeffs *Fdtd_EzCoeffsCreate(size_t width)
{
    // Create the structure
    Fdtd_EzCoeffs *coeffs = (Fdtd_EzCoeffs *) Fdtd_Alloc(sizeof(Fdtd_EzCoeffs));

    // Allocate coefficient arrays (aligned memory for vectorization)
    coeffs->fc_width = width;
    coeffs->fc_ch1 = Fdtd_AllocArray(width, sizeof(Fdtd_FloatType));
    coeffs->fc_ch2 = Fdtd_AllocArray(width, sizeof(Fdtd_FloatType));
    coeffs->fc_ce1 = Fdtd_AllocArray(width, sizeof(Fdtd_FloatType));
    coeffs->fc_ce2 = Fdtd_AllocArray(width, sizeof(Fdtd_FloatType));
    return coeffs;
}

// Compute coefficients from the material and simulation parameters
void Fdtd_EzCoeffsUpdate(Fdtd_EzCoeffs *coeffs, const Fdtd_EzMaterial *material, const Fdtd_EzState *state)
{
    // Parameters
    Fdtd_FloatType dt = state->fs_dt;
    Fdtd_FloatType dx = state->fs_dx;
    size_t width = coeffs->fc_width;

    // Coefficients for each cell from the material
    for (size_t k = 0; k < width; k++) {
        Fdtd_FloatType mu = material->fm_mu[k];
        Fdtd_FloatType eps = material->fm_eps[k];
        Fdtd_FloatType sigma_m = material->fm_sigma_m[k];
        Fdtd_FloatType sigma_e = material->fm_sigma_e[k];

        // Semi-implicit loss terms (magnetic and electric)
        Fdtd_FloatType b = sigma_m * dt / (2.0 * mu);
        Fdtd_FloatType a = sigma_e * dt / (2.0 * eps);

        // H_y coefficients
        coeffs->fc_ch1[k] = (1.0 - b) / (1.0 + b);
        coeffs->fc_ch2[k] = (dt / (mu * dx)) / (1.0 + b);

        // E_z coefficients
        coeffs->fc_ce1[k] = (1.0 - a) / (1.0 + a);
        coeffs->fc_ce2[k] = (dt / (eps * dx)) / (1.0 + a);
    }
}

// Free coefficient memory
void Fdtd_EzCoeffsDestroy(Fdtd_EzCoeffs *coeffs)
{
    Fdtd_FreeArray(coeffs->fc_ch1);
    Fdtd_FreeArray(coeffs->fc_ch2);
    Fdtd_FreeArray(coeffs->fc_ce1);
    Fdtd_FreeArray(coeffs->fc_ce2);
    Fdtd_Free(coeffs);
}
