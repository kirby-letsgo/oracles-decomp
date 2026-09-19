#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode14_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void enemyCode14_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ==================================================================================================
// ENEMY_SPIKED_BEETLE
// ==================================================================================================
void enemyCode14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x51d4, ecom_checkHazards_b0d_hook, 0x4051, 0x51d7);
  if (F & FZ) { CYCT(0x51d7, 0x51d9); goto normalStatus; } // jr z
  CYC(0x51d7, 0x51d9);
  CYC(0x51d9, 0x51db); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x51db); return; } // ret c
  CYC(0x51db, 0x51dc);
  if (F & FZ) { CYCT(0x51dc, 0x51df); enemyDie_hook(gb); return; } // jp z
  CYC(0x51dc, 0x51df);
  CYC(0x51df, 0x51e0); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x51e0, 0x51e2); goto knockback; } // jr nz
  CYC(0x51e0, 0x51e2);
  CYC(0x51e2, 0x51e3); H = D;
  CYC(0x51e3, 0x51e5); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x51e5, 0x51e7); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x51e7, 0x51e9); goto checkFlipCollision; } // jr z
  CYC(0x51e7, 0x51e9);
  CYC(0x51e9, 0x51eb); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x51eb, 0x51ec); A = mem_rd(gb, DE);
  CYC(0x51ec, 0x51ed); alu_rlca(gb);
  if (F & FC) { CYCT(0x51ed, 0x51ef); goto checkFlipCollision; } // jr c
  CYC(0x51ed, 0x51ef);
  CYC(0x51ef, 0x51f1); mem_wr(gb, HL, 0x00);

checkFlipCollision:
  CYC(0x51f1, 0x51f3); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x51f3, 0x51f4); A = mem_rd(gb, DE);
  CYC(0x51f4, 0x51f6); alu_cp(gb, 0x8c); // $80|ITEMCOLLISION_SHOVEL
  if (F & FZ) { CYCT(0x51f6, 0x51f8); goto alreadyFlipped; } // jr z
  CYC(0x51f6, 0x51f8);
  CYC(0x51f8, 0x51fa); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x51fa, 0x51fc); alu_sub(gb, 0x01); // ITEMCOLLISION_L1_SHIELD
  CYC(0x51fc, 0x51fe); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x51fe, 0x5200); goto normalStatus; } // jr nc
  CYC(0x51fe, 0x5200);

alreadyFlipped:
  CYC(0x5200, 0x5202); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5202, 0x5203); A = mem_rd(gb, DE);
  CYC(0x5203, 0x5205); alu_cp(gb, 0x0b);
  if (F & FZ) { RET_TAKEN(0x5205); return; } // ret z
  CYC(0x5205, 0x5206);
  CYC(0x5206, 0x5208); mem_wr(gb, HL, 0x01); // [var30] = $01
  CYC(0x5208, 0x520b); SET_BC(0xfe80); // -$180
  CALL_C(0x520b, objectSetSpeedZ_hook, 0x239d, 0x520e);
  CYC(0x520e, 0x5210); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5210, 0x5212); mem_wr(gb, HL, 0x0b);
  CYC(0x5212, 0x5214); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x5214, 0x5216); mem_wr(gb, HL, 0x51); // ENEMYCOLLISION_SPIKED_BEETLE_FLIPPED
  CYC(0x5216, 0x5218); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5218, 0x521a); mem_wr(gb, HL, 180);
  CYC(0x521a, 0x521c); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(0x521c, 0x521d); A = mem_rd(gb, HL);
  CYC(0x521d, 0x521f); alu_xor(gb, 0x10);
  CYC(0x521f, 0x5221); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5221, 0x5222); mem_wr(gb, HL, A);
  CYC(0x5222, 0x5224); A = 0x52; // SND_BOMB_LAND
  CALL_C(0x5224, playSound_b00_hook, 0x0c98, 0x5227);
  CYC(0x5227, 0x5229); A = 0x01;
  CYC(0x5229, 0x522c); enemySetAnimation_hook(gb); return; // jp

knockback:
  CYC(0x522c, 0x522e); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x522e, 0x522f); A = mem_rd(gb, DE);
  CYC(0x522f, 0x5230); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5230, 0x5233); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp z
  CYC(0x5230, 0x5233);
  CYC(0x5233, 0x5235); C = 0x18;
  CALL_C(0x5235, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x5238);
  CYC(0x5238, 0x523a); A = 0x01;
  if (!(F & FC)) { CYCT(0x523a, 0x523c); goto storeKnockback; } // jr nc
  CYC(0x523a, 0x523c);
  CYC(0x523c, 0x523d); alu_xor(gb, A);

storeKnockback:
  CYC(0x523d, 0x523f); E = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(0x523f, 0x5240); mem_wr(gb, DE, A);
  CYC(0x5240, 0x5242); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(0x5242, 0x5243); A = mem_rd(gb, DE);
  CYC(0x5243, 0x5244); C = A;
  CYC(0x5244, 0x5246); B = 0x23; // SPEED_e0
  CYC(0x5246, 0x5249); ecom_applyGivenVelocity_b0d_hook(gb); return; // jp

normalStatus:
  CYC(0x5249, 0x524b); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x524b, 0x524c); A = mem_rd(gb, DE);
  {
    CYC(0x524c, 0x524d); push_effect(gb, 0x524d);
    uint16_t target = enemyCode14_jump_table(gb);
    if (target == 0x5267) goto uninitialized;
    if (target == 0x526f) goto state_stub;
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x5270) goto state8;
    if (target == 0x5287) goto state9;
    if (target == 0x529a) goto stateA;
    if (target == 0x52b2) goto stateB;
    if (target == 0x52e1) goto stateC;
    HANDOFF(target);
  }

uninitialized:
  CYC(0x5267, 0x526a); push_effect(gb, 0x526a); goto setRandomAngleAndCounter1;
uninitialized_resume: // resumes at 0x526a
  CYC(0x526a, 0x526c); A = 0x0a; // SPEED_40
  CYC(0x526c, 0x526f); ecom_setSpeedAndState8AndVisible_b0d_hook(gb); return; // jp

state_stub:
  RET(0x526f); return; // ret

state8:
  CYC(0x5270, 0x5272); B = 0x08;
  CALL_C(0x5272, objectCheckCenteredWithLink_hook, 0x1fee, 0x5275);
  if (F & FC) { CYCT(0x5275, 0x5278); goto chargeLink; } // jp c
  CYC(0x5275, 0x5278);
  CALL_C(0x5278, ecom_decCounter1_b0d_hook, 0x439a, 0x527b);
  if (F & FZ) { CYCT(0x527b, 0x527e); goto setRandomAngleAndCounter1; } // jp z
  CYC(0x527b, 0x527e);
  CALL_C(0x527e, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x5281);
  if (F & FZ) { CYCT(0x5281, 0x5284); goto setRandomAngleAndCounter1; } // jp z
  CYC(0x5281, 0x5284);

animate:
  CYC(0x5284, 0x5287); enemyAnimate_hook(gb); return; // jp

state9:
  CALL_C(0x5287, ecom_decCounter2_b0d_hook, 0x43a3, 0x528a);
  CYC(0x528a, 0x528d); push_effect(gb, 0x528d); goto incSpeed;
state9_resume: // resumes at 0x528d
  CALL_C(0x528d, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x5290);
  if (!(F & FZ)) { CYCT(0x5290, 0x5292); goto animate; } // jr nz
  CYC(0x5290, 0x5292);
  CALL_C(0x5292, ecom_incState_b0d_hook, 0x4000, 0x5295);
  CYC(0x5295, 0x5297); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5297, 0x5299); mem_wr(gb, HL, 30);
  RET(0x5299); return; // ret

stateA:
  CYC(0x529a, 0x529c); B = 0x08;
  CALL_C(0x529c, objectCheckCenteredWithLink_hook, 0x1fee, 0x529f);
  if (F & FC) { CYCT(0x529f, 0x52a2); goto chargeLink; } // jp c
  CYC(0x529f, 0x52a2);
  CALL_C(0x52a2, ecom_decCounter1_b0d_hook, 0x439a, 0x52a5);
  if (!(F & FZ)) { CYCT(0x52a5, 0x52a7); goto animate; } // jr nz
  CYC(0x52a5, 0x52a7);
  CYC(0x52a7, 0x52a9); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x52a9, 0x52ab); mem_wr(gb, HL, 0x08);
  CYC(0x52ab, 0x52ad); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x52ad, 0x52af); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(0x52af, 0x52b2); goto setRandomAngleAndCounter1; // jp

stateB:
  CALL_C(0x52b2, ecom_decCounter1_b0d_hook, 0x439a, 0x52b5);
  if (!(F & FZ)) { CYCT(0x52b5, 0x52b7); goto waitingToFlipBack; } // jr nz
  CYC(0x52b5, 0x52b7);
  CYC(0x52b7, 0x52b8); L = E;
  CYC(0x52b8, 0x52b9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c
  CYC(0x52b9, 0x52bb); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x52bb, 0x52bd); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x52bd, 0x52bf); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x52bf, 0x52c1); mem_wr(gb, HL, 0x18); // ENEMYCOLLISION_SPIKED_BEETLE
  CYC(0x52c1, 0x52c3); L = ENEMY_BASE + OBJ_XH;
  CYC(0x52c3, 0x52c4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x52c4, 0x52c7); SET_BC(0xfe80); // -$180
  CALL_C(0x52c7, objectSetSpeedZ_hook, 0x239d, 0x52ca);
  CYC(0x52ca, 0x52cb); alu_xor(gb, A);
  CYC(0x52cb, 0x52ce); enemySetAnimation_hook(gb); return; // jp

waitingToFlipBack:
  CYC(0x52ce, 0x52cf); A = mem_rd(gb, HL);
  CYC(0x52cf, 0x52d1); alu_cp(gb, 0x3c);
  if (!(F & FC)) { CYCT(0x52d1, 0x52d3); goto animate; } // jr nc
  CYC(0x52d1, 0x52d3);
  CYC(0x52d3, 0x52d5); alu_and(gb, 0x06);
  CYC(0x52d5, 0x52d6); alu_rrca(gb);
  CYC(0x52d6, 0x52d9); SET_HL(0x5331); // @xOscillationOffsets (data)
  CYC(0x52d9, 0x52da); enemyCode14_addAToHl_from_rst(gb, 0x52da);
  CYC(0x52da, 0x52dc); E = ENEMY_BASE + OBJ_XH;
  CYC(0x52dc, 0x52dd); A = mem_rd(gb, DE);
  CYC(0x52dd, 0x52de); alu_add(gb, mem_rd(gb, HL));
  CYC(0x52de, 0x52df); mem_wr(gb, DE, A);
  CYC(0x52df, 0x52e1); goto animate; // jr

stateC:
  CALL_C(0x52e1, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x52e4);
  CALL_C(0x52e4, enemyAnimate_hook, 0x2818, 0x52e7);
  CYC(0x52e7, 0x52e9); C = 0x18;
  CALL_C(0x52e9, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x52ec);
  if (!(F & FZ)) { RET_TAKEN(0x52ec); return; } // ret nz
  CYC(0x52ec, 0x52ed);
  CYC(0x52ed, 0x52ef); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x52ef, 0x52f1); A = 0x08;
  CYC(0x52f1, 0x52f2); mem_wr(gb, DE, A);
  CYC(0x52f2, 0x52f4); B = 0x10;
  CALL_C(0x52f4, objectCheckCenteredWithLink_hook, 0x1fee, 0x52f7);
  if (F & FC) { CYCT(0x52f7, 0x52f9); goto chargeLink; } // jr c
  CYC(0x52f7, 0x52f9);
  CYC(0x52f9, 0x52fb); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x52fb, 0x52fd); A = 0x0a; // SPEED_40
  CYC(0x52fd, 0x52fe); mem_wr(gb, DE, A);
  RET(0x52fe); return; // ret

setRandomAngleAndCounter1:
  CYC(0x52ff, 0x5302); SET_BC(0x1830);
  CALL_C(0x5302, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x5305);
  CYC(0x5305, 0x5307); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5307, 0x5308); A = B;
  CYC(0x5308, 0x5309); mem_wr(gb, DE, A);
  CYC(0x5309, 0x530b); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x530b, 0x530d); A = 0x30;
  CYC(0x530d, 0x530e); alu_add(gb, C);
  CYC(0x530e, 0x530f); mem_wr(gb, DE, A);
  RET(0x530f);
  if (gb->pc == 0x526a && gb->sp == sp0_) goto uninitialized_resume;
  return; // ret

chargeLink:
  CALL_C(0x5310, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x5313);
  CYC(0x5313, 0x5314); H = D;
  CYC(0x5314, 0x5316); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5316, 0x5318); mem_wr(gb, HL, 0x09);
  CYC(0x5318, 0x531a); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x531a, 0x531c); mem_wr(gb, HL, 150);
  CYC(0x531c, 0x531e); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x531e, 0x5320); mem_wr(gb, HL, 0x0a); // SPEED_40
  RET(0x5320); return; // ret

incSpeed:
  CYC(0x5321, 0x5323); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x5323, 0x5324); A = mem_rd(gb, DE);
  CYC(0x5324, 0x5326); alu_and(gb, 0x03);
  if (!(F & FZ)) {
    RET_TAKEN(0x5326);
    if (gb->pc == 0x528d && gb->sp == sp0_) goto state9_resume;
    return;
  } // ret nz
  CYC(0x5326, 0x5327);
  CYC(0x5327, 0x5329); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5329, 0x532a); A = mem_rd(gb, DE);
  CYC(0x532a, 0x532c); alu_cp(gb, 0x3c); // SPEED_180
  if (!(F & FC)) {
    RET_TAKEN(0x532c);
    if (gb->pc == 0x528d && gb->sp == sp0_) goto state9_resume;
    return;
  } // ret nc
  CYC(0x532c, 0x532d);
  CYC(0x532d, 0x532f); alu_add(gb, 0x05); // SPEED_20
  CYC(0x532f, 0x5330); mem_wr(gb, DE, A);
  RET(0x5330);
  if (gb->pc == 0x528d && gb->sp == sp0_) goto state9_resume;
  return; // ret
}
