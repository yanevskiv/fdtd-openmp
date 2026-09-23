# Using the Software

The built distribution in the `dist` directory allows the software to be used either as a library or as a set of ready-made programs. This appendix presents the basic procedures for running a simulation, plotting its results, and measuring performance.

## Using the Library

The `dist/include` directory contains the public `fdtd.h` header and the other library headers, while the `dist/lib` directory contains the `libfdtd.a` static library. A user program includes `fdtd.h`, configures a simulation through the public functions, and finally releases the object with `Fdtd_EzSimDestroy()`. At compile time, the `-I` and `-L` options specify the paths to the headers and library.

The following example creates a grid of 200 cells, configures a PML, adds a Gaussian source, and performs 100 time steps. It writes the resulting state to `simulation.csv` for later analysis or resumption.

```c
#include <math.h>
#include <fdtd.h>

static void Pulse(Fdtd_EzSim *sim, Fdtd_EzSourceData *data)
{
    Fdtd_FloatType n = Fdtd_EzSimGetTime(sim);
    Fdtd_FloatType pulse =
        exp(-0.5 * pow((data->f1 - n) / data->f2, 2.0));
    Fdtd_EzSimInjectEz(sim, (size_t) data->f3, pulse);
}

int main(void)
{
    // Create a simulation with K = 200 cells
    Fdtd_EzSim *sim = Fdtd_EzSimCreate(200);

    // Configure a PML twenty cells wide
    Fdtd_EzSimSetPML(sim, 20);

    // Create a Gaussian source
    Fdtd_EzSourceData source = {
        .f1 = 40.0,   // Time of the pulse maximum (n0)
        .f2 = 12.0,   // Pulse width (w)
        .f3 = 100.0   // Source position (kc = 100)
    };

    Fdtd_EzSimAddSource(sim, Pulse, source);
    Fdtd_EzSimRun(sim, 100);
    Fdtd_EzSimWriteCSV(sim, "simulation.csv");
    Fdtd_EzSimDestroy(sim);
    return 0;
}
```

The example is compiled by linking against `libfdtd` with `-lfdtd` and the mathematics library with `-lm`. The `-fopenmp` option provides the OpenMP support required by the built library, while `-O3` enables a high optimization level. After compilation succeeds, the resulting program is run from the same directory.

```bash
gcc -O3 -fopenmp -Idist/include example.c \
    -Ldist/lib -lfdtd -lm -o example
./example
```

## Using the Program

The `dist/bin/fdtd` executable makes it possible to configure and run a simulation without writing a C program. Its options specify the grid width, discretization, materials, boundary condition, source, acceleration mode, and number of time steps. The complete list is available by running `dist/bin/fdtd --help`.

In the following example, we configure 200 cells, a PML twenty cells wide, and a Gaussian source, then perform 100 steps. If an output path is provided, the program writes the complete state to a CSV file; otherwise, it writes the state to standard output.

The `dist/bin/tools/plot_sim.py` script creates plots of the fields, materials, conductivities, and coefficients from a CSV state. The `-O` option selects the output directory, while `--prefix` and `--dpi` can change the filename prefix and plot resolution. Figure E.1 shows the `gauss_plot2d_Ez.png` file as an example of the script's output.

```bash
dist/bin/fdtd \
    --set-width=200 \
    --set-pml=20 \
    --add-pulse \
    --run=100 \
    simulation.csv
dist/bin/tools/plot_sim.py -O images simulation.csv
```

![Electric field $E_z$ produced by the simulation plotting script](results/images/gauss_plot2d_Ez.png)

## Using the Benchmark

The `dist/bin/benchmark` program measures the serial and OpenMP kernels for a grid width specified with `-K` and a number of time steps specified with `-N`. The `--max-threads` and `--repeat` options determine the largest thread count and the number of repetitions, while the result is written in CSV format.

The `dist/bin/tools/get_sys_info.sh` script retrieves information about the processor, cache memory, OpenMP environment, and other properties of the system. It writes this information as comments that can form the header of the `benchmark.csv` file. The benchmark output is appended to preserve those comments.

```bash
dist/bin/tools/get_sys_info.sh > benchmark.csv
dist/bin/benchmark \
    -K 80000 \
    -N 1024 \
    --max-threads=16 \
    --repeat=10 \
    >> benchmark.csv
```

The `dist/bin/tools/benchmark.sh` script repeats the same procedure for multiple grid widths and invokes the `benchmark` program supplied with the `-p` option. It writes results to standard output, which can be redirected to `benchmark.csv`. The `-n` option specifies the number of simulation steps, while `-r` specifies the number of measurement repetitions.

The `dist/bin/tools/plot_benchmark.py` script reads the resulting measurements and calculates throughput, speedup, and efficiency for every configuration. As with the simulation plots, the `-O` option selects the directory in which the images are written. Figure E.2 shows `benchmark_speedup.png` as an example of the script's output.

```bash
dist/bin/tools/get_sys_info.sh > benchmark.csv
dist/bin/tools/benchmark.sh \
    -p dist/bin/benchmark \
    -n 1024 \
    -r 10 \
    10000 20000 40000 80000 \
    >> benchmark.csv
dist/bin/tools/plot_benchmark.py -O images benchmark.csv
```

![Speedup of the OpenMP implementation produced by the benchmark plotting script](results/images/benchmark_speedup.png)
