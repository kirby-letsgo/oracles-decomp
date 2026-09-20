#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(func_5b2b), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(func_5b2b), (from), (to), true)

static uint16_t ganonTrident_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void ganonTrident_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void func_5b2b_hook(GB *gb) {
  BASE(func_5b2b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+4); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+4, b_+6); goto L_5b3e; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CALL_C(b_+8, objectSetVisible82_hook, SYM(objectSetVisible82), b_+11);
  CYC(b_+11, b_+13); A = 0xb1; // SND_BIGSWORD
  CALL_C(b_+13, playSound_b00_hook, SYM(playSound_b00), b_+16);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = 0xe1; // Part.animParameter

L_5b3e:
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+23); SET_HL(SYM(table_5b5b)); // table_5b5b
  CYC(b_+23, b_+24); ganonTrident_addAToHl_from_rst(gb, b_+24);
  CYC(b_+24, b_+26); E = 0xe6; // Part.collisionRadiusY
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); E = alu_inc8(gb, E);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+32, b_+33); B = A;
  CYC(b_+33, b_+34); C = mem_rd(gb, HL);
  CYC(b_+34, b_+35); SET_HL(pop_effect(gb));
  CYC(b_+35, b_+37); L = 0xb2; // Enemy.var32
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+39, b_+41); goto L_5b58; } // jr z
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); A = C;
  CYC(b_+42, b_+43); alu_cpl(gb);
  CYC(b_+43, b_+44); A = alu_inc8(gb, A);
  CYC(b_+44, b_+45); C = A;

L_5b58:
  CYC(b_+45, SYM(table_5b5b)); objectTakePositionWithOffset_hook(gb); return; // jp
}

void partCode50_hook(GB *gb) {
  BASE(partCode50);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x0e);
  if (F & FZ) { CYCT(b_+8, b_+11); partDelete_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); push_effect(gb, HL);
  CYC(b_+12, b_+14); E = 0xc4; // Part.state
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  {
    CYC(b_+15, b_+16); push_effect(gb, b_+16);
    uint16_t target = ganonTrident_jump_table(gb);
    if (target == b_+22) goto state0;
    if (target == b_+40) goto state1;
    goto state2;
  }

state0:
  CYC(b_+22, b_+24); A = 0x01;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26); SET_HL(pop_effect(gb));
  CALL_C(b_+26, objectTakePosition_hook, SYM(objectTakePosition), b_+29);
  CYC(b_+29, b_+31); L = 0xb2; // Enemy.var32
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+33); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+33, b_+35); goto L_5b05; } // jr z
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); A = 0x01;

L_5b05:
  CYC(b_+37, b_+40); partSetAnimation_hook(gb); return; // jp

state1:
  CALL_C(b_+40, partAnimate_hook, SYM(partAnimate), b_+43);
  CYC(b_+43, b_+45); E = 0xe1; // Part.animParameter
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+47, b_+49); func_5b2b_hook(gb); return; } // jr nz
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = 0xc4; // Part.state
  CYC(b_+52, b_+53); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+53, b_+55); L = 0xe6; // Part.collisionRadiusY
  CYC(b_+55, b_+57); A = 0x07;
  CYC(b_+57, b_+58); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+58, b_+59); mem_wr(gb, HL, A);
  CALL_C(b_+59, objectSetInvisible_hook, SYM(objectSetInvisible), b_+62);

state2:
  CYC(b_+62, b_+63); SET_HL(pop_effect(gb));
  CYC(b_+63, b_+64); L = alu_inc8(gb, L);
  CYC(b_+64, b_+65); A = mem_rd(gb, HL);
  CYC(b_+65, b_+66); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+66, b_+69); partDelete_hook(gb); return; } // jp z
  CYC(b_+66, b_+69);
  CYC(b_+69, b_+72); SET_BC((SYM(objectCheckCenteredWithLink) + 18));
  CYC(b_+72, SYM(func_5b2b)); objectTakePositionWithOffset_hook(gb); return; // jp
}
