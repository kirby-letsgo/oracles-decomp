#include "game/game.h"
#include "game/features.h"

Features features;

// A held extra button owns its native input until release. addr is nonzero only
// when the item temporarily replaces the item in that hand.
typedef struct {
  uint8_t button, item, previous;
  uint16_t addr;
  bool refresh;
} HeldItem;
static HeldItem held_items[4];

static uint8_t held_buttons(void) {
  uint8_t buttons = 0;
  for (int i = 0; i < 4; i++) buttons |= held_items[i].button;
  return buttons;
}

// the moments the player could open the inventory and change the buttons there
static bool can_change_items(GB *gb) {
  return !W8(wMenuDisabled) && !W8(wOpenedMenuType) && !W8(wTextIsActive) && !W8(wDisableLinkCollisionsAndMenu);
}

bool features_quick_swap(GB *gb) {
  if (!features.quick_swap || held_buttons() || !can_change_items(gb)) return false;
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

static void equip_extra(int slot, uint8_t item) {
  for (int i = 0; i < 4; i++)
    if (features.slot_item[i] == item) features.slot_item[i] = 0;
  features.slot_item[slot] = item;
}

bool features_assign_slot(GB *gb, int slot) {
  if (slot < 0 || slot >= features.extra_slots || W8(wOpenedMenuType) != MENU_INVENTORY || W8(wInventorySubmenu) != 0) return false;
  uint8_t item = mem_rd(gb, (uint16_t)(wInventoryStorage + W8(wInventorySubmenu0CursorPos)));
  if (!item) return false;
  equip_extra(slot, item);
  return true;
}

static bool in_storage(GB *gb, uint8_t item) {
  for (int i = 0; i < INVENTORY_CAPACITY; i++)
    if (mem_rd(gb, (uint16_t)(wInventoryStorage + i)) == item) return true;
  return false;
}

bool features_touch_available(GB *gb) {
  return !held_buttons() && can_change_items(gb);
}

// Companion screen slots are A, B, X, Y. Only the game thread changes inventory.
bool features_touch_equip(GB *gb, int slot, uint8_t item) {
  if (slot < 0 || slot >= 2 + features.extra_slots || !item || item >= 0x20 || !features_touch_available(gb)) return false;
  uint8_t *a = &W8(wInventoryA), *b = &W8(wInventoryB);
  uint8_t *stored = &W8(wInventoryStorage);
  if (item != *a && item != *b && !in_storage(gb, item)) return false;
  if (slot >= 2) {
    equip_extra(slot - 2, item);
    return true;
  }
  uint8_t *target = slot == 0 ? a : b, *other = slot == 0 ? b : a;
  if (*target == item) return true;
  if (*other == item) {
    *other = *target;
  } else {
    int pos = 0;
    while (pos < INVENTORY_CAPACITY && stored[pos] != item) pos++;
    if (pos == INVENTORY_CAPACITY) return false;
    if (item == 0x0c) { // Biggoron's Sword needs both hands. Stow both displaced items.
      int empty = pos;
      if (*target && *other) {
        for (empty = 0; empty < INVENTORY_CAPACITY && stored[empty]; empty++) {}
        if (empty == INVENTORY_CAPACITY) return false;
      }
      stored[pos] = *target;
      if (*other) stored[empty] = *other;
      *other = item;
    } else {
      stored[pos] = *target;
      if (*target == 0x0c) *other = 0;
    }
  }
  *target = item;
  W8(wStatusBarNeedsRefresh) |= 1;
  return true;
}

bool features_extra_shield_equipped(GB *gb) {
  for (int slot = 0; slot < features.extra_slots; slot++) {
    if (features.slot_item[slot] == ITEM_SHIELD && in_storage(gb, ITEM_SHIELD)) return true;
  }
  // A native shield may itself be temporarily covered by a borrowed item.
  for (int slot = 0; slot < 4; slot++) {
    const HeldItem *held = &held_items[slot];
    if (held->addr && held->previous == ITEM_SHIELD && mem_rd(gb, held->addr) == held->item) return true;
  }
  return false;
}

uint8_t features_slot_press(GB *gb, int slot, uint8_t native_buttons) {
  if (slot < 0 || slot >= features.extra_slots) return 0;
  uint8_t item = features.slot_item[slot];
  if (!item || held_items[slot].button) return held_items[slot].button;
  uint8_t a = W8(wInventoryA), b = W8(wInventoryB);
  if (item == b || item == a) {
    held_items[slot].button = item == b ? JOY_B : JOY_A;
    return held_items[slot].button;
  }
  if (!can_change_items(gb) || !in_storage(gb, item)) return 0;

  uint8_t flags = W8(wTilesetFlags);
  bool only_a = !game_seasons && (flags & 0x40) && !(flags & 0x20);
  bool only_b = (flags & 0x20) && W8(wLinkSwimmingState) &&
                (game_seasons || !(W8(w1Link_var2f) & 0x80));
  uint8_t available = JOY_A | JOY_B;
  if (only_a) available = JOY_A;
  if (only_b) available = JOY_B;
  available &= ~(native_buttons | held_buttons());
  if (!available) return 0;

  // Leave the other half of a sword/shield pair usable even if it is pressed second.
  bool keep_b = (item == ITEM_SHIELD && b == ITEM_SWORD) || (item == ITEM_SWORD && b == ITEM_SHIELD);
  uint8_t button = JOY_B;
  if (!(available & JOY_B) || (keep_b && (available & JOY_A))) button = JOY_A;
  uint16_t addr = button == JOY_A ? wInventoryA : wInventoryB;
  held_items[slot] = (HeldItem){.button = button, .item = item, .addr = addr,
                                .previous = mem_rd(gb, addr), .refresh = !features.slot_boxes};
  mem_wr(gb, addr, item);
  if (held_items[slot].refresh) W8(wStatusBarNeedsRefresh) |= 1;
  return button;
}

uint8_t features_hud_item(GB *gb, uint16_t addr) {
  uint8_t item = mem_rd(gb, addr);
  for (int slot = 0; slot < 4; slot++) {
    const HeldItem *held = &held_items[slot];
    if (held->addr == addr && !held->refresh && held->item == item) return held->previous;
  }
  return item;
}

void features_slot_release(GB *gb, int slot) {
  if (slot < 0 || slot >= 4) return;
  HeldItem held = held_items[slot];
  held_items[slot] = (HeldItem){0};
  if (!held.addr || mem_rd(gb, held.addr) != held.item) return;
  mem_wr(gb, held.addr, held.previous);
  if (held.refresh) W8(wStatusBarNeedsRefresh) |= 1;
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
