// Author: Ivan Janevski (C) 2026

#ifndef _FDTD_EZ_SIM_H_
#define _FDTD_EZ_SIM_H_

#include <stddef.h>
#include <fdtd/fdtd_types.h>
#include <fdtd/Ez/fdtd_ez_state.h>
#include <fdtd/Ez/fdtd_ez_fields.h>
#include <fdtd/Ez/fdtd_ez_material.h>
#include <fdtd/Ez/fdtd_ez_coeffs.h>
#include <fdtd/Ez/fdtd_ez_sources.h>

// 1D simulation
struct Fdtd_EzSim {
    Fdtd_EzState    *fsim_state;     // Global simulation parameters
    Fdtd_EzFields   *fsim_fields;    // Electromagnetic fields
    Fdtd_EzMaterial *fsim_material;  // Material properties
    Fdtd_EzCoeffs   *fsim_coeffs;    // Update coefficients
    Fdtd_EzSources  *fsim_sources;   // Source list (pulses, sinusoidal sources)
};

// Create a 1D simulation of the given width
Fdtd_EzSim *Fdtd_EzSimCreate(size_t width);

// Free simulation memory
void Fdtd_EzSimDestroy(Fdtd_EzSim *sim);

// Reset simulation fields and time
void Fdtd_EzSimReset(Fdtd_EzSim *sim);

// Get the number of simulation cells
size_t Fdtd_EzSimGetWidth(Fdtd_EzSim *sim);

// Get the time step dt (in seconds)
Fdtd_FloatType Fdtd_EzSimGetDt(Fdtd_EzSim *sim);

// Get cumulative simulation time (number of steps since the last reset)
Fdtd_FloatType Fdtd_EzSimGetTime(Fdtd_EzSim *sim);

// Set the current simulation time (number of steps, e.g. for resuming)
void Fdtd_EzSimSetTime(Fdtd_EzSim *sim, Fdtd_FloatType time);

// Set the Courant number and corresponding time step
void Fdtd_EzSimSetCourant(Fdtd_EzSim *sim, Fdtd_FloatType courant);

// Set the spatial step dx and corresponding time step
void Fdtd_EzSimSetDx(Fdtd_EzSim *sim, Fdtd_FloatType dx);

// Set permittivity eps in the given cell range [kstart, kend)
void Fdtd_EzSimSetEps(Fdtd_EzSim *sim, Fdtd_FloatType eps, size_t kstart, size_t kend);

// Set permeability mu in the given cell range [kstart, kend)
void Fdtd_EzSimSetMu(Fdtd_EzSim *sim, Fdtd_FloatType mu, size_t kstart, size_t kend);

// Set a PML absorbing layer pml_width cells thick at both boundaries
void Fdtd_EzSimSetPML(Fdtd_EzSim *sim, size_t pml_width);

// Disable the PML layer (a PEC reflecting boundary pec_width cells thick)
void Fdtd_EzSimSetPEC(Fdtd_EzSim *sim, size_t pec_width);

// Set the acceleration mode (serial / OpenMP)
void Fdtd_EzSimSetAccel(Fdtd_EzSim *sim, Fdtd_AccelType accel);

// Register a generic source (data is copied by value into the list)
void Fdtd_EzSimAddSource(Fdtd_EzSim *sim, Fdtd_EzSourceFn fn, Fdtd_EzSourceData data);

// Inject a value into the electric field E_z in cell kc (for source callbacks)
void Fdtd_EzSimInjectEz(Fdtd_EzSim *sim, size_t kc, Fdtd_FloatType value);

// Execute nsteps simulation steps (honors the selected acceleration mode)
void Fdtd_EzSimRun(Fdtd_EzSim *sim, size_t nsteps);

// Write the complete simulation state to a CSV file (fields, material, coefficients)
void Fdtd_EzSimWriteCSV(const Fdtd_EzSim *sim, const char *path);

// Create a simulation from a CSV file (width, fields, and material; coefficients are recomputed)
Fdtd_EzSim *Fdtd_EzSimCreateFromCSV(const char *path);

#endif
