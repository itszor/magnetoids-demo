Magnetoids demo (Frank Föhl / Oregan Developments, 1994) - static recompilation
================================================================================

A recipe: this repository holds no game data. It rebuilds the game as a
native executable from your own copy, recognised by content (recipe.json):

    ./build.sh "Magnetoids - Demo (Oregan).zip"     (or a directory holding !MGNTOIDS)
    cd recomp && ./magnetoids

build.sh runs recipe.json's steps with SuperSpider's tools/weave.py
(~/stuff/superspider, or $SUPERSPIDER): it unpacks the zip to
extracted/app (its RISC OS filetypes as ,load-exec suffixes) and !MGNTOIDS
to game/ (libroger's appdir), then analyses, translates and links with
libroger (~/stuff/libroger, or make ROGER=...). Needs python3, OCaml/dune,
gcc and SDL3.

The input: the public-domain demo of Magnetoids ("the first 3D-version of
Asteroids"), a RISC OS zip of !MGNTOIDS (sha256 1417b569...; its members'
Acorn extra fields declare 24 bytes where 20 follow, which weave.py reads
leniently). Only the demo: the full game isn't available.

    analysis/run.sh     SuperSpider on LOADCODE and PROGGY:
                        analysis/loadcode.json, analysis/proggy.json
    recomp/             make regen (analysis + superspider gen), make, ./magnetoids

Committed: recipe.json, build.sh, analysis/run.sh, analysis/*.roots (empty:
no gaps left), recomp/Makefile, recomp/game.c, this file.

Playing
-------

    ./magnetoids [--scale=N] [--fullscreen] [--no-sound] [--joystick]

The Oregan logo fades in and out, then the title, the credits and the high
scores. Fire starts a game; Space on the title opens the Variations menu
(ship, controls, sensitivity, auto-centring: cursor keys); Escape on the
title quits, in a game ends it. The default controls are the mouse: move
it to roll and pitch, Select thrusts, Adjust fires the lasers, Menu a
missile. Keyboard set A (chosen on the Variations menu): Z/X bank, / and '
pitch up and down, Space thrust, Return fire, Shift missile; set B:
< > bank, X/S pitch, Space, A, /. F1 pauses, cursor up/down move the camera.
A game controller holds set A's keys (game.c's pad_map), and with
`--joystick` (or a controller connected at start) also answers the Acorn
Joystick controls (Joystick_Read).

    ./magnetoids --limit=N     headless, deterministic: Adjust starts a game,
                               then thrust and fire (game.c's keys)
    ./magnetoids --trace-gaps  interpret untranslated code instead of stopping
                               (none found playing the title, the menus and a game)

What runs
---------

!Run selects MODE 15, RMLoads MemAlloc to shrink the RMA, font cache,
sprite area, RAM disc and system heap to nothing (freeing memory for the
screen: libroger starts that way, so it isn't run), takes a 513K WimpSlot
and runs LOADCODE, an absolute at &8000.

LOADCODE (genload/): MODE 8, the palette black, LOGODUMP loaded straight
into screen memory, faded in and out with VDU 19 and a vsync wait per step
(headless runs don't wait at OS_Byte 19, so there the logo only flashes
by), then PROGGY loaded at &825C, straight after itself, and entered
there: game.c's roger_program selects PROGGY's translation when that load
happens.

PROGGY (gen/, 512K, not packed) is position-independent: it finds its
workspace from its PC (&17704, kept in r12 throughout), relocates a table
of shape pointers by its load address, selects MODE 13 with a second bank
(it flips them with OS_Byte 113), and times its frames with the interval
timer (OS_Word 3/4). At start-up it times a delay loop to tell an ARM2 from
an ARM3 ("switching to ARM2 mode": headless runs, at 2 MIPS, are an ARM2;
interactive runs, unthrottled, an ARM3).

Its music is the Digital Symphony player (Oregan's), driving the sound
system itself: Sound_Configure with 8 channels and its own Level 1 handler
block, whose fill code (&4EBD4) libroger calls from the sound DMA. The
player patches its own mixing code for the number of channels (an
instruction from a table stored over a routine's last instruction): those
words run interpreted after the stores, about 4% of the instructions.

What SuperSpider needed (general mechanisms, see its PORTING.md):

- Sound_Configure's R3: the Level 1 handler block, its words at +0 (fill
  code) and +4 (overrun fix-up, here a lone movs pc,lr) code the OS calls;
  their values are what the program stores there
- --global-r12, with stores through r12 to the (constant) workspace
  tracked as PC-relative ones are: routines save LR at [r12,#&10], then
  re-establish r12 with add r12,pc,#k and return through the saved word
- a store over a code word of an instruction picked from a table by an
  unknown index (ldr r0,[r0,r10,lsl #2] ; str r0,[lr]): the word may fall
  through, if any of the table's instructions does

Not exercised: the Acorn and RTFM joystick controls on real hardware (the
RTFM interface isn't answered), the high-score entry.

Regression (SuperSpider's changes for this game)
------------------------------------------------

make regen in all 34 other games, the generated C diffed against what
SuperSpider before the changes generates: the same in all but five, each of
which gains one function: the overrun fix-up routine (stmdb ; ldr lr,
[r12,#-&48] ; ldmia ...,pc) of the same family of music player, found
through Sound_Configure's handler block - Gods (&39C14), Chuck Rock and
James Pond (MusicModule, &1802FDC), Lemmings (&1802F60), Lemmings 2
(MusicModule4, &182311C, and the intro's player, &4C08C). Headless runs
(--limit=100000000) of those five, both Lemmings 2 programs included:
.events, .vdu, .screen and .regs byte-identical before and after.
Fervour and Ego: Repton 4 (the other --global-r12 games) unchanged.
