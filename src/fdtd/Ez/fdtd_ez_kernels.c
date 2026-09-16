// Author: Ivan Janevski (C) 2026

#include <fdtd.h>

// Serial execution of nsteps simulation steps
void Fdtd_KernelSerial_EzSimRun(Fdtd_EzSim *sim, size_t nsteps)
{
    // FDTD algorithm parameters
    Fdtd_FloatType * restrict Hy = sim->fsim_fields->ff_hy;
    Fdtd_FloatType * restrict Ez = sim->fsim_fields->ff_ez;
    Fdtd_FloatType * restrict Ch1 = sim->fsim_coeffs->fc_ch1;
    Fdtd_FloatType * restrict Ch2 = sim->fsim_coeffs->fc_ch2;
    Fdtd_FloatType * restrict Ce1 = sim->fsim_coeffs->fc_ce1;
    Fdtd_FloatType * restrict Ce2 = sim->fsim_coeffs->fc_ce2;
    size_t width = sim->fsim_state->fs_width;

    // FDTD algorithm
    for (size_t n = 0; n < nsteps; n++) {
        
        // Update Hy
        for (size_t k = 0; k < width - 1; k++) {
            Hy[k] = Ch1[k] * Hy[k] 
                  + Ch2[k] * (Ez[k + 1] - Ez[k]);
        }

        // Update Ez
        for (size_t k = 1; k < width; k++) {
            Ez[k] = Ce1[k] * Ez[k] 
                  + Ce2[k] * (Hy[k] - Hy[k - 1]);
        }

        // Inject Ez source
        Fdtd_EzSourcesApply(sim->fsim_sources, sim);
        Fdtd_EzStateNextStep(sim->fsim_state);
    }
}

// OpenMP execution of nsteps simulation steps
void Fdtd_KernelOpenMP_EzSimRun(Fdtd_EzSim *sim, size_t nsteps)
{
    // FDTD algorithm parameters
    Fdtd_FloatType * restrict Hy = sim->fsim_fields->ff_hy;
    Fdtd_FloatType * restrict Ez = sim->fsim_fields->ff_ez;
    Fdtd_FloatType * restrict Ch1 = sim->fsim_coeffs->fc_ch1;
    Fdtd_FloatType * restrict Ch2 = sim->fsim_coeffs->fc_ch2;
    Fdtd_FloatType * restrict Ce1 = sim->fsim_coeffs->fc_ce1;
    Fdtd_FloatType * restrict Ce2 = sim->fsim_coeffs->fc_ce2;
    size_t width = sim->fsim_state->fs_width;

    // FDTD algorithm
    #pragma omp parallel \
        default(none) \
        firstprivate(Hy, Ez, Ch1, Ch2, Ce1, Ce2, width, nsteps) \
        shared(sim)
    {
        for (size_t n = 0; n < nsteps; n++) {

            // Update Hy
            #pragma omp for schedule(static)
            for (size_t k = 0; k < width - 1; k++) {
                Hy[k] = Ch1[k] * Hy[k] 
                    + Ch2[k] * (Ez[k + 1] - Ez[k]);
            }

            // Update Ez
            #pragma omp for schedule(static)
            for (size_t k = 1; k < width; k++) {
                Ez[k] = Ce1[k] * Ez[k]
                    + Ce2[k] * (Hy[k] - Hy[k - 1]);
            }

            // Inject Ez source
            #pragma omp single
            {
                Fdtd_EzSourcesApply(sim->fsim_sources, sim);
                Fdtd_EzStateNextStep(sim->fsim_state);
            }
        }
    }
}
