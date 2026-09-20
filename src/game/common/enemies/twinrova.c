#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4a94, twinrova_checkFireProjectile_hook, 0x4e6d, 0x4a97);
  CALL_C(0x4a97, ecom_getSubidAndCpStateTo08_b10_hook, 0x4426, 0x4a9a);
  if (!(F & FC)) { CYCT(0x4a9a, 0x4a9c); goto state8OrHigher; } CYC(0x4a9a, 0x4a9c);  // jr nc,@state8OrHigher
  CYC(0x4a9c, 0x4a9d); push_effect(gb, 0x4a9d);
  switch (twinrova_jump_table(gb)) {
    case 0x4ab3: twinrova_state_uninitialized_hook(gb); return;
    case 0x4ad4: twinrova_state_spawner_hook(gb); return;
    case 0x4b01: twinrova_state_stub_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
state8OrHigher:
  CYC(0x4aad, 0x4aae); A = B;  // ld a,b
  CYC(0x4aae, 0x4aaf); push_effect(gb, 0x4aaf);
  switch (twinrova_jump_table(gb)) {
    case 0x4b02: twinrova_subid0_hook(gb); return;
    case 0x4dc0: twinrova_subid1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void enemyCode03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if ((F & FZ)) { CYCT(0x4a57, 0x4a59); goto normalStatus; } CYC(0x4a57, 0x4a59);  // jr z,@normalStatus
  CYC(0x4a59, 0x4a5b); alu_sub(gb, 0x03);  // sub ENEMYSTATUS_NO_HEALTH
  if ((F & FC)) { RET_TAKEN(0x4a5b); return; } CYC(0x4a5b, 0x4a5c);  // ret c
  if (!(F & FZ)) { CYCT(0x4a5c, 0x4a5e); goto normalStatus; } CYC(0x4a5c, 0x4a5e);  // jr nz,@normalStatus
  CYC(0x4a5e, 0x4a5f); H = D;  // ld h,d
  CYC(0x4a5f, 0x4a61); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4a61, 0x4a63); alu_bit(gb, 6, mem_rd(gb, HL));  // bit 6,(hl)
  if ((F & FZ)) { CYCT(0x4a63, 0x4a65); goto normalStatus; } CYC(0x4a63, 0x4a65);  // jr z,@normalStatus
  CYC(0x4a65, 0x4a67); L = 0xa9;  // ld l,Enemy.health
  CYC(0x4a67, 0x4a69); mem_wr(gb, HL, 0x7f);  // ld (hl),$7f
  CYC(0x4a69, 0x4a6b); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x4a6b, 0x4a6d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(0x4a6d, 0x4a6f); L = 0x84;  // ld l,Enemy.state
  CYC(0x4a6f, 0x4a71); mem_wr(gb, HL, 0x0d);  // ld (hl),$0d
  CYC(0x4a71, 0x4a73); A = 0x29;  // ld a,Object.health
  CALL_C(0x4a73, objectGetRelatedObject1Var_hook, 0x2160, 0x4a76);
  CYC(0x4a76, 0x4a78); mem_wr(gb, HL, 0x7f);  // ld (hl),$7f
  CYC(0x4a78, 0x4a7a); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x4a7a, 0x4a7c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(0x4a7c, 0x4a7e); L = 0x84;  // ld l,Enemy.state
  CYC(0x4a7e, 0x4a80); mem_wr(gb, HL, 0x0f);  // ld (hl),$0f
  CYC(0x4a80, 0x4a82); A = 0xf0;  // ld a,SNDCTRL_STOPMUSIC
  CALL_C(0x4a82, playSound_b00_hook, 0x0c98, 0x4a85);
normalStatus:
  CALL_C(0x4a85, twinrova_updateZPosition_hook, 0x4e46, 0x4a88);
  CYC(0x4a88, 0x4a8b); push_effect(gb, 0x4a8b); enemyCode03__runState_hook(gb);  // call @runState
  CYC(0x4a8b, 0x4a8d); E = 0xb2;  // ld e,Enemy.var32
  CYC(0x4a8d, 0x4a8e); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4a8e, 0x4a90); alu_bit(gb, 7, A);  // bit 7,a
  if (!(F & FZ)) { CYCT(0x4a90, 0x4a93); if (hook_enabled_at(0x22e0)) { objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb); return; } HANDOFF(0x22e0); } CYC(0x4a90, 0x4a93);  // jp nz,$22e0
  RET(0x4a93); return;  // ret
}

void twinrova_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4ab3, 0x4ab5); A = 0x03;  // ld a,ENEMY_TWINROVA
  CYC(0x4ab5, 0x4ab8); mem_wr(gb, 0xcc1d, A);  // ld (wEnemyIDToLoadExtraGfx),a
  CYC(0x4ab8, 0x4aba); A = 0x01;  // ld a,$01
  CYC(0x4aba, 0x4abd); mem_wr(gb, 0xcc18, A);  // ld (wLoadedTreeGfxIndex),a
  CYC(0x4abd, 0x4abe); H = D;  // ld h,d
  CYC(0x4abe, 0x4ac0); L = 0x83;  // ld l,Enemy.var03
  CYC(0x4ac0, 0x4ac2); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  CYC(0x4ac2, 0x4ac4); A = 0x28;  // ld a,SPEED_100
  if (!(F & FZ)) { CYCT(0x4ac4, 0x4ac7); twinrova_initialize_hook(gb); return; } CYC(0x4ac4, 0x4ac7);  // jp nz,twinrova_initialize
  CYC(0x4ac7, 0x4ac8); L = E;  // ld l,e
  CYC(0x4ac8, 0x4ac9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4ac9, 0x4aca); alu_xor(gb, A);  // xor a
  CYC(0x4aca, 0x4acd); mem_wr(gb, 0xd008, A);  // ld (w1Link.direction),a
  CYC(0x4acd, 0x4ace); A = alu_inc8(gb, A);  // inc a
  CYC(0x4ace, 0x4ad1); mem_wr(gb, 0xcc8a, A);  // ld (wDisabledObjects),a
  CYC(0x4ad1, 0x4ad4); mem_wr(gb, 0xcc02, A);  // ld (wMenuDisabled),a
  twinrova_state_spawner_hook(gb); return;  // fallthrough
}

void twinrova_state_spawner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4ad4, 0x4ad6); B = 0x03;  // ld b,ENEMY_TWINROVA
  CALL_C(0x4ad6, ecom_spawnUncountedEnemyWithSubid01_b10_hook, 0x436d, 0x4ad9);
  if (!(F & FZ)) { RET_TAKEN(0x4ad9); return; } CYC(0x4ad9, 0x4ada);  // ret nz
  CYC(0x4ada, 0x4adb); L = alu_inc8(gb, L);  // inc l
  CYC(0x4adb, 0x4adc); E = L;  // ld e,l
  CYC(0x4adc, 0x4add); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4add, 0x4ade); A = alu_inc8(gb, A);  // inc a
  CYC(0x4ade, 0x4adf); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4adf, 0x4ae1); L = 0x96;  // ld l,Enemy.relatedObj1
  CYC(0x4ae1, 0x4ae2); E = L;  // ld e,l
  CYC(0x4ae2, 0x4ae4); A = 0x80;  // ld a,Enemy.start
  CYC(0x4ae4, 0x4ae5); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4ae5, 0x4ae6); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ld (hl+),a
  CYC(0x4ae6, 0x4ae7); E = alu_inc8(gb, E);  // inc e
  CYC(0x4ae7, 0x4ae8); A = H;  // ld a,h
  CYC(0x4ae8, 0x4ae9); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4ae9, 0x4aea); mem_wr(gb, HL, D);  // ld (hl),d
  CYC(0x4aea, 0x4aeb); A = H;  // ld a,h
  CYC(0x4aeb, 0x4aec); alu_cp(gb, D);  // cp d
  CYC(0x4aec, 0x4aee); A = 0x28;  // ld a,SPEED_100
  if (!(F & FC)) { CYCT(0x4aee, 0x4af1); twinrova_initialize_hook(gb); return; } CYC(0x4aee, 0x4af1);  // jp nc,twinrova_initialize
  CYC(0x4af1, 0x4af3); L = 0x80;  // ld l,Enemy.enabled
  CYC(0x4af3, 0x4af4); E = L;  // ld e,l
  CYC(0x4af4, 0x4af5); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4af5, 0x4af6); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4af6, 0x4af8); L = 0x82;  // ld l,Enemy.subid
  CYC(0x4af8, 0x4af9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(0x4af9, 0x4afa); H = D;  // ld h,d
  CYC(0x4afa, 0x4afb); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4afb, 0x4afc); L = alu_inc8(gb, L);  // inc l
  CYC(0x4afc, 0x4afd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4afd, 0x4aff); L = 0x84;  // ld l,Enemy.state
  CYC(0x4aff, 0x4b00); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  RET(0x4b00); return;  // ret
}

void twinrova_state_stub_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(0x4b01); return;  // ret
}

void twinrova_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4b02, 0x4b03); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4b03, 0x4b05); alu_sub(gb, 0x08);  // sub $08
  CYC(0x4b05, 0x4b06); push_effect(gb, 0x4b06);
  switch (twinrova_jump_table(gb)) {
    case 0x4b18: twinrova_state8_hook(gb); return;
    case 0x4b5e: twinrova_state9_hook(gb); return;
    case 0x4bf4: twinrova_subid0_stateA_hook(gb); return;
    case 0x4c02: twinrova_subid0_stateB_hook(gb); return;
    case 0x4c33: twinrova_subid0_stateC_hook(gb); return;
    case 0x4c49: twinrova_stateD_hook(gb); return;
    case 0x4c60: twinrova_stateE_hook(gb); return;
    case 0x4c7d: twinrova_stateF_hook(gb); return;
    case 0x4c9f: twinrova_state10_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void twinrova_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4b18, 0x4b19); H = D;  // ld h,d
  CYC(0x4b19, 0x4b1a); L = E;  // ld l,e
  CYC(0x4b1a, 0x4b1b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4b1b, 0x4b1d); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4b1d, 0x4b1f); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3)));  // set 3,(hl)
  CYC(0x4b1f, 0x4b21); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x4b21, 0x4b23); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x4b23, 0x4b25); mem_wr(gb, HL, 106);  // ld (hl),106
  CYC(0x4b25, 0x4b27); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x4b27, 0x4b29); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x4b29, 0x4b2b); L = 0x82;  // ld l,Enemy.subid
  CYC(0x4b2b, 0x4b2c); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4b2c, 0x4b2d); alu_add(gb, A);  // add a
  CYC(0x4b2d, 0x4b30); SET_HL(0x4b56);  // ld hl,@data
  CYC(0x4b30, 0x4b31);
  twinrova_addDoubleIndexToHl_from_rst(gb, 0x4b31);  // rst_addDoubleIndexToHl
  CYC(0x4b31, 0x4b33); E = 0x8d;  // ld e,Enemy.xh
  CYC(0x4b33, 0x4b34); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x4b34, 0x4b35); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4b35, 0x4b37); E = 0x9b;  // ld e,Enemy.oamFlagsBackup
  CYC(0x4b37, 0x4b38); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x4b38, 0x4b39); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4b39, 0x4b3a); E = alu_inc8(gb, E);  // inc e
  CYC(0x4b3a, 0x4b3b); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4b3b, 0x4b3d); E = 0x89;  // ld e,Enemy.angle
  CYC(0x4b3d, 0x4b3e); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x4b3e, 0x4b3f); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4b3f, 0x4b41); E = 0xb0;  // ld e,Enemy.var30
  CYC(0x4b41, 0x4b42); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4b42, 0x4b43); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4b43, 0x4b45); E = 0x82;  // ld e,Enemy.subid
  CYC(0x4b45, 0x4b46); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4b46, 0x4b47); alu_or(gb, A);  // or a
  CYC(0x4b47, 0x4b4a); SET_BC(0x2f04);  // ld bc,TX_2f04
  if ((F & FZ)) { CALL_C_CC(0x4b4a, showText_hook, 0x1872, 0x4b4d); } else CYC(0x4b4a, 0x4b4d);  // call z,showText
  CALL_C(0x4b4d, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4b50);
  CYC(0x4b50, 0x4b52); E = 0xb1;  // ld e,Enemy.var31
  CYC(0x4b52, 0x4b53); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4b53, 0x4b56); twinrova_updateAnimationFromAngle_hook(gb); return;  // jp twinrova_updateAnimationFromAngle
}

void twinrova_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4b5e, 0x4b5f); E = alu_inc8(gb, E);  // inc e
  CYC(0x4b5f, 0x4b60); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4b60, 0x4b61); push_effect(gb, 0x4b61);
  switch (twinrova_jump_table(gb)) {
    case 0x4b6b: goto substate0;
    case 0x4b79: goto substate1;
    case 0x4ba5: goto substate2;
    case 0x4bb7: goto substate3;
    case 0x4bcf: goto substate4;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CALL_C(0x4b6b, ecom_decCounter1_b10_hook, 0x439a, 0x4b6e);
  if (!(F & FZ)) { CYCT(0x4b6e, 0x4b70); goto applySpeed; } CYC(0x4b6e, 0x4b70);  // jr nz,@applySpeed
  CYC(0x4b70, 0x4b72); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x4b72, 0x4b73); L = alu_inc8(gb, L);  // inc l
  CYC(0x4b73, 0x4b75); mem_wr(gb, HL, 0x12);  // ld (hl),$12
  CYC(0x4b75, 0x4b76); L = E;  // ld l,e
  CYC(0x4b76, 0x4b77); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYCT(0x4b77, 0x4b7a); goto animate;  // jr @animate
substate1:
  CALL_C(0x4b79, ecom_decCounter1_b10_hook, 0x439a, 0x4b7c);
  if (!(F & FZ)) { CYCT(0x4b7c, 0x4b7e); goto applySpeed; } CYC(0x4b7c, 0x4b7e);  // jr nz,@applySpeed
  CYC(0x4b7e, 0x4b80); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  CYC(0x4b80, 0x4b81); L = alu_inc8(gb, L);  // inc l
  CYC(0x4b81, 0x4b82); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if (!(F & FZ)) { CYCT(0x4b82, 0x4b84); goto updateAngle; } CYC(0x4b82, 0x4b84);  // jr nz,@updateAngle
  CYC(0x4b84, 0x4b85); L = E;  // ld l,e
  CYC(0x4b85, 0x4b86); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4b86, 0x4b87); L = alu_inc8(gb, L);  // inc l
  CYC(0x4b87, 0x4b89); mem_wr(gb, HL, 30);  // ld (hl),30
  CALL_C(0x4b89, ecom_updateAngleTowardTarget_b10_hook, 0x43bf, 0x4b8c);
  CALL_C(0x4b8c, twinrova_calculateAnimationFromAngle_hook, 0x4ef7, 0x4b8f);
  CYC(0x4b8f, 0x4b90); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4b90, 0x4b93); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
updateAngle:
  CYC(0x4b93, 0x4b95); E = 0x89;  // ld e,Enemy.angle
  CYC(0x4b95, 0x4b97); L = 0xb0;  // ld l,Enemy.var30
  CYC(0x4b97, 0x4b98); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4b98, 0x4b99); alu_add(gb, mem_rd(gb, HL));  // add (hl)
  CYC(0x4b99, 0x4b9b); alu_and(gb, 0x1f);  // and $1f
  CYC(0x4b9b, 0x4b9c); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(0x4b9c, twinrova_updateAnimationFromAngle_hook, 0x4ed2, 0x4b9f);
applySpeed:
  CALL_C(0x4b9f, objectApplySpeed_hook, 0x201d, 0x4ba2);
animate:
  CYC(0x4ba2, 0x4ba5); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818);  // jp enemyAnimate
substate2:
  CALL_C(0x4ba5, ecom_decCounter1_b10_hook, 0x439a, 0x4ba8);
  if (!(F & FZ)) { CYCT(0x4ba8, 0x4baa); goto animate; } CYC(0x4ba8, 0x4baa);  // jr nz,@animate
  CYC(0x4baa, 0x4bab); L = E;  // ld l,e
  CYC(0x4bab, 0x4bac); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4bac, 0x4bae); E = 0x82;  // ld e,Enemy.subid
  CYC(0x4bae, 0x4baf); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4baf, 0x4bb0); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x4bb0); return; } CYC(0x4bb0, 0x4bb1);  // ret nz
  CYC(0x4bb1, 0x4bb4); SET_BC(0x2f05);  // ld bc,TX_2f05
  CYC(0x4bb4, 0x4bb7); if (hook_enabled_at(0x1872)) { showText_hook(gb); return; } HANDOFF(0x1872);  // jp showText
substate3:
  CYC(0x4bb7, 0x4bba); SET_BC(0x0502);  // ldbc INTERAC_PUFF,$02
  CALL_C(0x4bba, objectCreateInteraction_hook, 0x24c5, 0x4bbd);
  if (!(F & FZ)) { RET_TAKEN(0x4bbd); return; } CYC(0x4bbd, 0x4bbe);  // ret nz
  CYC(0x4bbe, 0x4bbf); A = H;  // ld a,h
  CYC(0x4bbf, 0x4bc0); H = D;  // ld h,d
  CYC(0x4bc0, 0x4bc2); L = 0x99;  // ld l,Enemy.relatedObj2+1
  CYC(0x4bc2, 0x4bc3); mem_wr(gb, HL, A); SET_HL(HL - 1);  // ldd (hl),a
  CYC(0x4bc3, 0x4bc5); mem_wr(gb, HL, 0x40);  // ld (hl),Interaction.start
  CYC(0x4bc5, 0x4bc7); L = 0x85;  // ld l,Enemy.substate
  CYC(0x4bc7, 0x4bc8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4bc8, 0x4bca); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4bca, 0x4bcc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(0x4bcc, 0x4bcf); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);  // jp objectSetInvisible
substate4:
  CYC(0x4bcf, 0x4bd1); A = 0x21;  // ld a,Object.animParameter
  CALL_C(0x4bd1, objectGetRelatedObject2Var_hook, 0x2164, 0x4bd4);
  CYC(0x4bd4, 0x4bd6); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if ((F & FZ)) { RET_TAKEN(0x4bd6); return; } CYC(0x4bd6, 0x4bd7);  // ret z
  CYC(0x4bd7, 0x4bd8); H = D;  // ld h,d
  CYC(0x4bd8, 0x4bda); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4bda, 0x4bdc); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CYC(0x4bdc, 0x4bdd); alu_xor(gb, A);  // xor a
  CYC(0x4bdd, 0x4be0); mem_wr(gb, 0xcc8a, A);  // ld (wDisabledObjects),a
  CYC(0x4be0, 0x4be3); mem_wr(gb, 0xcc02, A);  // ld (wMenuDisabled),a
  CYC(0x4be3, 0x4be5); A = 0x18;  // ld a,CUTSCENE_FLAMES_FLICKERING
  CYC(0x4be5, 0x4be8); mem_wr(gb, 0xcc04, A);  // ld (wCutsceneTrigger),a
  CALL_C(0x4be8, ecom_updateAngleTowardTarget_b10_hook, 0x43bf, 0x4beb);
  CALL_C(0x4beb, twinrova_calculateAnimationFromAngle_hook, 0x4ef7, 0x4bee);
  CYC(0x4bee, 0x4bf0); alu_add(gb, 0x04);  // add $04
  CYC(0x4bf0, 0x4bf1); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4bf1, 0x4bf4); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void twinrova_subid0_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4bf4, 0x4bf5); H = D;  // ld h,d
  CYC(0x4bf5, 0x4bf6); L = E;  // ld l,e
  CYC(0x4bf6, 0x4bf7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4bf7, 0x4bf9); A = 0x33;  // ld a,MUS_TWINROVA
  CYC(0x4bf9, 0x4bfc); mem_wr(gb, 0xcc35, A);  // ld (wActiveMusic),a
  CALL_C(0x4bfc, playSound_b00_hook, 0x0c98, 0x4bff);
  CYC(0x4bff, 0x4c02); twinrova_subid0_updateTargetPosition_hook(gb); return;  // jp twinrova_subid0_updateTargetPosition
}

void twinrova_subid0_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c02, 0x4c05); A = mem_rd(gb, 0xcc00);  // ld a,(wFrameCounter)
  CYC(0x4c05, 0x4c07); alu_and(gb, 0x7f);  // and $7f
  CYC(0x4c07, 0x4c09); A = 0x91;  // ld a,SND_FAIRYCUTSCENE
  if ((F & FZ)) { CALL_C_CC(0x4c09, playSound_b00_hook, 0x0c98, 0x4c0c); } else CYC(0x4c09, 0x4c0c);  // call z,playSound
  CALL_C(0x4c0c, twinrova_moveTowardTargetPosition_hook, 0x4f2f, 0x4c0f);
  if (!(F & FC)) { RET_TAKEN(0x4c0f); return; } CYC(0x4c0f, 0x4c10);  // ret nc
  CALL_C(0x4c10, twinrova_subid0_updateTargetPosition_hook, 0x4f0c, 0x4c13);
  if (!(F & FZ)) { CYCT(0x4c13, 0x4c15); goto waypointChanged; } CYC(0x4c13, 0x4c15);  // jr nz,@waypointChanged
  CALL_C(0x4c15, ecom_incState_b10_hook, 0x4000, 0x4c18);
  CYC(0x4c18, 0x4c1a); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x4c1a, 0x4c1c); mem_wr(gb, HL, 30);  // ld (hl),30
  RET(0x4c1c); return;  // ret
waypointChanged:
  CALL_C(0x4c1d, twinrova_checkAttackInProgress_hook, 0x4f60, 0x4c20);
  if (!(F & FZ)) { RET_TAKEN(0x4c20); return; } CYC(0x4c20, 0x4c21);  // ret nz
  CYC(0x4c21, 0x4c23); E = 0xb8;  // ld e,Enemy.var38
  CYC(0x4c23, 0x4c24); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4c24, 0x4c25); A = alu_inc8(gb, A);  // inc a
  CYC(0x4c25, 0x4c27); alu_and(gb, 0x07);  // and $07
  CYC(0x4c27, 0x4c28); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4c28, 0x4c2b); SET_HL(0x4c32);  // ld hl,@attackPattern
  CALL_C(0x4c2b, checkFlag_hook, 0x0205, 0x4c2e);
  if (!(F & FZ)) { CYCT(0x4c2e, 0x4c31); twinrova_chooseObjectToAttack_hook(gb); return; } CYC(0x4c2e, 0x4c31);  // jp nz,twinrova_chooseObjectToAttack
  RET(0x4c31); return;  // ret
}

void twinrova_subid0_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c33, ecom_decCounter1_b10_hook, 0x439a, 0x4c36);
  if (!(F & FZ)) { CYCT(0x4c36, 0x4c39); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x4c36, 0x4c39);  // jp nz,enemyAnimate
  CYC(0x4c39, 0x4c3a); L = E;  // ld l,e
  CYC(0x4c3a, 0x4c3b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CALL_C(0x4c3b, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4c3e);
  CYC(0x4c3e, 0x4c40); alu_and(gb, 0x03);  // and $03
  CYC(0x4c40, 0x4c42); E = 0xb3;  // ld e,Enemy.var33
  CYC(0x4c42, 0x4c43); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4c43, 0x4c44); E = alu_inc8(gb, E);  // inc e
  CYC(0x4c44, 0x4c45); alu_xor(gb, A);  // xor a
  CYC(0x4c45, 0x4c46); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4c46, 0x4c49); twinrova_subid0_updateTargetPosition_hook(gb); return;  // jp twinrova_subid0_updateTargetPosition
}

void twinrova_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c49, 0x4c4a); H = D;  // ld h,d
  CYC(0x4c4a, 0x4c4b); L = E;  // ld l,e
  CYC(0x4c4b, 0x4c4c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4c4c, 0x4c4e); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x4c4e, 0x4c50); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x4c50, 0x4c52); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4c52, 0x4c54); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 3)));  // res 3,(hl)
  CYC(0x4c54, 0x4c56); L = 0x89;  // ld l,Enemy.angle
  CYC(0x4c56, 0x4c58); alu_bit(gb, 4, mem_rd(gb, HL));  // bit 4,(hl)
  CYC(0x4c58, 0x4c5a); A = 0x0a;  // ld a,$0a
  if ((F & FZ)) { CYCT(0x4c5a, 0x4c5d); goto setAnim; } CYC(0x4c5a, 0x4c5c);  // jr z,+
  CYC(0x4c5c, 0x4c5d); A = alu_inc8(gb, A);  // inc a
setAnim:
  CYC(0x4c5d, 0x4c60); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void twinrova_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c60, 0x4c62); E = 0xa1;  // ld e,Enemy.animParameter
  CYC(0x4c62, 0x4c63); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4c63, 0x4c64); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { CYCT(0x4c64, 0x4c66); twinrova_animate_hook(gb); return; } CYC(0x4c64, 0x4c66);  // jr nz,twinrova_animate
  CYC(0x4c66, 0x4c68); E = 0x88;  // ld e,Enemy.direction
  CYC(0x4c68, 0x4c69); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4c69, 0x4c6b); alu_add(gb, 0x04);  // add $04
  CALL_C(0x4c6b, enemySetAnimation_hook, 0x282b, 0x4c6e);
  CALL_C(0x4c6e, ecom_incState_b10_hook, 0x4000, 0x4c71);
  CYC(0x4c71, 0x4c73); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x4c73, 0x4c74); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(0x4c74, 0x4c77); SET_BC(0x2f09);  // ld bc,TX_2f09
  CALL_C(0x4c77, showText_hook, 0x1872, 0x4c7a);
  twinrova_animate_hook(gb); return;  // fallthrough
}

void twinrova_animate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c7a, 0x4c7d); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818);  // jp enemyAnimate
}

void twinrova_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4c7d, twinrova_rise2PixelsAboveGround_hook, 0x4f7a, 0x4c80);
  if (!(F & FZ)) { CYCT(0x4c80, 0x4c82); twinrova_animate_hook(gb); return; } CYC(0x4c80, 0x4c82);  // jr nz,twinrova_animate
  CYC(0x4c82, 0x4c84); A = 0x32;  // ld a,Object.var32
  CALL_C(0x4c84, objectGetRelatedObject1Var_hook, 0x2160, 0x4c87);
  CYC(0x4c87, 0x4c89); alu_bit(gb, 4, mem_rd(gb, HL));  // bit 4,(hl)
  if (!(F & FZ)) { CYCT(0x4c89, 0x4c8b); goto nextState; } CYC(0x4c89, 0x4c8b);  // jr nz,@nextState
  CALL_C(0x4c8b, ecom_updateAngleTowardTarget_b10_hook, 0x43bf, 0x4c8e);
  CALL_C(0x4c8e, twinrova_updateMovingAnimation_hook, 0x4edd, 0x4c91);
  CYC(0x4c91, 0x4c93); twinrova_animate_hook(gb); return;  // jr twinrova_animate
nextState:
  CALL_C(0x4c93, ecom_incState_b10_hook, 0x4000, 0x4c96);
  CYC(0x4c96, 0x4c97); L = alu_inc8(gb, L);  // inc l
  CYC(0x4c97, 0x4c99); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x4c99, 0x4c9b); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4c9b, 0x4c9d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 3)));  // res 3,(hl)
  CYC(0x4c9d, 0x4c9f); twinrova_animate_hook(gb); return;  // jr twinrova_animate
}

void twinrova_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4c9f, 0x4ca0); E = alu_inc8(gb, E);  // inc e
  CYC(0x4ca0, 0x4ca1); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4ca1, 0x4ca2); push_effect(gb, 0x4ca2);
  switch (twinrova_jump_table(gb)) {
    case 0x4cb2: goto substate0;
    case 0x4cd0: goto substate1;
    case 0x4cfe: goto substate2;
    case 0x4d11: goto substate3;
    case 0x4d35: goto substate4;
    case 0x4d63: goto substate5;
    case 0x4d8e: goto substate6;
    case 0x4da1: goto substate7;
    default: hook_continue(gb, HL, sp0_); return;
  }
substate0:
  CYC(0x4cb2, 0x4cb3); H = D;  // ld h,d
  CYC(0x4cb3, 0x4cb4); L = E;  // ld l,e
  CYC(0x4cb4, 0x4cb5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4cb5, 0x4cb7); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4cb7, 0x4cb9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 1)));  // res 1,(hl)
  CYC(0x4cb9, 0x4cbb); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));  // res 0,(hl)
  CYC(0x4cbb, 0x4cbd); L = 0x88;  // ld l,Enemy.direction
  CYC(0x4cbd, 0x4cbf); mem_wr(gb, HL, 0x00);  // ld (hl),$00
  CYC(0x4cbf, 0x4cc2); SET_BC(0xfc20);  // ld bc,-$3e0
  CALL_C(0x4cc2, objectSetSpeedZ_hook, 0x239d, 0x4cc5);
  CYC(0x4cc5, 0x4cc7); E = 0x82;  // ld e,Enemy.subid
  CYC(0x4cc7, 0x4cc8); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4cc8, 0x4cc9); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x4cc9); return; } CYC(0x4cc9, 0x4cca);  // ret nz
  CYC(0x4cca, 0x4ccd); SET_BC(0x2f0a);  // ld bc,TX_2f0a
  CYC(0x4ccd, 0x4cd0); if (hook_enabled_at(0x1872)) { showText_hook(gb); return; } HANDOFF(0x1872);  // jp showText
substate1:
  CYC(0x4cd0, 0x4cd2); C = 0x08;  // ld c,$08
  CALL_C(0x4cd2, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x4cd5);
  CYC(0x4cd5, 0x4cd7); A = mem_rd(gb, 0xffaa);  // ldh a,(<hCameraY)
  CYC(0x4cd7, 0x4cd8); B = A;  // ld b,a
  CYC(0x4cd8, 0x4cda); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x4cda, 0x4cdb); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4cdb, 0x4cdc); alu_sub(gb, B);  // sub b
  if (!(F & FC)) { CYCT(0x4cdc, 0x4cde); goto substate1_afterSub; } CYC(0x4cdc, 0x4cde);  // jr nc,+
  CYC(0x4cde, 0x4cdf); A = mem_rd(gb, HL);  // ld a,(hl)
substate1_afterSub:
  CYC(0x4cdf, 0x4ce0); B = A;  // ld b,a
  CYC(0x4ce0, 0x4ce2); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x4ce2, 0x4ce3); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4ce3, 0x4ce5); alu_cp(gb, 0x80);  // cp $80
  if ((F & FC)) { CYCT(0x4ce5, 0x4ce7); goto substate1_setState2; } CYC(0x4ce5, 0x4ce7);  // jr c,++
  CYC(0x4ce7, 0x4ce8); alu_add(gb, B);  // add b
  CYC(0x4ce8, 0x4cea); alu_cp(gb, 0xf0);  // cp $f0
  if ((F & FC)) { CYCT(0x4cea, 0x4cec); goto state10_animate; } CYC(0x4cea, 0x4cec);  // jr c,@animate
substate1_setState2:
  CYC(0x4cec, 0x4cee); L = 0x85;  // ld l,Enemy.substate
  CYC(0x4cee, 0x4cef); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4cef, 0x4cf1); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4cf1, 0x4cf3); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));  // set 1,(hl)
  CYC(0x4cf3, 0x4cf5); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4cf5, 0x4cf7); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));  // res 7,(hl)
  CYC(0x4cf7, 0x4cf9); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x4cf9, 0x4cfb); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x4cfb, 0x4cfe); if (hook_enabled_at(0x1e7b)) { objectSetInvisible_hook(gb); return; } HANDOFF(0x1e7b);  // jp objectSetInvisible
substate2:
  CYC(0x4cfe, 0x4d00); E = 0x82;  // ld e,Enemy.subid
  CYC(0x4d00, 0x4d01); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4d01, 0x4d02); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x4d02); return; } CYC(0x4d02, 0x4d03);  // ret nz
  CYC(0x4d03, 0x4d05); A = 0x32;  // ld a,Object.var32
  CALL_C(0x4d05, objectGetRelatedObject1Var_hook, 0x2160, 0x4d08);
  CYC(0x4d08, 0x4d0a); alu_bit(gb, 1, mem_rd(gb, HL));  // bit 1,(hl)
  if ((F & FZ)) { RET_TAKEN(0x4d0a); return; } CYC(0x4d0a, 0x4d0b);  // ret z
  CYC(0x4d0b, 0x4d0d); L = 0x85;  // ld l,Enemy.substate
  CYC(0x4d0d, 0x4d0e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4d0e, 0x4d0f); H = D;  // ld h,d
  CYC(0x4d0f, 0x4d10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  RET(0x4d10); return;  // ret
substate3:
  CALL_C(0x4d11, ecom_decCounter1_b10_hook, 0x439a, 0x4d14);
  if (!(F & FZ)) { RET_TAKEN(0x4d14); return; } CYC(0x4d14, 0x4d15);  // ret nz
  CYC(0x4d15, 0x4d17); mem_wr(gb, HL, 48);  // ld (hl),48
  CYC(0x4d17, 0x4d18); L = E;  // ld l,e
  CYC(0x4d18, 0x4d19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4d19, 0x4d1b); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x4d1b, 0x4d1d); mem_wr(gb, HL, 0xa0);  // ld (hl),$a0
  CYC(0x4d1d, 0x4d1f); L = 0x89;  // ld l,Enemy.angle
  CYC(0x4d1f, 0x4d21); E = 0x82;  // ld e,Enemy.subid
  CYC(0x4d21, 0x4d22); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4d22, 0x4d23); alu_or(gb, A);  // or a
  CYC(0x4d23, 0x4d25); mem_wr(gb, HL, 0x08);  // ld (hl),$08
  if ((F & FZ)) { CYCT(0x4d25, 0x4d27); goto substate3_visible; } CYC(0x4d25, 0x4d27);  // jr z,+
  CYC(0x4d27, 0x4d29); mem_wr(gb, HL, 0x18);  // ld (hl),$18
substate3_visible:
  CYC(0x4d29, 0x4d2b); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4d2b, 0x4d2d); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(0x4d2d, objectSetVisiblec2_hook, 0x1e45, 0x4d30);
  CYC(0x4d30, 0x4d32); A = 0xd3;  // ld a,SND_WIND
  CALL_C(0x4d32, playSound_b00_hook, 0x0c98, 0x4d35);
substate4:
  CYC(0x4d35, 0x4d38); SET_BC(0x5878);  // ld bc,$5878
  CYC(0x4d38, 0x4d3a); E = 0x86;  // ld e,Enemy.counter1
  CYC(0x4d3a, 0x4d3b); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4d3b, 0x4d3d); E = 0x89;  // ld e,Enemy.angle
  CALL_C(0x4d3d, objectSetPositionInCircleArc_hook, 0x210e, 0x4d40);
  CYC(0x4d40, 0x4d42); E = 0x89;  // ld e,Enemy.angle
  CYC(0x4d42, 0x4d43); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4d43, 0x4d45); alu_add(gb, 0x08);  // add $08
  CYC(0x4d45, 0x4d47); alu_and(gb, 0x1f);  // and $1f
  CALL_C(0x4d47, twinrova_updateMovingAnimationGivenAngle_hook, 0x4ee0, 0x4d4a);
  CYC(0x4d4a, 0x4d4b); H = D;  // ld h,d
  CYC(0x4d4b, 0x4d4d); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x4d4d, 0x4d4e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4d4e, 0x4d4f); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4d4f, 0x4d50); alu_rrca(gb);  // rrca
  if ((F & FC)) { CYCT(0x4d50, 0x4d52); goto state10_animate; } CYC(0x4d50, 0x4d52);  // jr c,@animate
  CYC(0x4d52, 0x4d54); L = 0x89;  // ld l,Enemy.angle
  CYC(0x4d54, 0x4d55); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4d55, 0x4d56); A = alu_inc8(gb, A);  // inc a
  CYC(0x4d56, 0x4d58); alu_and(gb, 0x1f);  // and $1f
  CYC(0x4d58, 0x4d59); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4d59, 0x4d5b); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x4d5b, 0x4d5c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  if (!(F & FZ)) { CYCT(0x4d5c, 0x4d5e); goto state10_animate; } CYC(0x4d5c, 0x4d5e);  // jr nz,@animate
  CYC(0x4d5e, 0x4d5f); L = alu_dec8(gb, L);  // dec l
  CYC(0x4d5f, 0x4d60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
state10_animate:
  CYC(0x4d60, 0x4d63); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818);  // jp enemyAnimate
substate5:
  CYC(0x4d63, 0x4d65); E = 0x82;  // ld e,Enemy.subid
  CYC(0x4d65, 0x4d66); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4d66, 0x4d67); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(0x4d67, 0x4d6a); if (hook_enabled_at(0x2e47)) { enemyDelete_hook(gb); return; } HANDOFF(0x2e47); } CYC(0x4d67, 0x4d6a);  // jp nz,enemyDelete
  CYC(0x4d6a, 0x4d6d); A = mem_rd(gb, 0xcdd5);  // ld a,(wLinkDeathTrigger)
  CYC(0x4d6d, 0x4d6e); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x4d6e); return; } CYC(0x4d6e, 0x4d6f);  // ret nz
  CYC(0x4d6f, 0x4d70); A = alu_inc8(gb, A);  // inc a
  CYC(0x4d70, 0x4d73); mem_wr(gb, 0xcc8a, A);  // ld (wDisabledObjects),a
  CYC(0x4d73, 0x4d76); mem_wr(gb, 0xcbca, A);  // ld (wDisableLinkCollisionsAndMenu),a
  CYC(0x4d76, 0x4d77); H = D;  // ld h,d
  CYC(0x4d77, 0x4d79); L = 0x85;  // ld l,Enemy.substate
  CYC(0x4d79, 0x4d7a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4d7a, 0x4d7c); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x4d7c, 0x4d7d); alu_xor(gb, A);  // xor a
  CYC(0x4d7d, 0x4d7e); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x4d7e, 0x4d7f); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4d7f, 0x4d81); A = 0x0c;  // ld a,$0c
  CALL_C(0x4d81, enemySetAnimation_hook, 0x282b, 0x4d84);
  CYC(0x4d84, 0x4d86); A = 0xc0;  // ld a,SND_TRANSFORM
  CALL_C(0x4d86, playSound_b00_hook, 0x0c98, 0x4d89);
  CYC(0x4d89, 0x4d8b); A = 0x02;  // ld a,$02
  CYC(0x4d8b, 0x4d8e); if (hook_enabled_at(0x3284)) { fadeinFromWhiteWithDelay_hook(gb); return; } HANDOFF(0x3284);  // jp fadeinFromWhiteWithDelay
substate6:
  CYC(0x4d8e, 0x4d91); A = mem_rd(gb, 0xc4ab);  // ld a,(wPaletteThread_mode)
  CYC(0x4d91, 0x4d92); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x4d92); return; } CYC(0x4d92, 0x4d93);  // ret nz
  CYC(0x4d93, 0x4d94); H = D;  // ld h,d
  CYC(0x4d94, 0x4d96); L = 0x85;  // ld l,Enemy.substate
  CYC(0x4d96, 0x4d97); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4d97, 0x4d99); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4d99, 0x4d9b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));  // res 0,(hl)
  CYC(0x4d9b, 0x4d9d); A = 0x01;  // ld a,$01
  CYC(0x4d9d, 0x4da1); mem_wr(gb, 0xcc18, A);  // ld (wLoadedTreeGfxIndex),a
  RET(0x4da0); return;  // ret
substate7:
  CYC(0x4da1, 0x4da2); H = D;  // ld h,d
  CYC(0x4da2, 0x4da4); L = 0x80;  // ld l,Enemy.enabled
  CYC(0x4da4, 0x4da5); H = alu_inc8(gb, H);  // inc h
nextEnemy:
  CYC(0x4da5, 0x4da6); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4da6, 0x4da7); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x4da7, 0x4da9); goto foundFreeSlot; } CYC(0x4da7, 0x4da9);  // jr z,@foundFreeSlot
  CYC(0x4da9, 0x4daa); H = alu_inc8(gb, H);  // inc h
  CYC(0x4daa, 0x4dab); A = H;  // ld a,h
  CYC(0x4dab, 0x4dad); alu_cp(gb, 0xe0);  // cp LAST_ENEMY_INDEX+1
  if ((F & FC)) { CYCT(0x4dad, 0x4daf); goto nextEnemy; } CYC(0x4dad, 0x4daf);  // jr c,@nextEnemy
  RET(0x4daf); return;  // ret
foundFreeSlot:
  CYC(0x4db0, 0x4db1); E = L;  // ld e,l
  CYC(0x4db1, 0x4db2); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4db2, 0x4db3); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x4db3, 0x4db5); mem_wr(gb, HL, 0x01);  // ld (hl),ENEMY_MERGED_TWINROVA
  CALL_C(0x4db5, objectCopyPosition_hook, 0x2242, 0x4db8);
  CYC(0x4db8, 0x4dba); A = 0x01;  // ld a,$01
  CYC(0x4dba, 0x4dbd); mem_wr(gb, 0xcc18, A);  // ld (wLoadedTreeGfxIndex),a
  CYC(0x4dbd, 0x4dc0); if (hook_enabled_at(0x2e47)) { enemyDelete_hook(gb); return; } HANDOFF(0x2e47);  // jp enemyDelete
}

void twinrova_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4dc0, 0x4dc1); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4dc1, 0x4dc3); alu_sub(gb, 0x08);  // sub $08
  CYC(0x4dc3, 0x4dc4); push_effect(gb, 0x4dc4);
  switch (twinrova_jump_table(gb)) {
    case 0x4b18: twinrova_state8_hook(gb); return;
    case 0x4b5e: twinrova_state9_hook(gb); return;
    case 0x4dd6: twinrova_subid1_stateA_hook(gb); return;
    case 0x4ddc: twinrova_subid1_stateB_hook(gb); return;
    case 0x4dec: twinrova_subid1_stateC_hook(gb); return;
    case 0x4c49: twinrova_stateD_hook(gb); return;
    case 0x4c60: twinrova_stateE_hook(gb); return;
    case 0x4c7d: twinrova_stateF_hook(gb); return;
    case 0x4c9f: twinrova_state10_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void twinrova_subid1_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4dd6, 0x4dd8); A = 0x0b;  // ld a,$0b
  CYC(0x4dd8, 0x4dd9); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4dd9, 0x4ddc); twinrova_subid1_updateTargetPosition_hook(gb); return;  // jp twinrova_subid1_updateTargetPosition
}

void twinrova_subid1_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4ddc, twinrova_moveTowardTargetPosition_hook, 0x4f2f, 0x4ddf);
  if (!(F & FC)) { RET_TAKEN(0x4ddf); return; } CYC(0x4ddf, 0x4de0);  // ret nc
  CALL_C(0x4de0, twinrova_subid1_updateTargetPosition_hook, 0x4f11, 0x4de3);
  if (!(F & FZ)) { RET_TAKEN(0x4de3); return; } CYC(0x4de3, 0x4de4);  // ret nz
  CALL_C(0x4de4, ecom_incState_b10_hook, 0x4000, 0x4de7);
  CYC(0x4de7, 0x4de9); L = 0x86;  // ld l,Enemy.counter1
  CYC(0x4de9, 0x4deb); mem_wr(gb, HL, 30);  // ld (hl),30
  RET(0x4deb); return;  // ret
}

void twinrova_subid1_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4dec, ecom_decCounter1_b10_hook, 0x439a, 0x4def);
  if (!(F & FZ)) { CYCT(0x4def, 0x4df2); if (hook_enabled_at(0x2818)) { enemyAnimate_hook(gb); return; } HANDOFF(0x2818); } CYC(0x4def, 0x4df2);  // jp nz,enemyAnimate
  CYC(0x4df2, 0x4df3); L = E;  // ld l,e
  CYC(0x4df3, 0x4df4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CALL_C(0x4df4, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4df7);
  CYC(0x4df7, 0x4df9); alu_and(gb, 0x03);  // and $03
  CYC(0x4df9, 0x4dfb); E = 0xb3;  // ld e,Enemy.var33
  CYC(0x4dfb, 0x4dfc); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4dfc, 0x4dfd); E = alu_inc8(gb, E);  // inc e
  CYC(0x4dfd, 0x4dfe); alu_xor(gb, A);  // xor a
  CYC(0x4dfe, 0x4dff); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4dff, 0x4e02); twinrova_subid1_updateTargetPosition_hook(gb); return;  // jp twinrova_subid1_updateTargetPosition
}

void twinrova_initialize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4e02, 0x4e03); H = D;  // ld h,d
  CYC(0x4e03, 0x4e05); L = 0x83;  // ld l,Enemy.var03
  CYC(0x4e05, 0x4e07); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if ((F & FZ)) { CYCT(0x4e07, 0x4e0a); if (hook_enabled_at(0x4364)) { ecom_setSpeedAndState8_b10_hook(gb); return; } HANDOFF(0x4364); } CYC(0x4e07, 0x4e0a);  // jp z,ecom_setSpeedAndState8
  CYC(0x4e0a, 0x4e0b); alu_xor(gb, A);  // xor a
  CYC(0x4e0b, 0x4e0e); mem_wr(gb, 0xcc8a, A);  // ld (wDisabledObjects),a
  CYC(0x4e0e, 0x4e11); mem_wr(gb, 0xcc02, A);  // ld (wMenuDisabled),a
  CYC(0x4e11, 0x4e13); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x4e13, 0x4e15); mem_wr(gb, HL, 0x56);  // ld (hl),$56
  CYC(0x4e15, 0x4e17); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x4e17, 0x4e19); mem_wr(gb, HL, 0x60);  // ld (hl),$60
  CYC(0x4e19, 0x4e1b); E = 0x82;  // ld e,Enemy.subid
  CYC(0x4e1b, 0x4e1c); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4e1c, 0x4e1d); alu_or(gb, A);  // or a
  CYC(0x4e1d, 0x4e1f); A = 0x02;  // ld a,$02
  if ((F & FZ)) { CYCT(0x4e1f, 0x4e21); goto setAngle; } CYC(0x4e1f, 0x4e21);  // jr z,+
  CYC(0x4e21, 0x4e23); mem_wr(gb, HL, 0x90);  // ld (hl),$90 [xh]
  CYC(0x4e23, 0x4e24); A = alu_dec8(gb, A);  // dec a
setAngle:
  CYC(0x4e24, 0x4e26); L = 0x9b;  // ld l,Enemy.oamFlagsBackup
  CYC(0x4e26, 0x4e27); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ldi (hl),a
  CYC(0x4e27, 0x4e28); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4e28, 0x4e2a); L = 0x84;  // ld l,Enemy.state
  CYC(0x4e2a, 0x4e2c); mem_wr(gb, HL, 0x0a);  // ld (hl),$0a
  CYC(0x4e2c, 0x4e2e); L = 0x88;  // ld l,Enemy.direction
  CYC(0x4e2e, 0x4e30); mem_wr(gb, HL, 0xff);  // ld (hl),$ff
  CYC(0x4e30, 0x4e32); L = 0x90;  // ld l,Enemy.speed
  CYC(0x4e32, 0x4e34); mem_wr(gb, HL, 0x32);  // ld (hl),SPEED_140
  CYC(0x4e34, 0x4e36); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4e36, 0x4e38); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 3)));  // set 3,(hl)
  CYC(0x4e38, 0x4e3a); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));  // set 7,(hl)
  CALL_C(0x4e3a, ecom_updateAngleTowardTarget_b10_hook, 0x43bf, 0x4e3d);
  CALL_C(0x4e3d, twinrova_calculateAnimationFromAngle_hook, 0x4ef7, 0x4e40);
  CYC(0x4e40, 0x4e42); alu_add(gb, 0x04);  // add $04
  CYC(0x4e42, 0x4e43); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4e43, 0x4e46); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void twinrova_updateZPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4e46, 0x4e47); H = D;  // ld h,d
  CYC(0x4e47, 0x4e49); L = 0xb7;  // ld l,Enemy.var37
  CYC(0x4e49, 0x4e4a); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4e4a, 0x4e4b); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x4e4b, 0x4e4d); goto checkBobbing; } CYC(0x4e4b, 0x4e4d);  // jr z,+
  CYC(0x4e4d, 0x4e4e); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
checkBobbing:
  CYC(0x4e4e, 0x4e50); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4e50, 0x4e52); alu_bit(gb, 3, mem_rd(gb, HL));  // bit 3,(hl)
  if ((F & FZ)) { RET_TAKEN(0x4e52); return; } CYC(0x4e52, 0x4e53);  // ret z
  CYC(0x4e53, 0x4e55); L = 0xb1;  // ld l,Enemy.var31
  CYC(0x4e55, 0x4e56); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(0x4e56, 0x4e57); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4e57, 0x4e59); alu_and(gb, 0x07);  // and $07
  if (!(F & FZ)) { RET_TAKEN(0x4e59); return; } CYC(0x4e59, 0x4e5a);  // ret nz
  CYC(0x4e5a, 0x4e5b); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4e5b, 0x4e5d); alu_and(gb, 0x18);  // and $18
  CYC(0x4e5d, 0x4e5f); A = alu_swap(gb, A);  // swap a
  CYC(0x4e5f, 0x4e60); alu_rlca(gb);  // rlca
  CYC(0x4e60, 0x4e63); SET_HL(0x4e69);  // ld hl,@levitationZPositions
  CYC(0x4e63, 0x4e64);
  twinrova_addAToHl_from_rst(gb, 0x4e64);  // rst_addAToHl
  CYC(0x4e64, 0x4e66); E = 0x8f;  // ld e,Enemy.zh
  CYC(0x4e66, 0x4e67); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4e67, 0x4e68); mem_wr(gb, DE, A);  // ld (de),a
  RET(0x4e68); return;  // ret
}

void twinrova_checkFireProjectile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4e6d, 0x4e6e); H = D;  // ld h,d
  CYC(0x4e6e, 0x4e70); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4e70, 0x4e72); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  if ((F & FZ)) { RET_TAKEN(0x4e72); return; } CYC(0x4e72, 0x4e73);  // ret z
  CYC(0x4e73, 0x4e75); alu_bit(gb, 2, mem_rd(gb, HL));  // bit 2,(hl)
  if (!(F & FZ)) { CYCT(0x4e75, 0x4e77); goto fireProjectile; } CYC(0x4e75, 0x4e77);  // jr nz,@fireProjectile
  CYC(0x4e77, 0x4e79); L = 0xa4;  // ld l,Enemy.collisionType
  CYC(0x4e79, 0x4e7b); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if ((F & FZ)) { RET_TAKEN(0x4e7b); return; } CYC(0x4e7b, 0x4e7c);  // ret z
  CYC(0x4e7c, 0x4e7e); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x4e7e, 0x4e7f); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4e7f, 0x4e80); alu_or(gb, A);  // or a
  CYC(0x4e80, 0x4e82); E = 0xa1;  // ld e,Enemy.animParameter
  if ((F & FZ)) { CYCT(0x4e82, 0x4e84); goto var39Zero; } CYC(0x4e82, 0x4e84);  // jr z,@var39Zero
  CYC(0x4e84, 0x4e85); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  CYC(0x4e85, 0x4e86); A = mem_rd(gb, DE);  // ld a,(de) [animParameter]
  CYC(0x4e86, 0x4e87); A = alu_inc8(gb, A);  // inc a
  if (!(F & FZ)) { RET_TAKEN(0x4e87); return; } CYC(0x4e87, 0x4e88);  // ret nz
var39Zero:
  CYC(0x4e88, 0x4e89); A = alu_dec8(gb, A);  // dec a
  CYC(0x4e89, 0x4e8a); mem_wr(gb, DE, A);  // ld (de),a [animParameter] = $ff
  CYC(0x4e8a, 0x4e8c); E = 0x89;  // ld e,Enemy.angle
  CYC(0x4e8c, 0x4e8d); A = mem_rd(gb, DE);  // ld a,(de)
  CALL_C(0x4e8d, twinrova_calculateAnimationFromAngle_hook, 0x4ef7, 0x4e90);
  CYC(0x4e90, 0x4e91); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4e91, 0x4e93); alu_add(gb, 0x04);  // add $04
  CYC(0x4e93, 0x4e96); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
fireProjectile:
  CYC(0x4e96, 0x4e98); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 2)));  // res 2,(hl) [var32]
  CYC(0x4e98, 0x4e9a); L = 0x88;  // ld l,Enemy.direction
  CYC(0x4e9a, 0x4e9c); mem_wr(gb, HL, 0xff);  // ld (hl),$ff
  CYC(0x4e9c, 0x4e9e); L = 0xb9;  // ld l,Enemy.var39
  CYC(0x4e9e, 0x4ea0); mem_wr(gb, HL, 240);  // ld (hl),240
  CYC(0x4ea0, 0x4ea3); push_effect(gb, 0x4ea3); twinrova_checkFireProjectile__spawnProjectile_hook(gb);  // call @spawnProjectile
  CALL_C(0x4ea3, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x4ea6);
  CYC(0x4ea6, 0x4ea8); alu_cp(gb, 0x10);  // cp $10
  CYC(0x4ea8, 0x4eaa); A = 0x00;  // ld a,$00
  if ((F & FC)) { CYCT(0x4eaa, 0x4eac); goto fireProjectile_addAnim; } CYC(0x4eaa, 0x4eac);  // jr c,+
  CYC(0x4eac, 0x4ead); A = alu_inc8(gb, A);  // inc a
fireProjectile_addAnim:
  CYC(0x4ead, 0x4eaf); alu_add(gb, 0x08);  // add $08
  CYC(0x4eaf, 0x4eb2); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

// 10:4eb2 (real call from twinrova_checkFireProjectile's @fireProjectile block, whose own
// tail-jp into ecom_spawnProjectile_b10_hook eventually rets back through this C call chain
// to the code right after the call there, so this stays a plain non-hooked helper).
void twinrova_checkFireProjectile__spawnProjectile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4eb2, 0x4eb4); B = 0x4b;  // ld b,PART_RED_TWINROVA_PROJECTILE
  CYC(0x4eb4, 0x4eb6); E = 0x82;  // ld e,Enemy.subid
  CYC(0x4eb6, 0x4eb7); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4eb7, 0x4eb8); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x4eb8, 0x4eba); goto spawn; } CYC(0x4eb8, 0x4eba);  // jr z,+
  CYC(0x4eba, 0x4ebc); B = 0x4d;  // ld b,PART_BLUE_TWINROVA_PROJECTILE
spawn:
  CYC(0x4ebc, 0x4ebf); if (hook_enabled_at(0x437c)) { ecom_spawnProjectile_b10_hook(gb); return; } HANDOFF(0x437c);  // jp ecom_spawnProjectile
}

void twinrova_setTargetPositionToObject_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4ebf, 0x4ec1); L = 0x8b;  // ld l,Enemy.yh
  CYC(0x4ec1, 0x4ec2); E = L;  // ld e,l
  CYC(0x4ec2, 0x4ec3); B = mem_rd(gb, HL);  // ld b,(hl)
  CYC(0x4ec3, 0x4ec4); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4ec4, 0x4ec6); mem_wr(gb, 0xff8f, A);  // ldh (<hFF8F),a
  CYC(0x4ec6, 0x4ec8); L = 0x8d;  // ld l,Enemy.xh
  CYC(0x4ec8, 0x4ec9); E = L;  // ld e,l
  CYC(0x4ec9, 0x4eca); C = mem_rd(gb, HL);  // ld c,(hl)
  CYC(0x4eca, 0x4ecb); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4ecb, 0x4ecd); mem_wr(gb, 0xff8e, A);  // ldh (<hFF8E),a
  CALL_C(0x4ecd, ecom_moveTowardPosition_b10_hook, 0x4430, 0x4ed0);
  CYC(0x4ed0, 0x4ed2); twinrova_updateMovingAnimation_hook(gb); return;  // jr twinrova_updateMovingAnimation
}

void twinrova_updateAnimationFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4ed2, 0x4ed4); E = 0x89;  // ld e,Enemy.angle
  CYC(0x4ed4, 0x4ed5); A = mem_rd(gb, DE);  // ld a,(de)
  CALL_C(0x4ed5, twinrova_calculateAnimationFromAngle_hook, 0x4ef7, 0x4ed8);
  if ((F & FZ)) { RET_TAKEN(0x4ed8); return; } CYC(0x4ed8, 0x4ed9);  // ret z
  CYC(0x4ed9, 0x4eda); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4eda, 0x4edd); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void twinrova_updateMovingAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4edd, 0x4edf); E = 0x89;  // ld e,Enemy.angle
  CYC(0x4edf, 0x4ee0); A = mem_rd(gb, DE);  // ld a,(de)
  twinrova_updateMovingAnimationGivenAngle_hook(gb); return;  // fallthrough
}

void twinrova_updateMovingAnimationGivenAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4ee0, twinrova_calculateAnimationFromAngle_hook, 0x4ef7, 0x4ee3);
  if ((F & FZ)) { RET_TAKEN(0x4ee3); return; } CYC(0x4ee3, 0x4ee4);  // ret z
  CYC(0x4ee4, 0x4ee6); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if (!(F & FZ)) { RET_TAKEN(0x4ee6); return; } CYC(0x4ee6, 0x4ee7);  // ret nz
  CYC(0x4ee7, 0x4ee8); B = A;  // ld b,a
  CYC(0x4ee8, 0x4eea); E = 0xb7;  // ld e,Enemy.var37
  CYC(0x4eea, 0x4eeb); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4eeb, 0x4eec); alu_or(gb, A);  // or a
  if (!(F & FZ)) { RET_TAKEN(0x4eec); return; } CYC(0x4eec, 0x4eed);  // ret nz
  CYC(0x4eed, 0x4eef); A = 30;  // ld a,30
  CYC(0x4eef, 0x4ef0); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4ef0, 0x4ef1); A = B;  // ld a,b
  CYC(0x4ef1, 0x4ef2); mem_wr(gb, HL, A);  // ld (hl),a [direction]
  CYC(0x4ef2, 0x4ef4); alu_add(gb, 0x04);  // add $04
  CYC(0x4ef4, 0x4ef7); if (hook_enabled_at(0x282b)) { enemySetAnimation_hook(gb); return; } HANDOFF(0x282b);  // jp enemySetAnimation
}

void twinrova_calculateAnimationFromAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4ef7, 0x4ef8); C = A;  // ld c,a
  CYC(0x4ef8, 0x4efa); alu_add(gb, 0x04);  // add $04
  CYC(0x4efa, 0x4efc); alu_and(gb, 0x18);  // and $18
  CYC(0x4efc, 0x4efe); A = alu_swap(gb, A);  // swap a
  CYC(0x4efe, 0x4eff); alu_rlca(gb);  // rlca
  CYC(0x4eff, 0x4f00); B = A;  // ld b,a
  CYC(0x4f00, 0x4f01); H = D;  // ld h,d
  CYC(0x4f01, 0x4f03); L = 0x88;  // ld l,Enemy.direction
  CYC(0x4f03, 0x4f04); A = C;  // ld a,c
  CYC(0x4f04, 0x4f06); alu_and(gb, 0x07);  // and $07
  CYC(0x4f06, 0x4f08); alu_cp(gb, 0x04);  // cp $04
  CYC(0x4f08, 0x4f09); A = B;  // ld a,b
  if ((F & FZ)) { RET_TAKEN(0x4f09); return; } CYC(0x4f09, 0x4f0a);  // ret z
  CYC(0x4f0a, 0x4f0c); alu_cp(gb, mem_rd(gb, HL));  // cp (hl)
  RET(0x4f0c); return;  // ret
}

void twinrova_subid0_updateTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f0c, 0x4f0f); SET_HL(0x4f95);  // ld hl,twinrova_subid0_targetPositions
  CYCT(0x4f0f, 0x4f11); goto shared;  // jr ++
shared:
  CYC(0x4f14, 0x4f16); E = 0xb7;  // ld e,Enemy.var37
  CYC(0x4f16, 0x4f17); alu_xor(gb, A);  // xor a
  CYC(0x4f17, 0x4f18); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4f18, 0x4f1a); E = 0xb4;  // ld e,Enemy.var34
  CYC(0x4f1a, 0x4f1b); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4f1b, 0x4f1c); B = A;  // ld b,a
  CYC(0x4f1c, 0x4f1d); A = alu_inc8(gb, A);  // inc a
  CYC(0x4f1d, 0x4f1e); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4f1e, 0x4f1f); E = alu_dec8(gb, E);  // dec e
  CYC(0x4f1f, 0x4f20); A = mem_rd(gb, DE);  // ld a,(de) [var33]
  CYC(0x4f20, 0x4f21);
  twinrova_addDoubleIndexToHl_from_rst(gb, 0x4f21);  // rst_addDoubleIndexToHl
  CYC(0x4f21, 0x4f22); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x4f22, 0x4f23); H = mem_rd(gb, HL);  // ld h,(hl)
  CYC(0x4f23, 0x4f24); L = A;  // ld l,a
  CYC(0x4f24, 0x4f25); A = B;  // ld a,b
  CYC(0x4f25, 0x4f26);
  twinrova_addDoubleIndexToHl_from_rst(gb, 0x4f26);  // rst_addDoubleIndexToHl
  CYC(0x4f26, 0x4f28); E = 0xb5;  // ld e,Enemy.var35
  CYC(0x4f28, 0x4f29); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x4f29, 0x4f2a); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(0x4f2a); return; } CYC(0x4f2a, 0x4f2b);  // ret z
  CYC(0x4f2b, 0x4f2c); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4f2c, 0x4f2d); E = alu_inc8(gb, E);  // inc e
  CYC(0x4f2d, 0x4f2e); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4f2e, 0x4f2f); mem_wr(gb, DE, A);  // ld (de),a
  twinrova_moveTowardTargetPosition_hook(gb); return;  // fallthrough
}

void twinrova_subid1_updateTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f11, 0x4f14); SET_HL(0x4ff9);  // ld hl,twinrova_subid1_targetPositions
  CYC(0x4f14, 0x4f16); E = 0xb7;  // ld e,Enemy.var37
  CYC(0x4f16, 0x4f17); alu_xor(gb, A);  // xor a
  CYC(0x4f17, 0x4f18); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4f18, 0x4f1a); E = 0xb4;  // ld e,Enemy.var34
  CYC(0x4f1a, 0x4f1b); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x4f1b, 0x4f1c); B = A;  // ld b,a
  CYC(0x4f1c, 0x4f1d); A = alu_inc8(gb, A);  // inc a
  CYC(0x4f1d, 0x4f1e); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4f1e, 0x4f1f); E = alu_dec8(gb, E);  // dec e
  CYC(0x4f1f, 0x4f20); A = mem_rd(gb, DE);  // ld a,(de) [var33]
  CYC(0x4f20, 0x4f21);
  twinrova_addDoubleIndexToHl_from_rst(gb, 0x4f21);  // rst_addDoubleIndexToHl
  CYC(0x4f21, 0x4f22); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x4f22, 0x4f23); H = mem_rd(gb, HL);  // ld h,(hl)
  CYC(0x4f23, 0x4f24); L = A;  // ld l,a
  CYC(0x4f24, 0x4f25); A = B;  // ld a,b
  CYC(0x4f25, 0x4f26);
  twinrova_addDoubleIndexToHl_from_rst(gb, 0x4f26);  // rst_addDoubleIndexToHl
  CYC(0x4f26, 0x4f28); E = 0xb5;  // ld e,Enemy.var35
  CYC(0x4f28, 0x4f29); A = mem_rd(gb, HL); SET_HL(HL + 1);  // ld a,(hl+)
  CYC(0x4f29, 0x4f2a); alu_or(gb, A);  // or a
  if ((F & FZ)) { RET_TAKEN(0x4f2a); return; } CYC(0x4f2a, 0x4f2b);  // ret z
  CYC(0x4f2b, 0x4f2c); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x4f2c, 0x4f2d); E = alu_inc8(gb, E);  // inc e
  CYC(0x4f2d, 0x4f2e); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4f2e, 0x4f2f); mem_wr(gb, DE, A);  // ld (de),a
  twinrova_moveTowardTargetPosition_hook(gb); return;  // fallthrough
}

void twinrova_moveTowardTargetPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f2f, 0x4f30); H = D;  // ld h,d
  CYC(0x4f30, 0x4f32); L = 0xb5;  // ld l,Enemy.var35
  CALL_C(0x4f32, ecom_readPositionVars_b10_hook, 0x4439, 0x4f35);
  CYC(0x4f35, 0x4f36); alu_sub(gb, C);  // sub c
  CYC(0x4f36, 0x4f37); A = alu_inc8(gb, A);  // inc a
  CYC(0x4f37, 0x4f39); alu_cp(gb, 0x03);  // cp $03
  if (!(F & FC)) { CYCT(0x4f39, 0x4f3b); goto moveToward; } CYC(0x4f39, 0x4f3b);  // jr nc,@moveToward
  CYC(0x4f3b, 0x4f3d); A = mem_rd(gb, 0xff8f);  // ldh a,(<hFF8F)
  CYC(0x4f3d, 0x4f3e); alu_sub(gb, B);  // sub b
  CYC(0x4f3e, 0x4f3f); A = alu_inc8(gb, A);  // inc a
  CYC(0x4f3f, 0x4f41); alu_cp(gb, 0x03);  // cp $03
  if ((F & FC)) { RET_TAKEN(0x4f41); return; } CYC(0x4f41, 0x4f42);  // ret c
moveToward:
  CALL_C(0x4f42, ecom_moveTowardPosition_b10_hook, 0x4430, 0x4f45);
  CALL_C(0x4f45, twinrova_updateMovingAnimation_hook, 0x4edd, 0x4f48);
  CALL_C(0x4f48, enemyAnimate_hook, 0x2818, 0x4f4b);
  CYC(0x4f4b, 0x4f4c); alu_or(gb, D);  // or d
  RET(0x4f4c); return;  // ret
}

void twinrova_chooseObjectToAttack_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4f4d, getRandomNumber_noPreserveVars_hook, 0x0453, 0x4f50);
  CYC(0x4f50, 0x4f51); alu_rrca(gb);  // rrca
  CYC(0x4f51, 0x4f52); H = D;  // ld h,d
  CYC(0x4f52, 0x4f54); L = 0xb2;  // ld l,Enemy.var32
  if (!(F & FC)) { CYCT(0x4f54, 0x4f56); goto orFlag; } CYC(0x4f54, 0x4f56);  // jr nc,++
  CYC(0x4f56, 0x4f58); A = 0x32;  // ld a,Object.var32
  CALL_C(0x4f58, objectGetRelatedObject1Var_hook, 0x2160, 0x4f5b);
orFlag:
  CYC(0x4f5b, 0x4f5c); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4f5c, 0x4f5e); alu_or(gb, 0x05);  // or $05
  CYC(0x4f5e, 0x4f5f); mem_wr(gb, HL, A);  // ld (hl),a
  RET(0x4f5f); return;  // ret
}

void twinrova_checkAttackInProgress_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f60, 0x4f61); H = D;  // ld h,d
  CYC(0x4f61, 0x4f63); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4f63, 0x4f65); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  if (!(F & FZ)) { CYCT(0x4f65, 0x4f67); goto checkDirection; } CYC(0x4f65, 0x4f67);  // jr nz,++
  CYC(0x4f67, 0x4f69); A = 0x32;  // ld a,Object.var32
  CALL_C(0x4f69, objectGetRelatedObject1Var_hook, 0x2160, 0x4f6c);
  CYC(0x4f6c, 0x4f6e); alu_bit(gb, 0, mem_rd(gb, HL));  // bit 0,(hl)
  if ((F & FZ)) { RET_TAKEN(0x4f6e); return; } CYC(0x4f6e, 0x4f6f);  // ret z
checkDirection:
  CYC(0x4f6f, 0x4f71); L = 0x88;  // ld l,Enemy.direction
  CYC(0x4f71, 0x4f73); alu_bit(gb, 7, mem_rd(gb, HL));  // bit 7,(hl)
  if (!(F & FZ)) { RET_TAKEN(0x4f73); return; } CYC(0x4f73, 0x4f74);  // ret nz
  CYC(0x4f74, 0x4f76); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4f76, 0x4f78); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));  // res 0,(hl)
  CYC(0x4f78, 0x4f79); alu_or(gb, D);  // or d
  RET(0x4f79); return;  // ret
}

void twinrova_rise2PixelsAboveGround_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f7a, 0x4f7b); H = D;  // ld h,d
  CYC(0x4f7b, 0x4f7d); L = 0x8f;  // ld l,Enemy.zh
  CYC(0x4f7d, 0x4f7e); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4f7e, 0x4f80); alu_cp(gb, 0xfe);  // cp $fe
  if ((F & FC)) { CYCT(0x4f80, 0x4f82); goto belowThreshold; } CYC(0x4f80, 0x4f82);  // jr c,++
  CYC(0x4f82, 0x4f83); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  RET(0x4f83); return;  // ret
belowThreshold:
  CYC(0x4f84, 0x4f86); L = 0xb2;  // ld l,Enemy.var32
  CYC(0x4f86, 0x4f87); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x4f87, 0x4f89); alu_or(gb, 0x18);  // or $18
  CYC(0x4f89, 0x4f8a); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x4f8a, 0x4f8b); alu_xor(gb, A);  // xor a
  RET(0x4f8b); return;  // ret
}

void twinrova_incState2ForSelfAndTwin_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4f8c, 0x4f8e); A = 0x05;  // ld a,Object.substate
  CALL_C(0x4f8e, objectGetRelatedObject1Var_hook, 0x2160, 0x4f91);
  CYC(0x4f91, 0x4f92); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  CYC(0x4f92, 0x4f93); H = D;  // ld h,d
  CYC(0x4f93, 0x4f94); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));  // inc (hl)
  RET(0x4f94); return;  // ret
}
