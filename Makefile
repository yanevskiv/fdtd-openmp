CC = gcc
AR = ar
CFLAGS = -O3 -march=x86-64-v2 -fopenmp -Wall -Iinclude

DIST_DIR    = dist
OBJ_DIR     = obj
TOOLS_DIR   = tools
INCLUDE_DIR = include
SRC_DIR     = src
RESULTS_DIR = results
DOCS_DIR    = docs
DOCS_META_DIR = $(DOCS_DIR)/meta

LIB       = $(DIST_DIR)/lib/libfdtd.a
HEADER    = $(DIST_DIR)/include/fdtd.h
HEADERS   = $(INCLUDE_DIR)/fdtd.h $(wildcard $(INCLUDE_DIR)/fdtd/*.h) $(wildcard $(INCLUDE_DIR)/fdtd/Ez/*.h)
SOURCES   = $(wildcard $(SRC_DIR)/fdtd/*.c) $(wildcard $(SRC_DIR)/fdtd/Ez/*.c)
OBJ       = $(SOURCES:$(SRC_DIR)/fdtd/%.c=$(OBJ_DIR)/fdtd/%.o)
MAIN      = $(DIST_DIR)/bin/fdtd
BENCH     = $(DIST_DIR)/bin/benchmark
TOOLS     = $(DIST_DIR)/bin/tools
BUILDINFO = $(DIST_DIR)/buildinfo
DOCS_DIST_DIR = $(DIST_DIR)/docs
DOCS_PDF      = $(DOCS_DIST_DIR)/Fdtd_Thesis.pdf
DOCS_METADATA   = $(DOCS_META_DIR)/metadata.yaml
DOCS_REFERENCES = $(DOCS_META_DIR)/references.bib
DOCS_CSL        = $(DOCS_META_DIR)/ieee.csl
DOCS_SOURCES := \
	$(wildcard $(DOCS_DIR)/Chapter*.md) \
	$(wildcard $(DOCS_DIR)/References.md) \
	$(wildcard $(DOCS_DIR)/List_of_Abbreviations.md) \
	$(wildcard $(DOCS_DIR)/List_of_Figures.md) \
	$(wildcard $(DOCS_DIR)/List_of_Tables.md) \
	$(wildcard $(DOCS_DIR)/Appendix*.md)

PY           = python3
PANDOC       = pandoc
IMAGES_DIR   = $(RESULTS_DIR)/images
GAUSS_CSV    = $(RESULTS_DIR)/gauss.csv
BENCH_CSV    = $(RESULTS_DIR)/benchmark.csv
GAUSS_IMAGES = $(IMAGES_DIR)/.gauss_created
BENCH_IMAGES = $(IMAGES_DIR)/.bench_created
FIGURE_IMAGES = $(IMAGES_DIR)/.figures_created

BENCH_NSTEPS = 1024
BENCH_REPEAT = 10
BENCH_SIZES  = 10000 20000 40000 80000 160000 320000 640000 1280000

.PHONY: build all docs gauss gauss_images bench bench_images figures images buildinfo clean

.DELETE_ON_ERROR:

build: $(LIB) $(HEADER) $(MAIN) $(BENCH) $(TOOLS) $(BUILDINFO)

all: build images docs

$(LIB): $(OBJ) | $(DIST_DIR)/lib
	$(RM) $@
	$(AR) rcs $@ $^

$(HEADER): $(HEADERS) | $(DIST_DIR)/include/fdtd
	$(RM) -r $(DIST_DIR)/include/fdtd
	cp $(INCLUDE_DIR)/fdtd.h $@
	cp -r $(INCLUDE_DIR)/fdtd $(DIST_DIR)/include/fdtd

$(OBJ_DIR)/fdtd/%.o: $(SRC_DIR)/fdtd/%.c $(HEADERS) | $(OBJ_DIR)/fdtd $(OBJ_DIR)/fdtd/Ez
	$(CC) $(CFLAGS) -c $< -o $@

$(MAIN): $(SRC_DIR)/main.c $(LIB) $(HEADER) | $(DIST_DIR)/bin
	$(CC) $(CFLAGS) -I$(DIST_DIR)/include $(SRC_DIR)/main.c -L$(DIST_DIR)/lib -lfdtd -lm -o $@

$(BENCH): $(SRC_DIR)/benchmark.c $(LIB) $(HEADER) | $(DIST_DIR)/bin
	$(CC) $(CFLAGS) -I$(DIST_DIR)/include $(SRC_DIR)/benchmark.c -L$(DIST_DIR)/lib -lfdtd -lm -o $@

$(TOOLS): $(wildcard $(TOOLS_DIR)/*) | $(DIST_DIR)/bin
	rm -rf $@
	cp -r $(TOOLS_DIR) $@

$(BUILDINFO): Makefile | $(DIST_DIR)
	@printf '# cflags: %s\n' '$(CFLAGS)' > $@
	@$(CC) --version 2>/dev/null | head -n1 | sed 's/^/# compiler: /' >> $@

$(GAUSS_CSV): $(MAIN) | $(RESULTS_DIR)
	./$(MAIN) --add-pulse --run=100 $(GAUSS_CSV)

$(BENCH_CSV): $(BENCH) $(BUILDINFO) $(TOOLS) | $(RESULTS_DIR)
	cat $(BUILDINFO) > $@
	$(TOOLS)/get_sys_info.sh >> $@
	$(TOOLS)/benchmark.sh -n $(BENCH_NSTEPS) -r $(BENCH_REPEAT) -p $(BENCH) $(BENCH_SIZES) >> $@

$(DIST_DIR):
	@mkdir -p $@

$(DIST_DIR)/bin:
	@mkdir -p $@

$(DIST_DIR)/lib:
	@mkdir -p $@

$(DOCS_DIST_DIR):
	@mkdir -p $@

$(DIST_DIR)/include/fdtd:
	@mkdir -p $@

$(OBJ_DIR)/fdtd:
	@mkdir -p $@

$(OBJ_DIR)/fdtd/Ez:
	@mkdir -p $@

$(RESULTS_DIR):
	@mkdir -p $@

$(IMAGES_DIR):
	@mkdir -p $@

gauss: $(GAUSS_CSV) $(GAUSS_IMAGES)

bench: $(BENCH_CSV) $(BENCH_IMAGES)

buildinfo: $(BUILDINFO)

$(GAUSS_IMAGES): $(GAUSS_CSV) $(TOOLS) | $(IMAGES_DIR)
	$(PY) $(TOOLS)/plot_sim.py -O $(IMAGES_DIR) $(GAUSS_CSV)
	@touch $@

$(BENCH_IMAGES): $(BENCH_CSV) $(TOOLS) | $(IMAGES_DIR)
	$(PY) $(TOOLS)/plot_benchmark.py -O $(IMAGES_DIR) $(BENCH_CSV)
	@touch $@

$(FIGURE_IMAGES): $(TOOLS_DIR)/plot_figures.py | $(IMAGES_DIR)
	$(PY) $(TOOLS_DIR)/plot_figures.py -O $(IMAGES_DIR)
	@touch $@

$(DOCS_PDF): $(DOCS_METADATA) $(DOCS_REFERENCES) $(DOCS_CSL) $(DOCS_SOURCES) | $(DOCS_DIST_DIR)
	$(PANDOC) --from=markdown            \
		--citeproc                       \
		--pdf-engine=lualatex            \
		--metadata-file=$(DOCS_METADATA) \
		--resource-path=$(DOCS_DIR)      \
		$(DOCS_SOURCES) -o $@

gauss_images: $(GAUSS_IMAGES)

bench_images: $(BENCH_IMAGES)

figures: $(FIGURE_IMAGES)

docs: $(DOCS_PDF)

images: gauss_images bench_images figures

clean:
	rm -rf $(OBJ_DIR) $(DIST_DIR) $(RESULTS_DIR)
