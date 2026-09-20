#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/ages/interactions/miscellaneous2.s (INTERAC_MISCELLANEOUS_2), bank $10.

static uint16_t interactiondc_jump_table(GB *gb) {
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

static void interactiondc_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

void interactiondc_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x74fd, getThisRoomFlags_hook, 0x197d, 0x7500);
  CYC(0x7500, 0x7502); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x7502, 0x7505);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7502, 0x7505);
  CYC(0x7505, 0x7508); A = mem_rd(gb, 0xcc8f);
  CYC(0x7508, 0x750a); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(0x750a); return; }
  CYC(0x750a, 0x750b);
  CYC(0x750b, 0x750e); SET_BC(0x4200);
  CALL_C(0x750e, createTreasure_hook, 0x27d4, 0x7511);
  CALL_C(0x7511, objectCopyPosition_hook, 0x2242, 0x7514);
  CYC(0x7514, 0x7517);
  interactionDelete_hook(gb);
}

void interactiondc_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7517, checkInteractionState_hook, 0x23fe, 0x751a);
  if (!(F & FZ)) {
    CYCT(0x751a, 0x751d);
    interactionRunScript_hook(gb);
    return;
  }
  CYC(0x751a, 0x751d);
  CALL_C(0x751d, getThisRoomFlags_hook, 0x197d, 0x7520);
  CYC(0x7520, 0x7522); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(0x7522, 0x7525);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7522, 0x7525);
  CYC(0x7525, 0x7528); SET_HL(0x7f62);
  CALL_C(0x7528, interactionSetScript_hook, 0x2544, 0x752b);
  CALL_C(0x752b, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x752e);
  CYC(0x752e, 0x7531);
  interactionIncState_hook(gb);
}

void interactiondc_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7531, 0x7533); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7533, 0x7534); A = mem_rd(gb, DE);
  CYC(0x7534, 0x7535); push_effect(gb, 0x7535);
  switch (interactiondc_jump_table(gb)) {
    case 0x753b: goto state0;
    case 0x7559: goto state1;
    case 0x7580: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CALL_C(0x753b, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x753e);
  CALL_C(0x753e, getThisRoomFlags_hook, 0x197d, 0x7541);
  CYC(0x7541, 0x7543); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(0x7543, 0x7546);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7543, 0x7546);
  CYC(0x7546, 0x7547); A = D;
  CYC(0x7547, 0x754a); mem_wr(gb, 0xccde, A);
  CALL_C(0x754a, objectGetTileAtPosition_hook, 0x1444, 0x754d);
  CYC(0x754d, 0x754f); alu_cp(gb, 0x3a);
  if (!(F & FZ)) { RET_TAKEN(0x754f); return; }
  CYC(0x754f, 0x7550);
  CYC(0x7550, 0x7551); C = L;
  CYC(0x7551, 0x7553); A = 0x1c;
  CALL_C(0x7553, setTile_hook, 0x3a9c, 0x7556);
  CYC(0x7556, 0x7559);
  interactionIncState_hook(gb);
  return;
state1:
  CYC(0x7559, 0x755c); A = mem_rd(gb, 0xcc5a);
  CYC(0x755c, 0x755e); alu_cp(gb, 0x83);
  if (!(F & FZ)) { RET_TAKEN(0x755e); return; }
  CYC(0x755e, 0x755f);
  CYC(0x755f, 0x7561); A = 0x01;
  CYC(0x7561, 0x7564); mem_wr(gb, 0xd008, A);
  CYC(0x7564, 0x7566); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7566, 0x7568); A = 0x1e;
  CYC(0x7568, 0x7569); mem_wr(gb, DE, A);
  CALL_C(0x7569, checkLinkCollisionsEnabled_hook, 0x1d32, 0x756c);
  if (!(F & FC)) { RET_TAKEN(0x756c); return; }
  CYC(0x756c, 0x756d);
  CYC(0x756d, 0x756f); A = 0x01;
  CYC(0x756f, 0x7572); mem_wr(gb, 0xcc8a, A);
  CYC(0x7572, 0x7575); mem_wr(gb, 0xcc02, A);
  CALL_C(0x7575, resetLinkInvincibility_hook, 0x2ba9, 0x7578);
  CYC(0x7578, 0x757a); A = 0xf0;
  CALL_C(0x757a, playSound_b00_hook, 0x0c98, 0x757d);
  CYC(0x757d, 0x7580);
  interactionIncState_hook(gb);
  return;
state2:
  CYC(0x7580, 0x7582); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7582, 0x7583); A = mem_rd(gb, DE);
  CYC(0x7583, 0x7584); push_effect(gb, 0x7584);
  switch (interactiondc_jump_table(gb)) {
    case 0x7588: goto substate0;
    case 0x759f: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CALL_C(0x7588, interactionDecCounter1_hook, 0x23cc, 0x758b);
  if (!(F & FZ)) { RET_TAKEN(0x758b); return; }
  CYC(0x758b, 0x758c);
  CYC(0x758c, 0x758e); mem_wr(gb, HL, 0x3c);
  CYC(0x758e, 0x7590); A = 0x3c;
  CYC(0x7590, 0x7593); SET_BC(0xf800);
  CALL_C(0x7593, objectCreateExclamationMark_hook, 0x27e0, 0x7596);
  CALL_C(0x7596, clearAllParentItems_hook, 0x2c10, 0x7599);
  CALL_C(0x7599, dropLinkHeldItem_hook, 0x2c43, 0x759c);
  CYC(0x759c, 0x759f);
  interactionIncSubstate_hook(gb);
  return;
substate1:
  CYC(0x759f, 0x75a1); A = 0x28;
  CALL_C(0x75a1, setScreenShakeCounter_hook, 0x24bb, 0x75a4);
  CALL_C(0x75a4, interactionDecCounter1_hook, 0x23cc, 0x75a7);
  if (!(F & FZ)) { RET_TAKEN(0x75a7); return; }
  CYC(0x75a7, 0x75a8);
  CYC(0x75a8, 0x75aa); A = 0x1a;
  CYC(0x75aa, 0x75ad); mem_wr(gb, 0xcc04, A);
  CYC(0x75ad, 0x75b0);
  interactionDelete_hook(gb);
}

void interactiondc_subid3And4_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x75c5, objectGetTileAtPosition_hook, 0x1444, 0x75c8);
  CYC(0x75c8, 0x75ca); alu_cp(gb, 0x3a);
  if (!(F & FZ)) { RET_TAKEN(0x75ca); return; }
  CYC(0x75ca, 0x75cb);
  CYC(0x75cb, 0x75cd); A = 0xd7;
  CYC(0x75cd, 0x75ce); C = L;
  CALL_C(0x75ce, setTile_hook, 0x3a9c, 0x75d1);
  CALL_C(0x75d1, getThisRoomFlags_hook, 0x197d, 0x75d4);
  CYC(0x75d4, 0x75d6); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x75d6, 0x75d7); A = mem_rd(gb, DE);
  CYC(0x75d7, 0x75d8); alu_or(gb, mem_rd(gb, HL));
  CYC(0x75d8, 0x75d9); mem_wr(gb, HL, A);
  CYC(0x75d9, 0x75db); A = 0x4d;
  CALL_C(0x75db, playSound_b00_hook, 0x0c98, 0x75de);
  CYC(0x75de, 0x75e1);
  interactionDelete_hook(gb);
}

void interactiondc_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x75b0, checkInteractionState_hook, 0x23fe, 0x75b3);
  if (!(F & FZ)) {
    CYCT(0x75b3, 0x75b5);
    interactiondc_subid3And4_state1_hook(gb);
    return;
  }
  CYC(0x75b3, 0x75b5);
  CALL_C(0x75b5, getThisRoomFlags_hook, 0x197d, 0x75b8);
  CYC(0x75b8, 0x75ba); alu_and(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x75ba, 0x75bd);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x75ba, 0x75bd);
  CYC(0x75bd, 0x75bf); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x75bf, 0x75c1); A = 0x02;
  CYC(0x75c1, 0x75c2); mem_wr(gb, DE, A);
  CYC(0x75c2, 0x75c5);
  interactionIncState_hook(gb);
}

void interactiondc_subid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x75e1, checkInteractionState_hook, 0x23fe, 0x75e4);
  if (!(F & FZ)) {
    CYCT(0x75e4, 0x75e6);
    interactiondc_subid3And4_state1_hook(gb);
    return;
  }
  CYC(0x75e4, 0x75e6);
  CALL_C(0x75e6, getThisRoomFlags_hook, 0x197d, 0x75e9);
  CYC(0x75e9, 0x75eb); alu_and(gb, 0x04);
  if (!(F & FZ)) {
    CYCT(0x75eb, 0x75ee);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x75eb, 0x75ee);
  CYC(0x75ee, 0x75f0); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x75f0, 0x75f2); A = 0x04;
  CYC(0x75f2, 0x75f3); mem_wr(gb, DE, A);
  CYC(0x75f3, 0x75f6);
  interactionIncState_hook(gb);
}

void interactiondc_subid05_setRandomShakeDuration_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7641, getRandomNumber_hook, 0x043e, 0x7644);
  CYC(0x7644, 0x7646); alu_and(gb, 0x7f);
  CYC(0x7646, 0x7648); alu_sub(gb, 0x40);
  CYC(0x7648, 0x764a); alu_add(gb, 0x60);
  CYC(0x764a, 0x764c); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x764c, 0x764d); mem_wr(gb, DE, A);
  RET(0x764d); return;
}

void interactiondc_subid05_shakeScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x764e, setScreenShakeCounter_hook, 0x24bb, 0x7651);
  CYC(0x7651, 0x7654); A = mem_rd(gb, 0xcc00);
  CYC(0x7654, 0x7655); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x7655); return; }
  CYC(0x7655, 0x7656);
  CYC(0x7656, 0x7659);
  interactionDecCounter1_hook(gb);
}

void interactiondc_subid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x75f6, 0x75f8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x75f8, 0x75f9); A = mem_rd(gb, DE);
  CYC(0x75f9, 0x75fa); push_effect(gb, 0x75fa);
  switch (interactiondc_jump_table(gb)) {
    case 0x7600: goto state0;
    case 0x7622: goto state1;
    case 0x762d: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CYC(0x7600, 0x7602); A = 0x29;
  CALL_C(0x7602, checkGlobalFlag_hook, 0x31f3, 0x7605);
  if (!(F & FZ)) {
    CYCT(0x7605, 0x7608);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7605, 0x7608);
  CALL_C(0x7608, returnIfScrollMode01Unset_hook, 0x26e4, 0x760b);
  CYC(0x760b, 0x760d); A = 0xf1;
  CALL_C(0x760d, playSound_b00_hook, 0x0c98, 0x7610);
  CYC(0x7610, 0x7612); A = 0x01;
  CYC(0x7612, 0x7615); mem_wr(gb, 0xcc94, A);
  CYC(0x7615, 0x7618); push_effect(gb, 0x7618);
  interactiondc_subid05_setRandomShakeDuration_hook(gb);
  CYC(0x7618, 0x761b); A = mem_rd(gb, 0xcc00);
  CYC(0x761b, 0x761c); alu_rrca(gb);
  if (F & FC) {
    CALL_C_CC(0x761c, interactionIncState_hook, 0x23e0, 0x761f);
  } else {
    CYC(0x761c, 0x761f);
  }
  CYC(0x761f, 0x7622);
  interactionIncState_hook(gb);
  return;
state1:
  CYC(0x7622, 0x7623); alu_xor(gb, A);
  CYC(0x7623, 0x7626); push_effect(gb, 0x7626);
  interactiondc_subid05_shakeScreen_hook(gb);
  if (!(F & FZ)) { RET_TAKEN(0x7626); return; }
  CYC(0x7626, 0x7627); push_effect(gb, 0x762a);
  interactiondc_subid05_setRandomShakeDuration_hook(gb);
  CYC(0x762a, 0x762d);
  interactionIncState_hook(gb);
  return;
state2:
  CYC(0x762d, 0x7630); A = mem_rd(gb, 0xcc00);
  CYC(0x7630, 0x7632); alu_and(gb, 0x0f);
  CYC(0x7632, 0x7634); A = 0xb3;
  if (F & FZ) {
    CALL_C_CC(0x7634, playSound_b00_hook, 0x0c98, 0x7637);
  } else {
    CYC(0x7634, 0x7637);
  }
  CYC(0x7637, 0x7639); A = 0x08;
  CYC(0x7639, 0x763c); push_effect(gb, 0x763c);
  interactiondc_subid05_shakeScreen_hook(gb);
  if (!(F & FZ)) { RET_TAKEN(0x763c); return; }
  CYC(0x763c, 0x763d);
  CYC(0x763d, 0x763f); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x763f, 0x7641); mem_wr(gb, HL, 0x01);
  interactiondc_subid05_setRandomShakeDuration_hook(gb);
}

void interactiondc_subid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7659, 0x765b); A = 0x29;
  CALL_C(0x765b, checkGlobalFlag_hook, 0x31f3, 0x765e);
  if (!(F & FZ)) {
    CYCT(0x765e, 0x7660);
    goto delete_;
  }
  CYC(0x765e, 0x7660);
  CYC(0x7660, 0x7663); SET_BC(0xb201);
  CALL_C(0x7663, objectCreateInteraction_hook, 0x24c5, 0x7666);
delete_:
  CYC(0x7666, 0x7669);
  interactionDelete_hook(gb);
}

void interactiondc_subid07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x74b5, getThisRoomFlags_hook, 0x197d, 0x74b8);
  CYC(0x74b8, 0x74ba); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x74ba, 0x74bd);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x74ba, 0x74bd);
  CYC(0x74bd, 0x74c0); SET_BC(0x2b00);
  CALL_C(0x74c0, createTreasure_hook, 0x27d4, 0x74c3);
  CALL_C(0x74c3, objectCopyPosition_hook, 0x2242, 0x74c6);
  CYC(0x74c6, 0x74c9);
  interactionDelete_hook(gb);
}

void interactiondc_subid08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x74c9, checkInteractionState_hook, 0x23fe, 0x74cc);
  if (F & FZ) {
    CYCT(0x74cc, 0x74ce);
    goto state0;
  }
  CYC(0x74cc, 0x74ce);
  CYC(0x74ce, 0x74d0); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x74d0, 0x74d1); A = mem_rd(gb, DE);
  CYC(0x74d1, 0x74d2); C = A;
  CYC(0x74d2, 0x74d4); B = 0xcf;
  CYC(0x74d4, 0x74d5); A = mem_rd(gb, BC);
  CYC(0x74d5, 0x74d6); L = A;
  CYC(0x74d6, 0x74d8); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x74d8, 0x74d9); A = mem_rd(gb, DE);
  CYC(0x74d9, 0x74da); alu_cp(gb, L);
  if (F & FZ) { RET_TAKEN(0x74da); return; }
  CYC(0x74da, 0x74db);
  CALL_C(0x74db, getThisRoomFlags_hook, 0x197d, 0x74de);
  CYC(0x74de, 0x74e0); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x74e0, 0x74e1); A = mem_rd(gb, DE);
  CYC(0x74e1, 0x74e2); alu_or(gb, mem_rd(gb, HL));
  CYC(0x74e2, 0x74e3); mem_wr(gb, HL, A);
  CYC(0x74e3, 0x74e6);
  interactionDelete_hook(gb);
  return;
state0:
  CALL_C(0x74e6, getThisRoomFlags_hook, 0x197d, 0x74e9);
  CYC(0x74e9, 0x74eb); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x74eb, 0x74ec); A = mem_rd(gb, DE);
  CYC(0x74ec, 0x74ed); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x74ed, 0x74f0);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x74ed, 0x74f0);
  CYC(0x74f0, 0x74f2); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x74f2, 0x74f3); A = mem_rd(gb, DE);
  CYC(0x74f3, 0x74f4); C = A;
  CYC(0x74f4, 0x74f6); B = 0xcf;
  CYC(0x74f6, 0x74f7); A = mem_rd(gb, BC);
  CYC(0x74f7, 0x74f9); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x74f9, 0x74fa); mem_wr(gb, DE, A);
  CYC(0x74fa, 0x74fd);
  interactionIncState_hook(gb);
}

void interactiondc_subid09_replaceTileList_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
loop:
  CYC(0x76f0, 0x76f1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76f1, 0x76f2); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x76f2); return; }
  CYC(0x76f2, 0x76f3);
  CYC(0x76f3, 0x76f4); C = A;
  CYC(0x76f4, 0x76f5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x76f5, 0x76f6); push_effect(gb, HL);
  CALL_C(0x76f6, setTile_hook, 0x3a9c, 0x76f9);
  CYC(0x76f9, 0x76fa); SET_HL(pop_effect(gb));
  CYC(0x76fa, 0x76fd);
  goto loop;
}

void interactiondc_subid09_returnToState1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x76b6, interactionDecCounter1_hook, 0x23cc, 0x76b9);
  if (!(F & FZ)) { RET_TAKEN(0x76b9); return; }
  CYC(0x76b9, 0x76ba);
  CYC(0x76ba, 0x76bc); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x76bc, 0x76be); A = 0x01;
  CYC(0x76be, 0x76bf); mem_wr(gb, DE, A);
  RET(0x76bf); return;
}

void interactiondc_subid09_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7693, 0x7695); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7695, 0x7696); A = mem_rd(gb, DE);
  CYC(0x7696, 0x7697); push_effect(gb, 0x7697);
  switch (interactiondc_jump_table(gb)) {
    case 0x769d: goto substate0;
    case 0x76ab: goto substate1;
    case 0x76b6: interactiondc_subid09_returnToState1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x769d, 0x769f); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x769f, 0x76a1); A = 0x08;
  CYC(0x76a1, 0x76a2); mem_wr(gb, DE, A);
  CYC(0x76a2, 0x76a5); SET_HL(0x76fc);
replaceTileListAndIncSubstate:
  CYC(0x76a5, 0x76a8); push_effect(gb, 0x76a8);
  interactiondc_subid09_replaceTileList_hook(gb);
  CYC(0x76a8, 0x76ab);
  interactionIncSubstate_hook(gb);
  return;
substate1:
  CALL_C(0x76ab, interactionDecCounter1_hook, 0x23cc, 0x76ae);
  if (!(F & FZ)) { RET_TAKEN(0x76ae); return; }
  CYC(0x76ae, 0x76af);
  CYC(0x76af, 0x76b1); mem_wr(gb, HL, 0x08);
  CYC(0x76b1, 0x76b4); SET_HL(0x7705);
  CYC(0x76b4, 0x76b6);
  goto replaceTileListAndIncSubstate;
}

void interactiondc_subid09_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x76c0, 0x76c2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x76c2, 0x76c3); A = mem_rd(gb, DE);
  CYC(0x76c3, 0x76c4); push_effect(gb, 0x76c4);
  switch (interactiondc_jump_table(gb)) {
    case 0x76b6: interactiondc_subid09_returnToState1_hook(gb); return;
    case 0x76cc: goto substate0;
    case 0x76da: goto substate1;
    case 0x76e5: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x76cc, 0x76ce); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x76ce, 0x76d0); A = 0x0c;
  CYC(0x76d0, 0x76d1); mem_wr(gb, DE, A);
  CYC(0x76d1, 0x76d4); SET_HL(0x770e);
replaceTileListAndIncSubstate:
  CYC(0x76d4, 0x76d7); push_effect(gb, 0x76d7);
  interactiondc_subid09_replaceTileList_hook(gb);
  CYC(0x76d7, 0x76da);
  interactionIncSubstate_hook(gb);
  return;
substate1:
  CALL_C(0x76da, interactionDecCounter1_hook, 0x23cc, 0x76dd);
  if (!(F & FZ)) { RET_TAKEN(0x76dd); return; }
  CYC(0x76dd, 0x76de);
  CYC(0x76de, 0x76e0); mem_wr(gb, HL, 0x0c);
  CYC(0x76e0, 0x76e3); SET_HL(0x771f);
  CYC(0x76e3, 0x76e5);
  goto replaceTileListAndIncSubstate;
substate2:
  CALL_C(0x76e5, interactionDecCounter1_hook, 0x23cc, 0x76e8);
  if (!(F & FZ)) { RET_TAKEN(0x76e8); return; }
  CYC(0x76e8, 0x76e9);
  CYC(0x76e9, 0x76eb); mem_wr(gb, HL, 0x0c);
  CYC(0x76eb, 0x76ee); SET_HL(0x7730);
  CYC(0x76ee, 0x76f0);
  goto replaceTileListAndIncSubstate;
}

void interactiondc_subid09_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7669, 0x766b); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x766b, 0x766c); A = mem_rd(gb, DE);
  CYC(0x766c, 0x766d); push_effect(gb, 0x766d);
  switch (interactiondc_jump_table(gb)) {
    case 0x23e0: interactionIncState_hook(gb); return;
    case 0x7675: goto state1;
    case 0x7693: interactiondc_subid09_state2_hook(gb); return;
    case 0x76c0: interactiondc_subid09_state3_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
state1:
  CYC(0x7675, 0x7678); A = mem_rd(gb, 0xcc02);
  CYC(0x7678, 0x7679); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7679); return; }
  CYC(0x7679, 0x767a);
  CYC(0x767a, 0x767d); A = mem_rd(gb, 0xcc00);
  CYC(0x767d, 0x767f); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x767f); return; }
  CYC(0x767f, 0x7680);
  CALL_C(0x7680, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7683);
  CYC(0x7683, 0x7685); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x7685); return; }
  CYC(0x7685, 0x7686);
  CYC(0x7686, 0x7688); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x7688, 0x7689); alu_xor(gb, A);
  CYC(0x7689, 0x768a); mem_wr(gb, DE, A);
  CYC(0x768a, 0x768c); A = mem_rd(gb, 0xff95);
  CYC(0x768c, 0x768d); alu_rrca(gb);
  if (F & FC) {
    CALL_C_CC(0x768d, interactionIncState_hook, 0x23e0, 0x7690);
  } else {
    CYC(0x768d, 0x7690);
  }
  CYC(0x7690, 0x7693);
  interactionIncState_hook(gb);
}

void interactiondc_subid0A_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7741, checkInteractionState_hook, 0x23fe, 0x7744);
  if (F & FZ) {
    CYCT(0x7744, 0x7746);
    goto state0;
  }
  CYC(0x7744, 0x7746);
  CYC(0x7746, 0x7748); A = 0x31;
  CALL_C(0x7748, checkGlobalFlag_hook, 0x31f3, 0x774b);
  if (F & FZ) {
    CYCT(0x774b, 0x774e);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x774b, 0x774e);
  CYC(0x774e, 0x7750); A = 0x81;
  CYC(0x7750, 0x7753); mem_wr(gb, 0xcc8a, A);
  CYC(0x7753, 0x7756); mem_wr(gb, 0xcc02, A);
  CYC(0x7756, 0x7757); alu_xor(gb, A);
  CYC(0x7757, 0x775a); mem_wr(gb, 0xd008, A);
  CYC(0x775a, 0x775c); A = 0x1d;
  CYC(0x775c, 0x775f); mem_wr(gb, 0xcc04, A);
  CYC(0x775f, 0x7762);
  interactionDelete_hook(gb);
  return;
state0:
  CALL_C(0x7762, getThisRoomFlags_hook, 0x197d, 0x7765);
  CYC(0x7765, 0x7767); alu_and(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7767, 0x776a);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7767, 0x776a);
  CYC(0x776a, 0x776d);
  interactionIncState_hook(gb);
}

void interactiondc_subid0B_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x776d, 0x776f); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x776f, 0x7770); A = mem_rd(gb, DE);
  CYC(0x7770, 0x7771); push_effect(gb, 0x7771);
  switch (interactiondc_jump_table(gb)) {
    case 0x7779: goto state0;
    case 0x7787: goto state1;
    case 0x77ad: goto state2;
    case 0x77ba: goto state3;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CYC(0x7779, 0x777b); A = 0x01;
  CYC(0x777b, 0x777c); mem_wr(gb, DE, A);
  CYC(0x777c, 0x777e); A = 0x18;
  CALL_C(0x777e, objectSetCollideRadius_hook, 0x24a1, 0x7781);
  CYC(0x7781, 0x7784); SET_HL(0x77d2);
  CYC(0x7784, 0x7787);
  interactionSetMiniScript_hook(gb);
  return;
state1:
  CALL_C(0x7787, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x778a);
  if (!(F & FC)) { RET_TAKEN(0x778a); return; }
  CYC(0x778a, 0x778b);
  CALL_C(0x778b, checkLinkCollisionsEnabled_hook, 0x1d32, 0x778e);
  if (!(F & FC)) { RET_TAKEN(0x778e); return; }
  CYC(0x778e, 0x778f);
  CYC(0x778f, 0x7791); A = 0x01;
  CYC(0x7791, 0x7794); mem_wr(gb, 0xcc8a, A);
  CYC(0x7794, 0x7796); A = 0x50;
  CALL_C(0x7796, playSound_b00_hook, 0x0c98, 0x7799);
  CYC(0x7799, 0x779c); SET_HL(0xd000);
  CALL_C(0x779c, objectTakePosition_hook, 0x2274, 0x779f);
  CYC(0x779f, 0x77a1); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x77a1, 0x77a3); A = 0x1e;
  CYC(0x77a3, 0x77a4); mem_wr(gb, DE, A);
  CYC(0x77a4, 0x77a7); SET_BC(0xf808);
  CALL_C(0x77a7, objectCreateExclamationMark_hook, 0x27e0, 0x77aa);
  CYC(0x77aa, 0x77ad);
  interactionIncState_hook(gb);
  return;
state2:
  CALL_C(0x77ad, interactionDecCounter1_hook, 0x23cc, 0x77b0);
  if (!(F & FZ)) { RET_TAKEN(0x77b0); return; }
  CYC(0x77b0, 0x77b1);
  CYC(0x77b1, 0x77b3); mem_wr(gb, HL, 0x1e);
  CYC(0x77b3, 0x77b4); alu_xor(gb, A);
  CYC(0x77b4, 0x77b7); mem_wr(gb, 0xcc8a, A);
  CYC(0x77b7, 0x77ba);
  interactionIncState_hook(gb);
  return;
state3:
  CALL_C(0x77ba, interactionDecCounter1_hook, 0x23cc, 0x77bd);
  if (!(F & FZ)) { RET_TAKEN(0x77bd); return; }
  CYC(0x77bd, 0x77be);
  CYC(0x77be, 0x77c0); mem_wr(gb, HL, 0x07);
  CALL_C(0x77c0, interactionGetMiniScript_hook, 0x2791, 0x77c3);
  CYC(0x77c3, 0x77c4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x77c4, 0x77c5); C = A;
  CALL_C(0x77c5, interactionSetMiniScript_hook, 0x2798, 0x77c8);
  CYC(0x77c8, 0x77c9); A = C;
  CYC(0x77c9, 0x77ca); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x77ca, 0x77cd);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x77ca, 0x77cd);
  CYC(0x77cd, 0x77cf); A = 0x48;
  CYC(0x77cf, 0x77d2);
  breakCrackedFloor_hook(gb);
}

void interactiondc_subid0C_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x77ee, checkInteractionState_hook, 0x23fe, 0x77f1);
  if (F & FZ) {
    CYCT(0x77f1, 0x77f3);
    goto state0;
  }
  CYC(0x77f1, 0x77f3);
  CYC(0x77f3, 0x77f6); A = mem_rd(gb, 0xcca0);
  CYC(0x77f6, 0x77f7); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x77f7); return; }
  CYC(0x77f7, 0x77f8);
  CYC(0x77f8, 0x77fa); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x77fa, 0x77fb); A = mem_rd(gb, DE);
  CYC(0x77fb, 0x77fd); alu_sub(gb, 0x0c);
  CYC(0x77fd, 0x7800); SET_BC(0x0801);
  CYC(0x7800, 0x7802); E = 0x56;
  if (F & FZ) {
    CYCT(0x7802, 0x7804);
    goto spawnBridge;
  }
  CYC(0x7802, 0x7804);
  CYC(0x7804, 0x7807); SET_BC(0x0603);
  CYC(0x7807, 0x7809); E = 0x28;
spawnBridge:
  CALL_C(0x7809, getFreePartSlot_hook, 0x3e8e, 0x780c);
  if (!(F & FZ)) { RET_TAKEN(0x780c); return; }
  CYC(0x780c, 0x780d);
  CYC(0x780d, 0x780f); mem_wr(gb, HL, 0x0c);
  CYC(0x780f, 0x7811); L = PART_BASE + OBJ_COUNTER2;
  CYC(0x7811, 0x7812); mem_wr(gb, HL, B);
  CYC(0x7812, 0x7814); L = PART_BASE + OBJ_ANGLE;
  CYC(0x7814, 0x7815); mem_wr(gb, HL, C);
  CYC(0x7815, 0x7817); L = PART_BASE + OBJ_YH;
  CYC(0x7817, 0x7818); mem_wr(gb, HL, E);
  CALL_C(0x7818, getThisRoomFlags_hook, 0x197d, 0x781b);
  CYC(0x781b, 0x781d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x781d, 0x781f); A = 0x4d;
  CALL_C(0x781f, playSound_b00_hook, 0x0c98, 0x7822);
  CYC(0x7822, 0x7825);
  interactionDelete_hook(gb);
  return;
state0:
  CALL_C(0x7825, getThisRoomFlags_hook, 0x197d, 0x7828);
  CYC(0x7828, 0x782a); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(0x782a, 0x782d);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x782a, 0x782d);
  CYC(0x782d, 0x7830);
  interactionIncState_hook(gb);
}

void interactiondc_subid0E_spawnPuff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x788a, getFreeInteractionSlot_hook, 0x3aef, 0x788d);
  if (!(F & FZ)) { RET_TAKEN(0x788d); return; }
  CYC(0x788d, 0x788e);
  CYC(0x788e, 0x7890); mem_wr(gb, HL, 0x05);
  CYC(0x7890, 0x7892); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x7892, 0x7895);
  setShortPosition_paramC_hook(gb);
}

void interactiondc_subid0E_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7830, 0x7832); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7832, 0x7833); A = mem_rd(gb, DE);
  CYC(0x7833, 0x7834); push_effect(gb, 0x7834);
  switch (interactiondc_jump_table(gb)) {
    case 0x783a: goto state0;
    case 0x7845: goto state1;
    case 0x7895: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CALL_C(0x783a, getThisRoomFlags_hook, 0x197d, 0x783d);
  CYC(0x783d, 0x783f); alu_and(gb, 0x80);
  if (!(F & FZ)) {
    CYCT(0x783f, 0x7842);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x783f, 0x7842);
  CYC(0x7842, 0x7845);
  interactionIncState_hook(gb);
  return;
state1:
  CALL_C(0x7845, objectGetTileAtPosition_hook, 0x1444, 0x7848);
  CYC(0x7848, 0x784a); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(0x784a); return; }
  CYC(0x784a, 0x784b);
  CALL_C(0x784b, checkLinkVulnerable_hook, 0x1d28, 0x784e);
  if (!(F & FC)) { RET_TAKEN(0x784e); return; }
  CYC(0x784e, 0x784f);
  CYC(0x784f, 0x7851); A = 0x81;
  CYC(0x7851, 0x7854); mem_wr(gb, 0xcc8a, A);
  CYC(0x7854, 0x7857); mem_wr(gb, 0xcc02, A);
  CYC(0x7857, 0x7859); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7859, 0x785b); A = 0x2d;
  CYC(0x785b, 0x785c); mem_wr(gb, DE, A);
  CALL_C(0x785c, interactionIncState_hook, 0x23e0, 0x785f);
  CYC(0x785f, 0x7861); C = 0x04;
  CYC(0x7861, 0x7863); A = 0x30;
  CALL_C(0x7863, setTile_hook, 0x3a9c, 0x7866);
  CYC(0x7866, 0x7867); C = alu_inc8(gb, C);
  CYC(0x7867, 0x7869); A = 0x32;
  CALL_C(0x7869, setTile_hook, 0x3a9c, 0x786c);
  CYC(0x786c, 0x786e); C = 0x14;
  CYC(0x786e, 0x7870); A = 0x3a;
  CALL_C(0x7870, setTile_hook, 0x3a9c, 0x7873);
  CYC(0x7873, 0x7874); C = alu_inc8(gb, C);
  CYC(0x7874, 0x7876); A = 0x3a;
  CALL_C(0x7876, setTile_hook, 0x3a9c, 0x7879);
  CYC(0x7879, 0x787b); C = 0x04;
  CYC(0x787b, 0x787e); push_effect(gb, 0x787e);
  interactiondc_subid0E_spawnPuff_hook(gb);
  CYC(0x787e, 0x7880); C = 0x05;
  CYC(0x7880, 0x7883); push_effect(gb, 0x7883);
  interactiondc_subid0E_spawnPuff_hook(gb);
  CYC(0x7883, 0x7885); C = 0x14;
  CYC(0x7885, 0x7888); push_effect(gb, 0x7888);
  interactiondc_subid0E_spawnPuff_hook(gb);
  CYC(0x7888, 0x788a); C = 0x15;
  interactiondc_subid0E_spawnPuff_hook(gb);
  return;
state2:
  CALL_C(0x7895, interactionDecCounter1_hook, 0x23cc, 0x7898);
  if (!(F & FZ)) { RET_TAKEN(0x7898); return; }
  CYC(0x7898, 0x7899);
  CYC(0x7899, 0x789b); A = 0x4d;
  CALL_C(0x789b, playSound_b00_hook, 0x0c98, 0x789e);
  CALL_C(0x789e, getThisRoomFlags_hook, 0x197d, 0x78a1);
  CYC(0x78a1, 0x78a3); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x78a3, 0x78a4); alu_xor(gb, A);
  CYC(0x78a4, 0x78a7); mem_wr(gb, 0xcc8a, A);
  CYC(0x78a7, 0x78aa); mem_wr(gb, 0xcc02, A);
  CYC(0x78aa, 0x78ad);
  interactionDelete_hook(gb);
}

void interactiondc_subid0F_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x78ad, checkInteractionState_hook, 0x23fe, 0x78b0);
  if (F & FZ) {
    CYCT(0x78b0, 0x78b2);
    goto state0;
  }
  CYC(0x78b0, 0x78b2);
  CALL_C(0x78b2, objectCheckCollidedWithLink_notDead_hook, 0x1c2e, 0x78b5);
  if (!(F & FC)) { RET_TAKEN(0x78b5); return; }
  CYC(0x78b5, 0x78b6);
  CYC(0x78b6, 0x78b9); SET_BC(0x120a);
  CYC(0x78b9, 0x78bc); A = mem_rd(gb, 0xcc30);
  CYC(0x78bc, 0x78be); alu_cp(gb, 0xd0);
  if (!(F & FZ)) {
    CYCT(0x78be, 0x78c0);
    goto showText;
  }
  CYC(0x78be, 0x78c0);
  CYC(0x78c0, 0x78c3); SET_BC(0x0209);
showText:
  CALL_C(0x78c3, showText_hook, 0x1872, 0x78c6);
  CYC(0x78c6, 0x78c9);
  interactionDelete_hook(gb);
  return;
state0:
  CYC(0x78c9, 0x78cc); A = mem_rd(gb, 0xcd00);
  CYC(0x78cc, 0x78ce); alu_and(gb, 0x02);
  if (F & FZ) {
    CYCT(0x78ce, 0x78d1);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x78ce, 0x78d1);
  CYC(0x78d1, 0x78d4); A = mem_rd(gb, 0xd00b);
  CYC(0x78d4, 0x78d6); alu_cp(gb, 0x78);
  if (F & FC) {
    CYCT(0x78d6, 0x78d9);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x78d6, 0x78d9);
  CYC(0x78d9, 0x78db); A = 0x08;
  CALL_C(0x78db, objectSetCollideRadius_hook, 0x24a1, 0x78de);
  CYC(0x78de, 0x78e1);
  interactionIncState_hook(gb);
}

void interactiondc_subid10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x78e1, 0x78e3); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x78e3, 0x78e4); A = mem_rd(gb, DE);
  CYC(0x78e4, 0x78e5); push_effect(gb, 0x78e5);
  switch (interactiondc_jump_table(gb)) {
    case 0x78eb: goto state0;
    case 0x7900: goto state1;
    case 0x7907: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CYC(0x78eb, 0x78ee); SET_HL(0xcf44);
  CYC(0x78ee, 0x78ef); alu_xor(gb, A);
  CYC(0x78ef, 0x78f0); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x78f0, 0x78f1); mem_wr(gb, HL, A);
  CYC(0x78f1, 0x78f4); SET_BC(0x0410);
  CALL_C(0x78f4, objectSetCollideRadii_hook, 0x24a9, 0x78f7);
  CALL_C(0x78f7, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x78fa);
  if (!(F & FC)) {
    CALL_C_CC(0x78fa, interactionIncState_hook, 0x23e0, 0x78fd);
  } else {
    CYC(0x78fa, 0x78fd);
  }
  CYC(0x78fd, 0x7900);
  interactionIncState_hook(gb);
  return;
state1:
  CALL_C(0x7900, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x7903);
  if (F & FC) { RET_TAKEN(0x7903); return; }
  CYC(0x7903, 0x7904);
  CYC(0x7904, 0x7907);
  interactionIncState_hook(gb);
  return;
state2:
  CALL_C(0x7907, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, 0x1c28, 0x790a);
  if (!(F & FC)) { RET_TAKEN(0x790a); return; }
  CYC(0x790a, 0x790b);
  CALL_C(0x790b, checkLinkVulnerable_hook, 0x1d28, 0x790e);
  if (!(F & FC)) { RET_TAKEN(0x790e); return; }
  CYC(0x790e, 0x790f);
  CALL_C(0x790f, getThisRoomFlags_hook, 0x197d, 0x7912);
  CYC(0x7912, 0x7914); alu_and(gb, 0x01);
  CYC(0x7914, 0x7917); SET_HL(0x7927);
  if (F & FZ) {
    CYCT(0x7917, 0x7919);
    goto warpDest;
  }
  CYC(0x7917, 0x7919);
  CYC(0x7919, 0x791c); SET_HL(0x792c);
warpDest:
  CALL_C(0x791c, setWarpDestVariables_hook, 0x1997, 0x791f);
  CYC(0x791f, 0x7921); A = 0x6e;
  CALL_C(0x7921, playSound_b00_hook, 0x0c98, 0x7924);
  CYC(0x7924, 0x7927);
  interactionDelete_hook(gb);
}

void interactiondc_subid11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7931, getThisRoomFlags_hook, 0x197d, 0x7934);
  CYC(0x7934, 0x7936); alu_and(gb, 0x20);
  if (F & FZ) { RET_TAKEN(0x7936); return; }
  CYC(0x7936, 0x7937);
  CYC(0x7937, 0x793a); SET_HL(0xc682);
  CYC(0x793a, 0x793c); A = 0x0c;
  CYC(0x793c, 0x793f);
  setFlag_hook(gb);
}

void interactiondc_subid12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x793f, getThisRoomFlags_hook, 0x197d, 0x7942);
  CYC(0x7942, 0x7944); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(0x7944, 0x7947);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7944, 0x7947);
  CYC(0x7947, 0x794a); A = mem_rd(gb, 0xcdd2);
  CYC(0x794a, 0x794b); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x794b); return; }
  CYC(0x794b, 0x794c);
  CALL_C(0x794c, getFreePartSlot_hook, 0x3e8e, 0x794f);
  if (!(F & FZ)) { RET_TAKEN(0x794f); return; }
  CYC(0x794f, 0x7950);
  CYC(0x7950, 0x7952); mem_wr(gb, HL, 0x0c);
  CYC(0x7952, 0x7954); L = PART_BASE + OBJ_COUNTER2;
  CYC(0x7954, 0x7956); mem_wr(gb, HL, 0x0c);
  CYC(0x7956, 0x7958); L = PART_BASE + OBJ_ANGLE;
  CYC(0x7958, 0x795a); mem_wr(gb, HL, 0x01);
  CYC(0x795a, 0x795c); L = PART_BASE + OBJ_YH;
  CYC(0x795c, 0x795e); mem_wr(gb, HL, 0x13);
  CALL_C(0x795e, getThisRoomFlags_hook, 0x197d, 0x7961);
  CYC(0x7961, 0x7963); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(0x7963, 0x7965); A = 0x4d;
  CALL_C(0x7965, playSound_b00_hook, 0x0c98, 0x7968);
  CYC(0x7968, 0x796b);
  interactionDelete_hook(gb);
}

void interactiondc_subid13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x796b, returnIfScrollMode01Unset_hook, 0x26e4, 0x796e);
  CYC(0x796e, 0x7970); A = 0xe4;
  CYC(0x7970, 0x7973); SET_HL(0xcf14);
  CYC(0x7973, 0x7974); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7974, 0x7975); mem_wr(gb, HL, A);
  CYC(0x7975, 0x7977); L = 0x24;
  CYC(0x7977, 0x7978); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7978, 0x7979); mem_wr(gb, HL, A);
  CYC(0x7979, 0x797b); L = 0x34;
  CYC(0x797b, 0x797c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x797c, 0x797d); mem_wr(gb, HL, A);
  CYC(0x797d, 0x7980);
  interactionDelete_hook(gb);
}

void interactiondc_subid14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7980, objectGetTileAtPosition_hook, 0x1444, 0x7983);
  CYC(0x7983, 0x7985); alu_cp(gb, 0xdc);
  if (!(F & FZ)) {
    CYCT(0x7985, 0x7987);
    goto delete_;
  }
  CYC(0x7985, 0x7987);
  CYC(0x7987, 0x7989); B = 0x80;
  CALL_C(0x7989, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x798c);
delete_:
  CYC(0x798c, 0x798f);
  interactionDelete_hook(gb);
}

void interactiondc_subid15And16_setChestContents_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x799c, 0x799f); SET_HL(0x79ab);
  CYC(0x799f, 0x79a0);
  interactiondc_addDoubleIndexToHl_from_rst(gb, 0x79a0);
  CYC(0x79a0, 0x79a1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x79a1, 0x79a4); mem_wr(gb, 0xcca3, A);
  CYC(0x79a4, 0x79a5); A = mem_rd(gb, HL);
  CYC(0x79a5, 0x79a8); mem_wr(gb, 0xcca4, A);
  CYC(0x79a8, 0x79ab);
  interactionDelete_hook(gb);
}

void interactiondc_subid15And16_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x79af, getThisRoomFlags_hook, 0x197d, 0x79b2);
  CYC(0x79b2, 0x79b4); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x79b4, 0x79b7);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x79b4, 0x79b7);
  CYC(0x79b7, 0x79ba);
  interactionIncState_hook(gb);
}

void interactiondc_subid15_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x798f, checkInteractionState_hook, 0x23fe, 0x7992);
  if (F & FZ) {
    CYCT(0x7992, 0x7994);
    interactiondc_subid15And16_state0_hook(gb);
    return;
  }
  CYC(0x7992, 0x7994);
  CALL_C(0x7994, checkIsLinkedGame_hook, 0x1992, 0x7997);
  CYC(0x7997, 0x7999); A = 0x01;
  if (!(F & FZ)) {
    CYCT(0x7999, 0x799b);
    interactiondc_subid15And16_setChestContents_hook(gb);
    return;
  }
  CYC(0x7999, 0x799b);
  CYC(0x799b, 0x799c); A = alu_dec8(gb, A);
  interactiondc_subid15And16_setChestContents_hook(gb);
}

void interactiondc_subid16_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x79ba, checkInteractionState_hook, 0x23fe, 0x79bd);
  if (F & FZ) {
    CYCT(0x79bd, 0x79bf);
    interactiondc_subid15And16_state0_hook(gb);
    return;
  }
  CYC(0x79bd, 0x79bf);
  CALL_C(0x79bf, checkIsLinkedGame_hook, 0x1992, 0x79c2);
  CYC(0x79c2, 0x79c4); A = 0x00;
  if (!(F & FZ)) {
    CYCT(0x79c4, 0x79c6);
    interactiondc_subid15And16_setChestContents_hook(gb);
    return;
  }
  CYC(0x79c4, 0x79c6);
  CYC(0x79c6, 0x79c7); A = alu_inc8(gb, A);
  CYC(0x79c7, 0x79c9);
  interactiondc_subid15And16_setChestContents_hook(gb);
}

void interactiondc_subid17_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x79c9, checkInteractionState_hook, 0x23fe, 0x79cc);
  if (F & FZ) {
    CYCT(0x79cc, 0x79cf);
    interactionIncState_hook(gb);
    return;
  }
  CYC(0x79cc, 0x79cf);
  CYC(0x79cf, 0x79d2); A = mem_rd(gb, 0xd00b);
  CYC(0x79d2, 0x79d3); B = A;
  CYC(0x79d3, 0x79d6); A = mem_rd(gb, 0xd00d);
  CYC(0x79d6, 0x79d7); C = A;
  CYC(0x79d7, 0x79da); SET_HL(0x5e92);
  CYC(0x79da, 0x79dc); E = 0x05;
  CALL_C(0x79dc, interBankCall_hook, 0x008a, 0x79df);
  CYC(0x79df, 0x79e1); B = alu_rl(gb, B);
  if (!(F & FC)) { RET_TAKEN(0x79e1); return; }
  CYC(0x79e1, 0x79e2);
  CYC(0x79e2, 0x79e5); A = mem_rd(gb, 0xd004);
  CYC(0x79e5, 0x79e7); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(0x79e7); return; }
  CYC(0x79e7, 0x79e8);
  CYC(0x79e8, 0x79eb); SET_HL(0xcc4f);
  CYC(0x79eb, 0x79ec); A = mem_rd(gb, HL);
  CYC(0x79ec, 0x79ed); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x79ed); return; }
  CYC(0x79ed, 0x79ee);
  CYC(0x79ee, 0x79f0); A = 0x11;
  CYC(0x79f0, 0x79f1); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x79f1, 0x79f4); A = mem_rd(gb, 0xcca6);
  CYC(0x79f4, 0x79f6); alu_and(gb, 0x08);
  CYC(0x79f6, 0x79f8); alu_xor(gb, 0x08);
  CYC(0x79f8, 0x79f9); mem_wr(gb, HL, A);
  RET(0x79f9); return;
}

void interactionCodedc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7481, 0x7483); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7483, 0x7484); A = mem_rd(gb, DE);
  CYC(0x7484, 0x7485); push_effect(gb, 0x7485);
  switch (interactiondc_jump_table(gb)) {
    case 0x74fd: interactiondc_subid00_hook(gb); return;
    case 0x7517: interactiondc_subid01_hook(gb); return;
    case 0x7531: interactiondc_subid02_hook(gb); return;
    case 0x75b0: interactiondc_subid03_hook(gb); return;
    case 0x75e1: interactiondc_subid04_hook(gb); return;
    case 0x75f6: interactiondc_subid05_hook(gb); return;
    case 0x7659: interactiondc_subid06_hook(gb); return;
    case 0x74b5: interactiondc_subid07_hook(gb); return;
    case 0x74c9: interactiondc_subid08_hook(gb); return;
    case 0x7669: interactiondc_subid09_hook(gb); return;
    case 0x7741: interactiondc_subid0A_hook(gb); return;
    case 0x776d: interactiondc_subid0B_hook(gb); return;
    case 0x77ee: interactiondc_subid0C_hook(gb); return;
    case 0x7830: interactiondc_subid0E_hook(gb); return;
    case 0x78ad: interactiondc_subid0F_hook(gb); return;
    case 0x78e1: interactiondc_subid10_hook(gb); return;
    case 0x7931: interactiondc_subid11_hook(gb); return;
    case 0x793f: interactiondc_subid12_hook(gb); return;
    case 0x796b: interactiondc_subid13_hook(gb); return;
    case 0x7980: interactiondc_subid14_hook(gb); return;
    case 0x798f: interactiondc_subid15_hook(gb); return;
    case 0x79ba: interactiondc_subid16_hook(gb); return;
    case 0x79c9: interactiondc_subid17_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
