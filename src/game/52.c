#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t partCode52_jump_table(GB *gb) {
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

void partCode52_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c5c, 0x5c5e); A = 0x04;
  CALL_C(0x5c5e, objectGetRelatedObject1Var_hook, 0x2160, 0x5c61);
  CYC(0x5c61, 0x5c62); A = mem_rd(gb, HL);
  CYC(0x5c62, 0x5c64); alu_cp(gb, 0x0e);
  if (F & FZ) { CYCT(0x5c64, 0x5c67); partDelete_hook(gb); return; } // jp z
  CYC(0x5c64, 0x5c67);
  CYC(0x5c67, 0x5c69); E = 0xc2; // Part.subid
  CYC(0x5c69, 0x5c6a); A = mem_rd(gb, DE);
  CYC(0x5c6a, 0x5c6c); E = 0xc4; // Part.state
  {
    CYC(0x5c6c, 0x5c6d); push_effect(gb, 0x5c6d);
    uint16_t target = partCode52_jump_table(gb);
    if (target == 0x5c73) goto subid0;
    if (target == 0x5ca2) goto subid1;
    goto subid2;
  }

subid0:
  CYC(0x5c73, 0x5c74); A = mem_rd(gb, DE);
  {
    CYC(0x5c74, 0x5c75); push_effect(gb, 0x5c75);
    uint16_t target = partCode52_jump_table(gb);
    if (target == 0x5c7b) goto subid0_state0;
    if (target == 0x5c85) goto subid0_state1;
    goto subid0_state2;
  }

subid0_state0:
  CYC(0x5c7b, 0x5c7c); H = D;
  CYC(0x5c7c, 0x5c7d); L = E;
  CYC(0x5c7d, 0x5c7e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5c7e, 0x5c80); L = 0xc6; // Part.counter1
  CYC(0x5c80, 0x5c82); mem_wr(gb, HL, 0x0a);
  CYC(0x5c82, 0x5c85); objectSetVisible82_hook(gb); return; // jp

subid0_state1:
  CALL_C(0x5c85, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5c88);
  if (!(F & FZ)) { CYCT(0x5c88, 0x5c8a); goto L_5c9f; } // jr nz
  CYC(0x5c88, 0x5c8a);
  CYC(0x5c8a, 0x5c8b); L = E;
  CYC(0x5c8b, 0x5c8c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5c8c, 0x5c8e); A = 0xa4; // SND_BEAM
  CALL_C(0x5c8e, playSound_b00_hook, 0x0c98, 0x5c91);
  CYC(0x5c91, 0x5c93); A = 0x02;
  CALL_C(0x5c93, partSetAnimation_hook, 0x2988, 0x5c96);

subid0_state2:
  CALL_C(0x5c96, partCommon_checkOutOfBounds_hook, 0x407e, 0x5c99);
  if (F & FZ) { CYCT(0x5c99, 0x5c9c); partDelete_hook(gb); return; } // jp z
  CYC(0x5c99, 0x5c9c);
  CALL_C(0x5c9c, objectApplySpeed_hook, 0x201d, 0x5c9f);

L_5c9f:
  CYC(0x5c9f, 0x5ca2); partAnimate_hook(gb); return; // jp

subid1:
  CYC(0x5ca2, 0x5ca3); A = mem_rd(gb, DE);
  {
    CYC(0x5ca3, 0x5ca4); push_effect(gb, 0x5ca4);
    uint16_t target = partCode52_jump_table(gb);
    if (target == 0x5cac) goto subid1_state0;
    if (target == 0x5ce0) goto subid1_state1;
    if (target == 0x5d11) goto subid1_state2;
    goto subid0_state2;
  }

subid1_state0:
  CYC(0x5cac, 0x5cad); H = D;
  CYC(0x5cad, 0x5caf); L = 0xd0; // Part.speed
  CYC(0x5caf, 0x5cb1); mem_wr(gb, HL, 0x50);
  CYC(0x5cb1, 0x5cb2); L = E;
  CALL_C(0x5cb2, objectSetVisible82_hook, 0x1e69, 0x5cb5);
  CYC(0x5cb5, 0x5cb7); E = 0xc3; // Part.var03
  CYC(0x5cb7, 0x5cb8); A = mem_rd(gb, DE);
  CYC(0x5cb8, 0x5cb9); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5cb9, 0x5cbb); goto L_5cc4; } // jr z
  CYC(0x5cb9, 0x5cbb);
  CYC(0x5cbb, 0x5cbd); mem_wr(gb, HL, 0x03);
  CYC(0x5cbd, 0x5cbf); L = 0xe6; // Part.collisionRadiusY
  CYC(0x5cbf, 0x5cc1); A = 0x02;
  CYC(0x5cc1, 0x5cc2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5cc2, 0x5cc3); mem_wr(gb, HL, A);
  RET(0x5cc3); return; // ret

L_5cc4:
  CYC(0x5cc4, 0x5cc5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5cc5, 0x5cc7); L = 0xc6; // Part.counter1
  CYC(0x5cc7, 0x5cc9); mem_wr(gb, HL, 0x28);
  CYC(0x5cc9, 0x5ccb); L = 0xe6; // Part.collisionRadiusY
  CYC(0x5ccb, 0x5ccd); A = 0x04;
  CYC(0x5ccd, 0x5cce); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5cce, 0x5ccf); mem_wr(gb, HL, A);
  CYC(0x5ccf, 0x5cd1); E = 0xcb; // Part.yh
  CYC(0x5cd1, 0x5cd3); L = 0xf0; // Part.var30
  CYC(0x5cd3, 0x5cd4); A = mem_rd(gb, DE);
  CYC(0x5cd4, 0x5cd6); alu_add(gb, 0x20);
  CYC(0x5cd6, 0x5cd7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5cd7, 0x5cd9); E = 0xcd; // Part.xh
  CYC(0x5cd9, 0x5cda); A = mem_rd(gb, DE);
  CYC(0x5cda, 0x5cdb); mem_wr(gb, HL, A);
  CYC(0x5cdb, 0x5cdd); A = 0x01;
  CALL_C(0x5cdd, partSetAnimation_hook, 0x2988, 0x5ce0);

subid1_state1:
  CALL_C(0x5ce0, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5ce3);
  if (F & FZ) { CYCT(0x5ce3, 0x5ce5); goto L_5d02; } // jr z
  CYC(0x5ce3, 0x5ce5);
  CYC(0x5ce5, 0x5ce6); A = mem_rd(gb, HL);
  CYC(0x5ce6, 0x5ce7); alu_rrca(gb);
  CYC(0x5ce7, 0x5ce9); E = 0xc9; // Part.angle
  if (F & FC) { CYCT(0x5ce9, 0x5ceb); goto L_5cf0; } // jr c
  CYC(0x5ce9, 0x5ceb);
  CYC(0x5ceb, 0x5cec); A = mem_rd(gb, DE);
  CYC(0x5cec, 0x5ced); A = alu_inc8(gb, A);
  CYC(0x5ced, 0x5cef); alu_and(gb, 0x1f);
  CYC(0x5cef, 0x5cf0); mem_wr(gb, DE, A);

L_5cf0:
  CYC(0x5cf0, 0x5cf2); L = 0xda; // Part.visible
  CYC(0x5cf2, 0x5cf3); A = mem_rd(gb, HL);
  CYC(0x5cf3, 0x5cf5); alu_xor(gb, 0x80);
  CYC(0x5cf5, 0x5cf6); mem_wr(gb, HL, A);
  CYC(0x5cf6, 0x5cf8); L = 0xf0; // Part.var30
  CYC(0x5cf8, 0x5cf9); B = mem_rd(gb, HL);
  CYC(0x5cf9, 0x5cfa); L = alu_inc8(gb, L);
  CYC(0x5cfa, 0x5cfb); C = mem_rd(gb, HL);
  CYC(0x5cfb, 0x5cfd); A = 0x08;
  CALL_C(0x5cfd, objectSetPositionInCircleArc_hook, 0x210e, 0x5d00);
  CYC(0x5d00, 0x5d02); goto subid1_animate; // jr

L_5d02:
  CYC(0x5d02, 0x5d04); mem_wr(gb, HL, 0x0a);
  CYC(0x5d04, 0x5d05); L = E;
  CYC(0x5d05, 0x5d06); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d06, 0x5d08); A = 0xbe; // SND_VERAN_PROJECTILE
  CALL_C(0x5d08, playSound_b00_hook, 0x0c98, 0x5d0b);
  CALL_C(0x5d0b, objectSetVisible82_hook, 0x1e69, 0x5d0e);

subid1_animate:
  CYC(0x5d0e, 0x5d11); partAnimate_hook(gb); return; // jp

subid1_state2:
  CALL_C(0x5d11, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5d14);
  if (F & FZ) { CYCT(0x5d14, 0x5d16); goto L_5d1b; } // jr z
  CYC(0x5d14, 0x5d16);
  CALL_C(0x5d16, objectApplySpeed_hook, 0x201d, 0x5d19);
  CYC(0x5d19, 0x5d1b); goto subid1_animate; // jr

L_5d1b:
  CYC(0x5d1b, 0x5d1c); L = E;
  CYC(0x5d1c, 0x5d1d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d1d, 0x5d1f); L = 0xe6; // Part.collisionRadiusY
  CYC(0x5d1f, 0x5d21); A = 0x02;
  CYC(0x5d21, 0x5d22); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5d22, 0x5d23); mem_wr(gb, HL, A);
  CYC(0x5d23, 0x5d24); alu_xor(gb, A);
  CALL_C(0x5d24, partSetAnimation_hook, 0x2988, 0x5d27);
  CALL_C(0x5d27, objectCreatePuff_hook, 0x24c1, 0x5d2a);
  CYC(0x5d2a, 0x5d2c); B = 0xfd;
  CYC(0x5d2c, 0x5d2f); push_effect(gb, 0x5d2f); goto func_5d31_call1; // call $5d31

func_5d31_call1:
  CALL_C(0x5d31, getFreePartSlot_hook, 0x3e8e, 0x5d34);
  if (!(F & FZ)) { CYCT(0x5d34, 0x5d35); ret_effect(gb); goto resume_5d2f; } // ret nz
  CYC(0x5d34, 0x5d35);
  CYC(0x5d35, 0x5d37); mem_wr(gb, HL, 0x52); // PART_52
  CYC(0x5d37, 0x5d38); L = alu_inc8(gb, L);
  CYC(0x5d38, 0x5d39); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d39, 0x5d3a); L = alu_inc8(gb, L);
  CYC(0x5d3a, 0x5d3b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d3b, 0x5d3d); L = 0xc9; // Part.angle
  CYC(0x5d3d, 0x5d3e); E = L;
  CYC(0x5d3e, 0x5d3f); A = mem_rd(gb, DE);
  CYC(0x5d3f, 0x5d40); alu_add(gb, B);
  CYC(0x5d40, 0x5d42); alu_and(gb, 0x1f);
  CYC(0x5d42, 0x5d43); mem_wr(gb, HL, A);
  CYC(0x5d43, 0x5d46); objectCopyPosition_hook(gb); goto resume_5d2f; // jp

resume_5d2f:
  CYC(0x5d2f, 0x5d31); B = 0x03;

func_5d31_call2:
  CALL_C(0x5d31, getFreePartSlot_hook, 0x3e8e, 0x5d34);
  if (!(F & FZ)) { RET_TAKEN(0x5d34); return; } // ret nz
  CYC(0x5d34, 0x5d35);
  CYC(0x5d35, 0x5d37); mem_wr(gb, HL, 0x52); // PART_52
  CYC(0x5d37, 0x5d38); L = alu_inc8(gb, L);
  CYC(0x5d38, 0x5d39); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d39, 0x5d3a); L = alu_inc8(gb, L);
  CYC(0x5d3a, 0x5d3b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d3b, 0x5d3d); L = 0xc9; // Part.angle
  CYC(0x5d3d, 0x5d3e); E = L;
  CYC(0x5d3e, 0x5d3f); A = mem_rd(gb, DE);
  CYC(0x5d3f, 0x5d40); alu_add(gb, B);
  CYC(0x5d40, 0x5d42); alu_and(gb, 0x1f);
  CYC(0x5d42, 0x5d43); mem_wr(gb, HL, A);
  CYC(0x5d43, 0x5d46); objectCopyPosition_hook(gb); return; // jp

subid2:
  CYC(0x5d46, 0x5d47); A = mem_rd(gb, DE);
  {
    CYC(0x5d47, 0x5d48); push_effect(gb, 0x5d48);
    uint16_t target = partCode52_jump_table(gb);
    if (target == 0x5d50) goto subid2_state0;
    if (target == 0x5d5a) goto subid2_state1;
    if (target == 0x5d6e) goto subid2_state2;
    goto subid0_state2;
  }

subid2_state0:
  CYC(0x5d50, 0x5d51); H = D;
  CYC(0x5d51, 0x5d52); L = E;
  CYC(0x5d52, 0x5d53); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d53, 0x5d55); L = 0xc6; // Part.counter1
  CYC(0x5d55, 0x5d57); mem_wr(gb, HL, 0x0f);
  CYC(0x5d57, 0x5d5a); objectSetVisible82_hook(gb); return; // jp

subid2_state1:
  CALL_C(0x5d5a, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5d5d);
  if (!(F & FZ)) { CYCT(0x5d5d, 0x5d60); partAnimate_hook(gb); return; } // jp nz
  CYC(0x5d5d, 0x5d60);
  CYC(0x5d60, 0x5d62); mem_wr(gb, HL, 0x0f);
  CYC(0x5d62, 0x5d63); L = E;
  CYC(0x5d63, 0x5d64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d64, 0x5d66); A = 0xa8; // SND_VERAN_FAIRY_ATTACK
  CALL_C(0x5d66, playSound_b00_hook, 0x0c98, 0x5d69);
  CYC(0x5d69, 0x5d6b); A = 0x01;
  CYC(0x5d6b, 0x5d6e); partSetAnimation_hook(gb); return; // jp

subid2_state2:
  CALL_C(0x5d6e, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5d71);
  if (!(F & FZ)) { CYCT(0x5d71, 0x5d74); partAnimate_hook(gb); return; } // jp nz
  CYC(0x5d71, 0x5d74);
  CYC(0x5d74, 0x5d75); L = E;
  CYC(0x5d75, 0x5d76); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d76, 0x5d78); L = 0xd0; // Part.speed
  CYC(0x5d78, 0x5d7a); mem_wr(gb, HL, 0x5a);
  CALL_C(0x5d7a, objectGetAngleTowardLink_hook, 0x1e9c, 0x5d7d);
  CYC(0x5d7d, 0x5d7f); E = 0xc9; // Part.angle
  CYC(0x5d7f, 0x5d80); mem_wr(gb, DE, A);
  CYC(0x5d80, 0x5d82); A = 0x02;
  CYC(0x5d82, 0x5d85); partSetAnimation_hook(gb); return; // jp
}
