#pragma once
// Optional quality-of-life behaviour, set by the app from the player's settings. Everything is
// off by default and in every test and movie; with a flag off the game code runs exactly as the ROM.
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool fast_text;               // each line of a text box appears at once
  bool quick_swap;              // a button swaps the A and B items
  bool fast_menus;              // the fades into and out of the inventory and map take 4 frames
  bool four_slots;              // two more item buttons, X and Y
  uint8_t slot_item[2];         // the items on X and Y (0: none), assigned in the inventory
  bool slot_boxes;              // their own boxes are on screen, so leave the B box alone
  // set by the app for mods: the treasure (id, subid) loading in (group, room) becomes
  // (*new_id, *new_subid) when this returns true
  bool (*treasure_swap)(int group, int room, uint8_t id, uint8_t subid, uint8_t *new_id, uint8_t *new_subid);
} Features;

extern Features features;

struct GB;
// Swaps the A and B items when the game would let the player open the inventory to do it (no menu
// or text box open, menus enabled); returns whether it did. The status bar redraws the items.
bool features_quick_swap(struct GB *gb);
// Called by the app before each frame: applies the toggles that act on the game's state.
void features_frame(struct GB *gb);
// With the inventory's item page open: puts the highlighted item on X (slot 0) or Y (slot 1).
bool features_assign_slot(struct GB *gb, int slot);
// In play, pressing X/Y: returns the game buttons to hold. For an item on neither A nor B this puts
// it on B for as long as the button is held; features_slot_release puts B's own item back.
uint8_t features_slot_press(struct GB *gb, int slot);
void features_slot_release(struct GB *gb, int slot);
// The item shown on A/B, hiding an extra slot's temporary assignment when it has its own box.
uint8_t features_hud_item(struct GB *gb, uint16_t addr);
// The 4 treasureObjectData bytes of (id, subid), read from the ROM.
void features_treasure_object(struct GB *gb, uint8_t id, uint8_t subid, uint8_t out[4]);
// Called by interactionLoadTreasureData with the treasure object: applies treasure_swap, keeping
// the object's spawn mode and room-flag bit (the location's) and taking the rest from the new item.
void features_treasure_loaded(struct GB *gb, uint16_t obj);
