#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void enemyCode55_hook(GB *gb);
void candle_state_uninitialized_hook(GB *gb);
void candle_state_stub_hook(GB *gb);
void candle_state8_hook(GB *gb);
void candle_state9_hook(GB *gb);
void candle_stateA_hook(GB *gb);
void candle_stateB_hook(GB *gb);
void candle_applySpeed_hook(GB *gb);
void candle_animate_hook(GB *gb);
void candle_stateC_hook(GB *gb);
void candle_stateD_hook(GB *gb);
void candle_stateE_hook(GB *gb);

static uint16_t candle_jump_table(GB *gb) {
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
// ENEMY_CANDLE
//
// Variables:
//   relatedObj1: reference to INTERAC_EXPLOSION while exploding
// ==================================================================================================
void enemyCode55_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x760a, ecom_checkHazards_b0e_hook, 0x4051, 0x760d);
  if (F & FZ) { CYCT(0x760d, 0x760f); goto normalStatus; } // jr z
  CYC(0x760d, 0x760f);
  CYC(0x760f, 0x7611); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x7611); return; } // ret c
  CYC(0x7611, 0x7612);
  CYC(0x7612, 0x7614); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x7614, 0x7615); A = mem_rd(gb, DE);
  CYC(0x7615, 0x7617); alu_cp(gb, 0x9b); // $80|ITEMCOLLISION_EMBER_SEED
  if (!(F & FZ)) { CYCT(0x7617, 0x7619); goto normalStatus; } // jr nz
  CYC(0x7617, 0x7619);
  CYC(0x7619, 0x761b); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x761b, 0x761c); A = mem_rd(gb, DE);
  CYC(0x761c, 0x761e); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x761e, 0x7620); goto normalStatus; } // jr nc
  CYC(0x761e, 0x7620);
  CYC(0x7620, 0x7622); A = 0x0a;
  CYC(0x7622, 0x7623); mem_wr(gb, DE, A);

normalStatus:
  CYC(0x7623, 0x7625); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x7625, 0x7626); A = mem_rd(gb, DE);
  {
    CYC(0x7626, 0x7627); push_effect(gb, 0x7627);
    uint16_t target = candle_jump_table(gb);
    if (target == 0x7645) { candle_state_uninitialized_hook(gb); return; }
    if (target == 0x764f) { candle_state_stub_hook(gb); return; }
    if (target == 0x7650) { candle_state8_hook(gb); return; }
    if (target == 0x7669) { candle_state9_hook(gb); return; }
    if (target == 0x767b) { candle_stateA_hook(gb); return; }
    if (target == 0x7691) { candle_stateB_hook(gb); return; }
    if (target == 0x76ac) { candle_stateC_hook(gb); return; }
    if (target == 0x76b5) { candle_stateD_hook(gb); return; }
    if (target == 0x76d5) { candle_stateE_hook(gb); return; }
    HANDOFF(target);
  }
}

// 0e:7645, bare global; jump-table target from enemyCode55.
void candle_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7645, 0x7647); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7647, 0x7649); A = 0x1e; // 30
  CYC(0x7649, 0x764a); mem_wr(gb, DE, A);
  CYC(0x764a, 0x764c); A = 0x0a; // SPEED_40
  CYC(0x764c, 0x764f); ecom_setSpeedAndState8AndVisible_b0e_hook(gb); return; // jp
}

// 0e:764f, bare global; jump-table target from enemyCode55.
void candle_state_stub_hook(GB *gb) {
  RET(0x764f); return; // ret
}

// 0e:7650, bare global; jump-table target from enemyCode55. Standing still for
// [counter1] frames.
void candle_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7650, ecom_decCounter1_b0e_hook, 0x439a, 0x7653);
  if (!(F & FZ)) { RET_TAKEN(0x7653); return; } // ret nz
  CYC(0x7653, 0x7654);
  CYC(0x7654, 0x7655); L = E;
  CYC(0x7655, 0x7656); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x7656, 0x7658); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7658, 0x765a); mem_wr(gb, HL, 0x5a); // 90
  CALL_C(0x765a, getRandomNumber_noPreserveVars_hook, 0x0453, 0x765d);
  CYC(0x765d, 0x765f); alu_and(gb, 0x18);
  CYC(0x765f, 0x7661); alu_add(gb, 0x04);
  CYC(0x7661, 0x7663); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x7663, 0x7664); mem_wr(gb, DE, A);
  CYC(0x7664, 0x7666); A = 0x01;
  CYC(0x7666, 0x7669); enemySetAnimation_hook(gb); return; // jp $282b
}

// 0e:7669, bare global; jump-table target from enemyCode55. Walking for [counter1]
// frames.
void candle_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7669, ecom_decCounter1_b0e_hook, 0x439a, 0x766c);
  if (!(F & FZ)) { CYCT(0x766c, 0x766e); goto applySpeed; } // jr nz
  CYC(0x766c, 0x766e);
  CYC(0x766e, 0x7670); mem_wr(gb, HL, 0x1e); // [counter1] = 30
  CYC(0x7670, 0x7671); L = E;
  CYC(0x7671, 0x7672); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x7672, 0x7673); alu_xor(gb, A);
  CALL_C(0x7673, enemySetAnimation_hook, 0x282b, 0x7676);

applySpeed:
  CALL_C(0x7676, ecom_applyVelocityForSideviewEnemyNoHoles_b0e_hook, 0x4156, 0x7679);
  CYC(0x7679, 0x767b); candle_animate_hook(gb); return; // jr
}

// 0e:767b, bare global; jump-table target from enemyCode55. Just lit on fire.
void candle_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x767b, 0x767d); B = 0x36; // PART_CANDLE_FLAME
  CALL_C(0x767d, ecom_spawnProjectile_b0e_hook, 0x437c, 0x7680);
  if (!(F & FZ)) { RET_TAKEN(0x7680); return; } // ret nz
  CYC(0x7680, 0x7681);
  CALL_C(0x7681, ecom_incState_b0e_hook, 0x4000, 0x7684);
  CYC(0x7684, 0x7686); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x7686, 0x7688); mem_wr(gb, HL, 0x78); // 120
  CYC(0x7688, 0x768a); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x768a, 0x768c); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x768c, 0x768e); A = 0x02;
  CYC(0x768e, 0x7691); enemySetAnimation_hook(gb); return; // jp $282b
}

// 0e:7691, bare global; jump-table target from enemyCode55. Moving slowly at first.
void candle_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7691, ecom_decCounter1_b0e_hook, 0x439a, 0x7694);
  if (!(F & FZ)) { CYCT(0x7694, 0x7696); candle_applySpeed_hook(gb); return; } // jr nz
  CYC(0x7694, 0x7696);
  CYC(0x7696, 0x7698); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CYC(0x7698, 0x7699); L = E;
  CYC(0x7699, 0x769a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x769a, 0x769c); L = ENEMY_BASE + OBJ_SPEED;
  CYC(0x769c, 0x769e); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x769e, 0x76a0); A = 0x03;
  CALL_C(0x76a0, enemySetAnimation_hook, 0x282b, 0x76a3);
  candle_applySpeed_hook(gb); return; // fallthrough
}

// 0e:76a3, bare global; falls into from candle_stateB, also reached via tail-jump
// from candle_stateC/candle_stateD.
void candle_applySpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x76a3, objectApplySpeed_hook, 0x201d, 0x76a6);
  CALL_C(0x76a6, ecom_bounceOffWallsAndHoles_b0e_hook, 0x42de, 0x76a9);
  candle_animate_hook(gb); return; // fallthrough
}

// 0e:76a9, bare global; falls into from candle_applySpeed, also reached via tail-jump
// from candle_state9.
void candle_animate_hook(GB *gb) {
  CYC(0x76a9, 0x76ac); enemyAnimate_hook(gb); return; // jp
}

// 0e:76ac, bare global; jump-table target from enemyCode55. Moving faster.
void candle_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x76ac, ecom_decCounter1_b0e_hook, 0x439a, 0x76af);
  if (!(F & FZ)) { CYCT(0x76af, 0x76b1); candle_applySpeed_hook(gb); return; } // jr nz
  CYC(0x76af, 0x76b1);
  CYC(0x76b1, 0x76b3); mem_wr(gb, HL, 0x3c); // [counter1] = 60
  CYC(0x76b3, 0x76b4); L = E;
  CYC(0x76b4, 0x76b5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  candle_stateD_hook(gb); return; // fallthrough
}

// 0e:76b5, bare global; falls into from candle_stateC. Flickering visibility, about
// to explode.
void candle_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x76b5, ecom_flickerVisibility_b0e_hook, 0x441f, 0x76b8);
  CALL_C(0x76b8, ecom_decCounter1_b0e_hook, 0x439a, 0x76bb);
  if (!(F & FZ)) { CYCT(0x76bb, 0x76bd); candle_applySpeed_hook(gb); return; } // jr nz
  CYC(0x76bb, 0x76bd);
  CYC(0x76bd, 0x76be); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter1] = 1
  CYC(0x76be, 0x76c0); B = 0x56; // INTERAC_EXPLOSION
  CALL_C(0x76c0, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x76c3);
  if (!(F & FZ)) { RET_TAKEN(0x76c3); return; } // ret nz
  CYC(0x76c3, 0x76c4);
  CYC(0x76c4, 0x76c5); A = H;
  CYC(0x76c5, 0x76c6); H = D;
  CYC(0x76c6, 0x76c8); L = ENEMY_BASE + OBJ_RELATED1 + 1; // Enemy.relatedObj1+1
  CYC(0x76c8, 0x76c9); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a
  CYC(0x76c9, 0x76cb); mem_wr(gb, HL, 0x40); // Interaction.start == INTERACTION_BASE
  CYC(0x76cb, 0x76cd); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x76cd, 0x76ce); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x76ce, 0x76d0); L = ENEMY_BASE + OBJ_ENEMY_COLLISION_MODE;
  CYC(0x76d0, 0x76d2); mem_wr(gb, HL, 0x04); // ENEMYCOLLISION_PODOBOO
  CYC(0x76d2, 0x76d5); objectSetInvisible_hook(gb); return; // jp
}

// 0e:76d5, bare global; jump-table target from enemyCode55. Waiting for the
// explosion to end.
void candle_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x76d5, 0x76d7); A = 0x21; // Object.animParameter
  CALL_C(0x76d7, objectGetRelatedObject1Var_hook, 0x2160, 0x76da);
  CYC(0x76da, 0x76db); A = mem_rd(gb, HL);
  CYC(0x76db, 0x76dc); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x76dc); return; } // ret z
  CYC(0x76dc, 0x76dd);
  CYC(0x76dd, 0x76de); alu_rlca(gb);
  if (F & FC) { CYCT(0x76de, 0x76e0); goto done; } // jr c
  CYC(0x76de, 0x76e0);
  CYC(0x76e0, 0x76e2); mem_wr(gb, HL, 0x00); // [child.animParameter] = 0
  CYC(0x76e2, 0x76e4); L = ENEMY_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x76e4, 0x76e6); A = 0x0c;
  CYC(0x76e6, 0x76e7); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x76e7, 0x76e8); mem_wr(gb, HL, A);
  RET(0x76e8); return; // ret

done:
  CALL_C(0x76e9, markEnemyAsKilledInRoom_b00_hook, 0x320d, 0x76ec);
  CALL_C(0x76ec, decNumEnemies_hook, 0x24b3, 0x76ef);
  CYC(0x76ef, 0x76f2); enemyDelete_hook(gb); return; // jp
}
