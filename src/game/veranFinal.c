#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/ages/enemies/veranFinalForm.s (ENEMY_VERAN_FINAL_FORM), bank $10.

static void veranFinal_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void veranFinal_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

static uint16_t veranFinal_jump_table(GB *gb) {
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

void veranFinal_turtleForm_hook(GB *gb);
void veranFinal_turtleForm_state0_hook(GB *gb);
void veranFinal_turtleForm_state1_hook(GB *gb);
void veranFinal_turtleForm_state2_hook(GB *gb);
void veranFinal_turtleForm_state3_hook(GB *gb);
void veranFinal_turtleForm_state4_hook(GB *gb);
void veranFinal_turtleForm_state5_hook(GB *gb);
void veranFinal_turtleForm_state6_hook(GB *gb);
void veranFinal_turtleForm_state7_hook(GB *gb);
void veranFinal_turtleForm_state8_hook(GB *gb);
void veranFinal_turtleForm_state9_hook(GB *gb);
void veranFinal_turtleForm_stateA_hook(GB *gb);
void veranFinal_spiderForm_hook(GB *gb);
void veranFinal_spiderOrBeeForm_state0_hook(GB *gb);
void veranFinal_spiderForm_state1_hook(GB *gb);
void veranFinal_spiderForm_setCounter2AndInitState2_hook(GB *gb);
void veranFinal_spiderForm_initState2_hook(GB *gb);
void veranFinal_spiderForm_state2_hook(GB *gb);
void veranFinal_spiderForm_updateMovement_hook(GB *gb);
void veranFinal_spiderForm_animate_hook(GB *gb);
void veranFinal_spiderForm_state3_hook(GB *gb);
void veranFinal_spiderForm_state4_hook(GB *gb);
void veranFinal_spiderForm_rushAttack_hook(GB *gb);
void veranFinal_spiderForm_jumpAttack_hook(GB *gb);
void veranFinal_spiderForm_webAttack_hook(GB *gb);
void veranFinal_spiderForm_webAttack_substate0_hook(GB *gb);
void veranFinal_spiderForm_resetCollisionData_hook(GB *gb);
void veranFinal_spiderForm_webAttack_substate1_hook(GB *gb);
void veranFinal_spiderForm_setVulnerableCollisionData_hook(GB *gb);
void veranFinal_spiderForm_webAttack_substate2_hook(GB *gb);
void veranFinal_spiderForm_webAttack_substate3_hook(GB *gb);
void veranFinal_spiderForm_webAttack_updateLinkPosition_hook(GB *gb);
void veranFinal_spiderForm_webAttack_substate4_hook(GB *gb);
void veranFinal_spiderForm_webAttack_applyDamageToLink_hook(GB *gb);
void veranFinal_spiderForm_webAttack_substate5_hook(GB *gb);
void veranFinal_spiderForm_webAttack_substate6_hook(GB *gb);
void veranFinal_spiderForm_webAttack_substate7_hook(GB *gb);
void veranFinal_grabbingLink_hook(GB *gb);
void veranFinal_beeForm_hook(GB *gb);
void veranFinal_beeForm_state1_hook(GB *gb);
void veranFinal_initializeForm_hook(GB *gb);
void veranFinal_beeForm_state2_hook(GB *gb);
void veranFinal_beeForm_animate_hook(GB *gb);
void veranFinal_beeForm_state3_hook(GB *gb);
void veranFinal_beeForm_state4_hook(GB *gb);
void veranFinal_beeForm_state5_hook(GB *gb);
void veranFinal_beeForm_state6_hook(GB *gb);
void veranFinal_beeForm_state7_hook(GB *gb);
void veranFinal_beeForm_state8_hook(GB *gb);
void veranFinal_beeForm_state9_hook(GB *gb);
void veranFinal_beeForm_animate2_hook(GB *gb);
void veranFinal_beeForm_stateA_hook(GB *gb);
void label_10_173_hook(GB *gb);
void veranFinal_beeForm_stateB_hook(GB *gb);
void veranFinal_transformToBeeOrSpider_hook(GB *gb);
void veranFinal_spiderForm_checkLinkWithinDistance_hook(GB *gb);
void veranFinal_spiderForm_decideWhetherToAttack_hook(GB *gb);
void veranFinal_dead_hook(GB *gb);
void veranFinal_spiderForm_decideAngle_hook(GB *gb);
void veranFinal_beeForm_chooseRandomTargetPosition_hook(GB *gb);
void veranFinal_beeForm_nextTargetPosition_hook(GB *gb);
void veranFinal_moveTowardTargetPosition_hook(GB *gb);
void veranFinal_getQuadrant_hook(GB *gb);

void enemyCode02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if ((F & FZ)) { CYCT(0x5827, 0x5829); goto normalStatus; } CYC(0x5827, 0x5829);  // jr z,@normalStatus
  CYC(0x5829, 0x582b); alu_sub(gb, 0x03);  // sub ENEMYSTATUS_NO_HEALTH
  if ((F & FC)) { RET_TAKEN(0x582b); return; } CYC(0x582b, 0x582c);  // ret c
  if ((F & FZ)) { CYCT(0x582c, 0x582e); goto dead; } CYC(0x582c, 0x582e);  // jr z,@dead
  CYC(0x582e, 0x582f); A = alu_dec8(gb, A);  // dec a
  if ((F & FZ)) { CYCT(0x582f, 0x5831); goto justHit; } CYC(0x582f, 0x5831);  // jr z,@justHit
  CYC(0x5831, 0x5833); C = 0x20;  // ld c,$20
  CALL_C(0x5833, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5836);
  CYC(0x5836, 0x5839); if (hook_enabled_at(0x400a)) { ecom_updateKnockback_b10_hook(gb); return; } HANDOFF(0x400a);  // jp ecom_updateKnockback
justHit:
  CYC(0x5839, 0x583a); H = D;  // ld h,d
  CYC(0x583a, 0x583c); L = 0x82;  // ld l,Enemy.subid
  CYC(0x583c, 0x583d); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x583d, 0x583e); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(0x583e, 0x5840); goto notTurtleForm; } CYC(0x583e, 0x5840);  // jr nz,@notTurtleForm
  CYC(0x5840, 0x5842); L = 0xab;  // ld l,Enemy.invincibilityCounter
  CYC(0x5842, 0x5843); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5843, 0x5844); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x5844, 0x5846); goto normalStatus; } CYC(0x5844, 0x5846);  // jr z,@normalStatus
  CYC(0x5846, 0x5848); L = 0xb3;  // ld l,Enemy.var33
  CYC(0x5848, 0x584a); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYCT(0x584a, 0x584d); goto normalStatus;  // jr @normalStatus
notTurtleForm:
  CYC(0x584c, 0x584e); L = 0xad;  // ld l,Enemy.knockbackCounter
  CYC(0x584e, 0x584f); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x584f, 0x5850); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x5850, 0x5852); goto normalStatus; } CYC(0x5850, 0x5852);  // jr z,@normalStatus
  CYC(0x5852, 0x5854); L = 0x84;  // ld l,Enemy.state
  CYC(0x5854, 0x5856); mem_wr(gb, HL, 0x03);  // ld (hl),$03
  CYC(0x5856, 0x5858); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x5858, 0x585a); mem_wr(gb, HL, 105);  // ld (hl),105
  CYC(0x585a, 0x585c); L = 0xa5;  // ld l,Enemy.enemyCollisionMode
  CYC(0x585c, 0x585e); mem_wr(gb, HL, 0x6b);  // ld (hl),ENEMYCOLLISION_VERAN_SPIDER_FORM_VULNERABLE
  CYC(0x585e, 0x5861); A = mem_rd(gb, 0xd004);  // ld a,(w1Link.state)
  CYC(0x5861, 0x5863); alu_cp(gb, 0x0d);  // cp LINK_STATE_GRABBED
  if ((F & FZ)) { CALL_C_CC(0x5863, veranFinal_grabbingLink_hook, 0x5cd3, 0x5866); } else CYC(0x5863, 0x5866);  // call z,veranFinal_grabbingLink
  CYC(0x5866, 0x5868); A = 0x06;  // ld a,$06
  CYC(0x5868, 0x586b); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
dead:
  CALL_C(0x586b, veranFinal_dead_hook, 0x5f4e, 0x586e);
normalStatus:
  CYC(0x586e, 0x5870); E = 0x82;  // ld e,Enemy.subid
  CYC(0x5870, 0x5871); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5871, 0x5873); E = 0x84;  // ld e,Enemy.state
  CYC(0x5873, 0x5874); push_effect(gb, 0x5874);
  switch (veranFinal_jump_table(gb)) {
    case 0x587a: veranFinal_turtleForm_hook(gb); return;
    case 0x5ab4: veranFinal_spiderForm_hook(gb); return;
    case 0x5cdd: veranFinal_beeForm_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void veranFinal_turtleForm_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x587a, 0x587b); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x587b, 0x587c); push_effect(gb, 0x587c);
  switch (veranFinal_jump_table(gb)) {
    case 0x5892: veranFinal_turtleForm_state0_hook(gb); return;
    case 0x58cd: veranFinal_turtleForm_state1_hook(gb); return;
    case 0x5917: veranFinal_turtleForm_state2_hook(gb); return;
    case 0x593c: veranFinal_turtleForm_state3_hook(gb); return;
    case 0x5964: veranFinal_turtleForm_state4_hook(gb); return;
    case 0x598f: veranFinal_turtleForm_state5_hook(gb); return;
    case 0x59dc: veranFinal_turtleForm_state6_hook(gb); return;
    case 0x59f9: veranFinal_turtleForm_state7_hook(gb); return;
    case 0x5a06: veranFinal_turtleForm_state8_hook(gb); return;
    case 0x5a25: veranFinal_turtleForm_state9_hook(gb); return;
    case 0x5a47: veranFinal_turtleForm_stateA_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void veranFinal_turtleForm_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5892, 0x5894); A = 0x02;  // ld a,$02
  CYC(0x5894, 0x5897); mem_wr(gb, 0xcc1d, A);  // ld (wEnemyIDToLoadExtraGfx),a
  CYC(0x5897, 0x5899); A = 0x87;  // ld a,PALH_87
  CALL_C(0x5899, loadPaletteHeader_hook, 0x050b, 0x589c);
  CYC(0x589c, 0x589e); A = 0xf0;  // ld a,SNDCTRL_STOPMUSIC
  CALL_C(0x589e, playSound_b00_hook, 0x0c98, 0x58a1);
  CYC(0x58a1, 0x58a3); A = 0x01;  // ld a,$01
  CYC(0x58a3, 0x58a6); mem_wr(gb, 0xcc8a, A);  // ld (wDisabledObjects),a
  CYC(0x58a6, 0x58a9); mem_wr(gb, 0xcc02, A);  // ld (wMenuDisabled),a
  CYC(0x58a9, 0x58ac); SET_BC(0x0208);  // ld bc,$0208
  CALL_C(0x58ac, enemyBoss_spawnShadow_b10_hook, 0x4534, 0x58af);
  if (!(F & FZ)) { RET_TAKEN(0x58af); return; } CYC(0x58af, 0x58b0);  // ret nz
  CALL_C(0x58b0, ecom_incState_b10_hook, 0x4000, 0x58b3);
  CALL_C(0x58b3, checkIsLinkedGame_hook, 0x1992, 0x58b6);
  CYC(0x58b6, 0x58b8); L = 0xa9;  // ld l,Enemy.health
  CYC(0x58b8, 0x58b9); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x58b9, 0x58bc); SET_BC(0x0c18);  // ld bc,$0c18
  if (!(F & FZ)) { CYCT(0x58bc, 0x58be); goto setForms; } CYC(0x58bc, 0x58be);  // jr nz,+
  CYC(0x58be, 0x58c0); A = 0x14;  // ld a,$14
  CYC(0x58c0, 0x58c1); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x58c1, 0x58c4); SET_BC(0x080f);  // ld bc,$080f
setForms:
  CYC(0x58c4, 0x58c6); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x58c6, 0x58c7); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x58c7, 0x58c8); mem_wr(gb, HL, B);  // ld (hl),b [var31]
  CYC(0x58c8, 0x58c9); L = alu_inc8(gb, L);  // inc l
  CYC(0x58c9, 0x58ca); mem_wr(gb, HL, C);  // ld (hl),c [var32]
  CYC(0x58ca, 0x58cd); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72);  // jp objectSetVisible83
}

void veranFinal_turtleForm_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x58cd, 0x58ce); E = alu_inc8(gb, E);  // inc e
  CYC(0x58ce, 0x58cf); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x58cf, 0x58d0); push_effect(gb, 0x58d0);
  switch (veranFinal_jump_table(gb)) {
    case 0x58d6: goto substate0;
    case 0x58e9: goto substate1;
    case 0x5900: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x58d6, 0x58d9); A = mem_rd(gb, 0xc4ab);  // ld a,(wPaletteThread_mode)
  CYC(0x58d9, 0x58da); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x58da); return; } CYC(0x58da, 0x58db);  // ret nz
  CYC(0x58db, 0x58dd); A = 0xd2;  // ld a,SND_LIGHTNING
  CALL_C(0x58dd, playSound_b00_hook, 0x0c98, 0x58e0);
  CYC(0x58e0, 0x58e3); SET_BC(0x5614);  // ld bc,TX_5614
  CALL_C(0x58e3, showText_hook, 0x1872, 0x58e6);
  CYC(0x58e6, 0x58e9); if (hook_enabled_at(0x4005)) { ecom_incSubstate_b10_hook(gb); return; } HANDOFF(0x4005);  // jp ecom_incSubstate
substate1:
  CYC(0x58e9, 0x58ea); H = D;  // ld h,d
  CYC(0x58ea, 0x58eb); L = E;  // ld l,e
  CYC(0x58eb, 0x58ec); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x58ec, 0x58ed); alu_xor(gb, A);  // xor a
  CYC(0x58ed, 0x58f0); mem_wr(gb, 0xcc8a, A);  // ld (wDisabledObjects),a
  CYC(0x58f0, 0x58f3); mem_wr(gb, 0xcc02, A);  // ld (wMenuDisabled),a
  CYC(0x58f3, 0x58f5); A = 0x03;  // ld a,$03
  CALL_C(0x58f5, enemySetAnimation_hook, 0x282b, 0x58f8);
  CYC(0x58f8, 0x58fa); A = 0x32;  // ld a,MUS_FINAL_BOSS
  CYC(0x58fa, 0x58fd); mem_wr(gb, 0xcc35, A);  // ld (wActiveMusic),a
  CYC(0x58fd, 0x5900); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
substate2:
  CALL_C(0x5900, enemyAnimate_hook, 0x2818, 0x5903);
  CYC(0x5903, 0x5905); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(0x5905, 0x5906); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5906, 0x5907); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { RET_TAKEN(0x5907); return; } CYC(0x5907, 0x5908);  // ret nz
  CALL_C(0x5908, ecom_incState_b10_hook, 0x4000, 0x590b);
  CYC(0x590b, 0x590d); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x590d, 0x590f); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x590f, 0x5911); L = 0x90;  // ld l,Enemy.speed
  CYC(0x5911, 0x5913); mem_wr(gb, HL, 0x46);  // ld (hl),SPEED_1c0
  CYC(0x5913, 0x5914); A = alu_inc8(gb, A);  // inc a
  CYC(0x5914, 0x5917); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void veranFinal_turtleForm_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5917, 0x5919); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(0x5919, 0x591a); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x591a, 0x591b); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(0x591b, 0x591e); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x591b, 0x591e);  // jp nz,enemyAnimate
  CALL_C(0x591e, ecom_decCounter1_b10_hook, 0x439a, 0x5921);
  if (!(F & FZ)) { RET_TAKEN(0x5921); return; } CYC(0x5921, 0x5922);  // ret nz
  CYC(0x5922, 0x5924); L = 0x84;  // ld l,Enemy.state
  CYC(0x5924, 0x5925); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5925, 0x5927); L = 0x94;  // ld l,Enemy.speedZ
  CYC(0x5927, 0x5929); mem_wr(gb, HL, 0x00);  // ld (hl),<(-$400)
  CYC(0x5929, 0x592a); L = alu_inc8(gb, L);  // inc l
  CYC(0x592a, 0x592c); mem_wr(gb, HL, 0xfc);  // ld (hl),>(-$400)
  CALL_C(0x592c, ecom_updateAngleTowardTarget_b10_hook, 0x43bf, 0x592f);
  CALL_C(0x592f, objectSetVisible81_hook, 0x1e60, 0x5932);
  CYC(0x5932, 0x5934); A = 0x69;  // ld a,SND_UNKNOWN4
  CALL_C(0x5934, playSound_b00_hook, 0x0c98, 0x5937);
  CYC(0x5937, 0x5939); A = 0x02;  // ld a,$02
  CYC(0x5939, 0x593c); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void veranFinal_turtleForm_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x593c, 0x593e); C = 0x20;  // ld c,$20
  CALL_C(0x593e, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5941);
  CYC(0x5941, 0x5942); A = mem_rd(gb, HL); SET_HL(HL - 1);  // ldd a,(hl)
  CYC(0x5942, 0x5943); alu_or(gb, mem_rd(gb, HL));  // or (hl)
  if (!(F & FZ)) { CYCT(0x5943, 0x5946); if (hook_enabled_at(0x414c)) { ecom_applyVelocityForTopDownEnemyNoHoles_b10_hook(gb); return; } HANDOFF(0x414c); } CYC(0x5943, 0x5946);  // jp nz,ecom_applyVelocityForTopDownEnemyNoHoles
  CYC(0x5946, 0x5947); L = alu_inc8(gb, L);  // inc l
  CYC(0x5947, 0x5948); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [speedZ]=$0100
  CYC(0x5948, 0x594a); L = 0x84;  // ld l,Enemy.state
  CYC(0x594a, 0x594b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x594b, 0x594d); L = 0x90;  // ld l,Enemy.speed
  CYC(0x594d, 0x594f); mem_wr(gb, HL, 0x78);  // ld (hl),SPEED_300
  CYC(0x594f, 0x5951); L = 0xb6;  // ld l,Enemy.var36
  CYC(0x5951, 0x5953); A = mem_rd(gb, 0xffb0);  // ldh a,(<hEnemyTargetY)
  CYC(0x5953, 0x5955); alu_and(gb, 0xf0);  // and $f0
  CYC(0x5955, 0x5957); alu_add(gb, 0x08);  // add $08
  CYC(0x5957, 0x5958); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5958, 0x595a); A = mem_rd(gb, 0xffb1);  // ldh a,(<hEnemyTargetX)
  CYC(0x595a, 0x595c); alu_and(gb, 0xf0);  // and $f0
  CYC(0x595c, 0x595e); alu_add(gb, 0x08);  // add $08
  CYC(0x595e, 0x595f); mem_wr(gb, HL, A);  // ld (hl),a [var37]
  CYC(0x595f, 0x5961); A = 0x01;  // ld a,$01
  CYC(0x5961, 0x5964); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void veranFinal_turtleForm_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5964, 0x5966); C = 0x10;  // ld c,$10
  CALL_C(0x5966, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5969);
  if ((F & FZ)) { CYCT(0x5969, 0x596b); goto nextState; } CYC(0x5969, 0x596b);  // jr z,@nextState
  CALL_C(0x596b, veranFinal_moveTowardTargetPosition_hook, 0x601c, 0x596e);
  if (!(F & FC)) { RET_TAKEN(0x596e); return; } CYC(0x596e, 0x596f);  // ret nc
  CYC(0x596f, 0x5971); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x5971, 0x5972); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(0x5972, 0x5974); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5974, 0x5975); mem_wr(gb, HL, C);  // ld (hl),c
  RET(0x5975); return;  // ret
nextState:
  CYC(0x5976, 0x5978); A = 0x10;  // ld a,$10
  CALL_C(0x5978, setScreenShakeCounter_hook, 0x24bb, 0x597b);
  CALL_C(0x597b, ecom_incState_b10_hook, 0x4000, 0x597e);
  CYC(0x597e, 0x5980); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x5980, 0x5982); mem_wr(gb, HL, 0x0c);  // ld (hl),$0c
  CALL_C(0x5982, objectSetVisible83_hook, 0x1e72, 0x5985);
  CYC(0x5985, 0x5987); A = 0x98;  // ld a,$98
  CALL_C(0x5987, playSound_b00_hook, 0x0c98, 0x598a);
  CYC(0x598a, 0x598c); B = 0x57;  // ld b,PART_VERAN_ACID_POOL
  CYC(0x598c, 0x598f); if (hook_enabled_at(0x437c)) { ecom_spawnProjectile_b10_hook(gb); return; } HANDOFF(0x437c);  // jp ecom_spawnProjectile
}

void veranFinal_turtleForm_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x598f, ecom_decCounter1_b10_hook, 0x439a, 0x5992);
  if (!(F & FZ)) { RET_TAKEN(0x5992); return; } CYC(0x5992, 0x5993);  // ret nz
  CYC(0x5993, 0x5995); L = 0x90;  // ld l,Enemy.speed
  CYC(0x5995, 0x5997); mem_wr(gb, HL, 0x46);  // ld (hl),SPEED_1c0
  CYC(0x5997, 0x5999); L = 0xb3;  // ld l,Enemy.var33
  CYC(0x5999, 0x599b); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  CYC(0x599b, 0x599d); L = 0xb4;  // ld l,Enemy.var34
  if ((F & FZ)) { CYCT(0x599d, 0x599f); goto haveVar34; } CYC(0x599d, 0x599f);  // jr z,+
  CYC(0x599f, 0x59a0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
haveVar34:
  CYC(0x59a0, 0x59a1); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x59a1, 0x59a4); SET_BC(0x59d7);  // ld bc,@transformProbabilities
  CALL_C(0x59a4, addAToBc_hook, 0x006d, 0x59a7);
  CYC(0x59a7, 0x59a8); A = mem_rd(gb, BC);  // ld a,(bc)
  CYC(0x59a8, 0x59a9); B = A;  // ld b,a
  CYC(0x59a9, 0x59aa); A = alu_inc8(gb, A);  // inc a
  CYC(0x59aa, 0x59ab); L = E;  // ld l,e
  if ((F & FZ)) { CYCT(0x59ab, 0x59ad); goto tryOpenFace; } CYC(0x59ab, 0x59ad);  // jr z,++
  CALL_C(0x59ad, getRandomNumber_hook, 0x043e, 0x59b0);
  CYC(0x59b0, 0x59b1); alu_and(gb, B);  // and b
  if ((F & FZ)) { CYCT(0x59b1, 0x59b4); veranFinal_transformToBeeOrSpider_hook(gb); return; } CYC(0x59b1, 0x59b4);  // jp z,veranFinal_transformToBeeOrSpider
  CYC(0x59b4, 0x59b6); E = 0xb3;  // ld e,Enemy.var33
  CYC(0x59b6, 0x59b7); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x59b7, 0x59b8); alu_rrca(gb);  // rrca
  if ((F & FC)) { CYCT(0x59b8, 0x59ba); goto jumpAgain; } CYC(0x59b8, 0x59ba);  // jr c,@jumpAgain
tryOpenFace:
  CALL_C(0x59ba, getRandomNumber_hook, 0x043e, 0x59bd);
  CYC(0x59bd, 0x59bf); alu_cp(gb, 90);  // cp 90
  if (!(F & FC)) { CYCT(0x59bf, 0x59c1); goto jumpAgain; } CYC(0x59bf, 0x59c1);  // jr nc,@jumpAgain
  CYC(0x59c1, 0x59c2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [substate]=6
  CYC(0x59c2, 0x59c4); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x59c4, 0x59c6); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x59c6, 0x59c8); A = 0xc7;  // ld a,SND_GORON
  CALL_C(0x59c8, playSound_b00_hook, 0x0c98, 0x59cb);
  CYC(0x59cb, 0x59cd); A = 0x04;  // ld a,$04
  CYC(0x59cd, 0x59d0); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
jumpAgain:
  CYC(0x59d0, 0x59d2); mem_wr(gb, HL, 0x02);  // ld (hl),$02 [state]
  CYC(0x59d2, 0x59d4); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x59d4, 0x59d6); mem_wr(gb, HL, 30);  // ld (hl),30
  RET(0x59d6); return;  // ret
}

void veranFinal_turtleForm_state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x59dc, enemyAnimate_hook, 0x2818, 0x59df);
  CYC(0x59df, 0x59e0); H = D;  // ld h,d
  CYC(0x59e0, 0x59e2); L = 0xa1;  // ld l,Enemy.animParameter
  CYC(0x59e2, 0x59e4); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if (!(F & FZ)) { CYCT(0x59e4, 0x59e6); goto nextState; } CYC(0x59e4, 0x59e6);  // jr nz,@nextState
  CYC(0x59e6, 0x59e8); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  if ((F & FZ)) { RET_TAKEN(0x59e8); return; } CYC(0x59e8, 0x59e9);  // ret z
  CYC(0x59e9, 0x59eb); L = 0xa5;  // ld l,Enemy.enemyCollisionMode
  CYC(0x59eb, 0x59ed); mem_wr(gb, HL, 0x09);  // ld (hl),ENEMYCOLLISION_VERAN_TURTLE_FORM_VULNERABLE
  RET(0x59ed); return;  // ret
nextState:
  CYC(0x59ee, 0x59f0); L = 0x84;  // ld l,Enemy.state
  CYC(0x59f0, 0x59f1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x59f1, 0x59f3); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x59f3, 0x59f5); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(0x59f5, 0x59f6); alu_xor(gb, A);  // xor a
  CYC(0x59f6, 0x59f9); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void veranFinal_turtleForm_state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x59f9, ecom_decCounter1_b10_hook, 0x439a, 0x59fc);
  if (!(F & FZ)) { CYCT(0x59fc, 0x59ff); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x59fc, 0x59ff);  // jp nz,enemyAnimate
  CYC(0x59ff, 0x5a00); L = E;  // ld l,e
  CYC(0x5a00, 0x5a01); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5a01, 0x5a03); A = 0x03;  // ld a,$03
  CYC(0x5a03, 0x5a06); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void veranFinal_turtleForm_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a06, enemyAnimate_hook, 0x2818, 0x5a09);
  CYC(0x5a09, 0x5a0a); H = D;  // ld h,d
  CYC(0x5a0a, 0x5a0c); L = 0xa1;  // ld l,Enemy.animParameter
  CYC(0x5a0c, 0x5a0e); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if (!(F & FZ)) { CYCT(0x5a0e, 0x5a10); goto nextState; } CYC(0x5a0e, 0x5a10);  // jr nz,@nextState
  CYC(0x5a10, 0x5a12); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  if ((F & FZ)) { RET_TAKEN(0x5a12); return; } CYC(0x5a12, 0x5a13);  // ret z
  CYC(0x5a13, 0x5a15); L = 0xa5;  // ld l,Enemy.enemyCollisionMode
  CYC(0x5a15, 0x5a17); mem_wr(gb, HL, 0x6a);  // ld (hl),ENEMYCOLLISION_VERAN_TURTLE_FORM
  RET(0x5a17); return;  // ret
nextState:
  CYC(0x5a18, 0x5a1a); L = 0x84;  // ld l,Enemy.state
  CYC(0x5a1a, 0x5a1c); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(0x5a1c, 0x5a1e); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x5a1e, 0x5a20); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x5a20, 0x5a22); A = 0x01;  // ld a,$01
  CYC(0x5a22, 0x5a25); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void veranFinal_turtleForm_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5a25, enemyAnimate_hook, 0x2818, 0x5a28);
  CYC(0x5a28, 0x5a2a); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(0x5a2a, 0x5a2b); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5a2b, 0x5a2c); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { RET_TAKEN(0x5a2c); return; } CYC(0x5a2c, 0x5a2d);  // ret nz
  CYC(0x5a2d, 0x5a2e); H = D;  // ld h,d
  CYC(0x5a2e, 0x5a30); L = 0xb3;  // ld l,Enemy.var33
  CYC(0x5a30, 0x5a31); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a [var33]=0
  CYC(0x5a31, 0x5a32); mem_wr(gb, HL, A);  // ld (hl),a [var34]=0
  CYC(0x5a32, 0x5a34); L = 0x84;  // ld l,Enemy.state
  CYC(0x5a34, 0x5a35); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(0x5a35, 0x5a37); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5a37, 0x5a39); mem_wr(gb, HL, 0x82);  // ld (hl),$80|ENEMY_VERAN_FINAL_FORM
  CYC(0x5a39, 0x5a3a); L = alu_inc8(gb, L);  // inc l
  CYC(0x5a3a, 0x5a3c); mem_wr(gb, HL, 0x09);  // ld (hl),ENEMYCOLLISION_VERAN_TURTLE_FORM_VULNERABLE
  CYC(0x5a3c, 0x5a3e); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x5a3e, 0x5a40); A = 0x06;  // ld a,$06
  CYC(0x5a40, 0x5a41); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5a41, 0x5a42); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5a42, 0x5a44); A = 0x03;  // ld a,$03
  CYC(0x5a44, 0x5a47); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void veranFinal_turtleForm_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5a47, 0x5a48); E = alu_inc8(gb, E);  // inc e
  CYC(0x5a48, 0x5a49); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x5a49, 0x5a4a); push_effect(gb, 0x5a4a);
  switch (veranFinal_jump_table(gb)) {
    case 0x5a52: goto substate0;
    case 0x5a7a: goto substate1;
    case 0x5a91: goto substate2;
    case 0x5a9f: goto substate3;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x5a52, 0x5a54); E = 0xab;  // ld e,Enemy.invincibilityCounter
  CYC(0x5a54, 0x5a55); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5a55, 0x5a56); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x5a56); return; } CYC(0x5a56, 0x5a57);  // ret nz
  CALL_C(0x5a57, checkLinkVulnerable_hook, 0x1d28, 0x5a5a);
  if (!(F & FC)) { RET_TAKEN(0x5a5a); return; } CYC(0x5a5a, 0x5a5b);  // ret nc
  CYC(0x5a5b, 0x5a5d); A = 0x01;  // ld a,$01
  CYC(0x5a5d, 0x5a60); mem_wr(gb, 0xcc02, A);  // ld (wMenuDisabled),a
  CYC(0x5a60, 0x5a63); mem_wr(gb, 0xcc8a, A);  // ld (wDisabledObjects),a
  CALL_C(0x5a63, dropLinkHeldItem_hook, 0x2c43, 0x5a66);
  CALL_C(0x5a66, clearAllParentItems_hook, 0x2c10, 0x5a69);
  CALL_C(0x5a69, ecom_incSubstate_b10_hook, 0x4005, 0x5a6c);
  CALL_C(0x5a6c, checkIsLinkedGame_hook, 0x1992, 0x5a6f);
  CYC(0x5a6f, 0x5a72); SET_BC(0x5615);  // ld bc,TX_5615
  if ((F & FZ)) { CYCT(0x5a72, 0x5a74); goto showText; } CYC(0x5a72, 0x5a74);  // jr z,+
  CYC(0x5a74, 0x5a77); SET_BC(0x5616);  // ld bc,TX_5616
showText:
  CYC(0x5a77, 0x5a7a); if (hook_enabled_at(0x1872)) { showText_hook(gb); return; } HANDOFF(0x1872);  // jp showText
substate1:
  CYC(0x5a7a, 0x5a7d); A = mem_rd(gb, 0xcba0);  // ld a,(wTextIsActive)
  CYC(0x5a7d, 0x5a7e); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x5a7e); return; } CYC(0x5a7e, 0x5a7f);  // ret nz
  CALL_C(0x5a7f, ecom_incSubstate_b10_hook, 0x4005, 0x5a82);
  CYC(0x5a82, 0x5a84); L = 0x87;  // ld l,Enemy.counter2
  CYC(0x5a84, 0x5a86); mem_wr(gb, HL, 40);  // ld (hl),40
  CYC(0x5a86, 0x5a88); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x5a88, 0x5a89); B = mem_rd(gb, HL);  // ld b,(hl)
  CYC(0x5a89, 0x5a8b); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5a8b, 0x5a8c); C = mem_rd(gb, HL);  // ld c,(hl)
  CYC(0x5a8c, 0x5a8e); A = 0xff;  // ld a,$ff
  CYC(0x5a8e, 0x5a91); if (hook_enabled_at(0x29ed)) { createEnergySwirlGoingOut_hook(gb); return; } HANDOFF(0x29ed);  // jp createEnergySwirlGoingOut
substate2:
  CALL_C(0x5a91, ecom_decCounter2_b10_hook, 0x43a3, 0x5a94);
  if (!(F & FZ)) { RET_TAKEN(0x5a94); return; } CYC(0x5a94, 0x5a95);  // ret nz
  CYC(0x5a95, 0x5a98); SET_BC(0x9021);  // ldbc INTERAC_MISC_PUZZLES,$21
  CALL_C(0x5a98, objectCreateInteraction_hook, 0x24c5, 0x5a9b);
  if (!(F & FZ)) { RET_TAKEN(0x5a9b); return; } CYC(0x5a9b, 0x5a9c);  // ret nz
  CYC(0x5a9c, 0x5a9f); if (hook_enabled_at(0x4005)) { ecom_incSubstate_b10_hook(gb); return; } HANDOFF(0x4005);  // jp ecom_incSubstate
substate3:
  CYC(0x5a9f, 0x5aa2); A = mem_rd(gb, 0xc4ab);  // ld a,(wPaletteThread_mode)
  CYC(0x5aa2, 0x5aa3); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x5aa3); return; } CYC(0x5aa3, 0x5aa4);  // ret nz
  CYC(0x5aa4, 0x5aa7); SET_HL(0xc9fc);  // ld hl,wGroup4RoomFlags+(<ROOM_AGES_4fc)
  CYC(0x5aa7, 0x5aa9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x5aa9, 0x5aab); A = 0x09;  // ld a,CUTSCENE_BLACK_TOWER_ESCAPE
  CYC(0x5aab, 0x5aae); mem_wr(gb, 0xcc04, A);  // ld (wCutsceneTrigger),a
  CALL_C(0x5aae, incMakuTreeState_hook, 0x3e53, 0x5ab1);
  CYC(0x5ab1, 0x5ab4); if (hook_enabled_at(0x2e47)) { enemyDelete_hook(gb); return; } HANDOFF(0x2e47);  // jp enemyDelete
}

void veranFinal_spiderForm_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ab4, 0x5ab5); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5ab5, 0x5ab6); push_effect(gb, 0x5ab6);
  switch (veranFinal_jump_table(gb)) {
    case 0x5ac0: veranFinal_spiderOrBeeForm_state0_hook(gb); return;
    case 0x5ac1: veranFinal_spiderForm_state1_hook(gb); return;
    case 0x5afc: veranFinal_spiderForm_state2_hook(gb); return;
    case 0x5b15: veranFinal_spiderForm_state3_hook(gb); return;
    case 0x5b39: veranFinal_spiderForm_state4_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void veranFinal_spiderOrBeeForm_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(0x5ac0); return;  // ret
}

void veranFinal_spiderForm_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5ac1, enemyAnimate_hook, 0x2818, 0x5ac4);
  CYC(0x5ac4, 0x5ac6); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(0x5ac6, 0x5ac7); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5ac7, 0x5ac8); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { RET_TAKEN(0x5ac8); return; } CYC(0x5ac8, 0x5ac9);  // ret nz
  CYC(0x5ac9, 0x5acc); SET_BC(0x1010);  // ld bc,$1010
  CYC(0x5acc, 0x5ace); E = 0x6c;  // ld e,ENEMYCOLLISION_VERAN_SPIDER_FORM
  CYC(0x5ace, 0x5ad0); L = 0xb1;  // ld l,Enemy.var31
  CALL_C(0x5ad0, veranFinal_initializeForm_hook, 0x5d12, 0x5ad3);
  CYC(0x5ad3, 0x5ad5); A = 0x05;  // ld a,$05
  CALL_C(0x5ad5, enemySetAnimation_hook, 0x282b, 0x5ad8);
  veranFinal_spiderForm_setCounter2AndInitState2_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_setCounter2AndInitState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5ad8, 0x5ada); E = 0x87;  // ld e,Enemy.counter2
  CYC(0x5ada, 0x5adc); A = 120;  // ld a,120
  CYC(0x5adc, 0x5add); mem_wr(gb, DE, A);  // ld (de),a
  veranFinal_spiderForm_initState2_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_initState2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5add, 0x5ade); H = D;  // ld h,d
  CYC(0x5ade, 0x5ae0); L = 0x84;  // ld l,Enemy.state
  CYC(0x5ae0, 0x5ae2); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(0x5ae2, 0x5ae4); L = 0x90;  // ld l,Enemy.speed
  CYC(0x5ae4, 0x5ae6); mem_wr(gb, HL, 30);  // ld (hl),SPEED_c0
  CALL_C(0x5ae6, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5ae9);
  CYC(0x5ae9, 0x5aeb); alu_and(gb, 0x03);  // and $03
  CYC(0x5aeb, 0x5aee); SET_HL(0x5af8);  // ld hl,@counter1Vals
  CYC(0x5aee, 0x5aef);
  veranFinal_addAToHl_from_rst(gb, 0x5aef);  // rst_addAToHl
  CYC(0x5aef, 0x5af1); E = 0x86;  // ld e,Enemy.counter1
  CYC(0x5af1, 0x5af2); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5af2, 0x5af3); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(0x5af3, veranFinal_spiderForm_decideAngle_hook, 0x5f8f, 0x5af6);
  CYC(0x5af6, 0x5af8); veranFinal_spiderForm_animate_hook(gb); return;  // jr veranFinal_spiderForm_animate
}

void veranFinal_spiderForm_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5afc, ecom_decCounter2_b10_hook, 0x43a3, 0x5aff);
  if (!(F & FZ)) { CYCT(0x5aff, 0x5b01); goto decCounter1; } CYC(0x5aff, 0x5b01);  // jr nz,+
  CYC(0x5b01, 0x5b03); mem_wr(gb, HL, 120);  // ld (hl),120
  CALL_C(0x5b03, veranFinal_spiderForm_decideWhetherToAttack_hook, 0x5f11, 0x5b06);
  if ((F & FC)) { RET_TAKEN(0x5b06); return; } CYC(0x5b06, 0x5b07);  // ret c
decCounter1:
  CALL_C(0x5b07, ecom_decCounter1_b10_hook, 0x439a, 0x5b0a);
  if ((F & FZ)) { CYCT(0x5b0a, 0x5b0c); veranFinal_spiderForm_initState2_hook(gb); return; } CYC(0x5b0a, 0x5b0c);  // jr z,veranFinal_spiderForm_initState2
  veranFinal_spiderForm_updateMovement_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_updateMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5b0c, ecom_bounceOffWallsAndHoles_b10_hook, 0x42de, 0x5b0f);
  CALL_C(0x5b0f, objectApplySpeed_hook, 0x201d, 0x5b12);
  veranFinal_spiderForm_animate_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_animate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b12, 0x5b15); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818);  // jp enemyAnimate
}

void veranFinal_spiderForm_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b15, 0x5b17); E = 0x8f;  // ld e,Enemy.zh
  CYC(0x5b17, 0x5b18); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5b18, 0x5b19); alu_rlca(gb);  // rlca
  CYC(0x5b19, 0x5b1b); C = 0x20;  // ld c,$20
  if ((F & FC)) { CYCT(0x5b1b, 0x5b1e); if (hook_enabled_at(0x1f46)) { objectUpdateSpeedZ_paramC_hook(gb); return; } HANDOFF(0x1f46); } CYC(0x5b1b, 0x5b1e);  // jp c,objectUpdateSpeedZ_paramC
  CALL_C(0x5b1e, ecom_decCounter1_b10_hook, 0x439a, 0x5b21);
  if ((F & FZ)) { CYCT(0x5b21, 0x5b23); goto gotoState2; } CYC(0x5b21, 0x5b23);  // jr z,+
  CYC(0x5b23, 0x5b24); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5b24, 0x5b25); alu_rrca(gb);  // rrca
  if ((F & FC)) { RET_TAKEN(0x5b25); return; } CYC(0x5b25, 0x5b26);  // ret c
  CYC(0x5b26, 0x5b28); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x5b28, 0x5b29); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5b29, 0x5b2b); alu_xor(gb, 0x02);  // xor $02
  CYC(0x5b2b, 0x5b2c); mem_wr(gb, HL, A);  // ld (hl),a
  RET(0x5b2c); return;  // ret
gotoState2:
  CYC(0x5b2d, 0x5b2f); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x5b2f, 0x5b31); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CALL_C(0x5b31, objectSetVisible83_hook, 0x1e72, 0x5b34);
  CALL_C(0x5b34, veranFinal_spiderForm_resetCollisionData_hook, 0x5c16, 0x5b37);
  CYC(0x5b37, 0x5b3a); veranFinal_spiderForm_initState2_hook(gb); return;  // jr veranFinal_spiderForm_initState2
}

void veranFinal_spiderForm_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b39, 0x5b3b); E = 0x83;  // ld e,Enemy.var03
  CYC(0x5b3b, 0x5b3c); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5b3c, 0x5b3e); E = 0x85;  // ld e,Enemy.substate
  CYC(0x5b3e, 0x5b3f); push_effect(gb, 0x5b3f);
  switch (veranFinal_jump_table(gb)) {
    case 0x5b45: veranFinal_spiderForm_rushAttack_hook(gb); return;
    case 0x5b68: veranFinal_spiderForm_jumpAttack_hook(gb); return;
    case 0x5bfa: veranFinal_spiderForm_webAttack_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void veranFinal_spiderForm_rushAttack_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b45, 0x5b46); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5b46, 0x5b47); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x5b47, 0x5b49); goto substate0; } CYC(0x5b47, 0x5b49);  // jr z,@substate0
substate1:
  CALL_C(0x5b49, ecom_decCounter1_b10_hook, 0x439a, 0x5b4c);
  if ((F & FZ)) { CYCT(0x5b4c, 0x5b4e); veranFinal_spiderForm_setCounter2AndInitState2_hook(gb); return; } CYC(0x5b4c, 0x5b4e);  // jr z,veranFinal_spiderForm_setCounter2AndInitState2
  CALL_C(0x5b4e, veranFinal_spiderForm_updateMovement_hook, 0x5b0c, 0x5b51);
  CYC(0x5b51, 0x5b54); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818);  // jp enemyAnimate
substate0:
  CALL_C(0x5b54, ecom_incSubstate_b10_hook, 0x4005, 0x5b57);
  CYC(0x5b57, 0x5b58); L = alu_inc8(gb, L);  // inc l
  CYC(0x5b58, 0x5b5a); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x5b5a, 0x5b5c); L = 0x90;  // ld l,Enemy.speed
  CYC(0x5b5c, 0x5b5e); mem_wr(gb, HL, 60);  // ld (hl),SPEED_180
  CALL_C(0x5b5e, ecom_updateAngleTowardTarget_b10_hook, 0x43bf, 0x5b61);
  CYC(0x5b61, 0x5b63); alu_and(gb, 0x18);  // and $18
  CYC(0x5b63, 0x5b65); alu_add(gb, 0x04);  // add $04
  CYC(0x5b65, 0x5b66); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5b66, 0x5b69); veranFinal_spiderForm_animate_hook(gb); return;  // jr veranFinal_spiderForm_animate
}

void veranFinal_spiderForm_jumpAttack_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5b68, 0x5b69); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x5b69, 0x5b6a); push_effect(gb, 0x5b6a);
  switch (veranFinal_jump_table(gb)) {
    case 0x5b74: goto substate0;
    case 0x5b87: goto substate1;
    case 0x5bb6: goto substate2;
    case 0x5bda: goto substate3;
    case 0x5bf3: goto substate4;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x5b74, 0x5b76); B = 0x56;  // ld b,$56
  CALL_C(0x5b76, ecom_spawnProjectile_b10_hook, 0x437c, 0x5b79);
  if (!(F & FZ)) { RET_TAKEN(0x5b79); return; } CYC(0x5b79, 0x5b7a);  // ret nz
  CALL_C(0x5b7a, ecom_incSubstate_b10_hook, 0x4005, 0x5b7d);
  CYC(0x5b7d, 0x5b7f); L = 0xb8;  // ld l,Enemy.var38
  CYC(0x5b7f, 0x5b81); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CALL_C(0x5b81, veranFinal_spiderForm_setVulnerableCollisionData_hook, 0x5c2d, 0x5b84);
  CYC(0x5b84, 0x5b87); if (hook_enabled_at(0x1e60)) { objectSetVisible81_hook(gb); return; } HANDOFF(0x1e60);  // jp objectSetVisible81
substate1:
  CYC(0x5b87, 0x5b89); E = 0xb8;  // ld e,Enemy.var38
  CYC(0x5b89, 0x5b8a); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5b8a, 0x5b8b); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(0x5b8b); return; } CYC(0x5b8b, 0x5b8c);  // ret z
  CYC(0x5b8c, 0x5b8d); H = D;  // ld h,d
  CYC(0x5b8d, 0x5b8f); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x5b8f, 0x5b90); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5b90, 0x5b92); alu_sub(gb, 0x03);  // sub $03
  CYC(0x5b92, 0x5b93); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5b93, 0x5b95); alu_bit(gb, 7, A);  // bit 7,a
  if ((F & FZ)) { CYCT(0x5b95, 0x5b97); goto zh; } CYC(0x5b95, 0x5b97);  // jr z,+
  CYC(0x5b97, 0x5b99); alu_cp(gb, 0xe0);  // cp $e0
  if (!(F & FC)) { RET_TAKEN(0x5b99); return; } CYC(0x5b99, 0x5b9a);  // ret nc
  CYC(0x5b9a, 0x5b9c); A = mem_rd(gb, 0xffaa);  // ldh a,(<hCameraY)
  CYC(0x5b9c, 0x5b9d); B = A;  // ld b,a
  CYC(0x5b9d, 0x5b9e); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5b9e, 0x5ba0); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x5ba0, 0x5ba1); alu_add(gb, mem_rd(gb, HL));  // add (hl)
  CYC(0x5ba1, 0x5ba2); alu_sub(gb, B);  // sub b
  CYC(0x5ba2, 0x5ba4); alu_cp(gb, 0xb0);  // cp $b0
  if ((F & FC)) { RET_TAKEN(0x5ba4); return; } CYC(0x5ba4, 0x5ba5);  // ret c
zh:
  CYC(0x5ba5, 0x5ba7); L = 0x85;  // ld l,Enemy.substate
  CYC(0x5ba7, 0x5ba8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5ba8, 0x5ba9); L = alu_inc8(gb, L);  // inc l
  CYC(0x5ba9, 0x5bab); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(0x5bab, 0x5bad); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5bad, 0x5baf); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(0x5baf, 0x5bb1); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x5bb1, 0x5bb3); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x5bb3, 0x5bb6); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);  // jp objectSetInvisible
substate2:
  CALL_C(0x5bb6, ecom_decCounter1_b10_hook, 0x439a, 0x5bb9);
  if (!(F & FZ)) { RET_TAKEN(0x5bb9); return; } CYC(0x5bb9, 0x5bba);  // ret nz
  CYC(0x5bba, 0x5bbb); L = E;  // ld l,e
  CYC(0x5bbb, 0x5bbc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5bbc, 0x5bbe); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5bbe, 0x5bc0); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x5bc0, 0x5bc2); L = 0x94;  // ld l,Enemy.speedZ
  CYC(0x5bc2, 0x5bc3); alu_xor(gb, A);  // xor a
  CYC(0x5bc3, 0x5bc4); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5bc4, 0x5bc6); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(0x5bc6, 0x5bc8); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x5bc8, 0x5bca); A = mem_rd(gb, 0xffb0);  // ldh a,(<hEnemyTargetY)
  CYC(0x5bca, 0x5bcb); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5bcb, 0x5bcc); L = alu_inc8(gb, L);  // inc l
  CYC(0x5bcc, 0x5bce); A = mem_rd(gb, 0xffb1);  // ldh a,(<hEnemyTargetX)
  CYC(0x5bce, 0x5bcf); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5bcf, 0x5bd1); C = 0x08;  // ld c,$08
  CALL_C(0x5bd1, ecom_setZAboveScreen_b10_hook, 0x4446, 0x5bd4);
  CALL_C(0x5bd4, veranFinal_spiderForm_resetCollisionData_hook, 0x5c16, 0x5bd7);
  CYC(0x5bd7, 0x5bda); if (hook_enabled_at(0x1e60)) { objectSetVisible81_hook(gb); return; } HANDOFF(0x1e60);  // jp objectSetVisible81
substate3:
  CYC(0x5bda, 0x5bdc); C = 0x20;  // ld c,$20
  CALL_C(0x5bdc, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5bdf);
  if (!(F & FZ)) { RET_TAKEN(0x5bdf); return; } CYC(0x5bdf, 0x5be0);  // ret nz
  CYC(0x5be0, 0x5be2); L = 0x85;  // ld l,Enemy.substate
  CYC(0x5be2, 0x5be3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5be3, 0x5be4); L = alu_inc8(gb, L);  // inc l
  CYC(0x5be4, 0x5be6); mem_wr(gb, HL, 120);  // ld (hl),120
  CYC(0x5be6, 0x5be8); A = 0x81;  // ld a,SND_STRONG_POUND
  CALL_C(0x5be8, playSound_b00_hook, 0x0c98, 0x5beb);
  CYC(0x5beb, 0x5bed); A = 90;  // ld a,90
  CALL_C(0x5bed, setScreenShakeCounter_hook, 0x24bb, 0x5bf0);
  CYC(0x5bf0, 0x5bf3); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72);  // jp objectSetVisible83
substate4:
  CALL_C(0x5bf3, ecom_decCounter1_b10_hook, 0x439a, 0x5bf6);
  if (!(F & FZ)) { RET_TAKEN(0x5bf6); return; } CYC(0x5bf6, 0x5bf7);  // ret nz
  CYC(0x5bf7, 0x5bfa); veranFinal_spiderForm_setCounter2AndInitState2_hook(gb); return;  // jp veranFinal_spiderForm_setCounter2AndInitState2
}

void veranFinal_spiderForm_webAttack_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5bfa, 0x5bfb); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(0x5bfb, 0x5bfc); push_effect(gb, 0x5bfc);
  switch (veranFinal_jump_table(gb)) {
    case 0x5c0c: veranFinal_spiderForm_webAttack_substate0_hook(gb); return;
    case 0x5c24: veranFinal_spiderForm_webAttack_substate1_hook(gb); return;
    case 0x5c3b: veranFinal_spiderForm_webAttack_substate2_hook(gb); return;
    case 0x5c50: veranFinal_spiderForm_webAttack_substate3_hook(gb); return;
    case 0x5c76: veranFinal_spiderForm_webAttack_substate4_hook(gb); return;
    case 0x5c9b: veranFinal_spiderForm_webAttack_substate5_hook(gb); return;
    case 0x5cac: veranFinal_spiderForm_webAttack_substate6_hook(gb); return;
    case 0x5cc8: veranFinal_spiderForm_webAttack_substate7_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void veranFinal_spiderForm_webAttack_substate0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c0c, 0x5c0d); H = D;  // ld h,d
  CYC(0x5c0d, 0x5c0e); L = E;  // ld l,e
  CYC(0x5c0e, 0x5c0f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5c0f, 0x5c10); L = alu_inc8(gb, L);  // inc l
  CYC(0x5c10, 0x5c12); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x5c12, 0x5c14); L = 0xb8;  // ld l,Enemy.var38
  CYC(0x5c14, 0x5c16); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  veranFinal_spiderForm_resetCollisionData_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_resetCollisionData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c16, 0x5c17); H = D;  // ld h,d
  CYC(0x5c17, 0x5c19); L = 0xa5;  // ld l,Enemy.enemyCollisionMode
  CYC(0x5c19, 0x5c1b); mem_wr(gb, HL, 0x6c);  // ld (hl),ENEMYCOLLISION_VERAN_SPIDER_FORM
  CYC(0x5c1b, 0x5c1d); L = 0xa6;  // ld l,Enemy.collisionRadiusY
  CYC(0x5c1d, 0x5c1f); mem_wr(gb, HL, 0x10);  // ld (hl),$10
  CYC(0x5c1f, 0x5c21); A = 0x05;  // ld a,$05
  CYC(0x5c21, 0x5c24); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void veranFinal_spiderForm_webAttack_substate1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c24, ecom_decCounter1_b10_hook, 0x439a, 0x5c27);
  if (!(F & FZ)) { RET_TAKEN(0x5c27); return; } CYC(0x5c27, 0x5c28);  // ret nz
  CYC(0x5c28, 0x5c29); L = alu_inc8(gb, L);  // inc l
  CYC(0x5c29, 0x5c2b); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x5c2b, 0x5c2c); L = E;  // ld l,e
  CYC(0x5c2c, 0x5c2d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  veranFinal_spiderForm_setVulnerableCollisionData_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_setVulnerableCollisionData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c2d, 0x5c2e); H = D;  // ld h,d
  CYC(0x5c2e, 0x5c30); L = 0xa5;  // ld l,Enemy.enemyCollisionMode
  CYC(0x5c30, 0x5c32); mem_wr(gb, HL, 0x6b);  // ld (hl),ENEMYCOLLISION_VERAN_SPIDER_FORM_VULNERABLE
  CYC(0x5c32, 0x5c34); L = 0xa6;  // ld l,Enemy.collisionRadiusY
  CYC(0x5c34, 0x5c36); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x5c36, 0x5c38); A = 0x06;  // ld a,$06
  CYC(0x5c38, 0x5c3b); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void veranFinal_spiderForm_webAttack_substate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c3b, ecom_decCounter2_b10_hook, 0x43a3, 0x5c3e);
  if (!(F & FZ)) { RET_TAKEN(0x5c3e); return; } CYC(0x5c3e, 0x5c3f);  // ret nz
  CYC(0x5c3f, 0x5c41); B = 0x56;  // ld b,$56
  CALL_C(0x5c41, ecom_spawnProjectile_b10_hook, 0x437c, 0x5c44);
  if (!(F & FZ)) { RET_TAKEN(0x5c44); return; } CYC(0x5c44, 0x5c45);  // ret nz
  CYC(0x5c45, 0x5c47); L = 0xc2;  // ld l,Part.subid
  CYC(0x5c47, 0x5c48); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CALL_C(0x5c48, ecom_incSubstate_b10_hook, 0x4005, 0x5c4b);
  CYC(0x5c4b, 0x5c4c); L = alu_inc8(gb, L);  // inc l
  CYC(0x5c4c, 0x5c4e); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(0x5c4e, 0x5c51); veranFinal_spiderForm_resetCollisionData_hook(gb); return;  // jr veranFinal_spiderForm_resetCollisionData
}

void veranFinal_spiderForm_webAttack_substate3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c50, 0x5c52); E = 0xb8;  // ld e,Enemy.var38
  CYC(0x5c52, 0x5c53); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5c53, 0x5c54); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(0x5c54); return; } CYC(0x5c54, 0x5c55);  // ret z
  CALL_C(0x5c55, ecom_decCounter1_b10_hook, 0x439a, 0x5c58);
  if (!(F & FZ)) { RET_TAKEN(0x5c58); return; } CYC(0x5c58, 0x5c59);  // ret nz
  CYC(0x5c59, 0x5c5c); A = mem_rd(gb, 0xd004);  // ld a,(w1Link.state)
  CYC(0x5c5c, 0x5c5e); alu_cp(gb, 0x0d);  // cp LINK_STATE_GRABBED
  if (!(F & FZ)) { CYCT(0x5c5e, 0x5c61); veranFinal_spiderForm_setCounter2AndInitState2_hook(gb); return; } CYC(0x5c5e, 0x5c61);  // jp nz,veranFinal_spiderForm_setCounter2AndInitState2
  CALL_C(0x5c61, ecom_incSubstate_b10_hook, 0x4005, 0x5c64);
  CYC(0x5c64, 0x5c65); L = alu_inc8(gb, L);  // inc l
  CYC(0x5c65, 0x5c67); mem_wr(gb, HL, 0x10);  // ld (hl),$10
  CYC(0x5c67, 0x5c69); A = 0x06;  // ld a,$06
  CALL_C(0x5c69, enemySetAnimation_hook, 0x282b, 0x5c6c);
  CYC(0x5c6c, 0x5c6e); B = 0xf8;  // ld b,$f8
  veranFinal_spiderForm_webAttack_updateLinkPosition_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_webAttack_updateLinkPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c6e, 0x5c71); SET_HL(0xd000);  // ld hl,w1Link
  CYC(0x5c71, 0x5c73); C = 0x00;  // ld c,$00
  CYC(0x5c73, 0x5c76); if (hook_enabled_at(0x225a)) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(0x225a);  // jp objectCopyPositionWithOffset
}

void veranFinal_spiderForm_webAttack_substate4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c76, ecom_decCounter1_b10_hook, 0x439a, 0x5c79);
  if (!(F & FZ)) { RET_TAKEN(0x5c79); return; } CYC(0x5c79, 0x5c7a);  // ret nz
  CYC(0x5c7a, 0x5c7c); mem_wr(gb, HL, 0x04);  // ld (hl),$04
  CYC(0x5c7c, 0x5c7d); L = E;  // ld l,e
  CYC(0x5c7d, 0x5c7e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5c7e, 0x5c80); A = 0x05;  // ld a,$05
  CALL_C(0x5c80, enemySetAnimation_hook, 0x282b, 0x5c83);
  CYC(0x5c83, 0x5c85); A = 0x04;  // ld a,$04
  CALL_C(0x5c85, setScreenShakeCounter_hook, 0x24bb, 0x5c88);
  CYC(0x5c88, 0x5c8a); B = 0x14;  // ld b,$14
  CALL_C(0x5c8a, veranFinal_spiderForm_webAttack_updateLinkPosition_hook, 0x5c6e, 0x5c8d);
  CYC(0x5c8d, 0x5c90); SET_BC(0xfa08);  // ld bc,-6,$08
  veranFinal_spiderForm_webAttack_applyDamageToLink_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_webAttack_applyDamageToLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5c90, 0x5c92); L = 0x25;  // ld l,<w1Link.damageToApply
  CYC(0x5c92, 0x5c93); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(0x5c93, 0x5c95); L = 0x2b;  // ld l,<w1Link.invincibilityCounter
  CYC(0x5c95, 0x5c96); mem_wr(gb, HL, C);  // ld (hl),c
  CYC(0x5c96, 0x5c98); A = 0x81;  // ld a,SND_STRONG_POUND
  CYC(0x5c98, 0x5c9b); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
}

void veranFinal_spiderForm_webAttack_substate5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5c9b, ecom_decCounter1_b10_hook, 0x439a, 0x5c9e);
  if (!(F & FZ)) { RET_TAKEN(0x5c9e); return; } CYC(0x5c9e, 0x5c9f);  // ret nz
  CYC(0x5c9f, 0x5ca1); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x5ca1, 0x5ca2); L = E;  // ld l,e
  CYC(0x5ca2, 0x5ca3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5ca3, 0x5ca5); A = 0x06;  // ld a,$06
  CALL_C(0x5ca5, enemySetAnimation_hook, 0x282b, 0x5ca8);
  CYC(0x5ca8, 0x5caa); B = 0xf6;  // ld b,$f6
  CYC(0x5caa, 0x5cac); veranFinal_spiderForm_webAttack_updateLinkPosition_hook(gb); return;  // jr veranFinal_spiderForm_webAttack_updateLinkPosition
}

void veranFinal_spiderForm_webAttack_substate6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5cac, ecom_decCounter1_b10_hook, 0x439a, 0x5caf);
  if (!(F & FZ)) { RET_TAKEN(0x5caf); return; } CYC(0x5caf, 0x5cb0);  // ret nz
  CYC(0x5cb0, 0x5cb2); mem_wr(gb, HL, 0x0f);  // ld (hl),$0f
  CYC(0x5cb2, 0x5cb3); L = E;  // ld l,e
  CYC(0x5cb3, 0x5cb4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5cb4, 0x5cb6); A = 0x05;  // ld a,$05
  CALL_C(0x5cb6, enemySetAnimation_hook, 0x282b, 0x5cb9);
  CYC(0x5cb9, 0x5cbb); A = 0x14;  // ld a,$14
  CALL_C(0x5cbb, setScreenShakeCounter_hook, 0x24bb, 0x5cbe);
  CYC(0x5cbe, 0x5cc0); B = 0x14;  // ld b,$14
  CALL_C(0x5cc0, veranFinal_spiderForm_webAttack_updateLinkPosition_hook, 0x5c6e, 0x5cc3);
  CYC(0x5cc3, 0x5cc6); SET_BC(0xf618);  // ld bc,-10,$18
  CYC(0x5cc6, 0x5cc9); veranFinal_spiderForm_webAttack_applyDamageToLink_hook(gb); return;  // jr veranFinal_spiderForm_webAttack_applyDamageToLink
}

void veranFinal_spiderForm_webAttack_substate7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5cc8, ecom_decCounter1_b10_hook, 0x439a, 0x5ccb);
  if (!(F & FZ)) { RET_TAKEN(0x5ccb); return; } CYC(0x5ccb, 0x5ccc);  // ret nz
  CYC(0x5ccc, 0x5cce); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5cce, 0x5cd0); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(0x5cd0, veranFinal_spiderForm_setCounter2AndInitState2_hook, 0x5ad8, 0x5cd3);
  veranFinal_grabbingLink_hook(gb); return;  // fallthrough
}

void veranFinal_grabbingLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5cd3, 0x5cd6); SET_HL(0xd005);  // ld hl,w1Link.substate
  CYC(0x5cd6, 0x5cd8); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(0x5cd8, 0x5cda); L = 0x24;  // ld l,<w1Link.collisionType
  CYC(0x5cda, 0x5cdc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  RET(0x5cdc); return;  // ret
}

void veranFinal_beeForm_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5cdd, 0x5cde); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5cde, 0x5cdf); push_effect(gb, 0x5cdf);
  switch (veranFinal_jump_table(gb)) {
    case 0x5ac0: veranFinal_spiderOrBeeForm_state0_hook(gb); return;
    case 0x5cf7: veranFinal_beeForm_state1_hook(gb); return;
    case 0x5d29: veranFinal_beeForm_state2_hook(gb); return;
    case 0x5d52: veranFinal_beeForm_state3_hook(gb); return;
    case 0x5d7d: veranFinal_beeForm_state4_hook(gb); return;
    case 0x5da3: veranFinal_beeForm_state5_hook(gb); return;
    case 0x5dca: veranFinal_beeForm_state6_hook(gb); return;
    case 0x5df4: veranFinal_beeForm_state7_hook(gb); return;
    case 0x5e2f: veranFinal_beeForm_state8_hook(gb); return;
    case 0x5e43: veranFinal_beeForm_state9_hook(gb); return;
    case 0x5e4f: veranFinal_beeForm_stateA_hook(gb); return;
    case 0x5e82: veranFinal_beeForm_stateB_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void veranFinal_beeForm_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5cf7, enemyAnimate_hook, 0x2818, 0x5cfa);
  CYC(0x5cfa, 0x5cfc); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(0x5cfc, 0x5cfd); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5cfd, 0x5cfe); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { RET_TAKEN(0x5cfe); return; } CYC(0x5cfe, 0x5cff);  // ret nz
  CYC(0x5cff, 0x5d01); A = 0x07;  // ld a,$07
  CALL_C(0x5d01, enemySetAnimation_hook, 0x282b, 0x5d04);
  CALL_C(0x5d04, ecom_incState_b10_hook, 0x4000, 0x5d07);
  CYC(0x5d07, 0x5d09); L = 0x90;  // ld l,Enemy.speed
  CYC(0x5d09, 0x5d0b); mem_wr(gb, HL, 0x50);  // ld (hl),SPEED_200
  CYC(0x5d0b, 0x5d0e); SET_BC(0x100c);  // ld bc,$100c
  CYC(0x5d0e, 0x5d10); E = 0x6b;  // ld e,ENEMYCOLLISION_VERAN_SPIDER_FORM_VULNERABLE
  CYC(0x5d10, 0x5d12); L = 0xb2;  // ld l,Enemy.var32
  veranFinal_initializeForm_hook(gb); return;  // fallthrough
}

void veranFinal_initializeForm_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d12, 0x5d13); H = D;  // ld h,d
  CYC(0x5d13, 0x5d14); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5d14, 0x5d16); L = 0xa9;  // ld l,Enemy.health
  CYC(0x5d16, 0x5d17); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5d17, 0x5d19); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5d19, 0x5d1b); mem_wr(gb, HL, 0x82);  // ld (hl),$80|ENEMY_VERAN_FINAL_FORM
  CYC(0x5d1b, 0x5d1c); L = alu_inc8(gb, L);  // inc l
  CYC(0x5d1c, 0x5d1d); mem_wr(gb, HL, E);  // ld (hl),e
  CYC(0x5d1d, 0x5d1f); L = 0xa6;  // ld l,Enemy.collisionRadiusY
  CYC(0x5d1f, 0x5d20); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(0x5d20, 0x5d21); L = alu_inc8(gb, L);  // inc l
  CYC(0x5d21, 0x5d22); mem_wr(gb, HL, C);  // ld (hl),c
  CYC(0x5d22, 0x5d24); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x5d24, 0x5d26); A = 0x06;  // ld a,$06
  CYC(0x5d26, 0x5d27); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5d27, 0x5d28); mem_wr(gb, HL, A);  // ld (hl),a
  RET(0x5d28); return;  // ret
}

void veranFinal_beeForm_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d29, 0x5d2b); E = 0x8b;  // ld e,Enemy.yh
  CYC(0x5d2b, 0x5d2c); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5d2c, 0x5d2e); mem_wr(gb, 0xff8f, A);  // ldh (<hFF8F),a
  CYC(0x5d2e, 0x5d30); E = 0x8d;  // ld e,Enemy.xh
  CYC(0x5d30, 0x5d31); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5d31, 0x5d33); mem_wr(gb, 0xff8e, A);  // ldh (<hFF8E),a
  CYC(0x5d33, 0x5d36); SET_BC(0x5878);  // ldbc LARGE_ROOM_HEIGHT<<3,LARGE_ROOM_WIDTH<<3
  CYC(0x5d36, 0x5d37); alu_sub(gb, C);  // sub c
  CYC(0x5d37, 0x5d39); alu_add(gb, 0x02);  // add $02
  CYC(0x5d39, 0x5d3b); alu_cp(gb, 0x05);  // cp $05
  if (!(F & FC)) { CYCT(0x5d3b, 0x5d3d); goto updateMovement; } CYC(0x5d3b, 0x5d3d);  // jr nc,@updateMovement
  CYC(0x5d3d, 0x5d3f); A = mem_rd(gb, 0xff8f);  // ldh a,(<hFF8F)
  CYC(0x5d3f, 0x5d40); alu_sub(gb, B);  // sub b
  CYC(0x5d40, 0x5d42); alu_add(gb, 0x02);  // add $02
  CYC(0x5d42, 0x5d44); alu_cp(gb, 0x05);  // cp $05
  if (!(F & FC)) { CYCT(0x5d44, 0x5d46); goto updateMovement; } CYC(0x5d44, 0x5d46);  // jr nc,@updateMovement
  CALL_C(0x5d46, ecom_incState_b10_hook, 0x4000, 0x5d49);
  CYC(0x5d49, 0x5d4c); veranFinal_beeForm_chooseRandomTargetPosition_hook(gb); return;  // jp veranFinal_beeForm_chooseRandomTargetPosition
updateMovement:
  CALL_C(0x5d4c, ecom_moveTowardPosition_b10_hook, 0x4430, 0x5d4f);
  veranFinal_beeForm_animate_hook(gb); return;  // fallthrough
}

void veranFinal_beeForm_animate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d4f, 0x5d52); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818);  // jp enemyAnimate
}

void veranFinal_beeForm_state3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d52, veranFinal_moveTowardTargetPosition_hook, 0x601c, 0x5d55);
  if (!(F & FC)) { CYCT(0x5d55, 0x5d57); veranFinal_beeForm_animate_hook(gb); return; } CYC(0x5d55, 0x5d57);  // jr nc,veranFinal_beeForm_animate
  CYC(0x5d57, 0x5d59); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x5d59, 0x5d5a); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(0x5d5a, 0x5d5c); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5d5c, 0x5d5d); mem_wr(gb, HL, C);  // ld (hl),c
  CALL_C(0x5d5d, veranFinal_beeForm_nextTargetPosition_hook, 0x5fe5, 0x5d60);
  CALL_C(0x5d60, ecom_decCounter2_b10_hook, 0x43a3, 0x5d63);
  if (!(F & FZ)) { CYCT(0x5d63, 0x5d65); veranFinal_beeForm_animate_hook(gb); return; } CYC(0x5d63, 0x5d65);  // jr nz,veranFinal_beeForm_animate
  CYC(0x5d65, 0x5d67); L = 0x84;  // ld l,Enemy.state
  CYC(0x5d67, 0x5d68); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5d68, 0x5d6a); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x5d6a, 0x5d6c); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(0x5d6c, 0x5d6e); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5d6e, 0x5d70); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  CYC(0x5d70, 0x5d72); A = 0x00;  // ld a,$00
  if (!(F & FZ)) { CYCT(0x5d72, 0x5d74); goto setVar37; } CYC(0x5d72, 0x5d74);  // jr nz,+
  CYC(0x5d74, 0x5d76); A = 0xf0;  // ld a,$f0
setVar37:
  CYC(0x5d76, 0x5d78); L = 0xb7;  // ld l,Enemy.var37
  CYC(0x5d78, 0x5d79); mem_wr(gb, HL, A); SET_HL(HL - 1);  // ldd (hl),a
  CYC(0x5d79, 0x5d7b); mem_wr(gb, HL, 0xe0);  // ld (hl),$e0
  CYC(0x5d7b, 0x5d7d); veranFinal_beeForm_animate_hook(gb); return;  // jr veranFinal_beeForm_animate
}

void veranFinal_beeForm_state4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5d7d, ecom_decCounter1_b10_hook, 0x439a, 0x5d80);
  if (!(F & FZ)) { CYCT(0x5d80, 0x5d82); goto applySpeed; } CYC(0x5d80, 0x5d82);  // jr nz,+
  CYC(0x5d82, 0x5d84); mem_wr(gb, HL, 0x06);  // ld (hl),$06
  CYC(0x5d84, 0x5d86); L = 0xb6;  // ld l,Enemy.var36
  CALL_C(0x5d86, ecom_readPositionVars_b10_hook, 0x4439, 0x5d89);
  CALL_C(0x5d89, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x5d8c);
  CALL_C(0x5d8c, objectNudgeAngleTowards_hook, 0x1fd4, 0x5d8f);
applySpeed:
  CALL_C(0x5d8f, objectApplySpeed_hook, 0x201d, 0x5d92);
  CYC(0x5d92, 0x5d94); E = 0x8b;  // ld e,Enemy.yh
  CYC(0x5d94, 0x5d95); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5d95, 0x5d97); alu_cp(gb, 0xc0);  // cp (LARGE_ROOM_HEIGHT+1)<<4
  if ((F & FC)) { CYCT(0x5d97, 0x5d99); veranFinal_beeForm_animate_hook(gb); return; } CYC(0x5d97, 0x5d99);  // jr c,veranFinal_beeForm_animate
  CALL_C(0x5d99, ecom_incState_b10_hook, 0x4000, 0x5d9c);
  CYC(0x5d9c, 0x5d9e); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x5d9e, 0x5da0); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x5da0, 0x5da3); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);  // jp objectSetInvisible
}

void veranFinal_beeForm_state5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5da3, ecom_decCounter1_b10_hook, 0x439a, 0x5da6);
  if (!(F & FZ)) { RET_TAKEN(0x5da6); return; } CYC(0x5da6, 0x5da7);  // ret nz
  CYC(0x5da7, 0x5da9); mem_wr(gb, HL, 15);  // ld (hl),15
  CYC(0x5da9, 0x5daa); L = E;  // ld l,e
  CYC(0x5daa, 0x5dab); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5dab, 0x5dad); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x5dad, 0x5daf); mem_wr(gb, HL, 0x20);  // ld (hl),$20
  CALL_C(0x5daf, getRandomNumber_hook, 0x043e, 0x5db2);
  CYC(0x5db2, 0x5db4); alu_and(gb, 0x10);  // and $10
  CYC(0x5db4, 0x5db7); SET_BC(0x08e8);  // ldbc $08,$e8
  if ((F & FZ)) { CYCT(0x5db7, 0x5db9); goto haveAngle; } CYC(0x5db7, 0x5db9);  // jr z,++
  CYC(0x5db9, 0x5dba); B = C;  // ld b,c
  CYC(0x5dba, 0x5dbc); C = 0x08;  // ld c,$08
haveAngle:
  CYC(0x5dbc, 0x5dbe); alu_add(gb, 0x08);  // add $08
  CYC(0x5dbe, 0x5dc0); L = 0x89;  // ld l,Enemy.angle
  CYC(0x5dc0, 0x5dc1); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5dc1, 0x5dc3); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5dc3, 0x5dc4); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(0x5dc4, 0x5dc6); L = 0xb7;  // ld l,Enemy.var37
  CYC(0x5dc6, 0x5dc7); mem_wr(gb, HL, C);  // ld (hl),c
  CYC(0x5dc7, 0x5dca); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72);  // jp objectSetVisible83
}

void veranFinal_beeForm_state6_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5dca, ecom_decCounter1_b10_hook, 0x439a, 0x5dcd);
  if (!(F & FZ)) { CYCT(0x5dcd, 0x5dcf); goto applySpeed; } CYC(0x5dcd, 0x5dcf);  // jr nz,+
  CYC(0x5dcf, 0x5dd1); mem_wr(gb, HL, 15);  // ld (hl),15
  CYC(0x5dd1, 0x5dd3); B = 0x58;  // ld b,PART_VERAN_BEE_PROJECTILE
  CALL_C(0x5dd3, ecom_spawnProjectile_b10_hook, 0x437c, 0x5dd6);
applySpeed:
  CALL_C(0x5dd6, objectApplySpeed_hook, 0x201d, 0x5dd9);
  CYC(0x5dd9, 0x5ddb); E = 0x8d;  // ld e,Enemy.xh
  CYC(0x5ddb, 0x5ddc); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5ddc, 0x5ddd); H = D;  // ld h,d
  CYC(0x5ddd, 0x5ddf); L = 0xb7;  // ld l,Enemy.var37
  CYC(0x5ddf, 0x5de0); alu_sub(gb, mem_rd(gb, HL));  // sub (hl)
  CYC(0x5de0, 0x5de1); A = alu_inc8(gb, A);  // inc a
  CYC(0x5de1, 0x5de3); alu_cp(gb, 0x03);  // cp $03
  if (!(F & FC)) { CYCT(0x5de3, 0x5de6); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x5de3, 0x5de6);  // jp nc,enemyAnimate
  CALL_C(0x5de6, ecom_incState_b10_hook, 0x4000, 0x5de9);
  CYC(0x5de9, 0x5deb); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x5deb, 0x5ded); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x5ded, 0x5def); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5def, 0x5df1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(0x5df1, 0x5df4); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);  // jp objectSetInvisible
}

void veranFinal_beeForm_state7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5df4, ecom_decCounter1_b10_hook, 0x439a, 0x5df7);
  if (!(F & FZ)) { RET_TAKEN(0x5df7); return; } CYC(0x5df7, 0x5df8);  // ret nz
  CALL_C(0x5df8, veranFinal_getQuadrant_hook, 0x6036, 0x5dfb);
chooseQuadrant:
  CALL_C(0x5dfb, getRandomNumber_hook, 0x043e, 0x5dfe);
  CYC(0x5dfe, 0x5dff); C = A;  // ld c,a
  CYC(0x5dff, 0x5e01); alu_and(gb, 0x03);  // and $03
  CYC(0x5e01, 0x5e02); alu_cp(gb, B);  // cp b
  if ((F & FZ)) { CYCT(0x5e02, 0x5e04); goto chooseQuadrant; } CYC(0x5e02, 0x5e04);  // jr z,-
  CYC(0x5e04, 0x5e06); E = 0xb9;  // ld e,Enemy.var39
  CYC(0x5e06, 0x5e07); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5e07, 0x5e08); alu_add(gb, A);  // add a
  CYC(0x5e08, 0x5e0b); SET_HL(0x5eaa);  // ld hl,veranFinal_beeForm_screenCornerEntrances
  CYC(0x5e0b, 0x5e0c);
  veranFinal_addDoubleIndexToHl_from_rst(gb, 0x5e0c);  // rst_addDoubleIndexToHl
  CYC(0x5e0c, 0x5e0e); E = 0xb6;  // ld e,Enemy.var36
  CYC(0x5e0e, 0x5e0f); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x5e0f, 0x5e10); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5e10, 0x5e11); E = alu_inc8(gb, E);  // inc e
  CYC(0x5e11, 0x5e12); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x5e12, 0x5e13); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5e13, 0x5e15); E = 0x8b;  // ld e,Enemy.yh
  CYC(0x5e15, 0x5e16); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x5e16, 0x5e17); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5e17, 0x5e19); E = 0x8d;  // ld e,Enemy.xh
  CYC(0x5e19, 0x5e1a); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5e1a, 0x5e1b); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5e1b, 0x5e1c); A = C;  // ld a,c
  CYC(0x5e1c, 0x5e1e); alu_and(gb, 0x30);  // and $30
  CYC(0x5e1e, 0x5e20); A = alu_swap(gb, A);  // swap a
  CYC(0x5e20, 0x5e22); alu_add(gb, 0x02);  // add $02
  CYC(0x5e22, 0x5e24); E = 0x87;  // ld e,Enemy.counter2
  CYC(0x5e24, 0x5e25); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(0x5e25, ecom_incState_b10_hook, 0x4000, 0x5e28);
  CYC(0x5e28, 0x5e2a); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5e2a, 0x5e2c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x5e2c, 0x5e2f); if (hook_enabled_at(0x1e72)) { objectSetVisible83_hook(gb); return; } HANDOFF(0x1e72);  // jp objectSetVisible83
}

void veranFinal_beeForm_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5e2f, veranFinal_moveTowardTargetPosition_hook, 0x601c, 0x5e32);
  if (!(F & FC)) { CYCT(0x5e32, 0x5e34); veranFinal_beeForm_animate2_hook(gb); return; } CYC(0x5e32, 0x5e34);  // jr nc,veranFinal_beeForm_animate2
  CYC(0x5e34, 0x5e36); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x5e36, 0x5e37); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(0x5e37, 0x5e39); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5e39, 0x5e3a); mem_wr(gb, HL, C);  // ld (hl),c
  CYC(0x5e3a, 0x5e3c); L = 0x84;  // ld l,Enemy.state
  CYC(0x5e3c, 0x5e3d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5e3d, 0x5e3f); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x5e3f, 0x5e41); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x5e41, 0x5e43); veranFinal_beeForm_animate2_hook(gb); return;  // jr veranFinal_beeForm_animate2
}

void veranFinal_beeForm_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5e43, ecom_decCounter1_b10_hook, 0x439a, 0x5e46);
  if (!(F & FZ)) { CYCT(0x5e46, 0x5e48); veranFinal_beeForm_animate2_hook(gb); return; } CYC(0x5e46, 0x5e48);  // jr nz,veranFinal_beeForm_animate2
  CYC(0x5e48, 0x5e4a); mem_wr(gb, HL, 25);  // ld (hl),25
  CYC(0x5e4a, 0x5e4b); L = E;  // ld l,e
  CYC(0x5e4b, 0x5e4c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  veranFinal_beeForm_animate2_hook(gb); return;  // fallthrough
}

void veranFinal_beeForm_animate2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e4c, 0x5e4f); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818);  // jp enemyAnimate
}

void veranFinal_beeForm_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5e4f, ecom_decCounter1_b10_hook, 0x439a, 0x5e52);
  if ((F & FZ)) { CYCT(0x5e52, 0x5e54); label_10_173_hook(gb); return; } CYC(0x5e52, 0x5e54);  // jr z,label_10_173
  CYC(0x5e54, 0x5e55); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5e55, 0x5e57); alu_and(gb, 0x07);  // and $07
  if (!(F & FZ)) { CYCT(0x5e57, 0x5e59); veranFinal_beeForm_animate2_hook(gb); return; } CYC(0x5e57, 0x5e59);  // jr nz,veranFinal_beeForm_animate2
  CYC(0x5e59, 0x5e5a); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5e5a, 0x5e5c); alu_and(gb, 0x18);  // and $18
  CYC(0x5e5c, 0x5e5e); A = alu_swap(gb, A);  // swap a
  CYC(0x5e5e, 0x5e5f); alu_rlca(gb);  // rlca
  CYC(0x5e5f, 0x5e60); A = alu_dec8(gb, A);  // dec a
  CYC(0x5e60, 0x5e61); B = A;  // ld b,a
  CALL_C(0x5e61, getFreeEnemySlot_hook, 0x2e27, 0x5e64);
  if (!(F & FZ)) { CYCT(0x5e64, 0x5e66); veranFinal_beeForm_animate2_hook(gb); return; } CYC(0x5e64, 0x5e66);  // jr nz,veranFinal_beeForm_animate2
  CYC(0x5e66, 0x5e68); mem_wr(gb, HL, 0x1f);  // ld (hl),ENEMY_VERAN_CHILD_BEE
  CYC(0x5e68, 0x5e69); L = alu_inc8(gb, L);  // inc l
  CYC(0x5e69, 0x5e6a); mem_wr(gb, HL, B);  // ld (hl),b [child.subid]
  CALL_C(0x5e6a, objectCopyPosition_hook, 0x2242, 0x5e6d);
  CYC(0x5e6d, 0x5e6f); A = 0xba;  // ld a,SND_BEAM1
  CALL_C(0x5e6f, playSound_b00_hook, 0x0c98, 0x5e72);
  CYC(0x5e72, 0x5e74); veranFinal_beeForm_animate2_hook(gb); return;  // jr veranFinal_beeForm_animate2
}

void label_10_173_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5e74, 0x5e76); mem_wr(gb, HL, 20);  // ld (hl),20
  CYC(0x5e76, 0x5e77); L = alu_inc8(gb, L);  // inc l
  CYC(0x5e77, 0x5e78); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl) [counter2]
  CYC(0x5e78, 0x5e79); L = E;  // ld l,e
  if ((F & FZ)) { CYCT(0x5e79, 0x5e7b); goto setState02; } CYC(0x5e79, 0x5e7b);  // jr z,+
  CYC(0x5e7b, 0x5e7c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [state]=$0b
  CYC(0x5e7c, 0x5e7f); veranFinal_beeForm_animate2_hook(gb); return;  // jr veranFinal_beeForm_animate2
setState02:
  CYC(0x5e7e, 0x5e80); mem_wr(gb, HL, 0x02);  // ld (hl),$02 [state]
  CYC(0x5e80, 0x5e83); veranFinal_beeForm_animate2_hook(gb); return;  // jr veranFinal_beeForm_animate2
}

void veranFinal_beeForm_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5e82, ecom_decCounter1_b10_hook, 0x439a, 0x5e85);
  if (!(F & FZ)) { CYCT(0x5e85, 0x5e87); veranFinal_beeForm_animate2_hook(gb); return; } CYC(0x5e85, 0x5e87);  // jr nz,veranFinal_beeForm_animate2
  CYC(0x5e87, 0x5e88); L = E;  // ld l,e
  CYC(0x5e88, 0x5e8a); mem_wr(gb, HL, 0x08);  // ld (hl),$08 [state]
  CALL_C(0x5e8a, veranFinal_getQuadrant_hook, 0x6036, 0x5e8d);
chooseQuadrant:
  CALL_C(0x5e8d, getRandomNumber_hook, 0x043e, 0x5e90);
  CYC(0x5e90, 0x5e92); alu_and(gb, 0x03);  // and $03
  CYC(0x5e92, 0x5e93); alu_cp(gb, B);  // cp b
  if ((F & FZ)) { CYCT(0x5e93, 0x5e95); goto chooseQuadrant; } CYC(0x5e93, 0x5e95);  // jr z,@chooseQuadrant
  CYC(0x5e95, 0x5e96); H = D;  // ld h,d
  CYC(0x5e96, 0x5e98); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x5e98, 0x5e99); alu_cp(gb, mem_rd(gb, HL));  // cp (hl)
  if ((F & FZ)) { CYCT(0x5e99, 0x5e9b); goto chooseQuadrant; } CYC(0x5e99, 0x5e9b);  // jr z,@chooseQuadrant
  CYC(0x5e9b, 0x5e9c); mem_wr(gb, HL, A);  // ld (hl),a [var39]
  CYC(0x5e9c, 0x5e9d); alu_add(gb, A);  // add a
  CYC(0x5e9d, 0x5ea0); SET_HL(0x5eaa);  // ld hl,veranFinal_beeForm_screenCornerEntrances
  CYC(0x5ea0, 0x5ea1);
  veranFinal_addDoubleIndexToHl_from_rst(gb, 0x5ea1);  // rst_addDoubleIndexToHl
  CYC(0x5ea1, 0x5ea3); E = 0xb6;  // ld e,Enemy.var36
  CYC(0x5ea3, 0x5ea4); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x5ea4, 0x5ea5); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5ea5, 0x5ea6); E = alu_inc8(gb, E);  // inc e
  CYC(0x5ea6, 0x5ea7); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5ea7, 0x5ea8); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5ea8, 0x5eab); veranFinal_beeForm_animate2_hook(gb); return;  // jr veranFinal_beeForm_animate2
}

void veranFinal_transformToBeeOrSpider_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5eba, 0x5ebc); mem_wr(gb, HL, 0x01);  // ld (hl),$01 [state]
  CYC(0x5ebc, 0x5ebe); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5ebe, 0x5ec0); mem_wr(gb, HL, 0x96);  // ld (hl),$80|ENEMY_BEAMOS
  CYC(0x5ec0, 0x5ec2); L = 0xa9;  // ld l,Enemy.health
  CYC(0x5ec2, 0x5ec3); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5ec3, 0x5ec5); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x5ec5, 0x5ec6); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5ec6, 0x5ec8); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x5ec8, 0x5eca); A = 0x07;  // ld a,$07
  CYC(0x5eca, 0x5ecb); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5ecb, 0x5ecc); mem_wr(gb, HL, A);  // ld (hl),a
  CALL_C(0x5ecc, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5ecf);
  CYC(0x5ecf, 0x5ed1); alu_and(gb, 0x03);  // and $03
  CYC(0x5ed1, 0x5ed2); B = A;  // ld b,a
  CYC(0x5ed2, 0x5ed4); E = 0xb5;  // ld e,Enemy.var35
  CYC(0x5ed4, 0x5ed5); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5ed5, 0x5ed6); C = A;  // ld c,a
  CYC(0x5ed6, 0x5ed7); A = alu_inc8(gb, A);  // inc a
  CYC(0x5ed7, 0x5ed9); alu_and(gb, 0x07);  // and $07
  CYC(0x5ed9, 0x5eda); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5eda, 0x5edb); A = C;  // ld a,c
  CYC(0x5edb, 0x5edc); alu_add(gb, A);  // add a
  CYC(0x5edc, 0x5edd); alu_add(gb, A);  // add a
  CYC(0x5edd, 0x5ede); alu_add(gb, B);  // add b
  CYC(0x5ede, 0x5ee1); SET_HL(0x5ef6);  // ld hl,@transformSequence
  CALL_C(0x5ee1, checkFlag_hook, 0x0205, 0x5ee4);
  if ((F & FZ)) { CYCT(0x5ee4, 0x5ee6); goto haveSubid; } CYC(0x5ee4, 0x5ee6);  // jr z,+
  CYC(0x5ee6, 0x5ee8); A = 0x01;  // ld a,$01
haveSubid:
  CYC(0x5ee8, 0x5ee9); A = alu_inc8(gb, A);  // inc a
  CYC(0x5ee9, 0x5eeb); E = 0x82;  // ld e,Enemy.subid
  CYC(0x5eeb, 0x5eec); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5eec, 0x5eee); alu_add(gb, 0x09);  // add $09
  CALL_C(0x5eee, enemySetAnimation_hook, 0x282b, 0x5ef1);
  CYC(0x5ef1, 0x5ef3); A = 0xc0;  // ld a,SND_TRANSFORM
  CYC(0x5ef3, 0x5ef6); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
}

void veranFinal_spiderForm_checkLinkWithinDistance_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5efa, 0x5efb); A = B;  // ld a,b
  CYC(0x5efb, 0x5efc); alu_add(gb, A);  // add a
  CYC(0x5efc, 0x5efd); A = alu_inc8(gb, A);  // inc a
  CYC(0x5efd, 0x5efe); C = A;  // ld c,a
  CYC(0x5efe, 0x5f01); A = mem_rd(gb, 0xd00b);  // ld a,(w1Link.yh)
  CYC(0x5f01, 0x5f02); H = D;  // ld h,d
  CYC(0x5f02, 0x5f04); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x5f04, 0x5f05); alu_sub(gb, mem_rd(gb, HL));  // sub (hl)
  CYC(0x5f05, 0x5f06); alu_add(gb, B);  // add b
  CYC(0x5f06, 0x5f07); alu_cp(gb, C);  // cp c
  if (!(F & FC)) { RET_TAKEN(0x5f07); return; } CYC(0x5f07, 0x5f08);  // ret nc
  CYC(0x5f08, 0x5f0b); A = mem_rd(gb, 0xd00d);  // ld a,(w1Link.xh)
  CYC(0x5f0b, 0x5f0d); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x5f0d, 0x5f0e); alu_sub(gb, mem_rd(gb, HL));  // sub (hl)
  CYC(0x5f0e, 0x5f0f); alu_add(gb, B);  // add b
  CYC(0x5f0f, 0x5f10); alu_cp(gb, C);  // cp c
  RET(0x5f10); return;  // ret
}

void veranFinal_spiderForm_decideWhetherToAttack_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5f11, objectGetAngleTowardLink_hook, 0x1e9c, 0x5f14);
  CYC(0x5f14, 0x5f15); E = A;  // ld e,a
considerRushAttack:
  CYC(0x5f15, 0x5f17); B = 0x60;  // ld b,$60
  CALL_C(0x5f17, veranFinal_spiderForm_checkLinkWithinDistance_hook, 0x5efa, 0x5f1a);
  if (!(F & FC)) { CYCT(0x5f1a, 0x5f1c); goto considerJumpAttack; } CYC(0x5f1a, 0x5f1c);  // jr nc,@considerJumpAttack
  CYC(0x5f1c, 0x5f1e); alu_and(gb, 0x07);  // and $07
  CYC(0x5f1e, 0x5f20); alu_sub(gb, 0x03);  // sub $03
  CYC(0x5f20, 0x5f22); alu_cp(gb, 0x03);  // cp $03
  CYC(0x5f22, 0x5f24); A = 0x00;  // ld a,$00
  if ((F & FC)) { CYCT(0x5f24, 0x5f26); goto doAttack; } CYC(0x5f24, 0x5f26);  // jr c,@doAttack
considerJumpAttack:
  CYC(0x5f26, 0x5f28); B = 0x50;  // ld b,$50
  CALL_C(0x5f28, veranFinal_spiderForm_checkLinkWithinDistance_hook, 0x5efa, 0x5f2b);
  if ((F & FC)) { CYCT(0x5f2b, 0x5f2d); goto considerGrabAttack; } CYC(0x5f2b, 0x5f2d);  // jr c,@considerGrabAttack
  CYC(0x5f2d, 0x5f2e); A = E;  // ld a,e
  CYC(0x5f2e, 0x5f30); alu_and(gb, 0x07);  // and $07
  CYC(0x5f30, 0x5f32); alu_sub(gb, 0x03);  // sub $03
  CYC(0x5f32, 0x5f34); alu_cp(gb, 0x03);  // cp $03
  CYC(0x5f34, 0x5f35); alu_ccf(gb);  // ccf
  CYC(0x5f35, 0x5f37); A = 0x01;  // ld a,$01
  if ((F & FC)) { CYCT(0x5f37, 0x5f39); goto doAttack; } CYC(0x5f37, 0x5f39);  // jr c,@doAttack
considerGrabAttack:
  CYC(0x5f39, 0x5f3a); A = E;  // ld a,e
  CYC(0x5f3a, 0x5f3c); alu_sub(gb, 0x0c);  // sub $0c
  CYC(0x5f3c, 0x5f3e); alu_cp(gb, 0x09);  // cp $09
  if (!(F & FC)) { RET_TAKEN(0x5f3e); return; } CYC(0x5f3e, 0x5f3f);  // ret nc
  CYC(0x5f3f, 0x5f41); A = 0x02;  // ld a,$02
doAttack:
  CYC(0x5f41, 0x5f43); E = 0x83;  // ld e,Enemy.var03
  CYC(0x5f43, 0x5f44); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5f44, 0x5f45); H = D;  // ld h,d
  CYC(0x5f45, 0x5f47); L = 0x84;  // ld l,Enemy.state
  CYC(0x5f47, 0x5f49); mem_wr(gb, HL, 0x04);  // ld (hl),$04
  CYC(0x5f49, 0x5f4a); L = alu_inc8(gb, L);  // inc l
  CYC(0x5f4a, 0x5f4c); mem_wr(gb, HL, 0x00);  // ld (hl),$00 [substate]
  CYC(0x5f4c, 0x5f4d); alu_scf(gb);  // scf
  RET(0x5f4d); return;  // ret
}

void veranFinal_dead_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5f4e, 0x5f50); E = 0x82;  // ld e,Enemy.subid
  CYC(0x5f50, 0x5f51); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5f51, 0x5f52); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(0x5f52, 0x5f54); goto transformed; } CYC(0x5f52, 0x5f54);  // jr nz,@transformed
  CYC(0x5f54, 0x5f55); H = D;  // ld h,d
  CYC(0x5f55, 0x5f57); L = 0x84;  // ld l,Enemy.state
  CYC(0x5f57, 0x5f59); mem_wr(gb, HL, 0x0a);  // ld (hl),$0a
  CYC(0x5f59, 0x5f5a); L = alu_inc8(gb, L);  // inc l
  CYC(0x5f5a, 0x5f5c); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x5f5c, 0x5f5e); L = 0xa9;  // ld l,Enemy.health
  CYC(0x5f5e, 0x5f5f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x5f5f, 0x5f61); A = 0xf0;  // ld a,SNDCTRL_STOPMUSIC
  CYC(0x5f61, 0x5f64); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
transformed:
  CYC(0x5f64, 0x5f65); B = A;  // ld b,a
  CYC(0x5f65, 0x5f66); H = D;  // ld h,d
  CYC(0x5f66, 0x5f67); L = E;  // ld l,e
  CYC(0x5f67, 0x5f69); mem_wr(gb, HL, 0x00);  // ld (hl),$00 [subid]
  CYC(0x5f69, 0x5f6b); L = 0x84;  // ld l,Enemy.state
  CYC(0x5f6b, 0x5f6d); mem_wr(gb, HL, 0x09);  // ld (hl),$09
  CYC(0x5f6d, 0x5f6f); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x5f6f, 0x5f70); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5f70, 0x5f72); L = 0xa9;  // ld l,Enemy.health
  CYC(0x5f72, 0x5f73); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5f73, 0x5f75); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x5f75, 0x5f77); mem_wr(gb, HL, 0x96);  // ld (hl),$80|ENEMY_BEAMOS
  CYC(0x5f77, 0x5f79); L = 0xa6;  // ld l,Enemy.collisionRadiusY
  CYC(0x5f79, 0x5f7b); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x5f7b, 0x5f7c); L = alu_inc8(gb, L);  // inc l
  CYC(0x5f7c, 0x5f7e); mem_wr(gb, HL, 0x0a);  // ld (hl),$0a [collisionRadiusX]
  CYC(0x5f7e, 0x5f80); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x5f80, 0x5f82); A = 0x07;  // ld a,$07
  CYC(0x5f82, 0x5f83); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x5f83, 0x5f84); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x5f84, 0x5f85); A = B;  // ld a,b [subid]
  CYC(0x5f85, 0x5f87); alu_add(gb, 0x07);  // add $07
  CALL_C(0x5f87, enemySetAnimation_hook, 0x282b, 0x5f8a);
  CYC(0x5f8a, 0x5f8c); A = 0xc0;  // ld a,SND_TRANSFORM
  CYC(0x5f8c, 0x5f8f); if (hook_enabled_at(0x0c98)) { playSound_b00_hook(gb); return; } HANDOFF(0x0c98);  // jp playSound
}

void veranFinal_spiderForm_decideAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5f8f, 0x5f91); B = 0x00;  // ld b,$00
  CYC(0x5f91, 0x5f93); E = 0x8b;  // ld e,Enemy.yh
  CYC(0x5f93, 0x5f94); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5f94, 0x5f96); alu_cp(gb, 0x58);  // cp (LARGE_ROOM_HEIGHT<<4)/2
  if ((F & FC)) { CYCT(0x5f96, 0x5f98); goto haveB1; } CYC(0x5f96, 0x5f98);  // jr c,+
  CYC(0x5f98, 0x5f9a); B = 0x10;  // ld b,$10
haveB1:
  CYC(0x5f9a, 0x5f9c); E = 0x8d;  // ld e,Enemy.xh
  CYC(0x5f9c, 0x5f9d); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5f9d, 0x5f9f); alu_cp(gb, 0x78);  // cp (LARGE_ROOM_WIDTH<<4)/2
  if ((F & FC)) { CYCT(0x5f9f, 0x5fa1); goto haveB2; } CYC(0x5f9f, 0x5fa1);  // jr c,+
  CYC(0x5fa1, 0x5fa3); B = (uint8_t)(B | (1 << 3));  // set 3,b
haveB2:
  CALL_C(0x5fa3, getRandomNumber_hook, 0x043e, 0x5fa6);
  CYC(0x5fa6, 0x5fa8); alu_and(gb, 0x07);  // and $07
  CYC(0x5fa8, 0x5fa9); alu_add(gb, B);  // add b
  CYC(0x5fa9, 0x5fac); SET_HL(0x5fb2);  // ld hl,@angles
  CYC(0x5fac, 0x5fad);
  veranFinal_addAToHl_from_rst(gb, 0x5fad);  // rst_addAToHl
  CYC(0x5fad, 0x5faf); E = 0x89;  // ld e,Enemy.angle
  CYC(0x5faf, 0x5fb0); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5fb0, 0x5fb1); mem_wr(gb, DE, A);  // ld (de),a
  RET(0x5fb1); return;  // ret
}

void veranFinal_beeForm_chooseRandomTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5fd2, 0x5fd5); SET_BC(0x0801);  // ld bc,$0801
  CALL_C(0x5fd5, ecom_randomBitwiseAndBCE_b10_hook, 0x434f, 0x5fd8);
  CYC(0x5fd8, 0x5fda); E = 0x86;  // ld e,Enemy.counter1
  CYC(0x5fda, 0x5fdb); A = B;  // ld a,b
  CYC(0x5fdb, 0x5fdc); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5fdc, 0x5fdd); A = C;  // ld a,c
  CYC(0x5fdd, 0x5fe0); SET_HL(0x5ffa);  // ld hl,veranFinal_beeForm_counter2Vals
  CYC(0x5fe0, 0x5fe1);
  veranFinal_addAToHl_from_rst(gb, 0x5fe1);  // rst_addAToHl
  CYC(0x5fe1, 0x5fe3); E = 0x87;  // ld e,Enemy.counter2
  CYC(0x5fe3, 0x5fe4); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5fe4, 0x5fe5); mem_wr(gb, DE, A);  // ld (de),a
  veranFinal_beeForm_nextTargetPosition_hook(gb); return;  // fallthrough
}

void veranFinal_beeForm_nextTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5fe5, 0x5fe7); E = 0x86;  // ld e,Enemy.counter1
  CYC(0x5fe7, 0x5fe8); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x5fe8, 0x5fe9); B = A;  // ld b,a
  CYC(0x5fe9, 0x5fea); A = alu_inc8(gb, A);  // inc a
  CYC(0x5fea, 0x5fec); alu_and(gb, 0x0f);  // and $0f
  CYC(0x5fec, 0x5fed); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5fed, 0x5fee); A = B;  // ld a,b
  CYC(0x5fee, 0x5ff1); SET_HL(0x5ffc);  // ld hl,veranFinal_beeForm_targetPositions
  CYC(0x5ff1, 0x5ff2);
  veranFinal_addDoubleIndexToHl_from_rst(gb, 0x5ff2);  // rst_addDoubleIndexToHl
  CYC(0x5ff2, 0x5ff4); E = 0xb6;  // ld e,Enemy.var36
  CYC(0x5ff4, 0x5ff5); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x5ff5, 0x5ff6); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x5ff6, 0x5ff7); E = alu_inc8(gb, E);  // inc e
  CYC(0x5ff7, 0x5ff8); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x5ff8, 0x5ff9); mem_wr(gb, DE, A);  // ld (de),a
  RET(0x5ff9); return;  // ret
}

void veranFinal_moveTowardTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x601c, 0x601d); H = D;  // ld h,d
  CYC(0x601d, 0x601f); L = 0xb6;  // ld l,Enemy.var36
  CALL_C(0x601f, ecom_readPositionVars_b10_hook, 0x4439, 0x6022);
  CYC(0x6022, 0x6023); alu_sub(gb, C);  // sub c
  CYC(0x6023, 0x6025); alu_add(gb, 0x02);  // add $02
  CYC(0x6025, 0x6027); alu_cp(gb, 0x05);  // cp $05
  if (!(F & FC)) { CYCT(0x6027, 0x6029); goto moveToward; } CYC(0x6027, 0x6029);  // jr nc,+
  CYC(0x6029, 0x602b); A = mem_rd(gb, 0xff8f);  // ldh a,(<hFF8F)
  CYC(0x602b, 0x602c); alu_sub(gb, B);  // sub b
  CYC(0x602c, 0x602e); alu_add(gb, 0x02);  // add $02
  CYC(0x602e, 0x6030); alu_cp(gb, 0x05);  // cp $05
  if ((F & FC)) { RET_TAKEN(0x6030); return; } CYC(0x6030, 0x6031);  // ret c
moveToward:
  CALL_C(0x6031, ecom_moveTowardPosition_b10_hook, 0x4430, 0x6034);
  CYC(0x6034, 0x6035); alu_or(gb, D);  // or d
  RET(0x6035); return;  // ret
}

void veranFinal_getQuadrant_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6036, 0x6038); B = 0x00;  // ld b,$00
  CYC(0x6038, 0x603a); A = mem_rd(gb, 0xffb0);  // ldh a,(<hEnemyTargetY)
  CYC(0x603a, 0x603c); alu_cp(gb, 0x58);  // cp LARGE_ROOM_HEIGHT*16/2
  if ((F & FC)) { CYCT(0x603c, 0x603e); goto checkX; } CYC(0x603c, 0x603e);  // jr c,+
  CYC(0x603e, 0x6040); B = 0x02;  // ld b,$02
checkX:
  CYC(0x6040, 0x6042); A = mem_rd(gb, 0xffb1);  // ldh a,(<hEnemyTargetX)
  CYC(0x6042, 0x6044); alu_cp(gb, 0x78);  // cp LARGE_ROOM_WIDTH*16/2
  if ((F & FC)) { RET_TAKEN(0x6044); return; } CYC(0x6044, 0x6045);  // ret c
  CYC(0x6045, 0x6046); B = alu_inc8(gb, B);  // inc b
  RET(0x6046); return;  // ret
}
