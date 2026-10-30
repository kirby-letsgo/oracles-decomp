// Replays a real Seasons movie with generated Lua swaps through the native engine.
// Only test binaries link both the mod host and the engine; the engine itself does not.
#include "unit.h"
#include "mods/mods.h"
#include "core/gb.h"
#include "platform/tas.h"
#include "platform/setup.h"
#include "game/game.h"
#include "game/features.h"
#include "assets/assets.h"
#include "rt/fibers.h"

static int hits;
static GB *watched;
static bool waiting_gasha, waiting_map, got_gasha, got_map;
static uint8_t gasha_before;
static uint8_t tas_cb(void *ctx, uint64_t frame) { return tas_input_at((const Tas *)ctx, frame); }
static bool swap(int group, int room, uint8_t id, uint8_t subid, uint8_t *new_id, uint8_t *new_subid) {
  bool changed = mods_treasure_swap(group, room, id, subid, new_id, new_subid);
  if (changed) {
    printf("swap %d:%02x %02x/%02x -> %02x/%02x\n", group, room, id, subid, *new_id, *new_subid);
    hits++;
    if (watched && group == 4 && room == 0x19 && *new_id == 0x34) {
      gasha_before = watched->wram[0][0x6ba];
      waiting_gasha = true;
    }
    if (watched && group == 4 && room == 0x0d && *new_id == 0x33) {
      ASSERT(!(watched->wram[0][0x67e] & 2));
      waiting_map = true;
    }
  }
  return changed;
}

static void audit_objects(GB *gb) {
  // The source audit independently pins these three D1 targets. Check every
  // exported replacement against the actual ROM table, not duplicated reward bytes.
  static const uint8_t checks[][3] = {{0x0d, 0x34, 1}, {0x0f, 0x32, 2}, {0x19, 0x33, 2}};
  uint8_t saved[sizeof gb->wram];
  memcpy(saved, gb->wram, sizeof saved);
  for (unsigned i = 0; i < sizeof checks / sizeof checks[0]; i++) {
    uint8_t id, subid, object[4];
    ASSERT(mods_treasure_swap(4, checks[i][0], checks[i][1], checks[i][2], &id, &subid));
    features_treasure_object(gb, id, subid, object);
    W8(wActiveGroup) = 4;
    W8(wActiveRoom) = checks[i][0];
    const uint16_t at = 0xd240;
    mem_wr(gb, at + OBJ_VAR30, checks[i][1]);
    mem_wr(gb, at + OBJ_VAR03, checks[i][2]);
    mem_wr(gb, at + OBJ_VAR31, 3); // chest spawn mode belongs to the location
    features_treasure_loaded(gb, at);
    ASSERT_EQ(mem_rd(gb, at + OBJ_VAR30), id);
    ASSERT_EQ(mem_rd(gb, at + OBJ_VAR03), subid);
    ASSERT_EQ(mem_rd(gb, at + OBJ_VAR31), 3);
    ASSERT_EQ(mem_rd(gb, at + OBJ_VAR32), object[0] & 7);
    ASSERT_EQ(mem_rd(gb, at + OBJ_VAR34), object[1]);
    ASSERT_EQ(mem_rd(gb, at + OBJ_VAR35), object[2]);
    ASSERT_EQ(mem_rd(gb, at + OBJ_SUBID), object[3]);
  }
  memcpy(gb->wram, saved, sizeof saved);
  hits = 0; // only real movie pickups count towards the replay assertion
}

int main(int argc, char **argv) {
  ASSERT(argc == 2);
  setvbuf(stdout, NULL, _IOLBF, 0);
  size_t size;
  uint8_t *rom = oracles_read_file(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", &size);
  if (!rom) SKIP("Seasons ROM missing");
  Tas movie;
  if (!tas_load(&movie, TAS_DIR "/seasons-play.inputs")) SKIP("Seasons recording missing");
  GB *gb = calloc(1, sizeof *gb);
  ASSERT(gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, size));
  uint8_t *cycles = cyctab_alloc(rom, size);
  uint8_t *code = assets_code_bits(rom, size);
  gb->cyctab = cycles;
  gb->code_bits = code;
  assets_zero_code(rom, size);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, TAS_DIR "/seasons-boot.state")) SKIP("Seasons boot state missing");
  ASSERT(mods_init(argv[1], true));
  ASSERT_EQ(mods_count(), 1);
  ASSERT(mods_enabled(0));
  features.treasure_swap = swap;
  audit_objects(gb);
  watched = gb;
  gb->input_at = tas_cb;
  gb->input_ctx = &movie;
  // A changed pickup has different dialog timing, so a fixed vanilla input movie
  // is not a full-run completion oracle. Verify two real pickups and inventory
  // effects, not just that the swap callback was called or the engine did not hang.
  for (int i = 0; i < 100000 && !gb->hung; i++) {
    gb_run_frame(gb);
    if (waiting_gasha && gb->wram[0][0x6ba] > gasha_before) got_gasha = true;
    if (waiting_map && (gb->wram[0][0x67e] & 2)) got_map = true;
    if (got_gasha && got_map) { printf("Both replacement rewards collected at frame %d\n", i + 1); break; }
  }
  ASSERT(!gb->hung);
  printf("randomizer replay: %d swaps, received Gasha=%d, received D1 map=%d\n", hits, got_gasha, got_map);
  ASSERT(hits >= 2);
  ASSERT(got_gasha && got_map);
  watched = NULL;
  features.treasure_swap = NULL;
  mods_shutdown();
  fibers_reset(gb);
  free(cycles);
  free(code);
  free(gb);
  free(rom);
  tas_free(&movie);
  return 0;
}
