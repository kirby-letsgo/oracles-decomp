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

// The sprites of a room the camera is not in, in room coordinates (pixels from the room's top-left):
// RGB555, or -1 where there is none; behind: the sprite goes behind the background's colours 1-3.
typedef struct {
  int group, room;
  int16_t c[176][256];
  bool behind[176][256];
} WideRoomSprites;

typedef struct {
  bool seasons;
  bool dim;                     // the side strips at 75% brightness
  uint8_t border[3];            // RGB where the strips show nothing
  const uint8_t *rom;           // for the neighbouring rooms (the app's copy, code bytes zeroed, will do)
  size_t rom_size;
  bool slots;                   // 4 slots is on: the X and Y boxes take the left of the status bar
  uint8_t slot_item[2];         // what is on each of them (0: the box is empty)
  // the sprites of a neighbouring room, NULL where none are known (wide/npcs.h)
  const WideRoomSprites *(*room_sprites)(int group, int room);
  uint8_t glyph[4];             // the four slots' letters, X Y B A (ui/labels.h codes; 0: the game's own)
  bool compact;                 // 16:10 viewport
} WideOptions;

// out: (compact ? UI_W_16_10 : WIDE_W) x FB_H RGB24.
void wide_render(const GBSample *s, const WideOptions *o, uint8_t *out);
enum { WIDE_STRIP_W = 16, WIDE_BOX_W = 38 };
// Item X/Y beside the narrow picture: slot 0 (X) or 1 (Y) as an RGB24 strip FB_H tall, either the
// widescreen box (WIDE_BOX_W wide, as tall as the status bar) or, where that does not fit, the
// letter above the icon (WIDE_STRIP_W wide). False (out untouched) unless o->slots is on and the
// status bar is up and not blanked to one colour.
bool wide_slot_strip(const GBSample *s, const WideOptions *o, int slot, bool box, uint8_t *out);
// The room's background pixel (RGB555) at screen (x, y), x from -48 to 207, or -1 where the room is
// not shown there. Inside 0-159 it matches the PPU's background wherever no sprite covers it.
int wide_room_pixel(const GBSample *s, bool seasons, int x, int y);
// The first screen x the window layer covers on line y (it starts at WX - 7 and runs to the right
// edge), INT_MAX when it is off.
int wide_window_left(const GBSample *s, int y);
bool wide_window_at(const GBSample *s, int x, int y);
// The pixels of line y across the wide picture (x from -48) of the engine's off-screen sprite list
// alone (wide/npcs.c's copies list the room's objects there), RGB555 or -1, with the
// behind-background flag.
void wide_sprite_row(const GBSample *s, int y, int16_t c[WIDE_W], bool behind[WIDE_W]);
// The room-local position of screen pixel (0, y) in the room the camera is in, false outside play.
bool wide_room_origin(const GBSample *s, bool seasons, int y, int *rx, int *ry);
// The room pixel at the top-left of the play area (screen (0, 16), under the status bar), or false
// outside normal play (menus, scripted scenes, screen transitions, sidescrolling areas).
bool wide_play_origin(const GBSample *s, bool seasons, int *rx, int *ry);
