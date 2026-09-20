#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

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
  CALL_C(0x7139, objectSetCollideRadius_hook, 0x24a1, 0x713c);
  CYC(0x713c, 0x713e); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x713e, 0x7140); A = 0x07; // ENEMYCOLLISION_PROJECTILE_WITH_RING_MOD
  CYC(0x7140, 0x7141); mem_wr(gb, HL, A);
  RET(0x7141); return; // ret
}

// smog_state8_subid4@func_72b2: reached only by static `call`s from smog_state8_subid4
// itself (NOT HOOKED).
// @param[out] zflag z if counter1 reached 0 (should update speed)
static void smog_func_72b2(GB *gb, uint16_t sp0_) {
  CYC(0x72b2, 0x72b4); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x72b4, 0x72b5); A = mem_rd(gb, DE);
  CYC(0x72b5, 0x72b6); alu_or(gb, A);
  if (F & FZ) { CYCT(0x72b6, 0x72b8); goto parameter0; } // jr z
  CYC(0x72b6, 0x72b8);
  CYC(0x72b8, 0x72b9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x72b9, 0x72bb); goto parameter1; } // jr nz
  CYC(0x72b9, 0x72bb);
  CYC(0x72bb, 0x72bd); A = 0x04;
  CALL_C(0x72bd, enemySetAnimation_hook, 0x282b, 0x72c0);
  CYC(0x72c0, 0x72c3); smog_setCounterToFireProjectile_hook(gb); return; // jp

parameter1:
  CYC(0x72c3, 0x72c5); B = 0x4a; // PART_SMOG_PROJECTILE
  CALL_C(0x72c5, ecom_spawnProjectile_b0f_hook, 0x437c, 0x72c8);
  CYC(0x72c8, 0x72ca); L = PART_BASE + OBJ_SUBID; // Part.subid
  CYC(0x72ca, 0x72cb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x72cb, 0x72ce); SET_BC(0x0800);
  CYC(0x72ce, 0x72d1); objectCopyPositionWithOffset_hook(gb); return; // jp

parameter0:
  CALL_C(0x72d1, smog_decCounterToFireProjectile_hook, 0x73a6, 0x72d4);
  if (F & FZ) { CYCT(0x72d4, 0x72d6); goto skipAnim; } // jr z
  CYC(0x72d4, 0x72d6);
  CYC(0x72d6, 0x72d8); A = 0x05;
  CALL_C(0x72d8, enemySetAnimation_hook, 0x282b, 0x72db);

skipAnim:
  CALL_C(0x72db, objectApplySpeed_hook, 0x201d, 0x72de);
  CALL_C(0x72de, ecom_bounceOffScreenBoundary_b0f_hook, 0x42e5, 0x72e1);
  CALL_C(0x72e1, ecom_decCounter1_b0f_hook, 0x439a, 0x72e4);
  if (!(F & FZ)) { RET_TAKEN(0x72e4); return; } // ret nz
  CYC(0x72e4, 0x72e5);
  CYC(0x72e5, 0x72e7); mem_wr(gb, HL, 0x14); // 20
  CYC(0x72e7, 0x72e9); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x72e9, 0x72ea); A = mem_rd(gb, HL);
  RET(0x72ea); return; // ret
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
  uint16_t sp0_ = gb->sp;
  CYC(0x70a5, 0x70a7); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x70a7, 0x70a8); A = mem_rd(gb, DE);
  CYC(0x70a8, 0x70aa); alu_and(gb, 0x0f);
  CYC(0x70aa, 0x70ab); push_effect(gb, 0x70ab);
  {
    uint16_t target = smog_jump_table(gb);
    if (target == 0x70b9) goto subid0Init;
    if (target == 0x70c8) goto subid1Init;
    if (target == 0x70d1) goto subid2Init;
    if (target == 0x70ec) goto subid3Init;
    if (target == 0x710b) goto subid4Init;
    if (target == 0x712a) goto subid5Init;
    HANDOFF(target);
  }

subid0Init:
  CYC(0x70b9, 0x70bc); SET_BC(0x2f26); // TX_2f26
  CALL_C(0x70bc, showText_hook, 0x1872, 0x70bf);
  CYC(0x70bf, 0x70c1); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x70c1, 0x70c3); A = 0x3c; // 60
  CYC(0x70c3, 0x70c4); mem_wr(gb, DE, A);
  CYC(0x70c4, 0x70c6); A = 0x04;
  CYCT(0x70c6, 0x70c8); goto setAnimationAndCommonInit; // jr

subid1Init:
  CYC(0x70c8, 0x70ca); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x70ca, 0x70cc); A = 0x78; // 120
  CYC(0x70cc, 0x70cd); mem_wr(gb, DE, A);
  CYC(0x70cd, 0x70cf); A = 0x00;
  CYCT(0x70cf, 0x70d1); goto setAnimationAndCommonInit; // jr

subid2Init:
  CALL_C(0x70d1, enemyBoss_beginBoss_b0f_hook, 0x4584, 0x70d4);
  CYC(0x70d4, 0x70d5); H = D;
  CYC(0x70d5, 0x70d7); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x70d7, 0x70d9); L = ENEMY_BASE + 0x33; // var33
  CYC(0x70d9, 0x70da); A = mem_rd(gb, DE);
  CYC(0x70da, 0x70db); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x70db, 0x70dd); E = ENEMY_BASE + OBJ_YH;
  CYC(0x70dd, 0x70de); A = mem_rd(gb, DE);
  CYC(0x70de, 0x70df); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x70df, 0x70e1); E = ENEMY_BASE + OBJ_XH;
  CYC(0x70e1, 0x70e2); A = mem_rd(gb, DE);
  CYC(0x70e2, 0x70e3); mem_wr(gb, HL, A);
  CYC(0x70e3, 0x70e5); A = 0x04;
  CYC(0x70e5, 0x70e8); push_effect(gb, 0x70e8); smog_initCollisions(gb, sp0_);
  CYC(0x70e8, 0x70ea); A = 0x00;
  CYCT(0x70ea, 0x70ec); goto setAnimationAndCommonInit; // jr

subid3Init:
  CALL_C(0x70ec, ecom_decCounter2_b0f_hook, 0x43a3, 0x70ef);
  CYC(0x70ef, 0x70f1); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x70f1, 0x70f3); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  if (!(F & FZ)) { RET_TAKEN(0x70f3); return; } // ret nz
  CYC(0x70f3, 0x70f4);
  CYC(0x70f4, 0x70f6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x70f6, 0x70f9); A = mem_rd(gb, wNumEnemies);
  CYC(0x70f9, 0x70fb); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x70fb, 0x70fd); goto subid4Init; } // jr z
  CYC(0x70fb, 0x70fd);
  CYC(0x70fd, 0x70ff); E = INTERACTION_BASE + OBJ_COUNTER2; // BUG?: uses Interaction.counter2's offset
  CYC(0x70ff, 0x7101); A = 0x3c; // 60
  CYC(0x7101, 0x7102); mem_wr(gb, DE, A);
  CYC(0x7102, 0x7104); A = 0x06;
  CYC(0x7104, 0x7107); push_effect(gb, 0x7107); smog_initCollisions(gb, sp0_);
  CYC(0x7107, 0x7109); A = 0x02;
  CYCT(0x7109, 0x710b); goto setAnimationAndCommonInit; // jr

subid4Init:
  CYC(0x710b, 0x710d); A = 0x04;
  CYC(0x710d, 0x710e); mem_wr(gb, DE, A); // [subid] = 4
  CYC(0x710e, 0x7110); A = 0xa3; // TILEINDEX_DUNGEON_a3
  CYC(0x7110, 0x7112); C = 0x11;
  CALL_C(0x7112, setTile_hook, 0x3a9c, 0x7115);
  CYC(0x7115, 0x7117); A = 0x04;

setAnimationAndCommonInit:
  CALL_C(0x7117, enemySetAnimation_hook, 0x282b, 0x711a);
  CALL_C(0x711a, smog_setCounterToFireProjectile_hook, 0x73b3, 0x711d);
  CYC(0x711d, 0x711f); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x711f, 0x7120); A = mem_rd(gb, DE);
  CYC(0x7120, 0x7122); alu_and(gb, 0x0f);
  CYC(0x7122, 0x7125); SET_HL(0x7142); // @subidSpeedTable
  CYC(0x7125, 0x7126); smog_addAToHl_from_rst(gb, 0x7126);
  CYC(0x7126, 0x7127); A = mem_rd(gb, HL);
  CYC(0x7127, 0x712a); ecom_setSpeedAndState8AndVisible_b0f_hook(gb); return; // jp

subid5Init:
  CYC(0x712a, 0x712c); A = 0x7c; // ENEMY_SMOG
  CYC(0x712c, 0x712e); B = 0x00;
  CALL_C(0x712e, enemyBoss_initializeRoom_b0f_hook, 0x4546, 0x7131);
  CALL_C(0x7131, ecom_setSpeedAndState8_b0f_hook, 0x4364, 0x7134);
  CYC(0x7134, 0x7136); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x7136, 0x7138); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  RET(0x7138); return; // ret
}

void smog_state_stub_hook(GB *gb) {
  RET(0x7147); return; // ret
}

void smog_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7148, 0x714a); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x714a, 0x714b); A = mem_rd(gb, DE);
  CYC(0x714b, 0x714d); alu_and(gb, 0x0f);
  CYC(0x714d, 0x714e); push_effect(gb, 0x714e);
  {
    uint16_t target = smog_jump_table(gb);
    if (target == 0x715c) { smog_state8_subid0_hook(gb); return; }
    if (target == 0x7184) { smog_state8_subid1_hook(gb); return; }
    if (target == 0x7194) { smog_state8_subid3_hook(gb); return; } // subid2 and subid3 share this address
    if (target == 0x726a) { smog_state8_subid4_hook(gb); return; }
    if (target == 0x72f6) { smog_state8_subid5_hook(gb); return; }
    if (target == 0x718b) { smog_deleteSelf_hook(gb); return; }
    HANDOFF(target);
  }
}

// Splitting into two?
void smog_state8_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x715c, retIfTextIsActive_hook, 0x1859, 0x715f);
  CALL_C(0x715f, enemyAnimate_hook, 0x2818, 0x7162);
  CALL_C(0x7162, ecom_decCounter2_b0f_hook, 0x43a3, 0x7165);
  if (!(F & FZ)) { RET_TAKEN(0x7165); return; } // ret nz
  CYC(0x7165, 0x7166);
  CALL_C(0x7166, getFreeEnemySlot_hook, 0x2e27, 0x7169);
  CYC(0x7169, 0x716b); mem_wr(gb, HL, 0x7c); // ENEMY_SMOG
  CYC(0x716b, 0x716c); L = alu_inc8(gb, L);
  CYC(0x716c, 0x716e); mem_wr(gb, HL, 0x01); // [child.subid]
  CYC(0x716e, 0x7171); SET_BC(0x00f0);
  CALL_C(0x7171, objectCopyPositionWithOffset_hook, 0x225a, 0x7174);
  CALL_C(0x7174, getFreeEnemySlot_hook, 0x2e27, 0x7177);
  CYC(0x7177, 0x7179); mem_wr(gb, HL, 0x7c); // ENEMY_SMOG
  CYC(0x7179, 0x717a); L = alu_inc8(gb, L);
  CYC(0x717a, 0x717c); mem_wr(gb, HL, 0x01); // [child.subid]
  CYC(0x717c, 0x717f); SET_BC(0x0010);
  CALL_C(0x717f, objectCopyPositionWithOffset_hook, 0x225a, 0x7182);
  CYCT(0x7182, 0x7184); smog_deleteSelf_hook(gb); return; // jr
}

void smog_state8_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7184, enemyAnimate_hook, 0x2818, 0x7187);
  CALL_C(0x7187, ecom_decCounter2_b0f_hook, 0x43a3, 0x718a);
  if (!(F & FZ)) { RET_TAKEN(0x718a); return; } // ret nz
  CYC(0x718a, 0x718b);
  smog_deleteSelf_hook(gb); return; // fallthrough
}

void smog_deleteSelf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x718b, objectCreatePuff_hook, 0x24c1, 0x718e);
  CALL_C(0x718e, decNumEnemies_hook, 0x24b3, 0x7191);
  CYC(0x7191, 0x7194); enemyDelete_hook(gb); return; // jp
}

// Small or medium-sized smog; identical to smog_state8_subid3 (the disassembly labels
// the same address twice, once per jump-table slot).
void smog_state8_subid2_hook(GB *gb) {
  smog_state8_subid3_hook(gb);
}

void smog_state8_subid3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7194, 0x7196); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x7196, 0x7197); A = mem_rd(gb, DE);
  CYC(0x7197, 0x7199); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x7199, 0x719b); goto noCollision; } // jr z
  CYC(0x7199, 0x719b);
  CYC(0x719b, 0x719d); alu_and(gb, 0x7f);
  CYC(0x719d, 0x719f); alu_cp(gb, 0x04); // ITEMCOLLISION_L1_SWORD
  if (F & FC) { CYCT(0x719f, 0x71a1); goto noCollision; } // jr c
  CYC(0x719f, 0x71a1);
  CYC(0x71a1, 0x71a3); alu_cp(gb, 0x0a); // ITEMCOLLISION_SWORD_HELD+1
  if (!(F & FC)) { CYCT(0x71a3, 0x71a5); goto noCollision; } // jr nc
  CYC(0x71a3, 0x71a5);
  CYC(0x71a5, 0x71a7); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x71a7, 0x71a9); A = 0x1e; // 30
  CYC(0x71a9, 0x71aa); mem_wr(gb, DE, A);

noCollision:
  CALL_C(0x71aa, ecom_decCounter2_b0f_hook, 0x43a3, 0x71ad);
  if (!(F & FZ)) { CYCT(0x71ad, 0x71b0); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x71ad, 0x71b0);
  CALL_C(0x71b0, getThisRoomFlags_hook, 0x197d, 0x71b3);
  CYC(0x71b3, 0x71b5); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(0x71b5, 0x71b7); smog_deleteSelf_hook(gb); return; } // jr nz
  CYC(0x71b5, 0x71b7);
  CALL_C(0x71b7, enemyAnimate_hook, 0x2818, 0x71ba);
  CYC(0x71ba, 0x71bc); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x71bc, 0x71bd); A = mem_rd(gb, DE);
  CYC(0x71bd, 0x71be); alu_or(gb, A);
  if (F & FZ) { CYCT(0x71be, 0x71c0); goto doneShootingProjectile; } // jr z
  CYC(0x71be, 0x71c0);
  CYC(0x71c0, 0x71c2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x71c2, 0x71c3); A = mem_rd(gb, DE);
  CYC(0x71c3, 0x71c5); alu_and(gb, 0x0f);
  CYC(0x71c5, 0x71c6); alu_add(gb, A);
  CYC(0x71c6, 0x71c8); alu_add(gb, 0xfc); // -4
  CALL_C(0x71c8, enemySetAnimation_hook, 0x282b, 0x71cb);
  CALL_C(0x71cb, smog_setCounterToFireProjectile_hook, 0x73b3, 0x71ce);
  CYC(0x71ce, 0x71d0); B = 0x4a; // PART_SMOG_PROJECTILE
  CALL_C(0x71d0, ecom_spawnProjectile_b0f_hook, 0x437c, 0x71d3);
  CALL_C(0x71d3, objectCopyPosition_hook, 0x2242, 0x71d6);

doneShootingProjectile:
  CALL_C(0x71d6, smog_decCounterToFireProjectile_hook, 0x73a6, 0x71d9);
  if (F & FZ) { CYCT(0x71d9, 0x71db); goto runSubstate; } // jr z
  CYC(0x71d9, 0x71db);
  CYC(0x71db, 0x71dd); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x71dd, 0x71de); A = mem_rd(gb, DE);
  CYC(0x71de, 0x71e0); alu_and(gb, 0x0f);
  CYC(0x71e0, 0x71e1); alu_add(gb, A);
  CYC(0x71e1, 0x71e3); alu_add(gb, 0xfd); // -3
  CALL_C(0x71e3, enemySetAnimation_hook, 0x282b, 0x71e6);

runSubstate:
  CYC(0x71e6, 0x71e8); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x71e8, 0x71e9); A = mem_rd(gb, DE);
  CYC(0x71e9, 0x71ea); push_effect(gb, 0x71ea);
  {
    uint16_t target = smog_jump_table(gb);
    if (target == 0x71f0) goto substate0;
    if (target == 0x7212) goto substate1;
    if (target == 0x725b) goto substate2;
    HANDOFF(target);
  }

substate0:
  CALL_C(0x71f0, smog_applySpeed_hook, 0x735c, 0x71f3);
  CALL_C(0x71f3, smog_checkHuggingWall_hook, 0x7309, 0x71f6);
  if (!(F & FZ)) { CYCT(0x71f6, 0x71f8); goto checkHitWall; } // jr nz
  CYC(0x71f6, 0x71f8);
  CYC(0x71f8, 0x71fa); L = ENEMY_BASE + 0x32; // var32
  CYC(0x71fa, 0x71fc); mem_wr(gb, HL, 0x10);

gotoState1:
  CYC(0x71fc, 0x71fe); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x71fe, 0x71ff); A = mem_rd(gb, DE);
  CYC(0x71ff, 0x7200); alu_add(gb, A);
  CYC(0x7200, 0x7202); A = alu_swap(gb, A);
  CYC(0x7202, 0x7203); E = alu_dec8(gb, E);
  CYC(0x7203, 0x7204); mem_wr(gb, DE, A);
  CYC(0x7204, 0x7206); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x7206, 0x7208); A = 0x01;
  CYC(0x7208, 0x7209); mem_wr(gb, DE, A);
  CYC(0x7209, 0x720c); smog_applySpeed_hook(gb); return; // jp

checkHitWall:
  CALL_C(0x720c, smog_checkHitWall_hook, 0x72f7, 0x720f);
  if (!(F & FZ)) { CYCT(0x720f, 0x7211); goto hitWall; } // jr nz
  CYC(0x720f, 0x7211);
  RET(0x7211); return; // ret

substate1:
  CALL_C(0x7212, smog_applySpeed_hook, 0x735c, 0x7215);
  CALL_C(0x7215, smog_checkHuggingWall_hook, 0x7309, 0x7218);
  if (!(F & FZ)) { CYCT(0x7218, 0x721a); goto notHuggingWall; } // jr nz
  CYC(0x7218, 0x721a);
  CYC(0x721a, 0x721c); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x721c, 0x721d); A = mem_rd(gb, HL);
  CYC(0x721d, 0x721f); alu_and(gb, 0x0f);
  CYC(0x721f, 0x7221); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x7221, 0x7223); goto mediumSmog; } // jr nz
  CYC(0x7221, 0x7223);
  CYC(0x7223, 0x7225); L = ENEMY_BASE + 0x32; // var32
  CYC(0x7225, 0x7226); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x7226, 0x7228); goto gotoState1; } // jr nz
  CYC(0x7226, 0x7228);
  CALL_C(0x7228, objectCreatePuff_hook, 0x24c1, 0x722b);
  CYC(0x722b, 0x722c); H = D;
  CYC(0x722c, 0x722e); L = ENEMY_BASE + 0x33; // var33
  CYC(0x722e, 0x7230); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x7230, 0x7231); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7231, 0x7232); mem_wr(gb, DE, A);
  CYC(0x7232, 0x7234); E = ENEMY_BASE + OBJ_YH;
  CYC(0x7234, 0x7235); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7235, 0x7236); mem_wr(gb, DE, A);
  CYC(0x7236, 0x7238); E = ENEMY_BASE + OBJ_XH;
  CYC(0x7238, 0x7239); A = mem_rd(gb, HL);
  CYC(0x7239, 0x723a); mem_wr(gb, DE, A);
  CALL_C(0x723a, objectCreatePuff_hook, 0x24c1, 0x723d);
  CYCT(0x723d, 0x723f); goto notHuggingWall; // jr

mediumSmog:
  CALL_C(0x723f, smog_checkHitWall_hook, 0x72f7, 0x7242);
  if (F & FZ) { RET_TAKEN(0x7242); return; } // ret z
  CYC(0x7242, 0x7243);

hitWall:
  CYC(0x7243, 0x7245); B = 0x01;
  CYC(0x7245, 0x7247); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7247, 0x7248); A = mem_rd(gb, DE);
  CYC(0x7248, 0x724a); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x724a, 0x724c); goto L_724e; } // jr z
  CYC(0x724a, 0x724c);
  CYC(0x724c, 0x724e); B = 0xff;

L_724e:
  CYC(0x724e, 0x7250); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x7250, 0x7251); A = mem_rd(gb, DE);
  CYC(0x7251, 0x7252); alu_sub(gb, B);
  CYC(0x7252, 0x7254); alu_and(gb, 0x03);
  CYC(0x7254, 0x7255); mem_wr(gb, DE, A);
  CYC(0x7255, 0x7257); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x7257, 0x7259); A = 0x02;
  CYC(0x7259, 0x725a); mem_wr(gb, DE, A);
  RET(0x725a); return; // ret

substate2:
  CALL_C(0x725b, smog_updateAdjacentWallsBitset_hook, 0x7367, 0x725e);
  CALL_C(0x725e, smog_checkHitWall_hook, 0x72f7, 0x7261);
  if (!(F & FZ)) { CYCT(0x7261, 0x7263); goto hitWall; } // jr nz
  CYC(0x7261, 0x7263);

notHuggingWall:
  CYC(0x7263, 0x7265); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x7265, 0x7266); alu_xor(gb, A);
  CYC(0x7266, 0x7267); mem_wr(gb, DE, A);
  CYC(0x7267, 0x726a); smog_applySpeed_hook(gb); return; // jp
}

// Large smog (can be attacked)
void smog_state8_subid4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x726a, 0x726c); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x726c, 0x726d); A = mem_rd(gb, DE);
  CYC(0x726d, 0x726f); alu_cp(gb, 0xa0); // $80|ITEMCOLLISION_ELECTRIC_SHOCK
  if (!(F & FZ)) { CYCT(0x726f, 0x7271); goto animateAndDispatch; } // jr nz
  CYC(0x726f, 0x7271);
  CYC(0x7271, 0x7273); A = 0x03;
  CYC(0x7273, 0x7275); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x7275, 0x7276); mem_wr(gb, DE, A);
  CYC(0x7276, 0x7278); A = 0x46; // 70
  CYC(0x7278, 0x727a); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x727a, 0x727b); mem_wr(gb, DE, A);

animateAndDispatch:
  CALL_C(0x727b, enemyAnimate_hook, 0x2818, 0x727e);
  CYC(0x727e, 0x7280); E = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x7280, 0x7281); A = mem_rd(gb, DE);
  CYC(0x7281, 0x7282); push_effect(gb, 0x7282);
  {
    uint16_t target = smog_jump_table(gb);
    if (target == 0x728a) goto substate0;
    if (target == 0x7299) goto substate1;
    if (target == 0x72a6) goto substate2;
    if (target == 0x72eb) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(0x728a, 0x728c); A = 0x01;
  CYC(0x728c, 0x728d); mem_wr(gb, DE, A); // [substate] = 1
  CYC(0x728d, 0x728e); A = alu_dec8(gb, A);
  CYC(0x728e, 0x7290); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x7290, 0x7291); mem_wr(gb, DE, A);
  CALL_C(0x7291, ecom_updateAngleTowardTarget_b0f_hook, 0x43bf, 0x7294);
  CYC(0x7294, 0x7296); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7296, 0x7298); A = 0x14; // 20
  CYC(0x7298, 0x7299); mem_wr(gb, DE, A);

substate1:
  CYC(0x7299, 0x729c); push_effect(gb, 0x729c); smog_func_72b2(gb, sp0_);
  if (!(F & FZ)) { RET_TAKEN(0x729c); return; } // ret nz
  CYC(0x729c, 0x729d);
  CYC(0x729d, 0x729f); alu_add(gb, 0x05); // SPEED_20
  CYC(0x729f, 0x72a0); mem_wr(gb, HL, A);
  CYC(0x72a0, 0x72a2); alu_cp(gb, 0x1e); // SPEED_c0
  if (!(F & FZ)) { RET_TAKEN(0x72a2); return; } // ret nz
  CYC(0x72a2, 0x72a3);
  CYC(0x72a3, 0x72a6); ecom_incSubstate_b0f_hook(gb); return; // jp

substate2:
  CYC(0x72a6, 0x72a9); push_effect(gb, 0x72a9); smog_func_72b2(gb, sp0_);
  if (!(F & FZ)) { RET_TAKEN(0x72a9); return; } // ret nz
  CYC(0x72a9, 0x72aa);
  CYC(0x72aa, 0x72ac); alu_sub(gb, 0x05); // SPEED_20
  CYC(0x72ac, 0x72ad); mem_wr(gb, HL, A);
  if (!(F & FZ)) { RET_TAKEN(0x72ad); return; } // ret nz
  CYC(0x72ad, 0x72ae);
  CYC(0x72ae, 0x72b0); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x72b0, 0x72b1); mem_wr(gb, HL, A); // [substate] = 0
  RET(0x72b1); return; // ret

substate3:
  CALL_C(0x72eb, ecom_decCounter2_b0f_hook, 0x43a3, 0x72ee);
  if (!(F & FZ)) { RET_TAKEN(0x72ee); return; } // ret nz
  CYC(0x72ee, 0x72ef);
  CYC(0x72ef, 0x72f0); alu_xor(gb, A); // xor a
  CYC(0x72f0, 0x72f1); mem_wr(gb, DE, A);
  CYC(0x72f1, 0x72f3); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x72f3, 0x72f5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  RET(0x72f5); return; // ret
}

void smog_state8_subid5_hook(GB *gb) {
  RET(0x72f6); return; // ret
}

// @param[out] zflag nz if hit a wall
void smog_checkHitWall_hook(GB *gb) {
  CYC(0x72f7, 0x72f9); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x72f9, 0x72fa); A = mem_rd(gb, DE);
  CYC(0x72fa, 0x72fc); A = alu_swap(gb, A);
  CYC(0x72fc, 0x72fd); alu_rrca(gb);
  CYC(0x72fd, 0x72fe); E = alu_inc8(gb, E);
  CYC(0x72fe, 0x72ff); mem_wr(gb, DE, A);
  CYCT(0x72ff, 0x7301); smog_checkAdjacentWallsBitset_hook(gb); return; // jr
}

// @param[out] zflag nz if hugging a wall
void smog_checkHuggingWall_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7309, 0x730b); B = 0xff;
  CYC(0x730b, 0x730d); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x730d, 0x730e); A = mem_rd(gb, DE);
  CYC(0x730e, 0x7310); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x7310, 0x7312); goto L_7314; } // jr z
  CYC(0x7310, 0x7312);
  CYC(0x7312, 0x7314); B = 0x01;

L_7314:
  CYC(0x7314, 0x7316); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x7316, 0x7317); A = mem_rd(gb, DE);
  CYC(0x7317, 0x7318); alu_sub(gb, B);
  CYC(0x7318, 0x731a); alu_and(gb, 0x03);
  CYC(0x731a, 0x731b); C = A;
  CYC(0x731b, 0x731d); A = alu_swap(gb, A);
  CYC(0x731d, 0x731e); alu_rrca(gb);
  CYC(0x731e, 0x731f); E = alu_inc8(gb, E);
  CYC(0x731f, 0x7320); mem_wr(gb, DE, A);
  CYC(0x7320, 0x7321); A = C;
  CYC(0x7321, 0x7324); SET_HL(0x7301); // adjacent-tile offset table (shared with smog_checkHitWall)
  CYC(0x7324, 0x7325); smog_addDoubleIndexToHl_from_rst(gb, 0x7325);
  CYC(0x7325, 0x7326); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7326, 0x7327); C = mem_rd(gb, HL);
  CYC(0x7327, 0x7328); B = A;
  CALL_C(0x7328, objectGetRelativeTile_hook, 0x1435, 0x732b);
  CYC(0x732b, 0x732d); H = 0xce;
  CYC(0x732d, 0x732f); E = ENEMY_BASE + 0x31; // var31
  CYC(0x732f, 0x7330); A = L;
  CYC(0x7330, 0x7332); alu_and(gb, 0x0f);
  CYC(0x7332, 0x7333); C = A;
  CYC(0x7333, 0x7334); A = L;
  CYC(0x7334, 0x7336); A = alu_swap(gb, A);
  CYC(0x7336, 0x7338); alu_and(gb, 0x0f);
  CYC(0x7338, 0x7339); alu_add(gb, C);
  CYC(0x7339, 0x733a); mem_wr(gb, DE, A);
  smog_checkAdjacentWallsBitset_hook(gb); return; // fallthrough
}

// Checks if there is a wall in the direction of the "angle" variable. (Angle could be
// facing forward, or in the direction of the wall being hugged, depending when this is
// called.)
// @param[out] zflag nz if a wall exists
void smog_checkAdjacentWallsBitset_hook(GB *gb) {
  CYC(0x733a, 0x733b); H = D;
  CYC(0x733b, 0x733d); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x733d, 0x733e); A = mem_rd(gb, HL);
  CYC(0x733e, 0x7340); alu_bit(gb, 3, A);
  if (F & FZ) { CYCT(0x7340, 0x7342); goto upOrDown; } // jr z
  CYC(0x7340, 0x7342);

  CYC(0x7342, 0x7344); L = ENEMY_BASE + 0x30; // var30
  CYC(0x7344, 0x7345); B = mem_rd(gb, HL);
  CYC(0x7345, 0x7347); alu_bit(gb, 4, A);
  CYC(0x7347, 0x7349); A = 0x03;
  if (F & FZ) { CYCT(0x7349, 0x734b); goto L_734d; } // jr z
  CYC(0x7349, 0x734b);
  CYC(0x734b, 0x734d); A = 0x0c;

L_734d:
  CYC(0x734d, 0x734e); alu_and(gb, B);
  RET(0x734e); return; // ret

upOrDown:
  CYC(0x734f, 0x7351); L = ENEMY_BASE + 0x30; // var30
  CYC(0x7351, 0x7352); C = mem_rd(gb, HL);
  CYC(0x7352, 0x7354); alu_bit(gb, 4, A);
  CYC(0x7354, 0x7356); A = 0x30;
  if (!(F & FZ)) { CYCT(0x7356, 0x7358); goto L_735a; } // jr nz
  CYC(0x7356, 0x7358);
  CYC(0x7358, 0x735a); A = 0xc0;

L_735a:
  CYC(0x735a, 0x735b); alu_and(gb, C);
  RET(0x735b); return; // ret
}

// Applies speed and updates "adjacentWallsBitset"
void smog_applySpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x735c, 0x735e); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x735e, 0x735f); A = mem_rd(gb, DE);
  CYC(0x735f, 0x7361); A = alu_swap(gb, A);
  CYC(0x7361, 0x7362); alu_rrca(gb);
  CYC(0x7362, 0x7363); E = alu_inc8(gb, E);
  CYC(0x7363, 0x7364); mem_wr(gb, DE, A);
  CALL_C(0x7364, objectApplySpeed_hook, 0x201d, 0x7367);
  smog_updateAdjacentWallsBitset_hook(gb); return; // fallthrough
}

void smog_updateAdjacentWallsBitset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7367, 0x7369); E = ENEMY_BASE + 0x30; // var30
  CYC(0x7369, 0x736a); alu_xor(gb, A);
  CYC(0x736a, 0x736b); mem_wr(gb, DE, A);
  CYC(0x736b, 0x736c); H = D;
  CYC(0x736c, 0x736e); L = ENEMY_BASE + OBJ_YH;
  CYC(0x736e, 0x736f); B = mem_rd(gb, HL);
  CYC(0x736f, 0x7371); L = ENEMY_BASE + OBJ_XH;
  CYC(0x7371, 0x7372); C = mem_rd(gb, HL);
  CYC(0x7372, 0x7374); A = 0x01;
  CYC(0x7374, 0x7376); hram_wr(gb, 0x8b, A);
  CYC(0x7376, 0x7379); SET_HL(0x7396); // adjacent-tile offsets

loop:
  CYC(0x7379, 0x737a); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x737a, 0x737b); alu_add(gb, B);
  CYC(0x737b, 0x737c); B = A;
  CYC(0x737c, 0x737d); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x737d, 0x737e); alu_add(gb, C);
  CYC(0x737e, 0x737f); C = A;
  CYC(0x737f, 0x7380); push_effect(gb, HL);
  CALL_C(0x7380, getTileAtPosition_hook, 0x1447, 0x7383);
  CYC(0x7383, 0x7385); H = 0xce;
  CYC(0x7385, 0x7386); A = mem_rd(gb, HL);
  CYC(0x7386, 0x7387); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7387, 0x7389); goto L_738a; } // jr z
  CYC(0x7387, 0x7389);
  CYC(0x7389, 0x738a); alu_scf(gb);

L_738a:
  CYC(0x738a, 0x738b); SET_HL(pop_effect(gb));
  CYC(0x738b, 0x738d); A = hram_rd(gb, 0x8b);
  CYC(0x738d, 0x738e); alu_rla(gb);
  CYC(0x738e, 0x7390); hram_wr(gb, 0x8b, A);
  if (!(F & FC)) { CYCT(0x7390, 0x7392); goto loop; } // jr nc
  CYC(0x7390, 0x7392);
  CYC(0x7392, 0x7394); E = ENEMY_BASE + 0x30; // var30
  CYC(0x7394, 0x7395); mem_wr(gb, DE, A);
  RET(0x7395); return; // ret
}

// @param[out] zflag nz if smog should begin "firing projectile" animation
void smog_decCounterToFireProjectile_hook(GB *gb) {
  CYC(0x73a6, 0x73a8); E = ENEMY_BASE + OBJ_VAR36;
  CYC(0x73a8, 0x73a9); A = mem_rd(gb, DE);
  CYC(0x73a9, 0x73aa); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x73aa); return; } // ret z
  CYC(0x73aa, 0x73ab);
  CYC(0x73ab, 0x73ac); A = alu_dec8(gb, A);
  CYC(0x73ac, 0x73ad); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CYCT(0x73ad, 0x73af); goto zflag; } // jr nz
  CYC(0x73ad, 0x73af);
  CYC(0x73af, 0x73b0); alu_or(gb, D);
  RET(0x73b0); return; // ret

zflag:
  CYC(0x73b1, 0x73b2); alu_xor(gb, A);
  RET(0x73b2); return; // ret
}

// For given values of subid and var03, this reads one of four randomly chosen values and
// puts that value into var36 (counter until next projectile is fired).
void smog_setCounterToFireProjectile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x73b3, 0x73b5); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x73b5, 0x73b6); A = mem_rd(gb, DE);
  CYC(0x73b6, 0x73b8); alu_and(gb, 0x0f);
  CYC(0x73b8, 0x73ba); alu_sub(gb, 0x02);
  CYC(0x73ba, 0x73bd); SET_HL(0x73d0);
  CYC(0x73bd, 0x73be); smog_addAToHl_from_rst(gb, 0x73be);
  CYC(0x73be, 0x73bf); A = mem_rd(gb, HL);
  CYC(0x73bf, 0x73c0); smog_addAToHl_from_rst(gb, 0x73c0);
  CYC(0x73c0, 0x73c1); E = alu_inc8(gb, E);
  CYC(0x73c1, 0x73c2); A = mem_rd(gb, DE);
  CYC(0x73c2, 0x73c3); smog_addAToHl_from_rst(gb, 0x73c3);
  CYC(0x73c3, 0x73c4); A = mem_rd(gb, HL);
  CYC(0x73c4, 0x73c5); smog_addAToHl_from_rst(gb, 0x73c5);
  CALL_C(0x73c5, getRandomNumber_hook, 0x043e, 0x73c8);
  CYC(0x73c8, 0x73ca); alu_and(gb, 0x03);
  CYC(0x73ca, 0x73cb); smog_addAToHl_from_rst(gb, 0x73cb);
  CYC(0x73cb, 0x73cc); A = mem_rd(gb, HL);
  CYC(0x73cc, 0x73ce); E = ENEMY_BASE + OBJ_VAR36;
  CYC(0x73ce, 0x73cf); mem_wr(gb, DE, A);
  RET(0x73cf); return; // ret
}
