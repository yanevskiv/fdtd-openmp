// Author: Ivan Janevski (C) 2026

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <tgmath.h>
#include <omp.h>
#include <fdtd.h>

// Fixed test-simulation parameters
#define BENCH_PML_WIDTH      20
#define BENCH_PULSE_T0       40.0
#define BENCH_PULSE_SPREAD   12.0
#define BENCH_WARMUP_SECONDS 0.1

// Print usage instructions
static void Bench_ShowHelp(const char *prog)
{
    fprintf(stderr,
        "Usage: %s -K WIDTH -N NSTEPS [options] [OUTPUT_CSV]\n"
        "\n"
        "Measure serial and OpenMP FDTD simulation runtime.\n"
        "\n"
        "Options:\n"
        "  -K, --width=K          Number of grid cells\n"
        "  -N, --time-steps=N     Number of time steps\n"
        "      --max-threads=P    Maximum number of OpenMP threads\n"
        "      --repeat=R         Number of repetitions (default: 1)\n"
        "      --get-header       Print only the CSV header\n"
        "      --no-header        Do not print the CSV header\n"
        "  -h, --help             Show help\n"
        "  -v, --version          Show program version\n",
        prog
    );
}

// Print program version
static void Bench_ShowVersion(void)
{
    fprintf(stderr, "%s\n", FDTD_VERSION);
}

// Gaussian pulse (f1=t0, f2=spread, f3=center)
static void Bench_SourceFn_Pulse(Fdtd_EzSim *sim, Fdtd_EzSourceData *data)
{
    Fdtd_FloatType time = Fdtd_EzSimGetTime(sim);
    Fdtd_FloatType pulse = exp(-0.5 * pow((data->f1 - time) / data->f2, 2.0));
    Fdtd_EzSimInjectEz(sim, (size_t) data->f3, pulse);
}

// Warm up the processor and OpenMP runtime before measurement
static void Bench_WarmUp(Fdtd_EzSim *sim, size_t nsteps)
{
    double start = omp_get_wtime();
    do {
        Fdtd_EzSimReset(sim);
        Fdtd_EzSimRun(sim, nsteps);
    } while (omp_get_wtime() - start < BENCH_WARMUP_SECONDS);
}


// Measure the runtime of nsteps simulation steps
static double Bench_Measure(Fdtd_EzSim *sim, size_t nsteps)
{
    Fdtd_EzSimReset(sim);

    double start = omp_get_wtime();
    Fdtd_EzSimRun(sim, nsteps);
    double end = omp_get_wtime();

    return end - start;
}

// Print the header
static void Bench_PrintHeader(FILE *file)
{
    fprintf(file, "accel,threads,width,nsteps,repeat,seconds\n");
}

// Print one result
static void Bench_PrintRow(
    FILE *file,
    const char *accel,
    int threads,
    size_t width,
    size_t nsteps,
    int repeat,
    double seconds
) {
    fprintf(file, "%s,%d,%zu,%zu,%d,%.6f\n",
        accel, threads, width, nsteps, repeat, seconds);
}


// Run serial and OpenMP measurements
static void Bench_Run(
    FILE *file,
    Fdtd_EzSim *sim,
    size_t nsteps,
    int max_threads,
    int repeat
) {
    size_t width = Fdtd_EzSimGetWidth(sim);

    // Serial measurements
    Fdtd_EzSimSetAccel(sim, FDTD_ACCEL_NONE);
    Bench_WarmUp(sim, nsteps);
    for (int r = 0; r < repeat; r++) {
        double seconds = Bench_Measure(sim, nsteps);
        Bench_PrintRow(file, "none", 1, width, nsteps, r, seconds);
    }

    // OpenMP measurements
    Fdtd_EzSimSetAccel(sim, FDTD_ACCEL_OPENMP);
    for (int threads = 1; threads <= max_threads; threads++) {
        omp_set_num_threads(threads);
        Bench_WarmUp(sim, nsteps);
        for (int r = 0; r < repeat; r++) {
            double seconds = Bench_Measure(sim, nsteps);
            Bench_PrintRow(file, "openmp", threads, width, nsteps, r, seconds);
        }
    }
}


int main(int argc, char **argv)
{
    size_t width = 0;
    size_t nsteps = 0;
    int max_threads = omp_get_max_threads();
    int repeat = 10;
    int no_header = 0;
    const char *output = "-";

    // Long options without short forms
    enum {
        OPT_MAX_THREADS = 1000,
        OPT_REPEAT,
        OPT_GET_HEADER,
        OPT_NO_HEADER
    };

    static struct option options[] = {
        { "help",        no_argument,       NULL, 'h'             },
        { "width",       required_argument, NULL, 'K'             },
        { "time-steps",  required_argument, NULL, 'N'             },
        { "max-threads", required_argument, NULL, OPT_MAX_THREADS },
        { "repeat",      required_argument, NULL, OPT_REPEAT      },
        { "get-header",  no_argument,       NULL, OPT_GET_HEADER  },
        { "no-header",   no_argument,       NULL, OPT_NO_HEADER   },
        { "version",     no_argument,       NULL, 'v'             },
        { NULL, 0, NULL, 0 }
    };

    // Process command-line arguments
    int opt;
    while ((opt = getopt_long(argc, argv, "K:N:hv", options, NULL)) != -1) {
        switch (opt) {
            default: {
                Bench_ShowHelp(argv[0]);
                return EXIT_FAILURE;
            } break;

            case 'h': {
                Bench_ShowHelp(argv[0]);
                return EXIT_SUCCESS;
            } break;

            case 'v': {
                Bench_ShowVersion();
                return EXIT_SUCCESS;
            } break;

            case 'K': {
                width = (size_t) strtoull(optarg, NULL, 10);
            } break;

            case 'N': {
                nsteps = (size_t) strtoull(optarg, NULL, 10);
            } break;

            case OPT_MAX_THREADS: {
                max_threads = atoi(optarg);
            } break;

            case OPT_REPEAT: {
                repeat = atoi(optarg);
            } break;

            case OPT_GET_HEADER: {
                Bench_PrintHeader(stdout);
                return EXIT_SUCCESS;
            } break;

            case OPT_NO_HEADER: {
                no_header = 1;
            } break;
        }
    }

    // Validate required and numeric parameters
    if (width == 0 || nsteps == 0 || max_threads < 1 || repeat < 1) {
        Bench_ShowHelp(argv[0]);
        return EXIT_FAILURE;
    }

    if (optind < argc) {
        output = argv[optind];
    }

    // Open the output CSV file ("-" means standard output)
    FILE *file = (strcmp(output, "-") == 0)
        ? stdout
        : fopen(output, "w");

    if (file == NULL) {
        fprintf(stderr, "error: cannot open '%s'\n", output);
        return EXIT_FAILURE;
    }

    // Create the simulation
    Fdtd_EzSim *sim = Fdtd_EzSimCreate(width);

    // Set the PML boundary
    Fdtd_EzSimSetPML(sim, BENCH_PML_WIDTH);

    // Add a Gaussian source
    Fdtd_EzSourceData pulse = {
        NULL,
        BENCH_PULSE_T0,
        BENCH_PULSE_SPREAD,
        (Fdtd_FloatType) (width / 2),
        0.0,
        0.0,
        0.0
    };
    Fdtd_EzSimAddSource(sim, Bench_SourceFn_Pulse, pulse);

    // Print the CSV header
    if (!no_header) {
        Bench_PrintHeader(file);
    }

    // Benchmarking and CSV output
    Bench_Run(file, sim, nsteps, max_threads, repeat);

    // Free resources
    Fdtd_EzSimDestroy(sim);
    if (file != stdout) {
        fclose(file);
        printf("==> Output: %s\n", output);
    }

    return EXIT_SUCCESS;
}
