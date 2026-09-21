#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/twinrova.s (INTERAC_TWINROVA), bank 0x0a.
// This is a DIFFERENT, unrelated source file from object_code/common/enemies/twinrova.s
// (ENEMY_TWINROVA, bank 0x10, already ported in src/game/twinrova.c) that happens to share the
// basename "twinrova.s" in a different disasm directory -- kept as its own file to avoid the
// same collision class documented for sparkle.s/sparkleInteraction.c earlier this session.

static uint16_t twinrova_jump_table(GB *gb) {
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

static void twinrova_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void twinrova_loadGfx_hook(GB *gb);
void twinrova_initSubid06_hook(GB *gb);
void twinrova_initSubid02_hook(GB *gb);
void twinrova_initSubid04_hook(GB *gb);
void twinrova_initSubid00_hook(GB *gb);
void twinrova_genericInitialize_hook(GB *gb);
void twinrova_loadAngleAndCounterPreset_hook(GB *gb);
void loadAngleAndCounterPreset_hook(GB *gb);
void twinrova_updateDirectionFromAngle_hook(GB *gb);
void twinrova_initOtherHalf_hook(GB *gb);
void twinrova_takeInvertedPositionFromObject_hook(GB *gb);
void twinrova_state1_hook(GB *gb);
void twinrova_loadScript_hook(GB *gb);
void func_0a_7877_hook(GB *gb);

// ==================================================================================================
// INTERAC_TWINROVA
//
// Variables:
//   var3a: Index for "loadAngleAndCounterPreset" function
// ==================================================================================================
void interactionCode93_hook(GB *gb) {
  BASE(interactionCode93);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = twinrova_jump_table(gb);
    if (target == SYM(twinrova_state1)) { twinrova_state1_hook(gb); return; }
    // target == 0x7604 falls through to state0
  }

  // interactionCode93@state0
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+13, b_+15); goto subid2AndUp; } // jr nc
  CYC(b_+13, b_+15);

  // interactionCode93@subid0Or1
  CYC(b_+15, b_+18); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+18, b_+20); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; } // ret nz
  CYC(b_+20, b_+21);
  CALL_C(b_+21, twinrova_loadGfx_hook, SYM(twinrova_loadGfx), b_+24);
  CYC(b_+24, b_+26); goto afterLoadGfx; // jr

subid2AndUp:
  CYC(b_+26, b_+28); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CALL_C_CC(b_+28, interactionLoadExtraGraphics_hook, SYM(interactionLoadExtraGraphics), b_+31); } else CYC(b_+28, b_+31); // call nz

afterLoadGfx:
  CALL_C(b_+31, interactionIncState_hook, SYM(interactionIncState), b_+34);
  CALL_C(b_+34, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+37);
  CALL_C(b_+37, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+40);
  CYC(b_+40, b_+42); A = 0x28; // >TX_2800
  CALL_C(b_+42, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+45);
  CYC(b_+45, b_+47); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  {
    CYC(b_+48, b_+49); push_effect(gb, b_+49);
    uint16_t target = twinrova_jump_table(gb);
    if (target == SYM(twinrova_initOtherHalf)) { twinrova_initOtherHalf_hook(gb); return; }
    if (target == SYM(twinrova_initSubid02)) { twinrova_initSubid02_hook(gb); return; }
    if (target == SYM(twinrova_initSubid04)) { twinrova_initSubid04_hook(gb); return; }
    if (target == SYM(twinrova_initSubid06)) { twinrova_initSubid06_hook(gb); return; }
    TAIL(twinrova_initSubid00); // target == 0x7675
  }
}

void twinrova_loadGfx_hook(GB *gb) {
  BASE(twinrova_loadGfx);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wLoadedObjectGfx + 10);
  CYC(b_+3, b_+5); B = 0x03;
  CYC(b_+5, b_+7); A = 0x2c; // AGES_OBJ_GFXH_2c

loop:
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x01);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto loop; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); push_effect(gb, DE); // push de
  CALL_C(b_+16, reloadObjectGfx_b00_hook, SYM(reloadObjectGfx_b00), b_+19);
  CYC(b_+19, b_+20); SET_DE(pop_effect(gb)); // pop de
  RET(b_+20); return;
}

void twinrova_initSubid06_hook(GB *gb) {
  BASE(twinrova_initSubid06);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x00);
  CALL_C(b_+5, twinrova_loadScript_hook, SYM(twinrova_loadScript), b_+8);
  CYC(b_+8, b_+11); SET_BC((SYM(interactionCode7a__updateLinkPositionWhileRollerMoving) + 30));
  CYC(b_+11, b_+13); TAIL(twinrova_genericInitialize); // jr
}

void twinrova_initSubid02_hook(GB *gb) {
  BASE(twinrova_initSubid02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x04);
  CYC(b_+5, b_+7); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x02);
  CALL_C(b_+9, objectSetInvisible_hook, SYM(objectSetInvisible), b_+12);
  CYC(b_+12, b_+15); SET_BC(0x3850);
  CYC(b_+15, b_+17); TAIL(twinrova_genericInitialize); // jr
}

void twinrova_initSubid04_hook(GB *gb) {
  BASE(twinrova_initSubid04);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x1e);
  TAIL(twinrova_initSubid00); // falls through
}

void twinrova_initSubid00_hook(GB *gb) {
  BASE(twinrova_initSubid00);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x00);
  CYC(b_+5, b_+8); SET_BC(0xf888);
  TAIL(twinrova_genericInitialize); // falls through
}

void twinrova_genericInitialize_hook(GB *gb) {
  BASE(twinrova_genericInitialize);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionSetPosition_hook, SYM(interactionSetPosition), b_+3);
  CALL_C(b_+3, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+6);
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x02);
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+14, b_+16); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xf8); // -0x08

  // Spawn the other half (subid+1)
  CALL_C(b_+18, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+21);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto afterSpawn; } // jr nz
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x93); // INTERAC_TWINROVA
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);
  CYC(b_+26, b_+27); E = L;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); A = alu_inc8(gb, A);
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);
  CYC(b_+30, b_+32); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+32, b_+34); mem_wr(gb, HL, INTERACTION_BASE); // Interaction.start
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+36); mem_wr(gb, HL, D);

afterSpawn:
  CALL_C(b_+36, twinrova_loadAngleAndCounterPreset_hook, SYM(twinrova_loadAngleAndCounterPreset), b_+39);
  CALL_C(b_+39, twinrova_updateDirectionFromAngle_hook, SYM(twinrova_updateDirectionFromAngle), b_+42);
  CYC(b_+42, b_+44); A = 0xbb; // SND_BEAM2
  CALL_C(b_+44, playSound_b00_hook, SYM(playSound_b00), b_+47);
  CYC(b_+47, b_+50); SET_HL((SYM(interactionCode92) + 8)); // scriptHelp.objectWritePositionTocfd5
  CYC(b_+50, b_+52); E = 0x15; // bank of scriptHelp
  CYC(b_+52, b_+55); TAIL(interBankCall); // jp
}

void twinrova_loadAngleAndCounterPreset_hook(GB *gb) {
  BASE(twinrova_loadAngleAndCounterPreset);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  TAIL(loadAngleAndCounterPreset); // falls through
}

// Loads preset values for angle and counter1 variables for an interaction. The values it
// loads depend on parameter b (the preset index) and Interaction.counter2 (the index in
// the preset to use). presetInteractionAnglesAndCounters (0a:771d) is a pure ROM data
// table -- 6 pointers plus data0-data5 byte pairs -- referenced here only by address, not
// modeled as C.
void loadAngleAndCounterPreset_hook(GB *gb) {
  BASE(loadAngleAndCounterPreset);
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+4); SET_HL(SYM(presetInteractionAnglesAndCounters)); // presetInteractionAnglesAndCounters
  CYC(b_+4, b_+5); twinrova_addDoubleIndexToHl_from_rst(gb, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+6, b_+7); H = mem_rd(gb, HL);
  CYC(b_+7, b_+8); L = A;
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); twinrova_addDoubleIndexToHl_from_rst(gb, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_or(gb, A);
  CYC(b_+18, b_+19); B = A;
  if (F & FZ) { RET_TAKEN(b_+19); return; } // ret z
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+25, b_+27); alu_or(gb, 0x01);
  RET(b_+27); return;
}

void twinrova_updateDirectionFromAngle_hook(GB *gb) {
  BASE(twinrova_updateDirectionFromAngle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_ANGLE;
  CALL_C(b_+2, convertAngleDeToDirection_hook, SYM(convertAngleDeToDirection), b_+5);
  CYC(b_+5, b_+7); alu_and(gb, 0x03);
  CYC(b_+7, b_+9); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+9, b_+10); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+10); return; } // ret z
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+15); TAIL(interactionSetAnimation); // jp
}

// Initialize odd subids (the half of twinrova that just follows along)
void twinrova_initOtherHalf_hook(GB *gb) {
  BASE(twinrova_initOtherHalf);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+3);
  CYC(b_+3, b_+5); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x01);

  // Copy position & stuff from other half, inverted if necessary
  CYC(b_+7, b_+9); A = OBJ_ENABLED;
  CALL_C(b_+9, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), SYM(twinrova_takeInvertedPositionFromObject));
  TAIL(twinrova_takeInvertedPositionFromObject); // falls through
}

// @param h Object to copy visibility, direction, position from
void twinrova_takeInvertedPositionFromObject_hook(GB *gb) {
  BASE(twinrova_takeInvertedPositionFromObject);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+2, b_+3); E = L;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CALL_C(b_+5, objectTakePosition_hook, SYM(objectTakePosition), b_+8);
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+10, b_+11); B = mem_rd(gb, HL);
  CYC(b_+11, b_+13); A = 0x50;
  CYC(b_+13, b_+14); alu_sub(gb, B);
  CYC(b_+14, b_+16); alu_add(gb, 0x50);
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); B = A;
  CYC(b_+23, b_+25); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+25, b_+27); goto setDirection; } // jr z
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); A = B;
  CYC(b_+28, b_+30); B = 0x01;
  CYC(b_+30, b_+32); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+32, b_+34); goto setDirection; } // jr z
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); B = 0x03;

setDirection:
  CYC(b_+36, b_+37); A = B;
  CYC(b_+37, b_+38); H = D;
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+40, b_+41); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+41); return; } // ret z
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A);
  CYC(b_+43, b_+46); TAIL(interactionSetAnimation); // jp
}

// presetInteractionAnglesAndCounters (0a:771d): pure ROM data, not ported as code -- 6
// pointers to lists of (angle, counter1) byte pairs terminated by 0x00 0x00. Referenced
// only via SET_HL(0x771d) in loadAngleAndCounterPreset_hook above.

void twinrova_state1_hook(GB *gb) {
  BASE(twinrova_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = twinrova_jump_table(gb);
    if (target == b_+126) goto runOtherHalf;
    if (target == b_+142) goto runSubid02;
    if (target == b_+165) goto runSubid06;
    // target == 0x77c3 falls through to runSubid00
  }

  // twinrova_state1@runSubid00
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  {
    CYC(b_+23, b_+24); push_effect(gb, b_+24);
    uint16_t target = twinrova_jump_table(gb);
    if (target == b_+59) goto subid00State1;
    if (target == b_+95) goto subid00State2;
    // target == 0x77cd falls through to subid00State0
  }

subid00State0:
  CYC(b_+30, b_+33); SET_HL((SYM(interactionCode92) + 8)); // scriptHelp.objectWritePositionTocfd5
  CYC(b_+33, b_+35); E = 0x15; // bank of scriptHelp
  CALL_C(b_+35, interBankCall_hook, 0x008a, b_+38);
  CALL_C(b_+38, interactionAnimate_hook, SYM(interactionAnimate), b_+41);
  CALL_C(b_+41, objectApplySpeed_hook, SYM(objectApplySpeed), b_+44);
  CALL_C(b_+44, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+47);
  if (F & FZ) { CALL_C_CC(b_+47, twinrova_loadAngleAndCounterPreset_hook, SYM(twinrova_loadAngleAndCounterPreset), b_+50); } else CYC(b_+47, b_+50); // call z
  if (!(F & FZ)) { CYCT(b_+50, b_+53); twinrova_updateDirectionFromAngle_hook(gb); return; } // jp nz
  CYC(b_+50, b_+53);
  CALL_C(b_+53, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+56);
  CYC(b_+56, b_+59); TAIL(twinrova_loadScript); // jp

subid00State1:
  CALL_C(b_+59, interactionAnimate_hook, SYM(interactionAnimate), b_+62);
  CALL_C(b_+62, objectOscillateZ_hook, SYM(objectOscillateZ), b_+65);
  CALL_C(b_+65, interactionRunScript_hook, SYM(interactionRunScript), b_+68);
  if (!(F & FC)) { RET_TAKEN(b_+68); return; } // ret nc
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); A = 0xbb; // SND_BEAM2
  CALL_C(b_+71, playSound_b00_hook, SYM(playSound_b00), b_+74);
  CYC(b_+74, b_+77); SET_HL((SYM(interactionCode92) + 8)); // scriptHelp.objectWritePositionTocfd5
  CYC(b_+77, b_+79); E = 0x15; // bank of scriptHelp
  CALL_C(b_+79, interBankCall_hook, 0x008a, b_+82);
  CALL_C(b_+82, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+85);
  CYC(b_+85, b_+87); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x00);
  CYC(b_+89, b_+91); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+91, b_+92); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+92, b_+95); TAIL(twinrova_loadAngleAndCounterPreset); // jp

subid00State2:
  CYC(b_+95, b_+98); SET_HL((SYM(interactionCode92) + 8)); // scriptHelp.objectWritePositionTocfd5
  CYC(b_+98, b_+100); E = 0x15; // bank of scriptHelp
  CALL_C(b_+100, interBankCall_hook, 0x008a, b_+103);
  CALL_C(b_+103, interactionAnimate_hook, SYM(interactionAnimate), b_+106);
  CALL_C(b_+106, objectApplySpeed_hook, SYM(objectApplySpeed), b_+109);
  CALL_C(b_+109, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+112);
  if (F & FZ) { CALL_C_CC(b_+112, twinrova_loadAngleAndCounterPreset_hook, SYM(twinrova_loadAngleAndCounterPreset), b_+115); } else CYC(b_+112, b_+115); // call z
  if (!(F & FZ)) { CYCT(b_+115, b_+118); twinrova_updateDirectionFromAngle_hook(gb); return; } // jp nz
  CYC(b_+115, b_+118);
  CYC(b_+118, b_+120); A = 0x09;
  CYC(b_+120, b_+123); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(b_+123, b_+126); TAIL(interactionDelete); // jp

runOtherHalf:
  CALL_C(b_+126, interactionAnimate_hook, SYM(interactionAnimate), b_+129);
  CYC(b_+129, b_+131); A = OBJ_ENABLED;
  CALL_C(b_+131, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+134);
  CYC(b_+134, b_+135); A = mem_rd(gb, HL);
  CYC(b_+135, b_+136); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+136, b_+139); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+136, b_+139);
  CYC(b_+139, b_+142); TAIL(twinrova_takeInvertedPositionFromObject); // jp

runSubid02: // also runSubid04 (same address; both subid values reuse this dispatch)
  CYC(b_+142, b_+144); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+144, b_+145); A = mem_rd(gb, DE);
  {
    CYC(b_+145, b_+146); push_effect(gb, b_+146);
    uint16_t target = twinrova_jump_table(gb);
    if (target == b_+30) goto subid00State0;
    if (target == b_+59) goto subid00State1;
    if (target == b_+95) goto subid00State2;
    // target == 0x7849 falls through to subid02State0
  }

  // twinrova_state1@subid02State0
  CYC(b_+154, b_+155); H = D;
  CYC(b_+155, b_+157); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+157, b_+158); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { RET_TAKEN(b_+158); return; } // ret nz
  CYC(b_+158, b_+159);
  CALL_C(b_+159, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+162);
  CYC(b_+162, b_+165); TAIL(interactionIncSubstate); // jp

runSubid06:
  CYC(b_+165, b_+167); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+167, b_+168); A = mem_rd(gb, DE);
  CYC(b_+168, b_+169); push_effect(gb, b_+169);
  {
    uint16_t target = twinrova_jump_table(gb);
    if (target == b_+95) goto subid00State2;
    goto subid00State1; // target == 0x77ea
  }
}

void twinrova_loadScript_hook(GB *gb) {
  BASE(twinrova_loadScript);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+13); // @scriptTable
  CYC(b_+6, b_+7); twinrova_addDoubleIndexToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); TAIL(interactionSetScript); // jp
}

// Gets a position stored in wTmpcfc0_genericCutscene_cfd5/cfd6
// @param[out] bc Position
void func_0a_7877_hook(GB *gb) {
  BASE(func_0a_7877);
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0_genericCutscene_cfd5);
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); C = mem_rd(gb, HL);
  RET(b_+6); return;
}
