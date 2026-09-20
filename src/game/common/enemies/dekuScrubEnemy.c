#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

// NOTE: dekuScrub_targetAngles (0d:60db) is pure ROM data (a 32-byte lookup table used only
// via rst_addAToHl below), not code, despite lacking a "Table" suffix in its name -- verified
// with --report, which decodes it as garbage/illegal instructions:
//   60db  nop / nop / nop / nop / nop / nop / nop
//   60e2  ld ($0808),sp
//   60e5  inc c / inc c / inc c / inc c / inc c
//   60ea  stop  !! unsupported
// It is used below purely as a table base address (SET_HL(0x60db)); no _hook is written for it.

void dekuScrub_state_uninitialized_hook(GB *gb);
void dekuScrub_state_stub_hook(GB *gb);
void dekuScrub_state8_hook(GB *gb);
void dekuScrub_state9_hook(GB *gb);
void dekuScrub_stateA_hook(GB *gb);
void dekuScrub_animate_hook(GB *gb);
void dekuScrub_stateB_hook(GB *gb);
void dekuScrub_stateC_hook(GB *gb);
void dekuScrub_stateD_hook(GB *gb);
void dekuScrub_hideInBush_hook(GB *gb);
void dekuScrub_spawnBush_hook(GB *gb);

static uint16_t dekuScrub_jump_table(GB *gb) {
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

static void dekuScrub_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_DEKU_SCRUB
//
// Variables:
//   var03: Read by ENEMY_BUSH_OR_ROCK to control Z-offset
//   var30: Starts at 2, gets decremented each time one of the scrub's bullets hits itself.
//   var31: Index of ENEMY_BUSH_OR_ROCK
//   var32: "pressedAButton" variable (nonzero when player presses A)
//   var33: Former var03 value (low byte of text index, TX_45XX)
// ==================================================================================================
void enemyCode27_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5fc7, 0x5fc9); goto normalStatus; } // jr z
  CYC(0x5fc7, 0x5fc9);
  CYC(0x5fc9, 0x5fcb); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5fcb); return; } // ret c
  CYC(0x5fcb, 0x5fcc);
  if (F & FZ) { CYCT(0x5fcc, 0x5fce); goto dead; } // jr z
  CYC(0x5fcc, 0x5fce);
  CYC(0x5fce, 0x5fcf); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5fcf, 0x5fd1); goto normalStatus; } // jr nz
  CYC(0x5fcf, 0x5fd1);

  // ENEMYSTATUS_JUST_HIT
  CYC(0x5fd1, 0x5fd3); E = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x5fd3, 0x5fd4); A = mem_rd(gb, DE);
  CYC(0x5fd4, 0x5fd5); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5fd5); return; } // ret nz
  CYC(0x5fd5, 0x5fd6);
  CYC(0x5fd6, 0x5fd7); H = D;
  CYC(0x5fd7, 0x5fd9); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5fd9, 0x5fdb); mem_wr(gb, HL, 0x0c);
  CYC(0x5fdb, 0x5fdd); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x5fdd, 0x5fde); H = mem_rd(gb, HL);
  CYC(0x5fde, 0x5fe1); ecom_killObjectH_b0d_hook(gb); return; // jp

dead:
  CYC(0x5fe1, 0x5fe3); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5fe3, 0x5fe4); A = mem_rd(gb, DE);
  CYC(0x5fe4, 0x5fe5); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x5fe5, 0x5fe8); enemyDie_hook(gb); return; } // jp nz
  CYC(0x5fe5, 0x5fe8);

normalStatus:
  CYC(0x5fe8, 0x5fea); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5fea, 0x5feb); A = mem_rd(gb, DE);
  {
    CYC(0x5feb, 0x5fec); push_effect(gb, 0x5fec);
    uint16_t target = dekuScrub_jump_table(gb);
    if (target == 0x6008) { dekuScrub_state_uninitialized_hook(gb); return; }
    if (target == 0x6026) { dekuScrub_state_stub_hook(gb); return; }
    if (target == 0x6027) { dekuScrub_state8_hook(gb); return; }
    if (target == 0x6041) { dekuScrub_state9_hook(gb); return; }
    if (target == 0x6072) { dekuScrub_stateA_hook(gb); return; }
    if (target == 0x608c) { dekuScrub_stateB_hook(gb); return; }
    if (target == 0x609e) { dekuScrub_stateC_hook(gb); return; }
    if (target == 0x60af) { dekuScrub_stateD_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0d:6008, bare global; jump-table target from enemyCode27.
void dekuScrub_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6008, dekuScrub_spawnBush_hook, 0x6100, 0x600b);
  if (!(F & FZ)) { RET_TAKEN(0x600b); return; } // ret nz
  CYC(0x600b, 0x600c);
  CALL_C(0x600c, objectMakeTileSolid_hook, 0x20b2, 0x600f);
  CYC(0x600f, 0x6011); H = 0xcf; // >wRoomLayout
  CYC(0x6011, 0x6013); mem_wr(gb, HL, 0x00);
  // The value of 'a' here depends on objectMakeTileSolid; it should be 0 if the enemy
  // spawned on an empty space. This enemy doesn't move, so it shouldn't matter either way.
  CALL_C(0x6013, ecom_setSpeedAndState8_b0d_hook, 0x4364, 0x6016);
  CYC(0x6016, 0x6018); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6018, 0x6019); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6019, 0x601b); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x601b, 0x601d); mem_wr(gb, HL, 0x02);
  CYC(0x601d, 0x601f); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x601f, 0x6020); A = mem_rd(gb, HL);
  CYC(0x6020, 0x6022); mem_wr(gb, HL, 0x00);
  CYC(0x6022, 0x6024); L = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(0x6024, 0x6025); mem_wr(gb, HL, A);
  RET(0x6025); return; // ret
}

// 0d:6026, bare global; jump-table target from enemyCode27.
void dekuScrub_state_stub_hook(GB *gb) {
  RET(0x6026); return; // ret
}

// 0d:6027, bare global; jump-table target from enemyCode27. Waiting for Link to be a
// certain distance away.
void dekuScrub_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6027, 0x6029); C = 0x2c;
  CALL_C(0x6029, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x602c);
  if (F & FC) { RET_TAKEN(0x602c); return; } // ret c
  CYC(0x602c, 0x602d);
  CALL_C(0x602d, ecom_decCounter1_b0d_hook, 0x439a, 0x6030);
  if (!(F & FZ)) { RET_TAKEN(0x6030); return; } // ret nz
  CYC(0x6030, 0x6031);
  CYC(0x6031, 0x6033); mem_wr(gb, HL, 90);
  CYC(0x6033, 0x6035); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6035, 0x6036); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6036, 0x6038); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x6038, 0x603a); mem_wr(gb, HL, 0x02);
  CYC(0x603a, 0x603b); alu_xor(gb, A);
  CALL_C(0x603b, enemySetAnimation_hook, 0x282b, 0x603e);
  CYC(0x603e, 0x6041); objectSetVisiblec3_hook(gb); return; // jp
}

// 0d:6041, bare global; jump-table target from enemyCode27. Link is at a good distance,
// wait a bit longer before emerging from bush.
void dekuScrub_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6041, 0x6043); C = 0x2c;
  CALL_C(0x6043, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x6046);
  if (F & FC) { CYCT(0x6046, 0x6049); dekuScrub_hideInBush_hook(gb); return; } // jp c
  CYC(0x6046, 0x6049);
  CALL_C(0x6049, ecom_decCounter1_b0d_hook, 0x439a, 0x604c);
  if (!(F & FZ)) { CYCT(0x604c, 0x604e); dekuScrub_animate_hook(gb); return; } // jr nz
  CYC(0x604c, 0x604e);

  // Emerge from under the bush
  CYC(0x604e, 0x6050); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6050, 0x6051); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6051, 0x6053); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x6053, 0x6055); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x6055, 0x6057); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x6057, 0x6058); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

  // Calculate angle to shoot
  CALL_C(0x6058, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x605b);
  CYC(0x605b, 0x605e); SET_HL(0x60db); // dekuScrub_targetAngles (data)
  CYC(0x605e, 0x605f); dekuScrub_addAToHl_from_rst(gb, 0x605f);
  CYC(0x605f, 0x6060); A = mem_rd(gb, HL);
  CYC(0x6060, 0x6061); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6061, 0x6063); dekuScrub_hideInBush_hook(gb); return; } // jr z
  CYC(0x6061, 0x6063);
  CYC(0x6063, 0x6065); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x6065, 0x6066); mem_wr(gb, DE, A);
  CYC(0x6066, 0x6067); alu_rrca(gb);
  CYC(0x6067, 0x6068); alu_rrca(gb);
  CYC(0x6068, 0x606a); alu_sub(gb, 0x02);
  CYC(0x606a, 0x606d); SET_HL(0x60fb); // dekuScrub_fireAnimations (data)
  CYC(0x606d, 0x606e); dekuScrub_addAToHl_from_rst(gb, 0x606e);
  CYC(0x606e, 0x606f); A = mem_rd(gb, HL);
  CYC(0x606f, 0x6072); enemySetAnimation_hook(gb); return; // jp
}

// 0d:6072, bare global; jump-table target from enemyCode27. Firing sequence; falls through
// into dekuScrub_animate.
void dekuScrub_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6072, 0x6074); C = 0x2c;
  CALL_C(0x6074, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x6077);
  if (F & FC) { CYCT(0x6077, 0x6079); dekuScrub_hideInBush_hook(gb); return; } // jr c
  CYC(0x6077, 0x6079);
  CYC(0x6079, 0x607b); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x607b, 0x607c); A = mem_rd(gb, DE);
  CYC(0x607c, 0x607d); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x607d, 0x607f); dekuScrub_hideInBush_hook(gb); return; } // jr z
  CYC(0x607d, 0x607f);
  CYC(0x607f, 0x6080); A = mem_rd(gb, DE);
  CYC(0x6080, 0x6081); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6081, 0x6083); dekuScrub_animate_hook(gb); return; } // jr nz
  CYC(0x6081, 0x6083);
  CYC(0x6083, 0x6084); mem_wr(gb, DE, A);
  CYC(0x6084, 0x6086); B = 0x1e; // PART_DEKU_SCRUB_PROJECTILE
  CALL_C(0x6086, ecom_spawnProjectile_b0d_hook, 0x437c, 0x6089);
  dekuScrub_animate_hook(gb); return; // fallthrough
}

// 0d:6089, bare global; called from dekuScrub_state9/stateA/stateB/stateD.
void dekuScrub_animate_hook(GB *gb) {
  CYC(0x6089, 0x608c); enemyAnimate_hook(gb); return; // jp
}

// 0d:608c, bare global; jump-table target from enemyCode27. Go hide in the bush again.
void dekuScrub_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x608c, 0x608e); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x608e, 0x608f); A = mem_rd(gb, DE);
  CYC(0x608f, 0x6090); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x6090, 0x6092); dekuScrub_animate_hook(gb); return; } // jr nz
  CYC(0x6090, 0x6092);
  CYC(0x6092, 0x6093); H = D;
  CYC(0x6093, 0x6095); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6095, 0x6097); mem_wr(gb, HL, 0x08);
  CYC(0x6097, 0x6099); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x6099, 0x609b); mem_wr(gb, HL, 0x00);
  CYC(0x609b, 0x609e); objectSetInvisible_hook(gb); return; // jp
}

// 0d:609e, bare global; jump-table target from enemyCode27. He's just been defeated; falls
// through into dekuScrub_stateD.
void dekuScrub_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x609e, 0x609f); H = D;
  CYC(0x609f, 0x60a0); L = E;
  CYC(0x60a0, 0x60a1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = $0d
  CYC(0x60a1, 0x60a3); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x60a3, 0x60a5); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x60a5, 0x60a7); E = ENEMY_BASE + 0x32; // Enemy.var32
  CALL_C(0x60a7, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x60aa);
  CYC(0x60aa, 0x60ac); A = 0x07;
  CALL_C(0x60ac, enemySetAnimation_hook, 0x282b, 0x60af);
  dekuScrub_stateD_hook(gb); return; // fallthrough
}

// 0d:60af, bare global; jump-table target from enemyCode27, also falls into from
// dekuScrub_stateC. Waiting for Link to talk to him.
void dekuScrub_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x60af, objectSetPriorityRelativeToLink_withTerrainEffects_hook, 0x22e0, 0x60b2);
  CYC(0x60b2, 0x60b4); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x60b4, 0x60b5); A = mem_rd(gb, DE);
  CYC(0x60b5, 0x60b6); alu_or(gb, A);
  if (F & FZ) { CYCT(0x60b6, 0x60b8); dekuScrub_animate_hook(gb); return; } // jr z
  CYC(0x60b6, 0x60b8);

  // Pressed A in front of deku scrub
  CYC(0x60b8, 0x60ba); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x60ba, 0x60bb); alu_xor(gb, A);
  CYC(0x60bb, 0x60bc); mem_wr(gb, DE, A);

  // Show text
  CYC(0x60bc, 0x60be); E = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(0x60be, 0x60bf); A = mem_rd(gb, DE);
  CYC(0x60bf, 0x60c0); C = A;
  CYC(0x60c0, 0x60c2); B = 0x45; // >TX_4500
  CYC(0x60c2, 0x60c5); showText_hook(gb); return; // jp
}

// 0d:60c5, bare global; called from dekuScrub_state9 and dekuScrub_stateA.
void dekuScrub_hideInBush_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x60c5, 0x60c6); H = D;
  CYC(0x60c6, 0x60c8); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x60c8, 0x60ca); mem_wr(gb, HL, 0x0b);
  CYC(0x60ca, 0x60cc); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x60cc, 0x60ce); mem_wr(gb, HL, 120);
  CYC(0x60ce, 0x60d0); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x60d0, 0x60d2); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x60d2, 0x60d4); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x60d4, 0x60d6); mem_wr(gb, HL, 0x02);
  CYC(0x60d6, 0x60d8); A = 0x06;
  CYC(0x60d8, 0x60db); enemySetAnimation_hook(gb); return; // jp
}

// 0d:6100, bare global; called from dekuScrub_state_uninitialized.
// @param[out] zflag z if spawned bush successfully
void dekuScrub_spawnBush_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6100, 0x6102); B = 0x58; // ENEMY_BUSH_OR_ROCK
  CALL_C(0x6102, ecom_spawnUncountedEnemyWithSubid01_b0d_hook, 0x436d, 0x6105);
  if (!(F & FZ)) { RET_TAKEN(0x6105); return; } // ret nz
  CYC(0x6105, 0x6106);
  CALL_C(0x6106, objectCopyPosition_hook, 0x2242, 0x6109);

  // [child.relatedObj1] = this
  CYC(0x6109, 0x610b); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x610b, 0x610d); A = ENEMY_BASE; // Enemy.start
  CYC(0x610d, 0x610e); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi
  CYC(0x610e, 0x610f); mem_wr(gb, HL, D);

  // Save projectile's index to var31
  CYC(0x610f, 0x6111); E = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6111, 0x6112); A = H;
  CYC(0x6112, 0x6113); mem_wr(gb, DE, A);

  CYC(0x6113, 0x6115); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6115, 0x6116); E = L;
  CYC(0x6116, 0x6117); A = mem_rd(gb, DE);
  CYC(0x6117, 0x6118); mem_wr(gb, HL, A);
  CYC(0x6118, 0x6119); alu_xor(gb, A);
  RET(0x6119); return; // ret
}
