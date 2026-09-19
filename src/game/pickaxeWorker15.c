#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(0x5f15, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5f18);
  CYC(0x5f18, 0x5f1a); alu_and(gb, 0x1f);
  CYC(0x5f1a, 0x5f1c); alu_sub(gb, 0x10);
  CYC(0x5f1c, 0x5f1e); alu_add(gb, 0x3c);
  CYC(0x5f1e, 0x5f20); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5f20, 0x5f21); mem_wr(gb, DE, A);
  RET(0x5f21); return;
}

void pickaxeWorker_setAnimationFromVar03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5f22, 0x5f24); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5f24, 0x5f25); A = mem_rd(gb, DE);
  CYC(0x5f25, 0x5f28); SET_HL(0x5f2d); // @animations
  CYC(0x5f28, 0x5f29); pickaxeWorker15_addAToHl_from_rst(gb, 0x5f29);
  CYC(0x5f29, 0x5f2a); A = mem_rd(gb, HL);
  CYC(0x5f2a, 0x5f2d); interactionSetAnimation_hook(gb); return; // jp
}

void pickaxeWorker_chooseRandomBlackTowerText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5f35, getRandomNumber_hook, 0x043e, 0x5f38);
  CYC(0x5f38, 0x5f3a); alu_and(gb, 0x07);
  CYC(0x5f3a, 0x5f3d); SET_HL(0x5f47); // @blackTowerText
  CYC(0x5f3d, 0x5f3e); pickaxeWorker15_addAToHl_from_rst(gb, 0x5f3e);
  CYC(0x5f3e, 0x5f3f); A = mem_rd(gb, HL);
  CYC(0x5f3f, 0x5f41); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x5f41, 0x5f42); mem_wr(gb, DE, A);
  CYC(0x5f42, 0x5f44); A = 0x1b; // >TX_1b00
  CYC(0x5f44, 0x5f45); E = alu_inc8(gb, E);
  CYC(0x5f45, 0x5f46); mem_wr(gb, DE, A);
  RET(0x5f46); return;
}
