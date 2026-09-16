#!/usr/bin/env python3
# Author: Ivan Janevski (C) 2026

import os
import argparse
import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

MARKERS = ["o", "s", "^", "D", "v", "P", "X", "*"]
SELECTED_THREADS = [1, 2, 4, 8, 16]

# Read CSV
def load(path):
    with open(path, encoding="utf-8") as f:
        rows = [line for line in f if not line.lstrip().startswith("#")]

    return np.genfromtxt(
        rows,
        delimiter=",",
        names=True,
        dtype=None,
        encoding="utf-8",
    )


# Calculate derived performance metrics
def calculate(data):
    throughput = data["width"] * data["nsteps"] / data["seconds"] / 1e6
    speedup = np.ones(len(data))

    for width in np.unique(data["width"]):
        serial = data[(data["accel"] == "none") & (data["width"] == width)]
        parallel = (data["accel"] == "openmp") & (data["width"] == width)
        speedup[parallel] = np.median(serial["seconds"]) / data["seconds"][parallel]

    efficiency = speedup / data["threads"]
    return throughput, speedup, efficiency


# Median results for each x value
def statistics(data, values, x_column):
    xs = np.unique(data[x_column])
    ys = []

    for x in xs:
        selected = values[data[x_column] == x]
        ys.append(np.median(selected))

    return xs, ys


# Save a plot
def save(fig, args, name):
    os.makedirs(args.output_dir, exist_ok=True)

    path = os.path.join(args.output_dir, args.prefix + name)
    fig.savefig(path, dpi=args.dpi, bbox_inches="tight")
    plt.close(fig)

    print("==> Creating image:", path)


# Common plot settings
def setup(ax, xlabel, ylabel, title):
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.grid(axis="y", alpha=0.3)


# Plot by thread count, with one curve for each grid size
def plot_by_threads(
    args,
    data,
    values,
    ylabel,
    title,
    filename,
    ideal=None,
):
    omp = data[data["accel"] == "openmp"]
    omp_values = values[data["accel"] == "openmp"]

    widths = np.unique(omp["width"])
    threads = np.unique(omp["threads"])

    fig, ax = plt.subplots(figsize=(8, 5))

    for i, width in enumerate(widths):
        mask = omp["width"] == width
        x, y = statistics(
            omp[mask],
            omp_values[mask],
            "threads",
        )

        ax.plot(
            x,
            y,
            marker=MARKERS[i % len(MARKERS)],
            linewidth=1.8,
            label=f"K={int(width)}",
        )

    if ideal == "linear":
        ax.plot(
            threads,
            threads,
            "k--",
            linewidth=1.3,
            label="ideal",
        )

    elif ideal == "unit":
        ax.axhline(
            1,
            color="k",
            linestyle="--",
            linewidth=1.3,
            label="ideal",
        )

    ax.set_xticks(threads)
    setup(ax, "Number of threads", ylabel, title)
    ax.legend()

    save(fig, args, filename)


# Plot by grid size, with one curve for each selected thread count
def plot_by_width(
    args,
    data,
    values,
    ylabel,
    title,
    filename,
):
    omp = data[data["accel"] == "openmp"]
    omp_values = values[data["accel"] == "openmp"]

    available_threads = np.unique(omp["threads"])
    threads = [
        p for p in SELECTED_THREADS
        if p in available_threads
    ]

    widths = np.unique(omp["width"])

    fig, ax = plt.subplots(figsize=(8, 5))

    for i, threads_count in enumerate(threads):
        mask = omp["threads"] == threads_count
        x, y = statistics(
            omp[mask],
            omp_values[mask],
            "width",
        )

        ax.plot(
            x,
            y,
            marker=MARKERS[i % len(MARKERS)],
            linewidth=1.8,
            label=f"p={threads_count}",
        )

    ax.set_xscale("log")
    ax.set_xticks(widths)
    ax.set_xticklabels(
        [str(int(width)) for width in widths]
    )

    setup(
        ax,
        "Grid size K",
        ylabel,
        title,
    )

    ax.legend()
    save(fig, args, filename)


def main():
    parser = argparse.ArgumentParser(
        description="Plot FDTD simulation performance graphs."
    )

    parser.add_argument("csv")
    parser.add_argument("-O", "--output-dir", default="images")
    parser.add_argument("--prefix")
    parser.add_argument("--dpi", type=int, default=120)

    args = parser.parse_args()

    if args.prefix is None:
        name = os.path.splitext(os.path.basename(args.csv))[0]
        args.prefix = name + "_"

    data = load(args.csv)
    throughput, speedup, efficiency = calculate(data)

    plot_by_threads(
        args,
        data,
        speedup,
        "Speedup S(p)",
        "Speedup versus number of threads",
        "speedup.png",
        ideal="linear",
    )

    plot_by_threads(
        args,
        data,
        efficiency,
        "Efficiency E(p)",
        "Efficiency versus number of threads",
        "efficiency.png",
        ideal="unit",
    )

    plot_by_threads(
        args,
        data,
        throughput,
        "Throughput (Mcells/s)",
        "Throughput versus number of threads",
        "throughput.png",
    )

    plot_by_width(
        args,
        data,
        throughput,
        "Throughput (Mcells/s)",
        "Throughput versus grid size",
        "throughput_k.png",
    )

    plot_by_width(
        args,
        data,
        speedup,
        "Speedup S(p)",
        "Speedup versus grid size",
        "speedup_k.png",
    )


if __name__ == "__main__":
    main()
