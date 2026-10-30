#include "unit.h"
#include "core/gb.h"
#include "game/game.h"
#include "game/gen.h"
#include "game/features.h"
#include "hooks/hooks.h"
#include "platform/setup.h"
#include "assets/assets.h"

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
