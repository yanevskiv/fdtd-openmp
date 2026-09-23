# Pseudocode for the Finite-Difference Method Implementation

This appendix presents the FDTD algorithm as simplified C code. The blocks follow the order in which parameters are prepared, fields and materials are configured, coefficients are calculated, and time stepping is performed.

A more detailed analysis of the algorithm can be found in Sullivan's *Electromagnetic Simulation Using the FDTD Method* [@sullivan2013] and Taflove and Hagness's *Computational Electrodynamics: The Finite-Difference Time-Domain Method* [@taflove2005].

## Constants and Parameters

The constants `K` and `N` specify the number of spatial cells and time steps. The constants `MU_0`, `EPS_0`, and `C_0` describe free space and the wave speed from Equations (2.12) and (2.18), while `PI` is used to calculate the sinusoidal excitation from Equation (3.19).

```c
// Constants
#define K 200
#define N 100
#define PI 3.14159265
#define MU_0 1.25663706e-6
#define EPS_0 8.85418782e-12
#define C_0 299792458.0
```

## Arrays

The `Hy` and `Ez` arrays represent the field components from Equation (2.10), while the `mu`, `eps`, `sigma_m`, and `sigma_e` arrays describe the material parameters of the one-dimensional domain. The `Ch1`, `Ch2`, `Ce1`, and `Ce2` coefficients correspond to Equations (3.14)–(3.17) and are used directly when updating the fields. The quantities `S`, `dx`, and `dt` are related by the Courant condition in Equations (3.21) and (3.22).

```c
// Fields
double Hy[K], Ez[K];

// Material
double mu[K], eps[K], sigma_m[K], sigma_e[K];

// Update coefficients
double Ch1[K], Ch2[K], Ce1[K], Ce2[K];

// Simulation parameters
double S, dx, dt;
```

## Function Declarations

The declarations introduce the operations used below and show the organization of the pseudocode. The `Init()`, `SetHy()`, `SetEz()`, `SetMu()`, `SetEps()`, `SetPEC()`, `SetPML()`, and `UpdateCoeffs()` functions prepare the simulation, while `UpdateHy()`, `UpdateEz()`, `AddPulseEz()`, and `AddSineEz()` each perform one part of a time step. The `Fdtd_Run()` function defines their order, and `main()` provides an example of a complete run.

```c
// Initialization
void Init();

// Set fields and materials
void SetHy(double value, size_t start, size_t end);
void SetEz(double value, size_t start, size_t end);
void SetMu(double value, size_t start, size_t end);
void SetEps(double value, size_t start, size_t end);

// Configure boundaries
void SetPEC();
void SetPML(size_t pml_width);

// Update coefficients
void UpdateCoeffs();

// Update fields and excitations
void UpdateHy();
void UpdateEz();
void AddPulseEz(int n, double n0, double spread, size_t kc);
void AddSineEz(int n, double amp, double freq, double phase, size_t kc);

// Main functions
void Fdtd_Run(int nsteps, bool use_sine);
int main();
```

## Simulation Initialization

The `Init()` function sets the Courant number to its default value, $S=0.5$. It then calculates `dt` from the spatial increment `dx` and the speed `C_0` according to Equation (3.22). The resulting time step is later used by `UpdateCoeffs()` to calculate the update coefficients.

```c
// Initialize the time step
void Init()
{
    S = 0.5;
    dt = S * dx / C_0;
}
```

## Setting Fields and Materials

The `SetHy()` and `SetEz()` functions set the magnetic- and electric-field values in the selected half-open range `[start, end)`. Calling these functions with a value of zero over the entire domain produces the initial state.

```c
// Magnetic field
void SetHy(double value, size_t start, size_t end)
{
    for (size_t k = start; k < end; k++) {
        Hy[k] = value;
    }
}

// Electric field
void SetEz(double value, size_t start, size_t end)
{
    for (size_t k = start; k < end; k++) {
        Ez[k] = value;
    }
}
```

The `SetMu()` and `SetEps()` functions assign permeability and permittivity to the selected part of the domain. These arrays determine wave speed and impedance according to Equations (2.11), (2.12), and (2.18). `UpdateCoeffs()` is called after each change because the update coefficients depend directly on the material parameters.

```c
// Magnetic permeability
void SetMu(double value, size_t start, size_t end)
{
    for (size_t k = start; k < end; k++) {
        mu[k] = value;
    }

    // Update coefficients
    UpdateCoeffs();
}

// Electric permittivity
void SetEps(double value, size_t start, size_t end)
{
    for (size_t k = start; k < end; k++) {
        eps[k] = value;
    }

    // Update coefficients
    UpdateCoeffs();
}
```

## Configuring the Boundary Layer

The `SetPEC()` function sets the electric and magnetic conductivities to zero throughout the domain. This removes the loss terms from the differential Equations (2.16) and (2.17), after which a call to `UpdateCoeffs()` recalculates the coefficients. The `UpdateHy()` and `UpdateEz()` functions consequently retain the same form for differently configured boundaries.

```c
// PEC boundary condition
void SetPEC()
{
    for (size_t k = 0; k < K; k++) {
        // PEC conductivities
        sigma_m[k] = 0.0;
        sigma_e[k] = 0.0;
    }

    // Update coefficients
    UpdateCoeffs();
}
```

From the layer width, target reflection, and local impedance, the `SetPML()` function determines the maximum conductivity according to Equations (4.2) and (4.3). Electric and magnetic points have separate normalized depths, `de` and `dh`, which produce the polynomial conductivity profiles from Equations (4.4) and (4.5). After the `sigma_m` and `sigma_e` arrays have been set, `UpdateCoeffs()` incorporates their effects into the update coefficients.

```c
// PML boundary condition
void SetPML(size_t pml_width)
{
    // PML parameters
    int order = 3;
    double refl = 1.0e-6;
    double width = pml_width;
    double thickness = width * dx;

    // Conductivity grading
    for (size_t k = 0; k < K; k++) {
        // Maximum PML conductivity
        double eta = sqrt(mu[k] / eps[k]);
        double sigma_max = -(order + 1) * log(refl)
                         / (2.0 * eta * thickness);

        // PML depth of the E point
        double xe = k;
        double de = 0.0;
        if (xe < width) {
            de = (width - xe) / width;
        } else if (xe >= K - width) {
            de = (xe - (K - width) + 1.0) / width;
        }

        // PML depth of the H point
        double xh = k + 0.5;
        double dh = 0.0;
        if (xh < width) {
            dh = (width - xh) / width;
        } else if (xh >= K - width) {
            dh = (xh - (K - width) + 1.0) / width;
        }

        // PML conductivities and losses
        sigma_m[k] = sigma_max * pow(dh, order) * mu[k] / eps[k];
        sigma_e[k] = sigma_max * pow(de, order);
    }

    // Update coefficients
    UpdateCoeffs();
}
```

## Calculating Update Coefficients

For every cell, the `UpdateCoeffs()` function first calculates the dimensionless loss factors `a` and `b` from Equation (3.13). The `Ce1`, `Ce2`, `Ch1`, and `Ch2` coefficients are then obtained according to Equations (3.14)–(3.17). The coefficients are prepared before the time loop, allowing `UpdateHy()` and `UpdateEz()` to use them without repeating division in every step.

```c
// Update coefficients from material properties
void UpdateCoeffs()
{
    for (size_t k = 0; k < K; k++) {
        // Semi-implicit loss terms
        double b = dt * sigma_m[k] / (2.0 * mu[k]);
        double a = dt * sigma_e[k] / (2.0 * eps[k]);

        // Hy coefficients
        Ch1[k] = (1.0 - b) / (1.0 + b);
        Ch2[k] = dt / (dx * mu[k] * (1.0 + b));

        // Ez coefficients
        Ce1[k] = (1.0 - a) / (1.0 + a);
        Ce2[k] = dt / (dx * eps[k] * (1.0 + a));
    }
}
```

## Updating Fields and Excitations

The `UpdateHy()` function calculates the new magnetic field from the current electric field for indices from zero through `K - 2`, according to the update Equation (3.11). Only after it has finished do we call `UpdateEz()`, which uses the updated magnetic field and processes electric points from index one through `K - 1` according to Equation (3.12). The loop bounds correspond to the spatial staggering of $E_z$ and $H_y$ on the Yee grid in Equation (3.2).

```c
// Update Hy by one step
void UpdateHy()
{
    for (size_t k = 0; k < K - 1; k++) {
        Hy[k] = Ch1[k] * Hy[k] + Ch2[k] * (Ez[k + 1] - Ez[k]);
    }
}

// Update Ez by one step
void UpdateEz()
{
    for (size_t k = 1; k < K; k++) {
        Ez[k] = Ce1[k] * Ez[k] + Ce2[k] * (Hy[k] - Hy[k - 1]);
    }
}
```

The `AddPulseEz()` function calculates the Gaussian pulse from Equation (3.18), while `AddSineEz()` creates a sinusoidal excitation according to Equation (3.19). Both functions add the resulting value to cell `kc`, implementing the soft source injection from Equation (3.20). The source therefore does not replace the value previously calculated by `UpdateEz()`.

```c
// Gaussian excitation
void AddPulseEz(int n, double n0, double spread, size_t kc)
{
    double pulse = exp(-0.5 * pow((n0 - (double) n) / spread, 2.0));
    Ez[kc] += pulse;
}

// Sinusoidal excitation
void AddSineEz(int n, double amp, double freq, double phase, size_t kc)
{
    double sine = amp * sin(2.0 * PI * freq * (double) n + phase);
    Ez[kc] += sine;
}
```

## Main Function

The `Fdtd_Run()` function performs the specified number of time steps and selects the source type according to the `use_sine` parameter. In every step, it first calls `UpdateHy()`, then `UpdateEz()`, and finally `AddSineEz()` or `AddPulseEz()`. This order directly follows the update Equations (3.11) and (3.12), after which the source is added according to Equation (3.20).

```c
// FDTD algorithm
void Fdtd_Run(int nsteps, bool use_sine)
{
    for (int n = 0; n < nsteps; n++) {
        // Update Hy
        UpdateHy();

        // Update Ez
        UpdateEz();

        // Inject the source
        if (use_sine) {
            AddSineEz(n, 1.0, 0.05, 0.0, K / 2);
        } else {
            AddPulseEz(n, 40.0, 12.0, K / 2);
        }
    }
}
```

The `main()` function prepares an example simulation before entering the time loop. Calls to `SetHy()` and `SetEz()` establish zero initial field values, while `SetMu()` and `SetEps()` configure free space according to Equation (2.18). `SetPML()` then configures the absorbing layers, and `Fdtd_Run()` performs `N` steps with a Gaussian source because the `use_sine` parameter is `false`.

```c
// Main function
int main()
{
    // Prepare the simulation
    Init();

    // Set fields and materials
    SetHy(0.0, 0, K);
    SetEz(0.0, 0, K);
    SetMu(MU_0, 0, K);
    SetEps(EPS_0, 0, K);

    // Configure boundary conditions
    SetPML(20);

    // Simulate N steps with a Gaussian source
    Fdtd_Run(N, false);
}
```
