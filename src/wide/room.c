// The room loader's data path, from the disassembly (code/bank0.s loadTilesetData, loadRoomLayout,
// loadTileset(Layout), generateW3VramTilesAndAttributes, loadGfxHeader, decompressGraphics,
// loadPaletteHeader), reading only ROM data.
#include "wide/room.h"
#include <string.h>

typedef struct {
  int tilesets_by_group, tileset_data, temple_remains, moblin_keep;   // bank 4
  int layout_groups;                                                  // bank 4
  int layout_table, layout_dictionaries;                             // bank 1
  int mapping_bank;                                                   // tileMappingTable at :4004
  int gfx_headers, uncmp_gfx_headers, palette_headers;               // bank 1
  int palette_bank;                                                   // paletteDataStart
  int unique_gfx_headers;                                             // bank 4
  int animation_groups, animation_gfx_headers;                        // bank 4
  int alt_world;                                                      // bank 2
} Tables;

static const Tables ages_tables = {0x52d4, 0x4f9c, 0, 0, 0x4f6c, 0x787e, 0x7870, 0x18, 0x69da, 0x6744, 0x632c, 0x17, 0x5b28, 0x5b52, 0x5be9, 0x4057};
static const Tables seasons_tables = {0x533c, 0x4c84, 0x52fc, 0x531c, 0x4c4c, 0x7964, 0x794e, 0x17, 0x6926, 0x66d0, 0x6290, 0x16, 0x595e, 0x59b0, 0x5a48, 0x4057};

enum { TILESETFLAG_PAST = 0x80, TILESETFLAG_SIDESCROLL = 0x20, TILESETFLAG_LARGE_INDOORS = 0x10,
       TILESETFLAG_DUNGEON = 0x08, TILESETFLAG_INDOORS = 0x04, TILESETFLAG_OUTDOORS = 0x01 };

typedef struct {
  const uint8_t *rom;
  size_t size;
  const Tables *t;
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
  size_t d = at(1, le(r, at(1, r->t->layout_dictionaries + dict_index * 2)));   // a pointer to [bank|flag][addr hi][addr lo]
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
  size_t header = at(1, le(r, at(1, r->t->layout_table + layout_index * 2)));
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

// A list of 6-byte gfx headers (the continue bit chains them) into VRAM
static void load_gfx_headers(const Rom *r, size_t e, bool uncompressed, WideRoom *out) {
  static uint8_t buf[0x1000 * 16];
  for (int guard = 0; guard < 64; guard++, e += 6) {
    uint8_t h0 = b(r, e);
    if (h0 == 0 && !uncompressed) {                        // unique gfx: a palette entry
      uint8_t p = b(r, e + 1);
      load_palette_header(r, p & 0x7f, out);
      e -= 4;                                              // a 2-byte entry
      if (!(p & 0x80)) return;
      continue;
    }
    int src_bank = h0 & 0x3f, mode = uncompressed ? 0 : h0 >> 6;
    int src = be(r, e + 1), dest = be(r, e + 3);
    uint8_t h5 = b(r, e + 5);
    int n = ((h5 & 0x7f) + 1) * 16, bank = dest & 1, addr = (dest & 0xfff0) - 0x8000;
    gfx_decompress(r, at(src_bank, src), mode, n, buf);
    for (int i = 0; i < n; i++) {
      int a = addr + i;
      if (a < 0 || a >= 0x1800) continue;
      out->vram[bank][a] = buf[i];
      out->vram_set[bank][a / 16] = true;
    }
    if (!(h5 & 0x80)) return;
  }
}

static void load_gfx(const Rom *r, size_t table, int index, bool uncompressed, WideRoom *out) {
  load_gfx_headers(r, at(1, le(r, table + (size_t)index * 2)), uncompressed, out);
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

bool wide_decode_room(const uint8_t *rom, size_t size, bool seasons, const WideRoomKey *key, WideRoom *out) {
  Rom r = {rom, size, seasons ? &seasons_tables : &ages_tables};
  memset(out, 0, sizeof *out);
  if (key->group < 0 || key->group > 7 || key->room < 0 || key->room > 0xff) return false;
  int group = key->group;
  if (!seasons && group < 2 && (key->room_flags & 1)) group |= 2;          // the layout-swapped (underwater) room
  int t = b(&r, at(4, le(&r, at(4, r.t->tilesets_by_group + group * 2)) + key->room));
  size_t entry = at(4, r.t->tileset_data + (t & 0x7f) * 8);
  if (seasons && b(&r, entry) == 0xff) entry = at(4, le(&r, entry + 1) + (key->season & 3) * 8);
  uint8_t *tileset = out->tileset;
  for (int i = 0; i < 8; i++) tileset[i] = b(&r, entry + (size_t)i);
  out->flags = tileset[1];
  if (key->group >= 1) {                                    // indoor rooms of the other world
    int bits = le(&r, at(2, r.t->alt_world + key->group * 2));
    if (b(&r, at(2, bits + key->room / 8)) & (1 << (key->room & 7))) out->flags |= TILESETFLAG_PAST;
  }

  load_layout(&r, tileset[6], key->room, out);
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

  load_gfx(&r, at(1, r.t->gfx_headers), tileset[3], false, out);
  load_palette_header(&r, tileset[4], out);
  int unique = (tileset[2] | (t & 0x80)) & 0x7f;
  if (seasons && key->group == 0 && key->room == 0x96 && !(key->room_flags & 0x80)) unique = 0x20;
  out->unique = unique;
  if (unique) load_gfx_headers(&r, at(4, le(&r, at(4, r.t->unique_gfx_headers + unique * 2))), false, out);
  initialize_animations(&r, tileset[7], seasons, out);
  int dungeon = (out->flags & TILESETFLAG_DUNGEON) ? tileset[0] & 0x0f : 0xff;
  if (!seasons && dungeon != 0x0f && (out->flags & TILESETFLAG_PAST) && !(out->flags & (TILESETFLAG_OUTDOORS | TILESETFLAG_SIDESCROLL)) &&
      (out->flags & (TILESETFLAG_INDOORS | TILESETFLAG_DUNGEON | TILESETFLAG_LARGE_INDOORS)))
    load_gfx(&r, at(1, r.t->uncmp_gfx_headers), 0x37, true, out);    // the past's chest and sign
  return true;
}
