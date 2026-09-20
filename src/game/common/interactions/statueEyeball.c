#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/common/interactions/statueEyeball.s (INTERAC_STATUE_EYEBALL), bank $10.

static uint16_t interactionCodee2_jump_table(GB *gb) {
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

static void interactionCodee2_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void interactionCodee2__getDirectionToFace_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6fb0, objectGetAngleTowardLink_hook, 0x1e9c, 0x6fb3);
  CYC(0x6fb3, 0x6fb4); B = A;
  CYC(0x6fb4, 0x6fb6); alu_and(gb, 0x07);
  if (F & FZ) {
    CYCT(0x6fb6, 0x6fb8);
    goto returnValue;
  }
  CYC(0x6fb6, 0x6fb8);
  CYC(0x6fb8, 0x6fba); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(0x6fba, 0x6fbc);
    goto returnValue;
  }
  CYC(0x6fba, 0x6fbc);
  CYC(0x6fbc, 0x6fbe); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(0x6fbe, 0x6fc0);
    goto returnValue;
  }
  CYC(0x6fbe, 0x6fc0);
  CYC(0x6fc0, 0x6fc1); A = B;
  CYC(0x6fc1, 0x6fc3); alu_and(gb, 0xfc);
  CYC(0x6fc3, 0x6fc5); alu_or(gb, 0x04);
  CYC(0x6fc5, 0x6fc6); B = A;
returnValue:
  CYC(0x6fc6, 0x6fc7); A = B;
  CYC(0x6fc7, 0x6fc8); alu_rrca(gb);
  CYC(0x6fc8, 0x6fc9); alu_rrca(gb);
  CYC(0x6fc9, 0x6fcb); alu_and(gb, 0x07);
  RET(0x6fcb); return;
}

void interactionCodee2__centerOnTileAndGetDirectionToFace_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6fad, objectCenterOnTile_hook, 0x20db, 0x6fb0);
  interactionCodee2__getDirectionToFace_hook(gb);
}

void interactionCodee2__spawnChild_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6fdd, getFreeInteractionSlot_hook, 0x3aef, 0x6fe0);
  if (!(F & FZ)) { RET_TAKEN(0x6fe0); return; }
  CYC(0x6fe0, 0x6fe1);
  CYC(0x6fe1, 0x6fe3); mem_wr(gb, HL, 0xe2);
  CYC(0x6fe3, 0x6fe4); L = alu_inc8(gb, L);
  CYC(0x6fe4, 0x6fe5); mem_wr(gb, HL, E);
  CYC(0x6fe5, 0x6fe6); push_effect(gb, BC);
  CALL_C(0x6fe6, convertShortToLongPosition_paramC_hook, 0x20cc, 0x6fe9);
  CYC(0x6fe9, 0x6feb); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x6feb, 0x6fec); B = alu_dec8(gb, B);
  CYC(0x6fec, 0x6fed); B = alu_dec8(gb, B);
  CYC(0x6fed, 0x6fee); mem_wr(gb, HL, B);
  CYC(0x6fee, 0x6fef); L = alu_inc8(gb, L);
  CYC(0x6fef, 0x6ff0); L = alu_inc8(gb, L);
  CYC(0x6ff0, 0x6ff1); mem_wr(gb, HL, C);
  CYC(0x6ff1, 0x6ff2); SET_BC(pop_effect(gb));
  RET(0x6ff2); return;
}

void interactionCodee2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6f53, 0x6f55); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6f55, 0x6f56); A = mem_rd(gb, DE);
  CYC(0x6f56, 0x6f57); push_effect(gb, 0x6f57);
  switch (interactionCodee2_jump_table(gb)) {
    case 0x6f61: goto subid0;
    case 0x6f7b: goto subid2;
    case 0x6fcc: goto subid1;
    case 0x6ff3: goto subid3;
    case 0x700f: goto subid4;
    default: hook_continue(gb, HL, sp0_); return;
  }
subid0:
  CALL_C(0x6f61, checkInteractionState_hook, 0x23fe, 0x6f64);
  if (F & FZ) {
    CYCT(0x6f64, 0x6f66);
    goto state0Common;
  }
  CYC(0x6f64, 0x6f66);
  CYC(0x6f66, 0x6f69); A = mem_rd(gb, 0xcd00);
  CYC(0x6f69, 0x6f6b); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(0x6f6b); return; }
  CYC(0x6f6b, 0x6f6c);
  CYC(0x6f6c, 0x6f6f); push_effect(gb, 0x6f6f);
  interactionCodee2__getDirectionToFace_hook(gb);
  CYC(0x6f6f, 0x6f72);
  interactionSetAnimation_hook(gb);
  return;
state0Common:
  CYC(0x6f72, 0x6f74); A = 0x01;
  CYC(0x6f74, 0x6f75); mem_wr(gb, DE, A);
  CALL_C(0x6f75, interactionInitGraphics_hook, 0x15fb, 0x6f78);
  CYC(0x6f78, 0x6f7b);
  objectSetVisible83_hook(gb);
  return;
subid2:
  CALL_C(0x6f7b, checkInteractionState_hook, 0x23fe, 0x6f7e);
  if (F & FZ) {
    CYCT(0x6f7e, 0x6f80);
    goto state0Common;
  }
  CYC(0x6f7e, 0x6f80);
  CYC(0x6f80, 0x6f83); A = mem_rd(gb, 0xcd00);
  CYC(0x6f83, 0x6f85); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(0x6f85); return; }
  CYC(0x6f85, 0x6f86);
  CYC(0x6f86, 0x6f89); push_effect(gb, 0x6f89);
  interactionCodee2__centerOnTileAndGetDirectionToFace_hook(gb);
offsetPositionTowardLookingDirection:
  CYC(0x6f89, 0x6f8c); SET_HL(0x6f9d);
  CYC(0x6f8c, 0x6f8d);
  interactionCodee2_addDoubleIndexToHl_from_rst(gb, 0x6f8d);
  CYC(0x6f8d, 0x6f8f); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x6f8f, 0x6f90); A = mem_rd(gb, DE);
  CYC(0x6f90, 0x6f92); alu_and(gb, 0xf0);
  CYC(0x6f92, 0x6f93); alu_or(gb, mem_rd(gb, HL));
  CYC(0x6f93, 0x6f94); mem_wr(gb, DE, A);
  CYC(0x6f94, 0x6f95); SET_HL(HL + 1);
  CYC(0x6f95, 0x6f97); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x6f97, 0x6f98); A = mem_rd(gb, DE);
  CYC(0x6f98, 0x6f9a); alu_and(gb, 0xf0);
  CYC(0x6f9a, 0x6f9b); alu_or(gb, mem_rd(gb, HL));
  CYC(0x6f9b, 0x6f9c); mem_wr(gb, DE, A);
  RET(0x6f9c); return;
subid1:
  CYC(0x6fcc, 0x6fce); E = 0x02;
spawnChildren:
  CYC(0x6fce, 0x6fd1); SET_BC(0xcfae);
spawnChildrenLoop:
  CYC(0x6fd1, 0x6fd2); A = mem_rd(gb, BC);
  CYC(0x6fd2, 0x6fd4); alu_cp(gb, 0xee);
  if (F & FZ) {
    CYCT(0x6fd4, 0x6fd7);
    push_effect(gb, 0x6fd7);
    interactionCodee2__spawnChild_hook(gb);
  } else {
    CYC(0x6fd4, 0x6fd7);
  }
  CYC(0x6fd7, 0x6fd8); C = alu_dec8(gb, C);
  if (!(F & FZ)) {
    CYCT(0x6fd8, 0x6fda);
    goto spawnChildrenLoop;
  }
  CYC(0x6fd8, 0x6fda);
  CYC(0x6fda, 0x6fdd);
  interactionDelete_hook(gb);
  return;
subid3:
  CALL_C(0x6ff3, returnIfScrollMode01Unset_hook, 0x26e4, 0x6ff6);
  CYC(0x6ff6, 0x6ff9); A = mem_rd(gb, 0xcc37);
  CYC(0x6ff9, 0x6ffb); alu_cp(gb, 0x06);
  CYC(0x6ffb, 0x6ffd); A = 0x00;
  if (!(F & FC)) {
    CYCT(0x6ffd, 0x6fff);
    goto storeCorrectDirection;
  }
  CYC(0x6ffd, 0x6fff);
randomDirectionLoop:
  CALL_C(0x6fff, getRandomNumber_hook, 0x043e, 0x7002);
  CYC(0x7002, 0x7004); alu_and(gb, 0x03);
  CYC(0x7004, 0x7006); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x7006, 0x7008);
    goto randomDirectionLoop;
  }
  CYC(0x7006, 0x7008);
storeCorrectDirection:
  CYC(0x7008, 0x700b); mem_wr(gb, 0xcca5, A);
  CYC(0x700b, 0x700d); E = 0x04;
  CYC(0x700d, 0x700f);
  goto spawnChildren;
subid4:
  CYC(0x700f, 0x7011); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7011, 0x7012); A = mem_rd(gb, DE);
  CYC(0x7012, 0x7013); push_effect(gb, 0x7013);
  switch (interactionCodee2_jump_table(gb)) {
    case 0x6f72: goto state0Common;
    case 0x7019: goto subid4State1;
    case 0x1e72: objectSetVisible83_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
subid4State1:
  CALL_C(0x7019, checkInteractionSubstate_hook, 0x2403, 0x701c);
  if (F & FZ) {
    CYCT(0x701c, 0x701e);
    goto substate0;
  }
  CYC(0x701c, 0x701e);
  CALL_C(0x701e, interactionDecCounter1_hook, 0x23cc, 0x7021);
  if (!(F & FZ)) {
    CYCT(0x7021, 0x7023);
    goto eyeSpinning;
  }
  CYC(0x7021, 0x7023);
  CALL_C(0x7023, interactionIncState_hook, 0x23e0, 0x7026);
  CYC(0x7026, 0x7029); A = mem_rd(gb, 0xcca5);
  CYC(0x7029, 0x702a); B = A;
randomFacingLoop:
  CYC(0x702a, 0x702d); SET_HL(0xcc00);
  CYC(0x702d, 0x702e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x702e, 0x702f); A = mem_rd(gb, HL);
  CYC(0x702f, 0x7031); alu_and(gb, 0x03);
  CYC(0x7031, 0x7032); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x7032, 0x7034);
    goto randomFacingLoop;
  }
  CYC(0x7032, 0x7034);
  CYC(0x7034, 0x7035); alu_add(gb, A);
  CYC(0x7035, 0x7038);
  goto offsetPositionTowardLookingDirection;
eyeSpinning:
  CYC(0x7038, 0x703b); A = mem_rd(gb, 0xcc00);
  CYC(0x703b, 0x703d); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x703d); return; }
  CYC(0x703d, 0x703e);
  CALL_C(0x703e, getRandomNumber_hook, 0x043e, 0x7041);
  CYC(0x7041, 0x7043); alu_and(gb, 0x07);
  CYC(0x7043, 0x7046);
  goto offsetPositionTowardLookingDirection;
substate0:
  CYC(0x7046, 0x7048); A = 0x3c;
  CYC(0x7048, 0x7049); mem_wr(gb, DE, A);
  CYC(0x7049, 0x704b); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x704b, 0x704c); mem_wr(gb, DE, A);
  RET(0x704c); return;
}
