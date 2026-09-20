#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x75fc, 0x75fe); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x75fe, 0x75ff); A = mem_rd(gb, DE);
  {
    CYC(0x75ff, 0x7600); push_effect(gb, 0x7600);
    uint16_t target = twinrova_jump_table(gb);
    if (target == 0x77af) { twinrova_state1_hook(gb); return; }
    // target == 0x7604 falls through to state0
  }

  // interactionCode93@state0
  CYC(0x7604, 0x7606); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7606, 0x7607); A = mem_rd(gb, DE);
  CYC(0x7607, 0x7609); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(0x7609, 0x760b); goto subid2AndUp; } // jr nc
  CYC(0x7609, 0x760b);

  // interactionCode93@subid0Or1
  CYC(0x760b, 0x760e); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x760e, 0x7610); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(0x7610); return; } // ret nz
  CYC(0x7610, 0x7611);
  CALL_C(0x7611, twinrova_loadGfx_hook, 0x763d, 0x7614);
  CYC(0x7614, 0x7616); goto afterLoadGfx; // jr

subid2AndUp:
  CYC(0x7616, 0x7618); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CALL_C_CC(0x7618, interactionLoadExtraGraphics_hook, 0x2781, 0x761b); } else CYC(0x7618, 0x761b); // call nz

afterLoadGfx:
  CALL_C(0x761b, interactionIncState_hook, 0x23e0, 0x761e);
  CALL_C(0x761e, interactionInitGraphics_hook, 0x15fb, 0x7621);
  CALL_C(0x7621, objectSetVisiblec1_hook, 0x1e3c, 0x7624);
  CYC(0x7624, 0x7626); A = 0x28; // >TX_2800
  CALL_C(0x7626, interactionSetHighTextIndex_hook, 0x253b, 0x7629);
  CYC(0x7629, 0x762b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x762b, 0x762c); A = mem_rd(gb, DE);
  {
    CYC(0x762c, 0x762d); push_effect(gb, 0x762d);
    uint16_t target = twinrova_jump_table(gb);
    if (target == 0x76e3) { twinrova_initOtherHalf_hook(gb); return; }
    if (target == 0x765f) { twinrova_initSubid02_hook(gb); return; }
    if (target == 0x7670) { twinrova_initSubid04_hook(gb); return; }
    if (target == 0x7652) { twinrova_initSubid06_hook(gb); return; }
    twinrova_initSubid00_hook(gb); return; // target == 0x7675
  }
}

void twinrova_loadGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x763d, 0x7640); SET_HL(wLoadedObjectGfx + 10);
  CYC(0x7640, 0x7642); B = 0x03;
  CYC(0x7642, 0x7644); A = 0x2c; // AGES_OBJ_GFXH_2c

loop:
  CYC(0x7644, 0x7645); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7645, 0x7646); A = alu_inc8(gb, A);
  CYC(0x7646, 0x7648); mem_wr(gb, HL, 0x01);
  CYC(0x7648, 0x7649); L = alu_inc8(gb, L);
  CYC(0x7649, 0x764a); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x764a, 0x764c); goto loop; } // jr nz
  CYC(0x764a, 0x764c);
  CYC(0x764c, 0x764d); push_effect(gb, DE); // push de
  CALL_C(0x764d, reloadObjectGfx_b00_hook, 0x1630, 0x7650);
  CYC(0x7650, 0x7651); SET_DE(pop_effect(gb)); // pop de
  RET(0x7651); return;
}

void twinrova_initSubid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7652, 0x7653); H = D;
  CYC(0x7653, 0x7655); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x7655, 0x7657); mem_wr(gb, HL, 0x00);
  CALL_C(0x7657, twinrova_loadScript_hook, 0x785c, 0x765a);
  CYC(0x765a, 0x765d); SET_BC(0x4234);
  CYC(0x765d, 0x765f); twinrova_genericInitialize_hook(gb); return; // jr
}

void twinrova_initSubid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x765f, 0x7660); H = D;
  CYC(0x7660, 0x7662); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x7662, 0x7664); mem_wr(gb, HL, 0x04);
  CYC(0x7664, 0x7666); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x7666, 0x7668); mem_wr(gb, HL, 0x02);
  CALL_C(0x7668, objectSetInvisible_hook, 0x1e7b, 0x766b);
  CYC(0x766b, 0x766e); SET_BC(0x3850);
  CYC(0x766e, 0x7670); twinrova_genericInitialize_hook(gb); return; // jr
}

void twinrova_initSubid04_hook(GB *gb) {
  CYC(0x7670, 0x7671); H = D;
  CYC(0x7671, 0x7673); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x7673, 0x7675); mem_wr(gb, HL, 0x1e);
  twinrova_initSubid00_hook(gb); return; // falls through
}

void twinrova_initSubid00_hook(GB *gb) {
  CYC(0x7675, 0x7676); H = D;
  CYC(0x7676, 0x7678); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x7678, 0x767a); mem_wr(gb, HL, 0x00);
  CYC(0x767a, 0x767d); SET_BC(0xf888);
  twinrova_genericInitialize_hook(gb); return; // falls through
}

void twinrova_genericInitialize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x767d, interactionSetPosition_hook, 0x2773, 0x7680);
  CALL_C(0x7680, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7683);
  CYC(0x7683, 0x7685); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7685, 0x7687); mem_wr(gb, HL, 0x02);
  CYC(0x7687, 0x7689); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7689, 0x768b); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x768b, 0x768d); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x768d, 0x768f); mem_wr(gb, HL, 0xf8); // -0x08

  // Spawn the other half (subid+1)
  CALL_C(0x768f, getFreeInteractionSlot_hook, 0x3aef, 0x7692);
  if (!(F & FZ)) { CYCT(0x7692, 0x7694); goto afterSpawn; } // jr nz
  CYC(0x7692, 0x7694);
  CYC(0x7694, 0x7696); mem_wr(gb, HL, 0x93); // INTERAC_TWINROVA
  CYC(0x7696, 0x7697); L = alu_inc8(gb, L);
  CYC(0x7697, 0x7698); E = L;
  CYC(0x7698, 0x7699); A = mem_rd(gb, DE);
  CYC(0x7699, 0x769a); A = alu_inc8(gb, A);
  CYC(0x769a, 0x769b); mem_wr(gb, HL, A);
  CYC(0x769b, 0x769d); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x769d, 0x769f); mem_wr(gb, HL, INTERACTION_BASE); // Interaction.start
  CYC(0x769f, 0x76a0); L = alu_inc8(gb, L);
  CYC(0x76a0, 0x76a1); mem_wr(gb, HL, D);

afterSpawn:
  CALL_C(0x76a1, twinrova_loadAngleAndCounterPreset_hook, 0x76b4, 0x76a4);
  CALL_C(0x76a4, twinrova_updateDirectionFromAngle_hook, 0x76d4, 0x76a7);
  CYC(0x76a7, 0x76a9); A = 0xbb; // SND_BEAM2
  CALL_C(0x76a9, playSound_b00_hook, 0x0c98, 0x76ac);
  CYC(0x76ac, 0x76af); SET_HL(0x741b); // scriptHelp.objectWritePositionTocfd5
  CYC(0x76af, 0x76b1); E = 0x15; // bank of scriptHelp
  CYC(0x76b1, 0x76b4); interBankCall_hook(gb); return; // jp
}

void twinrova_loadAngleAndCounterPreset_hook(GB *gb) {
  CYC(0x76b4, 0x76b6); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x76b6, 0x76b7); A = mem_rd(gb, DE);
  CYC(0x76b7, 0x76b8); B = A;
  loadAngleAndCounterPreset_hook(gb); return; // falls through
}

// Loads preset values for angle and counter1 variables for an interaction. The values it
// loads depend on parameter b (the preset index) and Interaction.counter2 (the index in
// the preset to use). presetInteractionAnglesAndCounters (0a:771d) is a pure ROM data
// table -- 6 pointers plus data0-data5 byte pairs -- referenced here only by address, not
// modeled as C.
void loadAngleAndCounterPreset_hook(GB *gb) {
  CYC(0x76b8, 0x76b9); A = B;
  CYC(0x76b9, 0x76bc); SET_HL(0x771d); // presetInteractionAnglesAndCounters
  CYC(0x76bc, 0x76bd); twinrova_addDoubleIndexToHl_from_rst(gb, 0x76bd);
  CYC(0x76bd, 0x76be); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x76be, 0x76bf); H = mem_rd(gb, HL);
  CYC(0x76bf, 0x76c0); L = A;
  CYC(0x76c0, 0x76c2); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x76c2, 0x76c3); A = mem_rd(gb, DE);
  CYC(0x76c3, 0x76c4); twinrova_addDoubleIndexToHl_from_rst(gb, 0x76c4);
  CYC(0x76c4, 0x76c5); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x76c5, 0x76c7); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x76c7, 0x76c8); mem_wr(gb, DE, A);
  CYC(0x76c8, 0x76c9); A = mem_rd(gb, HL);
  CYC(0x76c9, 0x76ca); alu_or(gb, A);
  CYC(0x76ca, 0x76cb); B = A;
  if (F & FZ) { RET_TAKEN(0x76cb); return; } // ret z
  CYC(0x76cb, 0x76cc);
  CYC(0x76cc, 0x76cd); H = D;
  CYC(0x76cd, 0x76cf); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x76cf, 0x76d0); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x76d0, 0x76d1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x76d1, 0x76d3); alu_or(gb, 0x01);
  RET(0x76d3); return;
}

void twinrova_updateDirectionFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x76d4, 0x76d6); E = INTERACTION_BASE + OBJ_ANGLE;
  CALL_C(0x76d6, convertAngleDeToDirection_hook, 0x26f8, 0x76d9);
  CYC(0x76d9, 0x76db); alu_and(gb, 0x03);
  CYC(0x76db, 0x76dd); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x76dd, 0x76de); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x76de); return; } // ret z
  CYC(0x76de, 0x76df);
  CYC(0x76df, 0x76e0); mem_wr(gb, HL, A);
  CYC(0x76e0, 0x76e3); interactionSetAnimation_hook(gb); return; // jp
}

// Initialize odd subids (the half of twinrova that just follows along)
void twinrova_initOtherHalf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x76e3, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x76e6);
  CYC(0x76e6, 0x76e8); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x76e8, 0x76ea); mem_wr(gb, HL, 0x01);

  // Copy position & stuff from other half, inverted if necessary
  CYC(0x76ea, 0x76ec); A = OBJ_ENABLED;
  CALL_C(0x76ec, objectGetRelatedObject1Var_hook, 0x2160, 0x76ef);
  twinrova_takeInvertedPositionFromObject_hook(gb); return; // falls through
}

// @param h Object to copy visibility, direction, position from
void twinrova_takeInvertedPositionFromObject_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x76ef, 0x76f1); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x76f1, 0x76f2); E = L;
  CYC(0x76f2, 0x76f3); A = mem_rd(gb, HL);
  CYC(0x76f3, 0x76f4); mem_wr(gb, DE, A);
  CALL_C(0x76f4, objectTakePosition_hook, 0x2274, 0x76f7);
  CYC(0x76f7, 0x76f9); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x76f9, 0x76fa); B = mem_rd(gb, HL);
  CYC(0x76fa, 0x76fc); A = 0x50;
  CYC(0x76fc, 0x76fd); alu_sub(gb, B);
  CYC(0x76fd, 0x76ff); alu_add(gb, 0x50);
  CYC(0x76ff, 0x7701); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x7701, 0x7702); mem_wr(gb, DE, A);
  CYC(0x7702, 0x7704); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x7704, 0x7705); A = mem_rd(gb, HL);
  CYC(0x7705, 0x7706); B = A;
  CYC(0x7706, 0x7708); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(0x7708, 0x770a); goto setDirection; } // jr z
  CYC(0x7708, 0x770a);
  CYC(0x770a, 0x770b); A = B;
  CYC(0x770b, 0x770d); B = 0x01;
  CYC(0x770d, 0x770f); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(0x770f, 0x7711); goto setDirection; } // jr z
  CYC(0x770f, 0x7711);
  CYC(0x7711, 0x7713); B = 0x03;

setDirection:
  CYC(0x7713, 0x7714); A = B;
  CYC(0x7714, 0x7715); H = D;
  CYC(0x7715, 0x7717); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x7717, 0x7718); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x7718); return; } // ret z
  CYC(0x7718, 0x7719);
  CYC(0x7719, 0x771a); mem_wr(gb, HL, A);
  CYC(0x771a, 0x771d); interactionSetAnimation_hook(gb); return; // jp
}

// presetInteractionAnglesAndCounters (0a:771d): pure ROM data, not ported as code -- 6
// pointers to lists of (angle, counter1) byte pairs terminated by 0x00 0x00. Referenced
// only via SET_HL(0x771d) in loadAngleAndCounterPreset_hook above.

void twinrova_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x77af, 0x77b1); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x77b1, 0x77b2); A = mem_rd(gb, DE);
  {
    CYC(0x77b2, 0x77b3); push_effect(gb, 0x77b3);
    uint16_t target = twinrova_jump_table(gb);
    if (target == 0x782d) goto runOtherHalf;
    if (target == 0x783d) goto runSubid02;
    if (target == 0x7854) goto runSubid06;
    // target == 0x77c3 falls through to runSubid00
  }

  // twinrova_state1@runSubid00
  CYC(0x77c3, 0x77c5); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x77c5, 0x77c6); A = mem_rd(gb, DE);
  {
    CYC(0x77c6, 0x77c7); push_effect(gb, 0x77c7);
    uint16_t target = twinrova_jump_table(gb);
    if (target == 0x77ea) goto subid00State1;
    if (target == 0x780e) goto subid00State2;
    // target == 0x77cd falls through to subid00State0
  }

subid00State0:
  CYC(0x77cd, 0x77d0); SET_HL(0x741b); // scriptHelp.objectWritePositionTocfd5
  CYC(0x77d0, 0x77d2); E = 0x15; // bank of scriptHelp
  CALL_C(0x77d2, interBankCall_hook, 0x008a, 0x77d5);
  CALL_C(0x77d5, interactionAnimate_hook, 0x261b, 0x77d8);
  CALL_C(0x77d8, objectApplySpeed_hook, 0x201d, 0x77db);
  CALL_C(0x77db, interactionDecCounter1_hook, 0x23cc, 0x77de);
  if (F & FZ) { CALL_C_CC(0x77de, twinrova_loadAngleAndCounterPreset_hook, 0x76b4, 0x77e1); } else CYC(0x77de, 0x77e1); // call z
  if (!(F & FZ)) { CYCT(0x77e1, 0x77e4); twinrova_updateDirectionFromAngle_hook(gb); return; } // jp nz
  CYC(0x77e1, 0x77e4);
  CALL_C(0x77e4, interactionIncSubstate_hook, 0x23e5, 0x77e7);
  CYC(0x77e7, 0x77ea); twinrova_loadScript_hook(gb); return; // jp

subid00State1:
  CALL_C(0x77ea, interactionAnimate_hook, 0x261b, 0x77ed);
  CALL_C(0x77ed, objectOscillateZ_hook, 0x27a0, 0x77f0);
  CALL_C(0x77f0, interactionRunScript_hook, 0x2552, 0x77f3);
  if (!(F & FC)) { RET_TAKEN(0x77f3); return; } // ret nc
  CYC(0x77f3, 0x77f4);
  CYC(0x77f4, 0x77f6); A = 0xbb; // SND_BEAM2
  CALL_C(0x77f6, playSound_b00_hook, 0x0c98, 0x77f9);
  CYC(0x77f9, 0x77fc); SET_HL(0x741b); // scriptHelp.objectWritePositionTocfd5
  CYC(0x77fc, 0x77fe); E = 0x15; // bank of scriptHelp
  CALL_C(0x77fe, interBankCall_hook, 0x008a, 0x7801);
  CALL_C(0x7801, interactionIncSubstate_hook, 0x23e5, 0x7804);
  CYC(0x7804, 0x7806); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x7806, 0x7808); mem_wr(gb, HL, 0x00);
  CYC(0x7808, 0x780a); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x780a, 0x780b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x780b, 0x780e); twinrova_loadAngleAndCounterPreset_hook(gb); return; // jp

subid00State2:
  CYC(0x780e, 0x7811); SET_HL(0x741b); // scriptHelp.objectWritePositionTocfd5
  CYC(0x7811, 0x7813); E = 0x15; // bank of scriptHelp
  CALL_C(0x7813, interBankCall_hook, 0x008a, 0x7816);
  CALL_C(0x7816, interactionAnimate_hook, 0x261b, 0x7819);
  CALL_C(0x7819, objectApplySpeed_hook, 0x201d, 0x781c);
  CALL_C(0x781c, interactionDecCounter1_hook, 0x23cc, 0x781f);
  if (F & FZ) { CALL_C_CC(0x781f, twinrova_loadAngleAndCounterPreset_hook, 0x76b4, 0x7822); } else CYC(0x781f, 0x7822); // call z
  if (!(F & FZ)) { CYCT(0x7822, 0x7825); twinrova_updateDirectionFromAngle_hook(gb); return; } // jp nz
  CYC(0x7822, 0x7825);
  CYC(0x7825, 0x7827); A = 0x09;
  CYC(0x7827, 0x782a); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(0x782a, 0x782d); interactionDelete_hook(gb); return; // jp

runOtherHalf:
  CALL_C(0x782d, interactionAnimate_hook, 0x261b, 0x7830);
  CYC(0x7830, 0x7832); A = OBJ_ENABLED;
  CALL_C(0x7832, objectGetRelatedObject1Var_hook, 0x2160, 0x7835);
  CYC(0x7835, 0x7836); A = mem_rd(gb, HL);
  CYC(0x7836, 0x7837); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7837, 0x783a); interactionDelete_hook(gb); return; } // jp z
  CYC(0x7837, 0x783a);
  CYC(0x783a, 0x783d); twinrova_takeInvertedPositionFromObject_hook(gb); return; // jp

runSubid02: // also runSubid04 (same address; both subid values reuse this dispatch)
  CYC(0x783d, 0x783f); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x783f, 0x7840); A = mem_rd(gb, DE);
  {
    CYC(0x7840, 0x7841); push_effect(gb, 0x7841);
    uint16_t target = twinrova_jump_table(gb);
    if (target == 0x77cd) goto subid00State0;
    if (target == 0x77ea) goto subid00State1;
    if (target == 0x780e) goto subid00State2;
    // target == 0x7849 falls through to subid02State0
  }

  // twinrova_state1@subid02State0
  CYC(0x7849, 0x784a); H = D;
  CYC(0x784a, 0x784c); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x784c, 0x784d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { RET_TAKEN(0x784d); return; } // ret nz
  CYC(0x784d, 0x784e);
  CALL_C(0x784e, objectSetVisiblec1_hook, 0x1e3c, 0x7851);
  CYC(0x7851, 0x7854); interactionIncSubstate_hook(gb); return; // jp

runSubid06:
  CYC(0x7854, 0x7856); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7856, 0x7857); A = mem_rd(gb, DE);
  CYC(0x7857, 0x7858); push_effect(gb, 0x7858);
  {
    uint16_t target = twinrova_jump_table(gb);
    if (target == 0x780e) goto subid00State2;
    goto subid00State1; // target == 0x77ea
  }
}

void twinrova_loadScript_hook(GB *gb) {
  CYC(0x785c, 0x785e); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x785e, 0x785f); A = mem_rd(gb, DE);
  CYC(0x785f, 0x7862); SET_HL(0x7869); // @scriptTable
  CYC(0x7862, 0x7863); twinrova_addDoubleIndexToHl_from_rst(gb, 0x7863);
  CYC(0x7863, 0x7864); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7864, 0x7865); H = mem_rd(gb, HL);
  CYC(0x7865, 0x7866); L = A;
  CYC(0x7866, 0x7869); interactionSetScript_hook(gb); return; // jp
}

// Gets a position stored in wTmpcfc0_genericCutscene_cfd5/cfd6
// @param[out] bc Position
void func_0a_7877_hook(GB *gb) {
  CYC(0x7877, 0x787a); SET_HL(wTmpcfc0_genericCutscene_cfd5);
  CYC(0x787a, 0x787b); B = mem_rd(gb, HL);
  CYC(0x787b, 0x787c); L = alu_inc8(gb, L);
  CYC(0x787c, 0x787d); C = mem_rd(gb, HL);
  RET(0x787d); return;
}
