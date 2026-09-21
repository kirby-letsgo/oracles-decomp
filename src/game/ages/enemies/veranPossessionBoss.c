#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode61);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  CALL_C(b_+5, veranPossessionBoss_wasHit_hook, SYM(veranPossessionBoss_wasHit), b_+8);

normalStatus:
  CALL_C(b_+8, ecom_getSubidAndCpStateTo08_b0e_hook, SYM(ecom_getSubidAndCpStateTo08_b0e), b_+11);
  if (F & FC) { CYCT(b_+11, b_+13); goto commonState; } // jr c
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = B;
  {
    CYC(b_+14, b_+15); push_effect(gb, b_+15);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == SYM(veranPossessionBoss_subid0)) { veranPossessionBoss_subid0_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_subid1)) { veranPossessionBoss_subid1_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_subid2)) { veranPossessionBoss_subid2_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_subid3)) { veranPossessionBoss_subid3_hook(gb); return; }
    HANDOFF(target);
  }

commonState:
  {
    CYC(b_+23, b_+24); push_effect(gb, b_+24);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == SYM(veranPossessionBoss_state_uninitialized)) { veranPossessionBoss_state_uninitialized_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_state_stub)) { veranPossessionBoss_state_stub_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_state_switchHook)) { veranPossessionBoss_state_switchHook_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:7722, bare global; jump-table target from enemyCode61 (commonState).
void veranPossessionBoss_state_uninitialized_hook(GB *gb) {
  BASE(veranPossessionBoss_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); alu_bit(gb, 1, B);
  if (!(F & FZ)) { CYCT(b_+2, b_+4); goto skipExtraGfx; } // jr nz
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); A = 0x61; // ENEMY_VERAN_POSSESSION_BOSS
  CYC(b_+6, b_+9); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);

skipExtraGfx:
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+12); alu_add(gb, B);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, enemySetAnimation_hook, SYM(enemySetAnimation), b_+18);
  CALL_C(b_+18, objectSetVisible82_hook, SYM(objectSetVisible82), b_+21);
  CYC(b_+21, b_+23); A = 0x50; // SPEED_200
  CALL_C(b_+23, ecom_setSpeedAndState8_b0e_hook, SYM(ecom_setSpeedAndState8_b0e), b_+26);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+28, b_+30); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+30); return; } // ret z
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+33, b_+35); A = 0x01;
  CYC(b_+35, b_+36); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x0c);
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x14); // SPEED_80
  RET(b_+45); return; // ret
}

// 0e:7750, bare global; jump-table target from enemyCode61 (commonState).
void veranPossessionBoss_state_switchHook_hook(GB *gb) {
  BASE(veranPossessionBoss_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == b_+11) goto substate0;
    if (target == SYM(enemyAnimate)) { enemyAnimate_hook(gb); return; }
    if (target == b_+19) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+14, b_+16); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+16, b_+19); TAIL(ecom_incSubstate_b0e); // jp

substate3:
  CYC(b_+19, b_+21); B = 0x0b;
  CALL_C(b_+21, ecom_fallToGroundAndSetState_b0e_hook, SYM(ecom_fallToGroundAndSetState_b0e), b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x28); // 40
  RET(b_+28); return; // ret
}

// 0e:776d, bare global; jump-table target from enemyCode61 (commonState).
void veranPossessionBoss_state_stub_hook(GB *gb) {
  BASE(veranPossessionBoss_state_stub);
  RET(b_+0); return; // ret
}

// 0e:776e, bare global; jump-table target from enemyCode61. Possessed Nayru.
void veranPossessionBoss_subid0_hook(GB *gb) {
  BASE(veranPossessionBoss_subid0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == SYM(veranPossessionBoss_nayruAmbi_state8)) { veranPossessionBoss_nayruAmbi_state8_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_state9)) { veranPossessionBoss_nayruAmbi_state9_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_stateA)) { veranPossessionBoss_nayruAmbi_stateA_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_stateB)) { veranPossessionBoss_nayruAmbi_stateB_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayru_stateC)) { veranPossessionBoss_nayru_stateC_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayru_stateD)) { veranPossessionBoss_nayru_stateD_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_stateE)) { veranPossessionBoss_nayruAmbi_stateE_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_stateF)) { veranPossessionBoss_nayruAmbi_stateF_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_state10)) { veranPossessionBoss_nayruAmbi_state10_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_state11)) { veranPossessionBoss_nayruAmbi_state11_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_state12)) { veranPossessionBoss_nayruAmbi_state12_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_state13)) { veranPossessionBoss_nayruAmbi_state13_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayru_state14)) { veranPossessionBoss_nayru_state14_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:778c, bare global; jump-table target from veranPossessionBoss_subid0/subid1
// (nayru/ambi share these states). Initialization.
void veranPossessionBoss_nayruAmbi_state8_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x07); // PART_SHADOW
  CYC(b_+6, b_+8); L = PART_BASE + OBJ_VAR03;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x06); // Y-offset of shadow relative to self
  CYC(b_+10, b_+12); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+12, b_+14); A = 0x80; // Enemy.start == ENEMY_BASE
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+15, b_+16); mem_wr(gb, HL, D);
  CALL_C(b_+16, veranPossessionBoss_nayruAmbi_beginMoving_hook, SYM(veranPossessionBoss_nayruAmbi_beginMoving), b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(b_+21, b_+23); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x03);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+29); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [var34] = $ff
  CYC(b_+29, b_+30); alu_xor(gb, A);
  CYC(b_+30, b_+33); mem_wr(gb, wTmpcfc0_genericCutscene_cfd0, A);
  CYC(b_+33, b_+35); A = 0x2e; // MUS_BOSS
  CYC(b_+35, b_+38); mem_wr(gb, wActiveMusic, A);
  CYC(b_+38, b_+41); TAIL(playSound_b00); // jp
}

// 0e:77b5, bare global; jump-table target from veranPossessionBoss_subid0/subid1.
// Flickering before moving.
void veranPossessionBoss_nayruAmbi_state9_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ecom_flickerVisibility_b0e_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0xfe); // -2
  CALL_C(b_+12, objectSetInvisible_hook, SYM(objectSetInvisible), b_+15);

choosePillar:
  CALL_C(b_+15, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0x0e);
  CYC(b_+20, b_+22); alu_cp(gb, 0x0b);
  if (!(F & FC)) { CYCT(b_+22, b_+24); goto choosePillar; } // jr nc
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+27, b_+28); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+28, b_+30); goto choosePillar; } // jr z
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); // [var34]
  CYC(b_+31, b_+34); SET_HL(b_+75); // @pillarList
  CYC(b_+34, b_+35); veranPossessionBoss_addAToHl_from_rst(gb, b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+36, b_+37); B = A;
  CYC(b_+37, b_+38); C = mem_rd(gb, HL);
  PUSH(b_+38, BC);
  CYC(b_+39, b_+41); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+41, b_+43); mem_wr(gb, hFF8F, A);
  CYC(b_+43, b_+45); A = mem_rd(gb, hEnemyTargetX);
  CYC(b_+45, b_+47); mem_wr(gb, hFF8E, A);
  CALL_C(b_+47, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+50);
  CYC(b_+50, b_+52); alu_add(gb, 0x04);
  CYC(b_+52, b_+54); alu_and(gb, 0x18);
  CYC(b_+54, b_+55); alu_rrca(gb);
  CYC(b_+55, b_+56); alu_rrca(gb);
  CYC(b_+56, b_+59); SET_HL(b_+87); // @pillarOffsets
  CYC(b_+59, b_+60); veranPossessionBoss_addAToHl_from_rst(gb, b_+60);
  SET_BC(POP(b_+60));
  CYC(b_+61, b_+62); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+62, b_+63); alu_add(gb, B);
  CYC(b_+63, b_+65); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CYC(b_+65, b_+66); mem_wr(gb, DE, A);
  CYC(b_+66, b_+67); A = mem_rd(gb, HL);
  CYC(b_+67, b_+68); alu_add(gb, C);
  CYC(b_+68, b_+69); E = alu_inc8(gb, E);
  CYC(b_+69, b_+70); mem_wr(gb, DE, A); // [var32]
  CYC(b_+70, b_+72); A = 0xc9; // SND_CIRCLING
  CYC(b_+72, b_+75); TAIL(playSound_b00); // jp
}

// 0e:7814, bare global; jump-table target from veranPossessionBoss_subid0/subid1.
// Moving to the new position.
void veranPossessionBoss_nayruAmbi_stateA_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CALL_C(b_+3, ecom_readPositionVars_b0e_hook, SYM(ecom_readPositionVars_b0e), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+9); alu_add(gb, 0x02);
  CYC(b_+9, b_+11); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+11, b_+14); ecom_moveTowardPosition_b0e_hook(gb); return; } // jp nc
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); A = mem_rd(gb, hFF8F);
  CYC(b_+16, b_+17); alu_sub(gb, B);
  CYC(b_+17, b_+19); alu_add(gb, 0x02);
  CYC(b_+19, b_+21); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+21, b_+24); ecom_moveTowardPosition_b0e_hook(gb); return; } // jp nc
  CYC(b_+21, b_+24);
  CYC(b_+24, b_+26); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+26, b_+27); mem_wr(gb, HL, B);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+29, b_+30); mem_wr(gb, HL, C);
  CYC(b_+30, b_+31); L = E;
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x00);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x1e); // 30
  RET(b_+40); return; // ret
}

// 0e:783d, bare global; jump-table target from veranPossessionBoss_subid0/subid1. Just
// reached the new position.
void veranPossessionBoss_nayruAmbi_stateB_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); ecom_flickerVisibility_b0e_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CALL_C(b_+6, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x0f);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_add(gb, A);
  CYC(b_+17, b_+18); alu_add(gb, A);
  CYC(b_+18, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+21, b_+22); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+23); A = alu_dec8(gb, A);
  CYC(b_+23, b_+26); SET_HL(b_+60); // @attackProbabilities
  CYC(b_+26, b_+27); veranPossessionBoss_addAToHl_from_rst(gb, b_+27);
  CYC(b_+27, b_+28); A = B;
  CYC(b_+28, b_+29); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+29, b_+31); goto beginAttacking; } // jr c
  CYC(b_+29, b_+31);
  CALL_C(b_+31, veranPossessionBoss_nayruAmbi_beginMoving_hook, SYM(veranPossessionBoss_nayruAmbi_beginMoving), b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x1e); // 30
  CYC(b_+36, b_+39); TAIL(ecom_flickerVisibility_b0e); // jp

beginAttacking:
  CALL_C(b_+39, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+42);
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x1e); // 30
  CYC(b_+46, b_+48); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+48, b_+50); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+54); A = alu_inc8(gb, A);
  CALL_C(b_+54, enemySetAnimation_hook, SYM(enemySetAnimation), b_+57);
  CYC(b_+57, b_+60); TAIL(objectSetVisiblec2); // jp
}

// 0e:7883, bare global; jump-table target from veranPossessionBoss_subid0. Delay before
// attacking with projectiles (Nayru only).
void veranPossessionBoss_nayru_stateC_hook(GB *gb) {
  BASE(veranPossessionBoss_nayru_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x8e); // [counter1] = 142
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+8, b_+10); B = 0x37; // PART_VERAN_PROJECTILE
  CYC(b_+10, b_+13); TAIL(ecom_spawnProjectile_b0e); // jp
}

// 0e:7890, bare global; jump-table target from veranPossessionBoss_subid0, also called
// from veranPossessionBoss_ambi_stateD. Attacking with projectiles.
void veranPossessionBoss_nayru_stateD_hook(GB *gb) {
  BASE(veranPossessionBoss_nayru_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  TAIL(veranPossessionBoss_doneAttacking); // fallthrough
}

// 0e:7894, bare global; falls into from veranPossessionBoss_nayru_stateD, also called
// from veranPossessionBoss_ambi_stateD.
void veranPossessionBoss_doneAttacking_hook(GB *gb) {
  BASE(veranPossessionBoss_doneAttacking);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, veranPossessionBoss_nayruAmbi_beginMoving_hook, SYM(veranPossessionBoss_nayruAmbi_beginMoving), b_+3);
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+13); TAIL(enemySetAnimation); // jp
}

// 0e:78a1, bare global; jump-table target from veranPossessionBoss_subid0/subid1. Just
// shot with mystery seeds.
void veranPossessionBoss_nayruAmbi_stateE_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_stateE);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+7);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x61); // ENEMY_VERAN_POSSESSION_BOSS
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x02); // [child.subid]
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+15, b_+16); E = L;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); // [child.var33] = [this.var33]
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+20, b_+22); A = 0x80; // Enemy.start == ENEMY_BASE
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+23, b_+24); mem_wr(gb, HL, D);
  CYC(b_+24, b_+27); SET_BC(0xfc04);
  CALL_C(b_+27, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+30);
  CALL_C(b_+30, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+33);
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+35, b_+37); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+39, b_+41); A = 0x01;
  CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+42, b_+43); mem_wr(gb, HL, A);
  CYC(b_+43, b_+46); TAIL(objectSetVisible83); // jp
}

// 0e:78cf, bare global; jump-table target from veranPossessionBoss_subid0/subid1.
// Collapsed (ghost Veran is showing).
void veranPossessionBoss_nayruAmbi_stateF_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_stateF);
  RET(b_+0); return; // ret
}

// 0e:78d0, bare global; jump-table target from veranPossessionBoss_subid0/subid1. Veran
// just returned to nayru/ambi's body.
void veranPossessionBoss_nayruAmbi_state10_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_state10);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+5, b_+7); A = 0x06;
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x0f);
  CYC(b_+13, b_+16); TAIL(objectSetVisible82); // jp
}

// 0e:78e0, bare global; jump-table target from veranPossessionBoss_subid0/subid1.
// Remains collapsed on the floor for a few frames before moving again.
void veranPossessionBoss_nayruAmbi_state11_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_state11);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CALL_C(b_+7, enemySetAnimation_hook, SYM(enemySetAnimation), SYM(veranPossessionBoss_nayruAmbi_beginMoving));
  TAIL(veranPossessionBoss_nayruAmbi_beginMoving); // fallthrough
}

// 0e:78ea, bare global; falls into from veranPossessionBoss_nayruAmbi_state11, also
// called from several other states.
void veranPossessionBoss_nayruAmbi_beginMoving_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_beginMoving);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x09);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x3c); // 60
  RET(b_+9); return; // ret
}

// 0e:78f4, bare global; jump-table target from veranPossessionBoss_subid0/subid1. Veran
// was just defeated.
void veranPossessionBoss_nayruAmbi_state12_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_state12);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CALL_C(b_+5, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+8);
  CYC(b_+8, b_+10); A = 0x02;
  CYC(b_+10, b_+13); TAIL(fadeoutToWhiteWithDelay); // jp
}

// 0e:7901, bare global; jump-table target from veranPossessionBoss_subid0/subid1.
// Waiting for the screen to go white.
void veranPossessionBoss_nayruAmbi_state13_hook(GB *gb) {
  BASE(veranPossessionBoss_nayruAmbi_state13);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CALL_C(b_+5, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+8);
  CYC(b_+8, b_+11); SET_HL(SYM(clearAllItemsAndPutLinkOnGround)); // clearAllItemsAndPutLinkOnGround
  CYC(b_+11, b_+13); E = 0x00;
  CYC(b_+13, b_+16); TAIL(interBankCall); // jp
}

// 0e:7911, bare global; jump-table target from veranPossessionBoss_subid0. Deletes all
// objects (including self), resumes cutscene with a newly created object.
void veranPossessionBoss_nayru_state14_hook(GB *gb) {
  BASE(veranPossessionBoss_nayru_state14);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, clearWramBank1_hook, SYM(clearWramBank1), b_+3);
  CYC(b_+3, b_+6); SET_HL(w1Link); // w1Link
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x03); // [w1Link.enabled]
  CALL_C(b_+8, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x6e); // INTERAC_NAYRU_SAVED_CUTSCENE
  RET(b_+13); return; // ret
}

// 0e:791f, bare global; jump-table target from enemyCode61. Possessed Ambi.
void veranPossessionBoss_subid1_hook(GB *gb) {
  BASE(veranPossessionBoss_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == SYM(veranPossessionBoss_nayruAmbi_state8)) { veranPossessionBoss_nayruAmbi_state8_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_state9)) { veranPossessionBoss_nayruAmbi_state9_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_stateA)) { veranPossessionBoss_nayruAmbi_stateA_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_stateB)) { veranPossessionBoss_nayruAmbi_stateB_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_ambi_stateC)) { veranPossessionBoss_ambi_stateC_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_ambi_stateD)) { veranPossessionBoss_ambi_stateD_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_stateE)) { veranPossessionBoss_nayruAmbi_stateE_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_stateF)) { veranPossessionBoss_nayruAmbi_stateF_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_state10)) { veranPossessionBoss_nayruAmbi_state10_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_state11)) { veranPossessionBoss_nayruAmbi_state11_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_state12)) { veranPossessionBoss_nayruAmbi_state12_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_nayruAmbi_state13)) { veranPossessionBoss_nayruAmbi_state13_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_ambi_state14)) { veranPossessionBoss_ambi_state14_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:793d, bare global; jump-table target from veranPossessionBoss_subid1. Delay before
// attacking with projectiles or spawning spiders (Ambi only).
void veranPossessionBoss_ambi_stateC_hook(GB *gb) {
  BASE(veranPossessionBoss_ambi_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x8e); // [counter1] = 142
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(b_+8, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+11);
  CYC(b_+11, b_+13); alu_and(gb, 0x0f);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); A = alu_dec8(gb, A);
  CYC(b_+18, b_+21); SET_HL(b_+36); // @spiderSpawnProbabilities
  CYC(b_+21, b_+22); veranPossessionBoss_addAToHl_from_rst(gb, b_+22);
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+24); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x01);
  if (!(F & FC)) { RET_TAKEN(b_+29); return; } // ret nc
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+31, b_+33); B = 0x37; // PART_VERAN_PROJECTILE
  CYC(b_+33, b_+36); TAIL(ecom_spawnProjectile_b0e); // jp
}

// 0e:7966, bare global; jump-table target from veranPossessionBoss_subid1. Attacking
// with projectiles or spiders (Ambi only).
void veranPossessionBoss_ambi_stateD_hook(GB *gb) {
  BASE(veranPossessionBoss_ambi_stateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+7); veranPossessionBoss_nayru_stateD_hook(gb); return; } // jp z
  CYC(b_+4, b_+7);
  CALL_C(b_+7, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+13); veranPossessionBoss_doneAttacking_hook(gb); return; } // jp z
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+14, b_+16); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; } // ret nz
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); A = mem_rd(gb, wNumEnemies);
  CYC(b_+20, b_+22); alu_cp(gb, 0x06);
  if (!(F & FC)) { RET_TAKEN(b_+22); return; } // ret nc
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); B = 0x0f; // ENEMY_VERAN_SPIDER
  CYC(b_+25, b_+28); TAIL(ecom_spawnEnemyWithSubid01_b0e); // jp
}

// 0e:7982, bare global; jump-table target from veranPossessionBoss_subid1.
// Ambi-specific cutscene after Veran is defeated.
void veranPossessionBoss_ambi_state14_hook(GB *gb) {
  BASE(veranPossessionBoss_ambi_state14);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CALL_C(b_+5, clearWramBank1_hook, SYM(clearWramBank1), b_+8);
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+13); mem_wr(gb, wNumEnemies, A);
  CYC(b_+13, b_+16); SET_HL(w1Link); // w1Link
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x03); // [w1Link.enabled]
  CYC(b_+18, b_+20); L = 0x0b; // w1Link.yh
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x58);
  CYC(b_+22, b_+24); L = 0x0d; // w1Link.xh
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x78);
  CALL_C(b_+26, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+29);
  CALL_C(b_+29, resetCamera_hook, SYM(resetCamera), b_+32);
  CALL_C(b_+32, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+35);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x61); // ENEMY_VERAN_POSSESSION_BOSS
  CYC(b_+37, b_+38); L = alu_inc8(gb, L);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x03); // [subid]
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x48);
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x78);
  RET(b_+48); return; // ret
}

// 0e:79b3, bare global; jump-table target from enemyCode61. Veran emerged in human
// form.
void veranPossessionBoss_subid2_hook(GB *gb) {
  BASE(veranPossessionBoss_subid2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = veranPossessionBoss_jump_table(gb);
    if (target == SYM(veranPossessionBoss_humanForm_state8)) { veranPossessionBoss_humanForm_state8_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_humanForm_state9)) { veranPossessionBoss_humanForm_state9_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_humanForm_stateA)) { veranPossessionBoss_humanForm_stateA_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_humanForm_stateB)) { veranPossessionBoss_humanForm_stateB_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_humanForm_stateC)) { veranPossessionBoss_humanForm_stateC_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_humanForm_stateD)) { veranPossessionBoss_humanForm_stateD_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_humanForm_stateE)) { veranPossessionBoss_humanForm_stateE_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_humanForm_stateF)) { veranPossessionBoss_humanForm_stateF_hook(gb); return; }
    if (target == SYM(veranPossessionBoss_humanForm_state10)) { veranPossessionBoss_humanForm_state10_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:79c9, bare global; jump-table target from veranPossessionBoss_subid2. Moving
// upward just after spawning.
void veranPossessionBoss_humanForm_state8_hook(GB *gb) {
  BASE(veranPossessionBoss_humanForm_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x5b); // ENEMYCOLLISION_VERAN_GHOST
  CYC(b_+21, b_+23); A = 0x02; // Object.subid
  CALL_C(b_+23, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+28); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+28, b_+30); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+32, b_+34); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+34, b_+36); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [var35] |= 1
  CYC(b_+37, b_+40); SET_BC(0x2f2a); // TX_2f2a
  CALL_C(b_+40, showText_hook, SYM(showText), b_+43);
  CYC(b_+43, b_+45); TAIL(veranPossessionBoss_animate); // jr
}

// 0e:79f6, bare global; jump-table target from veranPossessionBoss_subid2. Waiting for
// Link to use switch hook.
void veranPossessionBoss_humanForm_state9_hook(GB *gb) {
  BASE(veranPossessionBoss_humanForm_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x0c); // [counter1] = 12
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  TAIL(veranPossessionBoss_animate); // fallthrough
}

// 0e:7a07, bare global; falls into from veranPossessionBoss_humanForm_state9, also
// reached via tail-jump from several other states.
void veranPossessionBoss_animate_hook(GB *gb) {
  BASE(veranPossessionBoss_animate);
  CYC(b_+0, b_+3); TAIL(enemyAnimate); // jp
}

// 0e:7a0a, bare global; jump-table target from veranPossessionBoss_subid2. Moving down
// to re-possess her victim.
void veranPossessionBoss_humanForm_stateA_hook(GB *gb) {
  BASE(veranPossessionBoss_humanForm_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+10, b_+12); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  TAIL(veranPossessionBoss_humanForm_returnToHost); // fallthrough
}

// 0e:7a16, bare global; falls into from veranPossessionBoss_humanForm_stateA, also
// called from veranPossessionBoss_humanForm_stateD.
void veranPossessionBoss_humanForm_returnToHost_hook(GB *gb) {
  BASE(veranPossessionBoss_humanForm_returnToHost);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x04; // Object.state
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+6, b_+8); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+8, b_+9); E = L;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+14); TAIL(enemyDelete); // jp
}

// 0e:7a24, bare global; jump-table target from veranPossessionBoss_subid2. Just
// finished using switch hook on ghost. Flickering between ghost and human forms.
void veranPossessionBoss_humanForm_stateB_hook(GB *gb) {
  BASE(veranPossessionBoss_humanForm_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto flickerBetweenForms; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x5c); // ENEMYCOLLISION_VERAN_HUMAN
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)

flickerBetweenForms:
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); // [counter1]
  CYC(b_+18, b_+19); alu_rrca(gb);
  CYC(b_+19, b_+21); A = 0x09;
  if (F & FC) { CYCT(b_+21, b_+23); goto setFlickerAnimation; } // jr c
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); A = 0x06;

setFlickerAnimation:
  CYC(b_+25, b_+28); TAIL(enemySetAnimation); // jp
}

// 0e:7a40, bare global; jump-table target from veranPossessionBoss_subid2. Veran is
// vulnerable to attacks.
void veranPossessionBoss_humanForm_stateC_hook(GB *gb) {
  BASE(veranPossessionBoss_humanForm_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); veranPossessionBoss_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+9, b_+11); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x64); // SPEED_280
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+17, b_+19); A = 0x80;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a -- speedZ = -$280
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0xfd);
  CALL_C(b_+22, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+25);
  CYC(b_+25, b_+27); A = 0x0b; // Object.yh
  CALL_C(b_+27, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+30);
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CYC(b_+32, b_+33); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+33, b_+34); mem_wr(gb, DE, A); // [this.var31]
  CYC(b_+34, b_+35); E = alu_inc8(gb, E);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+38); mem_wr(gb, DE, A); // [this.var32]
  CYC(b_+38, b_+40); TAIL(veranPossessionBoss_animate); // jr
}

// 0e:7a68, bare global; jump-table target from veranPossessionBoss_subid2. Moving back
// to nayru/ambi.
void veranPossessionBoss_humanForm_stateD_hook(GB *gb) {
  BASE(veranPossessionBoss_humanForm_stateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_PRESSED_A_BUTTON; // Enemy.var31
  CALL_C(b_+7, ecom_readPositionVars_b0e_hook, SYM(ecom_readPositionVars_b0e), b_+10);
  CYC(b_+10, b_+11); alu_sub(gb, C);
  CYC(b_+11, b_+13); alu_add(gb, 0x02);
  CYC(b_+13, b_+15); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+15, b_+18); ecom_moveTowardPosition_b0e_hook(gb); return; } // jp nc
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+20); A = mem_rd(gb, hFF8F);
  CYC(b_+20, b_+21); alu_sub(gb, B);
  CYC(b_+21, b_+23); alu_add(gb, 0x02);
  CYC(b_+23, b_+25); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+25, b_+28); ecom_moveTowardPosition_b0e_hook(gb); return; } // jp nc
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+30, b_+31); mem_wr(gb, HL, B);
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+33, b_+34); mem_wr(gb, HL, C);
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+38); return; } // ret nz
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+42); TAIL(veranPossessionBoss_humanForm_returnToHost); // jp
}

// 0e:7a92, bare global; jump-table target from veranPossessionBoss_subid2. Health is
// zero; about to begin the cutscene.
void veranPossessionBoss_humanForm_stateE_hook(GB *gb) {
  BASE(veranPossessionBoss_humanForm_stateE);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CALL_C(b_+5, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+8);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; } // ret nc
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); SET_BC(0x0502); // INTERAC_PUFF,$02
  CALL_C(b_+12, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+15);
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; } // ret nz
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); A = H;
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_RELATED2 + 1; // Enemy.relatedObj2+1
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x40); // Interaction.start == INTERACTION_BASE
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+26, b_+28); A = 0x01; // DISABLE_LINK
  CYC(b_+28, b_+31); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+31, b_+34); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+34, b_+37); TAIL(objectSetInvisible); // jp
}

// 0e:7ab7, bare global; jump-table target from veranPossessionBoss_subid2. Waiting for
// the puff to finish its animation.
void veranPossessionBoss_humanForm_stateF_hook(GB *gb) {
  BASE(veranPossessionBoss_humanForm_stateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x21; // Object.animParameter
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); TAIL(ecom_incState_b0e); // jp
}

// 0e:7ac2, bare global; jump-table target from veranPossessionBoss_subid2. Sets
// nayru/ambi's state to $12, shows text, then deletes self.
void veranPossessionBoss_humanForm_state10_hook(GB *gb) {
  BASE(veranPossessionBoss_humanForm_state10);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x04; // Object.state
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x12);
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+11); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+11, b_+14); SET_BC((SYM(swordEnemy_state9) + 12)); // TX_560b
  if (F & FZ) { CYCT(b_+14, b_+16); goto showText; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); SET_BC((SYM(swordEnemy_stateA) + 5)); // TX_5611

showText:
  CALL_C(b_+19, showText_hook, SYM(showText), b_+22);
  CYC(b_+22, b_+25); TAIL(enemyDelete); // jp
}

// 0e:7adb, bare global; jump-table target from enemyCode61. Collapsed Ambi after the
// fight.
void veranPossessionBoss_subid3_hook(GB *gb) {
  BASE(veranPossessionBoss_subid3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state9; } // jr nz
  CYC(b_+3, b_+5);

  CYC(b_+5, b_+8); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } // ret nz
  CYC(b_+9, b_+10);
  CALL_C(b_+10, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+13);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x3c); // 60
  CYC(b_+17, b_+19); A = 0x05;
  CALL_C(b_+19, enemySetAnimation_hook, SYM(enemySetAnimation), b_+22);
  CYC(b_+22, b_+25); TAIL(fadeinFromWhite); // jp

state9:
  CYC(b_+25, b_+28); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; } // ret nz
  CYC(b_+29, b_+30);
  CALL_C(b_+30, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; } // ret nz
  CYC(b_+33, b_+34);
  CALL_C(b_+34, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+37);
  if (!(F & FZ)) { RET_TAKEN(b_+37); return; } // ret nz
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x4d); // INTERAC_AMBI
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x07); // [subid]
  CALL_C(b_+43, objectCopyPosition_hook, SYM(objectCopyPosition), b_+46);
  CYC(b_+46, b_+48); A = 0x01; // TREE_GFXH_01
  CYC(b_+48, b_+51); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+51, b_+54); TAIL(enemyDelete); // jp
}

// 0e:7b11, bare global; called from enemyCode61.
void veranPossessionBoss_wasHit_hook(GB *gb) {
  BASE(veranPossessionBoss_wasHit);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x00);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x02);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  if (F & FZ) { CYCT(b_+13, b_+15); goto subid2; } // jr z
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+17, b_+19); alu_cp(gb, 0x1a); // $80|ITEMCOLLISION_MYSTERY_SEED - $80
  if (F & FZ) { CYCT(b_+19, b_+21); goto mysterySeed; } // jr z
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); alu_sub(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  if (F & FC) { RET_TAKEN(b_+23); return; } // ret c
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); alu_cp(gb, 0x09); // ITEMCOLLISION_SHOVEL-ITEMCOLLISION_L1_SWORD+1
  if (!(F & FC)) { RET_TAKEN(b_+26); return; } // ret nc
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+29, b_+31); mem_wr(gb, HL, 0xe8); // -24
  CYC(b_+31, b_+34); SET_HL(w1Link_invincibilityCounter);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x28); // 40
  CYC(b_+36, b_+37); L = alu_inc8(gb, L); // w1Link.knockbackAngle
  CYC(b_+37, b_+39); E = ENEMY_BASE + OBJ_KNOCKBACK_ANGLE;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+42); alu_xor(gb, 0x10);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x15); // [w1Link.knockbackCounter] = 21
  CYC(b_+45, b_+47); L = 0x25; // w1Link.damageToApply
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0xf8); // -8
  RET(b_+49); return; // ret

mysterySeed:
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x0e);
  CYC(b_+54, b_+56); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x1e); // 30
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+60, b_+62); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_USE_TEXT_ID; // Enemy.var30
  CYC(b_+64, b_+65); A = mem_rd(gb, HL);
  CYC(b_+65, b_+67); alu_add(gb, 0x02);
  CYC(b_+67, b_+70); TAIL(enemySetAnimation); // jp

subid2:
  CYC(b_+70, b_+72); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+72, b_+74); alu_cp(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  if (F & FC) { RET_TAKEN(b_+74); return; } // ret c
  CYC(b_+74, b_+75);
  CYC(b_+75, b_+77); alu_cp(gb, 0x0c); // ITEMCOLLISION_EXPERT_PUNCH+1
  if (!(F & FC)) { RET_TAKEN(b_+77); return; } // ret nc
  CYC(b_+77, b_+78);
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+83); alu_cp(gb, 0x5b); // ENEMYCOLLISION_VERAN_GHOST
  if (!(F & FZ)) { CYCT(b_+83, b_+85); goto damagePillarForm; } // jr nz
  CYC(b_+83, b_+85);
  CYC(b_+85, b_+87); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0xf8); // -8
  RET(b_+89); return; // ret

damagePillarForm:
  CYC(b_+90, b_+92); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x08);
  CYC(b_+94, b_+96); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+96, b_+97); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+97); return; } // ret nz
  CYC(b_+97, b_+98);
  CYC(b_+98, b_+100); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0x80);
  CYC(b_+102, b_+104); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+104, b_+106); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+106, b_+108); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x0e);
  CYC(b_+110, b_+112); A = 0x01;
  CYC(b_+112, b_+115); mem_wr(gb, wTmpcfc0_genericCutscene_cfd0, A);
  CYC(b_+115, b_+117); A = 0xf0; // SNDCTRL_STOPMUSIC
  CYC(b_+117, b_+120); TAIL(playSound_b00); // jp
}
