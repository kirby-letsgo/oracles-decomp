#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void armMimic_uninitialized_hook(GB *gb);
void armMimic_state_switchHook_hook(GB *gb);
void armMimic_state_stub_hook(GB *gb);
void armMimic_state8_hook(GB *gb);

static uint16_t armMimic_jump_table(GB *gb) {
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
// ENEMY_ARM_MIMIC
//
// Shares code with ENEMY_LINK_MIMIC (linkMimic.c).
//
// Variables:
//   var30: Animation index
// ==================================================================================================
void enemyCode4e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x612f, ecom_checkHazards_b0e_hook, 0x4051, 0x6132);
  if (F & FZ) { CYCT(0x6132, 0x6134); goto normalStatus; } // jr z
  CYC(0x6132, 0x6134);
  CYC(0x6134, 0x6136); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x6136); return; } // ret c
  CYC(0x6136, 0x6137);
  if (F & FZ) { CYCT(0x6137, 0x613a); enemyDie_hook(gb); return; } // jp z
  CYC(0x6137, 0x613a);
  CYC(0x613a, 0x613b); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x613b, 0x613e); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(0x613b, 0x613e);
  RET(0x613e); return; // ret

normalStatus:
  CYC(0x613f, 0x6141); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6141, 0x6142); A = mem_rd(gb, DE);
  {
    CYC(0x6142, 0x6143); push_effect(gb, 0x6143);
    uint16_t target = armMimic_jump_table(gb);
    if (target == 0x6155) { armMimic_uninitialized_hook(gb); return; }
    if (target == 0x6173) { armMimic_state_stub_hook(gb); return; }
    if (target == 0x6167) { armMimic_state_switchHook_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == 0x6174) { armMimic_state8_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6155, bare global; jump-table target from enemyCode4e, also called directly by
// linkMimic.c's enemyCode64@state_uninitialized.
void armMimic_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6155, 0x6157); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6157, 0x615a); A = mem_rd(gb, w1Link + OBJ_DIRECTION);
  CYC(0x615a, 0x615c); alu_add(gb, 0x02);
  CYC(0x615c, 0x615e); alu_and(gb, 0x03);
  CYC(0x615e, 0x615f); mem_wr(gb, DE, A);
  CALL_C(0x615f, enemySetAnimation_hook, 0x282b, 0x6162);
  CYC(0x6162, 0x6164); A = 0x28; // SPEED_100
  CYC(0x6164, 0x6167); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp
}

// 0e:6167, bare global; jump-table target from enemyCode4e.
void armMimic_state_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6167, 0x6168); E = alu_inc8(gb, E);
  CYC(0x6168, 0x6169); A = mem_rd(gb, DE);
  {
    CYC(0x6169, 0x616a); push_effect(gb, 0x616a);
    uint16_t target = armMimic_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == 0x6172) { RET(0x6172); return; } // ret (substate1/substate2)
    if (target == 0x44e0) { ecom_fallToGroundAndSetState8_b0e_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6173, bare global; jump-table target from enemyCode4e, also from enemyCode64.
void armMimic_state_stub_hook(GB *gb) {
  RET(0x6173); return; // ret
}

// 0e:6174, bare global; jump-table target from enemyCode4e, also called by
// linkMimic.c's linkMimic_state8. Only "normal" state; simply moves in reverse of Link's
// direction.
void armMimic_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6174, 0x6177); A = mem_rd(gb, wLinkAngle);
  CYC(0x6177, 0x6178); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(0x6178); return; } // ret z
  CYC(0x6178, 0x6179);
  CYC(0x6179, 0x617b); alu_add(gb, 0x0f);
  CYC(0x617b, 0x617d); alu_and(gb, 0x1f);
  CYC(0x617d, 0x617f); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x617f, 0x6180); mem_wr(gb, DE, A);
  CALL_C(0x6180, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x6183);
  CYC(0x6183, 0x6184); H = D;
  CYC(0x6184, 0x6186); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6186, 0x6189); A = mem_rd(gb, w1Link + OBJ_DIRECTION);
  CYC(0x6189, 0x618b); alu_add(gb, 0x02);
  CYC(0x618b, 0x618d); alu_and(gb, 0x03);
  CYC(0x618d, 0x618e); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x618e, 0x6190); goto animate; } // jr z
  CYC(0x618e, 0x6190);
  CYC(0x6190, 0x6191); mem_wr(gb, HL, A);
  CALL_C(0x6191, enemySetAnimation_hook, 0x282b, 0x6194);

animate:
  CYC(0x6194, 0x6197); enemyAnimate_hook(gb); return; // jp
}
