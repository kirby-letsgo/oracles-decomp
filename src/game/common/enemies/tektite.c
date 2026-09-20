#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t tektite_jump_table(GB *gb) {
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

// ==================================================================================================
// ENEMY_TEKTITE
//
// Variables:
//   var30: Gravity
//   var31: Minimum value for counter1 (lower value = more frequent jumping)
// ==================================================================================================
void enemyCode30_hook(GB *gb) {
  BASE(enemyCode30);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0e_hook, SYM(ecom_checkHazards_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  RET(b_+15); return; // ret

normalStatus:
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  {
    CYC(b_+19, b_+20); push_effect(gb, b_+20);
    uint16_t target = tektite_jump_table(gb);
    if (target == b_+44) goto state_uninitialized;
    if (target == b_+92) goto state_stub;
    if (target == b_+71) goto state_switchHook;
    if (target == b_+93) goto state8;
    if (target == b_+122) goto state9;
    if (target == b_+133) goto stateA;
    if (target == b_+177) goto stateB;
    HANDOFF(target);
  }

state_uninitialized:
  // Subid 1 has lower value for var31, meaning more frequent jumps.
  CYC(b_+44, b_+45); H = D;
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+47, b_+49); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+49, b_+51); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+51, b_+53); mem_wr(gb, HL, 90);
  if (F & FZ) { CYCT(b_+53, b_+55); goto L_4529; } // jr z
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+57); mem_wr(gb, HL, 45);

L_4529:
  CALL_C(b_+57, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+60);
  CYC(b_+60, b_+62); alu_and(gb, 0x7f);
  CYC(b_+62, b_+63); A = alu_inc8(gb, A);
  CYC(b_+63, b_+65); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  CYC(b_+66, b_+68); A = 0x32; // SPEED_140
  CYC(b_+68, b_+71); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp

state_switchHook:
  CYC(b_+71, b_+72); E = alu_inc8(gb, E);
  CYC(b_+72, b_+73); A = mem_rd(gb, DE);
  {
    CYC(b_+73, b_+74); push_effect(gb, b_+74);
    uint16_t target = tektite_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0e)) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == b_+82) goto substate1;
    if (target == b_+83) goto substate3;
    HANDOFF(target);
  }

substate1: // also substate2
  RET(b_+82); return; // ret

substate3:
  CYC(b_+83, b_+85); B = 0x08;
  CALL_C(b_+85, ecom_fallToGroundAndSetState_b0e_hook, SYM(ecom_fallToGroundAndSetState_b0e), b_+88);
  if (!(F & FZ)) { RET_TAKEN(b_+88); return; } // ret nz
  CYC(b_+88, b_+89);
  CYC(b_+89, b_+92); goto gotoState8; // jp

state_stub:
  RET(b_+92); return; // ret

// Standing in place for [counter1] frames
state8:
  CALL_C(b_+93, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+96);
  if (!(F & FZ)) { CYCT(b_+96, b_+98); goto animate; } // jr nz
  CYC(b_+96, b_+98);
  CALL_C(b_+98, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+101);
  CYC(b_+101, b_+103); alu_and(gb, 0x7f);
  CALL_C(b_+103, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+106);
  CYC(b_+106, b_+108); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+108, b_+109); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+109, b_+111); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+111, b_+112); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x18);
  CYC(b_+114, b_+116); A = 0x01;
  CYC(b_+116, b_+119); enemySetAnimation_hook(gb); return; // jp

animate:
  CYC(b_+119, b_+122); enemyAnimate_hook(gb); return; // jp

state9:
  CALL_C(b_+122, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+125);
  if (!(F & FZ)) { RET_TAKEN(b_+125); return; } // ret nz
  CYC(b_+125, b_+126);
  CYC(b_+126, b_+127); L = E;
  CYC(b_+127, b_+128); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+128, b_+130); A = 0x02;
  CYC(b_+130, b_+133); enemySetAnimation_hook(gb); return; // jp

stateA:
  CYC(b_+133, b_+135); A = 0x0b;
  CYC(b_+135, b_+136); mem_wr(gb, DE, A); // [state]
  CALL_C(b_+136, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+139);
  CYC(b_+139, b_+141); alu_and(gb, 0x07);
  CYC(b_+141, b_+144); SET_HL(b_+171); // @smallLeap (data)
  if (!(F & FZ)) { CYCT(b_+144, b_+146); goto L_4585; } // jr nz
  CYC(b_+144, b_+146);
  CYC(b_+146, b_+149); SET_HL(b_+174); // @bigLeap (data)

L_4585:
  CYC(b_+149, b_+151); E = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+151, b_+152); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+152, b_+153); mem_wr(gb, DE, A);
  CYC(b_+153, b_+154); E = alu_inc8(gb, E);
  CYC(b_+154, b_+155); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+155, b_+156); mem_wr(gb, DE, A);
  CYC(b_+156, b_+158); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+158, b_+159); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+159, b_+160); mem_wr(gb, DE, A);
  CALL_C(b_+160, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+163);
  CYC(b_+163, b_+165); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(b_+165, playSound_b00_hook, SYM(playSound_b00), b_+168);
  CYC(b_+168, b_+171); objectSetVisiblec1_hook(gb); return; // jp

stateB:
  CALL_C(b_+177, ecom_bounceOffScreenBoundary_b0e_hook, SYM(ecom_bounceOffScreenBoundary_b0e), b_+180);
  CYC(b_+180, b_+182); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+182, b_+183); A = mem_rd(gb, DE);
  CYC(b_+183, b_+184); C = A;
  CALL_C(b_+184, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+187);
  if (!(F & FZ)) { CYCT(b_+187, b_+190); ecom_applyVelocityForSideviewEnemy_b0e_hook(gb); return; } // jp nz
  CYC(b_+187, b_+190);

gotoState8:
  CALL_C(b_+190, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+193);
  CYC(b_+193, b_+195); alu_and(gb, 0x7f);
  CYC(b_+195, b_+196); H = D;
  CYC(b_+196, b_+198); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+198, b_+199); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+199, b_+201); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+201, b_+202); mem_wr(gb, HL, A);
  CYC(b_+202, b_+204); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+204, b_+206); mem_wr(gb, HL, 0x08);
  CYC(b_+206, b_+207); alu_xor(gb, A);
  CALL_C(b_+207, enemySetAnimation_hook, SYM(enemySetAnimation), b_+210);
  CYC(b_+210, b_+213); objectSetVisiblec2_hook(gb); return; // jp
}
