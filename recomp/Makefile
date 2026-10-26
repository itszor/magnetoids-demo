# Static recompilation of the Magnetoids demo: the translated programs
# (genload/ LOADCODE, gen/ PROGGY, from superspider gen) and game.c, linked
# with libroger (the RISC OS runtime).
ROGER   ?= $(HOME)/stuff/libroger
CC      ?= gcc
CFLAGS  ?= -O2 -g -w -fno-strict-aliasing $(if $(TRACE),-DRECOMP_TRACE)
GEN     := $(wildcard gen/code*.c) gen/entries.c $(wildcard genload/code*.c) genload/entries.c
OBJ     := $(GEN:.c=.o) game.o
LIBS    := $(shell pkg-config --libs sdl3) -ljpeg -lm -lpthread
INC     := -I$(ROGER)/include

magnetoids: $(OBJ) $(ROGER)/libroger.a
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(ROGER)/libroger.a $(LIBS)

$(ROGER)/libroger.a: FORCE
	$(MAKE) -C $(ROGER) libroger.a

gen/%.o: gen/%.c $(ROGER)/include/roger_cpu.h gen/gen.h
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

genload/%.o: genload/%.c $(ROGER)/include/roger_cpu.h genload/gen.h
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

game.o: game.c $(ROGER)/include/roger.h
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Coverage build (~/stuff/superspider/phase0): "make cov" builds magnetoids-cov
# from separate *.cov.o objects; it writes $$RECOMP_COV (default cov.out).
COVDIR  ?= $(HOME)/stuff/superspider/phase0/cov
COVOBJ  := $(OBJ:.o=.cov.o) cov.cov.o

cov: magnetoids-cov

magnetoids-cov: $(COVOBJ) FORCE
	$(MAKE) -C $(ROGER) cov COV=$(COVDIR)
	$(CC) $(CFLAGS) -o $@ $(COVOBJ) $(ROGER)/libroger-cov.a $(LIBS)

%.cov.o: %.c $(ROGER)/include/roger_cpu.h $(COVDIR)/cov.h
	$(CC) $(CFLAGS) -DRECOMP_COV $(INC) -I$(COVDIR) -c -o $@ $<

cov.cov.o: $(COVDIR)/cov.c $(COVDIR)/cov.h
	$(CC) $(CFLAGS) -c -o $@ $<

SS  ?= $(HOME)/stuff/superspider
# the programs, whatever their ,load-exec suffixes
G       := ../game/!MGNTOIDS
LOADCODE := $(firstword $(wildcard $(G)/LOADCODE,*) $(G)/LOADCODE)
PROGGY  := $(firstword $(wildcard $(G)/PROGGY,*) $(G)/PROGGY)

regen:
	cd ../analysis && SUPERSPIDER=$(SS) ./run.sh
	rm -rf gen genload
	$(SS)/_build/default/bin/main.exe gen '$(LOADCODE)' 8000 ../analysis/loadcode.json --c genload \
	    --prefix=L --tabprefix=recompL --files=1 --covtag=L
	$(SS)/_build/default/bin/main.exe gen '$(PROGGY)' 825c ../analysis/proggy.json --c gen

# "make roots" merges the gap entries a --trace-gaps run logged
# (untranslated.log, or LOG=...) into ../analysis/proggy.roots, so the next
# "make regen" translates them.
LOG ?= untranslated.log

roots:
	python3 $(SS)/tools/gap_roots.py '$(PROGGY)' 825c ../analysis/proggy.roots $(LOG)

clean:
	rm -f gen/*.o genload/*.o game.o magnetoids magnetoids-cov *.cov.o gen/*.cov.o genload/*.cov.o

.PHONY: FORCE cov regen roots clean
