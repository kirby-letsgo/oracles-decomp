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
static uint8_t held_b;

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
  held_slot = slot;
  held_b = W8(wInventoryB);
  W8(wInventoryB) = item;
  W8(wStatusBarNeedsRefresh) |= 0x01;
  return JOY_B;
}

void features_slot_release(GB *gb, int slot) {
  if (held_slot != slot) return;
  held_slot = -1;
  if (W8(wInventoryB) != features.slot_item[slot]) return;
  W8(wInventoryB) = held_b;
  W8(wStatusBarNeedsRefresh) |= 0x01;
}
