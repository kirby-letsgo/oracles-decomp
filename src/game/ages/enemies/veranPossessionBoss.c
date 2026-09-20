#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void enemyCode61_hook(GB *gb);
void veranPossessionBoss_state_uninitialized_hook(GB *gb);
void veranPossessionBoss_state_switchHook_hook(GB *gb);
void veranPossessionBoss_state_stub_hook(GB *gb);
void veranPossessionBoss_subid0_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_state8_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_state9_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_stateA_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_stateB_hook(GB *gb);
void veranPossessionBoss_nayru_stateC_hook(GB *gb);
void veranPossessionBoss_nayru_stateD_hook(GB *gb);
void veranPossessionBoss_doneAttacking_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_stateE_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_stateF_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_state10_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_state11_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_beginMoving_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_state12_hook(GB *gb);
void veranPossessionBoss_nayruAmbi_state13_hook(GB *gb);
void veranPossessionBoss_nayru_state14_hook(GB *gb);
void veranPossessionBoss_subid1_hook(GB *gb);
void veranPossessionBoss_ambi_stateC_hook(GB *gb);
void veranPossessionBoss_ambi_stateD_hook(GB *gb);
void veranPossessionBoss_ambi_state14_hook(GB *gb);
void veranPossessionBoss_subid2_hook(GB *gb);
void veranPossessionBoss_humanForm_state8_hook(GB *gb);
void veranPossessionBoss_humanForm_state9_hook(GB *gb);
void veranPossessionBoss_animate_hook(GB *gb);
void veranPossessionBoss_humanForm_stateA_hook(GB *gb);
void veranPossessionBoss_humanForm_returnToHost_hook(GB *gb);
void veranPossessionBoss_humanForm_stateB_hook(GB *gb);
void veranPossessionBoss_humanForm_stateC_hook(GB *gb);
void veranPossessionBoss_humanForm_stateD_hook(GB *gb);
void veranPossessionBoss_humanForm_stateE_hook(GB *gb);
void veranPossessionBoss_humanForm_stateF_hook(GB *gb);
void veranPossessionBoss_humanForm_state10_hook(GB *gb);
void veranPossessionBoss_subid3_hook(GB *gb);
void veranPossessionBoss_wasHit_hook(GB *gb);

static uint16_t veranPossessionBoss_jump_table(GB *gb) {
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

static void veranPossessionBoss_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_VERAN_POSSESSION_BOSS
//
// Variables:
//   relatedObj1: For subid 2 (veran ghost/human), this is a reference to subid 0 or 1
//                (nayru/ambi form).
//   var30: Animation index
//   var31/var32: Target position when moving
//   var33: Number of hits remaining
//   var34: Current pillar index
//   var35: Bit 0 set if already showed veran's "taunting" text after using switch hook
// ==================================================================================================
void enemyCode61_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x76fa, 0x76fc); goto normalStatus; } // jr z
  CYC(0x76fa, 0x76fc);
  CYC(0x76fc, 0x76fe); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x76fe); return; } // ret c
  CYC(0x76fe, 0x76ff);
  CALL_C(0x76ff, veranPossessionBoss_wasHit_hook, 0x7b11, 0x7702);

normalStatus:
  CALL_C(0x7702, ecom_getSubidAndCpStateTo08_b0e_hook, 0x4426, 0x7705);
  if (F & FC) { CYCT(0x7705, 0x7707); goto commonState; } // jr c
  CYC(0x7705, 0x7707);
  CYC(0x7707, 0x7708); A = B;
  {
    CYC(0x7708, 0x7709); push_effect(gb, 0x7709);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == 0x776e) { veranPossessionBoss_subid0_hook(gb); return; }
    if (target == 0x791f) { veranPossessionBoss_subid1_hook(gb); return; }
    if (target == 0x79b3) { veranPossessionBoss_subid2_hook(gb); return; }
    if (target == 0x7adb) { veranPossessionBoss_subid3_hook(gb); return; }
    HANDOFF(target);
  }

commonState:
  {
    CYC(0x7711, 0x7712); push_effect(gb, 0x7712);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == 0x7722) { veranPossessionBoss_state_uninitialized_hook(gb); return; }
    if (target == 0x776d) { veranPossessionBoss_state_stub_hook(gb); return; }
    if (target == 0x7750) { veranPossessionBoss_state_switchHook_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:7722, bare global; jump-table target from enemyCode61 (commonState).
void veranPossessionBoss_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7722, 0x7724); alu_bit(gb, 1, B);
  if (!(F & FZ)) { CYCT(0x7724, 0x7726); goto skipExtraGfx; } // jr nz
  CYC(0x7724, 0x7726);
  CYC(0x7726, 0x7728); A = 0x61; // ENEMY_VERAN_POSSESSION_BOSS
  CYC(0x7728, 0x772b); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);

skipExtraGfx:
  CYC(0x772b, 0x772c); A = B;
  CYC(0x772c, 0x772d); alu_add(gb, A);
  CYC(0x772d, 0x772e); alu_add(gb, B);
  CYC(0x772e, 0x7730); E = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(0x7730, 0x7731); mem_wr(gb, DE, A);
  CALL_C(0x7731, enemySetAnimation_hook, 0x282b, 0x7734);
  CALL_C(0x7734, objectSetVisible82_hook, 0x1e69, 0x7737);
  CYC(0x7737, 0x7739); A = 0x50; // SPEED_200
  CALL_C(0x7739, ecom_setSpeedAndState8_b0e_hook, 0x4364, 0x773c);
  CYC(0x773c, 0x773e); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x773e, 0x7740); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x7740); return; } // ret z
  CYC(0x7740, 0x7741);
  CYC(0x7741, 0x7743); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x7743, 0x7745); A = 0x01;
  CYC(0x7745, 0x7746); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7746, 0x7747); mem_wr(gb, HL, A);
  CYC(0x7747, 0x7749); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7749, 0x774b); mem_wr(gb, HL, 0x0c);
  CYC(0x774b, 0x774d); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x774d, 0x774f); mem_wr(gb, HL, 0x14); // SPEED_80
  RET(0x774f); return; // ret
}

// 0e:7750, bare global; jump-table target from enemyCode61 (commonState).
void veranPossessionBoss_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7750, 0x7751); E = alu_inc8(gb, E);
  CYC(0x7751, 0x7752); A = mem_rd(gb, DE);
  {
    CYC(0x7752, 0x7753); push_effect(gb, 0x7753);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == 0x775b) goto substate0;
    if (target == 0x2818) { enemyAnimate_hook(gb); return; }
    if (target == 0x7763) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(0x775b, 0x775c); H = D;
  CYC(0x775c, 0x775e); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x775e, 0x7760); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x7760, 0x7763); ecom_incSubstate_b0e_hook(gb); return; // jp

substate3:
  CYC(0x7763, 0x7765); B = 0x0b;
  CALL_C(0x7765, ecom_fallToGroundAndSetState_b0e_hook, 0x44e2, 0x7768);
  CYC(0x7768, 0x776a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x776a, 0x776c); mem_wr(gb, HL, 0x28); // 40
  RET(0x776c); return; // ret
}

// 0e:776d, bare global; jump-table target from enemyCode61 (commonState).
void veranPossessionBoss_state_stub_hook(GB *gb) {
  RET(0x776d); return; // ret
}

// 0e:776e, bare global; jump-table target from enemyCode61. Possessed Nayru.
void veranPossessionBoss_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x776e, 0x776f); A = mem_rd(gb, DE);
  CYC(0x776f, 0x7771); alu_sub(gb, 0x08);
  {
    CYC(0x7771, 0x7772); push_effect(gb, 0x7772);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == 0x778c) { veranPossessionBoss_nayruAmbi_state8_hook(gb); return; }
    if (target == 0x77b5) { veranPossessionBoss_nayruAmbi_state9_hook(gb); return; }
    if (target == 0x7814) { veranPossessionBoss_nayruAmbi_stateA_hook(gb); return; }
    if (target == 0x783d) { veranPossessionBoss_nayruAmbi_stateB_hook(gb); return; }
    if (target == 0x7883) { veranPossessionBoss_nayru_stateC_hook(gb); return; }
    if (target == 0x7890) { veranPossessionBoss_nayru_stateD_hook(gb); return; }
    if (target == 0x78a1) { veranPossessionBoss_nayruAmbi_stateE_hook(gb); return; }
    if (target == 0x78cf) { veranPossessionBoss_nayruAmbi_stateF_hook(gb); return; }
    if (target == 0x78d0) { veranPossessionBoss_nayruAmbi_state10_hook(gb); return; }
    if (target == 0x78e0) { veranPossessionBoss_nayruAmbi_state11_hook(gb); return; }
    if (target == 0x78f4) { veranPossessionBoss_nayruAmbi_state12_hook(gb); return; }
    if (target == 0x7901) { veranPossessionBoss_nayruAmbi_state13_hook(gb); return; }
    if (target == 0x7911) { veranPossessionBoss_nayru_state14_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:778c, bare global; jump-table target from veranPossessionBoss_subid0/subid1
// (nayru/ambi share these states). Initialization.
void veranPossessionBoss_nayruAmbi_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x778c, getFreePartSlot_hook, 0x3e8e, 0x778f);
  if (!(F & FZ)) { RET_TAKEN(0x778f); return; } // ret nz
  CYC(0x778f, 0x7790);
  CYC(0x7790, 0x7792); mem_wr(gb, HL, 0x07); // PART_SHADOW
  CYC(0x7792, 0x7794); L = PART_BASE + OBJ_VAR03;
  CYC(0x7794, 0x7796); mem_wr(gb, HL, 0x06); // Y-offset of shadow relative to self
  CYC(0x7796, 0x7798); L = PART_BASE + OBJ_RELATED1;
  CYC(0x7798, 0x779a); A = 0x80; // Enemy.start == ENEMY_BASE
  CYC(0x779a, 0x779b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x779b, 0x779c); mem_wr(gb, HL, D);
  CALL_C(0x779c, veranPossessionBoss_nayruAmbi_beginMoving_hook, 0x78ea, 0x779f);
  CYC(0x779f, 0x77a1); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x77a1, 0x77a3); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  CYC(0x77a3, 0x77a5); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x77a5, 0x77a7); mem_wr(gb, HL, 0x03);
  CYC(0x77a7, 0x77a8); L = alu_inc8(gb, L);
  CYC(0x77a8, 0x77a9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [var34] = $ff
  CYC(0x77a9, 0x77aa); alu_xor(gb, A);
  CYC(0x77aa, 0x77ad); mem_wr(gb, wTmpcfc0_genericCutscene_cfd0, A);
  CYC(0x77ad, 0x77af); A = 0x2e; // MUS_BOSS
  CYC(0x77af, 0x77b2); mem_wr(gb, wActiveMusic, A);
  CYC(0x77b2, 0x77b5); playSound_b00_hook(gb); return; // jp
}

// 0e:77b5, bare global; jump-table target from veranPossessionBoss_subid0/subid1.
// Flickering before moving.
void veranPossessionBoss_nayruAmbi_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x77b5, ecom_decCounter1_b0e_hook, 0x439a, 0x77b8);
  if (!(F & FZ)) { CYCT(0x77b8, 0x77bb); ecom_flickerVisibility_b0e_hook(gb); return; } // jp nz
  CYC(0x77b8, 0x77bb);
  CYC(0x77bb, 0x77bc); L = E;
  CYC(0x77bc, 0x77bd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x77bd, 0x77bf); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x77bf, 0x77c1); mem_wr(gb, HL, 0xfe); // -2
  CALL_C(0x77c1, objectSetInvisible_hook, 0x1e7b, 0x77c4);

choosePillar:
  CALL_C(0x77c4, getRandomNumber_noPreserveVars_hook, 0x0453, 0x77c7);
  CYC(0x77c7, 0x77c9); alu_and(gb, 0x0e);
  CYC(0x77c9, 0x77cb); alu_cp(gb, 0x0b);
  if (!(F & FC)) { CYCT(0x77cb, 0x77cd); goto choosePillar; } // jr nc
  CYC(0x77cb, 0x77cd);
  CYC(0x77cd, 0x77ce); H = D;
  CYC(0x77ce, 0x77d0); L = ENEMY_BASE + OBJ_VAR34;
  CYC(0x77d0, 0x77d1); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x77d1, 0x77d3); goto choosePillar; } // jr z
  CYC(0x77d1, 0x77d3);
  CYC(0x77d3, 0x77d4); mem_wr(gb, HL, A); // [var34]
  CYC(0x77d4, 0x77d7); SET_HL(0x7800); // @pillarList
  CYC(0x77d7, 0x77d8); veranPossessionBoss_addAToHl_from_rst(gb, 0x77d8);
  CYC(0x77d8, 0x77d9); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x77d9, 0x77da); B = A;
  CYC(0x77da, 0x77db); C = mem_rd(gb, HL);
  PUSH(0x77db, BC);
  CYC(0x77dc, 0x77de); A = mem_rd(gb, hEnemyTargetY);
  CYC(0x77de, 0x77e0); mem_wr(gb, 0xff8f, A);
  CYC(0x77e0, 0x77e2); A = mem_rd(gb, hEnemyTargetX);
  CYC(0x77e2, 0x77e4); mem_wr(gb, 0xff8e, A);
  CALL_C(0x77e4, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x77e7);
  CYC(0x77e7, 0x77e9); alu_add(gb, 0x04);
  CYC(0x77e9, 0x77eb); alu_and(gb, 0x18);
  CYC(0x77eb, 0x77ec); alu_rrca(gb);
  CYC(0x77ec, 0x77ed); alu_rrca(gb);
  CYC(0x77ed, 0x77f0); SET_HL(0x780c); // @pillarOffsets
  CYC(0x77f0, 0x77f1); veranPossessionBoss_addAToHl_from_rst(gb, 0x77f1);
  SET_BC(POP(0x77f1));
  CYC(0x77f2, 0x77f3); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x77f3, 0x77f4); alu_add(gb, B);
  CYC(0x77f4, 0x77f6); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CYC(0x77f6, 0x77f7); mem_wr(gb, DE, A);
  CYC(0x77f7, 0x77f8); A = mem_rd(gb, HL);
  CYC(0x77f8, 0x77f9); alu_add(gb, C);
  CYC(0x77f9, 0x77fa); E = alu_inc8(gb, E);
  CYC(0x77fa, 0x77fb); mem_wr(gb, DE, A); // [var32]
  CYC(0x77fb, 0x77fd); A = 0xc9; // SND_CIRCLING
  CYC(0x77fd, 0x7800); playSound_b00_hook(gb); return; // jp
}

// 0e:7814, bare global; jump-table target from veranPossessionBoss_subid0/subid1.
// Moving to the new position.
void veranPossessionBoss_nayruAmbi_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7814, 0x7815); H = D;
  CYC(0x7815, 0x7817); L = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CALL_C(0x7817, ecom_readPositionVars_b0e_hook, 0x4439, 0x781a);
  CYC(0x781a, 0x781b); alu_sub(gb, C);
  CYC(0x781b, 0x781d); alu_add(gb, 0x02);
  CYC(0x781d, 0x781f); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x781f, 0x7822); ecom_moveTowardPosition_b0e_hook(gb); return; } // jp nc
  CYC(0x781f, 0x7822);
  CYC(0x7822, 0x7824); A = mem_rd(gb, 0xff8f);
  CYC(0x7824, 0x7825); alu_sub(gb, B);
  CYC(0x7825, 0x7827); alu_add(gb, 0x02);
  CYC(0x7827, 0x7829); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x7829, 0x782c); ecom_moveTowardPosition_b0e_hook(gb); return; } // jp nc
  CYC(0x7829, 0x782c);
  CYC(0x782c, 0x782e); L = ENEMY_BASE + OBJ_YH;
  CYC(0x782e, 0x782f); mem_wr(gb, HL, B);
  CYC(0x782f, 0x7831); L = ENEMY_BASE + OBJ_XH;
  CYC(0x7831, 0x7832); mem_wr(gb, HL, C);
  CYC(0x7832, 0x7833); L = E;
  CYC(0x7833, 0x7834); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x7834, 0x7836); L = ENEMY_BASE + OBJ_ZH;
  CYC(0x7836, 0x7838); mem_wr(gb, HL, 0x00);
  CYC(0x7838, 0x783a); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x783a, 0x783c); mem_wr(gb, HL, 0x1e); // 30
  RET(0x783c); return; // ret
}

// 0e:783d, bare global; jump-table target from veranPossessionBoss_subid0/subid1. Just
// reached the new position.
void veranPossessionBoss_nayruAmbi_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x783d, ecom_decCounter1_b0e_hook, 0x439a, 0x7840);
  if (!(F & FZ)) { CYCT(0x7840, 0x7843); ecom_flickerVisibility_b0e_hook(gb); return; } // jp nz
  CYC(0x7840, 0x7843);
  CALL_C(0x7843, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7846);
  CYC(0x7846, 0x7848); alu_and(gb, 0x0f);
  CYC(0x7848, 0x7849); B = A;
  CYC(0x7849, 0x784a); H = D;
  CYC(0x784a, 0x784c); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x784c, 0x784d); A = mem_rd(gb, HL);
  CYC(0x784d, 0x784e); alu_add(gb, A);
  CYC(0x784e, 0x784f); alu_add(gb, A);
  CYC(0x784f, 0x7850); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7850, 0x7852); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7852, 0x7853); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7853, 0x7854); A = alu_dec8(gb, A);
  CYC(0x7854, 0x7857); SET_HL(0x7879); // @attackProbabilities
  CYC(0x7857, 0x7858); veranPossessionBoss_addAToHl_from_rst(gb, 0x7858);
  CYC(0x7858, 0x7859); A = B;
  CYC(0x7859, 0x785a); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x785a, 0x785c); goto beginAttacking; } // jr c
  CYC(0x785a, 0x785c);
  CALL_C(0x785c, veranPossessionBoss_nayruAmbi_beginMoving_hook, 0x78ea, 0x785f);
  CYC(0x785f, 0x7861); mem_wr(gb, HL, 0x1e); // 30
  CYC(0x7861, 0x7864); ecom_flickerVisibility_b0e_hook(gb); return; // jp

beginAttacking:
  CALL_C(0x7864, ecom_incState_b0e_hook, 0x4000, 0x7867);
  CYC(0x7867, 0x7869); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7869, 0x786b); mem_wr(gb, HL, 0x1e); // 30
  CYC(0x786b, 0x786d); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x786d, 0x786f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x786f, 0x7871); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(0x7871, 0x7872); A = mem_rd(gb, HL);
  CYC(0x7872, 0x7873); A = alu_inc8(gb, A);
  CALL_C(0x7873, enemySetAnimation_hook, 0x282b, 0x7876);
  CYC(0x7876, 0x7879); objectSetVisiblec2_hook(gb); return; // jp
}

// 0e:7883, bare global; jump-table target from veranPossessionBoss_subid0. Delay before
// attacking with projectiles (Nayru only).
void veranPossessionBoss_nayru_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7883, ecom_decCounter1_b0e_hook, 0x439a, 0x7886);
  if (!(F & FZ)) { RET_TAKEN(0x7886); return; } // ret nz
  CYC(0x7886, 0x7887);
  CYC(0x7887, 0x7889); mem_wr(gb, HL, 0x8e); // [counter1] = 142
  CYC(0x7889, 0x788a); L = E;
  CYC(0x788a, 0x788b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x788b, 0x788d); B = 0x37; // PART_VERAN_PROJECTILE
  CYC(0x788d, 0x7890); ecom_spawnProjectile_b0e_hook(gb); return; // jp
}

// 0e:7890, bare global; jump-table target from veranPossessionBoss_subid0, also called
// from veranPossessionBoss_ambi_stateD. Attacking with projectiles.
void veranPossessionBoss_nayru_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7890, ecom_decCounter1_b0e_hook, 0x439a, 0x7893);
  if (!(F & FZ)) { RET_TAKEN(0x7893); return; } // ret nz
  CYC(0x7893, 0x7894);
  veranPossessionBoss_doneAttacking_hook(gb); return; // fallthrough
}

// 0e:7894, bare global; falls into from veranPossessionBoss_nayru_stateD, also called
// from veranPossessionBoss_ambi_stateD.
void veranPossessionBoss_doneAttacking_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7894, veranPossessionBoss_nayruAmbi_beginMoving_hook, 0x78ea, 0x7897);
  CYC(0x7897, 0x7899); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7899, 0x789b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x789b, 0x789d); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(0x789d, 0x789e); A = mem_rd(gb, HL);
  CYC(0x789e, 0x78a1); enemySetAnimation_hook(gb); return; // jp
}

// 0e:78a1, bare global; jump-table target from veranPossessionBoss_subid0/subid1. Just
// shot with mystery seeds.
void veranPossessionBoss_nayruAmbi_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x78a1, ecom_decCounter2_b0e_hook, 0x43a3, 0x78a4);
  if (!(F & FZ)) { RET_TAKEN(0x78a4); return; } // ret nz
  CYC(0x78a4, 0x78a5);
  CALL_C(0x78a5, getFreeEnemySlot_uncounted_hook, 0x2e34, 0x78a8);
  if (!(F & FZ)) { RET_TAKEN(0x78a8); return; } // ret nz
  CYC(0x78a8, 0x78a9);
  CYC(0x78a9, 0x78ab); mem_wr(gb, HL, 0x61); // ENEMY_VERAN_POSSESSION_BOSS
  CYC(0x78ab, 0x78ac); L = alu_inc8(gb, L);
  CYC(0x78ac, 0x78ae); mem_wr(gb, HL, 0x02); // [child.subid]
  CYC(0x78ae, 0x78b0); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x78b0, 0x78b1); E = L;
  CYC(0x78b1, 0x78b2); A = mem_rd(gb, DE);
  CYC(0x78b2, 0x78b3); mem_wr(gb, HL, A); // [child.var33] = [this.var33]
  CYC(0x78b3, 0x78b5); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x78b5, 0x78b7); A = 0x80; // Enemy.start == ENEMY_BASE
  CYC(0x78b7, 0x78b8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x78b8, 0x78b9); mem_wr(gb, HL, D);
  CYC(0x78b9, 0x78bc); SET_BC(0xfc04);
  CALL_C(0x78bc, objectCopyPositionWithOffset_hook, 0x225a, 0x78bf);
  CALL_C(0x78bf, ecom_incState_b0e_hook, 0x4000, 0x78c2);
  CYC(0x78c2, 0x78c4); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x78c4, 0x78c6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x78c6, 0x78c8); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x78c8, 0x78ca); A = 0x01;
  CYC(0x78ca, 0x78cb); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x78cb, 0x78cc); mem_wr(gb, HL, A);
  CYC(0x78cc, 0x78cf); objectSetVisible83_hook(gb); return; // jp
}

// 0e:78cf, bare global; jump-table target from veranPossessionBoss_subid0/subid1.
// Collapsed (ghost Veran is showing).
void veranPossessionBoss_nayruAmbi_stateF_hook(GB *gb) {
  RET(0x78cf); return; // ret
}

// 0e:78d0, bare global; jump-table target from veranPossessionBoss_subid0/subid1. Veran
// just returned to nayru/ambi's body.
void veranPossessionBoss_nayruAmbi_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x78d0, 0x78d1); H = D;
  CYC(0x78d1, 0x78d2); L = E;
  CYC(0x78d2, 0x78d3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x78d3, 0x78d5); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x78d5, 0x78d7); A = 0x06;
  CYC(0x78d7, 0x78d8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x78d8, 0x78d9); mem_wr(gb, HL, A);
  CYC(0x78d9, 0x78db); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x78db, 0x78dd); mem_wr(gb, HL, 0x0f);
  CYC(0x78dd, 0x78e0); objectSetVisible82_hook(gb); return; // jp
}

// 0e:78e0, bare global; jump-table target from veranPossessionBoss_subid0/subid1.
// Remains collapsed on the floor for a few frames before moving again.
void veranPossessionBoss_nayruAmbi_state11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x78e0, ecom_decCounter1_b0e_hook, 0x439a, 0x78e3);
  if (!(F & FZ)) { RET_TAKEN(0x78e3); return; } // ret nz
  CYC(0x78e3, 0x78e4);
  CYC(0x78e4, 0x78e6); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(0x78e6, 0x78e7); A = mem_rd(gb, HL);
  CALL_C(0x78e7, enemySetAnimation_hook, 0x282b, 0x78ea);
  veranPossessionBoss_nayruAmbi_beginMoving_hook(gb); return; // fallthrough
}

// 0e:78ea, bare global; falls into from veranPossessionBoss_nayruAmbi_state11, also
// called from several other states.
void veranPossessionBoss_nayruAmbi_beginMoving_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x78ea, 0x78eb); H = D;
  CYC(0x78eb, 0x78ed); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x78ed, 0x78ef); mem_wr(gb, HL, 0x09);
  CYC(0x78ef, 0x78f1); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x78f1, 0x78f3); mem_wr(gb, HL, 0x3c); // 60
  RET(0x78f3); return; // ret
}

// 0e:78f4, bare global; jump-table target from veranPossessionBoss_subid0/subid1. Veran
// was just defeated.
void veranPossessionBoss_nayruAmbi_state12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x78f4, 0x78f7); A = mem_rd(gb, wTextIsActive);
  CYC(0x78f7, 0x78f8); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x78f8); return; } // ret nz
  CYC(0x78f8, 0x78f9);
  CALL_C(0x78f9, ecom_incState_b0e_hook, 0x4000, 0x78fc);
  CYC(0x78fc, 0x78fe); A = 0x02;
  CYC(0x78fe, 0x7901); fadeoutToWhiteWithDelay_hook(gb); return; // jp
}

// 0e:7901, bare global; jump-table target from veranPossessionBoss_subid0/subid1.
// Waiting for the screen to go white.
void veranPossessionBoss_nayruAmbi_state13_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7901, 0x7904); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7904, 0x7905); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7905); return; } // ret nz
  CYC(0x7905, 0x7906);
  CALL_C(0x7906, ecom_incState_b0e_hook, 0x4000, 0x7909);
  CYC(0x7909, 0x790c); SET_HL(0x19ad); // clearAllItemsAndPutLinkOnGround
  CYC(0x790c, 0x790e); E = 0x00;
  CYC(0x790e, 0x7911); interBankCall_hook(gb); return; // jp
}

// 0e:7911, bare global; jump-table target from veranPossessionBoss_subid0. Deletes all
// objects (including self), resumes cutscene with a newly created object.
void veranPossessionBoss_nayru_state14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7911, clearWramBank1_hook, 0x3597, 0x7914);
  CYC(0x7914, 0x7917); SET_HL(0xd000); // w1Link
  CYC(0x7917, 0x7919); mem_wr(gb, HL, 0x03); // [w1Link.enabled]
  CALL_C(0x7919, getFreeInteractionSlot_hook, 0x3aef, 0x791c);
  CYC(0x791c, 0x791e); mem_wr(gb, HL, 0x6e); // INTERAC_NAYRU_SAVED_CUTSCENE
  RET(0x791e); return; // ret
}

// 0e:791f, bare global; jump-table target from enemyCode61. Possessed Ambi.
void veranPossessionBoss_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x791f, 0x7920); A = mem_rd(gb, DE);
  CYC(0x7920, 0x7922); alu_sub(gb, 0x08);
  {
    CYC(0x7922, 0x7923); push_effect(gb, 0x7923);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == 0x778c) { veranPossessionBoss_nayruAmbi_state8_hook(gb); return; }
    if (target == 0x77b5) { veranPossessionBoss_nayruAmbi_state9_hook(gb); return; }
    if (target == 0x7814) { veranPossessionBoss_nayruAmbi_stateA_hook(gb); return; }
    if (target == 0x783d) { veranPossessionBoss_nayruAmbi_stateB_hook(gb); return; }
    if (target == 0x793d) { veranPossessionBoss_ambi_stateC_hook(gb); return; }
    if (target == 0x7966) { veranPossessionBoss_ambi_stateD_hook(gb); return; }
    if (target == 0x78a1) { veranPossessionBoss_nayruAmbi_stateE_hook(gb); return; }
    if (target == 0x78cf) { veranPossessionBoss_nayruAmbi_stateF_hook(gb); return; }
    if (target == 0x78d0) { veranPossessionBoss_nayruAmbi_state10_hook(gb); return; }
    if (target == 0x78e0) { veranPossessionBoss_nayruAmbi_state11_hook(gb); return; }
    if (target == 0x78f4) { veranPossessionBoss_nayruAmbi_state12_hook(gb); return; }
    if (target == 0x7901) { veranPossessionBoss_nayruAmbi_state13_hook(gb); return; }
    if (target == 0x7982) { veranPossessionBoss_ambi_state14_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:793d, bare global; jump-table target from veranPossessionBoss_subid1. Delay before
// attacking with projectiles or spawning spiders (Ambi only).
void veranPossessionBoss_ambi_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x793d, ecom_decCounter1_b0e_hook, 0x439a, 0x7940);
  if (!(F & FZ)) { RET_TAKEN(0x7940); return; } // ret nz
  CYC(0x7940, 0x7941);
  CYC(0x7941, 0x7943); mem_wr(gb, HL, 0x8e); // [counter1] = 142
  CYC(0x7943, 0x7944); L = E;
  CYC(0x7944, 0x7945); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(0x7945, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7948);
  CYC(0x7948, 0x794a); alu_and(gb, 0x0f);
  CYC(0x794a, 0x794b); B = A;
  CYC(0x794b, 0x794d); E = ENEMY_BASE + OBJ_VAR33;
  CYC(0x794d, 0x794e); A = mem_rd(gb, DE);
  CYC(0x794e, 0x794f); A = alu_dec8(gb, A);
  CYC(0x794f, 0x7952); SET_HL(0x7961); // @spiderSpawnProbabilities
  CYC(0x7952, 0x7953); veranPossessionBoss_addAToHl_from_rst(gb, 0x7953);
  CYC(0x7953, 0x7954); A = B;
  CYC(0x7954, 0x7955); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x7955, 0x7956); H = D;
  CYC(0x7956, 0x7958); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x7958, 0x795a); mem_wr(gb, HL, 0x01);
  if (!(F & FC)) { RET_TAKEN(0x795a); return; } // ret nc
  CYC(0x795a, 0x795b);
  CYC(0x795b, 0x795c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x795c, 0x795e); B = 0x37; // PART_VERAN_PROJECTILE
  CYC(0x795e, 0x7961); ecom_spawnProjectile_b0e_hook(gb); return; // jp
}

// 0e:7966, bare global; jump-table target from veranPossessionBoss_subid1. Attacking
// with projectiles or spiders (Ambi only).
void veranPossessionBoss_ambi_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7966, 0x7968); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x7968, 0x7969); A = mem_rd(gb, DE);
  CYC(0x7969, 0x796a); alu_or(gb, A);
  if (F & FZ) { CYCT(0x796a, 0x796d); veranPossessionBoss_nayru_stateD_hook(gb); return; } // jp z
  CYC(0x796a, 0x796d);
  CALL_C(0x796d, ecom_decCounter1_b0e_hook, 0x439a, 0x7970);
  if (F & FZ) { CYCT(0x7970, 0x7973); veranPossessionBoss_doneAttacking_hook(gb); return; } // jp z
  CYC(0x7970, 0x7973);
  CYC(0x7973, 0x7974); A = mem_rd(gb, HL); // [counter1]
  CYC(0x7974, 0x7976); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(0x7976); return; } // ret nz
  CYC(0x7976, 0x7977);
  CYC(0x7977, 0x797a); A = mem_rd(gb, wNumEnemies);
  CYC(0x797a, 0x797c); alu_cp(gb, 0x06);
  if (!(F & FC)) { RET_TAKEN(0x797c); return; } // ret nc
  CYC(0x797c, 0x797d);
  CYC(0x797d, 0x797f); B = 0x0f; // ENEMY_VERAN_SPIDER
  CYC(0x797f, 0x7982); ecom_spawnEnemyWithSubid01_b0e_hook(gb); return; // jp
}

// 0e:7982, bare global; jump-table target from veranPossessionBoss_subid1.
// Ambi-specific cutscene after Veran is defeated.
void veranPossessionBoss_ambi_state14_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7982, 0x7985); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7985, 0x7986); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7986); return; } // ret nz
  CYC(0x7986, 0x7987);
  CALL_C(0x7987, clearWramBank1_hook, 0x3597, 0x798a);
  CYC(0x798a, 0x798c); A = 0x01;
  CYC(0x798c, 0x798f); mem_wr(gb, wNumEnemies, A);
  CYC(0x798f, 0x7992); SET_HL(0xd000); // w1Link
  CYC(0x7992, 0x7994); mem_wr(gb, HL, 0x03); // [w1Link.enabled]
  CYC(0x7994, 0x7996); L = 0x0b; // w1Link.yh
  CYC(0x7996, 0x7998); mem_wr(gb, HL, 0x58);
  CYC(0x7998, 0x799a); L = 0x0d; // w1Link.xh
  CYC(0x799a, 0x799c); mem_wr(gb, HL, 0x78);
  CALL_C(0x799c, setCameraFocusedObjectToLink_hook, 0x12f0, 0x799f);
  CALL_C(0x799f, resetCamera_hook, 0x12ce, 0x79a2);
  CALL_C(0x79a2, getFreeEnemySlot_uncounted_hook, 0x2e34, 0x79a5);
  CYC(0x79a5, 0x79a7); mem_wr(gb, HL, 0x61); // ENEMY_VERAN_POSSESSION_BOSS
  CYC(0x79a7, 0x79a8); L = alu_inc8(gb, L);
  CYC(0x79a8, 0x79aa); mem_wr(gb, HL, 0x03); // [subid]
  CYC(0x79aa, 0x79ac); L = ENEMY_BASE + OBJ_YH;
  CYC(0x79ac, 0x79ae); mem_wr(gb, HL, 0x48);
  CYC(0x79ae, 0x79b0); L = ENEMY_BASE + OBJ_XH;
  CYC(0x79b0, 0x79b2); mem_wr(gb, HL, 0x78);
  RET(0x79b2); return; // ret
}

// 0e:79b3, bare global; jump-table target from enemyCode61. Veran emerged in human
// form.
void veranPossessionBoss_subid2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x79b3, 0x79b4); A = mem_rd(gb, DE);
  CYC(0x79b4, 0x79b6); alu_sub(gb, 0x08);
  {
    CYC(0x79b6, 0x79b7); push_effect(gb, 0x79b7);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == 0x79c9) { veranPossessionBoss_humanForm_state8_hook(gb); return; }
    if (target == 0x79f6) { veranPossessionBoss_humanForm_state9_hook(gb); return; }
    if (target == 0x7a0a) { veranPossessionBoss_humanForm_stateA_hook(gb); return; }
    if (target == 0x7a24) { veranPossessionBoss_humanForm_stateB_hook(gb); return; }
    if (target == 0x7a40) { veranPossessionBoss_humanForm_stateC_hook(gb); return; }
    if (target == 0x7a68) { veranPossessionBoss_humanForm_stateD_hook(gb); return; }
    if (target == 0x7a92) { veranPossessionBoss_humanForm_stateE_hook(gb); return; }
    if (target == 0x7ab7) { veranPossessionBoss_humanForm_stateF_hook(gb); return; }
    if (target == 0x7ac2) { veranPossessionBoss_humanForm_state10_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:79c9, bare global; jump-table target from veranPossessionBoss_subid2. Moving
// upward just after spawning.
void veranPossessionBoss_humanForm_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x79c9, objectApplySpeed_hook, 0x201d, 0x79cc);
  CALL_C(0x79cc, ecom_decCounter1_b0e_hook, 0x439a, 0x79cf);
  if (!(F & FZ)) { CYCT(0x79cf, 0x79d1); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(0x79cf, 0x79d1);
  CYC(0x79d1, 0x79d3); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CYC(0x79d3, 0x79d5); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x79d5, 0x79d6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x79d6, 0x79d8); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x79d8, 0x79da); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x79da, 0x79dc); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x79dc, 0x79de); mem_wr(gb, HL, 0x5b); // ENEMYCOLLISION_VERAN_GHOST
  CYC(0x79de, 0x79e0); A = 0x02; // Object.subid
  CALL_C(0x79e0, objectGetRelatedObject1Var_hook, 0x2160, 0x79e3);
  CYC(0x79e3, 0x79e4); A = mem_rd(gb, HL);
  CYC(0x79e4, 0x79e5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x79e5, 0x79e7); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(0x79e5, 0x79e7);
  CYC(0x79e7, 0x79e9); L = ENEMY_BASE + OBJ_VAR35;
  CYC(0x79e9, 0x79eb); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x79eb, 0x79ed); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(0x79eb, 0x79ed);
  CYC(0x79ed, 0x79ee); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [var35] |= 1
  CYC(0x79ee, 0x79f1); SET_BC(0x2f2a); // TX_2f2a
  CALL_C(0x79f1, showText_hook, 0x1872, 0x79f4);
  CYC(0x79f4, 0x79f6); veranPossessionBoss_animate_hook(gb); return; // jr
}

// 0e:79f6, bare global; jump-table target from veranPossessionBoss_subid2. Waiting for
// Link to use switch hook.
void veranPossessionBoss_humanForm_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x79f6, ecom_decCounter1_b0e_hook, 0x439a, 0x79f9);
  if (!(F & FZ)) { CYCT(0x79f9, 0x79fb); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(0x79f9, 0x79fb);
  CYC(0x79fb, 0x79fd); mem_wr(gb, HL, 0x0c); // [counter1] = 12
  CYC(0x79fd, 0x79fe); L = E;
  CYC(0x79fe, 0x79ff); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x79ff, 0x7a01); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7a01, 0x7a03); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x7a03, 0x7a05); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7a05, 0x7a07); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  veranPossessionBoss_animate_hook(gb); return; // fallthrough
}

// 0e:7a07, bare global; falls into from veranPossessionBoss_humanForm_state9, also
// reached via tail-jump from several other states.
void veranPossessionBoss_animate_hook(GB *gb) {
  CYC(0x7a07, 0x7a0a); enemyAnimate_hook(gb); return; // jp
}

// 0e:7a0a, bare global; jump-table target from veranPossessionBoss_subid2. Moving down
// to re-possess her victim.
void veranPossessionBoss_humanForm_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7a0a, objectApplySpeed_hook, 0x201d, 0x7a0d);
  CALL_C(0x7a0d, ecom_decCounter1_b0e_hook, 0x439a, 0x7a10);
  if (!(F & FZ)) { CYCT(0x7a10, 0x7a12); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(0x7a10, 0x7a12);
  CYC(0x7a12, 0x7a14); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7a14, 0x7a16); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  veranPossessionBoss_humanForm_returnToHost_hook(gb); return; // fallthrough
}

// 0e:7a16, bare global; falls into from veranPossessionBoss_humanForm_stateA, also
// called from veranPossessionBoss_humanForm_stateD.
void veranPossessionBoss_humanForm_returnToHost_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a16, 0x7a18); A = 0x04; // Object.state
  CALL_C(0x7a18, objectGetRelatedObject1Var_hook, 0x2160, 0x7a1b);
  CYC(0x7a1b, 0x7a1c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7a1c, 0x7a1e); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7a1e, 0x7a1f); E = L;
  CYC(0x7a1f, 0x7a20); A = mem_rd(gb, DE);
  CYC(0x7a20, 0x7a21); mem_wr(gb, HL, A);
  CYC(0x7a21, 0x7a24); enemyDelete_hook(gb); return; // jp
}

// 0e:7a24, bare global; jump-table target from veranPossessionBoss_subid2. Just
// finished using switch hook on ghost. Flickering between ghost and human forms.
void veranPossessionBoss_humanForm_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7a24, ecom_decCounter1_b0e_hook, 0x439a, 0x7a27);
  if (!(F & FZ)) { CYCT(0x7a27, 0x7a29); goto flickerBetweenForms; } // jr nz
  CYC(0x7a27, 0x7a29);
  CYC(0x7a29, 0x7a2b); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CYC(0x7a2b, 0x7a2d); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x7a2d, 0x7a2f); mem_wr(gb, HL, 0x5c); // ENEMYCOLLISION_VERAN_HUMAN
  CYC(0x7a2f, 0x7a30); L = E;
  CYC(0x7a30, 0x7a31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x7a31, 0x7a33); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7a33, 0x7a35); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

flickerBetweenForms:
  CYC(0x7a35, 0x7a36); A = mem_rd(gb, HL); // [counter1]
  CYC(0x7a36, 0x7a37); alu_rrca(gb);
  CYC(0x7a37, 0x7a39); A = 0x09;
  if (F & FC) { CYCT(0x7a39, 0x7a3b); goto setFlickerAnimation; } // jr c
  CYC(0x7a39, 0x7a3b);
  CYC(0x7a3b, 0x7a3d); A = 0x06;

setFlickerAnimation:
  CYC(0x7a3d, 0x7a40); enemySetAnimation_hook(gb); return; // jp
}

// 0e:7a40, bare global; jump-table target from veranPossessionBoss_subid2. Veran is
// vulnerable to attacks.
void veranPossessionBoss_humanForm_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7a40, ecom_decCounter1_b0e_hook, 0x439a, 0x7a43);
  if (!(F & FZ)) { CYCT(0x7a43, 0x7a45); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(0x7a43, 0x7a45);
  CYC(0x7a45, 0x7a46); L = E;
  CYC(0x7a46, 0x7a47); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x7a47, 0x7a49); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7a49, 0x7a4b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x7a4b, 0x7a4d); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x7a4d, 0x7a4f); mem_wr(gb, HL, 0x64); // SPEED_280
  CYC(0x7a4f, 0x7a51); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x7a51, 0x7a53); A = 0x80;
  CYC(0x7a53, 0x7a54); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a -- speedZ = -$280
  CYC(0x7a54, 0x7a56); mem_wr(gb, HL, 0xfd);
  CALL_C(0x7a56, objectSetVisiblec1_hook, 0x1e3c, 0x7a59);
  CYC(0x7a59, 0x7a5b); A = 0x0b; // Object.yh
  CALL_C(0x7a5b, objectGetRelatedObject1Var_hook, 0x2160, 0x7a5e);
  CYC(0x7a5e, 0x7a60); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CYC(0x7a60, 0x7a61); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7a61, 0x7a62); mem_wr(gb, DE, A); // [this.var31]
  CYC(0x7a62, 0x7a63); E = alu_inc8(gb, E);
  CYC(0x7a63, 0x7a64); L = alu_inc8(gb, L);
  CYC(0x7a64, 0x7a65); A = mem_rd(gb, HL);
  CYC(0x7a65, 0x7a66); mem_wr(gb, DE, A); // [this.var32]
  CYC(0x7a66, 0x7a68); veranPossessionBoss_animate_hook(gb); return; // jr
}

// 0e:7a68, bare global; jump-table target from veranPossessionBoss_subid2. Moving back
// to nayru/ambi.
void veranPossessionBoss_humanForm_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a68, 0x7a6a); C = 0x20;
  CALL_C(0x7a6a, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7a6d);
  CYC(0x7a6d, 0x7a6f); L = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CALL_C(0x7a6f, ecom_readPositionVars_b0e_hook, 0x4439, 0x7a72);
  CYC(0x7a72, 0x7a73); alu_sub(gb, C);
  CYC(0x7a73, 0x7a75); alu_add(gb, 0x02);
  CYC(0x7a75, 0x7a77); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x7a77, 0x7a7a); ecom_moveTowardPosition_b0e_hook(gb); return; } // jp nc
  CYC(0x7a77, 0x7a7a);
  CYC(0x7a7a, 0x7a7c); A = mem_rd(gb, 0xff8f);
  CYC(0x7a7c, 0x7a7d); alu_sub(gb, B);
  CYC(0x7a7d, 0x7a7f); alu_add(gb, 0x02);
  CYC(0x7a7f, 0x7a81); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x7a81, 0x7a84); ecom_moveTowardPosition_b0e_hook(gb); return; } // jp nc
  CYC(0x7a81, 0x7a84);
  CYC(0x7a84, 0x7a86); L = ENEMY_BASE + OBJ_YH;
  CYC(0x7a86, 0x7a87); mem_wr(gb, HL, B);
  CYC(0x7a87, 0x7a89); L = ENEMY_BASE + OBJ_XH;
  CYC(0x7a89, 0x7a8a); mem_wr(gb, HL, C);
  CYC(0x7a8a, 0x7a8c); E = ENEMY_BASE + OBJ_ZH;
  CYC(0x7a8c, 0x7a8d); A = mem_rd(gb, DE);
  CYC(0x7a8d, 0x7a8e); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7a8e); return; } // ret nz
  CYC(0x7a8e, 0x7a8f);
  CYC(0x7a8f, 0x7a92); veranPossessionBoss_humanForm_returnToHost_hook(gb); return; // jp
}

// 0e:7a92, bare global; jump-table target from veranPossessionBoss_subid2. Health is
// zero; about to begin the cutscene.
void veranPossessionBoss_humanForm_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a92, 0x7a94); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x7a94, 0x7a95); A = mem_rd(gb, DE);
  CYC(0x7a95, 0x7a96); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7a96); return; } // ret nz
  CYC(0x7a96, 0x7a97);
  CALL_C(0x7a97, checkLinkCollisionsEnabled_hook, 0x1d32, 0x7a9a);
  if (!(F & FC)) { RET_TAKEN(0x7a9a); return; } // ret nc
  CYC(0x7a9a, 0x7a9b);
  CYC(0x7a9b, 0x7a9e); SET_BC(0x0502); // INTERAC_PUFF,$02
  CALL_C(0x7a9e, objectCreateInteraction_hook, 0x24c5, 0x7aa1);
  if (!(F & FZ)) { RET_TAKEN(0x7aa1); return; } // ret nz
  CYC(0x7aa1, 0x7aa2);
  CYC(0x7aa2, 0x7aa3); A = H;
  CYC(0x7aa3, 0x7aa4); H = D;
  CYC(0x7aa4, 0x7aa6); L = ENEMY_BASE + OBJ_RELATED2 + 1; // Enemy.relatedObj2+1
  CYC(0x7aa6, 0x7aa7); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(0x7aa7, 0x7aa9); mem_wr(gb, HL, 0x40); // Interaction.start == INTERACTION_BASE
  CYC(0x7aa9, 0x7aab); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7aab, 0x7aac); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7aac, 0x7aae); A = 0x01; // DISABLE_LINK
  CYC(0x7aae, 0x7ab1); mem_wr(gb, wDisabledObjects, A);
  CYC(0x7ab1, 0x7ab4); mem_wr(gb, wMenuDisabled, A);
  CYC(0x7ab4, 0x7ab7); objectSetInvisible_hook(gb); return; // jp
}

// 0e:7ab7, bare global; jump-table target from veranPossessionBoss_subid2. Waiting for
// the puff to finish its animation.
void veranPossessionBoss_humanForm_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7ab7, 0x7ab9); A = 0x21; // Object.animParameter
  CALL_C(0x7ab9, objectGetRelatedObject2Var_hook, 0x2164, 0x7abc);
  CYC(0x7abc, 0x7abe); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x7abe); return; } // ret z
  CYC(0x7abe, 0x7abf);
  CYC(0x7abf, 0x7ac2); ecom_incState_b0e_hook(gb); return; // jp
}

// 0e:7ac2, bare global; jump-table target from veranPossessionBoss_subid2. Sets
// nayru/ambi's state to $12, shows text, then deletes self.
void veranPossessionBoss_humanForm_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7ac2, 0x7ac4); A = 0x04; // Object.state
  CALL_C(0x7ac4, objectGetRelatedObject1Var_hook, 0x2160, 0x7ac7);
  CYC(0x7ac7, 0x7ac9); mem_wr(gb, HL, 0x12);
  CYC(0x7ac9, 0x7acb); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7acb, 0x7acd); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(0x7acd, 0x7ad0); SET_BC(0x560b); // TX_560b
  if (F & FZ) { CYCT(0x7ad0, 0x7ad2); goto showText; } // jr z
  CYC(0x7ad0, 0x7ad2);
  CYC(0x7ad2, 0x7ad5); SET_BC(0x5611); // TX_5611

showText:
  CALL_C(0x7ad5, showText_hook, 0x1872, 0x7ad8);
  CYC(0x7ad8, 0x7adb); enemyDelete_hook(gb); return; // jp
}

// 0e:7adb, bare global; jump-table target from enemyCode61. Collapsed Ambi after the
// fight.
void veranPossessionBoss_subid3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7adb, 0x7adc); A = mem_rd(gb, DE);
  CYC(0x7adc, 0x7ade); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x7ade, 0x7ae0); goto state9; } // jr nz
  CYC(0x7ade, 0x7ae0);

  CYC(0x7ae0, 0x7ae3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7ae3, 0x7ae4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7ae4); return; } // ret nz
  CYC(0x7ae4, 0x7ae5);
  CALL_C(0x7ae5, ecom_incState_b0e_hook, 0x4000, 0x7ae8);
  CYC(0x7ae8, 0x7aea); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7aea, 0x7aec); mem_wr(gb, HL, 0x3c); // 60
  CYC(0x7aec, 0x7aee); A = 0x05;
  CALL_C(0x7aee, enemySetAnimation_hook, 0x282b, 0x7af1);
  CYC(0x7af1, 0x7af4); fadeinFromWhite_hook(gb); return; // jp

state9:
  CYC(0x7af4, 0x7af7); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x7af7, 0x7af8); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7af8); return; } // ret nz
  CYC(0x7af8, 0x7af9);
  CALL_C(0x7af9, ecom_decCounter2_b0e_hook, 0x43a3, 0x7afc);
  if (!(F & FZ)) { RET_TAKEN(0x7afc); return; } // ret nz
  CYC(0x7afc, 0x7afd);
  CALL_C(0x7afd, getFreeInteractionSlot_hook, 0x3aef, 0x7b00);
  if (!(F & FZ)) { RET_TAKEN(0x7b00); return; } // ret nz
  CYC(0x7b00, 0x7b01);
  CYC(0x7b01, 0x7b03); mem_wr(gb, HL, 0x4d); // INTERAC_AMBI
  CYC(0x7b03, 0x7b04); L = alu_inc8(gb, L);
  CYC(0x7b04, 0x7b06); mem_wr(gb, HL, 0x07); // [subid]
  CALL_C(0x7b06, objectCopyPosition_hook, 0x2242, 0x7b09);
  CYC(0x7b09, 0x7b0b); A = 0x01; // TREE_GFXH_01
  CYC(0x7b0b, 0x7b0e); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(0x7b0e, 0x7b11); enemyDelete_hook(gb); return; // jp
}

// 0e:7b11, bare global; called from enemyCode61.
void veranPossessionBoss_wasHit_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7b11, 0x7b12); H = D;
  CYC(0x7b12, 0x7b14); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(0x7b14, 0x7b16); mem_wr(gb, HL, 0x00);
  CYC(0x7b16, 0x7b18); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7b18, 0x7b19); A = mem_rd(gb, DE);
  CYC(0x7b19, 0x7b1b); alu_cp(gb, 0x02);
  CYC(0x7b1b, 0x7b1d); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x7b1d, 0x7b1e); A = mem_rd(gb, HL);
  if (F & FZ) { CYCT(0x7b1e, 0x7b20); goto subid2; } // jr z
  CYC(0x7b1e, 0x7b20);
  CYC(0x7b20, 0x7b22); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x7b22, 0x7b24); alu_cp(gb, 0x1a); // $80|ITEMCOLLISION_MYSTERY_SEED - $80
  if (F & FZ) { CYCT(0x7b24, 0x7b26); goto mysterySeed; } // jr z
  CYC(0x7b24, 0x7b26);
  CYC(0x7b26, 0x7b28); alu_sub(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  if (F & FC) { RET_TAKEN(0x7b28); return; } // ret c
  CYC(0x7b28, 0x7b29);
  CYC(0x7b29, 0x7b2b); alu_cp(gb, 0x09); // ITEMCOLLISION_SHOVEL-ITEMCOLLISION_L1_SWORD+1
  if (!(F & FC)) { RET_TAKEN(0x7b2b); return; } // ret nc
  CYC(0x7b2b, 0x7b2c);
  CYC(0x7b2c, 0x7b2e); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x7b2e, 0x7b30); mem_wr(gb, HL, 0xe8); // -24
  CYC(0x7b30, 0x7b33); SET_HL(w1Link_invincibilityCounter);
  CYC(0x7b33, 0x7b35); mem_wr(gb, HL, 0x28); // 40
  CYC(0x7b35, 0x7b36); L = alu_inc8(gb, L); // w1Link.knockbackAngle
  CYC(0x7b36, 0x7b38); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(0x7b38, 0x7b39); A = mem_rd(gb, DE);
  CYC(0x7b39, 0x7b3b); alu_xor(gb, 0x10);
  CYC(0x7b3b, 0x7b3c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7b3c, 0x7b3e); mem_wr(gb, HL, 0x15); // [w1Link.knockbackCounter] = 21
  CYC(0x7b3e, 0x7b40); L = 0x25; // w1Link.damageToApply
  CYC(0x7b40, 0x7b42); mem_wr(gb, HL, 0xf8); // -8
  RET(0x7b42); return; // ret

mysterySeed:
  CYC(0x7b43, 0x7b45); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7b45, 0x7b47); mem_wr(gb, HL, 0x0e);
  CYC(0x7b47, 0x7b49); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x7b49, 0x7b4b); mem_wr(gb, HL, 0x1e); // 30
  CYC(0x7b4b, 0x7b4d); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7b4d, 0x7b4f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x7b4f, 0x7b51); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(0x7b51, 0x7b52); A = mem_rd(gb, HL);
  CYC(0x7b52, 0x7b54); alu_add(gb, 0x02);
  CYC(0x7b54, 0x7b57); enemySetAnimation_hook(gb); return; // jp

subid2:
  CYC(0x7b57, 0x7b59); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x7b59, 0x7b5b); alu_cp(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  if (F & FC) { RET_TAKEN(0x7b5b); return; } // ret c
  CYC(0x7b5b, 0x7b5c);
  CYC(0x7b5c, 0x7b5e); alu_cp(gb, 0x0c); // ITEMCOLLISION_EXPERT_PUNCH+1
  if (!(F & FC)) { RET_TAKEN(0x7b5e); return; } // ret nc
  CYC(0x7b5e, 0x7b5f);
  CYC(0x7b5f, 0x7b61); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x7b61, 0x7b62); A = mem_rd(gb, HL);
  CYC(0x7b62, 0x7b64); alu_cp(gb, 0x5b); // ENEMYCOLLISION_VERAN_GHOST
  if (!(F & FZ)) { CYCT(0x7b64, 0x7b66); goto damagePillarForm; } // jr nz
  CYC(0x7b64, 0x7b66);
  CYC(0x7b66, 0x7b68); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x7b68, 0x7b6a); mem_wr(gb, HL, 0xf8); // -8
  RET(0x7b6a); return; // ret

damagePillarForm:
  CYC(0x7b6b, 0x7b6d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7b6d, 0x7b6f); mem_wr(gb, HL, 0x08);
  CYC(0x7b6f, 0x7b71); L = ENEMY_BASE + OBJ_VAR33;
  CYC(0x7b71, 0x7b72); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x7b72); return; } // ret nz
  CYC(0x7b72, 0x7b73);
  CYC(0x7b73, 0x7b75); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x7b75, 0x7b77); mem_wr(gb, HL, 0x80);
  CYC(0x7b77, 0x7b79); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7b79, 0x7b7b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x7b7b, 0x7b7d); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x7b7d, 0x7b7f); mem_wr(gb, HL, 0x0e);
  CYC(0x7b7f, 0x7b81); A = 0x01;
  CYC(0x7b81, 0x7b84); mem_wr(gb, wTmpcfc0_genericCutscene_cfd0, A);
  CYC(0x7b84, 0x7b86); A = 0xf0; // SNDCTRL_STOPMUSIC
  CYC(0x7b86, 0x7b89); playSound_b00_hook(gb); return; // jp
}
