#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// interactionCode38@scriptTable: one mainScripts.pastGirlScript_* pointer per game
// progress value returned by getGameProgress_2.
#define interactionCode38_scriptTable_bank08 SYM(interactionCode38__scriptTable)
#define getGameProgress_2_bank09 SYM(getGameProgress_2)

static uint16_t pastGirl_jumpTable(GB *gb) {
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

static void pastGirl_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_PAST_GIRL: the girl in the past whose script depends on game progress.
void interactionCode38_hook(GB *gb) {
  BASE(interactionCode38);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (pastGirl_jumpTable(gb));
    if (jt_ == b_+8) { goto state0; }
    else if (jt_ == b_+63) { goto state1; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+19); A = 0x1a; // >TX_1a00
  CALL_C(b_+19, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); push_effect(gb, b_+26);
  do { uint16_t jt_ = (pastGirl_jumpTable(gb));
    if (jt_ == b_+28) { goto subid0Init; }
    else { HANDOFF(HL); }
  } while (0);

subid0Init:
  // callab agesInteractionsBank09.getGameProgress_2
  CYC(b_+28, b_+31); SET_HL(getGameProgress_2_bank09);
  CYC(b_+31, b_+33); E = 0x09;
  CALL_C(b_+33, interBankCall_hook, 0x008a, b_+36);
  // NPC doesn't exist between beating d2 and saving Nayru
  CYC(b_+36, b_+37); A = B;
  CYC(b_+37, b_+39); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(b_+39, b_+42); interactionDelete_hook(gb); return;
  }
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+44); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(b_+44, b_+47); interactionDelete_hook(gb); return;
  }
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+48); A = B;
  CYC(b_+48, b_+51); SET_HL(interactionCode38_scriptTable_bank08);
  CYC(b_+51, b_+52); pastGirl_addDoubleIndex(gb, b_+52);
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+53, b_+54); H = mem_rd(gb, HL);
  CYC(b_+54, b_+55); L = A;
  CALL_C(b_+55, interactionSetScript_hook, SYM(interactionSetScript), b_+58);
  CALL_C(b_+58, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+61);
  CYC(b_+61, b_+63);

state1:
  CYC(b_+63, b_+65); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+67); push_effect(gb, b_+67);
  do { uint16_t jt_ = (pastGirl_jumpTable(gb));
    if (jt_ == b_+69) { goto subid0; }
    else { HANDOFF(HL); }
  } while (0);

subid0:
  CALL_C(b_+69, interactionRunScript_hook, SYM(interactionRunScript), b_+72);
  CYC(b_+72, b_+75); interactionAnimateAsNpc_hook(gb);
}
