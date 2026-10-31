#include "unit.h"
#include "core/gb.h"
#include "game/game.h"
#include "game/gen.h"
#include "game/features.h"
#include "hooks/hooks.h"
#include "platform/setup.h"
#include "assets/assets.h"
#include "wide/item.h"
#include "wide/wide.h"

static void check_counts(GB *gb, const uint8_t *rom, size_t size, bool seasons) {
  const uint8_t *page = &gb->wram[0][0x600];
  W8(wNumBombs) = 0x19;
  W8(wNumBombchus) = 0x07;
  ASSERT_EQ(wide_item_count(seasons, 3, page), 19);
  ASSERT_EQ(wide_item_count(seasons, 0x0d, page), 7);
  ASSERT_EQ(wide_item_count(seasons, 5, page), -1); // sword: no ammo
  ASSERT_EQ(wide_item_count(seasons, 0, page), -1);
  for (int seed = 0; seed < 5; seed++) mem_wr(gb, wNumEmberSeeds + seed, 0x11 * (seed + 1));
  for (int seed = 0; seed < 5; seed++) {
    W8(wSatchelSelectedSeeds) = seed;
    W8(wShooterSelectedSeeds) = 4 - seed;
    ASSERT_EQ(wide_item_count(seasons, 0x19, page), 11 * (seed + 1));
    ASSERT_EQ(wide_item_count(seasons, seasons ? 0x13 : 0x0f, page), 11 * (5 - seed));
  }
  W8(wSatchelSelectedSeeds) = 5;
  ASSERT_EQ(wide_item_count(seasons, 0x19, page), -1);
  W8(wSatchelSelectedSeeds) = 0;
  for (int count = 0; count <= 99; count++) {
    W8(wNumBombs) = (count / 10) * 16 + count % 10;
    ASSERT_EQ(wide_item_count(seasons, 3, page), count);
  }
  W8(wNumBombs) = 0xfa;
  ASSERT_EQ(wide_item_count(seasons, 3, page), -1);

  // The real ROM supplies the icons. Check visible digits in both side layouts and widescreen,
  // including an empty consumable, a live decrement and a replacement by a non-consumable.
  static GBSample s;
  static uint8_t picture[WIDE_W * FB_H * 3];
  memset(&s, 0, sizeof s);
  memcpy(s.wram[0], gb->wram[0], sizeof s.wram[0]);
  s.hram[seasons ? 0x19 : 0x1b] = 2;
  s.wram[0][0xd00] = 1;
  for (int y = 0; y < 16; y++) s.framebuffer[y * FB_W] = 0x7fff;
  WideOptions o = {.seasons = seasons, .rom = rom, .rom_size = size, .slots = true, .slot_item = {3, 0x19}};
  // Independently specified bitmaps for 0, 1, 8 and 9.
  const uint8_t zero[] = {7, 5, 5, 5, 7}, one[] = {2, 6, 2, 2, 7};
  const uint8_t eight[] = {7, 5, 7, 5, 7}, nine[] = {7, 5, 7, 1, 7};
  const int counts[] = {0x19, 0x18, 0, 0x99};
  const uint8_t *tens[] = {one, one, zero, nine}, *units[] = {nine, eight, zero, nine};
  for (int n = 0; n < 4; n++) {
    s.wram[0][wNumBombs & 0xfff] = counts[n];
    s.wram[0][wNumEmberSeeds & 0xfff] = counts[n];
    for (int slot = 0; slot < 2; slot++) for (int layout = 0; layout < 3; layout++) {
      int w = layout == 0 ? WIDE_STRIP_W : layout == 1 ? WIDE_BOX_W : WIDE_W;
      int x0 = layout == 0 ? 4 : layout == 1 ? (slot ? 8 : 1) : 2 + slot * 38;
      int y0 = layout == 0 ? 26 : 9;
      if (layout < 2) ASSERT(wide_slot_strip(&s, &o, slot, layout == 1, picture));
      else wide_render(&s, &o, picture);
      for (int y = 0; y < 5; y++) for (int x = 0; x < 7; x++) {
        bool ink = x < 3 ? (tens[n][y] & (4 >> x)) : x > 3 && (units[n][y] & (4 >> (x - 4)));
        for (int c = 0; c < 3; c++) ASSERT_EQ(picture[((y0 + y) * w + x0 + x) * 3 + c], ink ? 0 : 255);
      }
    }
  }
  o.slot_item[0] = 5;
  ASSERT(wide_slot_strip(&s, &o, 0, false, picture));
  for (int i = 26 * WIDE_STRIP_W * 3; i < WIDE_STRIP_W * FB_H * 3; i++) ASSERT_EQ(picture[i], 0);
  o.slots = false;
  ASSERT(!wide_slot_strip(&s, &o, 0, false, picture));
  o.slots = true;
  s.hram[seasons ? 0x19 : 0x1b] = 0;
  ASSERT(!wide_slot_strip(&s, &o, 0, false, picture));
}

static void draw(GB *gb, bool full) {
  gb->sp = 0xcff0;
  mem_wr(gb, 0x2000, 2);
  mem_wr(gb, hRomBank, 2);
  mem_wr(gb, 0xff70, 4);
  if (full) {
    loadEquippedItemGfx_hook(gb);
    gb->sp = 0xcff0;
  }
  drawItemTilesOnStatusBar_hook(gb);
  ASSERT(!gb->hung);
}

static void check(const char *path, bool seasons, bool eu, int item, bool underwater, int slot) {
  size_t size;
  uint8_t *rom = oracles_read_file(path, &size);
  if (!rom) SKIP("ROM missing");
  GB *gb = calloc(1, sizeof *gb);
  gb_init(gb);
  ASSERT(gb_load_rom(gb, rom, size));
  syms_select(seasons, eu);
  gb->cyctab = cyctab_alloc(rom, size);
  assets_zero_code(rom, size);
  gb_reset(gb);
  hooks_init();
  W8(wInventoryB) = 5; // sword
  W8(wInventoryA) = 1; // shield
  W8(wSwordLevel) = 1;
  W8(wShieldLevel) = 1;
  W8(wSeedSatchelLevel) = 1;
  W8(wNumBombs) = 0x19;
  W8(wNumEmberSeeds) = 0x19;
  W8(wObtainedTreasureFlags) = 0xff;
  mem_wr(gb, wObtainedTreasureFlags + 1, 0xff);
  W8(wTilesetFlags) = underwater ? 0x40 : 0;
  mem_wr(gb, wInventoryStorage, item);
  features = (Features){.four_slots = true, .slot_boxes = true, .slot_item = {0, 0}};
  features.slot_item[slot] = item;
  draw(gb, true);
  uint8_t expected[0x1000];
  memcpy(expected, gb->wram[4], sizeof expected);
  uint8_t battr = W8(wBItemSpriteAttribute1), aattr = W8(wAItemSpriteAttribute1);
  ASSERT_EQ(features_slot_press(gb, slot), underwater ? JOY_A : JOY_B);
  ASSERT_EQ(underwater ? W8(wInventoryA) : W8(wInventoryB), item);
  // Consumption requests this exact count/palette redraw, without loading new icon graphics.
  W8(wNumBombs) = 0x18;
  W8(wNumEmberSeeds) = 0x18;
  draw(gb, false);
  ASSERT_EQ(memcmp(expected, gb->wram[4], sizeof expected), 0);
  ASSERT_EQ(W8(wBItemSpriteAttribute1), battr);
  ASSERT_EQ(W8(wAItemSpriteAttribute1), aattr);
  ASSERT_EQ(underwater ? W8(wInventoryA) : W8(wInventoryB), item);
  // A full redraw during the press must also retain the original icon graphics.
  draw(gb, true);
  ASSERT_EQ(memcmp(expected, gb->wram[4], sizeof expected), 0);
  features_slot_release(gb, slot);
  ASSERT_EQ(W8(wInventoryB), 5);
  ASSERT_EQ(W8(wInventoryA), 1);
  draw(gb, false);
  ASSERT_EQ(memcmp(expected, gb->wram[4], sizeof expected), 0);
  // Without separate X/Y boxes, the existing behavior intentionally shows the borrowed item.
  features.slot_boxes = false;
  ASSERT_EQ(features_slot_press(gb, slot), underwater ? JOY_A : JOY_B);
  ASSERT_EQ(features_hud_item(gb, underwater ? wInventoryA : wInventoryB), item);
  draw(gb, true);
  ASSERT(memcmp(expected, gb->wram[4], sizeof expected) != 0);
  features_slot_release(gb, slot);
  draw(gb, true);
  ASSERT_EQ(memcmp(expected, gb->wram[4], sizeof expected), 0);
  if (slot == 0 && item == 3 && !underwater) check_counts(gb, rom, size, seasons);
  free((void *)gb->cyctab); free(gb); free(rom);
}
int main(void) {
  const char *paths[] = {
    GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (USA, Australia).gbc",
    GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc",
    GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Ages (Europe) (En,Fr,De,Es,It).gbc",
    GAME_ROM_DIR "/Legend of Zelda, The - Oracle of Seasons (Europe) (En,Fr,De,Es,It).gbc"};
  for (int variant = 0; variant < 4; variant++)
    for (int slot = 0; slot < 2; slot++)
      for (int item = 3; item <= 0x19; item += 0x16) {
        check(paths[variant], variant % 2, variant >= 2, item, false, slot);
        if (!(variant % 2)) check(paths[variant], false, variant >= 2, item, true, slot);
      }
  puts("slot HUD: both slots, bombs and satchel, USA/EU Ages/Seasons, underwater A, with/without boxes passed");
}
