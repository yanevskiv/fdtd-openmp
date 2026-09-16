// Author: Ivan Janevski (C) 2026

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fdtd.h>

// Create a 1D simulation of the given width
Fdtd_EzSim *Fdtd_EzSimCreate(size_t width)
{
    // Create the structure
    Fdtd_EzSim *sim = (Fdtd_EzSim *) Fdtd_Alloc(sizeof(Fdtd_EzSim));

    // Create simulation components
    sim->fsim_state = Fdtd_EzStateCreate(width);
    sim->fsim_fields = Fdtd_EzFieldsCreate(width);
    sim->fsim_material = Fdtd_EzMaterialCreate(width);
    sim->fsim_coeffs = Fdtd_EzCoeffsCreate(width);
    sim->fsim_sources = Fdtd_EzSourcesCreate(FDTD_EZ_SOURCES_DEFAULT_CAP);

    // Compute initial coefficients from the material
    Fdtd_EzCoeffsUpdate(sim->fsim_coeffs, sim->fsim_material, sim->fsim_state);
    return sim;
}

// Free simulation memory
void Fdtd_EzSimDestroy(Fdtd_EzSim *sim)
{
    // Validate pointers
    if (sim == NULL) {
        return;
    }

    // Free simulation components
    Fdtd_EzStateDestroy(sim->fsim_state);
    Fdtd_EzFieldsDestroy(sim->fsim_fields);
    Fdtd_EzMaterialDestroy(sim->fsim_material);
    Fdtd_EzCoeffsDestroy(sim->fsim_coeffs);
    Fdtd_EzSourcesDestroy(sim->fsim_sources);

    // Free the simulation structure
    Fdtd_Free(sim);
}

// Reset simulation fields and time
void Fdtd_EzSimReset(Fdtd_EzSim *sim)
{
    Fdtd_EzStateReset(sim->fsim_state);
    Fdtd_EzFieldsReset(sim->fsim_fields);
}

// Get the number of simulation cells
size_t Fdtd_EzSimGetWidth(Fdtd_EzSim *sim)
{
    return sim->fsim_state->fs_width;
}

// Get the time step dt (in seconds)
Fdtd_FloatType Fdtd_EzSimGetDt(Fdtd_EzSim *sim)
{
    return sim->fsim_state->fs_dt;
}

// Get cumulative simulation time (number of steps since the last reset)
Fdtd_FloatType Fdtd_EzSimGetTime(Fdtd_EzSim *sim)
{
    return Fdtd_EzStateGetTime(sim->fsim_state);
}

// Set the current simulation time (number of steps, e.g. for resuming)
void Fdtd_EzSimSetTime(Fdtd_EzSim *sim, Fdtd_FloatType time)
{
    Fdtd_EzStateSetTime(sim->fsim_state, time);
}

// Set the Courant number and corresponding time step
void Fdtd_EzSimSetCourant(Fdtd_EzSim *sim, Fdtd_FloatType courant)
{
    Fdtd_EzStateSetCourant(sim->fsim_state, courant);
    Fdtd_EzCoeffsUpdate(sim->fsim_coeffs, sim->fsim_material, sim->fsim_state);
}

// Set the spatial step dx and corresponding time step
void Fdtd_EzSimSetDx(Fdtd_EzSim *sim, Fdtd_FloatType dx)
{
    Fdtd_EzStateSetDx(sim->fsim_state, dx);
    Fdtd_EzCoeffsUpdate(sim->fsim_coeffs, sim->fsim_material, sim->fsim_state);
}

// Set permittivity eps in the given cell range [kstart, kend)
void Fdtd_EzSimSetEps(Fdtd_EzSim *sim, Fdtd_FloatType eps, size_t kstart, size_t kend)
{
    Fdtd_EzMaterialSetEps(sim->fsim_material, eps, kstart, kend);
    Fdtd_EzCoeffsUpdate(sim->fsim_coeffs, sim->fsim_material, sim->fsim_state);
}

// Set permeability mu in the given cell range [kstart, kend)
void Fdtd_EzSimSetMu(Fdtd_EzSim *sim, Fdtd_FloatType mu, size_t kstart, size_t kend)
{
    Fdtd_EzMaterialSetMu(sim->fsim_material, mu, kstart, kend);
    Fdtd_EzCoeffsUpdate(sim->fsim_coeffs, sim->fsim_material, sim->fsim_state);
}

// Set a PML absorbing layer pml_width cells thick at both boundaries
void Fdtd_EzSimSetPML(Fdtd_EzSim *sim, size_t pml_width)
{
    Fdtd_EzMaterialSetPML(sim->fsim_material, pml_width, sim->fsim_state->fs_dx);
    Fdtd_EzCoeffsUpdate(sim->fsim_coeffs, sim->fsim_material, sim->fsim_state);
}

// Disable the PML layer (a PEC reflecting boundary pec_width cells thick)
void Fdtd_EzSimSetPEC(Fdtd_EzSim *sim, size_t pec_width)
{
    Fdtd_EzMaterialSetPEC(sim->fsim_material, pec_width, sim->fsim_state->fs_dx);
    Fdtd_EzCoeffsUpdate(sim->fsim_coeffs, sim->fsim_material, sim->fsim_state);
}

// Set the acceleration mode (serial / OpenMP)
void Fdtd_EzSimSetAccel(Fdtd_EzSim *sim, Fdtd_AccelType accel)
{
    Fdtd_EzStateSetAccel(sim->fsim_state, accel);
}

// Register a generic source (data is copied by value into the list)
void Fdtd_EzSimAddSource(Fdtd_EzSim *sim, Fdtd_EzSourceFn fn, Fdtd_EzSourceData data)
{
    Fdtd_EzSourcesAdd(sim->fsim_sources, fn, data);
}

// Inject a value into the electric field E_z in cell kc (for source callbacks)
void Fdtd_EzSimInjectEz(Fdtd_EzSim *sim, size_t kc, Fdtd_FloatType value)
{
    Fdtd_EzFieldsInjectEz(sim->fsim_fields, kc, value);
}

// Execute nsteps simulation steps (honor the selected acceleration mode)
void Fdtd_EzSimRun(Fdtd_EzSim *sim, size_t nsteps)
{
    switch (sim->fsim_state->fs_accel) {
        case FDTD_ACCEL_NONE: {
            Fdtd_KernelSerial_EzSimRun(sim, nsteps);
        } break;
        case FDTD_ACCEL_OPENMP: {
            Fdtd_KernelOpenMP_EzSimRun(sim, nsteps);
        } break;
    }
}

// Write the complete simulation state to a CSV file (fields, material, coefficients)
void Fdtd_EzSimWriteCSV(const Fdtd_EzSim *sim, const char *path)
{
    // Open the output file; "-" means standard output
    FILE *file = (strcmp(path, "-") == 0) ? stdout : fopen(path, "w");
    assert(file != NULL);

    // Simulation components
    const Fdtd_EzState *state = sim->fsim_state;
    const Fdtd_EzFields *fields = sim->fsim_fields;
    const Fdtd_EzMaterial *material = sim->fsim_material;
    const Fdtd_EzCoeffs *coeffs = sim->fsim_coeffs;
    size_t width = state->fs_width;

    // Simulation parameters
    fprintf(file, "# width: %zu\n", state->fs_width);
    fprintf(file, "# set_dx: %.17g\n", state->fs_dx);
    fprintf(file, "# set_courant: %.17g\n", state->fs_courant);
    fprintf(file, "# set_time: %.17g\n", state->fs_time);

    // Write state for each cell: fields, material, and update coefficients
    // Material is written in relative form (mu_r = mu / mu0, eps_r = eps / eps0)
    fprintf(file, "k,Hy,Ez,mu_r,eps_r,sigma_m,sigma_e,ch1,ch2,ce1,ce2\n");
    for (size_t k = 0; k < width; k++) {
        fprintf(file, "%zu,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g\n", k,
                fields->ff_hy[k], fields->ff_ez[k],
                material->fm_mu[k] / FDTD_MU0, material->fm_eps[k] / FDTD_EPS0,
                material->fm_sigma_m[k], material->fm_sigma_e[k],
                coeffs->fc_ch1[k], coeffs->fc_ch2[k],
                coeffs->fc_ce1[k], coeffs->fc_ce2[k]);
    }

    // Close the output file
    if (file != stdout) {
        fclose(file);
    }
}

// Create a simulation from a CSV file (width, fields, and material; coefficients are recomputed)
Fdtd_EzSim *Fdtd_EzSimCreateFromCSV(const char *path)
{
    // Open the input file
    FILE *file = fopen(path, "r");
    assert(file != NULL);

    // Temporary buffer
    char line[1024];

    // Simulation width
    size_t width = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "# width: %zu", &width))
            break;
    }
    assert(width > 0);


    // Create the simulation
    Fdtd_EzSim *sim = Fdtd_EzSimCreate(width);

    // Read parameters from the header (# <key>: <value>)
    rewind(file);   
    double value;
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "# set_dx: %lf", &value))
            Fdtd_EzSimSetDx(sim, (Fdtd_FloatType) value);
        if (sscanf(line, "# set_courant: %lf", &value))
            Fdtd_EzSimSetCourant(sim, (Fdtd_FloatType) value);
        if (sscanf(line, "# set_time: %lf", &value))
            Fdtd_EzSimSetTime(sim, (Fdtd_FloatType) value);
    }

    // Load fields and material for each cell
    Fdtd_EzFields *fields = sim->fsim_fields;
    Fdtd_EzMaterial *material = sim->fsim_material;
    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        size_t k;
        double hy, ez, mu_r, eps_r, sigma_m, sigma_e;
        if (sscanf(line, "%zu,%lf,%lf,%lf,%lf,%lf,%lf", &k, &hy, &ez, &mu_r, &eps_r, &sigma_m, &sigma_e) == 7) {
            if (k >= 0 && k < width) {
                fields->ff_hy[k] = hy;
                fields->ff_ez[k] = ez;

                // Read material in relative form and convert it to absolute values (mu = mu_r * mu0, eps = eps_r * eps0)
                material->fm_mu[k] = mu_r * FDTD_MU0;
                material->fm_eps[k] = eps_r * FDTD_EPS0;
                material->fm_sigma_m[k] = sigma_m;
                material->fm_sigma_e[k] = sigma_e;
            }
        }
    }

    // Close the input file
    fclose(file);

    // Recompute coefficients from the loaded material
    Fdtd_EzCoeffsUpdate(sim->fsim_coeffs, sim->fsim_material, sim->fsim_state);
    return sim;
}
