#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void func_5e1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e1a, 0x5e1b); H = D;
  CYC(0x5e1b, 0x5e1d); L = 0xf2; // Part.var32
  CYC(0x5e1d, 0x5e1e); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x5e1e, 0x5e1f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5e1f, 0x5e21); goto L_5e31; } // jr nz
  CYC(0x5e1f, 0x5e21);
  CYC(0x5e21, 0x5e23); E = 0xc3; // Part.var03
  CYC(0x5e23, 0x5e24); A = mem_rd(gb, DE);
  CYC(0x5e24, 0x5e25); alu_add(gb, A);
  CYC(0x5e25, 0x5e26); alu_add(gb, A);
  CYC(0x5e26, 0x5e28); E = 0xc8;
  CYC(0x5e28, 0x5e29); mem_wr(gb, DE, A);
  CYC(0x5e29, 0x5e2a); C = mem_rd(gb, HL);
  CYC(0x5e2a, 0x5e2b); L = alu_dec8(gb, L);
  CYC(0x5e2b, 0x5e2c); B = mem_rd(gb, HL);
  CYC(0x5e2c, 0x5e2e); A = 0x38;
  CYC(0x5e2e, 0x5e31); objectSetPositionInCircleArc_hook(gb); return; // jp

L_5e31:
  CYC(0x5e31, 0x5e33); E = 0xcd; // Part.xh
  CYC(0x5e33, 0x5e34); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x5e34, 0x5e35); mem_wr(gb, DE, A);
  CYC(0x5e35, 0x5e37); E = 0xcb; // Part.yh
  CYC(0x5e37, 0x5e38); A = mem_rd(gb, HL);
  CYC(0x5e38, 0x5e39); mem_wr(gb, DE, A);
  RET(0x5e39); return; // ret
}

void blueEnergyBead_swirlBody_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5df5, 0x5df7); H8(hFF8B) = A;
  CYC(0x5df7, 0x5df8); push_effect(gb, DE);
  CYC(0x5df8, 0x5df9); E = L;
  CYC(0x5df9, 0x5dfb); D = 0x08;

L_5dfb:
  CALL_C(0x5dfb, getFreePartSlot_hook, 0x3e8e, 0x5dfe);
  if (!(F & FZ)) { CYCT(0x5dfe, 0x5e00); goto swirl_end; } // jr nz
  CYC(0x5dfe, 0x5e00);
  CYC(0x5e00, 0x5e02); mem_wr(gb, HL, 0x53); // PART_BLUE_ENERGY_BEAD
  CYC(0x5e02, 0x5e04); L = 0xc6; // Part.counter1
  CYC(0x5e04, 0x5e05); mem_wr(gb, HL, E);
  CYC(0x5e05, 0x5e07); L = 0xf0; // Part.var30
  CYC(0x5e07, 0x5e08); mem_wr(gb, HL, B);
  CYC(0x5e08, 0x5e09); L = alu_inc8(gb, L);
  CYC(0x5e09, 0x5e0a); mem_wr(gb, HL, C);
  CYC(0x5e0a, 0x5e0b); L = alu_inc8(gb, L);
  CYC(0x5e0b, 0x5e0d); A = H8(hFF8B);
  CYC(0x5e0d, 0x5e0e); mem_wr(gb, HL, A);
  CYC(0x5e0e, 0x5e10); L = 0xc3; // Part.var03
  CYC(0x5e10, 0x5e11); D = alu_dec8(gb, D);
  CYC(0x5e11, 0x5e12); mem_wr(gb, HL, D);
  if (!(F & FZ)) { CYCT(0x5e12, 0x5e14); goto L_5dfb; } // jr nz
  CYC(0x5e12, 0x5e14);

swirl_end:
  CYC(0x5e14, 0x5e15); SET_DE(pop_effect(gb));
  CYC(0x5e15, 0x5e17); A = 0x5c; // SND_ENERGYTHING
  CYC(0x5e17, 0x5e1a); playSound_b00_hook(gb); return; // jp
}

void createEnergySwirlGoingOut_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5df0, 0x5df2); A = 0x01;
  CYC(0x5df2, 0x5df4); blueEnergyBead_swirlBody_hook(gb); return; // jr
}

void createEnergySwirlGoingIn_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5df4, 0x5df5); alu_xor(gb, A);
  blueEnergyBead_swirlBody_hook(gb); return;
}

void partCode53_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d85, 0x5d87); E = 0xc4; // Part.state
  CYC(0x5d87, 0x5d88); A = mem_rd(gb, DE);
  CYC(0x5d88, 0x5d89); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5d89, 0x5d8b); goto state0; } // jr z
  CYC(0x5d89, 0x5d8b);
  CYC(0x5d8b, 0x5d8e); A = mem_rd(gb, 0xcd2d); // wDeleteEnergyBeads
  CYC(0x5d8e, 0x5d8f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5d8f, 0x5d92); partDelete_hook(gb); return; } // jp nz
  CYC(0x5d8f, 0x5d92);
  CYC(0x5d92, 0x5d93); H = D;
  CYC(0x5d93, 0x5d95); L = 0xc6; // Part.counter1
  CYC(0x5d95, 0x5d96); A = mem_rd(gb, HL);
  CYC(0x5d96, 0x5d97); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x5d97, 0x5d99); goto L_5d9d; } // jr z
  CYC(0x5d97, 0x5d99);
  CYC(0x5d99, 0x5d9a); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(0x5d9a, 0x5d9d); partDelete_hook(gb); return; } // jp z
  CYC(0x5d9a, 0x5d9d);

L_5d9d:
  CYC(0x5d9d, 0x5d9e); E = alu_inc8(gb, E);
  CYC(0x5d9e, 0x5d9f); A = mem_rd(gb, DE);
  CYC(0x5d9f, 0x5da0); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5da0, 0x5da2); goto L_5db9; } // jr nz
  CYC(0x5da0, 0x5da2);
  CYC(0x5da2, 0x5da3); L = alu_inc8(gb, L);
  CYC(0x5da3, 0x5da4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x5da4); return; } // ret nz
  CYC(0x5da4, 0x5da5);
  CYC(0x5da5, 0x5da6); L = E;
  CYC(0x5da6, 0x5da7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5da7, 0x5da9); L = 0xf2; // Part.var32
  CYC(0x5da9, 0x5daa); A = mem_rd(gb, HL);
  CYC(0x5daa, 0x5dac); A = alu_swap(gb, A);
  CYC(0x5dac, 0x5dad); alu_rrca(gb);
  CYC(0x5dad, 0x5daf); L = 0xc3; // Part.var03
  CYC(0x5daf, 0x5db0); alu_add(gb, mem_rd(gb, HL));
  CALL_C(0x5db0, partSetAnimation_hook, 0x2988, 0x5db3);
  CALL_C(0x5db3, func_5e1a_hook, 0x5e1a, 0x5db6);
  CYC(0x5db6, 0x5db9); objectSetVisible_hook(gb); return; // jp

L_5db9:
  CALL_C(0x5db9, objectApplySpeed_hook, 0x201d, 0x5dbc);
  CALL_C(0x5dbc, partAnimate_hook, 0x2978, 0x5dbf);
  CYC(0x5dbf, 0x5dc1); E = 0xe1; // Part.animParameter
  CYC(0x5dc1, 0x5dc2); A = mem_rd(gb, DE);
  CYC(0x5dc2, 0x5dc3); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5dc3); return; } // ret nz
  CYC(0x5dc3, 0x5dc4);
  CYC(0x5dc4, 0x5dc5); H = D;
  CYC(0x5dc5, 0x5dc7); L = 0xc5; // Part.substate
  CYC(0x5dc7, 0x5dc8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x5dc8, objectSetInvisible_hook, 0x1e7b, 0x5dcb);
  CYC(0x5dcb, 0x5dcd); goto L_5de6; // jr

state0:
  CYC(0x5dcd, 0x5dce); H = D;
  CYC(0x5dce, 0x5dcf); L = E;
  CYC(0x5dcf, 0x5dd0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5dd0, 0x5dd2); L = 0xc0;
  CYC(0x5dd2, 0x5dd4); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x5dd4, 0x5dd6); L = 0xd0; // Part.speed
  CYC(0x5dd6, 0x5dd8); mem_wr(gb, HL, 0x78);
  CYC(0x5dd8, 0x5dda); L = 0xc3; // Part.var03
  CYC(0x5dda, 0x5ddb); A = mem_rd(gb, HL);
  CYC(0x5ddb, 0x5ddc); alu_add(gb, A);
  CYC(0x5ddc, 0x5ddd); alu_add(gb, A);
  CYC(0x5ddd, 0x5ddf); alu_xor(gb, 0x10);
  CYC(0x5ddf, 0x5de1); L = 0xc9; // Part.angle
  CYC(0x5de1, 0x5de2); mem_wr(gb, HL, A);
  CYC(0x5de2, 0x5de3); alu_xor(gb, A);
  CYC(0x5de3, 0x5de6); mem_wr(gb, 0xcd2d, A); // wDeleteEnergyBeads

L_5de6:
  CALL_C(0x5de6, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5de9);
  CYC(0x5de9, 0x5deb); alu_and(gb, 0x07);
  CYC(0x5deb, 0x5dec); A = alu_inc8(gb, A);
  CYC(0x5dec, 0x5dee); E = 0xc7; // Part.counter2
  CYC(0x5dee, 0x5def); mem_wr(gb, DE, A);
  RET(0x5def); return; // ret
}
