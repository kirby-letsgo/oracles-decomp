#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode08_jump_table(GB *gb) {
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
// ENEMY_RIVER_ZORA
// ==================================================================================================
void enemyCode08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x44f0, 0x44f2); goto normalStatus; } // jr z
  CYC(0x44f0, 0x44f2);
  CYC(0x44f2, 0x44f4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x44f4); return; } // ret c
  CYC(0x44f4, 0x44f5);
  if (F & FZ) { CYCT(0x44f5, 0x44f8); enemyDie_hook(gb); return; } // jp z
  CYC(0x44f5, 0x44f8);

normalStatus:
  CYC(0x44f8, 0x44fa); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x44fa, 0x44fb); A = mem_rd(gb, DE);
  {
    CYC(0x44fb, 0x44fc); push_effect(gb, 0x44fc);
    uint16_t target = enemyCode08_jump_table(gb);
    if (target == 0x4514) goto state_uninitialized;
    if (target == 0x4518) goto state_stub;
    if (target == 0x4519) goto state_08;
    if (target == 0x4520) goto state_09;
    if (target == 0x454a) goto state_0a;
    if (target == 0x455a) goto state_0b;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x4514, 0x4516); A = 0x09;
  CYC(0x4516, 0x4517); mem_wr(gb, DE, A);
  RET(0x4517); return; // ret

state_stub:
  RET(0x4518); return; // ret

state_08:
  CALL_C(0x4519, ecom_decCounter1_b0d_hook, 0x439a, 0x451c);
  if (!(F & FZ)) { RET_TAKEN(0x451c); return; } // ret nz
  CYC(0x451c, 0x451d);
  CYC(0x451d, 0x451e); L = E;
  CYC(0x451e, 0x451f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x451f); return; // ret

state_09:
  CALL_C(0x4520, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4523);
  CYC(0x4523, 0x4525); alu_cp(gb, 0x98); // (SCREEN_WIDTH<<4)-8
  if (!(F & FC)) { RET_TAKEN(0x4525); return; } // ret nc
  CYC(0x4525, 0x4526);
  CYC(0x4526, 0x4527); C = A;
  CYC(0x4527, 0x4529); A = H8(hCameraX);
  CYC(0x4529, 0x452a); alu_add(gb, C); C = A;
  CYC(0x452a, 0x452b);
  CYC(0x452b, 0x452d); A = H8(hCameraY);
  CYC(0x452d, 0x452e); B = A;
  CYC(0x452e, 0x4530); A = H8(hRng2);
  CYC(0x4530, 0x4532); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x4532, 0x4533); alu_add(gb, B); B = A;
  CYC(0x4533, 0x4534);
  CALL_C(0x4534, checkTileAtPositionIsWater_hook, 0x22b1, 0x4537);
  if (!(F & FC)) { RET_TAKEN(0x4537); return; } // ret nc
  CYC(0x4537, 0x4538);
  CYC(0x4538, 0x4539); C = L;
  CALL_C(0x4539, objectSetShortPosition_hook, 0x20c3, 0x453c);
  CYC(0x453c, 0x453e); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x453e, 0x4540); mem_wr(gb, HL, 0x30); // 48
  CYC(0x4540, 0x4542); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x4542, 0x4543); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4543, 0x4544); alu_xor(gb, A);
  CALL_C(0x4544, enemySetAnimation_hook, 0x282b, 0x4547);
  CYC(0x4547, 0x454a); objectSetVisible83_hook(gb); return; // jp

state_0a:
  CALL_C(0x454a, ecom_decCounter1_b0d_hook, 0x439a, 0x454d);
  if (!(F & FZ)) { CYCT(0x454d, 0x454f); goto animate; } // jr nz
  CYC(0x454d, 0x454f);
  CYC(0x454f, 0x4550); L = E;
  CYC(0x4550, 0x4551); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4551, 0x4553); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4553, 0x4555); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x4555, 0x4557); A = 0x01;
  CYC(0x4557, 0x455a); enemySetAnimation_hook(gb); return; // jp

state_0b:
  CYC(0x455a, 0x455b); H = D;
  CYC(0x455b, 0x455d); L = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x455d, 0x455e); A = mem_rd(gb, HL);
  CYC(0x455e, 0x455f); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x455f, 0x4561); goto disappear; } // jr z
  CYC(0x455f, 0x4561);
  CYC(0x4561, 0x4562); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x4562, 0x4564); goto animate; } // jr z
  CYC(0x4562, 0x4564);
  CYC(0x4564, 0x4566); mem_wr(gb, HL, 0x00);
  CYC(0x4566, 0x4568); B = 0x19; // PART_ZORA_FIRE
  CALL_C(0x4568, ecom_spawnProjectile_b0d_hook, 0x437c, 0x456b);
  if (!(F & FZ)) { CYCT(0x456b, 0x456d); goto animate; } // jr nz
  CYC(0x456b, 0x456d);
  CYC(0x456d, 0x456f); L = PART_BASE + OBJ_SUBID; // Part.subid
  CYC(0x456f, 0x4570); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

animate:
  CYC(0x4570, 0x4573); enemyAnimate_hook(gb); return; // jp

disappear:
  CYC(0x4573, 0x4575); A = 0x08;
  CYC(0x4575, 0x4576); mem_wr(gb, DE, A);
  CYC(0x4576, 0x4578); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x4578, 0x457a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(0x457a, getRandomNumber_noPreserveVars_hook, 0x0453, 0x457d);
  CYC(0x457d, 0x457f); alu_and(gb, 0x1f);
  CYC(0x457f, 0x4581); alu_add(gb, 0x18);
  CYC(0x4581, 0x4583); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x4583, 0x4584); mem_wr(gb, DE, A);
  CYC(0x4584, 0x4586); B = 0x03; // INTERAC_SPLASH
  CALL_C(0x4586, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x4589);
  CYC(0x4589, 0x458c); objectSetInvisible_hook(gb); return; // jp
}
