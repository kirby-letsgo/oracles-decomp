#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void faroreCheckSecretValidity_hook(GB *gb);
void faroreShowTextForSecretHint_hook(GB *gb);
void faroreSpawnSecretChest_hook(GB *gb);
void faroreGenerateGameTransferSecret_hook(GB *gb);

static uint16_t farore_jumpTable(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void farore_setVar3f(GB *gb, uint16_t sp0_) {
  BASE(faroreCheckSecretValidity);
  CYC(b_+7, b_+9); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  RET(b_+10);
}

void faroreCheckSecretValidity_hook(GB *gb) {
  BASE(faroreCheckSecretValidity);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wSecretInputType);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
  } else {
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+7); alu_xor(gb, A);
    farore_setVar3f(gb, sp0_);
    return;
  }

  CYC(b_+11, b_+14); A = mem_rd(gb, wTextInputResult);
  CYC(b_+14, b_+16); alu_swap_a(gb);
  CYC(b_+16, b_+18); alu_and(gb, 0x03);
  CYC(b_+18, b_+19); push_effect(gb, b_+19);
  do { uint16_t jt_ = (farore_jumpTable(gb));
    if (jt_ == b_+31) { goto jump0; }
    else if (jt_ == b_+27) { goto jump1Or2; }
    else if (jt_ == b_+35) { goto jump3; }
    else { HANDOFF(HL); }
  } while (0);

jump1Or2:
  CYC(b_+27, b_+29); A = 0x04;
  CYC(b_+29, b_+31);
  farore_setVar3f(gb, sp0_);
  return;

jump0:
  CYC(b_+31, b_+33); A = 0x03;
  CYC(b_+33, b_+35);
  farore_setVar3f(gb, sp0_);
  return;

jump3:
  CYC(b_+35, b_+38); A = mem_rd(gb, wTextInputResult);
  CYC(b_+38, b_+40); alu_and(gb, 0x0f);
  CYC(b_+40, b_+42); alu_add(gb, GV(0x5a, 0x6e));
  CYC(b_+42, b_+43); B = A;
  CALL_C(b_+43, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+46);
  CYC(b_+46, b_+48); A = 0x02;
  if (!(F & FZ)) {
    CYCT(b_+48, b_+50);
    farore_setVar3f(gb, sp0_);
    return;
  }
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+51); A = B;
  CYC(b_+51, b_+53); alu_sub(gb, 0x0a);
  CALL_C(b_+53, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+56);
  CYC(b_+56, b_+58); A = 0x01;
  if (!(F & FZ)) {
    CYCT(b_+58, b_+60);
    farore_setVar3f(gb, sp0_);
    return;
  }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+62); A = 0x05;
  CYC(b_+62, b_+64);
  farore_setVar3f(gb, sp0_);
}

void faroreShowTextForSecretHint_hook(GB *gb) {
  BASE(faroreShowTextForSecretHint);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTextInputResult);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); alu_add(gb, 0x0f);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+10); B = 0x55;
  CYC(b_+10, b_+13); TAIL(showText);
}

void faroreSpawnSecretChest_hook(GB *gb) {
  BASE(faroreSpawnSecretChest);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) {
    RET_TAKEN(b_+3); return;
  }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xd9);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+10); A = mem_rd(gb, wTextInputResult);
  CYC(b_+10, b_+12); alu_and(gb, 0x0f);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+15); L = 0x4b;
  CYC(b_+15, b_+17); C = 0x75;
  CYC(b_+17, b_+20); TAIL(setShortPosition_paramC);
}

void faroreGenerateGameTransferSecret_hook(GB *gb) {
  BASE(faroreGenerateGameTransferSecret);
  CYC(b_+0, b_+3); SET_HL(0x481b);
  CYC(b_+3, b_+5); E = 0x03;
  CYC(b_+5, b_+8); interBankCall_hook(gb);
}
