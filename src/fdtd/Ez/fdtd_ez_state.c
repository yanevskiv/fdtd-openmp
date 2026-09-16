// Author: Ivan Janevski (C) 2026

#include <fdtd.h>

// Create parameters for a 1D simulation of the given width
Fdtd_EzState *Fdtd_EzStateCreate(size_t width)
{
    // Create the structure
    Fdtd_EzState *state = (Fdtd_EzState *) Fdtd_Alloc(sizeof(Fdtd_EzState));

    // Basic simulation parameters
    state->fs_width = width;
    state->fs_dx = 0.01;
    state->fs_time = 0.0;
    state->fs_accel = FDTD_ACCEL_NONE;

    // Default Courant number and corresponding time step
    Fdtd_EzStateSetCourant(state, 0.5);
    return state;
}

// Reset simulation time
void Fdtd_EzStateReset(Fdtd_EzState *state)
{
    state->fs_time = 0.0;
}

// Set the Courant number and corresponding time step
void Fdtd_EzStateSetCourant(Fdtd_EzState *state, Fdtd_FloatType courant)
{
    state->fs_courant = courant;
    state->fs_dt = courant * state->fs_dx / FDTD_C0;
}

// Set the spatial step dx and corresponding time step
void Fdtd_EzStateSetDx(Fdtd_EzState *state, Fdtd_FloatType dx)
{
    state->fs_dx = dx;
    state->fs_dt = state->fs_courant * dx / FDTD_C0;
}

// Set the acceleration mode (serial / OpenMP)
void Fdtd_EzStateSetAccel(Fdtd_EzState *state, Fdtd_AccelType accel)
{
    state->fs_accel = accel;
}

// Advance to the next time step
void Fdtd_EzStateNextStep(Fdtd_EzState *state)
{
    state->fs_time += 1.0;
}

// Get the current simulation time
Fdtd_FloatType Fdtd_EzStateGetTime(const Fdtd_EzState *state)
{
    return state->fs_time;
}

// Set the current simulation time (number of steps)
void Fdtd_EzStateSetTime(Fdtd_EzState *state, Fdtd_FloatType time)
{
    state->fs_time = time;
}

// Free simulation parameters
void Fdtd_EzStateDestroy(Fdtd_EzState *state)
{
    Fdtd_Free(state);
}
