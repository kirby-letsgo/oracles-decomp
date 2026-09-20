#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/ages/interactions/timeportal.s (INTERAC_TIMEPORTAL), bank $10.

static uint16_t timeportal_jump_table(GB *gb) {
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

void timeportal_updatePalette_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7bff, 0x7c02); A = mem_rd(gb, 0xcc00);
  CYC(0x7c02, 0x7c04); alu_and(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x7c04, 0x7c06);
    goto animate;
  }
  CYC(0x7c04, 0x7c06);
  CYC(0x7c06, 0x7c08); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7c08, 0x7c09); A = mem_rd(gb, DE);
  CYC(0x7c09, 0x7c0a); A = alu_inc8(gb, A);
  CYC(0x7c0a, 0x7c0c); alu_and(gb, 0x0b);
  CYC(0x7c0c, 0x7c0d); mem_wr(gb, DE, A);
animate:
  CYC(0x7c0d, 0x7c10);
  interactionAnimate_hook(gb);
}

void interactionBeginTimewarp_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7bce, resetLinkInvincibility_hook, 0x2ba9, 0x7bd1);
  CYC(0x7bd1, 0x7bd4); SET_HL(0xd000);
  CALL_C(0x7bd4, objectCopyPosition_hook, 0x2242, 0x7bd7);
  CYC(0x7bd7, 0x7bd9); L = 0x08;
  CYC(0x7bd9, 0x7bdb); mem_wr(gb, HL, 0x02);
  CYC(0x7bdb, 0x7bdd); A = 0x81;
  CYC(0x7bdd, 0x7be0); mem_wr(gb, 0xcc8a, A);
  CYC(0x7be0, 0x7be3); mem_wr(gb, 0xcbca, A);
  CALL_C(0x7be3, objectGetTileAtPosition_hook, 0x1444, 0x7be6);
  CYC(0x7be6, 0x7be9); mem_wr(gb, 0xcc9a, A);
  CYC(0x7be9, 0x7bea); A = L;
  CYC(0x7bea, 0x7bed); mem_wr(gb, 0xcc99, A);
  CYC(0x7bed, 0x7bee); A = alu_inc8(gb, A);
  CYC(0x7bee, 0x7bf1); mem_wr(gb, 0xcddc, A);
  CYC(0x7bf1, 0x7bf4); mem_wr(gb, 0xcde0, A);
  CYC(0x7bf4, 0x7bf6); A = 0x1b;
  CYC(0x7bf6, 0x7bf9); mem_wr(gb, 0xcc04, A);
  CALL_C(0x7bf9, restartSound_hook, 0x0cb2, 0x7bfc);
  CYC(0x7bfc, 0x7bff);
  interactionDelete_hook(gb);
}

void interactionCodede_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7b68, 0x7b6a); A = 0x02;
  CYC(0x7b6a, 0x7b6d); mem_wr(gb, 0xcddd, A);
  CYC(0x7b6d, 0x7b70); A = mem_rd(gb, 0xcc02);
  CYC(0x7b70, 0x7b71); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7b71, 0x7b74);
    objectSetInvisible_hook(gb);
    return;
  }
  CYC(0x7b71, 0x7b74);
  CALL_C(0x7b74, objectSetVisible_hook, 0x1e84, 0x7b77);
  CYC(0x7b77, 0x7b79); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7b79, 0x7b7a); A = mem_rd(gb, DE);
  CYC(0x7b7a, 0x7b7b); push_effect(gb, 0x7b7b);
  switch (timeportal_jump_table(gb)) {
    case 0x7b81: goto state0;
    case 0x7ba6: goto state1;
    case 0x7bae: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CYC(0x7b81, 0x7b83); C = 0xde;
  CALL_C(0x7b83, objectFindSameTypeObjectWithID_hook, 0x22c8, 0x7b86);
  CYC(0x7b86, 0x7b87); A = H;
  CYC(0x7b87, 0x7b88); alu_cp(gb, D);
  if (!(F & FZ)) {
    CYCT(0x7b88, 0x7b8b);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7b88, 0x7b8b);
  CYC(0x7b8b, 0x7b8d); A = 0x03;
  CALL_C(0x7b8d, objectSetCollideRadius_hook, 0x24a1, 0x7b90);
  CALL_C(0x7b90, objectGetShortPosition_hook, 0x2096, 0x7b93);
  CYC(0x7b93, 0x7b94); C = A;
  CALL_C(0x7b94, interactionIncState_hook, 0x23e0, 0x7b97);
  CYC(0x7b97, 0x7b99); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7b99, 0x7b9a); mem_wr(gb, HL, C);
  CALL_C(0x7b9a, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x7b9d);
  if (!(F & FC)) {
    CALL_C_CC(0x7b9d, interactionIncState_hook, 0x23e0, 0x7ba0);
  } else {
    CYC(0x7b9d, 0x7ba0);
  }
  CALL_C(0x7ba0, interactionInitGraphics_hook, 0x15fb, 0x7ba3);
  CYC(0x7ba3, 0x7ba6);
  objectSetVisible83_hook(gb);
  return;
state1:
  CALL_C(0x7ba6, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x7ba9);
  if (!(F & FC)) {
    CYCT(0x7ba9, 0x7bac);
    interactionIncState_hook(gb);
    return;
  }
  CYC(0x7ba9, 0x7bac);
  CYC(0x7bac, 0x7bae);
  timeportal_updatePalette_hook(gb);
  return;
state2:
  CYC(0x7bae, 0x7bb0); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7bb0, 0x7bb1); A = mem_rd(gb, DE);
  CYC(0x7bb1, 0x7bb2); B = A;
  CYC(0x7bb2, 0x7bb5); A = mem_rd(gb, 0xc640);
  CYC(0x7bb5, 0x7bb6); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x7bb6, 0x7bb9);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7bb6, 0x7bb9);
  CALL_C(0x7bb9, timeportal_updatePalette_hook, 0x7bff, 0x7bbc);
  CYC(0x7bbc, 0x7bbf); A = mem_rd(gb, 0xcc2c);
  CYC(0x7bbf, 0x7bc0); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x7bc0); return; }
  CYC(0x7bc0, 0x7bc1);
  CALL_C(0x7bc1, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x7bc4);
  if (!(F & FC)) { RET_TAKEN(0x7bc4); return; }
  CYC(0x7bc4, 0x7bc5);
  CALL_C(0x7bc5, checkLinkCollisionsEnabled_hook, 0x1d32, 0x7bc8);
  if (!(F & FC)) { RET_TAKEN(0x7bc8); return; }
  CYC(0x7bc8, 0x7bc9);
  CYC(0x7bc9, 0x7bcb); A = 0xff;
  CYC(0x7bcb, 0x7bce); mem_wr(gb, 0xc63e, A);
  interactionBeginTimewarp_hook(gb);
}
