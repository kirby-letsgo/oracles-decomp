#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t static_object_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0004, false); alu_add(gb, L); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else { burn_rom(gb, 0x00, 0x0004, 0x0007, false); H = alu_inc8(gb, H); }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  L = A;
  burn_rom(gb, 0x00, 0x0009, 0x000b, false);
  return HL;
}

void parseStaticObjects_hook(GB *gb) {
  BASE(parseStaticObjects);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+O(0), b_+OE(3)); SET_DE(wStaticObjects);
next:
  CYC(b_+O(3), b_+OE(4)); C = E;
  CYC(b_+O(4), b_+OE(5)); A = mem_rd(gb, DE);
  CYC(b_+O(5), b_+OE(6)); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+O(6), b_+OE(8)); goto no_match; }
  CYC(b_+O(6), b_+OE(8));
  CYC(b_+O(8), b_+OE(9)); E = alu_inc8(gb, E);
  CYC(b_+O(9), b_+OE(10)); A = mem_rd(gb, DE);
  CYC(b_+O(10), b_+OE(11)); B = A;
  CYC(b_+O(11), b_+OE(14)); A = mem_rd(gb, wActiveRoom);
  CYC(b_+O(14), b_+OE(15)); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+O(15), b_+OE(17)); goto found; }
  CYC(b_+O(15), b_+OE(17));
no_match:
  CYC(b_+O(17), b_+OE(18)); A = C;
  CYC(b_+O(18), b_+OE(20)); alu_add(gb, 0x08);
  CYC(b_+O(20), b_+OE(21)); E = A;
  if (game_seasons) {
    CYC(b_+S(21), b_+S(22)); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+S(22), b_+S(24)); goto next; }
    CYC(b_+S(22), b_+S(24));
  } else {
    CYC(b_+21, b_+23); alu_cp(gb, 0xc0);
    if (F & FC) { CYCT(b_+23, b_+25); goto next; }
    CYC(b_+23, b_+25);
  }
  CYC(b_+O(25), b_+OE(26));
  ret_effect(gb);
  return;
found:
  CYC(b_+O(26), b_+OE(27)); E = alu_dec8(gb, E);
  CYC(b_+O(27), b_+OE(28)); A = mem_rd(gb, DE);
  CYC(b_+O(28), b_+OE(30)); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+O(30), b_+OE(32)); goto no_match; }
  CYC(b_+O(30), b_+OE(32));
  CYC(b_+O(32), b_+OE(34)); alu_and(gb, 0x7f);
  CYC(b_+O(34), b_+OE(35)); push_effect(gb, b_+OE(35));
  do { uint16_t jt_ = (static_object_jump_table(gb));
    if (jt_ == b_+O(47)) { goto end; }
    else if (jt_ == b_+O(53)) { goto interaction; }
    else if (jt_ == b_+O(60)) { goto enemy; }
    else if (jt_ == b_+O(67)) { goto part; }
    else { HANDOFF(HL); }
  } while (0);
end:
  CYC(b_+O(47), b_+OE(48)); A = E;
  CYC(b_+O(48), b_+OE(50)); alu_add(gb, 0x08);
  CYC(b_+O(50), b_+OE(51)); E = A;
  CYC(b_+O(51), b_+OE(53)); goto next;
interaction:
  CALL_C(b_+O(53), getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+OE(56));
  if (!(F & FZ)) { CYCT(b_+O(56), b_+OE(58)); goto end; }
  CYC(b_+O(56), b_+OE(58));
  CYC(b_+O(58), b_+OE(60)); goto copy;
enemy:
  CALL_C(b_+O(60), getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+OE(63));
  if (!(F & FZ)) { CYCT(b_+O(63), b_+OE(65)); goto end; }
  CYC(b_+O(63), b_+OE(65));
  CYC(b_+O(65), b_+OE(67)); goto copy;
part:
  CALL_C(b_+O(67), getFreePartSlot_hook, SYM(getFreePartSlot), b_+OE(70));
  if (!(F & FZ)) { CYCT(b_+O(70), b_+OE(72)); goto end; }
  CYC(b_+O(70), b_+OE(72));
copy:
  CYC(b_+O(72), b_+OE(73)); E = alu_inc8(gb, E);
  CYC(b_+O(73), b_+OE(74)); E = alu_inc8(gb, E);
  CYC(b_+O(74), b_+OE(75)); A = mem_rd(gb, DE);
  CYC(b_+O(75), b_+OE(77)); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+O(77), b_+OE(79)); goto copy_data; }
  CYC(b_+O(77), b_+OE(79));
  CYC(b_+O(79), b_+OE(80)); L = alu_dec8(gb, L);
  CYC(b_+O(80), b_+OE(82)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x02));
  CYC(b_+O(82), b_+OE(83)); L = alu_inc8(gb, L);
copy_data:
  CYC(b_+O(83), b_+OE(85)); alu_and(gb, 0x7f);
  CYC(b_+O(85), b_+OE(86)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(86), b_+OE(87)); E = alu_inc8(gb, E);
  CYC(b_+O(87), b_+OE(88)); A = mem_rd(gb, DE);
  CYC(b_+O(88), b_+OE(89)); mem_wr(gb, HL, A);
  CYC(b_+O(89), b_+OE(90)); A = L;
  CYC(b_+O(90), b_+OE(92)); alu_add(gb, 0x09);
  CYC(b_+O(92), b_+OE(93)); L = A;
  CYC(b_+O(93), b_+OE(94)); E = alu_inc8(gb, E);
  CYC(b_+O(94), b_+OE(95)); A = mem_rd(gb, DE);
  CYC(b_+O(95), b_+OE(96)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(96), b_+OE(97)); L = alu_inc8(gb, L);
  CYC(b_+O(97), b_+OE(98)); E = alu_inc8(gb, E);
  CYC(b_+O(98), b_+OE(99)); A = mem_rd(gb, DE);
  CYC(b_+O(99), b_+OE(100)); mem_wr(gb, HL, A);
  CYC(b_+O(100), b_+OE(101)); A = L;
  CYC(b_+O(101), b_+OE(103)); alu_add(gb, 0x09);
  CYC(b_+O(103), b_+OE(104)); L = A;
  CYC(b_+O(104), b_+OE(105)); A = E;
  CYC(b_+O(105), b_+OE(107)); alu_and(gb, 0xf8);
  CYC(b_+O(107), b_+OE(108)); E = A;
  CYC(b_+O(108), b_+OE(109)); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+O(109), b_+OE(110)); mem_wr(gb, HL, D);
  CYC(b_+O(110), b_+OE(112)); goto end;
}

static void add_double_index_to_hl(GB *gb) {
  BASE(loadStaticObjects_body);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  burn_rom(gb, 0, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0, 0x001f, 0x0020, false); ret_effect(gb);
}

void loadStaticObjects_body_hook(GB *gb) {
  BASE(loadStaticObjects_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearStaticObjects_hook, SYM(clearStaticObjects), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wDungeonIndex);
  CYC(b_+6, b_+9); SET_HL(SYM(staticDungeonObjects));
  add_double_index_to_hl(gb);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); H = mem_rd(gb, HL);
  CYC(b_+12, b_+13); L = A;
  CYC(b_+13, b_+16); SET_DE(wStaticObjects);
  for (;;) {
    CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+17, b_+19); alu_cp(gb, 0xff);
    if (F & FZ) { CYCT(b_+19, b_+20); ret_effect(gb); return; }
    CYC(b_+19, b_+20);
    CYC(b_+20, b_+21); mem_wr(gb, DE, A);
    CYC(b_+21, b_+23); B = 5;
    for (;;) {
      CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
      CYC(b_+24, b_+25); E = alu_inc8(gb, E);
      CYC(b_+25, b_+26); mem_wr(gb, DE, A);
      CYC(b_+26, b_+27); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+27, b_+29); continue; }
      CYC(b_+27, b_+29);
      break;
    }
    CYC(b_+29, b_+30); E = alu_inc8(gb, E);
    CYC(b_+30, b_+31); E = alu_inc8(gb, E);
    CYC(b_+31, b_+32); E = alu_inc8(gb, E);
    CYC(b_+32, b_+34);
  }
}
