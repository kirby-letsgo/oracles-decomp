#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t partCode51_jump_table(GB *gb) {
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

static void partCode51_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void partCode51_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b67, 0x5b69); A = 0x04;
  CALL_C(0x5b69, objectGetRelatedObject1Var_hook, 0x2160, 0x5b6c);
  CYC(0x5b6c, 0x5b6d); A = mem_rd(gb, HL);
  CYC(0x5b6d, 0x5b6f); alu_cp(gb, 0x0e);
  if (F & FZ) { CYCT(0x5b6f, 0x5b72); partDelete_hook(gb); return; } // jp z
  CYC(0x5b6f, 0x5b72);
  CYC(0x5b72, 0x5b74); E = 0xc2; // Part.subid
  CYC(0x5b74, 0x5b75); A = mem_rd(gb, DE);
  CYC(0x5b75, 0x5b77); E = 0xc4; // Part.state
  {
    CYC(0x5b77, 0x5b78); push_effect(gb, 0x5b78);
    uint16_t target = partCode51_jump_table(gb);
    if (target == 0x5b7e) goto subid0;
    if (target == 0x5be2) goto subid1;
    goto subid2;
  }

subid0:
  CYC(0x5b7e, 0x5b7f); A = mem_rd(gb, DE);
  CYC(0x5b7f, 0x5b80); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5b80, 0x5b82); goto L_5b96; } // jr nz
  CYC(0x5b80, 0x5b82);
  CYC(0x5b82, 0x5b83); H = D;
  CYC(0x5b83, 0x5b84); L = E;
  CYC(0x5b84, 0x5b85); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5b85, 0x5b87); L = 0xc6; // Part.counter1
  CYC(0x5b87, 0x5b89); mem_wr(gb, HL, 0x40);
  CYC(0x5b89, 0x5b8b); L = 0xe8; // Part.damage
  CYC(0x5b8b, 0x5b8d); mem_wr(gb, HL, 0xf0);
  CYC(0x5b8d, 0x5b8f); L = 0xda; // Part.visible
  CYC(0x5b8f, 0x5b91); mem_wr(gb, HL, 0x02);
  CYC(0x5b91, 0x5b93); A = 0x5c; // SND_ENERGYTHING
  CALL_C(0x5b93, playSound_b00_hook, 0x0c98, 0x5b96);

L_5b96:
  CALL_C(0x5b96, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5b99);
  if (F & FZ) { CYCT(0x5b99, 0x5b9c); partDelete_hook(gb); return; } // jp z
  CYC(0x5b99, 0x5b9c);
  CYC(0x5b9c, 0x5b9e); goto L_5ba9; // jr

subid2:
  CYC(0x5b9e, 0x5b9f); A = mem_rd(gb, DE);
  CYC(0x5b9f, 0x5ba0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5ba0, 0x5ba2); goto L_5bb2; } // jr z
  CYC(0x5ba0, 0x5ba2);
  CYC(0x5ba2, 0x5ba4); E = 0xe1; // Part.animParameter
  CYC(0x5ba4, 0x5ba5); A = mem_rd(gb, DE);
  CYC(0x5ba5, 0x5ba6); alu_rlca(gb);
  if (F & FC) { CYCT(0x5ba6, 0x5ba9); partDelete_hook(gb); return; } // jp c
  CYC(0x5ba6, 0x5ba9);

L_5ba9:
  CYC(0x5ba9, 0x5bab); E = 0xda; // Part.visible
  CYC(0x5bab, 0x5bac); A = mem_rd(gb, DE);
  CYC(0x5bac, 0x5bae); alu_xor(gb, 0x80);
  CYC(0x5bae, 0x5baf); mem_wr(gb, DE, A);
  CYC(0x5baf, 0x5bb2); partAnimate_hook(gb); return; // jp

L_5bb2:
  CYC(0x5bb2, 0x5bb3); H = D;
  CYC(0x5bb3, 0x5bb4); L = E;
  CYC(0x5bb4, 0x5bb5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5bb5, 0x5bb7); L = 0xe4; // Part.collisionType
  CYC(0x5bb7, 0x5bb9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x5bb9, 0x5bbb); L = 0xc9; // Part.angle
  CYC(0x5bbb, 0x5bbc); A = mem_rd(gb, HL);
  CYC(0x5bbc, 0x5bbe); B = 0x01;
  CYC(0x5bbe, 0x5bc0); alu_cp(gb, 0x0c);
  if (F & FC) { CYCT(0x5bc0, 0x5bc2); goto L_5bc8; } // jr c
  CYC(0x5bc0, 0x5bc2);
  CYC(0x5bc2, 0x5bc3); B = alu_inc8(gb, B);
  CYC(0x5bc3, 0x5bc5); alu_cp(gb, 0x19);
  if (F & FC) { CYCT(0x5bc5, 0x5bc7); goto L_5bc8; } // jr c
  CYC(0x5bc5, 0x5bc7);
  CYC(0x5bc7, 0x5bc8); B = alu_inc8(gb, B);

L_5bc8:
  CYC(0x5bc8, 0x5bc9); A = B;
  CYC(0x5bc9, 0x5bca); A = alu_dec8(gb, A);
  CYC(0x5bca, 0x5bcc); alu_and(gb, 0x01);
  CYC(0x5bcc, 0x5bcf); SET_HL(0x5bde); // table_5bde
  CYC(0x5bcf, 0x5bd0); partCode51_addDoubleIndexToHl_from_rst(gb, 0x5bd0);
  CYC(0x5bd0, 0x5bd2); E = 0xe6; // Part.collisionRadiusY
  CYC(0x5bd2, 0x5bd3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5bd3, 0x5bd4); mem_wr(gb, DE, A);
  CYC(0x5bd4, 0x5bd5); E = alu_inc8(gb, E);
  CYC(0x5bd5, 0x5bd6); A = mem_rd(gb, HL);
  CYC(0x5bd6, 0x5bd7); mem_wr(gb, DE, A);
  CYC(0x5bd7, 0x5bd8); A = B;
  CALL_C(0x5bd8, partSetAnimation_hook, 0x2988, 0x5bdb);
  CYC(0x5bdb, 0x5bde); objectSetVisible83_hook(gb); return; // jp

subid1:
  CYC(0x5be2, 0x5be3); A = mem_rd(gb, DE);
  {
    CYC(0x5be3, 0x5be4); push_effect(gb, 0x5be4);
    uint16_t target = partCode51_jump_table(gb);
    if (target == 0x5bea) goto state0;
    if (target == 0x5bff) goto state1;
    goto state2;
  }

state0:
  CYC(0x5bea, 0x5beb); H = D;
  CYC(0x5beb, 0x5bec); L = E;
  CYC(0x5bec, 0x5bed); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5bed, 0x5bef); L = 0xdd; // Part.oamTileIndexBase
  CYC(0x5bef, 0x5bf0); A = mem_rd(gb, HL);
  CYC(0x5bf0, 0x5bf2); alu_add(gb, 0x0e);
  CYC(0x5bf2, 0x5bf3); mem_wr(gb, HL, A);
  CYC(0x5bf3, 0x5bf5); L = 0xc6; // Part.counter1
  CYC(0x5bf5, 0x5bf7); mem_wr(gb, HL, 0x18);
  CYC(0x5bf7, 0x5bf9); A = 0x04;
  CALL_C(0x5bf9, partSetAnimation_hook, 0x2988, 0x5bfc);
  CYC(0x5bfc, 0x5bff); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(0x5bff, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5c02);
  if (!(F & FZ)) { CYCT(0x5c02, 0x5c04); goto animate; } // jr nz
  CYC(0x5c02, 0x5c04);
  CYC(0x5c04, 0x5c05); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5c05, 0x5c06); L = E;
  CYC(0x5c06, 0x5c07); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5c07, 0x5c09); L = 0xe4; // Part.collisionType
  CYC(0x5c09, 0x5c0b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x5c0b, 0x5c0d); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x5c0d, 0x5c0f); A = 0x05;
  CYC(0x5c0f, 0x5c10); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5c10, 0x5c11); mem_wr(gb, HL, A);
  CYC(0x5c11, 0x5c13); L = 0xcb; // Part.yh
  CYC(0x5c13, 0x5c14); A = mem_rd(gb, HL);
  CYC(0x5c14, 0x5c16); alu_add(gb, 0x08);
  CYC(0x5c16, 0x5c17); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5c17, 0x5c18); L = alu_inc8(gb, L);
  CYC(0x5c18, 0x5c19); A = mem_rd(gb, HL);
  CYC(0x5c19, 0x5c1b); alu_sub(gb, 0x10);
  CYC(0x5c1b, 0x5c1c); mem_wr(gb, HL, A);
  CALL_C(0x5c1c, objectGetAngleTowardLink_hook, 0x1e9c, 0x5c1f);
  CYC(0x5c1f, 0x5c21); E = 0xc9; // Part.angle
  CYC(0x5c21, 0x5c22); mem_wr(gb, DE, A);
  CYC(0x5c22, 0x5c23); C = A;
  CYC(0x5c23, 0x5c25); B = 0x50;
  CYC(0x5c25, 0x5c27); A = 0x02;
  CYC(0x5c27, 0x5c2a); objectSetComponentSpeedByScaledVelocity_hook(gb); return; // jp

state2:
  CALL_C(0x5c2a, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x5c2d);
  if (!(F & FC)) { CYCT(0x5c2d, 0x5c2f); goto L_5c3c; } // jr nc
  CYC(0x5c2d, 0x5c2f);
  CYC(0x5c2f, 0x5c31); B = 0x56; // INTERAC_EXPLOSION
  CALL_C(0x5c31, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x5c34);
  CYC(0x5c34, 0x5c36); A = 0x3c;
  if (F & FZ) { CALL_C_CC(0x5c36, setScreenShakeCounter_hook, 0x24bb, 0x5c39); } else { CYC(0x5c36, 0x5c39); } // call z
  CYC(0x5c39, 0x5c3c); partDelete_hook(gb); return; // jp

L_5c3c:
  CALL_C(0x5c3c, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5c3f);
  CYC(0x5c3f, 0x5c40); A = mem_rd(gb, HL);
  CYC(0x5c40, 0x5c42); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x5c42, 0x5c44); goto L_5c56; } // jr nz
  CYC(0x5c42, 0x5c44);
  CALL_C(0x5c44, getFreePartSlot_hook, 0x3e8e, 0x5c47);
  if (!(F & FZ)) { CYCT(0x5c47, 0x5c49); goto L_5c56; } // jr nz
  CYC(0x5c47, 0x5c49);
  CYC(0x5c49, 0x5c4b); mem_wr(gb, HL, 0x51); // PART_51
  CYC(0x5c4b, 0x5c4c); L = alu_inc8(gb, L);
  CYC(0x5c4c, 0x5c4e); mem_wr(gb, HL, 0x02);
  CYC(0x5c4e, 0x5c50); L = 0xc9; // Part.angle
  CYC(0x5c50, 0x5c51); E = L;
  CYC(0x5c51, 0x5c52); A = mem_rd(gb, DE);
  CYC(0x5c52, 0x5c53); mem_wr(gb, HL, A);
  CALL_C(0x5c53, objectCopyPosition_hook, 0x2242, 0x5c56);

L_5c56:
  CALL_C(0x5c56, objectApplyComponentSpeed_hook, 0x2008, 0x5c59);

animate:
  CYC(0x5c59, 0x5c5c); partAnimate_hook(gb); return; // jp
}
