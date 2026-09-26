#pragma once
// Optional quality-of-life behaviour, set by the app from the player's settings. Everything is
// off by default and in every test and movie; with a flag off the game code runs exactly as the ROM.
#include <stdbool.h>

typedef struct {
  bool fast_text;               // each line of a text box appears at once
  bool quick_swap;              // a button swaps the A and B items
  bool fast_menus;              // the fades into and out of the inventory and map take 4 frames
  bool four_slots;              // two more item buttons, X and Y
  uint8_t slot_item[2];         // the items on X and Y (0: none), assigned in the inventory
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
