#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void func_5e1a_hook(GB *gb) {
  BASE(func_5e1a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xf2; // Part.var32
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_5e31; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = 0xc3; // Part.var03
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+12); alu_add(gb, A);
  CYC(b_+12, b_+14); E = 0xc8;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); C = mem_rd(gb, HL);
  CYC(b_+16, b_+17); L = alu_dec8(gb, L);
  CYC(b_+17, b_+18); B = mem_rd(gb, HL);
  CYC(b_+18, b_+20); A = 0x38;
  CYC(b_+20, b_+23); TAIL(objectSetPositionInCircleArc); // jp

L_5e31:
  CYC(b_+23, b_+25); E = 0xcd; // Part.xh
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); E = 0xcb; // Part.yh
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  RET(b_+31); return; // ret
}

void blueEnergyBead_swirlBody_hook(GB *gb) {
  BASE(createEnergySwirlGoingIn_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+1, b_+3); H8(hFF8B) = A;
  CYC(b_+3, b_+4); push_effect(gb, DE);
  CYC(b_+4, b_+5); E = L;
  CYC(b_+5, b_+7); D = 0x08;

L_5dfb:
  CALL_C(b_+7, getFreePartSlot_hook, SYM(getFreePartSlot), b_+10);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto swirl_end; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x53); // PART_BLUE_ENERGY_BEAD
  CYC(b_+14, b_+16); L = 0xc6; // Part.counter1
  CYC(b_+16, b_+17); mem_wr(gb, HL, E);
  CYC(b_+17, b_+19); L = 0xf0; // Part.var30
  CYC(b_+19, b_+20); mem_wr(gb, HL, B);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+22); mem_wr(gb, HL, C);
  CYC(b_+22, b_+23); L = alu_inc8(gb, L);
  CYC(b_+23, b_+25); A = H8(hFF8B);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+28); L = 0xc3; // Part.var03
  CYC(b_+28, b_+29); D = alu_dec8(gb, D);
  CYC(b_+29, b_+30); mem_wr(gb, HL, D);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto L_5dfb; } // jr nz
  CYC(b_+30, b_+32);

swirl_end:
  CYC(b_+32, b_+33); SET_DE(pop_effect(gb));
  CYC(b_+33, b_+35); A = 0x5c; // SND_ENERGYTHING
  CYC(b_+35, b_+38); TAIL(playSound_b00); // jp
}

void createEnergySwirlGoingOut_body_hook(GB *gb) {
  BASE(createEnergySwirlGoingOut_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+4); blueEnergyBead_swirlBody_hook(gb); return; // jr
}

void createEnergySwirlGoingIn_body_hook(GB *gb) {
  BASE(createEnergySwirlGoingIn_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  blueEnergyBead_swirlBody_hook(gb); return;
}

void partCode53_hook(GB *gb) {
  BASE(partCode53);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto state0; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wDeleteEnergyBeads); // wDeleteEnergyBeads
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); partDelete_hook(gb); return; } // jp nz
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+16); L = 0xc6; // Part.counter1
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+18, b_+20); goto L_5d9d; } // jr z
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+21, b_+24); partDelete_hook(gb); return; } // jp z
  CYC(b_+21, b_+24);

L_5d9d:
  CYC(b_+24, b_+25); E = alu_inc8(gb, E);
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  CYC(b_+26, b_+27); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+27, b_+29); goto L_5db9; } // jr nz
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; } // ret nz
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+33); L = E;
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+34, b_+36); L = 0xf2; // Part.var32
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+39); A = alu_swap(gb, A);
  CYC(b_+39, b_+40); alu_rrca(gb);
  CYC(b_+40, b_+42); L = 0xc3; // Part.var03
  CYC(b_+42, b_+43); alu_add(gb, mem_rd(gb, HL));
  CALL_C(b_+43, partSetAnimation_hook, SYM(partSetAnimation), b_+46);
  CALL_C(b_+46, func_5e1a_hook, SYM(func_5e1a), b_+49);
  CYC(b_+49, b_+52); TAIL(objectSetVisible); // jp

L_5db9:
  CALL_C(b_+52, objectApplySpeed_hook, SYM(objectApplySpeed), b_+55);
  CALL_C(b_+55, partAnimate_hook, SYM(partAnimate), b_+58);
  CYC(b_+58, b_+60); E = 0xe1; // Part.animParameter
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+62); return; } // ret nz
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+64); H = D;
  CYC(b_+64, b_+66); L = 0xc5; // Part.substate
  CYC(b_+66, b_+67); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+67, objectSetInvisible_hook, SYM(objectSetInvisible), b_+70);
  CYC(b_+70, b_+72); goto L_5de6; // jr

state0:
  CYC(b_+72, b_+73); H = D;
  CYC(b_+73, b_+74); L = E;
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+75, b_+77); L = 0xc0;
  CYC(b_+77, b_+79); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+79, b_+81); L = 0xd0; // Part.speed
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x78);
  CYC(b_+83, b_+85); L = 0xc3; // Part.var03
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+87); alu_add(gb, A);
  CYC(b_+87, b_+88); alu_add(gb, A);
  CYC(b_+88, b_+90); alu_xor(gb, 0x10);
  CYC(b_+90, b_+92); L = 0xc9; // Part.angle
  CYC(b_+92, b_+93); mem_wr(gb, HL, A);
  CYC(b_+93, b_+94); alu_xor(gb, A);
  CYC(b_+94, b_+97); mem_wr(gb, wDeleteEnergyBeads, A); // wDeleteEnergyBeads

L_5de6:
  CALL_C(b_+97, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+100);
  CYC(b_+100, b_+102); alu_and(gb, 0x07);
  CYC(b_+102, b_+103); A = alu_inc8(gb, A);
  CYC(b_+103, b_+105); E = 0xc7; // Part.counter2
  CYC(b_+105, b_+106); mem_wr(gb, DE, A);
  RET(b_+106); return; // ret
}
