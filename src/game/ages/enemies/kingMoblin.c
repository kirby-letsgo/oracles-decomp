#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void enemyCode7f_hook(GB *gb);
void kingMoblin_state_uninitialized_hook(GB *gb);
void kingMoblin_state_stub_hook(GB *gb);
void kingMoblin_state8_hook(GB *gb);
void kingMoblin_animate_hook(GB *gb);
void kingMoblin_state9_hook(GB *gb);
void kingMoblin_stateA_hook(GB *gb);
void kingMoblin_stateB_hook(GB *gb);
void kingMoblin_initBombPickupAnimation_hook(GB *gb);
void kingMoblin_stateC_hook(GB *gb);
void kingMoblin_stateD_hook(GB *gb);
void kingMoblin_stateE_hook(GB *gb);
void kingMoblin_stateF_hook(GB *gb);
void kingMoblin_grabBomb_hook(GB *gb);
void kingMoblin_state10_hook(GB *gb);
void kingMoblin_animate2_hook(GB *gb);
void kingMoblin_state11_hook(GB *gb);
void kingMoblin_state12_hook(GB *gb);
void kingMoblin_state13_hook(GB *gb);
void kingMoblin_state14_hook(GB *gb);
void kingMoblin_state15_hook(GB *gb);
void kingMoblin_checkMoveToCentre_hook(GB *gb);
void kingMoblin_moveToCentre_hook(GB *gb);
void kingMoblin_setAngleStateAndAnimation_hook(GB *gb);
void kingMoblin_setStateAndAnimation_hook(GB *gb);

static uint16_t kingMoblin_jump_table(GB *gb) {
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

static void kingMoblin_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// kingMoblin_state_uninitialized@spawnMinion: reached only by a static `call` from
// kingMoblin_state_uninitialized itself (NOT HOOKED).
// @param[out] a,h Object index
static void kingMoblin_spawnMinion(GB *gb, uint16_t sp0_) {
  BASE(kingMoblin_state_uninitialized);
  CALL_C(b_+57, getFreeEnemySlot_uncounted_hook, SYM(getFreeEnemySlot_uncounted), b_+60);
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x56); // ENEMY_KING_MOBLIN_MINION
  CYC(b_+62, b_+64); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(b_+64, b_+66); A = 0x80; // Enemy.start
  CYC(b_+66, b_+67); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+67, b_+68); mem_wr(gb, HL, D);
  CYC(b_+68, b_+69); A = H;
  RET(b_+69); return; // ret
}

// ==================================================================================================
// ENEMY_KING_MOBLIN
//
// Variables:
//   counter2: ?
//   relatedObj2: Instance of PART_KING_MOBLIN_BOMB
//   var30/var31: Object indices for two ENEMY_KING_MOBLIN_MINION instances
//   var32: Target x-position to walk toward to grab bomb
//   var33: Signal from ENEMY_KING_MOBLIN_MINION to trigger warp to the outside
// ==================================================================================================
void enemyCode7f_hook(GB *gb) {
  BASE(enemyCode7f);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+12); alu_cp(gb, 0x98); // $80|ITEMCOLLISION_BOMB
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto normalStatus; } // jr nz
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); A = 0x63; // SND_BOSS_DAMAGE
  CALL_C(b_+16, playSound_b00_hook, SYM(playSound_b00), b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); A = alu_dec8(gb, A);
  CYC(b_+23, b_+26); SET_HL(b_+33); // @speeds
  CYC(b_+26, b_+27); kingMoblin_addAToHl_from_rst(gb, b_+27);
  CYC(b_+27, b_+29); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYCT(b_+31, b_+33); goto normalStatus; // jr

dead:
  CALL_C(b_+39, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+42);
  if (!(F & FC)) { RET_TAKEN(b_+42); return; } // ret nc
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+45); A = 0x01;
  CYC(b_+45, b_+48); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+48, b_+51); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+51, b_+52); H = D;
  CYC(b_+52, b_+54); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+54, b_+56); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+56, b_+58); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+58, b_+59); mem_wr(gb, HL, A); // [health] = $01
  CYC(b_+59, b_+61); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x12);
  CYC(b_+63, b_+65); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x00);
  CYC(b_+67, b_+69); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x78); // SPEED_300
  CYC(b_+71, b_+73); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x00);
  CYC(b_+75, b_+77); A = 0x06;
  CALL_C(b_+77, enemySetAnimation_hook, SYM(enemySetAnimation), b_+80);

normalStatus:
  CYC(b_+80, b_+82); E = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+84); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+84); return; } // ret nz
  CYC(b_+84, b_+85);
  CYC(b_+85, b_+87); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+87, b_+88); A = mem_rd(gb, DE);
  CYC(b_+88, b_+89); push_effect(gb, b_+89);
  {
    uint16_t target = kingMoblin_jump_table(gb);
    if (target == SYM(kingMoblin_state_uninitialized)) { kingMoblin_state_uninitialized_hook(gb); return; }
    if (target == SYM(kingMoblin_state_stub)) { kingMoblin_state_stub_hook(gb); return; }
    if (target == SYM(kingMoblin_state8)) { kingMoblin_state8_hook(gb); return; }
    if (target == SYM(kingMoblin_state9)) { kingMoblin_state9_hook(gb); return; }
    if (target == SYM(kingMoblin_stateA)) { kingMoblin_stateA_hook(gb); return; }
    if (target == SYM(kingMoblin_stateB)) { kingMoblin_stateB_hook(gb); return; }
    if (target == SYM(kingMoblin_stateC)) { kingMoblin_stateC_hook(gb); return; }
    if (target == SYM(kingMoblin_stateD)) { kingMoblin_stateD_hook(gb); return; }
    if (target == SYM(kingMoblin_stateE)) { kingMoblin_stateE_hook(gb); return; }
    if (target == SYM(kingMoblin_stateF)) { kingMoblin_stateF_hook(gb); return; }
    if (target == SYM(kingMoblin_state10)) { kingMoblin_state10_hook(gb); return; }
    if (target == SYM(kingMoblin_state11)) { kingMoblin_state11_hook(gb); return; }
    if (target == SYM(kingMoblin_state12)) { kingMoblin_state12_hook(gb); return; }
    if (target == SYM(kingMoblin_state13)) { kingMoblin_state13_hook(gb); return; }
    if (target == SYM(kingMoblin_state14)) { kingMoblin_state14_hook(gb); return; }
    if (target == SYM(kingMoblin_state15)) { kingMoblin_state15_hook(gb); return; }
    HANDOFF(target);
  }
}

void kingMoblin_state_uninitialized_hook(GB *gb) {
  BASE(kingMoblin_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x7f; // ENEMY_KING_MOBLIN
  CYC(b_+2, b_+5); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);
  CYC(b_+5, b_+7); A = 0x8c; // PALH_8c
  CALL_C(b_+7, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+10);
  CYC(b_+10, b_+12); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15);
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+19); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+19, b_+20); A = alu_dec8(gb, A);
  CYC(b_+20, b_+23); mem_wr(gb, wActiveMusic, A);
  CYC(b_+23, b_+25); B = 0x02;
  CALL_C(b_+25, checkBEnemySlotsAvailable_hook, SYM(checkBEnemySlotsAvailable), b_+28);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; } // ret nz
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+32); push_effect(gb, b_+32); kingMoblin_spawnMinion(gb, sp0_);
  CYC(b_+32, b_+34); E = ENEMY_BASE + 0x30; // var30
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+38); push_effect(gb, b_+38); kingMoblin_spawnMinion(gb, sp0_);
  CYC(b_+38, b_+40); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+41, b_+43); E = ENEMY_BASE + 0x31; // var31
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+46); A = 0x1e; // SPEED_c0
  CALL_C(b_+46, ecom_setSpeedAndState8_b0f_hook, SYM(ecom_setSpeedAndState8_b0f), b_+49);
  CALL_C(b_+49, objectSetVisible83_hook, SYM(objectSetVisible83), b_+52);
  CYC(b_+52, b_+54); A = 0x02;
  CYC(b_+54, b_+57); enemySetAnimation_hook(gb); return; // jp
}

void kingMoblin_state_stub_hook(GB *gb) {
  BASE(kingMoblin_state_stub);
  RET(b_+0); return; // ret
}

// Waiting for Link to move in to start the fight
void kingMoblin_state8_hook(GB *gb) {
  BASE(kingMoblin_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(w1Link_xh);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_sub(gb, 0x40);
  CYC(b_+6, b_+8); alu_cp(gb, 0x20);
  if (!(F & FC)) { CYCT(b_+8, b_+10); kingMoblin_animate_hook(gb); return; } // jr nc
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); L = 0x0f; // w1Link.zh's low byte
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); kingMoblin_animate_hook(gb); return; } // jr nz
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); L = 0x08; // w1Link.direction's low byte
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x00); // DIR_UP
  CALL_C(b_+20, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+23);
  if (!(F & FC)) { RET_TAKEN(b_+23); return; } // ret nc
  CYC(b_+23, b_+24);
  CALL_C(b_+24, clearAllParentItems_hook, SYM(clearAllParentItems), b_+27);
  CYC(b_+27, b_+29); A = 0x01;
  CYC(b_+29, b_+32); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+32, b_+35); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+35, b_+37); C = 0x61;
  CYC(b_+37, b_+39); A = 0xa0; // TILEINDEX_STANDARD_FLOOR
  CALL_C(b_+39, setTile_hook, SYM(setTile), b_+42);
  CALL_C(b_+42, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+45);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto L_7d65; } // jr nz
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x05); // INTERAC_PUFF
  CYC(b_+49, b_+51); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x68);
  CYC(b_+53, b_+55); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x18);

L_7d65:
  CALL_C(b_+57, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+60);
  CYC(b_+60, b_+62); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x18);
  kingMoblin_animate_hook(gb); return; // fallthrough
}

void kingMoblin_animate_hook(GB *gb) {
  BASE(kingMoblin_animate);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// Delay before showing text
void kingMoblin_state9_hook(GB *gb) {
  BASE(kingMoblin_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); kingMoblin_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); L = E;
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CALL_C(b_+7, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+10);
  CYC(b_+10, b_+13); SET_BC(0x2f19); // TX_2f19
  if (F & FZ) { CYCT(b_+13, b_+15); goto L_7d81; } // jr z
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); SET_BC(0x2f1a); // TX_2f1a

L_7d81:
  CYC(b_+18, b_+21); showText_hook(gb); return; // jp
}

// Starting fight
void kingMoblin_stateA_hook(GB *gb) {
  BASE(kingMoblin_stateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0b
  CYC(b_+3, b_+5); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 30);
  CYC(b_+7, b_+9); L = ENEMY_BASE + 0x30; // var30
  CYC(b_+9, b_+10); B = mem_rd(gb, HL);
  CYC(b_+10, b_+11); C = E;
  CYC(b_+11, b_+13); A = 0x02;
  CYC(b_+13, b_+14); mem_wr(gb, BC, A); // [minion1.state] = $02
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+16); B = mem_rd(gb, HL);
  CYC(b_+16, b_+17); mem_wr(gb, BC, A); // [minion2.state] = $02
  CALL_C(b_+17, enemyBoss_beginBoss_b0f_hook, SYM(enemyBoss_beginBoss_b0f), b_+20);
  CYC(b_+20, b_+21); alu_xor(gb, A);
  CYC(b_+21, b_+24); enemySetAnimation_hook(gb); return; // jp
}

// Facing backwards while picking up a bomb
void kingMoblin_stateB_hook(GB *gb) {
  BASE(kingMoblin_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); kingMoblin_animate_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); B = 0x3f; // PART_KING_MOBLIN_BOMB
  CALL_C(b_+7, ecom_spawnProjectile_b0f_hook, SYM(ecom_spawnProjectile_b0f), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);
  CALL_C(b_+11, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), SYM(kingMoblin_initBombPickupAnimation));
  kingMoblin_initBombPickupAnimation_hook(gb); return; // fallthrough
}

void kingMoblin_initBombPickupAnimation_hook(GB *gb) {
  BASE(kingMoblin_initBombPickupAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  CYC(b_+4, b_+7); SET_HL(b_+17); // @counter2Vals
  CYC(b_+7, b_+8); kingMoblin_addAToHl_from_rst(gb, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); A = 0x04;
  CYC(b_+14, b_+17); enemySetAnimation_hook(gb); return; // jp
}

// Will raise bomb over head in [counter2] frames?
void kingMoblin_stateC_hook(GB *gb) {
  BASE(kingMoblin_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, kingMoblin_checkMoveToCentre_hook, SYM(kingMoblin_checkMoveToCentre), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+7);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0d
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  CYC(b_+15, b_+18); SET_HL(b_+44); // @counter2Vals
  CYC(b_+18, b_+19); kingMoblin_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+25); A = OBJ_STATE; // Object.state
  CALL_C(b_+25, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+31); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+31, b_+33); goto L_7de8; } // jr z
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+36); SET_BC(0xf0f2);
  CALL_C(b_+36, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+39);

L_7de8:
  CYC(b_+39, b_+41); A = 0x02;
  CYC(b_+41, b_+44); enemySetAnimation_hook(gb); return; // jp
}

// Delay before throwing bomb
void kingMoblin_stateD_hook(GB *gb) {
  BASE(kingMoblin_stateD);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, kingMoblin_checkMoveToCentre_hook, SYM(kingMoblin_checkMoveToCentre), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+7);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } // ret nz
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 30);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0e
  CALL_C(b_+13, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+16);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+19); alu_sub(gb, 0x0c);
  CYC(b_+19, b_+21); alu_cp(gb, 0x07);
  if (F & FC) { CYCT(b_+21, b_+23); goto L_7e11; } // jr c
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); B = 0x0c;
  CYC(b_+25, b_+26); alu_rlca(gb);
  if (F & FC) { CYCT(b_+26, b_+28); goto L_7e11; } // jr c
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); B = 0x13;

L_7e11:
  CYC(b_+30, b_+32); A = OBJ_STATE; // Object.state
  CALL_C(b_+32, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+38); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+38, b_+40); goto L_7e27; } // jr z
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x03); // [bomb.state] = $03
  CYC(b_+42, b_+44); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+44, b_+45); mem_wr(gb, HL, B);
  CYC(b_+45, b_+47); L = PART_BASE + OBJ_SPEED_Z;
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x80); // <(-$180)
  CYC(b_+49, b_+50); L = alu_inc8(gb, L);
  CYC(b_+50, b_+52); mem_wr(gb, HL, 0xfe); // >(-$180)

L_7e27:
  CYC(b_+52, b_+54); A = 0x05;
  CYC(b_+54, b_+57); enemySetAnimation_hook(gb); return; // jp
}

// Delay after throwing bomb
void kingMoblin_stateE_hook(GB *gb) {
  BASE(kingMoblin_stateE);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = E;
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0f
  CYC(b_+6, b_+8); A = 0x02;
  CYC(b_+8, b_+11); enemySetAnimation_hook(gb); return; // jp
}

// Waiting for something to do
void kingMoblin_stateF_hook(GB *gb) {
  BASE(kingMoblin_stateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_ID; // Object.id
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x3f); // PART_KING_MOBLIN_BOMB
  if (!(F & FZ)) { CYCT(b_+8, b_+11); kingMoblin_moveToCentre_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); L = PART_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); kingMoblin_animate2_hook(gb); return; } // jr nz
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); L = PART_BASE + OBJ_YH;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+21, b_+23); alu_cp(gb, 0x36);
  if (F & FC) { CYCT(b_+23, b_+25); kingMoblin_animate2_hook(gb); return; } // jr nc
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); // [bomb.xh]
  CYC(b_+27, b_+29); alu_sub(gb, 0x30);
  CYC(b_+29, b_+31); alu_cp(gb, 0x41);
  if (F & FC) { CYCT(b_+31, b_+33); kingMoblin_animate2_hook(gb); return; } // jr nc
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); B = A;
  CYC(b_+37, b_+38); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+38, b_+40); alu_add(gb, 0x08);
  CYC(b_+40, b_+42); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(b_+42, b_+44); kingMoblin_grabBomb_hook(gb); return; } // jr c
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+46); alu_cp(gb, B);
  CYC(b_+46, b_+47); H = D;
  CYC(b_+47, b_+49); L = ENEMY_BASE + 0x32; // var32
  CYC(b_+49, b_+50); mem_wr(gb, HL, A);
  CYC(b_+50, b_+52); B = 0x11; // state $11
  CYC(b_+52, b_+55); kingMoblin_setAngleStateAndAnimation_hook(gb); return; // jp
}

void kingMoblin_grabBomb_hook(GB *gb) {
  BASE(kingMoblin_grabBomb);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_STATE; // Object.state
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x05);
  if (F & FZ) { CYCT(b_+8, b_+10); goto L_7e80; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x01);
  CYC(b_+12, b_+15); SET_BC(0x0800);
  CALL_C(b_+15, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+18);

L_7e80:
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x0c);
  CYC(b_+23, b_+26); kingMoblin_initBombPickupAnimation_hook(gb); return; // jp
}

// Moving to centre of screen
void kingMoblin_state10_hook(GB *gb) {
  BASE(kingMoblin_state10);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x4e);
  CYC(b_+5, b_+7); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto L_7e9e; } // jr nc
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x0b);
  CYC(b_+14, b_+16); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 30);
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+22); enemySetAnimation_hook(gb); return; // jp

L_7e9e:
  CALL_C(b_+22, objectApplySpeed_hook, SYM(objectApplySpeed), SYM(kingMoblin_animate2));
  kingMoblin_animate2_hook(gb); return; // fallthrough
}

void kingMoblin_animate2_hook(GB *gb) {
  BASE(kingMoblin_animate2);
  CYC(b_+0, b_+3); enemyAnimate_hook(gb); return; // jp
}

// Moving toward bomb
void kingMoblin_state11_hook(GB *gb) {
  BASE(kingMoblin_state11);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, kingMoblin_checkMoveToCentre_hook, SYM(kingMoblin_checkMoveToCentre), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, objectApplySpeed_hook, SYM(objectApplySpeed), b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+10, b_+12); E = ENEMY_BASE + 0x32; // var32
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+14, b_+16); alu_add(gb, 0x08);
  CYC(b_+16, b_+18); alu_cp(gb, 0x11);
  if (F & FC) { CYCT(b_+18, b_+20); kingMoblin_grabBomb_hook(gb); return; } // jr c
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+23); enemyAnimate_hook(gb); return; // jp
}

// Just died
void kingMoblin_state12_hook(GB *gb) {
  BASE(kingMoblin_state12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_cp(gb, 0x0c);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; } // ret nc
  CYC(b_+8, b_+9);
  CALL_C(b_+9, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+12);
  CYC(b_+12, b_+14); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x10);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+20, b_+22); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+22, b_+24); A = 0xa0; // <(-$160)
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0xfe); // >(-$160)
  CYC(b_+27, b_+29); A = 60;
  CYC(b_+29, b_+32); setScreenShakeCounter_hook(gb); return; // jp
}

// Falling to ground
void kingMoblin_state13_hook(GB *gb) {
  BASE(kingMoblin_state13);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+8); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(b_+5, b_+8);
  CALL_C(b_+8, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+11);
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 150);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x20);
  RET(b_+19); return; // ret
}

// Wait for signal from ENEMY_KING_MOBLIN_MINION to go to state $15?
void kingMoblin_state14_hook(GB *gb) {
  BASE(kingMoblin_state14);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + 0x33; // var33
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto gotoState15; } // jr nz
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter);
  CYC(b_+9, b_+10); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+10); return; } // ret c
  CYC(b_+10, b_+11);
  CALL_C(b_+11, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+14);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x0b);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 30);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CYC(b_+24, b_+27); enemySetAnimation_hook(gb); return; // jp

gotoState15:
  CALL_C(b_+27, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+30);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 98);
  RET(b_+34); return; // ret
}

// All bombs at top of screen explode, then initiates warp outside
void kingMoblin_state15_hook(GB *gb) {
  BASE(kingMoblin_state15);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto warpOutside; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); A = alu_dec8(gb, A);
  CYC(b_+7, b_+9); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } // ret nz
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_and(gb, 0x60);
  CYC(b_+13, b_+14); alu_rrca(gb);
  CYC(b_+14, b_+16); A = alu_swap(gb, A);
  CYC(b_+16, b_+19); SET_HL(b_+65); // @explosionPositions
  CYC(b_+19, b_+20); kingMoblin_addAToHl_from_rst(gb, b_+20);
  CYC(b_+20, b_+21); C = mem_rd(gb, HL);
  CYC(b_+21, b_+23); B = 0x08;
  CALL_C(b_+23, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+26);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; } // ret nz
  CYC(b_+26, b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x56); // INTERAC_EXPLOSION
  CYC(b_+29, b_+31); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+31, b_+32); mem_wr(gb, HL, B);
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+34, b_+35); mem_wr(gb, HL, C);
  CALL_C(b_+35, getTileAtPosition_hook, SYM(getTileAtPosition), b_+38);
  CYC(b_+38, b_+39); C = L;
  CYC(b_+39, b_+41); A = 0xa1;
  CYC(b_+41, b_+44); setTile_hook(gb); return; // jp

warpOutside:
  CYC(b_+44, b_+47); SET_HL((wGroup0RoomFlags + 9)); // wPresentRoomFlags+9
  CYC(b_+47, b_+49); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)
  CYC(b_+49, b_+51); A = 0x1a; // GLOBALFLAG_MOBLINS_KEEP_DESTROYED
  CALL_C(b_+51, setGlobalFlag_hook, SYM(setGlobalFlag), b_+54);
  CYC(b_+54, b_+56); A = 0x16; // GLOBALFLAG_16
  CALL_C(b_+56, setGlobalFlag_hook, SYM(setGlobalFlag), b_+59);
  CYC(b_+59, b_+62); SET_HL(b_+69); // @warpDest
  CYC(b_+62, b_+65); setWarpDestVariables_hook(gb); return; // jp
}

// Updates state and angle values to move king moblin to centre of screen, if there is no
// bomb on screen. Sets state to $10 while moving, $0b when reached centre.
// @param[out] zflag nz if state changed
void kingMoblin_checkMoveToCentre_hook(GB *gb) {
  BASE(kingMoblin_checkMoveToCentre);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = OBJ_ID; // Object.id
  CALL_C(b_+2, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x3f); // PART_KING_MOBLIN_BOMB
  if (F & FZ) { RET_TAKEN(b_+8); return; } // ret z
  CYC(b_+8, b_+9);
  kingMoblin_moveToCentre_hook(gb); return; // fallthrough
}

void kingMoblin_moveToCentre_hook(GB *gb) {
  BASE(kingMoblin_moveToCentre);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_sub(gb, 0x4e);
  CYC(b_+6, b_+8); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+8, b_+10); goto moveTowardCentre; } // jr nc
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 30);
  CYC(b_+14, b_+16); B = 0x0b;
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYCT(b_+17, b_+19); kingMoblin_setStateAndAnimation_hook(gb); return; // jr

moveTowardCentre:
  CYC(b_+19, b_+21); alu_cp(gb, 0xb0);
  CYC(b_+21, b_+23); B = 0x10;
  kingMoblin_setAngleStateAndAnimation_hook(gb); return; // fallthrough
}

void kingMoblin_setAngleStateAndAnimation_hook(GB *gb) {
  BASE(kingMoblin_setAngleStateAndAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x08; // ANGLE_RIGHT
  if (!(F & FC)) { CYCT(b_+2, b_+4); goto L_7f82; } // jr nc
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); A = 0x18; // ANGLE_LEFT

L_7f82:
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+12); alu_rlca(gb);
  kingMoblin_setStateAndAnimation_hook(gb); return; // fallthrough
}

void kingMoblin_setStateAndAnimation_hook(GB *gb) {
  BASE(kingMoblin_setStateAndAnimation);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); mem_wr(gb, HL, B);
  CALL_C(b_+3, enemySetAnimation_hook, SYM(enemySetAnimation), b_+6);
  CYC(b_+6, b_+7); alu_or(gb, D);
  RET(b_+7); return; // ret
}
