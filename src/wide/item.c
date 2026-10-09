#include "wide/item.h"
#include <string.h>

// The tables, from the disassembly's symbols. treasureDisplayData1 and 2 are both in bank $3f.
typedef struct {
  int icons_bank, icons;        // spr_item_icons_1: the icon tiles, uncompressed
  int td1, td2;                 // treasureDisplayData1 / 2
  int small_icon;               // below this tile index the left half uses the other palette
} ItemTables;
static const ItemTables ages_items = {0x19, 0x4000, 0x6d41, 0x6d62, 0x84};
static const ItemTables seasons_items = {0x1b, 0x7620, 0x6d63, 0x6d87, 0x86};
enum { TD_BANK = 0x3f, ITEM_SLINGSHOT = 0x13, SLINGSHOT_LEVEL = 0xb3 };   // wSlingshotLevel is $c6b3

static size_t at(int bank, int addr) {
  return addr < 0x4000 ? (size_t)addr : (size_t)bank * 0x4000 + (size_t)(addr - 0x4000);
}
static uint8_t rb(const uint8_t *rom, size_t size, size_t o) { return o < size ? rom[o] : 0; }
static int le16(const uint8_t *rom, size_t size, size_t o) {
  return rb(rom, size, o) | (rb(rom, size, o + 1) << 8);
}

// One 8x16 half: two stacked 8x8 tiles, 16 bytes each, at spr_item_icons_1 + index * 32.
static void half_pixels(const uint8_t *rom, size_t size, const ItemTables *t, bool seasons,
                        int index, int x0, uint8_t ci[16][16]) {
  // Ages only: the harp song icons above $a3 have a smaller version two entries along, and the
  // status bar draws that one.
  if (!seasons && index >= 0xa3) index = (index + 2) & 0xff;
  // loadItemIconGfx doubles the index in a 8-bit register before multiplying by 16, so an index
  // past $7f wraps -- which is how the table's high indices ($9e for bombs, and so on) land inside
  // the 48 icons of spr_item_icons_1..3, which sit back to back in the ROM.
  size_t g = at(t->icons_bank, t->icons) + (size_t)(((index * 2) & 0xff) * 16);
  for (int y = 0; y < 16; y++) {
    size_t off = g + (size_t)(y >> 3) * 16 + (size_t)(y & 7) * 2;
    uint8_t lo = rb(rom, size, off), hi = rb(rom, size, off + 1);
    for (int x = 0; x < 8; x++) {
      int bit = 7 - x;
      ci[y][x0 + x] = (uint8_t)((((hi >> bit) & 1) << 1) | ((lo >> bit) & 1));
    }
  }
}

bool wide_item_icon(const uint8_t *rom, size_t size, bool seasons, uint8_t item,
                    const uint8_t *c600, WideItemIcon *out) {
  if (!rom || !item) return false;
  const ItemTables *t = seasons ? &seasons_items : &ages_items;

  // getTableIndices: walk the three-byte entries looking for this item. The list ends with a zero
  // entry whose own two remaining bytes are the default, so the walk lands on the right pair either
  // way: `d` picks the record within a sub-table, `e` picks the sub-table.
  int d = item;
  size_t p = at(TD_BANK, t->td1);
  while (rb(rom, size, p) && rb(rom, size, p) != item) p += 3;
  bool found = rb(rom, size, p) != 0;
  p++;
  // Seasons lists the slingshot twice and takes the second entry at level 2 (the hyper slingshot).
  if (found && seasons && item == ITEM_SLINGSHOT && c600[SLINGSHOT_LEVEL] == 0x02) p += 3;
  int level_byte = rb(rom, size, p), e = rb(rom, size, p + 1);
  if (level_byte) d = c600[level_byte];

  size_t rec = (size_t)le16(rom, size, at(TD_BANK, t->td2) + (size_t)e * 2);
  rec = at(TD_BANK, (int)((rec + (size_t)d * 7) & 0xffff));

  int left = rb(rom, size, rec + 1), left_attr = rb(rom, size, rec + 2);
  int right = rb(rom, size, rec + 3), right_attr = rb(rom, size, rec + 4);
  if (!left) return false;                              // no icon for this item

  // The seed satchel, shooter and slingshot icons sit below this index and take the palette one
  // group down, with bit 0 set.
  if (left < t->small_icon) left_attr = ((left_attr - 3) & 0xff) | 0x01;

  memset(out->ci, 0, sizeof out->ci);
  out->halves = right ? 2 : 1;
  out->pal[0] = (uint8_t)(left_attr & 7);
  out->pal[1] = (uint8_t)(right_attr & 7);
  half_pixels(rom, size, t, seasons, left, 0, out->ci);
  if (right) half_pixels(rom, size, t, seasons, right, 8, out->ci);
  return true;
}
