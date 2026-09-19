#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode25_jump_table(GB *gb) {
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
// ENEMY_GOPONGA_FLOWER
// ==================================================================================================
void enemyCode25_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5f4b, 0x5f4d); goto normalStatus; } // jr z
  CYC(0x5f4b, 0x5f4d);
  CYC(0x5f4d, 0x5f4f); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5f4f); return; } // ret c
  CYC(0x5f4f, 0x5f50);
  if (F & FZ) { CYCT(0x5f50, 0x5f53); enemyDie_hook(gb); return; } // jp z
  CYC(0x5f50, 0x5f53);
  CYC(0x5f53, 0x5f55); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x5f55, 0x5f56); A = mem_rd(gb, DE);
  CYC(0x5f56, 0x5f57); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5f57, 0x5f5a); ecom_updateKnockback_b0d_hook(gb); return; } // jp z
  CYC(0x5f57, 0x5f5a);

normalStatus:
  CYC(0x5f5a, 0x5f5c); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5f5c, 0x5f5d); A = mem_rd(gb, DE);
  {
    CYC(0x5f5d, 0x5f5e); push_effect(gb, 0x5f5e);
    uint16_t target = enemyCode25_jump_table(gb);
    if (target == 0x5f72) goto state_uninitialized;
    if (target == 0x5f8d) goto state_stub;
    if (target == 0x5f8e) goto state8;
    if (target == 0x5f9b) goto state9;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x5f72, 0x5f73); H = D;
  CYC(0x5f73, 0x5f75); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5f75, 0x5f77); mem_wr(gb, HL, 90);
  CYC(0x5f77, 0x5f79); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5f79, 0x5f7a); A = mem_rd(gb, HL);
  CYC(0x5f7a, 0x5f7b); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5f7b, 0x5f7d); goto setSpeedAndState8; } // jr z
  CYC(0x5f7b, 0x5f7d);
  CYC(0x5f7d, 0x5f7f); L = ENEMY_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(0x5f7f, 0x5f80); A = mem_rd(gb, HL);
  CYC(0x5f80, 0x5f82); alu_add(gb, 0x04);
  CYC(0x5f82, 0x5f83); mem_wr(gb, HL, A);
  CYC(0x5f83, 0x5f85); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x5f85, 0x5f87); mem_wr(gb, HL, 0x57); // ENEMYCOLLISION_BIG_GOPONGA_FLOWER

setSpeedAndState8:
  CALL_C(0x5f87, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x5f8a);
  CYC(0x5f8a, 0x5f8d); objectSetVisible83_hook(gb); return; // jp

state_stub:
  RET(0x5f8d); return; // ret

state8:
  CALL_C(0x5f8e, ecom_decCounter1_b0d_hook, 0x439a, 0x5f91);
  if (!(F & FZ)) { RET_TAKEN(0x5f91); return; } // ret nz
  CYC(0x5f91, 0x5f92);
  CYC(0x5f92, 0x5f94); mem_wr(gb, HL, 60);
  CYC(0x5f94, 0x5f95); L = E;
  CYC(0x5f95, 0x5f96); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5f96, 0x5f98); A = 0x01;
  CYC(0x5f98, 0x5f9b); enemySetAnimation_hook(gb); return; // jp

state9:
  CALL_C(0x5f9b, ecom_decCounter1_b0d_hook, 0x439a, 0x5f9e);
  if (F & FZ) { CYCT(0x5f9e, 0x5fa0); goto closeFlower; } // jr z
  CYC(0x5f9e, 0x5fa0);
  CYC(0x5fa0, 0x5fa1); A = mem_rd(gb, HL);
  CYC(0x5fa1, 0x5fa3); alu_cp(gb, 40);
  if (!(F & FZ)) { RET_TAKEN(0x5fa3); return; } // ret nz
  CYC(0x5fa3, 0x5fa4);
  CYC(0x5fa4, 0x5fa6); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5fa6, 0x5fa7); A = mem_rd(gb, DE);
  CYC(0x5fa7, 0x5fa8); A = alu_dec8(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x5fa8, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5fab); else CYC(0x5fa8, 0x5fab); // call nz
  CYC(0x5fab, 0x5fad); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x5fad); return; } // ret nz
  CYC(0x5fad, 0x5fae);
  CYC(0x5fae, 0x5fb0); B = 0x31; // PART_GOPONGA_PROJECTILE
  CYC(0x5fb0, 0x5fb3); ecom_spawnProjectile_b0d_hook(gb); return; // jp

closeFlower:
  CYC(0x5fb3, 0x5fb5); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5fb5, 0x5fb6); A = mem_rd(gb, DE);
  CYC(0x5fb6, 0x5fb9); SET_BC(0x5fc5); // @counter1Vals
  CALL_C(0x5fb9, addAToBc_hook, 0x006d, 0x5fbc);
  CYC(0x5fbc, 0x5fbd); A = mem_rd(gb, BC);
  CYC(0x5fbd, 0x5fbe); mem_wr(gb, HL, A);
  CYC(0x5fbe, 0x5fc0); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5fc0, 0x5fc1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5fc1, 0x5fc2); alu_xor(gb, A);
  CYC(0x5fc2, 0x5fc5); enemySetAnimation_hook(gb); return; // jp
}
