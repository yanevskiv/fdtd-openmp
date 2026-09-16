#!/usr/bin/env python3
# Author: Ivan Janevski (C) 2026

import os
import argparse
import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def save(args, fig, name):
    os.makedirs(args.output_dir, exist_ok=True)
    path = os.path.join(args.output_dir, args.prefix + name)
    fig.savefig(path, dpi=args.dpi, bbox_inches="tight")
    plt.close(fig)
    print("==> Creating image:", path)


def load(path):
    with open(path, encoding="utf-8") as f:
        rows = [line for line in f if not line.lstrip().startswith("#")]

    return np.genfromtxt(rows, delimiter=",", names=True)


def setup(ax, xlabel, ylabel, title):
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.grid(True, alpha=0.3)


def plot_2d(args, data):
    plots = {
        "Hy":      ("$H_y^{n+1/2}(k+1/2)$", "tab:red"),
        "Ez":      ("$E_z^n(k)$",            "tab:blue"),
        "mu_r":    ("$\\mu_r(k+1/2)$",       "tab:red"),
        "eps_r":   ("$\\epsilon_r(k)$",      "tab:blue"),
        "sigma_m": ("$\\sigma_m(k+1/2)$",    "tab:red"),
        "sigma_e": ("$\\sigma_e(k)$",        "tab:blue"),
        "ch1":     ("$c_{h1}(k+1/2)$",       "tab:red"),
        "ch2":     ("$c_{h2}(k+1/2)$",       "tab:red"),
        "ce1":     ("$c_{e1}(k)$",           "tab:blue"),
        "ce2":     ("$c_{e2}(k)$",           "tab:blue"),
    }

    k = data["k"]

    for column, (label, color) in plots.items():
        fig, ax = plt.subplots(figsize=(8, 4))

        ax.fill_between(k, data[column], color=color, alpha=0.3)
        ax.plot(k, data[column], color=color, label=label)

        setup(ax, "Cell k", label, f"Cross-section: {label}")
        ax.legend()

        save(args, fig, f"plot2d_{column}.png")


def plot_3d(args, data):
    k = data["k"]
    ez = data["Ez"]
    hy = data["Hy"]

    step = max(1, len(k) // 60)
    k = k[::step]
    ez = ez[::step]
    hy = hy[::step]

    ez /= max(np.max(np.abs(ez)), 1e-9)
    hy /= max(np.max(np.abs(hy)), 1e-9)

    amp = 0.35 * (k.max() - k.min())
    ez *= amp
    hy *= amp

    zero = np.zeros_like(k)

    fig = plt.figure(figsize=(9, 5))
    ax = fig.add_subplot(projection="3d")

    ax.plot(k, zero, zero, color="gray", lw=1)

    ax.quiver(k, zero, zero, zero, zero, ez, color="tab:blue", linewidth=1)
    ax.quiver(k, zero, zero, zero, hy, zero, color="tab:red", linewidth=1)

    ax.plot(k, zero, ez, color="tab:blue", alpha=0.4)
    ax.plot(k, hy, zero, color="tab:red", alpha=0.4)

    ax.set_ylim(-amp, amp)
    ax.set_zlim(-amp, amp)
    ax.set_xlabel("x (propagation)")
    ax.set_ylabel("y ($H_y$)")
    ax.set_zlabel("z ($E_z$)")
    ax.set_title("Three-dimensional illustration of plane-wave propagation")

    save(args, fig, "plot3d_EzHy.png")


def plot_pml(args, data):
    k = data["k"]
    sigma = data["sigma_e"]

    fig, ax = plt.subplots(figsize=(8, 4))

    ax.fill_between(k, sigma, color="tab:orange", alpha=0.3)
    ax.plot(k, sigma, color="tab:orange", label="$\\sigma_e$")

    setup(
        ax,
        "Cell k",
        "Conductivity $\\sigma_e$ (S/m)",
        "PML-layer conductivity profile",
    )
    ax.legend()

    save(args, fig, "pml.png")


def main():
    parser = argparse.ArgumentParser(
        description="Plot FDTD simulation graphs from a CSV file."
    )
    parser.add_argument("csv")
    parser.add_argument("-O", "--output-dir", default="images")
    parser.add_argument("--prefix")
    parser.add_argument("--dpi", type=int, default=120)
    args = parser.parse_args()

    if args.prefix is None:
        args.prefix = os.path.splitext(os.path.basename(args.csv))[0] + "_"

    data = load(args.csv)

    plot_2d(args, data)
    plot_3d(args, data)
    plot_pml(args, data)


if __name__ == "__main__":
    main()
