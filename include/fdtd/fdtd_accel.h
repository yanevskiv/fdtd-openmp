// Author: Ivan Janevski (C) 2026

#ifndef _FDTD_ACCEL_H_
#define _FDTD_ACCEL_H_

// Acceleration modes
typedef enum {
    FDTD_ACCEL_NONE,    // Serial execution
    FDTD_ACCEL_OPENMP   // Parallelization using OpenMP
} Fdtd_AccelType;

#endif
