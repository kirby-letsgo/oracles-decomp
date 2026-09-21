#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode77_jump_table(GB *gb) {
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

// INTERAC_SMALL_KEY_ON_ENEMY
void interactionCode77_hook(GB *gb) {
  BASE(interactionCode77);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode77_jump_table(gb);
    if (target == b_+63) goto state1;
    if (target == b_+86) goto state2;
  }

  // state0
  CALL_C(b_+10, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x20); // ROOMFLAG_ITEM
  if (!(F & FZ)) { CYCT(b_+15, b_+18); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+25); SET_HL(ENEMY_SLOTS + OBJ_ID); // FIRST_ENEMY_INDEX, Enemy.id

nextEnemy:
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+27); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+27, b_+29); goto foundMatch; } // jr z
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); H = alu_inc8(gb, H);
  CYC(b_+30, b_+31); A = H;
  CYC(b_+31, b_+33); alu_cp(gb, 0xe0); // LAST_ENEMY_INDEX+1
  // BUG: original game checks carry here, so this only works if the enemy is in the first slot.
  if (F & FC) { CYCT(b_+33, b_+36); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+33, b_+36);
  CYC(b_+36, b_+38); goto nextEnemy; // jr

foundMatch:
  CYC(b_+38, b_+39); L = alu_dec8(gb, L);
  CYC(b_+39, b_+40); A = L;
  CYC(b_+40, b_+42); E = INTERACTION_BASE + OBJ_RELATED2; // Interaction.relatedObj2 low byte
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+44); A = H;
  CYC(b_+44, b_+45); E = alu_inc8(gb, E);
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  CALL_C(b_+46, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+49); // SWITCHES THREADS
  CALL_C(b_+49, objectSetVisible80_hook, SYM(objectSetVisible80), b_+52);
  CALL_C(b_+52, interactionIncState_hook, SYM(interactionIncState), b_+55);

takeRelatedObj2Position:
  CYC(b_+55, b_+57); A = OBJ_Y; // Object.y
  CALL_C(b_+57, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+60);
  CYC(b_+60, b_+63); objectTakePosition_hook(gb);
  if (gb->pc == b_+76 && gb->sp == sp0_) goto afterTakeRelatedObj2Position;
  return; // jp

state1:
  CYC(b_+63, b_+65); A = OBJ_ENABLED; // Object.enabled
  CALL_C(b_+65, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+68);
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  CYC(b_+69, b_+70); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+70, b_+73); interactionIncState_hook(gb); return; } // jp z
  CYC(b_+70, b_+73);
  CYC(b_+73, b_+76); push_effect(gb, b_+76); goto takeRelatedObj2Position;

afterTakeRelatedObj2Position:
  CYC(b_+76, b_+78); A = OBJ_VISIBLE; // Object.visible
  CALL_C(b_+78, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+81);
  CYC(b_+81, b_+83); B = 0x01;
  CYC(b_+83, b_+86); TAIL(objectFlickerVisibility); // jp

state2:
  CALL_C(b_+86, objectSetVisible_hook, SYM(objectSetVisible), b_+89);
  CYC(b_+89, b_+91); C = 0x20;
  CALL_C(b_+91, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+94);
  if (!(F & FZ)) { RET_TAKEN(b_+94); return; } // ret nz
  CYC(b_+94, b_+95);
  CYC(b_+95, b_+98); SET_BC(0x3000); // TREASURE_SMALL_KEY, $00
  CALL_C(b_+98, createTreasure_hook, SYM(createTreasure), b_+101);
  CALL_C(b_+101, objectCopyPosition_hook, SYM(objectCopyPosition), b_+104);
  CYC(b_+104, b_+107); TAIL(interactionDelete); // jp
}
