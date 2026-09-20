#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x592f, 0x5931); goto normalStatus; } // jr z
  CYC(0x592f, 0x5931);
  CYC(0x5931, 0x5933); E = 0xea; // Part.var2a
  CYC(0x5933, 0x5934); A = mem_rd(gb, DE);
  CYC(0x5934, 0x5936); alu_cp(gb, 0x83); // ITEMCOLLISION_L3_SHIELD
  if (F & FZ) { CYCT(0x5936, 0x5939); partDelete_hook(gb); return; } // jp z
  CYC(0x5936, 0x5939);
  CYC(0x5939, 0x593b); alu_cp(gb, 0x80); // ITEMCOLLISION_LINK
  if (F & FZ) { CYCT(0x593b, 0x593d); goto normalStatus; } // jr z
  CYC(0x593b, 0x593d);
  CALL_C(0x593d, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x5940);
  CYC(0x5940, 0x5942); alu_xor(gb, 0x10);
  CYC(0x5942, 0x5943); H = D;
  CYC(0x5943, 0x5945); L = 0xc9; // Part.angle
  CYC(0x5945, 0x5946); mem_wr(gb, HL, A);
  CYC(0x5946, 0x5948); L = 0xc4; // Part.state
  CYC(0x5948, 0x594a); mem_wr(gb, HL, 0x03);
  CYC(0x594a, 0x594c); L = 0xd0; // Part.speed
  CYC(0x594c, 0x594e); mem_wr(gb, HL, 0x64); // SPEED_280

normalStatus:
  CYC(0x594e, 0x5950); A = 0x04; // Object.state
  CALL_C(0x5950, objectGetRelatedObject1Var_hook, 0x2160, 0x5953);
  CYC(0x5953, 0x5954); A = mem_rd(gb, HL);
  CYC(0x5954, 0x5956); alu_cp(gb, 0x0d);
  if (!(F & FC)) { CYCT(0x5956, 0x5959); goto deleteWithPoof; } // jp nc
  CYC(0x5956, 0x5959);
  CYC(0x5959, 0x595b); E = 0xc4; // Part.state
  CYC(0x595b, 0x595c); A = mem_rd(gb, DE);
  {
    CYC(0x595c, 0x595d); push_effect(gb, 0x595d);
    uint16_t target = twinrovaProjectile_jump_table(gb);
    if (target == 0x5965) goto state0;
    if (target == 0x5996) goto state1;
    if (target == 0x59b8) goto state2;
    goto state3;
  }

state0:
  CYC(0x5965, 0x5966); H = D;
  CYC(0x5966, 0x5967); L = E;
  CYC(0x5967, 0x5968); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5968, 0x596a); L = 0xc6; // Part.counter1
  CYC(0x596a, 0x596c); mem_wr(gb, HL, 0x1e);
  CYC(0x596c, 0x596e); L = 0xd0; // Part.speed
  CYC(0x596e, 0x5970); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x5970, 0x5972); L = 0xcf; // Part.zh
  CYC(0x5972, 0x5973); A = mem_rd(gb, HL);
  CYC(0x5973, 0x5975); mem_wr(gb, HL, 0x00);
  CYC(0x5975, 0x5977); L = 0xcb; // Part.yh
  CYC(0x5977, 0x5978); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5978, 0x5979); mem_wr(gb, HL, A);
  CYC(0x5979, 0x597b); A = 0x16; // Object.relatedObj1
  CALL_C(0x597b, objectGetRelatedObject1Var_hook, 0x2160, 0x597e);
  CYC(0x597e, 0x5980); E = 0xd8; // Part.relatedObj2
  CYC(0x5980, 0x5981); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5981, 0x5982); mem_wr(gb, DE, A);
  CYC(0x5982, 0x5983); E = alu_inc8(gb, E);
  CYC(0x5983, 0x5984); A = mem_rd(gb, HL);
  CYC(0x5984, 0x5985); mem_wr(gb, DE, A);
  CYC(0x5985, 0x5987); E = 0xc1; // Part.id
  CYC(0x5987, 0x5988); A = mem_rd(gb, DE);
  CYC(0x5988, 0x598a); alu_cp(gb, 0x4b); // PART_RED_TWINROVA_PROJECTILE
  CYC(0x598a, 0x598c); A = 0xba; // SND_BEAM1
  if (F & FZ) { CYCT(0x598c, 0x598e); goto playChargeSound; } // jr z
  CYC(0x598c, 0x598e);
  CYC(0x598e, 0x5990); A = 0xbb; // SND_BEAM2

playChargeSound:
  CALL_C(0x5990, playSound_b00_hook, 0x0c98, 0x5993);
  CALL_C(0x5993, objectSetVisible81_hook, 0x1e60, 0x5996);

// Being charged up
state1:
  CALL_C(0x5996, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5999);
  if (F & FZ) { CYCT(0x5999, 0x599b); goto fire; } // jr z
  CYC(0x5999, 0x599b);
  CYC(0x599b, 0x599d); A = 0x0b; // Object.yh
  CALL_C(0x599d, objectGetRelatedObject1Var_hook, 0x2160, 0x59a0);
  CYC(0x59a0, 0x59a3); SET_BC(0xea00);
  CALL_C(0x59a3, objectTakePositionWithOffset_hook, 0x2277, 0x59a6);
  CYC(0x59a6, 0x59a7); alu_xor(gb, A);
  CYC(0x59a7, 0x59a8); mem_wr(gb, DE, A); // Part.zh = 0
  CYC(0x59a8, 0x59aa); goto animate; // jr

fire:
  CALL_C(0x59aa, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x59ad);
  CYC(0x59ad, 0x59af); E = 0xc9; // Part.angle
  CYC(0x59af, 0x59b0); mem_wr(gb, DE, A);
  CYC(0x59b0, 0x59b1); H = D;
  CYC(0x59b1, 0x59b3); L = 0xc4; // Part.state
  CYC(0x59b3, 0x59b4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x59b4, 0x59b6); L = 0xe4; // Part.collisionType
  CYC(0x59b6, 0x59b8); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);

// Moving
state2:
  CALL_C(0x59b8, objectApplySpeed_hook, 0x201d, 0x59bb);
  CALL_C(0x59bb, partCommon_checkOutOfBounds_hook, 0x407e, 0x59be);
  if (F & FZ) { CYCT(0x59be, 0x59c0); goto delete; } // jr z
  CYC(0x59be, 0x59c0);

animate:
  CYC(0x59c0, 0x59c3); partAnimate_hook(gb); return; // jp

state3:
  CYC(0x59c3, 0x59c5); A = 0x00;
  CALL_C(0x59c5, objectGetRelatedObject2Var_hook, 0x2164, 0x59c8);
  CALL_C(0x59c8, checkObjectsCollided_hook, 0x1d5a, 0x59cb);
  if (!(F & FC)) { CYCT(0x59cb, 0x59cd); goto state2; } // jr nc
  CYC(0x59cb, 0x59cd);
  CYC(0x59cd, 0x59cf); L = 0xab; // Enemy.invincibilityCounter
  CYC(0x59cf, 0x59d1); mem_wr(gb, HL, 0x14);
  CYC(0x59d1, 0x59d3); L = 0xa9; // Enemy.health
  CYC(0x59d3, 0x59d4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x59d4, 0x59d6); goto decOtherHealth; } // jr nz
  CYC(0x59d4, 0x59d6);
  CYC(0x59d6, 0x59d8); L = 0xb2; // Enemy.var32
  CYC(0x59d8, 0x59da); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);

decOtherHealth:
  CYC(0x59da, 0x59dc); A = 0x29; // Object.health
  CALL_C(0x59dc, objectGetRelatedObject1Var_hook, 0x2160, 0x59df);
  CYC(0x59df, 0x59e0); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x59e0, 0x59e2); A = 0x63; // SND_BOSS_DAMAGE
  CALL_C(0x59e2, playSound_b00_hook, 0x0c98, 0x59e5);

delete:
  CYC(0x59e5, 0x59e8); partDelete_hook(gb); return; // jp

deleteWithPoof:
  CALL_C(0x59e8, objectCreatePuff_hook, 0x24c1, 0x59eb);
  CYC(0x59eb, 0x59ee); partDelete_hook(gb); return; // jp
}
