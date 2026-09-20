#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static void enemySword_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

void enemySword_func_5273_hook(GB *gb) {
  BASE(partCode1d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+141, b_+143); A = 0x01;
  CALL_C(b_+143, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+146);
  CYC(b_+146, b_+148); E = 0xf0; // Part.var30
  CYC(b_+148, b_+149); A = mem_rd(gb, DE);
  CYC(b_+149, b_+150); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+150, b_+151); return; } // ret nz
  CYC(b_+150, b_+151);
  CYC(b_+151, b_+153); L = 0xb0;
  CYC(b_+153, b_+155); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+155, b_+157); goto L_5294; } // jr nz
  CYC(b_+155, b_+157);
  CYC(b_+157, b_+159); L = 0xa9;
  CYC(b_+159, b_+160); A = mem_rd(gb, HL);
  CYC(b_+160, b_+161); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+161, b_+163); goto L_5294; } // jr z
  CYC(b_+161, b_+163);
  CYC(b_+163, b_+165); L = 0xae;
  CYC(b_+165, b_+166); A = mem_rd(gb, HL);
  CYC(b_+166, b_+167); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+167, b_+169); goto L_5294; } // jr nz
  CYC(b_+167, b_+169);
  CYC(b_+169, b_+171); L = 0xbf;
  CYC(b_+171, b_+173); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+173, b_+174); return; } // ret z
  CYC(b_+173, b_+174);

L_5294:
  CYC(b_+174, b_+176); E = 0xe4; // Part.collisionType
  CYC(b_+176, b_+177); A = mem_rd(gb, DE);
  CYC(b_+177, b_+179); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+179, b_+180); mem_wr(gb, DE, A);
  CYC(b_+180, b_+181); alu_xor(gb, A);
  CYC(b_+181, b_+182); return; // ret
}

void partCode1d_hook(GB *gb) {
  BASE(partCode1d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(b_+7, b_+9); goto normalStatus; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); alu_cp(gb, 0x8a);
  if (F & FZ) { CYCT(b_+11, b_+13); goto normalStatus; } // jr z
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = 0x2b;
  CALL_C(b_+15, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+22); goto L_5200; } // jr nz
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); E = 0xeb; // Part.invincibilityCounter
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);

L_5200:
  CYC(b_+26, b_+28); E = 0xec;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+33); E = 0xed; // Part.knockbackCounter
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);

normalStatus:
  CYC(b_+35, b_+37); E = 0xc4; // Part.state
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+39, b_+41); goto func_5261; } // jr z
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = 0xe4; // Part.collisionType
  CYC(b_+44, b_+46); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+46, b_+49); enemySword_func_5273_hook(gb);
  if (!(F & FZ)) { CYCT(b_+49, b_+52); partDelete_hook(gb); return; } // jp nz
  CYC(b_+49, b_+52);

func_521a:
  CYC(b_+52, b_+54); L = 0x8b;
  CYC(b_+54, b_+55); B = mem_rd(gb, HL);
  CYC(b_+55, b_+57); L = 0x8d;
  CYC(b_+57, b_+58); C = mem_rd(gb, HL);
  CYC(b_+58, b_+60); L = 0x89;
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+63); alu_add(gb, 0x04);
  CYC(b_+63, b_+65); alu_and(gb, 0x18);
  CYC(b_+65, b_+66); alu_rrca(gb);
  CYC(b_+66, b_+68); H8(hFF8B) = A;
  CYC(b_+68, b_+70); L = 0xa1;
  CYC(b_+70, b_+71); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+71, b_+72); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+72, b_+75); SET_HL(b_+103);
  CYC(b_+75, b_+76); enemySword_addAToHl_from_rst(gb, b_+76);
  CYC(b_+76, b_+78); E = 0xcb; // Part.yh
  CYC(b_+78, b_+79); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+79, b_+80); alu_add(gb, B);
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  CYC(b_+81, b_+83); E = 0xcd; // Part.xh
  CYC(b_+83, b_+84); A = mem_rd(gb, HL);
  CYC(b_+84, b_+85); alu_add(gb, C);
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+88); A = H8(hFF8B);
  CYC(b_+88, b_+89); alu_rrca(gb);
  CYC(b_+89, b_+91); alu_and(gb, 0x02);
  CYC(b_+91, b_+94); SET_HL(b_+119);
  CYC(b_+94, b_+95); enemySword_addAToHl_from_rst(gb, b_+95);
  CYC(b_+95, b_+97); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+97, b_+98); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+98, b_+99); mem_wr(gb, DE, A);
  CYC(b_+99, b_+100); E = alu_inc8(gb, E);
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);
  RET(b_+102); return; // ret

func_5261:
  CYC(b_+123, b_+124); H = D;
  CYC(b_+124, b_+125); L = E;
  CYC(b_+125, b_+126); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+126, b_+128); L = 0xfe;
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0x04);
  CYC(b_+130, b_+132); A = 0x01;
  CALL_C(b_+132, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+135);
  CYC(b_+135, b_+137); E = 0xf0; // Part.var30
  CYC(b_+137, b_+138); A = mem_rd(gb, HL);
  CYC(b_+138, b_+139); mem_wr(gb, DE, A);
  CYC(b_+139, b_+141); goto func_521a; // jr
}
