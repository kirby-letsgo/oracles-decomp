#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode4e), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode4e), (from), (to), true)

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
  BASE(enemyCode4e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0e_hook, SYM(ecom_checkHazards_b0e), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+12); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+12, b_+15); ecom_updateKnockbackAndCheckHazards_b0e_hook(gb); return; } // jp nz
  CYC(b_+12, b_+15);
  RET(b_+15); return; // ret

normalStatus:
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  {
    CYC(b_+19, b_+20); push_effect(gb, b_+20);
    uint16_t target = armMimic_jump_table(gb);
    if (target == SYM(armMimic_uninitialized)) { armMimic_uninitialized_hook(gb); return; }
    if (target == SYM(armMimic_state_stub)) { armMimic_state_stub_hook(gb); return; }
    if (target == SYM(armMimic_state_switchHook)) { armMimic_state_switchHook_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == SYM(armMimic_state8)) { armMimic_state8_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6155, bare global; jump-table target from enemyCode4e, also called directly by
// linkMimic.c's enemyCode64@state_uninitialized.
void armMimic_uninitialized_hook(GB *gb) {
  BASE(armMimic_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+2, b_+5); A = mem_rd(gb, w1Link + OBJ_DIRECTION);
  CYC(b_+5, b_+7); alu_add(gb, 0x02);
  CYC(b_+7, b_+9); alu_and(gb, 0x03);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CALL_C(b_+10, enemySetAnimation_hook, SYM(enemySetAnimation), b_+13);
  CYC(b_+13, b_+15); A = 0x28; // SPEED_100
  CYC(b_+15, SYM(armMimic_state_switchHook)); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp
}

// 0e:6167, bare global; jump-table target from enemyCode4e.
void armMimic_state_switchHook_hook(GB *gb) {
  BASE(armMimic_state_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  {
    CYC(b_+2, b_+3); push_effect(gb, b_+3);
    uint16_t target = armMimic_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0e)) { ecom_incSubstate_b0e_hook(gb); return; }
    if (target == b_+11) { RET(b_+11); return; } // ret (substate1/substate2)
    if (target == SYM(ecom_fallToGroundAndSetState8_b0e)) { ecom_fallToGroundAndSetState8_b0e_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6173, bare global; jump-table target from enemyCode4e, also from enemyCode64.
void armMimic_state_stub_hook(GB *gb) {
  BASE(armMimic_state_stub);
  RET(b_+0); return; // ret
}

// 0e:6174, bare global; jump-table target from enemyCode4e, also called by
// linkMimic.c's linkMimic_state8. Only "normal" state; simply moves in reverse of Link's
// direction.
void armMimic_state8_hook(GB *gb) {
  BASE(armMimic_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkAngle);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; } // ret z
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); alu_add(gb, 0x0f);
  CYC(b_+7, b_+9); alu_and(gb, 0x1f);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CALL_C(b_+12, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0e), b_+15);
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+18, b_+21); A = mem_rd(gb, w1Link + OBJ_DIRECTION);
  CYC(b_+21, b_+23); alu_add(gb, 0x02);
  CYC(b_+23, b_+25); alu_and(gb, 0x03);
  CYC(b_+25, b_+26); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+26, b_+28); goto animate; } // jr z
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+29); mem_wr(gb, HL, A);
  CALL_C(b_+29, enemySetAnimation_hook, SYM(enemySetAnimation), b_+32);

animate:
  CYC(b_+32, SYM(enemyCode4f)); enemyAnimate_hook(gb); return; // jp
}
