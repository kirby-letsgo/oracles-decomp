#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

void armos_uninitialized_hook(GB *gb);
void armos_state1_hook(GB *gb);
void armos_state_switchHook_hook(GB *gb);
void armos_state_stub_hook(GB *gb);
void armos_subid00_hook(GB *gb);
void armos_subid00_state8_hook(GB *gb);
void armos_state9_hook(GB *gb);
void armos_subid00_stateA_hook(GB *gb);
void armos_beginMoving_hook(GB *gb);
void armos_subid00_stateB_hook(GB *gb);
void armos_subid00_stateC_hook(GB *gb);
void armos_subid01_hook(GB *gb);
void armos_subid01_state8_hook(GB *gb);
void armos_subid01_stateA_hook(GB *gb);
void armos_subid02_stateB_hook(GB *gb);
void armos_subid03_stateC_hook(GB *gb);
void armos_spawnArmosAtPosition_hook(GB *gb);
void armos_dead_hook(GB *gb);
void armos_clearKilledArmosBuffer_hook(GB *gb);
void armos_replaceTileUnderSelf_hook(GB *gb);

static uint16_t armos_jump_table(GB *gb) {
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

static void armos_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_ARMOS
//
// Variables:
//   subid: If bit 7 is set, it's a real armos; otherwise it's an armos spawner.
//   var31: The initial position of the armos (subid 1 only)
// ==================================================================================================
void enemyCode1d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x58ac, ecom_checkHazards_b0d_hook, 0x4051, 0x58af);
  if (F & FZ) { CYCT(0x58af, 0x58b1); goto normalStatus; } // jr z
  CYC(0x58af, 0x58b1);
  CYC(0x58b1, 0x58b3); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x58b3); return; } // ret c
  CYC(0x58b3, 0x58b4);
  if (F & FZ) { CYCT(0x58b4, 0x58b7); armos_dead_hook(gb); return; } // jp z
  CYC(0x58b4, 0x58b7);
  CYC(0x58b7, 0x58b8); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x58b8, 0x58bb); ecom_updateKnockbackAndCheckHazards_b0d_hook(gb); return; } // jp nz
  CYC(0x58b8, 0x58bb);
  CYC(0x58bb, 0x58bd); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x58bd, 0x58be); A = mem_rd(gb, DE);
  CYC(0x58be, 0x58c0); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { RET_TAKEN(0x58c0); return; } // ret nz
  CYC(0x58c0, 0x58c1);
  CYC(0x58c1, 0x58c3); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x58c3, 0x58c4); A = mem_rd(gb, DE);
  CYC(0x58c4, 0x58c6); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x58c6, 0x58c8); goto normalStatus; } // jr nz
  CYC(0x58c6, 0x58c8);
  CYC(0x58c8, 0x58c9); H = D;
  CYC(0x58c9, 0x58cb); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x58cb, 0x58cc); A = mem_rd(gb, HL);
  CYC(0x58cc, 0x58ce); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(0x58ce, 0x58d0); goto normalStatus; } // jr nc
  CYC(0x58ce, 0x58d0);
  CYC(0x58d0, 0x58d2); mem_wr(gb, HL, 0x09);
  RET(0x58d2); return; // ret

normalStatus:
  CALL_C(0x58d3, ecom_getSubidAndCpStateTo08_b0d_hook, 0x4426, 0x58d6);
  if (!(F & FC)) { CYCT(0x58d6, 0x58d8); goto normalState; } // jr nc
  CYC(0x58d6, 0x58d8);
  {
    CYC(0x58d8, 0x58d9); push_effect(gb, 0x58d9);
    uint16_t target = armos_jump_table(gb);
    if (target == 0x58f1) { armos_uninitialized_hook(gb); return; }
    if (target == 0x591b) { armos_state1_hook(gb); return; }
    if (target == 0x5947) { armos_state_stub_hook(gb); return; }
    if (target == 0x5936) { armos_state_switchHook_hook(gb); return; }
    HANDOFF(target);
  }

normalState:
  CYC(0x58e9, 0x58eb); B = (uint8_t)(B & ~(1 << 7)); // res 7,b
  CYC(0x58eb, 0x58ec); A = B;
  {
    CYC(0x58ec, 0x58ed); push_effect(gb, 0x58ed);
    uint16_t target = armos_jump_table(gb);
    if (target == 0x5948) { armos_subid00_hook(gb); return; }
    if (target == 0x59a1) { armos_subid01_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:58f1, bare global; jump-table target from enemyCode1d.
void armos_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x58f1, 0x58f2); A = B;
  CYC(0x58f2, 0x58f4); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x58f4, 0x58f6); goto gotoState1; } // jr z
  CYC(0x58f4, 0x58f6);
  CYC(0x58f6, 0x58f7); alu_add(gb, A); // add a
  CYC(0x58f7, 0x58fa); SET_HL(0x5917); // @oamFlagsAndSpeeds
  CYC(0x58fa, 0x58fb); armos_addAToHl_from_rst(gb, 0x58fb);
  CYC(0x58fb, 0x58fd); E = ENEMY_BASE + OBJ_OAM_FLAGS;
  CYC(0x58fd, 0x58fe); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x58fe, 0x58ff); mem_wr(gb, DE, A);
  CYC(0x58ff, 0x5900); E = alu_dec8(gb, E);
  CYC(0x5900, 0x5901); mem_wr(gb, DE, A);
  CYC(0x5901, 0x5902); A = mem_rd(gb, HL);
  CALL_C(0x5902, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x5905);
  CYC(0x5905, 0x5907); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5907, 0x5909); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5909, 0x590b); goto noExtraHealth; } // jr z
  CYC(0x5909, 0x590b);
  CYC(0x590b, 0x590d); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x590d, 0x590e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

noExtraHealth:
  CYC(0x590e, 0x5910); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5910, 0x5912); mem_wr(gb, HL, 0x80 | 0x29); // ENEMY_PODOBOO
  RET(0x5912); return; // ret

gotoState1:
  CYC(0x5913, 0x5915); A = 0x01;
  CYC(0x5915, 0x5916); mem_wr(gb, DE, A);
  RET(0x5916); return; // ret
}

// 0d:591b, bare global; jump-table target from enemyCode1d. For subid where bit 7 isn't
// set: spawn armos at all positions where their tiles are. (Enemy.yh currently contains
// the tile to replace, Enemy.xh is the new tile it becomes.)
void armos_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x591b, 0x591d); E = ENEMY_BASE + OBJ_YH;
  CYC(0x591d, 0x591e); A = mem_rd(gb, DE);
  CYC(0x591e, 0x591f); B = A;
  CYC(0x591f, 0x5922); SET_HL(wRoomLayout);
  CYC(0x5922, 0x5924); C = 0xb0; // LARGE_ROOM_HEIGHT<<4

scanTiles:
  CYC(0x5924, 0x5925); A = mem_rd(gb, HL);
  CYC(0x5925, 0x5926); alu_cp(gb, B);
  if (F & FZ) CALL_C_CC(0x5926, armos_spawnArmosAtPosition_hook, 0x5a15, 0x5929); else CYC(0x5926, 0x5929); // call z
  CYC(0x5929, 0x592a); L = alu_inc8(gb, L);
  CYC(0x592a, 0x592b); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(0x592b, 0x592d); goto scanTiles; } // jr nz
  CYC(0x592b, 0x592d);
  CALL_C(0x592d, armos_clearKilledArmosBuffer_hook, 0x5a54, 0x5930);
  CALL_C(0x5930, decNumEnemies_hook, 0x24b3, 0x5933);
  CYC(0x5933, 0x5936); enemyDelete_hook(gb); return; // jp
}

// 0d:5936, bare global; jump-table target from enemyCode1d.
void armos_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5936, 0x5937); E = alu_inc8(gb, E);
  CYC(0x5937, 0x5938); A = mem_rd(gb, DE);
  {
    CYC(0x5938, 0x5939); push_effect(gb, 0x5939);
    uint16_t target = armos_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x5941) { RET(0x5941); return; } // ret (substate1/substate2)
    if (target == 0x5942) goto substate3;
    HANDOFF(target);
  }

substate3:
  CYC(0x5942, 0x5944); B = 0x0b;
  CYC(0x5944, 0x5947); ecom_fallToGroundAndSetState_b0d_hook(gb); return; // jp
}

// 0d:5947, bare global; jump-table target from enemyCode1d.
void armos_state_stub_hook(GB *gb) {
  RET(0x5947); return; // ret
}

// 0d:5948, bare global; jump-table target from enemyCode1d@normalState.
void armos_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5948, 0x5949); A = mem_rd(gb, DE);
  CYC(0x5949, 0x594b); alu_sub(gb, 0x08);
  {
    CYC(0x594b, 0x594c); push_effect(gb, 0x594c);
    uint16_t target = armos_jump_table(gb);
    if (target == 0x5956) { armos_subid00_state8_hook(gb); return; }
    if (target == 0x595f) { armos_state9_hook(gb); return; }
    if (target == 0x596d) { armos_subid00_stateA_hook(gb); return; }
    if (target == 0x5987) { armos_subid00_stateB_hook(gb); return; }
    if (target == 0x5991) { armos_subid00_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:5956, bare global; jump-table target from armos_subid00, also reached by genuine call
// from armos_subid01_state8. Waiting for Link to touch the statue (or for the wcca2 trigger).
void armos_subid00_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5956, 0x5959); A = mem_rd(gb, wcca2);
  CYC(0x5959, 0x595a); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x595a); return; } // ret z
  CYC(0x595a, 0x595b);
  CYC(0x595b, 0x595d); A = 0x09;
  CYC(0x595d, 0x595e); mem_wr(gb, DE, A);
  RET(0x595e); return; // ret
}

// 0d:595f, bare global; jump-table target shared by armos_subid00 and armos_subid01.
// The statue was just activated.
void armos_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x595f, 0x5960); H = D;
  CYC(0x5960, 0x5961); L = E;
  CYC(0x5961, 0x5962); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0a
  CYC(0x5962, 0x5964); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5964, 0x5966); mem_wr(gb, HL, 60);
  CYC(0x5966, 0x5968); L = ENEMY_BASE + OBJ_YH;
  CYC(0x5968, 0x5969); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5969, 0x596a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x596a, 0x596d); objectSetVisible82_hook(gb); return; // jp
}

// 0d:596d, bare global; jump-table target from armos_subid00. Flickering until it starts
// moving; falls through into armos_beginMoving.
void armos_subid00_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x596d, ecom_decCounter1_b0d_hook, 0x439a, 0x5970);
  if (!(F & FZ)) { CYCT(0x5970, 0x5973); ecom_flickerVisibility_b0d_hook(gb); return; } // jp nz
  CYC(0x5970, 0x5973);
  CYC(0x5973, 0x5975); A = 0x1e; // ENEMYCOLLISION_ACTIVE_RED_ARMOS
  armos_beginMoving_hook(gb); return; // fallthrough
}

// 0d:5975, bare global; falls into from armos_subid00_stateA, also reached by genuine jp
// from armos_subid01_stateA.
// @param a EnemyCollisionMode
void armos_beginMoving_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5975, 0x5976); L = E;
  CYC(0x5976, 0x5977); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0b
  CYC(0x5977, 0x5979); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5979, 0x597b); mem_wr(gb, HL, 0x80 | 0x1d); // ENEMY_ARMOS
  CYC(0x597b, 0x597c); L = alu_inc8(gb, L);
  CYC(0x597c, 0x597d); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a -- set enemyCollisionMode
  CYC(0x597d, 0x597f); A = 0x06;
  CYC(0x597f, 0x5980); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x5980, 0x5981); mem_wr(gb, HL, A);
  CALL_C(0x5981, armos_replaceTileUnderSelf_hook, 0x5a62, 0x5984);
  CYC(0x5984, 0x5987); objectSetVisiblec2_hook(gb); return; // jp
}

// 0d:5987, bare global; jump-table target from armos_subid00. Choose a direction to move;
// falls through into armos_subid00_stateC.
void armos_subid00_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5987, 0x5988); H = D;
  CYC(0x5988, 0x5989); L = E;
  CYC(0x5989, 0x598a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0c
  CYC(0x598a, 0x598c); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x598c, 0x598e); mem_wr(gb, HL, 61);
  CALL_C(0x598e, ecom_setRandomCardinalAngle_b0d_hook, 0x43c6, 0x5991);
  armos_subid00_stateC_hook(gb); return; // fallthrough
}

// 0d:5991, bare global; jump-table target from armos_subid00, also falls into from
// armos_subid00_stateB. Moving in some direction for [counter1] frames.
void armos_subid00_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5991, ecom_decCounter1_b0d_hook, 0x439a, 0x5994);
  if (!(F & FZ)) CALL_C_CC(0x5994, ecom_applyVelocityForTopDownEnemyNoHoles_b0d_hook, 0x414c, 0x5997); else CYC(0x5994, 0x5997); // call nz
  if (!(F & FZ)) { CYCT(0x5997, 0x5999); goto stillMoving; } // jr nz
  CYC(0x5997, 0x5999);
  CYC(0x5999, 0x599b); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x599b, 0x599d); A = 0x0b;
  CYC(0x599d, 0x599e); mem_wr(gb, DE, A);

stillMoving:
  CYC(0x599e, 0x59a1); enemyAnimate_hook(gb); return; // jp
}

// 0d:59a1, bare global; jump-table target from armos_subid00_hook... actually from
// enemyCode1d@normalState.
void armos_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x59a1, 0x59a2); A = mem_rd(gb, DE);
  CYC(0x59a2, 0x59a4); alu_sub(gb, 0x08);
  {
    CYC(0x59a4, 0x59a5); push_effect(gb, 0x59a5);
    uint16_t target = armos_jump_table(gb);
    if (target == 0x59af) { armos_subid01_state8_hook(gb); return; }
    if (target == 0x595f) { armos_state9_hook(gb); return; }
    if (target == 0x59d9) { armos_subid01_stateA_hook(gb); return; }
    if (target == 0x59e4) { armos_subid02_stateB_hook(gb); return; }
    if (target == 0x5a02) { armos_subid03_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:59af, bare global; jump-table target from armos_subid01. Waiting for Link to approach
// the statue.
void armos_subid01_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59af, armos_subid00_state8_hook, 0x5956, 0x59b2);
  if (!(F & FZ)) { RET_TAKEN(0x59b2); return; } // ret nz
  CYC(0x59b2, 0x59b3);
  CYC(0x59b3, 0x59b4); H = D;
  CYC(0x59b4, 0x59b6); L = ENEMY_BASE + OBJ_YH;
  CYC(0x59b6, 0x59b8); A = hram_rd(gb, 0xb0); // hEnemyTargetY
  CYC(0x59b8, 0x59b9); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x59b9, 0x59bb); alu_add(gb, 0x18);
  CYC(0x59bb, 0x59bd); alu_cp(gb, 0x31);
  if (!(F & FC)) { RET_TAKEN(0x59bd); return; } // ret nc
  CYC(0x59bd, 0x59be);
  CYC(0x59be, 0x59bf); B = mem_rd(gb, HL);
  CYC(0x59bf, 0x59c1); L = ENEMY_BASE + OBJ_XH;
  CYC(0x59c1, 0x59c3); A = hram_rd(gb, 0xb1); // hEnemyTargetX
  CYC(0x59c3, 0x59c4); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x59c4, 0x59c6); alu_add(gb, 0x18);
  CYC(0x59c6, 0x59c8); alu_cp(gb, 0x31);
  if (!(F & FC)) { RET_TAKEN(0x59c8); return; } // ret nc
  CYC(0x59c8, 0x59c9);
  CYC(0x59c9, 0x59ca); A = mem_rd(gb, HL);
  CYC(0x59ca, 0x59cc); alu_and(gb, 0xf0);
  CYC(0x59cc, 0x59ce); A = alu_swap(gb, A);
  CYC(0x59ce, 0x59cf); C = A;
  CYC(0x59cf, 0x59d0); A = B;
  CYC(0x59d0, 0x59d2); alu_and(gb, 0xf0);
  CYC(0x59d2, 0x59d3); alu_or(gb, C);
  CYC(0x59d3, 0x59d5); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x59d5, 0x59d6); mem_wr(gb, HL, A);
  CYC(0x59d6, 0x59d7); L = E;
  CYC(0x59d7, 0x59d8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 9
  RET(0x59d8); return; // ret
}

// 0d:59d9, bare global; jump-table target from armos_subid01. Flickering until it starts
// moving.
void armos_subid01_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x59d9, ecom_decCounter1_b0d_hook, 0x439a, 0x59dc);
  if (!(F & FZ)) { CYCT(0x59dc, 0x59df); ecom_flickerVisibility_b0d_hook(gb); return; } // jp nz
  CYC(0x59dc, 0x59df);
  CYC(0x59df, 0x59e1); A = 0x54; // ENEMYCOLLISION_ACTIVE_BLUE_ARMOS
  CYC(0x59e1, 0x59e4); armos_beginMoving_hook(gb); return; // jp
}

// 0d:59e4, bare global; jump-table target from armos_subid01. Choose random new direction
// & amount of time to move in that direction.
void armos_subid02_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x59e4, 0x59e6); A = 0x0c;
  CYC(0x59e6, 0x59e7); mem_wr(gb, DE, A); // [state] = $0c
  CYC(0x59e7, 0x59ea); SET_BC(0x0303);
  CALL_C(0x59ea, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x59ed);
  CYC(0x59ed, 0x59ee); A = B;
  CYC(0x59ee, 0x59f1); SET_HL(0x59fe); // @counter1Vals
  CYC(0x59f1, 0x59f2); armos_addAToHl_from_rst(gb, 0x59f2);
  CYC(0x59f2, 0x59f4); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x59f4, 0x59f5); A = mem_rd(gb, HL);
  CYC(0x59f5, 0x59f6); mem_wr(gb, DE, A);
  CYC(0x59f6, 0x59f7); A = C;
  CYC(0x59f7, 0x59f8); alu_or(gb, A);
  if (F & FZ) { CYCT(0x59f8, 0x59fb); ecom_updateCardinalAngleTowardTarget_b0d_hook(gb); return; } // jp z
  CYC(0x59f8, 0x59fb);
  CYC(0x59fb, 0x59fe); ecom_setRandomCardinalAngle_b0d_hook(gb); return; // jp
}

// 0d:5a02, bare global; jump-table target from armos_subid01. Moving in some direction for
// [counter1] frames.
void armos_subid03_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5a02, ecom_decCounter1_b0d_hook, 0x439a, 0x5a05);
  if (F & FZ) { CYCT(0x5a05, 0x5a07); goto stoppedMoving; } // jr z
  CYC(0x5a05, 0x5a07);
  CALL_C(0x5a07, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x5a0a);
  if (F & FZ) { CYCT(0x5a0a, 0x5a0c); goto stoppedMoving; } // jr z
  CYC(0x5a0a, 0x5a0c);
  CYC(0x5a0c, 0x5a0f); enemyAnimate_hook(gb); return; // jp

stoppedMoving:
  CYC(0x5a0f, 0x5a11); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5a11, 0x5a13); A = 0x0b;
  CYC(0x5a13, 0x5a14); mem_wr(gb, DE, A);
  RET(0x5a14); return; // ret
}

// 0d:5a15, bare global; called from armos_state1 for each matching tile position.
// @param l Position to spawn at
void armos_spawnArmosAtPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  PUSH(0x5a15, BC);
  PUSH(0x5a16, HL);
  CYC(0x5a17, 0x5a18); C = L;
  CYC(0x5a18, 0x5a1a); B = 0x1d; // ENEMY_ARMOS
  CALL_C(0x5a1a, ecom_spawnEnemyWithSubid01_b0d_hook, 0x4373, 0x5a1d);
  if (!(F & FZ)) { CYCT(0x5a1d, 0x5a1f); goto spawnRet; } // jr nz
  CYC(0x5a1d, 0x5a1f);
  CYC(0x5a1f, 0x5a20); E = L;
  CYC(0x5a20, 0x5a21); A = mem_rd(gb, DE);
  CYC(0x5a21, 0x5a23); A = (uint8_t)(A | (1 << 7)); // set 7,a
  CYC(0x5a23, 0x5a24); mem_wr(gb, HL, A); // [child.subid] = [this.subid]|$80
  CYC(0x5a24, 0x5a26); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5a26, 0x5a28); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5a28, 0x5a29); A = mem_rd(gb, DE);
  CYC(0x5a29, 0x5a2a); mem_wr(gb, HL, A);
  CYC(0x5a2a, 0x5a2b); L = E;
  CYC(0x5a2b, 0x5a2c); A = C;
  CYC(0x5a2c, 0x5a2e); alu_and(gb, 0x0f);
  CYC(0x5a2e, 0x5a30); A = alu_swap(gb, A);
  CYC(0x5a30, 0x5a32); alu_add(gb, 0x08);
  CYC(0x5a32, 0x5a33); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(0x5a33, 0x5a34); L = alu_dec8(gb, L);
  CYC(0x5a34, 0x5a35); A = C;
  CYC(0x5a35, 0x5a37); alu_and(gb, 0xf0);
  CYC(0x5a37, 0x5a39); alu_add(gb, 0x06);
  CYC(0x5a39, 0x5a3a); mem_wr(gb, HL, A);

spawnRet:
  SET_HL(POP(0x5a3a));
  SET_BC(POP(0x5a3b));
  RET(0x5a3c); return; // ret
}

// 0d:5a3d, bare global; called from enemyCode1d.
void armos_dead_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5a3d, 0x5a3f); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5a3f, 0x5a40); A = mem_rd(gb, DE);
  CYC(0x5a40, 0x5a41); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x5a41, 0x5a44); enemyDie_hook(gb); return; } // jp nc
  CYC(0x5a41, 0x5a44);
  CYC(0x5a44, 0x5a46); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x5a46, 0x5a47); A = mem_rd(gb, DE);
  CYC(0x5a47, 0x5a48); B = A;
  CYC(0x5a48, 0x5a4b); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions - 1);

findFreeSlot:
  CYC(0x5a4b, 0x5a4c); L = alu_inc8(gb, L);
  CYC(0x5a4c, 0x5a4d); A = mem_rd(gb, HL);
  CYC(0x5a4d, 0x5a4e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5a4e, 0x5a50); goto findFreeSlot; } // jr nz
  CYC(0x5a4e, 0x5a50);
  CYC(0x5a50, 0x5a51); mem_wr(gb, HL, B);
  CYC(0x5a51, 0x5a54); enemyDie_hook(gb); return; // jp
}

// 0d:5a54, bare global; called from armos_state1.
void armos_clearKilledArmosBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5a54, 0x5a57); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions);
  CYC(0x5a57, 0x5a58); alu_xor(gb, A);
  CYC(0x5a58, 0x5a5a); B = 0x04;

clearLoop:
  CYC(0x5a5a, 0x5a5b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x5a5b, 0x5a5c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x5a5c, 0x5a5d); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x5a5d, 0x5a5e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x5a5e, 0x5a5f); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x5a5f, 0x5a61); goto clearLoop; } // jr nz
  CYC(0x5a5f, 0x5a61);
  RET(0x5a61); return; // ret
}

// 0d:5a62, bare global; called from armos_beginMoving. Replace the tile underneath the
// armos with [var30].
void armos_replaceTileUnderSelf_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5a62, objectGetTileAtPosition_hook, 0x1444, 0x5a65);
  CYC(0x5a65, 0x5a66); C = L;
  CYC(0x5a66, 0x5a68); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5a68, 0x5a69); A = mem_rd(gb, DE);
  CYC(0x5a69, 0x5a6c); setTile_hook(gb); return; // jp
}
