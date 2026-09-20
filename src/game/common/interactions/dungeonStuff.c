#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// interactionCode12@dungeonTextIndices: low byte of TX_02xx, one per dungeon.
#define dungeonTextIndices_bank08 SYM(interactionCode12__dungeonTextIndices)
// interactionCode12@initialSpinnerValues: initial wSpinnerState, one per dungeon.
#define initialSpinnerValues_bank08 SYM(interactionCode12__initialSpinnerValues)
// interactionCode12@replacementTiles: staircase tiles replacing tiles $40-$43.
#define replacementTiles_bank08 SYM(interactionCode12__replacementTiles)

// mainScripts.* (bank $0c script data, referenced by address only).
#define dropSmallKeyWhenNoEnemiesScript_bank0c SYM(dropSmallKeyWhenNoEnemiesScript)
#define createChestWhenNoEnemiesScript_bank0c SYM(createChestWhenNoEnemiesScript)
#define setRoomFlagBit7WhenNoEnemiesScript_bank0c SYM(setRoomFlagBit7WhenNoEnemiesScript)

#define LARGE_ROOM_HEIGHT 11

static uint16_t dungeonStuff_jumpTable(GB *gb) {
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

static void dungeonStuff_addAToHl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

// interactionCode12@createPuff: spawn INTERAC_PUFF at the short position in C. Reached only
// by a static `call` from createStaircaseTile; not independently hookable.
static void dungeonStuff_createPuff(GB *gb, uint16_t sp0_) {
  BASE(interactionCode12);
  CALL_C(b_+242, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+245);
  if (!(F & FZ)) {
    CYCT(b_+245, b_+246); ret_effect(gb); return;
  }
  CYC(b_+245, b_+246);
  CYC(b_+246, b_+248); mem_wr(gb, HL, 0x05);
  CYC(b_+248, b_+250); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+250, b_+253); setShortPosition_paramC_hook(gb);
}

// interactionCode12@createStaircaseTile: replace tile $40+A at position C with a staircase
// and puff. Reached only by a static `call c` from interactionCode12 subid 4; not
// independently hookable.
static void dungeonStuff_createStaircaseTile(GB *gb, uint16_t sp0_) {
  BASE(interactionCode12);
  CYC(b_+222, b_+223); push_effect(gb, BC);
  CYC(b_+223, b_+224); push_effect(gb, HL);
  CYC(b_+224, b_+227); SET_HL(replacementTiles_bank08);
  CYC(b_+227, b_+228); push_effect(gb, b_+228); dungeonStuff_addAToHl(gb);
  CYC(b_+228, b_+229); A = mem_rd(gb, HL);
  CALL_C(b_+229, setTile_hook, SYM(setTile), b_+232);
  CYC(b_+232, b_+235); push_effect(gb, b_+235); dungeonStuff_createPuff(gb, sp0_);
  CYC(b_+235, b_+236); SET_HL(pop_effect(gb));
  CYC(b_+236, b_+237); SET_BC(pop_effect(gb));
  CYC(b_+237, b_+238); ret_effect(gb);
}

void interactionCode12_hook(GB *gb) {
  BASE(interactionCode12);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dungeonStuff_jumpTable(gb));
    if (jt_ == b_+14) { goto subid00; }
    else if (jt_ == b_+112) { goto subid01; }
    else if (jt_ == b_+139) { goto subid02; }
    else if (jt_ == b_+165) { goto subid03; }
    else if (jt_ == b_+181) { goto subid04; }
    else { HANDOFF(HL); }
  } while (0);

subid00:
  CALL_C(b_+14, checkInteractionState_hook, SYM(checkInteractionState), b_+17);
  if (!(F & FZ)) {
    CYCT(b_+17, b_+19); goto initialized;
  }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); A = mem_rd(gb, wScrollMode);
  CYC(b_+22, b_+24); alu_and(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+24, b_+27); interactionDelete_hook(gb); return;
  }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+30); A = W8(w1Link_yh);
  CYC(b_+30, b_+32); alu_cp(gb, 0x78);
  if (F & FC) {
    CYCT(b_+32, b_+35); interactionDelete_hook(gb); return;
  }
  CYC(b_+32, b_+35);
  CALL_C(b_+35, interactionIncState_hook, SYM(interactionIncState), b_+38);
  CYC(b_+38, b_+40); A = 0x08;
  CALL_C(b_+40, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+43);
  CALL_C(b_+43, initializeDungeonStuff_hook, SYM(initializeDungeonStuff), b_+46);
  CYC(b_+46, b_+49); A = mem_rd(gb, wDungeonIndex);
  CYC(b_+49, b_+52); SET_HL(initialSpinnerValues_bank08);
  CYC(b_+52, b_+53); push_effect(gb, b_+53); dungeonStuff_addAToHl(gb);
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+57); mem_wr(gb, wSpinnerState, A);

initialized:
  CALL_C(b_+57, objectCheckCollidedWithLink_notDead_hook, SYM(objectCheckCollidedWithLink_notDead), b_+60);
  if (!(F & FC)) {
    CYCT(b_+60, b_+61); ret_effect(gb); return;
  }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+64); A = mem_rd(gb, wDungeonIndex);
  CYC(b_+64, b_+67); SET_HL(dungeonTextIndices_bank08);
  CYC(b_+67, b_+68); push_effect(gb, b_+68); dungeonStuff_addAToHl(gb);
  CYC(b_+68, b_+69); C = mem_rd(gb, HL);
  CYC(b_+69, b_+71); B = 0x02;
  CALL_C(b_+71, showText_hook, SYM(showText), b_+74);
  CALL_C(b_+74, setDeathRespawnPoint_hook, SYM(setDeathRespawnPoint), b_+77);
  CYC(b_+77, b_+80); interactionDelete_hook(gb);
  return;

subid01:
  CALL_C(b_+112, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+115);
  CYC(b_+115, b_+117); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+117, b_+118); A = mem_rd(gb, DE);
  CYC(b_+118, b_+119); push_effect(gb, b_+119);
  do { uint16_t jt_ = (dungeonStuff_jumpTable(gb));
    if (jt_ == b_+123) { goto subid01_substate0; }
    else if (jt_ == b_+132) { goto runScript; }
    else { HANDOFF(HL); }
  } while (0);

subid01_substate0:
  CYC(b_+123, b_+125); A = 0x01;
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  CYC(b_+126, b_+129); SET_HL(dropSmallKeyWhenNoEnemiesScript_bank0c);
  CALL_C(b_+129, interactionSetScript_hook, SYM(interactionSetScript), b_+132);

runScript:
  CALL_C(b_+132, interactionRunScript_hook, SYM(interactionRunScript), b_+135);
  if (F & FC) {
    CYCT(b_+135, b_+138); interactionDelete_hook(gb); return;
  }
  CYC(b_+135, b_+138);
  CYC(b_+138, b_+139); ret_effect(gb);
  return;

subid02:
  CYC(b_+139, b_+141); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+143); push_effect(gb, b_+143);
  do { uint16_t jt_ = (dungeonStuff_jumpTable(gb));
    if (jt_ == b_+149) { goto subid02_substate0; }
    else if (jt_ == b_+132) { goto runScript; }
    else if (jt_ == b_+160) { goto subid02_substate2; }
    else { HANDOFF(HL); }
  } while (0);

subid02_substate0:
  CYC(b_+149, b_+151); A = 0x01;
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CYC(b_+152, b_+155); SET_HL(createChestWhenNoEnemiesScript_bank0c);
  CALL_C(b_+155, interactionSetScript_hook, SYM(interactionSetScript), b_+158);
  CYC(b_+158, b_+160); goto runScript;

subid02_substate2:
  CALL_C(b_+160, objectPreventLinkFromPassing_hook, SYM(objectPreventLinkFromPassing), b_+163);
  CYC(b_+163, b_+165); goto runScript;

subid03:
  CALL_C(b_+165, checkInteractionState_hook, SYM(checkInteractionState), b_+168);
  if (!(F & FZ)) {
    CYCT(b_+168, b_+170); goto runScript;
  }
  CYC(b_+168, b_+170);
  CYC(b_+170, b_+172); A = 0x01;
  CYC(b_+172, b_+173); mem_wr(gb, DE, A);
  CYC(b_+173, b_+176); SET_HL(setRoomFlagBit7WhenNoEnemiesScript_bank0c);
  CALL_C(b_+176, interactionSetScript_hook, SYM(interactionSetScript), b_+179);
  CYC(b_+179, b_+181); goto runScript;

subid04:
  CALL_C(b_+181, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+184);
  CALL_C(b_+184, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+187);
  CYC(b_+187, b_+189); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(b_+189, b_+192); interactionDelete_hook(gb); return;
  }
  CYC(b_+189, b_+192);
  CYC(b_+192, b_+195); A = mem_rd(gb, wNumEnemies);
  CYC(b_+195, b_+196); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+196, b_+197); ret_effect(gb); return;
  }
  CYC(b_+196, b_+197);
  CYC(b_+197, b_+199); A = 0x4d;
  CALL_C(b_+199, playSound_b00_hook, SYM(playSound_b00), b_+202);
  CALL_C(b_+202, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+205);
  CYC(b_+205, b_+207); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 7));
  CYC(b_+207, b_+210); SET_BC(wRoomLayout + LARGE_ROOM_HEIGHT * 16 - 1);
  for (;;) {
    CYC(b_+210, b_+211); A = mem_rd(gb, BC);
    CYC(b_+211, b_+213); alu_sub(gb, 0x40);
    CYC(b_+213, b_+215); alu_cp(gb, 0x04);
    if (F & FC) {
      CYCT(b_+215, b_+218); push_effect(gb, b_+218); dungeonStuff_createStaircaseTile(gb, sp0_);
    } else {
      CYC(b_+215, b_+218);
    }
    CYC(b_+218, b_+219); C = alu_dec8(gb, C);
    if (F & FZ) {
      CYC(b_+219, b_+221);
      break;
    }
    CYCT(b_+219, b_+221);
  }
  CYC(b_+221, b_+222); ret_effect(gb);
}
