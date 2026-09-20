#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void enemyCode79_hook(GB *gb);
void headThwomp_state_uninitialized_hook(GB *gb);
void headThwomp_state_stub_hook(GB *gb);
void headThwomp_state8_hook(GB *gb);
void headThwomp_state9_hook(GB *gb);
void headThwomp_stateA_hook(GB *gb);
void headThwomp_stateB_hook(GB *gb);
void headThwomp_stateC_hook(GB *gb);
void headThwomp_stateD_hook(GB *gb);
void headThwomp_stateE_hook(GB *gb);
void headThwomp_stateF_hook(GB *gb);
void headThwomp_poundGround_hook(GB *gb);
void headThwomp_state10_hook(GB *gb);
void headThwomp_state11_hook(GB *gb);
void headThwomp_setSolidTilesAroundSelf_hook(GB *gb);
void headThwomp_unsetSolidTilesAroundSelf_hook(GB *gb);
void headThwomp_rotate_hook(GB *gb);
void headThwomp_checkBombThrownIntoHead_hook(GB *gb);
void headThwomp_dropHeart_hook(GB *gb);
void headThwomp_checkShootProjectile_hook(GB *gb);

static uint16_t headThwomp_jump_table(GB *gb) {
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
// ENEMY_HEAD_THWOMP
//
// Variables:
//   direction: Current animation. Even numbers are face colors; odd numbers are
//              transitions.
//   var30: "Spin counter" used when bomb is thrown into head
//   var31: Which head the thwomp will settle on after throwing bomb in?
//   var32: Bit 0 triggers the effect of a bomb being thrown into head thwomp.
//   var33: Determines the initial angle of the circular projectiles' initial angle
//   var34: Counter which determines when head thwomp starts shooting fireballs / bombs
// ==================================================================================================

void enemyCode79_hook(GB *gb) {
  BASE(enemyCode79);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);

normalStatus:
  CYC(b_+8, b_+10); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  {
    uint16_t target = headThwomp_jump_table(gb);
    if (target == SYM(headThwomp_state_uninitialized)) { headThwomp_state_uninitialized_hook(gb); return; }
    if (target == SYM(headThwomp_state_stub)) { headThwomp_state_stub_hook(gb); return; }
    if (target == SYM(headThwomp_state8)) { headThwomp_state8_hook(gb); return; }
    if (target == SYM(headThwomp_state9)) { headThwomp_state9_hook(gb); return; }
    if (target == SYM(headThwomp_stateA)) { headThwomp_stateA_hook(gb); return; }
    if (target == SYM(headThwomp_stateB)) { headThwomp_stateB_hook(gb); return; }
    if (target == SYM(headThwomp_stateC)) { headThwomp_stateC_hook(gb); return; }
    if (target == SYM(headThwomp_stateD)) { headThwomp_stateD_hook(gb); return; }
    if (target == SYM(headThwomp_stateE)) { headThwomp_stateE_hook(gb); return; }
    if (target == SYM(headThwomp_stateF)) { headThwomp_stateF_hook(gb); return; }
    if (target == SYM(headThwomp_state10)) { headThwomp_state10_hook(gb); return; }
    if (target == SYM(headThwomp_state11)) { headThwomp_state11_hook(gb); return; }
    HANDOFF(target);
  }
}

void headThwomp_state_uninitialized_hook(GB *gb) {
  BASE(headThwomp_state_uninitialized);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x79; // ENEMY_HEAD_THWOMP
  CYC(b_+2, b_+4); B = 0x81; // PALH_81
  CALL_C(b_+4, enemyBoss_initializeRoom_b0f_hook, SYM(enemyBoss_initializeRoom_b0f), b_+7);
  CALL_C(b_+7, ecom_setSpeedAndState8_b0f_hook, SYM(ecom_setSpeedAndState8_b0f), b_+10);
  CYC(b_+10, b_+12); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x12); // 18
  CALL_C(b_+14, headThwomp_setSolidTilesAroundSelf_hook, SYM(headThwomp_setSolidTilesAroundSelf), b_+17);
  CYC(b_+17, b_+20); objectSetVisible80_hook(gb); return; // jp
}

void headThwomp_state_stub_hook(GB *gb) {
  BASE(headThwomp_state_stub);
  RET(b_+0); return; // ret
}

// Waiting for Link to move up for fight to start
void headThwomp_state8_hook(GB *gb) {
  BASE(headThwomp_state8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Link_yh);
  CYC(b_+3, b_+5); alu_cp(gb, 0x9c);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); C = 0xa4;
  CYC(b_+8, b_+10); A = 0x3d;
  CALL_C(b_+10, setTile_hook, SYM(setTile), b_+13);
  CYC(b_+13, b_+15); A = 0x70; // SND_DOORCLOSE
  CALL_C(b_+15, playSound_b00_hook, SYM(playSound_b00), b_+18);
  CYC(b_+18, b_+20); A = 0x98;
  CYC(b_+20, b_+23); mem_wr(gb, wLinkLocalRespawnY, A);
  CYC(b_+23, b_+25); A = 0x48;
  CYC(b_+25, b_+28); mem_wr(gb, wLinkLocalRespawnX, A);
  CALL_C(b_+28, ecom_incState_b0f_hook, SYM(ecom_incState_b0f), b_+31);
  CYC(b_+31, b_+33); L = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0xf0);
  CALL_C(b_+35, enemyBoss_beginBoss_b0f_hook, SYM(enemyBoss_beginBoss_b0f), SYM(headThwomp_state9));
  headThwomp_state9_hook(gb); return; // fallthrough
}

// Spinning normally
void headThwomp_state9_hook(GB *gb) {
  BASE(headThwomp_state9);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, headThwomp_checkBombThrownIntoHead_hook, SYM(headThwomp_checkBombThrownIntoHead), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CALL_C(b_+4, headThwomp_checkShootProjectile_hook, SYM(headThwomp_checkShootProjectile), b_+7);
  CALL_C(b_+7, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; } // ret nz
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  CYC(b_+15, b_+18); SET_BC(b_+43); // @rotationSpeeds
  CALL_C(b_+18, addDoubleIndexToBc_hook, 0x007e, b_+21);
  CYC(b_+21, b_+23); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); A = alu_inc8(gb, A);
  CYC(b_+25, b_+27); alu_and(gb, 0x07);
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+29, b_+31); goto L_683e; } // jr nc
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+32); SET_BC(BC + 1); // inc bc

L_683e:
  CYC(b_+32, b_+33); A = mem_rd(gb, BC);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+36); A = 0x58; // SND_CLINK2
  if (F & FC) CALL_C_CC(b_+36, playSound_b00_hook, SYM(playSound_b00), b_+39); else CYC(b_+36, b_+39); // call c
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+43); enemySetAnimation_hook(gb); return; // jp
}

// Bomb just thrown into head thwomp
void headThwomp_stateA_hook(GB *gb) {
  BASE(headThwomp_stateA);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x00); // [substate]
  RET(b_+11); return; // ret
}

// Spinning after bomb was thrown into head
void headThwomp_stateB_hook(GB *gb) {
  BASE(headThwomp_stateB);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = headThwomp_jump_table(gb);
    if (target == b_+11) goto substate0;
    if (target == b_+17) goto substate1;
    if (target == b_+37) goto substate2;
    if (target == b_+75) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x3c); // [counter1] = 60

substate1:
  CALL_C(b_+17, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+20);
  CYC(b_+20, b_+22); B = 0x08;
  if (!(F & FZ)) { CYCT(b_+22, b_+25); headThwomp_rotate_hook(gb); return; } // jp nz
  CYC(b_+22, b_+25);
  CYC(b_+25, b_+26); L = E;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x01); // [counter1]
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x02); // [counter2]
  CYC(b_+33, b_+35); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x01);

substate2:
  CALL_C(b_+37, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+40);
  if (!(F & FZ)) { RET_TAKEN(b_+40); return; } // ret nz
  CYC(b_+40, b_+41);
  CYC(b_+41, b_+42); L = alu_inc8(gb, L);
  CYC(b_+42, b_+43); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter2]
  if (!(F & FZ)) { CYCT(b_+43, b_+45); goto L_6894; } // jr nz
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x02);
  CYC(b_+47, b_+49); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+49, b_+50); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+50, b_+51); A = mem_rd(gb, HL);
  CYC(b_+51, b_+53); alu_cp(gb, 0x12);
  if (!(F & FC)) { CYCT(b_+53, b_+55); goto startSlowestSpinning; } // jr nc
  CYC(b_+53, b_+55);

L_6894:
  CYC(b_+55, b_+57); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(b_+57, b_+58); A = mem_rd(gb, HL);
  CYC(b_+58, b_+60); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+60, b_+61); mem_wr(gb, HL, A);
  CYC(b_+61, b_+63); B = 0x08;
  CYC(b_+63, b_+66); headThwomp_rotate_hook(gb); return; // jp

startSlowestSpinning:
  CYC(b_+66, b_+68); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x01);
  CYC(b_+70, b_+71); L = alu_inc8(gb, L);
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x06); // [counter2]
  CYC(b_+73, b_+74); L = E;
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]

substate3:
  CALL_C(b_+75, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+78);
  if (!(F & FZ)) { RET_TAKEN(b_+78); return; } // ret nz
  CYC(b_+78, b_+79);
  CYC(b_+79, b_+80); L = alu_inc8(gb, L);
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+83); alu_add(gb, 0x0c);
  CYC(b_+83, b_+84); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a [counter2]
  CYC(b_+84, b_+85); mem_wr(gb, HL, A); // [counter1]
  CYC(b_+85, b_+87); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+87, b_+88); A = mem_rd(gb, HL);
  CYC(b_+88, b_+90); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+90, b_+91); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+91, b_+93); B = 0x08;
  if (!(F & FZ)) { CYCT(b_+93, b_+96); headThwomp_rotate_hook(gb); return; } // jp nz
  CYC(b_+93, b_+96);
  CYC(b_+96, b_+98); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+98, b_+99); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+99, b_+101); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x10);
  RET(b_+103); return; // ret
}

// Just reached the target head color
void headThwomp_stateC_hook(GB *gb) {
  BASE(headThwomp_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); A = alu_srl(gb, A);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  CYC(b_+10, b_+11); L = E;
  CYC(b_+11, b_+12); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x00); // [substate]
  RET(b_+16); return; // ret
}

// Green face (shoots fireballs)
void headThwomp_stateD_hook(GB *gb) {
  BASE(headThwomp_stateD);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto substate1; } // jr nz
  CYC(b_+3, b_+5);

  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0xf0); // [counter1]
  CYC(b_+11, b_+14); SET_HL(wRoomCollisions + 0x47);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x00);
  RET(b_+16); return; // ret

substate1:
  CALL_C(b_+17, headThwomp_checkBombThrownIntoHead_hook, SYM(headThwomp_checkBombThrownIntoHead), b_+20);
  if (!(F & FZ)) { RET_TAKEN(b_+20); return; } // ret nz
  CYC(b_+20, b_+21);
  CALL_C(b_+21, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+24);
  if (F & FZ) { CYCT(b_+24, b_+26); goto resumeSpinning; } // jr z
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_cp(gb, 0xd2); // 210
  if (!(F & FC)) CALL_C_CC(b_+29, enemyAnimate_hook, SYM(enemyAnimate), b_+32); else CYC(b_+29, b_+32); // call nc
  CYC(b_+32, b_+34); E = 0xc6; // Part.counter2
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+37); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(b_+37); return; } // ret nz
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); B = 0x39; // PART_HEAD_THWOMP_FIREBALL
  CYC(b_+40, b_+43); ecom_spawnProjectile_b0f_hook(gb); return; // jp

resumeSpinning:
  CYC(b_+43, b_+45); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x11);
  CYC(b_+47, b_+49); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x01);
  RET(b_+51); return; // ret
}

// Blue face (fires circular projectiles)
void headThwomp_stateE_hook(GB *gb) {
  BASE(headThwomp_stateE);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = headThwomp_jump_table(gb);
    if (target == b_+11) goto substate0;
    if (target == b_+37) goto substate1;
    if (target == b_+81) goto substate2;
    if (target == b_+92) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+13); L = E;
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+17); A = 0x08;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a [counter1]
  CYC(b_+18, b_+19); mem_wr(gb, HL, A); // [counter2]
  CALL_C(b_+19, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+22);
  CYC(b_+22, b_+24); alu_and(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+24, b_+26); goto L_6926; } // jr nz
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); A = 0xfe;

L_6926:
  CYC(b_+28, b_+30); E = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+34); SET_HL(wRoomCollisions + 0x47);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x00);
  RET(b_+36); return; // ret

substate1:
  CALL_C(b_+37, headThwomp_checkBombThrownIntoHead_hook, SYM(headThwomp_checkBombThrownIntoHead), b_+40);
  if (!(F & FZ)) { RET_TAKEN(b_+40); return; } // ret nz
  CYC(b_+40, b_+41);
  CALL_C(b_+41, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+44);
  if (!(F & FZ)) { CYCT(b_+44, b_+47); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(b_+49, b_+51); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+51, b_+52); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+52, b_+55); SET_HL(wRoomCollisions + 0x47);
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x03);
  CALL_C(b_+57, getFreePartSlot_hook, SYM(getFreePartSlot), b_+60);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto L_6955; } // jr nz
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x3c); // PART_HEAD_THWOMP_CIRCULAR_PROJECTILE
  CYC(b_+64, b_+65); L = alu_inc8(gb, L);
  CYC(b_+65, b_+67); E = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  CYC(b_+68, b_+69); mem_wr(gb, HL, A); // [part.subid]
  CYC(b_+69, b_+72); SET_BC(0xf800);
  CALL_C(b_+72, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+75);

L_6955:
  CYC(b_+75, b_+77); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+77, b_+78); A = mem_rd(gb, DE);
  CYC(b_+78, b_+81); enemySetAnimation_hook(gb); return; // jp

substate2:
  CALL_C(b_+81, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+84);
  if (!(F & FZ)) { CYCT(b_+84, b_+87); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+84, b_+87);
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x1e); // [counter1] = 30
  CYC(b_+89, b_+90); L = E;
  CYC(b_+90, b_+91); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  RET(b_+91); return; // ret

substate3:
  CALL_C(b_+92, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+95);
  if (!(F & FZ)) { RET_TAKEN(b_+95); return; } // ret nz
  CYC(b_+95, b_+96);
  CYC(b_+96, b_+97); L = alu_inc8(gb, L);
  CYC(b_+97, b_+98); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter2]
  if (F & FZ) { CYCT(b_+98, b_+100); goto stateEResumeSpinning; } // jr z
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+101); L = E;
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x01); // [substate]
  CYC(b_+103, b_+105); A = 0x08;
  CYCT(b_+105, b_+107); goto L_697b; // jr

stateEResumeSpinning:
  CYC(b_+107, b_+109); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x11);
  CYC(b_+111, b_+113); A = 0x10;

L_697b:
  CYC(b_+113, b_+115); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+115, b_+116); mem_wr(gb, HL, A);
  CYC(b_+116, b_+119); SET_HL(wRoomCollisions + 0x47);
  CYC(b_+119, b_+121); mem_wr(gb, HL, 0x00);
  CYC(b_+121, b_+123); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+123, b_+124); A = mem_rd(gb, DE);
  CYC(b_+124, b_+126); alu_add(gb, 0x08);
  CYC(b_+126, b_+129); enemySetAnimation_hook(gb); return; // jp
}

// Purple face (stomps the ground)
void headThwomp_stateF_hook(GB *gb) {
  BASE(headThwomp_stateF);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  {
    uint16_t target = headThwomp_jump_table(gb);
    if (target == b_+13) goto substate0;
    if (target == b_+25) goto substate1;
    if (target == b_+53) goto substate2;
    if (target == b_+75) goto substate3;
    if (target == b_+92) goto substate4;
    HANDOFF(target);
  }

substate0:
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+15); L = E;
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+16, b_+18); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+20); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x02);
  CYC(b_+22, b_+25); headThwomp_unsetSolidTilesAroundSelf_hook(gb); return; // jp

substate1:
  CYC(b_+25, b_+27); A = 0x20;
  CALL_C(b_+27, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+30);
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+35); alu_cp(gb, 0x90);
  if (F & FC) { RET_TAKEN(b_+35); return; } // ret c
  CYC(b_+35, b_+36);
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(b_+39, b_+40); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x78); // [counter1] = 120
  headThwomp_poundGround_hook(gb); return; // fallthrough

substate2:
  CALL_C(b_+53, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+56);
  if (F & FZ) { CYCT(b_+56, b_+58); goto beginMovingUp; } // jr z
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL);
  CYC(b_+59, b_+61); alu_cp(gb, 0x1e); // 30
  if (F & FC) { RET_TAKEN(b_+61); return; } // ret c
  CYC(b_+61, b_+62);
  CYC(b_+62, b_+64); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+64); return; } // ret nz
  CYC(b_+64, b_+65);
  CALL_C(b_+65, getFreePartSlot_hook, SYM(getFreePartSlot), b_+68);
  if (!(F & FZ)) { RET_TAKEN(b_+68); return; } // ret nz
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x3b); // PART_3b
  RET(b_+71); return; // ret

beginMovingUp:
  CYC(b_+72, b_+73); L = E;
  CYC(b_+73, b_+74); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  RET(b_+74); return; // ret

substate3:
  CYC(b_+75, b_+76); H = D;
  CYC(b_+76, b_+78); L = ENEMY_BASE + OBJ_Y;
  CYC(b_+78, b_+79); A = mem_rd(gb, HL);
  CYC(b_+79, b_+81); alu_sub(gb, 0x80);
  CYC(b_+81, b_+82); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+82, b_+83); A = mem_rd(gb, HL);
  CYC(b_+83, b_+85); alu_sbc(gb, 0x00);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A);
  CYC(b_+86, b_+88); alu_cp(gb, 0x56);
  if (!(F & FZ)) { RET_TAKEN(b_+88); return; } // ret nz
  CYC(b_+88, b_+89);
  CYC(b_+89, b_+90); L = E;
  CYC(b_+90, b_+91); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  RET(b_+91); return; // ret

substate4:
  CYC(b_+92, b_+93); H = D;
  CYC(b_+93, b_+95); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+95, b_+98); A = mem_rd(gb, w1Link_yh);
  CYC(b_+98, b_+99); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+99, b_+101); alu_add(gb, 0x10);
  CYC(b_+101, b_+103); alu_cp(gb, 0x21);
  if (!(F & FC)) { CYCT(b_+103, b_+105); goto setSolidity; } // jr nc
  CYC(b_+103, b_+105);
  CYC(b_+105, b_+107); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+107, b_+110); A = mem_rd(gb, w1Link_xh);
  CYC(b_+110, b_+111); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+111, b_+113); alu_add(gb, 0x10);
  CYC(b_+113, b_+115); alu_cp(gb, 0x21);
  if (F & FC) { RET_TAKEN(b_+115); return; } // ret c
  CYC(b_+115, b_+116);

setSolidity:
  CYC(b_+116, b_+118); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+118, b_+120); mem_wr(gb, HL, 0x11);
  CYC(b_+120, b_+122); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+122, b_+124); mem_wr(gb, HL, 0x10);
  CYC(b_+124, b_+127); headThwomp_setSolidTilesAroundSelf_hook(gb); return; // jp
}

// Also used by the death sequence.
void headThwomp_poundGround_hook(GB *gb) {
  BASE(headThwomp_stateF);
  CYC(b_+43, b_+45); A = 0x3c;
  CYC(b_+45, b_+48); mem_wr(gb, wScreenShakeCounterY, A);
  CYC(b_+48, b_+50); A = 0x81; // SND_STRONG_POUND
  CYC(b_+50, b_+53); playSound_b00_hook(gb); return; // jp
}

// Red face (takes damage)
void headThwomp_state10_hook(GB *gb) {
  BASE(headThwomp_state10);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); E = alu_inc8(gb, E);
  CYC(b_+1, b_+2); A = mem_rd(gb, DE);
  CYC(b_+2, b_+3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto substate1; } // jr nz
  CYC(b_+3, b_+5);

  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+7); L = E;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CYC(b_+11, b_+13); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x18);
  CYC(b_+15, b_+17); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto L_6a27; } // jr nz
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CALL_C(b_+21, headThwomp_unsetSolidTilesAroundSelf_hook, SYM(headThwomp_unsetSolidTilesAroundSelf), b_+24);
  CYC(b_+24, b_+26); A = 0x01; // TREE_GFXH_01
  CYC(b_+26, b_+29); mem_wr(gb, wLoadedTreeGfxIndex, A);

L_6a27:
  CYC(b_+29, b_+31); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); A = alu_inc8(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+33, headThwomp_dropHeart_hook, SYM(headThwomp_dropHeart), b_+36); else CYC(b_+33, b_+36); // call nz
  CYC(b_+36, b_+38); A = 0x10;
  CALL_C(b_+38, enemySetAnimation_hook, SYM(enemySetAnimation), b_+41);
  CYC(b_+41, b_+43); A = 0x63; // SND_BOSS_DAMAGE
  CYC(b_+43, b_+46); playSound_b00_hook(gb); return; // jp

substate1:
  CALL_C(b_+46, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+49);
  if (F & FZ) { CYCT(b_+49, b_+51); goto state10ResumeSpinning; } // jr z
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+55); return; } // ret nz
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0xff);
  CYC(b_+58, b_+60); A = 0x20;
  CALL_C(b_+60, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+63);
  CYC(b_+63, b_+65); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+68); alu_cp(gb, 0x90);
  if (F & FC) { RET_TAKEN(b_+68); return; } // ret c
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+70); H = D;
  CYC(b_+70, b_+72); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x00);
  CYC(b_+74, b_+77); headThwomp_poundGround_hook(gb); return; // jp

state10ResumeSpinning:
  CYC(b_+77, b_+79); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x11);
  CYC(b_+81, b_+83); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x10);
  CYC(b_+85, b_+88); SET_HL(wRoomCollisions + 0x47);
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0x00);
  CYC(b_+90, b_+92); A = 0x0e;
  CYC(b_+92, b_+95); enemySetAnimation_hook(gb); return; // jp
}

void headThwomp_state11_hook(GB *gb) {
  BASE(headThwomp_state11);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); enemyAnimate_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [counter1]
  CYC(b_+7, b_+8); L = E;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x09); // [state]
  CYC(b_+10, b_+12); L = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0xf0);
  RET(b_+14); return; // ret
}

void headThwomp_setSolidTilesAroundSelf_hook(GB *gb) {
  BASE(headThwomp_setSolidTilesAroundSelf);
  CYC(b_+0, b_+3); SET_HL(wRoomCollisions + 0x46);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x01);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x02);
  CYC(b_+9, b_+10); A = L;
  CYC(b_+10, b_+12); alu_add(gb, 0x0e);
  CYC(b_+12, b_+13); L = A;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x05);
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x0f);
  CYC(b_+18, b_+19); L = alu_inc8(gb, L);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x0a);
  RET(b_+21); return; // ret
}

void headThwomp_unsetSolidTilesAroundSelf_hook(GB *gb) {
  BASE(headThwomp_unsetSolidTilesAroundSelf);
  CYC(b_+0, b_+3); SET_HL(wRoomCollisions + 0x46);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  CYC(b_+7, b_+9); L = 0x56;
  CYC(b_+9, b_+10); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  RET(b_+12); return; // ret
}

// @param b Animation base
void headThwomp_rotate_hook(GB *gb) {
  BASE(headThwomp_rotate);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x07);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CYC(b_+7, b_+8); alu_add(gb, B);
  CALL_C(b_+8, enemySetAnimation_hook, SYM(enemySetAnimation), b_+11);
  CYC(b_+11, b_+13); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+15); return; } // ret c
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+18); A = 0x58; // SND_CLINK2
  CYC(b_+18, b_+21); playSound_b00_hook(gb); return; // jp
}

// If a bomb is thrown into head thwomp, this sets the state to $0a.
// @param[out] zflag z if no bomb entered head thwomp
void headThwomp_checkBombThrownIntoHead_hook(GB *gb) {
  BASE(headThwomp_checkBombThrownIntoHead);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(w4Filler5); // FIRST_DYNAMIC_ITEM_INDEX, Item.start

itemLoop:
  CYC(b_+3, b_+5); L = OBJ_ID;
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x03); // ITEM_BOMB
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto nextItem; } // jr nz
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); L = OBJ_STATE;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+13, b_+14); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+14, b_+16); goto isNonExplodingBomb; } // jr z
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+19, b_+21); goto nextItem; } // jr c
  CYC(b_+19, b_+21);

isNonExplodingBomb:
  CYC(b_+21, b_+23); L = OBJ_YH;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+24, b_+26); alu_sub(gb, 0x50);
  CYC(b_+26, b_+28); alu_add(gb, 0x0c);
  CYC(b_+28, b_+30); alu_cp(gb, 0x19);
  if (!(F & FC)) { CYCT(b_+30, b_+32); goto nextItem; } // jr nc
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); alu_sub(gb, 0x78);
  CYC(b_+36, b_+38); alu_add(gb, 0x0c);
  CYC(b_+38, b_+40); alu_cp(gb, 0x19);
  if (F & FC) { CYCT(b_+40, b_+42); goto bombEnteredThwomp; } // jr c
  CYC(b_+40, b_+42);

nextItem:
  CYC(b_+42, b_+43); H = alu_inc8(gb, H);
  CYC(b_+43, b_+44); A = H;
  CYC(b_+44, b_+46); alu_cp(gb, 0xdc); // LAST_DYNAMIC_ITEM_INDEX+1
  if (F & FC) { CYCT(b_+46, b_+48); goto itemLoop; } // jr c
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+50); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_rrca(gb);
  if (F & FC) { CYCT(b_+52, b_+54); goto triggerBombEffect; } // jr c
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+55); alu_xor(gb, A);
  RET(b_+55); return; // ret

bombEnteredThwomp:
  CYC(b_+56, b_+58); L = OBJ_VAR2F;
  CYC(b_+58, b_+60); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)

triggerBombEffect:
  CYC(b_+60, b_+61); H = D;
  CYC(b_+61, b_+63); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+63, b_+65); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)
  CYC(b_+65, b_+67); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  CYC(b_+68, b_+70); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+70, b_+72); goto betweenTwoHeads; } // jr nz
  CYC(b_+70, b_+72);
  CYC(b_+72, b_+74); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(b_+74, b_+75); mem_wr(gb, HL, A);
  CYC(b_+75, b_+77); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x00);
  CYC(b_+79, b_+81); L = ENEMY_BASE + OBJ_STATE;
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x0a);
  CYC(b_+83, b_+85); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(b_+85, b_+87); mem_wr(gb, HL, 0x06);
  CALL_C(b_+87, enemySetAnimation_hook, SYM(enemySetAnimation), b_+90);
  CYC(b_+90, b_+93); SET_HL(wRoomCollisions + 0x47);
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x03);
  CYC(b_+95, b_+96); alu_or(gb, D);
  RET(b_+96); return; // ret

betweenTwoHeads:
  CALL_C(b_+97, ecom_decCounter1_b0f_hook, SYM(ecom_decCounter1_b0f), b_+100);
  if (!(F & FZ)) { RET_TAKEN(b_+100); return; } // ret nz
  CYC(b_+100, b_+101);
  CYC(b_+101, b_+103); B = 0x00;
  CALL_C(b_+103, headThwomp_rotate_hook, SYM(headThwomp_rotate), b_+106);
  CYCT(b_+106, b_+108); goto triggerBombEffect; // jr (backward)
}

void headThwomp_dropHeart_hook(GB *gb) {
  BASE(headThwomp_dropHeart);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x01); // PART_ITEM_DROP
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x01); // ITEM_DROP_HEART
  CYC(b_+9, b_+12); SET_BC(0x1400);
  CYC(b_+12, b_+15); objectCopyPositionWithOffset_hook(gb); return; // jp
}

void headThwomp_checkShootProjectile_hook(GB *gb) {
  BASE(headThwomp_checkShootProjectile);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+4); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto L_6b38; } // jr nz
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0xf0);

L_6b38:
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_cp(gb, 0x5a); // 90
  if (!(F & FC)) { RET_TAKEN(b_+16); return; } // ret nc
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+19); return; } // ret nz
  CYC(b_+19, b_+20);
  CALL_C(b_+20, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+23);
  CYC(b_+23, b_+25); alu_and(gb, 0x07);
  if (F & FZ) { CYCT(b_+25, b_+27); goto dropBomb; } // jr z
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); B = 0x39; // PART_HEAD_THWOMP_FIREBALL
  CYC(b_+29, b_+32); ecom_spawnProjectile_b0f_hook(gb); return; // jp

dropBomb:
  CYC(b_+32, b_+34); B = 0x02;
  CALL_C(b_+34, checkBPartSlotsAvailable_hook, SYM(checkBPartSlotsAvailable), b_+37);
  if (!(F & FZ)) { RET_TAKEN(b_+37); return; } // ret nz
  CYC(b_+37, b_+38);
  CALL_C(b_+38, getFreePartSlot_hook, SYM(getFreePartSlot), b_+41);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x01); // PART_ITEM_DROP
  CYC(b_+43, b_+44); L = alu_inc8(gb, L);
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x04); // ITEM_DROP_BOMBS
  CALL_C(b_+46, objectCopyPosition_hook, SYM(objectCopyPosition), b_+49);
  CYC(b_+49, b_+50); B = H;
  CALL_C(b_+50, getFreePartSlot_hook, SYM(getFreePartSlot), b_+53);
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x40); // PART_HEAD_THWOMP_BOMB_DROPPER
  CYC(b_+55, b_+57); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+57, b_+59); A = 0xc0; // Part.start
  CYC(b_+59, b_+60); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(b_+60, b_+61); mem_wr(gb, HL, B);
  CYC(b_+61, b_+64); objectCopyPosition_hook(gb); return; // jp
}
