#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode1b_jump_table(GB *gb) {
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

// enemyCode1b@checkBushOrRockGone: shared local reached by genuine `call` from @state8's
// return address 0x5829 and @state9's return address 0x584d, both at sp0_ nesting depth.
// Modeled as its own bare hook (called through CALL_C like any other routine) since it
// declares its own sp0_ and calls objectSetVisiblec3_hook mid-body with real continuation.
void enemyCode1b_checkBushOrRockGone_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x588f, 0x5891); E = ENEMY_BASE + OBJ_VAR19; // Enemy.relatedObj2+1
  CYC(0x5891, 0x5892); A = mem_rd(gb, DE);
  CYC(0x5892, 0x5893); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5893); return; } // ret nz
  CYC(0x5893, 0x5894);
  CYC(0x5894, 0x5895); H = D;
  CYC(0x5895, 0x5897); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5897, 0x5899); mem_wr(gb, HL, 0x0a);
  CYC(0x5899, 0x589b); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x589b, 0x589d); mem_wr(gb, HL, 60);
  CYC(0x589d, 0x589f); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x589f, 0x58a1); mem_wr(gb, HL, 0x80 | 0x16); // ENEMY_SPINY_BEETLE
  CYC(0x58a1, 0x58a3); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x58a3, 0x58a5); A = 0x06;
  CYC(0x58a5, 0x58a6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x58a6, 0x58a7); mem_wr(gb, HL, A);
  CALL_C(0x58a7, objectSetVisiblec3_hook, 0x1e4e, 0x58aa);
  CYC(0x58aa, 0x58ab); alu_xor(gb, A);
  RET(0x58ab); return; // ret
}

// ==================================================================================================
// ENEMY_SPINY_BEETLE
//
// Variables:
//   var03: $80 when stationary, $81 when charging Link. Child object (bush or rock) reads
//          this to determine relative Z position. Bit 7 is set to indicate it's grabbable.
//   var3b: Probably unused?
// ==================================================================================================
void enemyCode1b_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x57aa, ecom_checkHazards_b0d_hook, 0x4051, 0x57ad);
  if (F & FZ) { CYCT(0x57ad, 0x57af); goto normalStatus; } // jr z
  CYC(0x57ad, 0x57af);
  CYC(0x57af, 0x57b1); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x57b1); return; } // ret c
  CYC(0x57b1, 0x57b2);
  if (F & FZ) { CYCT(0x57b2, 0x57b5); enemyDie_hook(gb); return; } // jp z
  CYC(0x57b2, 0x57b5);
  CYC(0x57b5, 0x57b6); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x57b6, 0x57b9); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x57b6, 0x57b9);
  CYC(0x57b9, 0x57bb); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x57bb, 0x57bc); A = mem_rd(gb, DE);
  CYC(0x57bc, 0x57be); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(0x57be); return; } // ret nz
  CYC(0x57be, 0x57bf);
  CYC(0x57bf, 0x57c1); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x57c1, 0x57c2); A = mem_rd(gb, DE);
  CYC(0x57c2, 0x57c4); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x57c4, 0x57c6); goto normalStatus; } // jr nz
  CYC(0x57c4, 0x57c6);
  CALL_C(0x57c6, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x57c9);
  CYC(0x57c9, 0x57cc); goto chargeAtLink; // jp

normalStatus:
  CYC(0x57cc, 0x57ce); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x57ce, 0x57cf); A = mem_rd(gb, DE);
  {
    CYC(0x57cf, 0x57d0); push_effect(gb, 0x57d0);
    uint16_t target = enemyCode1b_jump_table(gb);
    if (target == 0x57e8) goto state_uninitialized;
    if (target == 0x5825) goto state_stub;
    if (target == 0x5819) goto state_switchHook;
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x5826) goto state8;
    if (target == 0x584d) goto state9;
    if (target == 0x586e) goto stateA;
    if (target == 0x5878) goto stateB;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x57e8, 0x57ea); B = 0x58; // ENEMY_BUSH_OR_ROCK
  CALL_C(0x57ea, ecom_spawnUncountedEnemyWithSubid01_b0d_hook, 0x436d, 0x57ed);
  if (!(F & FZ)) { RET_TAKEN(0x57ed); return; } // ret nz
  CYC(0x57ed, 0x57ee);
  CYC(0x57ee, 0x57ef); E = L;
  CYC(0x57ef, 0x57f0); A = mem_rd(gb, DE);
  CYC(0x57f0, 0x57f1); mem_wr(gb, HL, A); // [child.subid] = [this.subid]
  CYC(0x57f1, 0x57f3); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x57f3, 0x57f5); A = ENEMY_BASE; // Enemy.start
  CYC(0x57f5, 0x57f6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x57f6, 0x57f7); mem_wr(gb, HL, D);
  CYC(0x57f7, 0x57f9); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(0x57f9, 0x57fa); mem_wr(gb, DE, A);
  CYC(0x57fa, 0x57fb); E = alu_inc8(gb, E);
  CYC(0x57fb, 0x57fc); A = H;
  CYC(0x57fc, 0x57fd); mem_wr(gb, DE, A);
  CALL_C(0x57fd, objectCopyPosition_hook, 0x2242, 0x5800);
  CYC(0x5800, 0x5802); A = 0x23; // SPEED_e0
  CALL_C(0x5802, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x5805);
  CYC(0x5805, 0x5807); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5807, 0x5809); A = 0x03;
  CYC(0x5809, 0x580a); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x580a, 0x580b); mem_wr(gb, HL, A);
  CYC(0x580b, 0x580d); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x580d, 0x580f); mem_wr(gb, HL, 0x80);
  CYC(0x580f, 0x5810); L = alu_dec8(gb, L);
  CYC(0x5810, 0x5811); A = mem_rd(gb, HL);
  CYC(0x5811, 0x5813); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(0x5813); return; } // ret c
  CYC(0x5813, 0x5814);
  CYC(0x5814, 0x5816); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5816, 0x5818); mem_wr(gb, HL, 0x80 | 0x16); // ENEMY_BEAMOS
  RET(0x5818); return; // ret

state_switchHook:
  CYC(0x5819, 0x581a); E = alu_inc8(gb, E);
  CYC(0x581a, 0x581b); A = mem_rd(gb, DE);
  {
    CYC(0x581b, 0x581c); push_effect(gb, 0x581c);
    uint16_t target = enemyCode1b_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x5824) goto substate1;
    if (target == 0x44e0) { ecom_fallToGroundAndSetState8_b0d_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(0x5824); return; // ret

state_stub:
  RET(0x5825); return; // ret

state8:
  CALL_C(0x5826, enemyCode1b_checkBushOrRockGone_hook, 0x588f, 0x5829);
  if (F & FZ) { RET_TAKEN(0x5829); return; } // ret z
  CYC(0x5829, 0x582a);
  CALL_C(0x582a, ecom_decCounter2_b0d_hook, 0x43a3, 0x582d);
  if (!(F & FZ)) { RET_TAKEN(0x582d); return; } // ret nz
  CYC(0x582d, 0x582e);
  CYC(0x582e, 0x5830); B = 0x0c;
  CALL_C(0x5830, objectCheckCenteredWithLink_hook, 0x1fee, 0x5833);
  if (!(F & FC)) { RET_TAKEN(0x5833); return; } // ret nc
  CYC(0x5833, 0x5834);
  CALL_C(0x5834, ecom_updateCardinalAngleTowardTarget_b0d_hook, 0x43b4, 0x5837);
  CYC(0x5837, 0x5838); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5838); return; } // ret z (never moves up)
  CYC(0x5838, 0x5839);
  CYC(0x5839, 0x583b); A = 0x01;
  CALL_C(0x583b, ecom_getTopDownAdjacentWallsBitset_b0d_hook, 0x4204, 0x583e);
  if (!(F & FZ)) { RET_TAKEN(0x583e); return; } // ret nz
  CYC(0x583e, 0x583f);

chargeAtLink:
  CALL_C(0x583f, ecom_incState_b0d_hook, 0x4000, 0x5842); // [state] = 9
  CYC(0x5842, 0x5844); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5844, 0x5846); mem_wr(gb, HL, 0x38);
  CYC(0x5846, 0x5848); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x5848, 0x584a); mem_wr(gb, HL, 0x81);
  CYC(0x584a, 0x584d); objectSetVisiblec3_hook(gb); return; // jp

state9:
  CALL_C(0x584d, enemyCode1b_checkBushOrRockGone_hook, 0x588f, 0x5850);
  if (F & FZ) { RET_TAKEN(0x5850); return; } // ret z
  CYC(0x5850, 0x5851);
  CALL_C(0x5851, ecom_decCounter1_b0d_hook, 0x439a, 0x5854);
  if (F & FZ) { CYCT(0x5854, 0x5856); goto state9SlowDown; } // jr z
  CYC(0x5854, 0x5856);
  CALL_C(0x5856, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, 0x414c, 0x5859);
  if (!(F & FZ)) { CYCT(0x5859, 0x585b); goto animate; } // jr nz
  CYC(0x5859, 0x585b);

state9SlowDown:
  CYC(0x585b, 0x585c); H = D;
  CYC(0x585c, 0x585e); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x585e, 0x5860); mem_wr(gb, HL, 30);
  CYC(0x5860, 0x5862); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5862, 0x5863); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5863, 0x5865); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x5865, 0x5867); mem_wr(gb, HL, 0x80);
  CYC(0x5867, 0x5869); L = ENEMY_BASE + OBJ_VAR3B;
  CYC(0x5869, 0x586b); mem_wr(gb, HL, 0x00);
  CYC(0x586b, 0x586e); objectSetInvisible_hook(gb); return; // jp

stateA:
  CALL_C(0x586e, ecom_decCounter1_b0d_hook, 0x439a, 0x5871);
  if (!(F & FZ)) { CYCT(0x5871, 0x5873); goto animate; } // jr nz
  CYC(0x5871, 0x5873);
  CYC(0x5873, 0x5874); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5874, 0x5875); L = E;
  CYC(0x5875, 0x5876); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5876, 0x5878); goto animate; // jr

stateB:
  CALL_C(0x5878, ecom_decCounter1_b0d_hook, 0x439a, 0x587b);
  if (!(F & FZ)) { CYCT(0x587b, 0x587d); goto stateBApplyVelocity; } // jr nz
  CYC(0x587b, 0x587d);
  CYC(0x587d, 0x587f); mem_wr(gb, HL, 40);
  CALL_C(0x587f, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5882);
  CYC(0x5882, 0x5884); alu_and(gb, 0x1c);
  CYC(0x5884, 0x5886); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5886, 0x5887); mem_wr(gb, DE, A);
  CYC(0x5887, 0x5889); goto animate; // jr

stateBApplyVelocity:
  CALL_C(0x5889, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x588c);

animate:
  CYC(0x588c, 0x588f); enemyAnimate_hook(gb); return; // jp
}
