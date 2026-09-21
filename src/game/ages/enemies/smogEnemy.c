#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void smog_state_uninitialized_hook(GB *gb);
void smog_state_stub_hook(GB *gb);
void smog_state8_hook(GB *gb);
void smog_state8_subid0_hook(GB *gb);
void smog_state8_subid1_hook(GB *gb);
void smog_deleteSelf_hook(GB *gb);
void smog_state8_subid2_hook(GB *gb);
void smog_state8_subid3_hook(GB *gb);
void smog_state8_subid4_hook(GB *gb);
void smog_state8_subid5_hook(GB *gb);
void smog_checkHitWall_hook(GB *gb);
void smog_checkHuggingWall_hook(GB *gb);
void smog_checkAdjacentWallsBitset_hook(GB *gb);
void smog_applySpeed_hook(GB *gb);
void smog_updateAdjacentWallsBitset_hook(GB *gb);
void smog_decCounterToFireProjectile_hook(GB *gb);
void smog_setCounterToFireProjectile_hook(GB *gb);

static uint16_t smog_jump_table(GB *gb) {
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

static void smog_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void smog_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

// smog_state_uninitialized@initCollisions: reached only by a static `call` from
// smog_state_uninitialized itself (NOT HOOKED).
static void smog_initCollisions(GB *gb, uint16_t sp0_) {
  BASE(smog_state_uninitialized);
  CALL_C(b_+148, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+151);
  CYC(b_+151, b_+153); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+153, b_+155); A = 0x07; // ENEMYCOLLISION_PROJECTILE_WITH_RING_MOD
  CYC(b_+155, b_+156); mem_wr(gb, HL, A);
  RET(b_+156); return; // ret
}

// smog_state8_subid4@func_72b2: reached only by static `call`s from smog_state8_subid4
// itself (NOT HOOKED).
// @param[out] zflag z if counter1 reached 0 (should update speed)
static void smog_func_72b2(GB *gb, uint16_t sp0_) {
  BASE(smog_state8_subid4);
  CYC(b_+72, b_+74); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+74, b_+75); A = mem_rd(gb, DE);
  CYC(b_+75, b_+76); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+76, b_+78); goto parameter0; } // jr z
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+79); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+79, b_+81); goto parameter1; } // jr nz
  CYC(b_+79, b_+81);
  CYC(b_+81, b_+83); A = 0x04;
  CALL_C(b_+83, enemySetAnimation_hook, SYM(enemySetAnimation), b_+86);
  CYC(b_+86, b_+89); TAIL(smog_setCounterToFireProjectile); // jp

parameter1:
  CYC(b_+89, b_+91); B = 0x4a; // PART_SMOG_PROJECTILE
  CALL_C(b_+91, ecom_spawnProjectile_b0f_hook, SYM(ecom_spawnProjectile_b0f), b_+94);
  CYC(b_+94, b_+96); L = PART_BASE + OBJ_SUBID; // Part.subid
  CYC(b_+96, b_+97); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+97, b_+100); SET_BC(0x0800);
  CYC(b_+100, b_+103); TAIL(objectCopyPositionWithOffset); // jp

parameter0:
  CALL_C(b_+103, smog_decCounterToFireProjectile_hook, SYM(smog_decCounterToFireProjectile), b_+106);
  if (F & FZ) { CYCT(b_+106, b_+108); goto skipAnim; } // jr z
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+110); A = 0x05;
  CALL_C(b_+110, enemySetAnimation_hook, SYM(enemySetAnimation), b_+113);

skipAnim:
  CALL_C(b_+113, objectApplySpeed_hook, SYM(objectApplySpeed), b_+116);
  CALL_C(b_+116, ecom_bounceOffScreenBoundary_b0f_hook, SYM(ecom_bounceOffScreenBoundary_b0f), b_+119);
  CALL_C(b_+119, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+122);
  if (!(F & FZ)) { RET_TAKEN(b_+122); return; } // ret nz
  CYC(b_+122, b_+123);
  CYC(b_+123, b_+125); mem_wr(gb, HL, 0x14); // 20
  CYC(b_+125, b_+127); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+127, b_+128); A = mem_rd(gb, HL);
  RET(b_+128); return; // ret
}

// ==================================================================================================
// ENEMY_SMOG
//
// Variables:
//   var03: Phase of fight (0-3)
//   counter2: Stops movement temporarily (when sword collision occurs)
//   var30: "Adjacent walls bitset" (bitset of solid walls around smog, similar to the
//          variable used for special objects)
//   var31: Position of the tile it's "hugging"
//   var32: Number of frames to wait for a wall before disappearing and respawning
//   var33: Original value of "direction" (for subid 2 respawning)
//   var34/var35: Original Y/X position (for subid 2 respawning)
//   var36: Counter until "fire projectile" animation will begin
// ==================================================================================================
void smog_state_uninitialized_hook(GB *gb) {
  BASE(smog_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  {
    uint16_t target = smog_jump_table(gb);
    if (target == b_+20) goto subid0Init;
    if (target == b_+35) goto subid1Init;
    if (target == b_+44) goto subid2Init;
    if (target == b_+71) goto subid3Init;
    if (target == b_+102) goto subid4Init;
    if (target == b_+133) goto subid5Init;
    HANDOFF(target);
  }

subid0Init:
  CYC(b_+20, b_+23); SET_BC(0x2f26); // TX_2f26
  CALL_C(b_+23, showText_hook, SYM(showText), b_+26);
  CYC(b_+26, b_+28); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+28, b_+30); A = 0x3c; // 60
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+33); A = 0x04;
  CYCT(b_+33, b_+35); goto setAnimationAndCommonInit; // jr

subid1Init:
  CYC(b_+35, b_+37); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+37, b_+39); A = 0x78; // 120
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+42); A = 0x00;
  CYCT(b_+42, b_+44); goto setAnimationAndCommonInit; // jr

subid2Init:
  CALL_C(b_+44, enemyBoss_beginBoss_b0f_hook, SYM(enemyBoss_beginBoss_b0f), b_+47);
  CYC(b_+47, b_+48); H = D;
  CYC(b_+48, b_+50); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+50, b_+52); L = ENEMY_BASE + 0x33; // var33
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+58); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+58, b_+60); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); mem_wr(gb, HL, A);
  CYC(b_+62, b_+64); A = 0x04;
  CYC(b_+64, b_+67); push_effect(gb, b_+67); smog_initCollisions(gb, sp0_);
  CYC(b_+67, b_+69); A = 0x00;
  CYCT(b_+69, b_+71); goto setAnimationAndCommonInit; // jr

subid3Init:
  CALL_C(b_+71, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+74);
  CYC(b_+74, b_+76); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+76, b_+78); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  if (!(F & FZ)) { RET_TAKEN(b_+78); return; } // ret nz
  CYC(b_+78, b_+79);
  CYC(b_+79, b_+81); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+81, b_+84); A = mem_rd(gb, wNumEnemies);
  CYC(b_+84, b_+86); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+86, b_+88); goto subid4Init; } // jr z
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_COUNTER2; // BUG?: uses Interaction.counter2's offset
  CYC(b_+90, b_+92); A = 0x3c; // 60
  CYC(b_+92, b_+93); mem_wr(gb, DE, A);
  CYC(b_+93, b_+95); A = 0x06;
  CYC(b_+95, b_+98); push_effect(gb, b_+98); smog_initCollisions(gb, sp0_);
  CYC(b_+98, b_+100); A = 0x02;
  CYCT(b_+100, b_+102); goto setAnimationAndCommonInit; // jr

subid4Init:
  CYC(b_+102, b_+104); A = 0x04;
  CYC(b_+104, b_+105); mem_wr(gb, DE, A); // [subid] = 4
  CYC(b_+105, b_+107); A = 0xa3; // TILEINDEX_DUNGEON_a3
  CYC(b_+107, b_+109); C = 0x11;
  CALL_C(b_+109, setTile_hook, SYM(setTile), b_+112);
  CYC(b_+112, b_+114); A = 0x04;

setAnimationAndCommonInit:
  CALL_C(b_+114, enemySetAnimation_hook, SYM(enemySetAnimation), b_+117);
  CALL_C(b_+117, smog_setCounterToFireProjectile_hook, SYM(smog_setCounterToFireProjectile), b_+120);
  CYC(b_+120, b_+122); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+122, b_+123); A = mem_rd(gb, DE);
  CYC(b_+123, b_+125); alu_and(gb, 0x0f);
  CYC(b_+125, b_+128); SET_HL(b_+157); // @subidSpeedTable
  CYC(b_+128, b_+129); smog_addAToHl_from_rst(gb, b_+129);
  CYC(b_+129, b_+130); A = mem_rd(gb, HL);
  CYC(b_+130, b_+133); TAIL(ecom_setSpeedAndState8AndVisible_b0f); // jp

subid5Init:
  CYC(b_+133, b_+135); A = 0x7c; // ENEMY_SMOG
  CYC(b_+135, b_+137); B = 0x00;
  CALL_C(b_+137, enemyBoss_initializeRoom_b0f_hook, SYM(enemyBoss_initializeRoom_b0f), b_+140);
  CALL_C(b_+140, ecom_setSpeedAndState8_b0f_hook, SYM(ecom_setSpeedAndState8_b0f), b_+143);
  CYC(b_+143, b_+145); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+145, b_+147); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(b_+147); return; // ret
}

void smog_state_stub_hook(GB *gb) {
  BASE(smog_state_stub);
  RET(b_+0); return; // ret
}

void smog_state8_hook(GB *gb) {
  BASE(smog_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  {
    uint16_t target = smog_jump_table(gb);
    if (target == SYM(smog_state8_subid0)) { smog_state8_subid0_hook(gb); return; }
    if (target == SYM(smog_state8_subid1)) { smog_state8_subid1_hook(gb); return; }
    if (target == SYM(smog_state8_subid2)) { smog_state8_subid3_hook(gb); return; } // subid2 and subid3 share this address
    if (target == SYM(smog_state8_subid4)) { smog_state8_subid4_hook(gb); return; }
    if (target == SYM(smog_state8_subid5)) { smog_state8_subid5_hook(gb); return; }
    if (target == SYM(smog_deleteSelf)) { smog_deleteSelf_hook(gb); return; }
    HANDOFF(target);
  }
}

// Splitting into two?
void smog_state8_subid0_hook(GB *gb) {
  BASE(smog_state8_subid0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CALL_C(b_+3, enemyAnimate_hook, SYM(enemyAnimate), b_+6);
  CALL_C(b_+6, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } // ret nz
  CYC(b_+9, b_+10);
  CALL_C(b_+10, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+13);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x7c); // ENEMY_SMOG
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x01); // [child.subid]
  CYC(b_+18, b_+21); SET_BC(0x00f0);
  CALL_C(b_+21, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+24);
  CALL_C(b_+24, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+27);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x7c); // ENEMY_SMOG
  CYC(b_+29, b_+30); L = alu_inc8(gb, L);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x01); // [child.subid]
  CYC(b_+32, b_+35); SET_BC(0x0010);
  CALL_C(b_+35, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+38);
  CYCT(b_+38, b_+40); TAIL(smog_deleteSelf); // jr
}

void smog_state8_subid1_hook(GB *gb) {
  BASE(smog_state8_subid1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; } // ret nz
  CYC(b_+6, b_+7);
  TAIL(smog_deleteSelf); // fallthrough
}

void smog_deleteSelf_hook(GB *gb) {
  BASE(smog_deleteSelf);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectCreatePuff_hook, SYM(objectCreatePuff), b_+3);
  CALL_C(b_+3, decNumEnemies_hook, SYM(decNumEnemies), b_+6);
  CYC(b_+6, b_+9); TAIL(enemyDelete); // jp
}

// Small or medium-sized smog; identical to smog_state8_subid3 (the disassembly labels
// the same address twice, once per jump-table slot).
void smog_state8_subid2_hook(GB *gb) {
  smog_state8_subid3_hook(gb);
}

void smog_state8_subid3_hook(GB *gb) {
  BASE(smog_state8_subid2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto noCollision; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x7f);
  CYC(b_+9, b_+11); alu_cp(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  if (F & FC) { CYCT(b_+11, b_+13); goto noCollision; } // jr c
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); alu_cp(gb, 0x0a); // ITEMCOLLISION_SWORD_HELD+1
  if (!(F & FC)) { CYCT(b_+15, b_+17); goto noCollision; } // jr nc
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+19, b_+21); A = 0x1e; // 30
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);

noCollision:
  CALL_C(b_+22, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+25);
  if (!(F & FZ)) { CYCT(b_+25, b_+28); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+25, b_+28);
  CALL_C(b_+28, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+31);
  CYC(b_+31, b_+33); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); smog_deleteSelf_hook(gb); return; } // jr nz
  CYC(b_+33, b_+35);
  CALL_C(b_+35, enemyAnimate_hook, SYM(enemyAnimate), b_+38);
  CYC(b_+38, b_+40); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+42); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+42, b_+44); goto doneShootingProjectile; } // jr z
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+49); alu_and(gb, 0x0f);
  CYC(b_+49, b_+50); alu_add(gb, A);
  CYC(b_+50, b_+52); alu_add(gb, 0xfc); // -4
  CALL_C(b_+52, enemySetAnimation_hook, SYM(enemySetAnimation), b_+55);
  CALL_C(b_+55, smog_setCounterToFireProjectile_hook, SYM(smog_setCounterToFireProjectile), b_+58);
  CYC(b_+58, b_+60); B = 0x4a; // PART_SMOG_PROJECTILE
  CALL_C(b_+60, ecom_spawnProjectile_b0f_hook, SYM(ecom_spawnProjectile_b0f), b_+63);
  CALL_C(b_+63, objectCopyPosition_hook, SYM(objectCopyPosition), b_+66);

doneShootingProjectile:
  CALL_C(b_+66, smog_decCounterToFireProjectile_hook, SYM(smog_decCounterToFireProjectile), b_+69);
  if (F & FZ) { CYCT(b_+69, b_+71); goto runSubstate; } // jr z
  CYC(b_+69, b_+71);
  CYC(b_+71, b_+73); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+76); alu_and(gb, 0x0f);
  CYC(b_+76, b_+77); alu_add(gb, A);
  CYC(b_+77, b_+79); alu_add(gb, 0xfd); // -3
  CALL_C(b_+79, enemySetAnimation_hook, SYM(enemySetAnimation), b_+82);

runSubstate:
  CYC(b_+82, b_+84); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); push_effect(gb, b_+86);
  {
    uint16_t target = smog_jump_table(gb);
    if (target == b_+92) goto substate0;
    if (target == b_+126) goto substate1;
    if (target == b_+199) goto substate2;
    HANDOFF(target);
  }

substate0:
  CALL_C(b_+92, smog_applySpeed_hook, SYM(smog_applySpeed), b_+95);
  CALL_C(b_+95, smog_checkHuggingWall_hook, SYM(smog_checkHuggingWall), b_+98);
  if (!(F & FZ)) { CYCT(b_+98, b_+100); goto checkHitWall; } // jr nz
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+102); L = ENEMY_BASE + 0x32; // var32
  CYC(b_+102, b_+104); mem_wr(gb, HL, 0x10);

gotoState1:
  CYC(b_+104, b_+106); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+108); alu_add(gb, A);
  CYC(b_+108, b_+110); A = alu_swap(gb, A);
  CYC(b_+110, b_+111); E = alu_dec8(gb, E);
  CYC(b_+111, b_+112); mem_wr(gb, DE, A);
  CYC(b_+112, b_+114); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+114, b_+116); A = 0x01;
  CYC(b_+116, b_+117); mem_wr(gb, DE, A);
  CYC(b_+117, b_+120); TAIL(smog_applySpeed); // jp

checkHitWall:
  CALL_C(b_+120, smog_checkHitWall_hook, SYM(smog_checkHitWall), b_+123);
  if (!(F & FZ)) { CYCT(b_+123, b_+125); goto hitWall; } // jr nz
  CYC(b_+123, b_+125);
  RET(b_+125); return; // ret

substate1:
  CALL_C(b_+126, smog_applySpeed_hook, SYM(smog_applySpeed), b_+129);
  CALL_C(b_+129, smog_checkHuggingWall_hook, SYM(smog_checkHuggingWall), b_+132);
  if (!(F & FZ)) { CYCT(b_+132, b_+134); goto notHuggingWall; } // jr nz
  CYC(b_+132, b_+134);
  CYC(b_+134, b_+136); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+136, b_+137); A = mem_rd(gb, HL);
  CYC(b_+137, b_+139); alu_and(gb, 0x0f);
  CYC(b_+139, b_+141); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+141, b_+143); goto mediumSmog; } // jr nz
  CYC(b_+141, b_+143);
  CYC(b_+143, b_+145); L = ENEMY_BASE + 0x32; // var32
  CYC(b_+145, b_+146); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+146, b_+148); goto gotoState1; } // jr nz
  CYC(b_+146, b_+148);
  CALL_C(b_+148, objectCreatePuff_hook, SYM(objectCreatePuff), b_+151);
  CYC(b_+151, b_+152); H = D;
  CYC(b_+152, b_+154); L = ENEMY_BASE + 0x33; // var33
  CYC(b_+154, b_+156); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+156, b_+157); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+157, b_+158); mem_wr(gb, DE, A);
  CYC(b_+158, b_+160); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+160, b_+161); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+161, b_+162); mem_wr(gb, DE, A);
  CYC(b_+162, b_+164); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+164, b_+165); A = mem_rd(gb, HL);
  CYC(b_+165, b_+166); mem_wr(gb, DE, A);
  CALL_C(b_+166, objectCreatePuff_hook, SYM(objectCreatePuff), b_+169);
  CYCT(b_+169, b_+171); goto notHuggingWall; // jr

mediumSmog:
  CALL_C(b_+171, smog_checkHitWall_hook, SYM(smog_checkHitWall), b_+174);
  if (F & FZ) { RET_TAKEN(b_+174); return; } // ret z
  CYC(b_+174, b_+175);

hitWall:
  CYC(b_+175, b_+177); B = 0x01;
  CYC(b_+177, b_+179); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+179, b_+180); A = mem_rd(gb, DE);
  CYC(b_+180, b_+182); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+182, b_+184); goto L_724e; } // jr z
  CYC(b_+182, b_+184);
  CYC(b_+184, b_+186); B = 0xff;

L_724e:
  CYC(b_+186, b_+188); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+188, b_+189); A = mem_rd(gb, DE);
  CYC(b_+189, b_+190); alu_sub(gb, B);
  CYC(b_+190, b_+192); alu_and(gb, 0x03);
  CYC(b_+192, b_+193); mem_wr(gb, DE, A);
  CYC(b_+193, b_+195); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+195, b_+197); A = 0x02;
  CYC(b_+197, b_+198); mem_wr(gb, DE, A);
  RET(b_+198); return; // ret

substate2:
  CALL_C(b_+199, smog_updateAdjacentWallsBitset_hook, SYM(smog_updateAdjacentWallsBitset), b_+202);
  CALL_C(b_+202, smog_checkHitWall_hook, SYM(smog_checkHitWall), b_+205);
  if (!(F & FZ)) { CYCT(b_+205, b_+207); goto hitWall; } // jr nz
  CYC(b_+205, b_+207);

notHuggingWall:
  CYC(b_+207, b_+209); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+209, b_+210); alu_xor(gb, A);
  CYC(b_+210, b_+211); mem_wr(gb, DE, A);
  CYC(b_+211, b_+214); TAIL(smog_applySpeed); // jp
}

// Large smog (can be attacked)
void smog_state8_subid4_hook(GB *gb) {
  BASE(smog_state8_subid4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0xa0); // $80|ITEMCOLLISION_ELECTRIC_SHOCK
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto animateAndDispatch; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x03;
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+14); A = 0x46; // 70
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);

animateAndDispatch:
  CALL_C(b_+17, enemyAnimate_hook, SYM(enemyAnimate), b_+20);
  CYC(b_+20, b_+22); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); push_effect(gb, b_+24);
  {
    uint16_t target = smog_jump_table(gb);
    if (target == b_+32) goto substate0;
    if (target == b_+47) goto substate1;
    if (target == b_+60) goto substate2;
    if (target == b_+129) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(b_+32, b_+34); A = 0x01;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A); // [substate] = 1
  CYC(b_+35, b_+36); A = alu_dec8(gb, A);
  CYC(b_+36, b_+38); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);
  CALL_C(b_+39, ecom_updateAngleTowardTarget_b0f_hook, SYM(ecom_updateAngleTowardTarget_b0f), b_+42);
  CYC(b_+42, b_+44); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+44, b_+46); A = 0x14; // 20
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);

substate1:
  CYC(b_+47, b_+50); push_effect(gb, b_+50); smog_func_72b2(gb, sp0_);
  if (!(F & FZ)) { RET_TAKEN(b_+50); return; } // ret nz
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+53); alu_add(gb, 0x05); // SPEED_20
  CYC(b_+53, b_+54); mem_wr(gb, HL, A);
  CYC(b_+54, b_+56); alu_cp(gb, 0x1e); // SPEED_c0
  if (!(F & FZ)) { RET_TAKEN(b_+56); return; } // ret nz
  CYC(b_+56, b_+57);
  CYC(b_+57, b_+60); TAIL(ecom_incSubstate_b0f); // jp

substate2:
  CYC(b_+60, b_+63); push_effect(gb, b_+63); smog_func_72b2(gb, sp0_);
  if (!(F & FZ)) { RET_TAKEN(b_+63); return; } // ret nz
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+66); alu_sub(gb, 0x05); // SPEED_20
  CYC(b_+66, b_+67); mem_wr(gb, HL, A);
  if (!(F & FZ)) { RET_TAKEN(b_+67); return; } // ret nz
  CYC(b_+67, b_+68);
  CYC(b_+68, b_+70); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+70, b_+71); mem_wr(gb, HL, A); // [substate] = 0
  RET(b_+71); return; // ret

substate3:
  CALL_C(b_+129, ecom_decCounter2_b0f_hook, SYM(ecom_decCounter2_b0f), b_+132);
  if (!(F & FZ)) { RET_TAKEN(b_+132); return; } // ret nz
  CYC(b_+132, b_+133);
  CYC(b_+133, b_+134); alu_xor(gb, A); // xor a
  CYC(b_+134, b_+135); mem_wr(gb, DE, A);
  CYC(b_+135, b_+137); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+137, b_+139); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  RET(b_+139); return; // ret
}

void smog_state8_subid5_hook(GB *gb) {
  BASE(smog_state8_subid5);
  RET(b_+0); return; // ret
}

// @param[out] zflag nz if hit a wall
void smog_checkHitWall_hook(GB *gb) {
  BASE(smog_checkHitWall);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); alu_rrca(gb);
  CYC(b_+6, b_+7); E = alu_inc8(gb, E);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYCT(b_+8, b_+10); TAIL(smog_checkAdjacentWallsBitset); // jr
}

// @param[out] zflag nz if hugging a wall
void smog_checkHuggingWall_hook(GB *gb) {
  BASE(smog_checkHuggingWall);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); B = 0xff;
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_7314; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); B = 0x01;

L_7314:
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_sub(gb, B);
  CYC(b_+15, b_+17); alu_and(gb, 0x03);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+20); A = alu_swap(gb, A);
  CYC(b_+20, b_+21); alu_rrca(gb);
  CYC(b_+21, b_+22); E = alu_inc8(gb, E);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); A = C;
  CYC(b_+24, b_+27); SET_HL(SYM(smog_positionOffsets)); // adjacent-tile offset table (shared with smog_checkHitWall)
  CYC(b_+27, b_+28); smog_addDoubleIndexToHl_from_rst(gb, b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+29, b_+30); C = mem_rd(gb, HL);
  CYC(b_+30, b_+31); B = A;
  CALL_C(b_+31, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+34);
  CYC(b_+34, b_+36); H = 0xce;
  CYC(b_+36, b_+38); E = ENEMY_BASE + 0x31; // var31
  CYC(b_+38, b_+39); A = L;
  CYC(b_+39, b_+41); alu_and(gb, 0x0f);
  CYC(b_+41, b_+42); C = A;
  CYC(b_+42, b_+43); A = L;
  CYC(b_+43, b_+45); A = alu_swap(gb, A);
  CYC(b_+45, b_+47); alu_and(gb, 0x0f);
  CYC(b_+47, b_+48); alu_add(gb, C);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  TAIL(smog_checkAdjacentWallsBitset); // fallthrough
}

// Checks if there is a wall in the direction of the "angle" variable. (Angle could be
// facing forward, or in the direction of the wall being hugged, depending when this is
// called.)
// @param[out] zflag nz if a wall exists
void smog_checkAdjacentWallsBitset_hook(GB *gb) {
  BASE(smog_checkAdjacentWallsBitset);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_bit(gb, 3, A);
  if (F & FZ) { CYCT(b_+6, b_+8); goto upOrDown; } // jr z
  CYC(b_+6, b_+8);

  CYC(b_+8, b_+10); L = ENEMY_BASE + 0x30; // var30
  CYC(b_+10, b_+11); B = mem_rd(gb, HL);
  CYC(b_+11, b_+13); alu_bit(gb, 4, A);
  CYC(b_+13, b_+15); A = 0x03;
  if (F & FZ) { CYCT(b_+15, b_+17); goto L_734d; } // jr z
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0x0c;

L_734d:
  CYC(b_+19, b_+20); alu_and(gb, B);
  RET(b_+20); return; // ret

upOrDown:
  CYC(b_+21, b_+23); L = ENEMY_BASE + 0x30; // var30
  CYC(b_+23, b_+24); C = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_bit(gb, 4, A);
  CYC(b_+26, b_+28); A = 0x30;
  if (!(F & FZ)) { CYCT(b_+28, b_+30); goto L_735a; } // jr nz
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); A = 0xc0;

L_735a:
  CYC(b_+32, b_+33); alu_and(gb, C);
  RET(b_+33); return; // ret
}

// Applies speed and updates "adjacentWallsBitset"
void smog_applySpeed_hook(GB *gb) {
  BASE(smog_applySpeed);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); alu_rrca(gb);
  CYC(b_+6, b_+7); E = alu_inc8(gb, E);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, objectApplySpeed_hook, SYM(objectApplySpeed), SYM(smog_updateAdjacentWallsBitset));
  TAIL(smog_updateAdjacentWallsBitset); // fallthrough
}

void smog_updateAdjacentWallsBitset_hook(GB *gb) {
  BASE(smog_updateAdjacentWallsBitset);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + 0x30; // var30
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); H = D;
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+7, b_+8); B = mem_rd(gb, HL);
  CYC(b_+8, b_+10); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+10, b_+11); C = mem_rd(gb, HL);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+15); mem_wr(gb, hFF8B, A);
  CYC(b_+15, b_+18); SET_HL(b_+47); // adjacent-tile offsets

loop:
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+19, b_+20); alu_add(gb, B);
  CYC(b_+20, b_+21); B = A;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+22, b_+23); alu_add(gb, C);
  CYC(b_+23, b_+24); C = A;
  CYC(b_+24, b_+25); push_effect(gb, HL);
  CALL_C(b_+25, getTileAtPosition_hook, SYM(getTileAtPosition), b_+28);
  CYC(b_+28, b_+30); H = 0xce;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+32, b_+34); goto L_738a; } // jr z
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); alu_scf(gb);

L_738a:
  CYC(b_+35, b_+36); SET_HL(pop_effect(gb));
  CYC(b_+36, b_+38); A = mem_rd(gb, hFF8B);
  CYC(b_+38, b_+39); alu_rla(gb);
  CYC(b_+39, b_+41); mem_wr(gb, hFF8B, A);
  if (!(F & FC)) { CYCT(b_+41, b_+43); goto loop; } // jr nc
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+45); E = ENEMY_BASE + 0x30; // var30
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  RET(b_+46); return; // ret
}

// @param[out] zflag nz if smog should begin "firing projectile" animation
void smog_decCounterToFireProjectile_hook(GB *gb) {
  BASE(smog_decCounterToFireProjectile);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto zflag; } // jr nz
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); alu_or(gb, D);
  RET(b_+10); return; // ret

zflag:
  CYC(b_+11, b_+12); alu_xor(gb, A);
  RET(b_+12); return; // ret
}

// For given values of subid and var03, this reads one of four randomly chosen values and
// puts that value into var36 (counter until next projectile is fired).
void smog_setCounterToFireProjectile_hook(GB *gb) {
  BASE(smog_setCounterToFireProjectile);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); alu_sub(gb, 0x02);
  CYC(b_+7, b_+10); SET_HL(b_+29);
  CYC(b_+10, b_+11); smog_addAToHl_from_rst(gb, b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); smog_addAToHl_from_rst(gb, b_+13);
  CYC(b_+13, b_+14); E = alu_inc8(gb, E);
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); smog_addAToHl_from_rst(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); smog_addAToHl_from_rst(gb, b_+18);
  CALL_C(b_+18, getRandomNumber_hook, SYM(getRandomNumber), b_+21);
  CYC(b_+21, b_+23); alu_and(gb, 0x03);
  CYC(b_+23, b_+24); smog_addAToHl_from_rst(gb, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  RET(b_+28); return; // ret
}
