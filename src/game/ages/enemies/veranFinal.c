#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if ((F & FZ)) { CYCT(b_+0, b_+2); goto normalStatus; } CYC(b_+0, b_+2);  // jr z,@normalStatus
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);  // sub ENEMYSTATUS_NO_HEALTH
  if ((F & FC)) { RET_TAKEN(b_+4); return; } CYC(b_+4, b_+5);  // ret c
  if ((F & FZ)) { CYCT(b_+5, b_+7); goto dead; } CYC(b_+5, b_+7);  // jr z,@dead
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);  // dec a
  if ((F & FZ)) { CYCT(b_+8, b_+10); goto justHit; } CYC(b_+8, b_+10);  // jr z,@justHit
  CYC(b_+10, b_+12); C = 0x20;  // ld c,$20
  CALL_C(b_+12, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+15);
  CYC(b_+15, b_+18); if (hook_enabled_at(gb, SYM(ecom_updateKnockback_b10))) { ecom_updateKnockback_b10_hook(gb); return; } HANDOFF(SYM(ecom_updateKnockback_b10));  // jp ecom_updateKnockback
justHit:
  CYC(b_+18, b_+19); H = D;  // ld h,d
  CYC(b_+19, b_+21); L = 0x82;  // ld l,Enemy.subid
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+22, b_+23); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto notTurtleForm; } CYC(b_+23, b_+25);  // jr nz,@notTurtleForm
  CYC(b_+25, b_+27); L = 0xab;  // ld l,Enemy.invincibilityCounter
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+28, b_+29); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+29, b_+31); goto normalStatus; } CYC(b_+29, b_+31);  // jr z,@normalStatus
  CYC(b_+31, b_+33); L = 0xb3;  // ld l,Enemy.var33
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYCT(b_+35, b_+38); goto normalStatus;  // jr @normalStatus
notTurtleForm:
  CYC(b_+37, b_+39); L = 0xad;  // ld l,Enemy.knockbackCounter
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+40, b_+41); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+41, b_+43); goto normalStatus; } CYC(b_+41, b_+43);  // jr z,@normalStatus
  CYC(b_+43, b_+45); L = 0x84;  // ld l,Enemy.state
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x03);  // ld (hl),$03
  CYC(b_+47, b_+49); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+49, b_+51); mem_wr(gb, HL, 105);  // ld (hl),105
  CYC(b_+51, b_+53); L = 0xa5;  // ld l,Enemy.enemyCollisionMode
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x6b);  // ld (hl),ENEMYCOLLISION_VERAN_SPIDER_FORM_VULNERABLE
  CYC(b_+55, b_+58); A = mem_rd(gb, w1Link_state);  // ld a,(w1Link.state)
  CYC(b_+58, b_+60); alu_cp(gb, 0x0d);  // cp LINK_STATE_GRABBED
  if ((F & FZ)) { CALL_C_CC(b_+60, veranFinal_grabbingLink_hook, SYM(veranFinal_grabbingLink), b_+63); } else CYC(b_+60, b_+63);  // call z,veranFinal_grabbingLink
  CYC(b_+63, b_+65); A = 0x06;  // ld a,$06
  CYC(b_+65, b_+68); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
dead:
  CALL_C(b_+68, veranFinal_dead_hook, SYM(veranFinal_dead), b_+71);
normalStatus:
  CYC(b_+71, b_+73); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+74, b_+76); E = 0x84;  // ld e,Enemy.state
  CYC(b_+76, b_+77); push_effect(gb, b_+77);
  do { uint16_t jt_ = (veranFinal_jump_table(gb));
    if (jt_ == SYM(veranFinal_turtleForm)) { veranFinal_turtleForm_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm)) { veranFinal_spiderForm_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm)) { veranFinal_beeForm_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void veranFinal_turtleForm_hook(GB *gb) {
  BASE(veranFinal_turtleForm);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (veranFinal_jump_table(gb));
    if (jt_ == SYM(veranFinal_turtleForm_state0)) { veranFinal_turtleForm_state0_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_turtleForm_state1)) { veranFinal_turtleForm_state1_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_turtleForm_state2)) { veranFinal_turtleForm_state2_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_turtleForm_state3)) { veranFinal_turtleForm_state3_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_turtleForm_state4)) { veranFinal_turtleForm_state4_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_turtleForm_state5)) { veranFinal_turtleForm_state5_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_turtleForm_state6)) { veranFinal_turtleForm_state6_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_turtleForm_state7)) { veranFinal_turtleForm_state7_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_turtleForm_state8)) { veranFinal_turtleForm_state8_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_turtleForm_state9)) { veranFinal_turtleForm_state9_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_turtleForm_stateA)) { veranFinal_turtleForm_stateA_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void veranFinal_turtleForm_state0_hook(GB *gb) {
  BASE(veranFinal_turtleForm_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;  // ld a,$02
  CYC(b_+2, b_+5); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);  // ld (wEnemyIDToLoadExtraGfx),a
  CYC(b_+5, b_+7); A = 0x87;  // ld a,PALH_87
  CALL_C(b_+7, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+10);
  CYC(b_+10, b_+12); A = 0xf0;  // ld a,SNDCTRL_STOPMUSIC
  CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  CYC(b_+15, b_+17); A = 0x01;  // ld a,$01
  CYC(b_+17, b_+20); mem_wr(gb, wDisabledObjects, A);  // ld (wDisabledObjects),a
  CYC(b_+20, b_+23); mem_wr(gb, wMenuDisabled, A);  // ld (wMenuDisabled),a
  CYC(b_+23, b_+26); SET_BC(0x0208);  // ld bc,$0208
  CALL_C(b_+26, enemyBoss_spawnShadow_b10_hook, SYM(enemyBoss_spawnShadow_b10), b_+29);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; } CYC(b_+29, b_+30);  // ret nz
  CALL_C(b_+30, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+33);
  CALL_C(b_+33, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+36);
  CYC(b_+36, b_+38); L = 0xa9;  // ld l,Enemy.health
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+39, b_+42); SET_BC(0x0c18);  // ld bc,$0c18
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto setForms; } CYC(b_+42, b_+44);  // jr nz,+
  CYC(b_+44, b_+46); A = 0x14;  // ld a,$14
  CYC(b_+46, b_+47); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+47, b_+50); SET_BC(0x080f);  // ld bc,$080f
setForms:
  CYC(b_+50, b_+52); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+53, b_+54); mem_wr(gb, HL, B);  // ld (hl),b [var31]
  CYC(b_+54, b_+55); L = alu_inc8(gb, L);  // inc l
  CYC(b_+55, b_+56); mem_wr(gb, HL, C);  // ld (hl),c [var32]
  CYC(b_+56, b_+59); if (hook_enabled_at(gb, SYM(objectSetVisible83))) { objectSetVisible83_hook(gb); return; } HANDOFF(SYM(objectSetVisible83));  // jp objectSetVisible83
}

void veranFinal_turtleForm_state1_hook(GB *gb) {
  BASE(veranFinal_turtleForm_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (veranFinal_jump_table(gb));
    if (jt_ == b_+9) { goto substate0; }
    else if (jt_ == b_+28) { goto substate1; }
    else if (jt_ == b_+51) { goto substate2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CYC(b_+9, b_+12); A = mem_rd(gb, wPaletteThread_mode);  // ld a,(wPaletteThread_mode)
  CYC(b_+12, b_+13); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; } CYC(b_+13, b_+14);  // ret nz
  CYC(b_+14, b_+16); A = 0xd2;  // ld a,SND_LIGHTNING
  CALL_C(b_+16, playSound_b00_hook, SYM(playSound_b00), b_+19);
  CYC(b_+19, b_+22); SET_BC((SYM(ganon_stateC_substate8) + 29));  // ld bc,TX_5614
  CALL_C(b_+22, showText_hook, SYM(showText), b_+25);
  CYC(b_+25, b_+28); if (hook_enabled_at(gb, SYM(ecom_incSubstate_b10))) { ecom_incSubstate_b10_hook(gb); return; } HANDOFF(SYM(ecom_incSubstate_b10));  // jp ecom_incSubstate
substate1:
  CYC(b_+28, b_+29); H = D;  // ld h,d
  CYC(b_+29, b_+30); L = E;  // ld l,e
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+31, b_+32); alu_xor(gb, A);  // xor a
  CYC(b_+32, b_+35); mem_wr(gb, wDisabledObjects, A);  // ld (wDisabledObjects),a
  CYC(b_+35, b_+38); mem_wr(gb, wMenuDisabled, A);  // ld (wMenuDisabled),a
  CYC(b_+38, b_+40); A = 0x03;  // ld a,$03
  CALL_C(b_+40, enemySetAnimation_hook, SYM(enemySetAnimation), b_+43);
  CYC(b_+43, b_+45); A = 0x32;  // ld a,MUS_FINAL_BOSS
  CYC(b_+45, b_+48); mem_wr(gb, wActiveMusic, A);  // ld (wActiveMusic),a
  CYC(b_+48, b_+51); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
substate2:
  CALL_C(b_+51, enemyAnimate_hook, SYM(enemyAnimate), b_+54);
  CYC(b_+54, b_+56); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+57, b_+58); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { RET_TAKEN(b_+58); return; } CYC(b_+58, b_+59);  // ret nz
  CALL_C(b_+59, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+62);
  CYC(b_+62, b_+64); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+64, b_+66); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+66, b_+68); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x46);  // ld (hl),SPEED_1c0
  CYC(b_+70, b_+71); A = alu_inc8(gb, A);  // inc a
  CYC(b_+71, b_+74); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void veranFinal_turtleForm_state2_hook(GB *gb) {
  BASE(veranFinal_turtleForm_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(b_+4, b_+7); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+4, b_+7);  // jp nz,enemyAnimate
  CALL_C(b_+7, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } CYC(b_+10, b_+11);  // ret nz
  CYC(b_+11, b_+13); L = 0x84;  // ld l,Enemy.state
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+14, b_+16); L = 0x94;  // ld l,Enemy.speedZ
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x00);  // ld (hl),<(-$400)
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);  // inc l
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0xfc);  // ld (hl),>(-$400)
  CALL_C(b_+21, ecom_updateAngleTowardTarget_b10_hook, SYM(ecom_updateAngleTowardTarget_b10), b_+24);
  CALL_C(b_+24, objectSetVisible81_hook, SYM(objectSetVisible81), b_+27);
  CYC(b_+27, b_+29); A = 0x69;  // ld a,SND_UNKNOWN4
  CALL_C(b_+29, playSound_b00_hook, SYM(playSound_b00), b_+32);
  CYC(b_+32, b_+34); A = 0x02;  // ld a,$02
  CYC(b_+34, b_+37); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void veranFinal_turtleForm_state3_hook(GB *gb) {
  BASE(veranFinal_turtleForm_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;  // ld c,$20
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL - 1);  // ldd a,(hl)
  CYC(b_+6, b_+7); alu_or(gb, mem_rd(gb, HL));  // or (hl)
  if (!(F & FZ)) { CYCT(b_+7, b_+10); if (hook_enabled_at(gb, SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b10))) { ecom_applyVelocityForTopDownEnemyNoHoles_b10_hook(gb); return; } HANDOFF(SYM(ecom_applyVelocityForTopDownEnemyNoHoles_b10)); } CYC(b_+7, b_+10);  // jp nz,ecom_applyVelocityForTopDownEnemyNoHoles
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);  // inc l
  CYC(b_+11, b_+12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [speedZ]=$0100
  CYC(b_+12, b_+14); L = 0x84;  // ld l,Enemy.state
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+15, b_+17); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x78);  // ld (hl),SPEED_300
  CYC(b_+19, b_+21); L = 0xb6;  // ld l,Enemy.var36
  CYC(b_+21, b_+23); A = mem_rd(gb, hEnemyTargetY);  // ldh a,(<hEnemyTargetY)
  CYC(b_+23, b_+25); alu_and(gb, 0xf0);  // and $f0
  CYC(b_+25, b_+27); alu_add(gb, 0x08);  // add $08
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+28, b_+30); A = mem_rd(gb, hEnemyTargetX);  // ldh a,(<hEnemyTargetX)
  CYC(b_+30, b_+32); alu_and(gb, 0xf0);  // and $f0
  CYC(b_+32, b_+34); alu_add(gb, 0x08);  // add $08
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);  // ld (hl),a [var37]
  CYC(b_+35, b_+37); A = 0x01;  // ld a,$01
  CYC(b_+37, b_+40); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void veranFinal_turtleForm_state4_hook(GB *gb) {
  BASE(veranFinal_turtleForm_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;  // ld c,$10
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if ((F & FZ)) { CYCT(b_+5, b_+7); goto nextState; } CYC(b_+5, b_+7);  // jr z,@nextState
  CALL_C(b_+7, veranFinal_moveTowardTargetPosition_hook, SYM(veranFinal_moveTowardTargetPosition), b_+10);
  if (!(F & FC)) { RET_TAKEN(b_+10); return; } CYC(b_+10, b_+11);  // ret nc
  CYC(b_+11, b_+13); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+13, b_+14); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(b_+14, b_+16); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+16, b_+17); mem_wr(gb, HL, C);  // ld (hl),c
  RET(b_+17); return;  // ret
nextState:
  CYC(b_+18, b_+20); A = 0x10;  // ld a,$10
  CALL_C(b_+20, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+23);
  CALL_C(b_+23, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+26);
  CYC(b_+26, b_+28); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x0c);  // ld (hl),$0c
  CALL_C(b_+30, objectSetVisible83_hook, SYM(objectSetVisible83), b_+33);
  CYC(b_+33, b_+35); A = 0x98;  // ld a,$98
  CALL_C(b_+35, playSound_b00_hook, SYM(playSound_b00), b_+38);
  CYC(b_+38, b_+40); B = 0x57;  // ld b,PART_VERAN_ACID_POOL
  CYC(b_+40, b_+43); if (hook_enabled_at(gb, SYM(ecom_spawnProjectile_b10))) { ecom_spawnProjectile_b10_hook(gb); return; } HANDOFF(SYM(ecom_spawnProjectile_b10));  // jp ecom_spawnProjectile
}

void veranFinal_turtleForm_state5_hook(GB *gb) {
  BASE(veranFinal_turtleForm_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x46);  // ld (hl),SPEED_1c0
  CYC(b_+8, b_+10); L = 0xb3;  // ld l,Enemy.var33
  CYC(b_+10, b_+12); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  CYC(b_+12, b_+14); L = 0xb4;  // ld l,Enemy.var34
  if ((F & FZ)) { CYCT(b_+14, b_+16); goto haveVar34; } CYC(b_+14, b_+16);  // jr z,+
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
haveVar34:
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+18, b_+21); SET_BC(b_+72);  // ld bc,@transformProbabilities
  CALL_C(b_+21, addAToBc_hook, 0x006d, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, BC);  // ld a,(bc)
  CYC(b_+25, b_+26); B = A;  // ld b,a
  CYC(b_+26, b_+27); A = alu_inc8(gb, A);  // inc a
  CYC(b_+27, b_+28); L = E;  // ld l,e
  if ((F & FZ)) { CYCT(b_+28, b_+30); goto tryOpenFace; } CYC(b_+28, b_+30);  // jr z,++
  CALL_C(b_+30, getRandomNumber_hook, SYM(getRandomNumber), b_+33);
  CYC(b_+33, b_+34); alu_and(gb, B);  // and b
  if ((F & FZ)) { CYCT(b_+34, b_+37); veranFinal_transformToBeeOrSpider_hook(gb); return; } CYC(b_+34, b_+37);  // jp z,veranFinal_transformToBeeOrSpider
  CYC(b_+37, b_+39); E = 0xb3;  // ld e,Enemy.var33
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+40, b_+41); alu_rrca(gb);  // rrca
  if ((F & FC)) { CYCT(b_+41, b_+43); goto jumpAgain; } CYC(b_+41, b_+43);  // jr c,@jumpAgain
tryOpenFace:
  CALL_C(b_+43, getRandomNumber_hook, SYM(getRandomNumber), b_+46);
  CYC(b_+46, b_+48); alu_cp(gb, 90);  // cp 90
  if (!(F & FC)) { CYCT(b_+48, b_+50); goto jumpAgain; } CYC(b_+48, b_+50);  // jr nc,@jumpAgain
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [substate]=6
  CYC(b_+51, b_+53); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+55, b_+57); A = 0xc7;  // ld a,SND_GORON
  CALL_C(b_+57, playSound_b00_hook, SYM(playSound_b00), b_+60);
  CYC(b_+60, b_+62); A = 0x04;  // ld a,$04
  CYC(b_+62, b_+65); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
jumpAgain:
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x02);  // ld (hl),$02 [state]
  CYC(b_+67, b_+69); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+69, b_+71); mem_wr(gb, HL, 30);  // ld (hl),30
  RET(b_+71); return;  // ret
}

void veranFinal_turtleForm_state6_hook(GB *gb) {
  BASE(veranFinal_turtleForm_state6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+4); H = D;  // ld h,d
  CYC(b_+4, b_+6); L = 0xa1;  // ld l,Enemy.animParameter
  CYC(b_+6, b_+8); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto nextState; } CYC(b_+8, b_+10);  // jr nz,@nextState
  CYC(b_+10, b_+12); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  if ((F & FZ)) { RET_TAKEN(b_+12); return; } CYC(b_+12, b_+13);  // ret z
  CYC(b_+13, b_+15); L = 0xa5;  // ld l,Enemy.enemyCollisionMode
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x09);  // ld (hl),ENEMYCOLLISION_VERAN_TURTLE_FORM_VULNERABLE
  RET(b_+17); return;  // ret
nextState:
  CYC(b_+18, b_+20); L = 0x84;  // ld l,Enemy.state
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+21, b_+23); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+23, b_+25); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(b_+25, b_+26); alu_xor(gb, A);  // xor a
  CYC(b_+26, b_+29); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void veranFinal_turtleForm_state7_hook(GB *gb) {
  BASE(veranFinal_turtleForm_state7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+3, b_+6);  // jp nz,enemyAnimate
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0x03;  // ld a,$03
  CYC(b_+10, b_+13); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void veranFinal_turtleForm_state8_hook(GB *gb) {
  BASE(veranFinal_turtleForm_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+4); H = D;  // ld h,d
  CYC(b_+4, b_+6); L = 0xa1;  // ld l,Enemy.animParameter
  CYC(b_+6, b_+8); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto nextState; } CYC(b_+8, b_+10);  // jr nz,@nextState
  CYC(b_+10, b_+12); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  if ((F & FZ)) { RET_TAKEN(b_+12); return; } CYC(b_+12, b_+13);  // ret z
  CYC(b_+13, b_+15); L = 0xa5;  // ld l,Enemy.enemyCollisionMode
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x6a);  // ld (hl),ENEMYCOLLISION_VERAN_TURTLE_FORM
  RET(b_+17); return;  // ret
nextState:
  CYC(b_+18, b_+20); L = 0x84;  // ld l,Enemy.state
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(b_+22, b_+24); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+24, b_+26); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+26, b_+28); A = 0x01;  // ld a,$01
  CYC(b_+28, b_+31); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void veranFinal_turtleForm_state9_hook(GB *gb) {
  BASE(veranFinal_turtleForm_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } CYC(b_+7, b_+8);  // ret nz
  CYC(b_+8, b_+9); H = D;  // ld h,d
  CYC(b_+9, b_+11); L = 0xb3;  // ld l,Enemy.var33
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a [var33]=0
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);  // ld (hl),a [var34]=0
  CYC(b_+13, b_+15); L = 0x84;  // ld l,Enemy.state
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(b_+16, b_+18); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x82);  // ld (hl),$80|ENEMY_VERAN_FINAL_FORM
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);  // inc l
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x09);  // ld (hl),ENEMYCOLLISION_VERAN_TURTLE_FORM_VULNERABLE
  CYC(b_+23, b_+25); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+25, b_+27); A = 0x06;  // ld a,$06
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+29, b_+31); A = 0x03;  // ld a,$03
  CYC(b_+31, b_+34); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void veranFinal_turtleForm_stateA_hook(GB *gb) {
  BASE(veranFinal_turtleForm_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (veranFinal_jump_table(gb));
    if (jt_ == b_+11) { goto substate0; }
    else if (jt_ == b_+51) { goto substate1; }
    else if (jt_ == b_+74) { goto substate2; }
    else if (jt_ == b_+88) { goto substate3; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CYC(b_+11, b_+13); E = 0xab;  // ld e,Enemy.invincibilityCounter
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+14, b_+15); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; } CYC(b_+15, b_+16);  // ret nz
  CALL_C(b_+16, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+19);
  if (!(F & FC)) { RET_TAKEN(b_+19); return; } CYC(b_+19, b_+20);  // ret nc
  CYC(b_+20, b_+22); A = 0x01;  // ld a,$01
  CYC(b_+22, b_+25); mem_wr(gb, wMenuDisabled, A);  // ld (wMenuDisabled),a
  CYC(b_+25, b_+28); mem_wr(gb, wDisabledObjects, A);  // ld (wDisabledObjects),a
  CALL_C(b_+28, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+31);
  CALL_C(b_+31, clearAllParentItems_hook, SYM(clearAllParentItems), b_+34);
  CALL_C(b_+34, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+37);
  CALL_C(b_+37, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+40);
  CYC(b_+40, b_+43); SET_BC((SYM(ganon_stateC_substate8) + 30));  // ld bc,TX_5615
  if ((F & FZ)) { CYCT(b_+43, b_+45); goto showText; } CYC(b_+43, b_+45);  // jr z,+
  CYC(b_+45, b_+48); SET_BC((SYM(ganon_stateC_substate8) + 31));  // ld bc,TX_5616
showText:
  CYC(b_+48, b_+51); if (hook_enabled_at(gb, SYM(showText))) { showText_hook(gb); return; } HANDOFF(SYM(showText));  // jp showText
substate1:
  CYC(b_+51, b_+54); A = mem_rd(gb, wOamEnd);  // ld a,(wTextIsActive)
  CYC(b_+54, b_+55); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+55); return; } CYC(b_+55, b_+56);  // ret nz
  CALL_C(b_+56, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+59);
  CYC(b_+59, b_+61); L = 0x87;  // ld l,Enemy.counter2
  CYC(b_+61, b_+63); mem_wr(gb, HL, 40);  // ld (hl),40
  CYC(b_+63, b_+65); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+65, b_+66); B = mem_rd(gb, HL);  // ld b,(hl)
  CYC(b_+66, b_+68); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+68, b_+69); C = mem_rd(gb, HL);  // ld c,(hl)
  CYC(b_+69, b_+71); A = 0xff;  // ld a,$ff
  CYC(b_+71, b_+74); if (hook_enabled_at(gb, SYM(createEnergySwirlGoingOut))) { createEnergySwirlGoingOut_hook(gb); return; } HANDOFF(SYM(createEnergySwirlGoingOut));  // jp createEnergySwirlGoingOut
substate2:
  CALL_C(b_+74, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+77);
  if (!(F & FZ)) { RET_TAKEN(b_+77); return; } CYC(b_+77, b_+78);  // ret nz
  CYC(b_+78, b_+81); SET_BC(0x9021);  // ldbc INTERAC_MISC_PUZZLES,$21
  CALL_C(b_+81, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+84);
  if (!(F & FZ)) { RET_TAKEN(b_+84); return; } CYC(b_+84, b_+85);  // ret nz
  CYC(b_+85, b_+88); if (hook_enabled_at(gb, SYM(ecom_incSubstate_b10))) { ecom_incSubstate_b10_hook(gb); return; } HANDOFF(SYM(ecom_incSubstate_b10));  // jp ecom_incSubstate
substate3:
  CYC(b_+88, b_+91); A = mem_rd(gb, wPaletteThread_mode);  // ld a,(wPaletteThread_mode)
  CYC(b_+91, b_+92); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+92); return; } CYC(b_+92, b_+93);  // ret nz
  CYC(b_+93, b_+96); SET_HL((wGroup4RoomFlags + 252));  // ld hl,wGroup4RoomFlags+(<ROOM_AGES_4fc)
  CYC(b_+96, b_+98); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+98, b_+100); A = 0x09;  // ld a,CUTSCENE_BLACK_TOWER_ESCAPE
  CYC(b_+100, b_+103); mem_wr(gb, wCutsceneTrigger, A);  // ld (wCutsceneTrigger),a
  CALL_C(b_+103, incMakuTreeState_hook, SYM(incMakuTreeState), b_+106);
  CYC(b_+106, b_+109); if (hook_enabled_at(gb, SYM(enemyDelete))) { enemyDelete_hook(gb); return; } HANDOFF(SYM(enemyDelete));  // jp enemyDelete
}

void veranFinal_spiderForm_hook(GB *gb) {
  BASE(veranFinal_spiderForm);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (veranFinal_jump_table(gb));
    if (jt_ == SYM(veranFinal_spiderOrBeeForm_state0)) { veranFinal_spiderOrBeeForm_state0_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_state1)) { veranFinal_spiderForm_state1_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_state2)) { veranFinal_spiderForm_state2_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_state3)) { veranFinal_spiderForm_state3_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_state4)) { veranFinal_spiderForm_state4_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void veranFinal_spiderOrBeeForm_state0_hook(GB *gb) {
  BASE(veranFinal_spiderOrBeeForm_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;  // ret
}

void veranFinal_spiderForm_state1_hook(GB *gb) {
  BASE(veranFinal_spiderForm_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } CYC(b_+7, b_+8);  // ret nz
  CYC(b_+8, b_+11); SET_BC(0x1010);  // ld bc,$1010
  CYC(b_+11, b_+13); E = 0x6c;  // ld e,ENEMYCOLLISION_VERAN_SPIDER_FORM
  CYC(b_+13, b_+15); L = 0xb1;  // ld l,Enemy.var31
  CALL_C(b_+15, veranFinal_initializeForm_hook, SYM(veranFinal_initializeForm), b_+18);
  CYC(b_+18, b_+20); A = 0x05;  // ld a,$05
  CALL_C(b_+20, enemySetAnimation_hook, SYM(enemySetAnimation), SYM(veranFinal_spiderForm_setCounter2AndInitState2));
  veranFinal_spiderForm_setCounter2AndInitState2_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_setCounter2AndInitState2_hook(GB *gb) {
  BASE(veranFinal_spiderForm_setCounter2AndInitState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x87;  // ld e,Enemy.counter2
  CYC(b_+2, b_+4); A = 120;  // ld a,120
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);  // ld (de),a
  veranFinal_spiderForm_initState2_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_initState2_hook(GB *gb) {
  BASE(veranFinal_spiderForm_initState2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0x84;  // ld l,Enemy.state
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(b_+5, b_+7); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+7, b_+9); mem_wr(gb, HL, 30);  // ld (hl),SPEED_c0
  CALL_C(b_+9, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+12);
  CYC(b_+12, b_+14); alu_and(gb, 0x03);  // and $03
  CYC(b_+14, b_+17); SET_HL(b_+27);  // ld hl,@counter1Vals
  CYC(b_+17, b_+18);
  veranFinal_addAToHl_from_rst(gb, b_+18);  // rst_addAToHl
  CYC(b_+18, b_+20); E = 0x86;  // ld e,Enemy.counter1
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(b_+22, veranFinal_spiderForm_decideAngle_hook, SYM(veranFinal_spiderForm_decideAngle), b_+25);
  CYC(b_+25, b_+27); veranFinal_spiderForm_animate_hook(gb); return;  // jr veranFinal_spiderForm_animate
}

void veranFinal_spiderForm_state2_hook(GB *gb) {
  BASE(veranFinal_spiderForm_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto decCounter1; } CYC(b_+3, b_+5);  // jr nz,+
  CYC(b_+5, b_+7); mem_wr(gb, HL, 120);  // ld (hl),120
  CALL_C(b_+7, veranFinal_spiderForm_decideWhetherToAttack_hook, SYM(veranFinal_spiderForm_decideWhetherToAttack), b_+10);
  if ((F & FC)) { RET_TAKEN(b_+10); return; } CYC(b_+10, b_+11);  // ret c
decCounter1:
  CALL_C(b_+11, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+14);
  if ((F & FZ)) { CYCT(b_+14, b_+16); veranFinal_spiderForm_initState2_hook(gb); return; } CYC(b_+14, b_+16);  // jr z,veranFinal_spiderForm_initState2
  veranFinal_spiderForm_updateMovement_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_updateMovement_hook(GB *gb) {
  BASE(veranFinal_spiderForm_updateMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_bounceOffWallsAndHoles_b10_hook, SYM(ecom_bounceOffWallsAndHoles_b10), b_+3);
  CALL_C(b_+3, objectApplySpeed_hook, SYM(objectApplySpeed), SYM(veranFinal_spiderForm_animate));
  veranFinal_spiderForm_animate_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_animate_hook(GB *gb) {
  BASE(veranFinal_spiderForm_animate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate));  // jp enemyAnimate
}

void veranFinal_spiderForm_state3_hook(GB *gb) {
  BASE(veranFinal_spiderForm_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x8f;  // ld e,Enemy.zh
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); alu_rlca(gb);  // rlca
  CYC(b_+4, b_+6); C = 0x20;  // ld c,$20
  if ((F & FC)) { CYCT(b_+6, b_+9); if (hook_enabled_at(gb, SYM(objectUpdateSpeedZ_paramC))) { objectUpdateSpeedZ_paramC_hook(gb); return; } HANDOFF(SYM(objectUpdateSpeedZ_paramC)); } CYC(b_+6, b_+9);  // jp c,objectUpdateSpeedZ_paramC
  CALL_C(b_+9, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+12);
  if ((F & FZ)) { CYCT(b_+12, b_+14); goto gotoState2; } CYC(b_+12, b_+14);  // jr z,+
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+15, b_+16); alu_rrca(gb);  // rrca
  if ((F & FC)) { RET_TAKEN(b_+16); return; } CYC(b_+16, b_+17);  // ret c
  CYC(b_+17, b_+19); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+20, b_+22); alu_xor(gb, 0x02);  // xor $02
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);  // ld (hl),a
  RET(b_+23); return;  // ret
gotoState2:
  CYC(b_+24, b_+26); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CALL_C(b_+28, objectSetVisible83_hook, SYM(objectSetVisible83), b_+31);
  CALL_C(b_+31, veranFinal_spiderForm_resetCollisionData_hook, SYM(veranFinal_spiderForm_resetCollisionData), b_+34);
  CYC(b_+34, (SYM(veranFinal_spiderForm_state4) + 1)); veranFinal_spiderForm_initState2_hook(gb); return;  // jr veranFinal_spiderForm_initState2
}

void veranFinal_spiderForm_state4_hook(GB *gb) {
  BASE(veranFinal_spiderForm_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x83;  // ld e,Enemy.var03
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+5); E = 0x85;  // ld e,Enemy.substate
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (veranFinal_jump_table(gb));
    if (jt_ == SYM(veranFinal_spiderForm_rushAttack)) { veranFinal_spiderForm_rushAttack_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_jumpAttack)) { veranFinal_spiderForm_jumpAttack_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_webAttack)) { veranFinal_spiderForm_webAttack_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void veranFinal_spiderForm_rushAttack_hook(GB *gb) {
  BASE(veranFinal_spiderForm_rushAttack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+1, b_+2); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+2, b_+4); goto substate0; } CYC(b_+2, b_+4);  // jr z,@substate0
substate1:
  CALL_C(b_+4, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+7);
  if ((F & FZ)) { CYCT(b_+7, b_+9); veranFinal_spiderForm_setCounter2AndInitState2_hook(gb); return; } CYC(b_+7, b_+9);  // jr z,veranFinal_spiderForm_setCounter2AndInitState2
  CALL_C(b_+9, veranFinal_spiderForm_updateMovement_hook, SYM(veranFinal_spiderForm_updateMovement), b_+12);
  CYC(b_+12, b_+15); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate));  // jp enemyAnimate
substate0:
  CALL_C(b_+15, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+18);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);  // inc l
  CYC(b_+19, b_+21); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+21, b_+23); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+23, b_+25); mem_wr(gb, HL, 60);  // ld (hl),SPEED_180
  CALL_C(b_+25, ecom_updateAngleTowardTarget_b10_hook, SYM(ecom_updateAngleTowardTarget_b10), b_+28);
  CYC(b_+28, b_+30); alu_and(gb, 0x18);  // and $18
  CYC(b_+30, b_+32); alu_add(gb, 0x04);  // add $04
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+33, (SYM(veranFinal_spiderForm_jumpAttack) + 1)); veranFinal_spiderForm_animate_hook(gb); return;  // jr veranFinal_spiderForm_animate
}

void veranFinal_spiderForm_jumpAttack_hook(GB *gb) {
  BASE(veranFinal_spiderForm_jumpAttack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (veranFinal_jump_table(gb));
    if (jt_ == b_+12) { goto substate0; }
    else if (jt_ == b_+31) { goto substate1; }
    else if (jt_ == b_+78) { goto substate2; }
    else if (jt_ == b_+114) { goto substate3; }
    else if (jt_ == b_+139) { goto substate4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CYC(b_+12, b_+14); B = 0x56;  // ld b,$56
  CALL_C(b_+14, ecom_spawnProjectile_b10_hook, SYM(ecom_spawnProjectile_b10), b_+17);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; } CYC(b_+17, b_+18);  // ret nz
  CALL_C(b_+18, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+21);
  CYC(b_+21, b_+23); L = 0xb8;  // ld l,Enemy.var38
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CALL_C(b_+25, veranFinal_spiderForm_setVulnerableCollisionData_hook, SYM(veranFinal_spiderForm_setVulnerableCollisionData), b_+28);
  CYC(b_+28, b_+31); if (hook_enabled_at(gb, SYM(objectSetVisible81))) { objectSetVisible81_hook(gb); return; } HANDOFF(SYM(objectSetVisible81));  // jp objectSetVisible81
substate1:
  CYC(b_+31, b_+33); E = 0xb8;  // ld e,Enemy.var38
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+34, b_+35); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(b_+35); return; } CYC(b_+35, b_+36);  // ret z
  CYC(b_+36, b_+37); H = D;  // ld h,d
  CYC(b_+37, b_+39); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+40, b_+42); alu_sub(gb, 0x03);  // sub $03
  CYC(b_+42, b_+43); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+43, b_+45); alu_bit(gb, 7, A);  // bit 7,a
  if ((F & FZ)) { CYCT(b_+45, b_+47); goto zh; } CYC(b_+45, b_+47);  // jr z,+
  CYC(b_+47, b_+49); alu_cp(gb, 0xe0);  // cp $e0
  if (!(F & FC)) { RET_TAKEN(b_+49); return; } CYC(b_+49, b_+50);  // ret nc
  CYC(b_+50, b_+52); A = mem_rd(gb, hCameraY);  // ldh a,(<hCameraY)
  CYC(b_+52, b_+53); B = A;  // ld b,a
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+54, b_+56); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+56, b_+57); alu_add(gb, mem_rd(gb, HL));  // add (hl)
  CYC(b_+57, b_+58); alu_sub(gb, B);  // sub b
  CYC(b_+58, b_+60); alu_cp(gb, 0xb0);  // cp $b0
  if ((F & FC)) { RET_TAKEN(b_+60); return; } CYC(b_+60, b_+61);  // ret c
zh:
  CYC(b_+61, b_+63); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+64, b_+65); L = alu_inc8(gb, L);  // inc l
  CYC(b_+65, b_+67); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(b_+67, b_+69); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+69, b_+71); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(b_+71, b_+73); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+75, b_+78); if (hook_enabled_at(gb, SYM(objectSetInvisible))) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));  // jp objectSetInvisible
substate2:
  CALL_C(b_+78, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+81);
  if (!(F & FZ)) { RET_TAKEN(b_+81); return; } CYC(b_+81, b_+82);  // ret nz
  CYC(b_+82, b_+83); L = E;  // ld l,e
  CYC(b_+83, b_+84); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+84, b_+86); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+86, b_+88); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+88, b_+90); L = 0x94;  // ld l,Enemy.speedZ
  CYC(b_+90, b_+91); alu_xor(gb, A);  // xor a
  CYC(b_+91, b_+92); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(b_+94, b_+96); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+96, b_+98); A = mem_rd(gb, hEnemyTargetY);  // ldh a,(<hEnemyTargetY)
  CYC(b_+98, b_+99); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+99, b_+100); L = alu_inc8(gb, L);  // inc l
  CYC(b_+100, b_+102); A = mem_rd(gb, hEnemyTargetX);  // ldh a,(<hEnemyTargetX)
  CYC(b_+102, b_+103); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+103, b_+105); C = 0x08;  // ld c,$08
  CALL_C(b_+105, ecom_setZAboveScreen_b10_hook, SYM(ecom_setZAboveScreen_b10), b_+108);
  CALL_C(b_+108, veranFinal_spiderForm_resetCollisionData_hook, SYM(veranFinal_spiderForm_resetCollisionData), b_+111);
  CYC(b_+111, b_+114); if (hook_enabled_at(gb, SYM(objectSetVisible81))) { objectSetVisible81_hook(gb); return; } HANDOFF(SYM(objectSetVisible81));  // jp objectSetVisible81
substate3:
  CYC(b_+114, b_+116); C = 0x20;  // ld c,$20
  CALL_C(b_+116, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+119);
  if (!(F & FZ)) { RET_TAKEN(b_+119); return; } CYC(b_+119, b_+120);  // ret nz
  CYC(b_+120, b_+122); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+122, b_+123); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+123, b_+124); L = alu_inc8(gb, L);  // inc l
  CYC(b_+124, b_+126); mem_wr(gb, HL, 120);  // ld (hl),120
  CYC(b_+126, b_+128); A = 0x81;  // ld a,SND_STRONG_POUND
  CALL_C(b_+128, playSound_b00_hook, SYM(playSound_b00), b_+131);
  CYC(b_+131, b_+133); A = 90;  // ld a,90
  CALL_C(b_+133, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+136);
  CYC(b_+136, b_+139); if (hook_enabled_at(gb, SYM(objectSetVisible83))) { objectSetVisible83_hook(gb); return; } HANDOFF(SYM(objectSetVisible83));  // jp objectSetVisible83
substate4:
  CALL_C(b_+139, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+142);
  if (!(F & FZ)) { RET_TAKEN(b_+142); return; } CYC(b_+142, b_+143);  // ret nz
  CYC(b_+143, b_+146); veranFinal_spiderForm_setCounter2AndInitState2_hook(gb); return;  // jp veranFinal_spiderForm_setCounter2AndInitState2
}

void veranFinal_spiderForm_webAttack_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);  // ld a,(de) [substate]
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (veranFinal_jump_table(gb));
    if (jt_ == SYM(veranFinal_spiderForm_webAttack_substate0)) { veranFinal_spiderForm_webAttack_substate0_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_webAttack_substate1)) { veranFinal_spiderForm_webAttack_substate1_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_webAttack_substate2)) { veranFinal_spiderForm_webAttack_substate2_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_webAttack_substate3)) { veranFinal_spiderForm_webAttack_substate3_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_webAttack_substate4)) { veranFinal_spiderForm_webAttack_substate4_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_webAttack_substate5)) { veranFinal_spiderForm_webAttack_substate5_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_webAttack_substate6)) { veranFinal_spiderForm_webAttack_substate6_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_spiderForm_webAttack_substate7)) { veranFinal_spiderForm_webAttack_substate7_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void veranFinal_spiderForm_webAttack_substate0_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack_substate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+2); L = E;  // ld l,e
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+3, b_+4); L = alu_inc8(gb, L);  // inc l
  CYC(b_+4, b_+6); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+6, b_+8); L = 0xb8;  // ld l,Enemy.var38
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  veranFinal_spiderForm_resetCollisionData_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_resetCollisionData_hook(GB *gb) {
  BASE(veranFinal_spiderForm_resetCollisionData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xa5;  // ld l,Enemy.enemyCollisionMode
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x6c);  // ld (hl),ENEMYCOLLISION_VERAN_SPIDER_FORM
  CYC(b_+5, b_+7); L = 0xa6;  // ld l,Enemy.collisionRadiusY
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x10);  // ld (hl),$10
  CYC(b_+9, b_+11); A = 0x05;  // ld a,$05
  CYC(b_+11, b_+14); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void veranFinal_spiderForm_webAttack_substate1_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack_substate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);  // inc l
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+7, b_+8); L = E;  // ld l,e
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  veranFinal_spiderForm_setVulnerableCollisionData_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_setVulnerableCollisionData_hook(GB *gb) {
  BASE(veranFinal_spiderForm_setVulnerableCollisionData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xa5;  // ld l,Enemy.enemyCollisionMode
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x6b);  // ld (hl),ENEMYCOLLISION_VERAN_SPIDER_FORM_VULNERABLE
  CYC(b_+5, b_+7); L = 0xa6;  // ld l,Enemy.collisionRadiusY
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+9, b_+11); A = 0x06;  // ld a,$06
  CYC(b_+11, b_+14); if (hook_enabled_at(gb, SYM(enemySetAnimation))) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void veranFinal_spiderForm_webAttack_substate2_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack_substate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); B = 0x56;  // ld b,$56
  CALL_C(b_+6, ecom_spawnProjectile_b10_hook, SYM(ecom_spawnProjectile_b10), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } CYC(b_+9, b_+10);  // ret nz
  CYC(b_+10, b_+12); L = 0xc2;  // ld l,Part.subid
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CALL_C(b_+13, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+16);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);  // inc l
  CYC(b_+17, b_+19); mem_wr(gb, HL, 90);  // ld (hl),90
  CYC(b_+19, (SYM(veranFinal_spiderForm_webAttack_substate3) + 1)); veranFinal_spiderForm_resetCollisionData_hook(gb); return;  // jr veranFinal_spiderForm_resetCollisionData
}

void veranFinal_spiderForm_webAttack_substate3_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack_substate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xb8;  // ld e,Enemy.var38
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(b_+4); return; } CYC(b_+4, b_+5);  // ret z
  CALL_C(b_+5, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } CYC(b_+8, b_+9);  // ret nz
  CYC(b_+9, b_+12); A = mem_rd(gb, w1Link_state);  // ld a,(w1Link.state)
  CYC(b_+12, b_+14); alu_cp(gb, 0x0d);  // cp LINK_STATE_GRABBED
  if (!(F & FZ)) { CYCT(b_+14, b_+17); veranFinal_spiderForm_setCounter2AndInitState2_hook(gb); return; } CYC(b_+14, b_+17);  // jp nz,veranFinal_spiderForm_setCounter2AndInitState2
  CALL_C(b_+17, ecom_incSubstate_b10_hook, SYM(ecom_incSubstate_b10), b_+20);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);  // inc l
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x10);  // ld (hl),$10
  CYC(b_+23, b_+25); A = 0x06;  // ld a,$06
  CALL_C(b_+25, enemySetAnimation_hook, SYM(enemySetAnimation), b_+28);
  CYC(b_+28, b_+30); B = 0xf8;  // ld b,$f8
  veranFinal_spiderForm_webAttack_updateLinkPosition_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_webAttack_updateLinkPosition_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack_updateLinkPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link);  // ld hl,w1Link
  CYC(b_+3, b_+5); C = 0x00;  // ld c,$00
  CYC(b_+5, b_+8); if (hook_enabled_at(gb, SYM(objectCopyPositionWithOffset))) { objectCopyPositionWithOffset_hook(gb); return; } HANDOFF(SYM(objectCopyPositionWithOffset));  // jp objectCopyPositionWithOffset
}

void veranFinal_spiderForm_webAttack_substate4_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack_substate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x04);  // ld (hl),$04
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0x05;  // ld a,$05
  CALL_C(b_+10, enemySetAnimation_hook, SYM(enemySetAnimation), b_+13);
  CYC(b_+13, b_+15); A = 0x04;  // ld a,$04
  CALL_C(b_+15, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+18);
  CYC(b_+18, b_+20); B = 0x14;  // ld b,$14
  CALL_C(b_+20, veranFinal_spiderForm_webAttack_updateLinkPosition_hook, SYM(veranFinal_spiderForm_webAttack_updateLinkPosition), b_+23);
  CYC(b_+23, b_+26); SET_BC(0xfa08);  // ld bc,-6,$08
  veranFinal_spiderForm_webAttack_applyDamageToLink_hook(gb); return;  // fallthrough
}

void veranFinal_spiderForm_webAttack_applyDamageToLink_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack_applyDamageToLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0x25;  // ld l,<w1Link.damageToApply
  CYC(b_+2, b_+3); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(b_+3, b_+5); L = 0x2b;  // ld l,<w1Link.invincibilityCounter
  CYC(b_+5, b_+6); mem_wr(gb, HL, C);  // ld (hl),c
  CYC(b_+6, b_+8); A = 0x81;  // ld a,SND_STRONG_POUND
  CYC(b_+8, b_+11); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
}

void veranFinal_spiderForm_webAttack_substate5_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack_substate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0x06;  // ld a,$06
  CALL_C(b_+10, enemySetAnimation_hook, SYM(enemySetAnimation), b_+13);
  CYC(b_+13, b_+15); B = 0xf6;  // ld b,$f6
  CYC(b_+15, b_+17); veranFinal_spiderForm_webAttack_updateLinkPosition_hook(gb); return;  // jr veranFinal_spiderForm_webAttack_updateLinkPosition
}

void veranFinal_spiderForm_webAttack_substate6_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack_substate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x0f);  // ld (hl),$0f
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); A = 0x05;  // ld a,$05
  CALL_C(b_+10, enemySetAnimation_hook, SYM(enemySetAnimation), b_+13);
  CYC(b_+13, b_+15); A = 0x14;  // ld a,$14
  CALL_C(b_+15, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+18);
  CYC(b_+18, b_+20); B = 0x14;  // ld b,$14
  CALL_C(b_+20, veranFinal_spiderForm_webAttack_updateLinkPosition_hook, SYM(veranFinal_spiderForm_webAttack_updateLinkPosition), b_+23);
  CYC(b_+23, b_+26); SET_BC(0xf618);  // ld bc,-10,$18
  CYC(b_+26, (SYM(veranFinal_spiderForm_webAttack_substate7) + 1)); veranFinal_spiderForm_webAttack_applyDamageToLink_hook(gb); return;  // jr veranFinal_spiderForm_webAttack_applyDamageToLink
}

void veranFinal_spiderForm_webAttack_substate7_hook(GB *gb) {
  BASE(veranFinal_spiderForm_webAttack_substate7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(b_+8, veranFinal_spiderForm_setCounter2AndInitState2_hook, SYM(veranFinal_spiderForm_setCounter2AndInitState2), SYM(veranFinal_grabbingLink));
  veranFinal_grabbingLink_hook(gb); return;  // fallthrough
}

void veranFinal_grabbingLink_hook(GB *gb) {
  BASE(veranFinal_grabbingLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_substate);  // ld hl,w1Link.substate
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x02);  // ld (hl),$02
  CYC(b_+5, b_+7); L = 0x24;  // ld l,<w1Link.collisionType
  CYC(b_+7, b_+9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  RET(b_+9); return;  // ret
}

void veranFinal_beeForm_hook(GB *gb) {
  BASE(veranFinal_beeForm);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (veranFinal_jump_table(gb));
    if (jt_ == SYM(veranFinal_spiderOrBeeForm_state0)) { veranFinal_spiderOrBeeForm_state0_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_state1)) { veranFinal_beeForm_state1_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_state2)) { veranFinal_beeForm_state2_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_state3)) { veranFinal_beeForm_state3_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_state4)) { veranFinal_beeForm_state4_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_state5)) { veranFinal_beeForm_state5_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_state6)) { veranFinal_beeForm_state6_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_state7)) { veranFinal_beeForm_state7_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_state8)) { veranFinal_beeForm_state8_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_state9)) { veranFinal_beeForm_state9_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_stateA)) { veranFinal_beeForm_stateA_hook(gb); return; }
    else if (jt_ == SYM(veranFinal_beeForm_stateB)) { veranFinal_beeForm_stateB_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void veranFinal_beeForm_state1_hook(GB *gb) {
  BASE(veranFinal_beeForm_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } CYC(b_+7, b_+8);  // ret nz
  CYC(b_+8, b_+10); A = 0x07;  // ld a,$07
  CALL_C(b_+10, enemySetAnimation_hook, SYM(enemySetAnimation), b_+13);
  CALL_C(b_+13, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+16);
  CYC(b_+16, b_+18); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x50);  // ld (hl),SPEED_200
  CYC(b_+20, b_+23); SET_BC(0x100c);  // ld bc,$100c
  CYC(b_+23, b_+25); E = 0x6b;  // ld e,ENEMYCOLLISION_VERAN_SPIDER_FORM_VULNERABLE
  CYC(b_+25, b_+27); L = 0xb2;  // ld l,Enemy.var32
  veranFinal_initializeForm_hook(gb); return;  // fallthrough
}

void veranFinal_initializeForm_hook(GB *gb) {
  BASE(veranFinal_initializeForm);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+2); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+2, b_+4); L = 0xa9;  // ld l,Enemy.health
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+5, b_+7); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x82);  // ld (hl),$80|ENEMY_VERAN_FINAL_FORM
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);  // inc l
  CYC(b_+10, b_+11); mem_wr(gb, HL, E);  // ld (hl),e
  CYC(b_+11, b_+13); L = 0xa6;  // ld l,Enemy.collisionRadiusY
  CYC(b_+13, b_+14); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);  // inc l
  CYC(b_+15, b_+16); mem_wr(gb, HL, C);  // ld (hl),c
  CYC(b_+16, b_+18); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+18, b_+20); A = 0x06;  // ld a,$06
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);  // ld (hl),a
  RET(b_+22); return;  // ret
}

void veranFinal_beeForm_state2_hook(GB *gb) {
  BASE(veranFinal_beeForm_state2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x8b;  // ld e,Enemy.yh
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+5); mem_wr(gb, hFF8F, A);  // ldh (<hFF8F),a
  CYC(b_+5, b_+7); E = 0x8d;  // ld e,Enemy.xh
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+8, b_+10); mem_wr(gb, hFF8E, A);  // ldh (<hFF8E),a
  CYC(b_+10, b_+13); SET_BC((SYM(enemyCode02__normalStatus) + 10));  // ldbc LARGE_ROOM_HEIGHT<<3,LARGE_ROOM_WIDTH<<3
  CYC(b_+13, b_+14); alu_sub(gb, C);  // sub c
  CYC(b_+14, b_+16); alu_add(gb, 0x02);  // add $02
  CYC(b_+16, b_+18); alu_cp(gb, 0x05);  // cp $05
  if (!(F & FC)) { CYCT(b_+18, b_+20); goto updateMovement; } CYC(b_+18, b_+20);  // jr nc,@updateMovement
  CYC(b_+20, b_+22); A = mem_rd(gb, hFF8F);  // ldh a,(<hFF8F)
  CYC(b_+22, b_+23); alu_sub(gb, B);  // sub b
  CYC(b_+23, b_+25); alu_add(gb, 0x02);  // add $02
  CYC(b_+25, b_+27); alu_cp(gb, 0x05);  // cp $05
  if (!(F & FC)) { CYCT(b_+27, b_+29); goto updateMovement; } CYC(b_+27, b_+29);  // jr nc,@updateMovement
  CALL_C(b_+29, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+32);
  CYC(b_+32, b_+35); veranFinal_beeForm_chooseRandomTargetPosition_hook(gb); return;  // jp veranFinal_beeForm_chooseRandomTargetPosition
updateMovement:
  CALL_C(b_+35, ecom_moveTowardPosition_b10_hook, SYM(ecom_moveTowardPosition_b10), SYM(veranFinal_beeForm_animate));
  veranFinal_beeForm_animate_hook(gb); return;  // fallthrough
}

void veranFinal_beeForm_animate_hook(GB *gb) {
  BASE(veranFinal_beeForm_animate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate));  // jp enemyAnimate
}

void veranFinal_beeForm_state3_hook(GB *gb) {
  BASE(veranFinal_beeForm_state3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, veranFinal_moveTowardTargetPosition_hook, SYM(veranFinal_moveTowardTargetPosition), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+5); veranFinal_beeForm_animate_hook(gb); return; } CYC(b_+3, b_+5);  // jr nc,veranFinal_beeForm_animate
  CYC(b_+5, b_+7); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+7, b_+8); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(b_+8, b_+10); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+10, b_+11); mem_wr(gb, HL, C);  // ld (hl),c
  CALL_C(b_+11, veranFinal_beeForm_nextTargetPosition_hook, SYM(veranFinal_beeForm_nextTargetPosition), b_+14);
  CALL_C(b_+14, ecom_decCounter2_b10_hook, SYM(ecom_decCounter2_b10), b_+17);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); veranFinal_beeForm_animate_hook(gb); return; } CYC(b_+17, b_+19);  // jr nz,veranFinal_beeForm_animate
  CYC(b_+19, b_+21); L = 0x84;  // ld l,Enemy.state
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+22, b_+24); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x01);  // ld (hl),$01
  CYC(b_+26, b_+28); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+28, b_+30); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  CYC(b_+30, b_+32); A = 0x00;  // ld a,$00
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto setVar37; } CYC(b_+32, b_+34);  // jr nz,+
  CYC(b_+34, b_+36); A = 0xf0;  // ld a,$f0
setVar37:
  CYC(b_+36, b_+38); L = 0xb7;  // ld l,Enemy.var37
  CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL - 1);  // ldd (hl),a
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0xe0);  // ld (hl),$e0
  CYC(b_+41, b_+43); veranFinal_beeForm_animate_hook(gb); return;  // jr veranFinal_beeForm_animate
}

void veranFinal_beeForm_state4_hook(GB *gb) {
  BASE(veranFinal_beeForm_state4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto applySpeed; } CYC(b_+3, b_+5);  // jr nz,+
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x06);  // ld (hl),$06
  CYC(b_+7, b_+9); L = 0xb6;  // ld l,Enemy.var36
  CALL_C(b_+9, ecom_readPositionVars_b10_hook, SYM(ecom_readPositionVars_b10), b_+12);
  CALL_C(b_+12, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+15);
  CALL_C(b_+15, objectNudgeAngleTowards_hook, SYM(objectNudgeAngleTowards), b_+18);
applySpeed:
  CALL_C(b_+18, objectApplySpeed_hook, SYM(objectApplySpeed), b_+21);
  CYC(b_+21, b_+23); E = 0x8b;  // ld e,Enemy.yh
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+24, b_+26); alu_cp(gb, 0xc0);  // cp (LARGE_ROOM_HEIGHT+1)<<4
  if ((F & FC)) { CYCT(b_+26, b_+28); veranFinal_beeForm_animate_hook(gb); return; } CYC(b_+26, b_+28);  // jr c,veranFinal_beeForm_animate
  CALL_C(b_+28, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+31);
  CYC(b_+31, b_+33); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+33, b_+35); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+35, b_+38); if (hook_enabled_at(gb, SYM(objectSetInvisible))) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));  // jp objectSetInvisible
}

void veranFinal_beeForm_state5_hook(GB *gb) {
  BASE(veranFinal_beeForm_state5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CYC(b_+4, b_+6); mem_wr(gb, HL, 15);  // ld (hl),15
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+8, b_+10); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x20);  // ld (hl),$20
  CALL_C(b_+12, getRandomNumber_hook, SYM(getRandomNumber), b_+15);
  CYC(b_+15, b_+17); alu_and(gb, 0x10);  // and $10
  CYC(b_+17, b_+20); SET_BC(0x08e8);  // ldbc $08,$e8
  if ((F & FZ)) { CYCT(b_+20, b_+22); goto haveAngle; } CYC(b_+20, b_+22);  // jr z,++
  CYC(b_+22, b_+23); B = C;  // ld b,c
  CYC(b_+23, b_+25); C = 0x08;  // ld c,$08
haveAngle:
  CYC(b_+25, b_+27); alu_add(gb, 0x08);  // add $08
  CYC(b_+27, b_+29); L = 0x89;  // ld l,Enemy.angle
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+30, b_+32); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+32, b_+33); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(b_+33, b_+35); L = 0xb7;  // ld l,Enemy.var37
  CYC(b_+35, b_+36); mem_wr(gb, HL, C);  // ld (hl),c
  CYC(b_+36, b_+39); if (hook_enabled_at(gb, SYM(objectSetVisible83))) { objectSetVisible83_hook(gb); return; } HANDOFF(SYM(objectSetVisible83));  // jp objectSetVisible83
}

void veranFinal_beeForm_state6_hook(GB *gb) {
  BASE(veranFinal_beeForm_state6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto applySpeed; } CYC(b_+3, b_+5);  // jr nz,+
  CYC(b_+5, b_+7); mem_wr(gb, HL, 15);  // ld (hl),15
  CYC(b_+7, b_+9); B = 0x58;  // ld b,PART_VERAN_BEE_PROJECTILE
  CALL_C(b_+9, ecom_spawnProjectile_b10_hook, SYM(ecom_spawnProjectile_b10), b_+12);
applySpeed:
  CALL_C(b_+12, objectApplySpeed_hook, SYM(objectApplySpeed), b_+15);
  CYC(b_+15, b_+17); E = 0x8d;  // ld e,Enemy.xh
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+18, b_+19); H = D;  // ld h,d
  CYC(b_+19, b_+21); L = 0xb7;  // ld l,Enemy.var37
  CYC(b_+21, b_+22); alu_sub(gb, mem_rd(gb, HL));  // sub (hl)
  CYC(b_+22, b_+23); A = alu_inc8(gb, A);  // inc a
  CYC(b_+23, b_+25); alu_cp(gb, 0x03);  // cp $03
  if (!(F & FC)) { CYCT(b_+25, b_+28); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+25, b_+28);  // jp nc,enemyAnimate
  CALL_C(b_+28, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+31);
  CYC(b_+31, b_+33); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+33, b_+35); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+35, b_+37); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+37, b_+39); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(b_+39, b_+42); if (hook_enabled_at(gb, SYM(objectSetInvisible))) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));  // jp objectSetInvisible
}

void veranFinal_beeForm_state7_hook(GB *gb) {
  BASE(veranFinal_beeForm_state7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nz
  CALL_C(b_+4, veranFinal_getQuadrant_hook, SYM(veranFinal_getQuadrant), b_+7);
chooseQuadrant:
  CALL_C(b_+7, getRandomNumber_hook, SYM(getRandomNumber), b_+10);
  CYC(b_+10, b_+11); C = A;  // ld c,a
  CYC(b_+11, b_+13); alu_and(gb, 0x03);  // and $03
  CYC(b_+13, b_+14); alu_cp(gb, B);  // cp b
  if ((F & FZ)) { CYCT(b_+14, b_+16); goto chooseQuadrant; } CYC(b_+14, b_+16);  // jr z,-
  CYC(b_+16, b_+18); E = 0xb9;  // ld e,Enemy.var39
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+19, b_+20); alu_add(gb, A);  // add a
  CYC(b_+20, b_+23); SET_HL(SYM(veranFinal_beeForm_screenCornerEntrances));  // ld hl,veranFinal_beeForm_screenCornerEntrances
  CYC(b_+23, b_+24);
  veranFinal_addDoubleIndexToHl_from_rst(gb, b_+24);  // rst_addDoubleIndexToHl
  CYC(b_+24, b_+26); E = 0xb6;  // ld e,Enemy.var36
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+28, b_+29); E = alu_inc8(gb, E);  // inc e
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+31, b_+33); E = 0x8b;  // ld e,Enemy.yh
  CYC(b_+33, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+35, b_+37); E = 0x8d;  // ld e,Enemy.xh
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+39, b_+40); A = C;  // ld a,c
  CYC(b_+40, b_+42); alu_and(gb, 0x30);  // and $30
  CYC(b_+42, b_+44); A = alu_swap(gb, A);  // swap a
  CYC(b_+44, b_+46); alu_add(gb, 0x02);  // add $02
  CYC(b_+46, b_+48); E = 0x87;  // ld e,Enemy.counter2
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(b_+49, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+52);
  CYC(b_+52, b_+54); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+54, b_+56); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+56, b_+59); if (hook_enabled_at(gb, SYM(objectSetVisible83))) { objectSetVisible83_hook(gb); return; } HANDOFF(SYM(objectSetVisible83));  // jp objectSetVisible83
}

void veranFinal_beeForm_state8_hook(GB *gb) {
  BASE(veranFinal_beeForm_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, veranFinal_moveTowardTargetPosition_hook, SYM(veranFinal_moveTowardTargetPosition), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+5); veranFinal_beeForm_animate2_hook(gb); return; } CYC(b_+3, b_+5);  // jr nc,veranFinal_beeForm_animate2
  CYC(b_+5, b_+7); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+7, b_+8); mem_wr(gb, HL, B);  // ld (hl),b
  CYC(b_+8, b_+10); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+10, b_+11); mem_wr(gb, HL, C);  // ld (hl),c
  CYC(b_+11, b_+13); L = 0x84;  // ld l,Enemy.state
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+14, b_+16); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+16, b_+18); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+18, b_+20); veranFinal_beeForm_animate2_hook(gb); return;  // jr veranFinal_beeForm_animate2
}

void veranFinal_beeForm_state9_hook(GB *gb) {
  BASE(veranFinal_beeForm_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); veranFinal_beeForm_animate2_hook(gb); return; } CYC(b_+3, b_+5);  // jr nz,veranFinal_beeForm_animate2
  CYC(b_+5, b_+7); mem_wr(gb, HL, 25);  // ld (hl),25
  CYC(b_+7, b_+8); L = E;  // ld l,e
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  veranFinal_beeForm_animate2_hook(gb); return;  // fallthrough
}

void veranFinal_beeForm_animate2_hook(GB *gb) {
  BASE(veranFinal_beeForm_animate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); if (hook_enabled_at(gb, SYM(enemyAnimate))) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate));  // jp enemyAnimate
}

void veranFinal_beeForm_stateA_hook(GB *gb) {
  BASE(veranFinal_beeForm_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if ((F & FZ)) { CYCT(b_+3, b_+5); label_10_173_hook(gb); return; } CYC(b_+3, b_+5);  // jr z,label_10_173
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+6, b_+8); alu_and(gb, 0x07);  // and $07
  if (!(F & FZ)) { CYCT(b_+8, b_+10); veranFinal_beeForm_animate2_hook(gb); return; } CYC(b_+8, b_+10);  // jr nz,veranFinal_beeForm_animate2
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+11, b_+13); alu_and(gb, 0x18);  // and $18
  CYC(b_+13, b_+15); A = alu_swap(gb, A);  // swap a
  CYC(b_+15, b_+16); alu_rlca(gb);  // rlca
  CYC(b_+16, b_+17); A = alu_dec8(gb, A);  // dec a
  CYC(b_+17, b_+18); B = A;  // ld b,a
  CALL_C(b_+18, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+21);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); veranFinal_beeForm_animate2_hook(gb); return; } CYC(b_+21, b_+23);  // jr nz,veranFinal_beeForm_animate2
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x1f);  // ld (hl),ENEMY_VERAN_CHILD_BEE
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);  // inc l
  CYC(b_+26, b_+27); mem_wr(gb, HL, B);  // ld (hl),b [child.subid]
  CALL_C(b_+27, objectCopyPosition_hook, SYM(objectCopyPosition), b_+30);
  CYC(b_+30, b_+32); A = 0xba;  // ld a,SND_BEAM1
  CALL_C(b_+32, playSound_b00_hook, SYM(playSound_b00), b_+35);
  CYC(b_+35, b_+37); veranFinal_beeForm_animate2_hook(gb); return;  // jr veranFinal_beeForm_animate2
}

void label_10_173_hook(GB *gb) {
  BASE(label_10_173);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 20);  // ld (hl),20
  CYC(b_+2, b_+3); L = alu_inc8(gb, L);  // inc l
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl) [counter2]
  CYC(b_+4, b_+5); L = E;  // ld l,e
  if ((F & FZ)) { CYCT(b_+5, b_+7); goto setState02; } CYC(b_+5, b_+7);  // jr z,+
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl) [state]=$0b
  CYC(b_+8, b_+11); veranFinal_beeForm_animate2_hook(gb); return;  // jr veranFinal_beeForm_animate2
setState02:
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x02);  // ld (hl),$02 [state]
  CYC(b_+12, (SYM(veranFinal_beeForm_stateB) + 1)); veranFinal_beeForm_animate2_hook(gb); return;  // jr veranFinal_beeForm_animate2
}

void veranFinal_beeForm_stateB_hook(GB *gb) {
  BASE(veranFinal_beeForm_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); veranFinal_beeForm_animate2_hook(gb); return; } CYC(b_+3, b_+5);  // jr nz,veranFinal_beeForm_animate2
  CYC(b_+5, b_+6); L = E;  // ld l,e
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x08);  // ld (hl),$08 [state]
  CALL_C(b_+8, veranFinal_getQuadrant_hook, SYM(veranFinal_getQuadrant), b_+11);
chooseQuadrant:
  CALL_C(b_+11, getRandomNumber_hook, SYM(getRandomNumber), b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0x03);  // and $03
  CYC(b_+16, b_+17); alu_cp(gb, B);  // cp b
  if ((F & FZ)) { CYCT(b_+17, b_+19); goto chooseQuadrant; } CYC(b_+17, b_+19);  // jr z,@chooseQuadrant
  CYC(b_+19, b_+20); H = D;  // ld h,d
  CYC(b_+20, b_+22); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+22, b_+23); alu_cp(gb, mem_rd(gb, HL));  // cp (hl)
  if ((F & FZ)) { CYCT(b_+23, b_+25); goto chooseQuadrant; } CYC(b_+23, b_+25);  // jr z,@chooseQuadrant
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);  // ld (hl),a [var39]
  CYC(b_+26, b_+27); alu_add(gb, A);  // add a
  CYC(b_+27, b_+30); SET_HL(SYM(veranFinal_beeForm_screenCornerEntrances));  // ld hl,veranFinal_beeForm_screenCornerEntrances
  CYC(b_+30, b_+31);
  veranFinal_addDoubleIndexToHl_from_rst(gb, b_+31);  // rst_addDoubleIndexToHl
  CYC(b_+31, b_+33); E = 0xb6;  // ld e,Enemy.var36
  CYC(b_+33, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+35, b_+36); E = alu_inc8(gb, E);  // inc e
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+38, (SYM(veranFinal_beeForm_screenCornerEntrances) + 1)); veranFinal_beeForm_animate2_hook(gb); return;  // jr veranFinal_beeForm_animate2
}

void veranFinal_transformToBeeOrSpider_hook(GB *gb) {
  BASE(veranFinal_transformToBeeOrSpider);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x01);  // ld (hl),$01 [state]
  CYC(b_+2, b_+4); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x96);  // ld (hl),$80|ENEMY_BEAMOS
  CYC(b_+6, b_+8); L = 0xa9;  // ld l,Enemy.health
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+9, b_+11); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+12, b_+14); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+14, b_+16); A = 0x07;  // ld a,$07
  CYC(b_+16, b_+17); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);  // ld (hl),a
  CALL_C(b_+18, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+21);
  CYC(b_+21, b_+23); alu_and(gb, 0x03);  // and $03
  CYC(b_+23, b_+24); B = A;  // ld b,a
  CYC(b_+24, b_+26); E = 0xb5;  // ld e,Enemy.var35
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+27, b_+28); C = A;  // ld c,a
  CYC(b_+28, b_+29); A = alu_inc8(gb, A);  // inc a
  CYC(b_+29, b_+31); alu_and(gb, 0x07);  // and $07
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+32, b_+33); A = C;  // ld a,c
  CYC(b_+33, b_+34); alu_add(gb, A);  // add a
  CYC(b_+34, b_+35); alu_add(gb, A);  // add a
  CYC(b_+35, b_+36); alu_add(gb, B);  // add b
  CYC(b_+36, b_+39); SET_HL(b_+60);  // ld hl,@transformSequence
  CALL_C(b_+39, checkFlag_hook, SYM(checkFlag), b_+42);
  if ((F & FZ)) { CYCT(b_+42, b_+44); goto haveSubid; } CYC(b_+42, b_+44);  // jr z,+
  CYC(b_+44, b_+46); A = 0x01;  // ld a,$01
haveSubid:
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);  // inc a
  CYC(b_+47, b_+49); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+50, b_+52); alu_add(gb, 0x09);  // add $09
  CALL_C(b_+52, enemySetAnimation_hook, SYM(enemySetAnimation), b_+55);
  CYC(b_+55, b_+57); A = 0xc0;  // ld a,SND_TRANSFORM
  CYC(b_+57, b_+60); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
}

void veranFinal_spiderForm_checkLinkWithinDistance_hook(GB *gb) {
  BASE(veranFinal_spiderForm_checkLinkWithinDistance);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;  // ld a,b
  CYC(b_+1, b_+2); alu_add(gb, A);  // add a
  CYC(b_+2, b_+3); A = alu_inc8(gb, A);  // inc a
  CYC(b_+3, b_+4); C = A;  // ld c,a
  CYC(b_+4, b_+7); A = mem_rd(gb, w1Link_yh);  // ld a,(w1Link.yh)
  CYC(b_+7, b_+8); H = D;  // ld h,d
  CYC(b_+8, b_+10); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+10, b_+11); alu_sub(gb, mem_rd(gb, HL));  // sub (hl)
  CYC(b_+11, b_+12); alu_add(gb, B);  // add b
  CYC(b_+12, b_+13); alu_cp(gb, C);  // cp c
  if (!(F & FC)) { RET_TAKEN(b_+13); return; } CYC(b_+13, b_+14);  // ret nc
  CYC(b_+14, b_+17); A = mem_rd(gb, w1Link_xh);  // ld a,(w1Link.xh)
  CYC(b_+17, b_+19); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+19, b_+20); alu_sub(gb, mem_rd(gb, HL));  // sub (hl)
  CYC(b_+20, b_+21); alu_add(gb, B);  // add b
  CYC(b_+21, b_+22); alu_cp(gb, C);  // cp c
  RET(b_+22); return;  // ret
}

void veranFinal_spiderForm_decideWhetherToAttack_hook(GB *gb) {
  BASE(veranFinal_spiderForm_decideWhetherToAttack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+3);
  CYC(b_+3, b_+4); E = A;  // ld e,a
considerRushAttack:
  CYC(b_+4, b_+6); B = 0x60;  // ld b,$60
  CALL_C(b_+6, veranFinal_spiderForm_checkLinkWithinDistance_hook, SYM(veranFinal_spiderForm_checkLinkWithinDistance), b_+9);
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto considerJumpAttack; } CYC(b_+9, b_+11);  // jr nc,@considerJumpAttack
  CYC(b_+11, b_+13); alu_and(gb, 0x07);  // and $07
  CYC(b_+13, b_+15); alu_sub(gb, 0x03);  // sub $03
  CYC(b_+15, b_+17); alu_cp(gb, 0x03);  // cp $03
  CYC(b_+17, b_+19); A = 0x00;  // ld a,$00
  if ((F & FC)) { CYCT(b_+19, b_+21); goto doAttack; } CYC(b_+19, b_+21);  // jr c,@doAttack
considerJumpAttack:
  CYC(b_+21, b_+23); B = 0x50;  // ld b,$50
  CALL_C(b_+23, veranFinal_spiderForm_checkLinkWithinDistance_hook, SYM(veranFinal_spiderForm_checkLinkWithinDistance), b_+26);
  if ((F & FC)) { CYCT(b_+26, b_+28); goto considerGrabAttack; } CYC(b_+26, b_+28);  // jr c,@considerGrabAttack
  CYC(b_+28, b_+29); A = E;  // ld a,e
  CYC(b_+29, b_+31); alu_and(gb, 0x07);  // and $07
  CYC(b_+31, b_+33); alu_sub(gb, 0x03);  // sub $03
  CYC(b_+33, b_+35); alu_cp(gb, 0x03);  // cp $03
  CYC(b_+35, b_+36); alu_ccf(gb);  // ccf
  CYC(b_+36, b_+38); A = 0x01;  // ld a,$01
  if ((F & FC)) { CYCT(b_+38, b_+40); goto doAttack; } CYC(b_+38, b_+40);  // jr c,@doAttack
considerGrabAttack:
  CYC(b_+40, b_+41); A = E;  // ld a,e
  CYC(b_+41, b_+43); alu_sub(gb, 0x0c);  // sub $0c
  CYC(b_+43, b_+45); alu_cp(gb, 0x09);  // cp $09
  if (!(F & FC)) { RET_TAKEN(b_+45); return; } CYC(b_+45, b_+46);  // ret nc
  CYC(b_+46, b_+48); A = 0x02;  // ld a,$02
doAttack:
  CYC(b_+48, b_+50); E = 0x83;  // ld e,Enemy.var03
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+51, b_+52); H = D;  // ld h,d
  CYC(b_+52, b_+54); L = 0x84;  // ld l,Enemy.state
  CYC(b_+54, b_+56); mem_wr(gb, HL, 0x04);  // ld (hl),$04
  CYC(b_+56, b_+57); L = alu_inc8(gb, L);  // inc l
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x00);  // ld (hl),$00 [substate]
  CYC(b_+59, b_+60); alu_scf(gb);  // scf
  RET(b_+60); return;  // ret
}

void veranFinal_dead_hook(GB *gb) {
  BASE(veranFinal_dead);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto transformed; } CYC(b_+4, b_+6);  // jr nz,@transformed
  CYC(b_+6, b_+7); H = D;  // ld h,d
  CYC(b_+7, b_+9); L = 0x84;  // ld l,Enemy.state
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x0a);  // ld (hl),$0a
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);  // inc l
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+14, b_+16); L = 0xa9;  // ld l,Enemy.health
  CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+17, b_+19); A = 0xf0;  // ld a,SNDCTRL_STOPMUSIC
  CYC(b_+19, b_+22); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
transformed:
  CYC(b_+22, b_+23); B = A;  // ld b,a
  CYC(b_+23, b_+24); H = D;  // ld h,d
  CYC(b_+24, b_+25); L = E;  // ld l,e
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x00);  // ld (hl),$00 [subid]
  CYC(b_+27, b_+29); L = 0x84;  // ld l,Enemy.state
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0x09);  // ld (hl),$09
  CYC(b_+31, b_+33); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+34, b_+36); L = 0xa9;  // ld l,Enemy.health
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+37, b_+39); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x96);  // ld (hl),$80|ENEMY_BEAMOS
  CYC(b_+41, b_+43); L = 0xa6;  // ld l,Enemy.collisionRadiusY
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+45, b_+46); L = alu_inc8(gb, L);  // inc l
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x0a);  // ld (hl),$0a [collisionRadiusX]
  CYC(b_+48, b_+50); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+50, b_+52); A = 0x07;  // ld a,$07
  CYC(b_+52, b_+53); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+53, b_+54); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+54, b_+55); A = B;  // ld a,b [subid]
  CYC(b_+55, b_+57); alu_add(gb, 0x07);  // add $07
  CALL_C(b_+57, enemySetAnimation_hook, SYM(enemySetAnimation), b_+60);
  CYC(b_+60, b_+62); A = 0xc0;  // ld a,SND_TRANSFORM
  CYC(b_+62, b_+65); if (hook_enabled_at(gb, SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));  // jp playSound
}

void veranFinal_spiderForm_decideAngle_hook(GB *gb) {
  BASE(veranFinal_spiderForm_decideAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00;  // ld b,$00
  CYC(b_+2, b_+4); E = 0x8b;  // ld e,Enemy.yh
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+5, b_+7); alu_cp(gb, 0x58);  // cp (LARGE_ROOM_HEIGHT<<4)/2
  if ((F & FC)) { CYCT(b_+7, b_+9); goto haveB1; } CYC(b_+7, b_+9);  // jr c,+
  CYC(b_+9, b_+11); B = 0x10;  // ld b,$10
haveB1:
  CYC(b_+11, b_+13); E = 0x8d;  // ld e,Enemy.xh
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+14, b_+16); alu_cp(gb, 0x78);  // cp (LARGE_ROOM_WIDTH<<4)/2
  if ((F & FC)) { CYCT(b_+16, b_+18); goto haveB2; } CYC(b_+16, b_+18);  // jr c,+
  CYC(b_+18, b_+20); B = (uint8_t)(B | (1 << 3));  // set 3,b
haveB2:
  CALL_C(b_+20, getRandomNumber_hook, SYM(getRandomNumber), b_+23);
  CYC(b_+23, b_+25); alu_and(gb, 0x07);  // and $07
  CYC(b_+25, b_+26); alu_add(gb, B);  // add b
  CYC(b_+26, b_+29); SET_HL(b_+35);  // ld hl,@angles
  CYC(b_+29, b_+30);
  veranFinal_addAToHl_from_rst(gb, b_+30);  // rst_addAToHl
  CYC(b_+30, b_+32); E = 0x89;  // ld e,Enemy.angle
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);  // ld (de),a
  RET(b_+34); return;  // ret
}

void veranFinal_beeForm_chooseRandomTargetPosition_hook(GB *gb) {
  BASE(veranFinal_beeForm_chooseRandomTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0801);  // ld bc,$0801
  CALL_C(b_+3, ecom_randomBitwiseAndBCE_b10_hook, SYM(ecom_randomBitwiseAndBCE_b10), b_+6);
  CYC(b_+6, b_+8); E = 0x86;  // ld e,Enemy.counter1
  CYC(b_+8, b_+9); A = B;  // ld a,b
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+10, b_+11); A = C;  // ld a,c
  CYC(b_+11, b_+14); SET_HL(SYM(veranFinal_beeForm_counter2Vals));  // ld hl,veranFinal_beeForm_counter2Vals
  CYC(b_+14, b_+15);
  veranFinal_addAToHl_from_rst(gb, b_+15);  // rst_addAToHl
  CYC(b_+15, b_+17); E = 0x87;  // ld e,Enemy.counter2
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);  // ld (de),a
  veranFinal_beeForm_nextTargetPosition_hook(gb); return;  // fallthrough
}

void veranFinal_beeForm_nextTargetPosition_hook(GB *gb) {
  BASE(veranFinal_beeForm_nextTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x86;  // ld e,Enemy.counter1
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); B = A;  // ld b,a
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);  // inc a
  CYC(b_+5, b_+7); alu_and(gb, 0x0f);  // and $0f
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+8, b_+9); A = B;  // ld a,b
  CYC(b_+9, b_+12); SET_HL(SYM(veranFinal_beeForm_targetPositions));  // ld hl,veranFinal_beeForm_targetPositions
  CYC(b_+12, b_+13);
  veranFinal_addDoubleIndexToHl_from_rst(gb, b_+13);  // rst_addDoubleIndexToHl
  CYC(b_+13, b_+15); E = 0xb6;  // ld e,Enemy.var36
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+17, b_+18); E = alu_inc8(gb, E);  // inc e
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);  // ld (de),a
  RET(b_+20); return;  // ret
}

void veranFinal_moveTowardTargetPosition_hook(GB *gb) {
  BASE(veranFinal_moveTowardTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xb6;  // ld l,Enemy.var36
  CALL_C(b_+3, ecom_readPositionVars_b10_hook, SYM(ecom_readPositionVars_b10), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);  // sub c
  CYC(b_+7, b_+9); alu_add(gb, 0x02);  // add $02
  CYC(b_+9, b_+11); alu_cp(gb, 0x05);  // cp $05
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto moveToward; } CYC(b_+11, b_+13);  // jr nc,+
  CYC(b_+13, b_+15); A = mem_rd(gb, hFF8F);  // ldh a,(<hFF8F)
  CYC(b_+15, b_+16); alu_sub(gb, B);  // sub b
  CYC(b_+16, b_+18); alu_add(gb, 0x02);  // add $02
  CYC(b_+18, b_+20); alu_cp(gb, 0x05);  // cp $05
  if ((F & FC)) { RET_TAKEN(b_+20); return; } CYC(b_+20, b_+21);  // ret c
moveToward:
  CALL_C(b_+21, ecom_moveTowardPosition_b10_hook, SYM(ecom_moveTowardPosition_b10), b_+24);
  CYC(b_+24, b_+25); alu_or(gb, D);  // or d
  RET(b_+25); return;  // ret
}

void veranFinal_getQuadrant_hook(GB *gb) {
  BASE(veranFinal_getQuadrant);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00;  // ld b,$00
  CYC(b_+2, b_+4); A = mem_rd(gb, hEnemyTargetY);  // ldh a,(<hEnemyTargetY)
  CYC(b_+4, b_+6); alu_cp(gb, 0x58);  // cp LARGE_ROOM_HEIGHT*16/2
  if ((F & FC)) { CYCT(b_+6, b_+8); goto checkX; } CYC(b_+6, b_+8);  // jr c,+
  CYC(b_+8, b_+10); B = 0x02;  // ld b,$02
checkX:
  CYC(b_+10, b_+12); A = mem_rd(gb, hEnemyTargetX);  // ldh a,(<hEnemyTargetX)
  CYC(b_+12, b_+14); alu_cp(gb, 0x78);  // cp LARGE_ROOM_WIDTH*16/2
  if ((F & FC)) { RET_TAKEN(b_+14); return; } CYC(b_+14, b_+15);  // ret c
  CYC(b_+15, b_+16); B = alu_inc8(gb, B);  // inc b
  RET(b_+16); return;  // ret
}
