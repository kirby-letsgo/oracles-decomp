#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_TRAMPOLINE), bank 0x15.

static void trampoline_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Link bounces; the trampoline's tile (y in hFF8D, mirrored x in hFF8C) is looked up in the
// group's warp table for this room.
void s_trampoline_bounce_hook(GB *gb) {
  BASE(trampoline_bounce);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x09; // LINK_STATE_BOUNCING_ON_TRAMPOLINE
  CYC(b_+2, b_+5); mem_wr(gb, wLinkForceState, A);
  CYC(b_+5, b_+8); SET_HL(w1Link_yh);
  CALL_C(b_+8, objectCopyPosition_hook, SYM(objectCopyPosition), b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, w1Link_yh);
  CYC(b_+14, b_+16); A = alu_swap(gb, A);
  CYC(b_+16, b_+18); alu_and(gb, 0x0f);
  CYC(b_+18, b_+20); mem_wr(gb, hFF8D, A);
  CYC(b_+20, b_+23); A = mem_rd(gb, w1Link_xh);
  CYC(b_+23, b_+25); A = alu_swap(gb, A);
  CYC(b_+25, b_+27); alu_and(gb, 0x0f);
  CYC(b_+27, b_+29); alu_xor(gb, 0x0f);
  CYC(b_+29, b_+31); mem_wr(gb, hFF8C, A);
  CYC(b_+31, b_+34); A = mem_rd(gb, wActiveGroup);
  CYC(b_+34, b_+37); SET_HL(SYM(trampoline_group4Warps));
  CYC(b_+37, b_+39); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+39, b_+41); goto search; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+44); SET_HL(SYM(trampoline_group5Warps));
search:
  CYC(b_+44, b_+47); A = mem_rd(gb, wActiveRoom);
  CYC(b_+47, b_+48); E = A;
  for (;;) {
    CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+49, b_+50); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+50, b_+52); TAIL_S(trampoline_couldntFindRoom); }
    CYC(b_+50, b_+52);
    CYC(b_+52, b_+53); alu_cp(gb, E);
    if (F & FZ) { CYCT(b_+53, b_+55); TAIL_S(trampoline_foundRoom); }
    CYC(b_+53, b_+55);
    CYC(b_+55, b_+56); SET_HL(HL + 1);
    CYC(b_+56, b_+57); SET_HL(HL + 1);
    CYCT(b_+57, b_+59);
  }
}

// hl points at the room's table pointer; wcc50 = 1 when the tile's bit is set, else 0 for
// subid 0, else $82/$80 from the second table half.
void s_trampoline_foundRoom_hook(GB *gb) {
  BASE(trampoline_foundRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+1, b_+2); H = mem_rd(gb, HL);
  CYC(b_+2, b_+3); L = A;
  CYC(b_+3, b_+4); push_effect(gb, HL);
  CYC(b_+4, b_+6); A = mem_rd(gb, hFF8D);
  CYC(b_+6, b_+7); trampoline_add_double_index(gb, b_+7);
  CYC(b_+7, b_+9); A = mem_rd(gb, hFF8C);
  CALL_C(b_+9, checkFlag_hook, SYM(checkFlag), b_+12);
  CYC(b_+12, b_+14); C = 0x01;
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto pop; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); C = 0x00;
  CYC(b_+18, b_+20); E = 0x42; // Interaction.subid
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+22, b_+24); goto pop; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); SET_HL(pop_effect(gb));
  CYC(b_+25, b_+28); SET_BC(0x0016);
  CYC(b_+28, b_+29); alu_add_hl(gb, BC);
  CYC(b_+29, b_+31); A = mem_rd(gb, hFF8D);
  CYC(b_+31, b_+32); trampoline_add_double_index(gb, b_+32);
  CYC(b_+32, b_+34); A = mem_rd(gb, hFF8C);
  CALL_C(b_+34, checkFlag_hook, SYM(checkFlag), b_+37);
  CYC(b_+37, b_+39); C = 0x80;
  if (F & FZ) { CYCT(b_+39, b_+41); goto store; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); C = 0x82;
  CYCT(b_+43, b_+45); goto store;
pop:
  CYC(b_+45, b_+46); SET_HL(pop_effect(gb));
store:
  CYC(b_+46, b_+47); A = C;
  CYC(b_+47, b_+50); mem_wr(gb, wcc50, A);
  RET(b_+50); return;
}

void s_trampoline_couldntFindRoom_hook(GB *gb) {
  BASE(trampoline_couldntFindRoom);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x03;
  CYC(b_+2, b_+5); mem_wr(gb, wcc50, A);
  RET(b_+5); return;
}
