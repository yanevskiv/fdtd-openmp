# Introduction

This work addresses the parallel numerical simulation of electromagnetic wave propagation using the finite-difference time-domain method. This chapter defines the simulation problem, while the following chapters gradually derive a specific numerical model, describe its implementation, and examine its parallelization.

## Motivation and Scope

Reliable prediction of electromagnetic wave behavior is important in the development of antennas, transmission lines, sensors, and numerous electronic devices. Electromagnetic wave behavior is described by Maxwell's equations, formulated during the nineteenth century [@djordjevic2012]. These equations can generally be solved either analytically or numerically.

Analytical solutions to Maxwell's equations are available only for a limited number of relatively simple problems with suitable symmetry. Once different materials, boundary conditions, or time-varying excitations are introduced, practical calculations become considerably more difficult. A numerical model is therefore required.

This work considers a one-dimensional model with the $E_z$ and $H_y$ components, which clearly demonstrates the most important properties of the method without the complexity of a three-dimensional grid. The model still supports the simulation of propagation, reflection, material losses, and absorption at the boundaries. It provides a suitable basis for understanding the algorithm and examining its parallel execution.

## The Problem of Numerical Electromagnetic Simulation

An electromagnetic field varies continuously in space and time, whereas a computer can process only a finite number of values. In a numerical simulation, physical space is divided into cells and time into steps, so the numerical result inevitably represents an approximation. The main challenge is to ensure that this approximation remains stable and sufficiently faithful to the physical wave.

A finer grid represents the wave more accurately, but it also requires more memory and a greater number of operations. The time step cannot be increased freely to accelerate the computation because it is linked to the spatial step by a stability condition. A reliable simulation therefore requires a justified choice of parameters, rather than merely the largest possible number of samples.

A particular problem arises at the ends of a finite grid, where the wave may be artificially reflected and return to the observed region. Such a reflection can resemble a real physical phenomenon and lead to an incorrect interpretation of the results. For this reason, an absorbing layer that approximates the continuation of open space is introduced into the model.

## The Finite-Difference Time-Domain Method

The finite-difference time-domain (FDTD) method provides a direct solution to the problem described above [@sullivan2013]. Derivatives in Maxwell's equations are replaced by differences between neighboring samples, transforming complex continuous dynamics into a sequence of simple updates. This formulation makes it possible to establish a clear connection between the physical model and the corresponding program expressions.

The electric and magnetic fields are placed at staggered points on the Yee grid [@yee1966]. At each step, the magnetic field is first calculated from the electric field, after which the updated magnetic state is used to update the electric field [@taflove2005]. Bérenger's perfectly matched layer (PML) is used as the absorbing layer at the ends of the grid [@berenger1994].

A particular advantage of the FDTD method is that it directly represents the evolution of a wave over time and does not require the solution of a large system of equations. In a single simulation, a short Gaussian pulse can excite a broad frequency range and reveal how different wave components propagate through the grid. The method is therefore well suited to physical analysis, visualization, and the development of efficient software.

## The Need for Parallelization

The suitability of the FDTD method for simulation does not mean that its execution is always fast. Each time step processes almost every cell, so the total number of operations grows with both the grid length and the simulation duration. For fine or long-running models, computation time can limit the number of experiments that can be performed.

Updates to different cells within a single phase have separate destinations, which allows them to be distributed among multiple processor threads. This parallelism is introduced through the Open Multi-Processing (OpenMP) programming model and a small number of directives, while preserving a clear sequential structure [@openmp2021]. In this way, multiple cores of a single computer are used for the same physical computation.

Parallelization does not, however, always produce a speedup proportional to the number of threads, because thread creation and synchronization introduce overhead. Amdahl's law further shows that the sequential part of a program imposes an upper bound on speedup regardless of the number of available processing units [@amdahl1967]. Consequently, overhead may outweigh the benefit for small grids, while shared memory may limit performance for large grids, as demonstrated by the analysis in this work.

## Thesis Organization

This chapter has introduced the practical problem and explained the need for a numerical method and parallel execution. Chapter 2 begins with Maxwell's equations and derives the one-dimensional system for $E_z$ and $H_y$. Chapter 3 discretizes this system and obtains expressions that can be applied directly in the implementation.

Chapter 4 introduces the PML layer, which represents open space on a finite grid. Chapter 5 connects the mathematical model with the data structures, library, and sequential kernel implemented in the C programming language. Chapter 6 analyzes dependencies among iterations and presents the distribution of independent spatial iterations using the OpenMP model.

Chapter 7 defines the experimental environment, parameters, and reproducible measurement procedure. Chapter 8 analyzes visualizations of the fields and the measured performance indicators: execution time, throughput, speedup, and efficiency. Chapter 9 summarizes the results, identifies the limitations of the implementation, and proposes directions for further development.

Appendix A provides a table of the physical and numerical quantities used in the thesis. Appendix B presents a detailed derivation of the one-dimensional model, while Appendix C gives pseudocode for the finite-difference time-domain implementation. Appendix D describes the project architecture, data structures, and organization of the source code, distribution, and results, and Appendix E explains how to use the library, programs, and performance measurement tools.
