// Author: Ivan Janevski (C) 2026

#ifndef _FDTD_EZ_STATE_H_
#define _FDTD_EZ_STATE_H_

#include <stddef.h>
#include <fdtd/fdtd_types.h>
#include <fdtd/fdtd_accel.h>

// Simulation parameters
typedef struct {
    size_t fs_width;            // Number of cells in the simulation
    Fdtd_FloatType  fs_dx;      // Spatial step (m)
    Fdtd_FloatType  fs_dt;      // Time step (s)
    Fdtd_FloatType  fs_courant; // Courant number S = c * dt / dx
    Fdtd_FloatType  fs_time;    // Current simulation time (number of steps)
    Fdtd_AccelType fs_accel;        // Acceleration mode (serial / OpenMP)
} Fdtd_EzState;

// Create parameters for a 1D simulation of the given width
Fdtd_EzState *Fdtd_EzStateCreate(size_t width);

// Reset simulation time
void Fdtd_EzStateReset(Fdtd_EzState *state);

// Set the Courant number and update the time step dt
void Fdtd_EzStateSetCourant(Fdtd_EzState *state, Fdtd_FloatType courant);

// Set the spatial step dx and update the time step dt
void Fdtd_EzStateSetDx(Fdtd_EzState *state, Fdtd_FloatType dx);

// Set the acceleration mode (serial / OpenMP)
void Fdtd_EzStateSetAccel(Fdtd_EzState *state, Fdtd_AccelType accel);

// Advance to the next simulation time step
void Fdtd_EzStateNextStep(Fdtd_EzState *state);

// Get the current simulation time
Fdtd_FloatType Fdtd_EzStateGetTime(const Fdtd_EzState *state);

// Set the current simulation time (number of steps)
void Fdtd_EzStateSetTime(Fdtd_EzState *state, Fdtd_FloatType time);

// Free simulation parameters
void Fdtd_EzStateDestroy(Fdtd_EzState *state);

#endif
