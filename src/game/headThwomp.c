#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0f, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x67b3, 0x67b5); goto normalStatus; } // jr z
  CYC(0x67b3, 0x67b5);
  CYC(0x67b5, 0x67b7); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x67b7); return; } // ret c
  CYC(0x67b7, 0x67b8);
  if (F & FZ) { CYCT(0x67b8, 0x67bb); enemyBoss_dead_b0f_hook(gb); return; } // jp z
  CYC(0x67b8, 0x67bb);

normalStatus:
  CYC(0x67bb, 0x67bd); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x67bd, 0x67be); A = mem_rd(gb, DE);
  CYC(0x67be, 0x67bf); push_effect(gb, 0x67bf);
  {
    uint16_t target = headThwomp_jump_table(gb);
    if (target == 0x67e3) { headThwomp_state_uninitialized_hook(gb); return; }
    if (target == 0x67f7) { headThwomp_state_stub_hook(gb); return; }
    if (target == 0x67f8) { headThwomp_state8_hook(gb); return; }
    if (target == 0x681e) { headThwomp_state9_hook(gb); return; }
    if (target == 0x6851) { headThwomp_stateA_hook(gb); return; }
    if (target == 0x685d) { headThwomp_stateB_hook(gb); return; }
    if (target == 0x68c5) { headThwomp_stateC_hook(gb); return; }
    if (target == 0x68d6) { headThwomp_stateD_hook(gb); return; }
    if (target == 0x690a) { headThwomp_stateE_hook(gb); return; }
    if (target == 0x698b) { headThwomp_stateF_hook(gb); return; }
    if (target == 0x6a0a) { headThwomp_state10_hook(gb); return; }
    if (target == 0x6a69) { headThwomp_state11_hook(gb); return; }
    HANDOFF(target);
  }
}

void headThwomp_state_uninitialized_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x67e3, 0x67e5); A = 0x79; // ENEMY_HEAD_THWOMP
  CYC(0x67e5, 0x67e7); B = 0x81; // PALH_81
  CALL_C(0x67e7, enemyBoss_initializeRoom_b0f_hook, 0x4546, 0x67ea);
  CALL_C(0x67ea, ecom_setSpeedAndState8_b0f_hook, 0x4364, 0x67ed);
  CYC(0x67ed, 0x67ef); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x67ef, 0x67f1); mem_wr(gb, HL, 0x12); // 18
  CALL_C(0x67f1, headThwomp_setSolidTilesAroundSelf_hook, 0x6a78, 0x67f4);
  CYC(0x67f4, 0x67f7); objectSetVisible80_hook(gb); return; // jp
}

void headThwomp_state_stub_hook(GB *gb) {
  RET(0x67f7); return; // ret
}

// Waiting for Link to move up for fight to start
void headThwomp_state8_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x67f8, 0x67fb); A = mem_rd(gb, w1Link_yh);
  CYC(0x67fb, 0x67fd); alu_cp(gb, 0x9c);
  if (!(F & FC)) { RET_TAKEN(0x67fd); return; } // ret nc
  CYC(0x67fd, 0x67fe);
  CYC(0x67fe, 0x6800); C = 0xa4;
  CYC(0x6800, 0x6802); A = 0x3d;
  CALL_C(0x6802, setTile_hook, 0x3a9c, 0x6805);
  CYC(0x6805, 0x6807); A = 0x70; // SND_DOORCLOSE
  CALL_C(0x6807, playSound_b00_hook, 0x0c98, 0x680a);
  CYC(0x680a, 0x680c); A = 0x98;
  CYC(0x680c, 0x680f); mem_wr(gb, wLinkLocalRespawnY, A);
  CYC(0x680f, 0x6811); A = 0x48;
  CYC(0x6811, 0x6814); mem_wr(gb, wLinkLocalRespawnX, A);
  CALL_C(0x6814, ecom_incState_b0f_hook, 0x4000, 0x6817);
  CYC(0x6817, 0x6819); L = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(0x6819, 0x681b); mem_wr(gb, HL, 0xf0);
  CALL_C(0x681b, enemyBoss_beginBoss_b0f_hook, 0x4584, 0x681e);
  headThwomp_state9_hook(gb); return; // fallthrough
}

// Spinning normally
void headThwomp_state9_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x681e, headThwomp_checkBombThrownIntoHead_hook, 0x6ab0, 0x6821);
  if (!(F & FZ)) { RET_TAKEN(0x6821); return; } // ret nz
  CYC(0x6821, 0x6822);
  CALL_C(0x6822, headThwomp_checkShootProjectile_hook, 0x6b2b, 0x6825);
  CALL_C(0x6825, ecom_decCounter1_b0f_hook, 0x439a, 0x6828);
  if (!(F & FZ)) { RET_TAKEN(0x6828); return; } // ret nz
  CYC(0x6828, 0x6829);
  CYC(0x6829, 0x682b); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x682b, 0x682c); A = mem_rd(gb, DE);
  CYC(0x682c, 0x682d); A = alu_dec8(gb, A);
  CYC(0x682d, 0x6830); SET_BC(0x6849); // @rotationSpeeds
  CALL_C(0x6830, addDoubleIndexToBc_hook, 0x007e, 0x6833);
  CYC(0x6833, 0x6835); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6835, 0x6836); A = mem_rd(gb, DE);
  CYC(0x6836, 0x6837); A = alu_inc8(gb, A);
  CYC(0x6837, 0x6839); alu_and(gb, 0x07);
  CYC(0x6839, 0x683a); mem_wr(gb, DE, A);
  CYC(0x683a, 0x683b); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x683b, 0x683d); goto L_683e; } // jr nc
  CYC(0x683b, 0x683d);
  CYC(0x683d, 0x683e); SET_BC(BC + 1); // inc bc

L_683e:
  CYC(0x683e, 0x683f); A = mem_rd(gb, BC);
  CYC(0x683f, 0x6840); mem_wr(gb, HL, A);
  CYC(0x6840, 0x6842); A = 0x58; // SND_CLINK2
  if (F & FC) CALL_C_CC(0x6842, playSound_b00_hook, 0x0c98, 0x6845); else CYC(0x6842, 0x6845); // call c
  CYC(0x6845, 0x6846); A = mem_rd(gb, DE);
  CYC(0x6846, 0x6849); enemySetAnimation_hook(gb); return; // jp
}

// Bomb just thrown into head thwomp
void headThwomp_stateA_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6851, ecom_decCounter1_b0f_hook, 0x439a, 0x6854);
  if (!(F & FZ)) { CYCT(0x6854, 0x6857); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x6854, 0x6857);
  CYC(0x6857, 0x6858); L = E;
  CYC(0x6858, 0x6859); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [state]
  CYC(0x6859, 0x685a); L = alu_inc8(gb, L);
  CYC(0x685a, 0x685c); mem_wr(gb, HL, 0x00); // [substate]
  RET(0x685c); return; // ret
}

// Spinning after bomb was thrown into head
void headThwomp_stateB_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x685d, 0x685e); E = alu_inc8(gb, E);
  CYC(0x685e, 0x685f); A = mem_rd(gb, DE);
  CYC(0x685f, 0x6860); push_effect(gb, 0x6860);
  {
    uint16_t target = headThwomp_jump_table(gb);
    if (target == 0x6868) goto substate0;
    if (target == 0x686e) goto substate1;
    if (target == 0x6882) goto substate2;
    if (target == 0x68a8) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(0x6868, 0x6869); H = D;
  CYC(0x6869, 0x686a); L = E;
  CYC(0x686a, 0x686b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x686b, 0x686c); L = alu_inc8(gb, L);
  CYC(0x686c, 0x686e); mem_wr(gb, HL, 0x3c); // [counter1] = 60

substate1:
  CALL_C(0x686e, ecom_decCounter1_b0f_hook, 0x439a, 0x6871);
  CYC(0x6871, 0x6873); B = 0x08;
  if (!(F & FZ)) { CYCT(0x6873, 0x6876); headThwomp_rotate_hook(gb); return; } // jp nz
  CYC(0x6873, 0x6876);
  CYC(0x6876, 0x6877); L = E;
  CYC(0x6877, 0x6878); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x6878, 0x6879); L = alu_inc8(gb, L);
  CYC(0x6879, 0x687b); mem_wr(gb, HL, 0x01); // [counter1]
  CYC(0x687b, 0x687c); L = alu_inc8(gb, L);
  CYC(0x687c, 0x687e); mem_wr(gb, HL, 0x02); // [counter2]
  CYC(0x687e, 0x6880); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6880, 0x6882); mem_wr(gb, HL, 0x01);

substate2:
  CALL_C(0x6882, ecom_decCounter1_b0f_hook, 0x439a, 0x6885);
  if (!(F & FZ)) { RET_TAKEN(0x6885); return; } // ret nz
  CYC(0x6885, 0x6886);
  CYC(0x6886, 0x6887); L = alu_inc8(gb, L);
  CYC(0x6887, 0x6888); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter2]
  if (!(F & FZ)) { CYCT(0x6888, 0x688a); goto L_6894; } // jr nz
  CYC(0x6888, 0x688a);
  CYC(0x688a, 0x688c); mem_wr(gb, HL, 0x02);
  CYC(0x688c, 0x688e); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x688e, 0x688f); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x688f, 0x6890); A = mem_rd(gb, HL);
  CYC(0x6890, 0x6892); alu_cp(gb, 0x12);
  if (!(F & FC)) { CYCT(0x6892, 0x6894); goto startSlowestSpinning; } // jr nc
  CYC(0x6892, 0x6894);

L_6894:
  CYC(0x6894, 0x6896); L = ENEMY_BASE + 0x30; // Enemy.var30
  CYC(0x6896, 0x6897); A = mem_rd(gb, HL);
  CYC(0x6897, 0x6899); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6899, 0x689a); mem_wr(gb, HL, A);
  CYC(0x689a, 0x689c); B = 0x08;
  CYC(0x689c, 0x689f); headThwomp_rotate_hook(gb); return; // jp

startSlowestSpinning:
  CYC(0x689f, 0x68a1); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x68a1, 0x68a3); mem_wr(gb, HL, 0x01);
  CYC(0x68a3, 0x68a4); L = alu_inc8(gb, L);
  CYC(0x68a4, 0x68a6); mem_wr(gb, HL, 0x06); // [counter2]
  CYC(0x68a6, 0x68a7); L = E;
  CYC(0x68a7, 0x68a8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]

substate3:
  CALL_C(0x68a8, ecom_decCounter1_b0f_hook, 0x439a, 0x68ab);
  if (!(F & FZ)) { RET_TAKEN(0x68ab); return; } // ret nz
  CYC(0x68ab, 0x68ac);
  CYC(0x68ac, 0x68ad); L = alu_inc8(gb, L);
  CYC(0x68ad, 0x68ae); A = mem_rd(gb, HL);
  CYC(0x68ae, 0x68b0); alu_add(gb, 0x0c);
  CYC(0x68b0, 0x68b1); mem_wr(gb, HL, A); SET_HL(HL - 1); // ld (hl-),a [counter2]
  CYC(0x68b1, 0x68b2); mem_wr(gb, HL, A); // [counter1]
  CYC(0x68b2, 0x68b4); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x68b4, 0x68b5); A = mem_rd(gb, HL);
  CYC(0x68b5, 0x68b7); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x68b7, 0x68b8); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x68b8, 0x68ba); B = 0x08;
  if (!(F & FZ)) { CYCT(0x68ba, 0x68bd); headThwomp_rotate_hook(gb); return; } // jp nz
  CYC(0x68ba, 0x68bd);
  CYC(0x68bd, 0x68bf); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x68bf, 0x68c0); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x68c0, 0x68c2); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x68c2, 0x68c4); mem_wr(gb, HL, 0x10);
  RET(0x68c4); return; // ret
}

// Just reached the target head color
void headThwomp_stateC_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x68c5, ecom_decCounter1_b0f_hook, 0x439a, 0x68c8);
  if (!(F & FZ)) { RET_TAKEN(0x68c8); return; } // ret nz
  CYC(0x68c8, 0x68c9);
  CYC(0x68c9, 0x68cb); L = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x68cb, 0x68cc); A = mem_rd(gb, HL);
  CYC(0x68cc, 0x68ce); A = alu_srl(gb, A);
  CYC(0x68ce, 0x68cf); A = alu_inc8(gb, A);
  CYC(0x68cf, 0x68d0); L = E;
  CYC(0x68d0, 0x68d1); alu_add(gb, mem_rd(gb, HL));
  CYC(0x68d1, 0x68d2); mem_wr(gb, HL, A);
  CYC(0x68d2, 0x68d3); L = alu_inc8(gb, L);
  CYC(0x68d3, 0x68d5); mem_wr(gb, HL, 0x00); // [substate]
  RET(0x68d5); return; // ret
}

// Green face (shoots fireballs)
void headThwomp_stateD_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x68d6, 0x68d7); E = alu_inc8(gb, E);
  CYC(0x68d7, 0x68d8); A = mem_rd(gb, DE);
  CYC(0x68d8, 0x68d9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x68d9, 0x68db); goto substate1; } // jr nz
  CYC(0x68d9, 0x68db);

  CYC(0x68db, 0x68dc); H = D;
  CYC(0x68dc, 0x68dd); L = E;
  CYC(0x68dd, 0x68de); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x68de, 0x68df); L = alu_inc8(gb, L);
  CYC(0x68df, 0x68e1); mem_wr(gb, HL, 0xf0); // [counter1]
  CYC(0x68e1, 0x68e4); SET_HL(wRoomCollisions + 0x47);
  CYC(0x68e4, 0x68e6); mem_wr(gb, HL, 0x00);
  RET(0x68e6); return; // ret

substate1:
  CALL_C(0x68e7, headThwomp_checkBombThrownIntoHead_hook, 0x6ab0, 0x68ea);
  if (!(F & FZ)) { RET_TAKEN(0x68ea); return; } // ret nz
  CYC(0x68ea, 0x68eb);
  CALL_C(0x68eb, ecom_decCounter1_b0f_hook, 0x439a, 0x68ee);
  if (F & FZ) { CYCT(0x68ee, 0x68f0); goto resumeSpinning; } // jr z
  CYC(0x68ee, 0x68f0);
  CYC(0x68f0, 0x68f1); A = mem_rd(gb, HL);
  CYC(0x68f1, 0x68f3); alu_cp(gb, 0xd2); // 210
  if (!(F & FC)) CALL_C_CC(0x68f3, enemyAnimate_hook, 0x2818, 0x68f6); else CYC(0x68f3, 0x68f6); // call nc
  CYC(0x68f6, 0x68f8); E = 0xc6; // Part.counter2
  CYC(0x68f8, 0x68f9); A = mem_rd(gb, HL);
  CYC(0x68f9, 0x68fb); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(0x68fb); return; } // ret nz
  CYC(0x68fb, 0x68fc);
  CYC(0x68fc, 0x68fe); B = 0x39; // PART_HEAD_THWOMP_FIREBALL
  CYC(0x68fe, 0x6901); ecom_spawnProjectile_b0f_hook(gb); return; // jp

resumeSpinning:
  CYC(0x6901, 0x6903); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6903, 0x6905); mem_wr(gb, HL, 0x11);
  CYC(0x6905, 0x6907); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6907, 0x6909); mem_wr(gb, HL, 0x01);
  RET(0x6909); return; // ret
}

// Blue face (fires circular projectiles)
void headThwomp_stateE_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x690a, 0x690b); E = alu_inc8(gb, E);
  CYC(0x690b, 0x690c); A = mem_rd(gb, DE);
  CYC(0x690c, 0x690d); push_effect(gb, 0x690d);
  {
    uint16_t target = headThwomp_jump_table(gb);
    if (target == 0x6915) goto substate0;
    if (target == 0x692f) goto substate1;
    if (target == 0x695b) goto substate2;
    if (target == 0x6966) goto substate3;
    HANDOFF(target);
  }

substate0:
  CYC(0x6915, 0x6916); H = D;
  CYC(0x6916, 0x6917); L = E;
  CYC(0x6917, 0x6918); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x6918, 0x6919); L = alu_inc8(gb, L);
  CYC(0x6919, 0x691b); A = 0x08;
  CYC(0x691b, 0x691c); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a [counter1]
  CYC(0x691c, 0x691d); mem_wr(gb, HL, A); // [counter2]
  CALL_C(0x691d, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6920);
  CYC(0x6920, 0x6922); alu_and(gb, 0x02);
  if (!(F & FZ)) { CYCT(0x6922, 0x6924); goto L_6926; } // jr nz
  CYC(0x6922, 0x6924);
  CYC(0x6924, 0x6926); A = 0xfe;

L_6926:
  CYC(0x6926, 0x6928); E = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(0x6928, 0x6929); mem_wr(gb, DE, A);
  CYC(0x6929, 0x692c); SET_HL(wRoomCollisions + 0x47);
  CYC(0x692c, 0x692e); mem_wr(gb, HL, 0x00);
  RET(0x692e); return; // ret

substate1:
  CALL_C(0x692f, headThwomp_checkBombThrownIntoHead_hook, 0x6ab0, 0x6932);
  if (!(F & FZ)) { RET_TAKEN(0x6932); return; } // ret nz
  CYC(0x6932, 0x6933);
  CALL_C(0x6933, ecom_decCounter1_b0f_hook, 0x439a, 0x6936);
  if (!(F & FZ)) { CYCT(0x6936, 0x6939); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x6936, 0x6939);
  CYC(0x6939, 0x693b); mem_wr(gb, HL, 0x08); // [counter1]
  CYC(0x693b, 0x693d); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x693d, 0x693e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x693e, 0x6941); SET_HL(wRoomCollisions + 0x47);
  CYC(0x6941, 0x6943); mem_wr(gb, HL, 0x03);
  CALL_C(0x6943, getFreePartSlot_hook, 0x3e8e, 0x6946);
  if (!(F & FZ)) { CYCT(0x6946, 0x6948); goto L_6955; } // jr nz
  CYC(0x6946, 0x6948);
  CYC(0x6948, 0x694a); mem_wr(gb, HL, 0x3c); // PART_HEAD_THWOMP_CIRCULAR_PROJECTILE
  CYC(0x694a, 0x694b); L = alu_inc8(gb, L);
  CYC(0x694b, 0x694d); E = ENEMY_BASE + 0x33; // Enemy.var33
  CYC(0x694d, 0x694e); A = mem_rd(gb, DE);
  CYC(0x694e, 0x694f); mem_wr(gb, HL, A); // [part.subid]
  CYC(0x694f, 0x6952); SET_BC(0xf800);
  CALL_C(0x6952, objectCopyPositionWithOffset_hook, 0x225a, 0x6955);

L_6955:
  CYC(0x6955, 0x6957); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6957, 0x6958); A = mem_rd(gb, DE);
  CYC(0x6958, 0x695b); enemySetAnimation_hook(gb); return; // jp

substate2:
  CALL_C(0x695b, ecom_decCounter1_b0f_hook, 0x439a, 0x695e);
  if (!(F & FZ)) { CYCT(0x695e, 0x6961); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x695e, 0x6961);
  CYC(0x6961, 0x6963); mem_wr(gb, HL, 0x1e); // [counter1] = 30
  CYC(0x6963, 0x6964); L = E;
  CYC(0x6964, 0x6965); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  RET(0x6965); return; // ret

substate3:
  CALL_C(0x6966, ecom_decCounter1_b0f_hook, 0x439a, 0x6969);
  if (!(F & FZ)) { RET_TAKEN(0x6969); return; } // ret nz
  CYC(0x6969, 0x696a);
  CYC(0x696a, 0x696b); L = alu_inc8(gb, L);
  CYC(0x696b, 0x696c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl) [counter2]
  if (F & FZ) { CYCT(0x696c, 0x696e); goto stateEResumeSpinning; } // jr z
  CYC(0x696c, 0x696e);
  CYC(0x696e, 0x696f); L = E;
  CYC(0x696f, 0x6971); mem_wr(gb, HL, 0x01); // [substate]
  CYC(0x6971, 0x6973); A = 0x08;
  CYCT(0x6973, 0x6975); goto L_697b; // jr

stateEResumeSpinning:
  CYC(0x6975, 0x6977); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6977, 0x6979); mem_wr(gb, HL, 0x11);
  CYC(0x6979, 0x697b); A = 0x10;

L_697b:
  CYC(0x697b, 0x697d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x697d, 0x697e); mem_wr(gb, HL, A);
  CYC(0x697e, 0x6981); SET_HL(wRoomCollisions + 0x47);
  CYC(0x6981, 0x6983); mem_wr(gb, HL, 0x00);
  CYC(0x6983, 0x6985); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6985, 0x6986); A = mem_rd(gb, DE);
  CYC(0x6986, 0x6988); alu_add(gb, 0x08);
  CYC(0x6988, 0x698b); enemySetAnimation_hook(gb); return; // jp
}

// Purple face (stomps the ground)
void headThwomp_stateF_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x698b, 0x698c); E = alu_inc8(gb, E);
  CYC(0x698c, 0x698d); A = mem_rd(gb, DE);
  CYC(0x698d, 0x698e); push_effect(gb, 0x698e);
  {
    uint16_t target = headThwomp_jump_table(gb);
    if (target == 0x6998) goto substate0;
    if (target == 0x69a4) goto substate1;
    if (target == 0x69c0) goto substate2;
    if (target == 0x69d6) goto substate3;
    if (target == 0x69e7) goto substate4;
    HANDOFF(target);
  }

substate0:
  CYC(0x6998, 0x6999); H = D;
  CYC(0x6999, 0x699a); L = E;
  CYC(0x699a, 0x699b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x699b, 0x699d); L = ENEMY_BASE + OBJ_SPEED_Z;
  CYC(0x699d, 0x699e); alu_xor(gb, A);
  CYC(0x699e, 0x699f); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x699f, 0x69a1); mem_wr(gb, HL, 0x02);
  CYC(0x69a1, 0x69a4); headThwomp_unsetSolidTilesAroundSelf_hook(gb); return; // jp

substate1:
  CYC(0x69a4, 0x69a6); A = 0x20;
  CALL_C(0x69a6, objectUpdateSpeedZ_sidescroll_hook, 0x1f66, 0x69a9);
  CYC(0x69a9, 0x69ab); E = ENEMY_BASE + OBJ_YH;
  CYC(0x69ab, 0x69ac); A = mem_rd(gb, DE);
  CYC(0x69ac, 0x69ae); alu_cp(gb, 0x90);
  if (F & FC) { RET_TAKEN(0x69ae); return; } // ret c
  CYC(0x69ae, 0x69af);
  CYC(0x69af, 0x69b0); H = D;
  CYC(0x69b0, 0x69b2); L = ENEMY_BASE + OBJ_SUBSTATE;
  CYC(0x69b2, 0x69b3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x69b3, 0x69b4); L = alu_inc8(gb, L);
  CYC(0x69b4, 0x69b6); mem_wr(gb, HL, 0x78); // [counter1] = 120
  headThwomp_poundGround_hook(gb); return; // fallthrough

substate2:
  CALL_C(0x69c0, ecom_decCounter1_b0f_hook, 0x439a, 0x69c3);
  if (F & FZ) { CYCT(0x69c3, 0x69c5); goto beginMovingUp; } // jr z
  CYC(0x69c3, 0x69c5);
  CYC(0x69c5, 0x69c6); A = mem_rd(gb, HL);
  CYC(0x69c6, 0x69c8); alu_cp(gb, 0x1e); // 30
  if (F & FC) { RET_TAKEN(0x69c8); return; } // ret c
  CYC(0x69c8, 0x69c9);
  CYC(0x69c9, 0x69cb); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(0x69cb); return; } // ret nz
  CYC(0x69cb, 0x69cc);
  CALL_C(0x69cc, getFreePartSlot_hook, 0x3e8e, 0x69cf);
  if (!(F & FZ)) { RET_TAKEN(0x69cf); return; } // ret nz
  CYC(0x69cf, 0x69d0);
  CYC(0x69d0, 0x69d2); mem_wr(gb, HL, 0x3b); // PART_3b
  RET(0x69d2); return; // ret

beginMovingUp:
  CYC(0x69d3, 0x69d4); L = E;
  CYC(0x69d4, 0x69d5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  RET(0x69d5); return; // ret

substate3:
  CYC(0x69d6, 0x69d7); H = D;
  CYC(0x69d7, 0x69d9); L = ENEMY_BASE + OBJ_Y;
  CYC(0x69d9, 0x69da); A = mem_rd(gb, HL);
  CYC(0x69da, 0x69dc); alu_sub(gb, 0x80);
  CYC(0x69dc, 0x69dd); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x69dd, 0x69de); A = mem_rd(gb, HL);
  CYC(0x69de, 0x69e0); alu_sbc(gb, 0x00);
  CYC(0x69e0, 0x69e1); mem_wr(gb, HL, A);
  CYC(0x69e1, 0x69e3); alu_cp(gb, 0x56);
  if (!(F & FZ)) { RET_TAKEN(0x69e3); return; } // ret nz
  CYC(0x69e3, 0x69e4);
  CYC(0x69e4, 0x69e5); L = E;
  CYC(0x69e5, 0x69e6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  RET(0x69e6); return; // ret

substate4:
  CYC(0x69e7, 0x69e8); H = D;
  CYC(0x69e8, 0x69ea); L = ENEMY_BASE + OBJ_YH;
  CYC(0x69ea, 0x69ed); A = mem_rd(gb, w1Link_yh);
  CYC(0x69ed, 0x69ee); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x69ee, 0x69f0); alu_add(gb, 0x10);
  CYC(0x69f0, 0x69f2); alu_cp(gb, 0x21);
  if (!(F & FC)) { CYCT(0x69f2, 0x69f4); goto setSolidity; } // jr nc
  CYC(0x69f2, 0x69f4);
  CYC(0x69f4, 0x69f6); L = ENEMY_BASE + OBJ_XH;
  CYC(0x69f6, 0x69f9); A = mem_rd(gb, w1Link_xh);
  CYC(0x69f9, 0x69fa); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x69fa, 0x69fc); alu_add(gb, 0x10);
  CYC(0x69fc, 0x69fe); alu_cp(gb, 0x21);
  if (F & FC) { RET_TAKEN(0x69fe); return; } // ret c
  CYC(0x69fe, 0x69ff);

setSolidity:
  CYC(0x69ff, 0x6a01); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6a01, 0x6a03); mem_wr(gb, HL, 0x11);
  CYC(0x6a03, 0x6a05); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6a05, 0x6a07); mem_wr(gb, HL, 0x10);
  CYC(0x6a07, 0x6a0a); headThwomp_setSolidTilesAroundSelf_hook(gb); return; // jp
}

// Also used by the death sequence.
void headThwomp_poundGround_hook(GB *gb) {
  CYC(0x69b6, 0x69b8); A = 0x3c;
  CYC(0x69b8, 0x69bb); mem_wr(gb, wScreenShakeCounterY, A);
  CYC(0x69bb, 0x69bd); A = 0x81; // SND_STRONG_POUND
  CYC(0x69bd, 0x69c0); playSound_b00_hook(gb); return; // jp
}

// Red face (takes damage)
void headThwomp_state10_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a0a, 0x6a0b); E = alu_inc8(gb, E);
  CYC(0x6a0b, 0x6a0c); A = mem_rd(gb, DE);
  CYC(0x6a0c, 0x6a0d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6a0d, 0x6a0f); goto substate1; } // jr nz
  CYC(0x6a0d, 0x6a0f);

  CYC(0x6a0f, 0x6a10); H = D;
  CYC(0x6a10, 0x6a11); L = E;
  CYC(0x6a11, 0x6a12); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [substate]
  CYC(0x6a12, 0x6a13); L = alu_inc8(gb, L);
  CYC(0x6a13, 0x6a15); mem_wr(gb, HL, 0x78); // [counter1] = 120
  CYC(0x6a15, 0x6a17); L = ENEMY_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(0x6a17, 0x6a19); mem_wr(gb, HL, 0x18);
  CYC(0x6a19, 0x6a1b); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x6a1b, 0x6a1c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(0x6a1c, 0x6a1e); goto L_6a27; } // jr nz
  CYC(0x6a1c, 0x6a1e);
  CYC(0x6a1e, 0x6a1f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  CALL_C(0x6a1f, headThwomp_unsetSolidTilesAroundSelf_hook, 0x6a8e, 0x6a22);
  CYC(0x6a22, 0x6a24); A = 0x01; // TREE_GFXH_01
  CYC(0x6a24, 0x6a27); mem_wr(gb, wLoadedTreeGfxIndex, A);

L_6a27:
  CYC(0x6a27, 0x6a29); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x6a29, 0x6a2a); A = mem_rd(gb, DE);
  CYC(0x6a2a, 0x6a2b); A = alu_inc8(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x6a2b, headThwomp_dropHeart_hook, 0x6b1c, 0x6a2e); else CYC(0x6a2b, 0x6a2e); // call nz
  CYC(0x6a2e, 0x6a30); A = 0x10;
  CALL_C(0x6a30, enemySetAnimation_hook, 0x282b, 0x6a33);
  CYC(0x6a33, 0x6a35); A = 0x63; // SND_BOSS_DAMAGE
  CYC(0x6a35, 0x6a38); playSound_b00_hook(gb); return; // jp

substate1:
  CALL_C(0x6a38, ecom_decCounter1_b0f_hook, 0x439a, 0x6a3b);
  if (F & FZ) { CYCT(0x6a3b, 0x6a3d); goto state10ResumeSpinning; } // jr z
  CYC(0x6a3b, 0x6a3d);
  CYC(0x6a3d, 0x6a3f); E = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x6a3f, 0x6a40); A = mem_rd(gb, DE);
  CYC(0x6a40, 0x6a41); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6a41); return; } // ret nz
  CYC(0x6a41, 0x6a42);
  CYC(0x6a42, 0x6a44); mem_wr(gb, HL, 0xff);
  CYC(0x6a44, 0x6a46); A = 0x20;
  CALL_C(0x6a46, objectUpdateSpeedZ_sidescroll_hook, 0x1f66, 0x6a49);
  CYC(0x6a49, 0x6a4b); E = ENEMY_BASE + OBJ_YH;
  CYC(0x6a4b, 0x6a4c); A = mem_rd(gb, DE);
  CYC(0x6a4c, 0x6a4e); alu_cp(gb, 0x90);
  if (F & FC) { RET_TAKEN(0x6a4e); return; } // ret c
  CYC(0x6a4e, 0x6a4f);
  CYC(0x6a4f, 0x6a50); H = D;
  CYC(0x6a50, 0x6a52); L = ENEMY_BASE + OBJ_HEALTH;
  CYC(0x6a52, 0x6a54); mem_wr(gb, HL, 0x00);
  CYC(0x6a54, 0x6a57); headThwomp_poundGround_hook(gb); return; // jp

state10ResumeSpinning:
  CYC(0x6a57, 0x6a59); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6a59, 0x6a5b); mem_wr(gb, HL, 0x11);
  CYC(0x6a5b, 0x6a5d); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6a5d, 0x6a5f); mem_wr(gb, HL, 0x10);
  CYC(0x6a5f, 0x6a62); SET_HL(wRoomCollisions + 0x47);
  CYC(0x6a62, 0x6a64); mem_wr(gb, HL, 0x00);
  CYC(0x6a64, 0x6a66); A = 0x0e;
  CYC(0x6a66, 0x6a69); enemySetAnimation_hook(gb); return; // jp
}

void headThwomp_state11_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6a69, ecom_decCounter1_b0f_hook, 0x439a, 0x6a6c);
  if (!(F & FZ)) { CYCT(0x6a6c, 0x6a6f); enemyAnimate_hook(gb); return; } // jp nz
  CYC(0x6a6c, 0x6a6f);
  CYC(0x6a6f, 0x6a70); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl) [counter1]
  CYC(0x6a70, 0x6a71); L = E;
  CYC(0x6a71, 0x6a73); mem_wr(gb, HL, 0x09); // [state]
  CYC(0x6a73, 0x6a75); L = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(0x6a75, 0x6a77); mem_wr(gb, HL, 0xf0);
  RET(0x6a77); return; // ret
}

void headThwomp_setSolidTilesAroundSelf_hook(GB *gb) {
  CYC(0x6a78, 0x6a7b); SET_HL(wRoomCollisions + 0x46);
  CYC(0x6a7b, 0x6a7d); mem_wr(gb, HL, 0x01);
  CYC(0x6a7d, 0x6a7e); L = alu_inc8(gb, L);
  CYC(0x6a7e, 0x6a7f); L = alu_inc8(gb, L);
  CYC(0x6a7f, 0x6a81); mem_wr(gb, HL, 0x02);
  CYC(0x6a81, 0x6a82); A = L;
  CYC(0x6a82, 0x6a84); alu_add(gb, 0x0e);
  CYC(0x6a84, 0x6a85); L = A;
  CYC(0x6a85, 0x6a87); mem_wr(gb, HL, 0x05);
  CYC(0x6a87, 0x6a88); L = alu_inc8(gb, L);
  CYC(0x6a88, 0x6a8a); mem_wr(gb, HL, 0x0f);
  CYC(0x6a8a, 0x6a8b); L = alu_inc8(gb, L);
  CYC(0x6a8b, 0x6a8d); mem_wr(gb, HL, 0x0a);
  RET(0x6a8d); return; // ret
}

void headThwomp_unsetSolidTilesAroundSelf_hook(GB *gb) {
  CYC(0x6a8e, 0x6a91); SET_HL(wRoomCollisions + 0x46);
  CYC(0x6a91, 0x6a92); alu_xor(gb, A);
  CYC(0x6a92, 0x6a93); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6a93, 0x6a94); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6a94, 0x6a95); mem_wr(gb, HL, A);
  CYC(0x6a95, 0x6a97); L = 0x56;
  CYC(0x6a97, 0x6a98); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6a98, 0x6a99); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6a99, 0x6a9a); mem_wr(gb, HL, A);
  RET(0x6a9a); return; // ret
}

// @param b Animation base
void headThwomp_rotate_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6a9b, 0x6a9d); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6a9d, 0x6a9e); A = mem_rd(gb, DE);
  CYC(0x6a9e, 0x6a9f); A = alu_inc8(gb, A);
  CYC(0x6a9f, 0x6aa1); alu_and(gb, 0x07);
  CYC(0x6aa1, 0x6aa2); mem_wr(gb, DE, A);
  CYC(0x6aa2, 0x6aa3); alu_add(gb, B);
  CALL_C(0x6aa3, enemySetAnimation_hook, 0x282b, 0x6aa6);
  CYC(0x6aa6, 0x6aa8); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6aa8, 0x6aa9); A = mem_rd(gb, DE);
  CYC(0x6aa9, 0x6aaa); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x6aaa); return; } // ret c
  CYC(0x6aaa, 0x6aab);
  CYC(0x6aab, 0x6aad); A = 0x58; // SND_CLINK2
  CYC(0x6aad, 0x6ab0); playSound_b00_hook(gb); return; // jp
}

// If a bomb is thrown into head thwomp, this sets the state to $0a.
// @param[out] zflag z if no bomb entered head thwomp
void headThwomp_checkBombThrownIntoHead_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6ab0, 0x6ab3); SET_HL(0xd700); // FIRST_DYNAMIC_ITEM_INDEX, Item.start

itemLoop:
  CYC(0x6ab3, 0x6ab5); L = OBJ_ID;
  CYC(0x6ab5, 0x6ab6); A = mem_rd(gb, HL);
  CYC(0x6ab6, 0x6ab8); alu_cp(gb, 0x03); // ITEM_BOMB
  if (!(F & FZ)) { CYCT(0x6ab8, 0x6aba); goto nextItem; } // jr nz
  CYC(0x6ab8, 0x6aba);
  CYC(0x6aba, 0x6abc); L = OBJ_STATE;
  CYC(0x6abc, 0x6abd); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6abd, 0x6abe); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x6abe, 0x6ac0); goto isNonExplodingBomb; } // jr z
  CYC(0x6abe, 0x6ac0);
  CYC(0x6ac0, 0x6ac1); A = mem_rd(gb, HL);
  CYC(0x6ac1, 0x6ac3); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(0x6ac3, 0x6ac5); goto nextItem; } // jr c
  CYC(0x6ac3, 0x6ac5);

isNonExplodingBomb:
  CYC(0x6ac5, 0x6ac7); L = OBJ_YH;
  CYC(0x6ac7, 0x6ac8); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6ac8, 0x6aca); alu_sub(gb, 0x50);
  CYC(0x6aca, 0x6acc); alu_add(gb, 0x0c);
  CYC(0x6acc, 0x6ace); alu_cp(gb, 0x19);
  if (!(F & FC)) { CYCT(0x6ace, 0x6ad0); goto nextItem; } // jr nc
  CYC(0x6ace, 0x6ad0);
  CYC(0x6ad0, 0x6ad1); L = alu_inc8(gb, L);
  CYC(0x6ad1, 0x6ad2); A = mem_rd(gb, HL);
  CYC(0x6ad2, 0x6ad4); alu_sub(gb, 0x78);
  CYC(0x6ad4, 0x6ad6); alu_add(gb, 0x0c);
  CYC(0x6ad6, 0x6ad8); alu_cp(gb, 0x19);
  if (F & FC) { CYCT(0x6ad8, 0x6ada); goto bombEnteredThwomp; } // jr c
  CYC(0x6ad8, 0x6ada);

nextItem:
  CYC(0x6ada, 0x6adb); H = alu_inc8(gb, H);
  CYC(0x6adb, 0x6adc); A = H;
  CYC(0x6adc, 0x6ade); alu_cp(gb, 0xdc); // LAST_DYNAMIC_ITEM_INDEX+1
  if (F & FC) { CYCT(0x6ade, 0x6ae0); goto itemLoop; } // jr c
  CYC(0x6ade, 0x6ae0);
  CYC(0x6ae0, 0x6ae2); E = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6ae2, 0x6ae3); A = mem_rd(gb, DE);
  CYC(0x6ae3, 0x6ae4); alu_rrca(gb);
  if (F & FC) { CYCT(0x6ae4, 0x6ae6); goto triggerBombEffect; } // jr c
  CYC(0x6ae4, 0x6ae6);
  CYC(0x6ae6, 0x6ae7); alu_xor(gb, A);
  RET(0x6ae7); return; // ret

bombEnteredThwomp:
  CYC(0x6ae8, 0x6aea); L = OBJ_VAR2F;
  CYC(0x6aea, 0x6aec); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5))); // set 5,(hl)

triggerBombEffect:
  CYC(0x6aec, 0x6aed); H = D;
  CYC(0x6aed, 0x6aef); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6aef, 0x6af1); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)
  CYC(0x6af1, 0x6af3); E = ENEMY_BASE + OBJ_DIRECTION;
  CYC(0x6af3, 0x6af4); A = mem_rd(gb, DE);
  CYC(0x6af4, 0x6af6); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x6af6, 0x6af8); goto betweenTwoHeads; } // jr nz
  CYC(0x6af6, 0x6af8);
  CYC(0x6af8, 0x6afa); L = ENEMY_BASE + 0x31; // Enemy.var31
  CYC(0x6afa, 0x6afb); mem_wr(gb, HL, A);
  CYC(0x6afb, 0x6afd); L = ENEMY_BASE + 0x32; // Enemy.var32
  CYC(0x6afd, 0x6aff); mem_wr(gb, HL, 0x00);
  CYC(0x6aff, 0x6b01); L = ENEMY_BASE + OBJ_STATE;
  CYC(0x6b01, 0x6b03); mem_wr(gb, HL, 0x0a);
  CYC(0x6b03, 0x6b05); L = ENEMY_BASE + OBJ_COUNTER1;
  CYC(0x6b05, 0x6b07); mem_wr(gb, HL, 0x06);
  CALL_C(0x6b07, enemySetAnimation_hook, 0x282b, 0x6b0a);
  CYC(0x6b0a, 0x6b0d); SET_HL(wRoomCollisions + 0x47);
  CYC(0x6b0d, 0x6b0f); mem_wr(gb, HL, 0x03);
  CYC(0x6b0f, 0x6b10); alu_or(gb, D);
  RET(0x6b10); return; // ret

betweenTwoHeads:
  CALL_C(0x6b11, ecom_decCounter1_b0f_hook, 0x439a, 0x6b14);
  if (!(F & FZ)) { RET_TAKEN(0x6b14); return; } // ret nz
  CYC(0x6b14, 0x6b15);
  CYC(0x6b15, 0x6b17); B = 0x00;
  CALL_C(0x6b17, headThwomp_rotate_hook, 0x6a9b, 0x6b1a);
  CYCT(0x6b1a, 0x6b1c); goto triggerBombEffect; // jr (backward)
}

void headThwomp_dropHeart_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6b1c, getFreePartSlot_hook, 0x3e8e, 0x6b1f);
  if (!(F & FZ)) { RET_TAKEN(0x6b1f); return; } // ret nz
  CYC(0x6b1f, 0x6b20);
  CYC(0x6b20, 0x6b22); mem_wr(gb, HL, 0x01); // PART_ITEM_DROP
  CYC(0x6b22, 0x6b23); L = alu_inc8(gb, L);
  CYC(0x6b23, 0x6b25); mem_wr(gb, HL, 0x01); // ITEM_DROP_HEART
  CYC(0x6b25, 0x6b28); SET_BC(0x1400);
  CYC(0x6b28, 0x6b2b); objectCopyPositionWithOffset_hook(gb); return; // jp
}

void headThwomp_checkShootProjectile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6b2b, 0x6b2e); A = mem_rd(gb, wFrameCounter);
  CYC(0x6b2e, 0x6b2f); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(0x6b2f); return; } // ret c
  CYC(0x6b2f, 0x6b30);
  CYC(0x6b30, 0x6b31); H = D;
  CYC(0x6b31, 0x6b33); L = ENEMY_BASE + 0x34; // Enemy.var34
  CYC(0x6b33, 0x6b34); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(0x6b34, 0x6b36); goto L_6b38; } // jr nz
  CYC(0x6b34, 0x6b36);
  CYC(0x6b36, 0x6b38); mem_wr(gb, HL, 0xf0);

L_6b38:
  CYC(0x6b38, 0x6b39); A = mem_rd(gb, HL);
  CYC(0x6b39, 0x6b3b); alu_cp(gb, 0x5a); // 90
  if (!(F & FC)) { RET_TAKEN(0x6b3b); return; } // ret nc
  CYC(0x6b3b, 0x6b3c);
  CYC(0x6b3c, 0x6b3e); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(0x6b3e); return; } // ret nz
  CYC(0x6b3e, 0x6b3f);
  CALL_C(0x6b3f, getRandomNumber_noPreserveVars_hook, 0x0453, 0x6b42);
  CYC(0x6b42, 0x6b44); alu_and(gb, 0x07);
  if (F & FZ) { CYCT(0x6b44, 0x6b46); goto dropBomb; } // jr z
  CYC(0x6b44, 0x6b46);
  CYC(0x6b46, 0x6b48); B = 0x39; // PART_HEAD_THWOMP_FIREBALL
  CYC(0x6b48, 0x6b4b); ecom_spawnProjectile_b0f_hook(gb); return; // jp

dropBomb:
  CYC(0x6b4b, 0x6b4d); B = 0x02;
  CALL_C(0x6b4d, checkBPartSlotsAvailable_hook, 0x20f2, 0x6b50);
  if (!(F & FZ)) { RET_TAKEN(0x6b50); return; } // ret nz
  CYC(0x6b50, 0x6b51);
  CALL_C(0x6b51, getFreePartSlot_hook, 0x3e8e, 0x6b54);
  CYC(0x6b54, 0x6b56); mem_wr(gb, HL, 0x01); // PART_ITEM_DROP
  CYC(0x6b56, 0x6b57); L = alu_inc8(gb, L);
  CYC(0x6b57, 0x6b59); mem_wr(gb, HL, 0x04); // ITEM_DROP_BOMBS
  CALL_C(0x6b59, objectCopyPosition_hook, 0x2242, 0x6b5c);
  CYC(0x6b5c, 0x6b5d); B = H;
  CALL_C(0x6b5d, getFreePartSlot_hook, 0x3e8e, 0x6b60);
  CYC(0x6b60, 0x6b62); mem_wr(gb, HL, 0x40); // PART_HEAD_THWOMP_BOMB_DROPPER
  CYC(0x6b62, 0x6b64); L = PART_BASE + OBJ_RELATED1;
  CYC(0x6b64, 0x6b66); A = 0xc0; // Part.start
  CYC(0x6b66, 0x6b67); mem_wr(gb, HL, A); SET_HL(HL + 1); // ld (hl+),a
  CYC(0x6b67, 0x6b68); mem_wr(gb, HL, B);
  CYC(0x6b68, 0x6b6b); objectCopyPosition_hook(gb); return; // jp
}
