#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode50);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+1); return; } // ret z
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); A = alu_dec8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+3); return; } // ret z
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  {
    CYC(b_+7, b_+8); push_effect(gb, b_+8);
    uint16_t target = fireballShooter_jump_table(gb);
    if (target == SYM(fireballShooter_state_uninitialized)) { fireballShooter_state_uninitialized_hook(gb); return; }
    if (target == SYM(fireballShooter_state1)) { fireballShooter_state1_hook(gb); return; }
    if (target == SYM(fireballShooter_state_stub)) { fireballShooter_state_stub_hook(gb); return; }
    if (target == SYM(fireballShooter_state8)) { fireballShooter_state8_hook(gb); return; }
    if (target == SYM(fireballShooter_state9)) { fireballShooter_state9_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6356, bare global; jump-table target from enemyCode50.
void fireballShooter_state_uninitialized_hook(GB *gb) {
  BASE(fireballShooter_state_uninitialized);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+8); return; } // ret z
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x08); // [state]
  RET(b_+11); return; // ret
}

// 0e:6362, bare global; jump-table target from enemyCode50. "Spawner"; spawns shooters at each
// appropriate tile index, then deletes self.
void fireballShooter_state1_hook(GB *gb) {
  BASE(fireballShooter_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); hram_wr(gb, (uint8_t)hFF8D, A);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+10); SET_HL(wRoomLayout);
  CYC(b_+10, b_+12); B = 0xb0; // LARGE_ROOM_HEIGHT<<4

nextTile:
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+13, b_+14); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+14, b_+16); goto notThisTile; } // jr nz
  CYC(b_+14, b_+16);
  PUSH(b_+16, BC);
  PUSH(b_+17, HL);
  CYC(b_+18, b_+19); C = L;
  CYC(b_+19, b_+20); C = alu_dec8(gb, C);
  CYC(b_+20, b_+22); B = 0x50; // ENEMY_FIREBALL_SHOOTER
  CALL_C(b_+22, ecom_spawnUncountedEnemyWithSubid01_b0e_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b0e), b_+25);
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto delete; } // jr nz
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); E = L;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+31); A = (uint8_t)(A | (1 << 7)); // set 7,a
  CYC(b_+31, b_+32); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+32, b_+34); A = hram_rd(gb, (uint8_t)hFF8D);
  CYC(b_+34, b_+35); A = alu_inc8(gb, A);
  CYC(b_+35, b_+37); alu_and(gb, 0x03);
  CYC(b_+37, b_+39); hram_wr(gb, (uint8_t)hFF8D, A);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+41); A = C;
  CYC(b_+41, b_+43); alu_and(gb, 0xf0);
  CYC(b_+43, b_+45); alu_add(gb, 0x06);
  CYC(b_+45, b_+47); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+47, b_+48); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+48, b_+49); A = C;
  CYC(b_+49, b_+51); alu_and(gb, 0x0f);
  CYC(b_+51, b_+53); A = alu_swap(gb, A);
  CYC(b_+53, b_+55); alu_add(gb, 0x08);
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+57); mem_wr(gb, HL, A);
  SET_HL(POP(b_+57));
  SET_BC(POP(b_+58));

notThisTile:
  CYC(b_+59, b_+60); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto nextTile; } // jr nz
  CYC(b_+60, b_+62);

delete:
  CYC(b_+62, b_+65); enemyDelete_hook(gb); return; // jp
}

// 0e:63a3, bare global; jump-table target from enemyCode50.
void fireballShooter_state_stub_hook(GB *gb) {
  BASE(fireballShooter_state_stub);
  RET(b_+0); return; // ret
}

// 0e:63a4, bare global; jump-table target from enemyCode50. Initialization for "actual"
// shooter (not spawner).
void fireballShooter_state8_hook(GB *gb) {
  BASE(fireballShooter_state8);
  CYC(b_+0, b_+2); A = 0x09;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A); // [state] = 9
  CYC(b_+3, b_+5); E = ENEMY_BASE + 0x03; // Enemy.var03
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+9); SET_HL(SYM(fireballShooter_timingOffsets)); // fireballShooter_timingOffsets
  CYC(b_+9, b_+10); fireballShooter_addAToHl_from_rst(gb, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14); return; // ret
}

// 0e:63b3, bare global; jump-table target from enemyCode50. Main state for actual shooter.
void fireballShooter_state9_hook(GB *gb) {
  BASE(fireballShooter_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, fireballShooter_checkAllEnemiesKilled_hook, SYM(fireballShooter_checkAllEnemiesKilled), b_+3);
  CYC(b_+3, b_+5); C = 0x24;
  CALL_C(b_+5, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+8);
  if (F & FC) { RET_TAKEN(b_+8); return; } // ret c
  CYC(b_+8, b_+9);
  CALL_C(b_+9, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); B = 0x31; // PART_GOPONGA_PROJECTILE
  CALL_C(b_+15, ecom_spawnProjectile_b0e_hook, SYM(ecom_spawnProjectile_b0e), b_+18);
  CALL_C(b_+18, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+21);
  CYC(b_+21, b_+23); alu_and(gb, 0x07);
  CYC(b_+23, b_+25); alu_add(gb, 0xc0);
  CYC(b_+25, b_+27); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  RET(b_+28); return; // ret
}

// 0e:63d4, bare global; called from fireballShooter_state9. For subid $81 only, this deletes
// itself when all enemies are killed.
void fireballShooter_checkAllEnemiesKilled_hook(GB *gb) {
  BASE(fireballShooter_checkAllEnemiesKilled);
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x81);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } // ret nz
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wNumEnemies);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); enemyDelete_hook(gb); return; // jp
}
