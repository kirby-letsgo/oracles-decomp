#pragma once
#include "game/asm.h"
#include "game/ram.h"

#define IO_IF 0xff0f
#define IO_LCDC 0xff40
#define IO_LY 0xff44
#define IO_VBK 0xff4f
#define IO_SVBK 0xff70
#define IO_IE 0xffff
#define MBC_ROM_BANK 0x2222

#define flagLocationGroupTable 0x09cc
#define speedTable_bank3 0x404b
#define linkDirectionOffsets 0x1cf0
#define hazardCollisionTable 0x2408
#define rupeeValues 0x1791
#define specialCollisions_allowHoles 0x14e8
#define specialCollisions_disallowHoles 0x1519
#define specialCollisions_disallowSmallBridges 0x1535
#define bitTable 0x00f8
#define chestDataGroupTable_bank16 0x5108
#define tileUpdateRoomFlagsOnBreakTable 0x1184
#define tileIncreaseGashaMaturityOnBreakTable 0x11c1
#define adjacentRoomsData 0x122e
#define ROM_addToGashaMaturity 0x1821
#define ROM_checkTreasureObtained 0x1748
#define ROM_b3f_decideItemDrop_body 0x4744
#define ROM_b3f_checkItemDropAvailable_body 0x4782
#define ROM_b3f_giveTreasure_body 0x44c8
#define ROM_b3f_loseTreasure_body 0x44a1
#define ringTierTable_bank3f 0x4792
#define ROM_getFreeInteractionSlot 0x3aef
#define ROM_getTileMappingData 0x3a72
#define ROM_setTile 0x3a9c
#define ROM_playSound 0x0c98
#define ROM__interactionActuallyRunScript 0x2518
#define ROM__interactionSaveScriptAddress 0x2588
#define ROM_b07_checkEnemyAndPartCollisions 0x41d1
#define ROM_b05_specialObjectUpdatePositionGivenVelocity 0x5d9f
#define interactionAnimationTable_bank16 0x5855
#define adjacentTileDirOffsets 0x2738
#define OBJ_COLLIDE_RADIUS_Y 0x26
#define ROM_getActiveRoomFromDungeonMapPosition 0x2e12
#define ROM_getRoomInDungeon 0x2e15
#define ROM_loadUncompressedGfxHeader 0x05da
#define ROM_b06_showInfoTextForTile 0x42fb
#define ROM_b01_updateCameraPosition 0x4201
#define ROM_b01_updateGfxRegs2Scroll 0x42cb
#define ROM_b01_updateScreenShake 0x427d
#define ROM_b01_calculateCameraPosition 0x4256
#define ROM_b01_setScreenTransitionState02 0x40f5
#define ROM_b01_initWaveScrollValues_body 0x477e
#define ROM_b01_loadBigBufferScrollValues_body 0x47da

#define OBJ_STATE 0x04
#define OBJ_SUBSTATE 0x05
#define OBJ_COUNTER1 0x06
#define OBJ_COUNTER2 0x07
#define OBJ_Y 0x0a
#define OBJ_YH 0x0b
#define OBJ_XH 0x0d
#define OBJ_ZH 0x0f
#define OBJ_VISIBLE 0x1a
#define OBJ_COLLISION_RADIUS_Y 0x26
#define OBJ_VAR2A 0x2a
#define OBJ_ANGLE 0x09
#define OBJ_Z 0x0e
#define OBJ_SPEED 0x10
#define OBJ_SPEED_Z 0x14
#define OBJ_RELATED1 0x16
#define OBJ_RELATED2 0x18
#define INTERACTION_BASE 0x40
#define ENEMY_SLOTS 0xd080
#define PART_SLOTS 0xd0c0

void burn_rom(GB *gb, int bank, uint16_t from, uint16_t to, bool last_taken);
#define CYC(from, to) burn_rom(gb, 0, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0, (from), (to), true)
#define CALL_ROM(a, target) do { CYC((a), (a) + 3); push_effect(gb, (uint16_t)((a) + 3)); asm_call(gb, (target), (uint16_t)((a) + 3)); } while (0)

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
