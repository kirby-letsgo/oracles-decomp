#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t vireProjectile_jump_table(GB *gb) {
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

static void vireProjectile_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode3a_hook(GB *gb);
void vireProjectile_func_6d22_hook(GB *gb);
void func_6e2f_hook(GB *gb);
void func_6e4a_hook(GB *gb);
void func_6e50_hook(GB *gb);
void func_6e5d_hook(GB *gb);

void vireProjectile_func_6d22_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6d22, getFreePartSlot_hook, 0x3e8e, 0x6d25);
  CYC(0x6d25, 0x6d27); mem_wr(gb, HL, 0x3a); // PART_VIRE_PROJECTILE
  CYC(0x6d27, 0x6d28); L = alu_inc8(gb, L);
  CYC(0x6d28, 0x6d29); mem_wr(gb, HL, E);
  CYC(0x6d29, 0x6d2a); L = alu_inc8(gb, L);
  CYC(0x6d2a, 0x6d2b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6d2b, 0x6d2d); L = 0xc9; // Part.angle
  CYC(0x6d2d, 0x6d2e); mem_wr(gb, HL, B);
  CYC(0x6d2e, 0x6d30); L = 0xd6; // Part.relatedObj1
  CYC(0x6d30, 0x6d31); E = L;
  CYC(0x6d31, 0x6d32); A = mem_rd(gb, DE);
  CYC(0x6d32, 0x6d33); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6d33, 0x6d34); E = alu_inc8(gb, E);
  CYC(0x6d34, 0x6d35); A = mem_rd(gb, DE);
  CYC(0x6d35, 0x6d36); mem_wr(gb, HL, A);
  CYC(0x6d36, 0x6d39); objectCopyPosition_hook(gb); return; // jp
}

void partCode3a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x6cca, 0x6ccc); goto L_6cd9; } // jr z
  CYC(0x6cca, 0x6ccc);
  CYC(0x6ccc, 0x6cce); E = 0xea; // Part.var2a
  CYC(0x6cce, 0x6ccf); A = mem_rd(gb, DE);
  CYC(0x6ccf, 0x6cd1); A = (uint8_t)(A & ~(1 << 7));
  CYC(0x6cd1, 0x6cd3); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(0x6cd3, 0x6cd6); partDelete_hook(gb); return; } // jp c
  CYC(0x6cd3, 0x6cd6);
  CYC(0x6cd6, 0x6cd9); func_6e4a_hook(gb); return; // jp

L_6cd9:
  CYC(0x6cd9, 0x6cdb); E = 0xc2; // Part.subid
  CYC(0x6cdb, 0x6cdc); A = mem_rd(gb, DE);
  CYC(0x6cdc, 0x6cde); E = 0xc4; // Part.state
  {
    CYC(0x6cde, 0x6cdf); push_effect(gb, 0x6cdf);
    uint16_t target = vireProjectile_jump_table(gb);
    if (target == 0x6ce7) goto subid0;
    if (target == 0x6d06) goto subid1;
    if (target == 0x6d39) goto subid2;
    goto subid3;
  }

subid0:
  CYC(0x6ce7, 0x6ce8); A = mem_rd(gb, DE);
  CYC(0x6ce8, 0x6ce9); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6ce9, 0x6ceb); goto subid0_state0; } // jr z
  CYC(0x6ce9, 0x6ceb);
func_6ceb:
  CALL_C(0x6ceb, partCommon_checkOutOfBounds, 0x407e, 0x6cee);
  if (F & FZ) { CYCT(0x6cee, 0x6cf1); partDelete_hook(gb); return; } // jp z
  CYC(0x6cee, 0x6cf1);
  CALL_C(0x6cf1, objectApplySpeed_hook, 0x201d, 0x6cf4);
  CYC(0x6cf4, 0x6cf7); partAnimate_hook(gb); return; // jp

subid0_state0:
  CALL_C(0x6cf7, func_6e50_hook, 0x6e50, 0x6cfa);
  CALL_C(0x6cfa, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6cfd);
  CYC(0x6cfd, 0x6cff); E = 0xc9; // Part.angle
  CYC(0x6cff, 0x6d00); mem_wr(gb, DE, A);
  CALL_C(0x6d00, func_6e5d_hook, 0x6e5d, 0x6d03);
  CYC(0x6d03, 0x6d06); objectSetVisible80_hook(gb); return; // jp

subid1:
  CYC(0x6d06, 0x6d07); A = mem_rd(gb, DE);
  CYC(0x6d07, 0x6d08); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6d08, 0x6d0a); goto func_6ceb; } // jr nz
  CYC(0x6d08, 0x6d0a);
  CALL_C(0x6d0a, func_6e50_hook, 0x6e50, 0x6d0d);
  CALL_C(0x6d0d, func_6e2f_hook, 0x6e2f, 0x6d10);
  CYC(0x6d10, 0x6d12); E = 0xc3; // Part.var03
  CYC(0x6d12, 0x6d13); A = mem_rd(gb, DE);
  CYC(0x6d13, 0x6d14); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6d14); return; } // ret nz
  CYC(0x6d14, 0x6d15);
  CALL_C(0x6d15, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6d18);
  CYC(0x6d18, 0x6d1a); E = 0xc9; // Part.angle
  CYC(0x6d1a, 0x6d1b); mem_wr(gb, DE, A);
  CYC(0x6d1b, 0x6d1d); alu_sub(gb, 0x02);
  CYC(0x6d1d, 0x6d1f); alu_and(gb, 0x1f);
  CYC(0x6d1f, 0x6d20); B = A;
  CYC(0x6d20, 0x6d22); E = 0x01;
  vireProjectile_func_6d22_hook(gb); return; // falls through to func_6d22

subid2:
  CYC(0x6d39, 0x6d3a); A = mem_rd(gb, DE);
  {
    CYC(0x6d3a, 0x6d3b); push_effect(gb, 0x6d3b);
    uint16_t target = vireProjectile_jump_table(gb);
    if (target == 0x6d43) goto subid2_state0;
    if (target == 0x6d84) goto subid2_state1;
    if (target == 0x6dc6) goto subid2_state2;
    goto func_6ceb;
  }

subid2_state0:
  CYC(0x6d43, 0x6d44); H = D;
  CYC(0x6d44, 0x6d46); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x6d46, 0x6d48); A = 0x03;
  CYC(0x6d48, 0x6d49); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6d49, 0x6d4a); mem_wr(gb, HL, A);
  CYC(0x6d4a, 0x6d4c); L = 0xc3; // Part.var03
  CYC(0x6d4c, 0x6d4d); A = mem_rd(gb, HL);
  CYC(0x6d4d, 0x6d4e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6d4e, 0x6d50); goto fimc_6d5e; } // jr z
  CYC(0x6d4e, 0x6d50);
  CYC(0x6d50, 0x6d51); L = E;
  CYC(0x6d51, 0x6d53); mem_wr(gb, HL, 0x03);
  CALL_C(0x6d53, func_6e5d_hook, 0x6e5d, 0x6d56);
  CYC(0x6d56, 0x6d58); A = 0x01;
  CALL_C(0x6d58, partSetAnimation_hook, 0x2988, 0x6d5b);
  CYC(0x6d5b, 0x6d5e); objectSetVisible82_hook(gb); return; // jp

fimc_6d5e:
  CALL_C(0x6d5e, func_6e50_hook, 0x6e50, 0x6d61);
  CYC(0x6d61, 0x6d63); L = 0xf0; // Part.speedZ
  CYC(0x6d63, 0x6d65); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x6d65, 0x6d66); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6d66, 0x6d68); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x6d68, 0x6d69); mem_wr(gb, HL, A);
  CYC(0x6d69, 0x6d6b); A = 0x29; // Object.health
  CALL_C(0x6d6b, objectGetRelatedObject1Var_hook, 0x2160, 0x6d6e);
  CYC(0x6d6e, 0x6d6f); A = mem_rd(gb, HL);
  CYC(0x6d6f, 0x6d71); B = 0x19;
  CYC(0x6d71, 0x6d73); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(0x6d73, 0x6d75); goto L_6d7d; } // jr nc
  CYC(0x6d73, 0x6d75);
  CYC(0x6d75, 0x6d77); B = 0x2d;
  CYC(0x6d77, 0x6d79); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x6d79, 0x6d7b); goto L_6d7d; } // jr nc
  CYC(0x6d79, 0x6d7b);
  CYC(0x6d7b, 0x6d7d); B = 0x41;
L_6d7d:
  CYC(0x6d7d, 0x6d7f); E = 0xd0; // Part.speed
  CYC(0x6d7f, 0x6d80); A = B;
  CYC(0x6d80, 0x6d81); mem_wr(gb, DE, A);
  CYC(0x6d81, 0x6d84); objectSetVisible80_hook(gb); return; // jp

subid2_state1:
  CYC(0x6d84, 0x6d85); H = D;
  CYC(0x6d85, 0x6d87); L = 0xf0; // Part.speedZ
  CYC(0x6d87, 0x6d88); B = mem_rd(gb, HL);
  CYC(0x6d88, 0x6d89); L = alu_inc8(gb, L);
  CYC(0x6d89, 0x6d8a); C = mem_rd(gb, HL);
  CYC(0x6d8a, 0x6d8c); L = 0xcb; // Part.yh
  CYC(0x6d8c, 0x6d8d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6d8d, 0x6d8f); hram_wr(gb, 0x8f, A);
  CYC(0x6d8f, 0x6d90); L = alu_inc8(gb, L);
  CYC(0x6d90, 0x6d91); A = mem_rd(gb, HL);
  CYC(0x6d91, 0x6d93); hram_wr(gb, 0x8e, A);
  CYC(0x6d93, 0x6d94); alu_sub(gb, C);
  CYC(0x6d94, 0x6d96); alu_add(gb, 0x02);
  CYC(0x6d96, 0x6d98); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x6d98, 0x6d9a); goto func_6dba; } // jr nc
  CYC(0x6d98, 0x6d9a);
  CYC(0x6d9a, 0x6d9c); A = hram_rd(gb, 0x8f);
  CYC(0x6d9c, 0x6d9d); alu_sub(gb, B);
  CYC(0x6d9d, 0x6d9f); alu_add(gb, 0x02);
  CYC(0x6d9f, 0x6da1); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x6da1, 0x6da3); goto func_6dba; } // jr nc
  CYC(0x6da1, 0x6da3);
  CYC(0x6da3, 0x6da6); SET_BC(0x0502); // INTERAC_PUFF, $02
  CALL_C(0x6da6, objectCreateInteraction_hook, 0x24c5, 0x6da9);
  if (!(F & FZ)) { RET_TAKEN(0x6da9); return; } // ret nz
  CYC(0x6da9, 0x6daa);
  CYC(0x6daa, 0x6dac); E = 0xd8;
  CYC(0x6dac, 0x6dae); A = 0x40;
  CYC(0x6dae, 0x6daf); mem_wr(gb, DE, A);
  CYC(0x6daf, 0x6db0); E = alu_inc8(gb, E);
  CYC(0x6db0, 0x6db1); A = H;
  CYC(0x6db1, 0x6db2); mem_wr(gb, DE, A);
  CYC(0x6db2, 0x6db4); E = 0xc4; // Part.state
  CYC(0x6db4, 0x6db6); A = 0x02;
  CYC(0x6db6, 0x6db7); mem_wr(gb, DE, A);
  CYC(0x6db7, 0x6dba); objectSetInvisible_hook(gb); return; // jp

func_6dba:
  CALL_C(0x6dba, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x6dbd);
  CYC(0x6dbd, 0x6dbf); E = 0xc9; // Part.angle
  CYC(0x6dbf, 0x6dc0); mem_wr(gb, DE, A);
  CALL_C(0x6dc0, objectApplySpeed_hook, 0x201d, 0x6dc3);
  CYC(0x6dc3, 0x6dc6); partAnimate_hook(gb); return; // jp

subid2_state2:
  CYC(0x6dc6, 0x6dc8); A = 0x21;
  CALL_C(0x6dc8, objectGetRelatedObject2Var_hook, 0x2164, 0x6dcb);
  CYC(0x6dcb, 0x6dcd); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x6dcd); return; } // ret z
  CYC(0x6dcd, 0x6dce);
  CYC(0x6dce, 0x6dd0); B = 0x05;
  CALL_C(0x6dd0, checkBPartSlotsAvailable_hook, 0x20f2, 0x6dd3);
  if (!(F & FZ)) { RET_TAKEN(0x6dd3); return; } // ret nz
  CYC(0x6dd3, 0x6dd4);
  CYC(0x6dd4, 0x6dd6); C = 0x05;
L_6dd6:
  CYC(0x6dd6, 0x6dd7); A = C;
  CYC(0x6dd7, 0x6dd8); A = alu_dec8(gb, A);
  CYC(0x6dd8, 0x6ddb); SET_HL(0x6df8); // @table_6df8
  CYC(0x6ddb, 0x6ddc); vireProjectile_addAToHl_from_rst(gb, 0x6ddc);
  CYC(0x6ddc, 0x6ddd); B = mem_rd(gb, HL);
  CYC(0x6ddd, 0x6ddf); E = 0x02;
  CYC(0x6ddf, 0x6de2); push_effect(gb, 0x6de2); vireProjectile_func_6d22_hook(gb);
  CYC(0x6de2, 0x6de3); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x6de3, 0x6de5); goto L_6dd6; } // jr nz
  CYC(0x6de3, 0x6de5);
  CYC(0x6de5, 0x6de6); H = D;
  CYC(0x6de6, 0x6de8); L = 0xc4; // Part.state
  CYC(0x6de8, 0x6de9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6de9, 0x6deb); L = 0xc9; // Part.angle
  CYC(0x6deb, 0x6ded); mem_wr(gb, HL, 0x1d);
  CALL_C(0x6ded, func_6e5d_hook, 0x6e5d, 0x6df0);
  CYC(0x6df0, 0x6df2); A = 0x01;
  CALL_C(0x6df2, partSetAnimation_hook, 0x2988, 0x6df5);
  CYC(0x6df5, 0x6df8); objectSetVisible82_hook(gb); return; // jp

subid3:
  CYC(0x6dfd, 0x6dfe); A = mem_rd(gb, DE);
  CYC(0x6dfe, 0x6dff); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6dff, 0x6e01); goto subid3_state0; } // jr z
  CYC(0x6dff, 0x6e01);
  CALL_C(0x6e01, partCommon_decCounter1IfNonzero, 0x40a7, 0x6e04);
  if (F & FZ) { CYCT(0x6e04, 0x6e07); func_6e4a_hook(gb); return; } // jp z
  CYC(0x6e04, 0x6e07);
  CYC(0x6e07, 0x6e08); L = alu_inc8(gb, L);
  CYC(0x6e08, 0x6e09); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x6e09, 0x6e0b); goto L_6e13; } // jr nz
  CYC(0x6e09, 0x6e0b);
  CYC(0x6e0b, 0x6e0d); mem_wr(gb, HL, 0x07);
  CALL_C(0x6e0d, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6e10);
  CALL_C(0x6e10, objectNudgeAngleTowards_hook, 0x1fd4, 0x6e13);
L_6e13:
  CALL_C(0x6e13, objectApplySpeed_hook, 0x201d, 0x6e16);
  CYC(0x6e16, 0x6e19); partAnimate_hook(gb); return; // jp

subid3_state0:
  CALL_C(0x6e19, func_6e50_hook, 0x6e50, 0x6e1c);
  CYC(0x6e1c, 0x6e1e); L = 0xc6; // Part.counter1
  CYC(0x6e1e, 0x6e20); mem_wr(gb, HL, 0xf0);
  CYC(0x6e20, 0x6e21); L = alu_inc8(gb, L);
  CYC(0x6e21, 0x6e23); mem_wr(gb, HL, 0x07);
  CYC(0x6e23, 0x6e25); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x6e25, 0x6e27); A = 0x02;
  CYC(0x6e27, 0x6e28); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6e28, 0x6e29); mem_wr(gb, HL, A);
  CALL_C(0x6e29, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6e2c);
  CYC(0x6e2c, 0x6e2e); E = 0xc9; // Part.angle
  CYC(0x6e2e, 0x6e2f); mem_wr(gb, DE, A);
  func_6e2f_hook(gb); // falls through
}

void func_6e2f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6e2f, 0x6e31); A = 0x29; // Object.health
  CALL_C(0x6e31, objectGetRelatedObject1Var_hook, 0x2160, 0x6e34);
  CYC(0x6e34, 0x6e35); A = mem_rd(gb, HL);
  CYC(0x6e35, 0x6e37); B = 0x1e;
  CYC(0x6e37, 0x6e39); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(0x6e39, 0x6e3b); goto L_6e43; } // jr nc
  CYC(0x6e39, 0x6e3b);
  CYC(0x6e3b, 0x6e3d); B = 0x2d;
  CYC(0x6e3d, 0x6e3f); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x6e3f, 0x6e41); goto L_6e43; } // jr nc
  CYC(0x6e3f, 0x6e41);
  CYC(0x6e41, 0x6e43); B = 0x3c;
L_6e43:
  CYC(0x6e43, 0x6e45); E = 0xd0; // Part.speed
  CYC(0x6e45, 0x6e46); A = B;
  CYC(0x6e46, 0x6e47); mem_wr(gb, DE, A);
  CYC(0x6e47, 0x6e4a); objectSetVisible80_hook(gb); return; // jp
}

void func_6e4a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6e4a, objectCreatePuff_hook, 0x24c1, 0x6e4d);
  CYC(0x6e4d, 0x6e50); partDelete_hook(gb); return; // jp
}

void func_6e50_hook(GB *gb) {
  CYC(0x6e50, 0x6e51); H = D;
  CYC(0x6e51, 0x6e52); L = E; // Part.state
  CYC(0x6e52, 0x6e53); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6e53, 0x6e55); L = 0xcf; // Part.zh
  CYC(0x6e55, 0x6e56); A = mem_rd(gb, HL);
  CYC(0x6e56, 0x6e58); mem_wr(gb, HL, 0x00);
  CYC(0x6e58, 0x6e5a); L = 0xcb; // Part.yh
  CYC(0x6e5a, 0x6e5b); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6e5b, 0x6e5c); mem_wr(gb, HL, A);
  RET(0x6e5c); return; // ret
}

void func_6e5d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6e5d, 0x6e5f); A = 0x29; // Object.health
  CALL_C(0x6e5f, objectGetRelatedObject1Var_hook, 0x2160, 0x6e62);
  CYC(0x6e62, 0x6e63); A = mem_rd(gb, HL);
  CYC(0x6e63, 0x6e65); B = 0x3c;
  CYC(0x6e65, 0x6e67); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(0x6e67, 0x6e69); goto L_6e71; } // jr nc
  CYC(0x6e67, 0x6e69);
  CYC(0x6e69, 0x6e6b); B = 0x5a;
  CYC(0x6e6b, 0x6e6d); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x6e6d, 0x6e6f); goto L_6e71; } // jr nc
  CYC(0x6e6d, 0x6e6f);
  CYC(0x6e6f, 0x6e71); B = 0x78;
L_6e71:
  CYC(0x6e71, 0x6e73); E = 0xd0; // Part.speed
  CYC(0x6e73, 0x6e74); A = B;
  CYC(0x6e74, 0x6e75); mem_wr(gb, DE, A);
  RET(0x6e75); return; // ret
}
