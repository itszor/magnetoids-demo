/* Magnetoids demo (Frank Föhl / Oregan Developments, 1994; public domain):
 * what libroger needs to know about the game.
 *
 * !Run selects MODE 15 (ECHO |V<15>), RMLoads MemAlloc to shrink the RMA,
 * font cache, sprite area, RAM disc and system heap to nothing (freeing
 * memory for the screen; as libroger starts out, so it isn't run), sets a
 * 513K WimpSlot and runs LoadCode, an absolute at &8000.
 *
 * LoadCode selects MODE 8, blacks out the palette, loads LogoDump into
 * screen memory, fades it in and out, then loads Proggy straight after
 * itself, at &825C, and jumps there.
 *
 * The translated images: LoadCode (genload/), Proggy (gen/). */
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <roger.h>

#define APP "../game/!MGNTOIDS"

extern const struct roger_image recomp_image, recompL_image;

/* LoadCode loads Proggy to &825C and enters it there */
static const struct roger_program programs[] = {
    {"Proggy", &recomp_image, 0, 0, 0, NULL},
    {NULL, NULL, 0, 0, 0, NULL}};

static const char *const prefixes[] = {"<Obey$Dir>.", NULL};

/* a file in !MGNTOIDS, whatever ,load-exec suffix the extraction gave it
 * (its filetype and date stamp) */
static const char *in_app(const char *leaf)
{
    DIR *d = opendir(APP);
    struct dirent *e;
    char *path = NULL;
    while (d && !path && (e = readdir(d)))
        if (strcspn(e->d_name, ",") == strlen(leaf) && !strncasecmp(e->d_name, leaf, strlen(leaf))) {
            path = malloc(strlen(APP) + strlen(e->d_name) + 2);
            sprintf(path, "%s/%s", APP, e->d_name);
        }
    if (d) closedir(d);
    if (!path) { fprintf(stderr, "no %s/%s: build.sh extracts it\n", APP, leaf); exit(1); }
    return path;
}

/* The headless scenario (--limit runs; instruction counts, 2000 a ms), with
 * the default Mouse controls, which the game reads as keys (INKEY -10
 * Select, -12 Adjust; a scenario click would only raise the mouse event):
 * Adjust (fire) on the title starts a game, then Select thrusts and Adjust
 * fires the lasers for a while */
static const struct roger_key keys[] = {
    {12, 16000000, 16600000},                       /* Adjust: start */
    {10, 24000000, 44000000},                       /* Select: thrust */
    {12, 34000000, 54000000},                       /* Adjust: fire */
};

static struct roger_config config = {
    .title = "Magnetoids demo (recompiled)",
    .image = &recompL_image,
    .programs = programs,
    .appdir = APP,
    .dir_prefixes = prefixes,
    .command_line = "<Obey$Dir>.LoadCode",
    .initial_mode = 15,                 /* !Run: ECHO |V<15> */
    .wimpslot = 513 * 1024,             /* WimpSlot -min 513K */
    /* a controller: keyboard set A's keys (the Variations screen's
     * default with Mouse; INKEY numbers at workspace +&E8): Z X bank, / '
     * pitch up and down, Return fires, Space thrusts, Shift a missile */
    .pad_map = "left=Z,right=X,up=/,down=',a=Return,b=Space,x=Left Shift,start=Space,back=Escape",
    /* it reads Acorn's joystick module (Joystick_Read) every frame: its
     * Acorn Joystick controls, answered from a controller */
    .joystick = ROGER_JOY_ACORN,
    .keys = keys,
    .nkeys = sizeof keys / sizeof keys[0],
};

int main(int argc, char **argv)
{
    config.image_path = in_app("LoadCode");
    roger_main(argc, argv, &config);
}
