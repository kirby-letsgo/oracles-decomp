#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode1b);
  uint16_t sp0_ = gb->sp;
  CYC(b_+229, b_+231); E = ENEMY_BASE + OBJ_VAR19; // Enemy.relatedObj2+1
  CYC(b_+231, b_+232); A = mem_rd(gb, DE);
  CYC(b_+232, b_+233); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+233); return; } // ret nz
  CYC(b_+233, b_+234);
  CYC(b_+234, b_+235); H = D;
  CYC(b_+235, b_+237); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+237, b_+239); mem_wr(gb, HL, 0x0a);
  CYC(b_+239, b_+241); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+241, b_+243); mem_wr(gb, HL, 60);
  CYC(b_+243, b_+245); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+245, b_+247); mem_wr(gb, HL, 0x80 | 0x16); // ENEMY_SPINY_BEETLE
  CYC(b_+247, b_+249); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+249, b_+251); A = 0x06;
  CYC(b_+251, b_+252); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+252, b_+253); mem_wr(gb, HL, A);
  CALL_C(b_+253, objectSetVisiblec3_hook, SYM(objectSetVisiblec3), b_+256);
  CYC(b_+256, b_+257); alu_xor(gb, A);
  RET(b_+257); return; // ret
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
  BASE(enemyCode1b);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0d_hook, SYM(ecom_checkHazards_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; } // ret nz
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+26, b_+28); goto normalStatus; } // jr nz
  CYC(b_+26, b_+28);
  CALL_C(b_+28, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+31);
  CYC(b_+31, b_+34); goto chargeAtLink; // jp

normalStatus:
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  {
    CYC(b_+37, b_+38); push_effect(gb, b_+38);
    uint16_t target = enemyCode1b_jump_table(gb);
    if (target == b_+62) goto state_uninitialized;
    if (target == b_+123) goto state_stub;
    if (target == b_+111) goto state_switchHook;
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == b_+124) goto state8;
    if (target == b_+163) goto state9;
    if (target == b_+196) goto stateA;
    if (target == b_+206) goto stateB;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+62, b_+64); B = 0x58; // ENEMY_BUSH_OR_ROCK
  CALL_C(b_+64, ecom_spawnUncountedEnemyWithSubid01_b0d_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0d), b_+67);
  if (!(F & FZ)) { RET_TAKEN(b_+67); return; } // ret nz
  CYC(b_+67, b_+68);
  CYC(b_+68, b_+69); E = L;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+71); mem_wr(gb, HL, A); // [child.subid] = [this.subid]
  CYC(b_+71, b_+73); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+73, b_+75); A = ENEMY_BASE; // Enemy.start
  CYC(b_+75, b_+76); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+76, b_+77); mem_wr(gb, HL, D);
  CYC(b_+77, b_+79); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+81); E = alu_inc8(gb, E);
  CYC(b_+81, b_+82); A = H;
  CYC(b_+82, b_+83); mem_wr(gb, DE, A);
  CALL_C(b_+83, objectCopyPosition_hook, SYM(objectCopyPosition), b_+86);
  CYC(b_+86, b_+88); A = 0x23; // SPEED_e0
  CALL_C(b_+88, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+91);
  CYC(b_+91, b_+93); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+93, b_+95); A = 0x03;
  CYC(b_+95, b_+96); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+96, b_+97); mem_wr(gb, HL, A);
  CYC(b_+97, b_+99); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+99, b_+101); mem_wr(gb, HL, 0x80);
  CYC(b_+101, b_+102); L = alu_dec8(gb, L);
  CYC(b_+102, b_+103); A = mem_rd(gb, HL);
  CYC(b_+103, b_+105); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+105); return; } // ret c
  CYC(b_+105, b_+106);
  CYC(b_+106, b_+108); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x80 | 0x16); // ENEMY_BEAMOS
  RET(b_+110); return; // ret

state_switchHook:
  CYC(b_+111, b_+112); E = alu_inc8(gb, E);
  CYC(b_+112, b_+113); A = mem_rd(gb, DE);
  {
    CYC(b_+113, b_+114); push_effect(gb, b_+114);
    uint16_t target = enemyCode1b_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+122) goto substate1;
    if (target == SYM(ecom_fallToGroundAndSetState8_b0d)) { ecom_fallToGroundAndSetState8_b0d_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(b_+122); return; // ret

state_stub:
  RET(b_+123); return; // ret

state8:
  CALL_L(b_+124, enemyCode1b_checkBushOrRockGone_hook, b_+127);
  if (F & FZ) { RET_TAKEN(b_+127); return; } // ret z
  CYC(b_+127, b_+128);
  CALL_C(b_+128, ecom_decCounter2_b0d_hook, SYM(ecom_decCounter2_b0d), b_+131);
  if (!(F & FZ)) { RET_TAKEN(b_+131); return; } // ret nz
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+134); B = 0x0c;
  CALL_C(b_+134, objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+137);
  if (!(F & FC)) { RET_TAKEN(b_+137); return; } // ret nc
  CYC(b_+137, b_+138);
  CALL_C(b_+138, ecom_updateCardinalAngleTowardTarget_b0d_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0d), b_+141);
  CYC(b_+141, b_+142); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+142); return; } // ret z (never moves up)
  CYC(b_+142, b_+143);
  CYC(b_+143, b_+145); A = 0x01;
  CALL_C(b_+145, ecom_getTopDownAdjacentWallsBitset_b0d_hook, SYM(ecom_getTopDownAdjacentWallsBitset_b0d), b_+148);
  if (!(F & FZ)) { RET_TAKEN(b_+148); return; } // ret nz
  CYC(b_+148, b_+149);

chargeAtLink:
  CALL_C(b_+149, ecom_incState_b0d_hook, SYM(ecom_incState_b0d), b_+152); // [state] = 9
  CYC(b_+152, b_+154); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+154, b_+156); mem_wr(gb, HL, 0x38);
  CYC(b_+156, b_+158); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+158, b_+160); mem_wr(gb, HL, 0x81);
  CYC(b_+160, b_+163); objectSetVisiblec3_hook(gb); return; // jp

state9:
  CALL_L(b_+163, enemyCode1b_checkBushOrRockGone_hook, b_+166);
  if (F & FZ) { RET_TAKEN(b_+166); return; } // ret z
  CYC(b_+166, b_+167);
  CALL_C(b_+167, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+170);
  if (F & FZ) { CYCT(b_+170, b_+172); goto state9SlowDown; } // jr z
  CYC(b_+170, b_+172);
  CALL_C(b_+172, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b0d), b_+175);
  if (!(F & FZ)) { CYCT(b_+175, b_+177); goto animate; } // jr nz
  CYC(b_+175, b_+177);

state9SlowDown:
  CYC(b_+177, b_+178); H = D;
  CYC(b_+178, b_+180); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 30);
  CYC(b_+182, b_+184); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+184, b_+185); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+185, b_+187); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+187, b_+189); mem_wr(gb, HL, 0x80);
  CYC(b_+189, b_+191); L = ENEMY_BASE + OBJ_VAR3B;
  CYC(b_+191, b_+193); mem_wr(gb, HL, 0x00);
  CYC(b_+193, b_+196); objectSetInvisible_hook(gb); return; // jp

stateA:
  CALL_C(b_+196, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+199);
  if (!(F & FZ)) { CYCT(b_+199, b_+201); goto animate; } // jr nz
  CYC(b_+199, b_+201);
  CYC(b_+201, b_+202); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+202, b_+203); L = E;
  CYC(b_+203, b_+204); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+204, b_+206); goto animate; // jr

stateB:
  CALL_C(b_+206, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+209);
  if (!(F & FZ)) { CYCT(b_+209, b_+211); goto stateBApplyVelocity; } // jr nz
  CYC(b_+209, b_+211);
  CYC(b_+211, b_+213); mem_wr(gb, HL, 40);
  CALL_C(b_+213, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+216);
  CYC(b_+216, b_+218); alu_and(gb, 0x1c);
  CYC(b_+218, b_+220); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+220, b_+221); mem_wr(gb, DE, A);
  CYC(b_+221, b_+223); goto animate; // jr

stateBApplyVelocity:
  CALL_C(b_+223, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+226);

animate:
  CYC(b_+226, b_+229); enemyAnimate_hook(gb); return; // jp
}
