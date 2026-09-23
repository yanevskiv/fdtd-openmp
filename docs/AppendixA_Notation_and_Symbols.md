\appendix

# Notation and Symbols

This appendix provides Table A.1, which lists the most important physical and numerical quantities introduced in the preceding chapters. Quantities marked with an arrow are vectors and depend on three spatial coordinates and time.

The discrete indices $k$ and $n$ denote a spatial point and a time step. The electric field is written as $E_z^n[k]$, while the magnetic field is staggered and has the form $H_y^{n+1/2}[k + 1/2]$. The increments $\Delta x$ and $\Delta t$ are related by the Courant number $S$.

Table: Fields and material properties

| Symbol | Meaning |
| --- | --- |
| $\vec{E}$, $E_z(x,t)$ | electric-field vector and component (V/m) |
| $\vec{H}$, $H_y(x,t)$ | magnetic-field vector and component (A/m) |
| $\vec{D}$ | electric displacement vector (C/m²) |
| $\vec{B}$ | magnetic flux density vector (T) |
| $\varepsilon(x,t)$, $\varepsilon_r(x,t)$ | absolute and relative permittivity (F/m, 1) |
| $\mu(x,t)$, $\mu_r(x,t)$ | absolute and relative permeability (H/m, 1) |
| $\sigma_e(x,t)$, $\sigma_m(x,t)$ | electric and magnetic conductivity (S/m, adjusted unit) |
| $K$ | number of spatial cells |
| $N$ | number of time steps |
| $C_{e1}[k]$, $C_{e2}[k]$ | electric-field update coefficients |
| $C_{h1}[k + 1/2]$, $C_{h2}[k + 1/2]$ | magnetic-field update coefficients |
| $p$ | number of processing threads |
| $T_p$ | execution time with $p$ threads |
| $S_p$ | parallel speedup |
| $E_p$ | parallel efficiency |
