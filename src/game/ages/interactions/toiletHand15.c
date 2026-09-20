#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_TOILET_HAND), bank 0x15.

void toiletHand_checkLinkIsClose_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // Get Link's position, quantized to a 4x4 grid and packed into one byte.
  CYC(0x61b9, 0x61bc); SET_HL(w1Link_yh);
  CYC(0x61bc, 0x61bd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x61bd, 0x61bf); alu_add(gb, 0x04);
  CYC(0x61bf, 0x61c1); alu_and(gb, 0xf0);
  CYC(0x61c1, 0x61c2); B = A;
  CYC(0x61c2, 0x61c3); L = alu_inc8(gb, L);
  CYC(0x61c3, 0x61c4); A = mem_rd(gb, HL);
  CYC(0x61c4, 0x61c6); alu_sub(gb, 0x04);
  CYC(0x61c6, 0x61c8); alu_and(gb, 0xf0);
  CYC(0x61c8, 0x61ca); A = alu_swap(gb, A);
  CYC(0x61ca, 0x61cb); alu_or(gb, B);
  CYC(0x61cb, 0x61cc); B = A;
  CYC(0x61cc, 0x61cf); SET_HL(0x61da); // @data: list of positions close to the toilet

check_position:
  CYC(0x61cf, 0x61d0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x61d0, 0x61d1); alu_or(gb, A);
  CYC(0x61d1, 0x61d2); alu_scf(gb);
  if (F & FZ) { CYCT(0x61d2, 0x61d4); goto done; } // jr z
  CYC(0x61d2, 0x61d4);
  CYC(0x61d4, 0x61d5); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x61d5, 0x61d7); goto check_position; } // jr nz
  CYC(0x61d5, 0x61d7);

done:
  CYC(0x61d7, 0x61da); writeFlagsTocddb_hook(gb); return; // jp
}

void toiletHand_retreatIntoToilet_hook(GB *gb);

void toiletHand_retreatIntoToiletIfNotAlready_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x61de, 0x61e0); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x61e0, 0x61e1); A = mem_rd(gb, DE);
  CYC(0x61e1, 0x61e3); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(0x61e3); return; }
  CYC(0x61e3, 0x61e4);
  toiletHand_retreatIntoToilet_hook(gb); return; // fallthrough
}

void toiletHand_setAnimation_hook(GB *gb);

void toiletHand_retreatIntoToilet_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x61e4, 0x61e6); A = 0x02;
  CYC(0x61e6, 0x61e8); toiletHand_setAnimation_hook(gb); return; // jr
}

void toiletHand_comeOutOfToilet_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x61e8, 0x61ea); A = 0x01;
  CYC(0x61ea, 0x61ec); toiletHand_setAnimation_hook(gb); return; // jr
}

void toiletHand_disappear_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x61ec, 0x61ee); A = 0x00;
  toiletHand_setAnimation_hook(gb); return; // fallthrough
}

void toiletHand_setAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x61ee, 0x61f0); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x61f0, 0x61f1); mem_wr(gb, DE, A);
  CYC(0x61f1, 0x61f4); interactionSetAnimation_hook(gb); return; // jp
}

void toiletHand_checkVisibility_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x61f4, 0x61f6); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x61f6, 0x61f7); A = mem_rd(gb, DE);
  CYC(0x61f7, 0x61fa); mem_wr(gb, wcddb, A);
  RET(0x61fa); return;
}
