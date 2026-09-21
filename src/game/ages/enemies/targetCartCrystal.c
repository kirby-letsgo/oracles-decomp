#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode63);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+4, b_+6); A = 0x02;
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);

normalStatus:
  CYC(b_+7, b_+9); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  {
    CYC(b_+10, b_+11); push_effect(gb, b_+11);
    uint16_t target = targetCartCrystal_jump_table(gb);
    if (target == SYM(targetCartCrystal_state0)) { targetCartCrystal_state0_hook(gb); return; }
    if (target == SYM(targetCartCrystal_state1)) { targetCartCrystal_state1_hook(gb); return; }
    if (target == SYM(targetCartCrystal_state2)) { targetCartCrystal_state2_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:7d8a, bare global; jump-table target from enemyCode63. Initialization.
void targetCartCrystal_state0_hook(GB *gb) {
  BASE(targetCartCrystal_state0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A); // [state]
  CALL_C(b_+3, targetCartCrystal_loadPosition_hook, SYM(targetCartCrystal_loadPosition), b_+6);
  CALL_C(b_+6, targetCartCrystal_loadBehaviour_hook, SYM(targetCartCrystal_loadBehaviour), b_+9);
  if (F & FZ) { CYCT(b_+9, b_+11); goto skipInitSpeed; } // jr z
  CALL_C(b_+11, targetCartCrystal_initSpeed_hook, SYM(targetCartCrystal_initSpeed), b_+14);

skipInitSpeed:
  CYC(b_+14, b_+17); TAIL(objectSetVisible80); // jp
}

// 0e:7d9b, bare global; jump-table target from enemyCode63. Standard update state
// (update movement if it's a moving type).
void targetCartCrystal_state1_hook(GB *gb) {
  BASE(targetCartCrystal_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto checkDeleteInFirstRoom; } // jr z
  CALL_C(b_+6, targetCartCrystal_updateMovement_hook, SYM(targetCartCrystal_updateMovement), b_+9);

checkDeleteInFirstRoom:
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+14, b_+16); goto animate; } // jr nc
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); A = mem_rd(gb, wTmpcfc0_targetCarts_cfdf);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+20, b_+23); enemyDelete_hook(gb); return; } // jp nz
  CYC(b_+20, b_+23);

animate:
  CYC(b_+23, b_+26); TAIL(enemyAnimate); // jp
}

// 0e:7db5, bare global; jump-table target from enemyCode63. Target destroyed.
void targetCartCrystal_state2_hook(GB *gb) {
  BASE(targetCartCrystal_state2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wTmpcfc0_targetCarts_numTargetsHit);
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+6); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_cp(gb, 0x05);
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto playSoundAndSpawn; } // jr nc
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+14); SET_HL(wTmpcfc0_targetCarts_crystalsHitInFirstRoom);
  CALL_C(b_+14, setFlag_hook, SYM(setFlag), b_+17);

playSoundAndSpawn:
  CYC(b_+17, b_+19); A = 0x90; // SND_GALE_SEED
  CALL_C(b_+19, playSound_b00_hook, SYM(playSound_b00), b_+22);
  CYC(b_+22, b_+24); A = 0x04;

spawnNext:
  CYC(b_+24, b_+26); mem_wr(gb, hFF8B, A); // hFF8B
  CYC(b_+26, b_+29); SET_BC(0x9203); // INTERAC_FALLING_ROCK, $03
  CALL_C(b_+29, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+32);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto delete; } // jr nz
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_ANGLE; // Interaction.angle
  CYC(b_+36, b_+38); A = mem_rd(gb, hFF8B); // hFF8B
  CYC(b_+38, b_+39); A = alu_dec8(gb, A);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  if (!(F & FZ)) { CYCT(b_+40, b_+42); goto spawnNext; } // jr nz
  CYC(b_+40, b_+42);

delete:
  CYC(b_+42, b_+45); TAIL(enemyDelete); // jp
}

// 0e:7de2, bare global; called from targetCartCrystal_state0. Sets var03 to a
// "behaviour" value (0-2).
// @param[out] zflag z iff [var03] == 0
void targetCartCrystal_loadBehaviour_hook(GB *gb) {
  BASE(targetCartCrystal_loadBehaviour);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_targetCarts_targetConfiguration);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+8); SET_HL(b_+18); // @behaviourTable
  CYC(b_+8, b_+9); targetCartCrystal_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); targetCartCrystal_addAToHl_from_rst(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); E = alu_inc8(gb, E);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A); // [var03]
  CYC(b_+16, b_+17); alu_or(gb, A);
  RET(b_+17); return; // ret
}

// 0e:7e24, bare global; called from targetCartCrystal_state0. Sets Y/X position based
// on wTmpcfc0.targetCarts.targetConfiguration and subid.
void targetCartCrystal_loadPosition_hook(GB *gb) {
  BASE(targetCartCrystal_loadPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_targetCarts_targetConfiguration);
  CYC(b_+3, b_+6); SET_HL(b_+22); // @configurationTable
  CYC(b_+6, b_+7); targetCartCrystal_addAToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); targetCartCrystal_addAToHl_from_rst(gb, b_+9);
  CYC(b_+9, b_+11); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); targetCartCrystal_addDoubleIndexToHl_from_rst(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);
  CYC(b_+18, b_+20); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  RET(b_+21); return; // ret
}

// 0e:7e85, bare global; called from targetCartCrystal_state0.
void targetCartCrystal_initSpeed_hook(GB *gb) {
  BASE(targetCartCrystal_initSpeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+5, b_+7); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x20);
  CYC(b_+9, b_+11); L = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+14); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+14, b_+16); goto facingLeft; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x00); // ANGLE_UP
  RET(b_+20); return; // ret

facingLeft:
  CYC(b_+21, b_+23); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  RET(b_+25); return; // ret
}

// 0e:7e9f, bare global; called from targetCartCrystal_state1. Crystal moves for a
// bit, switches directions, moves the other way.
void targetCartCrystal_updateMovement_hook(GB *gb) {
  BASE(targetCartCrystal_updateMovement);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto applySpeed; } // jr nz
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x40); // [counter1] = 64
  CYC(b_+7, b_+9); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_xor(gb, 0x10);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);

applySpeed:
  CYC(b_+13, b_+16); TAIL(objectApplySpeed); // jp
}
