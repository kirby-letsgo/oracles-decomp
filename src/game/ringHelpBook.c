#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/common/interactions/ringHelpBook.s (INTERAC_RING_HELP_BOOK), bank $10.

static uint16_t interactionCodee5_jump_table(GB *gb) {
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

void interactionCodee5__runState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7063, 0x7065); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7065, 0x7066); A = mem_rd(gb, DE);
  CYC(0x7066, 0x7067); push_effect(gb, 0x7067);
  switch (interactionCodee5_jump_table(gb)) {
    case 0x706b: goto state0;
    case 0x7097: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }
state0:
  CALL_C(0x706b, interactionInitGraphics_hook, 0x15fb, 0x706e);
  CYC(0x706e, 0x7070); A = 0x30;
  CALL_C(0x7070, interactionSetHighTextIndex_hook, 0x253b, 0x7073);
  CALL_C(0x7073, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7076);
  CALL_C(0x7076, interactionIncState_hook, 0x23e0, 0x7079);
  CYC(0x7079, 0x707b); A = 0x06;
  CALL_C(0x707b, objectSetCollideRadius_hook, 0x24a1, 0x707e);
  CYC(0x707e, 0x7080); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7080, 0x7081); A = mem_rd(gb, DE);
  CYC(0x7081, 0x7084); SET_HL(0x4b44);
  CYC(0x7084, 0x7085); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7085, 0x7087);
    goto setScript;
  }
  CYC(0x7085, 0x7087);
  CYC(0x7087, 0x7089); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x7089, 0x708a); A = mem_rd(gb, DE);
  CYC(0x708a, 0x708b); A = alu_inc8(gb, A);
  CYC(0x708b, 0x708c); mem_wr(gb, DE, A);
  CYC(0x708c, 0x708f); SET_HL(0x4b35);
setScript:
  CALL_C(0x708f, interactionSetScript_hook, 0x2544, 0x7092);
  CYC(0x7092, 0x7094); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(0x7094, 0x7097);
  objectAddToAButtonSensitiveObjectList_hook(gb);
  return;
state1:
  CYC(0x7097, 0x709a);
  interactionRunScript_hook(gb);
}

void interactionCodee5_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x704d, 0x7050); A = W8(wOamEnd);
  CYC(0x7050, 0x7051); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7051, 0x7053);
    goto doneTextFlagSetup;
  }
  CYC(0x7051, 0x7053);
  CYC(0x7053, 0x7055); A = 0x02;
  CYC(0x7055, 0x7058); W8(wTextboxPosition) = A;
  CYC(0x7058, 0x705a); A = 0x08;
  CYC(0x705a, 0x705d); W8(wTextboxFlags) = A;
doneTextFlagSetup:
  CYC(0x705d, 0x7060); push_effect(gb, 0x7060);
  interactionCodee5__runState_hook(gb);
  CYC(0x7060, 0x7063);
  objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}
