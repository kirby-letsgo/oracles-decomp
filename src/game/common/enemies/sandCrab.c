#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode1a), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode1a), (from), (to), true)

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
  BASE(enemyCode1a);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ecom_updateKnockback_b0d_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);
  RET(b_+12); return; // ret

normalStatus:
  CALL_C(b_+13, ecom_checkScentSeedActive_b0d_hook, SYM(ecom_checkScentSeedActive_b0d), b_+16);
  CYC(b_+16, b_+18); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  {
    CYC(b_+19, b_+20); push_effect(gb, b_+20);
    uint16_t target = enemyCode1a_jump_table(gb);
    if (target == b_+40) goto state_uninitialized;
    if (target == b_+98) goto state_stub;
    if (target == b_+86) goto state_switchHook;
    if (target == b_+48) goto state_scentSeed;
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == b_+99) goto state8;
    if (target == b_+131) goto state9;
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+43); L = ENEMY_BASE + 0x3f; // Enemy.var3f
  CYC(b_+43, b_+45); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 4)); // set 4,(hl)
  CYC(b_+45, b_+48); ecom_setSpeedAndState8AndVisible_b0d_hook(gb); return; // jp

state_scentSeed:
  CYC(b_+48, b_+51); A = mem_rd(gb, wScentSeedActive);
  CYC(b_+51, b_+52); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+52, b_+54); goto scentSeedTimedOut; } // jr nz
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); A = 0x08;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A); // [state] = 8
  CYC(b_+57, b_+59); goto animate; // jr

scentSeedTimedOut:
  CALL_C(b_+59, ecom_updateAngleToScentSeed_b0d_hook, SYM(ecom_updateAngleToScentSeed_b0d), b_+62);
  CYC(b_+62, b_+64); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+67); alu_add(gb, 0x04);
  CYC(b_+67, b_+69); alu_and(gb, 0x18);
  CYC(b_+69, b_+70); mem_wr(gb, DE, A);
  CYC(b_+70, b_+72); alu_bit(gb, 3, A);
  CYC(b_+72, b_+74); A = 0x0a; // SPEED_40
  if (F & FZ) { CYCT(b_+74, b_+76); goto scentSeedSetSpeed; } // jr z
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+78); A = 0x28; // SPEED_100

scentSeedSetSpeed:
  CYC(b_+78, b_+80); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  CALL_C(b_+81, ecom_applyVelocityForSideviewEnemy_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemy_b0d), b_+84);
  CYC(b_+84, b_+86); goto animate; // jr

state_switchHook:
  CYC(b_+86, b_+87); E = alu_inc8(gb, E);
  CYC(b_+87, b_+88); A = mem_rd(gb, DE);
  {
    CYC(b_+88, b_+89); push_effect(gb, b_+89);
    uint16_t target = enemyCode1a_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+97) goto substate1;
    if (target == SYM(ecom_fallToGroundAndSetState8_b0d)) { ecom_fallToGroundAndSetState8_b0d_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(b_+97); return; // ret

state_stub:
  RET(b_+98); return; // ret

state8:
  CYC(b_+99, b_+101); A = 0x09;
  CYC(b_+101, b_+102); mem_wr(gb, DE, A); // [state] = 9
  CYC(b_+102, b_+105); SET_BC((SYM(addToGashaMaturity) + 15));
  CALL_C(b_+105, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+108);
  CYC(b_+108, b_+110); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+110, b_+112); A = 0x30;
  CYC(b_+112, b_+113); alu_add(gb, C);
  CYC(b_+113, b_+114); mem_wr(gb, DE, A);
  CYC(b_+114, b_+116); alu_bit(gb, 3, B);
  CYC(b_+116, b_+118); A = 0x0a; // SPEED_40
  if (F & FZ) { CYCT(b_+118, b_+120); goto state8SetSpeed; } // jr z
  CYC(b_+118, b_+120);
  CYC(b_+120, b_+122); A = 0x28; // SPEED_100

state8SetSpeed:
  CYC(b_+122, b_+124); E = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+124, b_+125); mem_wr(gb, DE, A);
  CYC(b_+125, b_+127); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+127, b_+128); A = B;
  CYC(b_+128, b_+129); mem_wr(gb, DE, A);
  CYC(b_+129, b_+131); goto animate; // jr

state9:
  CALL_C(b_+131, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+134);
  if (F & FZ) { CYCT(b_+134, b_+136); goto state9SetState8; } // jr z
  CYC(b_+134, b_+136);
  CALL_C(b_+136, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+139);
  if (!(F & FZ)) { CYCT(b_+139, b_+141); goto animate; } // jr nz
  CYC(b_+139, b_+141);

state9SetState8:
  CYC(b_+141, b_+143); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+143, b_+145); A = 0x08;
  CYC(b_+145, b_+146); mem_wr(gb, DE, A);

animate:
  CYC(b_+146, SYM(enemyCode1b)); enemyAnimate_hook(gb); return; // jp
}
