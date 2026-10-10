// Every room of both games, entered on the native build.
//
// The two TAS movies walk one path through each game, so they leave most of the map untouched,
// and the native build has no interpreter: an address with no hook stops the engine and the app
// closes. Five such addresses were found this way, in content the movies never reach.
//
// Settle into normal play by replaying the start of a movie, then for each group and room warp
// there from that same state and run a little. A room that stops the engine fails the test.
#include "unit.h"
#include "core/gb.h"
#include "platform/tas.h"
#include "platform/setup.h"
#include "hooks/hooks.h"
#include "game/game.h"
#include "assets/assets.h"
#include "rt/fibers.h"
#include <stdlib.h>

#define GROUPS 8
#define ROOMS 256
#define SETTLE_FRAMES_AGES 3200
#define SETTLE_FRAMES_SEASONS 6000
#define ROOM_FRAMES 80           // the stops all came within ~25 frames of the warp

// Rooms known to stop the engine, with why. A room listed here that no longer stops fails the
// test too, so the list cannot rot: fix the bug, drop the line.
typedef struct { const char *game; int group, room; const char *why; } Known;
static const Known known[] = {
  {"ages", 1, 0xaa, "09:593f -- interactionCode48 (tokay) burns 317 cycles where the ROM burns 312, "
                    "so interactionRunScript returns with a stack CALL_C does not recognise"},
};
static const Known *known_at(const char *game, int group, int room) {
  for (size_t i = 0; i < sizeof known / sizeof known[0]; i++)
    if (!strcmp(known[i].game, game) && known[i].group == group && known[i].room == room) return &known[i];
  return NULL;
}

static uint8_t tas_cb(void *ctx, uint64_t frame) { return tas_input_at((const Tas *)ctx, frame); }

// wWarpDestGroup..wWarpTransition2, five consecutive bytes the game warps on
static void warp_to(GB *gb, uint16_t dest, int group, int room) {
  uint8_t *w = &gb->wram[0][dest - 0xc000];
  w[0] = (uint8_t)(0x80 | group);   // bit 7: this group/room, not a warpDestTable index
  w[1] = (uint8_t)room;
  w[2] = 0x00;                      // transition
  w[3] = 0x44;                      // position, middle of the room
  w[4] = 0x01;                      // transition2: warp now, instantly
}

static void sweep(const char *game, const char *rom_path, const char *inputs, const char *boot_state,
                  uint64_t settle, uint16_t warp_dest) {
  size_t n;
  uint8_t *rom = oracles_read_file(rom_path, &n);
  if (!rom) SKIP("ROM not present");
  Tas t;
  if (!tas_load(&t, inputs)) SKIP("inputs missing");
  GB *gb = calloc(1, sizeof *gb), *base = calloc(1, sizeof *base);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->cyctab = cyctab_alloc(rom, n);
  // the app zeroes the code bytes, so zero them here too; the code map stays off because a data
  // read of a zeroed byte aborts the process, which would end the sweep at the first such room
  assets_zero_code(rom, n);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, boot_state)) SKIP("boot state missing");
  gb->input_at = tas_cb; gb->input_ctx = &t;
  for (uint64_t i = 0; i < settle; i++) gb_run_frame(gb);
  ASSERT(!gb->hung);
  gb->input_at = NULL; gb->input_ctx = NULL; gb->joy = 0;   // the sweep presses nothing
  *base = *gb;

  int failures = 0;
  for (int group = 0; group < GROUPS; group++) {
    for (int room = 0; room < ROOMS; room++) {
      oracles_copy_state(gb, base);
      fibers_reset(gb);
      gb_stop_reason[0] = '\0';
      warp_to(gb, warp_dest, group, room);
      for (int f = 0; f < ROOM_FRAMES && !gb->hung; f++) gb_run_frame(gb);
      const Known *k = known_at(game, group, room);
      if (gb->hung && !k) {
        fprintf(stderr, "  %s group %x room %02x: %s\n", game, group, room,
                gb_stop_reason[0] ? gb_stop_reason : "engine stopped");
        failures++;
      } else if (!gb->hung && k) {
        fprintf(stderr, "  %s group %x room %02x no longer stops; drop it from known[]\n", game, group, room);
        failures++;
      }
    }
  }
  if (failures) fprintf(stderr, "%d of %d rooms stopped the engine\n", failures, GROUPS * ROOMS);
  ASSERT_EQ(failures, 0);
  free(gb); free(base); free(rom); tas_free(&t);
}

static void ages_every_room_runs(void) {
  sweep("ages", GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc",
        TAS_DIR "/ages-consoleverified.inputs", TAS_DIR "/ages-boot.state",
        SETTLE_FRAMES_AGES, 0xcc47);
}

static void seasons_every_room_runs(void) {
  sweep("seasons", GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc",
        TAS_DIR "/seasons-consoleverified.inputs", TAS_DIR "/seasons-consoleverified-boot.state",
        SETTLE_FRAMES_SEASONS, 0xcc63);
}

int main(void) { RUN(ages_every_room_runs); RUN(seasons_every_room_runs); return 0; }
