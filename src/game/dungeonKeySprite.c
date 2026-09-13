#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

#define keyDoorGraphicTable_bank08 0x466b

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
  uint16_t sp0_ = gb->sp;
  CYC(0x4629, 0x462b); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x462b, 0x462c); A = mem_rd(gb, DE);
  CYC(0x462c, 0x462d); push_effect(gb, 0x462d);
  switch (dungeonKeySprite_jumpTable(gb)) {
    case 0x4633: goto state0;
    case 0x4655: goto state1;
    case 0x4662: goto state2;
    default: HANDOFF(HL);
  }

state0:
  CALL_C(0x4633, interactionIncState_hook, 0x23e0, 0x4636);
  CYC(0x4636, 0x4638); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x4638, 0x463a); mem_wr(gb, HL, 0xfc);
  CYC(0x463a, 0x463c); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x463c, 0x463e); mem_wr(gb, HL, 0x08);
  // Subid is the tile index of the door being opened; convert it to a graphic subid.
  CYC(0x463e, 0x4640); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4640, 0x4641); A = mem_rd(gb, HL);
  CYC(0x4641, 0x4644); SET_HL(keyDoorGraphicTable_bank08);
  CALL_C(0x4644, lookupCollisionTable_hook, 0x1e1f, 0x4647);
  CYC(0x4647, 0x4649); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4649, 0x464a); mem_wr(gb, DE, A);
  CALL_C(0x464a, interactionInitGraphics_hook, 0x15fb, 0x464d);
  CALL_C(0x464d, objectSetVisible80_hook, 0x1e57, 0x4650);
  CYC(0x4650, 0x4652); A = 0x5e; // SND_GETSEED
  CYC(0x4652, 0x4655); playSound_b00_hook(gb);
  return;

state1:
  CALL_C(0x4655, interactionDecCounter1_hook, 0x23cc, 0x4658);
  if (!(F & FZ)) {
    CYCT(0x4658, 0x4659); ret_effect(gb); return;
  }
  CYC(0x4658, 0x4659);
  CYC(0x4659, 0x465b); mem_wr(gb, HL, 0x14);
  CYC(0x465b, 0x465d); L = INTERACTION_BASE + OBJ_ZH;
  CYC(0x465d, 0x465f); mem_wr(gb, HL, 0xf8);
  CYC(0x465f, 0x4662); interactionIncState_hook(gb);
  return;

state2:
  CALL_C(0x4662, interactionDecCounter1_hook, 0x23cc, 0x4665);
  if (!(F & FZ)) {
    CYCT(0x4665, 0x4666); ret_effect(gb); return;
  }
  CYC(0x4665, 0x4666);
  CYC(0x4666, 0x4668); mem_wr(gb, HL, 0x0f);
  CYC(0x4668, 0x466b); interactionDelete_hook(gb);
}
