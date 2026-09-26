#pragma once
// A room decoded from the ROM alone, as the game's room loader would build it (widescreen shows
// the neighbouring rooms this way): the tile map with attributes, the tileset's BG tile graphics
// and its BG palettes. Changes that depend on game flags (opened chests, cut bushes, switches) are
// not applied: a room looks as the ROM describes it.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  int width, height;            // in 8x8 tiles: 20x16 small, 30x22 large
  uint8_t flags;                // the tileset flags (dungeon, indoors, sidescroll, ...)
  uint8_t layout[0xc0];         // metatiles, 16 a row
  uint8_t tiles[0x300], attrs[0x300];   // room-local tiles and CGB attributes, 32 a row
  uint8_t vram[2][0x1800];      // $8000-$97ff of both VRAM banks as the tileset loads them
  bool vram_set[2][0x1800 / 16]; // which tiles the tileset wrote (the rest is the game's common gfx)
  bool animated[2][0x1800 / 16]; // which of them the room's tile animations change over time
  uint8_t bg_pal[64];
  bool pal_set[8];              // which BG palettes the tileset loaded (2-7, some unique-gfx ones)
} WideRoom;

typedef struct {
  int group, room;
  int season;                   // Seasons: 0 spring .. 3 winter (wRoomStateModifier)
  uint8_t room_flags;           // the room's flag byte (Ages: bit 0 swaps the layout for the underwater one)
} WideRoomKey;

bool wide_decode_room(const uint8_t *rom, size_t size, bool seasons, const WideRoomKey *key, WideRoom *out);
