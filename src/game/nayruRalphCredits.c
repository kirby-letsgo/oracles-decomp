#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/common/interactions/nayruRalphCredits.s (INTERAC_NAYRU_RALPH_CREDITS), bank $10.

static uint16_t interactionCodedf_jump_table(GB *gb) {
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

void interactionCodedf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7c10, 0x7c12); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7c12, 0x7c13); A = mem_rd(gb, DE);
  CYC(0x7c13, 0x7c14); push_effect(gb, 0x7c14);
  switch (interactionCodedf_jump_table(gb)) {
    case 0x7c18: goto state0;
    case 0x7c35: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CYC(0x7c18, 0x7c1a); A = 0x01;
  CYC(0x7c1a, 0x7c1b); mem_wr(gb, DE, A);
  CALL_C(0x7c1b, interactionInitGraphics_hook, 0x15fb, 0x7c1e);
  CYC(0x7c1e, 0x7c1f); H = D;
  CYC(0x7c1f, 0x7c21); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7c21, 0x7c23); mem_wr(gb, HL, 0x14);
  CYC(0x7c23, 0x7c25); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7c25, 0x7c27); mem_wr(gb, HL, 0x18);
  CYC(0x7c27, 0x7c29); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7c29, 0x7c2b); mem_wr(gb, HL, 0x3c);
  CYC(0x7c2b, 0x7c2d); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7c2d, 0x7c2e); A = mem_rd(gb, HL);
  CYC(0x7c2e, 0x7c2f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7c2f, 0x7c32);
    objectSetVisiblec2_hook(gb);
    return;
  }
  CYC(0x7c2f, 0x7c32);
  CYC(0x7c32, 0x7c35);
  objectSetVisiblec0_hook(gb);
  return;
state1:
  CYC(0x7c35, 0x7c37); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7c37, 0x7c38); A = mem_rd(gb, DE);
  CYC(0x7c38, 0x7c39); push_effect(gb, 0x7c39);
  switch (interactionCodedf_jump_table(gb)) {
    case 0x7c47: goto substate0;
    case 0x7c4e: goto substate1;
    case 0x7c68: goto substate2;
    case 0x7c7d: goto substate3;
    case 0x7ccd: goto substate4;
    case 0x7ce4: goto substate5;
    case 0x7cf2: goto substate6;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CALL_C(0x7c47, interactionDecCounter1_hook, 0x23cc, 0x7c4a);
  if (!(F & FZ)) { RET_TAKEN(0x7c4a); return; }
  CYC(0x7c4a, 0x7c4b);
  CALL_C(0x7c4b, interactionIncSubstate_hook, 0x23e5, 0x7c4e);
substate1:
  CALL_C(0x7c4e, interactionAnimate_hook, 0x261b, 0x7c51);
  CALL_C(0x7c51, objectApplySpeed_hook, 0x201d, 0x7c54);
  CYC(0x7c54, 0x7c56); alu_cp(gb, 0x68);
  if (!(F & FZ)) { RET_TAKEN(0x7c56); return; }
  CYC(0x7c56, 0x7c57);
  CALL_C(0x7c57, interactionIncSubstate_hook, 0x23e5, 0x7c5a);
  CYC(0x7c5a, 0x7c5c); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7c5c, 0x7c5e); mem_wr(gb, HL, 0xb4);
  CYC(0x7c5e, 0x7c60); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7c60, 0x7c61); A = mem_rd(gb, HL);
  CYC(0x7c61, 0x7c62); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7c62); return; }
  CYC(0x7c62, 0x7c63);
  CYC(0x7c63, 0x7c65); A = 0x05;
  CYC(0x7c65, 0x7c68);
  interactionSetAnimation_hook(gb);
  return;
substate2:
  CALL_C(0x7c68, interactionDecCounter1_hook, 0x23cc, 0x7c6b);
  if (!(F & FZ)) { RET_TAKEN(0x7c6b); return; }
  CYC(0x7c6b, 0x7c6c);
  CYC(0x7c6c, 0x7c6f); SET_HL(0xcfd0);
  CYC(0x7c6f, 0x7c71); mem_wr(gb, HL, 0x01);
  CALL_C(0x7c71, interactionIncSubstate_hook, 0x23e5, 0x7c74);
  CYC(0x7c74, 0x7c76); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7c76, 0x7c78); mem_wr(gb, HL, 0x04);
  CYC(0x7c78, 0x7c79); L = alu_inc8(gb, L);
  CYC(0x7c79, 0x7c7b); mem_wr(gb, HL, 0x01);
  CYC(0x7c7b, 0x7c7d);
  goto setRandomVar38;
substate3:
  CYC(0x7c7d, 0x7c7e); H = D;
  CYC(0x7c7e, 0x7c80); L = INTERACTION_BASE + OBJ_COUNTER1;
  CALL_C(0x7c80, decHlRef16WithCap_hook, 0x0237, 0x7c83);
  if (!(F & FZ)) {
    CYCT(0x7c83, 0x7c85);
    goto label_10_330;
  }
  CYC(0x7c83, 0x7c85);
  CALL_C(0x7c85, interactionIncSubstate_hook, 0x23e5, 0x7c88);
  CYC(0x7c88, 0x7c8a); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7c8a, 0x7c8c); mem_wr(gb, HL, 0x64);
  CYC(0x7c8c, 0x7c8e); B = 0x14;
  CYC(0x7c8e, 0x7c90); C = 0x04;
  CYC(0x7c90, 0x7c92); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7c92, 0x7c93); A = mem_rd(gb, HL);
  CYC(0x7c93, 0x7c94); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7c94, 0x7c96);
    goto setSpeed;
  }
  CYC(0x7c94, 0x7c96);
  CYC(0x7c96, 0x7c98); B = 0x3c;
  CYC(0x7c98, 0x7c9a); C = 0x02;
setSpeed:
  CYC(0x7c9a, 0x7c9c); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7c9c, 0x7c9d); mem_wr(gb, HL, B);
  CYC(0x7c9d, 0x7c9e); A = C;
  CALL_C(0x7c9e, interactionSetAnimation_hook, 0x262e, 0x7ca1);
  CYC(0x7ca1, 0x7ca4); SET_HL(0xcfd0);
  CYC(0x7ca4, 0x7ca7); mem_wr(gb, HL, 0x02);
  RET(0x7ca6); return;
label_10_330:
  CYC(0x7ca7, 0x7ca9); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7ca9, 0x7caa); A = mem_rd(gb, HL);
  CYC(0x7caa, 0x7cab); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x7cab, interactionAnimate_hook, 0x261b, 0x7cae);
  } else {
    CYC(0x7cab, 0x7cae);
  }
  CYC(0x7cae, 0x7cb0); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x7cb0, 0x7cb1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x7cb1); return; }
  CYC(0x7cb1, 0x7cb2);
  CYC(0x7cb2, 0x7cb4); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x7cb4, 0x7cb5); A = mem_rd(gb, HL);
  CYC(0x7cb5, 0x7cb7); alu_xor(gb, 0x01);
  CYC(0x7cb7, 0x7cb8); mem_wr(gb, HL, A);
  CYC(0x7cb8, 0x7cba); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7cba, 0x7cbb); A = mem_rd(gb, DE);
  CYC(0x7cbb, 0x7cbc); alu_add(gb, A);
  CYC(0x7cbc, 0x7cbd); alu_add(gb, mem_rd(gb, HL));
  CALL_C(0x7cbd, interactionSetAnimation_hook, 0x262e, 0x7cc0);
setRandomVar38:
  CALL_C(0x7cc0, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7cc3);
  CYC(0x7cc3, 0x7cc5); alu_and(gb, 0x03);
  CYC(0x7cc5, 0x7cc7); A = alu_swap(gb, A);
  CYC(0x7cc7, 0x7cc9); alu_add(gb, 0x20);
  CYC(0x7cc9, 0x7ccb); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x7ccb, 0x7ccc); mem_wr(gb, DE, A);
  RET(0x7ccc); return;
substate4:
  CALL_C(0x7ccd, interactionDecCounter1_hook, 0x23cc, 0x7cd0);
  if (!(F & FZ)) { RET_TAKEN(0x7cd0); return; }
  CYC(0x7cd0, 0x7cd1);
  CYC(0x7cd1, 0x7cd3); B = 0x78;
  CYC(0x7cd3, 0x7cd5); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7cd5, 0x7cd6); A = mem_rd(gb, DE);
  CYC(0x7cd6, 0x7cd7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7cd7, 0x7cd9);
    goto storeCounter1_4;
  }
  CYC(0x7cd7, 0x7cd9);
  CYC(0x7cd9, 0x7cdb); B = 0xa0;
storeCounter1_4:
  CYC(0x7cdb, 0x7cdc); mem_wr(gb, HL, B);
  CYC(0x7cdc, 0x7cdf); SET_HL(0xcfd0);
  CYC(0x7cdf, 0x7ce1); mem_wr(gb, HL, 0x03);
  CYC(0x7ce1, 0x7ce4);
  interactionIncSubstate_hook(gb);
  return;
substate5:
  CALL_C(0x7ce4, interactionDecCounter1_hook, 0x23cc, 0x7ce7);
  if (!(F & FZ)) { RET_TAKEN(0x7ce7); return; }
  CYC(0x7ce7, 0x7ce8);
  CYC(0x7ce8, 0x7cea); mem_wr(gb, HL, 0x3c);
  CYC(0x7cea, 0x7ced); SET_HL(0xcfd0);
  CYC(0x7ced, 0x7cef); mem_wr(gb, HL, 0x04);
  CYC(0x7cef, 0x7cf2);
  interactionIncSubstate_hook(gb);
  return;
substate6:
  CALL_C(0x7cf2, interactionAnimate_hook, 0x261b, 0x7cf5);
  CALL_C(0x7cf5, objectApplySpeed_hook, 0x201d, 0x7cf8);
  CALL_C(0x7cf8, interactionDecCounter1_hook, 0x23cc, 0x7cfb);
  if (!(F & FZ)) { RET_TAKEN(0x7cfb); return; }
  CYC(0x7cfb, 0x7cfc);
  CYC(0x7cfc, 0x7cff); SET_HL(0xcfdf);
  CYC(0x7cff, 0x7d01); mem_wr(gb, HL, 0x01);
  RET(0x7d01); return;
}
