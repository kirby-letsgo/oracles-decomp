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
  uint8_t tileset[8];           // the tileset entry (b3 gfx, b4 palettes); unique: its unique gfx
  int unique;
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
  int tileset_add;              // Ages checkTilesetOverride: added to the room's tileset index
  bool remains_lava;            // Seasons: Temple Remains filled with lava (global flag $15)
  bool keep_destroyed;          // Seasons: Moblin's Keep destroyed (global flag $16)
  int companion;                // Seasons: wAnimalCompanion, which picks the destroyed keep's tileset
  int language;                 // European ROMs: wActiveLanguage, for the gfx that differ by language
} WideRoomKey;

// The ROM's region comes from its header (European ROMs: game code AZ8P, AZ7P).
bool wide_decode_room(const uint8_t *rom, size_t size, bool seasons, const WideRoomKey *key, WideRoom *out);

// The games keep a separate minimap room to remember the entrance to interiors.
// Plane 0 is Holodrum/present Labrynna, 1 is Subrosia/past Labrynna, 2 is a dungeon.
typedef struct {
  bool seasons;
  uint8_t plane, room, companion, floodgates, ship_docked, symmetry_restored, water_shifted;
  uint8_t visited[256];
} OverworldMap;
void overworld_map_state(bool seasons, const uint8_t wram[4096], OverworldMap *map);
int overworld_map_width(const OverworldMap *map);
int overworld_map_height(const OverworldMap *map);
// Draws the map grid into a tightly packed ARGB buffer, at most 128x128.
void overworld_map_pixels(const uint8_t *rom, size_t size, const OverworldMap *map, uint32_t *pixels);

enum { DUNGEON_MAX_FLOORS = 8, DUNGEON_CELLS = 64, DUNGEON_FLOOR_PIXELS = 64 * 64 };
typedef struct {
  uint8_t index, floors, base_floor, visible, link_floor, link_cell;
  uint8_t tiles[DUNGEON_MAX_FLOORS][DUNGEON_CELLS];
} DungeonMap;

void dungeon_map_state(bool seasons, const uint8_t *rom, size_t size,
                       const uint8_t wram[4096], const uint8_t bank2[4096], DungeonMap *map);
void dungeon_map_pixels(bool seasons, const uint8_t *rom, size_t size,
                        const DungeonMap *map, uint32_t *pixels);

// The inventory menu loads its own icon tiles and fixed palettes, independent of the room.
typedef struct {
  uint8_t tiles[0x800], palettes[64];
} InventoryArt;
void inventory_load_art(const uint8_t *rom, size_t size, bool seasons, InventoryArt *art);
