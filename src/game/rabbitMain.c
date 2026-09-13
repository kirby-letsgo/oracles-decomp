#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

static uint16_t rabbit_jump_table(GB *gb) {
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

static void rabbit_subid0_jump(GB *gb, uint16_t sp0_) {
  CYC(0x7923, 0x7926); SET_BC(0xfe00);
  CALL_C(0x7926, objectSetSpeedZ_hook, 0x239d, 0x7929);
  CYC(0x7929, 0x792b); A = 0x04;
  CYC(0x792b, 0x792e);
  interactionSetAnimation_hook(gb);
}

static void rabbit_subid0_substate0(GB *gb, uint16_t sp0_) {
  CYC(0x78f2, 0x78f5); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x78f5, 0x78f7); alu_cp(gb, 0x0e);
  if (!(F & FZ)) {
    CYCT(0x78f7, 0x78fa);
    interactionRunScript_hook(gb);
    return;
  }
  CYC(0x78f7, 0x78fa);
  CALL_C(0x78fa, interactionIncSubstate_hook, 0x23e5, 0x78fd);
  CYC(0x78fd, 0x78ff); A = 0x02;
  CYC(0x78ff, 0x7902);
  interactionSetAnimation_hook(gb);
}

static void rabbit_subid0_substate1(GB *gb, uint16_t sp0_) {
  CYC(0x7902, 0x7905); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd0);
  CYC(0x7905, 0x7907); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(0x7907, 0x790a);
    interactionRunScript_hook(gb);
    return;
  }
  CYC(0x7907, 0x790a);
  CALL_C(0x790a, interactionIncSubstate_hook, 0x23e5, 0x790d);
  CYC(0x790d, 0x790f); L = 0x46;
  CYC(0x790f, 0x7911); mem_wr(gb, HL, 0x28);
  CYC(0x7911, 0x7912); ret_effect(gb);
}

static void rabbit_subid0_substate2(GB *gb, uint16_t sp0_) {
  CALL_C(0x7912, interactionDecCounter1_hook, 0x23cc, 0x7915);
  if (!(F & FZ)) {
    CYCT(0x7915, 0x7918);
    interactionAnimate_hook(gb);
    return;
  }
  CYC(0x7915, 0x7918);
  CALL_C(0x7918, interactionIncSubstate_hook, 0x23e5, 0x791b);
  CYC(0x791b, 0x791d); L = 0x49;
  CYC(0x791d, 0x791f); mem_wr(gb, HL, 0x06);
  CYC(0x791f, 0x7921); L = 0x50;
  CYC(0x7921, 0x7923); mem_wr(gb, HL, 0x3c);
  rabbit_subid0_jump(gb, sp0_);
}

static void rabbit_subid0_substate3(GB *gb, uint16_t sp0_) {
  CALL_C(0x792e, objectCheckWithinScreenBoundary_hook, 0x2184, 0x7931);
  if (!(F & FC)) {
    CYCT(0x7931, 0x7934);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7931, 0x7934);
  CYC(0x7934, 0x7936); C = 0x20;
  CALL_C(0x7936, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7939);
  if (!(F & FZ)) {
    CYCT(0x7939, 0x793c);
    objectApplySpeed_hook(gb);
    return;
  }
  CYC(0x7939, 0x793c);
  CYC(0x793c, 0x793e);
  rabbit_subid0_jump(gb, sp0_);
}

void rabbitSubid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x78e3, interactionAnimateAsNpc_hook, 0x26db, 0x78e6);
  CYC(0x78e6, 0x78e8); E = 0x45;
  CYC(0x78e8, 0x78e9); A = mem_rd(gb, DE);
  CYC(0x78e9, 0x78ea); push_effect(gb, 0x78ea);
  switch (rabbit_jump_table(gb)) {
    case 0x78f2: rabbit_subid0_substate0(gb, sp0_); return;
    case 0x7902: rabbit_subid0_substate1(gb, sp0_); return;
    case 0x7912: rabbit_subid0_substate2(gb, sp0_); return;
    case 0x792e: rabbit_subid0_substate3(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void rabbit_set_jump_animation(GB *gb, uint16_t sp0_) {
  CYC(0x7856, 0x7858); A = 0x05;
  CALL_C(0x7858, interactionSetAnimation_hook, 0x262e, 0x785b);
  CYC(0x785b, 0x785e); SET_BC(0xfe80);
  CYC(0x785e, 0x7861);
  objectSetSpeedZ_hook(gb);
}

static void rabbit_subid1_substate0(GB *gb, uint16_t sp0_) {
  CALL_C(0x7968, interactionAnimate_hook, 0x261b, 0x796b);
  CYC(0x796b, 0x796d); E = 0x61;
  CYC(0x796d, 0x796e); A = mem_rd(gb, DE);
  CYC(0x796e, 0x796f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x796f, 0x7970); ret_effect(gb);
    return;
  }
  CYC(0x796f, 0x7970);
  CYC(0x7970, 0x7972); A = 0x53;
  CALL_C(0x7972, playSound_b00_hook, 0x0c98, 0x7975);
  CYC(0x7975, 0x7978);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid1_substate1(GB *gb, uint16_t sp0_) {
  CYC(0x7978, 0x797a); E = 0x4d;
  CYC(0x797a, 0x797b); A = mem_rd(gb, DE);
  CYC(0x797b, 0x797d); alu_cp(gb, 0xd0);
  if (!(F & FC)) {
    CYCT(0x797d, 0x7980);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x797d, 0x7980);
  CALL_C(0x7980, objectApplySpeed_hook, 0x201d, 0x7983);
  CYC(0x7983, 0x7985); C = 0x20;
  CALL_C(0x7985, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7988);
  if (!(F & FZ)) {
    CYCT(0x7988, 0x7989); ret_effect(gb);
    return;
  }
  CYC(0x7988, 0x7989);
  CYC(0x7989, 0x798a); H = D;
  CYC(0x798a, 0x798c); L = 0x45;
  CYC(0x798c, 0x798d); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x798d, 0x7990);
  rabbit_set_jump_animation(gb, sp0_);
}

static void rabbit_subid1_substate2(GB *gb, uint16_t sp0_) {
  CALL_C(0x7990, interactionDecCounter2_hook, 0x23d1, 0x7993);
  if (!(F & FZ)) {
    CYCT(0x7993, 0x7994); ret_effect(gb);
    return;
  }
  CYC(0x7993, 0x7994);
  CYC(0x7994, 0x7996); mem_wr(gb, HL, 0x3c);
  CYC(0x7996, 0x7998); L = 0x4d;
  CYC(0x7998, 0x7999); A = mem_rd(gb, HL);
  CYC(0x7999, 0x799b); L = 0x7d;
  CYC(0x799b, 0x799c); mem_wr(gb, HL, A);
  CYC(0x799c, 0x799f);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid1_substate3(GB *gb, uint16_t sp0_) {
  CYC(0x799f, 0x79a2); SET_HL(0x5d87);
  CYC(0x79a2, 0x79a4); E = 0x08;
  CALL_C(0x79a4, interBankCall_hook, 0x008a, 0x79a7);
  CALL_C(0x79a7, interactionDecCounter2_hook, 0x23d1, 0x79aa);
  if (!(F & FZ)) {
    CYCT(0x79aa, 0x79ab); ret_effect(gb);
    return;
  }
  CYC(0x79aa, 0x79ab);
  CYC(0x79ab, 0x79ad); mem_wr(gb, HL, 0x14);
  CYC(0x79ad, 0x79af); L = 0x5c;
  CYC(0x79af, 0x79b1); mem_wr(gb, HL, 0x06);
  CYC(0x79b1, 0x79b4);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid1_substate4(GB *gb, uint16_t sp0_) {
  CALL_C(0x79b4, interactionDecCounter2_hook, 0x23d1, 0x79b7);
  if (!(F & FZ)) {
    CYCT(0x79b7, 0x79b8); ret_effect(gb);
    return;
  }
  CYC(0x79b7, 0x79b8);
  CYC(0x79b8, 0x79bb); SET_BC(0x0000);
  CALL_C(0x79bb, objectSetSpeedZ_hook, 0x239d, 0x79be);
  CYC(0x79be, 0x79c1);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid1_substate5(GB *gb, uint16_t sp0_) {
  CYC(0x79c1, 0x79c3); C = 0x20;
  CALL_C(0x79c3, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x79c6);
  if (!(F & FZ)) {
    CYCT(0x79c6, 0x79c7); ret_effect(gb);
    return;
  }
  CYC(0x79c6, 0x79c7);
  CALL_C(0x79c7, interactionIncSubstate_hook, 0x23e5, 0x79ca);
  CYC(0x79ca, 0x79cc); L = 0x47;
  CYC(0x79cc, 0x79ce); mem_wr(gb, HL, 0xf0);
  CYC(0x79ce, 0x79d0); A = 0x04;
  CYC(0x79d0, 0x79d3);
  setScreenShakeCounter_hook(gb);
}

static void rabbit_subid1_substate6(GB *gb, uint16_t sp0_) {
  CALL_C(0x79d3, interactionDecCounter2_hook, 0x23d1, 0x79d6);
  if (!(F & FZ)) {
    CYCT(0x79d6, 0x79d7); ret_effect(gb);
    return;
  }
  CYC(0x79d6, 0x79d7);
  CYC(0x79d7, 0x79d9); A = 0xff;
  CYC(0x79d9, 0x79dc); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(0x79dc, 0x79dd); ret_effect(gb);
}

void rabbitSubid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x793e, 0x793f); H = D;
  CYC(0x793f, 0x7941); L = 0x46;
  CYC(0x7941, 0x7942); A = mem_rd(gb, HL);
  CYC(0x7942, 0x7943); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7943, 0x7945);
  } else {
    CYC(0x7943, 0x7945);
    CYC(0x7945, 0x7946); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (!(F & FZ)) {
      CYCT(0x7946, 0x7948);
    } else {
      CYC(0x7946, 0x7948);
      CYC(0x7948, 0x7949); L = alu_inc8(gb, L);
      CYC(0x7949, 0x794b); A = 0x1e;
      CYC(0x794b, 0x794c); mem_wr(gb, HL, A);
      CYC(0x794c, 0x794e); L = 0x45;
      CYC(0x794e, 0x7950); mem_wr(gb, HL, 0x02);
      CYC(0x7950, 0x7953); SET_BC(0xf000);
      CALL_C(0x7953, objectCreateExclamationMark_hook, 0x27e0, 0x7956);
    }
  }
  CYC(0x7956, 0x7958); E = 0x45;
  CYC(0x7958, 0x7959); A = mem_rd(gb, DE);
  CYC(0x7959, 0x795a); push_effect(gb, 0x795a);
  switch (rabbit_jump_table(gb)) {
    case 0x7968: rabbit_subid1_substate0(gb, sp0_); return;
    case 0x7978: rabbit_subid1_substate1(gb, sp0_); return;
    case 0x7990: rabbit_subid1_substate2(gb, sp0_); return;
    case 0x799f: rabbit_subid1_substate3(gb, sp0_); return;
    case 0x79b4: rabbit_subid1_substate4(gb, sp0_); return;
    case 0x79c1: rabbit_subid1_substate5(gb, sp0_); return;
    case 0x79d3: rabbit_subid1_substate6(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

static void rabbit_init_subid1(GB *gb, uint16_t sp0_) {
  CYC(0x784d, 0x784e); H = D;
  CYC(0x784e, 0x7850); L = 0x49;
  CYC(0x7850, 0x7852); mem_wr(gb, HL, 0x18);
  CYC(0x7852, 0x7854); L = 0x50;
  CYC(0x7854, 0x7856); mem_wr(gb, HL, 0x3c);
  rabbit_set_jump_animation(gb, sp0_);
}

static void rabbit_subid3_substate0(GB *gb, uint16_t sp0_) {
  CALL_C(0x7a13, interactionDecCounter1_hook, 0x23cc, 0x7a16);
  if (!(F & FZ)) {
    CYCT(0x7a16, 0x7a17); ret_effect(gb);
    return;
  }
  CYC(0x7a16, 0x7a17);
  CYC(0x7a17, 0x7a19); mem_wr(gb, HL, 0x5a);
  CYC(0x7a19, 0x7a1b); A = 0x01;
  CYC(0x7a1b, 0x7a1e); mem_wr(gb, wTmpcfc0_genericCutscene_cfd1, A);
  CYC(0x7a1e, 0x7a20); A = 0xc1;
  CALL_C(0x7a20, playSound_b00_hook, 0x0c98, 0x7a23);
  CYC(0x7a23, 0x7a26);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid3_substate1(GB *gb, uint16_t sp0_) {
  CALL_C(0x7a26, interactionDecCounter1_hook, 0x23cc, 0x7a29);
  if (!(F & FZ)) {
    CYC(0x7a29, 0x7a2b);
    CYC(0x7a2b, 0x7a2e); SET_HL(0x7b70);
    CYC(0x7a2e, 0x7a30); E = 0x08;
    CYC(0x7a30, 0x7a33);
    interBankCall_hook(gb);
    return;
  }
  CYCT(0x7a29, 0x7a2b);
  CALL_C(0x7a33, interactionIncSubstate_hook, 0x23e5, 0x7a36);
  CYC(0x7a36, 0x7a38); L = 0x5c;
  CYC(0x7a38, 0x7a3a); mem_wr(gb, HL, 0x02);
  CYC(0x7a3a, 0x7a3c); L = 0x78;
  CYC(0x7a3c, 0x7a3e); mem_wr(gb, HL, 0x20);
  CYC(0x7a3e, 0x7a41);
  rabbit_init_subid1(gb, sp0_);
}

void rabbitSubid3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a07, 0x7a09); E = 0x45;
  CYC(0x7a09, 0x7a0a); A = mem_rd(gb, DE);
  CYC(0x7a0a, 0x7a0b); push_effect(gb, 0x7a0b);
  switch (rabbit_jump_table(gb)) {
    case 0x7a13: rabbit_subid3_substate0(gb, sp0_); return;
    case 0x7a26: rabbit_subid3_substate1(gb, sp0_); return;
    case 0x7968: rabbit_subid1_substate0(gb, sp0_); return;
    case 0x7978: rabbit_subid1_substate1(gb, sp0_); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void rabbitJump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a76, 0x7a78); A = 0x07;
  CALL_C(0x7a78, interactionSetAnimation_hook, 0x262e, 0x7a7b);
  CYC(0x7a7b, 0x7a7e); SET_BC(0xff20);
  CYC(0x7a7e, 0x7a81);
  objectSetSpeedZ_hook(gb);
}

void rabbitSubid4Substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a81, 0x7a84); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd1);
  CYC(0x7a84, 0x7a86); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x7a86, 0x7a89);
    interactionRunScript_hook(gb);
    return;
  }
  CYC(0x7a86, 0x7a89);
  CALL_C(0x7a89, interactionIncSubstate_hook, 0x23e5, 0x7a8c);
  CYC(0x7a8c, 0x7a8e); L = 0x49;
  CYC(0x7a8e, 0x7a90); mem_wr(gb, HL, 0x18);
  CYC(0x7a90, 0x7a92); L = 0x50;
  CYC(0x7a92, 0x7a94); mem_wr(gb, HL, 0x19);
  CYC(0x7a94, 0x7a97); SET_BC(0xfe80);
  CALL_C(0x7a97, objectSetSpeedZ_hook, 0x239d, 0x7a9a);
  CYC(0x7a9a, 0x7a9c); A = 0x09;
  CYC(0x7a9c, 0x7a9f);
  interactionSetAnimation_hook(gb);
}

static void rabbit_subid5_substate3(GB *gb, uint16_t sp0_) {
  CALL_C(0x7af3, objectApplySpeed_hook, 0x201d, 0x7af6);
  CYC(0x7af6, 0x7af8); C = 0x20;
  CALL_C(0x7af8, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x7afb);
  if (!(F & FC)) {
    CYCT(0x7afb, 0x7afc); ret_effect(gb);
    return;
  }
  CYC(0x7afb, 0x7afc);
  CALL_C(0x7afc, interactionIncSubstate_hook, 0x23e5, 0x7aff);
  CYC(0x7aff, 0x7b01); L = 0x46;
  CYC(0x7b01, 0x7b03); mem_wr(gb, HL, 0x3c);
  CYC(0x7b03, 0x7b04); ret_effect(gb);
}

static void rabbit_subid4_substate0(GB *gb, uint16_t sp0_) {
  CYC(0x7a4f, 0x7a52); A = mem_rd(gb, wTmpcfc0_genericCutscene_cfd1);
  CYC(0x7a52, 0x7a54); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYC(0x7a54, 0x7a56);
    CYC(0x7a56, 0x7a57); H = D;
    CYC(0x7a57, 0x7a59); L = 0x45;
    CYC(0x7a59, 0x7a5b); mem_wr(gb, HL, 0x02);
    CYC(0x7a5b, 0x7a5e); SET_HL(0x635c);
    CYC(0x7a5e, 0x7a61);
    interactionSetScript_hook(gb);
    return;
  }
  CYCT(0x7a54, 0x7a56);
  CALL_C(0x7a61, interactionAnimate_hook, 0x261b, 0x7a64);
  CYC(0x7a64, 0x7a66); E = 0x61;
  CYC(0x7a66, 0x7a67); A = mem_rd(gb, DE);
  CYC(0x7a67, 0x7a68); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7a68, 0x7a69); ret_effect(gb);
    return;
  }
  CYC(0x7a68, 0x7a69);
  CYC(0x7a69, 0x7a6c);
  interactionIncSubstate_hook(gb);
}

static void rabbit_subid4_substate1(GB *gb, uint16_t sp0_) {
  CYC(0x7a6c, 0x7a6e); C = 0x20;
  CALL_C(0x7a6e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7a71);
  if (!(F & FZ)) {
    CYCT(0x7a71, 0x7a72); ret_effect(gb);
    return;
  }
  CYC(0x7a71, 0x7a72);
  CYC(0x7a72, 0x7a73); H = D;
  CYC(0x7a73, 0x7a75); L = 0x45;
  CYC(0x7a75, 0x7a76); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  rabbitJump_hook(gb);
}

static void rabbit_subid5_ret(GB *gb) {
  CYC(0x7b03, 0x7b04); ret_effect(gb);
}

void rabbitSubid4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a41, 0x7a43); E = 0x45;
  CYC(0x7a43, 0x7a44); A = mem_rd(gb, DE);
  CYC(0x7a44, 0x7a45); push_effect(gb, 0x7a45);
  switch (rabbit_jump_table(gb)) {
    case 0x7a4f: rabbit_subid4_substate0(gb, sp0_); return;
    case 0x7a6c: rabbit_subid4_substate1(gb, sp0_); return;
    case 0x7a81: rabbitSubid4Substate2_hook(gb); return;
    case 0x7af3: rabbit_subid5_substate3(gb, sp0_); return;
    case 0x7b03: rabbit_subid5_ret(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void rabbitSubid7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b0e, interactionRunScript_hook, 0x2552, 0x7b11);
  if (F & FC) {
    CYCT(0x7b11, 0x7b14);
    interactionDelete_hook(gb);
    return;
  }
  CYC(0x7b11, 0x7b14);
  CYC(0x7b14, 0x7b17);
  npcFaceLinkAndAnimate_hook(gb);
}

void rabbitSubid2SetRandomSpawnDelay_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7b17, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7b1a);
  CYC(0x7b1a, 0x7b1c); alu_and(gb, 0x03);
  CYC(0x7b1c, 0x7b1f); SET_BC(0x7b5c);
  CALL_C(0x7b1f, addAToBc_hook, 0x006d, 0x7b22);
  CYC(0x7b22, 0x7b23); A = mem_rd(gb, BC);
  CYC(0x7b23, 0x7b25); E = 0x78;
  CYC(0x7b25, 0x7b26); mem_wr(gb, DE, A);
  CYC(0x7b26, 0x7b27); ret_effect(gb);
}
