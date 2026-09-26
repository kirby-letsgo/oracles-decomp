// The ROM room decoder against the game: through both movies, each time a room has settled (normal
// play, fade finished) its tiles, attributes, tile graphics and palettes as the game loaded them are
// kept, then decoded standalone from the ROM and compared. (Skipped without the ROM.)
// ROOM_DIAG=1 lists the rooms that differ, ROOM_TILES=1 the differing tile graphics, ROOM_DUMP=i
// the layout, first tiles and palette 2 of the i-th settled room.
#include "unit.h"
#include "core/gb.h"
#include "platform/tas.h"
#include "platform/setup.h"
#include "game/game.h"
#include "assets/assets.h"
#include "wide/room.h"
#include <stdlib.h>

static uint8_t tas_cb(void *ctx, uint64_t frame) { return tas_input_at((const Tas *)ctx, frame); }

typedef struct {
  uint64_t frame;
  int group, room, modifier;
  uint8_t tiles[0x300], attrs[0x300], vram[2][0x1800], bg_pal[64], layout[0xc0];
  int width, height;
} Settled;

typedef struct {
  bool seasons;
  Settled *rooms;
  int n, cap;
  int last_group, last_room, last_modifier, stable;
} Recorder;

static void record_cb(GB *gb, const GBSample *s, void *ctx) {
  (void)gb;
  Recorder *r = ctx;
  const uint8_t *w = s->wram[0];
  int split = s->hram[(r->seasons ? 0xff99 : 0xff9b) - 0xff80];
  int group = w[(r->seasons ? 0xc49 : 0xc2d)], room = w[(r->seasons ? 0xc4c : 0xc30)], modifier = w[(r->seasons ? 0xc4e : 0xc32)];
  bool settled = (w[0xd00] & 0x0f) == 0x01 && (split == 2 || split == 3) && w[0x4ab] == 0 && w[0xbcb] == 0;
  // a room counts once it has been settled for 30 frames (after season changes and fades)
  if (!settled || group != r->last_group || room != r->last_room || modifier != r->last_modifier) {
    r->last_group = group; r->last_room = room; r->last_modifier = modifier;
    r->stable = 0;
    return;
  }
  if (++r->stable != 30) return;
  for (int i = 0; i < r->n; i++)
    if (r->rooms[i].group == group && r->rooms[i].room == room && r->rooms[i].modifier == modifier) return;
  if (r->n == r->cap) { r->cap = r->cap ? r->cap * 2 : 256; r->rooms = realloc(r->rooms, (size_t)r->cap * sizeof *r->rooms); }
  Settled *st = &r->rooms[r->n++];
  st->frame = s->frame;
  st->group = group; st->room = room; st->modifier = modifier;
  memcpy(st->tiles, &s->wram[3][0x800], 0x300);
  memcpy(st->attrs, &s->wram[3][0xc00], 0x300);
  memcpy(st->vram[0], s->vram[0], 0x1800);
  memcpy(st->vram[1], s->vram[1], 0x1800);
  memcpy(st->bg_pal, &s->wram[2][0xe80], 64);          // w2TilesetBgPalettes: as loaded, before fades and effects
  memcpy(st->layout, &w[0xf00], 0xc0);
  st->width = w[0xd0a];
  st->height = w[0xd0b];
}

static Recorder record_movie(const char *rom_path, const char *inputs, const char *state_path, uint64_t frames, bool seasons, uint8_t **rom_out, size_t *size_out) {
  size_t n;
  uint8_t *rom = oracles_read_file(rom_path, &n);
  if (!rom) SKIP("ROM not present");
  Tas t;
  if (!tas_load(&t, inputs)) SKIP("inputs missing");
  *rom_out = rom;                                                     // decoded from the app's copy: code bytes zeroed
  *size_out = n;
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, n));
  gb->cyctab = cyctab_alloc(rom, n);
  assets_zero_code(rom, n);
  gb_reset(gb);
  if (!oracles_load_boot_state(gb, state_path)) SKIP("boot state missing");
  Recorder r = {seasons, NULL, 0, 0, -1, -1, -1, 0};
  gb->input_at = tas_cb; gb->input_ctx = &t;
  gb->frame_cb = record_cb; gb->frame_ctx = &r;
  for (uint64_t i = 0; i < frames && !gb->hung; i++) gb_run_frame(gb);
  return r;
}

typedef struct { int rooms, tiles_exact, gfx_exact, pal_exact; } Score;

static Score compare(const Recorder *r, const uint8_t *rom, size_t size, bool seasons) {
  Score sc = {r->n, 0, 0, 0};
  int shown = 0;
  for (int i = 0; i < r->n; i++) {
    const Settled *st = &r->rooms[i];
    WideRoomKey key = {st->group, st->room, st->modifier & 3, 0};
    static WideRoom d;
    ASSERT(wide_decode_room(rom, size, seasons, &key, &d));
    int bad_tiles = 0, bad_gfx = 0, bad_pal = 0, gfx_tiles = 0;
    for (int y = 0; y < st->height; y++)
      for (int x = 0; x < st->width; x++) {
        int k = y * 32 + x;
        if (d.tiles[k] != st->tiles[k] || d.attrs[k] != st->attrs[k]) bad_tiles++;
      }
    for (int bank = 0; bank < 2; bank++)
      for (int t = 0; t < 0x180; t++)
        if (d.vram_set[bank][t] && !d.animated[bank][t]) {
          gfx_tiles++;
          if (memcmp(&d.vram[bank][t * 16], &st->vram[bank][t * 16], 16)) {
            if (getenv("ROOM_TILES") && bad_gfx < 4)
              fprintf(stderr, "    %s %d/%02x: tile %d:%04x differs\n", seasons ? "S" : "A", st->group, st->room, bank, 0x8000 + t * 16);
            bad_gfx++;
          }
        }
    for (int p = 0; p < 8; p++)
      if (d.pal_set[p] && memcmp(&d.bg_pal[p * 8], &st->bg_pal[p * 8], 8)) bad_pal++;
    if (getenv("ROOM_DUMP") && i == atoi(getenv("ROOM_DUMP"))) {
      fprintf(stderr, "layout game/decoded:\n");
      for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 16; x++) fprintf(stderr, "%02x ", st->layout[y * 16 + x]);
        fprintf(stderr, " | ");
        for (int x = 0; x < 16; x++) fprintf(stderr, "%02x ", d.layout[y * 16 + x]);
        fprintf(stderr, "\n");
      }
      fprintf(stderr, "tiles row0 game: "); for (int x = 0; x < 8; x++) fprintf(stderr, "%02x/%02x ", st->tiles[x], st->attrs[x]);
      fprintf(stderr, "\ntiles row0 dec:  "); for (int x = 0; x < 8; x++) fprintf(stderr, "%02x/%02x ", d.tiles[x], d.attrs[x]);
      fprintf(stderr, "\npal2 game: "); for (int k = 0; k < 8; k++) fprintf(stderr, "%02x ", st->bg_pal[16 + k]);
      fprintf(stderr, "\npal2 dec:  "); for (int k = 0; k < 8; k++) fprintf(stderr, "%02x ", d.bg_pal[16 + k]);
      fprintf(stderr, "\n");
    }
    sc.tiles_exact += !bad_tiles;
    sc.gfx_exact += !bad_gfx;
    sc.pal_exact += !bad_pal;
    if ((bad_tiles || bad_gfx || bad_pal) && getenv("ROOM_DIAG") && shown++ < 40)
      fprintf(stderr, "  %s group %d room %02x mod %d (frame %llu): %d/%d tiles, %d/%d gfx tiles, %d palettes differ\n",
              seasons ? "S" : "A", st->group, st->room, st->modifier, (unsigned long long)st->frame, bad_tiles,
              st->width * st->height, bad_gfx, gfx_tiles, bad_pal);
  }
  fprintf(stderr, "  %d rooms: tiles exact %d, gfx exact %d, palettes exact %d\n", sc.rooms, sc.tiles_exact, sc.gfx_exact, sc.pal_exact);
  return sc;
}

static void seasons_rooms_decode_like_the_game(void) {
  uint8_t *rom;
  size_t size;
  Recorder r = record_movie(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc", TAS_DIR "/seasons-play.inputs",
                            TAS_DIR "/seasons-boot.state", 265064, true, &rom, &size);
  Score sc = compare(&r, rom, size, true);
  ASSERT(sc.rooms > 100);
  ASSERT(sc.tiles_exact * 100 >= sc.rooms * 85);               // the rest: chests, doors, blocks (game flags)
  ASSERT(sc.gfx_exact * 100 >= sc.rooms * 90);
  ASSERT(sc.pal_exact * 100 >= sc.rooms * 95);
}

static void ages_rooms_decode_like_the_game(void) {
  uint8_t *rom;
  size_t size;
  Recorder r = record_movie(GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc", TAS_DIR "/ages-consoleverified.inputs",
                            TAS_DIR "/ages-boot.state", 289518, false, &rom, &size);
  Score sc = compare(&r, rom, size, false);
  ASSERT(sc.rooms > 70);
  ASSERT(sc.tiles_exact * 100 >= sc.rooms * 80);
  ASSERT(sc.gfx_exact * 100 >= sc.rooms * 80);
  ASSERT(sc.pal_exact * 100 >= sc.rooms * 95);
}

int main(void) {
  RUN(seasons_rooms_decode_like_the_game);
  RUN(ages_rooms_decode_like_the_game);
  return 0;
}
