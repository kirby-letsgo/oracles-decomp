#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode1a_jump_table(GB *gb) {
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
// ENEMY_SAND_CRAB
// ==================================================================================================
void enemyCode1a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x5715, 0x5717); goto normalStatus; } // jr z
  CYC(0x5715, 0x5717);
  CYC(0x5717, 0x5719); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5719); return; } // ret c
  CYC(0x5719, 0x571a);
  if (F & FZ) { CYCT(0x571a, 0x571d); enemyDie_hook(gb); return; } // jp z
  CYC(0x571a, 0x571d);
  CYC(0x571d, 0x571e); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x571e, 0x5721); ecom_updateKnockback_b0d_hook(gb); return; } // jp nz
  CYC(0x571e, 0x5721);
  RET(0x5721); return; // ret

normalStatus:
  CALL_C(0x5722, ecom_checkScentSeedActive_b0d_hook, 0x44b6, 0x5725);
  CYC(0x5725, 0x5727); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5727, 0x5728); A = mem_rd(gb, DE);
  {
    CYC(0x5728, 0x5729); push_effect(gb, 0x5729);
    uint16_t target = enemyCode1a_jump_table(gb);
    if (target == 0x573d) goto state_uninitialized;
    if (target == 0x5777) goto state_stub;
    if (target == 0x576b) goto state_switchHook;
    if (target == 0x5745) goto state_scentSeed;
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x5778) goto state8;
    if (target == 0x5798) goto state9;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x573d, 0x573e); H = D;
  CYC(0x573e, 0x5740); L = ENEMY_BASE + 0x3f; // Enemy.var3f
  CYC(0x5740, 0x5742); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 4)); // set 4,(hl)
  CYC(0x5742, 0x5745); ecom_setSpeedAndState8AndVisible_b0d_hook(gb); return; // jp

state_scentSeed:
  CYC(0x5745, 0x5748); A = mem_rd(gb, wScentSeedActive);
  CYC(0x5748, 0x5749); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5749, 0x574b); goto scentSeedTimedOut; } // jr nz
  CYC(0x5749, 0x574b);
  CYC(0x574b, 0x574d); A = 0x08;
  CYC(0x574d, 0x574e); mem_wr(gb, DE, A); // [state] = 8
  CYC(0x574e, 0x5750); goto animate; // jr

scentSeedTimedOut:
  CALL_C(0x5750, ecom_updateAngleToScentSeed_b0d_hook, 0x44cb, 0x5753);
  CYC(0x5753, 0x5755); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5755, 0x5756); A = mem_rd(gb, DE);
  CYC(0x5756, 0x5758); alu_add(gb, 0x04);
  CYC(0x5758, 0x575a); alu_and(gb, 0x18);
  CYC(0x575a, 0x575b); mem_wr(gb, DE, A);
  CYC(0x575b, 0x575d); alu_bit(gb, 3, A);
  CYC(0x575d, 0x575f); A = 0x0a; // SPEED_40
  if (F & FZ) { CYCT(0x575f, 0x5761); goto scentSeedSetSpeed; } // jr z
  CYC(0x575f, 0x5761);
  CYC(0x5761, 0x5763); A = 0x28; // SPEED_100

scentSeedSetSpeed:
  CYC(0x5763, 0x5765); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5765, 0x5766); mem_wr(gb, DE, A);
  CALL_C(0x5766, ecom_applyVelocityForSideviewEnemy_b0d_hook, 0x4153, 0x5769);
  CYC(0x5769, 0x576b); goto animate; // jr

state_switchHook:
  CYC(0x576b, 0x576c); E = alu_inc8(gb, E);
  CYC(0x576c, 0x576d); A = mem_rd(gb, DE);
  {
    CYC(0x576d, 0x576e); push_effect(gb, 0x576e);
    uint16_t target = enemyCode1a_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x5776) goto substate1;
    if (target == 0x44e0) { ecom_fallToGroundAndSetState8_b0d_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(0x5776); return; // ret

state_stub:
  RET(0x5777); return; // ret

state8:
  CYC(0x5778, 0x577a); A = 0x09;
  CYC(0x577a, 0x577b); mem_wr(gb, DE, A); // [state] = 9
  CYC(0x577b, 0x577e); SET_BC(0x1830);
  CALL_C(0x577e, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x5781);
  CYC(0x5781, 0x5783); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x5783, 0x5785); A = 0x30;
  CYC(0x5785, 0x5786); alu_add(gb, C);
  CYC(0x5786, 0x5787); mem_wr(gb, DE, A);
  CYC(0x5787, 0x5789); alu_bit(gb, 3, B);
  CYC(0x5789, 0x578b); A = 0x0a; // SPEED_40
  if (F & FZ) { CYCT(0x578b, 0x578d); goto state8SetSpeed; } // jr z
  CYC(0x578b, 0x578d);
  CYC(0x578d, 0x578f); A = 0x28; // SPEED_100

state8SetSpeed:
  CYC(0x578f, 0x5791); E = ENEMY_BASE + OBJ_SPEED;
  CYC(0x5791, 0x5792); mem_wr(gb, DE, A);
  CYC(0x5792, 0x5794); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x5794, 0x5795); A = B;
  CYC(0x5795, 0x5796); mem_wr(gb, DE, A);
  CYC(0x5796, 0x5798); goto animate; // jr

state9:
  CALL_C(0x5798, ecom_decCounter1_b0d_hook, 0x439a, 0x579b);
  if (F & FZ) { CYCT(0x579b, 0x579d); goto state9SetState8; } // jr z
  CYC(0x579b, 0x579d);
  CALL_C(0x579d, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x57a0);
  if (!(F & FZ)) { CYCT(0x57a0, 0x57a2); goto animate; } // jr nz
  CYC(0x57a0, 0x57a2);

state9SetState8:
  CYC(0x57a2, 0x57a4); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x57a4, 0x57a6); A = 0x08;
  CYC(0x57a6, 0x57a7); mem_wr(gb, DE, A);

animate:
  CYC(0x57a7, 0x57aa); enemyAnimate_hook(gb); return; // jp
}
