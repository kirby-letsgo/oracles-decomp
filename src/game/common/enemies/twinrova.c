#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/common/enemies/twinrova.s (ENEMY_TWINROVA), bank $10.

static void twinrova_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void twinrova_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static uint16_t twinrova_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void twinrova_state_uninitialized_hook(GB *gb);
void twinrova_state_spawner_hook(GB *gb);
void twinrova_state_stub_hook(GB *gb);
void twinrova_subid0_hook(GB *gb);
void twinrova_subid1_hook(GB *gb);
void twinrova_state8_hook(GB *gb);
void twinrova_state9_hook(GB *gb);
void twinrova_subid0_stateA_hook(GB *gb);
void twinrova_subid0_stateB_hook(GB *gb);
void twinrova_subid0_stateC_hook(GB *gb);
void twinrova_stateD_hook(GB *gb);
void twinrova_stateE_hook(GB *gb);
void twinrova_animate_hook(GB *gb);
void twinrova_stateF_hook(GB *gb);
void twinrova_state10_hook(GB *gb);
void twinrova_subid1_stateA_hook(GB *gb);
void twinrova_subid1_stateB_hook(GB *gb);
void twinrova_subid1_stateC_hook(GB *gb);
void twinrova_initialize_hook(GB *gb);
void twinrova_updateZPosition_hook(GB *gb);
void twinrova_checkFireProjectile_hook(GB *gb);
void twinrova_checkFireProjectile__spawnProjectile_hook(GB *gb);
void twinrova_setTargetPositionToObject_hook(GB *gb);
void twinrova_updateAnimationFromAngle_hook(GB *gb);
void twinrova_updateMovingAnimation_hook(GB *gb);
void twinrova_updateMovingAnimationGivenAngle_hook(GB *gb);
void twinrova_calculateAnimationFromAngle_hook(GB *gb);
void twinrova_subid0_updateTargetPosition_hook(GB *gb);
void twinrova_subid1_updateTargetPosition_hook(GB *gb);
void twinrova_moveTowardTargetPosition_hook(GB *gb);
void twinrova_chooseObjectToAttack_hook(GB *gb);
void twinrova_checkAttackInProgress_hook(GB *gb);
void twinrova_rise2PixelsAboveGround_hook(GB *gb);
void twinrova_incState2ForSelfAndTwin_hook(GB *gb);

// 10:4a94 (real call from enemyCode03, tail-jumps into whichever state/subid handler
// is selected; that handler's own eventual ret pops back through this C call chain to
// enemyCode03's continuation after the call, so this stays a plain non-hooked function
// invoked via explicit push_effect + direct call, not CALL_C).
void enemyCode03__runState_hook(GB *gb) {
  BASE(enemyCode03);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+61, twinrova_checkFireProjectile_hook, SYM(twinrova_checkFireProjectile), b_+64);
  CALL_C(b_+64, ecom_getSubidAndCpStateTo08_b10_hook, SYM(ecom_getSubidAndCpStateTo08_b10), b_+67);
  if (!(F & FC)) { CYCT(b_+67, b_+69); goto state8OrHigher; } CYC(b_+67, b_+69);  // jr nc,@state8OrHigher
  CYC(b_+69, b_+70); push_effect(gb, b_+70);
  do { uint16_t jt_ = (twinrova_jump_table(gb));
    if (jt_ == SYM(twinrova_state_uninitialized) && hook_is(gb, SYM(twinrova_state_uninitialized), twinrova_state_uninitialized_hook)) { twinrova_state_uninitialized_hook(gb); return; }
    else if (jt_ == SYM(twinrova_state_spawner) && hook_is(gb, SYM(twinrova_state_spawner), twinrova_state_spawner_hook)) { twinrova_state_spawner_hook(gb); return; }
    else if (jt_ == SYM(twinrova_state_stub) && hook_is(gb, SYM(twinrova_state_stub), twinrova_state_stub_hook)) { twinrova_state_stub_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state8OrHigher:
  CYC(b_+86, b_+87); A = B;  // ld a,b
  CYC(b_+87, b_+88); push_effect(gb, b_+88);
  do { uint16_t jt_ = (twinrova_jump_table(gb));
    if (jt_ == SYM(twinrova_subid0) && hook_is(gb, SYM(twinrova_subid0), twinrova_subid0_hook)) { twinrova_subid0_hook(gb); return; }
    else if (jt_ == SYM(twinrova_subid1) && hook_is(gb, SYM(twinrova_subid1), twinrova_subid1_hook)) { twinrova_subid1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void enemyCode03_hook(GB *gb) {
  BASE(enemyCode03);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if ((F & FZ)) { CYCT(b_+0, b_+2); goto normalStatus; } CYC(b_+0, b_+2);  // jr z,@normalStatus
  CYC(b_+2, b_+4); alu_sub(gb, 0x03);  // sub ENEMYSTATUS_NO_HEALTH
  if ((F & FC)) { RET_TAKEN(b_+4); return; } CYC(b_+4, b_+5);  // ret c
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto normalStatus; } CYC(b_+5, b_+7);  // jr nz,@normalStatus
  CYC(b_+7, b_+8); H = D;  // ld h,d
  CYC(b_+8, b_+10); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+10, b_+12); alu_bit(gb, 6, mem_rd(gb, HL));  // bit 6,(hl)
  if ((F & FZ)) { CYCT(b_+12, b_+14); goto normalStatus; } CYC(b_+12, b_+14);  // jr z,@normalStatus
  CYC(b_+14, b_+16); L = 0xa9;  // ld l,Enemy.health
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x7f);  // ld (hl),$7f
  CYC(b_+18, b_+20); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+20, b_+22); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(b_+22, b_+24); L = 0x84;  // ld l,Enemy.state
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x0d);  // ld (hl),$0d
  CYC(b_+26, b_+28); A = 0x29;  // ld a,Object.health
  CALL_C(b_+28, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+31);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x7f);  // ld (hl),$7f
  CYC(b_+33, b_+35); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+35, b_+37); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(b_+37, b_+39); L = 0x84;  // ld l,Enemy.state
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x0f);  // ld (hl),$0f
  CYC(b_+41, b_+43); A = 0xf0;  // ld a,SNDCTRL_STOPMUSIC
  CALL_C(b_+43, playSound_b00_hook, SYM(playSound_b00), b_+46);
normalStatus:
  CALL_C(b_+46, twinrova_updateZPosition_hook, SYM(twinrova_updateZPosition), b_+49);
  CYC(b_+49, b_+52); push_effect(gb, b_+52); enemyCode03__runState_hook(gb);  // call @runState
  CYC(b_+52, b_+54); E = 0xb2;  // ld e,Enemy.var32
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+55, b_+57); alu_bit(gb, 7, A);  // bit 7,a
  if (!(F & FZ)) { CYCT(b_+57, b_+60); if (hook_is(gb, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), objectSetPriorityRelativeToLink_withTerrainEffects_hook)) { objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb); return; } HANDOFF(SYM(objectSetPriorityRelativeToLink_withTerrainEffects)); } CYC(b_+57, b_+60);  // jp nz,$22e0
  RET(b_+60); return;  // ret
}

void twinrova_state_uninitialized_hook(GB *gb) {
  BASE(twinrova_state_uninitialized);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x03;  // ld a,ENEMY_TWINROVA
  CYC(b_+2, b_+5); mem_wr(gb, wEnemyIDToLoadExtraGfx, A);  // ld (wEnemyIDToLoadExtraGfx),a
  CYC(b_+5, b_+7); A = 0x01;  // ld a,$01
  CYC(b_+7, b_+10); mem_wr(gb, wLoadedObjectGfxEnd, A);  // ld (wLoadedTreeGfxIndex),a
  CYC(b_+10, b_+11); H = D;  // ld h,d
  CYC(b_+11, b_+13); L = 0x83;  // ld l,Enemy.var03
  CYC(b_+13, b_+15); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  CYC(b_+15, b_+17); A = 0x28;  // ld a,SPEED_100
  if (!(F & FZ)) { CYCT(b_+17, b_+20); twinrova_initialize_hook(gb); return; } CYC(b_+17, b_+20);  // jp nz,twinrova_initialize
  CYC(b_+20, b_+21); L = E;  // ld l,e
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+22, b_+23); alu_xor(gb, A);  // xor a
  CYC(b_+23, b_+26); mem_wr(gb, w1Link_direction, A);  // ld (w1Link.direction),a
  CYC(b_+26, b_+27); A = alu_inc8(gb, A);  // inc a
  CYC(b_+27, b_+30); mem_wr(gb, wDisabledObjects, A);  // ld (wDisabledObjects),a
  CYC(b_+30, b_+33); mem_wr(gb, wMenuDisabled, A);  // ld (wMenuDisabled),a
  TAIL(twinrova_state_spawner);  // fallthrough
}

void twinrova_state_spawner_hook(GB *gb) {
  BASE(twinrova_state_spawner);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x03;  // ld b,ENEMY_TWINROVA
  CALL_C(b_+2, ecom_spawnUncountedEnemyWithSubid01_b10_hook, SYM(ecom_spawnUncountedEnemyWithSubid01_b10), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; } CYC(b_+5, b_+6);  // ret nz
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);  // inc l
  CYC(b_+7, b_+8); E = L;  // ld e,l
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);  // inc a
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+11, b_+13); L = 0x96;  // ld l,Enemy.relatedObj1
  CYC(b_+13, b_+14); E = L;  // ld e,l
  CYC(b_+14, b_+16); A = 0x80;  // ld a,Enemy.start
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ld (hl+),a
  CYC(b_+18, b_+19); E = alu_inc8(gb, E);  // inc e
  CYC(b_+19, b_+20); A = H;  // ld a,h
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+21, b_+22); mem_wr(gb, HL, D);  // ld (hl),d
  CYC(b_+22, b_+23); A = H;  // ld a,h
  CYC(b_+23, b_+24); alu_cp(gb, D);  // cp d
  CYC(b_+24, b_+26); A = 0x28;  // ld a,SPEED_100
  if (!(F & FC)) { CYCT(b_+26, b_+29); twinrova_initialize_hook(gb); return; } CYC(b_+26, b_+29);  // jp nc,twinrova_initialize
  CYC(b_+29, b_+31); L = 0x80;  // ld l,Enemy.enabled
  CYC(b_+31, b_+32); E = L;  // ld e,l
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+34, b_+36); L = 0x82;  // ld l,Enemy.subid
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(b_+37, b_+38); H = D;  // ld h,d
  CYC(b_+38, b_+39); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+39, b_+40); L = alu_inc8(gb, L);  // inc l
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+41, b_+43); L = 0x84;  // ld l,Enemy.state
  CYC(b_+43, b_+44); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  RET(b_+44); return;  // ret
}

void twinrova_state_stub_hook(GB *gb) {
  BASE(twinrova_state_stub);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;  // ret
}

void twinrova_subid0_hook(GB *gb) {
  BASE(twinrova_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);  // sub $08
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (twinrova_jump_table(gb));
    if (jt_ == SYM(twinrova_state8) && hook_is(gb, SYM(twinrova_state8), twinrova_state8_hook)) { twinrova_state8_hook(gb); return; }
    else if (jt_ == SYM(twinrova_state9) && hook_is(gb, SYM(twinrova_state9), twinrova_state9_hook)) { twinrova_state9_hook(gb); return; }
    else if (jt_ == SYM(twinrova_subid0_stateA) && hook_is(gb, SYM(twinrova_subid0_stateA), twinrova_subid0_stateA_hook)) { twinrova_subid0_stateA_hook(gb); return; }
    else if (jt_ == SYM(twinrova_subid0_stateB) && hook_is(gb, SYM(twinrova_subid0_stateB), twinrova_subid0_stateB_hook)) { twinrova_subid0_stateB_hook(gb); return; }
    else if (jt_ == SYM(twinrova_subid0_stateC) && hook_is(gb, SYM(twinrova_subid0_stateC), twinrova_subid0_stateC_hook)) { twinrova_subid0_stateC_hook(gb); return; }
    else if (jt_ == SYM(twinrova_stateD) && hook_is(gb, SYM(twinrova_stateD), twinrova_stateD_hook)) { twinrova_stateD_hook(gb); return; }
    else if (jt_ == SYM(twinrova_stateE) && hook_is(gb, SYM(twinrova_stateE), twinrova_stateE_hook)) { twinrova_stateE_hook(gb); return; }
    else if (jt_ == SYM(twinrova_stateF) && hook_is(gb, SYM(twinrova_stateF), twinrova_stateF_hook)) { twinrova_stateF_hook(gb); return; }
    else if (jt_ == SYM(twinrova_state10) && hook_is(gb, SYM(twinrova_state10), twinrova_state10_hook)) { twinrova_state10_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void twinrova_state8_hook(GB *gb) {
  BASE(twinrova_state8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+2); L = E;  // ld l,e
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+3, b_+5); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+5, b_+7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3)));  // set 3,(hl)
  CYC(b_+7, b_+9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+9, b_+11); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+11, b_+13); mem_wr(gb, HL, 106);  // ld (hl),106
  CYC(b_+13, b_+15); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+17, b_+19); L = 0x82;  // ld l,Enemy.subid
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+20, b_+21); alu_add(gb, A);  // add a
  CYC(b_+21, b_+24); SET_HL(b_+62);  // ld hl,@data
  CYC(b_+24, b_+25);
  twinrova_addDoubleIndexToHl_from_rst(gb, b_+25);  // rst_addDoubleIndexToHl
  CYC(b_+25, b_+27); E = 0x8d;  // ld e,Enemy.xh
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+29, b_+31); E = 0x9b;  // ld e,Enemy.oamFlagsBackup
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+33, b_+34); E = alu_inc8(gb, E);  // inc e
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+35, b_+37); E = 0x89;  // ld e,Enemy.angle
  CYC(b_+37, b_+38); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+38, b_+39); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+39, b_+41); E = 0xb0;  // ld e,Enemy.var30
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+43, b_+45); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+46, b_+47); alu_or(gb, A);  // or a
  CYC(b_+47, b_+50); SET_BC(0x2f04);  // ld bc,TX_2f04
  if ((F & FZ)) { CALL_C_CC(b_+50, showText_hook, SYM(showText), b_+53); } else CYC(b_+50, b_+53);  // call z,showText
  CALL_C(b_+53, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+56);
  CYC(b_+56, b_+58); E = 0xb1;  // ld e,Enemy.var31
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+59, b_+62); TAIL(twinrova_updateAnimationFromAngle);  // jp twinrova_updateAnimationFromAngle
}

void twinrova_state9_hook(GB *gb) {
  BASE(twinrova_state9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (twinrova_jump_table(gb));
    if (jt_ == b_+13) { goto substate0; }
    else if (jt_ == b_+27) { goto substate1; }
    else if (jt_ == b_+71) { goto substate2; }
    else if (jt_ == b_+89) { goto substate3; }
    else if (jt_ == b_+113) { goto substate4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CALL_C(b_+13, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto applySpeed; } CYC(b_+16, b_+18);  // jr nz,@applySpeed
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);  // inc l
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x12);  // ld (hl),$12
  CYC(b_+23, b_+24); L = E;  // ld l,e
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+25, b_+27); goto animate;  // jr @animate
substate1:
  CALL_C(b_+27, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+30);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto applySpeed; } CYC(b_+30, b_+32);  // jr nz,@applySpeed
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);  // inc l
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto updateAngle; } CYC(b_+36, b_+38);  // jr nz,@updateAngle
  CYC(b_+38, b_+39); L = E;  // ld l,e
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);  // inc l
  CYC(b_+41, b_+43); mem_wr(gb, HL, 30);  // ld (hl),30
  CALL_C(b_+43, ecom_updateAngleTowardTarget_b10_hook, SYM(ecom_updateAngleTowardTarget_b10), b_+46);
  CALL_C(b_+46, twinrova_calculateAnimationFromAngle_hook, SYM(twinrova_calculateAnimationFromAngle), b_+49);
  CYC(b_+49, b_+50); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+50, b_+53); if (hook_is(gb, SYM(enemySetAnimation), enemySetAnimation_hook)) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
updateAngle:
  CYC(b_+53, b_+55); E = 0x89;  // ld e,Enemy.angle
  CYC(b_+55, b_+57); L = 0xb0;  // ld l,Enemy.var30
  CYC(b_+57, b_+58); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+58, b_+59); alu_add(gb, mem_rd(gb, HL));  // add (hl)
  CYC(b_+59, b_+61); alu_and(gb, 0x1f);  // and $1f
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(b_+62, twinrova_updateAnimationFromAngle_hook, SYM(twinrova_updateAnimationFromAngle), b_+65);
applySpeed:
  CALL_C(b_+65, objectApplySpeed_hook, SYM(objectApplySpeed), b_+68);
animate:
  CYC(b_+68, b_+71); if (hook_is(gb, SYM(enemyAnimate), enemyAnimate_hook)) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate));  // jp enemyAnimate
substate2:
  CALL_C(b_+71, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+74);
  if (!(F & FZ)) { CYCT(b_+74, b_+76); goto animate; } CYC(b_+74, b_+76);  // jr nz,@animate
  CYC(b_+76, b_+77); L = E;  // ld l,e
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+78, b_+80); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+80, b_+81); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+81, b_+82); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+82); return; } CYC(b_+82, b_+83);  // ret nz
  CYC(b_+83, b_+86); SET_BC(0x2f05);  // ld bc,TX_2f05
  CYC(b_+86, b_+89); if (hook_is(gb, SYM(showText), showText_hook)) { showText_hook(gb); return; } HANDOFF(SYM(showText));  // jp showText
substate3:
  CYC(b_+89, b_+92); SET_BC(0x0502);  // ldbc INTERAC_PUFF,$02
  CALL_C(b_+92, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+95);
  if (!(F & FZ)) { RET_TAKEN(b_+95); return; } CYC(b_+95, b_+96);  // ret nz
  CYC(b_+96, b_+97); A = H;  // ld a,h
  CYC(b_+97, b_+98); H = D;  // ld h,d
  CYC(b_+98, b_+100); L = 0x99;  // ld l,Enemy.relatedObj2+1
  CYC(b_+100, b_+101); mem_wr(gb, HL, A); SET_HL(HL - 1);  // ldd (hl),a
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x40);  // ld (hl),Interaction.start
  CYC(b_+103, b_+105); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+106, b_+108); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+108, b_+110); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(b_+110, b_+113); if (hook_is(gb, SYM(objectSetInvisible), objectSetInvisible_hook)) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));  // jp objectSetInvisible
substate4:
  CYC(b_+113, b_+115); A = 0x21;  // ld a,Object.animParameter
  CALL_C(b_+115, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+118);
  CYC(b_+118, b_+120); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if ((F & FZ)) { RET_TAKEN(b_+120); return; } CYC(b_+120, b_+121);  // ret z
  CYC(b_+121, b_+122); H = D;  // ld h,d
  CYC(b_+122, b_+124); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+124, b_+126); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(b_+126, b_+127); alu_xor(gb, A);  // xor a
  CYC(b_+127, b_+130); mem_wr(gb, wDisabledObjects, A);  // ld (wDisabledObjects),a
  CYC(b_+130, b_+133); mem_wr(gb, wMenuDisabled, A);  // ld (wMenuDisabled),a
  CYC(b_+133, b_+135); A = 0x18;  // ld a,CUTSCENE_FLAMES_FLICKERING
  CYC(b_+135, b_+138); mem_wr(gb, wCutsceneTrigger, A);  // ld (wCutsceneTrigger),a
  CALL_C(b_+138, ecom_updateAngleTowardTarget_b10_hook, SYM(ecom_updateAngleTowardTarget_b10), b_+141);
  CALL_C(b_+141, twinrova_calculateAnimationFromAngle_hook, SYM(twinrova_calculateAnimationFromAngle), b_+144);
  CYC(b_+144, b_+146); alu_add(gb, 0x04);  // add $04
  CYC(b_+146, b_+147); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+147, b_+150); if (hook_is(gb, SYM(enemySetAnimation), enemySetAnimation_hook)) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void twinrova_subid0_stateA_hook(GB *gb) {
  BASE(twinrova_subid0_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+2); L = E;  // ld l,e
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+3, b_+5); A = 0x33;  // ld a,MUS_TWINROVA
  CYC(b_+5, b_+8); mem_wr(gb, wActiveMusic, A);  // ld (wActiveMusic),a
  CALL_C(b_+8, playSound_b00_hook, SYM(playSound_b00), b_+11);
  CYC(b_+11, b_+14); TAIL(twinrova_subid0_updateTargetPosition);  // jp twinrova_subid0_updateTargetPosition
}

void twinrova_subid0_stateB_hook(GB *gb) {
  BASE(twinrova_subid0_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);  // ld a,(wFrameCounter)
  CYC(b_+3, b_+5); alu_and(gb, 0x7f);  // and $7f
  CYC(b_+5, b_+7); A = 0x91;  // ld a,SND_FAIRYCUTSCENE
  if ((F & FZ)) { CALL_C_CC(b_+7, playSound_b00_hook, SYM(playSound_b00), b_+10); } else CYC(b_+7, b_+10);  // call z,playSound
  CALL_C(b_+10, twinrova_moveTowardTargetPosition_hook, SYM(twinrova_moveTowardTargetPosition), b_+13);
  if (!(F & FC)) { RET_TAKEN(b_+13); return; } CYC(b_+13, b_+14);  // ret nc
  CALL_C(b_+14, twinrova_subid0_updateTargetPosition_hook, SYM(twinrova_subid0_updateTargetPosition), b_+17);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto waypointChanged; } CYC(b_+17, b_+19);  // jr nz,@waypointChanged
  CALL_C(b_+19, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+22);
  CYC(b_+22, b_+24); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+24, b_+26); mem_wr(gb, HL, 30);  // ld (hl),30
  RET(b_+26); return;  // ret
waypointChanged:
  CALL_C(b_+27, twinrova_checkAttackInProgress_hook, SYM(twinrova_checkAttackInProgress), b_+30);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; } CYC(b_+30, b_+31);  // ret nz
  CYC(b_+31, b_+33); E = 0xb8;  // ld e,Enemy.var38
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+34, b_+35); A = alu_inc8(gb, A);  // inc a
  CYC(b_+35, b_+37); alu_and(gb, 0x07);  // and $07
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+38, b_+41); SET_HL(b_+48);  // ld hl,@attackPattern
  CALL_C(b_+41, checkFlag_hook, SYM(checkFlag), b_+44);
  if (!(F & FZ)) { CYCT(b_+44, b_+47); twinrova_chooseObjectToAttack_hook(gb); return; } CYC(b_+44, b_+47);  // jp nz,twinrova_chooseObjectToAttack
  RET(b_+47); return;  // ret
}

void twinrova_subid0_stateC_hook(GB *gb) {
  BASE(twinrova_subid0_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); if (hook_is(gb, SYM(enemyAnimate), enemyAnimate_hook)) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+3, b_+6);  // jp nz,enemyAnimate
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CALL_C(b_+8, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+11);
  CYC(b_+11, b_+13); alu_and(gb, 0x03);  // and $03
  CYC(b_+13, b_+15); E = 0xb3;  // ld e,Enemy.var33
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);  // inc e
  CYC(b_+17, b_+18); alu_xor(gb, A);  // xor a
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+19, b_+22); TAIL(twinrova_subid0_updateTargetPosition);  // jp twinrova_subid0_updateTargetPosition
}

void twinrova_stateD_hook(GB *gb) {
  BASE(twinrova_stateD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+2); L = E;  // ld l,e
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+3, b_+5); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+7, b_+9); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+9, b_+11); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 3)));  // res 3,(hl)
  CYC(b_+11, b_+13); L = 0x89;  // ld l,Enemy.angle
  CYC(b_+13, b_+15); alu_bit(gb, 4, mem_rd(gb, HL));  // bit 4,(hl)
  CYC(b_+15, b_+17); A = 0x0a;  // ld a,$0a
  if ((F & FZ)) { CYCT(b_+17, b_+20); goto setAnim; } CYC(b_+17, b_+19);  // jr z,+
  CYC(b_+19, b_+20); A = alu_inc8(gb, A);  // inc a
setAnim:
  CYC(b_+20, b_+23); if (hook_is(gb, SYM(enemySetAnimation), enemySetAnimation_hook)) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void twinrova_stateE_hook(GB *gb) {
  BASE(twinrova_stateE);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { CYCT(b_+4, b_+6); twinrova_animate_hook(gb); return; } CYC(b_+4, b_+6);  // jr nz,twinrova_animate
  CYC(b_+6, b_+8); E = 0x88;  // ld e,Enemy.direction
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+9, b_+11); alu_add(gb, 0x04);  // add $04
  CALL_C(b_+11, enemySetAnimation_hook, SYM(enemySetAnimation), b_+14);
  CALL_C(b_+14, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+17);
  CYC(b_+17, b_+19); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+19, b_+20); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(b_+20, b_+23); SET_BC(0x2f09);  // ld bc,TX_2f09
  CALL_C(b_+23, showText_hook, SYM(showText), SYM(twinrova_animate));
  TAIL(twinrova_animate);  // fallthrough
}

void twinrova_animate_hook(GB *gb) {
  BASE(twinrova_animate);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); if (hook_is(gb, SYM(enemyAnimate), enemyAnimate_hook)) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate));  // jp enemyAnimate
}

void twinrova_stateF_hook(GB *gb) {
  BASE(twinrova_stateF);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, twinrova_rise2PixelsAboveGround_hook, SYM(twinrova_rise2PixelsAboveGround), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); twinrova_animate_hook(gb); return; } CYC(b_+3, b_+5);  // jr nz,twinrova_animate
  CYC(b_+5, b_+7); A = 0x32;  // ld a,Object.var32
  CALL_C(b_+7, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+10);
  CYC(b_+10, b_+12); alu_bit(gb, 4, mem_rd(gb, HL));  // bit 4,(hl)
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto nextState; } CYC(b_+12, b_+14);  // jr nz,@nextState
  CALL_C(b_+14, ecom_updateAngleTowardTarget_b10_hook, SYM(ecom_updateAngleTowardTarget_b10), b_+17);
  CALL_C(b_+17, twinrova_updateMovingAnimation_hook, SYM(twinrova_updateMovingAnimation), b_+20);
  CYC(b_+20, b_+22); TAIL(twinrova_animate);  // jr twinrova_animate
nextState:
  CALL_C(b_+22, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+25);
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);  // inc l
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+28, b_+30); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+30, b_+32); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 3)));  // res 3,(hl)
  CYC(b_+32, b_+34); TAIL(twinrova_animate);  // jr twinrova_animate
}

void twinrova_state10_hook(GB *gb) {
  BASE(twinrova_state10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);  // inc e
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (twinrova_jump_table(gb));
    if (jt_ == b_+19) { goto substate0; }
    else if (jt_ == b_+49) { goto substate1; }
    else if (jt_ == b_+95) { goto substate2; }
    else if (jt_ == b_+114) { goto substate3; }
    else if (jt_ == b_+150) { goto substate4; }
    else if (jt_ == b_+196) { goto substate5; }
    else if (jt_ == b_+239) { goto substate6; }
    else if (jt_ == b_+258) { goto substate7; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
substate0:
  CYC(b_+19, b_+20); H = D;  // ld h,d
  CYC(b_+20, b_+21); L = E;  // ld l,e
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+22, b_+24); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+24, b_+26); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));  // res 1,(hl)
  CYC(b_+26, b_+28); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));  // res 0,(hl)
  CYC(b_+28, b_+30); L = 0x88;  // ld l,Enemy.direction
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(b_+32, b_+35); SET_BC(0xfc20);  // ld bc,-$3e0
  CALL_C(b_+35, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+38);
  CYC(b_+38, b_+40); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+41, b_+42); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+42); return; } CYC(b_+42, b_+43);  // ret nz
  CYC(b_+43, b_+46); SET_BC(0x2f0a);  // ld bc,TX_2f0a
  CYC(b_+46, b_+49); if (hook_is(gb, SYM(showText), showText_hook)) { showText_hook(gb); return; } HANDOFF(SYM(showText));  // jp showText
substate1:
  CYC(b_+49, b_+51); C = 0x08;  // ld c,$08
  CALL_C(b_+51, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+54);
  CYC(b_+54, b_+56); A = mem_rd(gb, hCameraY);  // ldh a,(<hCameraY)
  CYC(b_+56, b_+57); B = A;  // ld b,a
  CYC(b_+57, b_+59); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+59, b_+60); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+60, b_+61); alu_sub(gb, B);  // sub b
  if (!(F & FC)) { CYCT(b_+61, b_+63); goto substate1_afterSub; } CYC(b_+61, b_+63);  // jr nc,+
  CYC(b_+63, b_+64); A = mem_rd(gb, HL);  // ld a,(hl)
substate1_afterSub:
  CYC(b_+64, b_+65); B = A;  // ld b,a
  CYC(b_+65, b_+67); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+68, b_+70); alu_cp(gb, 0x80);  // cp $80
  if ((F & FC)) { CYCT(b_+70, b_+72); goto substate1_setState2; } CYC(b_+70, b_+72);  // jr c,++
  CYC(b_+72, b_+73); alu_add(gb, B);  // add b
  CYC(b_+73, b_+75); alu_cp(gb, 0xf0);  // cp $f0
  if ((F & FC)) { CYCT(b_+75, b_+77); goto state10_animate; } CYC(b_+75, b_+77);  // jr c,@animate
substate1_setState2:
  CYC(b_+77, b_+79); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+79, b_+80); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+80, b_+82); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+82, b_+84); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));  // set 1,(hl)
  CYC(b_+84, b_+86); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+86, b_+88); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(b_+88, b_+90); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+90, b_+92); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+92, b_+95); if (hook_is(gb, SYM(objectSetInvisible), objectSetInvisible_hook)) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));  // jp objectSetInvisible
substate2:
  CYC(b_+95, b_+97); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+98, b_+99); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+99); return; } CYC(b_+99, b_+100);  // ret nz
  CYC(b_+100, b_+102); A = 0x32;  // ld a,Object.var32
  CALL_C(b_+102, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+105);
  CYC(b_+105, b_+107); alu_bit(gb, 1, mem_rd(gb, HL));  // bit 1,(hl)
  if ((F & FZ)) { RET_TAKEN(b_+107); return; } CYC(b_+107, b_+108);  // ret z
  CYC(b_+108, b_+110); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+110, b_+111); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+111, b_+112); H = D;  // ld h,d
  CYC(b_+112, b_+113); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  RET(b_+113); return;  // ret
substate3:
  CALL_C(b_+114, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+117);
  if (!(F & FZ)) { RET_TAKEN(b_+117); return; } CYC(b_+117, b_+118);  // ret nz
  CYC(b_+118, b_+120); mem_wr(gb, HL, 48);  // ld (hl),48
  CYC(b_+120, b_+121); L = E;  // ld l,e
  CYC(b_+121, b_+122); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+122, b_+124); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+124, b_+126); mem_wr(gb, HL, 0xa0);  // ld (hl),$a0
  CYC(b_+126, b_+128); L = 0x89;  // ld l,Enemy.angle
  CYC(b_+128, b_+130); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+131, b_+132); alu_or(gb, A);  // or a
  CYC(b_+132, b_+134); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  if ((F & FZ)) { CYCT(b_+134, b_+136); goto substate3_visible; } CYC(b_+134, b_+136);  // jr z,+
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x18);  // ld (hl),$18
substate3_visible:
  CYC(b_+138, b_+140); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+140, b_+142); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(b_+142, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+145);
  CYC(b_+145, b_+147); A = 0xd3;  // ld a,SND_WIND
  CALL_C(b_+147, playSound_b00_hook, SYM(playSound_b00), b_+150);
substate4:
  CYC(b_+150, b_+153); SET_BC(0x5878);  // ld bc,$5878
  CYC(b_+153, b_+155); E = 0x86;  // ld e,Enemy.counter1
  CYC(b_+155, b_+156); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+156, b_+158); E = 0x89;  // ld e,Enemy.angle
  CALL_C(b_+158, objectSetPositionInCircleArc_hook, SYM(objectSetPositionInCircleArc), b_+161);
  CYC(b_+161, b_+163); E = 0x89;  // ld e,Enemy.angle
  CYC(b_+163, b_+164); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+164, b_+166); alu_add(gb, 0x08);  // add $08
  CYC(b_+166, b_+168); alu_and(gb, 0x1f);  // and $1f
  CALL_C(b_+168, twinrova_updateMovingAnimationGivenAngle_hook, SYM(twinrova_updateMovingAnimationGivenAngle), b_+171);
  CYC(b_+171, b_+172); H = D;  // ld h,d
  CYC(b_+172, b_+174); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+174, b_+175); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+175, b_+176); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+176, b_+177); alu_rrca(gb);  // rrca
  if ((F & FC)) { CYCT(b_+177, b_+179); goto state10_animate; } CYC(b_+177, b_+179);  // jr c,@animate
  CYC(b_+179, b_+181); L = 0x89;  // ld l,Enemy.angle
  CYC(b_+181, b_+182); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+182, b_+183); A = alu_inc8(gb, A);  // inc a
  CYC(b_+183, b_+185); alu_and(gb, 0x1f);  // and $1f
  CYC(b_+185, b_+186); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+186, b_+188); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+188, b_+189); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if (!(F & FZ)) { CYCT(b_+189, b_+191); goto state10_animate; } CYC(b_+189, b_+191);  // jr nz,@animate
  CYC(b_+191, b_+192); L = alu_dec8(gb, L);  // dec l
  CYC(b_+192, b_+193); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
state10_animate:
  CYC(b_+193, b_+196); if (hook_is(gb, SYM(enemyAnimate), enemyAnimate_hook)) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate));  // jp enemyAnimate
substate5:
  CYC(b_+196, b_+198); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+198, b_+199); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+199, b_+200); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(b_+200, b_+203); if (hook_is(gb, SYM(enemyDelete), enemyDelete_hook)) { enemyDelete_hook(gb); return; } HANDOFF(SYM(enemyDelete)); } CYC(b_+200, b_+203);  // jp nz,enemyDelete
  CYC(b_+203, b_+206); A = mem_rd(gb, wLinkDeathTrigger);  // ld a,(wLinkDeathTrigger)
  CYC(b_+206, b_+207); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+207); return; } CYC(b_+207, b_+208);  // ret nz
  CYC(b_+208, b_+209); A = alu_inc8(gb, A);  // inc a
  CYC(b_+209, b_+212); mem_wr(gb, wDisabledObjects, A);  // ld (wDisabledObjects),a
  CYC(b_+212, b_+215); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(b_+215, b_+216); H = D;  // ld h,d
  CYC(b_+216, b_+218); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+218, b_+219); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+219, b_+221); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+221, b_+222); alu_xor(gb, A);  // xor a
  CYC(b_+222, b_+223); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+223, b_+224); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+224, b_+226); A = 0x0c;  // ld a,$0c
  CALL_C(b_+226, enemySetAnimation_hook, SYM(enemySetAnimation), b_+229);
  CYC(b_+229, b_+231); A = 0xc0;  // ld a,SND_TRANSFORM
  CALL_C(b_+231, playSound_b00_hook, SYM(playSound_b00), b_+234);
  CYC(b_+234, b_+236); A = 0x02;  // ld a,$02
  CYC(b_+236, b_+239); if (hook_is(gb, SYM(fadeinFromWhiteWithDelay), fadeinFromWhiteWithDelay_hook)) { fadeinFromWhiteWithDelay_hook(gb); return; } HANDOFF(SYM(fadeinFromWhiteWithDelay));  // jp fadeinFromWhiteWithDelay
substate6:
  CYC(b_+239, b_+242); A = mem_rd(gb, wPaletteThread_mode);  // ld a,(wPaletteThread_mode)
  CYC(b_+242, b_+243); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+243); return; } CYC(b_+243, b_+244);  // ret nz
  CYC(b_+244, b_+245); H = D;  // ld h,d
  CYC(b_+245, b_+247); L = 0x85;  // ld l,Enemy.substate
  CYC(b_+247, b_+248); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+248, b_+250); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+250, b_+252); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));  // res 0,(hl)
  CYC(b_+252, b_+254); A = 0x01;  // ld a,$01
  CYC(b_+254, b_+257); mem_wr(gb, wLoadedObjectGfxEnd, A);  // ld (wLoadedTreeGfxIndex),a
  RET(b_+257); return;  // ret
substate7:
  CYC(b_+258, b_+259); H = D;  // ld h,d
  CYC(b_+259, b_+261); L = 0x80;  // ld l,Enemy.enabled
  CYC(b_+261, b_+262); H = alu_inc8(gb, H);  // inc h
nextEnemy:
  CYC(b_+262, b_+263); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+263, b_+264); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+264, b_+266); goto foundFreeSlot; } CYC(b_+264, b_+266);  // jr z,@foundFreeSlot
  CYC(b_+266, b_+267); H = alu_inc8(gb, H);  // inc h
  CYC(b_+267, b_+268); A = H;  // ld a,h
  CYC(b_+268, b_+270); alu_cp(gb, 0xe0);  // cp LAST_ENEMY_INDEX+1
  if ((F & FC)) { CYCT(b_+270, b_+272); goto nextEnemy; } CYC(b_+270, b_+272);  // jr c,@nextEnemy
  RET(b_+272); return;  // ret
foundFreeSlot:
  CYC(b_+273, b_+274); E = L;  // ld e,l
  CYC(b_+274, b_+275); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+275, b_+276); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+276, b_+278); mem_wr(gb, HL, 0x01);  // ld (hl),ENEMY_MERGED_TWINROVA
  CALL_C(b_+278, objectCopyPosition_hook, SYM(objectCopyPosition), b_+281);
  CYC(b_+281, b_+283); A = 0x01;  // ld a,$01
  CYC(b_+283, b_+286); mem_wr(gb, wLoadedObjectGfxEnd, A);  // ld (wLoadedTreeGfxIndex),a
  CYC(b_+286, b_+289); if (hook_is(gb, SYM(enemyDelete), enemyDelete_hook)) { enemyDelete_hook(gb); return; } HANDOFF(SYM(enemyDelete));  // jp enemyDelete
}

void twinrova_subid1_hook(GB *gb) {
  BASE(twinrova_subid1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+1, b_+3); alu_sub(gb, 0x08);  // sub $08
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (twinrova_jump_table(gb));
    if (jt_ == SYM(twinrova_state8) && hook_is(gb, SYM(twinrova_state8), twinrova_state8_hook)) { twinrova_state8_hook(gb); return; }
    else if (jt_ == SYM(twinrova_state9) && hook_is(gb, SYM(twinrova_state9), twinrova_state9_hook)) { twinrova_state9_hook(gb); return; }
    else if (jt_ == SYM(twinrova_subid1_stateA) && hook_is(gb, SYM(twinrova_subid1_stateA), twinrova_subid1_stateA_hook)) { twinrova_subid1_stateA_hook(gb); return; }
    else if (jt_ == SYM(twinrova_subid1_stateB) && hook_is(gb, SYM(twinrova_subid1_stateB), twinrova_subid1_stateB_hook)) { twinrova_subid1_stateB_hook(gb); return; }
    else if (jt_ == SYM(twinrova_subid1_stateC) && hook_is(gb, SYM(twinrova_subid1_stateC), twinrova_subid1_stateC_hook)) { twinrova_subid1_stateC_hook(gb); return; }
    else if (jt_ == SYM(twinrova_stateD) && hook_is(gb, SYM(twinrova_stateD), twinrova_stateD_hook)) { twinrova_stateD_hook(gb); return; }
    else if (jt_ == SYM(twinrova_stateE) && hook_is(gb, SYM(twinrova_stateE), twinrova_stateE_hook)) { twinrova_stateE_hook(gb); return; }
    else if (jt_ == SYM(twinrova_stateF) && hook_is(gb, SYM(twinrova_stateF), twinrova_stateF_hook)) { twinrova_stateF_hook(gb); return; }
    else if (jt_ == SYM(twinrova_state10) && hook_is(gb, SYM(twinrova_state10), twinrova_state10_hook)) { twinrova_state10_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void twinrova_subid1_stateA_hook(GB *gb) {
  BASE(twinrova_subid1_stateA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x0b;  // ld a,$0b
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+3, b_+6); TAIL(twinrova_subid1_updateTargetPosition);  // jp twinrova_subid1_updateTargetPosition
}

void twinrova_subid1_stateB_hook(GB *gb) {
  BASE(twinrova_subid1_stateB);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, twinrova_moveTowardTargetPosition_hook, SYM(twinrova_moveTowardTargetPosition), b_+3);
  if (!(F & FC)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret nc
  CALL_C(b_+4, twinrova_subid1_updateTargetPosition_hook, SYM(twinrova_subid1_updateTargetPosition), b_+7);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; } CYC(b_+7, b_+8);  // ret nz
  CALL_C(b_+8, ecom_incState_b10_hook, SYM(ecom_incState_b10), b_+11);
  CYC(b_+11, b_+13); L = 0x86;  // ld l,Enemy.counter1
  CYC(b_+13, b_+15); mem_wr(gb, HL, 30);  // ld (hl),30
  RET(b_+15); return;  // ret
}

void twinrova_subid1_stateC_hook(GB *gb) {
  BASE(twinrova_subid1_stateC);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, ecom_decCounter1_b10_hook, SYM(ecom_decCounter1_b10), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); if (hook_is(gb, SYM(enemyAnimate), enemyAnimate_hook)) { enemyAnimate_hook(gb); return; } HANDOFF(SYM(enemyAnimate)); } CYC(b_+3, b_+6);  // jp nz,enemyAnimate
  CYC(b_+6, b_+7); L = E;  // ld l,e
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CALL_C(b_+8, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+11);
  CYC(b_+11, b_+13); alu_and(gb, 0x03);  // and $03
  CYC(b_+13, b_+15); E = 0xb3;  // ld e,Enemy.var33
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+16, b_+17); E = alu_inc8(gb, E);  // inc e
  CYC(b_+17, b_+18); alu_xor(gb, A);  // xor a
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+19, b_+22); TAIL(twinrova_subid1_updateTargetPosition);  // jp twinrova_subid1_updateTargetPosition
}

void twinrova_initialize_hook(GB *gb) {
  BASE(twinrova_initialize);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0x83;  // ld l,Enemy.var03
  CYC(b_+3, b_+5); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if ((F & FZ)) { CYCT(b_+5, b_+8); if (hook_is(gb, SYM(ecom_setSpeedAndState8_b10), ecom_setSpeedAndState8_b10_hook)) { ecom_setSpeedAndState8_b10_hook(gb); return; } HANDOFF(SYM(ecom_setSpeedAndState8_b10)); } CYC(b_+5, b_+8);  // jp z,ecom_setSpeedAndState8
  CYC(b_+8, b_+9); alu_xor(gb, A);  // xor a
  CYC(b_+9, b_+12); mem_wr(gb, wDisabledObjects, A);  // ld (wDisabledObjects),a
  CYC(b_+12, b_+15); mem_wr(gb, wMenuDisabled, A);  // ld (wMenuDisabled),a
  CYC(b_+15, b_+17); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x56);  // ld (hl),$56
  CYC(b_+19, b_+21); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x60);  // ld (hl),$60
  CYC(b_+23, b_+25); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+26, b_+27); alu_or(gb, A);  // or a
  CYC(b_+27, b_+29); A = 0x02;  // ld a,$02
  if ((F & FZ)) { CYCT(b_+29, b_+31); goto setAngle; } CYC(b_+29, b_+31);  // jr z,+
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x90);  // ld (hl),$90 [xh]
  CYC(b_+33, b_+34); A = alu_dec8(gb, A);  // dec a
setAngle:
  CYC(b_+34, b_+36); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+38, b_+40); L = 0x84;  // ld l,Enemy.state
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x0a);  // ld (hl),$0a
  CYC(b_+42, b_+44); L = 0x88;  // ld l,Enemy.direction
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0xff);  // ld (hl),$ff
  CYC(b_+46, b_+48); L = 0x90;  // ld l,Enemy.speed
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x32);  // ld (hl),SPEED_140
  CYC(b_+50, b_+52); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+52, b_+54); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3)));  // set 3,(hl)
  CYC(b_+54, b_+56); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(b_+56, ecom_updateAngleTowardTarget_b10_hook, SYM(ecom_updateAngleTowardTarget_b10), b_+59);
  CALL_C(b_+59, twinrova_calculateAnimationFromAngle_hook, SYM(twinrova_calculateAnimationFromAngle), b_+62);
  CYC(b_+62, b_+64); alu_add(gb, 0x04);  // add $04
  CYC(b_+64, b_+65); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+65, b_+68); if (hook_is(gb, SYM(enemySetAnimation), enemySetAnimation_hook)) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void twinrova_updateZPosition_hook(GB *gb) {
  BASE(twinrova_updateZPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xb7;  // ld l,Enemy.var37
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+4, b_+5); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+5, b_+7); goto checkBobbing; } CYC(b_+5, b_+7);  // jr z,+
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
checkBobbing:
  CYC(b_+8, b_+10); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+10, b_+12); alu_bit(gb, 3, mem_rd(gb, HL));  // bit 3,(hl)
  if ((F & FZ)) { RET_TAKEN(b_+12); return; } CYC(b_+12, b_+13);  // ret z
  CYC(b_+13, b_+15); L = 0xb1;  // ld l,Enemy.var31
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+17, b_+19); alu_and(gb, 0x07);  // and $07
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } CYC(b_+19, b_+20);  // ret nz
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+21, b_+23); alu_and(gb, 0x18);  // and $18
  CYC(b_+23, b_+25); A = alu_swap(gb, A);  // swap a
  CYC(b_+25, b_+26); alu_rlca(gb);  // rlca
  CYC(b_+26, b_+29); SET_HL(b_+35);  // ld hl,@levitationZPositions
  CYC(b_+29, b_+30);
  twinrova_addAToHl_from_rst(gb, b_+30);  // rst_addAToHl
  CYC(b_+30, b_+32); E = 0x8f;  // ld e,Enemy.zh
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);  // ld (de),a
  RET(b_+34); return;  // ret
}

void twinrova_checkFireProjectile_hook(GB *gb) {
  BASE(twinrova_checkFireProjectile);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  if ((F & FZ)) { RET_TAKEN(b_+5); return; } CYC(b_+5, b_+6);  // ret z
  CYC(b_+6, b_+8); alu_bit(gb, 2, mem_rd(gb, HL));  // bit 2,(hl)
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto fireProjectile; } CYC(b_+8, b_+10);  // jr nz,@fireProjectile
  CYC(b_+10, b_+12); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(b_+12, b_+14); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if ((F & FZ)) { RET_TAKEN(b_+14); return; } CYC(b_+14, b_+15);  // ret z
  CYC(b_+15, b_+17); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+17, b_+18); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+18, b_+19); alu_or(gb, A);  // or a
  CYC(b_+19, b_+21); E = 0xa1;  // ld e,Enemy.animParameter
  if ((F & FZ)) { CYCT(b_+21, b_+23); goto var39Zero; } CYC(b_+21, b_+23);  // jr z,@var39Zero
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);  // ld a,(de) [animParameter]
  CYC(b_+25, b_+26); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; } CYC(b_+26, b_+27);  // ret nz
var39Zero:
  CYC(b_+27, b_+28); A = alu_dec8(gb, A);  // dec a
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);  // ld (de),a [animParameter] = $ff
  CYC(b_+29, b_+31); E = 0x89;  // ld e,Enemy.angle
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);  // ld a,(de)
  CALL_C(b_+32, twinrova_calculateAnimationFromAngle_hook, SYM(twinrova_calculateAnimationFromAngle), b_+35);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+36, b_+38); alu_add(gb, 0x04);  // add $04
  CYC(b_+38, b_+41); if (hook_is(gb, SYM(enemySetAnimation), enemySetAnimation_hook)) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
fireProjectile:
  CYC(b_+41, b_+43); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 2)));  // res 2,(hl) [var32]
  CYC(b_+43, b_+45); L = 0x88;  // ld l,Enemy.direction
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0xff);  // ld (hl),$ff
  CYC(b_+47, b_+49); L = 0xb9;  // ld l,Enemy.var39
  CYC(b_+49, b_+51); mem_wr(gb, HL, 240);  // ld (hl),240
  CYC(b_+51, b_+54); push_effect(gb, b_+54); twinrova_checkFireProjectile__spawnProjectile_hook(gb);  // call @spawnProjectile
  CALL_C(b_+54, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+57);
  CYC(b_+57, b_+59); alu_cp(gb, 0x10);  // cp $10
  CYC(b_+59, b_+61); A = 0x00;  // ld a,$00
  if ((F & FC)) { CYCT(b_+61, b_+63); goto fireProjectile_addAnim; } CYC(b_+61, b_+63);  // jr c,+
  CYC(b_+63, b_+64); A = alu_inc8(gb, A);  // inc a
fireProjectile_addAnim:
  CYC(b_+64, b_+66); alu_add(gb, 0x08);  // add $08
  CYC(b_+66, b_+69); if (hook_is(gb, SYM(enemySetAnimation), enemySetAnimation_hook)) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

// 10:4eb2 (real call from twinrova_checkFireProjectile's @fireProjectile block, whose own
// tail-jp into ecom_spawnProjectile_b10_hook eventually rets back through this C call chain
// to the code right after the call there, so this stays a plain non-hooked helper).
void twinrova_checkFireProjectile__spawnProjectile_hook(GB *gb) {
  BASE(twinrova_checkFireProjectile);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+69, b_+71); B = 0x4b;  // ld b,PART_RED_TWINROVA_PROJECTILE
  CYC(b_+71, b_+73); E = 0x82;  // ld e,Enemy.subid
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+74, b_+75); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+75, b_+77); goto spawn; } CYC(b_+75, b_+77);  // jr z,+
  CYC(b_+77, b_+79); B = 0x4d;  // ld b,PART_BLUE_TWINROVA_PROJECTILE
spawn:
  CYC(b_+79, b_+82); if (hook_is(gb, SYM(ecom_spawnProjectile_b10), ecom_spawnProjectile_b10_hook)) { ecom_spawnProjectile_b10_hook(gb); return; } HANDOFF(SYM(ecom_spawnProjectile_b10));  // jp ecom_spawnProjectile
}

void twinrova_setTargetPositionToObject_hook(GB *gb) {
  BASE(twinrova_setTargetPositionToObject);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0x8b;  // ld l,Enemy.yh
  CYC(b_+2, b_+3); E = L;  // ld e,l
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);  // ld b,(hl)
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+5, b_+7); mem_wr(gb, hFF8F, A);  // ldh (<hFF8F),a
  CYC(b_+7, b_+9); L = 0x8d;  // ld l,Enemy.xh
  CYC(b_+9, b_+10); E = L;  // ld e,l
  CYC(b_+10, b_+11); C = mem_rd(gb, HL);  // ld c,(hl)
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+12, b_+14); mem_wr(gb, hFF8E, A);  // ldh (<hFF8E),a
  CALL_C(b_+14, ecom_moveTowardPosition_b10_hook, SYM(ecom_moveTowardPosition_b10), b_+17);
  CYC(b_+17, b_+19); TAIL(twinrova_updateMovingAnimation);  // jr twinrova_updateMovingAnimation
}

void twinrova_updateAnimationFromAngle_hook(GB *gb) {
  BASE(twinrova_updateAnimationFromAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x89;  // ld e,Enemy.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  CALL_C(b_+3, twinrova_calculateAnimationFromAngle_hook, SYM(twinrova_calculateAnimationFromAngle), b_+6);
  if ((F & FZ)) { RET_TAKEN(b_+6); return; } CYC(b_+6, b_+7);  // ret z
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+8, b_+11); if (hook_is(gb, SYM(enemySetAnimation), enemySetAnimation_hook)) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void twinrova_updateMovingAnimation_hook(GB *gb) {
  BASE(twinrova_updateMovingAnimation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x89;  // ld e,Enemy.angle
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);  // ld a,(de)
  TAIL(twinrova_updateMovingAnimationGivenAngle);  // fallthrough
}

void twinrova_updateMovingAnimationGivenAngle_hook(GB *gb) {
  BASE(twinrova_updateMovingAnimationGivenAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, twinrova_calculateAnimationFromAngle_hook, SYM(twinrova_calculateAnimationFromAngle), b_+3);
  if ((F & FZ)) { RET_TAKEN(b_+3); return; } CYC(b_+3, b_+4);  // ret z
  CYC(b_+4, b_+6); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; } CYC(b_+6, b_+7);  // ret nz
  CYC(b_+7, b_+8); B = A;  // ld b,a
  CYC(b_+8, b_+10); E = 0xb7;  // ld e,Enemy.var37
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+11, b_+12); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; } CYC(b_+12, b_+13);  // ret nz
  CYC(b_+13, b_+15); A = 30;  // ld a,30
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+16, b_+17); A = B;  // ld a,b
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);  // ld (hl),a [direction]
  CYC(b_+18, b_+20); alu_add(gb, 0x04);  // add $04
  CYC(b_+20, b_+23); if (hook_is(gb, SYM(enemySetAnimation), enemySetAnimation_hook)) { enemySetAnimation_hook(gb); return; } HANDOFF(SYM(enemySetAnimation));  // jp enemySetAnimation
}

void twinrova_calculateAnimationFromAngle_hook(GB *gb) {
  BASE(twinrova_calculateAnimationFromAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;  // ld c,a
  CYC(b_+1, b_+3); alu_add(gb, 0x04);  // add $04
  CYC(b_+3, b_+5); alu_and(gb, 0x18);  // and $18
  CYC(b_+5, b_+7); A = alu_swap(gb, A);  // swap a
  CYC(b_+7, b_+8); alu_rlca(gb);  // rlca
  CYC(b_+8, b_+9); B = A;  // ld b,a
  CYC(b_+9, b_+10); H = D;  // ld h,d
  CYC(b_+10, b_+12); L = 0x88;  // ld l,Enemy.direction
  CYC(b_+12, b_+13); A = C;  // ld a,c
  CYC(b_+13, b_+15); alu_and(gb, 0x07);  // and $07
  CYC(b_+15, b_+17); alu_cp(gb, 0x04);  // cp $04
  CYC(b_+17, b_+18); A = B;  // ld a,b
  if ((F & FZ)) { RET_TAKEN(b_+18); return; } CYC(b_+18, b_+19);  // ret z
  CYC(b_+19, b_+21); alu_cp(gb, mem_rd(gb, HL));  // cp (hl)
  RET(SYM(twinrova_subid0_updateTargetPosition)); return;  // ret
}

void twinrova_subid0_updateTargetPosition_hook(GB *gb) {
  BASE(twinrova_subid1_updateTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(SYM(twinrova_subid0_updateTargetPosition), (SYM(twinrova_subid0_updateTargetPosition) + 3)); SET_HL(SYM(twinrova_subid0_targetPositions));  // ld hl,twinrova_subid0_targetPositions
  CYCT((SYM(twinrova_subid0_updateTargetPosition) + 3), (SYM(twinrova_subid0_updateTargetPosition) + 5)); goto shared;  // jr ++
shared:
  CYC(b_+3, b_+5); E = 0xb7;  // ld e,Enemy.var37
  CYC(b_+5, b_+6); alu_xor(gb, A);  // xor a
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+7, b_+9); E = 0xb4;  // ld e,Enemy.var34
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+10, b_+11); B = A;  // ld b,a
  CYC(b_+11, b_+12); A = alu_inc8(gb, A);  // inc a
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+13, b_+14); E = alu_dec8(gb, E);  // dec e
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);  // ld a,(de) [var33]
  CYC(b_+15, b_+16);
  twinrova_addDoubleIndexToHl_from_rst(gb, b_+16);  // rst_addDoubleIndexToHl
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+17, b_+18); H = mem_rd(gb, HL);  // ld h,(hl)
  CYC(b_+18, b_+19); L = A;  // ld l,a
  CYC(b_+19, b_+20); A = B;  // ld a,b
  CYC(b_+20, b_+21);
  twinrova_addDoubleIndexToHl_from_rst(gb, b_+21);  // rst_addDoubleIndexToHl
  CYC(b_+21, b_+23); E = 0xb5;  // ld e,Enemy.var35
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+24, b_+25); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(b_+25); return; } CYC(b_+25, b_+26);  // ret z
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+27, b_+28); E = alu_inc8(gb, E);  // inc e
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);  // ld (de),a
  TAIL(twinrova_moveTowardTargetPosition);  // fallthrough
}

void twinrova_subid1_updateTargetPosition_hook(GB *gb) {
  BASE(twinrova_subid1_updateTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(twinrova_subid1_targetPositions));  // ld hl,twinrova_subid1_targetPositions
  CYC(b_+3, b_+5); E = 0xb7;  // ld e,Enemy.var37
  CYC(b_+5, b_+6); alu_xor(gb, A);  // xor a
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+7, b_+9); E = 0xb4;  // ld e,Enemy.var34
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+10, b_+11); B = A;  // ld b,a
  CYC(b_+11, b_+12); A = alu_inc8(gb, A);  // inc a
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+13, b_+14); E = alu_dec8(gb, E);  // dec e
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);  // ld a,(de) [var33]
  CYC(b_+15, b_+16);
  twinrova_addDoubleIndexToHl_from_rst(gb, b_+16);  // rst_addDoubleIndexToHl
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+17, b_+18); H = mem_rd(gb, HL);  // ld h,(hl)
  CYC(b_+18, b_+19); L = A;  // ld l,a
  CYC(b_+19, b_+20); A = B;  // ld a,b
  CYC(b_+20, b_+21);
  twinrova_addDoubleIndexToHl_from_rst(gb, b_+21);  // rst_addDoubleIndexToHl
  CYC(b_+21, b_+23); E = 0xb5;  // ld e,Enemy.var35
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(b_+24, b_+25); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(b_+25); return; } CYC(b_+25, b_+26);  // ret z
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+27, b_+28); E = alu_inc8(gb, E);  // inc e
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);  // ld (de),a
  TAIL(twinrova_moveTowardTargetPosition);  // fallthrough
}

void twinrova_moveTowardTargetPosition_hook(GB *gb) {
  BASE(twinrova_moveTowardTargetPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xb5;  // ld l,Enemy.var35
  CALL_C(b_+3, ecom_readPositionVars_b10_hook, SYM(ecom_readPositionVars_b10), b_+6);
  CYC(b_+6, b_+7); alu_sub(gb, C);  // sub c
  CYC(b_+7, b_+8); A = alu_inc8(gb, A);  // inc a
  CYC(b_+8, b_+10); alu_cp(gb, 0x03);  // cp $03
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto moveToward; } CYC(b_+10, b_+12);  // jr nc,@moveToward
  CYC(b_+12, b_+14); A = mem_rd(gb, hFF8F);  // ldh a,(<hFF8F)
  CYC(b_+14, b_+15); alu_sub(gb, B);  // sub b
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);  // inc a
  CYC(b_+16, b_+18); alu_cp(gb, 0x03);  // cp $03
  if ((F & FC)) { RET_TAKEN(b_+18); return; } CYC(b_+18, b_+19);  // ret c
moveToward:
  CALL_C(b_+19, ecom_moveTowardPosition_b10_hook, SYM(ecom_moveTowardPosition_b10), b_+22);
  CALL_C(b_+22, twinrova_updateMovingAnimation_hook, SYM(twinrova_updateMovingAnimation), b_+25);
  CALL_C(b_+25, enemyAnimate_hook, SYM(enemyAnimate), b_+28);
  CYC(b_+28, b_+29); alu_or(gb, D);  // or d
  RET(b_+29); return;  // ret
}

void twinrova_chooseObjectToAttack_hook(GB *gb) {
  BASE(twinrova_chooseObjectToAttack);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+4); alu_rrca(gb);  // rrca
  CYC(b_+4, b_+5); H = D;  // ld h,d
  CYC(b_+5, b_+7); L = 0xb2;  // ld l,Enemy.var32
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto orFlag; } CYC(b_+7, b_+9);  // jr nc,++
  CYC(b_+9, b_+11); A = 0x32;  // ld a,Object.var32
  CALL_C(b_+11, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+14);
orFlag:
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+15, b_+17); alu_or(gb, 0x05);  // or $05
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);  // ld (hl),a
  RET(b_+18); return;  // ret
}

void twinrova_checkAttackInProgress_hook(GB *gb) {
  BASE(twinrova_checkAttackInProgress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+3, b_+5); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto checkDirection; } CYC(b_+5, b_+7);  // jr nz,++
  CYC(b_+7, b_+9); A = 0x32;  // ld a,Object.var32
  CALL_C(b_+9, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+12);
  CYC(b_+12, b_+14); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  if ((F & FZ)) { RET_TAKEN(b_+14); return; } CYC(b_+14, b_+15);  // ret z
checkDirection:
  CYC(b_+15, b_+17); L = 0x88;  // ld l,Enemy.direction
  CYC(b_+17, b_+19); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } CYC(b_+19, b_+20);  // ret nz
  CYC(b_+20, b_+22); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+22, b_+24); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));  // res 0,(hl)
  CYC(b_+24, b_+25); alu_or(gb, D);  // or d
  RET(b_+25); return;  // ret
}

void twinrova_rise2PixelsAboveGround_hook(GB *gb) {
  BASE(twinrova_rise2PixelsAboveGround);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;  // ld h,d
  CYC(b_+1, b_+3); L = 0x8f;  // ld l,Enemy.zh
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+4, b_+6); alu_cp(gb, 0xfe);  // cp $fe
  if ((F & FC)) { CYCT(b_+6, b_+8); goto belowThreshold; } CYC(b_+6, b_+8);  // jr c,++
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  RET(b_+9); return;  // ret
belowThreshold:
  CYC(b_+10, b_+12); L = 0xb2;  // ld l,Enemy.var32
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+13, b_+15); alu_or(gb, 0x18);  // or $18
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+16, b_+17); alu_xor(gb, A);  // xor a
  RET(b_+17); return;  // ret
}

void twinrova_incState2ForSelfAndTwin_hook(GB *gb) {
  BASE(twinrova_incState2ForSelfAndTwin);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x05;  // ld a,Object.substate
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(b_+6, b_+7); H = D;  // ld h,d
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  RET(b_+8); return;  // ret
}
