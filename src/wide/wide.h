#pragma once
// Widescreen: a 256x144 frame built from a frame sample, read-only. The game's own 160 pixels go in
// the middle unchanged; the 48 pixels each side come from the room around the camera, past its
// edges from the neighbouring rooms (decoded from the ROM: the overworld grid, and in dungeons the
// visited rooms through a doorway), or the border colour where there is nothing to show (menus,
// cutscenes, houses and caves, the map's edge).
#include "core/gb.h"
#include <stdbool.h>
#include <stdint.h>

#define WIDE_W 256
#define WIDE_SIDE ((WIDE_W - FB_W) / 2)

typedef struct {
  bool seasons;
  bool dim;                     // the side strips at 75% brightness
  uint8_t border[3];            // RGB where the strips show nothing
  const uint8_t *rom;           // for the neighbouring rooms (the app's copy, code bytes zeroed, will do)
  size_t rom_size;
  bool slots;                   // 4 slots is on: the X and Y boxes take the left of the status bar
  uint8_t slot_item[2];         // what is on each of them (0: the box is empty)
} WideOptions;

// out: WIDE_W x FB_H RGB24.
void wide_render(const GBSample *s, const WideOptions *o, uint8_t *out);
// The room's background pixel (RGB555) at screen (x, y), x from -48 to 207, or -1 where the room is
// not shown there. Inside 0-159 it matches the PPU's background wherever no sprite covers it.
int wide_room_pixel(const GBSample *s, bool seasons, int x, int y);
bool wide_window_at(const GBSample *s, int x, int y);
