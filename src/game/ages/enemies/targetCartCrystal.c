#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void enemyCode63_hook(GB *gb);
void targetCartCrystal_state0_hook(GB *gb);
void targetCartCrystal_state1_hook(GB *gb);
void targetCartCrystal_state2_hook(GB *gb);
void targetCartCrystal_loadBehaviour_hook(GB *gb);
void targetCartCrystal_loadPosition_hook(GB *gb);
void targetCartCrystal_initSpeed_hook(GB *gb);
void targetCartCrystal_updateMovement_hook(GB *gb);

static uint16_t targetCartCrystal_jump_table(GB *gb) {
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

static void targetCartCrystal_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void targetCartCrystal_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// ENEMY_TARGET_CART_CRYSTAL
//
// Variables:
//   var03: 0 for no movement, 1 for up/down, 2 for left/right
// ==================================================================================================
void enemyCode63_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x7d79, 0x7d7b); goto normalStatus; } // jr z
  CYC(0x7d79, 0x7d7b);
  CYC(0x7d7b, 0x7d7d); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x7d7d, 0x7d7f); A = 0x02;
  CYC(0x7d7f, 0x7d80); mem_wr(gb, DE, A);

normalStatus:
  CYC(0x7d80, 0x7d82); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x7d82, 0x7d83); A = mem_rd(gb, DE);
  {
    CYC(0x7d83, 0x7d84); push_effect(gb, 0x7d84);
    uint16_t target = targetCartCrystal_jump_table(gb);
    if (target == 0x7d8a) { targetCartCrystal_state0_hook(gb); return; }
    if (target == 0x7d9b) { targetCartCrystal_state1_hook(gb); return; }
    if (target == 0x7db5) { targetCartCrystal_state2_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:7d8a, bare global; jump-table target from enemyCode63. Initialization.
void targetCartCrystal_state0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7d8a, 0x7d8c); A = 0x01;
  CYC(0x7d8c, 0x7d8d); mem_wr(gb, DE, A); // [state]
  CALL_C(0x7d8d, targetCartCrystal_loadPosition_hook, 0x7e24, 0x7d90);
  CALL_C(0x7d90, targetCartCrystal_loadBehaviour_hook, 0x7de2, 0x7d93);
  if (F & FZ) { CYCT(0x7d93, 0x7d95); goto skipInitSpeed; } // jr z
  CALL_C(0x7d95, targetCartCrystal_initSpeed_hook, 0x7e85, 0x7d98);

skipInitSpeed:
  CYC(0x7d98, 0x7d9b); objectSetVisible80_hook(gb); return; // jp
}

// 0e:7d9b, bare global; jump-table target from enemyCode63. Standard update state
// (update movement if it's a moving type).
void targetCartCrystal_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7d9b, 0x7d9d); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x7d9d, 0x7d9e); A = mem_rd(gb, DE);
  CYC(0x7d9e, 0x7d9f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7d9f, 0x7da1); goto checkDeleteInFirstRoom; } // jr z
  CALL_C(0x7da1, targetCartCrystal_updateMovement_hook, 0x7e9f, 0x7da4);

checkDeleteInFirstRoom:
  CYC(0x7da4, 0x7da6); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7da6, 0x7da7); A = mem_rd(gb, DE);
  CYC(0x7da7, 0x7da9); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x7da9, 0x7dab); goto animate; } // jr nc
  CYC(0x7da9, 0x7dab);
  CYC(0x7dab, 0x7dae); A = mem_rd(gb, wTmpcfc0_targetCarts_cfdf);
  CYC(0x7dae, 0x7daf); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7daf, 0x7db2); enemyDelete_hook(gb); return; } // jp nz
  CYC(0x7daf, 0x7db2);

animate:
  CYC(0x7db2, 0x7db5); enemyAnimate_hook(gb); return; // jp
}

// 0e:7db5, bare global; jump-table target from enemyCode63. Target destroyed.
void targetCartCrystal_state2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7db5, 0x7db8); SET_HL(wTmpcfc0_targetCarts_numTargetsHit);
  CYC(0x7db8, 0x7db9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7db9, 0x7dbb); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7dbb, 0x7dbc); A = mem_rd(gb, DE);
  CYC(0x7dbc, 0x7dbe); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(0x7dbe, 0x7dc0); goto playSoundAndSpawn; } // jr nc
  CYC(0x7dbe, 0x7dc0);
  CYC(0x7dc0, 0x7dc3); SET_HL(wTmpcfc0_targetCarts_crystalsHitInFirstRoom);
  CALL_C(0x7dc3, setFlag_hook, 0x020e, 0x7dc6);

playSoundAndSpawn:
  CYC(0x7dc6, 0x7dc8); A = 0x90; // SND_GALE_SEED
  CALL_C(0x7dc8, playSound_b00_hook, 0x0c98, 0x7dcb);
  CYC(0x7dcb, 0x7dcd); A = 0x04;

spawnNext:
  CYC(0x7dcd, 0x7dcf); mem_wr(gb, 0xff8b, A); // hFF8B
  CYC(0x7dcf, 0x7dd2); SET_BC(0x9203); // INTERAC_FALLING_ROCK, $03
  CALL_C(0x7dd2, objectCreateInteraction_hook, 0x24c5, 0x7dd5);
  if (!(F & FZ)) { CYCT(0x7dd5, 0x7dd7); goto delete; } // jr nz
  CYC(0x7dd5, 0x7dd7);
  CYC(0x7dd7, 0x7dd9); L = INTERACTION_BASE + OBJ_ANGLE; // Interaction.angle
  CYC(0x7dd9, 0x7ddb); A = mem_rd(gb, 0xff8b); // hFF8B
  CYC(0x7ddb, 0x7ddc); A = alu_dec8(gb, A);
  CYC(0x7ddc, 0x7ddd); mem_wr(gb, HL, A);
  if (!(F & FZ)) { CYCT(0x7ddd, 0x7ddf); goto spawnNext; } // jr nz
  CYC(0x7ddd, 0x7ddf);

delete:
  CYC(0x7ddf, 0x7de2); enemyDelete_hook(gb); return; // jp
}

// 0e:7de2, bare global; called from targetCartCrystal_state0. Sets var03 to a
// "behaviour" value (0-2).
// @param[out] zflag z iff [var03] == 0
void targetCartCrystal_loadBehaviour_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7de2, 0x7de5); A = mem_rd(gb, wTmpcfc0_targetCarts_targetConfiguration);
  CYC(0x7de5, 0x7de7); A = alu_swap(gb, A);
  CYC(0x7de7, 0x7dea); SET_HL(0x7df4); // @behaviourTable
  CYC(0x7dea, 0x7deb); targetCartCrystal_addAToHl_from_rst(gb, 0x7deb);
  CYC(0x7deb, 0x7ded); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7ded, 0x7dee); A = mem_rd(gb, DE);
  CYC(0x7dee, 0x7def); targetCartCrystal_addAToHl_from_rst(gb, 0x7def);
  CYC(0x7def, 0x7df0); A = mem_rd(gb, HL);
  CYC(0x7df0, 0x7df1); E = alu_inc8(gb, E);
  CYC(0x7df1, 0x7df2); mem_wr(gb, DE, A); // [var03]
  CYC(0x7df2, 0x7df3); alu_or(gb, A);
  RET(0x7df3); return; // ret
}

// 0e:7e24, bare global; called from targetCartCrystal_state0. Sets Y/X position based
// on wTmpcfc0.targetCarts.targetConfiguration and subid.
void targetCartCrystal_loadPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7e24, 0x7e27); A = mem_rd(gb, wTmpcfc0_targetCarts_targetConfiguration);
  CYC(0x7e27, 0x7e2a); SET_HL(0x7e3a); // @configurationTable
  CYC(0x7e2a, 0x7e2b); targetCartCrystal_addAToHl_from_rst(gb, 0x7e2b);
  CYC(0x7e2b, 0x7e2c); A = mem_rd(gb, HL);
  CYC(0x7e2c, 0x7e2d); targetCartCrystal_addAToHl_from_rst(gb, 0x7e2d);
  CYC(0x7e2d, 0x7e2f); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x7e2f, 0x7e30); A = mem_rd(gb, DE);
  CYC(0x7e30, 0x7e31); targetCartCrystal_addDoubleIndexToHl_from_rst(gb, 0x7e31);
  CYC(0x7e31, 0x7e32); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7e32, 0x7e34); E = ENEMY_BASE + OBJ_YH;
  CYC(0x7e34, 0x7e35); mem_wr(gb, DE, A);
  CYC(0x7e35, 0x7e36); A = mem_rd(gb, HL);
  CYC(0x7e36, 0x7e38); E = ENEMY_BASE + OBJ_XH;
  CYC(0x7e38, 0x7e39); mem_wr(gb, DE, A);
  RET(0x7e39); return; // ret
}

// 0e:7e85, bare global; called from targetCartCrystal_state0.
void targetCartCrystal_initSpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7e85, 0x7e86); H = D;
  CYC(0x7e86, 0x7e88); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x7e88, 0x7e8a); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x7e8a, 0x7e8c); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7e8c, 0x7e8e); mem_wr(gb, HL, 0x20);
  CYC(0x7e8e, 0x7e90); L = ENEMY_BASE + OBJ_VAR03;
  CYC(0x7e90, 0x7e91); A = mem_rd(gb, HL);
  CYC(0x7e91, 0x7e93); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x7e93, 0x7e95); goto facingLeft; } // jr z
  CYC(0x7e93, 0x7e95);
  CYC(0x7e95, 0x7e97); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7e97, 0x7e99); mem_wr(gb, HL, 0x00); // ANGLE_UP
  RET(0x7e99); return; // ret

facingLeft:
  CYC(0x7e9a, 0x7e9c); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7e9c, 0x7e9e); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  RET(0x7e9e); return; // ret
}

// 0e:7e9f, bare global; called from targetCartCrystal_state1. Crystal moves for a
// bit, switches directions, moves the other way.
void targetCartCrystal_updateMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7e9f, ecom_decCounter1_b0e_hook, 0x439a, 0x7ea2);
  if (!(F & FZ)) { CYCT(0x7ea2, 0x7ea4); goto applySpeed; } // jr nz
  CYC(0x7ea2, 0x7ea4);
  CYC(0x7ea4, 0x7ea6); mem_wr(gb, HL, 0x40); // [counter1] = 64
  CYC(0x7ea6, 0x7ea8); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7ea8, 0x7ea9); A = mem_rd(gb, HL);
  CYC(0x7ea9, 0x7eab); alu_xor(gb, 0x10);
  CYC(0x7eab, 0x7eac); mem_wr(gb, HL, A);

applySpeed:
  CYC(0x7eac, 0x7eaf); objectApplySpeed_hook(gb); return; // jp
}
