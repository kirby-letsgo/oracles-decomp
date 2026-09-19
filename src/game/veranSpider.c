#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode0f_jump_table(GB *gb) {
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

void veranSpider_state_uninitialized_hook(GB *gb);
void veranSpider_state_switchHook_hook(GB *gb);
void veranSpider_state_scentSeed_hook(GB *gb);
void veranSpider_updateAnimation_hook(GB *gb);
void veranSpider_gotoState9_hook(GB *gb);
void veranSpider_state_stub_hook(GB *gb);
void veranSpider_state8_hook(GB *gb);
void veranSpider_state9_hook(GB *gb);
void veranSpider_animate_hook(GB *gb);
void veranSpider_stateA_hook(GB *gb);
void veranSpider_setRandomAngleAndCounter1_hook(GB *gb);

// ==================================================================================================
// ENEMY_VERAN_SPIDER
// ==================================================================================================
void enemyCode0f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // Kill spiders when a cutscene trigger occurs
  CYC(0x67fd, 0x67fe); B = A;
  CYC(0x67fe, 0x6801); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x6801, 0x6802); alu_or(gb, A);
  CYC(0x6802, 0x6803); A = B;
  if (F & FZ) { CYCT(0x6803, 0x6805); goto afterStatusOverride; } // jr z
  CYC(0x6803, 0x6805);
  CYC(0x6805, 0x6807); A = 0x03; // ENEMYSTATUS_NO_HEALTH

afterStatusOverride:
  CYC(0x6807, 0x6808); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6808, 0x680a); goto normalStatus; } // jr z
  CYC(0x6808, 0x680a);
  CYC(0x680a, 0x680c); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { CYCT(0x680c, 0x680d); ret_effect(gb); return; } // ret c
  CYC(0x680c, 0x680d);
  if (F & FZ) { CYCT(0x680d, 0x6810); enemyDie_hook(gb); return; } // jp z
  CYC(0x680d, 0x6810);
  CYC(0x6810, 0x6811); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6811, 0x6814); ecom_updateKnockback_b0d_hook(gb); return; } // jp nz
  CYC(0x6811, 0x6814);
  RET(0x6814); return;

normalStatus:
  CALL_C(0x6815, ecom_checkScentSeedActive_b0d_hook, 0x44b6, 0x6818);
  if (F & FZ) { CYCT(0x6818, 0x681a); goto afterSpeedOverride; } // jr z
  CYC(0x6818, 0x681a);
  CYC(0x681a, 0x681c); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x681c, 0x681e); A = 0x32; // SPEED_140
  CYC(0x681e, 0x681f); mem_wr(gb, DE, A);

afterSpeedOverride:
  CYC(0x681f, 0x6821); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6821, 0x6822); A = mem_rd(gb, DE);
  CYC(0x6822, 0x6823); push_effect(gb, 0x6823);
  {
    uint16_t target = enemyCode0f_jump_table(gb);
    if (target == 0x6870) { veranSpider_state_switchHook_hook(gb); return; }
    if (target == 0x6881) { veranSpider_state_scentSeed_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x68ae) { veranSpider_state8_hook(gb); return; }
    if (target == 0x68cc) { veranSpider_state9_hook(gb); return; }
    if (target == 0x6900) { veranSpider_stateA_hook(gb); return; }
    if (target == 0x6839) { veranSpider_state_uninitialized_hook(gb); return; }
    veranSpider_state_stub_hook(gb); return; // states 1, 2, 6, 7 all target 0x68ad
  }
}

void veranSpider_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6839, 0x683b); A = 0x8a; // PALH_8a
  CALL_C(0x683b, loadPaletteHeader_hook, 0x050b, 0x683e);

  // Choose a random position roughly within the current screen bounds to spawn the spider at.
retryPosition:
  CALL_C(0x683e, getRandomNumber_hook, 0x043e, 0x6841);
  CYC(0x6841, 0x6843); alu_and(gb, 0x7f);
  CYC(0x6843, 0x6845); alu_cp(gb, 0x7a); // $70 + SCREEN_WIDTH
  if (!(F & FC)) { CYCT(0x6845, 0x6847); goto retryPosition; } // jr nc
  CYC(0x6845, 0x6847);
  CYC(0x6847, 0x6848); C = A;
  CALL_C(0x6848, objectSetShortPosition_hook, 0x20c3, 0x684b);
  // Adjust position to be relative to screen bounds
  CYC(0x684b, 0x684d); A = hram_rd(gb, 0xac); // hCameraX
  CYC(0x684d, 0x684e); alu_add(gb, mem_rd(gb, HL));
  CYC(0x684e, 0x684f); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x684f, 0x6850); C = A;
  CYC(0x6850, 0x6851); L = alu_dec8(gb, L);
  CYC(0x6851, 0x6853); A = hram_rd(gb, 0xaa); // hCameraY
  CYC(0x6853, 0x6854); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6854, 0x6855); mem_wr(gb, HL, A);
  CYC(0x6855, 0x6856); B = A;
  // If solid at this position, try again next frame.
  CALL_C(0x6856, getTileCollisionsAtPosition_hook, 0x14b7, 0x6859);
  if (!(F & FZ)) { CYCT(0x6859, 0x685a); ret_effect(gb); return; } // ret nz
  CYC(0x6859, 0x685a);
  CYC(0x685a, 0x685c); C = 0x08;
  CALL_C(0x685c, ecom_setZAboveScreen_b0d_hook, 0x4446, 0x685f);
  CYC(0x685f, 0x6861); A = 0x0f; // SPEED_60
  CALL_C(0x6861, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x6864);
  CYC(0x6864, 0x6866); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6866, 0x6868); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x6868, 0x686a); A = 0x59; // SND_FALLINHOLE
  CALL_C(0x686a, playSound_b00_hook, 0x0c98, 0x686d);
  CYC(0x686d, 0x6870); objectSetVisiblec1_hook(gb); return; // jp
}

void veranSpider_state_switchHook_hook(GB *gb) {
  CYC(0x6870, 0x6871); E = alu_inc8(gb, E);
  CYC(0x6871, 0x6872); A = mem_rd(gb, DE);
  CYC(0x6872, 0x6873); push_effect(gb, 0x6873);
  {
    uint16_t target = enemyCode0f_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x687c) {
      CYC(0x687c, 0x687e); B = 0x09;
      CYC(0x687e, 0x6881); ecom_fallToGroundAndSetState_b0d_hook(gb); return; // jp
    }
    RET(0x687b); return; // @substate1/@substate2 both target 0x687b, a bare `ret`
  }
}

void veranSpider_state_scentSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6881, 0x6884); A = W8(wScentSeedActive);
  CYC(0x6884, 0x6885); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6885, 0x6887); veranSpider_gotoState9_hook(gb); return; } // jr z
  CYC(0x6885, 0x6887);
  CALL_C(0x6887, ecom_updateAngleToScentSeed_b0d_hook, 0x44cb, 0x688a);
  CYC(0x688a, 0x688c); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x688c, 0x688d); A = mem_rd(gb, DE);
  CYC(0x688d, 0x688f); alu_and(gb, 0x18);
  CYC(0x688f, 0x6891); alu_add(gb, 0x04);
  CYC(0x6891, 0x6892); mem_wr(gb, DE, A);
  CALL_C(0x6892, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x6895);
  veranSpider_updateAnimation_hook(gb); return; // falls through
}

void veranSpider_updateAnimation_hook(GB *gb) {
  CYC(0x6895, 0x6896); H = D;
  CYC(0x6896, 0x6898); L = ENEMY_BASE + OBJ_ANIM_COUNTER;
  CYC(0x6898, 0x6899); A = mem_rd(gb, HL);
  CYC(0x6899, 0x689b); alu_sub(gb, 0x03);
  if (!(F & FC)) { CYCT(0x689b, 0x689d); goto incAndSet; } // jr nc
  CYC(0x689b, 0x689d);
  CYC(0x689d, 0x689e); alu_xor(gb, A);

incAndSet:
  CYC(0x689e, 0x689f); A = alu_inc8(gb, A);
  CYC(0x689f, 0x68a0); mem_wr(gb, HL, A);
  CYC(0x68a0, 0x68a3); enemyAnimate_hook(gb); return; // jp
}

void veranSpider_gotoState9_hook(GB *gb) {
  CYC(0x68a3, 0x68a4); H = D;
  CYC(0x68a4, 0x68a6); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x68a6, 0x68a8); mem_wr(gb, HL, 0x09);
  CYC(0x68a8, 0x68aa); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x68aa, 0x68ac); mem_wr(gb, HL, 0x0f); // SPEED_60
  RET(0x68ac); return;
}

void veranSpider_state_stub_hook(GB *gb) {
  RET(0x68ad); return;
}

// Falling from sky
void veranSpider_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x68ae, 0x68b0); C = 0x0e;
  CALL_C(0x68b0, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x68b3);
  if (!(F & FZ)) { CYCT(0x68b3, 0x68b4); ret_effect(gb); return; } // ret nz
  CYC(0x68b3, 0x68b4);
  // Landed on ground
  CYC(0x68b4, 0x68b6); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x68b6, 0x68b7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x68b7, 0x68b8); mem_wr(gb, HL, A);
  CYC(0x68b8, 0x68ba); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x68ba, 0x68bb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  // Enable scent seeds
  CYC(0x68bb, 0x68bd); L = ENEMY_BASE + OBJ_VAR3F;
  CYC(0x68bd, 0x68bf); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 4)));
  CALL_C(0x68bf, objectSetVisiblec2_hook, 0x1e45, 0x68c2);
  CYC(0x68c2, 0x68c4); A = 0x52; // SND_BOMB_LAND
  CALL_C(0x68c4, playSound_b00_hook, 0x0c98, 0x68c7);
  CALL_C(0x68c7, veranSpider_setRandomAngleAndCounter1_hook, 0x6912, 0x68ca);
  CYC(0x68ca, 0x68cc); veranSpider_animate_hook(gb); return; // jr
}

// Moving in some direction for [counter1] frames
void veranSpider_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  // Check if Link is along a diagonal relative to self?
  CALL_C(0x68cc, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x68cf);
  CYC(0x68cf, 0x68d1); alu_and(gb, 0x07);
  CYC(0x68d1, 0x68d3); alu_sub(gb, 0x04);
  CYC(0x68d3, 0x68d4); A = alu_inc8(gb, A);
  CYC(0x68d4, 0x68d6); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x68d6, 0x68d8); goto moveNormally; } // jr nc
  CYC(0x68d6, 0x68d8);
  // He is on a diagonal; if counter2 is zero, go to state $0a (charge at Link).
  CYC(0x68d8, 0x68da); E = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x68da, 0x68db); A = mem_rd(gb, DE);
  CYC(0x68db, 0x68dc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x68dc, 0x68de); goto moveNormally; } // jr nz
  CYC(0x68dc, 0x68de);
  CALL_C(0x68de, ecom_updateAngleTowardTarget_b0d_hook, 0x43bf, 0x68e1);
  CYC(0x68e1, 0x68e3); alu_and(gb, 0x18);
  CYC(0x68e3, 0x68e5); alu_add(gb, 0x04);
  CYC(0x68e5, 0x68e6); mem_wr(gb, DE, A);
  CALL_C(0x68e6, ecom_incState_b0d_hook, 0x4000, 0x68e9);
  CYC(0x68e9, 0x68eb); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x68eb, 0x68ed); mem_wr(gb, HL, 0x32); // SPEED_140
  CYC(0x68ed, 0x68ef); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x68ef, 0x68f1); mem_wr(gb, HL, 120);
  RET(0x68f1); return;

moveNormally:
  CALL_C(0x68f2, ecom_decCounter2_b0d_hook, 0x43a3, 0x68f5);
  CYC(0x68f5, 0x68f6); L = alu_dec8(gb, L);
  CYC(0x68f6, 0x68f7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [counter1]--
  if (!(F & FZ)) CALL_C_CC(0x68f7, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x68fa); else CYC(0x68f7, 0x68fa); // call nz
  if (F & FZ) { CYCT(0x68fa, 0x68fd); veranSpider_setRandomAngleAndCounter1_hook(gb); return; } // jp z
  CYC(0x68fa, 0x68fd);
  veranSpider_animate_hook(gb); return; // falls through
}

void veranSpider_animate_hook(GB *gb) {
  CYC(0x68fd, 0x6900); enemyAnimate_hook(gb); return; // jp
}

// Charging in some direction for [counter1] frames
void veranSpider_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6900, ecom_decCounter1_b0d_hook, 0x439a, 0x6903);
  if (F & FZ) { CYCT(0x6903, 0x6905); goto afterCharge; } // jr z
  CYC(0x6903, 0x6905);
  CALL_C(0x6905, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x6908);
  if (!(F & FZ)) { CYCT(0x6908, 0x690b); veranSpider_updateAnimation_hook(gb); return; } // jp nz
  CYC(0x6908, 0x690b);

afterCharge:
  CALL_C(0x690b, veranSpider_gotoState9_hook, 0x68a3, 0x690e);
  CYC(0x690e, 0x6910); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x6910, 0x6912); mem_wr(gb, HL, 0x40);
  veranSpider_setRandomAngleAndCounter1_hook(gb); return; // falls through
}

void veranSpider_setRandomAngleAndCounter1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6912, 0x6915); SET_BC(0x1870);
  CALL_C(0x6915, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x6918);
  CYC(0x6918, 0x691a); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x691a, 0x691b); A = B;
  CYC(0x691b, 0x691d); alu_add(gb, 0x04);
  CYC(0x691d, 0x691e); mem_wr(gb, DE, A);
  CYC(0x691e, 0x6920); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6920, 0x6921); A = C;
  CYC(0x6921, 0x6923); alu_add(gb, 0x70);
  CYC(0x6923, 0x6924); mem_wr(gb, DE, A);
  RET(0x6924); return;
}
