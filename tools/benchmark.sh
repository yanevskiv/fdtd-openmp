#!/bin/bash
# Author: Ivan Janevski (C) 2026

set -e

# Default parameters
NSTEPS=500
REPEAT=1
PROG=""

# Print usage instructions
usage() {
    echo "Usage: $0 -p BENCHMARK [-n NSTEPS] [-r REPEAT] WIDTH [WIDTH ...]" >&2
}

# Process options: -n steps, -r repetitions, -p path to the 'benchmark' program
while getopts "n:r:p:h" opt; do
    case "$opt" in
        n) NSTEPS="$OPTARG" ;;
        r) REPEAT="$OPTARG" ;;
        p) PROG="$OPTARG" ;;
        h) usage; exit 0 ;;
        *) usage; exit 1 ;;
    esac
done
shift $((OPTIND - 1))

# Validate the program and grid sizes
if [ -z "$PROG" ] || [ ! -x "$PROG" ]; then
    echo "error: '-p' must point to the 'benchmark' executable" >&2
    usage
    exit 1
fi
if [ "$#" -lt 1 ]; then
    echo "error: specify at least one grid size" >&2
    usage
    exit 1
fi

# Write CSV to standard output: header first, followed by row blocks for each size
"$PROG" --get-header
for WIDTH in "$@"; do
    echo "==> Running benchmark: width=$WIDTH nsteps=$NSTEPS repeat=$REPEAT" >&2
    "$PROG" -K "$WIDTH" -N "$NSTEPS" --repeat "$REPEAT" --no-header
done
