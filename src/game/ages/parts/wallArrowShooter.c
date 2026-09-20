#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6033, 0x6035); E = 0xc4; // Part.state
  CYC(0x6035, 0x6036); A = mem_rd(gb, DE);
  CYC(0x6036, 0x6037); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6037, 0x6039); goto L_6045; } // jr nz
  CYC(0x6037, 0x6039);
  CYC(0x6039, 0x603a); H = D;
  CYC(0x603a, 0x603b); L = E;
  CYC(0x603b, 0x603c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x603c, 0x603e); L = 0xc2; // Part.subid
  CYC(0x603e, 0x603f); A = mem_rd(gb, HL);
  CYC(0x603f, 0x6041); A = alu_swap(gb, A);
  CYC(0x6041, 0x6042); alu_rrca(gb);
  CYC(0x6042, 0x6044); L = 0xc9; // Part.angle
  CYC(0x6044, 0x6045); mem_wr(gb, HL, A);

L_6045:
  CALL_C(0x6045, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x6048);
  if (!(F & FZ)) { RET_TAKEN(0x6048); return; } // ret nz
  CYC(0x6048, 0x6049);
  CYC(0x6049, 0x604b); E = 0xc2; // Part.subid
  CYC(0x604b, 0x604c); A = mem_rd(gb, DE);
  CYC(0x604c, 0x604e); alu_bit(gb, 0, A);
  CYC(0x604e, 0x6050); E = 0xcd; // Part.xh
  CYC(0x6050, 0x6052); A = H8(hEnemyTargetX);
  if (F & FZ) { CYCT(0x6052, 0x6054); goto L_6058; } // jr z
  CYC(0x6052, 0x6054);
  CYC(0x6054, 0x6056); E = 0xcb; // Part.yh
  CYC(0x6056, 0x6058); A = H8(hEnemyTargetY);

L_6058:
  CYC(0x6058, 0x6059); B = A;
  CYC(0x6059, 0x605a); A = mem_rd(gb, DE);
  CYC(0x605a, 0x605b); alu_sub(gb, B);
  CYC(0x605b, 0x605d); alu_add(gb, 0x10);
  CYC(0x605d, 0x605f); alu_cp(gb, 0x21);
  if (!(F & FC)) { RET_TAKEN(0x605f); return; } // ret nc
  CYC(0x605f, 0x6060);
  CYC(0x6060, 0x6062); E = 0xc6; // Part.counter1
  CYC(0x6062, 0x6064); A = 0x21;
  CYC(0x6064, 0x6065); mem_wr(gb, DE, A);
  CYC(0x6065, 0x6068); SET_HL(0x6080); // table_6080
  CYC(0x6068, 0x606a); E = 0xc2; // Part.subid
  CYC(0x606a, 0x606b); A = mem_rd(gb, DE);
  CYC(0x606b, 0x606c); wallArrowShooter_addDoubleIndexToHl_from_rst(gb, 0x606c);
  CYC(0x606c, 0x606d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x606d, 0x606e); B = A;
  CYC(0x606e, 0x606f); C = mem_rd(gb, HL);
  CALL_C(0x606f, getFreePartSlot_hook, 0x3e8e, 0x6072);
  if (!(F & FZ)) { RET_TAKEN(0x6072); return; } // ret nz
  CYC(0x6072, 0x6073);
  CYC(0x6073, 0x6075); mem_wr(gb, HL, 0x1a); // PART_ENEMY_ARROW
  CYC(0x6075, 0x6076); L = alu_inc8(gb, L);
  CYC(0x6076, 0x6077); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x6077, objectCopyPositionWithOffset_hook, 0x225a, 0x607a);
  CYC(0x607a, 0x607c); L = 0xc9; // Part.angle
  CYC(0x607c, 0x607d); E = L;
  CYC(0x607d, 0x607e); A = mem_rd(gb, DE);
  CYC(0x607e, 0x607f); mem_wr(gb, HL, A);
  RET(0x607f); return; // ret
}
