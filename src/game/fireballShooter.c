#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void fireballShooter_state_uninitialized_hook(GB *gb);
void fireballShooter_state1_hook(GB *gb);
void fireballShooter_state_stub_hook(GB *gb);
void fireballShooter_state8_hook(GB *gb);
void fireballShooter_state9_hook(GB *gb);
void fireballShooter_checkAllEnemiesKilled_hook(GB *gb);

static uint16_t fireballShooter_jump_table(GB *gb) {
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

static void fireballShooter_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_FIREBALL_SHOOTER
// ==================================================================================================
void enemyCode50_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x633a, 0x633b); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x633b); return; } // ret z
  CYC(0x633b, 0x633c);
  CYC(0x633c, 0x633d); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(0x633d); return; } // ret z
  CYC(0x633d, 0x633e);
  CYC(0x633e, 0x6340); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6340, 0x6341); A = mem_rd(gb, DE);
  {
    CYC(0x6341, 0x6342); push_effect(gb, 0x6342);
    uint16_t target = fireballShooter_jump_table(gb);
    if (target == 0x6356) { fireballShooter_state_uninitialized_hook(gb); return; }
    if (target == 0x6362) { fireballShooter_state1_hook(gb); return; }
    if (target == 0x63a3) { fireballShooter_state_stub_hook(gb); return; }
    if (target == 0x63a4) { fireballShooter_state8_hook(gb); return; }
    if (target == 0x63b3) { fireballShooter_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6356, bare global; jump-table target from enemyCode50.
void fireballShooter_state_uninitialized_hook(GB *gb) {
  CYC(0x6356, 0x6357); H = D;
  CYC(0x6357, 0x6358); L = E;
  CYC(0x6358, 0x6359); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x6359, 0x635b); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x635b, 0x635c); A = mem_rd(gb, DE);
  CYC(0x635c, 0x635e); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(0x635e); return; } // ret z
  CYC(0x635e, 0x635f);
  CYC(0x635f, 0x6361); mem_wr(gb, HL, 0x08); // [state]
  RET(0x6361); return; // ret
}

// 0e:6362, bare global; jump-table target from enemyCode50. "Spawner"; spawns shooters at each
// appropriate tile index, then deletes self.
void fireballShooter_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6362, 0x6363); alu_xor(gb, A);
  CYC(0x6363, 0x6365); hram_wr(gb, (uint8_t)hFF8D, A);
  CYC(0x6365, 0x6367); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6367, 0x6368); A = mem_rd(gb, DE);
  CYC(0x6368, 0x6369); C = A;
  CYC(0x6369, 0x636c); SET_HL(wRoomLayout);
  CYC(0x636c, 0x636e); B = 0xb0; // LARGE_ROOM_HEIGHT<<4

nextTile:
  CYC(0x636e, 0x636f); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x636f, 0x6370); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(0x6370, 0x6372); goto notThisTile; } // jr nz
  CYC(0x6370, 0x6372);
  PUSH(0x6372, BC);
  PUSH(0x6373, HL);
  CYC(0x6374, 0x6375); C = L;
  CYC(0x6375, 0x6376); C = alu_dec8(gb, C);
  CYC(0x6376, 0x6378); B = 0x50; // ENEMY_FIREBALL_SHOOTER
  CALL_C(0x6378, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, 0x436d, 0x637b);
  if (!(F & FZ)) { CYCT(0x637b, 0x637d); goto delete; } // jr nz
  CYC(0x637b, 0x637d);
  CYC(0x637d, 0x637e); E = L;
  CYC(0x637e, 0x637f); A = mem_rd(gb, DE);
  CYC(0x637f, 0x6381); A = (uint8_t)(A | (1 << 7)); // set 7,a
  CYC(0x6381, 0x6382); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6382, 0x6384); A = hram_rd(gb, (uint8_t)hFF8D);
  CYC(0x6384, 0x6385); A = alu_inc8(gb, A);
  CYC(0x6385, 0x6387); alu_and(gb, 0x03);
  CYC(0x6387, 0x6389); hram_wr(gb, (uint8_t)hFF8D, A);
  CYC(0x6389, 0x638a); mem_wr(gb, HL, A);
  CYC(0x638a, 0x638b); A = C;
  CYC(0x638b, 0x638d); alu_and(gb, 0xf0);
  CYC(0x638d, 0x638f); alu_add(gb, 0x06);
  CYC(0x638f, 0x6391); L = ENEMY_BASE + OBJ_YH;
  CYC(0x6391, 0x6392); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6392, 0x6393); A = C;
  CYC(0x6393, 0x6395); alu_and(gb, 0x0f);
  CYC(0x6395, 0x6397); A = alu_swap(gb, A);
  CYC(0x6397, 0x6399); alu_add(gb, 0x08);
  CYC(0x6399, 0x639a); L = alu_inc8(gb, L);
  CYC(0x639a, 0x639b); mem_wr(gb, HL, A);
  SET_HL(POP(0x639b));
  SET_BC(POP(0x639c));

notThisTile:
  CYC(0x639d, 0x639e); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x639e, 0x63a0); goto nextTile; } // jr nz
  CYC(0x639e, 0x63a0);

delete:
  CYC(0x63a0, 0x63a3); enemyDelete_hook(gb); return; // jp
}

// 0e:63a3, bare global; jump-table target from enemyCode50.
void fireballShooter_state_stub_hook(GB *gb) {
  RET(0x63a3); return; // ret
}

// 0e:63a4, bare global; jump-table target from enemyCode50. Initialization for "actual"
// shooter (not spawner).
void fireballShooter_state8_hook(GB *gb) {
  CYC(0x63a4, 0x63a6); A = 0x09;
  CYC(0x63a6, 0x63a7); mem_wr(gb, DE, A); // [state] = 9
  CYC(0x63a7, 0x63a9); E = ENEMY_BASE + 0x03; // Enemy.var03
  CYC(0x63a9, 0x63aa); A = mem_rd(gb, DE);
  CYC(0x63aa, 0x63ad); SET_HL(0x63d0); // fireballShooter_timingOffsets
  CYC(0x63ad, 0x63ae); fireballShooter_addAToHl_from_rst(gb, 0x63ae);
  CYC(0x63ae, 0x63b0); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x63b0, 0x63b1); A = mem_rd(gb, HL);
  CYC(0x63b1, 0x63b2); mem_wr(gb, DE, A);
  RET(0x63b2); return; // ret
}

// 0e:63b3, bare global; jump-table target from enemyCode50. Main state for actual shooter.
void fireballShooter_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x63b3, fireballShooter_checkAllEnemiesKilled_hook, 0x63d4, 0x63b6);
  CYC(0x63b6, 0x63b8); C = 0x24;
  CALL_C(0x63b8, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x63bb);
  if (F & FC) { RET_TAKEN(0x63bb); return; } // ret c
  CYC(0x63bb, 0x63bc);
  CALL_C(0x63bc, ecom_decCounter1_b0e_hook, 0x439a, 0x63bf);
  if (!(F & FZ)) { RET_TAKEN(0x63bf); return; } // ret nz
  CYC(0x63bf, 0x63c0);
  CYC(0x63c0, 0x63c2); B = 0x31; // PART_GOPONGA_PROJECTILE
  CALL_C(0x63c2, ecom_spawnProjectile_b0e_hook, 0x437c, 0x63c5);
  CALL_C(0x63c5, getRandomNumber_noPreserveVars_hook, 0x0453, 0x63c8);
  CYC(0x63c8, 0x63ca); alu_and(gb, 0x07);
  CYC(0x63ca, 0x63cc); alu_add(gb, 0xc0);
  CYC(0x63cc, 0x63ce); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x63ce, 0x63cf); mem_wr(gb, DE, A);
  RET(0x63cf); return; // ret
}

// 0e:63d4, bare global; called from fireballShooter_state9. For subid $81 only, this deletes
// itself when all enemies are killed.
void fireballShooter_checkAllEnemiesKilled_hook(GB *gb) {
  CYC(0x63d4, 0x63d6); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x63d6, 0x63d7); A = mem_rd(gb, DE);
  CYC(0x63d7, 0x63d9); alu_cp(gb, 0x81);
  if (!(F & FZ)) { RET_TAKEN(0x63d9); return; } // ret nz
  CYC(0x63d9, 0x63da);
  CYC(0x63da, 0x63dd); A = mem_rd(gb, wNumEnemies);
  CYC(0x63dd, 0x63de); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x63de); return; } // ret nz
  CYC(0x63de, 0x63df);
  CYC(0x63df, 0x63e2); enemyDelete_hook(gb); return; // jp
}
