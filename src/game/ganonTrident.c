#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b2b, 0x5b2c); H = D;
  CYC(0x5b2c, 0x5b2d); L = E;
  CYC(0x5b2d, 0x5b2f); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5b2f, 0x5b31); goto L_5b3e; } // jr z
  CYC(0x5b2f, 0x5b31);
  CYC(0x5b31, 0x5b33); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CALL_C(0x5b33, objectSetVisible82_hook, 0x1e69, 0x5b36);
  CYC(0x5b36, 0x5b38); A = 0xb1; // SND_BIGSWORD
  CALL_C(0x5b38, playSound_b00_hook, 0x0c98, 0x5b3b);
  CYC(0x5b3b, 0x5b3c); H = D;
  CYC(0x5b3c, 0x5b3e); L = 0xe1; // Part.animParameter

L_5b3e:
  CYC(0x5b3e, 0x5b3f); A = mem_rd(gb, HL);
  CYC(0x5b3f, 0x5b42); SET_HL(0x5b5b); // table_5b5b
  CYC(0x5b42, 0x5b43); ganonTrident_addAToHl_from_rst(gb, 0x5b43);
  CYC(0x5b43, 0x5b45); E = 0xe6; // Part.collisionRadiusY
  CYC(0x5b45, 0x5b46); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5b46, 0x5b47); mem_wr(gb, DE, A);
  CYC(0x5b47, 0x5b48); E = alu_inc8(gb, E);
  CYC(0x5b48, 0x5b49); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5b49, 0x5b4a); mem_wr(gb, DE, A);
  CYC(0x5b4a, 0x5b4b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5b4b, 0x5b4c); B = A;
  CYC(0x5b4c, 0x5b4d); C = mem_rd(gb, HL);
  CYC(0x5b4d, 0x5b4e); SET_HL(pop_effect(gb));
  CYC(0x5b4e, 0x5b50); L = 0xb2; // Enemy.var32
  CYC(0x5b50, 0x5b51); A = mem_rd(gb, HL);
  CYC(0x5b51, 0x5b52); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5b52, 0x5b54); goto L_5b58; } // jr z
  CYC(0x5b52, 0x5b54);
  CYC(0x5b54, 0x5b55); A = C;
  CYC(0x5b55, 0x5b56); alu_cpl(gb);
  CYC(0x5b56, 0x5b57); A = alu_inc8(gb, A);
  CYC(0x5b57, 0x5b58); C = A;

L_5b58:
  CYC(0x5b58, 0x5b5b); objectTakePositionWithOffset_hook(gb); return; // jp
}

void partCode50_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ae0, 0x5ae2); A = 0x04;
  CALL_C(0x5ae2, objectGetRelatedObject1Var_hook, 0x2160, 0x5ae5);
  CYC(0x5ae5, 0x5ae6); A = mem_rd(gb, HL);
  CYC(0x5ae6, 0x5ae8); alu_cp(gb, 0x0e);
  if (F & FZ) { CYCT(0x5ae8, 0x5aeb); partDelete_hook(gb); return; } // jp z
  CYC(0x5ae8, 0x5aeb);
  CYC(0x5aeb, 0x5aec); push_effect(gb, HL);
  CYC(0x5aec, 0x5aee); E = 0xc4; // Part.state
  CYC(0x5aee, 0x5aef); A = mem_rd(gb, DE);
  {
    CYC(0x5aef, 0x5af0); push_effect(gb, 0x5af0);
    uint16_t target = ganonTrident_jump_table(gb);
    if (target == 0x5af6) goto state0;
    if (target == 0x5b08) goto state1;
    goto state2;
  }

state0:
  CYC(0x5af6, 0x5af8); A = 0x01;
  CYC(0x5af8, 0x5af9); mem_wr(gb, DE, A);
  CYC(0x5af9, 0x5afa); SET_HL(pop_effect(gb));
  CALL_C(0x5afa, objectTakePosition_hook, 0x2274, 0x5afd);
  CYC(0x5afd, 0x5aff); L = 0xb2; // Enemy.var32
  CYC(0x5aff, 0x5b00); A = mem_rd(gb, HL);
  CYC(0x5b00, 0x5b01); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5b01, 0x5b03); goto L_5b05; } // jr z
  CYC(0x5b01, 0x5b03);
  CYC(0x5b03, 0x5b05); A = 0x01;

L_5b05:
  CYC(0x5b05, 0x5b08); partSetAnimation_hook(gb); return; // jp

state1:
  CALL_C(0x5b08, partAnimate_hook, 0x2978, 0x5b0b);
  CYC(0x5b0b, 0x5b0d); E = 0xe1; // Part.animParameter
  CYC(0x5b0d, 0x5b0e); A = mem_rd(gb, DE);
  CYC(0x5b0e, 0x5b0f); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x5b0f, 0x5b11); func_5b2b_hook(gb); return; } // jr nz
  CYC(0x5b0f, 0x5b11);
  CYC(0x5b11, 0x5b12); H = D;
  CYC(0x5b12, 0x5b14); L = 0xc4; // Part.state
  CYC(0x5b14, 0x5b15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5b15, 0x5b17); L = 0xe6; // Part.collisionRadiusY
  CYC(0x5b17, 0x5b19); A = 0x07;
  CYC(0x5b19, 0x5b1a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5b1a, 0x5b1b); mem_wr(gb, HL, A);
  CALL_C(0x5b1b, objectSetInvisible_hook, 0x1e7b, 0x5b1e);

state2:
  CYC(0x5b1e, 0x5b1f); SET_HL(pop_effect(gb));
  CYC(0x5b1f, 0x5b20); L = alu_inc8(gb, L);
  CYC(0x5b20, 0x5b21); A = mem_rd(gb, HL);
  CYC(0x5b21, 0x5b22); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5b22, 0x5b25); partDelete_hook(gb); return; } // jp z
  CYC(0x5b22, 0x5b25);
  CYC(0x5b25, 0x5b28); SET_BC(0x2000);
  CYC(0x5b28, 0x5b2b); objectTakePositionWithOffset_hook(gb); return; // jp
}
