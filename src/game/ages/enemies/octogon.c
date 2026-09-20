#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void enemyCode7d_hook(GB *gb);
void octogon_state_uninitialized_hook(GB *gb);
void octogon_state_stub_hook(GB *gb);
void octogon_subid0_hook(GB *gb);
void octogon_subid0AboveWater_state8_hook(GB *gb);
void octogon_subid0AboveWater_state9_hook(GB *gb);
void octogon_subid0_pauseMovement_hook(GB *gb);
void octogon_subid0AboveWater_stateA_hook(GB *gb);
void octogon_chooseRandomTargetPosition_hook(GB *gb);
void octogon_loadTargetPosition_hook(GB *gb);
void octogon_subid0AboveWater_stateB_hook(GB *gb);
void octogon_subid0AboveWater_stateF_hook(GB *gb);
void octogon_subid0AboveWater_stateC_hook(GB *gb);
void octogon_subid0AboveWater_turningAround_hook(GB *gb);
void octogon_subid0AboveWater_stateD_hook(GB *gb);
void octogon_subid0AboveWater_stateE_hook(GB *gb);
void octogon_subid0AboveWater_state10_hook(GB *gb);
void octogon_subid0AboveWater_state11_hook(GB *gb);
void octogon_subid0BelowWater_hook(GB *gb);
void octogon_subid0BelowWater_state8_hook(GB *gb);
void octogon_subid0BelowWater_state9_hook(GB *gb);
void octogon_animate_hook(GB *gb);
void octogon_subid0BelowWater_stateA_hook(GB *gb);
void octogon_loadNormalSubmergedAnimation_hook(GB *gb);
void octogon_subid0BelowWater_stateB_hook(GB *gb);
void octogon_subid0BelowWater_stateC_hook(GB *gb);
void octogon_subid0BelowWater_stateD_hook(GB *gb);
void octogon_subid1_hook(GB *gb);
void octogon_subid1_belowWater_state8_hook(GB *gb);
void octogon_updateMovementAndAnimation_hook(GB *gb);
void octogon_doSplashAnimation_hook(GB *gb);
void octogon_subid1_belowWater_state9_hook(GB *gb);
void octogon_subid1_belowWater_stateA_hook(GB *gb);
void octogon_subid1_belowWater_stateB_hook(GB *gb);
void octogon_subid1_belowWater_stateC_hook(GB *gb);
void octogon_subid1_aboveWater_hook(GB *gb);
void octogon_subid2_hook(GB *gb);
void octogon_subid0_submergeIntoWater_hook(GB *gb);
void octogon_beginRisingAboveWater_hook(GB *gb);
void octogon_fixPositionAboveWater_hook(GB *gb);
void octogon_fireOctorokProjectile_hook(GB *gb);
void octogon_initializeProjectile_hook(GB *gb);
void octogon_decVar36IfNonzero_hook(GB *gb);
void octogon_moveTowardTargetPosition_hook(GB *gb);
void octogon_getClosestTargetPositionIndex_hook(GB *gb);

static uint16_t octogon_jump_table(GB *gb) {
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

static void octogon_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void octogon_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// enemyCode7d@doJumpTable: reached only by a static `call` from enemyCode7d itself
// (NOT HOOKED). Dispatches on state (if < 8) or subid (if state >= 8), tail-jumping
// into the matching named routine below.
static void octogon_doJumpTable(GB *gb, uint16_t sp0_) {
  CYC(0x74a5, 0x74a7); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x74a7, 0x74a8); A = mem_rd(gb, DE);
  CYC(0x74a8, 0x74aa); alu_cp(gb, 0x08);
  CYC(0x74aa, 0x74ac); E = ENEMY_BASE + OBJ_SUBID;
  if (F & FC) { CYCT(0x74ac, 0x74ae); goto state8OrLess; } // jr c
  CYC(0x74ac, 0x74ae);
  CYC(0x74ae, 0x74af); A = mem_rd(gb, DE);
  CYC(0x74af, 0x74b0); push_effect(gb, 0x74b0);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == 0x75a9) { octogon_subid0_hook(gb); return; }
    if (target == 0x7795) { octogon_subid1_hook(gb); return; }
    if (target == 0x78fe) { octogon_subid2_hook(gb); return; }
    HANDOFF(target);
  }

state8OrLess:
  CYC(0x74b6, 0x74b7); push_effect(gb, 0x74b7);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == 0x74c7) { octogon_state_uninitialized_hook(gb); return; }
    if (target == 0x75a8) { octogon_state_stub_hook(gb); return; }
    HANDOFF(target);
  }
}

// ==================================================================================================
// ENEMY_OCTOGON
//
// Variables:
//   var03: Where it actually is? (0 = above water, 1 = below water)
//   counter2: Counter until it moves above or below the water?
//   relatedObj1: Reference to other instance of ENEMY_OCTOGON?
//   var30: Index in "target position list"?
//   var31/var32: Target position to move to
//   var33/var34: Original Y/X position when this screen was entered
//   var35: Counter for animation purposes?
//   var36: Counter which, when 0 is reached, invokes a change of state (ie. fire at link
//          instead of moving around)
//   var37: Health value from when octogon appeared here (used to decide when to surface or
//          not)
// ==================================================================================================
void enemyCode7d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x740f, 0x7411); goto normalStatus; } // jr z
  CYC(0x740f, 0x7411);
  CYC(0x7411, 0x7413); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x7413); return; } // ret c
  CYC(0x7413, 0x7414);
  if (!(F & FZ)) { CYCT(0x7414, 0x7416); goto justHit; } // jr nz
  CYC(0x7414, 0x7416);
  CYC(0x7416, 0x7418); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7418, 0x7419); A = mem_rd(gb, DE);
  CYC(0x7419, 0x741b); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x741b, 0x741e); enemyDelete_hook(gb); return; } // jp z
  CYC(0x741b, 0x741e);
  CYC(0x741e, 0x7420); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7420, 0x7421); A = mem_rd(gb, DE);
  CYC(0x7421, 0x7422); alu_or(gb, A);
  if (!(F & FZ)) { CALL_C_CC(0x7422, ecom_killRelatedObj1_b0f_hook, 0x446d, 0x7425); } else { CYC(0x7422, 0x7425); } // call nz
  CYC(0x7425, 0x7428); enemyBoss_dead_b0f_hook(gb); return; // jp

justHit:
  CYC(0x7428, 0x742a); A = OBJ_INVINCIBILITY_COUNTER;
  CALL_C(0x742a, objectGetRelatedObject1Var_hook, 0x2160, 0x742d);
  CYC(0x742d, 0x742e); E = L;
  CYC(0x742e, 0x742f); A = mem_rd(gb, DE);
  CYC(0x742f, 0x7430); mem_wr(gb, HL, A);
  CYC(0x7430, 0x7432); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7432, 0x7433); A = mem_rd(gb, DE);
  CYC(0x7433, 0x7435); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x7435, 0x7437); goto normalStatus; } // jr z
  CYC(0x7435, 0x7437);
  CYC(0x7437, 0x7438); H = D;
  CYC(0x7438, 0x743a); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x743a, 0x743c); E = ENEMY_BASE + OBJ_VAR37;
  CYC(0x743c, 0x743d); A = mem_rd(gb, DE);
  CYC(0x743d, 0x743e); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x743e, 0x7440); alu_cp(gb, 0x0a);
  if (F & FC) { CYCT(0x7440, 0x7442); goto L_7446; } // jr c
  CYC(0x7440, 0x7442);
  CYC(0x7442, 0x7444); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7444, 0x7446); mem_wr(gb, HL, 0x01);

L_7446:
  CYC(0x7446, 0x7448); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x7448, 0x7449); A = mem_rd(gb, DE);
  CYC(0x7449, 0x744a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x744a, 0x744c); goto normalStatus; } // jr nz
  CYC(0x744a, 0x744c);
  CYC(0x744c, 0x744f); SET_HL(wGroup5RoomFlags + 0x2d); // ROOM_AGES_52d
  CYC(0x744f, 0x7451); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x7451, 0x7453); L = 0x36; // ROOM_AGES_536
  CYC(0x7453, 0x7455); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x7455, 0x7457); A = 0x2e; // MUS_BOSS
  CYC(0x7457, 0x745a); mem_wr(gb, wActiveMusic, A);
  RET(0x745a); return; // ret

normalStatus:
  CYC(0x745b, 0x745e); push_effect(gb, 0x745e); octogon_doJumpTable(gb, sp0_);
  CYC(0x745e, 0x745f); H = D;
  CYC(0x745f, 0x7461); L = ENEMY_BASE + OBJ_VAR34;
  CYC(0x7461, 0x7463); E = ENEMY_BASE + OBJ_XH;
  CYC(0x7463, 0x7464); A = mem_rd(gb, DE);
  CYC(0x7464, 0x7465); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x7465, 0x7467); E = ENEMY_BASE + OBJ_YH;
  CYC(0x7467, 0x7468); A = mem_rd(gb, DE);
  CYC(0x7468, 0x7469); mem_wr(gb, HL, A);
  CYC(0x7469, 0x746b); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x746b, 0x746c); A = mem_rd(gb, HL);
  CYC(0x746c, 0x746e); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(0x746e, 0x7470); goto L_7472; } // jr c
  CYC(0x746e, 0x7470);
  CYC(0x7470, 0x7472); A = 0x08;

L_7472:
  CYC(0x7472, 0x7474); alu_and(gb, 0x0c);
  CYC(0x7474, 0x7475); alu_rrca(gb);
  CYC(0x7475, 0x7478); SET_HL(0x749d); // @offsetData
  CYC(0x7478, 0x7479); octogon_addAToHl_from_rst(gb, 0x7479);
  CYC(0x7479, 0x747a); A = mem_rd(gb, DE);
  CYC(0x747a, 0x747b); alu_add(gb, mem_rd(gb, HL));
  CYC(0x747b, 0x747c); mem_wr(gb, DE, A);
  CYC(0x747c, 0x747e); E = ENEMY_BASE + OBJ_XH;
  CYC(0x747e, 0x747f); SET_HL(HL + 1); // inc hl
  CYC(0x747f, 0x7480); A = mem_rd(gb, DE);
  CYC(0x7480, 0x7481); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7481, 0x7482); mem_wr(gb, DE, A);
  CYC(0x7482, 0x7485); SET_HL(wTmpcfc0_octogonBoss_var03);
  CYC(0x7485, 0x7487); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x7487, 0x7488); A = mem_rd(gb, DE);
  CYC(0x7488, 0x7489); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7489, 0x748b); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x748b, 0x748c); A = mem_rd(gb, DE);
  CYC(0x748c, 0x748d); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [octogonBoss.direction]
  CYC(0x748d, 0x748f); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x748f, 0x7490); A = mem_rd(gb, DE);
  CYC(0x7490, 0x7491); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [octogonBoss.health]
  CYC(0x7491, 0x7493); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7493, 0x7494); A = mem_rd(gb, DE);
  CYC(0x7494, 0x7495); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [octogonBoss.var33]
  CYC(0x7495, 0x7496); E = alu_inc8(gb, E);
  CYC(0x7496, 0x7497); A = mem_rd(gb, DE); // [var34]
  CYC(0x7497, 0x7498); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [octogonBoss.var34]
  CYC(0x7498, 0x749a); E = ENEMY_BASE + 0x30; // var30
  CYC(0x749a, 0x749b); A = mem_rd(gb, DE);
  CYC(0x749b, 0x749c); mem_wr(gb, HL, A); // [octogonBoss.var30]
  RET(0x749c); return; // ret
}

void octogon_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x74c7, 0x74c9); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x74c9, 0x74ca); A = mem_rd(gb, DE);
  CYC(0x74ca, 0x74cc); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x74cc, 0x74ce); goto notSubid2; } // jr nz
  CYC(0x74cc, 0x74ce);
  CYC(0x74ce, 0x74d0); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x74d0, 0x74d2); A = 0x67; // ENEMYCOLLISION_OCTOGON_SHELL
  CYC(0x74d2, 0x74d3); mem_wr(gb, DE, A);
  CYC(0x74d3, 0x74d6); ecom_setSpeedAndState8_b0f_hook(gb); return; // jp

notSubid2:
  CYC(0x74d6, 0x74d8); A = 0x7d; // ENEMY_OCTOGON
  CYC(0x74d8, 0x74db); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);
  CYC(0x74db, 0x74dd); A = 0x88; // PALH_88
  CALL_C(0x74dd, loadPaletteHeader_hook, 0x050b, 0x74e0);
  CYC(0x74e0, 0x74e3); SET_HL(wTmpcfc0_octogonBoss_loadedExtraGfx);
  CYC(0x74e3, 0x74e4); A = mem_rd(gb, HL);
  CYC(0x74e4, 0x74e5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x74e5, 0x74e7); goto L_74eb; } // jr nz
  CYC(0x74e5, 0x74e7);
  CYC(0x74e7, 0x74e8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x74e8, enemyBoss_initializeRoomWithoutExtraGfx_b0f_hook, 0x4552, 0x74eb);

L_74eb:
  CALL_C(0x74eb, getFreeEnemySlot_uncounted_hook, 0x2e34, 0x74ee);
  if (!(F & FZ)) { RET_TAKEN(0x74ee); return; } // ret nz
  CYC(0x74ee, 0x74ef);
  CYC(0x74ef, 0x74f1); mem_wr(gb, HL, 0x7d); // ENEMY_OCTOGON
  CYC(0x74f1, 0x74f2); L = alu_inc8(gb, L);
  CYC(0x74f2, 0x74f4); mem_wr(gb, HL, 0x02); // [child.subid]
  CYC(0x74f4, 0x74f6); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x74f6, 0x74f7); E = L;
  CYC(0x74f7, 0x74f9); A = 0x80; // Enemy.start
  CYC(0x74f9, 0x74fa); mem_wr(gb, DE, A);
  CYC(0x74fa, 0x74fb); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x74fb, 0x74fc); E = alu_inc8(gb, E);
  CYC(0x74fc, 0x74fd); A = H;
  CYC(0x74fd, 0x74fe); mem_wr(gb, DE, A);
  CYC(0x74fe, 0x74ff); mem_wr(gb, HL, D);
  CYC(0x74ff, 0x7501); A = 0x28; // SPEED_100
  CALL_C(0x7501, ecom_setSpeedAndState8_b0f_hook, 0x4364, 0x7504);
  CYC(0x7504, 0x7506); L = ENEMY_BASE + OBJ_VAR35;
  CYC(0x7506, 0x7508); mem_wr(gb, HL, 0x0c); // [this.var35]
  CYC(0x7508, 0x7509); L = alu_inc8(gb, L);
  CYC(0x7509, 0x750b); mem_wr(gb, HL, 120); // [var36]
  CYC(0x750b, 0x750d); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x750d, 0x750e); A = mem_rd(gb, HL);
  CYC(0x750e, 0x750f); alu_add(gb, A);
  CYC(0x750f, 0x7510); B = A;
  CALL_C(0x7510, objectSetVisible83_hook, 0x1e72, 0x7513);
  CYC(0x7513, 0x7516); SET_HL(wTmpcfc0_octogonBoss_var30);
  CYC(0x7516, 0x7518); E = ENEMY_BASE + 0x30; // var30
  CYC(0x7518, 0x7519); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x7519, 0x751a); mem_wr(gb, DE, A);
  CYC(0x751a, 0x751c); E = ENEMY_BASE + OBJ_XH;
  CYC(0x751c, 0x751d); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x751d, 0x751e); mem_wr(gb, DE, A);
  CYC(0x751e, 0x7520); E = ENEMY_BASE + OBJ_YH;
  CYC(0x7520, 0x7521); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x7521, 0x7522); mem_wr(gb, DE, A);
  CYC(0x7522, 0x7524); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x7524, 0x7525); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x7525, 0x7526); mem_wr(gb, DE, A);
  CYC(0x7526, 0x7528); E = ENEMY_BASE + OBJ_VAR37;
  CYC(0x7528, 0x7529); mem_wr(gb, DE, A);
  CYC(0x7529, 0x752b); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x752b, 0x752c); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x752c, 0x752d); mem_wr(gb, DE, A);
  CYC(0x752d, 0x752f); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x752f, 0x7530); A = mem_rd(gb, HL);
  CYC(0x7530, 0x7531); mem_wr(gb, DE, A);
  CYC(0x7531, 0x7532); alu_add(gb, B);
  CYC(0x7532, 0x7533); push_effect(gb, 0x7533);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == 0x753b) goto subid0_0;
    if (target == 0x7558) goto subid0_1;
    if (target == 0x755e) goto subid1_0;
    if (target == 0x7570) goto subid1_1;
    HANDOFF(target);
  }

subid0_0:
  CALL_C(0x753b, octogon_fixPositionAboveWater_hook, 0x7967, 0x753e);
  CYC(0x753e, 0x753f); H = D;
  CYC(0x753f, 0x7541); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7541, 0x7543); mem_wr(gb, HL, 120);
  CYC(0x7543, 0x7545); L = ENEMY_BASE + 0x30; // var30
  CYC(0x7545, 0x7546); A = mem_rd(gb, HL);
  CYC(0x7546, 0x7547); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x7547, 0x754a); octogon_chooseRandomTargetPosition_hook(gb); return; } // jp z
  CYC(0x7547, 0x754a);
  CALL_C(0x754a, octogon_loadTargetPosition_hook, 0x763d, 0x754d);
  if (F & FZ) { RET_TAKEN(0x754d); return; } // ret z
  CYC(0x754d, 0x754e);
  CYC(0x754e, 0x754f); SET_HL(HL - 1); // dec hl
  CYC(0x754f, 0x7550); SET_HL(HL - 1); // dec hl
  CYC(0x7550, 0x7551); SET_HL(HL - 1); // dec hl
  CYC(0x7551, 0x7552); A = mem_rd(gb, HL);
  CYC(0x7552, 0x7554); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x7554, 0x7555); mem_wr(gb, DE, A);
  CYC(0x7555, 0x7558); enemySetAnimation_hook(gb); return; // jp

subid0_1:
  CALL_C(0x7558, octogon_fixPositionAboveWater_hook, 0x7967, 0x755b);
  CYC(0x755b, 0x755e); octogon_loadNormalSubmergedAnimation_hook(gb); return; // jp

subid1_0:
  CYC(0x755e, 0x755f); H = D;
  CYC(0x755f, 0x7561); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7561, 0x7563); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x7563, 0x7565); A = 0x00; // Object.start
  CALL_C(0x7565, objectGetRelatedObject1Var_hook, 0x2160, 0x7568);
  CYC(0x7568, 0x756a); B = 0x40;
  CALL_C(0x756a, clearMemory_hook, 0x046f, 0x756d);
  CYC(0x756d, 0x7570); objectSetInvisible_hook(gb); return; // jp

subid1_1:
  CYC(0x7570, 0x7572); A = 0x01;
  CYC(0x7572, 0x7575); mem_wr(gb, wTmpcfc0_octogonBoss_posNeedsFixing, A);
  CYC(0x7575, 0x7576); H = D;
  CYC(0x7576, 0x7578); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x7578, 0x757a); A = 0x06;
  CYC(0x757a, 0x757b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x757b, 0x757c); mem_wr(gb, HL, A);
  CYC(0x757c, 0x757e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x757e, 0x7580); mem_wr(gb, HL, 90);
  CYC(0x7580, 0x7581); L = alu_inc8(gb, L);
  CYC(0x7581, 0x7583); mem_wr(gb, HL, 150); // [counter2]
  CYC(0x7583, 0x7585); L = ENEMY_BASE + 0x31; // var31
  CYC(0x7585, 0x7587); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x7587, 0x7588); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7588, 0x758a); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x758a, 0x758b); mem_wr(gb, HL, A);
  CALL_C(0x758b, ecom_updateAngleTowardTarget_b0f_hook, 0x43bf, 0x758e);
  CYC(0x758e, 0x7590); alu_add(gb, 0x04);
  CYC(0x7590, 0x7592); alu_and(gb, 0x18);
  CYC(0x7592, 0x7593); alu_rrca(gb);
  CYC(0x7593, 0x7595); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x7595, 0x7596); mem_wr(gb, DE, A);
  CALL_C(0x7596, enemySetAnimation_hook, 0x282b, 0x7599);
  CALL_C(0x7599, getFreeInteractionSlot_hook, 0x3aef, 0x759c);
  if (!(F & FZ)) { RET_TAKEN(0x759c); return; } // ret nz
  CYC(0x759c, 0x759d);
  CYC(0x759d, 0x759f); mem_wr(gb, HL, 0x91); // INTERAC_BUBBLE
  CYC(0x759f, 0x75a0); L = alu_inc8(gb, L);
  CYC(0x75a0, 0x75a1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [bubble.subid] = 1
  CYC(0x75a1, 0x75a3); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x75a3, 0x75a5); A = 0x80; // Enemy.start
  CYC(0x75a5, 0x75a6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x75a6, 0x75a7); mem_wr(gb, HL, D);
  RET(0x75a7); return; // ret
}

void octogon_state_stub_hook(GB *gb) {
  RET(0x75a8); return; // ret
}

void octogon_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x75a9, 0x75aa); H = D;
  CYC(0x75aa, 0x75ac); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x75ac, 0x75ae); E = ENEMY_BASE + OBJ_YH;
  CYC(0x75ae, 0x75af); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x75af, 0x75b0); mem_wr(gb, DE, A);
  CYC(0x75b0, 0x75b2); E = ENEMY_BASE + OBJ_XH;
  CYC(0x75b2, 0x75b3); A = mem_rd(gb, HL);
  CYC(0x75b3, 0x75b4); mem_wr(gb, DE, A);
  CYC(0x75b4, 0x75b6); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x75b6, 0x75b7); A = mem_rd(gb, DE);
  CYC(0x75b7, 0x75b8); alu_or(gb, A);
  CYC(0x75b8, 0x75ba); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x75ba, 0x75bb); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(0x75bb, 0x75be); octogon_subid0BelowWater_hook(gb); return; } // jp nz
  CYC(0x75bb, 0x75be);
  CYC(0x75be, 0x75c0); alu_sub(gb, 0x08);
  CYC(0x75c0, 0x75c1); push_effect(gb, 0x75c1);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == 0x75d5) { octogon_subid0AboveWater_state8_hook(gb); return; }
    if (target == 0x75ea) { octogon_subid0AboveWater_state9_hook(gb); return; }
    if (target == 0x7628) { octogon_subid0AboveWater_stateA_hook(gb); return; }
    if (target == 0x76bc) { octogon_subid0AboveWater_stateB_hook(gb); return; }
    if (target == 0x76c0) { octogon_subid0AboveWater_stateC_hook(gb); return; }
    if (target == 0x76d4) { octogon_subid0AboveWater_stateD_hook(gb); return; }
    if (target == 0x76e5) { octogon_subid0AboveWater_stateE_hook(gb); return; }
    if (target == 0x76f7) { octogon_subid0AboveWater_state10_hook(gb); return; }
    if (target == 0x76fb) { octogon_subid0AboveWater_state11_hook(gb); return; }
    HANDOFF(target);
  }
}

// Wait for shutters to close
void octogon_subid0AboveWater_state8_hook(GB *gb) {
  CYC(0x75d5, 0x75d8); A = mem_rd(gb, wcc93);
  CYC(0x75d8, 0x75d9); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x75d9); return; } // ret nz
  CYC(0x75d9, 0x75da);
  CYC(0x75da, 0x75db); H = D;
  CYC(0x75db, 0x75dc); L = E;
  CYC(0x75dc, 0x75dd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(0x75dd, 0x75e0); SET_HL(wActiveMusic);
  CYC(0x75e0, 0x75e1); A = mem_rd(gb, HL);
  CYC(0x75e1, 0x75e2); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x75e2); return; } // ret z
  CYC(0x75e2, 0x75e3);
  CYC(0x75e3, 0x75e5); mem_wr(gb, HL, 0x00);
  CYC(0x75e5, 0x75e7); A = 0x2e; // MUS_BOSS
  CYC(0x75e7, 0x75ea); playSound_b00_hook(gb); return; // jp
}

// Moving normally around the room
void octogon_subid0AboveWater_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x75ea, octogon_decVar36IfNonzero_hook, 0x7a02, 0x75ed);
  CYC(0x75ed, 0x75f0); A = mem_rd(gb, wFrameCounter);
  CYC(0x75f0, 0x75f2); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x75f2, 0x75f4); goto L_75fa; } // jr nz
  CYC(0x75f2, 0x75f4);
  CYC(0x75f4, 0x75f6); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x75f6, 0x75f7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(0x75f7, 0x75fa); octogon_subid0_submergeIntoWater_hook(gb); return; } // jp z
  CYC(0x75f7, 0x75fa);

L_75fa:
  CYC(0x75fa, 0x75fc); C = 0x08;
  CALL_C(0x75fc, objectCheckCenteredWithLink_hook, 0x1fee, 0x75ff);
  if (!(F & FC)) { CYCT(0x75ff, 0x7602); octogon_updateMovementAndAnimation_hook(gb); return; } // jp nc
  CYC(0x75ff, 0x7602);
  CYC(0x7602, 0x7603); H = D;
  CYC(0x7603, 0x7605); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x7605, 0x7606); A = mem_rd(gb, HL);
  CYC(0x7606, 0x7607); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7607, 0x760a); octogon_updateMovementAndAnimation_hook(gb); return; } // jp nz
  CYC(0x7607, 0x760a);
  CYC(0x760a, 0x760c); mem_wr(gb, HL, 120); // [var36]
  CALL_C(0x760c, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x760f);
  CYC(0x760f, 0x7611); alu_add(gb, 0x14);
  CYC(0x7611, 0x7613); alu_and(gb, 0x18);
  CYC(0x7613, 0x7614); B = A;
  CYC(0x7614, 0x7616); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x7616, 0x7617); A = mem_rd(gb, DE);
  CYC(0x7617, 0x7619); alu_and(gb, 0x0c);
  CYC(0x7619, 0x761a); alu_add(gb, A);
  CYC(0x761a, 0x761b); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x761b, 0x761e); octogon_updateMovementAndAnimation_hook(gb); return; } // jp nz
  CYC(0x761b, 0x761e);
  CYC(0x761e, 0x761f); H = D;
  CYC(0x761f, 0x7621); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7621, 0x7623); mem_wr(gb, HL, 0x0b);
  CYC(0x7623, 0x7625); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7625, 0x7627); mem_wr(gb, HL, 0x08);
  RET(0x7627); return; // ret
}

// Waiting in place until counter1 reaches 0, then choosing the next target position
// (also reachable as octogon_subid0AboveWater_stateA, since both labels share this
// address).
void octogon_subid0_pauseMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7628, ecom_decCounter1_b0f_hook, 0x439a, 0x762b);
  if (!(F & FZ)) { RET_TAKEN(0x762b); return; } // ret nz
  CYC(0x762b, 0x762c);
  CYC(0x762c, 0x762d); L = E;
  CYC(0x762d, 0x762e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]--
  CYC(0x762e, 0x7630); L = ENEMY_BASE + 0x30; // var30
  CYC(0x7630, 0x7631); A = mem_rd(gb, HL);
  CYC(0x7631, 0x7632); A = alu_inc8(gb, A);
  CYC(0x7632, 0x7633); mem_wr(gb, HL, A);
  CYC(0x7633, 0x7635); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(0x7635, 0x7637); octogon_loadTargetPosition_hook(gb); return; } // jr nz
  CYC(0x7635, 0x7637);
  octogon_chooseRandomTargetPosition_hook(gb); return; // fallthrough
}

void octogon_subid0AboveWater_stateA_hook(GB *gb) {
  octogon_subid0_pauseMovement_hook(gb);
}

void octogon_chooseRandomTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7637, getRandomNumber_hook, 0x043e, 0x763a);
  CYC(0x763a, 0x763c); alu_and(gb, 0x18);
  CYC(0x763c, 0x763d); mem_wr(gb, HL, A);
  octogon_loadTargetPosition_hook(gb); return; // fallthrough
}

// @param hl Pointer to index for a table
// @param[out] hl Pointer to some data
// @param[out] zflag z if animation changed
void octogon_loadTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x763d, 0x763e); A = mem_rd(gb, HL);
  CYC(0x763e, 0x763f); alu_add(gb, A);
  CYC(0x763f, 0x7640); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7640, 0x7643); SET_HL(0x765c); // @targetPositionList
  CYC(0x7643, 0x7644); octogon_addAToHl_from_rst(gb, 0x7644);
  CYC(0x7644, 0x7646); E = ENEMY_BASE + 0x31; // var31
  CYC(0x7646, 0x7647); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7647, 0x7648); mem_wr(gb, DE, A);
  CYC(0x7648, 0x7649); E = alu_inc8(gb, E);
  CYC(0x7649, 0x764a); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x764a, 0x764b); mem_wr(gb, DE, A); // [var32]
  CYC(0x764b, 0x764d); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x764d, 0x764e); A = mem_rd(gb, DE);
  CYC(0x764e, 0x764f); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x764f); return; } // ret nz
  CYC(0x764f, 0x7650);
  CYC(0x7650, 0x7651); A = mem_rd(gb, HL);
  CYC(0x7651, 0x7653); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(0x7653); return; } // ret nz
  CYC(0x7653, 0x7654);
  CYC(0x7654, 0x7656); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x7656, 0x7657); mem_wr(gb, DE, A);
  CALL_C(0x7657, enemySetAnimation_hook, 0x282b, 0x765a);
  CYC(0x765a, 0x765b); alu_xor(gb, A);
  RET(0x765b); return; // ret
}

// Turning around to fire projectile (or just after doing so); also reached as
// octogon_subid0AboveWater_stateF (identical address).
void octogon_subid0AboveWater_stateB_hook(GB *gb) {
  CYC(0x76bc, 0x76be); B = 0x06;
  CYCT(0x76be, 0x76c0); octogon_subid0AboveWater_turningAround_hook(gb); return; // jr
}

void octogon_subid0AboveWater_stateF_hook(GB *gb) {
  octogon_subid0AboveWater_stateB_hook(gb);
}

// Turning around?
void octogon_subid0AboveWater_stateC_hook(GB *gb) {
  CYC(0x76c0, 0x76c2); B = 0x18;
  octogon_subid0AboveWater_turningAround_hook(gb); return; // fallthrough
}

void octogon_subid0AboveWater_turningAround_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x76c2, ecom_decCounter1_b0f_hook, 0x439a, 0x76c5);
  if (!(F & FZ)) { RET_TAKEN(0x76c5); return; } // ret nz
  CYC(0x76c5, 0x76c6);
  CYC(0x76c6, 0x76c7); mem_wr(gb, HL, B);
  CYC(0x76c7, 0x76c8); L = E;
  CYC(0x76c8, 0x76c9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(0x76c9, 0x76cb); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x76cb, 0x76cc); A = mem_rd(gb, HL);
  CYC(0x76cc, 0x76ce); alu_add(gb, 0x04);
  CYC(0x76ce, 0x76d0); alu_and(gb, 0x0c);
  CYC(0x76d0, 0x76d1); mem_wr(gb, HL, A);
  CYC(0x76d1, 0x76d4); enemySetAnimation_hook(gb); return; // jp
}

// About to fire projectile?
void octogon_subid0AboveWater_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x76d4, ecom_decCounter1_b0f_hook, 0x439a, 0x76d7);
  if (!(F & FZ)) { RET_TAKEN(0x76d7); return; } // ret nz
  CYC(0x76d7, 0x76d8);
  CYC(0x76d8, 0x76da); mem_wr(gb, HL, 0x08);
  CYC(0x76da, 0x76db); L = E;
  CYC(0x76db, 0x76dc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0e
  CYC(0x76dc, 0x76de); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x76de, 0x76df); A = mem_rd(gb, HL);
  CYC(0x76df, 0x76e1); alu_add(gb, 0x02);
  CYC(0x76e1, 0x76e2); mem_wr(gb, HL, A);
  CYC(0x76e2, 0x76e5); enemySetAnimation_hook(gb); return; // jp
}

// Firing projectile
void octogon_subid0AboveWater_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x76e5, ecom_decCounter1_b0f_hook, 0x439a, 0x76e8);
  if (!(F & FZ)) { RET_TAKEN(0x76e8); return; } // ret nz
  CYC(0x76e8, 0x76e9);
  CYC(0x76e9, 0x76eb); mem_wr(gb, HL, 40);
  CYC(0x76eb, 0x76ec); L = E;
  CYC(0x76ec, 0x76ed); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0f
  CYC(0x76ed, 0x76ef); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x76ef, 0x76f0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x76f0, 0x76f1); A = mem_rd(gb, HL);
  CALL_C(0x76f1, enemySetAnimation_hook, 0x282b, 0x76f4);
  CYC(0x76f4, 0x76f7); octogon_fireOctorokProjectile_hook(gb); return; // jp
}

// Turning around after firing projectile?
void octogon_subid0AboveWater_state10_hook(GB *gb) {
  CYC(0x76f7, 0x76f9); B = 0x0c;
  CYCT(0x76f9, 0x76fb); octogon_subid0AboveWater_turningAround_hook(gb); return; // jr
}

// Delay before resuming normal movement
void octogon_subid0AboveWater_state11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x76fb, ecom_decCounter1_b0f_hook, 0x439a, 0x76fe);
  if (!(F & FZ)) { RET_TAKEN(0x76fe); return; } // ret nz
  CYC(0x76fe, 0x76ff);
  CYC(0x76ff, 0x7700); L = E;
  CYC(0x7700, 0x7702); mem_wr(gb, HL, 0x09); // [state]
  RET(0x7702); return; // ret
}

// Octogon code where octogon itself is below water, and link is above water
void octogon_subid0BelowWater_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7703, 0x7705); alu_sub(gb, 0x08);
  CYC(0x7705, 0x7706); push_effect(gb, 0x7706);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == 0x7712) { octogon_subid0BelowWater_state8_hook(gb); return; }
    if (target == 0x773c) { octogon_subid0BelowWater_state9_hook(gb); return; }
    if (target == 0x7742) { octogon_subid0BelowWater_stateA_hook(gb); return; }
    if (target == 0x7760) { octogon_subid0BelowWater_stateB_hook(gb); return; }
    if (target == 0x777c) { octogon_subid0BelowWater_stateC_hook(gb); return; }
    if (target == 0x778a) { octogon_subid0BelowWater_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// Swimming normally
void octogon_subid0BelowWater_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7712, enemyAnimate_hook, 0x2818, 0x7715);
  CYC(0x7715, 0x7717); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x7717, 0x7718); A = mem_rd(gb, DE);
  CYC(0x7718, 0x7719); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7719, 0x771b); goto L_7722; } // jr nz
  CYC(0x7719, 0x771b);
  CYC(0x771b, 0x771c); A = alu_inc8(gb, A);
  CYC(0x771c, 0x771d); mem_wr(gb, DE, A);
  CYC(0x771d, 0x771f); A = 0x88; // SND_LINK_SWIM
  CALL_C(0x771f, playSound_b00_hook, 0x0c98, 0x7722);

L_7722:
  CALL_C(0x7722, octogon_decVar36IfNonzero_hook, 0x7a02, 0x7725);
  if (!(F & FZ)) { CYCT(0x7725, 0x7728); octogon_moveTowardTargetPosition_hook(gb); return; } // jp nz
  CYC(0x7725, 0x7728);
  CYC(0x7728, 0x772a); mem_wr(gb, HL, 90); // [var36]
  CALL_C(0x772a, getRandomNumber_hook, 0x043e, 0x772d);
  CYC(0x772d, 0x772f); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(0x772f, 0x7732); octogon_moveTowardTargetPosition_hook(gb); return; } // jp nc
  CYC(0x772f, 0x7732);
  CYC(0x7732, 0x7734); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7734, 0x7736); mem_wr(gb, HL, 0x0a);
  CYC(0x7736, 0x7738); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7738, 0x773a); mem_wr(gb, HL, 60);
  CYCT(0x773a, 0x773c); octogon_loadNormalSubmergedAnimation_hook(gb); return; // jr
}

// Waiting in place before moving again
void octogon_subid0BelowWater_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x773c, octogon_subid0_pauseMovement_hook, 0x7628, 0x773f);
  octogon_animate_hook(gb); return; // fallthrough
}

void octogon_animate_hook(GB *gb) {
  CYC(0x773f, 0x7742); enemyAnimate_hook(gb); return; // jp
}

// Delay before firing projectile
void octogon_subid0BelowWater_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7742, ecom_decCounter1_b0f_hook, 0x439a, 0x7745);
  if (F & FZ) { CYCT(0x7745, 0x7747); goto beginFiring; } // jr z
  CYC(0x7745, 0x7747);
  CYC(0x7747, 0x7748); A = mem_rd(gb, HL);
  CYC(0x7748, 0x774a); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(0x774a); return; } // ret nz
  CYC(0x774a, 0x774b);
  CYC(0x774b, 0x774d); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x774d, 0x774e); A = mem_rd(gb, HL);
  CYC(0x774e, 0x7750); alu_xor(gb, 0x01);
  CYC(0x7750, 0x7751); mem_wr(gb, HL, A);
  CYC(0x7751, 0x7754); enemySetAnimation_hook(gb); return; // jp

beginFiring:
  CYC(0x7754, 0x7756); mem_wr(gb, HL, 0x08);
  CYC(0x7756, 0x7757); L = E;
  CYC(0x7757, 0x7758); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0b
  octogon_loadNormalSubmergedAnimation_hook(gb); return; // fallthrough
}

void octogon_loadNormalSubmergedAnimation_hook(GB *gb) {
  CYC(0x7758, 0x775a); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x775a, 0x775c); A = 0x12;
  CYC(0x775c, 0x775d); mem_wr(gb, DE, A);
  CYC(0x775d, 0x7760); enemySetAnimation_hook(gb); return; // jp
}

// Firing projectile
void octogon_subid0BelowWater_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7760, ecom_decCounter1_b0f_hook, 0x439a, 0x7763);
  if (F & FZ) { CYCT(0x7763, 0x7765); goto fireProjectile; } // jr z
  CYC(0x7763, 0x7765);
  CYC(0x7765, 0x7766); A = mem_rd(gb, HL);
  CYC(0x7766, 0x7768); alu_cp(gb, 0x06);
  if (!(F & FZ)) { RET_TAKEN(0x7768); return; } // ret nz
  CYC(0x7768, 0x7769);
  CYC(0x7769, 0x776b); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x776b, 0x776d); A = 0x14;
  CYC(0x776d, 0x776e); mem_wr(gb, HL, A);
  CYC(0x776e, 0x7771); enemySetAnimation_hook(gb); return; // jp

fireProjectile:
  CYC(0x7771, 0x7773); mem_wr(gb, HL, 60); // [counter1]
  CYC(0x7773, 0x7774); L = E;
  CYC(0x7774, 0x7775); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c
  CYC(0x7775, 0x7777); B = 0x48; // PART_OCTOGON_DEPTH_CHARGE
  CALL_C(0x7777, ecom_spawnProjectile_b0f_hook, 0x437c, 0x777a);
  CYCT(0x777a, 0x777c); octogon_loadNormalSubmergedAnimation_hook(gb); return; // jr
}

// Delay before moving again
void octogon_subid0BelowWater_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x777c, ecom_decCounter1_b0f_hook, 0x439a, 0x777f);
  if (!(F & FZ)) { CYCT(0x777f, 0x7781); octogon_animate_hook(gb); return; } // jr nz
  CYC(0x777f, 0x7781);
  CYC(0x7781, 0x7783); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x7783, 0x7785); mem_wr(gb, HL, 90);
  CYC(0x7785, 0x7786); L = E;
  CYC(0x7786, 0x7788); mem_wr(gb, HL, 0x08); // [state]
  CYCT(0x7788, 0x778a); octogon_animate_hook(gb); return; // jr
}

// Just submerged into water
void octogon_subid0BelowWater_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x778a, ecom_decCounter1_b0f_hook, 0x439a, 0x778d);
  if (!(F & FZ)) { RET_TAKEN(0x778d); return; } // ret nz
  CYC(0x778d, 0x778e);
  CYC(0x778e, 0x7790); mem_wr(gb, HL, 30);
  CYC(0x7790, 0x7791); L = E;
  CYC(0x7791, 0x7793); mem_wr(gb, HL, 0x08); // [state]
  CYCT(0x7793, 0x7795); octogon_loadNormalSubmergedAnimation_hook(gb); return; // jr
}

// Link is below water
void octogon_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7795, 0x7796); H = D;
  CYC(0x7796, 0x7798); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7798, 0x779a); E = ENEMY_BASE + OBJ_YH;
  CYC(0x779a, 0x779b); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x779b, 0x779c); mem_wr(gb, DE, A);
  CYC(0x779c, 0x779e); E = ENEMY_BASE + OBJ_XH;
  CYC(0x779e, 0x779f); A = mem_rd(gb, HL);
  CYC(0x779f, 0x77a0); mem_wr(gb, DE, A);
  CYC(0x77a0, 0x77a2); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x77a2, 0x77a3); A = mem_rd(gb, DE);
  CYC(0x77a3, 0x77a4); alu_or(gb, A);
  CYC(0x77a4, 0x77a6); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x77a6, 0x77a7); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(0x77a7, 0x77aa); octogon_subid1_aboveWater_hook(gb); return; } // jp z
  CYC(0x77a7, 0x77aa);
  CYC(0x77aa, 0x77ac); alu_sub(gb, 0x08);
  CYC(0x77ac, 0x77ad); push_effect(gb, 0x77ad);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == 0x77b7) { octogon_subid1_belowWater_state8_hook(gb); return; }
    if (target == 0x7834) { octogon_subid1_belowWater_state9_hook(gb); return; }
    if (target == 0x786a) { octogon_subid1_belowWater_stateA_hook(gb); return; }
    if (target == 0x787f) { octogon_subid1_belowWater_stateB_hook(gb); return; }
    if (target == 0x789b) { octogon_subid1_belowWater_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// Normal movement (moving toward some target position decided already)
void octogon_subid1_belowWater_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x77b7, octogon_decVar36IfNonzero_hook, 0x7a02, 0x77ba);
  if (!(F & FZ)) { CYCT(0x77ba, 0x77bc); goto normalMovement; } // jr nz
  CYC(0x77ba, 0x77bc);
  CYC(0x77bc, 0x77be); mem_wr(gb, HL, 90);
  CALL_C(0x77be, getRandomNumber_hook, 0x043e, 0x77c1);
  CYC(0x77c1, 0x77c2); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x77c2, 0x77c4); goto normalMovement; } // jr nc
  CYC(0x77c2, 0x77c4);
  CALL_C(0x77c4, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x77c7);
  CYC(0x77c7, 0x77c9); alu_add(gb, 0x04);
  CYC(0x77c9, 0x77cb); alu_and(gb, 0x18);
  CYC(0x77cb, 0x77cc); B = A;
  CYC(0x77cc, 0x77ce); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x77ce, 0x77cf); A = mem_rd(gb, DE);
  CYC(0x77cf, 0x77d1); alu_add(gb, 0x04);
  CYC(0x77d1, 0x77d3); alu_and(gb, 0x18);
  CYC(0x77d3, 0x77d4); alu_cp(gb, B);
  CYC(0x77d4, 0x77d5); H = D;
  if (!(F & FZ)) { CYCT(0x77d5, 0x77d7); goto normalMovement; } // jr nz
  CYC(0x77d5, 0x77d7);
  CYC(0x77d7, 0x77d9); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x77d9, 0x77db); mem_wr(gb, HL, 0x0b);
  CYC(0x77db, 0x77dd); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x77dd, 0x77df); mem_wr(gb, HL, 0x08);
  CYC(0x77df, 0x77e1); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x77e1, 0x77e2); A = mem_rd(gb, HL);
  CYC(0x77e2, 0x77e4); alu_and(gb, 0x0c);
  CYC(0x77e4, 0x77e6); alu_add(gb, 0x02);
  CYC(0x77e6, 0x77e7); mem_wr(gb, HL, A);
  CYC(0x77e7, 0x77ea); enemySetAnimation_hook(gb); return; // jp

normalMovement:
  CYC(0x77ea, 0x77ed); A = mem_rd(gb, wFrameCounter);
  CYC(0x77ed, 0x77ef); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x77ef, 0x77f1); goto L_77f7; } // jr nz
  CYC(0x77ef, 0x77f1);
  CYC(0x77f1, 0x77f3); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x77f3, 0x77f4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(0x77f4, 0x77f7); octogon_beginRisingAboveWater_hook(gb); return; } // jp z
  CYC(0x77f4, 0x77f7);

L_77f7:
  CALL_C(0x77f7, ecom_decCounter1_b0f_hook, 0x439a, 0x77fa);
  if (!(F & FZ)) { CYCT(0x77fa, 0x77fc); octogon_updateMovementAndAnimation_hook(gb); return; } // jr nz
  CYC(0x77fa, 0x77fc);
  CYC(0x77fc, 0x77fe); mem_wr(gb, HL, 60); // [counter1]
  CYC(0x77fe, 0x7800); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7800, 0x7801); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x7801); return; // ret
}

// Moves toward target position and updates animation + sound effects accordingly
void octogon_updateMovementAndAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7802, octogon_moveTowardTargetPosition_hook, 0x7a0a, 0x7805);
  CYC(0x7805, 0x7806); H = D;
  CYC(0x7806, 0x7808); L = ENEMY_BASE + OBJ_VAR35;
  CYC(0x7808, 0x7809); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x7809); return; } // ret nz
  CYC(0x7809, 0x780a);
  CYC(0x780a, 0x780c); mem_wr(gb, HL, 0x0c);
  CYC(0x780c, 0x780e); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x780e, 0x780f); A = mem_rd(gb, HL);
  CYC(0x780f, 0x7811); alu_xor(gb, 0x01);
  CYC(0x7811, 0x7812); mem_wr(gb, HL, A);
  CALL_C(0x7812, enemySetAnimation_hook, 0x282b, 0x7815);
  CYC(0x7815, 0x7817); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7817, 0x7818); A = mem_rd(gb, DE);
  CYC(0x7818, 0x7819); alu_or(gb, A);
  CYC(0x7819, 0x781b); A = 0x88; // SND_LINK_SWIM
  if (!(F & FZ)) { CYCT(0x781b, 0x781e); playSound_b00_hook(gb); return; } // jp nz
  CYC(0x781b, 0x781e);
  octogon_doSplashAnimation_hook(gb); return; // fallthrough
}

// Above-water only (subid 0)
void octogon_doSplashAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x781e, 0x7820); A = 0x6b; // SND_SWORDSPIN
  CALL_C(0x7820, playSound_b00_hook, 0x0c98, 0x7823);
  CALL_C(0x7823, getFreeInteractionSlot_hook, 0x3aef, 0x7826);
  if (!(F & FZ)) { RET_TAKEN(0x7826); return; } // ret nz
  CYC(0x7826, 0x7827);
  CYC(0x7827, 0x7829); mem_wr(gb, HL, 0x8e); // INTERAC_OCTOGON_SPLASH
  CYC(0x7829, 0x782b); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x782b, 0x782c); A = mem_rd(gb, DE);
  CYC(0x782c, 0x782e); alu_and(gb, 0x0c);
  CYC(0x782e, 0x7830); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x7830, 0x7831); mem_wr(gb, HL, A);
  CYC(0x7831, 0x7834); objectCopyPosition_hook(gb); return; // jp
}

// Waiting in place until counter1 reaches 0, then will charge at Link.
void octogon_subid1_belowWater_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7834, ecom_decCounter1_b0f_hook, 0x439a, 0x7837);
  if (!(F & FZ)) { RET_TAKEN(0x7837); return; } // ret nz
  CYC(0x7837, 0x7838);
  CYC(0x7838, 0x783a); mem_wr(gb, HL, 0x0c);
  CYC(0x783a, 0x783b); L = E;
  CYC(0x783b, 0x783c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(0x783c, 0x783e); L = ENEMY_BASE + 0x31; // var31
  CYC(0x783e, 0x7840); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x7840, 0x7841); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7841, 0x7843); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x7843, 0x7844); mem_wr(gb, HL, A);
  CALL_C(0x7844, ecom_updateAngleTowardTarget_b0f_hook, 0x43bf, 0x7847);
  CYC(0x7847, 0x7848); H = D;
  CYC(0x7848, 0x784a); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x784a, 0x784b); A = mem_rd(gb, HL);
  CYC(0x784b, 0x784d); alu_and(gb, 0x0c);
  CYC(0x784d, 0x784e); alu_add(gb, A);
  CYC(0x784e, 0x784f); B = A;
  CYC(0x784f, 0x7851); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7851, 0x7852); A = mem_rd(gb, DE);
  CYC(0x7852, 0x7853); alu_sub(gb, B);
  CYC(0x7853, 0x7855); alu_and(gb, 0x1f);
  CYC(0x7855, 0x7856); B = A;
  CYC(0x7856, 0x7858); alu_sub(gb, 0x04);
  CYC(0x7858, 0x785a); alu_cp(gb, 0x18);
  if (!(F & FC)) { RET_TAKEN(0x785a); return; } // ret nc
  CYC(0x785a, 0x785b);
  CYC(0x785b, 0x785d); alu_bit(gb, 4, B);
  CYC(0x785d, 0x785f); A = 0x04;
  if (F & FZ) { CYCT(0x785f, 0x7861); goto L_7863; } // jr z
  CYC(0x785f, 0x7861);
  CYC(0x7861, 0x7863); A = 0x0c;

L_7863:
  CYC(0x7863, 0x7864); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7864, 0x7866); alu_and(gb, 0x0c);
  CYC(0x7866, 0x7867); mem_wr(gb, HL, A);
  CYC(0x7867, 0x786a); enemySetAnimation_hook(gb); return; // jp
}

// Waiting for a split second before charging
void octogon_subid1_belowWater_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x786a, ecom_decCounter1_b0f_hook, 0x439a, 0x786d);
  if (!(F & FZ)) { RET_TAKEN(0x786d); return; } // ret nz
  CYC(0x786d, 0x786e);
  CYC(0x786e, 0x7870); mem_wr(gb, HL, 90);
  CYC(0x7870, 0x7871); L = E;
  CYC(0x7871, 0x7873); mem_wr(gb, HL, 0x08); // [state]
  CYC(0x7873, 0x7875); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7875, 0x7876); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x7876, 0x7878); alu_add(gb, 0x04);
  CYC(0x7878, 0x787a); alu_and(gb, 0x18);
  CYC(0x787a, 0x787b); alu_rrca(gb);
  CYC(0x787b, 0x787c); mem_wr(gb, HL, A);
  CYC(0x787c, 0x787f); enemySetAnimation_hook(gb); return; // jp
}

// Delay before firing bubble
void octogon_subid1_belowWater_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x787f, ecom_decCounter1_b0f_hook, 0x439a, 0x7882);
  if (!(F & FZ)) { RET_TAKEN(0x7882); return; } // ret nz
  CYC(0x7882, 0x7883);
  CYC(0x7883, 0x7885); mem_wr(gb, HL, 60);
  CYC(0x7885, 0x7886); L = E;
  CYC(0x7886, 0x7887); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c
  CYC(0x7887, 0x7889); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x7889, 0x788a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x788a, 0x788b); A = mem_rd(gb, HL);
  CALL_C(0x788b, enemySetAnimation_hook, 0x282b, 0x788e);
  CALL_C(0x788e, getFreePartSlot_hook, 0x3e8e, 0x7891);
  if (!(F & FZ)) { CYCT(0x7891, 0x7893); goto L_7898; } // jr nz
  CYC(0x7891, 0x7893);
  CYC(0x7893, 0x7895); mem_wr(gb, HL, 0x55); // PART_OCTOGON_BUBBLE
  CALL_C(0x7895, octogon_initializeProjectile_hook, 0x79dd, 0x7898);

L_7898:
  CYC(0x7898, 0x789b); octogon_doSplashAnimation_hook(gb); return; // jp
}

// Delay after firing bubble
void octogon_subid1_belowWater_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x789b, ecom_decCounter1_b0f_hook, 0x439a, 0x789e);
  if (!(F & FZ)) { RET_TAKEN(0x789e); return; } // ret nz
  CYC(0x789e, 0x789f);
  CYC(0x789f, 0x78a0); L = E;
  CYC(0x78a0, 0x78a2); mem_wr(gb, HL, 0x08); // [state]
  CYC(0x78a2, 0x78a4); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x78a4, 0x78a5); A = mem_rd(gb, HL);
  CYC(0x78a5, 0x78a7); alu_and(gb, 0x0c);
  CYC(0x78a7, 0x78a8); mem_wr(gb, HL, A);
  CYC(0x78a8, 0x78ab); enemySetAnimation_hook(gb); return; // jp
}

// Octogon is above water, but Link is below water
void octogon_subid1_aboveWater_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x78ab, 0x78ad); alu_sub(gb, 0x08);
  CYC(0x78ad, 0x78ae); push_effect(gb, 0x78ae);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == 0x78b4) goto state8;
    if (target == 0x78bf) goto state9;
    if (target == 0x78d8) goto stateA;
    HANDOFF(target);
  }

state8:
  CALL_C(0x78b4, ecom_decCounter1_b0f_hook, 0x439a, 0x78b7);
  if (!(F & FZ)) { RET_TAKEN(0x78b7); return; } // ret nz
  CYC(0x78b7, 0x78b8);
  CYC(0x78b8, 0x78ba); mem_wr(gb, HL, 120);
  CYC(0x78ba, 0x78bc); B = 0x48; // PART_OCTOGON_DEPTH_CHARGE
  CYC(0x78bc, 0x78bf); ecom_spawnProjectile_b0f_hook(gb); return; // jp

state9:
  CALL_C(0x78bf, ecom_decCounter1_b0f_hook, 0x439a, 0x78c2);
  if (!(F & FZ)) { RET_TAKEN(0x78c2); return; } // ret nz
  CYC(0x78c2, 0x78c3);
  CYC(0x78c3, 0x78c4); L = E;
  CYC(0x78c4, 0x78c5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(0x78c5, 0x78c7); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x78c7, 0x78c9); A = 0x11;
  CYC(0x78c9, 0x78ca); mem_wr(gb, HL, A);
  CALL_C(0x78ca, enemySetAnimation_hook, 0x282b, 0x78cd);
  CYC(0x78cd, 0x78cf); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(0x78cf, playSound_b00_hook, 0x0c98, 0x78d2);
  CYC(0x78d2, 0x78d5); SET_BC(0x0208);
  CYC(0x78d5, 0x78d8); enemyBoss_spawnShadow_b0f_hook(gb); return; // jp

stateA:
  CYC(0x78d8, 0x78d9); H = D;
  CYC(0x78d9, 0x78db); L = ENEMY_BASE + OBJ_Z;
  CYC(0x78db, 0x78dc); A = mem_rd(gb, HL);
  CYC(0x78dc, 0x78de); alu_sub(gb, 0xc0);
  CYC(0x78de, 0x78df); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x78df, 0x78e0); A = mem_rd(gb, HL);
  CYC(0x78e0, 0x78e2); alu_sbc(gb, 0x00);
  CYC(0x78e2, 0x78e3); mem_wr(gb, HL, A);
  CYC(0x78e3, 0x78e5); alu_cp(gb, 0xd0);
  if (!(F & FC)) { RET_TAKEN(0x78e5); return; } // ret nc
  CYC(0x78e5, 0x78e6);
  CYC(0x78e6, 0x78e8); alu_cp(gb, 0xc0);
  if (!(F & FZ)) { CYCT(0x78e8, 0x78eb); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(0x78e8, 0x78eb);
  CYC(0x78eb, 0x78ed); mem_wr(gb, HL, 0x00);
  CYC(0x78ed, 0x78ee); L = E;
  CYC(0x78ee, 0x78f0); mem_wr(gb, HL, 0x08); // [state] = 8
  CYC(0x78f0, 0x78f2); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x78f2, 0x78f4); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x78f4, 0x78f6); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x78f6, 0x78f8); mem_wr(gb, HL, 60);
  CALL_C(0x78f8, objectSetInvisible_hook, 0x1e7b, 0x78fb);
  CYC(0x78fb, 0x78fe); ecom_killRelatedObj1_b0f_hook(gb); return; // jp
}

// Invisible collision box for the shell
void octogon_subid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x78fe, 0x7900); A = 0x08; // Object.direction
  CALL_C(0x7900, objectGetRelatedObject1Var_hook, 0x2160, 0x7903);
  CYC(0x7903, 0x7904); A = mem_rd(gb, HL);
  CYC(0x7904, 0x7906); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(0x7906, 0x7908); goto L_790a; } // jr c
  CYC(0x7906, 0x7908);
  CYC(0x7908, 0x790a); A = 0x08;

L_790a:
  CYC(0x790a, 0x790c); alu_and(gb, 0x0c);
  CYC(0x790c, 0x790d); push_effect(gb, HL);
  CYC(0x790d, 0x7910); SET_HL(0x7921); // @data
  CYC(0x7910, 0x7911); octogon_addAToHl_from_rst(gb, 0x7911);
  CYC(0x7911, 0x7913); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x7913, 0x7914); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7914, 0x7915); mem_wr(gb, DE, A);
  CYC(0x7915, 0x7916); E = alu_inc8(gb, E);
  CYC(0x7916, 0x7917); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7917, 0x7918); mem_wr(gb, DE, A);
  CYC(0x7918, 0x7919); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7919, 0x791a); C = mem_rd(gb, HL);
  CYC(0x791a, 0x791b); B = A;
  CYC(0x791b, 0x791c); SET_HL(pop_effect(gb));
  CALL_C(0x791c, objectTakePositionWithOffset_hook, 0x2277, 0x791f);
  CYC(0x791f, 0x7920); SET_HL(pop_effect(gb));
  RET(0x7920); return; // ret
}

void octogon_subid0_submergeIntoWater_hook(GB *gb) {
  CYC(0x7931, 0x7932); H = D;
  CYC(0x7932, 0x7934); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7934, 0x7936); mem_wr(gb, HL, 0x0d);
  CYC(0x7936, 0x7938); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7938, 0x793a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x793a, 0x793c); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x793c, 0x793e); mem_wr(gb, HL, 0x01);
  CYC(0x793e, 0x7940); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7940, 0x7942); mem_wr(gb, HL, 0x10);
  CYC(0x7942, 0x7944); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x7944, 0x7946); mem_wr(gb, HL, 90);
  CYC(0x7946, 0x7948); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x7948, 0x794a); A = 0x15;
  CYC(0x794a, 0x794b); mem_wr(gb, HL, A);
  CYC(0x794b, 0x794e); enemySetAnimation_hook(gb); return; // jp
}

void octogon_beginRisingAboveWater_hook(GB *gb) {
  CYC(0x794e, 0x794f); H = D;
  CYC(0x794f, 0x7951); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7951, 0x7953); mem_wr(gb, HL, 0x09);
  CYC(0x7953, 0x7955); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x7955, 0x7957); mem_wr(gb, HL, 0x00);
  CYC(0x7957, 0x7959); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7959, 0x795b); mem_wr(gb, HL, 30);
  CYC(0x795b, 0x795d); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x795d, 0x795f); mem_wr(gb, HL, 90);
  CYC(0x795f, 0x7961); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x7961, 0x7963); A = 0x10;
  CYC(0x7963, 0x7964); mem_wr(gb, HL, A);
  CYC(0x7964, 0x7967); enemySetAnimation_hook(gb); return; // jp
}

// Takes current position, fixes it to the closest valid spot above water, and decides
// a value for var30 (target position index).
void octogon_fixPositionAboveWater_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7967, 0x796a); A = mem_rd(gb, wTmpcfc0_octogonBoss_posNeedsFixing);
  CYC(0x796a, 0x796b); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x796b); return; } // ret z
  CYC(0x796b, 0x796c);
  CYC(0x796c, 0x796d); alu_xor(gb, A);
  CYC(0x796d, 0x7970); mem_wr(gb, wTmpcfc0_octogonBoss_posNeedsFixing, A);
  CYC(0x7970, 0x7971); H = D;
  CYC(0x7971, 0x7973); L = ENEMY_BASE + OBJ_YH;
  CYC(0x7973, 0x7974); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7974, 0x7975); B = A;
  CYC(0x7975, 0x7976); L = alu_inc8(gb, L);
  CYC(0x7976, 0x7977); C = mem_rd(gb, HL);
  CALL_C(0x7977, octogon_getClosestTargetPositionIndex_hook, 0x7a2e, 0x797a);
  CYC(0x797a, 0x797b); E = L;
  CYC(0x797b, 0x797e); A = mem_rd(gb, w1Link_yh);
  CYC(0x797e, 0x797f); B = A;
  CYC(0x797f, 0x7982); A = mem_rd(gb, w1Link_xh);
  CYC(0x7982, 0x7983); C = A;
  CALL_C(0x7983, octogon_getClosestTargetPositionIndex_hook, 0x7a2e, 0x7986);
  CYC(0x7986, 0x7987); alu_cp(gb, L); // BUG: should compare against e, not a
  CYC(0x7987, 0x7988); A = L;
  if (!(F & FZ)) { CYCT(0x7988, 0x798a); goto L_798f; } // jr nz
  CYC(0x7988, 0x798a);
  CYC(0x798a, 0x798d); SET_HL(0x79ce); // @linkCompensationIndices
  CYC(0x798d, 0x798e); octogon_addAToHl_from_rst(gb, 0x798e);
  CYC(0x798e, 0x798f); A = mem_rd(gb, HL);

L_798f:
  CYC(0x798f, 0x7990); alu_add(gb, A);
  CYC(0x7990, 0x7993); SET_HL(0x79aa); // @data
  CYC(0x7993, 0x7994); octogon_addDoubleIndexToHl_from_rst(gb, 0x7994);
  CYC(0x7994, 0x7996); E = ENEMY_BASE + OBJ_YH;
  CYC(0x7996, 0x7997); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7997, 0x7998); mem_wr(gb, DE, A);
  CYC(0x7998, 0x799a); E = ENEMY_BASE + OBJ_XH;
  CYC(0x799a, 0x799b); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x799b, 0x799c); mem_wr(gb, DE, A);
  CYC(0x799c, 0x799e); E = ENEMY_BASE + 0x30; // var30
  CYC(0x799e, 0x799f); A = mem_rd(gb, HL);
  CYC(0x799f, 0x79a0); mem_wr(gb, DE, A);
  CYC(0x79a0, 0x79a1); H = D;
  CYC(0x79a1, 0x79a2); L = E;
  CYC(0x79a2, 0x79a4); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x79a4, 0x79a7); octogon_chooseRandomTargetPosition_hook(gb); return; } // jp nz
  CYC(0x79a4, 0x79a7);
  octogon_loadTargetPosition_hook(gb); return; // jp
}

void octogon_fireOctorokProjectile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x79d7, getFreePartSlot_hook, 0x3e8e, 0x79da);
  if (!(F & FZ)) { RET_TAKEN(0x79da); return; } // ret nz
  CYC(0x79da, 0x79db);
  CYC(0x79db, 0x79dd); mem_wr(gb, HL, 0x18); // PART_OCTOROK_PROJECTILE
  octogon_initializeProjectile_hook(gb); return; // fallthrough
}

// @param h Projectile (could be PART_OCTOROK_PROJECTILE or PART_OCTOGON_BUBBLE)
void octogon_initializeProjectile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x79dd, 0x79df); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x79df, 0x79e0); A = mem_rd(gb, DE);
  CYC(0x79e0, 0x79e2); alu_and(gb, 0x0c);
  CYC(0x79e2, 0x79e3); B = A;
  CYC(0x79e3, 0x79e4); alu_add(gb, A);
  CYC(0x79e4, 0x79e6); L = PART_BASE + OBJ_ANGLE;
  CYC(0x79e6, 0x79e7); mem_wr(gb, HL, A);
  CYC(0x79e7, 0x79e8); A = B;
  CYC(0x79e8, 0x79e9); alu_rrca(gb);
  CYC(0x79e9, 0x79ea); push_effect(gb, HL);
  CYC(0x79ea, 0x79ed); SET_HL(0x79fa); // @positionOffsets
  CYC(0x79ed, 0x79ee); octogon_addAToHl_from_rst(gb, 0x79ee);
  CYC(0x79ee, 0x79ef); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x79ef, 0x79f0); B = A;
  CYC(0x79f0, 0x79f1); C = mem_rd(gb, HL);
  CYC(0x79f1, 0x79f2); SET_HL(pop_effect(gb));
  CALL_C(0x79f2, objectCopyPositionWithOffset_hook, 0x225a, 0x79f5);
  CYC(0x79f5, 0x79f7); A = 0xa6; // SND_STRIKE
  CYC(0x79f7, 0x79fa); playSound_b00_hook(gb); return; // jp
}

void octogon_decVar36IfNonzero_hook(GB *gb) {
  CYC(0x7a02, 0x7a03); H = D;
  CYC(0x7a03, 0x7a05); L = ENEMY_BASE + OBJ_VAR36;
  CYC(0x7a05, 0x7a06); A = mem_rd(gb, HL);
  CYC(0x7a06, 0x7a07); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x7a07); return; } // ret z
  CYC(0x7a07, 0x7a08);
  CYC(0x7a08, 0x7a09); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x7a09); return; // ret
}

// Moves toward position stored in var31/var32. Increments state and sets counter1 to 30
// when it reaches that position.
void octogon_moveTowardTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a0a, 0x7a0b); H = D;
  CYC(0x7a0b, 0x7a0d); L = ENEMY_BASE + 0x31; // var31
  CALL_C(0x7a0d, ecom_readPositionVars_b0f_hook, 0x4439, 0x7a10);
  CYC(0x7a10, 0x7a11); alu_sub(gb, C);
  CYC(0x7a11, 0x7a12); A = alu_inc8(gb, A);
  CYC(0x7a12, 0x7a14); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x7a14, 0x7a17); ecom_moveTowardPosition_b0f_hook(gb); return; } // jp nc
  CYC(0x7a14, 0x7a17);
  CYC(0x7a17, 0x7a19); A = hram_rd(gb, 0x8f); // hFF8F
  CYC(0x7a19, 0x7a1a); alu_sub(gb, B);
  CYC(0x7a1a, 0x7a1b); A = alu_inc8(gb, A);
  CYC(0x7a1b, 0x7a1d); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x7a1d, 0x7a20); ecom_moveTowardPosition_b0f_hook(gb); return; } // jp nc
  CYC(0x7a1d, 0x7a20);
  CYC(0x7a20, 0x7a22); L = ENEMY_BASE + OBJ_YH;
  CYC(0x7a22, 0x7a23); mem_wr(gb, HL, B);
  CYC(0x7a23, 0x7a25); L = ENEMY_BASE + OBJ_XH;
  CYC(0x7a25, 0x7a26); mem_wr(gb, HL, C);
  CYC(0x7a26, 0x7a28); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7a28, 0x7a29); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7a29, 0x7a2b); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7a2b, 0x7a2d); mem_wr(gb, HL, 30);
  RET(0x7a2d); return; // ret
}

// Given a position, this determines the "target position index" (value for var30) which
// that position most closely corresponds to.
// @param bc Position
// @param[out] a
// @param[out] e
void octogon_getClosestTargetPositionIndex_hook(GB *gb) {
  CYC(0x7a2e, 0x7a30); E = 0x00;

checkY:
  CYC(0x7a30, 0x7a31); A = B;
  CYC(0x7a31, 0x7a33); alu_cp(gb, 0x40);
  if (F & FC) { CYCT(0x7a33, 0x7a35); goto checkX; } // jr c
  CYC(0x7a33, 0x7a35);
  CYC(0x7a35, 0x7a37); E = 0x03;
  CYC(0x7a37, 0x7a39); alu_cp(gb, 0x70);
  if (F & FC) { CYCT(0x7a39, 0x7a3b); goto checkX; } // jr c
  CYC(0x7a39, 0x7a3b);
  CYC(0x7a3b, 0x7a3d); E = 0x06;

checkX:
  CYC(0x7a3d, 0x7a3e); A = C;
  CYC(0x7a3e, 0x7a40); alu_cp(gb, 0x50);
  if (F & FC) { CYCT(0x7a40, 0x7a42); goto L_7a48; } // jr c
  CYC(0x7a40, 0x7a42);
  CYC(0x7a42, 0x7a43); E = alu_inc8(gb, E);
  CYC(0x7a43, 0x7a45); alu_cp(gb, 0xa0);
  if (F & FC) { CYCT(0x7a45, 0x7a47); goto L_7a48; } // jr c
  CYC(0x7a45, 0x7a47);
  CYC(0x7a47, 0x7a48); E = alu_inc8(gb, E);

L_7a48:
  CYC(0x7a48, 0x7a49); A = E;
  CYC(0x7a49, 0x7a4b); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(0x7a4b); return; } // ret nz
  CYC(0x7a4b, 0x7a4c);
  CYC(0x7a4c, 0x7a4e); E = 0x00;
  CYC(0x7a4e, 0x7a4f); A = B;
  CYC(0x7a4f, 0x7a51); alu_cp(gb, 0x58);
  if (F & FC) { CYCT(0x7a51, 0x7a53); goto L_7a55; } // jr c
  CYC(0x7a51, 0x7a53);
  CYC(0x7a53, 0x7a55); E = 0x06;

L_7a55:
  CYC(0x7a55, 0x7a56); A = C;
  CYC(0x7a56, 0x7a58); alu_cp(gb, 0x78);
  if (F & FC) { RET_TAKEN(0x7a58); return; } // ret c
  CYC(0x7a58, 0x7a59);
  CYC(0x7a59, 0x7a5a); E = alu_inc8(gb, E);
  CYC(0x7a5a, 0x7a5b); E = alu_inc8(gb, E);
  RET(0x7a5b); return; // ret
}
