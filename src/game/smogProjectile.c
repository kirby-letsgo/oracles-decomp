#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t smogProjectile_jump_table(GB *gb) {
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

// PART_SMOG_PROJECTILE
void partCode4a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x79e5, 0x79e7); E = 0xc4; // Part.state
  CYC(0x79e7, 0x79e8); A = mem_rd(gb, DE);
  {
    CYC(0x79e8, 0x79e9); push_effect(gb, 0x79e9);
    uint16_t target = smogProjectile_jump_table(gb);
    if (target == 0x7a21) goto state1;
    if (target == 0x7a54) goto state2;
    goto state0;
  }

state0:
  CYC(0x79ef, 0x79f1); A = 0x01;
  CYC(0x79f1, 0x79f2); mem_wr(gb, DE, A); // Part.state
  CALL_C(0x79f2, objectSetVisible81_hook, 0x1e60, 0x79f5);
  CALL_C(0x79f5, objectGetAngleTowardLink_hook, 0x1e9c, 0x79f8);
  CYC(0x79f8, 0x79fa); E = 0xc9; // Part.angle
  CYC(0x79fa, 0x79fb); mem_wr(gb, DE, A);
  CYC(0x79fb, 0x79fc); C = A;
  CYC(0x79fc, 0x79fe); A = 0x1e; // SPEED_c0
  CYC(0x79fe, 0x7a00); E = 0xd0; // Part.speed
  CYC(0x7a00, 0x7a01); mem_wr(gb, DE, A);
  CYC(0x7a01, 0x7a03); E = 0xc2; // Part.subid
  CYC(0x7a03, 0x7a04); A = mem_rd(gb, DE);
  CYC(0x7a04, 0x7a05); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7a05, 0x7a07); goto setAnimation; } // jr z
  CYC(0x7a05, 0x7a07);
  CYC(0x7a07, 0x7a09); A = 0x28; // SPEED_100
  CYC(0x7a09, 0x7a0b); E = 0xd0; // Part.speed
  CYC(0x7a0b, 0x7a0c); mem_wr(gb, DE, A);
  CYC(0x7a0c, 0x7a0e); A = 0x05;
  CYC(0x7a0e, 0x7a10); E = 0xdc; // Part.oamFlags
  CYC(0x7a10, 0x7a11); mem_wr(gb, DE, A);
  CYC(0x7a11, 0x7a13); E = 0xe5; // Part.enemyCollisionMode
  CYC(0x7a13, 0x7a15); A = 0x04; // ENEMYCOLLISION_PODOBOO
  CYC(0x7a15, 0x7a16); mem_wr(gb, DE, A);
  CYC(0x7a16, 0x7a17); A = C;
  CALL_C(0x7a17, convertAngleToDirection_hook, 0x26f9, 0x7a1a);
  CYC(0x7a1a, 0x7a1c); alu_and(gb, 0x01);
  CYC(0x7a1c, 0x7a1e); alu_add(gb, 0x02);

setAnimation:
  CALL_C(0x7a1e, partSetAnimation_hook, 0x2988, 0x7a21);

state1:
  CALL_C(0x7a21, getThisRoomFlags_hook, 0x197d, 0x7a24);
  CYC(0x7a24, 0x7a26); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x7a26, 0x7a28); goto delete; } // jr nz
  CYC(0x7a26, 0x7a28);
  CYC(0x7a28, 0x7a2b); A = mem_rd(gb, wNumEnemies);
  CYC(0x7a2b, 0x7a2c); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x7a2c, 0x7a2e); goto delete; } // jr z
  CYC(0x7a2c, 0x7a2e);
  CALL_C(0x7a2e, objectCheckWithinScreenBoundary_hook, 0x2184, 0x7a31);
  if (!(F & FC)) { CYCT(0x7a31, 0x7a33); goto delete; } // jr nc
  CYC(0x7a31, 0x7a33);
  CALL_C(0x7a33, objectApplySpeed_hook, 0x201d, 0x7a36);
  CYC(0x7a36, 0x7a38); E = 0xc2; // Part.subid
  CYC(0x7a38, 0x7a39); A = mem_rd(gb, DE);
  CYC(0x7a39, 0x7a3a); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7a3a); return; } // ret nz
  CYC(0x7a3a, 0x7a3b);
  CYC(0x7a3b, 0x7a3d); E = 0xea; // Part.var2a
  CYC(0x7a3d, 0x7a3e); A = mem_rd(gb, DE);
  CYC(0x7a3e, 0x7a3f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7a3f, 0x7a41); goto beginDestroyAnimation; } // jr nz
  CYC(0x7a3f, 0x7a41);
  CALL_C(0x7a41, partCommon_getTileCollisionInFront_hook, 0x4000, 0x7a44);
  if (F & FZ) { CYCT(0x7a44, 0x7a46); goto state2; } // jr z
  CYC(0x7a44, 0x7a46);

beginDestroyAnimation:
  CYC(0x7a46, 0x7a47); H = D;
  CYC(0x7a47, 0x7a49); L = 0xe4; // Part.collisionType
  CYC(0x7a49, 0x7a4b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x7a4b, 0x7a4d); A = 0x02;
  CYC(0x7a4d, 0x7a4f); L = 0xc4; // Part.state
  CYC(0x7a4f, 0x7a50); mem_wr(gb, HL, A);
  CYC(0x7a50, 0x7a51); A = alu_dec8(gb, A);
  CALL_C(0x7a51, partSetAnimation_hook, 0x2988, 0x7a54);

state2:
  CALL_C(0x7a54, partAnimate_hook, 0x2978, 0x7a57);
  CYC(0x7a57, 0x7a59); E = 0xe1; // Part.animParameter
  CYC(0x7a59, 0x7a5a); A = mem_rd(gb, DE);
  CYC(0x7a5a, 0x7a5b); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x7a5b); return; } // ret z
  CYC(0x7a5b, 0x7a5c);

delete:
  CYC(0x7a5c, 0x7a5f); partDelete_hook(gb); return; // jp
}
