// The room loader's data path, from the disassembly (code/bank0.s loadTilesetData, loadRoomLayout,
// loadTileset(Layout), generateW3VramTilesAndAttributes, loadGfxHeader, decompressGraphics,
// loadPaletteHeader), reading only ROM data.
#include "wide/room.h"
#include <string.h>

typedef struct {
  int tilesets_by_group, tileset_data, temple_remains, moblin_keep;   // bank 4
  int layout_groups;                                                  // bank 4
  int layout_bank, layout_table, layout_dictionaries;
  int mapping_bank;                                                   // tileMappingTable at :4004
  int gfx_bank, gfx_headers;
  int uncmp_gfx_headers, palette_headers;                             // bank 1
  int palette_bank;                                                   // paletteDataStart
  int unique_gfx_headers;                                             // bank 4
  int animation_groups, animation_gfx_headers;                        // bank 4
  int alt_world;                                                      // bank 2
  bool localized;               // the European releases: gfx headers with a source per language
} Tables;

static const Tables ages_tables = {0x52d4, 0x4f9c, 0, 0, 0x4f6c, 0x01, 0x787e, 0x7870, 0x18, 0x01, 0x69da, 0x6744, 0x632c, 0x17, 0x5b28, 0x5b52, 0x5be9, 0x4057, false};
static const Tables seasons_tables = {0x533c, 0x4c84, 0x52fc, 0x531c, 0x4c4c, 0x01, 0x7964, 0x794e, 0x17, 0x01, 0x6926, 0x66d0, 0x6290, 0x16, 0x595e, 0x59b0, 0x5a48, 0x4057, false};
// The European ROMs moved the tileset layouts, the gfx header table and the tile and palette data
// banks up (loadTileset, loadGfxHeader, loadTilesetLayout, loadPaletteHeader); the bank 4 tables stay.
static const Tables ages_eu_tables = {0x52d4, 0x4f9c, 0, 0, 0x4f6c, 0x17, 0x5655, 0x5647, 0x19, 0x17, 0x451e, 0x6744, 0x632c, 0x18, 0x5b28, 0x5b52, 0x5be9, 0x4057, true};
static const Tables seasons_eu_tables = {0x533c, 0x4c84, 0x52fc, 0x531c, 0x4c4c, 0x16, 0x529d, 0x5287, 0x18, 0x16, 0x4000, 0x66d6, 0x6296, 0x17, 0x595e, 0x59b0, 0x5a48, 0x4057, true};

enum { TILESETFLAG_PAST = 0x80, TILESETFLAG_SIDESCROLL = 0x20, TILESETFLAG_LARGE_INDOORS = 0x10,
       TILESETFLAG_DUNGEON = 0x08, TILESETFLAG_INDOORS = 0x04, TILESETFLAG_OUTDOORS = 0x01 };

typedef struct {
  const uint8_t *rom;
  size_t size;
  const Tables *t;
  int language;                 // wActiveLanguage (European ROMs)
} Rom;

// A bank:address as a ROM file offset (reads that run past $7fff continue in the next bank, which
// is simply the next byte of the file).
static size_t at(int bank, int addr) { return addr < 0x4000 ? (size_t)addr : (size_t)bank * 0x4000 + (size_t)(addr - 0x4000); }
static uint8_t b(const Rom *r, size_t o) { return o < r->size ? r->rom[o] : 0; }
static int le(const Rom *r, size_t o) { return b(r, o) | b(r, o + 1) << 8; }
static int be(const Rom *r, size_t o) { return b(r, o) << 8 | b(r, o + 1); }

// --- room layout ---------------------------------------------------------------------------------

// Small rooms: raw, or bitmask runs over groups of 8 (mode 1) or 16 (mode 2) metatiles
static void small_layout(const Rom *r, size_t src, int mode, uint8_t *layout) {
  uint8_t out[80];
  int n = 0;
  if (mode == 0) {
    for (; n < 80; n++) out[n] = b(r, src++);
  } else {
    int group_len = mode == 1 ? 8 : 16;
    while (n < 80) {
      unsigned mask = b(r, src++);
      if (mode == 2) mask |= (unsigned)b(r, src++) << 8;
      if (!mask) {
        for (int i = 0; i < group_len && n < 80; i++) out[n++] = b(r, src++);
        continue;
      }
      uint8_t fill = b(r, src++);
      for (int i = 0; i < group_len && n < 80; i++) out[n++] = (mask >> i & 1) ? fill : b(r, src++);
    }
  }
  for (int y = 0; y < 8; y++) memcpy(layout + y * 16, out + y * 10, 10);
}

// Large rooms: literals and dictionary copies, 176 metatiles written straight through (16 a row)
static void large_layout(const Rom *r, size_t src, size_t dict, uint8_t *layout) {
  int n = 0;
  while (n < 0xb0) {
    uint8_t key = b(r, src++);
    for (int bit = 0; bit < 8 && n < 0xb0; bit++) {
      if (!(key >> bit & 1)) { layout[n++] = b(r, src++); continue; }
      uint8_t lo = b(r, src++), hi = b(r, src++);
      int len = (hi >> 4) + 3, off = (hi & 0x0f) << 8 | lo;
      for (int i = 0; i < len && n < 0xb0; i++) layout[n++] = b(r, dict + (size_t)off + (size_t)i);
    }
  }
}

static void load_layout(const Rom *r, int layout_group, int room, WideRoom *out) {
  size_t e = at(4, r->t->layout_groups + layout_group * 8);
  bool small = b(r, e) != 0;
  int table_bank = b(r, e + 1), table = le(r, e + 2), data_bank = b(r, e + 4), base = le(r, e + 5);
  memset(out->layout, 0, sizeof out->layout);
  if (small) {
    int w = le(r, at(table_bank, table + room * 2));
    int mode = (w & 0x8000) ? 2 : (w & 0x4000) ? 1 : 0;
    small_layout(r, at(data_bank, base) + (size_t)(w & 0x3fff), mode, out->layout);
    out->width = 20;
    out->height = 16;
  } else {
    int w = le(r, at(table_bank, table + 0x1000 + room * 2));
    large_layout(r, at(data_bank, base) + (size_t)(w - 0x200), at(table_bank, table), out->layout);
    out->width = 30;
    out->height = 22;
  }
}

// --- metatile mapping ----------------------------------------------------------------------------

// loadTilesetHlpr: literals from the source, copies from a dictionary (3-byte or 2-byte references)
static void tileset_decompress(const Rom *r, size_t src, int dict_index, int size, uint8_t *out) {
  int lb = r->t->layout_bank;
  size_t d = at(lb, le(r, at(lb, r->t->layout_dictionaries + dict_index * 2)));   // a pointer to [bank|flag][addr hi][addr lo]
  int flag_bank = b(r, d);
  size_t dict = at(flag_bank & 0x3f, be(r, d + 1));
  bool long_refs = flag_bank & 0x80;
  int n = 0;
  while (n < size) {
    uint8_t key = b(r, src++);
    for (int bit = 0; bit < 8 && n < size; bit++) {
      if (!(key >> bit & 1)) { out[n++] = b(r, src++); continue; }
      int len, off;
      if (long_refs) { len = b(r, src); off = le(r, src + 1); src += 3; }
      else { uint8_t lo = b(r, src++), hi = b(r, src++); off = (hi & 0x0f) << 8 | lo; len = (hi >> 4) + 3; }
      for (int i = 0; i < len && n < size; i++) out[n++] = b(r, dict + (size_t)off + (size_t)i);
    }
  }
}

// 256 metatiles of 4 tiles and 4 attributes each
static void load_mapping(const Rom *r, int layout_index, uint8_t mapping[256][8]) {
  int lb = r->t->layout_bank;
  size_t header = at(lb, le(r, at(lb, r->t->layout_table + layout_index * 2)));
  uint8_t indices[0x200];
  int size = (b(r, header + 6) & 0x7f) << 8 | b(r, header + 7);
  if (size > (int)sizeof indices) size = sizeof indices;
  memset(indices, 0, sizeof indices);
  tileset_decompress(r, at(b(r, header + 1), be(r, header + 2)), b(r, header), size, indices);
  int mb = r->t->mapping_bank;
  size_t table = at(mb, 0x4004), index_data = at(mb, le(r, at(mb, 0x4000))), attr_data = at(mb, le(r, at(mb, 0x4002)));
  for (int m = 0; m < 256; m++) {
    int idx = indices[m * 2] | indices[m * 2 + 1] << 8;
    size_t e = table + (size_t)idx * 3;
    uint8_t e0 = b(r, e), e1 = b(r, e + 1), e2 = b(r, e + 2);
    size_t t = index_data + (size_t)(((e1 >> 4) << 8 | e0) * 4), a = attr_data + (size_t)(((e1 & 0x0f) << 8 | e2) * 4);
    for (int k = 0; k < 4; k++) { mapping[m][k] = b(r, t + (size_t)k); mapping[m][4 + k] = b(r, a + (size_t)k); }
  }
}

// --- graphics ------------------------------------------------------------------------------------

// decompressGraphics: raw (mode 0), bitmask runs per 16 bytes (mode 2), and two LZ variants (1, 3)
static void gfx_decompress(const Rom *r, size_t src, int mode, int n, uint8_t *out) {
  int o = 0;
  if (mode == 0) { for (; o < n; o++) out[o] = b(r, src++); return; }
  if (mode == 2) {
    while (o < n) {
      unsigned mask = (unsigned)b(r, src) << 8 | b(r, src + 1);
      src += 2;
      if (!mask) { for (int i = 0; i < 16 && o < n; i++) out[o++] = b(r, src++); continue; }
      uint8_t fill = b(r, src++);
      for (int i = 15; i >= 0 && o < n; i--) out[o++] = (mask >> i & 1) ? fill : b(r, src++);
    }
    return;
  }
  while (o < n) {
    uint8_t key = b(r, src++);
    for (int bit = 7; bit >= 0 && o < n; bit--) {
      if (!(key >> bit & 1)) { out[o++] = b(r, src++); continue; }
      int off, len;
      if (mode == 1) {
        uint8_t x = b(r, src++);
        off = x & 0x1f;
        len = x >> 5;
        if (!len) { len = b(r, src++); if (!len) len = 256; } else len += 1;
      } else {
        uint8_t lo = b(r, src++), y = b(r, src++);
        off = (y & 7) << 8 | lo;
        len = y >> 3;
        if (!len) { len = b(r, src++); if (!len) len = 256; } else len += 2;
      }
      for (int i = 0; i < len && o < n; i++, o++) out[o] = o - off - 1 >= 0 ? out[o - off - 1] : 0;
    }
  }
}

static void load_palette_header(const Rom *r, int index, WideRoom *out);

enum { GFX_COMPRESSED, GFX_UNCOMPRESSED, GFX_UNIQUE, GFX_MINIMAP };

// A list of 6-byte gfx headers (the continue bit chains them) into VRAM. In the European ROMs a
// gfx header table entry that starts with 0 is localized: 0, five [bank|mode][source] triples (one
// per language), then the destination and size.
static void load_gfx_headers(const Rom *r, size_t e, int kind, WideRoom *out) {
  uint8_t buf[128 * 16];
  for (int guard = 0; guard < 64; guard++) {
    uint8_t h0 = b(r, e);
    if (h0 == 0 && kind == GFX_UNIQUE) {                   // unique gfx: a palette entry
      uint8_t p = b(r, e + 1);
      load_palette_header(r, p & 0x7f, out);
      e += 2;
      if (!(p & 0x80)) return;
      continue;
    }
    size_t src_at = e + 1, rest = e + 3;
    if (h0 == 0 && (kind == GFX_COMPRESSED || kind == GFX_MINIMAP) && r->t->localized) {
      int lang = r->language >= 0 && r->language < 5 ? r->language : 0;
      h0 = b(r, e + 1 + (size_t)lang * 3);
      src_at = e + 2 + (size_t)lang * 3;
      rest = e + 16;
    }
    int src_bank = h0 & 0x3f, mode = kind == GFX_UNCOMPRESSED ? 0 : h0 >> 6;
    int src = be(r, src_at), dest = be(r, rest);
    uint8_t h5 = b(r, rest + 2);
    int n = ((h5 & 0x7f) + 1) * 16, bank = dest & 1, addr = (dest & 0xfff0) - 0x8000;
    gfx_decompress(r, at(src_bank, src), mode, n, buf);
    // The minimap header also loads the 32-column map and attributes into WRAM bank 4.
    if (kind == GFX_MINIMAP && n <= (int)sizeof out->tiles) {
      if ((dest & 0xfff0) == 0xd000) memcpy(out->tiles, buf, n);
      if ((dest & 0xfff0) == 0xd400) memcpy(out->attrs, buf, n);
    }
    for (int i = 0; i < n; i++) {
      int a = addr + i;
      if (a < 0 || a >= 0x1800) continue;
      out->vram[bank][a] = buf[i];
      out->vram_set[bank][a / 16] = true;
    }
    if (!(h5 & 0x80)) return;
    e = rest + 3;
  }
}

static void load_gfx(const Rom *r, int bank, int table, int index, int kind, WideRoom *out) {
  load_gfx_headers(r, at(bank, le(r, at(bank, table + index * 2))), kind, out);
}

// --- tile animations -----------------------------------------------------------------------------

// initializeAnimations (code/animations.s): up to four slots, each a list of (duration, gfx index)
// pairs that a $ff byte loops back through; the room starts with Seasons advanced once and Ages
// three times (the last two forced), each advance copying the slot's current tiles into VRAM.
static void load_animation_gfx(const Rom *r, int index, WideRoom *out) {
  size_t h = at(4, r->t->animation_gfx_headers + index * 6);
  int src_bank = b(r, h), src = be(r, h + 1), dest = be(r, h + 3), n = (b(r, h + 5) + 1) * 16;
  int bank = dest & 1, addr = (dest & 0xfff0) - 0x8000;
  size_t from = at(src_bank, src);
  for (int i = 0; i < n; i++) {
    int a = addr + i;
    if (a < 0 || a >= 0x1800) continue;
    out->vram[bank][a] = b(r, from + (size_t)i);
    out->vram_set[bank][a / 16] = true;
    out->animated[bank][a / 16] = true;
  }
}

// Every tile any frame of an enabled slot writes, walked until the list loops
static void mark_animated(const Rom *r, int p, WideRoom *out) {
  for (int guard = 0; guard < 64; guard++) {
    int gfx = b(r, at(4, p + 1)), next = b(r, at(4, p + 2));
    size_t h = at(4, r->t->animation_gfx_headers + gfx * 6);
    int dest = be(r, h + 3), n = (b(r, h + 5) + 1) * 16, addr = (dest & 0xfff0) - 0x8000;
    for (int a = addr; a < addr + n; a += 16)
      if (a >= 0 && a < 0x1800) out->animated[dest & 1][a / 16] = true;
    if (next == 0xff) return;
    p += 2;
  }
}

static void initialize_animations(const Rom *r, int group, bool seasons, WideRoom *out) {
  if (group == 0xff) return;
  int table = le(r, at(4, r->t->animation_groups + group * 2));
  int state = b(r, at(4, table)), counter[4], ptr[4];
  for (int i = 0; i < 4; i++) {
    int p = le(r, at(4, table + 1 + i * 2));
    counter[i] = b(r, at(4, p));
    ptr[i] = p + 1;
    if (state >> i & 1) mark_animated(r, p, out);
  }
  for (int pass = 0; pass < (seasons ? 1 : 3); pass++) {
    if (pass) state |= 0x80;
    for (int i = 0; i < 4; i++) {
      if (!(state >> i & 1)) continue;
      if (!(state & 0x80) && --counter[i] & 0xff) continue;
      int gfx = b(r, at(4, ptr[i]++));
      int next = b(r, at(4, ptr[i]++));
      if (next == 0xff) {
        ptr[i] = (ptr[i] + (0xff00 | b(r, at(4, ptr[i])))) & 0xffff;
        next = b(r, at(4, ptr[i]++));
      }
      counter[i] = next;
      load_animation_gfx(r, gfx, out);
    }
    state &= 0x7f;
  }
}

// --- palettes ------------------------------------------------------------------------------------

static void load_palette_header(const Rom *r, int index, WideRoom *out) {
  size_t e = at(1, le(r, at(1, r->t->palette_headers + index * 2)));
  for (int guard = 0; guard < 16; guard++, e += 3) {
    uint8_t h = b(r, e);
    int count = (h & 7) + 1, first = h >> 3 & 7;
    size_t data = at(r->t->palette_bank, le(r, e + 1));
    if (!(h & 0x40))
      for (int p = 0; p < count && first + p < 8; p++) {
        for (int k = 0; k < 8; k++) out->bg_pal[(first + p) * 8 + k] = b(r, data + (size_t)(p * 8 + k));
        out->pal_set[first + p] = true;
      }
    if (!(h & 0x80)) return;
  }
}

// --- the room ------------------------------------------------------------------------------------

static bool is_temple_remains(int room) {             // checkIsTempleRemains
  return room >= 0x14 && room - 4 < 0x30 && ((room - 4) & 0x0f) < 4;
}

static bool is_moblin_keep(int room) {                // moblinKeepRooms
  static const uint8_t rooms[5] = {0x5b, 0x5c, 0x6b, 0x6c, 0x7b};
  for (int i = 0; i < 5; i++)
    if (rooms[i] == room) return true;
  return false;
}

bool wide_decode_room(const uint8_t *rom, size_t size, bool seasons, const WideRoomKey *key, WideRoom *out) {
  bool eu = size > 0x142 && rom[0x142] == 'P';             // the game code's region letter: AZ8P, AZ7P
  Rom r = {rom, size, eu ? (seasons ? &seasons_eu_tables : &ages_eu_tables) : (seasons ? &seasons_tables : &ages_tables), key->language};
  memset(out, 0, sizeof *out);
  if (key->group < 0 || key->group > 7 || key->room < 0 || key->room > 0xff) return false;
  int group = key->group;
  if (!seasons && group < 2 && (key->room_flags & 1)) group |= 2;          // the layout-swapped (underwater) room
  int t = b(&r, at(4, le(&r, at(4, r.t->tilesets_by_group + group * 2)) + key->room));
  if (key->tileset_add && !(b(&r, at(4, r.t->tileset_data + (t & 0x7f) * 8) + 1) & TILESETFLAG_SIDESCROLL))
    t = (t + key->tileset_add) & 0xff;
  size_t entry = at(4, r.t->tileset_data + (t & 0x7f) * 8);
  if (seasons && b(&r, entry) == 0xff) entry = at(4, le(&r, entry + 1) + (key->season & 3) * 8);
  int layout_room = key->room;
  if (seasons && key->group == 0 && key->remains_lava && is_temple_remains(key->room)) {   // getTempleRemainsSeasonsTilesetData
    static const uint8_t season_offsets[4] = {0xbc, 0xc0, 0xc4, 0xc8};    // adjustLoadingRoomForTempleRemains
    entry = at(4, r.t->temple_remains + (key->season & 3) * 8);
    t = 0;
    layout_room = (key->room + season_offsets[key->season & 3]) & 0xff;
  } else if (seasons && key->group == 0 && key->keep_destroyed && is_moblin_keep(key->room)) {   // getMoblinKeepSeasonsTilesetData
    entry = at(4, r.t->moblin_keep + ((key->companion - 0x0a) & 3) * 8);
    t = 0;
  }
  uint8_t *tileset = out->tileset;
  for (int i = 0; i < 8; i++) tileset[i] = b(&r, entry + (size_t)i);
  out->flags = tileset[1];
  if (key->group >= 1) {                                    // indoor rooms of the other world
    int bits = le(&r, at(2, r.t->alt_world + key->group * 2));
    if (b(&r, at(2, bits + key->room / 8)) & (1 << (key->room & 7))) out->flags |= TILESETFLAG_PAST;
  }

  load_layout(&r, tileset[6], layout_room, out);
  static uint8_t mapping[256][8];
  load_mapping(&r, tileset[5], mapping);
  int collisions = seasons ? tileset[0] >> 4 : (tileset[0] >> 4) & 7;
  if (!seasons && collisions == 0 && (out->flags & TILESETFLAG_PAST) && key->room != 0x38)
    for (int m = 0x40; m < 0x80; m++)                       // setPastCliffPalettesToRed
      for (int k = 4; k < 8; k++)
        if ((mapping[m][k] & 7) == 6) mapping[m][k] &= 0xf8;
  for (int y = 0; y < 11; y++)
    for (int x = 0; x < 16; x++) {
      const uint8_t *mt = mapping[out->layout[y * 16 + x]];
      int base = y * 64 + x * 2;
      int pos[4] = {base, base + 1, base + 32, base + 33};
      for (int k = 0; k < 4; k++) { out->tiles[pos[k]] = mt[k]; out->attrs[pos[k]] = mt[4 + k]; }
    }

  load_gfx(&r, r.t->gfx_bank, r.t->gfx_headers, tileset[3], GFX_COMPRESSED, out);
  load_palette_header(&r, tileset[4], out);
  int unique = (tileset[2] | (t & 0x80)) & 0x7f;
  if (seasons && key->group == 0 && key->room == 0x96 && !(key->room_flags & 0x80)) unique = 0x20;
  out->unique = unique;
  if (unique) load_gfx(&r, 4, r.t->unique_gfx_headers, unique, GFX_UNIQUE, out);
  initialize_animations(&r, tileset[7], seasons, out);
  int dungeon = (out->flags & TILESETFLAG_DUNGEON) ? tileset[0] & 0x0f : 0xff;
  if (!seasons && dungeon != 0x0f && (out->flags & TILESETFLAG_PAST) && !(out->flags & (TILESETFLAG_OUTDOORS | TILESETFLAG_SIDESCROLL)) &&
      (out->flags & (TILESETFLAG_INDOORS | TILESETFLAG_DUNGEON | TILESETFLAG_LARGE_INDOORS)))
    load_gfx(&r, 1, r.t->uncmp_gfx_headers, 0x37, GFX_UNCOMPRESSED, out);    // the past's chest and sign
  return true;
}

static void seasons_map_location(const uint8_t wram[4096], OverworldMap *map) {
  int group = wram[0x63a], room = wram[0x63b];
  if (group == 0 && room == 0xc9) room = 0x40; // Sword upgrade
  if (group == 2) room = 0xc9;                  // Maku Tree
  if (group == 4 && wram[0xc49] == 3 && wram[0xc4c] == 0x98) group = room = 0;
  map->plane = group >= 4 ? 2 : group == 1;
  map->room = room;

  map->floodgates = wram[0x781] & 0x80;
  map->ship_docked = wram[0x6ca + 0x17/8] & (1 << (0x17%8));
}

void overworld_map_state(bool seasons, const uint8_t wram[4096], OverworldMap *map) {
  *map = (OverworldMap){.seasons = seasons, .companion = wram[0x610]};
  if (seasons) {
    seasons_map_location(wram, map);
  } else {
    uint8_t flags = wram[0xc34];
    map->plane = (flags & TILESETFLAG_DUNGEON) && !(flags & TILESETFLAG_LARGE_INDOORS)
        ? 2 : !!(flags & TILESETFLAG_PAST);
    map->room = map->plane != 2 && (flags & 2) ? 0x38 : wram[0x63b];
    map->symmetry_restored = wram[0x713] & 1;
    map->water_shifted = wram[0x841] & 1;
  }
  for (int i = 0; i < 256; i++)
    map->visited[i] = wram[(map->plane == 1 ? 0x800 : 0x700) + i] & 0x10;
  if (seasons && !map->plane) map->visited[0xc9] = wram[0x80b] & 0x10;
}

// The original map stores alternate artwork in the off-screen columns of each row.
static void map_substitution(WideRoom *art, int dst, int src, int w, int h) {
  for (int y = 0; y < h; y++, dst += 32, src += 32) {
    memcpy(art->tiles + dst, art->tiles + src, w);
    memcpy(art->attrs + dst, art->attrs + src, w);
  }
}

int overworld_map_width(const OverworldMap *map) {
  if (map->plane > 1) return 0;
  return map->seasons ? (map->plane ? 88 : 128) : 112;
}

int overworld_map_height(const OverworldMap *map) {
  if (map->plane > 1) return 0;
  return map->seasons && map->plane == 1 ? 64 : overworld_map_width(map);
}

static uint32_t map_pixel(const WideRoom *art, int tile, int attr, int x, int y) {
  int row = (attr & 0x40) ? 7 - y%8 : y%8, bit = (attr & 0x20) ? x%8 : 7 - x%8;
  const uint8_t *data = art->vram[(attr>>3)&1] + 0x1000 + (int8_t)tile*16 + row*2;
  int ci = ((data[0]>>bit)&1) | (((data[1]>>bit)&1)<<1);
  int p = (attr&7)*8 + ci*2;
  int rgb = art->bg_pal[p] | art->bg_pal[p+1]<<8;
  return 0xff000000u | ((rgb&31)*255/31)<<16 |
                         (((rgb>>5)&31)*255/31)<<8 | ((rgb>>10)&31)*255/31;
}

void overworld_map_pixels(const uint8_t *rom, size_t size, const OverworldMap *map, uint32_t *pixels) {
  if (size <= 0x142 || map->plane > 1) return;
  bool eu = rom[0x142] == 'P';
  const Tables *tables = map->seasons ? (eu ? &seasons_eu_tables : &seasons_tables)
                                      : (eu ? &ages_eu_tables : &ages_tables);
  Rom r = {rom, size, tables, 0};
  WideRoom art = {0};
  load_gfx(&r, r.t->gfx_bank, r.t->gfx_headers, 0x0d + map->plane, GFX_MINIMAP, &art);
  load_palette_header(&r, 7 + map->plane, &art);
  if (!map->seasons) {
    if (map->plane == 0) {
      if (map->companion == 0x0c || map->companion == 0x0d)
        map_substitution(&art, 0x68, map->companion == 0x0c ? 0x75 : 0x78, 3, 3);
      if (map->symmetry_restored) map_substitution(&art, 0x45, 0x7b, 3, 2);
    }
    if (map->water_shifted) map_substitution(&art, 0xc3, 0xd5, 3, 2);
  } else if (map->plane == 0) {
    if (map->companion == 0x0c || map->companion == 0x0d)
      map_substitution(&art, 0xa8, map->companion == 0x0c ? 0xb6 : 0xbb, 5, 4);
    if (map->floodgates) map_substitution(&art, 0xe2, 0xf4, 2, 2);
    if (map->ship_docked) {
      map_substitution(&art, 0x1e4, 0x1fe, 1, 2);
      map_substitution(&art, 0x1f0, 0x1ff, 1, 1);
    }
  } else if (map->ship_docked) map_substitution(&art, 0x168, 0x178, 1, 2);

  int width = overworld_map_width(map)/8, height = overworld_map_height(map)/8;
  int start = !map->seasons ? 2*32+3 : (map->plane ? 5*32+4 : 32+2);
  for (int y = 0; y < height*8; y++) for (int x = 0; x < width*8; x++) {
    int pos = start + (y/8)*32 + x/8;
    bool visited = map->visited[(y/8)*16 + x/8] != 0;
    int tile = visited ? art.tiles[pos] : 4, attr = visited ? art.attrs[pos] : 0x0a;
    pixels[y*width*8+x] = map_pixel(&art, tile, attr, x, y);
  }
}

// Matches dungeonMap_getTileForRoom. Compass markers supersede ordinary room tiles.
static uint8_t dungeon_tile(int room, int flags, int properties, bool compass, bool has_map) {
  if (!room || properties == 0x60 || properties == 0x70) return 0xac;
  if (compass) {
    int feature = properties & 0x70;
    if (feature == 0x40) return 0x83;
    if ((feature == 0x20 || feature == 0x30) && !(flags & 0x20)) return 0xae;
  }
  if (flags & 0x10) return 0xb0 + ((flags | properties) & 15);
  return has_map ? 0xaf : 0xac;
}

void dungeon_map_state(bool seasons, const uint8_t *rom, size_t size,
                       const uint8_t wram[4096], const uint8_t bank2[4096], DungeonMap *map) {
  *map = (DungeonMap){0};
  int offset = seasons ? 0x1c : 0;
  int index = wram[0xc39 + offset], floors = wram[0xc40 + offset];
  int flags_page = wram[0xc3d + offset];
  if (index >= (seasons ? 12 : 16) || !floors || floors > DUNGEON_MAX_FLOORS ||
      (flags_page != 0xc9 && flags_page != 0xca)) return;

  int group = wram[0xc2d + offset];
  // Bank 1 room-property tables have the same addresses in the US and European releases.
  size_t props = (seasons ? 0x4d41 : 0x4dce) + (group & 1) * 256;
  if (props + 256 > size) return;
  int item_byte = (seasons ? 0x67c : 0x684) + index / 8;
  int bit = 1 << (index % 8);
  bool compass = wram[item_byte] & bit;
  bool has_map = wram[item_byte + 2] & bit;
  map->index = index;
  map->floors = floors;
  map->base_floor = wram[0xc41 + offset];
  map->link_floor = wram[0x63d];
  map->link_cell = wram[0x63c];
  if (group == 5 && wram[0xc30 + offset] == (seasons ? 0x9e : 0xf5)) map->link_cell = 0x13;
  int visible = wram[0x662 + index];
  if (compass) visible |= wram[0xc42 + offset];
  map->visible = (has_map ? 0xff : visible) & ((1 << floors) - 1);
  for (int floor = 0; floor < floors; floor++) {
    if (!(map->visible & (1 << floor))) continue;
    for (int cell = 0; cell < DUNGEON_CELLS; cell++) {
      int room = bank2[0xc00 + floor * DUNGEON_CELLS + cell];
      int flags = wram[(flags_page - 0xc0) * 256 + room];
      map->tiles[floor][cell] = dungeon_tile(room, flags, rom[props + room], compass, has_map);
    }
  }
}

void dungeon_map_pixels(bool seasons, const uint8_t *rom, size_t size,
                        const DungeonMap *map, uint32_t *pixels) {
  if (!map->floors || size <= 0x142) return;
  bool eu = rom[0x142] == 'P';
  const Tables *tables = seasons ? (eu ? &seasons_eu_tables : &seasons_tables)
                                 : (eu ? &ages_eu_tables : &ages_tables);
  Rom r = {rom, size, tables, 0};
  WideRoom art = {0};
  load_gfx(&r, tables->gfx_bank, tables->gfx_headers, 0x0f, GFX_MINIMAP, &art);
  load_palette_header(&r, 9, &art);
  for (int floor = 0; floor < map->floors; floor++) {
    for (int y = 0; y < 64; y++) for (int x = 0; x < 64; x++) {
      int tile = map->tiles[floor][(y/8)*8 + x/8];
      int palette = tile == 0x83 ? 0 : tile == 0xae ? 2 : tile == 0xaf ? 4 : 5;
      pixels[floor * DUNGEON_FLOOR_PIXELS + y*64+x] = map_pixel(&art, tile ? tile : 0xac, palette, x, y);
    }
  }
}

void inventory_load_art(const uint8_t *rom, size_t size, bool seasons, InventoryArt *art) {
  bool eu = size > 0x142 && rom[0x142] == 'P';
  const Tables *tables = seasons ? (eu ? &seasons_eu_tables : &seasons_tables)
                                 : (eu ? &ages_eu_tables : &ages_tables);
  Rom r = {rom, size, tables, 0};
  WideRoom menu = {0};
  load_gfx(&r, tables->gfx_bank, tables->gfx_headers, 8, GFX_MINIMAP, &menu);
  load_palette_header(&r, 0x0a, &menu);
  memcpy(art->tiles, menu.vram[1], sizeof art->tiles);
  memcpy(art->palettes, menu.bg_pal, sizeof art->palettes);
}
