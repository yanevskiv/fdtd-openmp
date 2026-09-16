// Author: Ivan Janevski (C) 2026

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <tgmath.h>
#include <fdtd.h>

// Number of sources
#define MAIN_MAX_SOURCES 32

// Number of materials
#define MAIN_MAX_MATERIALS 32

// Default Gaussian pulse parameters
#define MAIN_DEFAULT_PULSE_T0     40.0
#define MAIN_DEFAULT_PULSE_SPREAD 12.0

// Default sinusoidal source parameters
#define MAIN_DEFAULT_WAVE_AMP   1.0
#define MAIN_DEFAULT_WAVE_FREQ  0.1
#define MAIN_DEFAULT_WAVE_PHASE 0.0

// Marker indicating that kc was not specified (the grid midpoint is used by default)
#define MAIN_SOURCE_KC_DEFAULT ((size_t) -1)

// Long options without short forms
enum {
    OPT_SET_PML = 1000,
    OPT_SET_PEC,
    OPT_SET_ACCEL,
    OPT_ADD_PULSE,
    OPT_ADD_WAVE,
    OPT_GET_DT
};

// Pending Gaussian source
typedef struct {
    size_t pulse_kc;             // source-center cell
    Fdtd_FloatType pulse_t0;     // time of maximum
    Fdtd_FloatType pulse_spread; // pulse width
} Main_PendingPulse;

// Pending sinusoidal source
typedef struct {
    size_t wave_kc;            // source-center cell
    Fdtd_FloatType wave_amp;   // amplitude
    Fdtd_FloatType wave_freq;  // frequency
    Fdtd_FloatType wave_phase; // phase
} Main_PendingWave;

// Pending material change
typedef struct {
    int  mat_mu;              // 0 = relative permittivity, 1 = relative permeability
    Fdtd_FloatType mat_value; // relative value (multiplier of FDTD_EPS0 or FDTD_MU0)
    size_t mat_kstart;        // first cell in range
    size_t mat_kend;          // end cell of range (exclusive)
} Main_PendingMaterial;

// Print usage instructions
static void Main_ShowHelp(const char *prog)
{
    const char *usage =
        "Usage: %s [options] [OUTPUT_CSV]\n"
        "\n"
        "1D FDTD simulation of an electromagnetic field. Run the simulation with --run;\n"
        "the complete grid state is written as CSV to standard output or OUTPUT_CSV,\n"
        "and can be plotted or loaded again.\n"
        "\n"
        "Options:\n"
        "  -n, --time-start=N                    Initial time step (default: 0)\n"
        "  -r, --run=N                           Run the simulation for N time steps\n"
        "  -K, --set-width=K                     Number of grid cells\n"
        "  -S, --set-courant=S                   Courant number (S = c*dt/dx, default: 0.5)\n"
        "  -d, --set-dx=val                      Spatial step dx in meters (default: 0.01)\n"
        "  -e, --set-eps-r=val[,kstart,kend]     Relative permittivity eps_r\n"
        "  -m, --set-mu-r=val[,kstart,kend]      Relative permeability mu_r\n"
        "      --set-pml=K                       Absorbing PML boundary\n"
        "      --set-pec=K                       Reflecting PEC boundary\n"
        "      --set-accel=MODE                  Acceleration: none or openmp (default: none)\n"
        "      --add-pulse=[kc[,t0,spread]]      Gaussian source (default kc: K/2)\n"
        "      --add-wave=[kc[,amp,freq,phase]]  Sinusoidal source (default kc: K/2)\n"
        "      --get-dt                          Time step dt (dt = S * dx / c)\n"
        "  -i, --input-csv=CSV                   Load initial state from a CSV file\n"
        "  -h, --help                            Show this message and exit\n"
        "  -V, --version                         Show program information\n"
        "\n"
        "Examples:\n"
        "  fdtd --set-width=200 --add-pulse --run=200 simulation.csv\n"
        "  fdtd --input-csv=out.csv --add-wave --run=100 simulation.csv\n";
    fprintf(stderr, usage, prog);
}

// Print program information
static void Main_ShowVersion(void)
{
    fprintf(stderr, "FDTD-HPC simulation package\n");
}

// Parse up to n comma-separated values; return the number read
static int Main_ParseFloats(const char *arg, Fdtd_FloatType *out, int n)
{
    char buf[128];
    strncpy(buf, arg, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    int i = 0;
    char *tok = strtok(buf, ",");
    while (tok != NULL && i < n) {
        out[i++] = strtod(tok, NULL);
        tok = strtok(NULL, ",");
    }
    return i;
}

// (Callback) Gaussian pulse (f1=t0, f2=spread, f3=center)
static void Main_SourceFn_Pulse(Fdtd_EzSim *sim, Fdtd_EzSourceData *data)
{
    Fdtd_FloatType time = Fdtd_EzSimGetTime(sim);
    Fdtd_FloatType pulse = exp(-0.5 * pow((data->f1 - time) / data->f2, 2.0));
    Fdtd_EzSimInjectEz(sim, (size_t) data->f3, pulse);
}

// (Callback) Sinusoidal source (f1=amp, f2=freq, f3=phase, f4=center)
static void Main_SourceFn_Wave(Fdtd_EzSim *sim, Fdtd_EzSourceData *data)
{
    Fdtd_FloatType time = Fdtd_EzSimGetTime(sim);
    Fdtd_FloatType wave = data->f1 * sin(2.0 * (Fdtd_FloatType) M_PI * data->f2 * time + data->f3);
    Fdtd_EzSimInjectEz(sim, (size_t) data->f4, wave);
}

// (Helper) Add a Gaussian pulse to the simulation (kc=cell, t0=maximum, spread=width)
static void Main_AddPulse(Fdtd_EzSim *sim, size_t kc, Fdtd_FloatType t0, Fdtd_FloatType spread)
{
    // Field layout: f1=t0, f2=spread, f3=center
    Fdtd_EzSourceData data = { NULL, t0, spread, (Fdtd_FloatType) kc, 0.0, 0.0, 0.0 };
    Fdtd_EzSimAddSource(sim, Main_SourceFn_Pulse, data);
}

// (Helper) Add a sinusoidal source to the simulation (kc=cell, amp, freq, phase)
static void Main_AddWave(Fdtd_EzSim *sim, size_t kc, Fdtd_FloatType amp, Fdtd_FloatType freq, Fdtd_FloatType phase)
{
    // Field layout: f1=amp, f2=freq, f3=phase, f4=center
    Fdtd_EzSourceData data = { NULL, amp, freq, phase, (Fdtd_FloatType) kc, 0.0, 0.0 };
    Fdtd_EzSimAddSource(sim, Main_SourceFn_Wave, data);
}

int main(int argc, char **argv)
{
    // Parameters
    size_t width = 200;
    size_t nsteps = 0;
    size_t nstart = 0;
    size_t pml_width = 20;
    size_t pec_width = 20;
    Fdtd_FloatType courant = 0.0;
    Fdtd_FloatType dx = 0.0;

    // Option flags
    int have_pml = 0;
    int have_pec = 0;
    int have_run = 0;
    int have_courant = 0;
    int have_dx = 0;
    int have_nstart = 0;
    int have_get_dt = 0;

    // Acceleration
    Fdtd_AccelType accel = FDTD_ACCEL_NONE;

    // Paths
    const char *input = NULL;
    const char *output = "-";

    // Gaussian sources
    Main_PendingPulse pulses[MAIN_MAX_SOURCES];
    size_t npulses = 0;

    // Sinusoidal sources
    Main_PendingWave waves[MAIN_MAX_SOURCES];
    size_t nwaves = 0;

    // Materials
    Main_PendingMaterial materials[MAIN_MAX_MATERIALS];
    size_t nmaterials = 0;

    // Long CLI options
    static struct option long_opts[] = {
        { "time-start",  required_argument, 0, 'n'            },
        { "run",         required_argument, 0, 'r'            },
        { "set-width",   required_argument, 0, 'K'            },
        { "set-courant", required_argument, 0, 'S'            },
        { "set-dx",      required_argument, 0, 'd'            },
        { "set-eps-r",   required_argument, 0, 'e'            },
        { "set-mu-r",    required_argument, 0, 'm'            },
        { "set-pml",     required_argument, 0, OPT_SET_PML    },
        { "set-pec",     required_argument, 0, OPT_SET_PEC    },
        { "set-accel",   required_argument, 0, OPT_SET_ACCEL  },
        { "add-pulse",   optional_argument, 0, OPT_ADD_PULSE  },
        { "add-wave",    optional_argument, 0, OPT_ADD_WAVE   },
        { "get-dt",      no_argument,       0, OPT_GET_DT     },
        { "input-csv",   required_argument, 0, 'i'            },
        { "help",        no_argument,       0, 'h'            },
        { "version",     no_argument,       0, 'V'            },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "n:r:K:S:d:e:m:i:hV", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'n': {
                nstart = (size_t) strtoull(optarg, NULL, 10);
                have_nstart = 1;
            } break;
            case 'r': {
                nsteps = (size_t) strtoull(optarg, NULL, 10);
                have_run = 1;
            } break;
            case 'K': {
                width = (size_t) strtoull(optarg, NULL, 10);
            } break;
            case 'S': {
                courant = strtod(optarg, NULL);
                have_courant = 1;
            } break;
            case 'd': {
                dx = strtod(optarg, NULL);
                have_dx = 1;
            } break;
            case 'e':
            case 'm': {
                Fdtd_FloatType p[3] = { 0.0, 0.0, 0.0 };
                int got = Main_ParseFloats(optarg, p, 3);
                if (nmaterials < MAIN_MAX_MATERIALS) {
                    materials[nmaterials].mat_mu = (opt == 'm');
                    materials[nmaterials].mat_value = p[0];
                    materials[nmaterials].mat_kstart = (got >= 2) ? (size_t) p[1] : FDTD_EZ_MATERIAL_START;
                    materials[nmaterials].mat_kend = (got >= 3) ? (size_t) p[2] : FDTD_EZ_MATERIAL_END;
                    nmaterials++;
                }
            } break;
            case OPT_SET_PML: {
                pml_width = (size_t) strtoull(optarg, NULL, 10);
                have_pml = 1;
            } break;
            case OPT_SET_PEC: {
                pec_width = (size_t) strtoull(optarg, NULL, 10);
                have_pec = 1;
            } break;
            case OPT_SET_ACCEL: {
                if (strcmp(optarg, "none") == 0) {
                    accel = FDTD_ACCEL_NONE;
                } else if (strcmp(optarg, "openmp") == 0) {
                    accel = FDTD_ACCEL_OPENMP;
                } else {
                    fprintf(stderr, "Error: unknown acceleration mode '%s' (none|openmp)\n", optarg);
                    return 1;
                }
            } break;
            case OPT_ADD_PULSE: {
                Fdtd_FloatType p[3] = { 0.0, MAIN_DEFAULT_PULSE_T0, MAIN_DEFAULT_PULSE_SPREAD };
                int got = (optarg != NULL) ? Main_ParseFloats(optarg, p, 3) : 0;
                if (npulses < MAIN_MAX_SOURCES) {
                    pulses[npulses].pulse_kc = (got >= 1) ? (size_t) p[0] : MAIN_SOURCE_KC_DEFAULT;
                    pulses[npulses].pulse_t0 = p[1];
                    pulses[npulses].pulse_spread = p[2];
                    npulses++;
                }
            } break;
            case OPT_ADD_WAVE: {
                Fdtd_FloatType p[4] = { 0.0, MAIN_DEFAULT_WAVE_AMP, MAIN_DEFAULT_WAVE_FREQ, MAIN_DEFAULT_WAVE_PHASE };
                int got = (optarg != NULL) ? Main_ParseFloats(optarg, p, 4) : 0;
                if (nwaves < MAIN_MAX_SOURCES) {
                    waves[nwaves].wave_kc = (got >= 1) ? (size_t) p[0] : MAIN_SOURCE_KC_DEFAULT;
                    waves[nwaves].wave_amp = p[1];
                    waves[nwaves].wave_freq = p[2];
                    waves[nwaves].wave_phase = p[3];
                    nwaves++;
                }
            } break;
            case OPT_GET_DT: {
                have_get_dt = 1;
            } break;
            case 'i': {
                input = optarg;
            } break;
            case 'h': {
                Main_ShowHelp(argv[0]);
                return 0;
            }
            case 'V': {
                Main_ShowVersion();
                return 0;
            }
            default: {
                Main_ShowHelp(argv[0]);
                return 1;
            }
        }
    }

    // No action specified (-r/--run or --get-dt)
    if (!have_run && !have_get_dt) {
        Main_ShowHelp(argv[0]);
        return 1;
    }

    // Output CSV ("-" means standard output)
    if (optind < argc) {
        output = argv[optind];
    }

    // Create the simulation
    Fdtd_EzSim *sim = (input != NULL) ? Fdtd_EzSimCreateFromCSV(input) : Fdtd_EzSimCreate(width);
    Fdtd_EzSimSetAccel(sim, accel);

    // Courant number
    if (have_courant) {
        Fdtd_EzSimSetCourant(sim, courant);
    }

    // Spatial step dx
    if (have_dx) {
        Fdtd_EzSimSetDx(sim, dx);
    }

    // Materials
    for (size_t i = 0; i < nmaterials; i++) {
        Main_PendingMaterial *mat = &materials[i];
        if (mat->mat_mu) {
            Fdtd_EzSimSetMu(sim, mat->mat_value * FDTD_MU0, mat->mat_kstart, mat->mat_kend);
        } else {
            Fdtd_EzSimSetEps(sim, mat->mat_value * FDTD_EPS0, mat->mat_kstart, mat->mat_kend);
        }
    }

    // Boundary conditions
    if (have_pec) {
        Fdtd_EzSimSetPEC(sim, pec_width);
    } else if (have_pml) {
        Fdtd_EzSimSetPML(sim, pml_width);
    } else if (input == NULL) {
        Fdtd_EzSimSetPML(sim, 20);
    }

    // Sources: Gaussian pulses
    for (size_t i = 0; i < npulses; i++) {
        Main_PendingPulse *pulse = &pulses[i];
        size_t kc = (pulse->pulse_kc != MAIN_SOURCE_KC_DEFAULT) 
            ? pulse->pulse_kc 
            : Fdtd_EzSimGetWidth(sim) / 2;
        Main_AddPulse(sim, kc, pulse->pulse_t0, pulse->pulse_spread);
    }

    // Sources: sinusoidal sources
    for (size_t i = 0; i < nwaves; i++) {
        Main_PendingWave *wave = &waves[i];
        size_t kc = (wave->wave_kc != MAIN_SOURCE_KC_DEFAULT)
            ? wave->wave_kc 
            : Fdtd_EzSimGetWidth(sim) / 2;
        Main_AddWave(sim, kc, wave->wave_amp, wave->wave_freq, wave->wave_phase);
    }

    // Set the initial time (-n); otherwise retain the CSV time or use 0
    if (have_nstart) {
        Fdtd_EzSimSetTime(sim, (Fdtd_FloatType) nstart);
    }

    // Print the time step dt
    if (have_get_dt) {
        printf("==> dt = %g s\n", Fdtd_EzSimGetDt(sim));
    }

    // Simulation and output
    if (have_run) {
        Fdtd_EzSimRun(sim, nsteps);
        Fdtd_EzSimWriteCSV(sim, output);
        if (strcmp(output, "-") != 0) {
            printf("==> Output: %s\n", output);
        }
    }

    // Free memory
    Fdtd_EzSimDestroy(sim);
    return 0;
}
