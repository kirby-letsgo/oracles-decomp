#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void wallArrowShooter_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void partCode25_hook(GB *gb) {
  BASE(partCode25);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto L_6045; } // jr nz
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+11); L = 0xc2; // Part.subid
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); A = alu_swap(gb, A);
  CYC(b_+14, b_+15); alu_rrca(gb);
  CYC(b_+15, b_+17); L = 0xc9; // Part.angle
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);

L_6045:
  CALL_C(b_+18, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+21);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; } // ret nz
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); E = 0xc2; // Part.subid
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+27); alu_bit(gb, 0, A);
  CYC(b_+27, b_+29); E = 0xcd; // Part.xh
  CYC(b_+29, b_+31); A = H8(hEnemyTargetX);
  if (F & FZ) { CYCT(b_+31, b_+33); goto L_6058; } // jr z
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); E = 0xcb; // Part.yh
  CYC(b_+35, b_+37); A = H8(hEnemyTargetY);

L_6058:
  CYC(b_+37, b_+38); B = A;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); alu_sub(gb, B);
  CYC(b_+40, b_+42); alu_add(gb, 0x10);
  CYC(b_+42, b_+44); alu_cp(gb, 0x21);
  if (!(F & FC)) { RET_TAKEN(b_+44); return; } // ret nc
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+47); E = 0xc6; // Part.counter1
  CYC(b_+47, b_+49); A = 0x21;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+53); SET_HL(SYM(table_6080)); // table_6080
  CYC(b_+53, b_+55); E = 0xc2; // Part.subid
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+57); wallArrowShooter_addDoubleIndexToHl_from_rst(gb, b_+57);
  CYC(b_+57, b_+58); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+58, b_+59); B = A;
  CYC(b_+59, b_+60); C = mem_rd(gb, HL);
  CALL_C(b_+60, getFreePartSlot_hook, SYM(getFreePartSlot), b_+63);
  if (!(F & FZ)) { RET_TAKEN(b_+63); return; } // ret nz
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x1a); // PART_ENEMY_ARROW
  CYC(b_+66, b_+67); L = alu_inc8(gb, L);
  CYC(b_+67, b_+68); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+68, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+71);
  CYC(b_+71, b_+73); L = 0xc9; // Part.angle
  CYC(b_+73, b_+74); E = L;
  CYC(b_+74, b_+75); A = mem_rd(gb, DE);
  CYC(b_+75, b_+76); mem_wr(gb, HL, A);
  RET(b_+76); return; // ret
}
