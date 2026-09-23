# Project Architecture and Organization

This appendix presents the organization of the source project, the built distribution, and the experimental results. For each part, we first describe the role of its directories and then provide a tree with a brief description of each file.

## Project Directory

The root `fdtd-hpc` directory contains the library source code, two user-facing programs, documentation, and utility scripts. Its organization separates the public programming interface from the implementation and from the procedures used to produce results.

```text
fdtd-hpc/
|-- include/                       Public header directory
|   |-- fdtd/                      FDTD library headers
|   |   |-- Ez/                    Ez simulation headers
|   |   |   |-- fdtd_ez_coeffs.h   Update-coefficient API
|   |   |   |-- fdtd_ez_fields.h   Field API
|   |   |   |-- fdtd_ez_kernels.h  Serial and OpenMP kernel API
|   |   |   |-- fdtd_ez_material.h Material API
|   |   |   |-- fdtd_ez_sim.h      Simulation API
|   |   |   |-- fdtd_ez_sources.h  Source API
|   |   |   `-- fdtd_ez_state.h    State API
|   |   |-- fdtd_accel.h           Acceleration modes
|   |   |-- fdtd_alloc.h           Memory allocation
|   |   |-- fdtd_const.h           Physical constants
|   |   |-- fdtd_ez.h              Ez module
|   |   `-- fdtd_types.h           Data types
|   `-- fdtd.h                      Public API entry point
|-- docs/                           English document sources and results
|-- src/                            Source-file directory
|   |-- fdtd/                       FDTD library implementation
|   |   |-- Ez/                     Ez simulation implementation
|   |   |   |-- fdtd_ez_coeffs.c   Coefficient calculation
|   |   |   |-- fdtd_ez_fields.c   Field management
|   |   |   |-- fdtd_ez_kernels.c  Serial and OpenMP kernels
|   |   |   |-- fdtd_ez_material.c Material management
|   |   |   |-- fdtd_ez_sim.c      Simulation management
|   |   |   |-- fdtd_ez_sources.c  Source management
|   |   |   `-- fdtd_ez_state.c    State management
|   |   `-- fdtd_alloc.c           Aligned memory allocation
|   |-- benchmark.c                Performance measurement program
|   `-- main.c                     Simulation program
|-- tools/                          Utility scripts
|   |-- benchmark.sh               Repeated performance measurements
|   |-- get_sys_info.sh            System-information collection
|   |-- plot_benchmark.py          Performance plots
|   |-- plot_sim.py                Simulation plots
|   `-- plot_figures.py            Standalone illustrations
|-- Makefile                       Build and experiment rules
`-- README.md                      Project description
```

The `include` directory contains the library's public declarations. The `fdtd.h` header is the interface entry point, while the `fdtd` subdirectory separates general types and functions from declarations for the one-dimensional simulation in the `Ez` subdirectory.

The `src` directory closely follows the organization of the public headers because most declarations have a corresponding implementation file. In addition to the library code, it contains the `main.c` simulation program and the `benchmark.c` performance measurement program.

The `tools` directory contains scripts for repeating measurements, collecting system information, and plotting results. The `plot_sim.py` and `plot_benchmark.py` scripts process CSV files produced by the executables, while `plot_figures.py` creates illustrations that do not depend on execution results. The `docs` directory contains the English Markdown sources, metadata, and copies of the results used to build this document.

## Distribution Directory

Building the project creates the `dist` directory containing the files needed to use the library. It contains no source or temporary object files and therefore forms a self-contained distribution intended for users. The `make docs` target additionally places the generated PDF in its `docs` subdirectory.

```text
dist/
|-- bin/                            Executable directory
|   |-- tools/                      Utility scripts
|   |   |-- benchmark.sh           Benchmark repetition
|   |   |-- get_sys_info.sh        System-information collection
|   |   |-- plot_benchmark.py      Performance plots
|   |   |-- plot_sim.py            Simulation plots
|   |   `-- plot_figures.py        Standalone illustrations
|   |-- benchmark                   Performance measurement executable
|   `-- fdtd                        Simulation executable
|-- docs/
|   `-- Fdtd_Thesis.pdf            Generated English document
|-- include/                        Public headers
|   |-- fdtd/
|   |   |-- Ez/
|   |   |   |-- fdtd_ez_coeffs.h   Update-coefficient API
|   |   |   |-- fdtd_ez_fields.h   Field API
|   |   |   |-- fdtd_ez_kernels.h  Serial and OpenMP kernel API
|   |   |   |-- fdtd_ez_material.h Material API
|   |   |   |-- fdtd_ez_sim.h      Simulation API
|   |   |   |-- fdtd_ez_sources.h  Source API
|   |   |   `-- fdtd_ez_state.h    State API
|   |   |-- fdtd_accel.h           Acceleration modes
|   |   |-- fdtd_alloc.h           Memory allocation
|   |   |-- fdtd_const.h           Physical constants
|   |   |-- fdtd_ez.h              Ez module
|   |   `-- fdtd_types.h           Data types
|   `-- fdtd.h                      Public API entry point
|-- lib/
|   `-- libfdtd.a                   Static FDTD library
`-- buildinfo                       Compiler and build information
```

The `bin` directory contains the `fdtd` simulation executable and the `benchmark` performance measurement program. Its `tools` subdirectory contains the same utility scripts as the source project, allowing results to be produced and processed from the distribution.

The `include` directory contains a copy of the public headers and retains their original organization. A user program includes them at compile time to access the library's types, constants, and functions.

The `lib` directory contains the `libfdtd.a` static library, which is linked into a user program. The `buildinfo` file at the distribution root records the compiler and compilation options, allowing the build environment to be checked later. The `docs` directory contains the PDF produced from the Markdown sources.

## Results Directory

Running the simulation, measurement, and image-generation targets creates the `results` directory. It combines raw CSV data, plots derived from those data, and standalone illustrations required to describe the model.

```text
results/
|-- images/                         Generated plots
|   |-- benchmark_efficiency.png   Parallel efficiency
|   |-- benchmark_speedup.png      Speedup by thread count
|   |-- benchmark_speedup_k.png    Speedup by grid width
|   |-- benchmark_throughput.png   Throughput by thread count
|   |-- benchmark_throughput_k.png Throughput by grid width
|   |-- figure_fork_join.png       Thread fork and join
|   |-- figure_yee.png             Yee grid
|   |-- gauss_plot2d_Ez.png        Electric field Ez
|   |-- gauss_plot2d_Hy.png        Magnetic field Hy
|   |-- gauss_plot2d_ce1.png       Coefficient Ce1
|   |-- gauss_plot2d_ce2.png       Coefficient Ce2
|   |-- gauss_plot2d_ch1.png       Coefficient Ch1
|   |-- gauss_plot2d_ch2.png       Coefficient Ch2
|   |-- gauss_plot2d_eps_r.png     Relative permittivity
|   |-- gauss_plot2d_mu_r.png      Relative permeability
|   |-- gauss_plot2d_sigma_e.png   Electrical conductivity
|   |-- gauss_plot2d_sigma_m.png   Magnetic conductivity
|   |-- gauss_plot3d_EzHy.png      Spatial Ez and Hy field plot
|   `-- gauss_pml.png              PML positions
|-- benchmark.csv                  Performance measurements
`-- gauss.csv                      Gaussian-source simulation state
```

The `gauss.csv` file contains the state of a one-dimensional simulation with a Gaussian source, while `benchmark.csv` contains system information and individual timing measurements. The first file is used to verify physical behavior, and the second to calculate throughput, speedup, and efficiency.

The `images` directory contains plots of the fields, materials, coefficients, and PML, together with the performance plots. The `figure_*` images are produced separately by the `make figures` target and do not require a simulation or benchmark run.

## Data Structures

The data structures separate the responsibilities of the individual parts of the one-dimensional simulation and connect them through a central object. Their declarations from the library's public headers are presented below, proceeding from the complete simulation to its constituent parts.

The `Fdtd_EzSim` structure represents the entire one-dimensional simulation and contains pointers to all of its components. It owns the state, fields, material, coefficients, and source list, so public simulation functions receive a single pointer to this object.

```c
struct Fdtd_EzSim {
    Fdtd_EzState    *fsim_state;
    Fdtd_EzFields   *fsim_fields;
    Fdtd_EzMaterial *fsim_material;
    Fdtd_EzCoeffs   *fsim_coeffs;
    Fdtd_EzSources  *fsim_sources;
};
```

The `Fdtd_EzState` structure stores the global simulation parameters: the number of cells, spatial and temporal increments, Courant number, current time, and acceleration mode. Keeping these values in one place simplifies their updating and use by different kernels.

```c
typedef struct {
    size_t fs_width;
    Fdtd_FloatType fs_dx;
    Fdtd_FloatType fs_dt;
    Fdtd_FloatType fs_courant;
    Fdtd_FloatType fs_time;
    Fdtd_AccelType fs_accel;
} Fdtd_EzState;
```

The `Fdtd_EzFields` structure contains the grid width and arrays of discrete magnetic-field $H_y$ and electric-field $E_z$ values. Both arrays contain one element for each cell, while their physical staggering determines how their indices are used in the update equations.

```c
typedef struct {
    size_t ff_width;
    Fdtd_FloatType *ff_hy;
    Fdtd_FloatType *ff_ez;
} Fdtd_EzFields;
```

The `Fdtd_EzMaterial` structure describes the spatial distribution of material properties and losses. Arrays of permeability, permittivity, and the two conductivities allow every cell to have separate properties and the PML to be configured without changing the computational kernel.

```c
typedef struct {
    size_t fm_width;
    Fdtd_FloatType *fm_mu;
    Fdtd_FloatType *fm_eps;
    Fdtd_FloatType *fm_sigma_m;
    Fdtd_FloatType *fm_sigma_e;
} Fdtd_EzMaterial;
```

The `Fdtd_EzCoeffs` structure stores four arrays of precalculated field-update coefficients. They are recalculated after a change in discretization, material, or boundary condition, removing division and material-property processing from the time loop.

```c
typedef struct {
    size_t fc_width;
    Fdtd_FloatType *fc_ch1;
    Fdtd_FloatType *fc_ch2;
    Fdtd_FloatType *fc_ce1;
    Fdtd_FloatType *fc_ce2;
} Fdtd_EzCoeffs;
```

The `Fdtd_EzSources` structure manages a dynamic array of registered sources. It stores a pointer to its elements, the available capacity, and the number of occupied entries, allowing all sources to be applied in order after the electric field has been updated.

```c
typedef struct {
    Fdtd_EzSource *fsrc_items;
    size_t fsrc_size;
    size_t fsrc_count;
} Fdtd_EzSources;
```
