#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// interactionCode12@dungeonTextIndices: low byte of TX_02xx, one per dungeon.
#define dungeonTextIndices_bank08 0x4320
// interactionCode12@initialSpinnerValues: initial wSpinnerState, one per dungeon.
#define initialSpinnerValues_bank08 0x4330
// interactionCode12@replacementTiles: staircase tiles replacing tiles $40-$43.
#define replacementTiles_bank08 0x43be

// mainScripts.* (bank $0c script data, referenced by address only).
#define dropSmallKeyWhenNoEnemiesScript_bank0c 0x4683
#define createChestWhenNoEnemiesScript_bank0c 0x4689
#define setRoomFlagBit7WhenNoEnemiesScript_bank0c 0x4697

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
  CALL_C(0x43c2, getFreeInteractionSlot_hook, 0x3aef, 0x43c5);
  if (!(F & FZ)) {
    CYCT(0x43c5, 0x43c6); ret_effect(gb); return;
  }
  CYC(0x43c5, 0x43c6);
  CYC(0x43c6, 0x43c8); mem_wr(gb, HL, 0x05);
  CYC(0x43c8, 0x43ca); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x43ca, 0x43cd); setShortPosition_paramC_hook(gb);
}

// interactionCode12@createStaircaseTile: replace tile $40+A at position C with a staircase
// and puff. Reached only by a static `call c` from interactionCode12 subid 4; not
// independently hookable.
static void dungeonStuff_createStaircaseTile(GB *gb, uint16_t sp0_) {
  CYC(0x43ae, 0x43af); push_effect(gb, BC);
  CYC(0x43af, 0x43b0); push_effect(gb, HL);
  CYC(0x43b0, 0x43b3); SET_HL(replacementTiles_bank08);
  CYC(0x43b3, 0x43b4); push_effect(gb, 0x43b4); dungeonStuff_addAToHl(gb);
  CYC(0x43b4, 0x43b5); A = mem_rd(gb, HL);
  CALL_C(0x43b5, setTile_hook, 0x3a9c, 0x43b8);
  CYC(0x43b8, 0x43bb); push_effect(gb, 0x43bb); dungeonStuff_createPuff(gb, sp0_);
  CYC(0x43bb, 0x43bc); SET_HL(pop_effect(gb));
  CYC(0x43bc, 0x43bd); SET_BC(pop_effect(gb));
  CYC(0x43bd, 0x43be); ret_effect(gb);
}

void interactionCode12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x42d0, 0x42d2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x42d2, 0x42d3); A = mem_rd(gb, DE);
  CYC(0x42d3, 0x42d4); push_effect(gb, 0x42d4);
  switch (dungeonStuff_jumpTable(gb)) {
    case 0x42de: goto subid00;
    case 0x4340: goto subid01;
    case 0x435b: goto subid02;
    case 0x4375: goto subid03;
    case 0x4385: goto subid04;
    default: HANDOFF(HL);
  }

subid00:
  CALL_C(0x42de, checkInteractionState_hook, 0x23fe, 0x42e1);
  if (!(F & FZ)) {
    CYCT(0x42e1, 0x42e3); goto initialized;
  }
  CYC(0x42e1, 0x42e3);
  CYC(0x42e3, 0x42e6); A = mem_rd(gb, wScrollMode);
  CYC(0x42e6, 0x42e8); alu_and(gb, 0x02);
  if (F & FZ) {
    CYCT(0x42e8, 0x42eb); interactionDelete_hook(gb); return;
  }
  CYC(0x42e8, 0x42eb);
  CYC(0x42eb, 0x42ee); A = W8(w1Link_yh);
  CYC(0x42ee, 0x42f0); alu_cp(gb, 0x78);
  if (F & FC) {
    CYCT(0x42f0, 0x42f3); interactionDelete_hook(gb); return;
  }
  CYC(0x42f0, 0x42f3);
  CALL_C(0x42f3, interactionIncState_hook, 0x23e0, 0x42f6);
  CYC(0x42f6, 0x42f8); A = 0x08;
  CALL_C(0x42f8, objectSetCollideRadius_hook, 0x24a1, 0x42fb);
  CALL_C(0x42fb, initializeDungeonStuff_hook, 0x2dc4, 0x42fe);
  CYC(0x42fe, 0x4301); A = mem_rd(gb, wDungeonIndex);
  CYC(0x4301, 0x4304); SET_HL(initialSpinnerValues_bank08);
  CYC(0x4304, 0x4305); push_effect(gb, 0x4305); dungeonStuff_addAToHl(gb);
  CYC(0x4305, 0x4306); A = mem_rd(gb, HL);
  CYC(0x4306, 0x4309); mem_wr(gb, wSpinnerState, A);

initialized:
  CALL_C(0x4309, objectCheckCollidedWithLink_notDead_hook, 0x1c2e, 0x430c);
  if (!(F & FC)) {
    CYCT(0x430c, 0x430d); ret_effect(gb); return;
  }
  CYC(0x430c, 0x430d);
  CYC(0x430d, 0x4310); A = mem_rd(gb, wDungeonIndex);
  CYC(0x4310, 0x4313); SET_HL(dungeonTextIndices_bank08);
  CYC(0x4313, 0x4314); push_effect(gb, 0x4314); dungeonStuff_addAToHl(gb);
  CYC(0x4314, 0x4315); C = mem_rd(gb, HL);
  CYC(0x4315, 0x4317); B = 0x02;
  CALL_C(0x4317, showText_hook, 0x1872, 0x431a);
  CALL_C(0x431a, setDeathRespawnPoint_hook, 0x1100, 0x431d);
  CYC(0x431d, 0x4320); interactionDelete_hook(gb);
  return;

subid01:
  CALL_C(0x4340, returnIfScrollMode01Unset_hook, 0x26e4, 0x4343);
  CYC(0x4343, 0x4345); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x4345, 0x4346); A = mem_rd(gb, DE);
  CYC(0x4346, 0x4347); push_effect(gb, 0x4347);
  switch (dungeonStuff_jumpTable(gb)) {
    case 0x434b: goto subid01_substate0;
    case 0x4354: goto runScript;
    default: HANDOFF(HL);
  }

subid01_substate0:
  CYC(0x434b, 0x434d); A = 0x01;
  CYC(0x434d, 0x434e); mem_wr(gb, DE, A);
  CYC(0x434e, 0x4351); SET_HL(dropSmallKeyWhenNoEnemiesScript_bank0c);
  CALL_C(0x4351, interactionSetScript_hook, 0x2544, 0x4354);

runScript:
  CALL_C(0x4354, interactionRunScript_hook, 0x2552, 0x4357);
  if (F & FC) {
    CYCT(0x4357, 0x435a); interactionDelete_hook(gb); return;
  }
  CYC(0x4357, 0x435a);
  CYC(0x435a, 0x435b); ret_effect(gb);
  return;

subid02:
  CYC(0x435b, 0x435d); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x435d, 0x435e); A = mem_rd(gb, DE);
  CYC(0x435e, 0x435f); push_effect(gb, 0x435f);
  switch (dungeonStuff_jumpTable(gb)) {
    case 0x4365: goto subid02_substate0;
    case 0x4354: goto runScript;
    case 0x4370: goto subid02_substate2;
    default: HANDOFF(HL);
  }

subid02_substate0:
  CYC(0x4365, 0x4367); A = 0x01;
  CYC(0x4367, 0x4368); mem_wr(gb, DE, A);
  CYC(0x4368, 0x436b); SET_HL(createChestWhenNoEnemiesScript_bank0c);
  CALL_C(0x436b, interactionSetScript_hook, 0x2544, 0x436e);
  CYC(0x436e, 0x4370); goto runScript;

subid02_substate2:
  CALL_C(0x4370, objectPreventLinkFromPassing_hook, 0x2680, 0x4373);
  CYC(0x4373, 0x4375); goto runScript;

subid03:
  CALL_C(0x4375, checkInteractionState_hook, 0x23fe, 0x4378);
  if (!(F & FZ)) {
    CYCT(0x4378, 0x437a); goto runScript;
  }
  CYC(0x4378, 0x437a);
  CYC(0x437a, 0x437c); A = 0x01;
  CYC(0x437c, 0x437d); mem_wr(gb, DE, A);
  CYC(0x437d, 0x4380); SET_HL(setRoomFlagBit7WhenNoEnemiesScript_bank0c);
  CALL_C(0x4380, interactionSetScript_hook, 0x2544, 0x4383);
  CYC(0x4383, 0x4385); goto runScript;

subid04:
  CALL_C(0x4385, returnIfScrollMode01Unset_hook, 0x26e4, 0x4388);
  CALL_C(0x4388, getThisRoomFlags_hook, 0x197d, 0x438b);
  CYC(0x438b, 0x438d); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x438d, 0x4390); interactionDelete_hook(gb); return;
  }
  CYC(0x438d, 0x4390);
  CYC(0x4390, 0x4393); A = mem_rd(gb, wNumEnemies);
  CYC(0x4393, 0x4394); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4394, 0x4395); ret_effect(gb); return;
  }
  CYC(0x4394, 0x4395);
  CYC(0x4395, 0x4397); A = 0x4d;
  CALL_C(0x4397, playSound_b00_hook, 0x0c98, 0x439a);
  CALL_C(0x439a, getThisRoomFlags_hook, 0x197d, 0x439d);
  CYC(0x439d, 0x439f); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 7));
  CYC(0x439f, 0x43a2); SET_BC(wRoomLayout + LARGE_ROOM_HEIGHT * 16 - 1);
  for (;;) {
    CYC(0x43a2, 0x43a3); A = mem_rd(gb, BC);
    CYC(0x43a3, 0x43a5); alu_sub(gb, 0x40);
    CYC(0x43a5, 0x43a7); alu_cp(gb, 0x04);
    if (F & FC) {
      CYCT(0x43a7, 0x43aa); push_effect(gb, 0x43aa); dungeonStuff_createStaircaseTile(gb, sp0_);
    } else {
      CYC(0x43a7, 0x43aa);
    }
    CYC(0x43aa, 0x43ab); C = alu_dec8(gb, C);
    if (F & FZ) {
      CYC(0x43ab, 0x43ad);
      break;
    }
    CYCT(0x43ab, 0x43ad);
  }
  CYC(0x43ad, 0x43ae); ret_effect(gb);
}
