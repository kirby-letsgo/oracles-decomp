#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode40), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode40), (from), (to), true)

static void headThwompBombDropper_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_HEAD_THWOMP_BOMB_DROPPER
void partCode40_hook(GB *gb) {
  BASE(partCode40);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01; // Object.id
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); partDelete_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); E = 0xc4; // Part.state
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+15, b_+17); goto state0; } // jr z
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0x20;
  CALL_C(b_+19, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+22);
  if (F & FC) { CYCT(b_+22, b_+25); partDelete_hook(gb); return; } // jp c
  CYC(b_+22, b_+25);
  CALL_C(b_+25, objectApplySpeed_hook, SYM(objectApplySpeed), b_+28);
  CYC(b_+28, b_+30); A = 0x00; // Object.state
  CALL_C(b_+30, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+33);
  CYC(b_+33, b_+36); objectCopyPosition_hook(gb); return; // jp

state0:
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+38); L = E;
  CYC(b_+38, b_+39); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+39, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+42);
  CYC(b_+42, b_+43); B = A;
  CYC(b_+43, b_+45); alu_and(gb, 0x03);
  CYC(b_+45, b_+48); SET_HL(b_+79); // table_732c (speedVals)
  CYC(b_+48, b_+49); headThwompBombDropper_addAToHl_from_rst(gb, b_+49);
  CYC(b_+49, b_+51); E = 0xd0; // Part.speed
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+54); A = B;
  CYC(b_+54, b_+56); alu_and(gb, 0x60);
  CYC(b_+56, b_+58); A = alu_swap(gb, A);
  CYC(b_+58, b_+61); SET_HL(b_+83); // table_7330 (speedZVals)
  CYC(b_+61, b_+62); headThwompBombDropper_addAToHl_from_rst(gb, b_+62);
  CYC(b_+62, b_+64); E = 0xd4; // Part.speedZ
  CYC(b_+64, b_+65); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  CYC(b_+66, b_+67); E = alu_inc8(gb, E);
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+71); A = mem_rd(gb, hRng2); // hRng2
  CYC(b_+71, b_+73); alu_and(gb, 0x10);
  CYC(b_+73, b_+75); alu_add(gb, 0x08);
  CYC(b_+75, b_+77); E = 0xc9; // Part.angle
  CYC(b_+77, b_+78); mem_wr(gb, DE, A);
  RET(b_+78); return; // ret
}
