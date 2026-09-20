#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode4b), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode4b), (from), (to), true)

static uint16_t twinrovaProjectile_jump_table(GB *gb) {
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

// PART_RED_TWINROVA_PROJECTILE / PART_BLUE_TWINROVA_PROJECTILE
void partCode4b_hook(GB *gb) {
  BASE(partCode4b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x83); // ITEMCOLLISION_L3_SHIELD
  if (F & FZ) { CYCT(b_+7, b_+10); partDelete_hook(gb); return; } // jp z
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+12); alu_cp(gb, 0x80); // ITEMCOLLISION_LINK
  if (F & FZ) { CYCT(b_+12, b_+14); goto normalStatus; } // jr z
  CYC(b_+12, b_+14);
  CALL_C(b_+14, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+17);
  CYC(b_+17, b_+19); alu_xor(gb, 0x10);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = 0xc9; // Part.angle
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+25); L = 0xc4; // Part.state
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x03);
  CYC(b_+27, b_+29); L = 0xd0; // Part.speed
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x64); // SPEED_280

normalStatus:
  CYC(b_+31, b_+33); A = 0x04; // Object.state
  CALL_C(b_+33, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+36);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+39); alu_cp(gb, 0x0d);
  if (!(F & FC)) { CYCT(b_+39, b_+42); goto deleteWithPoof; } // jp nc
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+44); E = 0xc4; // Part.state
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  {
    CYC(b_+45, b_+46); push_effect(gb, b_+46);
    uint16_t target = twinrovaProjectile_jump_table(gb);
    if (target == b_+54) goto state0;
    if (target == b_+103) goto state1;
    if (target == b_+137) goto state2;
    goto state3;
  }

state0:
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+56); L = E;
  CYC(b_+56, b_+57); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+57, b_+59); L = 0xc6; // Part.counter1
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x1e);
  CYC(b_+61, b_+63); L = 0xd0; // Part.speed
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+65, b_+67); L = 0xcf; // Part.zh
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x00);
  CYC(b_+70, b_+72); L = 0xcb; // Part.yh
  CYC(b_+72, b_+73); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+73, b_+74); mem_wr(gb, HL, A);
  CYC(b_+74, b_+76); A = 0x16; // Object.relatedObj1
  CALL_C(b_+76, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+79);
  CYC(b_+79, b_+81); E = 0xd8; // Part.relatedObj2
  CYC(b_+81, b_+82); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+82, b_+83); mem_wr(gb, DE, A);
  CYC(b_+83, b_+84); E = alu_inc8(gb, E);
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+88); E = 0xc1; // Part.id
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+91); alu_cp(gb, 0x4b); // PART_RED_TWINROVA_PROJECTILE
  CYC(b_+91, b_+93); A = 0xba; // SND_BEAM1
  if (F & FZ) { CYCT(b_+93, b_+95); goto playChargeSound; } // jr z
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+97); A = 0xbb; // SND_BEAM2

playChargeSound:
  CALL_C(b_+97, playSound_b00_hook, SYM(playSound_b00), b_+100);
  CALL_C(b_+100, objectSetVisible81_hook, SYM(objectSetVisible81), b_+103);

// Being charged up
state1:
  CALL_C(b_+103, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+106);
  if (F & FZ) { CYCT(b_+106, b_+108); goto fire; } // jr z
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+110); A = 0x0b; // Object.yh
  CALL_C(b_+110, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+113);
  CYC(b_+113, b_+116); SET_BC(0xea00);
  CALL_C(b_+116, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+119);
  CYC(b_+119, b_+120); alu_xor(gb, A);
  CYC(b_+120, b_+121); mem_wr(gb, DE, A); // Part.zh = 0
  CYC(b_+121, b_+123); goto animate; // jr

fire:
  CALL_C(b_+123, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+126);
  CYC(b_+126, b_+128); E = 0xc9; // Part.angle
  CYC(b_+128, b_+129); mem_wr(gb, DE, A);
  CYC(b_+129, b_+130); H = D;
  CYC(b_+130, b_+132); L = 0xc4; // Part.state
  CYC(b_+132, b_+133); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+133, b_+135); L = 0xe4; // Part.collisionType
  CYC(b_+135, b_+137); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);

// Moving
state2:
  CALL_C(b_+137, objectApplySpeed_hook, SYM(objectApplySpeed), b_+140);
  CALL_C(b_+140, partCommon_checkOutOfBounds_hook, SYM(partCommon_checkOutOfBounds), b_+143);
  if (F & FZ) { CYCT(b_+143, b_+145); goto delete; } // jr z
  CYC(b_+143, b_+145);

animate:
  CYC(b_+145, b_+148); partAnimate_hook(gb); return; // jp

state3:
  CYC(b_+148, b_+150); A = 0x00;
  CALL_C(b_+150, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+153);
  CALL_C(b_+153, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+156);
  if (!(F & FC)) { CYCT(b_+156, b_+158); goto state2; } // jr nc
  CYC(b_+156, b_+158);
  CYC(b_+158, b_+160); L = 0xab; // Enemy.invincibilityCounter
  CYC(b_+160, b_+162); mem_wr(gb, HL, 0x14);
  CYC(b_+162, b_+164); L = 0xa9; // Enemy.health
  CYC(b_+164, b_+165); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+165, b_+167); goto decOtherHealth; } // jr nz
  CYC(b_+165, b_+167);
  CYC(b_+167, b_+169); L = 0xb2; // Enemy.var32
  CYC(b_+169, b_+171); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);

decOtherHealth:
  CYC(b_+171, b_+173); A = 0x29; // Object.health
  CALL_C(b_+173, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+176);
  CYC(b_+176, b_+177); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+177, b_+179); A = 0x63; // SND_BOSS_DAMAGE
  CALL_C(b_+179, playSound_b00_hook, SYM(playSound_b00), b_+182);

delete:
  CYC(b_+182, b_+185); partDelete_hook(gb); return; // jp

deleteWithPoof:
  CALL_C(b_+185, objectCreatePuff_hook, SYM(objectCreatePuff), b_+188);
  CYC(b_+188, SYM(partCode4c)); partDelete_hook(gb); return; // jp
}
