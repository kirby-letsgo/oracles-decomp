#pragma once
// An item's status-bar icon, decoded from the ROM alone, the way the game builds it:
// treasureAndDrops.s loadTreasureDisplayData picks the record (an item with levels or selected
// ammo has one per variant), and bank2.s loadItemIconGfx reads the tiles out of spr_item_icons_1.
// Widescreen uses this to show the X and Y items beside the status bar, which the game never draws.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  int halves;                   // 1 (the icon is 8 wide) or 2 (16 wide)
  uint8_t ci[16][16];           // colour index 0-3 per pixel, [y][x]
  uint8_t pal[2];               // the BG palette each half is drawn with
} WideItemIcon;

// `c600` is the game's $c600 page: the entries name a byte in it (a level, or which seeds are
// selected) that picks the variant. Returns false for an item with no icon.
bool wide_item_icon(const uint8_t *rom, size_t size, bool seasons, uint8_t item,
                    const uint8_t *c600, WideItemIcon *out);
