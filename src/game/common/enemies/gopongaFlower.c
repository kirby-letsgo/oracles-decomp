#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode25);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+12, b_+15); ecom_updateKnockback_b0d_hook(gb); return; } // jp z
  CYC(b_+12, b_+15);

normalStatus:
  CYC(b_+15, b_+17); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  {
    CYC(b_+18, b_+19); push_effect(gb, b_+19);
    uint16_t target = enemyCode25_jump_table(gb);
    if (target == b_+39) goto state_uninitialized;
    if (target == b_+66) goto state_stub;
    if (target == b_+67) goto state8;
    if (target == b_+80) goto state9;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+39, b_+40); H = D;
  CYC(b_+40, b_+42); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 90);
  CYC(b_+44, b_+46); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+46, b_+47); A = mem_rd(gb, HL);
  CYC(b_+47, b_+48); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+48, b_+50); goto setSpeedAndState8; } // jr z
  CYC(b_+48, b_+50);
  CYC(b_+50, b_+52); L = ENEMY_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+55); alu_add(gb, 0x04);
  CYC(b_+55, b_+56); mem_wr(gb, HL, A);
  CYC(b_+56, b_+58); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(b_+58, b_+60); mem_wr(gb, HL, GV(0x57, 0x54)); // ENEMYCOLLISION_BIG_GOPONGA_FLOWER

setSpeedAndState8:
  CALL_C(b_+60, ecom_setSpeedAndState8_b0d_hook, SYM(ecom_setSpeedAndState8_b0d), b_+63);
  CYC(b_+63, b_+66); TAIL(objectSetVisible83); // jp

state_stub:
  RET(b_+66); return; // ret

state8:
  CALL_C(b_+67, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+70);
  if (!(F & FZ)) { RET_TAKEN(b_+70); return; } // ret nz
  CYC(b_+70, b_+71);
  CYC(b_+71, b_+73); mem_wr(gb, HL, 60);
  CYC(b_+73, b_+74); L = E;
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+75, b_+77); A = 0x01;
  CYC(b_+77, b_+80); TAIL(enemySetAnimation); // jp

state9:
  CALL_C(b_+80, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+83);
  if (F & FZ) { CYCT(b_+83, b_+85); goto closeFlower; } // jr z
  CYC(b_+83, b_+85);
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+88); alu_cp(gb, 40);
  if (!(F & FZ)) { RET_TAKEN(b_+88); return; } // ret nz
  CYC(b_+88, b_+89);
  CYC(b_+89, b_+91); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+93); A = alu_dec8(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+93, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+96); else CYC(b_+93, b_+96); // call nz
  CYC(b_+96, b_+98); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+98); return; } // ret nz
  CYC(b_+98, b_+99);
  CYC(b_+99, b_+101); B = 0x31; // PART_GOPONGA_PROJECTILE
  CYC(b_+101, b_+104); TAIL(ecom_spawnProjectile_b0d); // jp

closeFlower:
  CYC(b_+104, b_+106); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+110); SET_BC(b_+122); // @counter1Vals
  CALL_C(b_+110, addAToBc_hook, 0x006d, b_+113);
  CYC(b_+113, b_+114); A = mem_rd(gb, BC);
  CYC(b_+114, b_+115); mem_wr(gb, HL, A);
  CYC(b_+115, b_+117); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+117, b_+118); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+118, b_+119); alu_xor(gb, A);
  CYC(b_+119, b_+122); TAIL(enemySetAnimation); // jp
}
