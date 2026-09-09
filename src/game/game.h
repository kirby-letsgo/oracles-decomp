#pragma once
#include "game/asm.h"
#include "game/ram.h"

#define IO_IF 0xff0f
#define IO_LCDC 0xff40
#define IO_LY 0xff44
#define IO_VBK 0xff4f
#define IO_IE 0xffff
#define MBC_ROM_BANK 0x2222

#define flagLocationGroupTable 0x09cc

#define OBJ_STATE 0x04
#define OBJ_SUBSTATE 0x05
#define OBJ_COUNTER1 0x06
#define OBJ_COUNTER2 0x07
#define OBJ_Y 0x0a
#define OBJ_YH 0x0b
#define OBJ_VISIBLE 0x1a
#define INTERACTION_BASE 0x40

void burn_rom(GB *gb, int bank, uint16_t from, uint16_t to, bool last_taken);
#define CYC(from, to) burn_rom(gb, 0, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0, (from), (to), true)

static inline uint16_t rd16(GB *gb, uint16_t a) { return (uint16_t)(mem_rd(gb, a) | (mem_rd(gb, a + 1) << 8)); }
static inline void wr16(GB *gb, uint16_t a, uint16_t v) { mem_wr(gb, a, (uint8_t)v); mem_wr(gb, a + 1, v >> 8); }
static inline int bitlen8(uint8_t v) { int n = 0; while (v) { n++; v >>= 1; } return n; }
static inline int popcount8(uint8_t v) { int n = 0; while (v) { n += v & 1; v >>= 1; } return n; }

void fillMemory(GB *gb, uint16_t dst, uint8_t value, unsigned n);
void clearMemory(GB *gb, uint16_t dst, unsigned n);
void fillMemoryBc(GB *gb, uint16_t dst, uint8_t value, unsigned n);
void clearMemoryBc(GB *gb, uint16_t dst, unsigned n);
void copyMemory(GB *gb, uint16_t dst, uint16_t src, unsigned n);
void copyMemoryBc(GB *gb, uint16_t dst, uint16_t src, unsigned n);
uint8_t getRandomNumber(GB *gb);
uint8_t getRandomNumber_noPreserveVars(GB *gb);
void clearOam(GB *gb);
bool disableLcd(GB *gb);
void clearVram(GB *gb);
void initializeVramMap0(GB *gb);
void initializeVramMap1(GB *gb);
void initializeVramMaps(GB *gb);
void setStatusBarNeedsRefreshBit1(GB *gb);
