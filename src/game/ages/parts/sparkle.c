#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void sparkle_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

void partCode26_hook(GB *gb) {
  BASE(partCode26);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto state0; } // jr z
  CYC(b_+4, b_+6);
  CALL_C(b_+6, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto counter1NonZero; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); L = 0xf0; // Part.var30
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); alu_cpl(gb);
  CYC(b_+18, b_+20); alu_add(gb, 0x01);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); alu_cpl(gb);
  CYC(b_+23, b_+25); alu_adc(gb, 0x00);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);

counter1NonZero:
  CYC(b_+26, b_+28); E = 0xcd; // Part.xh
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); B = A;
  CYC(b_+30, b_+31); E = alu_dec8(gb, E);
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); C = A;
  CYC(b_+33, b_+35); L = 0xd2;
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+37); H = mem_rd(gb, HL);
  CYC(b_+37, b_+38); L = A;
  CYC(b_+38, b_+39); alu_add_hl(gb, BC);
  CYC(b_+39, b_+40); A = L;
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CYC(b_+41, b_+42); E = alu_inc8(gb, E);
  CYC(b_+42, b_+43); A = H;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+46); E = 0xf0; // Part.var30
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); C = A;
  CYC(b_+48, b_+49); E = alu_inc8(gb, E);
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); B = A;
  CYC(b_+51, b_+53); E = 0xd3;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); H = A;
  CYC(b_+55, b_+56); E = alu_dec8(gb, E);
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+58); L = A;
  CYC(b_+58, b_+59); alu_add_hl(gb, BC);
  CYC(b_+59, b_+60); A = L;
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+62); E = alu_inc8(gb, E);
  CYC(b_+62, b_+63); A = H;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CYC(b_+64, b_+65); H = D;
  CYC(b_+65, b_+67); L = 0xce;
  CYC(b_+67, b_+69); E = 0xd4; // Part.speedZ
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+71); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+71, b_+72); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+72, b_+73); A = mem_rd(gb, HL);
  CYC(b_+73, b_+75); alu_adc(gb, 0x00);
  if (F & FZ) { CYCT(b_+75, b_+78); partDelete_hook(gb); return; } // jp z
  CYC(b_+75, b_+78);
  CYC(b_+78, b_+79); mem_wr(gb, HL, A);
  CYC(b_+79, b_+81); alu_cp(gb, 0xe8);
  if (F & FC) { CYCT(b_+81, b_+83); goto animate; } // jr c
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+85); L = 0xda; // Part.visible
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+88); alu_xor(gb, 0x80);
  CYC(b_+88, b_+89); mem_wr(gb, HL, A);

animate:
  CYC(b_+89, b_+92); TAIL(partAnimate); // jp

state0:
  CYC(b_+92, b_+93); H = D;
  CYC(b_+93, b_+94); L = E;
  CYC(b_+94, b_+95); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+95, objectGetZAboveScreen_hook, SYM(objectGetZAboveScreen), b_+98);
  CYC(b_+98, b_+100); L = 0xcf; // Part.zh
  CYC(b_+100, b_+101); mem_wr(gb, HL, A);
  CYC(b_+101, b_+103); E = 0xc3; // Part.var03
  CYC(b_+103, b_+104); A = mem_rd(gb, DE);
  CYC(b_+104, b_+105); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+105, b_+107); goto var03_00; } // jr z
  CYC(b_+105, b_+107);
  CYC(b_+107, b_+109); mem_wr(gb, HL, 0xf0);

var03_00:
  CALL_C(b_+109, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+112);
  CYC(b_+112, b_+114); alu_and(gb, 0x0c);
  CYC(b_+114, b_+117); SET_HL(SYM(table_6114)); // table_6114
  CYC(b_+117, b_+118); sparkle_addAToHl_from_rst(gb, b_+118);
  CYC(b_+118, b_+120); E = 0xf0; // Part.var30
  CYC(b_+120, b_+121); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+121, b_+122); mem_wr(gb, DE, A);
  CYC(b_+122, b_+123); E = alu_inc8(gb, E);
  CYC(b_+123, b_+124); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+124, b_+125); mem_wr(gb, DE, A);
  CYC(b_+125, b_+127); E = 0xd4; // Part.speedZ
  CYC(b_+127, b_+128); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+128, b_+129); mem_wr(gb, DE, A);
  CYC(b_+129, b_+131); E = 0xc6; // Part.counter1
  CYC(b_+131, b_+132); A = mem_rd(gb, HL);
  CYC(b_+132, b_+133); mem_wr(gb, DE, A);
  CYC(b_+133, b_+134); E = alu_inc8(gb, E);
  CYC(b_+134, b_+135); A = alu_dec8(gb, A);
  CYC(b_+135, b_+136); alu_add(gb, A);
  CYC(b_+136, b_+137); mem_wr(gb, DE, A);
  CYC(b_+137, b_+140); TAIL(objectSetVisible81); // jp
}
