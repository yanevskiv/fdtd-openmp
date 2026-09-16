#!/usr/bin/env python3
# Author: Ivan Janevski (C) 2026

import argparse
import os

import matplotlib
import numpy as np

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle


def save(args, fig, name):
    os.makedirs(args.output_dir, exist_ok=True)
    path = os.path.join(args.output_dir, name)
    fig.savefig(path, dpi=args.dpi, bbox_inches="tight")
    plt.close(fig)
    print("==> Creating image:", path)

# Yee-grid figure (figure_yee.png)
def draw_yee(args):
    n = 6
    xe = np.arange(n + 1)
    xh = np.arange(n) + 0.5

    fig, ax = plt.subplots(figsize=(12, 2.8))

    ax.axhline(0, color="gray", lw=1)
    ax.scatter(
        xe,
        np.zeros_like(xe),
        s=90,
        color="tab:blue",
        label="$E_z$ (integer nodes)",
    )
    ax.scatter(
        xh,
        np.zeros_like(xh),
        s=90,
        marker="s",
        color="tab:red",
        label="$H_y$ (half-integer nodes)",
    )

    for x in xe:
        ax.annotate(
            f"$E_z^n[{x}]$",
            (x, 0.10),
            ha="center",
            color="tab:blue",
            fontsize=12,
        )

    for x in xh:
        ax.annotate(
            f"$H_y^{{n+1/2}}[{int(2*x)}/2]$",
            (x, -0.15),
            ha="center",
            color="tab:red",
            fontsize=12,
        )

    ax.set_xlim(-0.3, n + 0.3)
    ax.set_ylim(-0.4, 0.4)
    ax.set_xlabel("x / $\\Delta x$")
    ax.set_yticks([])
    ax.set_title(
        "The $E_z$ and $H_y$ components on a one-dimensional Yee grid"
    )
    ax.legend(loc="upper right", fontsize=8)

    save(args, fig, "figure_yee.png")

# Fork/join figure (figure_fork_join.png)
def draw_fork_join(args):
    fig, ax = plt.subplots(figsize=(8, 4.8))

    # Base coordinates
    x_main = 3.0
    y_fork = 4.25
    y_join = 2.35

    # Thread dimensions
    thread_width = 0.7
    thread_height = y_fork - y_join
    thread_spacing = thread_width

    # Helper-text positions
    x_left = 0.2
    x_pragma = 6.05
    x_fork_join_label = x_main - 0.15

    # Arrow
    ax.annotate(
        "",
        xy=(x_main, 1.05),
        xytext=(x_main, 5.55),
        arrowprops={"arrowstyle": "->", "color": "black", "lw": 1.4},
    )

    # Left-side text
    left_labels = [
        ("Serial section", 4.9),
        ("Parallel region", 3.2),
        ("Serial section", 1.7),
    ]
    for text, y in left_labels:
        ax.text(x_left, y, text, fontsize=12, va="center")

    # Thread rectangles
    labels = ("Thread 1", "Thread 2", "…", "Thread $p$")
    hatches = (None, "///", "\\\\", "xxx")
    colors = ("#1f77b4", "#ff7f0e", "#2ca02c", "#d62728")

    for i, (label, hatch, color) in enumerate(
        zip(labels, hatches, colors)
    ):
        x = x_main + thread_spacing * i

        ax.add_patch(Rectangle(
            (x, y_join),
            thread_width,
            thread_height,
            facecolor=color,
            edgecolor="black",
            linewidth=1.2,
            hatch=hatch,
        ))

        ax.text(
            x + thread_width / 2,
            y_join - 0.23,
            label,
            ha="center",
            va="top",
            fontsize=9,
        )

    # Fork/join points
    ax.plot(x_main, y_fork, "ko", markersize=5)
    ax.plot(x_main, y_join, "ko", markersize=5)

    # OpenMP pragma
    pragma_style = {
        "color": "red",
        "family": "monospace",
        "va": "center",
    }

    ax.text(
        x_pragma, y_fork + 0.30,
        "#pragma omp parallel",
        fontsize=10,
        **pragma_style,
    )
    ax.text(
        x_pragma, y_fork,
        "{",
        fontsize=11,
        **pragma_style,
    )
    ax.text(
        x_pragma, y_join,
        "}",
        fontsize=11,
        **pragma_style,
    )

    # Fork/join labels
    label_style = {
        "ha": "right",
        "va": "center",
        "fontsize": 9,
        "fontweight": "bold",
    }

    ax.text(
        x_fork_join_label, y_fork,
        "fork",
        **label_style,
    )
    ax.text(
        x_fork_join_label, y_join,
        "join",
        **label_style,
    )

    # Plot boundaries
    ax.set_xlim(0, 8)
    ax.set_ylim(0.75, 5.8)
    ax.axis("off")

    save(args, fig, "figure_fork_join.png")




def main():
    parser = argparse.ArgumentParser(
        description="Plot illustrations independent of simulation results."
    )
    parser.add_argument("-O", "--output-dir", default="images")
    parser.add_argument("--dpi", type=int, default=120)
    args = parser.parse_args()

    draw_yee(args)
    draw_fork_join(args)


if __name__ == "__main__":
    main()
