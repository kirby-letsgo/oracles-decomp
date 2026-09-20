#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0d, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0d, (from), (to), true)

static uint16_t enemyCode12_jump_table(GB *gb) {
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
// ENEMY_GIBDO
// ==================================================================================================
void enemyCode12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5002, ecom_checkHazards_b0d_hook, 0x4051, 0x5005);
  if (F & FZ) { CYCT(0x5005, 0x5007); goto normalStatus; } // jr z
  CYC(0x5005, 0x5007);
  CYC(0x5007, 0x5009); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x5009); return; } // ret c
  CYC(0x5009, 0x500a);
  if (F & FZ) { CYCT(0x500a, 0x500d); enemyDie_hook(gb); return; } // jp z
  CYC(0x500a, 0x500d);
  CYC(0x500d, 0x500f); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(0x500f, 0x5010); A = mem_rd(gb, DE);
  CYC(0x5010, 0x5012); alu_cp(gb, 0x9b); // $80|ITEMCOLLISION_EMBER_SEED
  if (!(F & FZ)) { RET_TAKEN(0x5012); return; } // ret nz
  CYC(0x5012, 0x5013);
  CYC(0x5013, 0x5014); H = D;
  CYC(0x5014, 0x5016); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x5016, 0x5018); A = 0x0a;
  CYC(0x5018, 0x5019); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x5019); return; } // ret z
  CYC(0x5019, 0x501a);
  CYC(0x501a, 0x501b); mem_wr(gb, HL, A);
  CYC(0x501b, 0x501d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x501d, 0x501f); mem_wr(gb, HL, 30);
  CYC(0x501f, 0x5021); L = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(0x5021, 0x5023); mem_wr(gb, HL, 0x00);
  RET(0x5023); return; // ret

normalStatus:
  CYC(0x5024, 0x5026); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x5026, 0x5027); A = mem_rd(gb, DE);
  {
    CYC(0x5027, 0x5028); push_effect(gb, 0x5028);
    uint16_t target = enemyCode12_jump_table(gb);
    if (target == 0x503e) goto uninitialized;
    if (target == 0x504f) goto state_stub;
    if (target == 0x5043) goto state_switchHook;
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == 0x5050) goto state8;
    if (target == 0x5065) goto state9;
    if (target == 0x5072) goto stateA;
    HANDOFF(target);
  }

uninitialized:
  CYC(0x503e, 0x5040); A = 0x14; // SPEED_80
  CYC(0x5040, 0x5043); ecom_setSpeedAndState8AndVisible_b0d_hook(gb); return; // jp

state_switchHook:
  CYC(0x5043, 0x5044); E = alu_inc8(gb, E);
  CYC(0x5044, 0x5045); A = mem_rd(gb, DE);
  {
    CYC(0x5045, 0x5046); push_effect(gb, 0x5046);
    uint16_t target = enemyCode12_jump_table(gb);
    if (target == 0x4005) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == 0x504e) goto substate1;
    if (target == 0x44e0) { ecom_fallToGroundAndSetState8_b0d_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(0x504e); return; // ret

state_stub:
  RET(0x504f); return; // ret

state8:
  CYC(0x5050, 0x5052); A = 0x09;
  CYC(0x5052, 0x5053); mem_wr(gb, DE, A);
  CYC(0x5053, 0x5056); SET_BC(0x187f);
  CALL_C(0x5056, ecom_randomBitwiseAndBCE_b0d_hook, 0x434f, 0x5059);
  CYC(0x5059, 0x505b); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(0x505b, 0x505c); A = B;
  CYC(0x505c, 0x505d); mem_wr(gb, DE, A);
  CYC(0x505d, 0x505f); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x505f, 0x5061); A = 0x40;
  CYC(0x5061, 0x5062); alu_add(gb, C);
  CYC(0x5062, 0x5063); mem_wr(gb, DE, A);
  CYC(0x5063, 0x5065); goto animate; // jr

state9:
  CALL_C(0x5065, ecom_decCounter1_b0d_hook, 0x439a, 0x5068);
  if (F & FZ) { CYCT(0x5068, 0x506a); goto gotoState8; } // jr z
  CYC(0x5068, 0x506a);
  CALL_C(0x506a, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, 0x4156, 0x506d);
  if (F & FZ) { CYCT(0x506d, 0x506f); goto gotoState8; } // jr z
  CYC(0x506d, 0x506f);

animate:
  CYC(0x506f, 0x5072); enemyAnimate_hook(gb); return; // jp

stateA:
  CALL_C(0x5072, ecom_decCounter1_b0d_hook, 0x439a, 0x5075);
  if (!(F & FZ)) { RET_TAKEN(0x5075); return; } // ret nz
  CYC(0x5075, 0x5076);
  CYC(0x5076, 0x5079); SET_BC(0x3102); // ENEMY_STALFOS, subid 2
  CYC(0x5079, 0x507c); enemyReplaceWithID_hook(gb); return; // jp

gotoState8:
  CYC(0x507c, 0x507e); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x507e, 0x5080); A = 0x08;
  CYC(0x5080, 0x5081); mem_wr(gb, DE, A);
  CYC(0x5081, 0x5084); goto animate; // jr
}
