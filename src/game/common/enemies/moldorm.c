#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void moldorm_state_uninitialized_hook(GB *gb);
void moldorm_state1_hook(GB *gb);
void moldorm_state_stub_hook(GB *gb);
void moldorm_head_hook(GB *gb);
void moldorm_tail_hook(GB *gb);
void moldorm_tail_delete_hook(GB *gb);
void moldorm_tail_setRelatedObj1AndCopyPosition_hook(GB *gb);
void moldorm_head_updateAnimationFromAngle_hook(GB *gb);
void moldorm_tail_clearOffsetBuffer_hook(GB *gb);
void moldorm_checkHazards_hook(GB *gb);

static uint16_t moldorm_jump_table(GB *gb) {
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
// ENEMY_MOLDORM
//
// Variables for head (subid 1):
//   var30: Tail 1 object index
//   var31: Tail 2 object index
//   var32: Animation index
//   var33: Angular speed (added to angle)
//
// Variables for tail (subids 2-3):
//   relatedObj1: Object to follow (either the head or the tail in front)
//   var30: Index for offset buffer
//   var31/var32: Parent object's position last frame
//   var33-var3b: Offset buffer. Stores the parent's movement offsets for up to 8 frames.
// ==================================================================================================
void enemyCode4f_hook(GB *gb) {
  BASE(enemyCode4f);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(0), moldorm_checkHazards_hook, SYM(moldorm_checkHazards), b_+OE(3));
  if (F & FZ) { CYCT(b_+O(3), b_+OE(5)); goto normalStatus; } // jr z
  CYC(b_+O(3), b_+OE(5));
  CYC(b_+O(5), b_+OE(7)); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+O(7)); return; } // ret c
  CYC(b_+O(7), b_+OE(8));
  if (F & FZ) { CYCT(b_+O(8), b_+OE(10)); goto dead; } // jr z
  CYC(b_+O(8), b_+OE(10));
  CYC(b_+O(10), b_+OE(11)); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(11), b_+OE(13)); goto knockback; } // jr nz
  CYC(b_+O(11), b_+OE(13));
  CYC(b_+O(13), b_+OE(15)); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+O(15), b_+OE(16)); A = mem_rd(gb, DE);
  CYC(b_+O(16), b_+OE(17)); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(17), b_+OE(19)); goto normalStatus; } // jr nz
  CYC(b_+O(17), b_+OE(19));
  CYC(b_+O(19), b_+OE(21)); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+O(21), b_+OE(22)); L = E;
  CYC(b_+O(22), b_+OE(23)); A = mem_rd(gb, DE);
  CYC(b_+O(23), b_+OE(24)); B = A;
  CYC(b_+O(24), b_+OE(26)); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+O(26), b_+OE(27)); A = mem_rd(gb, DE);
  CYC(b_+O(27), b_+OE(28)); H = A;
  CYC(b_+O(28), b_+OE(29)); mem_wr(gb, HL, B);
  CYC(b_+O(29), b_+OE(30)); E = alu_inc8(gb, E);
  CYC(b_+O(30), b_+OE(31)); A = mem_rd(gb, DE);
  CYC(b_+O(31), b_+OE(32)); H = A;
  CYC(b_+O(32), b_+OE(33)); mem_wr(gb, HL, B);
  RET(b_+O(33)); return; // ret

dead:
  CYC(b_+O(34), b_+OE(36)); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+O(36), b_+OE(37)); A = mem_rd(gb, DE);
  CYC(b_+O(37), b_+OE(38)); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(38), b_+OE(41)); moldorm_tail_delete_hook(gb); return; } // jp nz
  CYC(b_+O(38), b_+OE(41));
  CYC(b_+O(41), b_+OE(43)); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+O(43), b_+OE(44)); A = mem_rd(gb, DE);
  CYC(b_+O(44), b_+OE(45)); H = A;
  CALL_C(b_+O(45), ecom_killObjectH_b0e_hook, SYM(ecom_killObjectH_b0e), b_+OE(48));
  CYC(b_+O(48), b_+OE(49)); E = alu_inc8(gb, E);
  CYC(b_+O(49), b_+OE(50)); A = mem_rd(gb, DE);
  CYC(b_+O(50), b_+OE(51)); H = A;
  CALL_C(b_+O(51), ecom_killObjectH_b0e_hook, SYM(ecom_killObjectH_b0e), b_+OE(54));
  if (game_seasons) {
    CYC(b_+S(54), b_+S(57)); A = W8(wActiveRoom);
    CYC(b_+S(57), b_+S(59)); alu_cp(gb, 0xf4); // <ROOM_SEASONS_0f4
    if (!(F & FZ)) { CYCT(b_+S(59), b_+S(61)); goto die; }
    CYC(b_+S(59), b_+S(61));
    CYC(b_+S(61), b_+S(64)); A = W8(wActiveGroup);
    CYC(b_+S(64), b_+S(65)); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+S(65), b_+S(67)); goto die; }
    CYC(b_+S(65), b_+S(67));
    CYC(b_+S(67), b_+S(68)); A = alu_inc8(gb, A);
    CYC(b_+S(68), b_+S(71)); W8(wRoomLayoutEnd) = A;
  }
die:
  CYC(b_+O(54), b_+OE(57)); TAIL(enemyDie); // jp

knockback:
  CYC(b_+O(57), b_+OE(59)); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+O(59), b_+OE(60)); A = mem_rd(gb, DE);
  CYC(b_+O(60), b_+OE(61)); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(61), b_+OE(63)); goto normalStatus; } // jr nz
  CYC(b_+O(61), b_+OE(63));
  CYC(b_+O(63), b_+OE(66)); TAIL(ecom_updateKnockbackAndCheckHazards_b0e); // jp

normalStatus:
  CALL_C(b_+O(66), ecom_getSubidAndCpStateTo08_b0e_hook, SYM(ecom_getSubidAndCpStateTo08_b0e), b_+OE(69));
  if (!(F & FC)) { CYCT(b_+O(69), b_+OE(71)); goto normalState; } // jr nc
  CYC(b_+O(69), b_+OE(71));
  {
    CYC(b_+O(71), b_+OE(72)); push_effect(gb, b_+OE(72));
    uint16_t target = moldorm_jump_table(gb);
    if (target == SYM(moldorm_state_uninitialized)) { moldorm_state_uninitialized_hook(gb); return; }
    if (target == SYM(moldorm_state1)) { moldorm_state1_hook(gb); return; }
    if (target == SYM(moldorm_state_stub)) { moldorm_state_stub_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(b_+O(88), b_+OE(89)); B = alu_dec8(gb, B);
  CYC(b_+O(89), b_+OE(90)); A = B;
  {
    CYC(b_+O(90), b_+OE(91)); push_effect(gb, b_+OE(91));
    uint16_t target = moldorm_jump_table(gb);
    if (target == SYM(moldorm_head)) { moldorm_head_hook(gb); return; }
    if (target == SYM(moldorm_tail)) { moldorm_tail_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:61f8, bare global; jump-table target from enemyCode4f.
void moldorm_state_uninitialized_hook(GB *gb) {
  BASE(moldorm_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+4); goto notSpawner; } // jr nz
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+5); A = alu_inc8(gb, A);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A); // [state] = 1
  CYC(b_+6, b_+8); TAIL(moldorm_state1); // jr

notSpawner:
  CALL_C(b_+8, ecom_setSpeedAndState8AndVisible_b0e_hook, SYM(ecom_setSpeedAndState8AndVisible_b0e), b_+11);
  CYC(b_+11, b_+12); A = B;
  CYC(b_+12, b_+13); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+13); return; } // ret z
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); alu_add(gb, 0x07);
  CYC(b_+16, b_+19); TAIL(enemySetAnimation); // jp
}

// 0e:620b, bare global; jump-table target from enemyCode4f, also falls into from
// moldorm_state_uninitialized. Spawner; spawn the head and tails, then delete self.
void moldorm_state1_hook(GB *gb) {
  BASE(moldorm_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0x03;
  CALL_C(b_+2, checkBEnemySlotsAvailable_hook, SYM(checkBEnemySlotsAvailable), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); objectSetVisible82_hook(gb); return; } // jp nz
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); B = 0x4f; // ENEMY_MOLDORM
  CALL_C(b_+10, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0e), b_+13);
  CYC(b_+13, b_+14); C = H;
  PUSH(b_+14, HL);
  CALL_C(b_+15, ecom_spawnEnemyWithSubid01_b0e_hook, SYM(ecom_spawnEnemyWithSubid01_b0e), b_+18);
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid] = 2
  CALL_C(b_+19, moldorm_tail_setRelatedObj1AndCopyPosition_hook, SYM(moldorm_tail_setRelatedObj1AndCopyPosition), b_+22);
  CYC(b_+22, b_+23); C = H;
  CALL_C(b_+23, ecom_spawnEnemyWithSubid01_b0e_hook, SYM(ecom_spawnEnemyWithSubid01_b0e), b_+26);
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+28); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid] = 3
  CALL_C(b_+28, moldorm_tail_setRelatedObj1AndCopyPosition_hook, SYM(moldorm_tail_setRelatedObj1AndCopyPosition), b_+31);
  CYC(b_+31, b_+32); B = H;
  SET_HL(POP(b_+32));
  CYC(b_+33, b_+35); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+35, b_+36); mem_wr(gb, HL, C);
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+38); mem_wr(gb, HL, B);
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+40, b_+41); E = L;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A);
  CALL_C(b_+43, objectCopyPosition_hook, SYM(objectCopyPosition), b_+46);
  CYC(b_+46, b_+49); TAIL(enemyDelete); // jp
}

// 0e:623c, bare global; jump-table target from enemyCode4f.
void moldorm_state_stub_hook(GB *gb) {
  BASE(moldorm_state_stub);
  RET(b_+0); return; // ret
}

// 0e:623d, bare global; jump-table target from enemyCode4f@normalState. Subid 1.
void moldorm_head_hook(GB *gb) {
  BASE(moldorm_head);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = moldorm_jump_table(gb);
    if (target == b_+8) goto state8;
    if (target == b_+29) goto state9;
    HANDOFF(target);
  }

state8:
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+10); L = E;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x08);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x02);
  CALL_C(b_+23, ecom_setRandomAngle_b0e_hook, SYM(ecom_setRandomAngle_b0e), b_+26);
  CYC(b_+26, b_+29); TAIL(moldorm_head_updateAnimationFromAngle); // jp

state9:
  CALL_C(b_+29, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+32);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto applySpeed; } // jr nz
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(b_+36, b_+38); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+38, b_+40); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+42); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+42, b_+44); alu_and(gb, 0x1f);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CALL_C(b_+45, moldorm_head_updateAnimationFromAngle_hook, SYM(moldorm_head_updateAnimationFromAngle), b_+48);
  CALL_C(b_+48, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+51);
  CYC(b_+51, b_+53); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+53, b_+55); goto applySpeed; } // jr nz
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+57); E = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+59); alu_cpl(gb);
  CYC(b_+59, b_+60); A = alu_inc8(gb, A);
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);

applySpeed:
  CALL_C(b_+61, ecom_bounceOffWallsAndHoles_b0e_hook, SYM(ecom_bounceOffWallsAndHoles_b0e), b_+64);
  if (!(F & FZ)) CALL_C_CC(b_+64, moldorm_head_updateAnimationFromAngle_hook, SYM(moldorm_head_updateAnimationFromAngle), b_+67); else CYC(b_+64, b_+67); // call nz
  CYC(b_+67, b_+70); TAIL(objectApplySpeed); // jp
}

// 0e:6283, bare global; jump-table target from enemyCode4f@normalState.
void moldorm_tail_hook(GB *gb) {
  BASE(moldorm_tail);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x08);
  {
    CYC(b_+5, b_+6); push_effect(gb, b_+6);
    uint16_t target = moldorm_jump_table(gb);
    if (target == b_+10) goto state8;
    if (target == b_+33) goto state9;
    HANDOFF(target);
  }

state8:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+13, b_+15); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+15, b_+17); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+19, b_+20); H = mem_rd(gb, HL);
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+22, b_+24); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+27); E = alu_inc8(gb, E);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+33); TAIL(moldorm_tail_clearOffsetBuffer); // jp

state9:
  CYC(b_+33, b_+35); A = OBJ_ENABLED; // Object.enabled
  CALL_C(b_+35, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+38);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL);
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+40, b_+42); moldorm_tail_delete_hook(gb); return; } // jr z
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+44); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+44, b_+46); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); B = A;
  CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+49, b_+50); alu_sub(gb, B);
  CYC(b_+50, b_+52); alu_add(gb, 0x08);
  CYC(b_+52, b_+54); A = alu_swap(gb, A);
  CYC(b_+54, b_+55); B = A;
  CYC(b_+55, b_+56); E = alu_inc8(gb, E);
  CYC(b_+56, b_+57); L = alu_inc8(gb, L);
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);
  CYC(b_+58, b_+59); C = A;
  CYC(b_+59, b_+60); A = mem_rd(gb, HL);
  CYC(b_+60, b_+61); alu_sub(gb, C);
  CYC(b_+61, b_+63); alu_add(gb, 0x08);
  CYC(b_+63, b_+64); alu_or(gb, B);
  CYC(b_+64, b_+65); B = A;
  CYC(b_+65, b_+66); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+66, b_+67); mem_wr(gb, DE, A);
  CYC(b_+67, b_+68); E = alu_dec8(gb, E);
  CYC(b_+68, b_+69); L = alu_dec8(gb, L);
  CYC(b_+69, b_+70); A = mem_rd(gb, HL);
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CYC(b_+71, b_+73); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+76); alu_add(gb, ENEMY_BASE + OBJ_VAR33); // add Enemy.var33
  CYC(b_+76, b_+77); E = A;
  CYC(b_+77, b_+78); A = B;
  CYC(b_+78, b_+79); mem_wr(gb, DE, A);
  CYC(b_+79, b_+80); H = D;
  CYC(b_+80, b_+82); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+82, b_+84); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); A = alu_inc8(gb, A);
  CYC(b_+86, b_+88); alu_and(gb, 0x07);
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  CYC(b_+89, b_+91); alu_add(gb, ENEMY_BASE + OBJ_VAR33); // add Enemy.var33
  CYC(b_+91, b_+92); E = A;
  CYC(b_+92, b_+93); A = mem_rd(gb, DE);
  CYC(b_+93, b_+94); B = A;
  CYC(b_+94, b_+96); alu_and(gb, 0xf0);
  CYC(b_+96, b_+98); A = alu_swap(gb, A);
  CYC(b_+98, b_+100); alu_sub(gb, 0x08);
  CYC(b_+100, b_+101); alu_add(gb, mem_rd(gb, HL)); // [yh]
  CYC(b_+101, b_+102); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+102, b_+103); L = alu_inc8(gb, L);
  CYC(b_+103, b_+104); A = B;
  CYC(b_+104, b_+106); alu_and(gb, 0x0f);
  CYC(b_+106, b_+108); alu_sub(gb, 0x08);
  CYC(b_+108, b_+109); alu_add(gb, mem_rd(gb, HL)); // [xh]
  CYC(b_+109, b_+110); mem_wr(gb, HL, A);
  RET(b_+110); return; // ret
}

// 0e:62f2, bare global; called from enemyCode4f and moldorm_tail.
void moldorm_tail_delete_hook(GB *gb) {
  BASE(moldorm_tail_delete);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, decNumEnemies_hook, SYM(decNumEnemies), b_+3);
  CYC(b_+3, b_+6); TAIL(enemyDelete); // jp
}

// 0e:62f8, bare global; called from moldorm_state1 for each spawned tail.
// @param h Object to follow (either the head or the tail in front)
void moldorm_tail_setRelatedObj1AndCopyPosition_hook(GB *gb) {
  BASE(moldorm_tail_setRelatedObj1AndCopyPosition);
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+2, b_+4); A = ENEMY_BASE; // Enemy.start
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+5, b_+6); mem_wr(gb, HL, C);
  CYC(b_+6, b_+9); TAIL(objectCopyPosition); // jp
}

// 0e:6301, bare global; called from moldorm_head.
void moldorm_head_updateAnimationFromAngle_hook(GB *gb) {
  BASE(moldorm_head_updateAnimationFromAngle);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_add(gb, 0x02);
  CYC(b_+5, b_+7); alu_and(gb, 0x1c);
  CYC(b_+7, b_+8); alu_rrca(gb);
  CYC(b_+8, b_+9); alu_rrca(gb);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+12, b_+13); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+13); return; } // ret z
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+18); TAIL(enemySetAnimation); // jp
}

// 0e:6313, bare global; called from moldorm_tail.
void moldorm_tail_clearOffsetBuffer_hook(GB *gb) {
  BASE(moldorm_tail_clearOffsetBuffer);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VAR33;
  CYC(b_+3, b_+5); B = 0x02;
  CYC(b_+5, b_+7); A = 0x88;

L_631a:
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(b_+11, b_+12); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto L_631a; } // jr nz
  CYC(b_+12, b_+14);
  RET(b_+14); return; // ret
}

// 0e:6322, bare global; called from enemyCode4f.
void moldorm_checkHazards_hook(GB *gb) {
  BASE(moldorm_checkHazards);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto checkHazards; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = OBJ_VAR3F; // Object.var3f
  CALL_C(b_+9, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto checkHazards; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); A = B;
  CYC(b_+18, b_+19); alu_or(gb, A);
  RET(b_+19); return; // ret

checkHazards:
  CYC(b_+20, b_+21); A = B;
  CYC(b_+21, b_+24); TAIL(ecom_checkHazardsNoAnimationForHoles_b0e); // jp
}
