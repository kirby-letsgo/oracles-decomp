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
#define linkAngleToDirectionButtons 0x2a15
#define wallCollisionDirections 0x2b55
#define enemyAnimationTable_bank0d 0x6d5c
#define partAnimationTable_bank16 0x7668
#define dungeonRoomPropertiesGroupTable_bank01 0x4dca
#define ROM_getFreePartSlot 0x3e8e
#define ROM_markEnemyAsKilledInRoom 0x320d
#define ROM_checkGlobalFlag 0x31f3
#define ROM_setGlobalFlag 0x31f9
#define ROM_b0b_objectCreateFloatingImage 0x4085
#define ROM_b11_createEnergySwirlGoingIn_body 0x5df4
#define ROM_b11_createEnergySwirlGoingOut_body 0x5df0
#define ROM_b06_specialObjectNextAnimationFrame 0x4432
#define ROM_b06_specialObjectSetAnimation_body 0x4427
#define ROM_b03_clearFadingPalettes_body 0x525a
#define ROM_b03_flashScreen_body 0x522e
#define ROM_b01_loadDungeonLayout_b01 0x564e
#define ROM_b06_functionCaller 0x4870
#define ROM_b06_tryToBreakTile_body 0x4734
#define wPaletteFadeOffset 0xc2ff
#define dungeonsUsingToggleBlocks 0x3657
#define objectTable2_bank12 0x76dd
#define ROM_b02_getIndexOfGashaSpotInRoom_body 0x7a54
#define ROM_b04_setInterleavedTile_body 0x6cb3
#define ROM_b09_checkNpcShouldExistAtGameStage_body 0x55a6
#define wInteractionSlots (w1ReservedInteraction0 + 0x200)

#define musicAssignmentGroupTable_bank04 0x495c
#define roomPackData_bank04 0x475c
#define animationGroupTable_bank04 0x5b52
#define uniqueGfxHeaderTable_bank04 0x5b28
#define ROM_b04_applyWarpDest_b04 0x45d0
#define ROM_b05_companionDismount 0x4630
#define ROM_b05_saveLinkLocalRespawnAndCompanionPosition 0x467c
#define ROM_loadTilesetData 0x3889
#define ROM_loadTilesetGraphics 0x3796
#define ROM_loadTilesetAndRoomLayout 0x38a5
#define ROM_loadUniqueGfxHeaderEntry 0x3843
#define IO_P1 0xff00
#define IO_HDMA1 0xff51
#define IO_TIMA 0xff05
#define IO_TAC 0xff07
#define IO_KEY1 0xff4d
#define interactionAnimationFrameTable_bank16 0x5a23
#define enemyAnimationFrameTable_bank0d 0x6e5c
#define partAnimationFrameTable_bank16 0x771e
#define ROM_jpBc 0x0cda
#define ROM_jpHl 0x00a0
#define ROM_updateEnemy 0x2f07
#define ROM_enemyStandardUpdate 0x28fd
#define ROM_updateInteraction 0x3b62
#define ROM_updateEnemies 0x2ea5
#define ROM_updateInteractions 0x3b36
#define ROM_setEnemyTargetToLinkPosition 0x3616
#define ROM_updateCamera 0x12ae
#define ROM_drawAllSprites 0x0d9a
#define ROM_loadTileset 0x0799
#define ROM_loadTilesetLayout 0x3712
#define ROM_loadTilesetLayout_helper 0x373b
#define ROM_loadRoomLayout 0x38dc
#define ROM_copyMemoryReverse 0x047f
#define ROM_refreshObjectGfx 0x1618
#define ROM_checkSpawnTimeportalInteraction 0x321d
#define interactionCodeTable 0x3b8b
#define ROM_b05_updateSpecialObjects 0x4000
#define ROM_b07_updateItems 0x4872
#define ROM_b11_updateParts 0x5e58
#define ROM_b01_func_4000 0x4000
#define ROM_b05_func_410d 0x410d
#define ROM_b06_updateGrabbedObjectPosition 0x54df
#define ROM_b07_updateItemsPost 0x491a
#define ROM_b01_checkUpdateFollowingLinkObject 0x494d
#define ROM_b04_updateChangedTileQueue 0x6c32
#define ROM_b04_updateAnimations 0x5906
#define ROM_b04_setPastCliffPalettesToRed 0x6e63
#define ROM_b04_initializeAnimations 0x58e4
#define ROM_b02_func_02_7a77 0x7a77
#define ROM_b02_checkLoadPastSignAndChestGfx 0x7dc7
#define ROM_b04_loadTilesetData_body 0x6d7a
#define ROM_b02_updateTilesetFlagsForIndoorRoomInAltWorld 0x403e
#define ROM_b04_applyAllTileSubstitutions 0x5fef
#define ROM_b01_clearSolidObjectPositions 0x62da
#define ROM_b02_calculateRoomStateModifier 0x79e2
#define ROM_b12_runRoomSpecificCode 0x5872
#define ROM_b02_createSeaEffectsPartIfApplicable 0x7a12
#define ROM_b01_checkLoadPirateShip 0x7de1
#define ROM_b02_loadRememberedCompanion 0x768a
#define ROM_b02_checkAndSpawnMaple 0x76d3
#define ROM_b12_parseObjectData 0x55b7
#define ROM_b16_parseStaticObjects 0x5015
#define ROM_b16_loadStaticObjects_body 0x5085
#define ROM_b03_disableLcdAndLoadRoom_body 0x5fe5
#define ROM_b10_playWaveSoundAtRandomIntervals_body 0x7328
#define ROM_b03_func_03_7841 0x7841
#define ROM_b03_func_03_7849 0x7849
#define ROM_b03_endgameCutsceneHandler_body 0x5414
#define ROM_b02_fileSelect_redrawDecorationsAndSetWramBank4 0x4cd7
#define ROM_b06_loadLinkAndCompanionAnimationFrame_body 0x44c9
#define data_5951_bank3f 0x5951
#define ROM_pushDirectionData 0x1f05
#define ROM_b02_functionCaller 0x77b2
#define ROM_b09_objectOscillateZ_body 0x492d
#define ROM_b0b_objectCreateExclamationMark_body 0x406d
#define wildTokayObjectTable_bank12 0x78bd
#define objectTable1_bank12 0x5482
#define linkInteractPositionOffsets 0x1bd3
#define ROM_threadStop 0x089b
#define ROM_threadRestart 0x08a3
#define ROM_clearAllParentItems 0x2c10
#define ROM_dropLinkHeldItem 0x2c43
#define ROM_clearMemory 0x046f
#define ROM_putLinkOnGround 0x2a8c
#define ROM_retrieveTextCharacter_func_18fd 0x18fd
#define ROM_b3f_interactionLoadGraphics 0x4404
#define ROM_b3f_refreshObjectGfx_body 0x4154
#define ROM_b3f_loadObjectGfxHeaderToSlot4_body 0x41ec
#define ROM_b3f_loadTreeGfx_body 0x41f5
#define textGfxSourceTable 0x18f7
#define gfx_font_start_bank1c 0x4000
#define gfx_font_jp_bank1c 0x4720
#define wcc20 0xcc20
#define ROM_b01_initializeRoomBoundaryAndLoadAnimations 0x4027
#define ROM_b04_func_04_6ed1 0x6ed1
#define ROM_b04_func_04_6f31 0x6f31
#define ROM_b03_secretFunctionCaller_body 0x4836
#define ROM_b02_updateMenus 0x4fcf
#define ROM_b02_runBank2Function 0x4f17
#define gfxRegisterStates 0x0306
#define vblankDmaFunctionOffset 0x0a8d
#define uncmpGfxHeaderTable_bank01 0x6744
#define gfxHeaderTable_bank01 0x69da
#define tilesetHeaderTable_bank01 0x787e
#define tilesetGfxHeaderTable_bank01 0x7870
#define ROM_loadTilesetHlpr 0x07fe
#define ROM_interBankCall 0x008a
#define ROM_loadPaletteHeader 0x050b
#define ROM_queueDmaTransfer 0x058a
#define ROM_decompressGraphics 0x0672
#define ROM_addSpritesToOam 0x0d5e
#define ROM_addSpritesToOam_withOffset 0x0d61
#define ROM_b02_applyRoomSpecificTileChangesAfterGfxLoad 0x7a88
#define ROM_b04_generateW3VramTilesAndAttributes 0x6bf1
#define ROM_b12_checkLinkCanSurface_isUnderwater 0x78e4
#define ROM_b0e_objectLoadMovementScript_body 0x6b2d
#define ROM_b0e_objectRunMovementScript_body 0x6b4c
#define creditsSpriteTable_bank16 0x4556
#define simpleScriptCommandTable 0x3dd7
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
#define CALL_ROM_CC(a, target) do { CYCT((a), (a) + 3); push_effect(gb, (uint16_t)((a) + 3)); asm_call(gb, (target), (uint16_t)((a) + 3)); } while (0)
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
