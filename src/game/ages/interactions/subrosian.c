#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t subrosian_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void subrosian_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void subrosian_subid00_hook(GB *gb);
void subrosian_subid01_hook(GB *gb);
void subrosian_subid02_hook(GB *gb);
void subrosian_subid03_hook(GB *gb);
void subrosian_subid04_hook(GB *gb);
void subrosian_initGraphicsAndIncState_hook(GB *gb);
void subrosian_unused_63ec_hook(GB *gb);
void subrosian_initSubid02_hook(GB *gb);
void subrosian_loadScript_hook(GB *gb);
void subrosian_loadScriptIndex_hook(GB *gb);
void subrosian_getScriptPtr_hook(GB *gb);

void interactionCode4e_hook(GB *gb) {
  BASE(interactionCode4e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosian_jump_table(gb));
    if (jt_ == SYM(subrosian_subid00) && hook_is(gb, SYM(subrosian_subid00), subrosian_subid00_hook)) { subrosian_subid00_hook(gb); return; }
    else if (jt_ == SYM(subrosian_subid01) && hook_is(gb, SYM(subrosian_subid01), subrosian_subid01_hook)) { subrosian_subid01_hook(gb); return; }
    else if (jt_ == SYM(subrosian_subid02) && hook_is(gb, SYM(subrosian_subid02), subrosian_subid02_hook)) { subrosian_subid02_hook(gb); return; }
    else if (jt_ == SYM(subrosian_subid03) && hook_is(gb, SYM(subrosian_subid03), subrosian_subid03_hook)) { subrosian_subid03_hook(gb); return; }
    else if (jt_ == SYM(subrosian_subid04) && hook_is(gb, SYM(subrosian_subid04), subrosian_subid04_hook)) { subrosian_subid04_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void subrosian_subid00_hook(GB *gb) {
  BASE(subrosian_subid00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, interactionIncState_hook, SYM(interactionIncState), b_+8);
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CALL_C(b_+11, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+14);
  CYC(b_+14, b_+16); A = 0x1c;
  CALL_C(b_+16, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+19);
  CALL_C(b_+19, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+22);
  if (F & FZ) { CYCT(b_+22, b_+25); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; }
  CYC(b_+22, b_+25);
  CYC(b_+25, b_+28); SET_HL(SYM(getGameProgress_2));
  CYC(b_+28, b_+30); E = 0x09;
  CALL_C(b_+30, interBankCall_hook, 0x008a, b_+33);
  CYC(b_+33, b_+34); A = B;
  CYC(b_+34, b_+36); alu_cp(gb, 5);
  CYC(b_+36, b_+39); SET_HL((SYM(impaNpc_determineTextAndPositionInHouse__val02) + 3));
  if (F & FZ) { CYCT(b_+39, b_+41); goto set_script; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+43); alu_cp(gb, 7);
  if (!(F & FZ)) { CYCT(b_+43, b_+46); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; }
  CYC(b_+43, b_+46);
  CYC(b_+46, b_+49); SET_HL((SYM(impaNpc_determineTextAndPositionInHouse__val02) + 5));
set_script:
  CALL_C(b_+49, interactionSetScript_hook, SYM(interactionSetScript), b_+52);
state1:
  CALL_C(b_+52, interactionRunScript_hook, SYM(interactionRunScript), b_+55);
  CYC(b_+55, b_+58); TAIL(npcFaceLinkAndAnimate);
}

void subrosian_subid01_hook(GB *gb) {
  BASE(subrosian_subid01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(goronSubid01));
  CYC(b_+3, b_+5); E = 9;
  CYC(b_+5, b_+8); TAIL(interBankCall);
}

void subrosian_subid02_hook(GB *gb) {
  BASE(subrosian_subid02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, subrosian_initSubid02_hook, SYM(subrosian_initSubid02), b_+8);
  CALL_C(b_+8, interactionRunScript_hook, SYM(interactionRunScript), b_+11);
state1:
  CALL_C(b_+11, interactionRunScript_hook, SYM(interactionRunScript), b_+14);
  if (F & FC) { CYCT(b_+14, b_+17); interactionDelete_hook(gb); return; }
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+20); TAIL(npcFaceLinkAndAnimate);
}

void subrosian_subid03_hook(GB *gb) {
  BASE(subrosian_subid04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(SYM(subrosian_subid03), checkInteractionState_hook, SYM(checkInteractionState), (SYM(subrosian_subid03) + 3));
  if (!(F & FZ)) { CYCT((SYM(subrosian_subid03) + 3), (SYM(subrosian_subid03) + 5)); goto state1; }
  CYC((SYM(subrosian_subid03) + 3), (SYM(subrosian_subid03) + 5));
  CALL_C(SYM(subrosian_subid03__state0), subrosian_initGraphicsAndIncState_hook, SYM(subrosian_initGraphicsAndIncState), (SYM(subrosian_subid03__state0) + 3));
  CYC((SYM(subrosian_subid03__state0) + 3), (SYM(subrosian_subid03__state0) + 5)); A = 2;
  CYC((SYM(subrosian_subid03__state0) + 5), (SYM(subrosian_subid03__state0) + 7));
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+16); SET_HL((SYM(goronDanceScriptTable) + 241));
  CALL_C(b_+16, interactionSetScript_hook, SYM(interactionSetScript), b_+19);
  CALL_C(b_+19, interactionRunScript_hook, SYM(interactionRunScript), b_+22);
state1:
  CALL_C(b_+22, interactionRunScript_hook, SYM(interactionRunScript), b_+25);
  if (F & FC) { CYCT(b_+25, b_+28); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; }
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+31); TAIL(npcFaceLinkAndAnimate);
}

void subrosian_subid04_hook(GB *gb) {
  BASE(subrosian_subid04);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, subrosian_initGraphicsAndIncState_hook, SYM(subrosian_initGraphicsAndIncState), b_+8);
  CYC(b_+8, b_+10); A = 4;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+16); SET_HL((SYM(goronDanceScriptTable) + 241));
  CALL_C(b_+16, interactionSetScript_hook, SYM(interactionSetScript), b_+19);
  CALL_C(b_+19, interactionRunScript_hook, SYM(interactionRunScript), b_+22);
state1:
  CALL_C(b_+22, interactionRunScript_hook, SYM(interactionRunScript), b_+25);
  if (F & FC) { CYCT(b_+25, b_+28); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; }
  CYC(b_+25, b_+28);
  CYC(b_+28, b_+31); TAIL(npcFaceLinkAndAnimate);
}

void subrosian_initGraphicsAndIncState_hook(GB *gb) {
  BASE(subrosian_initGraphicsAndIncState);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+6);
  CYC(b_+6, b_+9); TAIL(interactionIncState);
}

void subrosian_unused_63ec_hook(GB *gb) {
  BASE(subrosian_unused_63ec);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+6);
  CYC(b_+6, b_+8); TAIL(subrosian_loadScript);
}

void subrosian_initSubid02_hook(GB *gb) {
  BASE(subrosian_initSubid02);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+6);
  CYC(b_+6, b_+8); TAIL(subrosian_loadScriptIndex);
}

void subrosian_loadScript_hook(GB *gb) {
  BASE(subrosian_loadScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, subrosian_getScriptPtr_hook, SYM(subrosian_getScriptPtr), b_+3);
  CALL_C(b_+3, interactionSetScript_hook, SYM(interactionSetScript), b_+6);
  CYC(b_+6, b_+9); TAIL(interactionIncState);
}

void subrosian_loadScriptIndex_hook(GB *gb) {
  BASE(subrosian_loadScriptIndex);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, subrosian_getScriptPtr_hook, SYM(subrosian_getScriptPtr), b_+3);
  CYC(b_+3, b_+4); E = alu_inc8(gb, E);
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); subrosian_add_double_index(gb, b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+8); H = mem_rd(gb, HL);
  CYC(b_+8, b_+9); L = A;
  CALL_C(b_+9, interactionSetScript_hook, SYM(interactionSetScript), b_+12);
  CYC(b_+12, b_+15); TAIL(interactionIncState);
}

void subrosian_getScriptPtr_hook(GB *gb) {
  BASE(subrosian_getScriptPtr);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x1c;
  CALL_C(b_+2, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+5);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+11); SET_HL(b_+16);
  CYC(b_+11, b_+12); subrosian_add_double_index(gb, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); H = mem_rd(gb, HL);
  CYC(b_+14, b_+15); L = A;
  CYC(b_+15, b_+16); ret_effect(gb);
}
