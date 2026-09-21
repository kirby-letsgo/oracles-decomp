#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode12);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_checkHazards_b0d_hook, SYM(ecom_checkHazards_b0d), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto normalStatus; } // jr z
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+7); return; } // ret c
  CYC(b_+7, b_+8);
  if (F & FZ) { CYCT(b_+8, b_+11); enemyDie_hook(gb); return; } // jp z
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+16); alu_cp(gb, 0x9b); // $80|ITEMCOLLISION_EMBER_SEED
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; } // ret nz
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+20, b_+22); A = 0x0a;
  CYC(b_+22, b_+23); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+23); return; } // ret z
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CYC(b_+25, b_+27); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 30);
  CYC(b_+29, b_+31); L = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x00);
  RET(b_+33); return; // ret

normalStatus:
  CYC(b_+34, b_+36); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  {
    CYC(b_+37, b_+38); push_effect(gb, b_+38);
    uint16_t target = enemyCode12_jump_table(gb);
    if (target == b_+60) goto uninitialized;
    if (target == b_+77) goto state_stub;
    if (target == b_+65) goto state_switchHook;
    if (target == SYM(ecom_blownByGaleSeedState_b0d)) { ecom_blownByGaleSeedState_b0d_hook(gb); return; }
    if (target == b_+78) goto state8;
    if (target == b_+99) goto state9;
    if (target == b_+112) goto stateA;
    HANDOFF(target);
  }

uninitialized:
  CYC(b_+60, b_+62); A = 0x14; // SPEED_80
  CYC(b_+62, b_+65); TAIL(ecom_setSpeedAndState8AndVisible_b0d); // jp

state_switchHook:
  CYC(b_+65, b_+66); E = alu_inc8(gb, E);
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  {
    CYC(b_+67, b_+68); push_effect(gb, b_+68);
    uint16_t target = enemyCode12_jump_table(gb);
    if (target == SYM(ecom_incSubstate_b0d)) { ecom_incSubstate_b0d_hook(gb); return; }
    if (target == b_+76) goto substate1;
    if (target == SYM(ecom_fallToGroundAndSetState8_b0d)) { ecom_fallToGroundAndSetState8_b0d_hook(gb); return; }
    HANDOFF(target);
  }

substate1:
  RET(b_+76); return; // ret

state_stub:
  RET(b_+77); return; // ret

state8:
  CYC(b_+78, b_+80); A = 0x09;
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  CYC(b_+81, b_+84); SET_BC(0x187f);
  CALL_C(b_+84, ecom_randomBitwiseAndBCE_b0d_hook, SYM(ecom_randomBitwiseAndBCE_b0d), b_+87);
  CYC(b_+87, b_+89); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+89, b_+90); A = B;
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CYC(b_+91, b_+93); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+93, b_+95); A = 0x40;
  CYC(b_+95, b_+96); alu_add(gb, C);
  CYC(b_+96, b_+97); mem_wr(gb, DE, A);
  CYC(b_+97, b_+99); goto animate; // jr

state9:
  CALL_C(b_+99, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+102);
  if (F & FZ) { CYCT(b_+102, b_+104); goto gotoState8; } // jr z
  CYC(b_+102, b_+104);
  CALL_C(b_+104, ecom_applyVelocityForSideviewEnemyNoHoles_b0d_hook, SYM(ecom_applyVelocityForSideviewEnemyNoHoles_b0d), b_+107);
  if (F & FZ) { CYCT(b_+107, b_+109); goto gotoState8; } // jr z
  CYC(b_+107, b_+109);

animate:
  CYC(b_+109, b_+112); TAIL(enemyAnimate); // jp

stateA:
  CALL_C(b_+112, ecom_decCounter1_b0d_hook, SYM(ecom_decCounter1_b0d), b_+115);
  if (!(F & FZ)) { RET_TAKEN(b_+115); return; } // ret nz
  CYC(b_+115, b_+116);
  CYC(b_+116, b_+119); SET_BC(0x3102); // ENEMY_STALFOS, subid 2
  CYC(b_+119, b_+122); TAIL(enemyReplaceWithID); // jp

gotoState8:
  CYC(b_+122, b_+124); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+124, b_+126); A = 0x08;
  CYC(b_+126, b_+127); mem_wr(gb, DE, A);
  CYC(b_+127, (SYM(enemyCode13) + 1)); goto animate; // jr
}
