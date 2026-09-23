# Conclusion

The preceding chapters presented the implementation in detail, together with an analysis of the serial and parallel implementations of the finite-difference time-domain method. This conclusion summarizes the results, limitations, and possible directions for further development.

## Achieved Results

We derived a one-dimensional model for the $E_z$ and $H_y$ components with the spatial and temporal staggering of the Yee grid. Electrical and magnetic losses were incorporated into the discrete expressions, so the same form of the update equations applies both in the interior region and in the PML. The Gaussian-pulse plots show the expected splitting and propagation of the wave in both directions.

The model was implemented as a static library and a command-line program written in C. The state, fields, material, coefficients, and sources were separated into distinct structures, facilitating verification and further extension of the implementation. The serial and OpenMP kernels use the same data and identical update equations.

Measurements on eight physical and sixteen logical cores showed that the benefit of parallelization depends strongly on the size of the working set. The greatest speedup is 6.218 for 80,000 cells and sixteen threads, while 40,000 cells with eight threads reaches 6.109. On the largest grid of 1,280,000 cells, the best speedup is only 1.195 with four threads, demonstrating the limitation imposed by the memory subsystem.

## Contributions of the Work

The theoretical part of this work connects Maxwell's equations, one-dimensional polarization, and the final expressions used in the program. Particular attention was given to the half-time-step position $H_y^{n+1/2}[k + 1/2]$ and the spatial positions of the magnetic coefficients. This consistency simplifies verification of the loop bounds and the meaning of each program array.

The software portion demonstrates how the physical model can be separated from the execution method. A user program can change the material, source, or boundary without modifying the serial and parallel expressions. The same library supports simulation examples, resumption from a saved state, and reproducible performance measurements.

The experimental portion does not present only the best result, but compares multiple grid widths, thread counts, and repeated samples. This comparison reveals the transition from a regime dominated by thread-management overhead to one in which multiple cores provide a substantial benefit. The observed variation also warns that a single measurement may give a misleading picture.

## Limitations of the Implemented Solution

The model is one-dimensional and cannot represent diffraction, complex geometry, or transverse modes. It considers only the polarization with the $E_z$ and $H_y$ components, while another polarization would require a separate set of functions. The results therefore cannot be transferred directly to every three-dimensional electromagnetic problem.

The PML implementation uses a fixed cubic profile and a fixed target reflection. The user can change the layer width, but cannot change its grading order or target reflection through the public interface. Absorption is verified from the shape of the fields, without a separate measurement of the reflection coefficient.

The measurements were performed on a single Intel Core i7-11700F processor with eight physical cores, so they do not characterize every possible environment. Hardware performance counters were not used to separate the effects of cache behavior, memory bandwidth, clock frequency, and execution units. Possible causes can therefore be inferred only from the code and the measured curves.

## Directions for Future Development

A natural extension is the introduction of two- and three-dimensional domains with additional field components. A greater number of cells would increase the need for parallelism, but would also introduce more complex boundaries and data exchange between spatial blocks. The library's existing separation of components can serve as a foundation for new polarizations and simulation types.

The parallel kernel already retains the same team of threads throughout the entire sequence of time steps and distributes both spatial loops using `omp for` directives. Barriers between dependent phases and after source application nevertheless remain necessary in every step. Their cost could be reduced by more sophisticated spatial and temporal blocking, but such a change would require a different data organization and additional correctness verification.

Future measurements could include more processor architectures, higher core counts, and hardware performance counters. Automatic selection of serial or parallel execution could use the grid width and a short initial measurement. This would allow the library to retain a simple interface while choosing the more favorable execution method itself.

## Recommendations for Use

Based on the medians of ten repetitions, Table 9.1 lists the fastest measured configuration for each grid width. Parallel execution provides a benefit in every tested case, but the most favorable thread count decreases for large working sets. These recommendations apply to the processor used, the thread-binding policy, and the current implementation.

Table: Fastest i7-11700F configurations

| Grid width | Thread count | Median speedup | Median throughput |
| ---: | ---: | ---: | ---: |
| 10,000 | 7 | 2.636 | 3,782.09 Mcells/s |
| 20,000 | 8 | 5.423 | 5,776.34 Mcells/s |
| 40,000 | 8 | 6.109 | 5,768.27 Mcells/s |
| 80,000 | 16 | 6.218 | 5,852.47 Mcells/s |
| 160,000 | 8 | 3.763 | 3,568.99 Mcells/s |
| 320,000 | 6 | 3.448 | 3,033.04 Mcells/s |
| 640,000 | 5 | 1.870 | 1,143.72 Mcells/s |
| 1,280,000 | 4 | 1.195 | 619.30 Mcells/s |

Medium-sized grids of 20,000 to 80,000 cells make the best use of the available cores and reduce execution time by factors ranging from 5.423 to 6.218. As the grid grows further, the optimal thread count decreases, and on a grid of 1,280,000 cells four threads provide a benefit of only 19.5%. These recommendations should be reevaluated after changing the processor, compiler, thread binding, or build configuration.

The choice should not be based solely on the number of available threads, but also on the grid width, timing stability, and overall system load. When multiple independent simulations are executed, it may be preferable to assign them to separate groups of cores instead of allocating all sixteen threads to each one. For a single simulation, the measured maximum throughput should be selected because the largest thread count is not automatically the fastest.
