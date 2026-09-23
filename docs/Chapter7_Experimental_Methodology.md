# Experimental Methodology

In the preceding chapter, we presented the parallel version of the FDTD algorithm. We now define how its performance is measured and compared with that of the serial version.

## Hardware and Software Environment

All performance measurements were performed on the `rtidev5.etf.rs` computer, using an Intel Core i7-11700F processor with eight physical and sixteen logical cores. The availability of two hardware threads per core allows execution with up to eight threads and the use of simultaneous multithreading above that limit to be examined separately. Details of the hardware and software environment are given in Table 7.1.

Table: Measurement environment recorded with the results

| Property | Value |
| --- | --- |
| computer | rtidev5.etf.rs |
| processor model | 11th Gen Intel Core i7-11700F @ 2.50 GHz |
| physical cores | 8 |
| logical cores | 16 |
| threads per core | 2 |
| L2 cache | 4 MiB, eight instances |
| L3 cache | 16 MiB, one instance |
| compiler | GCC 11.4.0 |
| compiler options | `-O3 -march=native -fopenmp -Wall` |
| measurement date | August 15, 2026 |

The program was compiled with a high optimization level and OpenMP support. The `-march=native` option permits the use of instructions available on the particular processor, while `-O3` enables aggressive loop optimization. The same options are used for serial and parallel execution, making their comparison fair.

The program automatically writes system information as comments at the beginning of the resulting CSV file. The same header records `OMP_DYNAMIC=false`, the placement policy `OMP_PLACES=cores`, and the binding policy `OMP_PROC_BIND=close`. The plotting script skips comment lines, but they remain available for subsequent verification.

## Simulation Parameters

The measurements cover domains whose widths double from 10,000 to 1,280,000 cells because the relationship between computational work and memory traffic changes with the size of the working set. Each run performs 1,024 time steps. This choice provides a sufficiently long measurement interval and makes adjacent domain sizes easy to compare. Table 7.2 summarizes all measurement parameters.

Table: Basic measurement parameters

| Parameter | Values |
| --- | --- |
| width $K$ | 10,000; 20,000; 40,000; 80,000; 160,000; 320,000; 640,000; 1,280,000 |
| number of steps $N$ | 1,024 |
| number of threads $p$ | 1 to 16 |
| number of repetitions | 10 |
| PML width | 20 cells at each boundary |
| source | Gaussian pulse at the center |

Each test simulation has a PML twenty cells wide and a Gaussian source at the center of the grid. The pulse parameters are $n_0=40$ and $w=12$, corresponding to the example used to verify physical behavior. Although the source adds a small amount of serial work, it ensures that the measured loop represents realistic use of the library.

Serial execution is measured separately, while parallel execution is measured for every thread count from one through sixteen. A parallel measurement with one thread isolates the OpenMP overhead without benefiting from an additional core. Values above eight threads show whether the second hardware thread on each physical core provides any further benefit.

## Performance Measurement Methodology

Before each group of measurements, the simulation is repeatedly reset and run until the warm-up lasts for at least 0.1 seconds. This activates memory pages, loads the code into cache, and starts the OpenMP runtime before measurement. Each measurement then begins by resetting the same state again.

Time is measured with `omp_get_wtime()` immediately before and after the call to `Fdtd_EzSimRun()`. Simulation creation, coefficient preparation, file opening, and plot generation are excluded from the measured interval. This isolates the time loop that is the subject of parallelization.

Each combination of grid width and thread count is repeated ten times because the operating system, processor frequency, and cache state affect execution time. The median is used as the representative value for each group, and the reference time for speedup is the median of the serial measurements for the same width. Every individual row is retained in the CSV file for subsequent verification.

## OpenMP Environment Control

The number of worker threads is set before each group of measurements with `omp_set_num_threads()`. Ten samples are then collected with the same number of threads before moving to the next value. This order reduces the number of environment changes and simplifies interpretation of the output table.

Iteration distribution in both kernels uses `schedule(static)`, so each thread receives an approximately equal contiguous block. Because every iteration contains the same numerical expression, no imbalance that would justify dynamic scheduling is expected. Static scheduling also has low management overhead and a predictable memory-access pattern.

The thread count is not increased beyond the sixteen logical cores recorded in the environment. Up to eight threads, each thread can use a separate physical core; higher values activate the second hardware thread on some of the cores. Results above eight threads must therefore also be interpreted in terms of shared execution and memory resources within each core.

## Performance Metrics

The primary measured quantity is the time $T_p$ required to perform $N$ steps on a grid of width $K$ with $p$ threads. Time alone is insufficient for comparing different widths because a larger grid performs more useful work. We therefore also calculate throughput in Mcells/s.

$$
P_p=\frac{KN}{10^6T_p}
\tag{7.1}
$$

Speedup $S_p$ is the ratio of the median serial execution time $\widetilde{T}_{\mathrm{seq}}$ to the execution time of the parallel version being considered. A value greater than one indicates that parallelization provides a benefit, whereas a value below one indicates a slowdown. Ideal speedup equals the number of threads, but is not expected in a real program because of serial work, synchronization, and shared hardware resources.

$$
S_p=\frac{\widetilde{T}_{\mathrm{seq}}}{T_p}
\tag{7.2}
$$

Efficiency $E_p$ divides speedup by the number of threads and indicates the utilization of the additional processing resources. A value close to one represents nearly linear speedup, while a lower value indicates growing overhead or saturation of a shared resource. Considering throughput, speedup, and efficiency together provides a more complete picture than any single metric.

$$
E_p=\frac{S_p}{p}
\tag{7.3}
$$

## Verification of Numerical Correctness

A faster program is of no value if it changes the physical result, so the parallelization uses the same expressions and loop bounds as the reference implementation. Different threads write to distinct elements, while the implicit barrier completes the $H_y$ phase before the $E_z$ phase begins. This preserves the same ordering of dependent operations in every time step.

Field verification includes the positions of the two parts of the Gaussian pulse, the opposite sign of the magnetic component, and attenuation within the PML. Plots of the material parameters additionally confirm that losses are present only at the ends of the domain. These tests can reveal an indexing error that would not be apparent from execution time alone.

The same simulation was executed in serial mode and in OpenMP mode with four threads, after which the complete output CSV files were compared. After one hundred time steps, all parameters and field values were identical. This comparison confirms that the parallel distribution did not alter the numerical result.

## Statistical Processing of Repeated Measurements

Repeated measurements are not completely identical because the operating system occasionally interrupts the program, while processor frequency and cache state vary. For each grid width, execution mode, and thread count, the ten obtained values are sorted. The median is calculated as the mean of the fifth and sixth samples in this ordered sequence.

The plots show median execution times from which throughput, speedup, and efficiency are derived. This statistic is less sensitive to an occasional slow execution than the arithmetic mean and therefore describes a typical run more accurately. The same median serial execution time serves as the reference for all OpenMP configurations at a given width.

The original CSV remains part of the project and contains all 1,360 measured rows, allowing the dispersion and alternative statistical measures to be examined later. The plots do not show intervals of variation, so small differences between adjacent thread counts should not be interpreted as a certain advantage. Nevertheless, ten repetitions provide a more stable basis than a single isolated measurement.

## Threats to Experimental Validity

Internal validity depends on whether the measured time actually belongs to the computational kernel. Timed warm-up, state resetting, and narrowly placed timestamps reduce the effect of initialization and result output. Operating-system activity and dynamic changes in clock frequency may nevertheless affect individual measurements.

External validity is limited by the use of a single processor model, one compiler, and at most sixteen threads. Another architecture may have different cache sizes, vector instructions, and memory bandwidth, so the exact break-even points cannot be considered universal. The measurement procedure is nevertheless reproducible and can be applied to another machine without changing the metrics.

Construct validity depends on the chosen throughput metric, which counts one cell once per time step even though there are two spatial loops. The CSV records only the measured time in seconds, while the script consistently calculates throughput in Mcells/s from $K$, $N$, and time. Before comparing these results with other programs, it is necessary to verify that they use the same definition of throughput.

## Reproducing the Measurements

The entire set of measurements can be reproduced from the `fdtd-hpc` directory using the following `make` commands. The first command builds the program with the recorded compiler options and runs the measurement script for eight grid widths and ten repetitions. The plots used in the analysis can then be regenerated from the same CSV file.

```bash
make bench
make bench_images
```

The `results/benchmark.csv` file begins with comments containing information about the compiler, processor, cache memory, and OpenMP environment, followed by the individual measurements in tabular form. The `accel` column identifies the kernel used, `threads` gives the number of threads, `width` is the grid width, `nsteps` is the number of time steps, `repeat` is the repetition index, and `seconds` is the measured execution time. The CSV header has the following form:

```csv
# cflags: -O3 -march=native -fopenmp -Wall -Iinclude
# compiler: gcc (Ubuntu 11.4.0-1ubuntu1~22.04.3) 11.4.0
# timestamp: 2026-08-15T15:13:59Z
# hostname: rtidev5
# cpu_model: 11th Gen Intel(R) Core(TM) i7-11700F @ 2.50GHz
# cores_physical: 8
# cores_logical: 16
# threads_per_core: 2
# numa_nodes: 1
# l2_cache: 4 MiB (8 instances)
# l3_cache: 16 MiB (1 instance)
# omp_dynamic: false
# omp_places: cores
# omp_proc_bind: close
accel,threads,width,nsteps,repeat,seconds
```

The `make bench_images` command processes the measurements from the CSV file and writes five plots whose names begin with `benchmark_` to `dist/results/images`. These plots show efficiency $E_p$, speedup $S_p$, and throughput $P_p$; speedup and throughput are also plotted as functions of the grid width $K$.

```text
dist/results/images/
|-- benchmark_efficiency.png
|-- benchmark_speedup.png
|-- benchmark_speedup_k.png
|-- benchmark_throughput.png
`-- benchmark_throughput_k.png
```
