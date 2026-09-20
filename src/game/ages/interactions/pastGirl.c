#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

// interactionCode38@scriptTable: one mainScripts.pastGirlScript_* pointer per game
// progress value returned by getGameProgress_2.
#define interactionCode38_scriptTable_bank08 0x7431
#define getGameProgress_2_bank09 0x5559

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
  uint16_t sp0_ = gb->sp;
  CYC(0x73e6, 0x73e8); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x73e8, 0x73e9); A = mem_rd(gb, DE);
  CYC(0x73e9, 0x73ea); push_effect(gb, 0x73ea);
  switch (pastGirl_jumpTable(gb)) {
    case 0x73ee: goto state0;
    case 0x7425: goto state1;
    default: HANDOFF(HL);
  }

state0:
  CYC(0x73ee, 0x73f0); A = 0x01;
  CYC(0x73f0, 0x73f1); mem_wr(gb, DE, A);
  CALL_C(0x73f1, interactionInitGraphics_hook, 0x15fb, 0x73f4);
  CALL_C(0x73f4, objectSetVisiblec2_hook, 0x1e45, 0x73f7);
  CYC(0x73f7, 0x73f9); A = 0x1a; // >TX_1a00
  CALL_C(0x73f9, interactionSetHighTextIndex_hook, 0x253b, 0x73fc);
  CYC(0x73fc, 0x73fe); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x73fe, 0x73ff); A = mem_rd(gb, DE);
  CYC(0x73ff, 0x7400); push_effect(gb, 0x7400);
  switch (pastGirl_jumpTable(gb)) {
    case 0x7402: goto subid0Init;
    default: HANDOFF(HL);
  }

subid0Init:
  // callab agesInteractionsBank09.getGameProgress_2
  CYC(0x7402, 0x7405); SET_HL(getGameProgress_2_bank09);
  CYC(0x7405, 0x7407); E = 0x09;
  CALL_C(0x7407, interBankCall_hook, 0x008a, 0x740a);
  // NPC doesn't exist between beating d2 and saving Nayru
  CYC(0x740a, 0x740b); A = B;
  CYC(0x740b, 0x740d); alu_cp(gb, 0x01);
  if (F & FZ) {
    CYCT(0x740d, 0x7410); interactionDelete_hook(gb); return;
  }
  CYC(0x740d, 0x7410);
  CYC(0x7410, 0x7412); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x7412, 0x7415); interactionDelete_hook(gb); return;
  }
  CYC(0x7412, 0x7415);
  CYC(0x7415, 0x7416); A = B;
  CYC(0x7416, 0x7419); SET_HL(interactionCode38_scriptTable_bank08);
  CYC(0x7419, 0x741a); pastGirl_addDoubleIndex(gb, 0x741a);
  CYC(0x741a, 0x741b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x741b, 0x741c); H = mem_rd(gb, HL);
  CYC(0x741c, 0x741d); L = A;
  CALL_C(0x741d, interactionSetScript_hook, 0x2544, 0x7420);
  CALL_C(0x7420, objectMarkSolidPosition_hook, 0x24f0, 0x7423);
  CYC(0x7423, 0x7425);

state1:
  CYC(0x7425, 0x7427); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7427, 0x7428); A = mem_rd(gb, DE);
  CYC(0x7428, 0x7429); push_effect(gb, 0x7429);
  switch (pastGirl_jumpTable(gb)) {
    case 0x742b: goto subid0;
    default: HANDOFF(HL);
  }

subid0:
  CALL_C(0x742b, interactionRunScript_hook, 0x2552, 0x742e);
  CYC(0x742e, 0x7431); interactionAnimateAsNpc_hook(gb);
}
