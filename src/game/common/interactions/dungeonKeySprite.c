#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

#define keyDoorGraphicTable_bank08 SYM(keyDoorGraphicTable)

static uint16_t dungeonKeySprite_jumpTable(GB *gb) {
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

// INTERAC_DUNGEON_KEY_SPRITE: the key sprite shown when a dungeon key door opens.
void interactionCode17_hook(GB *gb) {
  BASE(interactionCode17);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (dungeonKeySprite_jumpTable(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+44) { goto state1; }
    else if (jt_ == b_+57) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+10, interactionIncState_hook, SYM(interactionIncState), b_+13);
  CYC(b_+13, b_+15); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0xfc);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x08);
  // Subid is the tile index of the door being opened; convert it to a graphic subid.
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+27); SET_HL(keyDoorGraphicTable_bank08);
  CALL_C(b_+27, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+30);
  CYC(b_+30, b_+32); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CALL_C(b_+33, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+36);
  CALL_C(b_+36, objectSetVisible80_hook, SYM(objectSetVisible80), b_+39);
  CYC(b_+39, b_+41); A = 0x5e; // SND_GETSEED
  CYC(b_+41, b_+44); playSound_b00_hook(gb);
  return;

state1:
  CALL_C(b_+44, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+47);
  if (!(F & FZ)) {
    CYCT(b_+47, b_+48); ret_effect(gb); return;
  }
  CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x14);
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0xf8);
  CYC(b_+54, b_+57); interactionIncState_hook(gb);
  return;

state2:
  CALL_C(b_+57, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+60);
  if (!(F & FZ)) {
    CYCT(b_+60, b_+61); ret_effect(gb); return;
  }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+63); mem_wr(gb, HL, 0x0f);
  CYC(b_+63, b_+66); interactionDelete_hook(gb);
}
