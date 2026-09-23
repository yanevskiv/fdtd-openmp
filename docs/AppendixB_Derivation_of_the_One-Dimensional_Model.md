# Derivation of the One-Dimensional Model

Chapter 2 presented the final system of differential equations for the $H_y$ and $E_z$ components. In this appendix, we derive that system in detail, starting from the differential forms of Faraday's law in Equation (2.3) and the Ampère-Maxwell law in Equation (2.4).

$$
\nabla \times \vec{E}=-\frac{\partial \vec{B}}{\partial t}
\tag{B.1}
$$

$$
\nabla \times \vec{H}=\vec{J}+\frac{\partial \vec{D}}{\partial t}
\tag{B.2}
$$

## The Nabla Operator

The nabla operator $\nabla$ provides a compact notation for spatial derivatives of a vector field. In the Cartesian coordinate system, its components are partial-derivative operators with respect to the $x$, $y$, and $z$ coordinates, while $\vec{i}_x$, $\vec{i}_y$, and $\vec{i}_z$ denote the corresponding unit vectors.

$$
\nabla=
\vec{i}_x\frac{\partial}{\partial x}
+
\vec{i}_y\frac{\partial}{\partial y}
+
\vec{i}_z\frac{\partial}{\partial z}
\tag{B.3}
$$

The curl of a vector field is obtained from the cross product of the nabla operator and that field. For the electric field $\vec{E}=(E_x,E_y,E_z)$, this product can be written as a determinant whose first row contains the unit vectors, whose second row contains the spatial derivatives, and whose third row contains the field components.

$$
\nabla\times\vec{E}
=
\begin{vmatrix}
\vec{i}_x & \vec{i}_y & \vec{i}_z \\
\frac{\partial}{\partial x} &
\frac{\partial}{\partial y} &
\frac{\partial}{\partial z} \\
E_x & E_y & E_z
\end{vmatrix}
\tag{B.4}
$$

The same procedure applies to the magnetic field $\vec{H}=(H_x,H_y,H_z)$. The following sections expand the curls of the electric and magnetic fields because they determine the spatial coupling of the components in the time-evolution equations.

## Time-Domain Form of the Equations

We begin with the two curl equations because they contain the time derivatives of the electric and magnetic flux densities. Into them we substitute the constitutive relations $\vec{D}=\varepsilon\vec{E}$, $\vec{B}=\mu\vec{H}$, and $\vec{J}=\sigma_e\vec{E}$ from Equations (2.5), (2.6), and (2.7).

In Faraday's law, we isolate the time derivative of the magnetic field; in the Ampère-Maxwell law, we isolate the time derivative of the electric field. The parameters $\mu$, $\varepsilon$, and $\sigma_e$ are treated as time-invariant properties of a linear, isotropic medium.

$$
\frac{\partial\vec{H}}{\partial t}
=
-\frac{1}{\mu}\nabla\times\vec{E}
\tag{B.5}
$$

$$
\frac{\partial\vec{E}}{\partial t}
=
\frac{1}{\varepsilon}\nabla\times\vec{H}
-
\frac{\sigma_e}{\varepsilon}\vec{E}
\tag{B.6}
$$

In the resulting equations, the time derivative of the magnetic field depends on the curl of the electric field, while the time derivative of the electric field depends on the curl of the magnetic field and on electrical losses. The curls still contain all three spatial components, so the next section expands them to obtain a scalar equation for each field component.

## Expansion of the Curls and Scalar Form of the Equations

We expand the curl of the electric field in terms of the $E_x$, $E_y$, and $E_z$ components and the curl of the magnetic field in the same manner using $H_x$, $H_y$, and $H_z$. Each curl component contains the difference of two spatial derivatives that describe the variation of the field in the plane normal to the direction under consideration.

$$
\nabla\times\vec{E}
=
\left(
\frac{\partial E_z}{\partial y}-\frac{\partial E_y}{\partial z},
\frac{\partial E_x}{\partial z}-\frac{\partial E_z}{\partial x},
\frac{\partial E_y}{\partial x}-\frac{\partial E_x}{\partial y}
\right)
\tag{B.7}
$$

$$
\nabla\times\vec{H}
=
\left(
\frac{\partial H_z}{\partial y}-\frac{\partial H_y}{\partial z},
\frac{\partial H_x}{\partial z}-\frac{\partial H_z}{\partial x},
\frac{\partial H_y}{\partial x}-\frac{\partial H_x}{\partial y}
\right)
\tag{B.8}
$$

The expanded curls are substituted into their corresponding time-domain equations. The first three scalar equations describe the time variation of the magnetic-field components, while the next three describe the time variation of the electric-field components in the presence of electrical losses.

$$
\frac{\partial H_x}{\partial t}
=
-\frac{1}{\mu}
\left(
\frac{\partial E_z}{\partial y}
-
\frac{\partial E_y}{\partial z}
\right)
\tag{B.9}
$$

$$
\frac{\partial H_y}{\partial t}
=
-\frac{1}{\mu}
\left(
\frac{\partial E_x}{\partial z}
-
\frac{\partial E_z}{\partial x}
\right)
\tag{B.10}
$$

$$
\frac{\partial H_z}{\partial t}
=
-\frac{1}{\mu}
\left(
\frac{\partial E_y}{\partial x}
-
\frac{\partial E_x}{\partial y}
\right)
\tag{B.11}
$$

$$
\frac{\partial E_x}{\partial t}
=
\frac{1}{\varepsilon}
\left(
\frac{\partial H_z}{\partial y}
-
\frac{\partial H_y}{\partial z}
\right)
-
\frac{\sigma_e}{\varepsilon}E_x
\tag{B.12}
$$

$$
\frac{\partial E_y}{\partial t}
=
\frac{1}{\varepsilon}
\left(
\frac{\partial H_x}{\partial z}
-
\frac{\partial H_z}{\partial x}
\right)
-
\frac{\sigma_e}{\varepsilon}E_y
\tag{B.13}
$$

$$
\frac{\partial E_z}{\partial t}
=
\frac{1}{\varepsilon}
\left(
\frac{\partial H_y}{\partial x}
-
\frac{\partial H_x}{\partial y}
\right)
-
\frac{\sigma_e}{\varepsilon}E_z
\tag{B.14}
$$

These six equations describe the time variation of all electromagnetic-field components in three-dimensional space. For the model in this work, we retain only the spatial and vector components required for propagation along a single axis.

## One-Dimensional System of Equations

We consider a plane electromagnetic wave propagating in one direction. Without loss of generality, propagation is chosen along the $x$ axis, so the fields depend only on the coordinate $x$ and time $t$. Derivatives with respect to the transverse coordinates $y$ and $z$ are therefore zero.

For the polarization, we choose the electric field in the $z$ direction and the magnetic field in the $y$ direction. Only the $E_z$ and $H_y$ components are retained, while the remaining four components are set to zero.

$$
\frac{\partial}{\partial y}=0,
\qquad
\frac{\partial}{\partial z}=0,
\qquad
E_x=0,
\qquad
E_y=0,
\qquad
H_x=0,
\qquad
H_z=0
\tag{B.15}
$$

We first apply the conditions of the one-dimensional model to the magnetic-field equations. The equations for $H_x$ and $H_z$ become trivial, while the equation for $H_y$ retains the spatial derivative of $E_z$ with respect to $x$.

$$
\frac{\partial H_y}{\partial t}
=
\frac{1}{\mu}\frac{\partial E_z}{\partial x}
\tag{B.16}
$$

We then apply the same conditions to the electric-field equations. The equations for $E_x$ and $E_y$ become trivial, while the equation for $E_z$ retains the spatial derivative of $H_y$ and the electrical-loss term.

$$
\frac{\partial E_z}{\partial t}
=
\frac{1}{\varepsilon}\frac{\partial H_y}{\partial x}
-
\frac{\sigma_e}{\varepsilon}E_z
\tag{B.17}
$$

The remaining two equations form a coupled one-dimensional system that is first order in space and time. A change in $E_z$ determines the new value of $H_y$, while a change in $H_y$ in turn determines the new value of $E_z$.

## Magnetic Currents and a Symmetric System of Equations

Faraday's law and the Ampère-Maxwell law have similar structures, but Faraday's law lacks a term corresponding to the free-charge current density $\vec{J}$. This asymmetry results from the physical absence of magnetic monopoles that would act as carriers of magnetic charge.

For the purposes of a symmetric mathematical model, however, we can formally introduce the magnetic current density $\vec{J}_m$ and denote the existing quantity $\vec{J}$ as the electric current density $\vec{J}_e$. In the linear, isotropic medium under consideration, both current densities are related to their corresponding fields through constitutive relations, where $\sigma_m$ denotes the fictitious magnetic conductivity and $\sigma_e$ the electrical conductivity.

$$
\vec{J}_m = \sigma_m \vec{H}
\tag{B.18}
$$

$$
\vec{J}_e = \sigma_e \vec{E}
\tag{B.19}
$$

Using the introduced magnetic current density, we extend Faraday's law with the term $-\vec{J}_m$, while the Ampère-Maxwell law retains the same form with the notation $\vec{J}_e$. This gives a symmetric system of vector equations.

$$
\nabla \times \vec{E}=-\vec{J}_m-\frac{\partial \vec{B}}{\partial t}
\tag{B.20}
$$

$$
\nabla \times \vec{H}=+\vec{J}_e+\frac{\partial \vec{D}}{\partial t}
\tag{B.21}
$$

Following the same procedure as in the preceding sections, we substitute the constitutive relations and apply the selected propagation direction and polarization. This yields the symmetric one-dimensional system for the $H_y$ and $E_z$ components.

$$
\frac{\partial H_y}{\partial t}
=
\frac{1}{\mu}\frac{\partial E_z}{\partial x}
-
\frac{\sigma_m}{\mu}H_y
\tag{B.22}
$$

$$
\frac{\partial E_z}{\partial t}
=
\frac{1}{\varepsilon}\frac{\partial H_y}{\partial x}
-
\frac{\sigma_e}{\varepsilon}E_z
\tag{B.23}
$$

The resulting system consists precisely of Equations (2.16) and (2.17), which Chapter 2 uses as the starting point for discretization. Introducing fictitious magnetic conductivity makes it possible to match electric and magnetic losses, which is important for implementing absorbing boundary conditions such as the Berenger PML presented in Chapter 4.
