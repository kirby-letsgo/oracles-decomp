#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// verifyTiles data: b0 = tile index, then positions; $ff starts a new group, $00 ends.
#define subid01_tileData_bank08 SYM(subid01_tileData)
#define interaction21_subid05_tileData_bank08 SYM(interaction21_subid05__tileData)
#define interaction21_subid09_tileData_bank08 SYM(interaction21_subid09__tileData)
#define interaction21_subid0f_tileData_bank08 SYM(interaction21_subid0f__tileData)
#define interaction21_subid10_tileData_bank08 SYM(interaction21_subid10__tileData)
// The positions in a circle around the owl statue, $00-terminated.
#define interaction21_subid13_positionsToCheck_bank08 SYM(interaction21_subid13__positionsToCheck)
#define interaction21_subid14_tileData_bank08 SYM(interaction21_subid14__tileData)
#define interaction21_subid15_tileData_bank08 SYM(interaction21_subid15__tileData)

// objectData.* (bank $12) and mainScripts.* (bank $0c), referenced by address only.
#define moonlitGrotto_orb_bank12 SYM(moonlitGrotto_orb)
#define moonlitGrotto_onOrbActivation_bank12 SYM(moonlitGrotto_onOrbActivation)
#define objectData77d4_bank12 SYM(objectData77d4)
#define moonlitGrotto_onArmosSwitchPressed_bank12 SYM(moonlitGrotto_onArmosSwitchPressed)
#define moonlitGrottoScript_brokeCrystal_bank0c SYM(moonlitGrottoScript_brokeCrystal)
#define moonlitGrottoScript_brokeAllCrystals_bank0c SYM(moonlitGrottoScript_brokeAllCrystals)
#define moonlitGrotto_enableControlAfterBreakingCrystal_bank15 SYM(moonlitGrotto_enableControlAfterBreakingCrystal)

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
  BASE(interaction21_subid03);
  CYC(b_+47, b_+50); SET_HL(wRotatingCubeColor);
  CYC(b_+50, b_+52); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+52, b_+54); A = 0x72; // SND_LIGHTTORCH
  CYC(b_+54, b_+57); playSound_b00_hook(gb);
}

// Body shared by subid06 (`jr ++`) and subid19 (fallthrough): set trigger 0 iff
// [wRotatingCubeColor] == B.
static void dungeonEvents_setTriggerIfCubeColorIs(GB *gb) {
  BASE(interaction21_subid19);
  CYC(b_+2, b_+5); A = mem_rd(gb, wRotatingCubeColor);
  CYC(b_+5, b_+6); alu_cp(gb, B);
  CYC(b_+6, b_+8); A = 0x01;
  if (F & FZ) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  }
  CYC(b_+11, b_+14); mem_wr(gb, wActiveTriggers, A);
  CYC(b_+14, b_+15); ret_effect(gb);
}

// D2: Verify a 2x2 floor pattern. Falls through into verifyTilesAndDropSmallKey.
void interaction21_subid01_hook(GB *gb) {
  BASE(interaction21_subid01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+3);
  CYC(b_+3, b_+6); SET_HL(subid01_tileData_bank08);
  TAIL(verifyTilesAndDropSmallKey);
}

void verifyTilesAndDropSmallKey_hook(GB *gb) {
  BASE(verifyTilesAndDropSmallKey);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, verifyTiles_hook, SYM(verifyTiles), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); TAIL(spawnSmallKeyFromCeiling);
}

// D2: Verify a floor tile is red to open a door
void interaction21_subid02_hook(GB *gb) {
  BASE(interaction21_subid02);
  CYC(b_+0, b_+3); A = mem_rd(gb, wRoomLayout + 0x5a);
  CYC(b_+3, b_+5); alu_cp(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(b_+5, b_+7); A = 0x01;
  if (F & FZ) {
    CYCT(b_+7, b_+9);
  } else {
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  }
  CYC(b_+10, b_+13); mem_wr(gb, wActiveTriggers, A);
  CYC(b_+13, b_+14); ret_effect(gb);
}

// Light torches when a colored cube rolls into this position.
void interaction21_subid03_hook(GB *gb) {
  BASE(interaction21_subid03);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); goto initialized;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, interactionIncState_hook, SYM(interactionIncState), b_+8);
  CALL_C(b_+8, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, wRotatingCubePos);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); alu_cp(gb, L);
  if (F & FZ) {
    CYCT(b_+18, b_+21); push_effect(gb, b_+21); dungeonEvents_lightCubeTorches(gb);
  } else {
    CYC(b_+18, b_+21);
  }

initialized:
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); B = A;
  CYC(b_+25, b_+28); A = mem_rd(gb, wRotatingCubePos);
  CYC(b_+28, b_+29); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+29, b_+30); ret_effect(gb); return;
  }
  CYC(b_+29, b_+30);
  CALL_C(b_+30, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+33);
  CYC(b_+33, b_+36); A = mem_rd(gb, wRotatingCubePos);
  CYC(b_+36, b_+37); alu_cp(gb, L);
  if (F & FZ) {
    CYCT(b_+37, b_+40); push_effect(gb, b_+40); dungeonEvents_lightCubeTorches(gb);
  } else {
    CYC(b_+37, b_+40);
  }
  CYC(b_+40, b_+43); A = mem_rd(gb, wRotatingCubePos);
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+47); ret_effect(gb);
}

// d2: Set torch color based on the color of the tile at this position.
void interaction21_subid04_hook(GB *gb) {
  BASE(interaction21_subid04);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); goto initialized;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, interactionIncState_hook, SYM(interactionIncState), b_+8);
  CALL_C(b_+8, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+11);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+16); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(b_+16, b_+18); A |= 1 << 7;
  CYC(b_+18, b_+21); mem_wr(gb, wRotatingCubeColor, A);
  CYC(b_+21, b_+23); A = 0x57;
  CYC(b_+23, b_+26); mem_wr(gb, wRotatingCubePos, A);

initialized:
  CALL_C(b_+26, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+29);
  CYC(b_+29, b_+30); B = A;
  CYC(b_+30, b_+32); alu_sub(gb, 0xad);
  CYC(b_+32, b_+34); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+34, b_+35); ret_effect(gb); return;
  }
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+39, b_+40); ret_effect(gb); return;
  }
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+41); A = B;
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+44); alu_sub(gb, 0xad);
  CYC(b_+44, b_+46); A |= 1 << 7;
  CYC(b_+46, b_+49); mem_wr(gb, wRotatingCubeColor, A);
  CYC(b_+49, b_+50); ret_effect(gb);
}

// d2: Drop a small key here when a colored block puzzle has been solved.
void interaction21_subid05_hook(GB *gb) {
  BASE(interaction21_subid05);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+3);
  CYC(b_+3, b_+6); SET_HL(interaction21_subid05_tileData_bank08);
  CYC(b_+6, b_+9); TAIL(verifyTilesAndDropSmallKey);
}

// d2: Set trigger 0 when the colored flames are lit red.
void interaction21_subid06_hook(GB *gb) {
  BASE(interaction21_subid06);
  CYC(b_+0, b_+2); B = 0x80;
  CYC(b_+2, b_+4); dungeonEvents_setTriggerIfCubeColorIs(gb);
}

// d1: Set trigger 0 when the colored flames are lit blue.
void interaction21_subid19_hook(GB *gb) {
  BASE(interaction21_subid19);
  CYC(b_+0, b_+2); B = 0x82;
  dungeonEvents_setTriggerIfCubeColorIs(gb);
}

// Toggle a bit in wSwitchState based on whether a toggleable floor tile at position Y is
// blue. The bitmask to use is X. Falls through into setSwitch.
void interaction21_subid07_hook(GB *gb) {
  BASE(interaction21_subid07);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); B = wRoomLayout >> 8;
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+9); alu_sub(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  CYC(b_+9, b_+11); alu_cp(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, BC);
  CYC(b_+13, b_+15); alu_cp(gb, 0xad); // TILEINDEX_RED_TOGGLE_FLOOR
  if (F & FZ) {
    CYCT(b_+15, b_+17); TAIL(unsetSwitch);
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); alu_cp(gb, 0xae); // TILEINDEX_YELLOW_TOGGLE_FLOOR
  if (F & FZ) {
    CYCT(b_+19, b_+21); TAIL(unsetSwitch);
  }
  CYC(b_+19, b_+21);
  TAIL(setSwitch);
}

void setSwitch_hook(GB *gb) {
  BASE(setSwitch);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(wSwitchState);
  CYC(b_+6, b_+7); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void unsetSwitch_hook(GB *gb) {
  BASE(unsetSwitch);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_cpl(gb);
  CYC(b_+4, b_+7); SET_HL(wSwitchState);
  CYC(b_+7, b_+8); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+10); ret_effect(gb);
}

// Toggle a bit in wSwitchState based on whether blue flames are lit. The bitmask to use is
// X.
void interaction21_subid08_hook(GB *gb) {
  BASE(interaction21_subid08);
  CYC(b_+0, b_+3); SET_HL(wRotatingCubeColor);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+11, b_+13); TAIL(setSwitch);
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); TAIL(unsetSwitch);
}

// d3: Drop a small key when 3 blocks have been pushed.
void interaction21_subid09_hook(GB *gb) {
  BASE(interaction21_subid09);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+3);
  CYC(b_+3, b_+6); SET_HL(interaction21_subid09_tileData_bank08);
  CYC(b_+6, b_+9); TAIL(verifyTilesAndDropSmallKey);
}

// d3: When an orb is hit, spawn an armos, as well as interaction which will spawn a chest
// when it's killed.
void interaction21_subid0a_hook(GB *gb) {
  BASE(interaction21_subid0a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); goto initialized;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); SET_HL(wToggleBlocksState);
  CYC(b_+8, b_+10); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 4)));
  CYC(b_+10, b_+13); SET_HL(moonlitGrotto_orb_bank12);
  CALL_C(b_+13, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+16);
  CALL_C(b_+16, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+19);
  CALL_C(b_+19, interactionIncState_hook, SYM(interactionIncState), b_+22);

initialized:
  CYC(b_+22, b_+25); SET_HL(wToggleBlocksState);
  CYC(b_+25, b_+27); alu_bit(gb, 4, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+27, b_+28); ret_effect(gb); return;
  }
  CYC(b_+27, b_+28);
  // Do something with the chest?
  CYC(b_+28, b_+30); A = 0x01;
  CYC(b_+30, b_+33); mem_wr(gb, wcca2, A);
  CYC(b_+33, b_+36); SET_HL(moonlitGrotto_onOrbActivation_bank12);
  CALL_C(b_+36, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+39);
  CYC(b_+39, b_+42); TAIL(interactionDelete);
}

// Unused? A chest appears when 4 torches in a diamond formation are lit?
void interaction21_subid0b_hook(GB *gb) {
  BASE(interaction21_subid0b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); goto initialized;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+13); TAIL(interactionDelete);
  }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+16); SET_HL(objectData77d4_bank12);
  CALL_C(b_+16, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+19);

  CYC(b_+19, b_+22); SET_BC((SYM(interactionCode1f__subid0) + 1));
  CALL_C(b_+22, makeTorchAtPositionTemporarilyLightable_hook, SYM(makeTorchAtPositionTemporarilyLightable), b_+25);
  if (!(F & FZ)) {
    CYCT(b_+25, b_+28); TAIL(interactionDelete);
  }
  CYC(b_+25, b_+28);

  CYC(b_+28, b_+31); SET_BC((SYM(interactionCode1f__subid0) + 31));
  CALL_C(b_+31, makeTorchAtPositionTemporarilyLightable_hook, SYM(makeTorchAtPositionTemporarilyLightable), b_+34);
  if (!(F & FZ)) {
    CYCT(b_+34, b_+37); TAIL(interactionDelete);
  }
  CYC(b_+34, b_+37);

  CYC(b_+37, b_+40); SET_BC((SYM(interactionCode1f__subid0) + 35));
  CALL_C(b_+40, makeTorchAtPositionTemporarilyLightable_hook, SYM(makeTorchAtPositionTemporarilyLightable), b_+43);
  if (!(F & FZ)) {
    CYCT(b_+43, b_+46); TAIL(interactionDelete);
  }
  CYC(b_+43, b_+46);

  CYC(b_+46, b_+49); SET_BC((SYM(interactionCode1f__subid0__initialize) + 14));
  CALL_C(b_+49, makeTorchAtPositionTemporarilyLightable_hook, SYM(makeTorchAtPositionTemporarilyLightable), b_+52);
  if (!(F & FZ)) {
    CYCT(b_+52, b_+55); TAIL(interactionDelete);
  }
  CYC(b_+52, b_+55);

  CALL_C(b_+55, interactionIncState_hook, SYM(interactionIncState), b_+58);

initialized:
  CYC(b_+58, b_+61); A = mem_rd(gb, wNumTorchesLit);
  CYC(b_+61, b_+63); alu_cp(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(b_+63, b_+64); ret_effect(gb); return;
  }
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+67); SET_HL(wDisabledObjects);
  CYC(b_+67, b_+69); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3)));
  CALL_C(b_+69, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+72);
  CYC(b_+72, b_+74); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+74, b_+76); A = 0x01;
  CYC(b_+76, b_+79); mem_wr(gb, wActiveTriggers, A);
  CYC(b_+79, b_+82); TAIL(interactionDelete);
}

// d3: 4 armos spawn when trigger 0 is activated.
void interaction21_subid0c_hook(GB *gb) {
  BASE(interaction21_subid0c);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); mem_wr(gb, wcca2, A);
  CYC(b_+8, b_+11); SET_HL(moonlitGrotto_onArmosSwitchPressed_bank12);
  CALL_C(b_+11, parseGivenObjectData_b00_hook, SYM(parseGivenObjectData_b00), b_+14);
  CYC(b_+14, b_+17); TAIL(interactionDelete);
}

// d3: Crystal breakage handler
void interaction21_subid0d_hook(GB *gb) {
  BASE(interaction21_subid0d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dungeonEvents_jumpTable(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+37) { goto state1; }
    else if (jt_ == SYM(interactionRunScript) && hook_is(gb, SYM(interactionRunScript), interactionRunScript_hook)) { interactionRunScript_hook(gb); return; }
    else if (jt_ == b_+76) { goto state3; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+12, b_+14); A = 0x0f; // GLOBALFLAG_D3_CRYSTALS
  CALL_C(b_+14, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+17);
  if (!(F & FZ)) {
    CYCT(b_+17, b_+20); TAIL(interactionDelete);
  }
  CYC(b_+17, b_+20);
  CALL_C(b_+20, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+23);
  CYC(b_+23, b_+25); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(b_+25, b_+28); TAIL(interactionDelete);
  }
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+31); A = mem_rd(gb, wSwitchState);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+37); interactionIncState_hook(gb);
  return;

state1:
  CYC(b_+37, b_+40); A = mem_rd(gb, wSwitchState);
  CYC(b_+40, b_+41); B = A;
  CYC(b_+41, b_+43); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+45); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(b_+45, b_+46); ret_effect(gb); return;
  }
  CYC(b_+45, b_+46);
  CYC(b_+46, b_+49); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+49, b_+50); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+50, b_+51); ret_effect(gb); return;
  }
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+52); A = alu_inc8(gb, A);
  CYC(b_+52, b_+55); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+55, b_+58); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+58, b_+61); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+61, b_+64); mem_wr(gb, wDisableWarpTiles, A);
  CYC(b_+64, b_+67); SET_HL(moonlitGrottoScript_brokeCrystal_bank0c);
  CALL_C(b_+67, interactionSetScript_hook, SYM(interactionSetScript), b_+70);
  CALL_C(b_+70, interactionRunScript_hook, SYM(interactionRunScript), b_+73);
  CYC(b_+73, b_+76); interactionIncState_hook(gb);
  return;

state3:
  CYC(b_+76, b_+79); A = mem_rd(gb, wSwitchState);
  CYC(b_+79, b_+81); alu_and(gb, 0xf0);
  CYC(b_+81, b_+83); alu_cp(gb, 0xf0);
  if (!(F & FZ)) {
    CYCT(b_+83, b_+85); goto enableControl;
  }
  CYC(b_+83, b_+85);
  CYC(b_+85, b_+87); A = 0x02;
  CYC(b_+87, b_+90); mem_wr(gb, wScreenShakeMagnitude, A);
  CYC(b_+90, b_+93); SET_HL(moonlitGrottoScript_brokeAllCrystals_bank0c);
  CALL_C(b_+93, interactionSetScript_hook, SYM(interactionSetScript), b_+96);
  CYC(b_+96, b_+98); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+98, b_+100); A = 0x02;
  CYC(b_+100, b_+101); mem_wr(gb, DE, A);
  CYC(b_+101, b_+102); alu_xor(gb, A);
  CYC(b_+102, b_+105); mem_wr(gb, wSpinnerState, A);
  CYC(b_+105, b_+106); ret_effect(gb);
  return;

enableControl:
  // jpab scriptHelp.moonlitGrotto_enableControlAfterBreakingCrystal
  CYC(b_+106, b_+109); SET_HL(moonlitGrotto_enableControlAfterBreakingCrystal_bank15);
  CYC(b_+109, b_+111); E = 0x15;
  CYC(b_+111, b_+114); TAIL(interBankCall);
}

// d3: Small key falls when a block is pushed into place
void interaction21_subid0e_hook(GB *gb) {
  BASE(interaction21_subid0e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+3);
  CYC(b_+3, b_+6); SET_HL(wRoomLayout + 0x4a);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_cp(gb, 0x2a);
  if (!(F & FZ)) {
    CYCT(b_+9, b_+10); ret_effect(gb); return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+13); TAIL(spawnSmallKeyFromCeiling);
}

// d4: A door opens when a certain floor pattern is achieved
void interaction21_subid0f_hook(GB *gb) {
  BASE(interaction21_subid0f);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CYC(b_+3, b_+6); SET_HL(interaction21_subid0f_tileData_bank08);
  CALL_C(b_+6, verifyTiles_hook, SYM(verifyTiles), b_+9);
  CYC(b_+9, b_+11); A = 0x01;
  if (F & FZ) {
    CYCT(b_+11, b_+13);
  } else {
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+14); A = alu_dec8(gb, A);
  }
  CYC(b_+14, b_+17); mem_wr(gb, wActiveTriggers, A);
  CYC(b_+17, b_+18); ret_effect(gb);
}

// d4: A small key falls when a certain floor pattern is achieved
void interaction21_subid10_hook(GB *gb) {
  BASE(interaction21_subid10);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+6);
  CYC(b_+6, b_+9); SET_HL(interaction21_subid10_tileData_bank08);
  CYC(b_+9, b_+12); TAIL(verifyTilesAndDropSmallKey);
}

// Tile-filling puzzle: when all the blue turns red, a chest will spawn here. Falls through
// into spawnChestAndDeleteSelf.
void interaction21_subid11_hook(GB *gb) {
  BASE(interaction21_subid11);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+6);
  CYC(b_+6, b_+8); A = 0x9f; // TILEINDEX_BLUE_FLOOR
  CALL_C(b_+8, findTileInRoom_hook, SYM(findTileInRoom), b_+11);
  if (F & FZ) {
    CYCT(b_+11, b_+12); ret_effect(gb); return;
  }
  CYC(b_+11, b_+12);
  TAIL(spawnChestAndDeleteSelf);
}

void spawnChestAndDeleteSelf_hook(GB *gb) {
  BASE(spawnChestAndDeleteSelf);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x4d; // SND_SOLVEPUZZLE
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CALL_C(b_+5, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+8);
  CYC(b_+8, b_+9); C = L;
  CYC(b_+9, b_+11); A = 0xf1; // TILEINDEX_CHEST
  CALL_C(b_+11, setTile_hook, SYM(setTile), b_+14);
  CALL_C(b_+14, objectCreatePuff_hook, SYM(objectCreatePuff), b_+17);
  CYC(b_+17, b_+20); TAIL(interactionDelete);
}

// d4: A chest spawns here when the torches light up with the color blue.
void interaction21_subid12_hook(GB *gb) {
  BASE(interaction21_subid12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wRotatingCubeColor);
  CYC(b_+6, b_+8); alu_bit(gb, 7, A);
  if (F & FZ) {
    CYCT(b_+8, b_+9); ret_effect(gb); return;
  }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x03);
  CYC(b_+11, b_+13); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+14); ret_effect(gb); return;
  }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); TAIL(spawnChestAndDeleteSelf);
}

// d5: A chest spawns here when all the spaces around the owl statue are filled.
void interaction21_subid13_hook(GB *gb) {
  BASE(interaction21_subid13);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+6);
  CYC(b_+6, b_+8); B = wRoomLayout >> 8;
  CYC(b_+8, b_+11); SET_HL(interaction21_subid13_positionsToCheck_bank08);
next:
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+13, b_+15); TAIL(spawnChestAndDeleteSelf);
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+17); A = mem_rd(gb, BC);
  CYC(b_+17, b_+19); alu_sub(gb, 0x2c); // TILEINDEX_RED_PUSHABLE_BLOCK
  CYC(b_+19, b_+21); alu_cp(gb, 0x03);
  if (F & FC) {
    CYCT(b_+21, b_+23); goto next;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); ret_effect(gb);
}

// d5: A chest spawns here when two blocks are pushed to the right places
void interaction21_subid14_hook(GB *gb) {
  BASE(interaction21_subid14);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+6);
  CYC(b_+6, b_+9); SET_HL(interaction21_subid14_tileData_bank08);
  CALL_C(b_+9, verifyTiles_hook, SYM(verifyTiles), b_+12);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+13); ret_effect(gb); return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); TAIL(spawnChestAndDeleteSelf);
}

// d5: Cane of Somaria chest spawns here when blocks are pushed into a pattern
void interaction21_subid15_hook(GB *gb) {
  BASE(interaction21_subid15);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, interactionDeleteAndRetIfItemFlagSet_hook, SYM(interactionDeleteAndRetIfItemFlagSet), b_+6);
  CYC(b_+6, b_+9); SET_HL(interaction21_subid15_tileData_bank08);
  CALL_C(b_+9, verifyTiles_hook, SYM(verifyTiles), b_+12);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+13); ret_effect(gb); return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); TAIL(spawnChestAndDeleteSelf);
}

// d5: Sets floor tiles to show a pattern when a switch is held down.
void interaction21_subid16_hook(GB *gb) {
  BASE(interaction21_subid16);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (dungeonEvents_jumpTable(gb));
    if (jt_ == b_+11) { goto state0; }
    else if (jt_ == SYM(interaction21_subid16_state1) && hook_is(gb, SYM(interaction21_subid16_state1), interaction21_subid16_state1_hook)) { interaction21_subid16_state1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+11, b_+14); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+15, b_+16); ret_effect(gb); return;
  }
  CYC(b_+15, b_+16);
  CALL_C(b_+16, interactionIncState_hook, SYM(interactionIncState), b_+19);

  CYC(b_+19, b_+21); C = 0x5c;
  CYC(b_+21, b_+23); A = 0xad; // TILEINDEX_RED_TOGGLE_FLOOR
  CALL_C(b_+23, setTileWithPuff_hook, SYM(setTileWithPuff), b_+26);

  CYC(b_+26, b_+28); C = 0x6a;
  CYC(b_+28, b_+30); A = 0xad;
  CALL_C(b_+30, setTileWithPuff_hook, SYM(setTileWithPuff), b_+33);

  CYC(b_+33, b_+35); C = 0x3b;
  CYC(b_+35, b_+37); A = 0xae; // TILEINDEX_YELLOW_TOGGLE_FLOOR
  CALL_C(b_+37, setTileWithPuff_hook, SYM(setTileWithPuff), b_+40);

  CYC(b_+40, b_+42); C = 0x5a;
  CYC(b_+42, b_+44); A = 0xae;
  CALL_C(b_+44, setTileWithPuff_hook, SYM(setTileWithPuff), b_+47);

  CYC(b_+47, b_+49); C = 0x4c;
  CYC(b_+49, b_+51); A = 0xaf; // TILEINDEX_BLUE_TOGGLE_FLOOR
  CALL_C(b_+51, setTileWithPuff_hook, SYM(setTileWithPuff), b_+54);

  CYC(b_+54, b_+56); C = 0x7b;
  CYC(b_+56, b_+58); A = 0xaf;
  CYC(b_+58, b_+60); TAIL(setTileWithPuff);
}

// Falls through into setTileWithPuff.
void setTileToStandardFloor_hook(GB *gb) {
  BASE(setTileToStandardFloor);
  CYC(b_+0, b_+2); A = 0xa0; // TILEINDEX_STANDARD_FLOOR
  TAIL(setTileWithPuff);
}

// Falls through into createPuffAt.
void setTileWithPuff_hook(GB *gb) {
  BASE(setTileWithPuff);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, setTile_hook, SYM(setTile), SYM(createPuffAt));
  TAIL(createPuffAt);
}

// @param c Position to create puff at
void createPuffAt_hook(GB *gb) {
  BASE(createPuffAt);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+8, b_+11); TAIL(setShortPosition_paramC);
}

void interaction21_subid16_state1_hook(GB *gb) {
  BASE(interaction21_subid16_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); C = 0x5c;
  CALL_C(b_+7, setTileToStandardFloor_hook, SYM(setTileToStandardFloor), b_+10);
  CYC(b_+10, b_+12); C = 0x6a;
  CALL_C(b_+12, setTileToStandardFloor_hook, SYM(setTileToStandardFloor), b_+15);
  CYC(b_+15, b_+17); C = 0x3b;
  CALL_C(b_+17, setTileToStandardFloor_hook, SYM(setTileToStandardFloor), b_+20);
  CYC(b_+20, b_+22); C = 0x5a;
  CALL_C(b_+22, setTileToStandardFloor_hook, SYM(setTileToStandardFloor), b_+25);
  CYC(b_+25, b_+27); C = 0x4c;
  CALL_C(b_+27, setTileToStandardFloor_hook, SYM(setTileToStandardFloor), b_+30);
  CYC(b_+30, b_+32); C = 0x7b;
  CALL_C(b_+32, setTileToStandardFloor_hook, SYM(setTileToStandardFloor), b_+35);

  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+37, b_+38); alu_xor(gb, A);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+40); ret_effect(gb);
}

// Create a chest at position Y which appears when [wActiveTriggers] == X, but which also
// disappears when the trigger is released.
void interaction21_subid17_hook(GB *gb) {
  BASE(interaction21_subid17);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CALL_C(b_+3, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+6);
  CYC(b_+6, b_+8); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) {
    CYCT(b_+8, b_+11); TAIL(interactionDelete);
  }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+18); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+18, b_+19); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21); goto triggerInactive;
  }
  CYC(b_+19, b_+21);

  // @triggerActive
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+27); B = wRoomLayout >> 8;
  CYC(b_+27, b_+28); A = mem_rd(gb, BC);
  CYC(b_+28, b_+30); alu_cp(gb, 0xf1); // TILEINDEX_CHEST
  if (F & FZ) {
    CYCT(b_+30, b_+31); ret_effect(gb); return;
  }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); A = 0xf1; // TILEINDEX_CHEST
  CALL_C(b_+33, setTile_hook, SYM(setTile), b_+36);
  CALL_C(b_+36, createPuffAt_hook, SYM(createPuffAt), b_+39);
  CYC(b_+39, b_+41); A = 0x4d; // SND_SOLVEPUZZLE
  CYC(b_+41, b_+44); playSound_b00_hook(gb);
  return;

triggerInactive:
  CYC(b_+44, b_+46); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); C = A;
  CYC(b_+48, b_+50); B = wRoomLayout >> 8;
  CYC(b_+50, b_+51); A = mem_rd(gb, BC);
  CYC(b_+51, b_+53); alu_cp(gb, 0xf1); // TILEINDEX_CHEST
  if (!(F & FZ)) {
    CYCT(b_+53, b_+54); ret_effect(gb); return;
  }
  CYC(b_+53, b_+54);
  // Retrieve whatever tile was there before the chest
  CYC(b_+54, b_+56); A = 0x03; // :w3RoomLayoutBuffer
  CYC(b_+56, b_+58); mem_wr(gb, 0xff70, A); // R_SVBK
  CYC(b_+58, b_+60); B = w3RoomLayoutBuffer >> 8;
  CYC(b_+60, b_+61); A = mem_rd(gb, BC);
  CYC(b_+61, b_+62); L = A;
  CYC(b_+62, b_+63); alu_xor(gb, A);
  CYC(b_+63, b_+65); mem_wr(gb, 0xff70, A); // R_SVBK
  CYC(b_+65, b_+66); A = L;
  CALL_C(b_+66, setTile_hook, SYM(setTile), b_+69);
  CYC(b_+69, b_+72); TAIL(createPuffAt);
}

// d3: Calculate the value for [wSwitchState] based on which crystals are broken.
void interaction21_subid18_hook(GB *gb) {
  BASE(interaction21_subid18);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); B = 0x00;

  CYC(b_+5, b_+7); L = 0x5d; // <ROOM_AGES_45d
  CYC(b_+7, b_+9); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+9, b_+11);
  } else {
    CYC(b_+9, b_+11);
    CYC(b_+11, b_+13); B |= 1 << 4;
  }

  CYC(b_+13, b_+15); L = 0x5f; // <ROOM_AGES_45f
  CYC(b_+15, b_+17); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+17, b_+19);
  } else {
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+21); B |= 1 << 5;
  }

  CYC(b_+21, b_+23); L = 0x61; // <ROOM_AGES_461
  CYC(b_+23, b_+25); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+25, b_+27);
  } else {
    CYC(b_+25, b_+27);
    CYC(b_+27, b_+29); B |= 1 << 6;
  }

  CYC(b_+29, b_+31); L = 0x63; // <ROOM_AGES_463
  CYC(b_+31, b_+33); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+33, b_+35);
  } else {
    CYC(b_+33, b_+35);
    CYC(b_+35, b_+37); B |= 1 << 7;
  }

  CYC(b_+37, b_+40); A = mem_rd(gb, wSwitchState);
  CYC(b_+40, b_+41); alu_or(gb, B);
  CYC(b_+41, b_+44); mem_wr(gb, wSwitchState, A);
  CYC(b_+44, b_+47); TAIL(interactionDelete);
}

// Deletes the interaction and returns to the caller's caller if this room's item flag is
// set.
void interactionDeleteAndRetIfItemFlagSet_hook(GB *gb) {
  BASE(interactionDeleteAndRetIfItemFlagSet);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb); return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); SET_HL(pop_effect(gb));
  CYC(b_+7, b_+10); TAIL(interactionDelete);
}

void spawnSmallKeyFromCeiling_hook(GB *gb) {
  BASE(spawnSmallKeyFromCeiling);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x3001); // TREASURE_SMALL_KEY, $01
  CALL_C(b_+3, createTreasure_hook, SYM(createTreasure), b_+6);
  if (!(F & FZ)) {
    CYCT(b_+6, b_+7); ret_effect(gb); return;
  }
  CYC(b_+6, b_+7);
  CALL_C(b_+7, objectCopyPosition_hook, SYM(objectCopyPosition), b_+10);
  CYC(b_+10, b_+13); TAIL(interactionDelete);
}

// Verifies that certain tiles in the room layout equal specified values.
// @param hl Data structure: first byte is a tile index, subsequent bytes are positions where
//           the tile is expected; $ff starts a new group, $00 ends the structure.
// @param[out] zflag Set if the tiles all match the expected values.
void verifyTiles_hook(GB *gb) {
  BASE(verifyTiles);
  CYC(b_+0, b_+2); B = wRoomLayout >> 8;
nextTileIndex:
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+5); ret_effect(gb); return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); E = A;
nextPosition:
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+9, b_+10); ret_effect(gb); return;
  }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+11, b_+13); goto nextTileIndex;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, BC);
  CYC(b_+14, b_+15); alu_cp(gb, E);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+16); ret_effect(gb); return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); goto nextPosition;
}

// @param b Number of frames it can stay lit before burning out
// @param c Position
// @param[out] zflag Set if the part object was created successfully
void makeTorchAtPositionTemporarilyLightable_hook(GB *gb) {
  BASE(makeTorchAtPositionTemporarilyLightable);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+4); ret_effect(gb); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x06); // PART_LIGHTABLE_TORCH
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x01);
  CYC(b_+9, b_+11); L = PART_BASE + OBJ_COUNTER2;
  CYC(b_+11, b_+12); mem_wr(gb, HL, B);
  CYC(b_+12, b_+14); L = PART_BASE + OBJ_YH;
  CALL_C(b_+14, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+17);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+19); ret_effect(gb);
}

// INTERAC_DUNGEON_EVENTS: subid-dispatched one-off dungeon puzzle handlers.
void interactionCode21_hook(GB *gb) {
  BASE(interactionCode21);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dungeonEvents_jumpTable(gb));
    if (jt_ == SYM(interactionDelete) && hook_is(gb, SYM(interactionDelete), interactionDelete_hook)) { interactionDelete_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid01) && hook_is(gb, SYM(interaction21_subid01), interaction21_subid01_hook)) { interaction21_subid01_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid02) && hook_is(gb, SYM(interaction21_subid02), interaction21_subid02_hook)) { interaction21_subid02_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid03) && hook_is(gb, SYM(interaction21_subid03), interaction21_subid03_hook)) { interaction21_subid03_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid04) && hook_is(gb, SYM(interaction21_subid04), interaction21_subid04_hook)) { interaction21_subid04_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid05) && hook_is(gb, SYM(interaction21_subid05), interaction21_subid05_hook)) { interaction21_subid05_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid06) && hook_is(gb, SYM(interaction21_subid06), interaction21_subid06_hook)) { interaction21_subid06_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid07) && hook_is(gb, SYM(interaction21_subid07), interaction21_subid07_hook)) { interaction21_subid07_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid08) && hook_is(gb, SYM(interaction21_subid08), interaction21_subid08_hook)) { interaction21_subid08_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid09) && hook_is(gb, SYM(interaction21_subid09), interaction21_subid09_hook)) { interaction21_subid09_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid0a) && hook_is(gb, SYM(interaction21_subid0a), interaction21_subid0a_hook)) { interaction21_subid0a_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid0b) && hook_is(gb, SYM(interaction21_subid0b), interaction21_subid0b_hook)) { interaction21_subid0b_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid0c) && hook_is(gb, SYM(interaction21_subid0c), interaction21_subid0c_hook)) { interaction21_subid0c_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid0d) && hook_is(gb, SYM(interaction21_subid0d), interaction21_subid0d_hook)) { interaction21_subid0d_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid0e) && hook_is(gb, SYM(interaction21_subid0e), interaction21_subid0e_hook)) { interaction21_subid0e_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid0f) && hook_is(gb, SYM(interaction21_subid0f), interaction21_subid0f_hook)) { interaction21_subid0f_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid10) && hook_is(gb, SYM(interaction21_subid10), interaction21_subid10_hook)) { interaction21_subid10_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid11) && hook_is(gb, SYM(interaction21_subid11), interaction21_subid11_hook)) { interaction21_subid11_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid12) && hook_is(gb, SYM(interaction21_subid12), interaction21_subid12_hook)) { interaction21_subid12_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid13) && hook_is(gb, SYM(interaction21_subid13), interaction21_subid13_hook)) { interaction21_subid13_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid14) && hook_is(gb, SYM(interaction21_subid14), interaction21_subid14_hook)) { interaction21_subid14_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid15) && hook_is(gb, SYM(interaction21_subid15), interaction21_subid15_hook)) { interaction21_subid15_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid16) && hook_is(gb, SYM(interaction21_subid16), interaction21_subid16_hook)) { interaction21_subid16_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid17) && hook_is(gb, SYM(interaction21_subid17), interaction21_subid17_hook)) { interaction21_subid17_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid18) && hook_is(gb, SYM(interaction21_subid18), interaction21_subid18_hook)) { interaction21_subid18_hook(gb); return; }
    else if (jt_ == SYM(interaction21_subid19) && hook_is(gb, SYM(interaction21_subid19), interaction21_subid19_hook)) { interaction21_subid19_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}
