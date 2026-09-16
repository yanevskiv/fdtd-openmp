#!/bin/bash
# Author: Ivan Janevski (C) 2026

# Print one '# key: value' line
emit() {
    printf '# %s: %s\n' "$1" "${2:-unknown}"
}

# Read a field from lscpu
lscpu_field() {
    LC_ALL=C lscpu | sed -n "s/^$1:[ \t]*//p"
}

get_timestamp() {
    emit timestamp "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
}

get_hostname() {
    emit hostname "$(uname -n)"
}

get_cpu_model() {
    emit cpu_model "$(lscpu_field 'Model name')"
}

get_cores_physical() {
    local sockets cores

    sockets="$(lscpu_field 'Socket(s)')"
    cores="$(lscpu_field 'Core(s) per socket')"

    emit cores_physical "$(( ${sockets:-1} * ${cores:-1} ))"
}

get_cores_logical() {
    emit cores_logical "$(nproc --all 2>/dev/null)"
}

get_threads_per_core() {
    emit threads_per_core "$(lscpu_field 'Thread(s) per core')"
}

get_numa_nodes() {
    emit numa_nodes "$(lscpu_field 'NUMA node(s)')"
}

get_l2_cache() {
    emit l2_cache "$(lscpu_field 'L2 cache')"
}

get_l3_cache() {
    emit l3_cache "$(lscpu_field 'L3 cache')"
}

get_omp_dynamic() {
    emit omp_dynamic "${OMP_DYNAMIC:-unset}"
}

get_omp_places() {
    emit omp_places "${OMP_PLACES:-unset}"
}

get_omp_proc_bind() {
    emit omp_proc_bind "${OMP_PROC_BIND:-unset}"
}


get_timestamp
get_hostname
get_cpu_model
get_cores_physical
get_cores_logical
get_threads_per_core
get_numa_nodes
get_l2_cache
get_l3_cache
get_omp_dynamic
get_omp_places
get_omp_proc_bind
