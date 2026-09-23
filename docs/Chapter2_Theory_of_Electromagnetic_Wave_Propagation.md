# Theory of Electromagnetic Wave Propagation

The introduction explained why we use the FDTD method to simulate electromagnetic waves. In this chapter, we start from Maxwell's equations and derive a one-dimensional model for the $E_z$ and $H_y$ fields.

## Maxwell's Equations and Constitutive Relations

Maxwell's equations relate the electric and magnetic fields to the charge and current in the medium under consideration [@djordjevic2012]. Gauss's law describes the electric flux produced by electric charge, while the corresponding law for magnetism expresses the absence of magnetic monopoles. These two relations impose spatial constraints on the fields.

$$
\nabla \cdot \vec{D}=\rho \tag{2.1}
$$

$$
\nabla \cdot \vec{B}=0 \tag{2.2}
$$

Faraday's law relates the change in magnetic flux density to the curl of the electric field. The Ampère-Maxwell law relates electric current and the change in electric flux density to the curl of the magnetic field. These two curl equations are subsequently used to implement the simulation.

$$
\nabla \times \vec{E}=-\frac{\partial \vec{B}}{\partial t} \tag{2.3}
$$

$$
\nabla \times \vec{H}=\vec{J}+\frac{\partial \vec{D}}{\partial t} \tag{2.4}
$$

In this work, we consider a linear, isotropic, and time-invariant medium. Its response is described by the permittivity $\varepsilon$, permeability $\mu$, and electrical conductivity $\sigma_e$. The constitutive relations express $\vec{D}$, $\vec{B}$, and $\vec{J}$ in terms of the fields $\vec{E}$ and $\vec{H}$.

$$
\vec{D}=\varepsilon\vec{E} \tag{2.5}
$$

$$
\vec{B}=\mu\vec{H} \tag{2.6}
$$

$$
\vec{J}=\sigma_e\vec{E} \tag{2.7}
$$

## One-Dimensional Model

In the one-dimensional model, we assume that the field varies only along the $x$-axis and that the wave propagates through a homogeneous cross-section. Derivatives with respect to the $y$ and $z$ coordinates are then formally equal to zero, so a grid is not required in those dimensions. This assumption corresponds to the propagation of a plane wave through layers normal to the $x$-axis.

$$
\frac{\partial}{\partial y}=0,\qquad \frac{\partial}{\partial z}=0 \tag{2.8}
$$

Because of this spatial restriction, the one-dimensional model cannot represent diffraction, scattering around an obstacle, or a complex transverse wave profile. Nevertheless, it accurately represents wave propagation, reflection, changes in velocity, and attenuation along a single axis. Each cell represents a thin layer with distinct electric and magnetic properties.

The domain length $L$ is approximated by multiplying the integer number of cells $K$ by the spatial step $\Delta x$. This relation provides the basis for the subsequent discretization of the one-dimensional model, while variations in grid width are later used in the performance analysis.

$$
L=K\Delta x \tag{2.9}
$$

## The $E_z$ and $H_y$ Field Components

For the selected polarization, we retain the electric-field component $E_z$ and the magnetic-field component $H_y$. Both components are perpendicular to the direction of propagation and to each other. The remaining four field components are set to zero.

$$
\vec{E}=(0,0,E_z),\qquad \vec{H}=(0,H_y,0) \tag{2.10}
$$

The ratio of the electric- and magnetic-field amplitudes in a lossless medium is determined by the wave impedance $\eta$. Its value depends on the local permittivity and permeability, so a change in material may produce reflected and transmitted waves. An abrupt change in impedance produces a stronger reflection than a gradual change.

$$
\eta=\sqrt{\frac{\mu}{\varepsilon}} \tag{2.11}
$$

Wave velocity also depends on the permittivity and permeability of the medium. The vacuum constants $\varepsilon_0$ and $\mu_0$ correspond to the speed of light $c_0$. The program uses this reference velocity when determining the time step.

$$
c=\frac{1}{\sqrt{\mu\varepsilon}} \tag{2.12}
$$

## System of Differential Equations

After introducing the one-dimensional polarization, we reduce Maxwell's curl equations to two coupled scalar equations. The first describes the change in $H_y$ caused by the spatial variation of $E_z$, while the second describes the reciprocal effect of $H_y$ on $E_z$ and the attenuation caused by the electrical conductivity $\sigma_e$.

$$
\frac{\partial H_y}{\partial t}=\frac{1}{\mu}\frac{\partial E_z}{\partial x} \tag{2.13}
$$

$$
\frac{\partial E_z}{\partial t}=\frac{1}{\varepsilon}\frac{\partial H_y}{\partial x}-\frac{\sigma_e}{\varepsilon}E_z \tag{2.14}
$$

A second-order wave equation can also be derived from this system. We differentiate the equation for $E_z$ with respect to time and substitute the time derivative of $H_y$, assuming a homogeneous, lossless medium. The resulting equation relates wave velocity to the material properties and helps us select the resolution, although the FDTD method discretizes the original first-order system.

$$
\frac{\partial^2 E_z}{\partial x^2}=\mu\varepsilon\frac{\partial^2 E_z}{\partial t^2} \tag{2.15}
$$

## Symmetric System of Equations

The equations from the previous section can be made symmetric by introducing a fictitious magnetic conductivity $\sigma_m$ into Faraday's law. This additional term does not describe physical magnetic losses in a conventional material, but it allows the electric and magnetic equations to be written in the same form. We use this system to implement the absorbing boundary conditions in Chapter 4, while a more detailed derivation is provided in Appendix B.

The components $E_z$ and $H_y$ represent the electric and magnetic fields of the selected polarization. The permittivity $\varepsilon$ and permeability $\mu$ describe the electric and magnetic properties of the medium, while $\sigma_e$ denotes the electrical conductivity and $\sigma_m$ the fictitious magnetic conductivity. Derivatives with respect to the spatial coordinate $x$ couple the two field components, while the conductivity terms introduce their attenuation.

$$
\frac{\partial H_y}{\partial t}=\frac{1}{\mu}\frac{\partial E_z}{\partial x}-\frac{\sigma_m}{\mu}H_y \tag{2.16}
$$

$$
\frac{\partial E_z}{\partial t}=\frac{1}{\varepsilon}\frac{\partial H_y}{\partial x}-\frac{\sigma_e}{\varepsilon}E_z \tag{2.17}
$$

Both conductivities are zero by default in the interior region of the simulation, so the symmetric system reduces to the basic lossless system. In the absorbing layers at the ends of the simulation domain, we specify matched electric and magnetic losses, thereby reducing wave reflection from the grid boundaries.

## Material Properties

Material properties generally depend on position, which allows us to represent layered and inhomogeneous media. Relative permittivity $\varepsilon_r$ specifies the ratio with respect to vacuum, while relative permeability $\mu_r$ has the same role for the magnetic response. The absolute values are obtained by multiplying them by the corresponding physical constants.

$$
\varepsilon=\varepsilon_r\varepsilon_0,\qquad \mu=\mu_r\mu_0 \tag{2.18}
$$

A higher permittivity reduces the wave velocity if the permeability remains unchanged, while also changing the wave impedance. Electrical conductivity converts part of the field energy into heat and appears in the equation as an attenuation term. Magnetic conductivity is an auxiliary quantity used to match the electric and magnetic losses in the PML layer. The physical meanings and units of these quantities are summarized in Table 2.1.

Table: Material quantities in the theoretical model

| Quantity | Physical meaning | Unit |
| --- | --- | --- |
| $\varepsilon$ | permittivity of the medium | F/m |
| $\mu$ | permeability of the medium | H/m |
| $\sigma_e$ | electrical conductivity | S/m |
| $\sigma_m$ | equivalent magnetic conductivity | adjusted unit |

Spatial variations in these quantities change the local wave velocity and impedance, so reflected and transmitted components may arise at a material interface. Conductivity additionally determines how quickly the wave energy decays during propagation. In the next chapter, these quantities are distributed over the discrete spatial grid.
