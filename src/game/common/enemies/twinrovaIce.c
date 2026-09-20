#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(enemyCode5d), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(enemyCode5d), (from), (to), true)

void twinrovaIce_bounceOffShield_hook(GB *gb);

static uint16_t twinrovaIce_jump_table(GB *gb) {
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

static void twinrovaIce_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// ==================================================================================================
// ENEMY_TWINROVA_ICE
//
// Variables:
//   var3e: ?
// ==================================================================================================
void enemyCode5d_hook(GB *gb) {
  BASE(enemyCode5d);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = ENEMY_BASE + OBJ_VAR2A;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (F & FZ) { CYCT(b_+10, b_+12); goto normalStatus; } // jr z
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); A = (uint8_t)(A & ~(1 << 7)); // res 7,a
  CYC(b_+14, b_+16); alu_sub(gb, 0x02); // ITEMCOLLISION_L2_SHIELD
  CYC(b_+16, b_+18); alu_cp(gb, 0x02); // ITEMCOLLISION_L3_SHIELD-ITEMCOLLISION_L2_SHIELD+1
  if (F & FC) CALL_C_CC(b_+18, twinrovaIce_bounceOffShield_hook, SYM(twinrovaIce_bounceOffShield), b_+21); else CYC(b_+18, b_+21); // call c
  CALL_C(b_+21, ecom_updateCardinalAngleAwayFromTarget_b0e_hook, SYM(ecom_updateCardinalAngleAwayFromTarget_b0e), b_+24);

normalStatus:
  CYC(b_+24, b_+26); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  {
    CYC(b_+27, b_+28); push_effect(gb, b_+28);
    uint16_t target = twinrovaIce_jump_table(gb);
    if (target == b_+34) goto state0;
    if (target == b_+57) goto state1;
    if (target == b_+65) goto state2;
    HANDOFF(target);
  }

state0:
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+36); L = E;
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x46); // SPEED_1c0
  CYC(b_+41, b_+43); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+43, b_+45); mem_wr(gb, HL, 120);
  CYC(b_+45, b_+47); L = ENEMY_BASE + 0x3e; // Enemy.var3e
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x08);
  CYC(b_+49, b_+51); A = 0x98; // SND_POOF
  CALL_C(b_+51, playSound_b00_hook, SYM(playSound_b00), b_+54);
  CYC(b_+54, b_+57); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(b_+57, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+60);
  if (!(F & FZ)) { CYCT(b_+60, b_+63); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+60, b_+63);
  CYC(b_+63, b_+64); L = E;
  CYC(b_+64, b_+65); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]

state2:
  CYC(b_+65, b_+67); A = OBJ_HEALTH; // Object.health
  CALL_C(b_+67, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+70);
  CYC(b_+70, b_+71); A = mem_rd(gb, HL);
  CYC(b_+71, b_+72); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+72, b_+74); goto delete; } // jr z
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+76); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+79); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+79, b_+81); goto delete; } // jr z
  CYC(b_+79, b_+81);
  CALL_C(b_+81, objectApplySpeed_hook, SYM(objectApplySpeed), b_+84);
  CALL_C(b_+84, ecom_bounceOffWallsAndHoles_b0e_hook, SYM(ecom_bounceOffWallsAndHoles_b0e), b_+87);
  if (F & FZ) { RET_TAKEN(b_+87); return; } // ret z
  CYC(b_+87, b_+88);
  CYC(b_+88, b_+90); A = 0x50; // SND_CLINK
  CYC(b_+90, b_+93); playSound_b00_hook(gb); return; // jp

delete:
  CALL_C(b_+93, objectCreatePuff_hook, SYM(objectCreatePuff), b_+96);
  CYC(b_+96, SYM(twinrovaIce_bounceOffShield)); enemyDelete_hook(gb); return; // jp
}

// 0e:6972, bare global; called from enemyCode5d. This doesn't appear to do anything other
// than make a sound, because the angle is immediately overwritten after this is called.
void twinrovaIce_bounceOffShield_hook(GB *gb) {
  BASE(twinrovaIce_bounceOffShield);
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link + OBJ_DIRECTION);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_add(gb, B);
  CYC(b_+10, b_+13); SET_HL(b_+23); // @bounceTable
  CYC(b_+13, b_+14); twinrovaIce_addAToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+16); E = ENEMY_BASE + OBJ_ANGLE;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+20); A = 0x50; // SND_CLINK
  CYC(b_+20, b_+23); playSound_b00_hook(gb); return; // jp
}
