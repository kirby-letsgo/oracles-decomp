#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void vasu_giveRingBox_hook(GB *gb);
void vasu_openRingMenu_hook(GB *gb);
void redSnake_openSecretInputMenu_hook(GB *gb);
void redSnake_generateRingSecret_hook(GB *gb);
void vasu_checkEarnedSpecialRing_hook(GB *gb);
void vasu_giveFriendshipRing_hook(GB *gb);
void vasu_giveHundredthRing_hook(GB *gb);
void vasu_giveRingInVar3a_hook(GB *gb);

void vasu_giveRingBox_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4256, getFreeInteractionSlot_hook, 0x3aef, 0x4259);
  CYC(0x4259, 0x425c); SET_BC(0x2c00);
  CYC(0x425c, 0x425e); mem_wr(gb, HL, 0x60);
  CYC(0x425e, 0x425f); L = alu_inc8(gb, L);
  CYC(0x425f, 0x4260); mem_wr(gb, HL, B);
  CYC(0x4260, 0x4261); L = alu_inc8(gb, L);
  CYC(0x4261, 0x4262); mem_wr(gb, HL, C);
  CYC(0x4262, 0x4264); L = 0x4b;
  CYC(0x4264, 0x4267); A = mem_rd(gb, 0xd00b);
  CYC(0x4267, 0x4268); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4268, 0x4269); L = alu_inc8(gb, L);
  CYC(0x4269, 0x426c); A = mem_rd(gb, 0xd00d);
  CYC(0x426c, 0x426d); mem_wr(gb, HL, A);
  RET(0x426d);
}

void vasu_openRingMenu_hook(GB *gb) {
  CYC(0x426e, 0x4271); mem_wr(gb, 0xcbd3, A);
  CYC(0x4271, 0x4273); A = 0x01;
  CYC(0x4273, 0x4276); mem_wr(gb, 0xcc8a, A);
  CYC(0x4276, 0x4278); A = 0x04;
  CYC(0x4278, 0x427b); openMenu_hook(gb);
}

void redSnake_openSecretInputMenu_hook(GB *gb) {
  CYC(0x427b, 0x427d); A = 0x02;
  CYC(0x427d, 0x4280); openSecretInputMenu_hook(gb);
}

void redSnake_generateRingSecret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4280, 0x4282); A = 0x28;
  CALL_C(0x4282, setGlobalFlag_hook, 0x31f9, 0x4285);
  CYC(0x4285, 0x4288); SET_BC(0x0002);
  CYC(0x4288, 0x428b); secretFunctionCaller_hook(gb);
}

static void vasu_checkFlagSet(GB *gb, uint16_t sp0_) {
  CYC(0x42d4, 0x42d5); C = A;
  CALL_C(0x42d5, checkGlobalFlag_hook, 0x31f3, 0x42d8);
  if (F & FZ) {
    CYCT(0x42d8, 0x42da);
    goto flagNotSet;
  }
  CYC(0x42d8, 0x42da);
  CYC(0x42da, 0x42db); A = C;
  CYC(0x42db, 0x42dd); alu_add(gb, 0x04);
  CYC(0x42dd, 0x42de); C = A;
  CALL_C(0x42de, checkGlobalFlag_hook, 0x31f3, 0x42e1);
  if (!(F & FZ)) {
    CYCT(0x42e1, 0x42e3);
    goto flagNotSet;
  }
  CYC(0x42e1, 0x42e3);
  CYC(0x42e3, 0x42e4); A = C;
  CALL_C(0x42e4, setGlobalFlag_hook, 0x31f9, 0x42e7);
  CYC(0x42e7, 0x42e8); A = C;
  CYC(0x42e8, 0x42ea); alu_add(gb, 0x30);
  RET(0x42ea);
  return;

flagNotSet:
  CYC(0x42eb, 0x42ec); alu_xor(gb, A);
  RET(0x42ec);
}

void vasu_checkEarnedSpecialRing_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x42b2, 0x42b4); A = 0x00;
  CYC(0x42b4, 0x42b7); push_effect(gb, 0x42b7); vasu_checkFlagSet(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(0x42b7, 0x42b9);
    goto setRingAndAction;
  }
  CYC(0x42b7, 0x42b9);
  CYC(0x42b9, 0x42bb); A = 0x01;
  CYC(0x42bb, 0x42be); push_effect(gb, 0x42be); vasu_checkFlagSet(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(0x42be, 0x42c0);
    goto setRingAndAction;
  }
  CYC(0x42be, 0x42c0);
  CYC(0x42c0, 0x42c2); A = 0x02;
  CYC(0x42c2, 0x42c5); push_effect(gb, 0x42c5); vasu_checkFlagSet(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(0x42c5, 0x42c7);
    goto setRingAndAction;
  }
  CYC(0x42c5, 0x42c7);
  CYC(0x42c7, 0x42c9); A = 0x03;

setAction:
  CYC(0x42c9, 0x42cb); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x42cb, 0x42cc); mem_wr(gb, DE, A);
  RET(0x42cc);
  return;

setRingAndAction:
  CYC(0x42cd, 0x42cf); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x42cf, 0x42d0); mem_wr(gb, DE, A);
  CYC(0x42d0, 0x42d2); alu_sub(gb, 0x34);
  CYC(0x42d2, 0x42d4);
  goto setAction;
}

static void vasu_giveRingFromA(GB *gb) {
  CYC(0x42f8, 0x42f9); B = A;
  CYC(0x42f9, 0x42fb); C = 0x00;
  CYC(0x42fb, 0x42fe); giveRingToLink_hook(gb);
}

void vasu_giveFriendshipRing_hook(GB *gb) {
  CYC(0x42ed, 0x42ef); A = 0x00;
  CYC(0x42ef, 0x42f1);
  vasu_giveRingFromA(gb);
}

void vasu_giveHundredthRing_hook(GB *gb) {
  CYC(0x42f1, 0x42f3); A = 0x38;
  CYC(0x42f3, 0x42f5);
  vasu_giveRingFromA(gb);
}

void vasu_giveRingInVar3a_hook(GB *gb) {
  CYC(0x42f5, 0x42f7); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x42f7, 0x42f8); A = mem_rd(gb, DE);
  vasu_giveRingFromA(gb);
}
