# Software Implementation

In the preceding chapters, we derived the FDTD update equations and introduced the PML. This chapter presents the main parts of the software implementation written in the C programming language.

## Software Architecture

We divide the implementation into smaller components for the state, fields, materials, coefficients, sources, and acceleration mode. Each component represents one part of the mathematical model that can be verified independently. This division establishes a clear connection between the physical model and the program code.

The `Fdtd_EzSim` structure connects all parts of a simulation. The grid size, fields, materials, coefficients, and sources are nevertheless stored in separate structures. The main software components and their roles are shown in Table 5.1.

Table: Main components of the software solution

| Component | Structure | Role |
| --- | --- | --- |
| state | `Fdtd_EzState` | parameters $K$, $S$, $\Delta x$, and $\Delta t$ |
| fields | `Fdtd_EzFields` | arrays for $E_z$ and $H_y$ |
| material | `Fdtd_EzMaterial` | arrays for $\varepsilon$, $\mu$, $\sigma_e$, and $\sigma_m$ |
| coefficients | `Fdtd_EzCoeffs` | arrays for $C_{e1}$, $C_{e2}$, $C_{h1}$, and $C_{h2}$ |
| sources | `Fdtd_EzSources` | excitation functions and their data |
| simulation | `Fdtd_EzSim` | connection between all components |

This architecture allows the serial and parallel kernels to share the same data and public interface. Changing the acceleration mode therefore does not modify the material, sources, or result format. Such separation is important for a fair performance comparison because both versions execute the same numerical procedure.

## The Static Library and Public API

The software is delivered as a package consisting of the `libfdtd.a` static library, a set of headers, two executable programs, and utility scripts. Users can construct simulations through the library functions. The `fdtd` executable provides access to the same functions from the command line.

The public `fdtd.h` header includes the types, constants, and functions required by the user. Function names begin with the `Fdtd_` prefix, reducing the possibility of conflicts with other C libraries. A separate `fdtd_ez.h` header groups the components belonging to the selected polarization.

```c
#include <fdtd.h>

Fdtd_EzSim *sim = Fdtd_EzSimCreate(200);
Fdtd_EzSimSetPML(sim, 20);
Fdtd_EzSimRun(sim, 100);
Fdtd_EzSimDestroy(sim);
```

The basic lifecycle comprises creating, configuring, running, and destroying the object. Between these steps, the user can change the material, spatial step, Courant number, or acceleration mode. A clear lifecycle reduces the possibility of memory leaks and facilitates reproducible experiments.

## Simulation Representation and Data Structures

We represent each spatial quantity as a separately allocated dynamic array of $K$ real numbers. Consecutive elements of an array use cache lines efficiently and preserve favorable spatial locality. The `Fdtd_EzFields` structure is one example and stores the electromagnetic-field values on the grid.

```c
typedef struct {
    size_t ff_width;       // Number of cells
    Fdtd_FloatType *ff_hy; // Magnetic field H_y
    Fdtd_FloatType *ff_ez; // Electric field E_z
} Fdtd_EzFields;
```

The simulation-state structure stores the width, $\Delta x$, $\Delta t$, Courant number, current time, and selected acceleration mode. The time step is recalculated automatically whenever the spatial step or Courant number changes. This approach prevents every user program from having to maintain the important stability relationship itself.

```c
typedef struct {
    size_t fs_width;             // Number of cells in the simulation
    Fdtd_FloatType fs_dx;        // Spatial step (m)
    Fdtd_FloatType fs_dt;        // Time step (s)
    Fdtd_FloatType fs_courant;   // Courant number S = c * dt / dx
    Fdtd_FloatType fs_time;      // Current simulation time (number of steps)
    Fdtd_AccelType fs_accel;     // Acceleration mode (serial / OpenMP)
} Fdtd_EzState;
```

Material properties and update coefficients are stored in four arrays each, with the same number of elements as the field arrays. This choice uses more memory than calculating the coefficients on demand, but considerably reduces the number of operations in every time step. The same coefficients are used hundreds of times, so the lower computational cost justifies the additional memory.

## FDTD Algorithm Implementation

We implement the update equations from Chapter 3 with two loops in the `Fdtd_KernelSerial_EzSimRun()` function. Local pointers shorten access through the structures, while the `restrict` qualifier informs the compiler that the arrays do not overlap. The resulting code remains readable and suitable for automatic optimization.

```c
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

    // Inject Ez source and advance time
    Fdtd_EzSourcesApply(sim->fsim_sources, sim);
    Fdtd_EzStateNextStep(sim->fsim_state);
}
```

The first loop updates $H_y^{n+1/2}[k + 1/2]$ for indices from zero through $K-2$ because it reads two neighboring values of $E_z^n[k]$. The second loop starts at index one and updates $E_z^{n+1}[k]$ through the end of the domain while reading $H_y^{n+1/2}[k - 1/2]$. The bounds of both loops follow directly from the field positions on the Yee grid.

A dependency exists between the two loops, but the iterations within each loop are independent. The serial code serves as the reference implementation for validating the parallel version and calculating speedup. Users do not invoke the simulation kernel directly, but instead call the public `Fdtd_EzSimRun()` function.

## PML Integration

The `Fdtd_EzMaterialSetPML()` function fills the electrical- and magnetic-conductivity arrays at both ends of the domain. It is called by the public `Fdtd_EzSimSetPML()` function, which subsequently recalculates the update coefficients through `Fdtd_EzCoeffsUpdate()`. In this way, configuring the PML changes the update-coefficient values used by the FDTD algorithm.

The `pml_width` argument specifies the number of PML cells $N_\text{PML}$, while `dx` is the spatial step $\Delta x$; multiplying these two quantities gives `thickness`, the PML thickness $d$. The helper function `Fdtd_PmlDepth()` calculates the normalized depth $q$. Selecting `order = 3` gives the cubic conductivity profile $m=3$, while `refl` represents the target reflection coefficient $R_0$.

```c
// PML-layer parameters
int order = 3;
Fdtd_FloatType refl = 1.0e-6;
Fdtd_FloatType thickness = pml_width * dx;
```

The loop skips the interior and processes only the cells in the left and right PML regions. For each cell, it calculates the maximum conductivity `sigma_max` from the local impedance `eta`, then determines separate depths for the electric- and magnetic-field points. This produces spatially staggered `sigma_e` and `sigma_m` profiles that subsequently affect the update coefficients.

```c
// Grade conductivity in the left and right PML layers
for (size_t k = 0; k < width; k++) {
    if (k > pml_width && k < width - pml_width)
        continue;

    // Wave impedance and maximum conductivity for the given reflection
    Fdtd_FloatType mu = material->fm_mu[k];
    Fdtd_FloatType eps = material->fm_eps[k];
    Fdtd_FloatType eta = sqrt(mu / eps);
    Fdtd_FloatType sigma_max =
        -(Fdtd_FloatType) (order + 1) * log(refl)
        / (2.0 * eta * thickness);

    // Magnetic conductivity at the H point (x = k + 1/2),
    // condition sigma*/mu = sigma/eps
    Fdtd_FloatType dh = Fdtd_PmlDepth(
        (Fdtd_FloatType) k + 0.5, pml_width, width);
    if (dh > 0.0) {
        material->fm_sigma_m[k] =
            sigma_max * pow(dh, (Fdtd_FloatType) order) * mu / eps;
    }

    // Electric conductivity at the E point (x = k)
    Fdtd_FloatType de = Fdtd_PmlDepth(
        (Fdtd_FloatType) k, pml_width, width);
    if (de > 0.0) {
        material->fm_sigma_e[k] =
            sigma_max * pow(de, (Fdtd_FloatType) order);
    }
}
```

## Memory Management

The large number of consecutive accesses makes memory layout an important aspect of performance. The `Fdtd_AllocArray()` function allocates aligned storage, using 64-byte alignment by default, and verifies that the allocation succeeded. Alignment facilitates vector loads and reduces the number of unfavorable accesses that cross cache-line boundaries.

Every function that creates a structure has a corresponding function that destroys it. The main simulation object destroys its components in the reverse order from their creation. The user therefore needs to call only `Fdtd_EzSimDestroy()`, without manually freeing the internal arrays.

We initialize the fields to zero when they are created or reset, so repeated measurements begin from the same state. Before each sample, the benchmark performs a short warm-up, preventing the first access to allocated pages from being included in the measured time. This procedure reduces the effect of memory initialization on comparisons between computational kernels.

## Simulation Configuration and Execution

The command-line tool allows the model to be configured without writing a new program. Users can specify the number of cells, spatial step, Courant number, material regions, boundary, source, and number of time steps. This configuration is convenient for quickly repeating simulation examples.

```bash
fdtd --set-width=200 --set-pml=20 \
     --add-pulse --run=100 results/gauss.csv
```

The program first processes the options, then creates the simulation and applies the requested material changes. Sources are registered after the materials, and the time loop begins only after all coefficients are ready. The result is finally written to a comma-separated values (CSV) file containing the state of every cell.

An existing state can be loaded with the `--input-csv` option and advanced by a new number of steps. This capability is useful when a long computation is divided into several parts or when the same instant is used as the basis for multiple experiments. The `--get-dt` option additionally provides a direct check of the calculated time step.

## Visualization and Performance Measurement

Raw arrays of numbers make it difficult to determine whether a wave propagates as expected, so the project includes the `plot_sim.py` tool. It reads a CSV state and plots the fields, material quantities, coefficients, and absorbing-layer profile. The plots thereby connect the program's internal data with the physical meaning of the model.

A separate `benchmark` program measures the serial reference and OpenMP version with different thread counts. For every repetition, it records the execution mode, number of threads, grid width, number of steps, and measured duration. The `plot_benchmark.py` script calculates throughput, speedup, and efficiency from these values and produces plots that compare domain sizes.

Simulation output is kept separate from performance results, so the time spent plotting or writing a large state does not affect the measured kernel. The measured interval includes only the time-loop call with data that has already been prepared. The resulting values therefore describe the computational portion that is actually parallelized.

## State Format and Simulation Resumption

The output CSV file contains simulation parameters in comments and one tabular row for every cell. The header records the width, spatial step, Courant number, and current time, while the columns contain the fields, relative material properties, conductivities, and coefficients. This format is human-readable and can be loaded directly into commonly used analysis tools.

```text
# width: 200
# set_dx: 0.01
# set_courant: 0.5
# set_time: 100
k,Hy,Ez,mu_r,eps_r,sigma_m,sigma_e,ch1,ch2,ce1,ce2
```

The `Fdtd_EzSimCreateFromCSV()` function first reads the width and creates a new object of the appropriate size. It then loads the steps, time, fields, and material arrays before recalculating the coefficients. We recalculate the coefficients to ensure that they correspond to the parameters that were actually loaded.

Saving the complete state makes it possible to resume a long simulation and separate computation from visualization. The cost of this approach is a large text file and slower output compared with a binary format. Output is performed outside the performance-measurement interval and only when the user requests a result.

## Input Validation and Error Handling

The command-line tool checks whether the required parameters are present and whether a recognized acceleration mode was selected. At most 32 sources and the same number of material modifications can be specified, preventing writes beyond the bounds of the designated arrays. The range of every material modification is also limited by the actual grid width.

Memory-allocation functions use `assert()` to terminate the program immediately if the system cannot provide the requested storage. The same type of check is used when opening a file and reading the width from a saved state. This behavior is sufficient for a research prototype because it prevents execution from continuing with an invalid pointer or incomplete object.

A more robust library intended for wider use should return an error code instead of terminating the process. It should also verify that the PML width, source position, Courant number, and all numerical values are within their valid ranges. Such an extension would allow the calling program to display a meaningful message and safely release any resources that have already been allocated.

## Build Process and Distribution Structure

The project uses a `Makefile`, so the library, programs, and tools are always built with the same options. The `make build` command compiles the source files, creates the static library, and links the user-facing programs. Details of the project structure itself are provided in Appendix D.

```text
dist/
|-- bin/
|   |-- benchmark
|   |-- fdtd
|   `-- tools/
|-- docs/
|   `-- Fdtd_Thesis.pdf
|-- include/
|-- lib/
|   `-- libfdtd.a
`-- buildinfo
```

The `dist` directory stores the executable programs, public headers, library, utility tools, documentation, and build information separately. Users do not need the internal source-tree structure to link their programs with `libfdtd.a`. The plotting and measurement tools are available in the same directory, eliminating the need to collect files manually.
