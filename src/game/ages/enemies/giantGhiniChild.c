#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void giantGhiniChild_state_stub_hook(GB *gb);
void giantGhiniChild_state_uninitialized_hook(GB *gb);
void giantGhiniChild_state8_hook(GB *gb);
void giantGhiniChild_state9_hook(GB *gb);
void giantGhiniChild_gotoStateA_hook(GB *gb);
void giantGhiniChild_stateA_hook(GB *gb);
void giantGhiniChild_stateB_hook(GB *gb);
void giantGhiniChild_stateC_hook(GB *gb);

static uint16_t giantGhiniChild_jump_table(GB *gb) {
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

static void giantGhiniChild_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_GIANT_GHINI_CHILD
// ==================================================================================================
void enemyCode3f_hook(GB *gb) {
  BASE(enemyCode3f);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto dead; } // jr z
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); ecom_updateKnockbackNoSolidity_b0e_hook(gb); return; } // jp nz
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto normalStatus; } // jr nz
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x0b);
  CYC(b_+23, b_+25); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 120);
  CYC(b_+27, b_+29); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+29, b_+31); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+31, b_+33); L = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x00);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+37, b_+38); H = mem_rd(gb, HL);
  CYC(b_+38, b_+40); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x01);
  CYC(b_+42, b_+44); goto normalStatus; // jr

dead:
  CYC(b_+44, b_+46); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); H = A;
  CYC(b_+48, b_+50); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+51, b_+54); enemyDie_hook(gb); return; // jp

normalStatus:
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+58); H = A;
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+62); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+62, b_+64); goto dead; } // jr z
  CYC(b_+62, b_+64);
  CYC(b_+64, b_+66); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  {
    CYC(b_+67, b_+68); push_effect(gb, b_+68);
    uint16_t target = giantGhiniChild_jump_table(gb);
    if (target == SYM(giantGhiniChild_state_uninitialized)) { giantGhiniChild_state_uninitialized_hook(gb); return; }
    if (target == SYM(giantGhiniChild_state_stub)) { giantGhiniChild_state_stub_hook(gb); return; }
    if (target == SYM(giantGhiniChild_state8)) { giantGhiniChild_state8_hook(gb); return; }
    if (target == SYM(giantGhiniChild_state9)) { giantGhiniChild_state9_hook(gb); return; }
    if (target == SYM(giantGhiniChild_stateA)) { giantGhiniChild_stateA_hook(gb); return; }
    if (target == SYM(giantGhiniChild_stateB)) { giantGhiniChild_stateB_hook(gb); return; }
    if (target == SYM(giantGhiniChild_stateC)) { giantGhiniChild_stateC_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:6dad, bare global; jump-table target from enemyCode3f.
void giantGhiniChild_state_stub_hook(GB *gb) {
  BASE(giantGhiniChild_state_stub);
  RET(b_+0); return; // ret
}

// 0e:6dae, bare global; jump-table target from enemyCode3f.
void giantGhiniChild_state_uninitialized_hook(GB *gb) {
  BASE(giantGhiniChild_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x7f);
  CYC(b_+5, b_+6); A = alu_dec8(gb, A);
  CYC(b_+6, b_+9); SET_HL(SYM(giantGhiniChild_spawnOffsets)); // giantGhiniChild_spawnOffsets
  CYC(b_+9, b_+10); giantGhiniChild_addDoubleIndexToHl_from_rst(gb, b_+10);
  CYC(b_+10, b_+12); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); SET_HL(HL + 1); // inc hl
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+23); A = 0x1e; // SPEED_c0
  CALL_C(b_+23, ecom_setSpeedAndState8_b0e_hook, SYM(ecom_setSpeedAndState8_b0e), b_+26);
  CYC(b_+26, b_+28); L = ENEMY_BASE + OBJ_Z + 1; // Enemy.zh
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0xfc);
  CYC(b_+30, b_+32); L = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+34); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+34); return; } // ret c
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+37); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x09);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 30);
  CALL_C(b_+43, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+46);
  CYC(b_+46, b_+49); objectCreatePuff_hook(gb); return; // jp
}

// 0e:6ddf, bare global; jump-table target from enemyCode3f. Waiting for battle to start.
void giantGhiniChild_state8_hook(GB *gb) {
  BASE(giantGhiniChild_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_cp(gb, 0x09);
  if (F & FC) { CYCT(b_+9, b_+11); goto battleNotStartedYet; } // jr c
  CYC(b_+9, b_+11);
  CALL_C(b_+11, giantGhiniChild_gotoStateA_hook, SYM(giantGhiniChild_gotoStateA), b_+14);
  CYC(b_+14, b_+17); objectSetVisiblec1_hook(gb); return; // jp

battleNotStartedYet:
  CYC(b_+17, b_+19); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+19, b_+20); E = L;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_or(gb, 0x40);
  CYC(b_+23, b_+24); mem_wr(gb, DE, A);
  RET(b_+24); return; // ret
}

// 0e:6df8, bare global; jump-table target from enemyCode3f. Just spawned in, will charge
// after [counter1] frames. Falls through into giantGhiniChild_gotoStateA.
void giantGhiniChild_state9_hook(GB *gb) {
  BASE(giantGhiniChild_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  giantGhiniChild_gotoStateA_hook(gb); return; // fallthrough
}

// 0e:6dfc, bare global; called from giantGhiniChild_state8, also falls into from
// giantGhiniChild_state9.
void giantGhiniChild_gotoStateA_hook(GB *gb) {
  BASE(giantGhiniChild_gotoStateA);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); A = 0x0a;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+7, b_+9); A = 0x05;
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CALL_C(b_+10, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  RET(b_+16); return; // ret
}

// 0e:6e0d, bare global; jump-table target from enemyCode3f. Charging at Link.
void giantGhiniChild_stateA_hook(GB *gb) {
  BASE(giantGhiniChild_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CALL_C(b_+3, objectApplySpeed_hook, SYM(objectApplySpeed), b_+6);
  CALL_C(b_+6, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; } // ret nz
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x05); // [counter1]
  CALL_C(b_+12, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+15);
  CYC(b_+15, b_+18); objectNudgeAngleTowards_hook(gb); return; // jp
}

// 0e:6e1f, bare global; jump-table target from enemyCode3f. Attached to Link.
void giantGhiniChild_stateB_hook(GB *gb) {
  BASE(giantGhiniChild_stateB);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, w1Link + OBJ_YH);
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+12); A = mem_rd(gb, w1Link + OBJ_XH);
  CYC(b_+12, b_+14); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+18);
  if (F & FZ) { CYCT(b_+18, b_+20); goto detach; } // jr z
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+23); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+24, b_+26); goto adjustVisibility; } // jr z
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_sub(gb, 0x03); // BTN_A|BTN_B
  if (F & FC) { CYCT(b_+29, b_+31); goto storeCounter1; } // jr nc
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); A = 0x01;

storeCounter1:
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);

adjustVisibility:
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+37); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+37, b_+39); goto slowLink; } // jr nz
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  CYC(b_+42, b_+44); alu_xor(gb, 0x80);
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);

slowLink:
  CYC(b_+45, b_+48); SET_HL(wccd8);
  CYC(b_+48, b_+50); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  CYC(b_+50, b_+53); A = mem_rd(gb, wFrameCounter);
  CYC(b_+53, b_+54); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+54); return; } // ret nc
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+58); SET_HL(wLinkImmobilized);
  CYC(b_+58, b_+60); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)
  RET(b_+60); return; // ret

detach:
  CYC(b_+61, b_+63); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x0c);
  CYC(b_+65, b_+67); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x3c); // 60
  CYC(b_+69, b_+71); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+71, b_+72); H = mem_rd(gb, HL);
  CYC(b_+72, b_+74); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x00);
  RET(b_+76); return; // ret
}

// 0e:6e6c, bare global; jump-table target from enemyCode3f. Just detached from Link, fading
// away.
void giantGhiniChild_stateC_hook(GB *gb) {
  BASE(giantGhiniChild_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, enemyAnimate_hook, SYM(enemyAnimate), b_+3);
  CYC(b_+3, b_+5); E = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_xor(gb, 0x80);
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CALL_C(b_+9, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } // ret nz
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); H = A;
  CYC(b_+17, b_+19); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+20, decNumEnemies_hook, SYM(decNumEnemies), b_+23);
  CYC(b_+23, b_+26); enemyDelete_hook(gb); return; // jp
}
