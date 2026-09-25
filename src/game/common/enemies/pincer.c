#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void pincer_state_uninitialized_hook(GB *gb);
void pincer_state1_hook(GB *gb);
void pincer_state_stub_hook(GB *gb);
void pincer_head_hook(GB *gb);
void pincer_head_state8_hook(GB *gb);
void pincer_head_state9_hook(GB *gb);
void pincer_head_stateA_hook(GB *gb);
void pincer_head_stateB_hook(GB *gb);
void pincer_head_stateC_hook(GB *gb);
void pincer_head_stateD_hook(GB *gb);
void pincer_head_stateE_hook(GB *gb);
void pincer_body_hook(GB *gb);
void pincer_setChildRelatedObj1_hook(GB *gb);
void pincer_updatePosition_hook(GB *gb);
void pincer_body_updateExtendedAmount_hook(GB *gb);

static uint16_t pincer_jump_table(GB *gb) {
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

// 0e:5feb, local sub-label of pincer_body_updateExtendedAmount reached by genuine call/ret.
static uint8_t pincer_getExtendedAmountDividedByFour(GB *gb) {
  BASE(pincer_body_updateExtendedAmount);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+37, b_+38); E = L;
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+41); A = alu_srl(gb, A);
  CYC(b_+41, b_+43); A = alu_srl(gb, A);
  CYC(b_+43, b_+44);
  return A;
}

// ==================================================================================================
// ENEMY_PINCER
//
// Variables:
//   relatedObj1: Pointer to "head", aka subid 1 (only for body parts, subids 2+)
//   var31/var32: Base Y/X position (where it originates from)
//   var33: Amount extended (0 means still in hole)
//   var34: Copy of parent's "id" value. For body parts only.
// ==================================================================================================
void enemyCode45_hook(GB *gb) {
  BASE(enemyCode45);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);

normalStatus:
  CALL_C(b_+8, ecom_getSubidAndCpStateTo08_b0e_hook, SYM(ecom_getSubidAndCpStateTo08_b0e), b_+11);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto normalState; } // jr nc
  CYC(b_+11, b_+13);
  {
    CYC(b_+13, b_+14); push_effect(gb, b_+14);
    uint16_t target = pincer_jump_table(gb);
    if (target == SYM(pincer_state_uninitialized)) { pincer_state_uninitialized_hook(gb); return; }
    if (target == SYM(pincer_state1)) { pincer_state1_hook(gb); return; }
    if (target == SYM(pincer_state_stub)) { pincer_state_stub_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+30, b_+31); B = alu_dec8(gb, B); // dec b
  CYC(b_+31, b_+32); A = B; // ld a,b
  {
    CYC(b_+32, b_+33); push_effect(gb, b_+33);
    uint16_t target = pincer_jump_table(gb);
    if (target == SYM(pincer_head)) { pincer_head_hook(gb); return; }
    if (target == SYM(pincer_body)) { pincer_body_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5e83, bare global; jump-table target from enemyCode45. Falls through into pincer_state1
// for the spawner (subid 0).
void pincer_state_uninitialized_hook(GB *gb) {
  BASE(pincer_state_uninitialized);
  CYC(b_+0, b_+1); A = B; // ld a,b
  CYC(b_+1, b_+2); alu_or(gb, A); // or a
  if (!(F & FZ)) { CYCT(b_+2, b_+5); ecom_setSpeedAndState8_b0e_hook(gb); return; } // jp nz
  CYC(b_+2, b_+5);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A); // inc a
  CYC(b_+6, b_+7); mem_wr(gb, DE, A); // [state] = 1
  TAIL(pincer_state1); // fallthrough
}

// 0e:5e8a, bare global; jump-table target from enemyCode45, also falls into from
// pincer_state_uninitialized. Spawner only (subid 0): spawn head and body parts, then delete self.
void pincer_state1_hook(GB *gb) {
  BASE(pincer_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x04;
  CALL_C(b_+2, checkBEnemySlotsAvailable_hook, SYM(checkBEnemySlotsAvailable), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); B = 0x45; // ENEMY_PINCER
  CALL_C(b_+8, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0e), b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+13, b_+14); E = L;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CALL_C(b_+16, objectCopyPosition_hook, SYM(objectCopyPosition), b_+19);
  CYC(b_+19, b_+20); C = H;
  CALL_C(b_+20, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0e), b_+23);
  CALL_C(b_+23, pincer_setChildRelatedObj1_hook, SYM(pincer_setChildRelatedObj1), b_+26);
  CALL_C(b_+26, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0e), b_+29);
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl), [child.subid]
  CALL_C(b_+30, pincer_setChildRelatedObj1_hook, SYM(pincer_setChildRelatedObj1), b_+33);
  CALL_C(b_+33, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0e), b_+36);
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl), [child.subid]
  CYC(b_+37, b_+38); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl), [child.subid]
  CALL_C(b_+38, pincer_setChildRelatedObj1_hook, SYM(pincer_setChildRelatedObj1), b_+41);
  CYC(b_+41, b_+44); TAIL(enemyDelete); // jp
}

// 0e:5eb6, bare global; jump-table target from enemyCode45.
void pincer_state_stub_hook(GB *gb) {
  BASE(pincer_state_stub);
  RET(b_+0); return; // ret
}

// 0e:5eb7, bare global; jump-table target from enemyCode45@normalState. Subid 1: head of
// pincer (the "main" part, which is attackable).
void pincer_head_hook(GB *gb) {
  BASE(pincer_head);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = pincer_jump_table(gb);
    if (target == SYM(pincer_head_state8)) { pincer_head_state8_hook(gb); return; }
    if (target == SYM(pincer_head_state9)) { pincer_head_state9_hook(gb); return; }
    if (target == SYM(pincer_head_stateA)) { pincer_head_stateA_hook(gb); return; }
    if (target == SYM(pincer_head_stateB)) { pincer_head_stateB_hook(gb); return; }
    if (target == SYM(pincer_head_stateC)) { pincer_head_stateC_hook(gb); return; }
    if (target == SYM(pincer_head_stateD)) { pincer_head_stateD_hook(gb); return; }
    if (target == SYM(pincer_head_stateE)) { pincer_head_stateE_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:5ec9, bare global; jump-table target from pincer_head. Initialization.
void pincer_head_state8_hook(GB *gb) {
  BASE(pincer_head_state8);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+7); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  RET(b_+13); return; // ret
}

// 0e:5ed7, bare global; jump-table target from pincer_head. Waiting for Link to approach.
void pincer_head_state9_hook(GB *gb) {
  BASE(pincer_head_state9);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x28;
  CALL_C(b_+2, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+5);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+8, b_+10); A = 0x0a;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+14); TAIL(objectSetVisible82); // jp
}

// 0e:5ee5, bare global; jump-table target from pincer_head. Showing eyes as a "warning" that
// it's about to attack.
void pincer_head_stateA_hook(GB *gb) {
  BASE(pincer_head_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(2)); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(4), b_+OE(7)); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+O(4), b_+OE(7));
  CALL_C(b_+O(7), ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+OE(10));
  CYC(b_+O(10), b_+OE(12)); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+O(12), b_+OE(14)); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+O(14), b_+OE(16)); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+O(16), b_+OE(18)); mem_wr(gb, HL, 0x00);
  CYC(b_+O(18), b_+OE(20)); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+O(20), b_+OE(21)); B = mem_rd(gb, HL);
  CYC(b_+O(21), b_+OE(23)); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+O(23), b_+OE(24)); C = mem_rd(gb, HL);
  CYC(b_+O(24), b_+OE(26)); A = 0x06; // BREAKABLETILESOURCE_SHOVEL
  CALL_C(b_+O(26), tryToBreakTile_hook, SYM(tryToBreakTile), b_+OE(29));
  if (!game_seasons) {
    CALL_C(b_+29, objectCheckTileAtPositionIsWater_hook, SYM(objectCheckTileAtPositionIsWater), b_+32);
    if (!(F & FC)) { CYCT(b_+32, b_+34); goto attack; } // jr nc
    CYC(b_+32, b_+34);
    CALL_C(b_+34, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+37);
    if (!(F & FZ)) { CYCT(b_+37, b_+39); goto attack; } // jr nz
    CYC(b_+37, b_+39);
    CYC(b_+39, b_+41); mem_wr(gb, HL, 0x03); // INTERAC_SPLASH
    CYC(b_+41, b_+44); SET_BC(0xfa00);
    CALL_C(b_+44, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+47);
  }

attack:
  CALL_C(b_+O(47), ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+OE(50));
  CYC(b_+O(50), b_+OE(52)); alu_add(gb, 0x02);
  CYC(b_+O(52), b_+OE(54)); alu_and(gb, 0x1c);
  CYC(b_+O(54), b_+OE(55)); alu_rrca(gb);
  CYC(b_+O(55), b_+OE(56)); alu_rrca(gb);
  CYC(b_+O(56), b_+OE(57)); A = alu_inc8(gb, A);
  CYC(b_+O(57), b_+OE(60)); TAIL(enemySetAnimation); // jp
}

// 0e:5f21, bare global; jump-table target from pincer_head. Extending toward target.
void pincer_head_stateB_hook(GB *gb) {
  BASE(pincer_head_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, pincer_updatePosition_hook, SYM(pincer_updatePosition), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_add(gb, 0x02);
  CYC(b_+8, b_+10); alu_cp(gb, 0x20);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto fullyExtended; } // jr nc
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return; // ret

fullyExtended:
  CALL_C(b_+14, ecom_incState_b0e_hook, SYM(ecom_incState_b0e), b_+17);
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x08);
  RET(b_+21); return; // ret
}

// 0e:5f37, bare global; jump-table target from pincer_head. Staying fully extended for
// several frames.
void pincer_head_stateC_hook(GB *gb) {
  BASE(pincer_head_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  RET(b_+6); return; // ret
}

// 0e:5f3e, bare global; jump-table target from pincer_head. Retracting.
void pincer_head_stateD_hook(GB *gb) {
  BASE(pincer_head_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, pincer_updatePosition_hook, SYM(pincer_updatePosition), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 30);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+17, b_+19); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+19, b_+22); TAIL(objectSetInvisible); // jp
}

// 0e:5f54, bare global; jump-table target from pincer_head. Fully retracted; on cooldown.
void pincer_head_stateE_hook(GB *gb) {
  BASE(pincer_head_stateE);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x09); // [state]
  CYC(b_+7, b_+9); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+21); TAIL(enemySetAnimation); // jp
}

// 0e:5f69, bare global; jump-table target from enemyCode45@normalState. Subid 2-4: body of
// pincer (just decoration).
void pincer_body_hook(GB *gb) {
  BASE(pincer_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = pincer_jump_table(gb);
    if (target == b_+8) goto state8;
    if (target == b_+35) goto state9;
    HANDOFF(target);
  }

state8:
  CYC(b_+8, b_+10); A = 0x09;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A); // [state] = 9
  CYC(b_+11, b_+13); A = OBJ_YH; // Object.yh
  CALL_C(b_+13, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+22); E = alu_inc8(gb, E);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  CYC(b_+24, b_+26); E = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_ID;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+32); A = 0x09;
  CYC(b_+32, b_+35); TAIL(enemySetAnimation); // jp

state9:
  CYC(b_+35, b_+37); A = OBJ_ID; // Object.id
  CALL_C(b_+37, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+40);
  CYC(b_+40, b_+42); E = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+44, b_+47); enemyDelete_hook(gb); return; } // jp nz
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+49, b_+50); E = L;
  CYC(b_+50, b_+51); A = mem_rd(gb, HL);
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+54); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+54, b_+55); E = L;
  CYC(b_+55, b_+56); A = mem_rd(gb, HL);
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+59); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+59, b_+60); A = mem_rd(gb, HL);
  CYC(b_+60, b_+62); alu_cp(gb, 0x0b);
  if (F & FC) { CYCT(b_+62, b_+64); goto updateExtendedAmount; } // jr c
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+66); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+66, b_+67); E = L;
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);

updateExtendedAmount:
  CALL_C(b_+69, pincer_body_updateExtendedAmount_hook, SYM(pincer_body_updateExtendedAmount), b_+72);
  CYC(b_+72, b_+74); TAIL(pincer_updatePosition); // jr
}

// 0e:5fb3, bare global; called from pincer_state1 for each of the 3 body parts. Sets
// relatedObj1 of object 'h' to object 'c'. 'h' is part of the pincer's body, 'c' is the
// pincer's head. Also increments the body part's subid since that does need to be done.
void pincer_setChildRelatedObj1_hook(GB *gb) {
  BASE(pincer_setChildRelatedObj1);
  CYC(b_+0, b_+1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid]++
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+3, b_+5); A = ENEMY_BASE; // Enemy.start
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+6, b_+7); mem_wr(gb, HL, C);
  RET(b_+7); return; // ret
}

// 0e:5fbb, bare global; called from pincer_body and both pincer_head extend/retract states.
// Updates position based on "base position" (var31), angle, and distance extended (var33).
void pincer_updatePosition_hook(GB *gb) {
  BASE(pincer_updatePosition);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); C = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+10, b_+13); TAIL(objectSetPositionInCircleArc); // jp
}

// 0e:5fc8, bare global; called from pincer_body. Calculates value for var33 (amount extended)
// for a body part.
void pincer_body_updateExtendedAmount_hook(GB *gb) {
  BASE(pincer_body_updateExtendedAmount);
  uint16_t sp0_ = gb->sp;
  PUSH(b_+0, HL);
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+6); alu_sub(gb, 0x02);
  {
    CYC(b_+6, b_+7); push_effect(gb, b_+7);
    uint16_t target = pincer_jump_table(gb);
    if (target == b_+13) goto subid2;
    if (target == b_+22) goto subid3;
    if (target == b_+29) goto subid4;
    HANDOFF(target);
  }

subid2:
  SET_HL(POP(b_+13));
  CYC(b_+14, b_+17); A = pincer_getExtendedAmountDividedByFour(gb);
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+19); alu_add(gb, A); // add a,a
  CYC(b_+19, b_+20); alu_add(gb, B); // add a,b
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return; // ret

subid3:
  SET_HL(POP(b_+22));
  CYC(b_+23, b_+26); A = pincer_getExtendedAmountDividedByFour(gb);
  CYC(b_+26, b_+27); alu_add(gb, A); // add a,a
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  RET(b_+28); return; // ret

subid4:
  SET_HL(POP(b_+29));
  CYC(b_+30, b_+33); A = pincer_getExtendedAmountDividedByFour(gb);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  RET(b_+34); return; // ret
}
