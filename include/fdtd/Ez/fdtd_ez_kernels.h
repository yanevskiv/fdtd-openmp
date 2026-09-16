// Author: Ivan Janevski (C) 2026

#ifndef _FDTD_EZ_KERNELS_H_
#define _FDTD_EZ_KERNELS_H_

#include <fdtd/Ez/fdtd_ez_sim.h>

// Serial execution of nsteps simulation steps
void Fdtd_KernelSerial_EzSimRun(Fdtd_EzSim *sim, size_t nsteps);

// OpenMP execution of nsteps simulation steps
void Fdtd_KernelOpenMP_EzSimRun(Fdtd_EzSim *sim, size_t nsteps);

#endif
