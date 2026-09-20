#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void vasu_giveRingBox_hook(GB *gb);
void vasu_openRingMenu_hook(GB *gb);
void redSnake_openSecretInputMenu_hook(GB *gb);
void redSnake_generateRingSecret_hook(GB *gb);
void vasu_checkEarnedSpecialRing_hook(GB *gb);
void vasu_giveFriendshipRing_hook(GB *gb);
void vasu_giveHundredthRing_hook(GB *gb);
void vasu_giveRingInVar3a_hook(GB *gb);

void vasu_giveRingBox_hook(GB *gb) {
  BASE(vasu_giveRingBox);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  CYC(b_+3, b_+6); SET_BC(0x2c00);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x60);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); mem_wr(gb, HL, B);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+12); mem_wr(gb, HL, C);
  CYC(b_+12, b_+14); L = 0x4b;
  CYC(b_+14, b_+17); A = mem_rd(gb, w1Link_yh);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+22); A = mem_rd(gb, w1Link_xh);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  RET(b_+23);
}

void vasu_openRingMenu_hook(GB *gb) {
  BASE(vasu_openRingMenu);
  CYC(b_+0, b_+3); mem_wr(gb, wRingMenu_mode, A);
  CYC(b_+3, b_+5); A = 0x01;
  CYC(b_+5, b_+8); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+8, b_+10); A = 0x04;
  CYC(b_+10, b_+13); openMenu_hook(gb);
}

void redSnake_openSecretInputMenu_hook(GB *gb) {
  BASE(redSnake_openSecretInputMenu);
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+5); openSecretInputMenu_hook(gb);
}

void redSnake_generateRingSecret_hook(GB *gb) {
  BASE(redSnake_generateRingSecret);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = GV(0x28, 0x31);
  CALL_C(b_+2, setGlobalFlag_hook, SYM(setGlobalFlag), b_+5);
  CYC(b_+5, b_+8); SET_BC(0x0002);
  CYC(b_+8, b_+11); secretFunctionCaller_hook(gb);
}

static void vasu_checkFlagSet(GB *gb, uint16_t sp0_) {
  BASE(vasu_checkEarnedSpecialRing);
  CYC(b_+34, b_+35); C = A;
  CALL_C(b_+35, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+38);
  if (F & FZ) {
    CYCT(b_+38, b_+40);
    goto flagNotSet;
  }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); A = C;
  CYC(b_+41, b_+43); alu_add(gb, 0x04);
  CYC(b_+43, b_+44); C = A;
  CALL_C(b_+44, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+47);
  if (!(F & FZ)) {
    CYCT(b_+47, b_+49);
    goto flagNotSet;
  }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); A = C;
  CALL_C(b_+50, setGlobalFlag_hook, SYM(setGlobalFlag), b_+53);
  CYC(b_+53, b_+54); A = C;
  CYC(b_+54, b_+56); alu_add(gb, 0x30);
  RET(b_+56);
  return;

flagNotSet:
  CYC(b_+57, b_+58); alu_xor(gb, A);
  RET(b_+58);
}

void vasu_checkEarnedSpecialRing_hook(GB *gb) {
  BASE(vasu_checkEarnedSpecialRing);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+5); push_effect(gb, b_+5); vasu_checkFlagSet(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+7);
    goto setRingAndAction;
  }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x01;
  CYC(b_+9, b_+12); push_effect(gb, b_+12); vasu_checkFlagSet(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+14);
    goto setRingAndAction;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); A = 0x02;
  CYC(b_+16, b_+19); push_effect(gb, b_+19); vasu_checkFlagSet(gb, sp0_);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
    goto setRingAndAction;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); A = 0x03;

setAction:
  CYC(b_+23, b_+25); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  RET(b_+26);
  return;

setRingAndAction:
  CYC(b_+27, b_+29); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+32); alu_sub(gb, 0x34);
  CYC(b_+32, b_+34);
  goto setAction;
}

static void vasu_giveRingFromA(GB *gb) {
  BASE(vasu_giveRingInVar3a);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); C = 0x00;
  CYC(b_+6, b_+9); giveRingToLink_hook(gb);
}

void vasu_giveFriendshipRing_hook(GB *gb) {
  BASE(vasu_giveFriendshipRing);
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+4);
  vasu_giveRingFromA(gb);
}

void vasu_giveHundredthRing_hook(GB *gb) {
  BASE(vasu_giveHundredthRing);
  CYC(b_+0, b_+2); A = 0x38;
  CYC(b_+2, b_+4);
  vasu_giveRingFromA(gb);
}

void vasu_giveRingInVar3a_hook(GB *gb) {
  BASE(vasu_giveRingInVar3a);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  vasu_giveRingFromA(gb);
}
