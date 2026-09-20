#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_TOILET_HAND), bank 0x15.

void toiletHand_checkLinkIsClose_hook(GB *gb) {
  BASE(toiletHand_checkLinkIsClose);
  uint16_t sp0_ = gb->sp;
  // Get Link's position, quantized to a 4x4 grid and packed into one byte.
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+6); alu_add(gb, 0x04);
  CYC(b_+6, b_+8); alu_and(gb, 0xf0);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_sub(gb, 0x04);
  CYC(b_+13, b_+15); alu_and(gb, 0xf0);
  CYC(b_+15, b_+17); A = alu_swap(gb, A);
  CYC(b_+17, b_+18); alu_or(gb, B);
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+22); SET_HL(b_+33); // @data: list of positions close to the toilet

check_position:
  CYC(b_+22, b_+23); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+23, b_+24); alu_or(gb, A);
  CYC(b_+24, b_+25); alu_scf(gb);
  if (F & FZ) { CYCT(b_+25, b_+27); goto done; } // jr z
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto check_position; } // jr nz
  CYC(b_+28, b_+30);

done:
  CYC(b_+30, b_+33); writeFlagsTocddb_hook(gb); return; // jp
}

void toiletHand_retreatIntoToilet_hook(GB *gb);

void toiletHand_retreatIntoToiletIfNotAlready_hook(GB *gb) {
  BASE(toiletHand_retreatIntoToiletIfNotAlready);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  toiletHand_retreatIntoToilet_hook(gb); return; // fallthrough
}

void toiletHand_setAnimation_hook(GB *gb);

void toiletHand_retreatIntoToilet_hook(GB *gb) {
  BASE(toiletHand_retreatIntoToilet);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); toiletHand_setAnimation_hook(gb); return; // jr
}

void toiletHand_comeOutOfToilet_hook(GB *gb) {
  BASE(toiletHand_comeOutOfToilet);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4); toiletHand_setAnimation_hook(gb); return; // jr
}

void toiletHand_disappear_hook(GB *gb) {
  BASE(toiletHand_disappear);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); A = 0x00;
  toiletHand_setAnimation_hook(gb); return; // fallthrough
}

void toiletHand_setAnimation_hook(GB *gb) {
  BASE(toiletHand_setAnimation);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+6); interactionSetAnimation_hook(gb); return; // jp
}

void toiletHand_checkVisibility_hook(GB *gb) {
  BASE(toiletHand_checkVisibility);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); mem_wr(gb, wcddb, A);
  RET(b_+6); return;
}
