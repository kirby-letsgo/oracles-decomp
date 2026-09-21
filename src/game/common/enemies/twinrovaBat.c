#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void enemyCode5e_checkInBounds_hook(GB *gb);
void enemyCode5e_updateOamFlags_hook(GB *gb);

static uint16_t twinrovaBat_jump_table(GB *gb) {
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
// ENEMY_TWINROVA_BAT
// ==================================================================================================
void enemyCode5e_hook(GB *gb) {
  BASE(enemyCode5e);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto afterHealthCheck; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_uncounted_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);

afterHealthCheck:
  CYC(b_+8, b_+10); A = OBJ_ID; // Object.id
  CALL_C(b_+10, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_cp(gb, 0x01); // ENEMY_MERGED_TWINROVA
  if (!(F & FZ)) { CYCT(b_+16, b_+19); enemyDelete_hook(gb); return; } // jp nz
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); A = alu_inc8(gb, A);
  CYC(b_+23, b_+25); alu_and(gb, 0x1f);
  CYC(b_+25, b_+27); A = 0x78; // SND_BOOMERANG
  if (F & FZ) CALL_C_CC(b_+27, playSound_b00_hook, SYM(playSound_b00), b_+30); else CYC(b_+27, b_+30); // call z
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  {
    CYC(b_+33, b_+34); push_effect(gb, b_+34);
    uint16_t target = twinrovaBat_jump_table(gb);
    if (target == b_+40) goto state0;
    if (target == b_+65) goto state1;
    if (target == b_+78) goto state2;
    HANDOFF(target);
  }

state0:
  CYC(b_+40, b_+41); H = D;
  CYC(b_+41, b_+42); L = E;
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_SPEED;
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_COUNTER2;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x50);
  CALL_C(b_+51, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+54);
  CYC(b_+54, b_+56); E = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+59); A = 0xa8; // SND_VERAN_FAIRY_ATTACK
  CALL_C(b_+59, playSound_b00_hook, SYM(playSound_b00), b_+62);
  CYC(b_+62, b_+65); TAIL(objectSetVisible82); // jp

state1:
  CALL_C(b_+65, enemyCode5e_updateOamFlags_hook, b_+105, b_+68);
  CALL_C(b_+68, ecom_decCounter2_b0e_hook, SYM(ecom_decCounter2_b0e), b_+71);
  if (!(F & FZ)) { CYCT(b_+71, b_+73); goto animate; } // jr nz
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+74); L = E;
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CALL_C(b_+75, ecom_updateAngleTowardTarget_b0e_hook, SYM(ecom_updateAngleTowardTarget_b0e), b_+78);

state2:
  CALL_C(b_+78, enemyCode5e_checkInBounds_hook, b_+93, b_+81);
  if (!(F & FC)) { CYCT(b_+81, b_+84); enemyDelete_hook(gb); return; } // jp nc
  CYC(b_+81, b_+84);
  CALL_C(b_+84, enemyCode5e_updateOamFlags_hook, b_+105, b_+87);
  CALL_C(b_+87, objectApplySpeed_hook, SYM(objectApplySpeed), b_+90);

animate:
  CYC(b_+90, b_+93); TAIL(enemyAnimate); // jp
}

// 0e:6a36, bare local (no exported symbol); called via genuine call/ret from
// enemyCode5e@state2.
// @return cflag c if in bounds
void enemyCode5e_checkInBounds_hook(GB *gb) {
  BASE(enemyCode5e);
  CYC(b_+93, b_+95); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+95, b_+96); A = mem_rd(gb, DE);
  CYC(b_+96, b_+98); alu_cp(gb, 0xb0); // LARGE_ROOM_HEIGHT<<4
  if (!(F & FC)) { RET_TAKEN(b_+98); return; } // ret nc
  CYC(b_+98, b_+99);
  CYC(b_+99, b_+101); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+101, b_+102); A = mem_rd(gb, DE);
  CYC(b_+102, b_+104); alu_cp(gb, 0xf0); // LARGE_ROOM_WIDTH<<4
  RET(b_+104); return; // ret
}

// 0e:6a42, bare local (no exported symbol); called via genuine call/ret from
// enemyCode5e@state1 and @state2.
void enemyCode5e_updateOamFlags_hook(GB *gb) {
  BASE(enemyCode5e);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+105, ecom_decCounter1_b0e_hook, SYM(ecom_decCounter1_b0e), b_+108);
  CYC(b_+108, b_+109); A = mem_rd(gb, HL);
  CYC(b_+109, b_+111); alu_and(gb, 0x04);
  CYC(b_+111, b_+112); alu_rrca(gb);
  CYC(b_+112, b_+113); alu_rrca(gb);
  CYC(b_+113, b_+115); alu_add(gb, 0x02);
  CYC(b_+115, b_+117); L = ENEMY_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+117, b_+118); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+118, b_+119); mem_wr(gb, HL, A);
  RET(b_+119); return; // ret
}
