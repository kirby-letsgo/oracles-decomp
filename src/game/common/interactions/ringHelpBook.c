#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodee5), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodee5), (from), (to), true)

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
  BASE(interactionCodee5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); push_effect(gb, b_+26);
  do { uint16_t jt_ = (interactionCodee5_jump_table(gb));
    if (jt_ == b_+30) { goto state0; }
    else if (jt_ == b_+74) { goto state1; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
state0:
  CALL_C(b_+30, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+33);
  CYC(b_+33, b_+35); A = 0x30;
  CALL_C(b_+35, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+38);
  CALL_C(b_+38, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+41);
  CALL_C(b_+41, interactionIncState_hook, SYM(interactionIncState), b_+44);
  CYC(b_+44, b_+46); A = 0x06;
  CALL_C(b_+46, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+49);
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+55); SET_HL((SYM(twinrova_state8) + 44));
  CYC(b_+55, b_+56); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+56, b_+58);
    goto setScript;
  }
  CYC(b_+56, b_+58);
  CYC(b_+58, b_+60); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); A = alu_inc8(gb, A);
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CYC(b_+63, b_+66); SET_HL((SYM(twinrova_state8) + 29));
setScript:
  CALL_C(b_+66, interactionSetScript_hook, SYM(interactionSetScript), b_+69);
  CYC(b_+69, b_+71); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+71, b_+74);
  objectAddToAButtonSensitiveObjectList_hook(gb);
  return;
state1:
  CYC(b_+74, SYM(templeIntro_simulatedInput));
  interactionRunScript_hook(gb);
}

void interactionCodee5_hook(GB *gb) {
  BASE(interactionCodee5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wOamEnd);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto doneTextFlagSetup;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); A = 0x02;
  CYC(b_+8, b_+11); W8(wTextboxPosition) = A;
  CYC(b_+11, b_+13); A = 0x08;
  CYC(b_+13, b_+16); W8(wTextboxFlags) = A;
doneTextFlagSetup:
  CYC(b_+16, b_+19); push_effect(gb, b_+19);
  interactionCodee5__runState_hook(gb);
  CYC(b_+19, b_+22);
  objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}
