#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t past_old_lady_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void past_old_lady_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void past_old_lady_init_graphics_and_inc_state(GB *gb, uint16_t sp0_) {
  BASE(interactionCode45);
  CALL_C(b_+73, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+76);
  CALL_C(b_+76, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+79);
  CYC(b_+79, b_+82); interactionIncState_hook(gb);
}

static void past_old_lady_init_graphics_text_and_script(GB *gb, uint16_t sp0_) {
  BASE(interactionCode45);
  CALL_C(b_+82, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+85);
  CALL_C(b_+85, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+88);
  CYC(b_+88, b_+90); A = 0x18;
  CALL_C(b_+90, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+93);
  CYC(b_+93, b_+95); E = 0x42;
  CYC(b_+95, b_+96); A = mem_rd(gb, DE);
  CYC(b_+96, b_+99); SET_HL(b_+109);
  CYC(b_+99, b_+100); past_old_lady_add_double_index(gb, b_+100);
  CYC(b_+100, b_+101); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+101, b_+102); H = mem_rd(gb, HL);
  CYC(b_+102, b_+103); L = A;
  CALL_C(b_+103, interactionSetScript_hook, SYM(interactionSetScript), b_+106);
  CYC(b_+106, b_+109); interactionIncState_hook(gb);
}

void interactionCode45_hook(GB *gb) {
  BASE(interactionCode45);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (past_old_lady_jump_table(gb));
    if (jt_ == b_+8) { goto subid0; }
    else if (jt_ == b_+30) { goto subid1; }
    else { HANDOFF(HL); }
  } while (0);

subid0:
  CALL_C(b_+8, checkInteractionState_hook, SYM(checkInteractionState), b_+11);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto run0; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); A = 0x14;
  CALL_C(b_+15, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+18);
  if (!(F & FZ)) { CYCT(b_+18, b_+21); interactionDelete_hook(gb); return; }
  CYC(b_+18, b_+21);
  CYC(b_+21, b_+24); push_effect(gb, b_+24);
  past_old_lady_init_graphics_text_and_script(gb, sp0_);

run0:
  CALL_C(b_+24, interactionRunScript_hook, SYM(interactionRunScript), b_+27);
  CYC(b_+27, b_+30); interactionAnimateAsNpc_hook(gb);
  return;

subid1:
  CALL_C(b_+30, checkInteractionState_hook, SYM(checkInteractionState), b_+33);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto run1; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+38); SET_HL(SYM(getGameProgress_2));
  CYC(b_+38, b_+40); E = 0x09;
  CALL_C(b_+40, interBankCall_hook, 0x008a, b_+43);
  CYC(b_+43, b_+44); A = B;
  CYC(b_+44, b_+46); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(b_+46, b_+49); interactionDelete_hook(gb); return; }
  CYC(b_+46, b_+49);
  CYC(b_+49, b_+52); SET_HL(b_+113);
  CYC(b_+52, b_+53); past_old_lady_add_double_index(gb, b_+53);
  CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+54, b_+55); H = mem_rd(gb, HL);
  CYC(b_+55, b_+56); L = A;
  CALL_C(b_+56, interactionSetScript_hook, SYM(interactionSetScript), b_+59);
  CYC(b_+59, b_+61); A = 0x18;
  CALL_C(b_+61, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+64);
  CYC(b_+64, b_+67); push_effect(gb, b_+67);
  past_old_lady_init_graphics_and_inc_state(gb, sp0_);

run1:
  CALL_C(b_+67, interactionRunScript_hook, SYM(interactionRunScript), b_+70);
  CYC(b_+70, b_+73); interactionAnimateAsNpc_hook(gb);
}
