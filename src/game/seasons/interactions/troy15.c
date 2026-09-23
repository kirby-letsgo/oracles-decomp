#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_TROY), bank 0x15: the target
// carts minigame helpers.

// Clears the score bytes and var38, counter1 = 1, Link's items put away.
void s_seasonsFunc_15_6443_hook(GB *gb) {
  BASE(seasonsFunc_15_6443);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wcce1);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  CYC(b_+7, b_+9); E = 0x78; // Interaction.var38
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+12); E = 0x46; // Interaction.counter1
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+18);
  TAIL_SG(clearAllItemsAndPutLinkOnGround);
}

void s_seasonsFunc_15_6455_hook(GB *gb) {
  BASE(seasonsFunc_15_6455);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4); E = 0x7b; // Interaction.var3b
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+8);
  TAIL(objectSetInvisible);
}

void s_seasonsFunc_15_645d_hook(GB *gb) {
  BASE(seasonsFunc_15_645d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = 0x7b; // Interaction.var3b
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+7);
  TAIL(objectSetVisible);
}

// Clears every enemy, item and part; Link faces up at ($88,$78) on the ground.
void s_seasonsFunc_15_6464_hook(GB *gb) {
  BASE(seasonsFunc_15_6464);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CALL_C(b_+1, clearEnemies_hook, SYM(clearEnemies), b_+4);
  CALL_C(b_+4, clearItems_hook, SYM(clearItems), b_+7);
  CALL_C(b_+7, clearParts_hook, SYM(clearParts), b_+10);
  CYC(b_+10, b_+11); SET_DE(pop_effect(gb));
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+15); mem_wr(gb, wNumEnemies, A);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+20); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CALL_C(b_+20, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+23);
  CYC(b_+23, b_+26); SET_HL(w1Link_direction);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x00); // DIR_UP
  CYC(b_+28, b_+30); L = 0x0b; // w1Link.yh
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x88);
  CYC(b_+32, b_+34); L = 0x0d; // w1Link.xh
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x78);
  CYC(b_+36, b_+38); L = 0x0f; // w1Link.zh
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x00);
  RET(b_+40); return;
}

void s_createSwirlAtLink_hook(GB *gb) {
  BASE(createSwirlAtLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_yh);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+7); A = mem_rd(gb, w1Link_xh);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+10); A = 0x6e;
  CYC(b_+10, b_+13);
  TAIL(createEnergySwirlGoingIn);
}

void s_troyMinigame_createSparkle_hook(GB *gb) {
  BASE(troyMinigame_createSparkle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x8400); // INTERAC_SPARKLE
  CYC(b_+3, b_+6);
  TAIL(objectCreateInteraction);
}
