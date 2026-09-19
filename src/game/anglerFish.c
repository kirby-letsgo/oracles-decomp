#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

void ecom_killRelatedObj1_b0f_hook(GB *gb);
void ecom_getSubidAndCpStateTo08_b0f_hook(GB *gb);
void ecom_incState_b0f_hook(GB *gb);
void ecom_decCounter1_b0f_hook(GB *gb);
void ecom_decCounter2_b0f_hook(GB *gb);
void ecom_bounceOffWallsAndHoles_b0f_hook(GB *gb);
void ecom_flickerVisibility_b0f_hook(GB *gb);
void ecom_setSpeedAndState8_b0f_hook(GB *gb);
void ecom_spawnUncountedEnemyWithSubid01_b0f_hook(GB *gb);

void enemyBoss_dead_b0f_hook(GB *gb);
void enemyBoss_initializeRoom_b0f_hook(GB *gb);
void enemyBoss_beginMiniboss_b0f_hook(GB *gb);

void enemyCode76_hook(GB *gb);
void anglerFish_state_uninitialized_hook(GB *gb);
void anglerFish_state_stub_hook(GB *gb);
void anglerFish_main_hook(GB *gb);
void anglerFish_main_state8_hook(GB *gb);
void anglerFish_main_state9_hook(GB *gb);
void anglerFish_main_stateA_hook(GB *gb);
void anglerFish_bounceOffGround_hook(GB *gb);
void anglerFish_main_stateB_hook(GB *gb);
void anglerFish_updatePosition_hook(GB *gb);
void anglerFish_applySpeed_hook(GB *gb);
void anglerFish_updateAnimation_hook(GB *gb);
void anglerFish_main_stateC_hook(GB *gb);
void anglerFish_main_stateD_hook(GB *gb);
void anglerFish_main_stateE_hook(GB *gb);
void anglerFish_main_stateF_hook(GB *gb);
void anglerFish_antenna_hook(GB *gb);
void anglerFish_main_checkFireProjectile_hook(GB *gb);

static uint16_t anglerFish_jump_table(GB *gb) {
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

static void anglerFish_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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
// ENEMY_ANGLER_FISH
// ==================================================================================================
void enemyCode76_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5b72, 0x5b74); goto normalStatus; } // jr z
  CYC(0x5b72, 0x5b74);
  CYC(0x5b74, 0x5b76); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5b76); return; } // ret c
  CYC(0x5b76, 0x5b77);
  if (!(F & FZ)) { CYCT(0x5b77, 0x5b79); goto justHit; } // jr nz
  CYC(0x5b77, 0x5b79);
  CYC(0x5b79, 0x5b7b); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5b7b, 0x5b7c); A = mem_rd(gb, DE);
  CYC(0x5b7c, 0x5b7d); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5b7d, 0x5b80); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(0x5b7d, 0x5b80);
  CALL_C(0x5b80, ecom_killRelatedObj1_b0f_hook, 0x446d, 0x5b83);
  CYC(0x5b83, 0x5b86); enemyDelete_hook(gb); return; // jp

justHit:
  CYC(0x5b86, 0x5b88); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5b88, 0x5b89); A = mem_rd(gb, DE);
  CYC(0x5b89, 0x5b8a); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5b8a, 0x5b8c); goto fishHit; } // jr z
  CYC(0x5b8a, 0x5b8c);

antennaHit:
  CYC(0x5b8c, 0x5b8e); A = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CALL_C(0x5b8e, objectGetRelatedObject1Var_hook, 0x2160, 0x5b91);
  CYC(0x5b91, 0x5b92); E = L;
  CYC(0x5b92, 0x5b93); A = mem_rd(gb, DE);
  CYC(0x5b93, 0x5b94); mem_wr(gb, HL, A);
  CYCT(0x5b94, 0x5b96); goto normalStatus; // jr

fishHit:
  CYC(0x5b96, 0x5b98); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x5b98, 0x5b99); A = mem_rd(gb, DE);
  CYC(0x5b99, 0x5b9b); alu_cp(gb, 0x9c); // $80|ITEMCOLLISION_SCENT_SEED
  if (!(F & FZ)) { CYCT(0x5b9b, 0x5b9d); goto normalStatus; } // jr nz
  CYC(0x5b9b, 0x5b9d);
  CYC(0x5b9d, 0x5b9e); H = D;
  CYC(0x5b9e, 0x5ba0); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5ba0, 0x5ba2); mem_wr(gb, HL, 0x0d);
  CYC(0x5ba2, 0x5ba4); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5ba4, 0x5ba6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x5ba6, 0x5ba8); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x5ba8, 0x5ba9); A = mem_rd(gb, DE);
  CYC(0x5ba9, 0x5bab); alu_and(gb, 0x01);
  CYC(0x5bab, 0x5bad); alu_add(gb, 0x04);
  CYC(0x5bad, 0x5bae); mem_wr(gb, DE, A);
  CALL_C(0x5bae, enemySetAnimation_hook, 0x282b, 0x5bb1);
  CYC(0x5bb1, 0x5bb3); B = 0x56; // INTERAC_EXPLOSION
  CALL_C(0x5bb3, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x5bb6);

normalStatus:
  CALL_C(0x5bb6, ecom_getSubidAndCpStateTo08_b0f_hook, 0x4426, 0x5bb9);
  if (F & FC) { CYCT(0x5bb9, 0x5bbb); goto commonState; } // jr c
  CYC(0x5bb9, 0x5bbb);
  CYC(0x5bbb, 0x5bbc); A = B;
  CYC(0x5bbc, 0x5bbd); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5bbd, 0x5bc0); anglerFish_main_hook(gb); return; } // jp z
  CYC(0x5bbd, 0x5bc0);
  CYC(0x5bc0, 0x5bc3); anglerFish_antenna_hook(gb); return; // jp

commonState:
  CYC(0x5bc3, 0x5bc4); push_effect(gb, 0x5bc4);
  {
    uint16_t target = anglerFish_jump_table(gb);
    if (target == 0x5bd4) { anglerFish_state_uninitialized_hook(gb); return; }
    if (target == 0x5c08) { anglerFish_state_stub_hook(gb); return; }
    HANDOFF(target);
  }
}

void anglerFish_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5bd4, 0x5bd6); A = 0xff;
  CYC(0x5bd6, 0x5bd8); B = 0x00;
  CALL_C(0x5bd8, enemyBoss_initializeRoom_b0f_hook, 0x4546, 0x5bdb);
  CYC(0x5bdb, 0x5bdd); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5bdd, 0x5bde); A = mem_rd(gb, DE);
  CYC(0x5bde, 0x5be0); alu_bit(gb, 7, A);
  CYC(0x5be0, 0x5be2); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(0x5be2, 0x5be3); mem_wr(gb, DE, A);
  if (!(F & FZ)) { CYCT(0x5be3, 0x5be5); goto doneInit; } // jr nz
  CYC(0x5be3, 0x5be5);
  CYC(0x5be5, 0x5be6); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x5be6, 0x5be8); goto doneInit; } // jr z
  CYC(0x5be6, 0x5be8);
  CYC(0x5be8, 0x5bea); B = 0x76; // ENEMY_ANGLER_FISH
  CALL_C(0x5bea, ecom_spawnUncountedEnemyWithSubid01_b0f_hook, 0x436d, 0x5bed);
  if (!(F & FZ)) { RET_TAKEN(0x5bed); return; } // ret nz
  CYC(0x5bed, 0x5bee);
  CYC(0x5bee, 0x5bf0); E = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x5bf0, 0x5bf1); L = E;
  CYC(0x5bf1, 0x5bf3); A = ENEMY_BASE; // Enemy.start
  CYC(0x5bf3, 0x5bf4); mem_wr(gb, DE, A);
  CYC(0x5bf4, 0x5bf5); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5bf5, 0x5bf6); E = alu_inc8(gb, E);
  CYC(0x5bf6, 0x5bf7); mem_wr(gb, HL, D);
  CYC(0x5bf7, 0x5bf8); A = H;
  CYC(0x5bf8, 0x5bf9); mem_wr(gb, DE, A);
  CYC(0x5bf9, 0x5bfa); A = H;
  CYC(0x5bfa, 0x5bfb); alu_cp(gb, D);
  if (!(F & FC)) { CYCT(0x5bfb, 0x5bfd); goto doneInit; } // jr nc
  CYC(0x5bfb, 0x5bfd);
  CYC(0x5bfd, 0x5bff); L = ENEMY_BASE + OBJ_SUBID;
  CYC(0x5bff, 0x5c01); mem_wr(gb, HL, 0x80);
  CYC(0x5c01, 0x5c02); E = L;
  CYC(0x5c02, 0x5c04); A = 0x01;
  CYC(0x5c04, 0x5c05); mem_wr(gb, DE, A);

doneInit:
  CYC(0x5c05, 0x5c08); ecom_setSpeedAndState8_b0f_hook(gb); return; // jp
}

void anglerFish_state_stub_hook(GB *gb) {
  RET(0x5c08); return;
}

void anglerFish_main_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5c09, 0x5c0a); A = mem_rd(gb, DE);
  CYC(0x5c0a, 0x5c0c); alu_sub(gb, 0x08);
  CYC(0x5c0c, 0x5c0d); push_effect(gb, 0x5c0d);
  {
    uint16_t target = anglerFish_jump_table(gb);
    if (target == 0x5c1d) { anglerFish_main_state8_hook(gb); return; }
    if (target == 0x5c49) { anglerFish_main_state9_hook(gb); return; }
    if (target == 0x5c5a) { anglerFish_main_stateA_hook(gb); return; }
    if (target == 0x5c83) { anglerFish_main_stateB_hook(gb); return; }
    if (target == 0x5cc1) { anglerFish_main_stateC_hook(gb); return; }
    if (target == 0x5cef) { anglerFish_main_stateD_hook(gb); return; }
    if (target == 0x5cfd) { anglerFish_main_stateE_hook(gb); return; }
    if (target == 0x5d0d) { anglerFish_main_stateF_hook(gb); return; }
    HANDOFF(target);
  }
}

// Waiting for Link to enter
void anglerFish_main_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5c1d, 0x5c20); A = mem_rd(gb, 0xd004); // w1Link.state
  CYC(0x5c20, 0x5c22); alu_cp(gb, 0x0b); // LINK_STATE_FORCE_MOVEMENT
  if (F & FZ) { RET_TAKEN(0x5c22); return; } // ret z
  CYC(0x5c22, 0x5c23);
  CALL_C(0x5c23, checkLinkVulnerable_hook, 0x1d28, 0x5c26);
  if (!(F & FC)) { RET_TAKEN(0x5c26); return; } // ret nc
  CYC(0x5c26, 0x5c27);
  CYC(0x5c27, 0x5c29); A = 0x01; // DISABLE_LINK
  CYC(0x5c29, 0x5c2c); mem_wr(gb, wDisabledObjects, A);
  CYC(0x5c2c, 0x5c2f); mem_wr(gb, wMenuDisabled, A);
  CYC(0x5c2f, 0x5c31); A = 0x42;
  CYC(0x5c31, 0x5c33); C = 0x80;
  CALL_C(0x5c33, setTile_hook, 0x3a9c, 0x5c36);
  CYC(0x5c36, 0x5c38); A = 0x52;
  CYC(0x5c38, 0x5c3a); C = 0x90;
  CALL_C(0x5c3a, setTile_hook, 0x3a9c, 0x5c3d);
  CALL_C(0x5c3d, ecom_incState_b0f_hook, 0x4000, 0x5c40);
  CYC(0x5c40, 0x5c42); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5c42, 0x5c44); mem_wr(gb, HL, 0x1e);
  CYC(0x5c44, 0x5c46); A = 0x70; // SND_DOORCLOSE
  CYC(0x5c46, 0x5c49); playSound_b00_hook(gb); return; // jp
}

// Delay before starting fight
void anglerFish_main_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c49, ecom_decCounter1_b0f_hook, 0x439a, 0x5c4c);
  if (!(F & FZ)) { RET_TAKEN(0x5c4c); return; } // ret nz
  CYC(0x5c4c, 0x5c4d);
  CYC(0x5c4d, 0x5c4e); L = E;
  CYC(0x5c4e, 0x5c4f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5c4f, 0x5c51); L = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5c51, 0x5c53); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(0x5c53, 0x5c55); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5c55, 0x5c57); mem_wr(gb, HL, 0x1e); // SPEED_c0
  CYC(0x5c57, 0x5c5a); objectSetVisible82_hook(gb); return; // jp
}

// Falling to the ground, then the fight will begin
void anglerFish_main_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5c5a, 0x5c5c); B = 0x0c;
  CYC(0x5c5c, 0x5c5e); A = 0x10;
  CALL_C(0x5c5e, objectUpdateSpeedZ_sidescroll_givenYOffset_hook, 0x1f68, 0x5c61);
  if (F & FC) { CYCT(0x5c61, 0x5c63); goto hitGround; } // jr c
  CYC(0x5c61, 0x5c63);
  CYC(0x5c63, 0x5c65); L = ENEMY_BASE + 0x15; // Enemy.speedZ+1
  CYC(0x5c65, 0x5c66); A = mem_rd(gb, HL);
  CYC(0x5c66, 0x5c68); alu_cp(gb, 0x02);
  if (F & FC) { RET_TAKEN(0x5c68); return; } // ret c
  CYC(0x5c68, 0x5c69);
  CYC(0x5c69, 0x5c6b); mem_wr(gb, HL, 0x02);
  RET(0x5c6b); return;

hitGround:
  CALL_C(0x5c6c, enemyBoss_beginMiniboss_b0f_hook, 0x4580, 0x5c6f);
  CALL_C(0x5c6f, ecom_incState_b0f_hook, 0x4000, 0x5c72);
  CYC(0x5c72, 0x5c74); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(0x5c74, 0x5c76); mem_wr(gb, HL, 0xb4); // 180
  anglerFish_bounceOffGround_hook(gb); return; // fallthrough
}

void anglerFish_bounceOffGround_hook(GB *gb) {
  CYC(0x5c76, 0x5c77); H = D;
  CYC(0x5c77, 0x5c79); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x5c79, 0x5c7b); A = 0xe0; // -$320 low byte
  CYC(0x5c7b, 0x5c7c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5c7c, 0x5c7e); mem_wr(gb, HL, 0xfc); // -$320 high byte
  CYC(0x5c7e, 0x5c80); A = 0x98; // SND_POOF
  CYC(0x5c80, 0x5c83); playSound_b00_hook(gb); return; // jp
}

// Bouncing around normally
void anglerFish_main_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5c83, ecom_decCounter2_b0f_hook, 0x43a3, 0x5c86);
  if (F & FZ) { CALL_C_CC(0x5c86, anglerFish_main_checkFireProjectile_hook, 0x5d80, 0x5c89); } else { CYC(0x5c86, 0x5c89); } // call z
  anglerFish_updatePosition_hook(gb); return; // fallthrough
}

void anglerFish_updatePosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5c89, 0x5c8b); B = 0x0c;
  CYC(0x5c8b, 0x5c8d); A = 0x10;
  CALL_C(0x5c8d, objectUpdateSpeedZ_sidescroll_givenYOffset_hook, 0x1f68, 0x5c90);
  if (!(F & FC)) { CYCT(0x5c90, 0x5c92); anglerFish_applySpeed_hook(gb); return; } // jr nc
  CYC(0x5c90, 0x5c92);
  CALL_C(0x5c92, anglerFish_bounceOffGround_hook, 0x5c76, 0x5c95);
  CALL_C(0x5c95, getRandomNumber_noPreserveVars_hook, 0x0453, 0x5c98);
  CYC(0x5c98, 0x5c9a); alu_and(gb, 0x10);
  CYC(0x5c9a, 0x5c9c); alu_add(gb, 0x08);
  CYC(0x5c9c, 0x5c9e); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5c9e, 0x5c9f); mem_wr(gb, DE, A);
  CYC(0x5c9f, 0x5ca1); alu_and(gb, 0x10);
  CYC(0x5ca1, 0x5ca3); alu_xor(gb, 0x10);
  CYC(0x5ca3, 0x5ca5); A = alu_swap(gb, A);
  CYC(0x5ca5, 0x5ca6); B = A;
  CYC(0x5ca6, 0x5ca8); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x5ca8, 0x5ca9); A = mem_rd(gb, DE);
  CYC(0x5ca9, 0x5cab); alu_and(gb, 0x01);
  CYC(0x5cab, 0x5cac); alu_cp(gb, B);
  if (!(F & FZ)) { CALL_C_CC(0x5cac, anglerFish_updateAnimation_hook, 0x5cb8, 0x5caf); } else { CYC(0x5cac, 0x5caf); } // call nz
  anglerFish_applySpeed_hook(gb); return; // fallthrough
}

void anglerFish_applySpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5caf, objectApplySpeed_hook, 0x201d, 0x5cb2);
  CALL_C(0x5cb2, ecom_bounceOffWallsAndHoles_b0f_hook, 0x42de, 0x5cb5);
  if (F & FZ) { CYCT(0x5cb5, 0x5cb8); enemyAnimate_hook(gb); return; } // jp z
  CYC(0x5cb5, 0x5cb8);
  anglerFish_updateAnimation_hook(gb); return; // fallthrough
}

void anglerFish_updateAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5cb8, 0x5cba); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x5cba, 0x5cbb); A = mem_rd(gb, DE);
  CYC(0x5cbb, 0x5cbd); alu_xor(gb, 0x01);
  CYC(0x5cbd, 0x5cbe); mem_wr(gb, DE, A);
  CYC(0x5cbe, 0x5cc1); enemySetAnimation_hook(gb); return; // jp
}

// Firing a projectile
void anglerFish_main_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5cc1, 0x5cc3); E = ENEMY_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x5cc3, 0x5cc4); A = mem_rd(gb, DE);
  CYC(0x5cc4, 0x5cc5); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x5cc5, 0x5cc7); goto doneFiring; } // jr z
  CYC(0x5cc5, 0x5cc7);
  CYC(0x5cc7, 0x5cc8); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x5cc8, 0x5cca); anglerFish_updatePosition_hook(gb); return; } // jr z
  CYC(0x5cc8, 0x5cca);
  CYC(0x5cca, 0x5ccb); alu_xor(gb, A);
  CYC(0x5ccb, 0x5ccc); mem_wr(gb, DE, A);
  CALL_C(0x5ccc, getFreeEnemySlot_uncounted_hook, 0x2e34, 0x5ccf);
  if (!(F & FZ)) { CYCT(0x5ccf, 0x5cd1); anglerFish_updatePosition_hook(gb); return; } // jr nz
  CYC(0x5ccf, 0x5cd1);
  CYC(0x5cd1, 0x5cd3); mem_wr(gb, HL, 0x26); // ENEMY_ANGLER_FISH_BUBBLE
  CYC(0x5cd3, 0x5cd5); L = ENEMY_BASE + OBJ_RELATED1;
  CYC(0x5cd5, 0x5cd7); A = ENEMY_BASE; // Enemy.start
  CYC(0x5cd7, 0x5cd8); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5cd8, 0x5cd9); mem_wr(gb, HL, D);
  CYC(0x5cd9, 0x5cdb); A = 0x59; // SND_FALLINHOLE
  CALL_C(0x5cdb, playSound_b00_hook, 0x0c98, 0x5cde);
  CYCT(0x5cde, 0x5ce0); anglerFish_updatePosition_hook(gb); return; // jr

doneFiring:
  CYC(0x5ce0, 0x5ce1); H = D;
  CYC(0x5ce1, 0x5ce3); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5ce3, 0x5ce4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5ce4, 0x5ce6); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x5ce6, 0x5ce7); A = mem_rd(gb, HL);
  CYC(0x5ce7, 0x5ce9); alu_sub(gb, 0x02);
  CYC(0x5ce9, 0x5cea); mem_wr(gb, HL, A);
  CALL_C(0x5cea, enemySetAnimation_hook, 0x282b, 0x5ced);
  CYCT(0x5ced, 0x5cef); anglerFish_updatePosition_hook(gb); return; // jr
}

// Just hit with a scent seed, falling to ground
void anglerFish_main_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5cef, 0x5cf1); A = 0x20;
  CALL_C(0x5cf1, objectUpdateSpeedZ_sidescroll_hook, 0x1f66, 0x5cf4);
  if (!(F & FC)) { RET_TAKEN(0x5cf4); return; } // ret nc
  CYC(0x5cf4, 0x5cf5);
  CALL_C(0x5cf5, ecom_incState_b0f_hook, 0x4000, 0x5cf8);
  CYC(0x5cf8, 0x5cfa); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5cfa, 0x5cfc); mem_wr(gb, HL, 0x96); // 150
  RET(0x5cfc); return;
}

// Vulnerable for [counter1] frames
void anglerFish_main_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5cfd, ecom_decCounter1_b0f_hook, 0x439a, 0x5d00);
  if (!(F & FZ)) { CYCT(0x5d00, 0x5d03); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x5d00, 0x5d03);
  CYC(0x5d03, 0x5d04); L = E;
  CYC(0x5d04, 0x5d05); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5d05, 0x5d07); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x5d07, 0x5d09); A = 0x00;
  CYC(0x5d09, 0x5d0a); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5d0a, 0x5d0c); mem_wr(gb, HL, 0xfc);
  RET(0x5d0c); return;
}

// Bouncing back up after being deflated
void anglerFish_main_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d0d, 0x5d0f); A = 0x20;
  CALL_C(0x5d0f, objectUpdateSpeedZ_sidescroll_hook, 0x1f66, 0x5d12);
  CYC(0x5d12, 0x5d14); L = ENEMY_BASE + 0x15; // Enemy.speedZ+1
  CYC(0x5d14, 0x5d15); A = mem_rd(gb, HL);
  CYC(0x5d15, 0x5d16); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5d16, 0x5d18); anglerFish_applySpeed_hook(gb); return; } // jr nz
  CYC(0x5d16, 0x5d18);
  CYC(0x5d18, 0x5d1a); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5d1a, 0x5d1c); mem_wr(gb, HL, 0x0b);
  CYC(0x5d1c, 0x5d1e); L = ENEMY_BASE + OBJ_COLLISION_TYPE;
  CYC(0x5d1e, 0x5d20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(0x5d20, 0x5d22); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5d22, 0x5d24); mem_wr(gb, HL, 0xb4); // 180
  CYC(0x5d24, 0x5d26); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x5d26, 0x5d27); A = mem_rd(gb, HL);
  CYC(0x5d27, 0x5d29); alu_sub(gb, 0x04);
  CYC(0x5d29, 0x5d2a); mem_wr(gb, HL, A);
  CALL_C(0x5d2a, enemySetAnimation_hook, 0x282b, 0x5d2d);
  CYC(0x5d2d, 0x5d2f); A = 0x98; // SND_POOF
  CYC(0x5d2f, 0x5d32); playSound_b00_hook(gb); return; // jp
}

void anglerFish_antenna_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d32, 0x5d33); A = mem_rd(gb, DE);
  CYC(0x5d33, 0x5d35); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(0x5d35, 0x5d37); goto state8; } // jr z
  CYC(0x5d35, 0x5d37);

state9:
  CYC(0x5d37, 0x5d39); A = OBJ_DIRECTION;
  CALL_C(0x5d39, objectGetRelatedObject1Var_hook, 0x2160, 0x5d3c);
  CYC(0x5d3c, 0x5d3d); A = mem_rd(gb, HL);
  CYC(0x5d3d, 0x5d3e); push_effect(gb, HL);
  CYC(0x5d3e, 0x5d41); SET_HL(0x5d58);
  CYC(0x5d41, 0x5d42); anglerFish_addDoubleIndexToHl_from_rst(gb, 0x5d42);
  CYC(0x5d42, 0x5d43); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5d43, 0x5d44); B = A;
  CYC(0x5d44, 0x5d45); C = mem_rd(gb, HL);
  CYC(0x5d45, 0x5d46); SET_HL(pop_effect(gb));
  CALL_C(0x5d46, objectTakePositionWithOffset_hook, 0x2277, 0x5d49);
  CYC(0x5d49, 0x5d4b); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x5d4b, 0x5d4c); A = mem_rd(gb, HL);
  CYC(0x5d4c, 0x5d4d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5d4d, 0x5d50); objectSetInvisible_hook(gb); return; } // jp nz
  CYC(0x5d4d, 0x5d50);
  CYC(0x5d50, 0x5d53); A = mem_rd(gb, wFrameCounter);
  CYC(0x5d53, 0x5d54); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x5d54); return; } // ret c
  CYC(0x5d54, 0x5d55);
  CYC(0x5d55, 0x5d58); ecom_flickerVisibility_b0f_hook(gb); return; // jp

state8:
  CYC(0x5d64, 0x5d65); H = D;
  CYC(0x5d65, 0x5d66); L = E;
  CYC(0x5d66, 0x5d67); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x5d67, 0x5d69); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x5d69, 0x5d6b); mem_wr(gb, HL, 0x47); // ENEMYCOLLISION_ANGLER_FISH_ANTENNA
  CYC(0x5d6b, 0x5d6d); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x5d6d, 0x5d6f); A = 0x03;
  CYC(0x5d6f, 0x5d70); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5d70, 0x5d71); mem_wr(gb, HL, A);
  CYC(0x5d71, 0x5d73); L = ENEMY_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(0x5d73, 0x5d75); mem_wr(gb, HL, 0x1e);
  CYC(0x5d75, 0x5d77); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x5d77, 0x5d79); A = 0x0d;
  CYC(0x5d79, 0x5d7a); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5d7a, 0x5d7b); mem_wr(gb, HL, A);
  CYC(0x5d7b, 0x5d7d); A = 0x06;
  CYC(0x5d7d, 0x5d80); enemySetAnimation_hook(gb); return; // jp
}

// Changes state to $0c if conditions are appropriate to fire a projectile.
void anglerFish_main_checkFireProjectile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5d80, 0x5d82); E = ENEMY_BASE + OBJ_YH;
  CYC(0x5d82, 0x5d83); A = mem_rd(gb, DE);
  CYC(0x5d83, 0x5d85); alu_cp(gb, 0x5c);
  if (!(F & FC)) { RET_TAKEN(0x5d85); return; } // ret nc
  CYC(0x5d85, 0x5d86);
  CYC(0x5d86, 0x5d88); E = ENEMY_BASE + OBJ_XH;
  CYC(0x5d88, 0x5d89); A = mem_rd(gb, DE);
  CYC(0x5d89, 0x5d8b); alu_sub(gb, 0x38);
  CYC(0x5d8b, 0x5d8d); alu_cp(gb, 0x70);
  if (!(F & FC)) { RET_TAKEN(0x5d8d); return; } // ret nc
  CYC(0x5d8d, 0x5d8e);
  CYC(0x5d8e, 0x5d90); mem_wr(gb, HL, 0xb4); // [counter2] = 180 (hl left by caller's ecom_decCounter2)
  CYC(0x5d90, 0x5d92); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5d92, 0x5d93); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5d93, 0x5d95); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x5d95, 0x5d96); A = mem_rd(gb, HL);
  CYC(0x5d96, 0x5d98); alu_add(gb, 0x02);
  CYC(0x5d98, 0x5d99); mem_wr(gb, HL, A);
  CYC(0x5d99, 0x5d9c); enemySetAnimation_hook(gb); return; // jp
}
