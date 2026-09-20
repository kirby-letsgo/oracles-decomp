#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode35), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode35), (from), (to), true)

void floormaster_state_uninitialized_hook(GB *gb);
void floormaster_state1_hook(GB *gb);
void floormaster_state_galeSeed_hook(GB *gb);
void floormaster_state_stub_hook(GB *gb);
void floormaster_state8_hook(GB *gb);
void floormaster_state9_hook(GB *gb);
void floormaster_stateA_hook(GB *gb);
void floormaster_stateB_hook(GB *gb);
void floormaster_animate_hook(GB *gb);
void floormaster_stateC_hook(GB *gb);
void floormaster_stateD_hook(GB *gb);
void floormaster_updateAngleTowardLink_hook(GB *gb);
void floormaster_updateZPosition_hook(GB *gb);
void floormaster_checkLinkMoved8PixelsAway_hook(GB *gb);
void floormaster_initSpawner_hook(GB *gb);
void floormaster_getAdjacentWallsBitset_hook(GB *gb);

static uint16_t floormaster_jump_table(GB *gb) {
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

static void floormaster_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_FLOORMASTER
//
// Variables for subids other than 0:
//   relatedObj1: Reference to spawner object (subid 0)
//   var30: Animation index
//   var31: Index for z-position to use while chasing Link (0-7)
//   var32: Angle relative to Link where floormaster should spawn
//
// Variables for spawner (subid 0):
//   var30: Number of floormaster currently spawned (they delete themselves after
//          disappearing into the ground)
//   var31/var32: Link's position last time a floormaster was spawned. If Link hasn't moved
//                far from here, the floormaster will spawn at a random angle relative to
//                him; otherwise it will spawn in the direction Link is moving.
//   var33: # floormasters to spawn. Children decrement this when they're killed.
//          (High nibble of original Y value.)
//   var34: Subid for child objects (high nibble of original X value, plus one)
// ==================================================================================================
void enemyCode35_hook(GB *gb) {
  BASE(enemyCode35);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; } // ret nz
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x0c);
  CYC(b_+22, b_+24); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0xfb);
  CALL_C(b_+26, floormaster_updateAngleTowardLink_hook, SYM(floormaster_updateAngleTowardLink), b_+29);
  CYC(b_+29, b_+31); alu_add(gb, 0x04);
  CALL_C(b_+31, enemySetAnimation_hook, SYM(enemySetAnimation), b_+34);
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+37, b_+40); A = mem_rd(gb, w1Link_yh);
  CYC(b_+40, b_+41); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+41, b_+43); A = alu_sra(gb, A);
  CYC(b_+43, b_+44); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+47, b_+50); A = mem_rd(gb, w1Link_xh);
  CYC(b_+50, b_+51); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+51, b_+53); A = alu_sra(gb, A);
  CYC(b_+53, b_+54); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+54, b_+55); mem_wr(gb, HL, A);
  RET(b_+55); return; // ret

dead:
  CYC(b_+56, b_+58); A = 0x30; // Object.var30
  CALL_C(b_+58, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+61);
  CYC(b_+61, b_+62); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+64, b_+65); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+65, b_+68); enemyDie_uncounted_hook(gb); return; // jp

normalStatus:
  CYC(b_+68, b_+70); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+70, b_+71); A = mem_rd(gb, DE);
  {
    CYC(b_+71, b_+72); push_effect(gb, b_+72);
    uint16_t target = floormaster_jump_table(gb);
    if (target == SYM(floormaster_state_uninitialized)) { floormaster_state_uninitialized_hook(gb); return; }
    if (target == SYM(floormaster_state1)) { floormaster_state1_hook(gb); return; }
    if (target == SYM(floormaster_state_stub)) { floormaster_state_stub_hook(gb); return; }
    if (target == SYM(floormaster_state_galeSeed)) { floormaster_state_galeSeed_hook(gb); return; }
    if (target == SYM(floormaster_state8)) { floormaster_state8_hook(gb); return; }
    if (target == SYM(floormaster_state9)) { floormaster_state9_hook(gb); return; }
    if (target == SYM(floormaster_stateA)) { floormaster_stateA_hook(gb); return; }
    if (target == SYM(floormaster_stateB)) { floormaster_stateB_hook(gb); return; }
    if (target == SYM(floormaster_stateC)) { floormaster_stateC_hook(gb); return; }
    if (target == SYM(floormaster_stateD)) { floormaster_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

void floormaster_state_uninitialized_hook(GB *gb) {
  BASE(floormaster_state_uninitialized);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 60);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state] = 1
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+14); floormaster_initSpawner_hook(gb); return; } // jp z
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x08); // [state] = 8
  RET(b_+16); return; // ret
}

// State 1: only for subid 0 (spawner).
void floormaster_state1_hook(GB *gb) {
  BASE(floormaster_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto delete_; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+11); return; } // ret nc
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { RET_TAKEN(b_+15); return; } // ret nz
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x01);
  CYC(b_+18, b_+20); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(b_+23); return; } // ret nc
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); B = 0x35; // ENEMY_FLOORMASTER
  CALL_C(b_+26, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0e), b_+29);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; } // ret nz
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); E = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); // [child.subid] = [this.var34]
  CYC(b_+34, b_+36); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+36, b_+38); A = 0x80; // Enemy.start
  CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+39, b_+40); mem_wr(gb, HL, D);
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+43); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x80);
  RET(b_+48); return; // ret

delete_:
  CALL_C(b_+49, decNumEnemies_hook, SYM(decNumEnemies), b_+52);
  CALL_C(b_+52, markEnemyAsKilledInRoom_b00_hook, SYM(markEnemyAsKilledInRoom_b00), b_+55);
  CYC(b_+55, SYM(floormaster_state_galeSeed)); enemyDelete_hook(gb); return; // jp
}

void floormaster_state_galeSeed_hook(GB *gb) {
  BASE(floormaster_state_galeSeed);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_galeSeedEffect_b0e_hook, SYM(ecom_galeSeedEffect_b0e), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x30; // Object.var30
  CALL_C(b_+6, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+9);
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+13, SYM(floormaster_state_stub)); enemyDelete_hook(gb); return; // jp
}

void floormaster_state_stub_hook(GB *gb) {
  BASE(floormaster_state_stub);
  RET(b_+0); return; // ret
}

// Choosing a position to spawn at.
void floormaster_state8_hook(GB *gb) {
  BASE(floormaster_state8);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, floormaster_checkLinkMoved8PixelsAway_hook, SYM(floormaster_checkLinkMoved8PixelsAway), b_+3);
  CYC(b_+3, b_+5); A = 0x00;
  PUSH(b_+5, BC);
  if (F & FC) CALL_C_CC(b_+6, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+9); else CYC(b_+6, b_+9); // call c
  SET_BC(POP(b_+9));
  CYC(b_+10, b_+11); E = A;
  CYC(b_+11, b_+14); A = mem_rd(gb, w1Link_angle);
  CYC(b_+14, b_+15); alu_add(gb, E);
  CYC(b_+15, b_+17); alu_and(gb, 0x1f);
  CYC(b_+17, b_+19); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+22); A = 0x50;
  CYC(b_+22, b_+24); hram_wr(gb, 0x8a, A);

tryDistance:
  CYC(b_+24, b_+26); A = hram_rd(gb, 0x8a);
  CYC(b_+26, b_+28); alu_sub(gb, 0x10);
  if (F & FZ) { CYCT(b_+28, b_+30); goto doneLoop; } // jr z
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); hram_wr(gb, 0x8a, A);
  PUSH(b_+32, BC);
  CYC(b_+33, b_+35); E = ENEMY_BASE + 0x32; // Enemy.var32
  CALL_C(b_+35, objectSetPositionInCircleArc_hook, SYM(objectSetPositionInCircleArc), b_+38);
  SET_BC(POP(b_+38));
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); E = A;
  CYC(b_+41, b_+44); A = mem_rd(gb, w1Link_xh);
  CYC(b_+44, b_+45); alu_sub(gb, E);
  if (!(F & FC)) { CYCT(b_+45, b_+47); goto L_4bf8; } // jr nc
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+48); alu_cpl(gb);
  CYC(b_+48, b_+49); A = alu_inc8(gb, A);

L_4bf8:
  CYC(b_+49, b_+51); alu_cp(gb, 0x80);
  if (!(F & FC)) { CYCT(b_+51, b_+53); goto tryDistance; } // jr nc
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+58); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (!(F & FC)) { CYCT(b_+58, b_+60); goto tryDistance; } // jr nc
  CYC(b_+58, b_+60);
  PUSH(b_+60, BC);
  CALL_C(b_+61, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+64);
  SET_BC(POP(b_+64));
  if (!(F & FZ)) { CYCT(b_+65, b_+67); goto tryDistance; } // jr nz
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+68); H = D;
  CYC(b_+68, b_+70); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x09);
  CYC(b_+72, b_+74); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x20);
  CALL_C(b_+76, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+79);
  CYC(b_+79, b_+80); B = A;
  CYC(b_+80, b_+82); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+84); A = alu_dec8(gb, A);
  CYC(b_+84, b_+85); A = B;
  if (!(F & FZ)) { CYCT(b_+85, b_+87); goto L_4c22; } // jr nz
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+89); alu_add(gb, 0x04);
  CYC(b_+89, b_+91); alu_and(gb, 0x18);

L_4c22:
  CYC(b_+91, b_+93); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+93, b_+94); mem_wr(gb, DE, A);
  CYC(b_+94, b_+96); alu_cp(gb, 0x10);
  CYC(b_+96, b_+98); A = 0x00;
  if (!(F & FC)) { CYCT(b_+98, b_+100); goto L_4c2c; } // jr nc
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+101); A = alu_inc8(gb, A);

L_4c2c:
  CYC(b_+101, b_+103); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+103, b_+104); mem_wr(gb, DE, A);
  CALL_C(b_+104, enemySetAnimation_hook, SYM(enemySetAnimation), b_+107);
  CALL_C(b_+107, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+110);

doneLoop:
  CYC(b_+110, b_+112); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+112, b_+113); A = mem_rd(gb, DE);
  CYC(b_+113, b_+114); H = A;
  CYC(b_+114, b_+116); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+116, b_+119); A = mem_rd(gb, w1Link_yh);
  CYC(b_+119, b_+120); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+120, b_+123); A = mem_rd(gb, w1Link_xh);
  CYC(b_+123, b_+124); mem_wr(gb, HL, A);
  RET(b_+124); return; // ret
}

// Emerging from ground
void floormaster_state9_hook(GB *gb) {
  BASE(floormaster_state9);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+7); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+11); A = 0x0a;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_add(gb, 0x02);
  CYC(b_+17, SYM(floormaster_stateA)); enemySetAnimation_hook(gb); return; // jp
}

// Floating in place for [counter1] frames before chasing Link
void floormaster_stateA_hook(GB *gb) {
  BASE(floormaster_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto beginChasing; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); A = alu_srl(gb, A);
  CYC(b_+8, b_+10); A = alu_srl(gb, A);
  CYC(b_+10, b_+13); SET_HL(b_+58); // @zVals (data)
  CYC(b_+13, b_+14); floormaster_addAToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  RET(b_+18); return; // ret

beginChasing:
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0xf0); // [counter1]
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+23, b_+25); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x0b);
  CALL_C(b_+29, floormaster_updateAngleTowardLink_hook, SYM(floormaster_updateAngleTowardLink), b_+32);
  CYC(b_+32, b_+33); B = A;
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); H = A;
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+39, b_+41); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(b_+41, b_+42); H = D;
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x0f); // SPEED_60
  if (F & FZ) { CYCT(b_+46, b_+48); goto L_4c8a; } // jr z
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x19); // SPEED_a0

L_4c8a:
  CYC(b_+50, b_+51); A = B;
  CYC(b_+51, b_+53); alu_add(gb, 0x02);
  CALL_C(b_+53, enemySetAnimation_hook, SYM(enemySetAnimation), b_+56);
  CYCT(b_+56, b_+58); floormaster_animate_hook(gb); return; // jr
}

// Chasing Link
void floormaster_stateB_hook(GB *gb) {
  BASE(floormaster_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto stillChasing; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+11, b_+13); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x0d); // [state]
  CYC(b_+16, b_+18); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+18, b_+20); A = 0x06;
  CYC(b_+20, b_+21); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+21, b_+24); enemySetAnimation_hook(gb); return; // jp

stillChasing:
  CYC(b_+24, b_+26); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+29); hram_wr(gb, 0x8d, A);
  CALL_C(b_+29, floormaster_updateAngleTowardLink_hook, SYM(floormaster_updateAngleTowardLink), b_+32);
  CYC(b_+32, b_+33); B = A;
  CYC(b_+33, b_+35); A = hram_rd(gb, 0x8d);
  CYC(b_+35, b_+36); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+36, b_+38); goto L_4cc6; } // jr z
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+40); A = 0x02;
  CYC(b_+40, b_+41); alu_add(gb, B);
  CALL_C(b_+41, enemySetAnimation_hook, SYM(enemySetAnimation), b_+44);

L_4cc6:
  CALL_C(b_+44, floormaster_updateZPosition_hook, SYM(floormaster_updateZPosition), b_+47);
  CALL_C(b_+47, floormaster_getAdjacentWallsBitset_hook, SYM(floormaster_getAdjacentWallsBitset), b_+50);
  CALL_C(b_+50, ecom_applyVelocityGivenAdjacentWalls_b0e_hook, SYM(ecom_applyVelocityGivenAdjacentWalls_b0e), SYM(floormaster_animate));
  floormaster_animate_hook(gb); return; // fallthrough
}

void floormaster_animate_hook(GB *gb) {
  BASE(floormaster_animate);
  CYC(b_+0, SYM(floormaster_stateC)); enemyAnimate_hook(gb); return; // jp
}

// Grabbing Link
void floormaster_stateC_hook(GB *gb) {
  BASE(floormaster_stateC);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto makeLinkInvisible; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto setZToZero; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); floormaster_animate_hook(gb); return; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = 0x02;
  CYC(b_+14, b_+17); mem_wr(gb, w1Link_substate, A);
  CYC(b_+17, b_+20); objectSetInvisible_hook(gb); return; // jp

makeLinkInvisible: // [animParameter] == 1
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+24); mem_wr(gb, w1Link_visible, A);
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+26, b_+29); A = mem_rd(gb, w1Link_yh);
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+32, b_+35); A = mem_rd(gb, w1Link_xh);
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  RET(b_+36); return; // ret

setZToZero: // [animParameter] == 2
  CYC(b_+37, b_+38); alu_xor(gb, A);
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CYC(b_+39, b_+41); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYCT(b_+42, SYM(floormaster_stateD)); floormaster_animate_hook(gb); return; // jr
}

// Sinking into ground
void floormaster_stateD_hook(GB *gb) {
  BASE(floormaster_stateD);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); floormaster_animate_hook(gb); return; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_RELATED1 + 1;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); H = A;
  CYC(b_+11, b_+13); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CYC(b_+14, SYM(floormaster_updateAngleTowardLink)); enemyDelete_hook(gb); return; // jp
}

// @param[out]  a  Value written to var30 (0 if Link is to the left, 1 if right)
void floormaster_updateAngleTowardLink_hook(GB *gb) {
  BASE(floormaster_updateAngleTowardLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); push_effect(gb, b_+3); // call @checkLinkCollisionsEnabled (inlined local sub-label; not independently hookable)
  goto checkLinkCollisionsEnabled;

afterCheck:
  if (!(F & FC)) { RET_TAKEN(b_+3); return; } // ret nc
  CYC(b_+3, b_+4);
  CALL_C(b_+4, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+7);
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+10); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto L_4d23; } // jr nz
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+18); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  RET(b_+19); return; // ret

L_4d23:
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); A = alu_dec8(gb, A);
  CYC(b_+24, b_+25); A = B;
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto subid0; } // jr nz
  CYC(b_+25, b_+27);

  // subid1: only move in cardinal directions
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+29, b_+31); alu_and(gb, 0xf8);
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34); alu_cp(gb, 0x10);
  CYC(b_+34, b_+36); A = 0x00;
  if (!(F & FC)) { CYCT(b_+36, b_+38); goto L_4d36; } // jr nc
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);

L_4d36:
  CYC(b_+39, b_+41); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  RET(b_+42); return; // ret

subid0:
  CYC(b_+43, b_+45); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CYC(b_+46, b_+48); alu_cp(gb, 0x10);
  CYC(b_+48, b_+50); A = 0x00;
  if (!(F & FC)) { CYCT(b_+50, b_+52); goto L_4d44; } // jr nc
  CYC(b_+50, b_+52);
  CYC(b_+52, b_+53); A = alu_inc8(gb, A);

L_4d44:
  CYC(b_+53, b_+55); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  RET(b_+56); return; // ret

checkLinkCollisionsEnabled:
  CYC(b_+57, b_+60); A = mem_rd(gb, w1Link_collisionType);
  CYC(b_+60, b_+61); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+61); goto afterCheck; } // ret c
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+64); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  RET(b_+65); goto afterCheck; // ret
}

void floormaster_updateZPosition_hook(GB *gb) {
  BASE(floormaster_updateZPosition);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+12); alu_and(gb, 0x07);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+16); SET_HL(b_+22); // @zVals (data)
  CYC(b_+16, b_+17); floormaster_addAToHl_from_rst(gb, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_ZH;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return; // ret
}

// Checks whether Link has moved 8 pixels away from his position last time a floormaster
// was spawned.
//
// @param[out]  bc  Link's position
// @param[out]  cflag  c if he's within 8 pixels
void floormaster_checkLinkMoved8PixelsAway_hook(GB *gb) {
  BASE(floormaster_checkLinkMoved8PixelsAway);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x31; // Object.var31
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, w1Link_yh);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+10); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+10, b_+12); alu_add(gb, 0x08);
  CYC(b_+12, b_+14); alu_cp(gb, 0x10);
  CYC(b_+14, b_+17); A = mem_rd(gb, w1Link_xh);
  CYC(b_+17, b_+18); C = A;
  if (!(F & FC)) { RET_TAKEN(b_+18); return; } // ret nc
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); L = alu_inc8(gb, L);
  CYC(b_+20, b_+21); alu_sub(gb, mem_rd(gb, HL)); // [var32]
  CYC(b_+21, b_+23); alu_add(gb, 0x08);
  CYC(b_+23, b_+25); alu_cp(gb, 0x10);
  RET(b_+25); return; // ret
}

void floormaster_initSpawner_hook(GB *gb) {
  BASE(floormaster_initSpawner);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0xf0);
  CYC(b_+5, b_+7); alu_swap_a(gb);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_and(gb, 0xf0);
  CYC(b_+15, b_+17); alu_swap_a(gb);
  CYC(b_+17, b_+18); A = alu_inc8(gb, A);
  CYC(b_+18, b_+20); E = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return; // ret
}

// Only screen boundaries count as "walls" for floormaster.
void floormaster_getAdjacentWallsBitset_hook(GB *gb) {
  BASE(floormaster_getAdjacentWallsBitset);
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, SYM(enemyCode36)); ecom_getTopDownAdjacentWallsBitset_b0e_hook(gb); return; // jp
}
