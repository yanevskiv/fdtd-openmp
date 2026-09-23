# FDTD-HPC

FDTD-HPC is a high-performance one-dimensional electromagnetic-wave simulator based on the finite-difference time-domain (FDTD) method, with serial and OpenMP execution, configurable materials and boundaries, CSV state export, plotting tools, and benchmarking support. The `docs/` directory contains the English thesis sources and the results used in the document.

## Configuration

The Gaussian simulation and benchmark parameters are configured in `Makefile`:

```make
GAUSS_CSV = results/gauss.csv
BENCH_NSTEPS = 500
BENCH_REPEAT = 5
BENCH_SIZES = 1000 5000 10000 50000 100000
```

Display the command-line options with:

```sh
dist/bin/fdtd --help
```

Build the thesis PDF with Pandoc and LuaLaTeX:

```sh
make docs
```

The generated document is written to `dist/docs/Fdtd_Thesis.pdf`. The `all` target builds the software, figures, and thesis.

## Project structure

After `make clean`, the project source tree is:

```
.
├── include                             - Header directory
│   ├── fdtd                            - FDTD header directory
│   │   ├── Ez                          - Ez simulation header directory
│   │   │   ├── fdtd_ez_coeffs.h        - Coefficient header
│   │   │   ├── fdtd_ez_fields.h        - Field header
│   │   │   ├── fdtd_ez_kernels.h       - Serial and OpenMP kernel header
│   │   │   ├── fdtd_ez_material.h      - Material header
│   │   │   ├── fdtd_ez_sim.h           - Simulation header
│   │   │   ├── fdtd_ez_sources.h       - Source header
│   │   │   └── fdtd_ez_state.h         - State header
│   │   ├── fdtd_accel.h                - Acceleration mode header
│   │   ├── fdtd_alloc.h                - Memory allocation header
│   │   ├── fdtd_const.h                - Physical constants header
│   │   ├── fdtd_ez.h                   - Ez module header
│   │   └── fdtd_types.h                - Data type header
│   └── fdtd.h                          - Public API header
├── docs                                - English thesis sources and results
├── src                                 - Source file directory
│   ├── fdtd                            - FDTD source file directory
│   │   ├── Ez                          - Ez simulation source directory
│   │   │   ├── fdtd_ez_coeffs.c        - Coefficient source file
│   │   │   ├── fdtd_ez_fields.c        - Field source file
│   │   │   ├── fdtd_ez_kernels.c       - Serial and OpenMP kernel
│   │   │   ├── fdtd_ez_material.c      - Material source file
│   │   │   ├── fdtd_ez_sim.c           - Simulation source file
│   │   │   ├── fdtd_ez_sources.c       - Source source file
│   │   │   └── fdtd_ez_state.c         - State source file
│   │   └── fdtd_alloc.c                - Memory allocation source file
│   ├── benchmark.c                     - Performance measurement source file
│   └── main.c                          - Simulation program source file
├── tools                               - Utility script directory
│   ├── benchmark.sh                    - Benchmark repetition script
│   ├── get_sys_info.sh                 - System information script
│   ├── plot_benchmark.py               - Performance plotting script
│   ├── plot_sim.py                     - Simulation plotting script
│   └── plot_figures.py                 - Standalone figure script
├── Makefile                            - Build and result rules
└── README.md                           - Project description
```

`make build` creates the distribution in `dist/`, including the executables, library, headers, tools, and build information:

```
dist                                    - Distribution directory
├── bin                                 - Executable directory
│   ├── tools                           - Utility script directory
│   │   ├── benchmark.sh                - Benchmark repetition script
│   │   ├── get_sys_info.sh             - System information script
│   │   ├── plot_benchmark.py           - Performance plotting script
│   │   ├── plot_sim.py                 - Simulation plotting script
│   │   └── plot_figures.py             - Standalone figure script
│   ├── benchmark                       - Performance measurement executable
│   └── fdtd                            - Field simulation executable
├── include                             - Header directory
│   ├── fdtd                            - FDTD header directory
│   │   ├── Ez                          - Ez simulation header directory
│   │   │   ├── fdtd_ez_coeffs.h        - Coefficient header
│   │   │   ├── fdtd_ez_fields.h        - Field header
│   │   │   ├── fdtd_ez_kernels.h       - Serial and OpenMP kernel header
│   │   │   ├── fdtd_ez_material.h      - Material header
│   │   │   ├── fdtd_ez_sim.h           - Simulation header
│   │   │   ├── fdtd_ez_sources.h       - Source header
│   │   │   └── fdtd_ez_state.h         - State header
│   │   ├── fdtd_accel.h                - Acceleration mode header
│   │   ├── fdtd_alloc.h                - Memory allocation header
│   │   ├── fdtd_const.h                - Physical constants header
│   │   ├── fdtd_ez.h                   - Ez module header
│   │   └── fdtd_types.h                - Data type header
│   └── fdtd.h                          - Public API header
├── lib                                 - Library directory
│   └── libfdtd.a                       - Simulation construction library
└── buildinfo                           - Build report
```

## Simulation

`make gauss` runs a Gaussian-pulse simulation for 100 time steps. The CSV result and plots of the fields, materials, coefficients, and PML layer are stored in `results/`.

The simulation can also be run directly:

```sh
dist/bin/fdtd --set-width=200 --add-pulse --run=200 simulation.csv
dist/bin/tools/plot_sim.py simulation.csv
```

## Benchmarking

`make bench` runs `tools/benchmark.sh` with the parameters from `Makefile`. The CSV result and performance plots are stored in `results/`.

## Illustrations

`make figures` creates plots that do not depend on simulation or measurement CSV results. `tools/plot_figures.py` illustrates the one-dimensional Yee grid and the thread fork/join model.

## Cleanup

`make clean` removes `obj/`, `dist/`, and `results/`.

## Python libraries

The Python utility scripts (`plot_sim.py`, `plot_benchmark.py`, and `plot_figures.py`) use the non-standard `matplotlib` and `numpy` libraries. Install them before running the corresponding `make` targets if they are unavailable:

```sh
python -m venv .venv
source .venv/bin/activate
pip install numpy matplotlib
```
