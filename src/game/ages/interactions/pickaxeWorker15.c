#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_PICKAXE_WORKER), bank 0x15.

static void pickaxeWorker15_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void pickaxeWorker_setRandomDelay_hook(GB *gb) {
  BASE(pickaxeWorker_setRandomDelay);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x1f);
  CYC(b_+5, b_+7); alu_sub(gb, 0x10);
  CYC(b_+7, b_+9); alu_add(gb, 0x3c);
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  RET(b_+12); return;
}

void pickaxeWorker_setAnimationFromVar03_hook(GB *gb) {
  BASE(pickaxeWorker_setAnimationFromVar03);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+11); // @animations
  CYC(b_+6, b_+7); pickaxeWorker15_addAToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+11); interactionSetAnimation_hook(gb); return; // jp
}

void pickaxeWorker_chooseRandomBlackTowerText_hook(GB *gb) {
  BASE(pickaxeWorker_chooseRandomBlackTowerText);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_hook, SYM(getRandomNumber), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  CYC(b_+5, b_+8); SET_HL(b_+18); // @blackTowerText
  CYC(b_+8, b_+9); pickaxeWorker15_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); A = 0x1b; // >TX_1b00
  CYC(b_+15, b_+16); E = alu_inc8(gb, E);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  RET(b_+17); return;
}
