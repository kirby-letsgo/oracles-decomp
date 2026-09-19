#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x44f0, ecom_checkHazards_b0e_hook, 0x4051, 0x44f3);
  if (F & FZ) { CYCT(0x44f3, 0x44f5); goto normalStatus; } // jr z
  CYC(0x44f3, 0x44f5);
  CYC(0x44f5, 0x44f7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x44f7); return; } // ret c
  CYC(0x44f7, 0x44f8);
  if (F & FZ) { CYCT(0x44f8, 0x44fb); enemyDie_hook(gb); return; } // jp z
  CYC(0x44f8, 0x44fb);
  CYC(0x44fb, 0x44fc); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x44fc, 0x44ff); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(0x44fc, 0x44ff);
  RET(0x44ff); return; // ret

normalStatus:
  CYC(0x4500, 0x4502); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x4502, 0x4503); A = mem_rd(gb, DE);
  {
    CYC(0x4503, 0x4504); push_effect(gb, 0x4504);
    uint16_t target = tektite_jump_table(gb);
    if (target == 0x451c) goto state_uninitialized;
    if (target == 0x454c) goto state_stub;
    if (target == 0x4537) goto state_switchHook;
    if (target == 0x454d) goto state8;
    if (target == 0x456a) goto state9;
    if (target == 0x4575) goto stateA;
    if (target == 0x45a1) goto stateB;
    HANDOFF(target);
  }

state_uninitialized:
  // Subid 1 has lower value for var31, meaning more frequent jumps.
  CYC(0x451c, 0x451d); H = D;
  CYC(0x451d, 0x451f); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x451f, 0x4521); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(0x4521, 0x4523); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x4523, 0x4525); mem_wr(gb, HL, 90);
  if (F & FZ) { CYCT(0x4525, 0x4527); goto L_4529; } // jr z
  CYC(0x4525, 0x4527);
  CYC(0x4527, 0x4529); mem_wr(gb, HL, 45);

L_4529:
  CALL_C(0x4529, getRandomNumber_noPreserveVars_hook, 0x0453, 0x452c);
  CYC(0x452c, 0x452e); alu_and(gb, 0x7f);
  CYC(0x452e, 0x452f); A = alu_inc8(gb, A);
  CYC(0x452f, 0x4531); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4531, 0x4532); mem_wr(gb, DE, A);
  CYC(0x4532, 0x4534); A = 0x32; // SPEED_140
  CYC(0x4534, 0x4537); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp

state_switchHook:
  CYC(0x4537, 0x4538); E = alu_inc8(gb, E);
  CYC(0x4538, 0x4539); A = mem_rd(gb, DE);
  {
    CYC(0x4539, 0x453a); push_effect(gb, 0x453a);
    uint16_t target = tektite_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == 0x4542) goto substate1;
    if (target == 0x4543) goto substate3;
    HANDOFF(target);
  }

substate1: // also substate2
  RET(0x4542); return; // ret

substate3:
  CYC(0x4543, 0x4545); B = 0x08;
  CALL_C(0x4545, ecom_fallToGroundAndSetState_b0e_hook, 0x44e2, 0x4548);
  if (!(F & FZ)) { RET_TAKEN(0x4548); return; } // ret nz
  CYC(0x4548, 0x4549);
  CYC(0x4549, 0x454c); goto gotoState8; // jp

state_stub:
  RET(0x454c); return; // ret

// Standing in place for [counter1] frames
state8:
  CALL_C(0x454d, ecom_decCounter1_b0e_hook, 0x439a, 0x4550);
  if (!(F & FZ)) { CYCT(0x4550, 0x4552); goto animate; } // jr nz
  CYC(0x4550, 0x4552);
  CALL_C(0x4552, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4555);
  CYC(0x4555, 0x4557); alu_and(gb, 0x7f);
  CALL_C(0x4557, ecom_incState_b0e_hook, 0x4000, 0x455a);
  CYC(0x455a, 0x455c); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x455c, 0x455d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x455d, 0x455f); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x455f, 0x4560); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x4560, 0x4562); mem_wr(gb, HL, 0x18);
  CYC(0x4562, 0x4564); A = 0x01;
  CYC(0x4564, 0x4567); enemySetAnimation_hook(gb); return; // jp

animate:
  CYC(0x4567, 0x456a); enemyAnimate_hook(gb); return; // jp

state9:
  CALL_C(0x456a, ecom_decCounter2_b0e_hook, 0x43a3, 0x456d);
  if (!(F & FZ)) { RET_TAKEN(0x456d); return; } // ret nz
  CYC(0x456d, 0x456e);
  CYC(0x456e, 0x456f); L = E;
  CYC(0x456f, 0x4570); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x4570, 0x4572); A = 0x02;
  CYC(0x4572, 0x4575); enemySetAnimation_hook(gb); return; // jp

stateA:
  CYC(0x4575, 0x4577); A = 0x0b;
  CYC(0x4577, 0x4578); mem_wr(gb, DE, A); // [state]
  CALL_C(0x4578, getRandomNumber_noPreserveVars_hook, 0x0453, 0x457b);
  CYC(0x457b, 0x457d); alu_and(gb, 0x07);
  CYC(0x457d, 0x4580); SET_HL(0x459b); // @smallLeap (data)
  if (!(F & FZ)) { CYCT(0x4580, 0x4582); goto L_4585; } // jr nz
  CYC(0x4580, 0x4582);
  CYC(0x4582, 0x4585); SET_HL(0x459e); // @bigLeap (data)

L_4585:
  CYC(0x4585, 0x4587); E = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x4587, 0x4588); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x4588, 0x4589); mem_wr(gb, DE, A);
  CYC(0x4589, 0x458a); E = alu_inc8(gb, E);
  CYC(0x458a, 0x458b); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x458b, 0x458c); mem_wr(gb, DE, A);
  CYC(0x458c, 0x458e); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x458e, 0x458f); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x458f, 0x4590); mem_wr(gb, DE, A);
  CALL_C(0x4590, ecom_updateAngleTowardTarget_b0e_hook, 0x43bf, 0x4593);
  CYC(0x4593, 0x4595); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(0x4595, playSound_b00_hook, 0x0c98, 0x4598);
  CYC(0x4598, 0x459b); objectSetVisiblec1_hook(gb); return; // jp

stateB:
  CALL_C(0x45a1, ecom_bounceOffScreenBoundary_b0e_hook, 0x42e5, 0x45a4);
  CYC(0x45a4, 0x45a6); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x45a6, 0x45a7); A = mem_rd(gb, DE);
  CYC(0x45a7, 0x45a8); C = A;
  CALL_C(0x45a8, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x45ab);
  if (!(F & FZ)) { CYCT(0x45ab, 0x45ae); ecom_applyVelocityForSideviewEnemy_b0e_hook(gb); return; } // jp nz
  CYC(0x45ab, 0x45ae);

gotoState8:
  CALL_C(0x45ae, getRandomNumber_noPreserveVars_hook, 0x0453, 0x45b1);
  CYC(0x45b1, 0x45b3); alu_and(gb, 0x7f);
  CYC(0x45b3, 0x45b4); H = D;
  CYC(0x45b4, 0x45b6); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x45b6, 0x45b7); alu_add(gb, mem_rd(gb, HL));
  CYC(0x45b7, 0x45b9); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x45b9, 0x45ba); mem_wr(gb, HL, A);
  CYC(0x45ba, 0x45bc); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x45bc, 0x45be); mem_wr(gb, HL, 0x08);
  CYC(0x45be, 0x45bf); alu_xor(gb, A);
  CALL_C(0x45bf, enemySetAnimation_hook, 0x282b, 0x45c2);
  CYC(0x45c2, 0x45c5); objectSetVisiblec2_hook(gb); return; // jp
}
