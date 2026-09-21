#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void enemyCode54_hook(GB *gb);
void ambiGuard_tossesLinkOut_hook(GB *gb);
void ambiGuard_tossesLinkOut_uninitialized_hook(GB *gb);
void ambiGuard_state_galeSeed_hook(GB *gb);
void ambiGuard_state_stub_hook(GB *gb);
void ambiGuard_state8_hook(GB *gb);
void ambiGuard_state9_hook(GB *gb);
void ambiGuard_stateA_hook(GB *gb);
void ambiGuard_stateB_hook(GB *gb);
void ambiGuard_stateC_hook(GB *gb);
void ambiGuard_stateE_hook(GB *gb);
void ambiGuard_animate_hook(GB *gb);
void ambiGuard_stateD_hook(GB *gb);
void ambiGuard_tossesLinkOut_stateF_hook(GB *gb);
void ambiGuard_tossesLinkOut_state10_hook(GB *gb);
void ambiGuard_tossesLinkOut_state11_hook(GB *gb);
void ambiGuard_attacksLink_hook(GB *gb);
void ambiGuard_attacksLink_state_uninitialized_hook(GB *gb);
void ambiGuard_attacksLink_stateF_hook(GB *gb);
void ambiGuard_attacksLink_state10_hook(GB *gb);
void ambiGuard_turnToFaceLink_hook(GB *gb);
void ambiGuard_attacksLink_state11_hook(GB *gb);
void ambiGuard_commonInitialization_hook(GB *gb);
void ambiGuard_runMovementScript_hook(GB *gb);
void ambiGuard_checkAlertTrigger_hook(GB *gb);
void ambiGuard_alertAllGuards_hook(GB *gb);
void ambiGuard_checkSpottedLink_hook(GB *gb);
void ambiGuard_createExclamationMark_hook(GB *gb);
void ambiGuard_collisionOccured_hook(GB *gb);
void ambiGuard_setAngle_hook(GB *gb);
void ambiGuard_directAttackOccurred_hook(GB *gb);
void ambiGuard_setCounter2ForAttackingTypeOnly_hook(GB *gb);
void ambiGuard_noHealth_hook(GB *gb);

static uint16_t ambiGuard_jump_table(GB *gb) {
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
// ENEMY_AMBI_GUARD
//
// Variables:
//   relatedObj2: PART_DETECTION_HELPER; checks when Link is visible.
//   var30-var31: Movement script address
//   var32: Y-destination (reserved by movement script)
//   var33: X-destination (reserved by movement script)
//   var34: Bit 0 set when Link should be noticed; Bit 1 set once the guard has started
//          reacting to Link (shown exclamation mark).
//   var35: Nonzero if just hit with an indirect attack (moves more quickly)
//   var36: While this is nonzero, all "normal code" is ignored. It counts down to zero,
//          and once it's done, it sets var35 to 1 (move more quickly) and normal code
//          resumes. Used for the delay between noticing Link and taking action.
//   var37: Timer until guard "notices" scent seed.
//   var3a: When set to $ff, faces PART_DETECTION_HELPER?
//   var3b: When set to $ff, the guard immediately notices Link. (Written to by
//          PART_DETECTION_HELPER.)
// ==================================================================================================
void enemyCode54_hook(GB *gb) {
  BASE(enemyCode54);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); ambiGuard_noHealth_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ecom_updateKnockback_b0e_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);
  CALL_C(b_+12, ambiGuard_collisionOccured_hook, SYM(ambiGuard_collisionOccured), b_+15);

normalStatus:
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_rlca(gb);
  if (F & FC) { CYCT(b_+19, b_+22); ambiGuard_attacksLink_hook(gb); return; } // jp c
  CYC(b_+19, b_+22);
  TAIL(ambiGuard_tossesLinkOut); // fallthrough
}

// 0e:70b4, bare global; falls into from enemyCode54. Subids $00-$7f.
void ambiGuard_tossesLinkOut_hook(GB *gb) {
  BASE(ambiGuard_tossesLinkOut);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ambiGuard_checkSpottedLink_hook, SYM(ambiGuard_checkSpottedLink), b_+3);
  CALL_C(b_+3, ambiGuard_checkAlertTrigger_hook, SYM(ambiGuard_checkAlertTrigger), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  {
    CYC(b_+9, b_+10); push_effect(gb, b_+10);
    uint16_t target = ambiGuard_jump_table(gb);
    if (target == SYM(ambiGuard_tossesLinkOut_uninitialized)) { ambiGuard_tossesLinkOut_uninitialized_hook(gb); return; }
    if (target == SYM(ambiGuard_state_stub)) { ambiGuard_state_stub_hook(gb); return; }
    if (target == SYM(ambiGuard_state_galeSeed)) { ambiGuard_state_galeSeed_hook(gb); return; }
    if (target == SYM(ambiGuard_state8)) { ambiGuard_state8_hook(gb); return; }
    if (target == SYM(ambiGuard_state9)) { ambiGuard_state9_hook(gb); return; }
    if (target == SYM(ambiGuard_stateA)) { ambiGuard_stateA_hook(gb); return; }
    if (target == SYM(ambiGuard_stateB)) { ambiGuard_stateB_hook(gb); return; }
    if (target == SYM(ambiGuard_stateC)) { ambiGuard_stateC_hook(gb); return; }
    if (target == SYM(ambiGuard_stateD)) { ambiGuard_stateD_hook(gb); return; }
    if (target == SYM(ambiGuard_tossesLinkOut_stateF)) { ambiGuard_tossesLinkOut_stateF_hook(gb); return; }
    if (target == SYM(ambiGuard_tossesLinkOut_state10)) { ambiGuard_tossesLinkOut_state10_hook(gb); return; }
    if (target == SYM(ambiGuard_tossesLinkOut_state11)) { ambiGuard_tossesLinkOut_state11_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:70e2, bare global; jump-table target from ambiGuard_tossesLinkOut.
void ambiGuard_tossesLinkOut_uninitialized_hook(GB *gb) {
  BASE(ambiGuard_tossesLinkOut_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(SYM(ambiGuard_tossesLinkOut_scriptTable)); // ambiGuard_tossesLinkOut_scriptTable
  CALL_C(b_+3, objectLoadMovementScript_hook, SYM(objectLoadMovementScript), b_+6);
  CALL_C(b_+6, ambiGuard_commonInitialization_hook, SYM(ambiGuard_commonInitialization), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } // ret nz
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+16); TAIL(enemySetAnimation); // jp
}

// 0e:70f2, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// NOTE: Guards don't seem to react to gale seeds? Is this unused?
void ambiGuard_state_galeSeed_hook(GB *gb) {
  BASE(ambiGuard_state_galeSeed);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_galeSeedEffect_b0e_hook, SYM(ecom_galeSeedEffect_b0e), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_or(gb, A);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_VAR35;
  if (F & FZ) { CALL_C_CC(b_+10, ambiGuard_alertAllGuards_hook, SYM(ambiGuard_alertAllGuards), b_+13); } else { CYC(b_+10, b_+13); } // call z
  CALL_C(b_+13, decNumEnemies_hook, SYM(decNumEnemies), b_+16);
  CYC(b_+16, b_+19); TAIL(enemyDelete); // jp
}

// 0e:7105, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
void ambiGuard_state_stub_hook(GB *gb) {
  BASE(ambiGuard_state_stub);
  RET(b_+0); return; // ret
}

// 0e:7106, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// Moving up.
void ambiGuard_state8_hook(GB *gb) {
  BASE(ambiGuard_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto reachedDestination8; } // jr nc
  CALL_C(b_+9, objectApplySpeed_hook, SYM(objectApplySpeed), b_+12);
  CYC(b_+12, b_+14); TAIL(ambiGuard_animate); // jr

reachedDestination8:
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+19); TAIL(ambiGuard_runMovementScript); // jp
}

// 0e:7119, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// Moving right.
void ambiGuard_state9_hook(GB *gb) {
  BASE(ambiGuard_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto reachedDestination9; } // jr nc
  CALL_C(b_+9, objectApplySpeed_hook, SYM(objectApplySpeed), b_+12);
  CYC(b_+12, b_+14); TAIL(ambiGuard_animate); // jr

reachedDestination9:
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+19); TAIL(ambiGuard_runMovementScript); // jp
}

// 0e:712c, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// Moving down.
void ambiGuard_stateA_hook(GB *gb) {
  BASE(ambiGuard_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_VAR32;
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto reachedDestinationA; } // jr nc
  CALL_C(b_+9, objectApplySpeed_hook, SYM(objectApplySpeed), b_+12);
  CYC(b_+12, b_+14); TAIL(ambiGuard_animate); // jr

reachedDestinationA:
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+19); TAIL(ambiGuard_runMovementScript); // jp
}

// 0e:713f, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// Moving left.
void ambiGuard_stateB_hook(GB *gb) {
  BASE(ambiGuard_stateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto reachedDestinationB; } // jr nc
  CALL_C(b_+9, objectApplySpeed_hook, SYM(objectApplySpeed), b_+12);
  CYC(b_+12, b_+14); TAIL(ambiGuard_animate); // jr

reachedDestinationB:
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+19); TAIL(ambiGuard_runMovementScript); // jp
}

// 0e:7152, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink
// (also as ambiGuard_stateE, an identical entry further down the same jump table). Waiting.
void ambiGuard_stateC_hook(GB *gb) {
  BASE(ambiGuard_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); ambiGuard_runMovementScript_hook(gb); return; } // jp z
  CYC(b_+3, b_+6);
  TAIL(ambiGuard_animate); // fallthrough
}

// 0e:7152, jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink; identical
// code to ambiGuard_stateC (see ambiGuard.s, where the two labels share one body).
void ambiGuard_stateE_hook(GB *gb) {
  TAIL(ambiGuard_stateC);
}

// 0e:7158, bare global; falls into from ambiGuard_stateC/E, also reached via tail-jump from
// ambiGuard_state8/9/A/B.
void ambiGuard_animate_hook(GB *gb) {
  BASE(ambiGuard_animate);
  CYC(b_+0, b_+3); TAIL(enemyAnimate); // jp
}

// 0e:715b, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// Standing in place for [counter1] frames, then turn the other way for 30 frames, then
// resume movement.
void ambiGuard_stateD_hook(GB *gb) {
  BASE(ambiGuard_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x1e); // 30
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_xor(gb, 0x10);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+18); A = alu_swap(gb, A);
  CYC(b_+18, b_+19); alu_rlca(gb);
  CYC(b_+19, b_+22); TAIL(enemySetAnimation); // jp
}

// 0e:7171, bare global; jump-table target from ambiGuard_tossesLinkOut. Begin moving toward
// Link after noticing him.
void ambiGuard_tossesLinkOut_stateF_hook(GB *gb) {
  BASE(ambiGuard_tossesLinkOut_stateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x5a); // 90
  CALL_C(b_+7, ambiGuard_turnToFaceLink_hook, SYM(ambiGuard_turnToFaceLink), b_+10);
  CYC(b_+10, b_+12); A = 0xcc; // SND_WHISTLE
  CYC(b_+12, b_+15); TAIL(playSound_b00); // jp
}

// 0e:7180, bare global; jump-table target from ambiGuard_tossesLinkOut. Moving toward Link
// until the screen fades out and Link gets booted out.
void ambiGuard_tossesLinkOut_state10_hook(GB *gb) {
  BASE(ambiGuard_tossesLinkOut_state10);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+6);
  if (F & FZ) { CYCT(b_+6, b_+8); goto boot; } // jr z
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); C = 0x18;
  CALL_C(b_+10, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+13);
  if (!(F & FC)) { CYCT(b_+13, b_+16); ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook(gb); return; } // jp nc
  CYC(b_+13, b_+16);

boot:
  CYC(b_+16, b_+18); A = 0x14; // CUTSCENE_BOOTED_FROM_PALACE
  CYC(b_+18, b_+21); mem_wr(gb, wCutsceneTrigger, A);
  RET(b_+21); return; // ret
}

// 0e:7196, bare global; jump-table target from ambiGuard_tossesLinkOut.
void ambiGuard_tossesLinkOut_state11_hook(GB *gb) {
  BASE(ambiGuard_tossesLinkOut_state11);
  RET(b_+0); return; // ret
}

// 0e:7197, bare global; jump-table target from enemyCode54.
void ambiGuard_attacksLink_hook(GB *gb) {
  BASE(ambiGuard_attacksLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ambiGuard_checkSpottedLink_hook, SYM(ambiGuard_checkSpottedLink), b_+3);
  CALL_C(b_+3, ambiGuard_checkAlertTrigger_hook, SYM(ambiGuard_checkAlertTrigger), b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  {
    CYC(b_+9, b_+10); push_effect(gb, b_+10);
    uint16_t target = ambiGuard_jump_table(gb);
    if (target == SYM(ambiGuard_attacksLink_state_uninitialized)) { ambiGuard_attacksLink_state_uninitialized_hook(gb); return; }
    if (target == SYM(ambiGuard_state_stub)) { ambiGuard_state_stub_hook(gb); return; }
    if (target == SYM(ambiGuard_state_galeSeed)) { ambiGuard_state_galeSeed_hook(gb); return; }
    if (target == SYM(ambiGuard_state8)) { ambiGuard_state8_hook(gb); return; }
    if (target == SYM(ambiGuard_state9)) { ambiGuard_state9_hook(gb); return; }
    if (target == SYM(ambiGuard_stateA)) { ambiGuard_stateA_hook(gb); return; }
    if (target == SYM(ambiGuard_stateB)) { ambiGuard_stateB_hook(gb); return; }
    if (target == SYM(ambiGuard_stateC)) { ambiGuard_stateC_hook(gb); return; }
    if (target == SYM(ambiGuard_stateD)) { ambiGuard_stateD_hook(gb); return; }
    if (target == SYM(ambiGuard_attacksLink_stateF)) { ambiGuard_attacksLink_stateF_hook(gb); return; }
    if (target == SYM(ambiGuard_attacksLink_state10)) { ambiGuard_attacksLink_state10_hook(gb); return; }
    if (target == SYM(ambiGuard_attacksLink_state11)) { ambiGuard_attacksLink_state11_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:71c5, bare global; jump-table target from ambiGuard_attacksLink.
void ambiGuard_attacksLink_state_uninitialized_hook(GB *gb) {
  BASE(ambiGuard_attacksLink_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+3, b_+5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+5, b_+8); SET_HL(SYM(ambiGuard_attacksLink_scriptTable)); // ambiGuard_attacksLink_scriptTable
  CALL_C(b_+8, objectLoadMovementScript_hook, SYM(objectLoadMovementScript), b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+14, b_+16); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(b_+16, ambiGuard_commonInitialization_hook, SYM(ambiGuard_commonInitialization), b_+19);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } // ret nz
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+26); TAIL(enemySetAnimation); // jp
}

// 0e:71df, bare global; jump-table target from ambiGuard_attacksLink. Just noticed Link.
void ambiGuard_attacksLink_stateF_hook(GB *gb) {
  BASE(ambiGuard_attacksLink_stateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto haveCounter2; } // jr nz
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x3c); // 60

haveCounter2:
  CALL_C(b_+11, ambiGuard_createExclamationMark_hook, SYM(ambiGuard_createExclamationMark), b_+14);
  CYC(b_+14, b_+16); TAIL(ambiGuard_turnToFaceLink); // jr
}

// 0e:71ef, bare global; jump-table target from ambiGuard_attacksLink. Looking at Link;
// counting down until he starts chasing him.
void ambiGuard_attacksLink_state10_hook(GB *gb) {
  BASE(ambiGuard_attacksLink_state10);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto beginChasing; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x3c); // 60
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); A = 0xcc; // SND_WHISTLE
  CALL_C(b_+11, playSound_b00_hook, SYM(playSound_b00), b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+16, b_+19); TAIL(ambiGuard_alertAllGuards); // jp

beginChasing:
  CYC(b_+19, b_+20); L = alu_dec8(gb, L);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x14); // [counter1] = 20
  CYC(b_+22, b_+23); L = E;
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x3d); // ENEMYCOLLISION_AMBI_GUARD_CHASING_LINK
  TAIL(ambiGuard_turnToFaceLink); // fallthrough
}

// 0e:720f, bare global; falls into from ambiGuard_attacksLink_state10, also called from
// several other states.
void ambiGuard_turnToFaceLink_hook(GB *gb) {
  BASE(ambiGuard_turnToFaceLink);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_updateCardinalAngleTowardTarget_b0e_hook, SYM(ecom_updateCardinalAngleTowardTarget_b0e), b_+3);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); alu_rlca(gb);
  CYC(b_+6, b_+9); TAIL(enemySetAnimation); // jp
}

// 0e:7218, bare global; jump-table target from ambiGuard_attacksLink. Currently chasing
// Link.
void ambiGuard_attacksLink_state11_hook(GB *gb) {
  BASE(ambiGuard_attacksLink_state11);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto stillChasing; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x14); // [counter1] = 20
  CALL_C(b_+7, ambiGuard_turnToFaceLink_hook, SYM(ambiGuard_turnToFaceLink), b_+10);

stillChasing:
  CALL_C(b_+10, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+13);
  CYC(b_+13, b_+16); TAIL(enemyAnimate); // jp
}

// 0e:7228, bare global; called from ambiGuard_tossesLinkOut_uninitialized and
// ambiGuard_attacksLink_state_uninitialized. Deletes self if Veran was defeated, otherwise
// spawns PART_DETECTION_HELPER.
// @param[out] zflag nz if caller should return immediately (deleted self)
void ambiGuard_commonInitialization_hook(GB *gb) {
  BASE(ambiGuard_commonInitialization);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+5, b_+7); goto notDefeated; } // jr z
  CALL_C(b_+7, enemyDelete_hook, SYM(enemyDelete), b_+10);
  CYC(b_+10, b_+11); alu_or(gb, D);
  RET(b_+11); return; // ret

notDefeated:
  CALL_C(b_+12, getFreePartSlot_hook, SYM(getFreePartSlot), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto noFreeSlot; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x0e); // PART_DETECTION_HELPER
  CYC(b_+19, b_+21); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+21, b_+23); A = 0x80; // Enemy.start == ENEMY_BASE
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+24, b_+25); mem_wr(gb, HL, D);
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(b_+27, b_+29); A = 0xc0; // Part.start == PART_BASE
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+31); E = alu_inc8(gb, E);
  CYC(b_+31, b_+32); A = H;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+36, b_+37); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+37, b_+39); A = alu_swap(gb, A);
  CYC(b_+39, b_+40); alu_rrca(gb);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CALL_C(b_+41, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+44);
  CYC(b_+44, b_+45); alu_xor(gb, A);
  RET(b_+45); return; // ret

noFreeSlot:
  CYC(b_+46, b_+48); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+48, b_+49); alu_xor(gb, A);
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  RET(b_+50); return; // ret
}

// 0e:725b, bare global; called from several states after a movement script step.
void ambiGuard_runMovementScript_hook(GB *gb) {
  BASE(ambiGuard_runMovementScript);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectRunMovementScript_hook, SYM(objectRunMovementScript), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_and(gb, 0x18);
  CYC(b_+8, b_+10); A = alu_swap(gb, A);
  CYC(b_+10, b_+11); alu_rlca(gb);
  CYC(b_+11, b_+14); TAIL(enemySetAnimation); // jp
}

// 0e:7269, bare global; called from ambiGuard_tossesLinkOut and ambiGuard_attacksLink. When
// var36 is nonzero, this counts it down, then sets var35 to nonzero when var36 reaches 0.
// (This alerts the guard to start moving faster.) Also, all other guards on-screen will be
// alerted this way. As long as var36 is nonzero, this "returns from caller" (discards the
// return address).
void ambiGuard_checkAlertTrigger_hook(GB *gb) {
  BASE(ambiGuard_checkAlertTrigger);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  SET_BC(POP(b_+6)); // pop bc -- discard caller's return address
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto stillCountingDown; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_sub(gb, 0x08);
  CYC(b_+17, b_+19); alu_cp(gb, 0x04);
  if (!(F & FC)) { RET_TAKEN(b_+19); return; } // ret nc
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+21); B = A;
  CYC(b_+21, b_+23); A = alu_swap(gb, A);
  CYC(b_+23, b_+24); alu_rrca(gb);
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); A = B;
  CYC(b_+28, b_+31); TAIL(enemySetAnimation); // jp

stillCountingDown:
  CYC(b_+31, b_+33); alu_cp(gb, 0x3b); // 59
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; } // ret nz
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+36); A = 0xce; // SND_MAKU_TREE_PAST
  CALL_C(b_+36, playSound_b00_hook, SYM(playSound_b00), b_+39);
  CYC(b_+39, b_+41); E = ENEMY_BASE + OBJ_VAR35;
  TAIL(ambiGuard_alertAllGuards); // fallthrough
}

// 0e:7292, bare global; falls into from ambiGuard_checkAlertTrigger, also called from
// ambiGuard_state_galeSeed and ambiGuard_attacksLink_state10.
// @param de Variable to set on the guards. "var34" to alert them to Link immediately,
//           "var35" to make them patrol faster.
void ambiGuard_alertAllGuards_hook(GB *gb) {
  BASE(ambiGuard_alertAllGuards);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL((w1ReservedInteraction0_var3f + 1)); // FIRST_ENEMY_INDEX,Enemy.enabled

nextGuard:
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_ID;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x54); // ENEMY_AMBI_GUARD
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto nextEnemy; } // jr nz
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = H;
  CYC(b_+11, b_+12); alu_cp(gb, D);
  if (F & FZ) { CYCT(b_+12, b_+14); goto nextEnemy; } // jr z
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); L = E;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto nextEnemy; } // jr nz
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+20, b_+22); alu_bit(gb, 0, L);
  if (F & FZ) { CYCT(b_+22, b_+24); goto nextEnemy; } // jr z
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x3c); // 60

nextEnemy:
  CYC(b_+28, b_+29); H = alu_inc8(gb, H);
  CYC(b_+29, b_+30); A = H;
  CYC(b_+30, b_+32); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(b_+32, b_+34); goto nextGuard; } // jr c
  CYC(b_+32, b_+34);
  RET(b_+34); return; // ret
}

// 0e:72b5, bare global; called from ambiGuard_tossesLinkOut and ambiGuard_attacksLink.
// Checks for spotting Link, among other things.
void ambiGuard_checkSpottedLink_hook(GB *gb) {
  BASE(ambiGuard_checkSpottedLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto scentSeed; } // jr nz

normalCheck:
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto faceLink; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_VAR3A;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto commonUpdate; } // jr nz
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A); // [var3a] = 0
  CYC(b_+19, b_+21); A = 0x0b; // Object.yh
  CALL_C(b_+21, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+24);
  CYC(b_+24, b_+25); B = mem_rd(gb, HL);
  CYC(b_+25, b_+27); L = 0x0d; // Object.xh
  CYC(b_+27, b_+28); C = mem_rd(gb, HL);
  CALL_C(b_+28, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+31);
  CYC(b_+31, b_+33); goto alertGuardToMoveFast; // jr

scentSeed:
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+38, b_+40); goto noticedScentSeed; } // jr z
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+43); A = mem_rd(gb, wFrameCounter);
  CYC(b_+43, b_+44); alu_rrca(gb);
  if (F & FC) { CYCT(b_+44, b_+46); goto normalCheck; } // jr c
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+47); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+47, b_+49); goto normalCheck; // jr

noticedScentSeed:
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x96); // [var37] = 150

faceLink:
  CALL_C(b_+51, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+54);

alertGuardToMoveFast:
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+57); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+57, b_+58); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+58, b_+59); L = alu_inc8(gb, L);
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x3c); // [var36] = 60
  CALL_C(b_+61, ambiGuard_setAngle_hook, SYM(ambiGuard_setAngle), b_+64);

commonUpdate:
  CYC(b_+64, b_+65); H = D;
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_VAR3B;
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x00);
  CYC(b_+70, b_+71); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+71, b_+73); goto checkNoticed; } // jr nz
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+75); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+75, b_+76); A = mem_rd(gb, HL);
  CYC(b_+76, b_+77); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+77, b_+79); goto checkNoticed; } // jr nz
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+80); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [var34]
  CALL_C(b_+80, ambiGuard_setCounter2ForAttackingTypeOnly_hook, SYM(ambiGuard_setCounter2ForAttackingTypeOnly), b_+83);

checkNoticed:
  CYC(b_+83, b_+85); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+87); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+87, b_+89); goto haventSeenLinkYet; } // jr nc
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+90); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+90); return; } // ret c
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+93); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+93, b_+95); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+95, b_+97); goto noticedLink; } // jr nz
  CYC(b_+95, b_+97);
  CALL_C(b_+97, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+100);
  if (!(F & FC)) { RET_TAKEN(b_+100); return; } // ret nc
  CYC(b_+100, b_+101);
  CYC(b_+101, b_+104); A = mem_rd(gb, w1Link + OBJ_ZH);
  CYC(b_+104, b_+105); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+105); return; } // ret c
  CYC(b_+105, b_+106);
  CYC(b_+106, b_+108); A = 0x80;
  CYC(b_+108, b_+111); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+111, b_+113); A = 0x21; // DISABLE_COMPANION|DISABLE_LINK
  CYC(b_+113, b_+116); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+116, b_+119); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+119, b_+121); E = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+121, b_+123); A = 0x3c; // 60
  CYC(b_+123, b_+124); mem_wr(gb, DE, A);
  CALL_C(b_+124, ambiGuard_createExclamationMark_hook, SYM(ambiGuard_createExclamationMark), b_+127);

noticedLink:
  CYC(b_+127, b_+128); H = D;
  CYC(b_+128, b_+130); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+130, b_+132); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1))); // set 1,(hl)
  CYC(b_+132, b_+134); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+134, b_+136); mem_wr(gb, HL, 0x0f);
  CYC(b_+136, b_+138); A = 0x29; // Object.health
  CALL_C(b_+138, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+141);
  CYC(b_+141, b_+143); mem_wr(gb, HL, 0x00);
  RET(b_+143); return; // ret

haventSeenLinkYet:
  CYC(b_+144, b_+145); E = alu_inc8(gb, E);
  CYC(b_+145, b_+146); A = mem_rd(gb, DE); // [var35]
  CYC(b_+146, b_+147); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+147); return; } // ret nc
  CYC(b_+147, b_+148);
  CYC(b_+148, b_+149); alu_xor(gb, A);
  CYC(b_+149, b_+150); mem_wr(gb, DE, A);
  CYC(b_+150, b_+152); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+152, b_+154); mem_wr(gb, HL, 0x32); // SPEED_140
  TAIL(ambiGuard_createExclamationMark); // fallthrough
}

// 0e:734f, bare global; falls into from ambiGuard_checkSpottedLink, also called from
// several other states.
void ambiGuard_createExclamationMark_hook(GB *gb) {
  BASE(ambiGuard_createExclamationMark);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x2d;
  CYC(b_+2, b_+5); SET_BC(0xf408);
  CYC(b_+5, b_+8); TAIL(objectCreateExclamationMark); // jp
}

// 0e:7357, bare global; called from enemyCode54.
void ambiGuard_collisionOccured_hook(GB *gb) {
  BASE(ambiGuard_collisionOccured);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+11); alu_cp(gb, 0x92); // collisions up to & including ITEMCOLLISION_11 are direct attacks
  if (F & FC) { CYCT(b_+11, b_+13); ambiGuard_directAttackOccurred_hook(gb); return; } // jr c
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); alu_cp(gb, 0x9e); // $80|ITEMCOLLISION_GALE_SEED
  if (F & FZ) { RET_TAKEN(b_+15); return; } // ret z
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+21); return; } // ret nz
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [var35] = 1
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x5a); // [var36] = 90
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); alu_xor(gb, 0x10);
  TAIL(ambiGuard_setAngle); // fallthrough
}

// 0e:7376, bare global; falls into from ambiGuard_collisionOccured, also called from
// ambiGuard_checkSpottedLink.
// @param a Angle
void ambiGuard_setAngle_hook(GB *gb) {
  BASE(ambiGuard_setAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_add(gb, 0x04);
  CYC(b_+2, b_+4); alu_and(gb, 0x18);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+10); alu_rlca(gb);
  CYC(b_+10, b_+13); TAIL(enemySetAnimation); // jp
}

// 0e:7383, bare global; falls into from ambiGuard_collisionOccured. A collision with one of
// Link's direct attacks (sword, fist, etc) occurred; the guard notices Link right away.
void ambiGuard_directAttackOccurred_hook(GB *gb) {
  BASE(ambiGuard_directAttackOccurred);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  TAIL(ambiGuard_setCounter2ForAttackingTypeOnly); // fallthrough
}

// 0e:7388, bare global; falls into from ambiGuard_directAttackOccurred, also called from
// ambiGuard_checkSpottedLink. Does some initialization for "attacking Link" type only, when
// they just notice Link.
void ambiGuard_setCounter2ForAttackingTypeOnly_hook(GB *gb) {
  BASE(ambiGuard_setCounter2ForAttackingTypeOnly);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+4); return; } // ret nc
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+7, b_+9); A = 0x5a; // 90
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return; // ret
}

// 0e:7397, bare global; called from enemyCode54. Scampering away when health is 0.
void ambiGuard_noHealth_hook(GB *gb) {
  BASE(ambiGuard_noHealth);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = ambiGuard_jump_table(gb);
    if (target == b_+10) goto substate0;
    if (target == b_+20) goto substate1;
    if (target == b_+56) goto substate2;
    HANDOFF(target);
  }

substate0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+15, b_+17); A = 0x00;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0xff);

substate1:
  CYC(b_+20, b_+22); C = 0x20;
  CALL_C(b_+22, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+25);
  if (!(F & FZ)) { RET_TAKEN(b_+25); return; } // ret nz
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+31, b_+33); A = 0x40;
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a -- speedZ = -$1c0
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0xfe);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+48); alu_add(gb, 0x04);
  CYC(b_+48, b_+50); alu_and(gb, 0x18);
  CYC(b_+50, b_+52); A = alu_swap(gb, A);
  CYC(b_+52, b_+53); alu_rlca(gb);
  CYC(b_+53, b_+56); TAIL(enemySetAnimation); // jp

substate2:
  CYC(b_+56, b_+58); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+61); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(b_+61, b_+64); enemyDelete_hook(gb); return; } // jp nc
  CYC(b_+61, b_+64);
  CYC(b_+64, b_+66); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+69); alu_cp(gb, 0xf0); // LARGE_ROOM_WIDTH<<4
  if (!(F & FC)) { CYCT(b_+69, b_+72); enemyDelete_hook(gb); return; } // jp nc
  CYC(b_+69, b_+72);
  CALL_C(b_+72, objectApplySpeed_hook, SYM(objectApplySpeed), b_+75);
  CYC(b_+75, b_+77); C = 0x20;
  CALL_C(b_+77, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+80);
  if (!(F & FZ)) { CYCT(b_+80, b_+83); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+80, b_+83);
  CYC(b_+83, b_+85); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+85, b_+87); A = 0x40;
  CYC(b_+87, b_+88); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a -- speedZ = -$1c0
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0xfe);
  RET(b_+90); return; // ret
}
