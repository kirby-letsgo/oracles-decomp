#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t bank09_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

void interactionCode4c_hook(GB *gb);
void interactionCode4c__state0_hook(GB *gb);
void interactionCode4c__afterCall5ffe_hook(GB *gb);
void interactionCode4c__initSubid_hook(GB *gb);
void interactionCode4c__initSubid00_hook(GB *gb);
void interactionCode4c__initSubid04_hook(GB *gb);
void interactionCode4c__impaNotMoved_hook(GB *gb);
void interactionCode4c__initSubid01_hook(GB *gb);
void interactionCode4c__setAnimation0AndJump_hook(GB *gb);
void interactionCode4c__state1_hook(GB *gb);
void bird_runSubid0_hook(GB *gb);
void bird_runSubid0__substate0_hook(GB *gb);
void bird_runSubid0__substate1_hook(GB *gb);
void bird_runSubid0__substate2_hook(GB *gb);
void bird_runSubid0__substate3_hook(GB *gb);
void bird_runSubid4_hook(GB *gb);
void bird_updateGravityAndHopWhenHitGround_hook(GB *gb);
void bird_hop_hook(GB *gb);

void interactionCode4c_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ff0, 0x5ff2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x5ff2, 0x5ff3); A = mem_rd(gb, DE);
  CYC(0x5ff3, 0x5ff4); push_effect(gb, 0x5ff4); SET_HL(bank09_jump_table(gb));
  switch (HL) {
    case 0x5ff8: interactionCode4c__state0_hook(gb); return;
    case 0x6069: interactionCode4c__state1_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void interactionCode4c__state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5ff8:
  CYC(0x5ff8, 0x5ffa); A = 0x01;
  CYC(0x5ffa, 0x5ffb); mem_wr(gb, DE, A);
  CALL_C(0x5ffb, interactionInitGraphics_hook, 0x15fb, 0x5ffe);
L_5ffe:
  CALL_C(0x5ffe, objectSetVisiblec2_hook, 0x1e45, 0x6001);
  CALL_C(0x6001, interactionCode4c__initSubid_hook, 0x600c, 0x6004);
  CYC(0x6004, 0x6006); E = 0x40;
  CYC(0x6006, 0x6007); A = mem_rd(gb, DE);
  CYC(0x6007, 0x6008); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6008, 0x600b); if (hook_enabled_at(0x24f0)) { objectMarkSolidPosition_hook(gb); return; } HANDOFF(0x24f0); } CYC(0x6008, 0x600b);
  CYC(0x600b, 0x600c); ret_effect(gb); return;
}

void interactionCode4c__afterCall5ffe_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5ffe:
  CALL_C(0x5ffe, objectSetVisiblec2_hook, 0x1e45, 0x6001);
  CALL_C(0x6001, interactionCode4c__initSubid_hook, 0x600c, 0x6004);
  CYC(0x6004, 0x6006); E = 0x40;
  CYC(0x6006, 0x6007); A = mem_rd(gb, DE);
  CYC(0x6007, 0x6008); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6008, 0x600b); if (hook_enabled_at(0x24f0)) { objectMarkSolidPosition_hook(gb); return; } HANDOFF(0x24f0); } CYC(0x6008, 0x600b);
  CYC(0x600b, 0x600c); ret_effect(gb); return;
}

void interactionCode4c__initSubid_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_600c:
  CYC(0x600c, 0x600e); E = 0x42;
  CYC(0x600e, 0x600f); A = mem_rd(gb, DE);
  CYC(0x600f, 0x6010); push_effect(gb, 0x6010); SET_HL(bank09_jump_table(gb));
  switch (HL) { case 0x601a: goto L_601a; case 0x6023: goto L_6023; case 0x605e: goto L_605e; default: HANDOFF(HL); }
L_601a:
  CALL_C(0x601a, bird_hop_hook, 0x6108, 0x601d);
  CYC(0x601d, 0x6020); SET_HL(0x6369);
  CYC(0x6020, 0x6023); if (hook_enabled_at(0x2544)) { interactionSetScript_hook(gb); return; } HANDOFF(0x2544);
L_6023:
  CYC(0x6023, 0x6026); A = mem_rd(gb, 0xc6bf);
  CYC(0x6026, 0x6028); alu_bit(gb, 2, A);
  if ((F & FZ)) { CYCT(0x6028, 0x602b); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6028, 0x602b);
  CALL_C(0x602b, checkIsLinkedGame_hook, 0x1992, 0x602e);
  if ((F & FZ)) { CYCT(0x602e, 0x6031); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x602e, 0x6031);
  CYC(0x6031, 0x6033); A = 0x38;
  CALL_C(0x6033, checkGlobalFlag_hook, 0x31f3, 0x6036);
  if (!(F & FZ)) { CYCT(0x6036, 0x6039); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6036, 0x6039);
  CYC(0x6039, 0x603c); SET_HL(0x6390);
  CALL_C(0x603c, interactionSetScript_hook, 0x2544, 0x603f);
  CALL_C(0x603f, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6042);
  CYC(0x6042, 0x6044); A = 0x39;
  CALL_C(0x6044, checkGlobalFlag_hook, 0x31f3, 0x6047);
  if ((F & FZ)) { CYCT(0x6047, 0x6049); goto L_6062; } CYC(0x6047, 0x6049);
  CYC(0x6049, 0x604b); A = 0x3c;
  CALL_C(0x604b, checkGlobalFlag_hook, 0x31f3, 0x604e);
  if ((F & FZ)) { CYCT(0x604e, 0x6050); goto L_6057; } CYC(0x604e, 0x6050);
  CYC(0x6050, 0x6052); E = 0x4b;
  CYC(0x6052, 0x6054); A = 0x58;
  CYC(0x6054, 0x6055); mem_wr(gb, DE, A);
  CYC(0x6055, 0x6057); goto L_6062;
L_6057:
  CYC(0x6057, 0x6059); E = 0x4d;
  CYC(0x6059, 0x605b); A = 0x68;
  CYC(0x605b, 0x605c); mem_wr(gb, DE, A);
  CYC(0x605c, 0x605e); goto L_6062;
L_605e:
  CYC(0x605e, 0x605f); A = mem_rd(gb, DE);
  CYC(0x605f, 0x6061); E = 0x5c;
  CYC(0x6061, 0x6062); mem_wr(gb, DE, A);
L_6062:
  CYC(0x6062, 0x6063); alu_xor(gb, A);
  CALL_C(0x6063, interactionSetAnimation_hook, 0x262e, 0x6066);
  CYC(0x6066, 0x6069); bird_hop_hook(gb); return;
}

void interactionCode4c__initSubid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_601a:
  CALL_C(0x601a, bird_hop_hook, 0x6108, 0x601d);
  CYC(0x601d, 0x6020); SET_HL(0x6369);
  CYC(0x6020, 0x6023); if (hook_enabled_at(0x2544)) { interactionSetScript_hook(gb); return; } HANDOFF(0x2544);
}

void interactionCode4c__initSubid04_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6023:
  CYC(0x6023, 0x6026); A = mem_rd(gb, 0xc6bf);
  CYC(0x6026, 0x6028); alu_bit(gb, 2, A);
  if ((F & FZ)) { CYCT(0x6028, 0x602b); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6028, 0x602b);
  CALL_C(0x602b, checkIsLinkedGame_hook, 0x1992, 0x602e);
  if ((F & FZ)) { CYCT(0x602e, 0x6031); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x602e, 0x6031);
  CYC(0x6031, 0x6033); A = 0x38;
  CALL_C(0x6033, checkGlobalFlag_hook, 0x31f3, 0x6036);
  if (!(F & FZ)) { CYCT(0x6036, 0x6039); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x6036, 0x6039);
  CYC(0x6039, 0x603c); SET_HL(0x6390);
  CALL_C(0x603c, interactionSetScript_hook, 0x2544, 0x603f);
  CALL_C(0x603f, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6042);
  CYC(0x6042, 0x6044); A = 0x39;
  CALL_C(0x6044, checkGlobalFlag_hook, 0x31f3, 0x6047);
  if ((F & FZ)) { CYCT(0x6047, 0x6049); goto L_6062; } CYC(0x6047, 0x6049);
  CYC(0x6049, 0x604b); A = 0x3c;
  CALL_C(0x604b, checkGlobalFlag_hook, 0x31f3, 0x604e);
  if ((F & FZ)) { CYCT(0x604e, 0x6050); goto L_6057; } CYC(0x604e, 0x6050);
  CYC(0x6050, 0x6052); E = 0x4b;
  CYC(0x6052, 0x6054); A = 0x58;
  CYC(0x6054, 0x6055); mem_wr(gb, DE, A);
  CYC(0x6055, 0x6057); goto L_6062;
L_6057:
  CYC(0x6057, 0x6059); E = 0x4d;
  CYC(0x6059, 0x605b); A = 0x68;
  CYC(0x605b, 0x605c); mem_wr(gb, DE, A);
  CYC(0x605c, 0x605e); goto L_6062;
L_6062:
  CYC(0x6062, 0x6063); alu_xor(gb, A);
  CALL_C(0x6063, interactionSetAnimation_hook, 0x262e, 0x6066);
  CYC(0x6066, 0x6069); bird_hop_hook(gb); return;
}

void interactionCode4c__impaNotMoved_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6057:
  CYC(0x6057, 0x6059); E = 0x4d;
  CYC(0x6059, 0x605b); A = 0x68;
  CYC(0x605b, 0x605c); mem_wr(gb, DE, A);
  CYC(0x605c, 0x605e); goto L_6062;
L_6062:
  CYC(0x6062, 0x6063); alu_xor(gb, A);
  CALL_C(0x6063, interactionSetAnimation_hook, 0x262e, 0x6066);
  CYC(0x6066, 0x6069); bird_hop_hook(gb); return;
}

void interactionCode4c__initSubid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_605e:
  CYC(0x605e, 0x605f); A = mem_rd(gb, DE);
  CYC(0x605f, 0x6061); E = 0x5c;
  CYC(0x6061, 0x6062); mem_wr(gb, DE, A);
L_6062:
  CYC(0x6062, 0x6063); alu_xor(gb, A);
  CALL_C(0x6063, interactionSetAnimation_hook, 0x262e, 0x6066);
  CYC(0x6066, 0x6069); bird_hop_hook(gb); return;
}

void interactionCode4c__setAnimation0AndJump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6062:
  CYC(0x6062, 0x6063); alu_xor(gb, A);
  CALL_C(0x6063, interactionSetAnimation_hook, 0x262e, 0x6066);
  CYC(0x6066, 0x6069); bird_hop_hook(gb); return;
}

void interactionCode4c__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6069:
  CYC(0x6069, 0x606b); E = 0x42;
  CYC(0x606b, 0x606c); A = mem_rd(gb, DE);
  CYC(0x606c, 0x606d); push_effect(gb, 0x606d); SET_HL(bank09_jump_table(gb));
  switch (HL) {
    case 0x6077: bird_runSubid0_hook(gb); return;
    case 0x60e0: bird_runSubid4_hook(gb); return;
    case 0x261b: interactionAnimate_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void bird_runSubid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6077, interactionAnimateAsNpc_hook, 0x26db, 0x607a);
  CYC(0x607a, 0x607c); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x607c, 0x607d); A = mem_rd(gb, DE);
  CYC(0x607d, 0x607e); push_effect(gb, 0x607e); SET_HL(bank09_jump_table(gb));
  switch (HL) {
    case 0x6086: bird_runSubid0__substate0_hook(gb); return;
    case 0x609f: bird_runSubid0__substate1_hook(gb); return;
    case 0x60b4: bird_runSubid0__substate2_hook(gb); return;
    case 0x60d3: bird_runSubid0__substate3_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void bird_runSubid0__substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6086:
  CYC(0x6086, 0x6089); A = mem_rd(gb, 0xcfd0);
  CYC(0x6089, 0x608b); alu_cp(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x608b, 0x608d); goto L_6095; } CYC(0x608b, 0x608d);
  CALL_C(0x608d, interactionIncSubstate_hook, 0x23e5, 0x6090);
  CYC(0x6090, 0x6092); A = 0x01;
  CYC(0x6092, 0x6095); if (hook_enabled_at(0x262e)) { interactionSetAnimation_hook(gb); return; } HANDOFF(0x262e);
L_6095:
  CYC(0x6095, 0x6097); E = 0x77;
  CYC(0x6097, 0x6098); A = mem_rd(gb, DE);
  CYC(0x6098, 0x6099); alu_or(gb, A);
  if (!(F & FZ)) { CALL_C_CC(0x6099, bird_updateGravityAndHopWhenHitGround_hook, 0x6101, 0x609c); } else CYC(0x6099, 0x609c);
  CYC(0x609c, 0x609f); if (hook_enabled_at(0x2552)) { interactionRunScript_hook(gb); return; } HANDOFF(0x2552);
}

void bird_runSubid0__substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_609f:
  CYC(0x609f, 0x60a2); A = mem_rd(gb, 0xcfd0);
  CYC(0x60a2, 0x60a4); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(0x60a4, 0x60a5); ret_effect(gb); return; } CYC(0x60a4, 0x60a5);
  CALL_C(0x60a5, interactionIncSubstate_hook, 0x23e5, 0x60a8);
  CYC(0x60a8, 0x60aa); L = 0x46;
  CYC(0x60aa, 0x60ac); mem_wr(gb, HL, 0x1e);
  CALL_C(0x60ac, bird_hop_hook, 0x6108, 0x60af);
  CYC(0x60af, 0x60b1); A = 0x02;
  CYC(0x60b1, 0x60b4); if (hook_enabled_at(0x262e)) { interactionSetAnimation_hook(gb); return; } HANDOFF(0x262e);
}

void bird_runSubid0__substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_60b4:
  CALL_C(0x60b4, interactionDecCounter1_hook, 0x23cc, 0x60b7);
  if (!(F & FZ)) { CYCT(0x60b7, 0x60b9); bird_updateGravityAndHopWhenHitGround_hook(gb); return; } CYC(0x60b7, 0x60b9);
  CALL_C(0x60b9, interactionIncSubstate_hook, 0x23e5, 0x60bc);
  CYC(0x60bc, 0x60be); L = 0x4f;
  CYC(0x60be, 0x60c0); mem_wr(gb, HL, 0x00);
  CYC(0x60c0, 0x60c2); L = 0x49;
  CYC(0x60c2, 0x60c4); mem_wr(gb, HL, 0x01);
  CYC(0x60c4, 0x60c6); L = 0x50;
  CYC(0x60c6, 0x60c8); mem_wr(gb, HL, 0x28);
  CYC(0x60c8, 0x60cb); SET_BC(0xff00);
  CALL_C(0x60cb, objectSetSpeedZ_hook, 0x239d, 0x60ce);
  CYC(0x60ce, 0x60d0); A = 0x03;
  CYC(0x60d0, 0x60d3); if (hook_enabled_at(0x262e)) { interactionSetAnimation_hook(gb); return; } HANDOFF(0x262e);
}

void bird_runSubid0__substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_60d3:
  CALL_C(0x60d3, objectCheckWithinScreenBoundary_hook, 0x2184, 0x60d6);
  if (!(F & FC)) { CYCT(0x60d6, 0x60d9); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x60d6, 0x60d9);
  CYC(0x60d9, 0x60da); alu_xor(gb, A);
  CALL_C(0x60da, objectUpdateSpeedZ_hook, 0x1f45, 0x60dd);
  CYC(0x60dd, 0x60e0); if (hook_enabled_at(0x201d)) { objectApplySpeed_hook(gb); return; } HANDOFF(0x201d);
}

void bird_runSubid4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x60e0, interactionAnimateAsNpc_hook, 0x26db, 0x60e3);
  CALL_C(0x60e3, bird_updateGravityAndHopWhenHitGround_hook, 0x6101, 0x60e6);
  CALL_C(0x60e6, interactionRunScript_hook, 0x2552, 0x60e9);
  if ((F & FC)) { CYCT(0x60e9, 0x60ec); if (hook_enabled_at(0x3b05)) { interactionDelete_hook(gb); return; } HANDOFF(0x3b05); } CYC(0x60e9, 0x60ec);
  CYC(0x60ec, 0x60ee); A = 0x39;
  CALL_C(0x60ee, checkGlobalFlag_hook, 0x31f3, 0x60f1);
  if ((F & FZ)) { CYCT(0x60f1, 0x60f2); ret_effect(gb); return; } CYC(0x60f1, 0x60f2);
  CYC(0x60f2, 0x60f4); A = 0x3c;
  CALL_C(0x60f4, checkGlobalFlag_hook, 0x31f3, 0x60f7);
  if (!(F & FZ)) { CYCT(0x60f7, 0x60f8); ret_effect(gb); return; } CYC(0x60f7, 0x60f8);
  CYC(0x60f8, 0x60fa); E = 0x4d;
  CYC(0x60fa, 0x60fb); A = mem_rd(gb, DE);
  CYC(0x60fb, 0x60fd); alu_cp(gb, 0x68);
  if ((F & FZ)) { CYCT(0x60fd, 0x60fe); ret_effect(gb); return; } CYC(0x60fd, 0x60fe);
  CYC(0x60fe, 0x60ff); A = alu_inc8(gb, A);
  CYC(0x60ff, 0x6100); mem_wr(gb, DE, A);
  CYC(0x6100, 0x6101); ret_effect(gb); return;
}

void bird_updateGravityAndHopWhenHitGround_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6101, 0x6103); C = 0x20;
  CALL_C(0x6103, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6106);
  if (F & FZ) {
    CYC(0x6106, 0x6107);
    CYC(0x6107, 0x6108); H = D;
    bird_hop_hook(gb);
    return;
  }
  CYCT(0x6106, 0x6107); ret_effect(gb);
}

void bird_hop_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6108, 0x610b); SET_BC(0xff40);
  CYC(0x610b, 0x610e); if (hook_enabled_at(0x239d)) { objectSetSpeedZ_hook(gb); return; } HANDOFF(0x239d);
}
