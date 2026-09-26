#pragma once
// Widescreen: a 256x144 frame built from a frame sample, read-only. The game's own 160 pixels go in
// the middle unchanged; the 48 pixels each side come from the room's tile map around the camera,
// or the border colour where there is nothing to show (menus, cutscenes, no room there).
#include "core/gb.h"
#include <stdbool.h>
#include <stdint.h>

#define WIDE_W 256
#define WIDE_SIDE ((WIDE_W - FB_W) / 2)

typedef struct {
  bool seasons;
  bool dim;                     // the side strips at 75% brightness
  uint8_t border[3];            // RGB where the strips show nothing
} WideOptions;

// out: WIDE_W x FB_H RGB24.
void wide_render(const GBSample *s, const WideOptions *o, uint8_t *out);
// The room's background pixel (RGB555) at screen (x, y), x from -48 to 207, or -1 where the room is
// not shown there. Inside 0-159 it matches the PPU's background wherever no sprite covers it.
int wide_room_pixel(const GBSample *s, bool seasons, int x, int y);
bool wide_window_at(const GBSample *s, int x, int y);
