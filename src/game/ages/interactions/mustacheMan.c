#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t mustache_man_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void mustache_man_init(GB *gb, uint16_t sp0_) {
  BASE(interactionCode42);
  CALL_C(b_+58, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+61);
  CALL_C(b_+61, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+64);
  CYC(b_+64, b_+66); A = 0x0f;
  CALL_C(b_+66, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+69);
  CYC(b_+69, b_+71); E = 0x42;
  CYC(b_+71, b_+72); A = mem_rd(gb, DE);
  CYC(b_+72, b_+75); SET_HL(b_+85);
  CYC(b_+75, b_+76); push_effect(gb, b_+76);
  CYC(0x0018, 0x0019); push_effect(gb, BC);
  CYC(0x0019, 0x001a); C = A;
  CYC(0x001a, 0x001c); B = 0;
  CYC(0x001c, 0x001d); alu_add_hl(gb, BC);
  CYC(0x001d, 0x001e); alu_add_hl(gb, BC);
  CYC(0x001e, 0x001f); SET_BC(pop_effect(gb));
  CYC(0x001f, 0x0020); ret_effect(gb);
  CYC(b_+76, b_+77); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+77, b_+78); H = mem_rd(gb, HL);
  CYC(b_+78, b_+79); L = A;
  CALL_C(b_+79, interactionSetScript_hook, SYM(interactionSetScript), b_+82);
  CYC(b_+82, b_+85); interactionIncState_hook(gb);
}

void interactionCode42_hook(GB *gb) {
  BASE(interactionCode42);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (mustache_man_jump_table(gb));
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
  CYC(b_+21, b_+24); push_effect(gb, b_+24); mustache_man_init(gb, sp0_);
run0:
  CALL_C(b_+24, interactionRunScript_hook, SYM(interactionRunScript), b_+27);
  CYC(b_+27, b_+30); interactionAnimateAsNpc_hook(gb);
  return;
subid1:
  CALL_C(b_+30, checkInteractionState_hook, SYM(checkInteractionState), b_+33);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto run1; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); E = 0x72;
  CYC(b_+37, b_+39); A = 2;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+43); push_effect(gb, b_+43); mustache_man_init(gb, sp0_);
run1:
  CALL_C(b_+43, interactionRunScript_hook, SYM(interactionRunScript), b_+46);
  CYC(b_+46, b_+49); interactionAnimateAsNpc_hook(gb);
}
