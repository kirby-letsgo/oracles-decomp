#include "game/game.h"
#include "game/features.h"

Features features;

// the moments the player could open the inventory and change the buttons there
static bool can_change_items(GB *gb) {
  return !W8(wMenuDisabled) && !W8(wOpenedMenuType) && !W8(wTextIsActive) && !W8(wDisableLinkCollisionsAndMenu);
}

bool features_quick_swap(GB *gb) {
  if (!features.quick_swap || !can_change_items(gb)) return false;
  uint8_t b = W8(wInventoryB), a = W8(wInventoryA);
  if (a == b) return false;     // both empty, or Biggoron's Sword filling both
  W8(wInventoryB) = a;
  W8(wInventoryA) = b;
  W8(wStatusBarNeedsRefresh) |= 0x01;
  return true;
}

// A menu opens and closes behind a fade to white (palette thread modes 1 and 2) that adds its speed
// to the fade offset each frame until $20; speed 8 ends it in 4 frames. Other fades have no menu open.
void features_frame(GB *gb) {
  if (!features.fast_menus || !W8(wOpenedMenuType)) return;
  uint8_t mode = W8(wPaletteThread_mode);
  if ((mode == 1 || mode == 2) && W8(wPaletteThread_speed) < 8) W8(wPaletteThread_speed) = 8;
}

#define MENU_INVENTORY 1
#define INVENTORY_CAPACITY 0x10

bool features_assign_slot(GB *gb, int slot) {
  if (!features.four_slots || W8(wOpenedMenuType) != MENU_INVENTORY || W8(wInventorySubmenu) != 0) return false;
  uint8_t item = mem_rd(gb, (uint16_t)(wInventoryStorage + W8(wInventorySubmenu0CursorPos)));
  if (!item) return false;
  if (features.slot_item[!slot] == item) features.slot_item[!slot] = 0;
  features.slot_item[slot] = item;
  return true;
}

static int held_slot = -1;
static uint16_t held_addr;
static uint8_t held_prev;
static bool held_refresh;                // the press redrew the bar, so the release must too

static bool in_storage(GB *gb, uint8_t item) {
  for (int i = 0; i < INVENTORY_CAPACITY; i++)
    if (mem_rd(gb, (uint16_t)(wInventoryStorage + i)) == item) return true;
  return false;
}

uint8_t features_slot_press(GB *gb, int slot) {
  uint8_t item = features.slot_item[slot];
  if (!features.four_slots || !item || held_slot >= 0) return 0;
  if (item == W8(wInventoryB)) return JOY_B;
  if (item == W8(wInventoryA)) return JOY_A;
  if (!can_change_items(gb) || !in_storage(gb, item)) return 0;
  // Ages' underwater rooms only read A (B is the swim kick), so the item borrows A there
  uint8_t flags = W8(wTilesetFlags);
  bool underwater = !game_seasons && (flags & 0x40) && !(flags & 0x20);
  held_slot = slot;
  held_addr = underwater ? wInventoryA : wInventoryB;
  held_prev = mem_rd(gb, held_addr);
  mem_wr(gb, held_addr, item);
  // Only redraw the bar when the item has nowhere else to be seen: with the X and Y boxes on screen
  // the player can already see what they pressed, and the B box should keep its own item.
  held_refresh = !features.slot_boxes;
  if (held_refresh) W8(wStatusBarNeedsRefresh) |= 0x01;
  return underwater ? JOY_A : JOY_B;
}

// HUD reads must not expose the temporary A/B assignment when X/Y have their own boxes.
// Read through rather than swapping RAM around a draw: rendering can yield to other threads.
uint8_t features_hud_item(GB *gb, uint16_t addr) {
  uint8_t item = mem_rd(gb, addr);
  if (held_slot >= 0 && !held_refresh && addr == held_addr && item == features.slot_item[held_slot])
    return held_prev;
  return item;
}

void features_slot_release(GB *gb, int slot) {
  if (held_slot != slot) return;
  held_slot = -1;
  if (mem_rd(gb, held_addr) != features.slot_item[slot]) return;
  mem_wr(gb, held_addr, held_prev);
  if (held_refresh) W8(wStatusBarNeedsRefresh) |= 0x01;
}

static uint8_t rom_at(GB *gb, uint8_t bank, uint16_t addr) {
  size_t at = (size_t)bank * 0x4000 + (addr & 0x3fff);
  return at < gb->rom_size ? gb->rom[at] : 0;
}

void features_treasure_object(GB *gb, uint8_t id, uint8_t subid, uint8_t out[4]) {
  uint8_t bank = SYMBANK(treasureObjectData);
  uint16_t at = (uint16_t)(SYM(treasureObjectData) + id * 4);
  if (rom_at(gb, bank, at) & 0x80) at = (uint16_t)((rom_at(gb, bank, at + 1) | rom_at(gb, bank, at + 2) << 8) + subid * 4);
  for (int i = 0; i < 4; i++) out[i] = rom_at(gb, bank, (uint16_t)(at + i));
}

void features_treasure_loaded(GB *gb, uint16_t obj) {
  if (!features.treasure_swap) return;
  uint8_t new_id, new_subid, t[4];
  if (!features.treasure_swap(W8(wActiveGroup), W8(wActiveRoom), mem_rd(gb, (uint16_t)(obj + OBJ_VAR30)),
                              mem_rd(gb, (uint16_t)(obj + OBJ_VAR03)), &new_id, &new_subid)) return;
  features_treasure_object(gb, new_id, new_subid, t);
  mem_wr(gb, (uint16_t)(obj + OBJ_VAR03), new_subid);
  mem_wr(gb, (uint16_t)(obj + OBJ_VAR30), new_id);
  mem_wr(gb, (uint16_t)(obj + OBJ_VAR32), t[0] & 0x07);
  mem_wr(gb, (uint16_t)(obj + OBJ_VAR34), t[1]);
  mem_wr(gb, (uint16_t)(obj + OBJ_VAR35), t[2]);
  mem_wr(gb, (uint16_t)(obj + OBJ_SUBID), t[3]);
  mem_wr(gb, (uint16_t)(obj + OBJ_VAR38), 0);
}
