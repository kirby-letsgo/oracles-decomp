#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode7d);
  CYC(b_+150, b_+152); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+152, b_+153); A = mem_rd(gb, DE);
  CYC(b_+153, b_+155); alu_cp(gb, 0x08);
  CYC(b_+155, b_+157); E = ENEMY_BASE + OBJ_SUBID;
  if (F & FC) { CYCT(b_+157, b_+159); goto state8OrLess; } // jr c
  CYC(b_+157, b_+159);
  CYC(b_+159, b_+160); A = mem_rd(gb, DE);
  CYC(b_+160, b_+161); push_effect(gb, b_+161);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == SYM(octogon_subid0)) { octogon_subid0_hook(gb); return; }
    if (target == SYM(octogon_subid1)) { octogon_subid1_hook(gb); return; }
    if (target == SYM(octogon_subid2)) { octogon_subid2_hook(gb); return; }
    HANDOFF(target);
  }

state8OrLess:
  CYC(b_+167, b_+168); push_effect(gb, b_+168);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == SYM(octogon_state_uninitialized)) { octogon_state_uninitialized_hook(gb); return; }
    if (target == SYM(octogon_state_stub)) { octogon_state_stub_hook(gb); return; }
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
  BASE(enemyCode7d);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto justHit; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+12, b_+15); enemyDelete_hook(gb); return; } // jp z
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) { CALL_C_CC(b_+19, ecom_killRelatedObj1_b0f_hook, SYM(ecom_killRelatedObj1_b0f), b_+22); } else { CYC(b_+19, b_+22); } // call nz
  CYC(b_+22, b_+25); TAIL(enemyBoss_dead_b0f); // jp

justHit:
  CYC(b_+25, b_+27); A = OBJ_INVINCIBILITY_COUNTER;
  CALL_C(b_+27, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+30);
  CYC(b_+30, b_+31); E = L;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+38); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+38, b_+40); goto normalStatus; } // jr z
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+43, b_+45); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+47, b_+49); alu_cp(gb, 0x0a);
  if (F & FC) { CYCT(b_+49, b_+51); goto L_7446; } // jr c
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x01);

L_7446:
  CYC(b_+55, b_+57); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+59); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+59, b_+61); goto normalStatus; } // jr nz
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+64); SET_HL(wGroup5RoomFlags + 0x2d); // ROOM_AGES_52d
  CYC(b_+64, b_+66); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+66, b_+68); L = 0x36; // ROOM_AGES_536
  CYC(b_+68, b_+70); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+70, b_+72); A = 0x2e; // MUS_BOSS
  CYC(b_+72, b_+75); mem_wr(gb, wActiveMusic, A);
  RET(b_+75); return; // ret

normalStatus:
  CYC(b_+76, b_+79); push_effect(gb, b_+79); octogon_doJumpTable(gb, sp0_);
  CYC(b_+79, b_+80); H = D;
  CYC(b_+80, b_+82); L = ENEMY_BASE + OBJ_VAR34;
  CYC(b_+82, b_+84); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+86, b_+88); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); mem_wr(gb, HL, A);
  CYC(b_+90, b_+92); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+92, b_+93); A = mem_rd(gb, HL);
  CYC(b_+93, b_+95); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+95, b_+97); goto L_7472; } // jr c
  CYC(b_+95, b_+97);
  CYC(b_+97, b_+99); A = 0x08;

L_7472:
  CYC(b_+99, b_+101); alu_and(gb, 0x0c);
  CYC(b_+101, b_+102); alu_rrca(gb);
  CYC(b_+102, b_+105); SET_HL(b_+142); // @offsetData
  CYC(b_+105, b_+106); octogon_addAToHl_from_rst(gb, b_+106);
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+111); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+111, b_+112); SET_HL(HL + 1); // inc hl
  CYC(b_+112, b_+113); A = mem_rd(gb, DE);
  CYC(b_+113, b_+114); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+114, b_+115); mem_wr(gb, DE, A);
  CYC(b_+115, b_+118); SET_HL(wTmpcfc0_octogonBoss_var03);
  CYC(b_+118, b_+120); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+120, b_+121); A = mem_rd(gb, DE);
  CYC(b_+121, b_+122); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+122, b_+124); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+124, b_+125); A = mem_rd(gb, DE);
  CYC(b_+125, b_+126); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [octogonBoss.direction]
  CYC(b_+126, b_+128); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+130); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [octogonBoss.health]
  CYC(b_+130, b_+132); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+132, b_+133); A = mem_rd(gb, DE);
  CYC(b_+133, b_+134); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [octogonBoss.var33]
  CYC(b_+134, b_+135); E = alu_inc8(gb, E);
  CYC(b_+135, b_+136); A = mem_rd(gb, DE); // [var34]
  CYC(b_+136, b_+137); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a [octogonBoss.var34]
  CYC(b_+137, b_+139); E = ENEMY_BASE + 0x30; // var30
  CYC(b_+139, b_+140); A = mem_rd(gb, DE);
  CYC(b_+140, b_+141); mem_wr(gb, HL, A); // [octogonBoss.var30]
  RET(b_+141); return; // ret
}

void octogon_state_uninitialized_hook(GB *gb) {
  BASE(octogon_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto notSubid2; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+9, b_+11); A = 0x67; // ENEMYCOLLISION_OCTOGON_SHELL
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+15); TAIL(ecom_setSpeedAndState8_b0f); // jp

notSubid2:
  CYC(b_+15, b_+17); A = 0x7d; // ENEMY_OCTOGON
  CYC(b_+17, b_+20); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);
  CYC(b_+20, b_+22); A = 0x88; // PALH_88
  CALL_C(b_+22, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+25);
  CYC(b_+25, b_+28); SET_HL(wTmpcfc0_octogonBoss_loadedExtraGfx);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto L_74eb; } // jr nz
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+33, enemyBoss_initializeRoomWithoutExtraGfx_b0f_hook, SYM(enemyBoss_initializeRoomWithoutExtraGfx_b0f), b_+36);

L_74eb:
  CALL_C(b_+36, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+39);
  if (!(F & FZ)) { RET_TAKEN(b_+39); return; } // ret nz
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x7d); // ENEMY_OCTOGON
  CYC(b_+42, b_+43); L = alu_inc8(gb, L);
  CYC(b_+43, b_+45); mem_wr(gb, HL, 0x02); // [child.subid]
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+47, b_+48); E = L;
  CYC(b_+48, b_+50); A = 0x80; // Enemy.start
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+52, b_+53); E = alu_inc8(gb, E);
  CYC(b_+53, b_+54); A = H;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+56); mem_wr(gb, HL, D);
  CYC(b_+56, b_+58); A = 0x28; // SPEED_100
  CALL_C(b_+58, ecom_setSpeedAndState8_b0f_hook, SYM(ecom_setSpeedAndState8_b0f), b_+61);
  CYC(b_+61, b_+63); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x0c); // [this.var35]
  CYC(b_+65, b_+66); L = alu_inc8(gb, L);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 120); // [var36]
  CYC(b_+68, b_+70); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+70, b_+71); A = mem_rd(gb, HL);
  CYC(b_+71, b_+72); alu_add(gb, A);
  CYC(b_+72, b_+73); B = A;
  CALL_C(b_+73, objectSetVisible83_hook, SYM(objectSetVisible83), b_+76);
  CYC(b_+76, b_+79); SET_HL(wTmpcfc0_octogonBoss_var30);
  CYC(b_+79, b_+81); E = ENEMY_BASE + 0x30; // var30
  CYC(b_+81, b_+82); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+82, b_+83); mem_wr(gb, DE, A);
  CYC(b_+83, b_+85); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+85, b_+86); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+86, b_+87); mem_wr(gb, DE, A);
  CYC(b_+87, b_+89); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+89, b_+90); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CYC(b_+91, b_+93); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+93, b_+94); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+94, b_+95); mem_wr(gb, DE, A);
  CYC(b_+95, b_+97); E = ENEMY_BASE + OBJ_VAR37;
  CYC(b_+97, b_+98); mem_wr(gb, DE, A);
  CYC(b_+98, b_+100); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+100, b_+101); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);
  CYC(b_+102, b_+104); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  CYC(b_+105, b_+106); mem_wr(gb, DE, A);
  CYC(b_+106, b_+107); alu_add(gb, B);
  CYC(b_+107, b_+108); push_effect(gb, b_+108);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == b_+116) goto subid0_0;
    if (target == b_+145) goto subid0_1;
    if (target == b_+151) goto subid1_0;
    if (target == b_+169) goto subid1_1;
    HANDOFF(target);
  }

subid0_0:
  CALL_C(b_+116, octogon_fixPositionAboveWater_hook, SYM(octogon_fixPositionAboveWater), b_+119);
  CYC(b_+119, b_+120); H = D;
  CYC(b_+120, b_+122); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+122, b_+124); mem_wr(gb, HL, 120);
  CYC(b_+124, b_+126); L = ENEMY_BASE + 0x30; // var30
  CYC(b_+126, b_+127); A = mem_rd(gb, HL);
  CYC(b_+127, b_+128); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+128, b_+131); octogon_chooseRandomTargetPosition_hook(gb); return; } // jp z
  CYC(b_+128, b_+131);
  CALL_C(b_+131, octogon_loadTargetPosition_hook, SYM(octogon_loadTargetPosition), b_+134);
  if (F & FZ) { RET_TAKEN(b_+134); return; } // ret z
  CYC(b_+134, b_+135);
  CYC(b_+135, b_+136); SET_HL(HL - 1); // dec hl
  CYC(b_+136, b_+137); SET_HL(HL - 1); // dec hl
  CYC(b_+137, b_+138); SET_HL(HL - 1); // dec hl
  CYC(b_+138, b_+139); A = mem_rd(gb, HL);
  CYC(b_+139, b_+141); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+141, b_+142); mem_wr(gb, DE, A);
  CYC(b_+142, b_+145); TAIL(enemySetAnimation); // jp

subid0_1:
  CALL_C(b_+145, octogon_fixPositionAboveWater_hook, SYM(octogon_fixPositionAboveWater), b_+148);
  CYC(b_+148, b_+151); TAIL(octogon_loadNormalSubmergedAnimation); // jp

subid1_0:
  CYC(b_+151, b_+152); H = D;
  CYC(b_+152, b_+154); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+154, b_+156); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+156, b_+158); A = 0x00; // Object.start
  CALL_C(b_+158, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+161);
  CYC(b_+161, b_+163); B = 0x40;
  CALL_C(b_+163, clearMemory_hook, SYM(clearMemory), b_+166);
  CYC(b_+166, b_+169); TAIL(objectSetInvisible); // jp

subid1_1:
  CYC(b_+169, b_+171); A = 0x01;
  CYC(b_+171, b_+174); mem_wr(gb, wTmpcfc0_octogonBoss_posNeedsFixing, A);
  CYC(b_+174, b_+175); H = D;
  CYC(b_+175, b_+177); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+177, b_+179); A = 0x06;
  CYC(b_+179, b_+180); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+180, b_+181); mem_wr(gb, HL, A);
  CYC(b_+181, b_+183); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+183, b_+185); mem_wr(gb, HL, 90);
  CYC(b_+185, b_+186); L = alu_inc8(gb, L);
  CYC(b_+186, b_+188); mem_wr(gb, HL, 150); // [counter2]
  CYC(b_+188, b_+190); L = ENEMY_BASE + 0x31; // var31
  CYC(b_+190, b_+192); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+192, b_+193); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+193, b_+195); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+195, b_+196); mem_wr(gb, HL, A);
  CALL_C(b_+196, ecom_updateAngleTowardTarget_b0f_hook, SYM(ecom_updateAngleTowardTarget_b0f), b_+199);
  CYC(b_+199, b_+201); alu_add(gb, 0x04);
  CYC(b_+201, b_+203); alu_and(gb, 0x18);
  CYC(b_+203, b_+204); alu_rrca(gb);
  CYC(b_+204, b_+206); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+206, b_+207); mem_wr(gb, DE, A);
  CALL_C(b_+207, enemySetAnimation_hook, SYM(enemySetAnimation), b_+210);
  CALL_C(b_+210, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+213);
  if (!(F & FZ)) { RET_TAKEN(b_+213); return; } // ret nz
  CYC(b_+213, b_+214);
  CYC(b_+214, b_+216); mem_wr(gb, HL, 0x91); // INTERAC_BUBBLE
  CYC(b_+216, b_+217); L = alu_inc8(gb, L);
  CYC(b_+217, b_+218); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [bubble.subid] = 1
  CYC(b_+218, b_+220); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+220, b_+222); A = 0x80; // Enemy.start
  CYC(b_+222, b_+223); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+223, b_+224); mem_wr(gb, HL, D);
  RET(b_+224); return; // ret
}

void octogon_state_stub_hook(GB *gb) {
  BASE(octogon_state_stub);
  RET(b_+0); return; // ret
}

void octogon_subid0_hook(GB *gb) {
  BASE(octogon_subid0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(b_+18, b_+21); octogon_subid0BelowWater_hook(gb); return; } // jp nz
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+23); alu_sub(gb, 0x08);
  CYC(b_+23, b_+24); push_effect(gb, b_+24);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == SYM(octogon_subid0AboveWater_state8)) { octogon_subid0AboveWater_state8_hook(gb); return; }
    if (target == SYM(octogon_subid0AboveWater_state9)) { octogon_subid0AboveWater_state9_hook(gb); return; }
    if (target == SYM(octogon_subid0AboveWater_stateA)) { octogon_subid0AboveWater_stateA_hook(gb); return; }
    if (target == SYM(octogon_subid0AboveWater_stateB)) { octogon_subid0AboveWater_stateB_hook(gb); return; }
    if (target == SYM(octogon_subid0AboveWater_stateC)) { octogon_subid0AboveWater_stateC_hook(gb); return; }
    if (target == SYM(octogon_subid0AboveWater_stateD)) { octogon_subid0AboveWater_stateD_hook(gb); return; }
    if (target == SYM(octogon_subid0AboveWater_stateE)) { octogon_subid0AboveWater_stateE_hook(gb); return; }
    if (target == SYM(octogon_subid0AboveWater_state10)) { octogon_subid0AboveWater_state10_hook(gb); return; }
    if (target == SYM(octogon_subid0AboveWater_state11)) { octogon_subid0AboveWater_state11_hook(gb); return; }
    HANDOFF(target);
  }
}

// Wait for shutters to close
void octogon_subid0AboveWater_state8_hook(GB *gb) {
  BASE(octogon_subid0AboveWater_state8);
  CYC(b_+0, b_+3); A = mem_rd(gb, wcc93);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  CYC(b_+8, b_+11); SET_HL(wActiveMusic);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+13); return; } // ret z
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x00);
  CYC(b_+16, b_+18); A = 0x2e; // MUS_BOSS
  CYC(b_+18, b_+21); TAIL(playSound_b00); // jp
}

// Moving normally around the room
void octogon_subid0AboveWater_state9_hook(GB *gb) {
  BASE(octogon_subid0AboveWater_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, octogon_decVar36IfNonzero_hook, SYM(octogon_decVar36IfNonzero), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wFrameCounter);
  CYC(b_+6, b_+8); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto L_75fa; } // jr nz
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+13, b_+16); octogon_subid0_submergeIntoWater_hook(gb); return; } // jp z
  CYC(b_+13, b_+16);

L_75fa:
  CYC(b_+16, b_+18); C = 0x08;
  CALL_C(b_+18, objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+21);
  if (!(F & FC)) { CYCT(b_+21, b_+24); octogon_updateMovementAndAnimation_hook(gb); return; } // jp nc
  CYC(b_+21, b_+24);
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+27, b_+28); A = mem_rd(gb, HL);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+29, b_+32); octogon_updateMovementAndAnimation_hook(gb); return; } // jp nz
  CYC(b_+29, b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 120); // [var36]
  CALL_C(b_+34, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+37);
  CYC(b_+37, b_+39); alu_add(gb, 0x14);
  CYC(b_+39, b_+41); alu_and(gb, 0x18);
  CYC(b_+41, b_+42); B = A;
  CYC(b_+42, b_+44); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+47); alu_and(gb, 0x0c);
  CYC(b_+47, b_+48); alu_add(gb, A);
  CYC(b_+48, b_+49); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+49, b_+52); octogon_updateMovementAndAnimation_hook(gb); return; } // jp nz
  CYC(b_+49, b_+52);
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x0b);
  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x08);
  RET(b_+61); return; // ret
}

// Waiting in place until counter1 reaches 0, then choosing the next target position
// (also reachable as octogon_subid0AboveWater_stateA, since both labels share this
// address).
void octogon_subid0_pauseMovement_hook(GB *gb) {
  BASE(octogon_subid0AboveWater_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]--
  CYC(b_+6, b_+8); L = ENEMY_BASE + 0x30; // var30
  CYC(b_+8, b_+9); A = mem_rd(gb, HL);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+13); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); octogon_loadTargetPosition_hook(gb); return; } // jr nz
  CYC(b_+13, b_+15);
  TAIL(octogon_chooseRandomTargetPosition); // fallthrough
}

void octogon_subid0AboveWater_stateA_hook(GB *gb) {
  octogon_subid0_pauseMovement_hook(gb);
}

void octogon_chooseRandomTargetPosition_hook(GB *gb) {
  BASE(octogon_chooseRandomTargetPosition);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getRandomNumber_hook, SYM(getRandomNumber), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x18);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  TAIL(octogon_loadTargetPosition); // fallthrough
}

// @param hl Pointer to index for a table
// @param[out] hl Pointer to some data
// @param[out] zflag z if animation changed
void octogon_loadTargetPosition_hook(GB *gb) {
  BASE(octogon_loadTargetPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, HL);
  CYC(b_+1, b_+2); alu_add(gb, A);
  CYC(b_+2, b_+3); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+3, b_+6); SET_HL(b_+31); // @targetPositionList
  CYC(b_+6, b_+7); octogon_addAToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + 0x31; // var31
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+12); E = alu_inc8(gb, E);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+13, b_+14); mem_wr(gb, DE, A); // [var32]
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; } // ret nz
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; } // ret nz
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+25); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CALL_C(b_+26, enemySetAnimation_hook, SYM(enemySetAnimation), b_+29);
  CYC(b_+29, b_+30); alu_xor(gb, A);
  RET(b_+30); return; // ret
}

// Turning around to fire projectile (or just after doing so); also reached as
// octogon_subid0AboveWater_stateF (identical address).
void octogon_subid0AboveWater_stateB_hook(GB *gb) {
  BASE(octogon_subid0AboveWater_stateB);
  CYC(b_+0, b_+2); B = 0x06;
  CYCT(b_+2, b_+4); TAIL(octogon_subid0AboveWater_turningAround); // jr
}

void octogon_subid0AboveWater_stateF_hook(GB *gb) {
  TAIL(octogon_subid0AboveWater_stateB);
}

// Turning around?
void octogon_subid0AboveWater_stateC_hook(GB *gb) {
  BASE(octogon_subid0AboveWater_stateC);
  CYC(b_+0, b_+2); B = 0x18;
  TAIL(octogon_subid0AboveWater_turningAround); // fallthrough
}

void octogon_subid0AboveWater_turningAround_hook(GB *gb) {
  BASE(octogon_subid0AboveWater_turningAround);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); mem_wr(gb, HL, B);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_add(gb, 0x04);
  CYC(b_+12, b_+14); alu_and(gb, 0x0c);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+18); TAIL(enemySetAnimation); // jp
}

// About to fire projectile?
void octogon_subid0AboveWater_stateD_hook(GB *gb) {
  BASE(octogon_subid0AboveWater_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x08);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0e
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_add(gb, 0x02);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+17); TAIL(enemySetAnimation); // jp
}

// Firing projectile
void octogon_subid0AboveWater_stateE_hook(GB *gb) {
  BASE(octogon_subid0AboveWater_stateE);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 40);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0f
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CALL_C(b_+12, enemySetAnimation_hook, SYM(enemySetAnimation), b_+15);
  CYC(b_+15, b_+18); TAIL(octogon_fireOctorokProjectile); // jp
}

// Turning around after firing projectile?
void octogon_subid0AboveWater_state10_hook(GB *gb) {
  BASE(octogon_subid0AboveWater_state10);
  CYC(b_+0, b_+2); B = 0x0c;
  CYCT(b_+2, b_+4); TAIL(octogon_subid0AboveWater_turningAround); // jr
}

// Delay before resuming normal movement
void octogon_subid0AboveWater_state11_hook(GB *gb) {
  BASE(octogon_subid0AboveWater_state11);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x09); // [state]
  RET(b_+7); return; // ret
}

// Octogon code where octogon itself is below water, and link is above water
void octogon_subid0BelowWater_hook(GB *gb) {
  BASE(octogon_subid0BelowWater);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); alu_sub(gb, 0x08);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == SYM(octogon_subid0BelowWater_state8)) { octogon_subid0BelowWater_state8_hook(gb); return; }
    if (target == SYM(octogon_subid0BelowWater_state9)) { octogon_subid0BelowWater_state9_hook(gb); return; }
    if (target == SYM(octogon_subid0BelowWater_stateA)) { octogon_subid0BelowWater_stateA_hook(gb); return; }
    if (target == SYM(octogon_subid0BelowWater_stateB)) { octogon_subid0BelowWater_stateB_hook(gb); return; }
    if (target == SYM(octogon_subid0BelowWater_stateC)) { octogon_subid0BelowWater_stateC_hook(gb); return; }
    if (target == SYM(octogon_subid0BelowWater_stateD)) { octogon_subid0BelowWater_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// Swimming normally
void octogon_subid0BelowWater_state8_hook(GB *gb) {
  BASE(octogon_subid0BelowWater_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto L_7722; } // jr nz
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x88; // SND_LINK_SWIM
  CALL_C(b_+13, playSound_b00_hook, SYM(playSound_b00), b_+16);

L_7722:
  CALL_C(b_+16, octogon_decVar36IfNonzero_hook, SYM(octogon_decVar36IfNonzero), b_+19);
  if (!(F & FZ)) { CYCT(b_+19, b_+22); octogon_moveTowardTargetPosition_hook(gb); return; } // jp nz
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+24); mem_wr(gb, HL, 90); // [var36]
  CALL_C(b_+24, getRandomNumber_hook, SYM(getRandomNumber), b_+27);
  CYC(b_+27, b_+29); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+29, b_+32); octogon_moveTowardTargetPosition_hook(gb); return; } // jp nc
  CYC(b_+29, b_+32);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x0a);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 60);
  CYCT(b_+40, b_+42); TAIL(octogon_loadNormalSubmergedAnimation); // jr
}

// Waiting in place before moving again
void octogon_subid0BelowWater_state9_hook(GB *gb) {
  BASE(octogon_subid0BelowWater_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, octogon_subid0_pauseMovement_hook, SYM(octogon_subid0AboveWater_stateA), SYM(octogon_animate));
  TAIL(octogon_animate); // fallthrough
}

void octogon_animate_hook(GB *gb) {
  BASE(octogon_animate);
  CYC(b_+0, b_+3); TAIL(enemyAnimate); // jp
}

// Delay before firing projectile
void octogon_subid0BelowWater_stateA_hook(GB *gb) {
  BASE(octogon_subid0BelowWater_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto beginFiring; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_xor(gb, 0x01);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+18); TAIL(enemySetAnimation); // jp

beginFiring:
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x08);
  CYC(b_+20, b_+21); L = E;
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0b
  TAIL(octogon_loadNormalSubmergedAnimation); // fallthrough
}

void octogon_loadNormalSubmergedAnimation_hook(GB *gb) {
  BASE(octogon_loadNormalSubmergedAnimation);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+4); A = 0x12;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+8); TAIL(enemySetAnimation); // jp
}

// Firing projectile
void octogon_subid0BelowWater_stateB_hook(GB *gb) {
  BASE(octogon_subid0BelowWater_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto fireProjectile; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x06);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+11, b_+13); A = 0x14;
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+17); TAIL(enemySetAnimation); // jp

fireProjectile:
  CYC(b_+17, b_+19); mem_wr(gb, HL, 60); // [counter1]
  CYC(b_+19, b_+20); L = E;
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c
  CYC(b_+21, b_+23); B = 0x48; // PART_OCTOGON_DEPTH_CHARGE
  CALL_C(b_+23, ecom_spawnProjectile_b0f_hook, SYM(ecom_spawnProjectile_b0f), b_+26);
  CYCT(b_+26, b_+28); TAIL(octogon_loadNormalSubmergedAnimation); // jr
}

// Delay before moving again
void octogon_subid0BelowWater_stateC_hook(GB *gb) {
  BASE(octogon_subid0BelowWater_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); octogon_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 90);
  CYC(b_+9, b_+10); L = E;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x08); // [state]
  CYCT(b_+12, b_+14); TAIL(octogon_animate); // jr
}

// Just submerged into water
void octogon_subid0BelowWater_stateD_hook(GB *gb) {
  BASE(octogon_subid0BelowWater_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 30);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x08); // [state]
  CYCT(b_+9, b_+11); TAIL(octogon_loadNormalSubmergedAnimation); // jr
}

// Link is below water
void octogon_subid1_hook(GB *gb) {
  BASE(octogon_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(b_+18, b_+21); octogon_subid1_aboveWater_hook(gb); return; } // jp z
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+23); alu_sub(gb, 0x08);
  CYC(b_+23, b_+24); push_effect(gb, b_+24);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == SYM(octogon_subid1_belowWater_state8)) { octogon_subid1_belowWater_state8_hook(gb); return; }
    if (target == SYM(octogon_subid1_belowWater_state9)) { octogon_subid1_belowWater_state9_hook(gb); return; }
    if (target == SYM(octogon_subid1_belowWater_stateA)) { octogon_subid1_belowWater_stateA_hook(gb); return; }
    if (target == SYM(octogon_subid1_belowWater_stateB)) { octogon_subid1_belowWater_stateB_hook(gb); return; }
    if (target == SYM(octogon_subid1_belowWater_stateC)) { octogon_subid1_belowWater_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// Normal movement (moving toward some target position decided already)
void octogon_subid1_belowWater_state8_hook(GB *gb) {
  BASE(octogon_subid1_belowWater_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, octogon_decVar36IfNonzero_hook, SYM(octogon_decVar36IfNonzero), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto normalMovement; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 90);
  CALL_C(b_+7, getRandomNumber_hook, SYM(getRandomNumber), b_+10);
  CYC(b_+10, b_+11); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto normalMovement; } // jr nc
  CYC(b_+11, b_+13);
  CALL_C(b_+13, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+16);
  CYC(b_+16, b_+18); alu_add(gb, 0x04);
  CYC(b_+18, b_+20); alu_and(gb, 0x18);
  CYC(b_+20, b_+21); B = A;
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); alu_add(gb, 0x04);
  CYC(b_+26, b_+28); alu_and(gb, 0x18);
  CYC(b_+28, b_+29); alu_cp(gb, B);
  CYC(b_+29, b_+30); H = D;
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto normalMovement; } // jr nz
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x0b);
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x08);
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+45); alu_and(gb, 0x0c);
  CYC(b_+45, b_+47); alu_add(gb, 0x02);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CYC(b_+48, b_+51); TAIL(enemySetAnimation); // jp

normalMovement:
  CYC(b_+51, b_+54); A = mem_rd(gb, wFrameCounter);
  CYC(b_+54, b_+56); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto L_77f7; } // jr nz
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+60, b_+61); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+61, b_+64); octogon_beginRisingAboveWater_hook(gb); return; } // jp z
  CYC(b_+61, b_+64);

L_77f7:
  CALL_C(b_+64, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+67);
  if (!(F & FZ)) { CYCT(b_+67, b_+69); octogon_updateMovementAndAnimation_hook(gb); return; } // jr nz
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 60); // [counter1]
  CYC(b_+71, b_+73); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+73, b_+74); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+74); return; // ret
}

// Moves toward target position and updates animation + sound effects accordingly
void octogon_updateMovementAndAnimation_hook(GB *gb) {
  BASE(octogon_updateMovementAndAnimation);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, octogon_moveTowardTargetPosition_hook, SYM(octogon_moveTowardTargetPosition), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_VAR35;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x0c);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_xor(gb, 0x01);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CALL_C(b_+16, enemySetAnimation_hook, SYM(enemySetAnimation), b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); alu_or(gb, A);
  CYC(b_+23, b_+25); A = 0x88; // SND_LINK_SWIM
  if (!(F & FZ)) { CYCT(b_+25, b_+28); playSound_b00_hook(gb); return; } // jp nz
  CYC(b_+25, b_+28);
  TAIL(octogon_doSplashAnimation); // fallthrough
}

// Above-water only (subid 0)
void octogon_doSplashAnimation_hook(GB *gb) {
  BASE(octogon_doSplashAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x6b; // SND_SWORDSPIN
  CALL_C(b_+2, playSound_b00_hook, SYM(playSound_b00), b_+5);
  CALL_C(b_+5, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+8);
  if (!(F & FZ)) { RET_TAKEN(b_+8); return; } // ret nz
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x8e); // INTERAC_OCTOGON_SPLASH
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_and(gb, 0x0c);
  CYC(b_+16, b_+18); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+22); TAIL(objectCopyPosition); // jp
}

// Waiting in place until counter1 reaches 0, then will charge at Link.
void octogon_subid1_belowWater_state9_hook(GB *gb) {
  BASE(octogon_subid1_belowWater_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x0c);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(b_+8, b_+10); L = ENEMY_BASE + 0x31; // var31
  CYC(b_+10, b_+12); A = mem_rd(gb, hEnemyTargetY); // hEnemyTargetY
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+13, b_+15); A = mem_rd(gb, hEnemyTargetX); // hEnemyTargetX
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CALL_C(b_+16, ecom_updateAngleTowardTarget_b0f_hook, SYM(ecom_updateAngleTowardTarget_b0f), b_+19);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); alu_and(gb, 0x0c);
  CYC(b_+25, b_+26); alu_add(gb, A);
  CYC(b_+26, b_+27); B = A;
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_sub(gb, B);
  CYC(b_+31, b_+33); alu_and(gb, 0x1f);
  CYC(b_+33, b_+34); B = A;
  CYC(b_+34, b_+36); alu_sub(gb, 0x04);
  CYC(b_+36, b_+38); alu_cp(gb, 0x18);
  if (!(F & FC)) { RET_TAKEN(b_+38); return; } // ret nc
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); alu_bit(gb, 4, B);
  CYC(b_+41, b_+43); A = 0x04;
  if (F & FZ) { CYCT(b_+43, b_+45); goto L_7863; } // jr z
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); A = 0x0c;

L_7863:
  CYC(b_+47, b_+48); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+48, b_+50); alu_and(gb, 0x0c);
  CYC(b_+50, b_+51); mem_wr(gb, HL, A);
  CYC(b_+51, b_+54); TAIL(enemySetAnimation); // jp
}

// Waiting for a split second before charging
void octogon_subid1_belowWater_stateA_hook(GB *gb) {
  BASE(octogon_subid1_belowWater_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 90);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x08); // [state]
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+12, b_+14); alu_add(gb, 0x04);
  CYC(b_+14, b_+16); alu_and(gb, 0x18);
  CYC(b_+16, b_+17); alu_rrca(gb);
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+21); TAIL(enemySetAnimation); // jp
}

// Delay before firing bubble
void octogon_subid1_belowWater_stateB_hook(GB *gb) {
  BASE(octogon_subid1_belowWater_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 60);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CALL_C(b_+12, enemySetAnimation_hook, SYM(enemySetAnimation), b_+15);
  CALL_C(b_+15, getFreePartSlot_hook, SYM(getFreePartSlot), b_+18);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto L_7898; } // jr nz
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x55); // PART_OCTOGON_BUBBLE
  CALL_C(b_+22, octogon_initializeProjectile_hook, SYM(octogon_initializeProjectile), b_+25);

L_7898:
  CYC(b_+25, b_+28); TAIL(octogon_doSplashAnimation); // jp
}

// Delay after firing bubble
void octogon_subid1_belowWater_stateC_hook(GB *gb) {
  BASE(octogon_subid1_belowWater_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x08); // [state]
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_and(gb, 0x0c);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+16); TAIL(enemySetAnimation); // jp
}

// Octogon is above water, but Link is below water
void octogon_subid1_aboveWater_hook(GB *gb) {
  BASE(octogon_subid1_aboveWater);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); alu_sub(gb, 0x08);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = octogon_jump_table(gb);
    if (target == b_+9) goto state8;
    if (target == b_+20) goto state9;
    if (target == b_+45) goto stateA;
    HANDOFF(target);
  }

state8:
  CALL_C(b_+9, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 120);
  CYC(b_+15, b_+17); B = 0x48; // PART_OCTOGON_DEPTH_CHARGE
  CYC(b_+17, b_+20); TAIL(ecom_spawnProjectile_b0f); // jp

state9:
  CALL_C(b_+20, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+23);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; } // ret nz
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); L = E;
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+28, b_+30); A = 0x11;
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CALL_C(b_+31, enemySetAnimation_hook, SYM(enemySetAnimation), b_+34);
  CYC(b_+34, b_+36); A = 0x8f; // SND_ENEMY_JUMP
  CALL_C(b_+36, playSound_b00_hook, SYM(playSound_b00), b_+39);
  CYC(b_+39, b_+42); SET_BC(0x0208);
  CYC(b_+42, b_+45); TAIL(enemyBoss_spawnShadow_b0f); // jp

stateA:
  CYC(b_+45, b_+46); H = D;
  CYC(b_+46, b_+48); L = ENEMY_BASE + OBJ_Z;
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+51); alu_sub(gb, 0xc0);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+55); alu_sbc(gb, 0x00);
  CYC(b_+55, b_+56); mem_wr(gb, HL, A);
  CYC(b_+56, b_+58); alu_cp(gb, 0xd0);
  if (!(F & FC)) { RET_TAKEN(b_+58); return; } // ret nc
  CYC(b_+58, b_+59);
  CYC(b_+59, b_+61); alu_cp(gb, 0xc0);
  if (!(F & FZ)) { CYCT(b_+61, b_+64); ecom_flickerVisibility_b0f_hook(gb); return; } // jp nz
  CYC(b_+61, b_+64);
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x00);
  CYC(b_+66, b_+67); L = E;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x08); // [state] = 8
  CYC(b_+69, b_+71); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+71, b_+73); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+73, b_+75); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+75, b_+77); mem_wr(gb, HL, 60);
  CALL_C(b_+77, objectSetInvisible_hook, SYM(objectSetInvisible), b_+80);
  CYC(b_+80, b_+83); TAIL(ecom_killRelatedObj1_b0f); // jp
}

// Invisible collision box for the shell
void octogon_subid2_hook(GB *gb) {
  BASE(octogon_subid2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x08; // Object.direction
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+8, b_+10); goto L_790a; } // jr c
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); A = 0x08;

L_790a:
  CYC(b_+12, b_+14); alu_and(gb, 0x0c);
  CYC(b_+14, b_+15); push_effect(gb, HL);
  CYC(b_+15, b_+18); SET_HL(b_+35); // @data
  CYC(b_+18, b_+19); octogon_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); E = alu_inc8(gb, E);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+27, b_+28); C = mem_rd(gb, HL);
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+30); SET_HL(pop_effect(gb));
  CALL_C(b_+30, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+33);
  CYC(b_+33, b_+34); SET_HL(pop_effect(gb));
  RET(b_+34); return; // ret
}

void octogon_subid0_submergeIntoWater_hook(GB *gb) {
  BASE(octogon_subid0_submergeIntoWater);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x0d);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+7, b_+9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x01);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x10);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 90);
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+23, b_+25); A = 0x15;
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
  CYC(b_+26, b_+29); TAIL(enemySetAnimation); // jp
}

void octogon_beginRisingAboveWater_hook(GB *gb) {
  BASE(octogon_beginRisingAboveWater);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x09);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 30);
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 90);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+19, b_+21); A = 0x10;
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+25); TAIL(enemySetAnimation); // jp
}

// Takes current position, fixes it to the closest valid spot above water, and decides
// a value for var30 (target position index).
void octogon_fixPositionAboveWater_hook(GB *gb) {
  BASE(octogon_fixPositionAboveWater);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_octogonBoss_posNeedsFixing);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+9); mem_wr(gb, wTmpcfc0_octogonBoss_posNeedsFixing, A);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+16); C = mem_rd(gb, HL);
  CALL_C(b_+16, octogon_getClosestTargetPositionIndex_hook, SYM(octogon_getClosestTargetPositionIndex), b_+19);
  CYC(b_+19, b_+20); E = L;
  CYC(b_+20, b_+23); A = mem_rd(gb, w1Link_yh);
  CYC(b_+23, b_+24); B = A;
  CYC(b_+24, b_+27); A = mem_rd(gb, w1Link_xh);
  CYC(b_+27, b_+28); C = A;
  CALL_C(b_+28, octogon_getClosestTargetPositionIndex_hook, SYM(octogon_getClosestTargetPositionIndex), b_+31);
  CYC(b_+31, b_+32); alu_cp(gb, L); // BUG: should compare against e, not a
  CYC(b_+32, b_+33); A = L;
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto L_798f; } // jr nz
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+38); SET_HL(b_+103); // @linkCompensationIndices
  CYC(b_+38, b_+39); octogon_addAToHl_from_rst(gb, b_+39);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);

L_798f:
  CYC(b_+40, b_+41); alu_add(gb, A);
  CYC(b_+41, b_+44); SET_HL(b_+67); // @data
  CYC(b_+44, b_+45); octogon_addDoubleIndexToHl_from_rst(gb, b_+45);
  CYC(b_+45, b_+47); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+47, b_+48); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+51); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+51, b_+52); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+55); E = ENEMY_BASE + 0x30; // var30
  CYC(b_+55, b_+56); A = mem_rd(gb, HL);
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+58); H = D;
  CYC(b_+58, b_+59); L = E;
  CYC(b_+59, b_+61); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+61, b_+64); octogon_chooseRandomTargetPosition_hook(gb); return; } // jp nz
  CYC(b_+61, b_+64);
  TAIL(octogon_loadTargetPosition); // jp
}

void octogon_fireOctorokProjectile_hook(GB *gb) {
  BASE(octogon_fireOctorokProjectile);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x18); // PART_OCTOROK_PROJECTILE
  TAIL(octogon_initializeProjectile); // fallthrough
}

// @param h Projectile (could be PART_OCTOROK_PROJECTILE or PART_OCTOGON_BUBBLE)
void octogon_initializeProjectile_hook(GB *gb) {
  BASE(octogon_initializeProjectile);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x0c);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+7); alu_add(gb, A);
  CYC(b_+7, b_+9); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+11); A = B;
  CYC(b_+11, b_+12); alu_rrca(gb);
  CYC(b_+12, b_+13); push_effect(gb, HL);
  CYC(b_+13, b_+16); SET_HL(b_+29); // @positionOffsets
  CYC(b_+16, b_+17); octogon_addAToHl_from_rst(gb, b_+17);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+18, b_+19); B = A;
  CYC(b_+19, b_+20); C = mem_rd(gb, HL);
  CYC(b_+20, b_+21); SET_HL(pop_effect(gb));
  CALL_C(b_+21, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+24);
  CYC(b_+24, b_+26); A = 0xa6; // SND_STRIKE
  CYC(b_+26, b_+29); TAIL(playSound_b00); // jp
}

void octogon_decVar36IfNonzero_hook(GB *gb) {
  BASE(octogon_decVar36IfNonzero);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+7); return; // ret
}

// Moves toward position stored in var31/var32. Increments state and sets counter1 to 30
// when it reaches that position.
void octogon_moveTowardTargetPosition_hook(GB *gb) {
  BASE(octogon_moveTowardTargetPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x31; // var31
  CALL_C(b_+3, ecom_readPositionVars_b0f_hook, SYM(ecom_readPositionVars_b0f), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);
  CYC(b_+7, b_+8); A = alu_inc8(gb, A);
  CYC(b_+8, b_+10); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+10, b_+13); ecom_moveTowardPosition_b0f_hook(gb); return; } // jp nc
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); A = mem_rd(gb, hFF8F); // hFF8F
  CYC(b_+15, b_+16); alu_sub(gb, B);
  CYC(b_+16, b_+17); A = alu_inc8(gb, A);
  CYC(b_+17, b_+19); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+19, b_+22); ecom_moveTowardPosition_b0f_hook(gb); return; } // jp nc
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+24, b_+25); mem_wr(gb, HL, B);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+27, b_+28); mem_wr(gb, HL, C);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 30);
  RET(b_+35); return; // ret
}

// Given a position, this determines the "target position index" (value for var30) which
// that position most closely corresponds to.
// @param bc Position
// @param[out] a
// @param[out] e
void octogon_getClosestTargetPositionIndex_hook(GB *gb) {
  BASE(octogon_getClosestTargetPositionIndex);
  CYC(b_+0, b_+2); E = 0x00;

checkY:
  CYC(b_+2, b_+3); A = B;
  CYC(b_+3, b_+5); alu_cp(gb, 0x40);
  if (F & FC) { CYCT(b_+5, b_+7); goto checkX; } // jr c
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = 0x03;
  CYC(b_+9, b_+11); alu_cp(gb, 0x70);
  if (F & FC) { CYCT(b_+11, b_+13); goto checkX; } // jr c
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); E = 0x06;

checkX:
  CYC(b_+15, b_+16); A = C;
  CYC(b_+16, b_+18); alu_cp(gb, 0x50);
  if (F & FC) { CYCT(b_+18, b_+20); goto L_7a48; } // jr c
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); E = alu_inc8(gb, E);
  CYC(b_+21, b_+23); alu_cp(gb, 0xa0);
  if (F & FC) { CYCT(b_+23, b_+25); goto L_7a48; } // jr c
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); E = alu_inc8(gb, E);

L_7a48:
  CYC(b_+26, b_+27); A = E;
  CYC(b_+27, b_+29); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; } // ret nz
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); E = 0x00;
  CYC(b_+32, b_+33); A = B;
  CYC(b_+33, b_+35); alu_cp(gb, 0x58);
  if (F & FC) { CYCT(b_+35, b_+37); goto L_7a55; } // jr c
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); E = 0x06;

L_7a55:
  CYC(b_+39, b_+40); A = C;
  CYC(b_+40, b_+42); alu_cp(gb, 0x78);
  if (F & FC) { RET_TAKEN(b_+42); return; } // ret c
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+44); E = alu_inc8(gb, E);
  CYC(b_+44, b_+45); E = alu_inc8(gb, E);
  RET(b_+45); return; // ret
}
