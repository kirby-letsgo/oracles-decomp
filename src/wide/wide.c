#include "wide/wide.h"
#include "wide/item.h"
#include "wide/room.h"
#include "wide/hud.h"
#include "wide/put555.h"
#include "hw/render.h"
#include <limits.h>
#include <string.h>

// The RAM the renderer reads (the same address in both games unless two are given)
typedef struct {
  uint16_t tileset_flags, lcd_behaviour, camera_y, camera_x;
  uint16_t group, room, modifier, pack, dungeon_index, map_position, floor, dungeon_properties, dungeon_flags_h, game_over, next_lcd_behaviour;
  int room_packs, pack_seasons;       // roomPackData (bank 4; Ages a page per group), Seasons roomPackSeasonTable (bank 1)
  int pack_seasons_eu;                // roomPackSeasonTable in the European ROM
  uint16_t loading_room;              // wLoadingRoom: the room whose layout was loaded last
} GameRam;
static const GameRam ages_ram = {0xcc34, 0xff9b, 0xffaa, 0xffac, 0xcc2d, 0xcc30, 0xcc32, 0xcc31, 0xcc39, 0xcc3a, 0xcc3b, 0xcc3c, 0xcc3d, 0xcdd6, 0xff9d,
                                 4 * 0x4000 + 0x075c, 0, 0, 0xcc2f};
static const GameRam seasons_ram = {0xcc50, 0xff99, 0xffa8, 0xffaa, 0xcc49, 0xcc4c, 0xcc4e, 0xcc4d, 0xcc55, 0xcc56, 0xcc57, 0xcc58, 0xcc59, 0xcc35, 0xff9b,
                                    4 * 0x4000 + 0x073c, 1 * 0x4000 + 0x3e50, 1 * 0x4000 + 0x2a68, 0xcc4b};
enum {
  W_SCROLL_MODE = 0xcd00, W_OPENED_MENU = 0xcbcb, W_OFFSET_Y = 0xcd08, W_OFFSET_X = 0xcd09,
  W_PALETTE_FADE = 0xc4ab,            // wPaletteThread_mode: non-zero while a screen is fading
  W_ROOM_WIDTH = 0xcd0a, W_ROOM_HEIGHT = 0xcd0b,
  W2_DUNGEON_LAYOUT = 0xc00,          // $dc00 in WRAM bank 2: $40 bytes a floor, 8x8
  TILESETFLAG_SIDESCROLL = 0x20, TILESETFLAG_DUNGEON = 0x08, STATUS_BAR_LINES = 16, ROOMFLAG_VISITED = 0x10,
};

static uint8_t rd(const GBSample *s, uint16_t a) {
  if (a >= 0xff80) return s->hram[a - 0xff80];
  return s->wram[0][a - 0xc000];
}

// When the strips show rooms: the status bar split on and drawn, no menu open (the game-over screen is not
// opened as a menu, so it has its own flag), and either normal play (scroll mode 1; bit 7 means the camera is moving) or a scrolling screen
// transition (states 3-5), during which wActiveRoom is already the next room while the scroll and
// wScreenOffsetX/Y still count from the previous one. Scroll mode 0 is a scripted scene. Scroll
// mode 4 is a transition Link has only just asked for (state 3 and the direction already set): the
// screen and the scroll are still the room he is leaving, so it shows as normal play in that room.
// The game steps wActiveRoom (in a dungeon wDungeonMapPosition too) at the start of the main-loop
// pass that loads the next room and sets mode 8 only once the tileset is in, so a frame can end
// with either room in wActiveRoom; wLoadingRoom still holds the one on screen until the load. Taken
// from the wrong room, the strips show the rooms beside another one for a frame (the European Ages
// with Impa following lands samples on both sides of that step).
typedef struct {
  bool shown;
  bool moving;                  // a scrolling transition: every strip pixel comes from decoded rooms
  int dx, dy;                   // the grid step from the previous room to the current one while moving
  bool stepped;                 // scroll mode 4 with wActiveRoom already one step (bx, by) past the
  int bx, by;                   // room on screen
} View;

enum { W_TRANSITION_DIRECTION = 0xcd02, W_TRANSITION_STATE = 0xcd04, W_ACTIVE_LANGUAGE = 0xc62a };

// The European ROMs (game code AZ8P, AZ7P) keep the same RAM; a few ROM tables moved.
static bool rom_is_eu(const WideOptions *o) { return o->rom && o->rom_size > 0x142 && o->rom[0x142] == 'P'; }
enum { AGES_ANIMAL_COMPANION = 0xc610, SEASONS_ANIMAL_COMPANION = 0xc610, AGES_JABU_WATER_LEVEL = 0xc6e9, AGES_PAST_ROOM_FLAGS = 0xc800,
       SPECIALOBJECT_RICKY = 0x0b, SPECIALOBJECT_MOOSH = 0x0d, ROOMPACK_NUUN = 0x7f };

// Behaviour 2 is the status bar followed by a scroll per line (the underwater wave): the LCD
// interrupt drops it to 0 once the bar is drawn, and the next vblank restores it.
static int lcd_split(const GBSample *s, const GameRam *r) {
  int split = rd(s, r->lcd_behaviour);
  return split < 2 ? rd(s, r->next_lcd_behaviour) : split;
}

// Scenes drawn over the whole screen (Ages' ending and credits, Seasons' opening) keep the split
// behaviour but set wGfxRegsFinal.LYC past the last line, so LCDC never switches at the bar's end.
// A lit frame shows whether it switched; a blank one only has the LYC the next frame will get.
enum { W_GFX_REGS_FINAL_LYC = 0xc49c };
static bool whole_screen(const GBSample *s) {
  if (s->line_lcdc[0] & 0x80) return s->line_lcdc[0] == s->line_lcdc[STATUS_BAR_LINES];
  return rd(s, W_GFX_REGS_FINAL_LYC) >= FB_H;
}

// Whether the game has already moved wActiveRoom one grid step (dx, dy) on from wLoadingRoom, the
// room on screen: on the overworld's grid the next room number, in a dungeon the room the step
// left behind on the floor's layout. Rooms that load another room's layout (Seasons' Temple
// Remains, Ages' D3 spinner) keep the two apart without being a step apart, and stay as they are.
static bool stepped_on(const GBSample *s, const GameRam *r, int dx, int dy) {
  int room = rd(s, r->room), loading = rd(s, r->loading_room);
  if (room == loading) return false;
  if (rd(s, r->group) <= 1) return ((loading + dy * 16 + dx) & 0xff) == room;
  int pos = rd(s, r->map_position) - dy * 8 - dx, floor = rd(s, r->floor);
  if (pos < 0 || pos > 63 || floor > 15) return false;
  return s->wram[2][W2_DUNGEON_LAYOUT + floor * 0x40 + pos] == loading;
}

static View view_of(const GBSample *s, const GameRam *r) {
  View v = {false, false, 0, 0, false, 0, 0};
  int split = lcd_split(s, r), mode = rd(s, W_SCROLL_MODE) & 0x0f;
  if (!(split == 2 || split == 3) || whole_screen(s) || rd(s, W_OPENED_MENU) || rd(s, r->game_over)) return v;
  int state = rd(s, W_TRANSITION_STATE), dir = rd(s, W_TRANSITION_DIRECTION);
  static const int step[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};  // up, right, down, left
  if (mode == 0x01 || mode == 0x04) {
    v.shown = true;
    if (mode == 0x04 && dir <= 3 && stepped_on(s, r, step[dir][0], step[dir][1])) {
      v.stepped = true;
      v.bx = step[dir][0];
      v.by = step[dir][1];
    }
    return v;
  }
  if (!(mode & 0x0e) || state < 3 || state > 5 || dir > 3) return v;
  v.shown = v.moving = true;
  v.dx = step[dir][0];
  v.dy = step[dir][1];
  return v;
}

// The status bar is up whenever the split is on, menu or no menu. A menu drawn over the whole screen
// (the map) leaves the split flag set but never switches LCDC at the bar's end, so its top lines are
// its own picture, not a status bar.
static bool bar_up(const GBSample *s, const GameRam *r) {
  int split = lcd_split(s, r);
  return (split == 2 || split == 3) &&
         (view_of(s, r).shown || (rd(s, W_OPENED_MENU) && s->line_lcdc[0] != s->line_lcdc[STATUS_BAR_LINES]));
}

static bool room_shown(const GBSample *s, const GameRam *r) {
  View v = view_of(s, r);
  return v.shown && !v.moving;
}

// A register difference as a small signed offset (the camera plus the screen shake of up to 3)
enum { SHAKE = 3 };
static int wrap_offset(int v) { v &= 0xff; return v > 200 ? v - 256 : v; }

// What a background pixel does to the sprites over it: bit 0 a colour other than 0 (which a sprite
// with OAM priority goes behind), bit 7 the tile's own priority (every sprite goes behind).
static uint8_t bg_cover(int ci, uint8_t attr) { return ci ? (uint8_t)(1 | (attr & 0x80)) : 0; }

// The background pixel the PPU would draw at screen (x, y) if the screen were wider: the same scroll
// and map, so VRAM-only changes (text, door animations) show exactly as in the middle. RGB555.
static int bg_pixel(const GBSample *s, int x, int y, uint8_t *cover) {
  uint8_t lcdc = s->line_lcdc[y];
  int d = s->line_drawn[y] & 1;
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
  if (cover) *cover = bg_cover(ci, attr);
  return s->drawn_bg_pal[d][i] | (s->drawn_bg_pal[d][i + 1] << 8);
}

int wide_window_left(const GBSample *s, int y) {
  uint8_t lcdc = s->line_lcdc[y];
  return (lcdc & 0x20) && y >= s->line_wy[y] && s->line_wx[y] <= 166 ? s->line_wx[y] - 7 : INT_MAX;
}

bool wide_window_at(const GBSample *s, int x, int y) { return x >= wide_window_left(s, y); }

enum { NOTHING = -1, OUTSIDE = -2 };

// The room-local pixel the background shows at screen (x, y), from the scroll line y was drawn with
static void room_position(const GBSample *s, int x, int y, int *rx, int *ry) {
  *rx = wrap_offset(s->line_scx[y] - rd(s, W_OFFSET_X)) + x;
  *ry = wrap_offset(s->line_scy[y] - rd(s, W_OFFSET_Y)) + y;
}

bool wide_play_origin(const GBSample *s, bool seasons, int *rx, int *ry) {
  const GameRam *r = seasons ? &seasons_ram : &ages_ram;
  if (!room_shown(s, r) || (s->line_lcdc[STATUS_BAR_LINES] & 0x81) != 0x81) return false;
  room_position(s, 0, STATUS_BAR_LINES, rx, ry);
  // the play area must show the room, give or take the screen shake: the scroll can also point
  // at a picture that is not the room (a cutscene panning over its art) while the mode says play
  int w = rd(s, W_ROOM_WIDTH) * 8, h = rd(s, W_ROOM_HEIGHT) * 8;
  return w >= FB_W && h >= FB_H - STATUS_BAR_LINES && w <= 256 && h <= 256 &&
         *rx >= -SHAKE && *rx <= w - FB_W + SHAKE && *ry >= -SHAKE && *ry <= h - (FB_H - STATUS_BAR_LINES) + SHAKE;
}

// The current room's pixel at screen (x, y), NOTHING outside normal play, or OUTSIDE (with the
// room-local position) where the screen reaches past the room's edge.
static int room_or_outside(const GBSample *s, bool seasons, int x, int y, int *rx, int *ry) {
  const GameRam *r = seasons ? &seasons_ram : &ages_ram;
  if (y < STATUS_BAR_LINES || y >= FB_H || !room_shown(s, r) || (s->line_lcdc[y] & 0x81) != 0x81) return NOTHING;
  if (wide_window_at(s, x < 0 ? 0 : x, y)) return NOTHING;  // a line the window covers shows no room
  // room-local position: outside the room the map holds leftovers of other rooms
  room_position(s, x, y, rx, ry);
  int width = rd(s, W_ROOM_WIDTH), height = rd(s, W_ROOM_HEIGHT);
  if (width > 32 || height > 32 || *ry < 0 || *ry >= height * 8) return NOTHING;
  if (*rx < 0 || *rx >= width * 8) return OUTSIDE;
  return bg_pixel(s, x, y, NULL);
}

bool wide_room_origin(const GBSample *s, bool seasons, int y, int *rx, int *ry) {
  return room_or_outside(s, seasons, 0, y, rx, ry) != NOTHING;
}

int wide_room_pixel(const GBSample *s, bool seasons, int x, int y) {
  int rx, ry, c = room_or_outside(s, seasons, x, y, &rx, &ry);
  return c < 0 ? -1 : c;
}

// --- the rooms around ---------------------------------------------------------------------------

// Ages checkTilesetOverride: Nuun Highlands takes the companion's tileset, the Maku Tree's room
// its saved one, and Jabu-Jabu's flooded floors the underwater one.
static int ages_tileset_add(const GBSample *s, const WideOptions *o, const GameRam *r, int group, int room) {
  if (group == 0) {
    if (room == 0x38 && (rd(s, AGES_PAST_ROOM_FLAGS + 0x48) & 0x01)) return 2;
    int companion = rd(s, AGES_ANIMAL_COMPANION);
    if (o->rom_size > (size_t)r->room_packs + 0x100 && o->rom[r->room_packs + room] == ROOMPACK_NUUN &&
        companion > SPECIALOBJECT_RICKY && companion <= SPECIALOBJECT_MOOSH)
      return companion - SPECIALOBJECT_RICKY;
    return 0;
  }
  static const uint8_t jabu_floors[3] = {0x00, 0x01, 0x03};
  int level = rd(s, AGES_JABU_WATER_LEVEL) & 7, floor = rd(s, r->floor);
  if (rd(s, r->dungeon_index) == 0x07 && level < 3 && floor < 8 && (jabu_floors[level] >> floor & 1)) return 1;
  return 0;
}

// The room at grid step (tx, ty) from the current one, or false where there is none to show: on
// the overworld's grid (Ages 14 wide, Seasons 16) any room; in a dungeon the current room, while
// moving the one it came from, and otherwise the rooms beside it through a doorway once visited.
// Houses, caves and the other groups show nothing around the room.
static bool room_at(const GBSample *s, const WideOptions *o, const View *v, int tx, int ty, WideRoomKey *key) {
  const GameRam *r = o->seasons ? &seasons_ram : &ages_ram;
  int group = rd(s, r->group), room = rd(s, v->stepped ? r->loading_room : r->room);
  memset(key, 0, sizeof *key);                   // keys are compared as bytes, padding included
  key->group = group;
  key->season = rd(s, r->modifier) & 3;
  if (rom_is_eu(o)) key->language = rd(s, W_ACTIVE_LANGUAGE);
  if (group <= 1) {
    int col = (room & 0x0f) + tx, row = (room >> 4) + ty, last = o->seasons ? 15 : 13;
    if (col < 0 || col > last || row < 0 || row > 15) return false;
    key->room = row * 16 + col;
    key->room_flags = rd(s, (uint16_t)(0xc700 + group * 0x100 + key->room));
    if (!o->seasons) key->tileset_add = ages_tileset_add(s, o, r, group, key->room);
    if (o->seasons && group == 0) {
      key->remains_lava = s->wram[0][0x6ca + 2] & 0x20;          // global flags $15 and $16
      key->keep_destroyed = s->wram[0][0x6ca + 2] & 0x40;
      if (key->keep_destroyed) key->companion = rd(s, SEASONS_ANIMAL_COMPANION);
    }
    if (o->seasons && group == 0 && key->room != room && o->rom_size > (size_t)r->room_packs + 0x100) {
      int here = rd(s, r->pack), there = o->rom[r->room_packs + key->room];
      // another area starts in its own season (determineSeasonForRoomPack); while "always spring"
      // (global flag $30: before Din is captured, after Onox) only its low nibble picks the entry
      if (s->wram[0][0x6ca + 6] & 0x01) there &= 0x0f;
      int pack_seasons = rom_is_eu(o) ? r->pack_seasons_eu : r->pack_seasons;
      if (there != here && there != 0 && there < 0xf0 && o->rom_size > (size_t)pack_seasons + there)
        key->season = o->rom[pack_seasons + there] & 3;
    }
    return true;
  }
  if (rd(s, r->dungeon_index) == 0xff || !(rd(s, r->tileset_flags) & TILESETFLAG_DUNGEON)) return false;
  int pos = rd(s, r->map_position) - (v->stepped ? v->by * 8 + v->bx : 0), floor = rd(s, r->floor), flags_page = rd(s, r->dungeon_flags_h) << 8;
  if (floor > 15 || flags_page < 0xc000 || flags_page > 0xcf00) return false;
  if (!o->seasons) key->tileset_add = ages_tileset_add(s, o, r, group, room);
  if (!tx && !ty) { key->room = room; return true; }
  bool back = v->moving && tx == -v->dx && ty == -v->dy;
  bool beside = !v->moving && !ty && (tx == 1 || tx == -1);
  if (!back && !beside) return false;
  int col = (pos & 7) + tx, row = (pos >> 3) + ty;
  if (col < 0 || col > 7 || row < 0 || row > 7) return false;
  if (beside) {
    int exits = (rd(s, (uint16_t)(flags_page + room)) | rd(s, r->dungeon_properties)) & 0x0f;
    if (!(exits & (tx > 0 ? 0x02 : 0x08))) return false;     // no doorway on that side
  }
  int next = s->wram[2][W2_DUNGEON_LAYOUT + floor * 0x40 + row * 8 + col];
  if (!next) return false;
  key->room = next;
  key->room_flags = rd(s, (uint16_t)(flags_page + next));
  return back || (key->room_flags & ROOMFLAG_VISITED);
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
  const WideRoomSprites *sprites;   // its characters, from a copy of the game that went in
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
  const uint8_t *drawn = s->drawn_bg_pal[s->line_drawn[line] & 1], *stored = &s->wram[2][0xe80];
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
static int neighbour_pixel(const GBSample *s, const Neighbour *n, int nx, int ny, int y, uint8_t *cover) {
  const WideRoom *w = n->room;
  if (nx < 0 || ny < 0 || nx >= w->width * 8 || ny >= w->height * 8) return NOTHING;
  int k = (ny >> 3) * 32 + (nx >> 3), d = s->line_drawn[y] & 1;
  uint8_t tile = w->tiles[k], attr = w->attrs[k];
  int fx = nx & 7, fy = ny & 7, bank = (attr >> 3) & 1, addr = (tile < 0x80 ? 0x1000 : 0) + tile * 16;
  if (attr & 0x20) fx = 7 - fx;
  if (attr & 0x40) fy = 7 - fy;
  const uint8_t *td = (!n->live && w->vram_set[bank][addr / 16]) ? &w->vram[bank][addr + fy * 2] : &s->drawn_vram[d][bank][addr + fy * 2];
  int bit = 7 - fx, ci = (((td[1] >> bit) & 1) << 1) | ((td[0] >> bit) & 1), p = attr & 7;
  *cover = bg_cover(ci, attr);
  if (n->live || !w->pal_set[p]) return s->drawn_bg_pal[d][p * 8 + ci * 2] | (s->drawn_bg_pal[d][p * 8 + ci * 2 + 1] << 8);
  int stored = w->bg_pal[p * 8 + ci * 2] | (w->bg_pal[p * 8 + ci * 2 + 1] << 8);
  return n->curve[0][stored & 31] | n->curve[1][stored >> 5 & 31] << 5 | n->curve[2][stored >> 10 & 31] << 10;
}

static int floor_div(int a, int b) { return a >= 0 ? a / b : -((-a + b - 1) / b); }

// The rooms on the grid around the reference room this frame, looked up once each
typedef struct {
  const GBSample *s;
  const WideOptions *o;
  View v;
  const WideRoom *current;
  uint8_t curve[3][32];
  bool resolved[5][5];
  Neighbour cell[5][5];         // grid step -2..2 from the reference room
} Around;

static const Neighbour *around_cell(Around *a, int gx, int gy) {
  if (gx < -2 || gx > 2 || gy < -2 || gy > 2) return NULL;
  Neighbour *n = &a->cell[gy + 2][gx + 2];
  if (!a->resolved[gy + 2][gx + 2]) {
    a->resolved[gy + 2][gx + 2] = true;
    memset(n, 0, sizeof *n);
    WideRoomKey key;
    // the reference is the previous room while moving: grid steps count from it
    if (room_at(a->s, a->o, &a->v, gx - a->v.dx, gy - a->v.dy, &key)) {
      const WideRoom *w = decoded(a->o, &key);
      if (w && !(w->flags & TILESETFLAG_SIDESCROLL)) {
        const WideRoom *c = a->current;
        n->room = w;
        n->live = !a->v.moving && c && w->tileset[3] == c->tileset[3] && w->tileset[4] == c->tileset[4] && w->unique == c->unique;
        n->curve = a->curve;
        // the rooms Link is in and is leaving have their own objects, drawn live
        bool live_objects = (!gx && !gy) || (gx == a->v.dx && gy == a->v.dy);
        if (a->o->room_sprites && !live_objects) n->sprites = a->o->room_sprites(key.group, key.room);
      }
    }
  }
  return n->room ? n : NULL;
}

// --- sprites in the strips ------------------------------------------------------------------------

// One line of sprite pixels across the strips: the parts of the OAM's sprites that hang past the
// screen's edges, then the sprites the game left out for being off the screen (the engine reports
// them, OffscreenSprites), in that order of priority as the PPU gives lower OAM entries.
typedef struct { int16_t c[WIDE_W]; bool behind[WIDE_W]; } SpriteLine;

static void sprite_line_put(const GBSample *s, int d, int sx, int sy, uint8_t tile, uint8_t attr, int y, bool middle, SpriteLine *out) {
  int h = (s->line_lcdc[y] & 0x04) ? 16 : 8, row = y - sy;
  if (row < 0 || row >= h) return;
  if (h == 16) tile &= 0xfe;
  if (attr & 0x40) row = h - 1 - row;
  const uint8_t *td = &s->drawn_vram[d][(attr >> 3) & 1][tile * 16 + row * 2];
  for (int i = 0; i < 8; i++) {
    int wx = sx + i + WIDE_SIDE;
    if (wx < 0 || wx >= WIDE_W || (!middle && wx >= WIDE_SIDE && wx < WIDE_SIDE + FB_W) || out->c[wx] >= 0) continue;
    int bit = (attr & 0x20) ? i : 7 - i, ci = (((td[1] >> bit) & 1) << 1) | ((td[0] >> bit) & 1);
    if (!ci) continue;
    int p = (attr & 7) * 8 + ci * 2;
    out->c[wx] = (int16_t)(s->ob_pal[p] | (s->ob_pal[p + 1] << 8));
    out->behind[wx] = attr & 0x80;
  }
}

// list_only: the off-screen list across the whole width and not the OAM (a copy's room objects)
static void sprite_line(const GBSample *s, int y, bool list_only, SpriteLine *out) {
  for (int x = 0; x < WIDE_W; x++) out->c[x] = -1;
  if (!(s->line_lcdc[y] & 0x02)) return;
  int d = s->line_drawn[y];
  const uint8_t *oam = s->drawn_oam[d];
  for (int i = 0; i < 40 && !list_only; i++) {
    int sx = oam[i * 4 + 1] - 8;
    if (sx < 0 || sx > FB_W - 8) sprite_line_put(s, d, sx, oam[i * 4] - 16, oam[i * 4 + 2], oam[i * 4 + 3], y, false, out);
  }
  const OffscreenSprites *off = &s->drawn_offscreen[d];
  for (int i = 0; i < off->n; i++) sprite_line_put(s, d, off->s[i].x, off->s[i].y, off->s[i].tile, off->s[i].attr, y, list_only, out);
}

void wide_sprite_row(const GBSample *s, int y, int16_t c[WIDE_W], bool behind[WIDE_W]) {
  static SpriteLine line;
  sprite_line(s, y, true, &line);
  memcpy(c, line.c, sizeof line.c);
  memcpy(behind, line.behind, sizeof line.behind);
}

// The X and Y items, which the game has no boxes for. They go at the left end of the status bar and
// the game's own bar slides right by HUD_SHIFT to make room, so all four item slots read left to
// right as X, Y, B, A. Each slot is a letter and then a box wide enough for the widest icon.
// Laid out like the game's own boxes: the letter, then a bracket, the icon, and the closing
// bracket, with the same eight pixels between one box and the next letter that the game leaves
// between its B and A boxes. The game's brackets are a one-pixel stroke with two-pixel serifs
// running from line 1 to line 14, so these are drawn the same way rather than copied, which would
// tie us to wherever the bar happens to put its own.
enum { HUD_SHIFT = 30, SLOT_LEFT = 2, SLOT_PITCH = 38,
       SLOT_OPEN = 8, SLOT_ICON = 10, SLOT_CLOSE = 28, SLOT_BOX_W = 16,
       SLOT_TOP = 1, SLOT_BOTTOM = 14, SLOT_BOX = SLOT_CLOSE + 2 };

enum { SLOT_LETTER_TOP = 1, SLOT_COUNT_TOP = 9, COUNT_W = 7, COUNT_H = 5 };

typedef struct { bool show; bool on[2]; WideItemIcon icon[2]; int x[2], count[2]; } SlotIcons;

// Two 3x5 digits fit below the button label, clear of both the bracket and item art.
static void count_row(uint8_t *row, int x, int y, int count, int ink) {
  static const uint8_t digits[10][COUNT_H] = {
    {7, 5, 5, 5, 7}, {2, 6, 2, 2, 7}, {7, 1, 7, 4, 7}, {7, 1, 7, 1, 7}, {5, 5, 7, 1, 1},
    {7, 4, 7, 1, 7}, {7, 4, 7, 5, 7}, {7, 1, 1, 1, 1}, {7, 5, 7, 5, 7}, {7, 5, 7, 1, 7}
  };
  if (count < 0 || y < 0 || y >= COUNT_H) return;
  for (int digit = 0; digit < 2; digit++) {
    int bits = digits[digit ? count % 10 : count / 10][y];
    for (int col = 0; col < 3; col++)
      if (bits & (4 >> col)) put555(row + (x + digit * 4 + col) * 3, ink, false);
  }
}

// Some scenes (taking an essence or the harp, and the end of a fade) keep the split on but turn the
// whole bar one colour and take the game's B and A items away: X and Y go too, or they would hang
// over the scene. The scene's own sprites can reach into the bar's lines (the harp Link holds up
// over the darkened room), so pixels a sprite covers do not count.
static bool bar_blank(const GBSample *s) {
  static SpriteLine sprites;
  int paper = -1;
  for (int y = 0; y < STATUS_BAR_LINES; y++) {
    int d = s->line_drawn[y];
    for (int x = 0; x < WIDE_W; x++) sprites.c[x] = -1;
    if (s->line_lcdc[y] & 0x02)
      for (int i = 0; i < 40; i++) {
        const uint8_t *e = &s->drawn_oam[d][i * 4];
        sprite_line_put(s, d, e[1] - 8, e[0] - 16, e[2], e[3], y, true, &sprites);
      }
    for (int x = 0; x < FB_W; x++) {
      if (sprites.c[WIDE_SIDE + x] >= 0) continue;
      int c = s->framebuffer[y * FB_W + x];
      if (paper < 0) paper = c;
      else if (c != paper) return false;
    }
  }
  return true;
}

static void slot_icons(const GBSample *s, const WideOptions *o, SlotIcons *si) {
  si->show = o->slots && !bar_blank(s);
  for (int i = 0; i < 2; i++) {
    si->on[i] = o->slots && o->rom && o->slot_item[i] &&
                wide_item_icon(o->rom, o->rom_size, o->seasons, o->slot_item[i], &s->wram[0][0x600], &si->icon[i]);
    if (!si->on[i]) continue;
    si->count[i] = wide_item_count(o->seasons, o->slot_item[i], &s->wram[0][0x600]);
    // Centre a narrow icon in its box, so the two boxes line up whatever is in them.
    si->x[i] = SLOT_LEFT + i * SLOT_PITCH + SLOT_ICON + (SLOT_BOX_W - si->icon[i].halves * 8) / 2;
  }
}

static void draw_slot_icons(const GBSample *s, const WideOptions *o, const SlotIcons *si, int y, uint8_t *row) {
  if (!si->show || y >= 16) return;                     // an icon is as tall as the status bar
  // The letters sit in the middle of the bar's height, in the colour the game writes its own text in.
  const uint8_t *bg = s->drawn_bg_pal[s->line_drawn[y] & 1];
  int ink = bg[3 * 2] | (bg[3 * 2 + 1] << 8);           // BG palette 0, colour 3
  for (int i = 0; i < 2; i++) {
    int base = SLOT_LEFT + i * SLOT_PITCH;
    if (y >= SLOT_LETTER_TOP && y < SLOT_LETTER_TOP + HUD_GLYPH_H) {
      const uint8_t *g = hud_glyph(o->glyph[i]);
      if (!g) g = hud_glyph(i ? 'Y' : 'X');
      uint8_t bits = g[y - SLOT_LETTER_TOP];
      for (int x = 0; x < 6; x++)
        if (bits & (0x80 >> x)) put555(row + (base + x) * 3, ink, false);
    }
    if (y >= SLOT_TOP && y <= SLOT_BOTTOM) {
      bool serif = y == SLOT_TOP || y == SLOT_BOTTOM;      // the short arms top and bottom
      put555(row + (base + SLOT_OPEN) * 3, ink, false);
      put555(row + (base + SLOT_CLOSE + 1) * 3, ink, false);
      if (serif) {
        put555(row + (base + SLOT_OPEN + 1) * 3, ink, false);
        put555(row + (base + SLOT_CLOSE) * 3, ink, false);
      }
    }
    if (!si->on[i]) continue;
    count_row(row, base, y - SLOT_COUNT_TOP, si->count[i], ink);
    // The status bar draws item icons as sprites (spr_item_icons, wBItemSpriteAttribute1), so they
    // take an OBJ palette and colour 0 is clear, leaving the bar behind them.
    const WideItemIcon *ic = &si->icon[i];
    for (int x = 0; x < ic->halves * 8; x++) {
      int ci = ic->ci[y][x];
      if (!ci) continue;
      int p = ic->pal[x >> 3] * 8 + ci * 2;
      put555(row + (si->x[i] + x) * 3, s->ob_pal[p] | (s->ob_pal[p + 1] << 8), false);
    }
  }
}

// One slot for the narrow picture's side bars, the rest of the strip black: with room, the box
// widescreen draws (the same pixels, cut out of a bar row drawn the same way); without, its letter
// above its icon on a tab of the bar's colour hanging from the top.
enum { STRIP_LETTER_X = 5, STRIP_ICON_TOP = 9, STRIP_H = STRIP_ICON_TOP + 17,
       STRIP_COUNT_TOP = STRIP_H, STRIP_COUNT_H = STRIP_COUNT_TOP + COUNT_H + 1 };

bool wide_slot_strip(const GBSample *s, const WideOptions *o, int slot, bool box, uint8_t *out) {
  const GameRam *r = o->seasons ? &seasons_ram : &ages_ram;
  if (!o->slots || !bar_up(s, r) || bar_blank(s)) return false;
  int w = box ? WIDE_BOX_W : WIDE_STRIP_W;
  memset(out, 0, (size_t)w * FB_H * 3);
  if (box) {
    static SlotIcons icons;
    static uint8_t row[WIDE_W * 3];
    slot_icons(s, o, &icons);
    // the game's gap of eight between boxes, towards the picture: after X's box, before Y's
    int at = slot ? SLOT_PITCH - SLOT_BOX : 1;
    for (int y = 0; y < STATUS_BAR_LINES; y++) {
      for (int x = 0; x < WIDE_W; x++) put555(row + x * 3, s->framebuffer[y * FB_W], false);
      draw_slot_icons(s, o, &icons, y, row);
      uint8_t *line = out + (size_t)y * w * 3;
      for (int x = 0; x < w; x++) memcpy(line + x * 3, row, 3);
      memcpy(line + at * 3, row + (SLOT_LEFT + slot * SLOT_PITCH) * 3, SLOT_BOX * 3);
    }
    return true;
  }
  const uint8_t *bg = s->drawn_bg_pal[s->line_drawn[0] & 1];
  int paper = s->framebuffer[0], ink = bg[3 * 2] | (bg[3 * 2 + 1] << 8);
  const uint8_t *g = hud_glyph(o->glyph[slot]);
  if (!g) g = hud_glyph(slot ? 'Y' : 'X');
  WideItemIcon ic;
  bool icon = o->rom && o->slot_item[slot] &&
              wide_item_icon(o->rom, o->rom_size, o->seasons, o->slot_item[slot], &s->wram[0][0x600], &ic);
  int count = icon ? wide_item_count(o->seasons, o->slot_item[slot], &s->wram[0][0x600]) : -1;
  int icon_x = icon ? (w - ic.halves * 8) / 2 : 0;
  for (int y = 0; y < (count >= 0 ? STRIP_COUNT_H : STRIP_H); y++) {
    uint8_t *row = out + (size_t)y * w * 3;
    for (int x = 0; x < w; x++) put555(row + x * 3, paper, false);
    count_row(row, (w - COUNT_W) / 2, y - STRIP_COUNT_TOP, count, ink);
    if (y >= SLOT_LETTER_TOP && y < SLOT_LETTER_TOP + HUD_GLYPH_H)
      for (int x = 0; x < 6; x++)
        if (g[y - SLOT_LETTER_TOP] & (0x80 >> x)) put555(row + (STRIP_LETTER_X + x) * 3, ink, false);
    int iy = y - STRIP_ICON_TOP;
    if (!icon || iy < 0 || iy >= 16) continue;
    for (int x = 0; x < ic.halves * 8; x++) {
      int ci = ic.ci[iy][x];
      if (!ci) continue;
      int p = ic.pal[x >> 3] * 8 + ci * 2;
      put555(row + (icon_x + x) * 3, s->ob_pal[p] | (s->ob_pal[p + 1] << 8), false);
    }
  }
  return true;
}

// The colour that goes beside a menu. The strips take the one colour its edges are mostly made of:
// those edges are patterned (the inventory's frame alternates green and blue down the right), so
// carrying each row's own pixel outwards would smear the pattern across the strips.
static uint32_t dominant(const uint8_t *middle, bool edges) {
  struct { uint32_t c; int n; } seen[8];
  int used = 0;
  for (int y = STATUS_BAR_LINES; y < FB_H; y += edges ? 1 : 4)
    for (int i = 0; i < (edges ? 2 : FB_W / 4); i++) {
      const uint8_t *p = middle + ((size_t)y * FB_W + (edges ? (i ? FB_W - 1 : 0) : i * 4)) * 3;
      uint32_t c = (uint32_t)p[0] << 16 | (uint32_t)p[1] << 8 | p[2];
      int k = 0;
      while (k < used && seen[k].c != c) k++;
      if (k < used) seen[k].n++;
      else if (used < 8) { seen[used].c = c; seen[used].n = 1; used++; }
    }
  int best = 0;
  for (int k = 1; k < used; k++)
    if (seen[k].n > seen[best].n) best = k;
  return used ? seen[best].c : 0;
}

// Only an edge colour that differs from what fills the body is the menu's own. While the game
// redraws a page it whites the screen out -- edges included -- with the LCD still on and no fade
// flagged, and taking that would flash the strips white on every page change.
static bool menu_colour(const uint8_t *middle, uint8_t out[3]) {
  uint32_t edge = dominant(middle, true);
  if (edge == dominant(middle, false)) return false;
  out[0] = (uint8_t)(edge >> 16);
  out[1] = (uint8_t)(edge >> 8);
  out[2] = (uint8_t)edge;
  return true;
}

void wide_render(const GBSample *s, const WideOptions *o, uint8_t *out) {
  static uint8_t middle[FB_W * FB_H * 3];
  static Around a;
  framebuffer_to_rgb(s->framebuffer, middle);
  hud_relabel(s, o->glyph[2], o->glyph[3], middle);
  const GameRam *r = o->seasons ? &seasons_ram : &ages_ram;
  memset(a.resolved, 0, sizeof a.resolved);
  a.s = s;
  a.o = o;
  a.v = view_of(s, r);
  a.current = NULL;
  if (a.v.shown && o->rom) {
    WideRoomKey here;
    if (room_at(s, o, &a.v, 0, 0, &here)) a.current = decoded(o, &here);
    learn_curve(s, 100, a.curve);
  }
  static SlotIcons icons;
  slot_icons(s, o, &icons);
  // The status bar is up whenever the split is on, menu or no menu, so it carries across the whole
  // width in both. The strips below it follow the menu when there is one.
  bool menu = rd(s, W_OPENED_MENU) != 0;
  bool bar_shown = bar_up(s, r);
  // A menu's strips hold one steady colour. Between pages the game fades the screen to white and
  // then blanks it, and the frame the fade ends on is still white, so the colour is taken only from
  // a picture that has been settled for a couple of frames and kept the rest of the time -- strips
  // that followed would flash white on every page change. The colour outlives the menu, so opening
  // one again does not flash either.
  static uint8_t menu_rgb[3];
  static bool menu_rgb_set;
  static int settled;
  bool quiet = !rd(s, W_PALETTE_FADE) && (s->line_lcdc[STATUS_BAR_LINES] & 0x80);
  settled = quiet ? (settled < 4 ? settled + 1 : settled) : 0;
  // Once a colour is held only a settled picture replaces it; with nothing held yet, the first
  // picture that offers one is taken, so the first menu of a session does not start on the border
  // colour and jump.
  if (menu && (settled >= 2 || !menu_rgb_set) && menu_colour(middle, menu_rgb)) menu_rgb_set = true;
  const uint8_t *fill = menu && menu_rgb_set ? menu_rgb : o->border;
  int width = rd(s, W_ROOM_WIDTH) * 8, height = rd(s, W_ROOM_HEIGHT) * 8;
  bool rooms = a.v.shown && o->rom && width > 0 && height > 0 && width <= 256 && height <= 256;
  for (int y = 0; y < FB_H; y++) {
    uint8_t *row = out + (size_t)y * WIDE_W * 3;
    // The status bar is the game's own strip across the top 160 pixels, and there is no room to
    // show beside it. Its end colours carry on to the edges of the wider picture, so it reads as one
    // bar the whole way across instead of a box floating in the middle of a black band. Never
    // dimmed, unlike the room strips: half a dimmed bar would only put a seam down it.
    if (bar_shown && y < STATUS_BAR_LINES) {
      const uint8_t *mid = middle + (size_t)y * FB_W * 3;
      int bar = WIDE_SIDE + (o->slots ? HUD_SHIFT : 0);
      for (int x = 0; x < bar; x++) memcpy(row + x * 3, mid, 3);
      for (int x = bar + FB_W; x < WIDE_W; x++) memcpy(row + x * 3, mid + (FB_W - 1) * 3, 3);
      memcpy(row + (size_t)bar * 3, mid, FB_W * 3);
      draw_slot_icons(s, o, &icons, y, row);
      continue;
    }
    bool line = rooms && y >= STATUS_BAR_LINES && (s->line_lcdc[y] & 0x81) == 0x81 && !wide_window_at(s, 0, y);
    int ox = (s->line_scx[y] - rd(s, W_OFFSET_X)) & 0xff, oy = (s->line_scy[y] - rd(s, W_OFFSET_Y)) & 0xff;
    // the offset from the reference room's origin: a left or upward scroll only goes negative
    ox = a.v.moving && a.v.dx < 0 ? (ox ? ox - 256 : 0) : wrap_offset(ox);
    oy = a.v.moving && a.v.dy < 0 ? oy - 256 : wrap_offset(oy);
    static SpriteLine sprites;
    if (line) sprite_line(s, y, false, &sprites);
    for (int x = 0; x < WIDE_W; x++) {
      if (x == WIDE_SIDE) x += FB_W;
      if (x >= WIDE_W) break;
      int c = NOTHING;
      if (line) {
        int rx = ox + x - WIDE_SIDE, ry = oy + y, gx = floor_div(rx, width), gy = floor_div(ry, height);
        uint8_t cover = 0;
        if (!a.v.moving && !gx && !gy) c = bg_pixel(s, x - WIDE_SIDE, y, &cover);
        else {
          const Neighbour *n = around_cell(&a, gx, gy);
          int nx = rx - gx * width, ny = ry - gy * height;
          if (n) c = neighbour_pixel(s, n, nx, ny, y, &cover);
          if (c >= 0 && n->sprites && nx >= 0 && nx < 256 && ny >= 0 && ny < 176) {
            int sc = n->sprites->c[ny][nx];
            if (sc >= 0 && !((cover & 0x80) || (n->sprites->behind[ny][nx] && (cover & 1)))) c = sc;
          }
        }
        if (c >= 0 && sprites.c[x] >= 0 && !((cover & 0x80) || (sprites.behind[x] && (cover & 1)))) c = sprites.c[x];
      }
      // LCD off: blank white, as in the middle -- and never dimmed, or the strips would turn grey
      // against a white middle every time the game blanks the screen between menus.
      bool blank = !(s->line_lcdc[y] & 0x80);
      if (blank && !menu) c = 0x7fff;                     // a menu's strips keep their colour
      if (c < 0) memcpy(row + x * 3, fill, 3);
      else put555(row + x * 3, c, o->dim && !blank);
    }
    memcpy(row + WIDE_SIDE * 3, middle + (size_t)y * FB_W * 3, FB_W * 3);
  }
}
