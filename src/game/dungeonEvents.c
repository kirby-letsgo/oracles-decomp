#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// verifyTiles data: b0 = tile index, then positions; $ff starts a new group, $00 ends.
#define subid01_tileData_bank08 0x4cdf
#define interaction21_subid05_tileData_bank08 0x4d69
#define interaction21_subid09_tileData_bank08 0x4dcb
#define interaction21_subid0f_tileData_bank08 0x4eee
#define interaction21_subid10_tileData_bank08 0x4f09
// The positions in a circle around the owl statue, $00-terminated.
#define interaction21_subid13_positionsToCheck_bank08 0x4f59
#define interaction21_subid14_tileData_bank08 0x4f72
#define interaction21_subid15_tileData_bank08 0x4f86

// objectData.* (bank $12) and mainScripts.* (bank $0c), referenced by address only.
#define moonlitGrotto_orb_bank12 0x77c3
#define moonlitGrotto_onOrbActivation_bank12 0x77c8
#define objectData77d4_bank12 0x77d4
#define moonlitGrotto_onArmosSwitchPressed_bank12 0x77da
#define moonlitGrottoScript_brokeCrystal_bank0c 0x4c8e
#define moonlitGrottoScript_brokeAllCrystals_bank0c 0x4c9e
#define moonlitGrotto_enableControlAfterBreakingCrystal_bank15 0x4fa3

// Part.* fields use the same layout as Interaction.* but from $c0.

void interaction21_subid01_hook(GB *gb);
void verifyTilesAndDropSmallKey_hook(GB *gb);
void interaction21_subid02_hook(GB *gb);
void interaction21_subid03_hook(GB *gb);
void interaction21_subid04_hook(GB *gb);
void interaction21_subid05_hook(GB *gb);
void interaction21_subid06_hook(GB *gb);
void interaction21_subid19_hook(GB *gb);
void interaction21_subid07_hook(GB *gb);
void setSwitch_hook(GB *gb);
void unsetSwitch_hook(GB *gb);
void interaction21_subid08_hook(GB *gb);
void interaction21_subid09_hook(GB *gb);
void interaction21_subid0a_hook(GB *gb);
void interaction21_subid0b_hook(GB *gb);
void interaction21_subid0c_hook(GB *gb);
void interaction21_subid0d_hook(GB *gb);
void interaction21_subid0e_hook(GB *gb);
void interaction21_subid0f_hook(GB *gb);
void interaction21_subid10_hook(GB *gb);
void interaction21_subid11_hook(GB *gb);
void spawnChestAndDeleteSelf_hook(GB *gb);
void interaction21_subid12_hook(GB *gb);
void interaction21_subid13_hook(GB *gb);
void interaction21_subid14_hook(GB *gb);
void interaction21_subid15_hook(GB *gb);
void interaction21_subid16_hook(GB *gb);
void setTileToStandardFloor_hook(GB *gb);
void setTileWithPuff_hook(GB *gb);
void createPuffAt_hook(GB *gb);
void interaction21_subid16_state1_hook(GB *gb);
void interaction21_subid17_hook(GB *gb);
void interaction21_subid18_hook(GB *gb);
void interactionDeleteAndRetIfItemFlagSet_hook(GB *gb);
void spawnSmallKeyFromCeiling_hook(GB *gb);
void verifyTiles_hook(GB *gb);
void makeTorchAtPositionTemporarilyLightable_hook(GB *gb);

static uint16_t dungeonEvents_jumpTable(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// interaction21_subid03@lightCubeTorches: mark the cube color as "lit" and play the torch
// sound. Reached only by static `call z`s from interaction21_subid03 itself.
static void dungeonEvents_lightCubeTorches(GB *gb) {
  CYC(0x4d24, 0x4d27); SET_HL(wRotatingCubeColor);
  CYC(0x4d27, 0x4d29); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x4d29, 0x4d2b); A = 0x72; // SND_LIGHTTORCH
  CYC(0x4d2b, 0x4d2e); playSound_b00_hook(gb);
}

// Body shared by subid06 (`jr ++`) and subid19 (fallthrough): set trigger 0 iff
// [wRotatingCubeColor] == B.
static void dungeonEvents_setTriggerIfCubeColorIs(GB *gb) {
  CYC(0x4d7e, 0x4d81); A = mem_rd(gb, wRotatingCubeColor);
  CYC(0x4d81, 0x4d82); alu_cp(gb, B);
  CYC(0x4d82, 0x4d84); A = 0x01;
  if (F & FZ) {
    CYCT(0x4d84, 0x4d86);
  } else {
    CYC(0x4d84, 0x4d86);
    CYC(0x4d86, 0x4d87); A = alu_dec8(gb, A);
  }
  CYC(0x4d87, 0x4d8a); mem_wr(gb, wActiveTriggers, A);
  CYC(0x4d8a, 0x4d8b); ret_effect(gb);
}

// D2: Verify a 2x2 floor pattern. Falls through into verifyTilesAndDropSmallKey.
void interaction21_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4cd2, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4cd5);
  CYC(0x4cd5, 0x4cd8); SET_HL(subid01_tileData_bank08);
  verifyTilesAndDropSmallKey_hook(gb);
}

void verifyTilesAndDropSmallKey_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4cd8, verifyTiles_hook, 0x5094, 0x4cdb);
  if (!(F & FZ)) {
    CYCT(0x4cdb, 0x4cdc); ret_effect(gb); return;
  }
  CYC(0x4cdb, 0x4cdc);
  CYC(0x4cdc, 0x4cdf); spawnSmallKeyFromCeiling_hook(gb);
}

// D2: Verify a floor tile is red to open a door
void interaction21_subid02_hook(GB *gb) {
  CYC(0x4ce7, 0x4cea); A = mem_rd(gb, wRoomLayout + 0x5a);
  CYC(0x4cea, 0x4cec); alu_cp(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(0x4cec, 0x4cee); A = 0x01;
  if (F & FZ) {
    CYCT(0x4cee, 0x4cf0);
  } else {
    CYC(0x4cee, 0x4cf0);
    CYC(0x4cf0, 0x4cf1); A = alu_dec8(gb, A);
  }
  CYC(0x4cf1, 0x4cf4); mem_wr(gb, wActiveTriggers, A);
  CYC(0x4cf4, 0x4cf5); ret_effect(gb);
}

// Light torches when a colored cube rolls into this position.
void interaction21_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4cf5, checkInteractionState_hook, 0x23fe, 0x4cf8);
  if (!(F & FZ)) {
    CYCT(0x4cf8, 0x4cfa); goto initialized;
  }
  CYC(0x4cf8, 0x4cfa);
  CALL_C(0x4cfa, interactionIncState_hook, 0x23e0, 0x4cfd);
  CALL_C(0x4cfd, objectGetTileAtPosition_hook, 0x1444, 0x4d00);
  CYC(0x4d00, 0x4d03); A = mem_rd(gb, wRotatingCubePos);
  CYC(0x4d03, 0x4d05); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4d05, 0x4d06); mem_wr(gb, DE, A);
  CYC(0x4d06, 0x4d07); alu_cp(gb, L);
  if (F & FZ) {
    CYCT(0x4d07, 0x4d0a); push_effect(gb, 0x4d0a); dungeonEvents_lightCubeTorches(gb);
  } else {
    CYC(0x4d07, 0x4d0a);
  }

initialized:
  CYC(0x4d0a, 0x4d0c); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4d0c, 0x4d0d); A = mem_rd(gb, DE);
  CYC(0x4d0d, 0x4d0e); B = A;
  CYC(0x4d0e, 0x4d11); A = mem_rd(gb, wRotatingCubePos);
  CYC(0x4d11, 0x4d12); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x4d12, 0x4d13); ret_effect(gb); return;
  }
  CYC(0x4d12, 0x4d13);
  CALL_C(0x4d13, objectGetTileAtPosition_hook, 0x1444, 0x4d16);
  CYC(0x4d16, 0x4d19); A = mem_rd(gb, wRotatingCubePos);
  CYC(0x4d19, 0x4d1a); alu_cp(gb, L);
  if (F & FZ) {
    CYCT(0x4d1a, 0x4d1d); push_effect(gb, 0x4d1d); dungeonEvents_lightCubeTorches(gb);
  } else {
    CYC(0x4d1a, 0x4d1d);
  }
  CYC(0x4d1d, 0x4d20); A = mem_rd(gb, wRotatingCubePos);
  CYC(0x4d20, 0x4d22); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4d22, 0x4d23); mem_wr(gb, DE, A);
  CYC(0x4d23, 0x4d24); ret_effect(gb);
}

// d2: Set torch color based on the color of the tile at this position.
void interaction21_subid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4d2e, checkInteractionState_hook, 0x23fe, 0x4d31);
  if (!(F & FZ)) {
    CYCT(0x4d31, 0x4d33); goto initialized;
  }
  CYC(0x4d31, 0x4d33);
  CALL_C(0x4d33, interactionIncState_hook, 0x23e0, 0x4d36);
  CALL_C(0x4d36, objectGetTileAtPosition_hook, 0x1444, 0x4d39);
  CYC(0x4d39, 0x4d3b); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4d3b, 0x4d3c); mem_wr(gb, DE, A);
  CYC(0x4d3c, 0x4d3e); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(0x4d3e, 0x4d40); A |= 1 << 7;
  CYC(0x4d40, 0x4d43); mem_wr(gb, wRotatingCubeColor, A);
  CYC(0x4d43, 0x4d45); A = 0x57;
  CYC(0x4d45, 0x4d48); mem_wr(gb, wRotatingCubePos, A);

initialized:
  CALL_C(0x4d48, objectGetTileAtPosition_hook, 0x1444, 0x4d4b);
  CYC(0x4d4b, 0x4d4c); B = A;
  CYC(0x4d4c, 0x4d4e); alu_sub(gb, 0xad);
  CYC(0x4d4e, 0x4d50); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x4d50, 0x4d51); ret_effect(gb); return;
  }
  CYC(0x4d50, 0x4d51);
  CYC(0x4d51, 0x4d53); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x4d53, 0x4d54); A = mem_rd(gb, DE);
  CYC(0x4d54, 0x4d55); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x4d55, 0x4d56); ret_effect(gb); return;
  }
  CYC(0x4d55, 0x4d56);
  CYC(0x4d56, 0x4d57); A = B;
  CYC(0x4d57, 0x4d58); mem_wr(gb, DE, A);
  CYC(0x4d58, 0x4d5a); alu_sub(gb, 0xad);
  CYC(0x4d5a, 0x4d5c); A |= 1 << 7;
  CYC(0x4d5c, 0x4d5f); mem_wr(gb, wRotatingCubeColor, A);
  CYC(0x4d5f, 0x4d60); ret_effect(gb);
}

// d2: Drop a small key here when a colored block puzzle has been solved.
void interaction21_subid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4d60, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4d63);
  CYC(0x4d63, 0x4d66); SET_HL(interaction21_subid05_tileData_bank08);
  CYC(0x4d66, 0x4d69); verifyTilesAndDropSmallKey_hook(gb);
}

// d2: Set trigger 0 when the colored flames are lit red.
void interaction21_subid06_hook(GB *gb) {
  CYC(0x4d78, 0x4d7a); B = 0x80;
  CYC(0x4d7a, 0x4d7c); dungeonEvents_setTriggerIfCubeColorIs(gb);
}

// d1: Set trigger 0 when the colored flames are lit blue.
void interaction21_subid19_hook(GB *gb) {
  CYC(0x4d7c, 0x4d7e); B = 0x82;
  dungeonEvents_setTriggerIfCubeColorIs(gb);
}

// Toggle a bit in wSwitchState based on whether a toggleable floor tile at position Y is
// blue. The bitmask to use is X. Falls through into setSwitch.
void interaction21_subid07_hook(GB *gb) {
  CYC(0x4d8b, 0x4d8d); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x4d8d, 0x4d8e); A = mem_rd(gb, DE);
  CYC(0x4d8e, 0x4d8f); C = A;
  CYC(0x4d8f, 0x4d91); B = wRoomLayout >> 8;
  CYC(0x4d91, 0x4d92); A = mem_rd(gb, BC);
  CYC(0x4d92, 0x4d94); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(0x4d94, 0x4d96); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x4d96, 0x4d97); ret_effect(gb); return;
  }
  CYC(0x4d96, 0x4d97);
  CYC(0x4d97, 0x4d98); A = mem_rd(gb, BC);
  CYC(0x4d98, 0x4d9a); alu_cp(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  if (F & FZ) {
    CYCT(0x4d9a, 0x4d9c); unsetSwitch_hook(gb); return;
  }
  CYC(0x4d9a, 0x4d9c);
  CYC(0x4d9c, 0x4d9e); alu_cp(gb, 0xae); // TILEINDEX_YELLOW_TOGGLE_FLOOR
  if (F & FZ) {
    CYCT(0x4d9e, 0x4da0); unsetSwitch_hook(gb); return;
  }
  CYC(0x4d9e, 0x4da0);
  setSwitch_hook(gb);
}

void setSwitch_hook(GB *gb) {
  CYC(0x4da0, 0x4da2); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x4da2, 0x4da3); A = mem_rd(gb, DE);
  CYC(0x4da3, 0x4da6); SET_HL(wSwitchState);
  CYC(0x4da6, 0x4da7); alu_or(gb, mem_rd(gb, HL));
  CYC(0x4da7, 0x4da8); mem_wr(gb, HL, A);
  CYC(0x4da8, 0x4da9); ret_effect(gb);
}

void unsetSwitch_hook(GB *gb) {
  CYC(0x4da9, 0x4dab); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x4dab, 0x4dac); A = mem_rd(gb, DE);
  CYC(0x4dac, 0x4dad); alu_cpl(gb);
  CYC(0x4dad, 0x4db0); SET_HL(wSwitchState);
  CYC(0x4db0, 0x4db1); alu_and(gb, mem_rd(gb, HL));
  CYC(0x4db1, 0x4db2); mem_wr(gb, HL, A);
  CYC(0x4db2, 0x4db3); ret_effect(gb);
}

// Toggle a bit in wSwitchState based on whether blue flames are lit. The bitmask to use is
// X.
void interaction21_subid08_hook(GB *gb) {
  CYC(0x4db3, 0x4db6); SET_HL(wRotatingCubeColor);
  CYC(0x4db6, 0x4db8); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4db8, 0x4db9); ret_effect(gb); return;
  }
  CYC(0x4db8, 0x4db9);
  CYC(0x4db9, 0x4dbb); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x4dbb, 0x4dbc); A = mem_rd(gb, HL);
  CYC(0x4dbc, 0x4dbe); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x4dbe, 0x4dc0); setSwitch_hook(gb); return;
  }
  CYC(0x4dbe, 0x4dc0);
  CYC(0x4dc0, 0x4dc2); unsetSwitch_hook(gb);
}

// d3: Drop a small key when 3 blocks have been pushed.
void interaction21_subid09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4dc2, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4dc5);
  CYC(0x4dc5, 0x4dc8); SET_HL(interaction21_subid09_tileData_bank08);
  CYC(0x4dc8, 0x4dcb); verifyTilesAndDropSmallKey_hook(gb);
}

// d3: When an orb is hit, spawn an armos, as well as interaction which will spawn a chest
// when it's killed.
void interaction21_subid0a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4dd0, checkInteractionState_hook, 0x23fe, 0x4dd3);
  if (!(F & FZ)) {
    CYCT(0x4dd3, 0x4dd5); goto initialized;
  }
  CYC(0x4dd3, 0x4dd5);
  CYC(0x4dd5, 0x4dd8); SET_HL(wToggleBlocksState);
  CYC(0x4dd8, 0x4dda); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  CYC(0x4dda, 0x4ddd); SET_HL(moonlitGrotto_orb_bank12);
  CALL_C(0x4ddd, parseGivenObjectData_b00_hook, 0x3171, 0x4de0);
  CALL_C(0x4de0, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4de3);
  CALL_C(0x4de3, interactionIncState_hook, 0x23e0, 0x4de6);

initialized:
  CYC(0x4de6, 0x4de9); SET_HL(wToggleBlocksState);
  CYC(0x4de9, 0x4deb); alu_bit(gb, 4, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4deb, 0x4dec); ret_effect(gb); return;
  }
  CYC(0x4deb, 0x4dec);
  // Do something with the chest?
  CYC(0x4dec, 0x4dee); A = 0x01;
  CYC(0x4dee, 0x4df1); mem_wr(gb, wcca2, A);
  CYC(0x4df1, 0x4df4); SET_HL(moonlitGrotto_onOrbActivation_bank12);
  CALL_C(0x4df4, parseGivenObjectData_b00_hook, 0x3171, 0x4df7);
  CYC(0x4df7, 0x4dfa); interactionDelete_hook(gb);
}

// Unused? A chest appears when 4 torches in a diamond formation are lit?
void interaction21_subid0b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4dfa, checkInteractionState_hook, 0x23fe, 0x4dfd);
  if (!(F & FZ)) {
    CYCT(0x4dfd, 0x4dff); goto initialized;
  }
  CYC(0x4dfd, 0x4dff);
  CALL_C(0x4dff, getThisRoomFlags_hook, 0x197d, 0x4e02);
  CYC(0x4e02, 0x4e04); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(0x4e04, 0x4e07); interactionDelete_hook(gb); return;
  }
  CYC(0x4e04, 0x4e07);
  CYC(0x4e07, 0x4e0a); SET_HL(objectData77d4_bank12);
  CALL_C(0x4e0a, parseGivenObjectData_b00_hook, 0x3171, 0x4e0d);

  CYC(0x4e0d, 0x4e10); SET_BC(0x4b35);
  CALL_C(0x4e10, makeTorchAtPositionTemporarilyLightable_hook, 0x50a6, 0x4e13);
  if (!(F & FZ)) {
    CYCT(0x4e13, 0x4e16); interactionDelete_hook(gb); return;
  }
  CYC(0x4e13, 0x4e16);

  CYC(0x4e16, 0x4e19); SET_BC(0x4b53);
  CALL_C(0x4e19, makeTorchAtPositionTemporarilyLightable_hook, 0x50a6, 0x4e1c);
  if (!(F & FZ)) {
    CYCT(0x4e1c, 0x4e1f); interactionDelete_hook(gb); return;
  }
  CYC(0x4e1c, 0x4e1f);

  CYC(0x4e1f, 0x4e22); SET_BC(0x4b57);
  CALL_C(0x4e22, makeTorchAtPositionTemporarilyLightable_hook, 0x50a6, 0x4e25);
  if (!(F & FZ)) {
    CYCT(0x4e25, 0x4e28); interactionDelete_hook(gb); return;
  }
  CYC(0x4e25, 0x4e28);

  CYC(0x4e28, 0x4e2b); SET_BC(0x4b75);
  CALL_C(0x4e2b, makeTorchAtPositionTemporarilyLightable_hook, 0x50a6, 0x4e2e);
  if (!(F & FZ)) {
    CYCT(0x4e2e, 0x4e31); interactionDelete_hook(gb); return;
  }
  CYC(0x4e2e, 0x4e31);

  CALL_C(0x4e31, interactionIncState_hook, 0x23e0, 0x4e34);

initialized:
  CYC(0x4e34, 0x4e37); A = mem_rd(gb, wNumTorchesLit);
  CYC(0x4e37, 0x4e39); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x4e39, 0x4e3a); ret_effect(gb); return;
  }
  CYC(0x4e39, 0x4e3a);
  CYC(0x4e3a, 0x4e3d); SET_HL(wDisabledObjects);
  CYC(0x4e3d, 0x4e3f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3)));
  CALL_C(0x4e3f, getThisRoomFlags_hook, 0x197d, 0x4e42);
  CYC(0x4e42, 0x4e44); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x4e44, 0x4e46); A = 0x01;
  CYC(0x4e46, 0x4e49); mem_wr(gb, wActiveTriggers, A);
  CYC(0x4e49, 0x4e4c); interactionDelete_hook(gb);
}

// d3: 4 armos spawn when trigger 0 is activated.
void interaction21_subid0c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4e4c, 0x4e4f); A = mem_rd(gb, wActiveTriggers);
  CYC(0x4e4f, 0x4e50); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4e50, 0x4e51); ret_effect(gb); return;
  }
  CYC(0x4e50, 0x4e51);
  CYC(0x4e51, 0x4e54); mem_wr(gb, wcca2, A);
  CYC(0x4e54, 0x4e57); SET_HL(moonlitGrotto_onArmosSwitchPressed_bank12);
  CALL_C(0x4e57, parseGivenObjectData_b00_hook, 0x3171, 0x4e5a);
  CYC(0x4e5a, 0x4e5d); interactionDelete_hook(gb);
}

// d3: Crystal breakage handler
void interaction21_subid0d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4e5d, 0x4e5f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4e5f, 0x4e60); A = mem_rd(gb, DE);
  CYC(0x4e60, 0x4e61); push_effect(gb, 0x4e61);
  switch (dungeonEvents_jumpTable(gb)) {
    case 0x4e69: goto state0;
    case 0x4e82: goto state1;
    case 0x2552: interactionRunScript_hook(gb); return;
    case 0x4ea9: goto state3;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x4e69, 0x4e6b); A = 0x0f; // GLOBALFLAG_D3_CRYSTALS
  CALL_C(0x4e6b, checkGlobalFlag_hook, 0x31f3, 0x4e6e);
  if (!(F & FZ)) {
    CYCT(0x4e6e, 0x4e71); interactionDelete_hook(gb); return;
  }
  CYC(0x4e6e, 0x4e71);
  CALL_C(0x4e71, getThisRoomFlags_hook, 0x197d, 0x4e74);
  CYC(0x4e74, 0x4e76); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(0x4e76, 0x4e79); interactionDelete_hook(gb); return;
  }
  CYC(0x4e76, 0x4e79);
  CYC(0x4e79, 0x4e7c); A = mem_rd(gb, wSwitchState);
  CYC(0x4e7c, 0x4e7e); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x4e7e, 0x4e7f); mem_wr(gb, DE, A);
  CYC(0x4e7f, 0x4e82); interactionIncState_hook(gb);
  return;

state1:
  CYC(0x4e82, 0x4e85); A = mem_rd(gb, wSwitchState);
  CYC(0x4e85, 0x4e86); B = A;
  CYC(0x4e86, 0x4e88); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x4e88, 0x4e89); A = mem_rd(gb, DE);
  CYC(0x4e89, 0x4e8a); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x4e8a, 0x4e8b); ret_effect(gb); return;
  }
  CYC(0x4e8a, 0x4e8b);
  CYC(0x4e8b, 0x4e8e); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(0x4e8e, 0x4e8f); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4e8f, 0x4e90); ret_effect(gb); return;
  }
  CYC(0x4e8f, 0x4e90);
  CYC(0x4e90, 0x4e91); A = alu_inc8(gb, A);
  CYC(0x4e91, 0x4e94); mem_wr(gb, wDisabledObjects, A);
  CYC(0x4e94, 0x4e97); mem_wr(gb, wMenuDisabled, A);
  CYC(0x4e97, 0x4e9a); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(0x4e9a, 0x4e9d); mem_wr(gb, wDisableWarpTiles, A);
  CYC(0x4e9d, 0x4ea0); SET_HL(moonlitGrottoScript_brokeCrystal_bank0c);
  CALL_C(0x4ea0, interactionSetScript_hook, 0x2544, 0x4ea3);
  CALL_C(0x4ea3, interactionRunScript_hook, 0x2552, 0x4ea6);
  CYC(0x4ea6, 0x4ea9); interactionIncState_hook(gb);
  return;

state3:
  CYC(0x4ea9, 0x4eac); A = mem_rd(gb, wSwitchState);
  CYC(0x4eac, 0x4eae); alu_and(gb, 0xf0);
  CYC(0x4eae, 0x4eb0); alu_cp(gb, 0xf0);
  if (!(F & FZ)) {
    CYCT(0x4eb0, 0x4eb2); goto enableControl;
  }
  CYC(0x4eb0, 0x4eb2);
  CYC(0x4eb2, 0x4eb4); A = 0x02;
  CYC(0x4eb4, 0x4eb7); mem_wr(gb, wScreenShakeMagnitude, A);
  CYC(0x4eb7, 0x4eba); SET_HL(moonlitGrottoScript_brokeAllCrystals_bank0c);
  CALL_C(0x4eba, interactionSetScript_hook, 0x2544, 0x4ebd);
  CYC(0x4ebd, 0x4ebf); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4ebf, 0x4ec1); A = 0x02;
  CYC(0x4ec1, 0x4ec2); mem_wr(gb, DE, A);
  CYC(0x4ec2, 0x4ec3); alu_xor(gb, A);
  CYC(0x4ec3, 0x4ec6); mem_wr(gb, wSpinnerState, A);
  CYC(0x4ec6, 0x4ec7); ret_effect(gb);
  return;

enableControl:
  // jpab scriptHelp.moonlitGrotto_enableControlAfterBreakingCrystal
  CYC(0x4ec7, 0x4eca); SET_HL(moonlitGrotto_enableControlAfterBreakingCrystal_bank15);
  CYC(0x4eca, 0x4ecc); E = 0x15;
  CYC(0x4ecc, 0x4ecf); interBankCall_hook(gb);
}

// d3: Small key falls when a block is pushed into place
void interaction21_subid0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4ecf, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4ed2);
  CYC(0x4ed2, 0x4ed5); SET_HL(wRoomLayout + 0x4a);
  CYC(0x4ed5, 0x4ed6); A = mem_rd(gb, HL);
  CYC(0x4ed6, 0x4ed8); alu_cp(gb, 0x2a);
  if (!(F & FZ)) {
    CYCT(0x4ed8, 0x4ed9); ret_effect(gb); return;
  }
  CYC(0x4ed8, 0x4ed9);
  CYC(0x4ed9, 0x4edc); spawnSmallKeyFromCeiling_hook(gb);
}

// d4: A door opens when a certain floor pattern is achieved
void interaction21_subid0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4edc, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x4edf);
  CYC(0x4edf, 0x4ee2); SET_HL(interaction21_subid0f_tileData_bank08);
  CALL_C(0x4ee2, verifyTiles_hook, 0x5094, 0x4ee5);
  CYC(0x4ee5, 0x4ee7); A = 0x01;
  if (F & FZ) {
    CYCT(0x4ee7, 0x4ee9);
  } else {
    CYC(0x4ee7, 0x4ee9);
    CYC(0x4ee9, 0x4eea); A = alu_dec8(gb, A);
  }
  CYC(0x4eea, 0x4eed); mem_wr(gb, wActiveTriggers, A);
  CYC(0x4eed, 0x4eee); ret_effect(gb);
}

// d4: A small key falls when a certain floor pattern is achieved
void interaction21_subid10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4efd, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x4f00);
  CALL_C(0x4f00, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4f03);
  CYC(0x4f03, 0x4f06); SET_HL(interaction21_subid10_tileData_bank08);
  CYC(0x4f06, 0x4f09); verifyTilesAndDropSmallKey_hook(gb);
}

// Tile-filling puzzle: when all the blue turns red, a chest will spawn here. Falls through
// into spawnChestAndDeleteSelf.
void interaction21_subid11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f11, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x4f14);
  CALL_C(0x4f14, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4f17);
  CYC(0x4f17, 0x4f19); A = 0x9f; // TILEINDEX_BLUE_FLOOR
  CALL_C(0x4f19, findTileInRoom_hook, 0x15cc, 0x4f1c);
  if (F & FZ) {
    CYCT(0x4f1c, 0x4f1d); ret_effect(gb); return;
  }
  CYC(0x4f1c, 0x4f1d);
  spawnChestAndDeleteSelf_hook(gb);
}

void spawnChestAndDeleteSelf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4f1d, 0x4f1f); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(0x4f1f, playSound_b00_hook, 0x0c98, 0x4f22);
  CALL_C(0x4f22, objectGetTileAtPosition_hook, 0x1444, 0x4f25);
  CYC(0x4f25, 0x4f26); C = L;
  CYC(0x4f26, 0x4f28); A = 0xf1; // TILEINDEX_CHEST
  CALL_C(0x4f28, setTile_hook, 0x3a9c, 0x4f2b);
  CALL_C(0x4f2b, objectCreatePuff_hook, 0x24c1, 0x4f2e);
  CYC(0x4f2e, 0x4f31); interactionDelete_hook(gb);
}

// d4: A chest spawns here when the torches light up with the color blue.
void interaction21_subid12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f31, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4f34);
  CYC(0x4f34, 0x4f37); A = mem_rd(gb, wRotatingCubeColor);
  CYC(0x4f37, 0x4f39); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(0x4f39, 0x4f3a); ret_effect(gb); return;
  }
  CYC(0x4f39, 0x4f3a);
  CYC(0x4f3a, 0x4f3c); alu_and(gb, 0x03);
  CYC(0x4f3c, 0x4f3e); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x4f3e, 0x4f3f); ret_effect(gb); return;
  }
  CYC(0x4f3e, 0x4f3f);
  CYC(0x4f3f, 0x4f41); spawnChestAndDeleteSelf_hook(gb);
}

// d5: A chest spawns here when all the spaces around the owl statue are filled.
void interaction21_subid13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f41, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x4f44);
  CALL_C(0x4f44, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4f47);
  CYC(0x4f47, 0x4f49); B = wRoomLayout >> 8;
  CYC(0x4f49, 0x4f4c); SET_HL(interaction21_subid13_positionsToCheck_bank08);
next:
  CYC(0x4f4c, 0x4f4d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4f4d, 0x4f4e); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4f4e, 0x4f50); spawnChestAndDeleteSelf_hook(gb); return;
  }
  CYC(0x4f4e, 0x4f50);
  CYC(0x4f50, 0x4f51); C = A;
  CYC(0x4f51, 0x4f52); A = mem_rd(gb, BC);
  CYC(0x4f52, 0x4f54); alu_sub(gb, 0x2c); // TILEINDEX_RED_PUSHABLE_BLOCK
  CYC(0x4f54, 0x4f56); alu_cp(gb, 0x03);
  if (F & FC) {
    CYCT(0x4f56, 0x4f58); goto next;
  }
  CYC(0x4f56, 0x4f58);
  CYC(0x4f58, 0x4f59); ret_effect(gb);
}

// d5: A chest spawns here when two blocks are pushed to the right places
void interaction21_subid14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f62, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x4f65);
  CALL_C(0x4f65, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4f68);
  CYC(0x4f68, 0x4f6b); SET_HL(interaction21_subid14_tileData_bank08);
  CALL_C(0x4f6b, verifyTiles_hook, 0x5094, 0x4f6e);
  if (!(F & FZ)) {
    CYCT(0x4f6e, 0x4f6f); ret_effect(gb); return;
  }
  CYC(0x4f6e, 0x4f6f);
  CYC(0x4f6f, 0x4f72); spawnChestAndDeleteSelf_hook(gb);
}

// d5: Cane of Somaria chest spawns here when blocks are pushed into a pattern
void interaction21_subid15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f76, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x4f79);
  CALL_C(0x4f79, interactionDeleteAndRetIfItemFlagSet_hook, 0x507d, 0x4f7c);
  CYC(0x4f7c, 0x4f7f); SET_HL(interaction21_subid15_tileData_bank08);
  CALL_C(0x4f7f, verifyTiles_hook, 0x5094, 0x4f82);
  if (!(F & FZ)) {
    CYCT(0x4f82, 0x4f83); ret_effect(gb); return;
  }
  CYC(0x4f82, 0x4f83);
  CYC(0x4f83, 0x4f86); spawnChestAndDeleteSelf_hook(gb);
}

// d5: Sets floor tiles to show a pattern when a switch is held down.
void interaction21_subid16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4f92, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x4f95);
  CYC(0x4f95, 0x4f97); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4f97, 0x4f98); A = mem_rd(gb, DE);
  CYC(0x4f98, 0x4f99); push_effect(gb, 0x4f99);
  switch (dungeonEvents_jumpTable(gb)) {
    case 0x4f9d: goto state0;
    case 0x4fde: interaction21_subid16_state1_hook(gb); return;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x4f9d, 0x4fa0); A = mem_rd(gb, wActiveTriggers);
  CYC(0x4fa0, 0x4fa1); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4fa1, 0x4fa2); ret_effect(gb); return;
  }
  CYC(0x4fa1, 0x4fa2);
  CALL_C(0x4fa2, interactionIncState_hook, 0x23e0, 0x4fa5);

  CYC(0x4fa5, 0x4fa7); C = 0x5c;
  CYC(0x4fa7, 0x4fa9); A = 0xad; // TILEINDEX_RED_TOGGLE_FLOOR
  CALL_C(0x4fa9, setTileWithPuff_hook, 0x4fd0, 0x4fac);

  CYC(0x4fac, 0x4fae); C = 0x6a;
  CYC(0x4fae, 0x4fb0); A = 0xad;
  CALL_C(0x4fb0, setTileWithPuff_hook, 0x4fd0, 0x4fb3);

  CYC(0x4fb3, 0x4fb5); C = 0x3b;
  CYC(0x4fb5, 0x4fb7); A = 0xae; // TILEINDEX_YELLOW_TOGGLE_FLOOR
  CALL_C(0x4fb7, setTileWithPuff_hook, 0x4fd0, 0x4fba);

  CYC(0x4fba, 0x4fbc); C = 0x5a;
  CYC(0x4fbc, 0x4fbe); A = 0xae;
  CALL_C(0x4fbe, setTileWithPuff_hook, 0x4fd0, 0x4fc1);

  CYC(0x4fc1, 0x4fc3); C = 0x4c;
  CYC(0x4fc3, 0x4fc5); A = 0xaf; // TILEINDEX_BLUE_TOGGLE_FLOOR
  CALL_C(0x4fc5, setTileWithPuff_hook, 0x4fd0, 0x4fc8);

  CYC(0x4fc8, 0x4fca); C = 0x7b;
  CYC(0x4fca, 0x4fcc); A = 0xaf;
  CYC(0x4fcc, 0x4fce); setTileWithPuff_hook(gb);
}

// Falls through into setTileWithPuff.
void setTileToStandardFloor_hook(GB *gb) {
  CYC(0x4fce, 0x4fd0); A = 0xa0; // TILEINDEX_STANDARD_FLOOR
  setTileWithPuff_hook(gb);
}

// Falls through into createPuffAt.
void setTileWithPuff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fd0, setTile_hook, 0x3a9c, 0x4fd3);
  createPuffAt_hook(gb);
}

// @param c Position to create puff at
void createPuffAt_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fd3, getFreeInteractionSlot_hook, 0x3aef, 0x4fd6);
  if (!(F & FZ)) {
    CYCT(0x4fd6, 0x4fd7); ret_effect(gb); return;
  }
  CYC(0x4fd6, 0x4fd7);
  CYC(0x4fd7, 0x4fd9); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(0x4fd9, 0x4fdb); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x4fdb, 0x4fde); setShortPosition_paramC_hook(gb);
}

void interaction21_subid16_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4fde, 0x4fe1); A = mem_rd(gb, wActiveTriggers);
  CYC(0x4fe1, 0x4fe2); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4fe2, 0x4fe3); ret_effect(gb); return;
  }
  CYC(0x4fe2, 0x4fe3);
  CYC(0x4fe3, 0x4fe5); C = 0x5c;
  CALL_C(0x4fe5, setTileToStandardFloor_hook, 0x4fce, 0x4fe8);
  CYC(0x4fe8, 0x4fea); C = 0x6a;
  CALL_C(0x4fea, setTileToStandardFloor_hook, 0x4fce, 0x4fed);
  CYC(0x4fed, 0x4fef); C = 0x3b;
  CALL_C(0x4fef, setTileToStandardFloor_hook, 0x4fce, 0x4ff2);
  CYC(0x4ff2, 0x4ff4); C = 0x5a;
  CALL_C(0x4ff4, setTileToStandardFloor_hook, 0x4fce, 0x4ff7);
  CYC(0x4ff7, 0x4ff9); C = 0x4c;
  CALL_C(0x4ff9, setTileToStandardFloor_hook, 0x4fce, 0x4ffc);
  CYC(0x4ffc, 0x4ffe); C = 0x7b;
  CALL_C(0x4ffe, setTileToStandardFloor_hook, 0x4fce, 0x5001);

  CYC(0x5001, 0x5003); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5003, 0x5004); alu_xor(gb, A);
  CYC(0x5004, 0x5005); mem_wr(gb, DE, A);
  CYC(0x5005, 0x5006); ret_effect(gb);
}

// Create a chest at position Y which appears when [wActiveTriggers] == X, but which also
// disappears when the trigger is released.
void interaction21_subid17_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5006, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x5009);
  CALL_C(0x5009, getThisRoomFlags_hook, 0x197d, 0x500c);
  CYC(0x500c, 0x500e); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) {
    CYCT(0x500e, 0x5011); interactionDelete_hook(gb); return;
  }
  CYC(0x500e, 0x5011);
  CYC(0x5011, 0x5013); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5013, 0x5014); A = mem_rd(gb, DE);
  CYC(0x5014, 0x5015); B = A;
  CYC(0x5015, 0x5018); A = mem_rd(gb, wActiveTriggers);
  CYC(0x5018, 0x5019); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x5019, 0x501b); goto triggerInactive;
  }
  CYC(0x5019, 0x501b);

  // @triggerActive
  CYC(0x501b, 0x501d); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x501d, 0x501e); A = mem_rd(gb, DE);
  CYC(0x501e, 0x501f); C = A;
  CYC(0x501f, 0x5021); B = wRoomLayout >> 8;
  CYC(0x5021, 0x5022); A = mem_rd(gb, BC);
  CYC(0x5022, 0x5024); alu_cp(gb, 0xf1); // TILEINDEX_CHEST
  if (F & FZ) {
    CYCT(0x5024, 0x5025); ret_effect(gb); return;
  }
  CYC(0x5024, 0x5025);
  CYC(0x5025, 0x5027); A = 0xf1; // TILEINDEX_CHEST
  CALL_C(0x5027, setTile_hook, 0x3a9c, 0x502a);
  CALL_C(0x502a, createPuffAt_hook, 0x4fd3, 0x502d);
  CYC(0x502d, 0x502f); A = 0x4d; // SND_SOLVEPUZZLE
  CYC(0x502f, 0x5032); playSound_b00_hook(gb);
  return;

triggerInactive:
  CYC(0x5032, 0x5034); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x5034, 0x5035); A = mem_rd(gb, DE);
  CYC(0x5035, 0x5036); C = A;
  CYC(0x5036, 0x5038); B = wRoomLayout >> 8;
  CYC(0x5038, 0x5039); A = mem_rd(gb, BC);
  CYC(0x5039, 0x503b); alu_cp(gb, 0xf1); // TILEINDEX_CHEST
  if (!(F & FZ)) {
    CYCT(0x503b, 0x503c); ret_effect(gb); return;
  }
  CYC(0x503b, 0x503c);
  // Retrieve whatever tile was there before the chest
  CYC(0x503c, 0x503e); A = 0x03; // :w3RoomLayoutBuffer
  CYC(0x503e, 0x5040); mem_wr(gb, 0xff70, A); // R_SVBK
  CYC(0x5040, 0x5042); B = w3RoomLayoutBuffer >> 8;
  CYC(0x5042, 0x5043); A = mem_rd(gb, BC);
  CYC(0x5043, 0x5044); L = A;
  CYC(0x5044, 0x5045); alu_xor(gb, A);
  CYC(0x5045, 0x5047); mem_wr(gb, 0xff70, A); // R_SVBK
  CYC(0x5047, 0x5048); A = L;
  CALL_C(0x5048, setTile_hook, 0x3a9c, 0x504b);
  CYC(0x504b, 0x504e); createPuffAt_hook(gb);
}

// d3: Calculate the value for [wSwitchState] based on which crystals are broken.
void interaction21_subid18_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x504e, getThisRoomFlags_hook, 0x197d, 0x5051);
  CYC(0x5051, 0x5053); B = 0x00;

  CYC(0x5053, 0x5055); L = 0x5d; // <ROOM_AGES_45d
  CYC(0x5055, 0x5057); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5057, 0x5059);
  } else {
    CYC(0x5057, 0x5059);
    CYC(0x5059, 0x505b); B |= 1 << 4;
  }

  CYC(0x505b, 0x505d); L = 0x5f; // <ROOM_AGES_45f
  CYC(0x505d, 0x505f); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x505f, 0x5061);
  } else {
    CYC(0x505f, 0x5061);
    CYC(0x5061, 0x5063); B |= 1 << 5;
  }

  CYC(0x5063, 0x5065); L = 0x61; // <ROOM_AGES_461
  CYC(0x5065, 0x5067); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x5067, 0x5069);
  } else {
    CYC(0x5067, 0x5069);
    CYC(0x5069, 0x506b); B |= 1 << 6;
  }

  CYC(0x506b, 0x506d); L = 0x63; // <ROOM_AGES_463
  CYC(0x506d, 0x506f); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x506f, 0x5071);
  } else {
    CYC(0x506f, 0x5071);
    CYC(0x5071, 0x5073); B |= 1 << 7;
  }

  CYC(0x5073, 0x5076); A = mem_rd(gb, wSwitchState);
  CYC(0x5076, 0x5077); alu_or(gb, B);
  CYC(0x5077, 0x507a); mem_wr(gb, wSwitchState, A);
  CYC(0x507a, 0x507d); interactionDelete_hook(gb);
}

// Deletes the interaction and returns to the caller's caller if this room's item flag is
// set.
void interactionDeleteAndRetIfItemFlagSet_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x507d, getThisRoomFlags_hook, 0x197d, 0x5080);
  CYC(0x5080, 0x5082); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (F & FZ) {
    CYCT(0x5082, 0x5083); ret_effect(gb); return;
  }
  CYC(0x5082, 0x5083);
  CYC(0x5083, 0x5084); SET_HL(pop_effect(gb));
  CYC(0x5084, 0x5087); interactionDelete_hook(gb);
}

void spawnSmallKeyFromCeiling_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5087, 0x508a); SET_BC(0x3001); // TREASURE_SMALL_KEY, $01
  CALL_C(0x508a, createTreasure_hook, 0x27d4, 0x508d);
  if (!(F & FZ)) {
    CYCT(0x508d, 0x508e); ret_effect(gb); return;
  }
  CYC(0x508d, 0x508e);
  CALL_C(0x508e, objectCopyPosition_hook, 0x2242, 0x5091);
  CYC(0x5091, 0x5094); interactionDelete_hook(gb);
}

// Verifies that certain tiles in the room layout equal specified values.
// @param hl Data structure: first byte is a tile index, subsequent bytes are positions where
//           the tile is expected; $ff starts a new group, $00 ends the structure.
// @param[out] zflag Set if the tiles all match the expected values.
void verifyTiles_hook(GB *gb) {
  CYC(0x5094, 0x5096); B = wRoomLayout >> 8;
nextTileIndex:
  CYC(0x5096, 0x5097); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5097, 0x5098); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5098, 0x5099); ret_effect(gb); return;
  }
  CYC(0x5098, 0x5099);
  CYC(0x5099, 0x509a); E = A;
nextPosition:
  CYC(0x509a, 0x509b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x509b, 0x509c); C = A;
  CYC(0x509c, 0x509d); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x509d, 0x509e); ret_effect(gb); return;
  }
  CYC(0x509d, 0x509e);
  CYC(0x509e, 0x509f); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(0x509f, 0x50a1); goto nextTileIndex;
  }
  CYC(0x509f, 0x50a1);
  CYC(0x50a1, 0x50a2); A = mem_rd(gb, BC);
  CYC(0x50a2, 0x50a3); alu_cp(gb, E);
  if (!(F & FZ)) {
    CYCT(0x50a3, 0x50a4); ret_effect(gb); return;
  }
  CYC(0x50a3, 0x50a4);
  CYC(0x50a4, 0x50a6); goto nextPosition;
}

// @param b Number of frames it can stay lit before burning out
// @param c Position
// @param[out] zflag Set if the part object was created successfully
void makeTorchAtPositionTemporarilyLightable_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x50a6, getFreePartSlot_hook, 0x3e8e, 0x50a9);
  if (!(F & FZ)) {
    CYCT(0x50a9, 0x50aa); ret_effect(gb); return;
  }
  CYC(0x50a9, 0x50aa);
  CYC(0x50aa, 0x50ac); mem_wr(gb, HL, 0x06); // PART_LIGHTABLE_TORCH
  CYC(0x50ac, 0x50ad); L = alu_inc8(gb, L);
  CYC(0x50ad, 0x50af); mem_wr(gb, HL, 0x01);
  CYC(0x50af, 0x50b1); L = PART_BASE + OBJ_COUNTER2;
  CYC(0x50b1, 0x50b2); mem_wr(gb, HL, B);
  CYC(0x50b2, 0x50b4); L = PART_BASE + OBJ_YH;
  CALL_C(0x50b4, setShortPosition_paramC_hook, 0x20b9, 0x50b7);
  CYC(0x50b7, 0x50b8); alu_xor(gb, A);
  CYC(0x50b8, 0x50b9); ret_effect(gb);
}

// INTERAC_DUNGEON_EVENTS: subid-dispatched one-off dungeon puzzle handlers.
void interactionCode21_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4c9a, 0x4c9c); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4c9c, 0x4c9d); A = mem_rd(gb, DE);
  CYC(0x4c9d, 0x4c9e); push_effect(gb, 0x4c9e);
  switch (dungeonEvents_jumpTable(gb)) {
    case 0x3b05: interactionDelete_hook(gb); return;
    case 0x4cd2: interaction21_subid01_hook(gb); return;
    case 0x4ce7: interaction21_subid02_hook(gb); return;
    case 0x4cf5: interaction21_subid03_hook(gb); return;
    case 0x4d2e: interaction21_subid04_hook(gb); return;
    case 0x4d60: interaction21_subid05_hook(gb); return;
    case 0x4d78: interaction21_subid06_hook(gb); return;
    case 0x4d8b: interaction21_subid07_hook(gb); return;
    case 0x4db3: interaction21_subid08_hook(gb); return;
    case 0x4dc2: interaction21_subid09_hook(gb); return;
    case 0x4dd0: interaction21_subid0a_hook(gb); return;
    case 0x4dfa: interaction21_subid0b_hook(gb); return;
    case 0x4e4c: interaction21_subid0c_hook(gb); return;
    case 0x4e5d: interaction21_subid0d_hook(gb); return;
    case 0x4ecf: interaction21_subid0e_hook(gb); return;
    case 0x4edc: interaction21_subid0f_hook(gb); return;
    case 0x4efd: interaction21_subid10_hook(gb); return;
    case 0x4f11: interaction21_subid11_hook(gb); return;
    case 0x4f31: interaction21_subid12_hook(gb); return;
    case 0x4f41: interaction21_subid13_hook(gb); return;
    case 0x4f62: interaction21_subid14_hook(gb); return;
    case 0x4f76: interaction21_subid15_hook(gb); return;
    case 0x4f92: interaction21_subid16_hook(gb); return;
    case 0x5006: interaction21_subid17_hook(gb); return;
    case 0x504e: interaction21_subid18_hook(gb); return;
    case 0x4d7c: interaction21_subid19_hook(gb); return;
    default: HANDOFF(HL);
  }
}
