#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode92);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = fallingRock_jump_table(gb);
    if (target == SYM(fallingRock_subid00)) { fallingRock_subid00_hook(gb); return; }
    if (target == SYM(fallingRock_subid01)) { fallingRock_subid01_hook(gb); return; }
    if (target == SYM(fallingRock_subid02)) { fallingRock_subid02_hook(gb); return; }
    if (target == SYM(fallingRock_subid03)) { fallingRock_subid03_hook(gb); return; }
    if (target == SYM(fallingRock_subid04)) { fallingRock_subid04_hook(gb); return; }
    fallingRock_subid06_hook(gb); return; // target == 0x7550
  }
}

// Spawner of falling rocks; stops when $cfdf is nonzero. Used when freeing goron elder.
void fallingRock_subid00_hook(GB *gb) {
  BASE(fallingRock_subid00);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // fallingRock_subid00@state0
  CALL_C(b_+5, interactionIncState_hook, SYM(interactionIncState), b_+8);
  CYC(b_+8, b_+10); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x01);

state1:
  CYC(b_+12, b_+15); A = W8(wTmpcfc0_goronCutscenes_elder_stopFallingRockSpawner);
  CYC(b_+15, b_+16); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+16, b_+19);

  CALL_C(b_+19, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+22);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; } // ret nz
  CYC(b_+22, b_+23);

  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 20);
  CALL_C(b_+27, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+30);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; } // ret nz
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x92); // INTERAC_FALLING_ROCK
  CYC(b_+33, b_+34); L = alu_inc8(gb, L);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x01);
  CYC(b_+36, b_+37); L = alu_inc8(gb, L);
  CYC(b_+37, b_+39); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  RET(b_+41); return;
}

// Instance of falling rock spawned by subid $00
void fallingRock_subid01_hook(GB *gb) {
  BASE(fallingRock_subid01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz

  // fallingRock_subid01@state0
  CYC(b_+3, b_+5);
  CALL_C(b_+5, fallingRock_initGraphicsAndIncState_hook, SYM(fallingRock_initGraphicsAndIncState), b_+8);
  CALL_C(b_+8, fallingRock_chooseRandomPosition_hook, SYM(fallingRock_chooseRandomPosition), b_+11);

state1:
  CYC(b_+11, b_+13); C = 0x10;
  CALL_C(b_+13, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto ret; } // jr nz
  CYC(b_+16, b_+18);

  // Rock has hit the ground
  CALL_C(b_+18, objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+21);
  if (F & FC) { CYCT(b_+21, b_+24); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+21, b_+24);

  CYC(b_+24, b_+26); A = 0xa5; // SND_BREAK_ROCK
  CALL_C(b_+26, playSound_b00_hook, SYM(playSound_b00), b_+29);
  CYC(b_+29, b_+32); push_effect(gb, b_+32); goto spawnDebris;
afterSpawnDebris:
  CYC(b_+32, b_+34); A = 0x04;
  CALL_C(b_+34, setScreenShakeCounter_hook, SYM(setScreenShakeCounter), b_+37);
  CYC(b_+37, b_+40); interactionDelete_hook(gb); return; // jp

ret:
  RET(b_+40); return;

  // fallingRock_subid01@spawnDebris; single caller (state1 above), top-level depth sp0_.
spawnDebris:
  CALL_C(b_+41, getRandomNumber_hook, SYM(getRandomNumber), b_+44);
  CYC(b_+44, b_+46); alu_and(gb, 0x03);
  CYC(b_+46, b_+47); C = A;
  CYC(b_+47, b_+49); B = 0x00;

next:
  CYC(b_+49, b_+50); push_effect(gb, BC);
  CYC(b_+50, b_+53); SET_BC(0x9202); // INTERAC_FALLING_ROCK, $02
  CALL_C(b_+53, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+56);
  CYC(b_+56, b_+57); SET_BC(pop_effect(gb));
  if (!(F & FZ)) {
    RET_TAKEN(b_+57);
    if (gb->pc == b_+32 && gb->sp == sp0_) goto afterSpawnDebris;
    return;
  } // ret nz
  CYC(b_+57, b_+58);
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+60, b_+61); mem_wr(gb, HL, C);
  CYC(b_+61, b_+63); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+63, b_+64); mem_wr(gb, HL, B);
  CYC(b_+64, b_+65); B = alu_inc8(gb, B);
  CYC(b_+65, b_+66); A = B;
  CYC(b_+66, b_+68); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+68, b_+70); goto next; } // jr nz
  CYC(b_+68, b_+70);
  RET(b_+70);
  if (gb->pc == b_+32 && gb->sp == sp0_) goto afterSpawnDebris;
  return; // ret
}

// Used by gorons when freeing elder?
void fallingRock_subid02_hook(GB *gb) {
  BASE(fallingRock_subid02);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // fallingRock_subid02@state0
  CALL_C(b_+5, fallingRock_initGraphicsAndIncState_hook, SYM(fallingRock_initGraphicsAndIncState), b_+8);
  CALL_C(b_+8, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+11);
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto highAngleOffset; } // jr nz
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+22); goto loadAngle; // jr

highAngleOffset:
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_add(gb, 0x04);

  // fallingRock_subid02@loadAngle
  // Table is read at ([counter1] + ([var03] != 0 ? 4 : 0)) * 4 + [angle].
loadAngle:
  CYC(b_+27, b_+28); alu_add(gb, A);
  CYC(b_+28, b_+29); alu_add(gb, A);
  CYC(b_+29, b_+31); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+31, b_+32); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+32, b_+35); SET_BC(b_+70); // @angles
  CALL_C(b_+35, addAToBc_hook, 0x006d, b_+38);
  CYC(b_+38, b_+39); A = mem_rd(gb, BC);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A);
  CYC(b_+40, b_+42); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+42, b_+43); A = mem_rd(gb, HL);
  CYC(b_+43, b_+44); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); goto lowSpeed; } // jr nz
  CYC(b_+44, b_+46);

  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+52, b_+54); A = 0x18;
  CYC(b_+54, b_+55); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0xff);
  RET(b_+57); return;

lowSpeed:
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+62, b_+64); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+64, b_+66); A = 0x1c;
  CYC(b_+66, b_+67); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0xff);
  RET(b_+69); return;

state1:
  CYC(b_+94, b_+97); A = W8(wTmpcfc0_goronCutscenes_cfde);
  CYC(b_+97, b_+98); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+98, b_+101); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+98, b_+101);
  fallingRock_updateSpeedAndDeleteWhenLanded_hook(gb); return; // falls through
}

void fallingRock_updateSpeedAndDeleteWhenLanded_hook(GB *gb) {
  BASE(fallingRock_updateSpeedAndDeleteWhenLanded);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x18;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+11); objectApplySpeed_hook(gb); return; // jp
}

// A twinkle? angle is a value from 0-3, indicating a diagonal to move in.
void fallingRock_subid03_hook(GB *gb) {
  BASE(fallingRock_subid03);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); fallingRock_subid03_state1_hook(gb); return; } // jr nz
  CYC(b_+3, b_+5);

  // fallingRock_subid03@state0
  CALL_C(b_+5, fallingRock_initGraphicsAndIncState_hook, SYM(fallingRock_initGraphicsAndIncState), b_+8);
  CALL_C(b_+8, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), SYM(fallingRock_initDiagonalAngle));
  fallingRock_initDiagonalAngle_hook(gb); return; // falls through
}

void fallingRock_initDiagonalAngle_hook(GB *gb) {
  BASE(fallingRock_initDiagonalAngle);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+6); SET_BC(b_+16); // @diagonalAngles
  CALL_C(b_+6, addAToBc_hook, 0x006d, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, BC);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x28); // SPEED_100
  RET(b_+15); return;
}

void fallingRock_subid03_state1_hook(GB *gb) {
  BASE(fallingRock_subid03_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+5, b_+8); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CALL_C(b_+8, interactionAnimate_hook, SYM(interactionAnimate), b_+11);
  CYC(b_+11, b_+14); objectApplySpeed_hook(gb); return; // jp
}

// Blue/Red rock debris, moving straight on a diagonal? (angle from 0-3). fallingRock_subid04
// and fallingRock_subid05 both target 0a:7533 in interactionCode92's jump table -- identical
// code, so subid04 just hands off to subid05.
void fallingRock_subid05_hook(GB *gb) {
  BASE(fallingRock_subid04);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // fallingRock_subid05@state0
  CALL_C(b_+5, fallingRock_initGraphicsAndIncState_hook, SYM(fallingRock_initGraphicsAndIncState), b_+8);
  CALL_C(b_+8, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+11);
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x0c);
  CYC(b_+15, b_+17); fallingRock_initDiagonalAngle_hook(gb); return; // jr

state1:
  CALL_C(b_+17, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+20);
  if (F & FZ) { CYCT(b_+20, b_+23); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+20, b_+23);
  CALL_C(b_+23, interactionAnimate_hook, SYM(interactionAnimate), b_+26);
  CYC(b_+26, b_+29); objectApplySpeed_hook(gb); return; // jp
}

void fallingRock_subid04_hook(GB *gb) {
  fallingRock_subid05_hook(gb);
}

// Debris from pickaxe workers?
void fallingRock_subid06_hook(GB *gb) {
  BASE(fallingRock_subid06);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); fallingRock_updateSpeedAndDeleteWhenLanded_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);

  // fallingRock_subid06@state0
  CALL_C(b_+6, fallingRock_initGraphicsAndIncState_hook, SYM(fallingRock_initGraphicsAndIncState), b_+9);
  CALL_C(b_+9, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+12);
  CYC(b_+12, b_+14); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_or(gb, 0x08);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+24, b_+26); goto haveCounter2; } // jr z
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); A = alu_dec8(gb, A);

haveCounter2:
  CYC(b_+27, b_+28); B = A;
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); alu_and(gb, 0xbc);
  CYC(b_+33, b_+34); alu_or(gb, B);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A);
  CYC(b_+35, b_+37); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+37, b_+38); A = mem_rd(gb, HL);
  CYC(b_+38, b_+41); SET_BC(b_+58); // @angles
  CALL_C(b_+41, addAToBc_hook, 0x006d, b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, BC);
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+52, b_+54); A = 0x40;
  CYC(b_+54, b_+55); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0xff);
  RET(b_+57); return;
}

void fallingRock_initGraphicsAndIncState_hook(GB *gb) {
  BASE(fallingRock_initGraphicsAndIncState);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+6);
  CYC(b_+6, b_+9); interactionIncState_hook(gb); return; // jp
}

// Randomly choose a position from a list of possible positions. var03 determines which
// list it reads from?
void fallingRock_chooseRandomPosition_hook(GB *gb) {
  BASE(fallingRock_chooseRandomPosition);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+7); SET_HL(b_+39); // @positionList1
  if (F & FZ) { CYCT(b_+7, b_+9); goto pickPosition; } // jr z
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_HL(b_+71); // @positionList2
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+14, b_+16); A = 0x04;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);

pickPosition:
  CALL_C(b_+17, getRandomNumber_hook, SYM(getRandomNumber), b_+20);
  CYC(b_+20, b_+22); alu_and(gb, 0x0f);
  CYC(b_+22, b_+23); fallingRock_add_double_index(gb, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); alu_cpl(gb);
  CYC(b_+28, b_+29); A = alu_inc8(gb, A);
  CYC(b_+29, b_+31); alu_sub(gb, 0x08);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+37, b_+38); mem_wr(gb, DE, A);
  RET(b_+38); return;
}
