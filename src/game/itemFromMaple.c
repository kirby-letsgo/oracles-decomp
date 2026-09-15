#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t itemFromMaple_jump_table(GB *gb) {
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

static void itemFromMaple_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void itemFromMaple_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

/* PARTSTATUS_JUST_HIT is unreachable via the top-level cp/jr since @normalStatus is fallen
   through immediately below; both partCode14 and partCode15 alias the same code. */

static void itemFromMaple_setOamData(GB *gb) {
  CYC(0x4ec0, 0x4ec2); E = 0xc2; // Part.subid
  CYC(0x4ec2, 0x4ec3); A = mem_rd(gb, DE);
  CYC(0x4ec3, 0x4ec4); C = A;
  CYC(0x4ec4, 0x4ec5); alu_add(gb, A);
  CYC(0x4ec5, 0x4ec6); alu_add(gb, C);
  CYC(0x4ec6, 0x4ec9); SET_HL(0x4ed9);
  CYC(0x4ec9, 0x4eca); itemFromMaple_addAToHl_from_rst(gb, 0x4eca);
  CYC(0x4eca, 0x4ecc); E = 0xdd; // Part.oamTileIndexBase
  CYC(0x4ecc, 0x4ecd); A = mem_rd(gb, DE);
  CYC(0x4ecd, 0x4ece); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4ece, 0x4ecf); mem_wr(gb, DE, A);
  CYC(0x4ecf, 0x4ed0); SET_HL(HL + 1);
  CYC(0x4ed0, 0x4ed1); E = alu_dec8(gb, E);
  CYC(0x4ed1, 0x4ed2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4ed2, 0x4ed3); mem_wr(gb, DE, A);
  CYC(0x4ed3, 0x4ed4); E = alu_dec8(gb, E);
  CYC(0x4ed4, 0x4ed5); mem_wr(gb, DE, A);
  CYC(0x4ed5, 0x4ed6); A = mem_rd(gb, HL);
  CYC(0x4ed6, 0x4ed9); partSetAnimation_hook(gb); return; // jp
}

static void itemFromMaple_setDroppedItemPosition(GB *gb) {
  CYC(0x4f03, 0x4f04); H = D;
  CYC(0x4f04, 0x4f06); L = 0xcb; // Part.yh
  CYC(0x4f06, 0x4f07); A = mem_rd(gb, HL);
  CYC(0x4f07, 0x4f09); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(0x4f09, 0x4f0b); goto L_4f0c; } // jr c
  CYC(0x4f09, 0x4f0b);
  CYC(0x4f0b, 0x4f0c); alu_xor(gb, A);
L_4f0c:
  CYC(0x4f0c, 0x4f0e); alu_cp(gb, 0x20);
  if (!(F & FC)) { CYCT(0x4f0e, 0x4f10); goto L_4f14; } // jr nc
  CYC(0x4f0e, 0x4f10);
  CYC(0x4f10, 0x4f12); mem_wr(gb, HL, 0x20);
  CYC(0x4f12, 0x4f14); goto L_4f1a; // jr
L_4f14:
  CYC(0x4f14, 0x4f16); alu_cp(gb, 0x78);
  if (F & FC) { CYCT(0x4f16, 0x4f18); goto L_4f1a; } // jr c
  CYC(0x4f16, 0x4f18);
  CYC(0x4f18, 0x4f1a); mem_wr(gb, HL, 0x78);
L_4f1a:
  CYC(0x4f1a, 0x4f1c); L = 0xcd; // Part.xh
  CYC(0x4f1c, 0x4f1d); A = mem_rd(gb, HL);
  CYC(0x4f1d, 0x4f1f); alu_cp(gb, 0xf0);
  if (F & FC) { CYCT(0x4f1f, 0x4f21); goto L_4f22; } // jr c
  CYC(0x4f1f, 0x4f21);
  CYC(0x4f21, 0x4f22); alu_xor(gb, A);
L_4f22:
  CYC(0x4f22, 0x4f24); alu_cp(gb, 0x08);
  if (!(F & FC)) { CYCT(0x4f24, 0x4f26); goto L_4f29; } // jr nc
  CYC(0x4f24, 0x4f26);
  CYC(0x4f26, 0x4f28); mem_wr(gb, HL, 0x08);
  RET(0x4f28); return; // ret
L_4f29:
  CYC(0x4f29, 0x4f2b); alu_cp(gb, 0x98);
  if (F & FC) { RET_TAKEN(0x4f2b); return; } // ret c
  CYC(0x4f2b, 0x4f2c);
  CYC(0x4f2c, 0x4f2e); mem_wr(gb, HL, 0x98);
  RET(0x4f2e); return; // ret
}

void itemFromMaple_moveToMaple_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f2f, 0x4f31); L = 0x0b; // <w1Companion.yh
  CYC(0x4f31, 0x4f32); B = mem_rd(gb, HL);
  CYC(0x4f32, 0x4f34); L = 0x0d; // <w1Companion.xh
  CYC(0x4f34, 0x4f35); C = mem_rd(gb, HL);
  CYC(0x4f35, 0x4f36); push_effect(gb, BC);
  CALL_C(0x4f36, objectGetRelativeAngle_hook, 0x1ea4, 0x4f39);
  CYC(0x4f39, 0x4f3b); E = 0xc9; // Part.angle
  CYC(0x4f3b, 0x4f3c); mem_wr(gb, DE, A);
  CALL_C(0x4f3c, objectApplySpeed_hook, 0x201d, 0x4f3f);
  CYC(0x4f3f, 0x4f40); SET_BC(pop_effect(gb));
  CYC(0x4f40, 0x4f41); H = D;
  CYC(0x4f41, 0x4f43); L = 0xcb; // Part.yh
  CYC(0x4f43, 0x4f44); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4f44, 0x4f45); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(0x4f45); return; } // ret nz
  CYC(0x4f45, 0x4f46);
  CYC(0x4f46, 0x4f47); L = alu_inc8(gb, L);
  CYC(0x4f47, 0x4f48); A = mem_rd(gb, HL);
  CYC(0x4f48, 0x4f49); alu_cp(gb, C);
  RET(0x4f49); return; // ret
}

void partCode14_hook(GB *gb);

void partCode14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4d13, 0x4d15); E = 0xc2; // Part.subid
  if (F & FZ) { CYCT(0x4d15, 0x4d17); goto normalStatus; } // jr z
  CYC(0x4d15, 0x4d17);
  CYC(0x4d17, 0x4d19); alu_cp(gb, 0x02); // PARTSTATUS_DEAD
  if (F & FZ) { CYCT(0x4d19, 0x4d1c); goto linkCollectedItem; } // jp z
  CYC(0x4d19, 0x4d1c);
  CYC(0x4d1c, 0x4d1d); H = D;
  CYC(0x4d1d, 0x4d1f); L = 0xc2; // Part.subid
  CYC(0x4d1f, 0x4d21); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x4d21, 0x4d23); L = 0xc4; // Part.state
  CYC(0x4d23, 0x4d25); mem_wr(gb, HL, 0x03);
  CYC(0x4d25, 0x4d26); L = alu_inc8(gb, L);
  CYC(0x4d26, 0x4d28); mem_wr(gb, HL, 0x00);

normalStatus:
  CYC(0x4d28, 0x4d2a); E = 0xc4; // Part.state
  CYC(0x4d2a, 0x4d2b); A = mem_rd(gb, DE);
  {
    CYC(0x4d2b, 0x4d2c); push_effect(gb, 0x4d2c);
    uint16_t target = itemFromMaple_jump_table(gb);
    if (target == 0x4d36) goto state0;
    if (target == 0x4d83) goto state1;
    if (target == 0x4d9b) goto state3;
    if (target == 0x4dcc) goto state4;
    objectReplaceWithAnimationIfOnHazard_hook(gb); return;
  }

state0:
  CYC(0x4d36, 0x4d37); H = D;
  CYC(0x4d37, 0x4d38); L = E; // Part.state
  CYC(0x4d38, 0x4d39); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4d39, 0x4d3b); L = 0xe6; // Part.collisionRadiusY
  CYC(0x4d3b, 0x4d3d); A = 0x06;
  CYC(0x4d3d, 0x4d3e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4d3e, 0x4d3f); mem_wr(gb, HL, A);
  CALL_C(0x4d3f, getRandomNumber_hook, 0x043e, 0x4d42);
  CYC(0x4d42, 0x4d43); B = A;
  CYC(0x4d43, 0x4d45); alu_and(gb, 0x70);
  CYC(0x4d45, 0x4d47); A = alu_swap(gb, A);
  CYC(0x4d47, 0x4d4a); SET_HL(0x4d6b); // @speedValues
  CYC(0x4d4a, 0x4d4b); itemFromMaple_addAToHl_from_rst(gb, 0x4d4b);
  CYC(0x4d4b, 0x4d4d); E = 0xd0; // Part.speed
  CYC(0x4d4d, 0x4d4e); A = mem_rd(gb, HL);
  CYC(0x4d4e, 0x4d4f); mem_wr(gb, DE, A);
  CYC(0x4d4f, 0x4d50); A = B;
  CYC(0x4d50, 0x4d52); alu_and(gb, 0x0e);
  CYC(0x4d52, 0x4d55); SET_HL(0x4d73); // @speedZValues
  CYC(0x4d55, 0x4d56); itemFromMaple_addAToHl_from_rst(gb, 0x4d56);
  CYC(0x4d56, 0x4d58); E = 0xd4; // Part.speedZ
  CYC(0x4d58, 0x4d59); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4d59, 0x4d5a); mem_wr(gb, DE, A);
  CYC(0x4d5a, 0x4d5b); E = alu_inc8(gb, E);
  CYC(0x4d5b, 0x4d5c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4d5c, 0x4d5d); mem_wr(gb, DE, A);
  CALL_C(0x4d5d, getRandomNumber_hook, 0x043e, 0x4d60);
  CYC(0x4d60, 0x4d62); E = 0xc9; // Part.angle
  CYC(0x4d62, 0x4d64); alu_and(gb, 0x1f);
  CYC(0x4d64, 0x4d65); mem_wr(gb, DE, A);
  CYC(0x4d65, 0x4d68); push_effect(gb, 0x4d68); itemFromMaple_setOamData(gb);
  CYC(0x4d68, 0x4d6b); objectSetVisiblec3_hook(gb); return; // jp

state1:
  CALL_C(0x4d83, objectApplySpeed_hook, 0x201d, 0x4d86);
  CYC(0x4d86, 0x4d89); push_effect(gb, 0x4d89); itemFromMaple_setDroppedItemPosition(gb);
  CYC(0x4d89, 0x4d8b); C = 0x20;
  CALL_C(0x4d8b, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x4d8e);
  if (!(F & FC)) { CYCT(0x4d8e, 0x4d90); goto L_4d98; } // jr nc
  CYC(0x4d8e, 0x4d90);
  CYC(0x4d90, 0x4d91); H = D;
  CYC(0x4d91, 0x4d93); L = 0xe4; // Part.collisionType
  CYC(0x4d93, 0x4d95); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x4d95, 0x4d97); L = 0xc4; // Part.state
  CYC(0x4d97, 0x4d98); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_4d98:
  CYC(0x4d98, 0x4d9b); objectReplaceWithAnimationIfOnHazard_hook(gb); return; // jp

state3:
  CYC(0x4d9b, 0x4d9c); E = alu_inc8(gb, E); // Part.substate
  CYC(0x4d9c, 0x4d9d); A = mem_rd(gb, DE);
  CYC(0x4d9d, 0x4d9e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4d9e, 0x4da0); goto L_4db3; } // jr nz
  CYC(0x4d9e, 0x4da0);
  CYC(0x4da0, 0x4da1); H = D;
  CYC(0x4da1, 0x4da2); L = E; // Part.substate
  CYC(0x4da2, 0x4da3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4da3, 0x4da5); L = 0xcf; // Part.zh
  CYC(0x4da5, 0x4da7); mem_wr(gb, HL, 0x00);
  CYC(0x4da7, 0x4da9); A = 0x01; // Object.id
  CALL_C(0x4da9, objectGetRelatedObject1Var_hook, 0x2160, 0x4dac);
  CYC(0x4dac, 0x4dad); A = mem_rd(gb, HL);
  CYC(0x4dad, 0x4daf); E = 0xf0; // Part.var30
  CYC(0x4daf, 0x4db0); mem_wr(gb, DE, A);
  CALL_C(0x4db0, objectSetVisible80_hook, 0x1e57, 0x4db3);
L_4db3:
  CALL_C(0x4db3, objectCheckCollidedWithLink_hook, 0x1c41, 0x4db6);
  if (F & FC) { CYCT(0x4db6, 0x4db9); goto linkCollectedItem; } // jp c
  CYC(0x4db6, 0x4db9);
  CYC(0x4db9, 0x4dbb); A = 0x00; // Object.enabled
  CALL_C(0x4dbb, objectGetRelatedObject1Var_hook, 0x2160, 0x4dbe);
  CYC(0x4dbe, 0x4dbf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4dbf, 0x4dc0); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4dc0, 0x4dc2); goto L_4dc9; } // jr z
  CYC(0x4dc0, 0x4dc2);
  CYC(0x4dc2, 0x4dc4); E = 0xf0; // Part.var30
  CYC(0x4dc4, 0x4dc5); A = mem_rd(gb, DE);
  CYC(0x4dc5, 0x4dc6); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x4dc6, 0x4dc9); objectTakePosition_hook(gb); return; } // jp z
  CYC(0x4dc6, 0x4dc9);
L_4dc9:
  CYC(0x4dc9, 0x4dcc); partDelete_hook(gb); return; // jp

state4:
  CYC(0x4dcc, 0x4dcd); E = alu_inc8(gb, E); // Part.substate
  CYC(0x4dcd, 0x4dce); A = mem_rd(gb, DE);
  {
    CYC(0x4dce, 0x4dcf); push_effect(gb, 0x4dcf);
    uint16_t target = itemFromMaple_jump_table(gb);
    if (target == 0x4dd7) goto substate0;
    if (target == 0x4de6) goto substate1;
    if (target == 0x4dfe) goto substate2;
    goto substate3;
  }

substate0:
  CYC(0x4dd7, 0x4dd8); H = D;
  CYC(0x4dd8, 0x4dd9); L = E; // Part.substate
  CYC(0x4dd9, 0x4dda); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4dda, 0x4ddd); A = mem_rd(gb, 0xd128); // w1Companion.damage
  CYC(0x4ddd, 0x4dde); A = alu_dec8(gb, A);
  CYC(0x4dde, 0x4de0); L = 0xd0; // Part.speed
  CYC(0x4de0, 0x4de2); mem_wr(gb, HL, 0x14); // SPEED_80
  if (F & FZ) { CYCT(0x4de2, 0x4de4); goto substate1; } // jr z
  CYC(0x4de2, 0x4de4);
  CYC(0x4de4, 0x4de6); mem_wr(gb, HL, 0x28); // SPEED_100

substate1:
  CYC(0x4de6, 0x4de9); SET_HL(0xd128); // w1Companion.damage
  CYC(0x4de9, 0x4dea); A = mem_rd(gb, HL);
  CYC(0x4dea, 0x4deb); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4deb, 0x4ded); goto L_4e09; } // jr z
  CYC(0x4deb, 0x4ded);
  CYC(0x4ded, 0x4df0); push_effect(gb, 0x4df0); itemFromMaple_moveToMaple_hook(gb);
  if (!(F & FZ)) { RET_TAKEN(0x4df0); return; } // ret nz
  CYC(0x4df0, 0x4df1);
  CYC(0x4df1, 0x4df3); L = 0xc5; // Part.substate
  CYC(0x4df3, 0x4df4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4df4, 0x4df6); L = 0xe4; // Part.collisionType
  CYC(0x4df6, 0x4df8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x4df8, 0x4dfb); SET_BC(0xffc0);
  CYC(0x4dfb, 0x4dfe); objectSetSpeedZ_hook(gb); return; // jp

substate2:
  CYC(0x4dfe, 0x4e00); C = 0x00;
  CALL_C(0x4e00, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4e03);
  CYC(0x4e03, 0x4e05); E = 0xcf; // Part.zh
  CYC(0x4e05, 0x4e06); A = mem_rd(gb, DE);
  CYC(0x4e06, 0x4e08); alu_cp(gb, 0xf7);
  if (!(F & FC)) { RET_TAKEN(0x4e08); return; } // ret nc
  CYC(0x4e08, 0x4e09);
L_4e09:
  CYC(0x4e09, 0x4e0b); A = 0x01;
  CYC(0x4e0b, 0x4e0e); mem_wr(gb, 0xd125, A); // w1Companion.damageToApply
  CYC(0x4e0e, 0x4e0f); H = D;
  CYC(0x4e0f, 0x4e11); L = 0xc5; // Part.substate
  CYC(0x4e11, 0x4e13); mem_wr(gb, HL, 0x03);
  CYC(0x4e13, 0x4e15); L = 0xc3; // Part.var03
  CYC(0x4e15, 0x4e17); mem_wr(gb, HL, 0x00);
  RET(0x4e17); return; // ret

substate3:
  CYC(0x4e18, 0x4e1a); E = 0xc3; // Part.var03
  CYC(0x4e1a, 0x4e1b); A = mem_rd(gb, DE);
  CYC(0x4e1b, 0x4e1c); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(0x4e1c); return; } // ret nc
  CYC(0x4e1c, 0x4e1d);
  CYC(0x4e1d, 0x4e20); partDelete_hook(gb); return; // jp

linkCollectedItem:
  CYC(0x4e20, 0x4e23); A = mem_rd(gb, 0xcc8a); // wDisabledObjects
  CYC(0x4e23, 0x4e25); alu_bit(gb, 0, A);
  if (!(F & FZ)) { RET_TAKEN(0x4e25); return; } // ret nz
  CYC(0x4e25, 0x4e26);
  CYC(0x4e26, 0x4e28); E = 0xc2; // Part.subid
  CYC(0x4e28, 0x4e29); A = mem_rd(gb, DE);
  CYC(0x4e29, 0x4e2b); alu_and(gb, 0x7f);
  CYC(0x4e2b, 0x4e2e); SET_HL(0x4f4a); // @obtainedValue
  CYC(0x4e2e, 0x4e2f); itemFromMaple_addAToHl_from_rst(gb, 0x4e2f);
  CYC(0x4e2f, 0x4e32); A = mem_rd(gb, 0xd12a); // w1Companion.var2a
  CYC(0x4e32, 0x4e33); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4e33, 0x4e36); mem_wr(gb, 0xd12a, A); // w1Companion.var2a
  CYC(0x4e36, 0x4e37); A = mem_rd(gb, DE);
  CYC(0x4e37, 0x4e39); alu_and(gb, 0x7f);
  if (F & FZ) { CYCT(0x4e39, 0x4e3b); goto func_4e6e; } // jr z
  CYC(0x4e39, 0x4e3b);
  CYC(0x4e3b, 0x4e3c); alu_add(gb, A);
  CYC(0x4e3c, 0x4e3f); SET_HL(0x4e88); // @itemDropTreasureTable
  CYC(0x4e3f, 0x4e40); itemFromMaple_addDoubleIndexToHl_from_rst(gb, 0x4e40);
  CYC(0x4e40, 0x4e41); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4e41, 0x4e42); B = A;
  CYC(0x4e42, 0x4e44); A = 0x26; // GOLD_JOY_RING
  CALL_C(0x4e44, cpActiveRing_hook, 0x23b0, 0x4e47);
  CYC(0x4e47, 0x4e48); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (F & FZ) { CYCT(0x4e48, 0x4e4a); goto L_4e53; } // jr z
  CYC(0x4e48, 0x4e4a);
  CYC(0x4e4a, 0x4e4c); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x4e4c, 0x4e4e); goto L_4e54; } // jr z
  CYC(0x4e4c, 0x4e4e);
  CALL_C(0x4e4e, cpActiveRing_hook, 0x23b0, 0x4e51);
  if (!(F & FZ)) { CYCT(0x4e51, 0x4e53); goto L_4e54; } // jr nz
  CYC(0x4e51, 0x4e53);
L_4e53:
  CYC(0x4e53, 0x4e54); SET_HL(HL + 1);
L_4e54:
  CYC(0x4e54, 0x4e55); C = mem_rd(gb, HL);
  CYC(0x4e55, 0x4e56); A = B;
  CYC(0x4e56, 0x4e58); alu_cp(gb, 0x2d); // TREASURE_RING
  if (!(F & FZ)) { CYCT(0x4e58, 0x4e5a); goto L_4e5d; } // jr nz
  CYC(0x4e58, 0x4e5a);
  CALL_C(0x4e5a, getRandomRingOfGivenTier_hook, 0x17e0, 0x4e5d);
L_4e5d:
  CYC(0x4e5d, 0x4e5f); alu_cp(gb, 0x2f); // TREASURE_POTION
  if (!(F & FZ)) { CYCT(0x4e5f, 0x4e61); goto L_4e68; } // jr nz
  CYC(0x4e5f, 0x4e61);
  CYC(0x4e61, 0x4e63); A = 0x5e; // SND_GETSEED
  CALL_C(0x4e63, playSound_b00_hook, 0x0c98, 0x4e66);
  CYC(0x4e66, 0x4e68); A = 0x2f; // TREASURE_POTION
L_4e68:
  CALL_C(0x4e68, giveTreasure_hook, 0x171c, 0x4e6b);
  CYC(0x4e6b, 0x4e6e); partDelete_hook(gb); return; // jp

func_4e6e:
  CYC(0x4e6e, 0x4e71); SET_BC(0x2b02); // TREASURE_HEART_PIECE, $02
  CALL_C(0x4e71, createTreasure_hook, 0x27d4, 0x4e74);
  if (!(F & FZ)) { RET_TAKEN(0x4e74); return; } // ret nz
  CYC(0x4e74, 0x4e75);
  CYC(0x4e75, 0x4e77); L = 0x4b; // Interaction.yh
  CYC(0x4e77, 0x4e7a); A = mem_rd(gb, 0xd00b); // w1Link.yh
  CYC(0x4e7a, 0x4e7b); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4e7b, 0x4e7c); L = alu_inc8(gb, L);
  CYC(0x4e7c, 0x4e7f); A = mem_rd(gb, 0xd00d); // w1Link.xh
  CYC(0x4e7f, 0x4e80); mem_wr(gb, HL, A);
  CYC(0x4e80, 0x4e83); SET_HL(0xc644); // wMapleState
  CYC(0x4e83, 0x4e85); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x4e85, 0x4e88); partDelete_hook(gb); return; // jp
}
