#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t veranSpiderweb_jump_table(GB *gb) {
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

void partCode56_hook(GB *gb);

void partCode56_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x7bab, 0x7bad); goto normalStatus; } // jr z
  CYC(0x7bab, 0x7bad);
  CYC(0x7bad, 0x7baf); E = 0xea; // Part.var2a
  CYC(0x7baf, 0x7bb0); A = mem_rd(gb, DE);
  CYC(0x7bb0, 0x7bb2); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x7bb2, 0x7bb4); goto normalStatus; } // jr nz
  CYC(0x7bb2, 0x7bb4);
  CYC(0x7bb4, 0x7bb7); SET_HL(0xd031);
  CYC(0x7bb7, 0x7bb9); mem_wr(gb, HL, 0x10);
  CYC(0x7bb9, 0x7bbb); L = 0x30;
  CYC(0x7bbb, 0x7bbd); mem_wr(gb, HL, 0x00);
  CYC(0x7bbd, 0x7bbf); L = 0x24;
  CYC(0x7bbf, 0x7bc1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x7bc1, 0x7bc4); SET_BC(0xfa00);
  CALL_C(0x7bc4, objectCopyPositionWithOffset_hook, 0x225a, 0x7bc7);
  CYC(0x7bc7, 0x7bc8); H = D;
  CYC(0x7bc8, 0x7bca); L = 0xf0; // Part.var30
  CYC(0x7bca, 0x7bcc); mem_wr(gb, HL, 0x01);
  CYC(0x7bcc, 0x7bce); L = 0xc4; // Part.state
  CYC(0x7bce, 0x7bcf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7bcf, 0x7bd0); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x7bd0, 0x7bd2); goto normalStatus; } // jr nz
  CYC(0x7bd0, 0x7bd2);
  CYC(0x7bd2, 0x7bd3); L = alu_inc8(gb, L);
  CYC(0x7bd3, 0x7bd5); A = 0x01;
  CYC(0x7bd5, 0x7bd6); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7bd6, 0x7bd7); mem_wr(gb, HL, A);

normalStatus:
  CYC(0x7bd7, 0x7bd9); E = 0xc2; // Part.subid
  CYC(0x7bd9, 0x7bda); A = mem_rd(gb, DE);
  CYC(0x7bda, 0x7bdc); E = 0xc4; // Part.state
  {
    CYC(0x7bdc, 0x7bdd); push_effect(gb, 0x7bdd);
    uint16_t target = veranSpiderweb_jump_table(gb);
    if (target == 0x7be5) goto subid0;
    if (target == 0x7c3c) goto subid1;
    if (target == 0x7d1f) goto subid2;
    goto subid3;
  }

subid0:
  CYC(0x7be5, 0x7be6); A = mem_rd(gb, DE);
  CYC(0x7be6, 0x7be7); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7be7, 0x7be9); goto func_7c2e; } // jr z
  CYC(0x7be7, 0x7be9);
  CALL_C(0x7be9, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7bec);
  if (!(F & FZ)) { CYCT(0x7bec, 0x7bee); goto L_7c05; } // jr nz
  CYC(0x7bec, 0x7bee);
  CYC(0x7bee, 0x7bf0); mem_wr(gb, HL, 0x04);
  CALL_C(0x7bf0, getFreePartSlot_hook, 0x3e8e, 0x7bf3);
  if (!(F & FZ)) { CYCT(0x7bf3, 0x7bf5); goto L_7c05; } // jr nz
  CYC(0x7bf3, 0x7bf5);
  CYC(0x7bf5, 0x7bf7); mem_wr(gb, HL, 0x56); // PART_VERAN_SPIDERWEB
  CYC(0x7bf7, 0x7bf8); L = alu_inc8(gb, L);
  CYC(0x7bf8, 0x7bfa); mem_wr(gb, HL, 0x02);
  CYC(0x7bfa, 0x7bfc); L = 0xd6; // Part.relatedObj1
  CYC(0x7bfc, 0x7bfd); E = L;
  CYC(0x7bfd, 0x7bfe); A = mem_rd(gb, DE);
  CYC(0x7bfe, 0x7bff); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7bff, 0x7c00); E = alu_inc8(gb, E);
  CYC(0x7c00, 0x7c01); A = mem_rd(gb, DE);
  CYC(0x7c01, 0x7c02); mem_wr(gb, HL, A);
  CALL_C(0x7c02, objectCopyPosition_hook, 0x2242, 0x7c05);
L_7c05:
  CYC(0x7c05, 0x7c07); A = 0x02;
  CALL_C(0x7c07, objectGetRelatedObject1Var_hook, 0x2160, 0x7c0a);
  CYC(0x7c0a, 0x7c0b); A = mem_rd(gb, HL);
  CYC(0x7c0b, 0x7c0c); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x7c0c, 0x7c0f); goto func_7c28; } // jp nz
  CYC(0x7c0c, 0x7c0f);
  CYC(0x7c0f, 0x7c10); C = H;
  CYC(0x7c10, 0x7c12); A = hram_rd(gb, 0xaa); // hCameraY
  CYC(0x7c12, 0x7c13); B = A;
  CYC(0x7c13, 0x7c15); E = 0xcf; // Part.zh
  CYC(0x7c15, 0x7c16); A = mem_rd(gb, DE);
  CYC(0x7c16, 0x7c18); alu_sub(gb, 0x04);
  CYC(0x7c18, 0x7c19); mem_wr(gb, DE, A);
  CYC(0x7c19, 0x7c1a); H = D;
  CYC(0x7c1a, 0x7c1c); L = 0xcb; // Part.yh
  CYC(0x7c1c, 0x7c1d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7c1d, 0x7c1e); alu_sub(gb, B);
  CYC(0x7c1e, 0x7c20); alu_cp(gb, 0xb0);
  if (F & FC) { RET_TAKEN(0x7c20); return; } // ret c
  CYC(0x7c20, 0x7c21);
  CYC(0x7c21, 0x7c22); H = C;
  CYC(0x7c22, 0x7c24); L = 0xb8; // Enemy.animParameter?
  CYC(0x7c24, 0x7c25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7c25, 0x7c28); partDelete_hook(gb); return; // jp

func_7c28:
  CALL_C(0x7c28, objectCreatePuff_hook, 0x24c1, 0x7c2b);
  CYC(0x7c2b, 0x7c2e); partDelete_hook(gb); return; // jp

func_7c2e:
  CYC(0x7c2e, 0x7c2f); H = D;
  CYC(0x7c2f, 0x7c30); L = E; // Part.state
  CYC(0x7c30, 0x7c31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7c31, 0x7c33); L = 0xc6; // Part.counter1
  CYC(0x7c33, 0x7c34); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x7c34, objectSetVisible80_hook, 0x1e57, 0x7c37);
beamSound:
  CYC(0x7c37, 0x7c39); A = 0xbb; // SND_BEAM2
  CYC(0x7c39, 0x7c3c); playSound_b00_hook(gb); return; // jp

subid1:
  CYC(0x7c3c, 0x7c3e); A = 0x02;
  CALL_C(0x7c3e, objectGetRelatedObject1Var_hook, 0x2160, 0x7c41);
  CYC(0x7c41, 0x7c42); A = mem_rd(gb, HL);
  CYC(0x7c42, 0x7c43); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x7c43, 0x7c45); goto func_7c28; } // jr nz
  CYC(0x7c43, 0x7c45);
  CYC(0x7c45, 0x7c47); L = 0xad; // Part.var2d?
  CYC(0x7c47, 0x7c48); A = mem_rd(gb, HL);
  CYC(0x7c48, 0x7c49); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7c49, 0x7c4b); goto func_7c28; } // jr nz
  CYC(0x7c49, 0x7c4b);
  CYC(0x7c4b, 0x7c4d); E = 0xc4; // Part.state
  CYC(0x7c4d, 0x7c4e); A = mem_rd(gb, DE);
  {
    CYC(0x7c4e, 0x7c4f); push_effect(gb, 0x7c4f);
    uint16_t target = veranSpiderweb_jump_table(gb);
    if (target == 0x7c59) goto subid1_state0;
    if (target == 0x7c92) goto subid1_state1;
    if (target == 0x7cc9) goto subid1_state2;
    if (target == 0x7cd1) goto subid1_state3;
    goto subid1_state4;
  }

subid1_state0:
  CYC(0x7c59, 0x7c5a); H = D;
  CYC(0x7c5a, 0x7c5b); L = E; // Part.state
  CYC(0x7c5b, 0x7c5c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7c5c, 0x7c5e); L = 0xc6; // Part.counter1
  CYC(0x7c5e, 0x7c60); mem_wr(gb, HL, 0x01);
  CYC(0x7c60, 0x7c61); L = alu_inc8(gb, L);
  CYC(0x7c61, 0x7c63); mem_wr(gb, HL, 0x05);
  CYC(0x7c63, 0x7c65); L = 0xe4; // Part.collisionType
  CYC(0x7c65, 0x7c67); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x7c67, 0x7c69); L = 0xd0; // Part.speed
  CYC(0x7c69, 0x7c6b); mem_wr(gb, HL, 0x50);
  CYC(0x7c6b, 0x7c6d); L = 0xf1; // Part.speedZ
  CYC(0x7c6d, 0x7c6f); E = 0xcb; // Part.yh
  CYC(0x7c6f, 0x7c70); A = mem_rd(gb, DE);
  CYC(0x7c70, 0x7c72); alu_add(gb, 0x10);
  CYC(0x7c72, 0x7c73); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7c73, 0x7c74); mem_wr(gb, DE, A);
  CYC(0x7c74, 0x7c76); E = 0xcd; // Part.xh
  CYC(0x7c76, 0x7c77); A = mem_rd(gb, DE);
  CYC(0x7c77, 0x7c78); mem_wr(gb, HL, A);
  CALL_C(0x7c78, objectGetAngleTowardLink_hook, 0x1e9c, 0x7c7b);
  CYC(0x7c7b, 0x7c7d); alu_cp(gb, 0x0e);
  CYC(0x7c7d, 0x7c7f); B = 0x0c;
  if (F & FC) { CYCT(0x7c7f, 0x7c81); goto L_7c89; } // jr c
  CYC(0x7c7f, 0x7c81);
  CYC(0x7c81, 0x7c83); B = 0x10;
  CYC(0x7c83, 0x7c85); alu_cp(gb, 0x13);
  if (F & FC) { CYCT(0x7c85, 0x7c87); goto L_7c89; } // jr c
  CYC(0x7c85, 0x7c87);
  CYC(0x7c87, 0x7c89); B = 0x14;
L_7c89:
  CYC(0x7c89, 0x7c8b); E = 0xc9; // Part.angle
  CYC(0x7c8b, 0x7c8c); A = B;
  CYC(0x7c8c, 0x7c8d); mem_wr(gb, DE, A);
  CALL_C(0x7c8d, objectSetVisible81_hook, 0x1e60, 0x7c90);
  CYC(0x7c90, 0x7c92); goto beamSound; // jr

subid1_state1:
  CALL_C(0x7c92, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7c95);
  if (!(F & FZ)) { CYCT(0x7c95, 0x7c97); goto L_7cb0; } // jr nz
  CYC(0x7c95, 0x7c97);
  CYC(0x7c97, 0x7c99); mem_wr(gb, HL, 0x08); // Part.counter1
  CYC(0x7c99, 0x7c9a); L = alu_inc8(gb, L);
  CYC(0x7c9a, 0x7c9b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // Part.counter2
  if (F & FZ) { CYCT(0x7c9b, 0x7c9d); goto L_7cb6; } // jr z
  CYC(0x7c9b, 0x7c9d);
  CALL_C(0x7c9d, getFreePartSlot_hook, 0x3e8e, 0x7ca0);
  if (!(F & FZ)) { CYCT(0x7ca0, 0x7ca2); goto L_7cb0; } // jr nz
  CYC(0x7ca0, 0x7ca2);
  CYC(0x7ca2, 0x7ca4); mem_wr(gb, HL, 0x56); // PART_VERAN_SPIDERWEB
  CYC(0x7ca4, 0x7ca5); L = alu_inc8(gb, L);
  CYC(0x7ca5, 0x7ca7); mem_wr(gb, HL, 0x03);
  CYC(0x7ca7, 0x7ca9); L = 0xd6; // Part.relatedObj1
  CYC(0x7ca9, 0x7cab); A = 0xc0;
  CYC(0x7cab, 0x7cac); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7cac, 0x7cad); mem_wr(gb, HL, D);
  CALL_C(0x7cad, objectCopyPosition_hook, 0x2242, 0x7cb0);
L_7cb0:
  CALL_C(0x7cb0, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x7cb3);
  if (!(F & FC)) { CYCT(0x7cb3, 0x7cb6); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(0x7cb3, 0x7cb6);
L_7cb6:
  CYC(0x7cb6, 0x7cb7); H = D;
  CYC(0x7cb7, 0x7cb9); L = 0xc4; // Part.state
  CYC(0x7cb9, 0x7cba); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7cba, 0x7cbc); L = 0xc6; // Part.counter1
  CYC(0x7cbc, 0x7cbe); mem_wr(gb, HL, 0x1e);
  CYC(0x7cbe, 0x7cc0); L = 0xd0; // Part.speed
  CYC(0x7cc0, 0x7cc2); mem_wr(gb, HL, 0x3c);
  CYC(0x7cc2, 0x7cc4); L = 0xc9; // Part.angle
  CYC(0x7cc4, 0x7cc5); A = mem_rd(gb, HL);
  CYC(0x7cc5, 0x7cc7); alu_xor(gb, 0x10);
  CYC(0x7cc7, 0x7cc8); mem_wr(gb, HL, A);
  RET(0x7cc8); return; // ret

subid1_state2:
  CALL_C(0x7cc9, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7ccc);
  if (!(F & FZ)) { RET_TAKEN(0x7ccc); return; } // ret nz
  CYC(0x7ccc, 0x7ccd);
  CYC(0x7ccd, 0x7ccf); L = 0xc4; // Part.state
  CYC(0x7ccf, 0x7cd0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x7cd0); return; // ret

subid1_state3:
  CALL_C(0x7cd1, objectApplySpeed_hook, 0x201d, 0x7cd4);
  CYC(0x7cd4, 0x7cd6); E = 0xf0; // Part.var30
  CYC(0x7cd6, 0x7cd7); A = mem_rd(gb, DE);
  CYC(0x7cd7, 0x7cd8); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7cd8, 0x7cda); goto L_7ce3; } // jr z
  CYC(0x7cd8, 0x7cda);
  CYC(0x7cda, 0x7cdd); SET_BC(0xfa00);
  CYC(0x7cdd, 0x7ce0); SET_HL(0xd000);
  CALL_C(0x7ce0, objectCopyPositionWithOffset_hook, 0x225a, 0x7ce3);
L_7ce3:
  CYC(0x7ce3, 0x7ce4); H = D;
  CYC(0x7ce4, 0x7ce6); L = 0xf1; // Part.speedZ
  CYC(0x7ce6, 0x7ce8); E = 0xcb; // Part.yh
  CYC(0x7ce8, 0x7ce9); A = mem_rd(gb, DE);
  CYC(0x7ce9, 0x7cea); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x7cea, 0x7cec); alu_add(gb, 0x02);
  CYC(0x7cec, 0x7cee); alu_cp(gb, 0x05);
  if (!(F & FC)) { RET_TAKEN(0x7cee); return; } // ret nc
  CYC(0x7cee, 0x7cef);
  CYC(0x7cef, 0x7cf1); L = 0xf2; // Part.speedZ+1
  CYC(0x7cf1, 0x7cf3); E = 0xcd; // Part.xh
  CYC(0x7cf3, 0x7cf4); A = mem_rd(gb, DE);
  CYC(0x7cf4, 0x7cf5); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x7cf5, 0x7cf7); alu_add(gb, 0x02);
  CYC(0x7cf7, 0x7cf9); alu_cp(gb, 0x05);
  if (!(F & FC)) { RET_TAKEN(0x7cf9); return; } // ret nc
  CYC(0x7cf9, 0x7cfa);
  CYC(0x7cfa, 0x7cfc); A = 0x38;
  CALL_C(0x7cfc, objectGetRelatedObject1Var_hook, 0x2160, 0x7cff);
  CYC(0x7cff, 0x7d00); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7d00, 0x7d02); E = 0xf0; // Part.var30
  CYC(0x7d02, 0x7d03); A = mem_rd(gb, DE);
  CYC(0x7d03, 0x7d04); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7d04, 0x7d07); partDelete_hook(gb); return; } // jp z
  CYC(0x7d04, 0x7d07);
  CYC(0x7d07, 0x7d09); L = 0x86; // Enemy.var06?
  CYC(0x7d09, 0x7d0b); mem_wr(gb, HL, 0x08);
  CYC(0x7d0b, 0x7d0c); H = D;
  CYC(0x7d0c, 0x7d0e); L = 0xc4; // Part.state
  CYC(0x7d0e, 0x7d0f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x7d0f); return; // ret

subid1_state4:
  CYC(0x7d10, 0x7d13); SET_HL(0xd005);
  CYC(0x7d13, 0x7d14); A = mem_rd(gb, HL);
  CYC(0x7d14, 0x7d16); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x7d16, 0x7d19); partDelete_hook(gb); return; } // jp z
  CYC(0x7d16, 0x7d19);
  CYC(0x7d19, 0x7d1c); SET_BC(0x0600);
  CYC(0x7d1c, 0x7d1f); objectTakePositionWithOffset_hook(gb); return; // jp

subid2:
  CYC(0x7d1f, 0x7d20); A = mem_rd(gb, DE);
  CYC(0x7d20, 0x7d21); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7d21, 0x7d23); goto func_7d39; } // jr z
  CYC(0x7d21, 0x7d23);
  CYC(0x7d23, 0x7d25); A = 0x1a;
  CALL_C(0x7d25, objectGetRelatedObject1Var_hook, 0x2160, 0x7d28);
  CYC(0x7d28, 0x7d2a); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7d2a, 0x7d2c); goto subid2_delete; } // jr z
  CYC(0x7d2a, 0x7d2c);
  CYC(0x7d2c, 0x7d2e); L = 0x8f; // Enemy.var0f?
  CYC(0x7d2e, 0x7d2f); B = mem_rd(gb, HL);
  CYC(0x7d2f, 0x7d30); B = alu_dec8(gb, B);
  CYC(0x7d30, 0x7d32); E = 0xcf; // Part.zh
  CYC(0x7d32, 0x7d33); A = mem_rd(gb, DE);
  CYC(0x7d33, 0x7d34); A = alu_dec8(gb, A);
  CYC(0x7d34, 0x7d35); alu_cp(gb, B);
  if (F & FC) { RET_TAKEN(0x7d35); return; } // ret c
  CYC(0x7d35, 0x7d36);
subid2_delete:
  CYC(0x7d36, 0x7d39); partDelete_hook(gb); return; // jp

func_7d39:
  CYC(0x7d39, 0x7d3a); A = alu_inc8(gb, A);
  CYC(0x7d3a, 0x7d3b); mem_wr(gb, DE, A);
  CYC(0x7d3b, 0x7d3c); A = alu_inc8(gb, A);
  CALL_C(0x7d3c, partSetAnimation_hook, 0x2988, 0x7d3f);
  CYC(0x7d3f, 0x7d42); objectSetVisible80_hook(gb); return; // jp

subid3:
  CYC(0x7d42, 0x7d43); A = mem_rd(gb, DE);
  CYC(0x7d43, 0x7d44); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7d44, 0x7d46); goto func_7d59; } // jr z
  CYC(0x7d44, 0x7d46);
  CYC(0x7d46, 0x7d48); A = 0x01;
  CALL_C(0x7d48, objectGetRelatedObject1Var_hook, 0x2160, 0x7d4b);
  CYC(0x7d4b, 0x7d4c); A = mem_rd(gb, HL);
  CYC(0x7d4c, 0x7d4e); alu_cp(gb, 0x56);
  if (!(F & FZ)) { CYCT(0x7d4e, 0x7d50); goto subid3_delete; } // jr nz
  CYC(0x7d4e, 0x7d50);
  CYC(0x7d50, 0x7d52); L = 0xcb; // Part.yh
  CYC(0x7d52, 0x7d53); E = L;
  CYC(0x7d53, 0x7d54); A = mem_rd(gb, DE);
  CYC(0x7d54, 0x7d55); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { RET_TAKEN(0x7d55); return; } // ret c
  CYC(0x7d55, 0x7d56);
subid3_delete:
  CYC(0x7d56, 0x7d59); partDelete_hook(gb); return; // jp

func_7d59:
  CYC(0x7d59, 0x7d5a); A = alu_inc8(gb, A);
  CYC(0x7d5a, 0x7d5b); mem_wr(gb, DE, A);
  CYC(0x7d5b, 0x7d5d); A = 0x09;
  CALL_C(0x7d5d, objectGetRelatedObject1Var_hook, 0x2160, 0x7d60);
  CYC(0x7d60, 0x7d61); A = mem_rd(gb, HL);
  CYC(0x7d61, 0x7d63); alu_sub(gb, 0x0c);
  CYC(0x7d63, 0x7d64); alu_rrca(gb);
  CYC(0x7d64, 0x7d65); alu_rrca(gb);
  CYC(0x7d65, 0x7d66); A = alu_inc8(gb, A);
  CALL_C(0x7d66, partSetAnimation_hook, 0x2988, 0x7d69);
  CYC(0x7d69, 0x7d6c); objectSetVisible83_hook(gb); return; // jp
}
