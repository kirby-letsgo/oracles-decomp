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
  CYC(b_+0, b_+3); SET_DE(wStaticObjects);
next:
  CYC(b_+3, b_+4); C = E;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); goto no_match; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); E = alu_inc8(gb, E);
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+14); A = mem_rd(gb, wActiveRoom);
  CYC(b_+14, b_+15); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+15, b_+17); goto found; }
  CYC(b_+15, b_+17);
no_match:
  CYC(b_+17, b_+18); A = C;
  CYC(b_+18, b_+20); alu_add(gb, 0x08);
  CYC(b_+20, b_+21); E = A;
  CYC(b_+21, b_+23); alu_cp(gb, 0xc0);
  if (F & FC) { CYCT(b_+23, b_+25); goto next; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26);
  ret_effect(gb);
  return;
found:
  CYC(b_+26, b_+27); E = alu_dec8(gb, E);
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+30); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto no_match; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); alu_and(gb, 0x7f);
  CYC(b_+34, b_+35); push_effect(gb, b_+35);
  do { uint16_t jt_ = (static_object_jump_table(gb));
    if (jt_ == b_+47) { goto end; }
    else if (jt_ == b_+53) { goto interaction; }
    else if (jt_ == b_+60) { goto enemy; }
    else if (jt_ == b_+67) { goto part; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
end:
  CYC(b_+47, b_+48); A = E;
  CYC(b_+48, b_+50); alu_add(gb, 0x08);
  CYC(b_+50, b_+51); E = A;
  CYC(b_+51, b_+53); goto next;
interaction:
  CALL_C(b_+53, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+56);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto end; }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+60); goto copy;
enemy:
  CALL_C(b_+60, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+63);
  if (!(F & FZ)) { CYCT(b_+63, b_+65); goto end; }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); goto copy;
part:
  CALL_C(b_+67, getFreePartSlot_hook, SYM(getFreePartSlot), b_+70);
  if (!(F & FZ)) { CYCT(b_+70, b_+72); goto end; }
  CYC(b_+70, b_+72);
copy:
  CYC(b_+72, b_+73); E = alu_inc8(gb, E);
  CYC(b_+73, b_+74); E = alu_inc8(gb, E);
  CYC(b_+74, b_+75); A = mem_rd(gb, DE);
  CYC(b_+75, b_+77); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+77, b_+79); goto copy_data; }
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+80); L = alu_dec8(gb, L);
  CYC(b_+80, b_+82); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x02));
  CYC(b_+82, b_+83); L = alu_inc8(gb, L);
copy_data:
  CYC(b_+83, b_+85); alu_and(gb, 0x7f);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+86, b_+87); E = alu_inc8(gb, E);
  CYC(b_+87, b_+88); A = mem_rd(gb, DE);
  CYC(b_+88, b_+89); mem_wr(gb, HL, A);
  CYC(b_+89, b_+90); A = L;
  CYC(b_+90, b_+92); alu_add(gb, 0x09);
  CYC(b_+92, b_+93); L = A;
  CYC(b_+93, b_+94); E = alu_inc8(gb, E);
  CYC(b_+94, b_+95); A = mem_rd(gb, DE);
  CYC(b_+95, b_+96); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+96, b_+97); L = alu_inc8(gb, L);
  CYC(b_+97, b_+98); E = alu_inc8(gb, E);
  CYC(b_+98, b_+99); A = mem_rd(gb, DE);
  CYC(b_+99, b_+100); mem_wr(gb, HL, A);
  CYC(b_+100, b_+101); A = L;
  CYC(b_+101, b_+103); alu_add(gb, 0x09);
  CYC(b_+103, b_+104); L = A;
  CYC(b_+104, b_+105); A = E;
  CYC(b_+105, b_+107); alu_and(gb, 0xf8);
  CYC(b_+107, b_+108); E = A;
  CYC(b_+108, b_+109); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+109, b_+110); mem_wr(gb, HL, D);
  CYC(b_+110, b_+112); goto end;
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
    CYC(b_+32, (SYM(staticDungeonObjects) + 1));
  }
}
