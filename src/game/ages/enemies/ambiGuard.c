#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x709e, 0x70a0); goto normalStatus; } // jr z
  CYC(0x709e, 0x70a0);
  CYC(0x70a0, 0x70a2); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x70a2); return; } // ret c
  CYC(0x70a2, 0x70a3);
  if (F & FZ) { CYCT(0x70a3, 0x70a6); ambiGuard_noHealth_hook(gb); return; } // jp z
  CYC(0x70a3, 0x70a6);
  CYC(0x70a6, 0x70a7); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x70a7, 0x70aa); ecom_updateKnockback_b0e_hook(gb); return; } // jp nz
  CYC(0x70a7, 0x70aa);
  CALL_C(0x70aa, ambiGuard_collisionOccured_hook, 0x7357, 0x70ad);

normalStatus:
  CYC(0x70ad, 0x70af); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x70af, 0x70b0); A = mem_rd(gb, DE);
  CYC(0x70b0, 0x70b1); alu_rlca(gb);
  if (F & FC) { CYCT(0x70b1, 0x70b4); ambiGuard_attacksLink_hook(gb); return; } // jp c
  CYC(0x70b1, 0x70b4);
  ambiGuard_tossesLinkOut_hook(gb); return; // fallthrough
}

// 0e:70b4, bare global; falls into from enemyCode54. Subids $00-$7f.
void ambiGuard_tossesLinkOut_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x70b4, ambiGuard_checkSpottedLink_hook, 0x72b5, 0x70b7);
  CALL_C(0x70b7, ambiGuard_checkAlertTrigger_hook, 0x7269, 0x70ba);
  CYC(0x70ba, 0x70bc); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x70bc, 0x70bd); A = mem_rd(gb, DE);
  {
    CYC(0x70bd, 0x70be); push_effect(gb, 0x70be);
    uint16_t target = ambiGuard_jump_table(gb);
    if (target == 0x70e2) { ambiGuard_tossesLinkOut_uninitialized_hook(gb); return; }
    if (target == 0x7105) { ambiGuard_state_stub_hook(gb); return; }
    if (target == 0x70f2) { ambiGuard_state_galeSeed_hook(gb); return; }
    if (target == 0x7106) { ambiGuard_state8_hook(gb); return; }
    if (target == 0x7119) { ambiGuard_state9_hook(gb); return; }
    if (target == 0x712c) { ambiGuard_stateA_hook(gb); return; }
    if (target == 0x713f) { ambiGuard_stateB_hook(gb); return; }
    if (target == 0x7152) { ambiGuard_stateC_hook(gb); return; }
    if (target == 0x715b) { ambiGuard_stateD_hook(gb); return; }
    if (target == 0x7171) { ambiGuard_tossesLinkOut_stateF_hook(gb); return; }
    if (target == 0x7180) { ambiGuard_tossesLinkOut_state10_hook(gb); return; }
    if (target == 0x7196) { ambiGuard_tossesLinkOut_state11_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:70e2, bare global; jump-table target from ambiGuard_tossesLinkOut.
void ambiGuard_tossesLinkOut_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x70e2, 0x70e5); SET_HL(0x73f2); // ambiGuard_tossesLinkOut_scriptTable
  CALL_C(0x70e5, objectLoadMovementScript_hook, 0x3035, 0x70e8);
  CALL_C(0x70e8, ambiGuard_commonInitialization_hook, 0x7228, 0x70eb);
  if (!(F & FZ)) { RET_TAKEN(0x70eb); return; } // ret nz
  CYC(0x70eb, 0x70ec);
  CYC(0x70ec, 0x70ee); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x70ee, 0x70ef); A = mem_rd(gb, DE);
  CYC(0x70ef, 0x70f2); enemySetAnimation_hook(gb); return; // jp
}

// 0e:70f2, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// NOTE: Guards don't seem to react to gale seeds? Is this unused?
void ambiGuard_state_galeSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x70f2, ecom_galeSeedEffect_b0e_hook, 0x447b, 0x70f5);
  if (F & FC) { RET_TAKEN(0x70f5); return; } // ret c
  CYC(0x70f5, 0x70f6);
  CYC(0x70f6, 0x70f8); E = ENEMY_BASE + OBJ_VAR34;
  CYC(0x70f8, 0x70f9); A = mem_rd(gb, DE);
  CYC(0x70f9, 0x70fa); alu_or(gb, A);
  CYC(0x70fa, 0x70fc); E = ENEMY_BASE + OBJ_VAR35;
  if (F & FZ) { CALL_C_CC(0x70fc, ambiGuard_alertAllGuards_hook, 0x7292, 0x70ff); } else { CYC(0x70fc, 0x70ff); } // call z
  CALL_C(0x70ff, decNumEnemies_hook, 0x24b3, 0x7102);
  CYC(0x7102, 0x7105); enemyDelete_hook(gb); return; // jp
}

// 0e:7105, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
void ambiGuard_state_stub_hook(GB *gb) {
  RET(0x7105); return; // ret
}

// 0e:7106, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// Moving up.
void ambiGuard_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7106, 0x7108); E = ENEMY_BASE + OBJ_VAR32;
  CYC(0x7108, 0x7109); A = mem_rd(gb, DE);
  CYC(0x7109, 0x710a); H = D;
  CYC(0x710a, 0x710c); L = ENEMY_BASE + OBJ_YH;
  CYC(0x710c, 0x710d); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x710d, 0x710f); goto reachedDestination8; } // jr nc
  CALL_C(0x710f, objectApplySpeed_hook, 0x201d, 0x7112);
  CYC(0x7112, 0x7114); ambiGuard_animate_hook(gb); return; // jr

reachedDestination8:
  CYC(0x7114, 0x7115); A = mem_rd(gb, DE);
  CYC(0x7115, 0x7116); mem_wr(gb, HL, A);
  CYC(0x7116, 0x7119); ambiGuard_runMovementScript_hook(gb); return; // jp
}

// 0e:7119, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// Moving right.
void ambiGuard_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7119, 0x711b); E = ENEMY_BASE + OBJ_XH;
  CYC(0x711b, 0x711c); A = mem_rd(gb, DE);
  CYC(0x711c, 0x711d); H = D;
  CYC(0x711d, 0x711f); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x711f, 0x7120); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x7120, 0x7122); goto reachedDestination9; } // jr nc
  CALL_C(0x7122, objectApplySpeed_hook, 0x201d, 0x7125);
  CYC(0x7125, 0x7127); ambiGuard_animate_hook(gb); return; // jr

reachedDestination9:
  CYC(0x7127, 0x7128); A = mem_rd(gb, HL);
  CYC(0x7128, 0x7129); mem_wr(gb, DE, A);
  CYC(0x7129, 0x712c); ambiGuard_runMovementScript_hook(gb); return; // jp
}

// 0e:712c, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// Moving down.
void ambiGuard_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x712c, 0x712e); E = ENEMY_BASE + OBJ_YH;
  CYC(0x712e, 0x712f); A = mem_rd(gb, DE);
  CYC(0x712f, 0x7130); H = D;
  CYC(0x7130, 0x7132); L = ENEMY_BASE + OBJ_VAR32;
  CYC(0x7132, 0x7133); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x7133, 0x7135); goto reachedDestinationA; } // jr nc
  CALL_C(0x7135, objectApplySpeed_hook, 0x201d, 0x7138);
  CYC(0x7138, 0x713a); ambiGuard_animate_hook(gb); return; // jr

reachedDestinationA:
  CYC(0x713a, 0x713b); A = mem_rd(gb, HL);
  CYC(0x713b, 0x713c); mem_wr(gb, DE, A);
  CYC(0x713c, 0x713f); ambiGuard_runMovementScript_hook(gb); return; // jp
}

// 0e:713f, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// Moving left.
void ambiGuard_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x713f, 0x7141); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7141, 0x7142); A = mem_rd(gb, DE);
  CYC(0x7142, 0x7143); H = D;
  CYC(0x7143, 0x7145); L = ENEMY_BASE + OBJ_XH;
  CYC(0x7145, 0x7146); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x7146, 0x7148); goto reachedDestinationB; } // jr nc
  CALL_C(0x7148, objectApplySpeed_hook, 0x201d, 0x714b);
  CYC(0x714b, 0x714d); ambiGuard_animate_hook(gb); return; // jr

reachedDestinationB:
  CYC(0x714d, 0x714e); A = mem_rd(gb, DE);
  CYC(0x714e, 0x714f); mem_wr(gb, HL, A);
  CYC(0x714f, 0x7152); ambiGuard_runMovementScript_hook(gb); return; // jp
}

// 0e:7152, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink
// (also as ambiGuard_stateE, an identical entry further down the same jump table). Waiting.
void ambiGuard_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7152, ecom_decCounter1_b0e_hook, 0x439a, 0x7155);
  if (F & FZ) { CYCT(0x7155, 0x7158); ambiGuard_runMovementScript_hook(gb); return; } // jp z
  CYC(0x7155, 0x7158);
  ambiGuard_animate_hook(gb); return; // fallthrough
}

// 0e:7152, jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink; identical
// code to ambiGuard_stateC (see ambiGuard.s, where the two labels share one body).
void ambiGuard_stateE_hook(GB *gb) {
  ambiGuard_stateC_hook(gb); return;
}

// 0e:7158, bare global; falls into from ambiGuard_stateC/E, also reached via tail-jump from
// ambiGuard_state8/9/A/B.
void ambiGuard_animate_hook(GB *gb) {
  CYC(0x7158, 0x715b); enemyAnimate_hook(gb); return; // jp
}

// 0e:715b, bare global; jump-table target from ambiGuard_tossesLinkOut/ambiGuard_attacksLink.
// Standing in place for [counter1] frames, then turn the other way for 30 frames, then
// resume movement.
void ambiGuard_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x715b, ecom_decCounter1_b0e_hook, 0x439a, 0x715e);
  if (!(F & FZ)) { RET_TAKEN(0x715e); return; } // ret nz
  CYC(0x715e, 0x715f);
  CYC(0x715f, 0x7160); L = E;
  CYC(0x7160, 0x7161); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x7161, 0x7163); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7163, 0x7165); mem_wr(gb, HL, 0x1e); // 30
  CYC(0x7165, 0x7167); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7167, 0x7168); A = mem_rd(gb, HL);
  CYC(0x7168, 0x716a); alu_xor(gb, 0x10);
  CYC(0x716a, 0x716b); mem_wr(gb, HL, A);
  CYC(0x716b, 0x716d); A = alu_swap(gb, A);
  CYC(0x716d, 0x716e); alu_rlca(gb);
  CYC(0x716e, 0x7171); enemySetAnimation_hook(gb); return; // jp
}

// 0e:7171, bare global; jump-table target from ambiGuard_tossesLinkOut. Begin moving toward
// Link after noticing him.
void ambiGuard_tossesLinkOut_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7171, 0x7172); H = D;
  CYC(0x7172, 0x7173); L = E;
  CYC(0x7173, 0x7174); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x7174, 0x7176); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7176, 0x7178); mem_wr(gb, HL, 0x5a); // 90
  CALL_C(0x7178, ambiGuard_turnToFaceLink_hook, 0x720f, 0x717b);
  CYC(0x717b, 0x717d); A = 0xcc; // SND_WHISTLE
  CYC(0x717d, 0x7180); playSound_b00_hook(gb); return; // jp
}

// 0e:7180, bare global; jump-table target from ambiGuard_tossesLinkOut. Moving toward Link
// until the screen fades out and Link gets booted out.
void ambiGuard_tossesLinkOut_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7180, enemyAnimate_hook, 0x2818, 0x7183);
  CALL_C(0x7183, ecom_decCounter1_b0e_hook, 0x439a, 0x7186);
  if (F & FZ) { CYCT(0x7186, 0x7188); goto boot; } // jr z
  CYC(0x7186, 0x7188);
  CYC(0x7188, 0x718a); C = 0x18;
  CALL_C(0x718a, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x718d);
  if (!(F & FC)) { CYCT(0x718d, 0x7190); ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook(gb); return; } // jp nc
  CYC(0x718d, 0x7190);

boot:
  CYC(0x7190, 0x7192); A = 0x14; // CUTSCENE_BOOTED_FROM_PALACE
  CYC(0x7192, 0x7195); mem_wr(gb, wCutsceneTrigger, A);
  RET(0x7195); return; // ret
}

// 0e:7196, bare global; jump-table target from ambiGuard_tossesLinkOut.
void ambiGuard_tossesLinkOut_state11_hook(GB *gb) {
  RET(0x7196); return; // ret
}

// 0e:7197, bare global; jump-table target from enemyCode54.
void ambiGuard_attacksLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7197, ambiGuard_checkSpottedLink_hook, 0x72b5, 0x719a);
  CALL_C(0x719a, ambiGuard_checkAlertTrigger_hook, 0x7269, 0x719d);
  CYC(0x719d, 0x719f); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x719f, 0x71a0); A = mem_rd(gb, DE);
  {
    CYC(0x71a0, 0x71a1); push_effect(gb, 0x71a1);
    uint16_t target = ambiGuard_jump_table(gb);
    if (target == 0x71c5) { ambiGuard_attacksLink_state_uninitialized_hook(gb); return; }
    if (target == 0x7105) { ambiGuard_state_stub_hook(gb); return; }
    if (target == 0x70f2) { ambiGuard_state_galeSeed_hook(gb); return; }
    if (target == 0x7106) { ambiGuard_state8_hook(gb); return; }
    if (target == 0x7119) { ambiGuard_state9_hook(gb); return; }
    if (target == 0x712c) { ambiGuard_stateA_hook(gb); return; }
    if (target == 0x713f) { ambiGuard_stateB_hook(gb); return; }
    if (target == 0x7152) { ambiGuard_stateC_hook(gb); return; }
    if (target == 0x715b) { ambiGuard_stateD_hook(gb); return; }
    if (target == 0x71df) { ambiGuard_attacksLink_stateF_hook(gb); return; }
    if (target == 0x71ef) { ambiGuard_attacksLink_state10_hook(gb); return; }
    if (target == 0x7218) { ambiGuard_attacksLink_state11_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:71c5, bare global; jump-table target from ambiGuard_attacksLink.
void ambiGuard_attacksLink_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71c5, 0x71c6); H = D;
  CYC(0x71c6, 0x71c8); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x71c8, 0x71ca); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x71ca, 0x71cd); SET_HL(0x7505); // ambiGuard_attacksLink_scriptTable
  CALL_C(0x71cd, objectLoadMovementScript_hook, 0x3035, 0x71d0);
  CYC(0x71d0, 0x71d1); H = D;
  CYC(0x71d1, 0x71d3); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x71d3, 0x71d5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CALL_C(0x71d5, ambiGuard_commonInitialization_hook, 0x7228, 0x71d8);
  if (!(F & FZ)) { RET_TAKEN(0x71d8); return; } // ret nz
  CYC(0x71d8, 0x71d9);
  CYC(0x71d9, 0x71db); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x71db, 0x71dc); A = mem_rd(gb, DE);
  CYC(0x71dc, 0x71df); enemySetAnimation_hook(gb); return; // jp
}

// 0e:71df, bare global; jump-table target from ambiGuard_attacksLink. Just noticed Link.
void ambiGuard_attacksLink_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x71df, 0x71e0); H = D;
  CYC(0x71e0, 0x71e1); L = E;
  CYC(0x71e1, 0x71e2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x71e2, 0x71e4); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x71e4, 0x71e5); A = mem_rd(gb, HL);
  CYC(0x71e5, 0x71e6); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x71e6, 0x71e8); goto haveCounter2; } // jr nz
  CYC(0x71e6, 0x71e8);
  CYC(0x71e8, 0x71ea); mem_wr(gb, HL, 0x3c); // 60

haveCounter2:
  CALL_C(0x71ea, ambiGuard_createExclamationMark_hook, 0x734f, 0x71ed);
  CYC(0x71ed, 0x71ef); ambiGuard_turnToFaceLink_hook(gb); return; // jr
}

// 0e:71ef, bare global; jump-table target from ambiGuard_attacksLink. Looking at Link;
// counting down until he starts chasing him.
void ambiGuard_attacksLink_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x71ef, ecom_decCounter2_b0e_hook, 0x43a3, 0x71f2);
  if (F & FZ) { CYCT(0x71f2, 0x71f4); goto beginChasing; } // jr z
  CYC(0x71f2, 0x71f4);
  CYC(0x71f4, 0x71f5); A = mem_rd(gb, HL);
  CYC(0x71f5, 0x71f7); alu_cp(gb, 0x3c); // 60
  if (!(F & FZ)) { RET_TAKEN(0x71f7); return; } // ret nz
  CYC(0x71f7, 0x71f8);
  CYC(0x71f8, 0x71fa); A = 0xcc; // SND_WHISTLE
  CALL_C(0x71fa, playSound_b00_hook, 0x0c98, 0x71fd);
  CYC(0x71fd, 0x71ff); E = ENEMY_BASE + OBJ_VAR34;
  CYC(0x71ff, 0x7202); ambiGuard_alertAllGuards_hook(gb); return; // jp

beginChasing:
  CYC(0x7202, 0x7203); L = alu_dec8(gb, L);
  CYC(0x7203, 0x7205); mem_wr(gb, HL, 0x14); // [counter1] = 20
  CYC(0x7205, 0x7206); L = E;
  CYC(0x7206, 0x7207); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x7207, 0x7209); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x7209, 0x720b); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x720b, 0x720d); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x720d, 0x720f); mem_wr(gb, HL, 0x3d); // ENEMYCOLLISION_AMBI_GUARD_CHASING_LINK
  ambiGuard_turnToFaceLink_hook(gb); return; // fallthrough
}

// 0e:720f, bare global; falls into from ambiGuard_attacksLink_state10, also called from
// several other states.
void ambiGuard_turnToFaceLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x720f, ecom_updateCardinalAngleTowardTarget_b0e_hook, 0x43b4, 0x7212);
  CYC(0x7212, 0x7214); A = alu_swap(gb, A);
  CYC(0x7214, 0x7215); alu_rlca(gb);
  CYC(0x7215, 0x7218); enemySetAnimation_hook(gb); return; // jp
}

// 0e:7218, bare global; jump-table target from ambiGuard_attacksLink. Currently chasing
// Link.
void ambiGuard_attacksLink_state11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7218, ecom_decCounter1_b0e_hook, 0x439a, 0x721b);
  if (!(F & FZ)) { CYCT(0x721b, 0x721d); goto stillChasing; } // jr nz
  CYC(0x721b, 0x721d);
  CYC(0x721d, 0x721f); mem_wr(gb, HL, 0x14); // [counter1] = 20
  CALL_C(0x721f, ambiGuard_turnToFaceLink_hook, 0x720f, 0x7222);

stillChasing:
  CALL_C(0x7222, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x7225);
  CYC(0x7225, 0x7228); enemyAnimate_hook(gb); return; // jp
}

// 0e:7228, bare global; called from ambiGuard_tossesLinkOut_uninitialized and
// ambiGuard_attacksLink_state_uninitialized. Deletes self if Veran was defeated, otherwise
// spawns PART_DETECTION_HELPER.
// @param[out] zflag nz if caller should return immediately (deleted self)
void ambiGuard_commonInitialization_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7228, 0x722b); SET_HL(wGroup4RoomFlags + 0xfc);
  CYC(0x722b, 0x722d); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x722d, 0x722f); goto notDefeated; } // jr z
  CALL_C(0x722f, enemyDelete_hook, 0x2e47, 0x7232);
  CYC(0x7232, 0x7233); alu_or(gb, D);
  RET(0x7233); return; // ret

notDefeated:
  CALL_C(0x7234, getFreePartSlot_hook, 0x3e8e, 0x7237);
  if (!(F & FZ)) { CYCT(0x7237, 0x7239); goto noFreeSlot; } // jr nz
  CYC(0x7237, 0x7239);
  CYC(0x7239, 0x723b); mem_wr(gb, HL, 0x0e); // PART_DETECTION_HELPER
  CYC(0x723b, 0x723d); L = PART_BASE + OBJ_RELATED1;
  CYC(0x723d, 0x723f); A = 0x80; // Enemy.start == ENEMY_BASE
  CYC(0x723f, 0x7240); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7240, 0x7241); mem_wr(gb, HL, D);
  CYC(0x7241, 0x7243); E = ENEMY_BASE + OBJ_RELATED2;
  CYC(0x7243, 0x7245); A = 0xc0; // Part.start == PART_BASE
  CYC(0x7245, 0x7246); mem_wr(gb, DE, A);
  CYC(0x7246, 0x7247); E = alu_inc8(gb, E);
  CYC(0x7247, 0x7248); A = H;
  CYC(0x7248, 0x7249); mem_wr(gb, DE, A);
  CYC(0x7249, 0x724a); H = D;
  CYC(0x724a, 0x724c); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x724c, 0x724d); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x724d, 0x724f); A = alu_swap(gb, A);
  CYC(0x724f, 0x7250); alu_rrca(gb);
  CYC(0x7250, 0x7251); mem_wr(gb, HL, A);
  CALL_C(0x7251, objectSetVisiblec2_hook, 0x1e45, 0x7254);
  CYC(0x7254, 0x7255); alu_xor(gb, A);
  RET(0x7255); return; // ret

noFreeSlot:
  CYC(0x7256, 0x7258); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x7258, 0x7259); alu_xor(gb, A);
  CYC(0x7259, 0x725a); mem_wr(gb, DE, A);
  RET(0x725a); return; // ret
}

// 0e:725b, bare global; called from several states after a movement script step.
void ambiGuard_runMovementScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x725b, objectRunMovementScript_hook, 0x3049, 0x725e);
  CYC(0x725e, 0x7260); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7260, 0x7261); A = mem_rd(gb, DE);
  CYC(0x7261, 0x7263); alu_and(gb, 0x18);
  CYC(0x7263, 0x7265); A = alu_swap(gb, A);
  CYC(0x7265, 0x7266); alu_rlca(gb);
  CYC(0x7266, 0x7269); enemySetAnimation_hook(gb); return; // jp
}

// 0e:7269, bare global; called from ambiGuard_tossesLinkOut and ambiGuard_attacksLink. When
// var36 is nonzero, this counts it down, then sets var35 to nonzero when var36 reaches 0.
// (This alerts the guard to start moving faster.) Also, all other guards on-screen will be
// alerted this way. As long as var36 is nonzero, this "returns from caller" (discards the
// return address).
void ambiGuard_checkAlertTrigger_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7269, 0x726a); H = D;
  CYC(0x726a, 0x726c); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x726c, 0x726d); A = mem_rd(gb, HL);
  CYC(0x726d, 0x726e); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x726e); return; } // ret z
  CYC(0x726e, 0x726f);
  SET_BC(POP(0x726f)); // pop bc -- discard caller's return address
  CYC(0x7270, 0x7271); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x7271, 0x7272); A = mem_rd(gb, HL);
  CYC(0x7272, 0x7273); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x7273, 0x7275); goto stillCountingDown; } // jr nz
  CYC(0x7273, 0x7275);
  CYC(0x7275, 0x7277); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7277, 0x7278); A = mem_rd(gb, HL);
  CYC(0x7278, 0x727a); alu_sub(gb, 0x08);
  CYC(0x727a, 0x727c); alu_cp(gb, 0x04);
  if (!(F & FC)) { RET_TAKEN(0x727c); return; } // ret nc
  CYC(0x727c, 0x727d);
  CYC(0x727d, 0x727e); B = A;
  CYC(0x727e, 0x7280); A = alu_swap(gb, A);
  CYC(0x7280, 0x7281); alu_rrca(gb);
  CYC(0x7281, 0x7283); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7283, 0x7284); mem_wr(gb, DE, A);
  CYC(0x7284, 0x7285); A = B;
  CYC(0x7285, 0x7288); enemySetAnimation_hook(gb); return; // jp

stillCountingDown:
  CYC(0x7288, 0x728a); alu_cp(gb, 0x3b); // 59
  if (!(F & FZ)) { RET_TAKEN(0x728a); return; } // ret nz
  CYC(0x728a, 0x728b);
  CYC(0x728b, 0x728d); A = 0xce; // SND_MAKU_TREE_PAST
  CALL_C(0x728d, playSound_b00_hook, 0x0c98, 0x7290);
  CYC(0x7290, 0x7292); E = ENEMY_BASE + OBJ_VAR35;
  ambiGuard_alertAllGuards_hook(gb); return; // fallthrough
}

// 0e:7292, bare global; falls into from ambiGuard_checkAlertTrigger, also called from
// ambiGuard_state_galeSeed and ambiGuard_attacksLink_state10.
// @param de Variable to set on the guards. "var34" to alert them to Link immediately,
//           "var35" to make them patrol faster.
void ambiGuard_alertAllGuards_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7292, 0x7295); SET_HL(0xd080); // FIRST_ENEMY_INDEX,Enemy.enabled

nextGuard:
  CYC(0x7295, 0x7297); L = ENEMY_BASE + OBJ_ID;
  CYC(0x7297, 0x7298); A = mem_rd(gb, HL);
  CYC(0x7298, 0x729a); alu_cp(gb, 0x54); // ENEMY_AMBI_GUARD
  if (!(F & FZ)) { CYCT(0x729a, 0x729c); goto nextEnemy; } // jr nz
  CYC(0x729a, 0x729c);
  CYC(0x729c, 0x729d); A = H;
  CYC(0x729d, 0x729e); alu_cp(gb, D);
  if (F & FZ) { CYCT(0x729e, 0x72a0); goto nextEnemy; } // jr z
  CYC(0x729e, 0x72a0);
  CYC(0x72a0, 0x72a1); L = E;
  CYC(0x72a1, 0x72a2); A = mem_rd(gb, HL);
  CYC(0x72a2, 0x72a3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x72a3, 0x72a5); goto nextEnemy; } // jr nz
  CYC(0x72a3, 0x72a5);
  CYC(0x72a5, 0x72a6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x72a6, 0x72a8); alu_bit(gb, 0, L);
  if (F & FZ) { CYCT(0x72a8, 0x72aa); goto nextEnemy; } // jr z
  CYC(0x72a8, 0x72aa);
  CYC(0x72aa, 0x72ac); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x72ac, 0x72ae); mem_wr(gb, HL, 0x3c); // 60

nextEnemy:
  CYC(0x72ae, 0x72af); H = alu_inc8(gb, H);
  CYC(0x72af, 0x72b0); A = H;
  CYC(0x72b0, 0x72b2); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  if (F & FC) { CYCT(0x72b2, 0x72b4); goto nextGuard; } // jr c
  CYC(0x72b2, 0x72b4);
  RET(0x72b4); return; // ret
}

// 0e:72b5, bare global; called from ambiGuard_tossesLinkOut and ambiGuard_attacksLink.
// Checks for spotting Link, among other things.
void ambiGuard_checkSpottedLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x72b5, 0x72b8); A = mem_rd(gb, wScentSeedActive);
  CYC(0x72b8, 0x72b9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x72b9, 0x72bb); goto scentSeed; } // jr nz

normalCheck:
  CYC(0x72b9, 0x72bb);
  CYC(0x72bb, 0x72be); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(0x72be, 0x72bf); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x72bf, 0x72c1); goto faceLink; } // jr nz
  CYC(0x72bf, 0x72c1);
  CYC(0x72c1, 0x72c3); E = ENEMY_BASE + OBJ_VAR3A;
  CYC(0x72c3, 0x72c4); A = mem_rd(gb, DE);
  CYC(0x72c4, 0x72c5); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x72c5, 0x72c7); goto commonUpdate; } // jr nz
  CYC(0x72c5, 0x72c7);
  CYC(0x72c7, 0x72c8); mem_wr(gb, DE, A); // [var3a] = 0
  CYC(0x72c8, 0x72ca); A = 0x0b; // Object.yh
  CALL_C(0x72ca, objectGetRelatedObject2Var_hook, 0x2164, 0x72cd);
  CYC(0x72cd, 0x72ce); B = mem_rd(gb, HL);
  CYC(0x72ce, 0x72d0); L = 0x0d; // Object.xh
  CYC(0x72d0, 0x72d1); C = mem_rd(gb, HL);
  CALL_C(0x72d1, objectGetRelativeAngle_hook, 0x1ea4, 0x72d4);
  CYC(0x72d4, 0x72d6); goto alertGuardToMoveFast; // jr

scentSeed:
  CYC(0x72d6, 0x72d7); H = D;
  CYC(0x72d7, 0x72d9); L = ENEMY_BASE + OBJ_VAR37;
  CYC(0x72d9, 0x72da); A = mem_rd(gb, HL);
  CYC(0x72da, 0x72db); alu_or(gb, A);
  if (F & FZ) { CYCT(0x72db, 0x72dd); goto noticedScentSeed; } // jr z
  CYC(0x72db, 0x72dd);
  CYC(0x72dd, 0x72e0); A = mem_rd(gb, wFrameCounter);
  CYC(0x72e0, 0x72e1); alu_rrca(gb);
  if (F & FC) { CYCT(0x72e1, 0x72e3); goto normalCheck; } // jr c
  CYC(0x72e1, 0x72e3);
  CYC(0x72e3, 0x72e4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x72e4, 0x72e6); goto normalCheck; // jr

noticedScentSeed:
  CYC(0x72e6, 0x72e8); mem_wr(gb, HL, 0x96); // [var37] = 150

faceLink:
  CALL_C(0x72e8, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x72eb);

alertGuardToMoveFast:
  CYC(0x72eb, 0x72ec); H = D;
  CYC(0x72ec, 0x72ee); L = ENEMY_BASE + OBJ_VAR35;
  CYC(0x72ee, 0x72ef); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x72ef, 0x72f0); L = alu_inc8(gb, L);
  CYC(0x72f0, 0x72f2); mem_wr(gb, HL, 0x3c); // [var36] = 60
  CALL_C(0x72f2, ambiGuard_setAngle_hook, 0x7376, 0x72f5);

commonUpdate:
  CYC(0x72f5, 0x72f6); H = D;
  CYC(0x72f6, 0x72f8); L = ENEMY_BASE + OBJ_VAR3B;
  CYC(0x72f8, 0x72f9); A = mem_rd(gb, HL);
  CYC(0x72f9, 0x72fb); mem_wr(gb, HL, 0x00);
  CYC(0x72fb, 0x72fc); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x72fc, 0x72fe); goto checkNoticed; } // jr nz
  CYC(0x72fc, 0x72fe);
  CYC(0x72fe, 0x7300); L = ENEMY_BASE + OBJ_VAR34;
  CYC(0x7300, 0x7301); A = mem_rd(gb, HL);
  CYC(0x7301, 0x7302); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7302, 0x7304); goto checkNoticed; } // jr nz
  CYC(0x7302, 0x7304);
  CYC(0x7304, 0x7305); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [var34]
  CALL_C(0x7305, ambiGuard_setCounter2ForAttackingTypeOnly_hook, 0x7388, 0x7308);

checkNoticed:
  CYC(0x7308, 0x730a); E = ENEMY_BASE + OBJ_VAR34;
  CYC(0x730a, 0x730b); A = mem_rd(gb, DE);
  CYC(0x730b, 0x730c); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x730c, 0x730e); goto haventSeenLinkYet; } // jr nc
  CYC(0x730c, 0x730e);
  CYC(0x730e, 0x730f); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x730f); return; } // ret c
  CYC(0x730f, 0x7310);
  CYC(0x7310, 0x7312); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7312, 0x7314); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x7314, 0x7316); goto noticedLink; } // jr nz
  CYC(0x7314, 0x7316);
  CALL_C(0x7316, checkLinkCollisionsEnabled_hook, 0x1d32, 0x7319);
  if (!(F & FC)) { RET_TAKEN(0x7319); return; } // ret nc
  CYC(0x7319, 0x731a);
  CYC(0x731a, 0x731d); A = mem_rd(gb, w1Link + OBJ_ZH);
  CYC(0x731d, 0x731e); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(0x731e); return; } // ret c
  CYC(0x731e, 0x731f);
  CYC(0x731f, 0x7321); A = 0x80;
  CYC(0x7321, 0x7324); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7324, 0x7326); A = 0x21; // DISABLE_COMPANION|DISABLE_LINK
  CYC(0x7326, 0x7329); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7329, 0x732c); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(0x732c, 0x732e); E = ENEMY_BASE + OBJ_VAR36;
  CYC(0x732e, 0x7330); A = 0x3c; // 60
  CYC(0x7330, 0x7331); mem_wr(gb, DE, A);
  CALL_C(0x7331, ambiGuard_createExclamationMark_hook, 0x734f, 0x7334);

noticedLink:
  CYC(0x7334, 0x7335); H = D;
  CYC(0x7335, 0x7337); L = ENEMY_BASE + OBJ_VAR34;
  CYC(0x7337, 0x7339); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1))); // set 1,(hl)
  CYC(0x7339, 0x733b); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x733b, 0x733d); mem_wr(gb, HL, 0x0f);
  CYC(0x733d, 0x733f); A = 0x29; // Object.health
  CALL_C(0x733f, objectGetRelatedObject2Var_hook, 0x2164, 0x7342);
  CYC(0x7342, 0x7344); mem_wr(gb, HL, 0x00);
  RET(0x7344); return; // ret

haventSeenLinkYet:
  CYC(0x7345, 0x7346); E = alu_inc8(gb, E);
  CYC(0x7346, 0x7347); A = mem_rd(gb, DE); // [var35]
  CYC(0x7347, 0x7348); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x7348); return; } // ret nc
  CYC(0x7348, 0x7349);
  CYC(0x7349, 0x734a); alu_xor(gb, A);
  CYC(0x734a, 0x734b); mem_wr(gb, DE, A);
  CYC(0x734b, 0x734d); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x734d, 0x734f); mem_wr(gb, HL, 0x32); // SPEED_140
  ambiGuard_createExclamationMark_hook(gb); return; // fallthrough
}

// 0e:734f, bare global; falls into from ambiGuard_checkSpottedLink, also called from
// several other states.
void ambiGuard_createExclamationMark_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x734f, 0x7351); A = 0x2d;
  CYC(0x7351, 0x7354); SET_BC(0xf408);
  CYC(0x7354, 0x7357); objectCreateExclamationMark_hook(gb); return; // jp
}

// 0e:7357, bare global; called from enemyCode54.
void ambiGuard_collisionOccured_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7357, 0x7359); E = ENEMY_BASE + OBJ_VAR34;
  CYC(0x7359, 0x735a); A = mem_rd(gb, DE);
  CYC(0x735a, 0x735b); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x735b); return; } // ret nz
  CYC(0x735b, 0x735c);
  CYC(0x735c, 0x735d); H = D;
  CYC(0x735d, 0x735f); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x735f, 0x7360); A = mem_rd(gb, HL);
  CYC(0x7360, 0x7362); alu_cp(gb, 0x92); // collisions up to & including ITEMCOLLISION_11 are direct attacks
  if (F & FC) { CYCT(0x7362, 0x7364); ambiGuard_directAttackOccurred_hook(gb); return; } // jr c
  CYC(0x7362, 0x7364);
  CYC(0x7364, 0x7366); alu_cp(gb, 0x9e); // $80|ITEMCOLLISION_GALE_SEED
  if (F & FZ) { RET_TAKEN(0x7366); return; } // ret z
  CYC(0x7366, 0x7367);
  CYC(0x7367, 0x7368); H = D;
  CYC(0x7368, 0x736a); L = ENEMY_BASE + OBJ_VAR35;
  CYC(0x736a, 0x736b); A = mem_rd(gb, HL);
  CYC(0x736b, 0x736c); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x736c); return; } // ret nz
  CYC(0x736c, 0x736d);
  CYC(0x736d, 0x736e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [var35] = 1
  CYC(0x736e, 0x736f); L = alu_inc8(gb, L);
  CYC(0x736f, 0x7371); mem_wr(gb, HL, 0x5a); // [var36] = 90
  CYC(0x7371, 0x7373); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(0x7373, 0x7374); A = mem_rd(gb, HL);
  CYC(0x7374, 0x7376); alu_xor(gb, 0x10);
  ambiGuard_setAngle_hook(gb); return; // fallthrough
}

// 0e:7376, bare global; falls into from ambiGuard_collisionOccured, also called from
// ambiGuard_checkSpottedLink.
// @param a Angle
void ambiGuard_setAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7376, 0x7378); alu_add(gb, 0x04);
  CYC(0x7378, 0x737a); alu_and(gb, 0x18);
  CYC(0x737a, 0x737c); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x737c, 0x737d); mem_wr(gb, DE, A);
  CYC(0x737d, 0x737f); A = alu_swap(gb, A);
  CYC(0x737f, 0x7380); alu_rlca(gb);
  CYC(0x7380, 0x7383); enemySetAnimation_hook(gb); return; // jp
}

// 0e:7383, bare global; falls into from ambiGuard_collisionOccured. A collision with one of
// Link's direct attacks (sword, fist, etc) occurred; the guard notices Link right away.
void ambiGuard_directAttackOccurred_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7383, 0x7385); E = ENEMY_BASE + OBJ_VAR34;
  CYC(0x7385, 0x7387); A = 0x01;
  CYC(0x7387, 0x7388); mem_wr(gb, DE, A);
  ambiGuard_setCounter2ForAttackingTypeOnly_hook(gb); return; // fallthrough
}

// 0e:7388, bare global; falls into from ambiGuard_directAttackOccurred, also called from
// ambiGuard_checkSpottedLink. Does some initialization for "attacking Link" type only, when
// they just notice Link.
void ambiGuard_setCounter2ForAttackingTypeOnly_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7388, 0x738a); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x738a, 0x738b); A = mem_rd(gb, DE);
  CYC(0x738b, 0x738c); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(0x738c); return; } // ret nc
  CYC(0x738c, 0x738d);
  CYC(0x738d, 0x738f); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x738f, 0x7391); A = 0x5a; // 90
  CYC(0x7391, 0x7392); mem_wr(gb, DE, A);
  CYC(0x7392, 0x7394); E = ENEMY_BASE + OBJ_VAR36;
  CYC(0x7394, 0x7395); alu_xor(gb, A);
  CYC(0x7395, 0x7396); mem_wr(gb, DE, A);
  RET(0x7396); return; // ret
}

// 0e:7397, bare global; called from enemyCode54. Scampering away when health is 0.
void ambiGuard_noHealth_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7397, 0x7399); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x7399, 0x739a); A = mem_rd(gb, DE);
  {
    CYC(0x739a, 0x739b); push_effect(gb, 0x739b);
    uint16_t target = ambiGuard_jump_table(gb);
    if (target == 0x73a1) goto substate0;
    if (target == 0x73ab) goto substate1;
    if (target == 0x73cf) goto substate2;
    HANDOFF(target);
  }

substate0:
  CYC(0x73a1, 0x73a2); H = D;
  CYC(0x73a2, 0x73a3); L = E;
  CYC(0x73a3, 0x73a4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(0x73a4, 0x73a6); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x73a6, 0x73a8); A = 0x00;
  CYC(0x73a8, 0x73a9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x73a9, 0x73ab); mem_wr(gb, HL, 0xff);

substate1:
  CYC(0x73ab, 0x73ad); C = 0x20;
  CALL_C(0x73ad, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x73b0);
  if (!(F & FZ)) { RET_TAKEN(0x73b0); return; } // ret nz
  CYC(0x73b0, 0x73b1);
  CYC(0x73b1, 0x73b3); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x73b3, 0x73b4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x73b4, 0x73b6); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x73b6, 0x73b8); A = 0x40;
  CYC(0x73b8, 0x73b9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a -- speedZ = -$1c0
  CYC(0x73b9, 0x73bb); mem_wr(gb, HL, 0xfe);
  CYC(0x73bb, 0x73bd); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x73bd, 0x73bf); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(0x73bf, 0x73c1); L = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(0x73c1, 0x73c2); A = mem_rd(gb, HL);
  CYC(0x73c2, 0x73c4); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x73c4, 0x73c5); mem_wr(gb, HL, A);
  CYC(0x73c5, 0x73c7); alu_add(gb, 0x04);
  CYC(0x73c7, 0x73c9); alu_and(gb, 0x18);
  CYC(0x73c9, 0x73cb); A = alu_swap(gb, A);
  CYC(0x73cb, 0x73cc); alu_rlca(gb);
  CYC(0x73cc, 0x73cf); enemySetAnimation_hook(gb); return; // jp

substate2:
  CYC(0x73cf, 0x73d1); E = ENEMY_BASE + OBJ_YH;
  CYC(0x73d1, 0x73d2); A = mem_rd(gb, DE);
  CYC(0x73d2, 0x73d4); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(0x73d4, 0x73d7); enemyDelete_hook(gb); return; } // jp nc
  CYC(0x73d4, 0x73d7);
  CYC(0x73d7, 0x73d9); E = ENEMY_BASE + OBJ_XH;
  CYC(0x73d9, 0x73da); A = mem_rd(gb, DE);
  CYC(0x73da, 0x73dc); alu_cp(gb, 0xf0); // LARGE_ROOM_WIDTH<<4
  if (!(F & FC)) { CYCT(0x73dc, 0x73df); enemyDelete_hook(gb); return; } // jp nc
  CYC(0x73dc, 0x73df);
  CALL_C(0x73df, objectApplySpeed_hook, 0x201d, 0x73e2);
  CYC(0x73e2, 0x73e4); C = 0x20;
  CALL_C(0x73e4, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x73e7);
  if (!(F & FZ)) { CYCT(0x73e7, 0x73ea); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x73e7, 0x73ea);
  CYC(0x73ea, 0x73ec); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x73ec, 0x73ee); A = 0x40;
  CYC(0x73ee, 0x73ef); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a -- speedZ = -$1c0
  CYC(0x73ef, 0x73f1); mem_wr(gb, HL, 0xfe);
  RET(0x73f1); return; // ret
}
