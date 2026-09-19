#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t fallingRock_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void fallingRock_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void fallingRock_subid00_hook(GB *gb);
void fallingRock_subid01_hook(GB *gb);
void fallingRock_subid02_hook(GB *gb);
void fallingRock_updateSpeedAndDeleteWhenLanded_hook(GB *gb);
void fallingRock_subid03_hook(GB *gb);
void fallingRock_initDiagonalAngle_hook(GB *gb);
void fallingRock_subid03_state1_hook(GB *gb);
void fallingRock_subid04_hook(GB *gb);
void fallingRock_subid05_hook(GB *gb);
void fallingRock_subid06_hook(GB *gb);
void fallingRock_initGraphicsAndIncState_hook(GB *gb);
void fallingRock_chooseRandomPosition_hook(GB *gb);

// INTERAC_FALLING_ROCK
void interactionCode92_hook(GB *gb) {
  CYC(0x7413, 0x7415); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7415, 0x7416); A = mem_rd(gb, DE);
  {
    CYC(0x7416, 0x7417); push_effect(gb, 0x7417);
    uint16_t target = fallingRock_jump_table(gb);
    if (target == 0x7425) { fallingRock_subid00_hook(gb); return; }
    if (target == 0x744f) { fallingRock_subid01_hook(gb); return; }
    if (target == 0x7496) { fallingRock_subid02_hook(gb); return; }
    if (target == 0x7506) { fallingRock_subid03_hook(gb); return; }
    if (target == 0x7533) { fallingRock_subid04_hook(gb); return; }
    fallingRock_subid06_hook(gb); return; // target == 0x7550
  }
}

// Spawner of falling rocks; stops when $cfdf is nonzero. Used when freeing goron elder.
void fallingRock_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7425, checkInteractionState_hook, 0x23fe, 0x7428);
  if (!(F & FZ)) { CYCT(0x7428, 0x742a); goto state1; } // jr nz
  CYC(0x7428, 0x742a);

  // fallingRock_subid00@state0
  CALL_C(0x742a, interactionIncState_hook, 0x23e0, 0x742d);
  CYC(0x742d, 0x742f); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x742f, 0x7431); mem_wr(gb, HL, 0x01);

state1:
  CYC(0x7431, 0x7434); A = W8(wTmpcfc0_goronCutscenes_elder_stopFallingRockSpawner);
  CYC(0x7434, 0x7435); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7435, 0x7438); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x7435, 0x7438);

  CALL_C(0x7438, interactionDecCounter2_hook, 0x23d1, 0x743b);
  if (!(F & FZ)) { RET_TAKEN(0x743b); return; } // ret nz
  CYC(0x743b, 0x743c);

  CYC(0x743c, 0x743e); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x743e, 0x7440); mem_wr(gb, HL, 20);
  CALL_C(0x7440, getFreeInteractionSlot_hook, 0x3aef, 0x7443);
  if (!(F & FZ)) { RET_TAKEN(0x7443); return; } // ret nz
  CYC(0x7443, 0x7444);
  CYC(0x7444, 0x7446); mem_wr(gb, HL, 0x92); // INTERAC_FALLING_ROCK
  CYC(0x7446, 0x7447); L = alu_inc8(gb, L);
  CYC(0x7447, 0x7449); mem_wr(gb, HL, 0x01);
  CYC(0x7449, 0x744a); L = alu_inc8(gb, L);
  CYC(0x744a, 0x744c); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x744c, 0x744d); A = mem_rd(gb, DE);
  CYC(0x744d, 0x744e); mem_wr(gb, HL, A);
  RET(0x744e); return;
}

// Instance of falling rock spawned by subid $00
void fallingRock_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x744f, checkInteractionState_hook, 0x23fe, 0x7452);
  if (!(F & FZ)) { CYCT(0x7452, 0x7454); goto state1; } // jr nz

  // fallingRock_subid01@state0
  CYC(0x7452, 0x7454);
  CALL_C(0x7454, fallingRock_initGraphicsAndIncState_hook, 0x758c, 0x7457);
  CALL_C(0x7457, fallingRock_chooseRandomPosition_hook, 0x7595, 0x745a);

state1:
  CYC(0x745a, 0x745c); C = 0x10;
  CALL_C(0x745c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x745f);
  if (!(F & FZ)) { CYCT(0x745f, 0x7461); goto ret; } // jr nz
  CYC(0x745f, 0x7461);

  // Rock has hit the ground
  CALL_C(0x7461, objectReplaceWithAnimationIfOnHazard_hook, 0x2225, 0x7464);
  if (F & FC) { CYCT(0x7464, 0x7467); interactionDelete_hook(gb); return; } // jp c
  CYC(0x7464, 0x7467);

  CYC(0x7467, 0x7469); A = 0xa5; // SND_BREAK_ROCK
  CALL_C(0x7469, playSound_b00_hook, 0x0c98, 0x746c);
  CYC(0x746c, 0x746f); push_effect(gb, 0x746f); goto spawnDebris;
afterSpawnDebris:
  CYC(0x746f, 0x7471); A = 0x04;
  CALL_C(0x7471, setScreenShakeCounter_hook, 0x24bb, 0x7474);
  CYC(0x7474, 0x7477); interactionDelete_hook(gb); return; // jp

ret:
  RET(0x7477); return;

  // fallingRock_subid01@spawnDebris; single caller (state1 above), top-level depth sp0_.
spawnDebris:
  CALL_C(0x7478, getRandomNumber_hook, 0x043e, 0x747b);
  CYC(0x747b, 0x747d); alu_and(gb, 0x03);
  CYC(0x747d, 0x747e); C = A;
  CYC(0x747e, 0x7480); B = 0x00;

next:
  CYC(0x7480, 0x7481); push_effect(gb, BC);
  CYC(0x7481, 0x7484); SET_BC(0x9202); // INTERAC_FALLING_ROCK, $02
  CALL_C(0x7484, objectCreateInteraction_hook, 0x24c5, 0x7487);
  CYC(0x7487, 0x7488); SET_BC(pop_effect(gb));
  if (!(F & FZ)) {
    RET_TAKEN(0x7488);
    if (gb->pc == 0x746f && gb->sp == sp0_) goto afterSpawnDebris;
    return;
  } // ret nz
  CYC(0x7488, 0x7489);
  CYC(0x7489, 0x748b); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x748b, 0x748c); mem_wr(gb, HL, C);
  CYC(0x748c, 0x748e); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x748e, 0x748f); mem_wr(gb, HL, B);
  CYC(0x748f, 0x7490); B = alu_inc8(gb, B);
  CYC(0x7490, 0x7491); A = B;
  CYC(0x7491, 0x7493); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x7493, 0x7495); goto next; } // jr nz
  CYC(0x7493, 0x7495);
  RET(0x7495);
  if (gb->pc == 0x746f && gb->sp == sp0_) goto afterSpawnDebris;
  return; // ret
}

// Used by gorons when freeing elder?
void fallingRock_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7496, checkInteractionState_hook, 0x23fe, 0x7499);
  if (!(F & FZ)) { CYCT(0x7499, 0x749b); goto state1; } // jr nz
  CYC(0x7499, 0x749b);

  // fallingRock_subid02@state0
  CALL_C(0x749b, fallingRock_initGraphicsAndIncState_hook, 0x758c, 0x749e);
  CALL_C(0x749e, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x74a1);
  CYC(0x74a1, 0x74a3); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x74a3, 0x74a4); A = mem_rd(gb, HL);
  CYC(0x74a4, 0x74a5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x74a5, 0x74a7); goto highAngleOffset; } // jr nz
  CYC(0x74a5, 0x74a7);
  CYC(0x74a7, 0x74a9); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x74a9, 0x74aa); A = mem_rd(gb, HL);
  CYC(0x74aa, 0x74ac); goto loadAngle; // jr

highAngleOffset:
  CYC(0x74ac, 0x74ae); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x74ae, 0x74af); A = mem_rd(gb, HL);
  CYC(0x74af, 0x74b1); alu_add(gb, 0x04);

  // fallingRock_subid02@loadAngle
  // Table is read at ([counter1] + ([var03] != 0 ? 4 : 0)) * 4 + [angle].
loadAngle:
  CYC(0x74b1, 0x74b2); alu_add(gb, A);
  CYC(0x74b2, 0x74b3); alu_add(gb, A);
  CYC(0x74b3, 0x74b5); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x74b5, 0x74b6); alu_add(gb, mem_rd(gb, HL));
  CYC(0x74b6, 0x74b9); SET_BC(0x74dc); // @angles
  CALL_C(0x74b9, addAToBc_hook, 0x006d, 0x74bc);
  CYC(0x74bc, 0x74bd); A = mem_rd(gb, BC);
  CYC(0x74bd, 0x74be); mem_wr(gb, HL, A);
  CYC(0x74be, 0x74c0); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x74c0, 0x74c1); A = mem_rd(gb, HL);
  CYC(0x74c1, 0x74c2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x74c2, 0x74c4); goto lowSpeed; } // jr nz
  CYC(0x74c2, 0x74c4);

  CYC(0x74c4, 0x74c6); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x74c6, 0x74c8); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(0x74c8, 0x74ca); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(0x74ca, 0x74cc); A = 0x18;
  CYC(0x74cc, 0x74cd); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x74cd, 0x74cf); mem_wr(gb, HL, 0xff);
  RET(0x74cf); return;

lowSpeed:
  CYC(0x74d0, 0x74d2); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x74d2, 0x74d4); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x74d4, 0x74d6); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(0x74d6, 0x74d8); A = 0x1c;
  CYC(0x74d8, 0x74d9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x74d9, 0x74db); mem_wr(gb, HL, 0xff);
  RET(0x74db); return;

state1:
  CYC(0x74f4, 0x74f7); A = W8(wTmpcfc0_goronCutscenes_cfde);
  CYC(0x74f7, 0x74f8); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x74f8, 0x74fb); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x74f8, 0x74fb);
  fallingRock_updateSpeedAndDeleteWhenLanded_hook(gb); return; // falls through
}

void fallingRock_updateSpeedAndDeleteWhenLanded_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x74fb, 0x74fd); C = 0x18;
  CALL_C(0x74fd, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7500);
  if (F & FZ) { CYCT(0x7500, 0x7503); interactionDelete_hook(gb); return; } // jp z
  CYC(0x7500, 0x7503);
  CYC(0x7503, 0x7506); objectApplySpeed_hook(gb); return; // jp
}

// A twinkle? angle is a value from 0-3, indicating a diagonal to move in.
void fallingRock_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7506, checkInteractionState_hook, 0x23fe, 0x7509);
  if (!(F & FZ)) { CYCT(0x7509, 0x750b); fallingRock_subid03_state1_hook(gb); return; } // jr nz
  CYC(0x7509, 0x750b);

  // fallingRock_subid03@state0
  CALL_C(0x750b, fallingRock_initGraphicsAndIncState_hook, 0x758c, 0x750e);
  CALL_C(0x750e, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7511);
  fallingRock_initDiagonalAngle_hook(gb); return; // falls through
}

void fallingRock_initDiagonalAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7511, 0x7513); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7513, 0x7514); A = mem_rd(gb, HL);
  CYC(0x7514, 0x7517); SET_BC(0x7521); // @diagonalAngles
  CALL_C(0x7517, addAToBc_hook, 0x006d, 0x751a);
  CYC(0x751a, 0x751b); A = mem_rd(gb, BC);
  CYC(0x751b, 0x751c); mem_wr(gb, HL, A);
  CYC(0x751c, 0x751e); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x751e, 0x7520); mem_wr(gb, HL, 0x28); // SPEED_100
  RET(0x7520); return;
}

void fallingRock_subid03_state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7525, 0x7527); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x7527, 0x7528); A = mem_rd(gb, DE);
  CYC(0x7528, 0x752a); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x752a, 0x752d); interactionDelete_hook(gb); return; } // jp z
  CYC(0x752a, 0x752d);
  CALL_C(0x752d, interactionAnimate_hook, 0x261b, 0x7530);
  CYC(0x7530, 0x7533); objectApplySpeed_hook(gb); return; // jp
}

// Blue/Red rock debris, moving straight on a diagonal? (angle from 0-3). fallingRock_subid04
// and fallingRock_subid05 both target 0a:7533 in interactionCode92's jump table -- identical
// code, so subid04 just hands off to subid05.
void fallingRock_subid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7533, checkInteractionState_hook, 0x23fe, 0x7536);
  if (!(F & FZ)) { CYCT(0x7536, 0x7538); goto state1; } // jr nz
  CYC(0x7536, 0x7538);

  // fallingRock_subid05@state0
  CALL_C(0x7538, fallingRock_initGraphicsAndIncState_hook, 0x758c, 0x753b);
  CALL_C(0x753b, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x753e);
  CYC(0x753e, 0x7540); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7540, 0x7542); mem_wr(gb, HL, 0x0c);
  CYC(0x7542, 0x7544); fallingRock_initDiagonalAngle_hook(gb); return; // jr

state1:
  CALL_C(0x7544, interactionDecCounter1_hook, 0x23cc, 0x7547);
  if (F & FZ) { CYCT(0x7547, 0x754a); interactionDelete_hook(gb); return; } // jp z
  CYC(0x7547, 0x754a);
  CALL_C(0x754a, interactionAnimate_hook, 0x261b, 0x754d);
  CYC(0x754d, 0x7550); objectApplySpeed_hook(gb); return; // jp
}

void fallingRock_subid04_hook(GB *gb) {
  fallingRock_subid05_hook(gb);
}

// Debris from pickaxe workers?
void fallingRock_subid06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7550, checkInteractionState_hook, 0x23fe, 0x7553);
  if (!(F & FZ)) { CYCT(0x7553, 0x7556); fallingRock_updateSpeedAndDeleteWhenLanded_hook(gb); return; } // jp nz
  CYC(0x7553, 0x7556);

  // fallingRock_subid06@state0
  CALL_C(0x7556, fallingRock_initGraphicsAndIncState_hook, 0x758c, 0x7559);
  CALL_C(0x7559, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x755c);
  CYC(0x755c, 0x755e); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x755e, 0x755f); A = mem_rd(gb, HL);
  CYC(0x755f, 0x7561); alu_or(gb, 0x08);
  CYC(0x7561, 0x7563); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7563, 0x7564); mem_wr(gb, HL, A);
  CYC(0x7564, 0x7566); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x7566, 0x7567); A = mem_rd(gb, HL);
  CYC(0x7567, 0x7568); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7568, 0x756a); goto haveCounter2; } // jr z
  CYC(0x7568, 0x756a);
  CYC(0x756a, 0x756b); A = alu_dec8(gb, A);

haveCounter2:
  CYC(0x756b, 0x756c); B = A;
  CYC(0x756c, 0x756e); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x756e, 0x756f); A = mem_rd(gb, HL);
  CYC(0x756f, 0x7571); alu_and(gb, 0xbc);
  CYC(0x7571, 0x7572); alu_or(gb, B);
  CYC(0x7572, 0x7573); mem_wr(gb, HL, A);
  CYC(0x7573, 0x7575); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x7575, 0x7576); A = mem_rd(gb, HL);
  CYC(0x7576, 0x7579); SET_BC(0x758a); // @angles
  CALL_C(0x7579, addAToBc_hook, 0x006d, 0x757c);
  CYC(0x757c, 0x757d); A = mem_rd(gb, BC);
  CYC(0x757d, 0x757e); mem_wr(gb, HL, A);
  CYC(0x757e, 0x7580); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7580, 0x7582); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x7582, 0x7584); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(0x7584, 0x7586); A = 0x40;
  CYC(0x7586, 0x7587); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7587, 0x7589); mem_wr(gb, HL, 0xff);
  RET(0x7589); return;
}

void fallingRock_initGraphicsAndIncState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x758c, interactionInitGraphics_hook, 0x15fb, 0x758f);
  CALL_C(0x758f, objectSetVisiblec1_hook, 0x1e3c, 0x7592);
  CYC(0x7592, 0x7595); interactionIncState_hook(gb); return; // jp
}

// Randomly choose a position from a list of possible positions. var03 determines which
// list it reads from?
void fallingRock_chooseRandomPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7595, 0x7597); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7597, 0x7598); A = mem_rd(gb, DE);
  CYC(0x7598, 0x7599); alu_or(gb, A);
  CYC(0x7599, 0x759c); SET_HL(0x75bc); // @positionList1
  if (F & FZ) { CYCT(0x759c, 0x759e); goto pickPosition; } // jr z
  CYC(0x759c, 0x759e);
  CYC(0x759e, 0x75a1); SET_HL(0x75dc); // @positionList2
  CYC(0x75a1, 0x75a3); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x75a3, 0x75a5); A = 0x04;
  CYC(0x75a5, 0x75a6); mem_wr(gb, DE, A);

pickPosition:
  CALL_C(0x75a6, getRandomNumber_hook, 0x043e, 0x75a9);
  CYC(0x75a9, 0x75ab); alu_and(gb, 0x0f);
  CYC(0x75ab, 0x75ac); fallingRock_add_double_index(gb, 0x75ac);
  CYC(0x75ac, 0x75ad); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x75ad, 0x75af); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x75af, 0x75b0); mem_wr(gb, DE, A);
  CYC(0x75b0, 0x75b1); alu_cpl(gb);
  CYC(0x75b1, 0x75b2); A = alu_inc8(gb, A);
  CYC(0x75b2, 0x75b4); alu_sub(gb, 0x08);
  CYC(0x75b4, 0x75b6); E = INTERACTION_BASE + OBJ_ZH;
  CYC(0x75b6, 0x75b7); mem_wr(gb, DE, A);
  CYC(0x75b7, 0x75b8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x75b8, 0x75ba); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x75ba, 0x75bb); mem_wr(gb, DE, A);
  RET(0x75bb); return;
}
