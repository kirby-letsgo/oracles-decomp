#include "wide/wide.h"
#include "wide/room.h"
#include "hw/render.h"
#include <string.h>

// The RAM the renderer reads (the same address in both games unless two are given)
typedef struct {
  uint16_t tileset_flags, lcd_behaviour, camera_y, camera_x;
  uint16_t group, room, modifier, pack, dungeon_index, map_position, floor, dungeon_properties, dungeon_flags_h;
  int room_packs, pack_seasons;       // Seasons ROM: roomPackData (bank 4), roomPackSeasonTable (bank 1)
} GameRam;
static const GameRam ages_ram = {0xcc34, 0xff9b, 0xffaa, 0xffac, 0xcc2d, 0xcc30, 0xcc32, 0xcc31, 0xcc39, 0xcc3a, 0xcc3b, 0xcc3c, 0xcc3d, 0, 0};
static const GameRam seasons_ram = {0xcc50, 0xff99, 0xffa8, 0xffaa, 0xcc49, 0xcc4c, 0xcc4e, 0xcc4d, 0xcc55, 0xcc56, 0xcc57, 0xcc58, 0xcc59,
                                    4 * 0x4000 + 0x073c, 1 * 0x4000 + 0x3e50};
enum {
  W_SCROLL_MODE = 0xcd00, W_OPENED_MENU = 0xcbcb, W_OFFSET_Y = 0xcd08, W_OFFSET_X = 0xcd09,
  W_ROOM_WIDTH = 0xcd0a, W_ROOM_HEIGHT = 0xcd0b,
  W2_DUNGEON_LAYOUT = 0xc00,          // $dc00 in WRAM bank 2: $40 bytes a floor, 8x8
  TILESETFLAG_SIDESCROLL = 0x20, TILESETFLAG_DUNGEON = 0x08, STATUS_BAR_LINES = 16, ROOMFLAG_VISITED = 0x10,
};

static uint8_t rd(const GBSample *s, uint16_t a) {
  if (a >= 0xff80) return s->hram[a - 0xff80];
  return s->wram[0][a - 0xc000];
}

// The room is shown around the camera only in normal play: the status bar split on, no menu
// open, the camera following Link (no transition or scripted scene) and not a sidescrolling area.
static bool room_shown(const GBSample *s, const GameRam *r) {
  int split = rd(s, r->lcd_behaviour);                     // 2 or 3: the status bar split
  // scroll mode 1 is normal play (bit 7: the camera is moving); 0 is a scripted scene, bits 1-3 a
  // screen transition
  return (split == 2 || split == 3) && rd(s, W_OPENED_MENU) == 0 && (rd(s, W_SCROLL_MODE) & 0x0f) == 0x01 &&
         !(rd(s, r->tileset_flags) & TILESETFLAG_SIDESCROLL);
}

// A register difference as a small signed offset (the camera plus the screen shake of up to 3)
static int wrap_offset(int v) { v &= 0xff; return v > 200 ? v - 256 : v; }

// The background pixel the PPU would draw at screen (x, y) if the screen were wider: the same scroll
// and map, so VRAM-only changes (text, door animations) show exactly as in the middle. RGB555.
static int bg_pixel(const GBSample *s, int x, int y) {
  uint8_t lcdc = s->line_lcdc[y];
  int d = s->line_drawn[y];
  int px = (x + s->line_scx[y]) & 0xff, py = (y + s->line_scy[y]) & 0xff;
  int idx = ((lcdc & 0x08) ? 0x1c00 : 0x1800) + (py >> 3) * 32 + (px >> 3);
  uint8_t tile = s->drawn_vram[d][0][idx], attr = s->drawn_vram[d][1][idx];
  int fx = px & 7, fy = py & 7;
  if (attr & 0x20) fx = 7 - fx;
  if (attr & 0x40) fy = 7 - fy;
  int base = (lcdc & 0x10) ? tile * 16 : 0x1000 + (int8_t)tile * 16;
  const uint8_t *td = &s->drawn_vram[d][(attr >> 3) & 1][base + fy * 2];
  int bit = 7 - fx, ci = (((td[1] >> bit) & 1) << 1) | ((td[0] >> bit) & 1);
  int i = (attr & 7) * 8 + ci * 2;
  return s->drawn_bg_pal[d][i] | (s->drawn_bg_pal[d][i + 1] << 8);
}

static void put555(uint8_t *out, int c, bool dim) {
  int r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
  uint8_t rgb[3] = {(uint8_t)((r << 3) | (r >> 2)), (uint8_t)((g << 3) | (g >> 2)), (uint8_t)((b << 3) | (b >> 2))};
  for (int k = 0; k < 3; k++) out[k] = dim ? (uint8_t)(rgb[k] * 3 / 4) : rgb[k];
}

// The window layer covers screen x on line y (it starts at WX - 7 and runs to the right edge)
bool wide_window_at(const GBSample *s, int x, int y) {
  uint8_t lcdc = s->line_lcdc[y];
  return (lcdc & 0x20) && y >= s->line_wy[y] && s->line_wx[y] <= 166 && x + 7 >= s->line_wx[y];
}

enum { NOTHING = -1, OUTSIDE = -2 };

// The current room's pixel at screen (x, y), NOTHING outside normal play, or OUTSIDE (with the
// room-local position) where the screen reaches past the room's edge.
static int room_or_outside(const GBSample *s, bool seasons, int x, int y, int *rx, int *ry) {
  const GameRam *r = seasons ? &seasons_ram : &ages_ram;
  if (y < STATUS_BAR_LINES || y >= FB_H || !room_shown(s, r) || (s->line_lcdc[y] & 0x81) != 0x81) return NOTHING;
  if (wide_window_at(s, x < 0 ? 0 : x, y)) return NOTHING;  // a line the window covers shows no room
  // room-local position: outside the room the map holds leftovers of other rooms
  *rx = wrap_offset(s->line_scx[y] - rd(s, W_OFFSET_X)) + x;
  *ry = wrap_offset(s->line_scy[y] - rd(s, W_OFFSET_Y)) + y;
  int width = rd(s, W_ROOM_WIDTH), height = rd(s, W_ROOM_HEIGHT);
  if (width > 32 || height > 32 || *ry < 0 || *ry >= height * 8) return NOTHING;
  if (*rx < 0 || *rx >= width * 8) return OUTSIDE;
  return bg_pixel(s, x, y);
}

int wide_room_pixel(const GBSample *s, bool seasons, int x, int y) {
  int rx, ry, c = room_or_outside(s, seasons, x, y, &rx, &ry);
  return c < 0 ? -1 : c;
}

// --- the neighbouring rooms ----------------------------------------------------------------------

// The room beside the current one (dir -1 left, +1 right), or false where there is none to show:
// the overworld's grid (Ages 14 wide, Seasons 16), a dungeon floor's 8x8 map through a doorway and
// only once visited; houses, caves and the other groups have no neighbours.
static bool neighbour_key(const GBSample *s, const WideOptions *o, int dir, WideRoomKey *key) {
  const GameRam *r = o->seasons ? &seasons_ram : &ages_ram;
  int group = rd(s, r->group), room = rd(s, r->room);
  memset(key, 0, sizeof *key);
  key->group = group;
  key->season = rd(s, r->modifier) & 3;
  if (group <= 1) {
    int col = room & 0x0f, last = o->seasons ? 15 : 13;
    if ((dir < 0 && col == 0) || (dir > 0 && col >= last)) return false;
    key->room = room + dir;
    key->room_flags = rd(s, (uint16_t)(0xc700 + group * 0x100 + key->room));
    if (o->seasons && group == 0 && o->rom_size > (size_t)r->room_packs + 0x100) {
      int here = rd(s, r->pack), there = o->rom[r->room_packs + key->room];
      // another area starts in its own season (determineSeasonForRoomPack); while "always spring"
      // (global flag $30: before Din is captured, after Onox) only its low nibble picks the entry
      if (s->wram[0][0x6ca + 6] & 0x01) there &= 0x0f;
      if (there != here && there != 0 && there < 0xf0 && o->rom_size > (size_t)r->pack_seasons + there)
        key->season = o->rom[r->pack_seasons + there] & 3;
    }
    return true;
  }
  if (rd(s, r->dungeon_index) == 0xff || !(rd(s, r->tileset_flags) & TILESETFLAG_DUNGEON)) return false;
  int pos = rd(s, r->map_position), floor = rd(s, r->floor), col = pos & 7;
  if ((dir < 0 && col == 0) || (dir > 0 && col == 7) || floor > 15) return false;
  int flags_page = rd(s, r->dungeon_flags_h) << 8;
  if (flags_page < 0xc000 || flags_page > 0xcf00) return false;
  int exits = (rd(s, (uint16_t)(flags_page + room)) | rd(s, r->dungeon_properties)) & 0x0f;
  if (!(exits & (dir > 0 ? 0x02 : 0x08))) return false;      // no doorway on that side
  int next = s->wram[2][W2_DUNGEON_LAYOUT + floor * 0x40 + pos + dir];
  if (!next) return false;
  key->room = next;
  key->room_flags = rd(s, (uint16_t)(flags_page + next));
  return (key->room_flags & ROOMFLAG_VISITED) != 0;
}

// Decoded rooms, reused while they stay beside the camera. Least recently used goes first, so a
// room looked up this frame is never evicted by the next lookup of the same frame.
static const WideRoom *decoded(const WideOptions *o, const WideRoomKey *key) {
  enum { SLOTS = 8 };
  static struct { WideRoomKey key; bool seasons, ok, used; uint64_t last; WideRoom room; } cache[SLOTS];
  static uint64_t clock;
  clock++;
  int victim = 0;
  for (int i = 0; i < SLOTS; i++) {
    if (cache[i].used && cache[i].seasons == o->seasons && !memcmp(&cache[i].key, key, sizeof *key)) {
      cache[i].last = clock;
      return cache[i].ok ? &cache[i].room : NULL;
    }
    if (!cache[i].used || cache[i].last < cache[victim].last) victim = i;
  }
  cache[victim].used = true;
  cache[victim].last = clock;
  cache[victim].key = *key;
  cache[victim].seasons = o->seasons;
  cache[victim].ok = wide_decode_room(o->rom, o->rom_size, o->seasons, key, &cache[victim].room);
  return cache[victim].ok ? &cache[victim].room : NULL;
}

typedef struct {
  const WideRoom *room;
  bool live;                    // same tileset gfx and palettes as the current room: draw with live VRAM
  const uint8_t (*curve)[32];   // how the game turns stored colours into the ones on screen
} Neighbour;

// On a GBA the game brightens every colour channel through this table (gbaModePaletteData,
// code/loadGraphics.s @gbaBrightenPalette); on a GBC it shows them as stored.
static const uint8_t gba_curve[32] = {
  0x00, 0x05, 0x07, 0x08, 0x0a, 0x0b, 0x0c, 0x0e, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1b, 0x1c, 0x1c, 0x1d, 0x1d, 0x1e, 0x1e, 0x1e, 0x1f, 0x1f, 0x1f, 0x1f,
};

// How the game turns stored colours into the ones on screen, for a decoded neighbour's colours:
// the GBA table (or none on a GBC) when the current room's stored and drawn palettes agree with it,
// else (a fade in progress) a curve learnt from those pairs, per channel.
static void learn_curve(const GBSample *s, int line, uint8_t curve[3][32]) {
  bool gba = s->hram[0xff96 - 0xff80] == 0xff, exact = true;
  bool known[3][32] = {{0}};
  const uint8_t *drawn = s->drawn_bg_pal[s->line_drawn[line]], *stored = &s->wram[2][0xe80];
  for (int i = 0; i < 64; i += 2) {
    int from = stored[i] | stored[i + 1] << 8, to = drawn[i] | drawn[i + 1] << 8;
    for (int c = 0; c < 3; c++) {
      int v = from >> (5 * c) & 31;
      int shown = to >> (5 * c) & 31;
      if (shown != (gba ? gba_curve[v] : v)) exact = false;
      if (!known[c][v]) { curve[c][v] = (uint8_t)shown; known[c][v] = true; }
    }
  }
  if (exact) {
    for (int c = 0; c < 3; c++)
      for (int v = 0; v < 32; v++) curve[c][v] = gba ? gba_curve[v] : (uint8_t)v;
    return;
  }
  for (int c = 0; c < 3; c++)
    for (int v = 0; v < 32; v++) {
      if (known[c][v]) continue;
      int lo = v, hi = v;
      while (lo >= 0 && !known[c][lo]) lo--;
      while (hi < 32 && !known[c][hi]) hi++;
      if (lo >= 0 && hi < 32) curve[c][v] = (uint8_t)(curve[c][lo] + (curve[c][hi] - curve[c][lo]) * (v - lo) / (hi - lo));
      else if (hi < 32) curve[c][v] = (uint8_t)(hi ? curve[c][hi] * v / hi : curve[c][hi]);
      else if (lo >= 0) curve[c][v] = (uint8_t)(curve[c][lo] + (31 - curve[c][lo]) * (v - lo) / (31 - lo > 0 ? 31 - lo : 1));
      else curve[c][v] = (uint8_t)v;
    }
}

// A neighbour's pixel at its room-local (nx, ny) on screen line y
static int neighbour_pixel(const GBSample *s, const Neighbour *n, int nx, int ny, int y) {
  const WideRoom *w = n->room;
  if (nx < 0 || ny < 0 || nx >= w->width * 8 || ny >= w->height * 8) return NOTHING;
  int k = (ny >> 3) * 32 + (nx >> 3), d = s->line_drawn[y];
  uint8_t tile = w->tiles[k], attr = w->attrs[k];
  int fx = nx & 7, fy = ny & 7, bank = (attr >> 3) & 1, addr = (tile < 0x80 ? 0x1000 : 0) + tile * 16;
  if (attr & 0x20) fx = 7 - fx;
  if (attr & 0x40) fy = 7 - fy;
  const uint8_t *td = (!n->live && w->vram_set[bank][addr / 16]) ? &w->vram[bank][addr + fy * 2] : &s->drawn_vram[d][bank][addr + fy * 2];
  int bit = 7 - fx, ci = (((td[1] >> bit) & 1) << 1) | ((td[0] >> bit) & 1), p = attr & 7;
  if (n->live || !w->pal_set[p]) return s->drawn_bg_pal[d][p * 8 + ci * 2] | (s->drawn_bg_pal[d][p * 8 + ci * 2 + 1] << 8);
  int stored = w->bg_pal[p * 8 + ci * 2] | (w->bg_pal[p * 8 + ci * 2 + 1] << 8);
  return n->curve[0][stored & 31] | n->curve[1][stored >> 5 & 31] << 5 | n->curve[2][stored >> 10 & 31] << 10;
}

static void find_neighbours(const GBSample *s, const WideOptions *o, Neighbour side[2]) {
  memset(side, 0, sizeof(Neighbour) * 2);
  if (!o->rom) return;
  const GameRam *r = o->seasons ? &seasons_ram : &ages_ram;
  WideRoomKey here;
  memset(&here, 0, sizeof here);                 // keys are compared as bytes, padding included
  here.group = rd(s, r->group);
  here.room = rd(s, r->room);
  here.season = rd(s, r->modifier) & 3;
  here.room_flags = here.group <= 1 ? rd(s, (uint16_t)(0xc700 + here.group * 0x100 + here.room)) : 0;
  const WideRoom *cur = decoded(o, &here);
  static uint8_t curve[3][32];
  learn_curve(s, 100, curve);
  for (int i = 0; i < 2; i++) {
    WideRoomKey key;
    if (!neighbour_key(s, o, i ? 1 : -1, &key)) continue;
    const WideRoom *n = decoded(o, &key);
    if (!n || (n->flags & TILESETFLAG_SIDESCROLL)) continue;
    bool same = cur && n->tileset[3] == cur->tileset[3] && n->tileset[4] == cur->tileset[4] && n->unique == cur->unique;
    side[i].room = n;
    side[i].live = same;
    side[i].curve = curve;
  }
}

void wide_render(const GBSample *s, const WideOptions *o, uint8_t *out) {
  static uint8_t middle[FB_W * FB_H * 3];
  framebuffer_to_rgb(s->framebuffer, middle);
  Neighbour side[2];
  find_neighbours(s, o, side);
  int width = rd(s, W_ROOM_WIDTH);
  for (int y = 0; y < FB_H; y++) {
    uint8_t *row = out + (size_t)y * WIDE_W * 3;
    for (int x = 0; x < WIDE_W; x++) {
      if (x == WIDE_SIDE) x += FB_W;
      if (x >= WIDE_W) break;
      int rx, ry, c = room_or_outside(s, o->seasons, x - WIDE_SIDE, y, &rx, &ry);
      if (c == OUTSIDE) {
        const Neighbour *n = rx < 0 ? &side[0] : &side[1];
        c = !n->room ? NOTHING : neighbour_pixel(s, n, rx < 0 ? n->room->width * 8 + rx : rx - width * 8, ry, y);
      }
      if (!(s->line_lcdc[y] & 0x80)) c = 0x7fff;            // LCD off: blank white, as in the middle
      if (c < 0) memcpy(row + x * 3, o->border, 3);
      else put555(row + x * 3, c, o->dim);
    }
    memcpy(row + WIDE_SIDE * 3, middle + (size_t)y * FB_W * 3, FB_W * 3);
  }
}
