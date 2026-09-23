#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_GOLDEN_CAVE_SUBROSIAN), bank 0x15.

void s_goldenCaveSubrosian_emptyLinksItemsAndSetPosition_hook(GB *gb) {
  BASE(goldenCaveSubrosian_emptyLinksItemsAndSetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearAllParentItems_hook, SYM(clearAllParentItems), b_+3);
  CALL_C(b_+3, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+6);
  CALL_C(b_+6, clearItems_hook, SYM(clearItems), b_+9);
  CALL_C(b_+9, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+12);
  CYC(b_+12, b_+15); SET_HL(w1Link_direction);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x00); // DIR_UP
  RET(b_+17); return;
}

void s_goldenCaveSubrosian_faceLinkUp_hook(GB *gb) {
  BASE(goldenCaveSubrosian_faceLinkUp);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_direction);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x00); // DIR_UP
  RET(b_+5); return;
}

// Once: var39 = 1 when var38 is below 3.
void s_seasonsFunc_15_64e9_hook(GB *gb) {
  BASE(seasonsFunc_15_64e9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x79; // Interaction.var39
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = 0x78; // Interaction.var38
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); B = 0x00;
  CYC(b_+11, b_+13); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+13, b_+15); goto store; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); B = 0x01;
store:
  CYC(b_+17, b_+19); L = 0x79; // Interaction.var39
  CYC(b_+19, b_+20); mem_wr(gb, HL, B);
  RET(b_+20); return;
}

// Warps back into the same room (group 4 room $f0, transition $0f) at position a.
void s_goldenCaveSubrosian_refreshRoom_hook(GB *gb) {
  BASE(goldenCaveSubrosian_refreshRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+4); SET_HL(wWarpDestGroup);
  CYC(b_+4, b_+6); A = 0x84;
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+9); A = 0xf0;
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+10, b_+12); A = 0x0f;
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); A = B;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+17); A = 0x00;
  CYC(b_+17, b_+20); mem_wr(gb, wWarpTransition, A);
  CYC(b_+20, b_+22); A = 0x03;
  CYC(b_+22, b_+25); mem_wr(gb, wWarpTransition2, A);
  RET(b_+25); return;
}

// var33 = $4c; var32 = $25 for room flags mode 1, else $27.
void s_seasonsFunc_15_6518_hook(GB *gb) {
  BASE(seasonsFunc_15_6518);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x73; // Interaction.var33
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x4c);
  CYC(b_+5, b_+7); B = 0x25;
  CALL_C(b_+7, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+10);
  CYC(b_+10, b_+12); alu_and(gb, 0x03);
  CYC(b_+12, b_+13); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+13, b_+15); goto store; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); B = 0x27;
store:
  CYC(b_+17, b_+18); A = B;
  CYC(b_+18, b_+20); E = 0x72; // Interaction.var32
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return;
}

// Re-enables objects and the menu; keeps only bits 6 and 7 of the room flags.
void s_seasonsFunc_15_652e_hook(GB *gb) {
  BASE(seasonsFunc_15_652e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+4, b_+7); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+7, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+10);
  CYC(b_+10, b_+12); alu_and(gb, 0xc0);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  RET(b_+13); return;
}

// The room flags' low bits = a.
void s_seasonsFunc_15_653c_hook(GB *gb) {
  BASE(seasonsFunc_15_653c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CALL_C(b_+1, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+4);
  CYC(b_+4, b_+6); alu_and(gb, 0xc0);
  CYC(b_+6, b_+7); alu_or(gb, B);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  RET(b_+8); return;
}

// var3c = the room flags' low two bits.
void s_seasonsFunc_15_6545_hook(GB *gb) {
  BASE(seasonsFunc_15_6545);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+7); E = 0x7c; // Interaction.var3c
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  RET(b_+8); return;
}
