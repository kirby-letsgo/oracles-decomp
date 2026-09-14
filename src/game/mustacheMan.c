#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

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
  CALL_C(0x52f1, interactionInitGraphics_hook, 0x15fb, 0x52f4);
  CALL_C(0x52f4, objectMarkSolidPosition_hook, 0x24f0, 0x52f7);
  CYC(0x52f7, 0x52f9); A = 0x0f;
  CALL_C(0x52f9, interactionSetHighTextIndex_hook, 0x253b, 0x52fc);
  CYC(0x52fc, 0x52fe); E = 0x42;
  CYC(0x52fe, 0x52ff); A = mem_rd(gb, DE);
  CYC(0x52ff, 0x5302); SET_HL(0x530c);
  CYC(0x5302, 0x5303); push_effect(gb, 0x5303);
  CYC(0x0018, 0x0019); push_effect(gb, BC);
  CYC(0x0019, 0x001a); C = A;
  CYC(0x001a, 0x001c); B = 0;
  CYC(0x001c, 0x001d); alu_add_hl(gb, BC);
  CYC(0x001d, 0x001e); alu_add_hl(gb, BC);
  CYC(0x001e, 0x001f); SET_BC(pop_effect(gb));
  CYC(0x001f, 0x0020); ret_effect(gb);
  CYC(0x5303, 0x5304); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5304, 0x5305); H = mem_rd(gb, HL);
  CYC(0x5305, 0x5306); L = A;
  CALL_C(0x5306, interactionSetScript_hook, 0x2544, 0x5309);
  CYC(0x5309, 0x530c); interactionIncState_hook(gb);
}

void interactionCode42_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x52b7, 0x52b9); E = 0x42;
  CYC(0x52b9, 0x52ba); A = mem_rd(gb, DE);
  CYC(0x52ba, 0x52bb); push_effect(gb, 0x52bb);
  switch (mustache_man_jump_table(gb)) {
    case 0x52bf: goto subid0;
    case 0x52d5: goto subid1;
    default: HANDOFF(HL);
  }
subid0:
  CALL_C(0x52bf, checkInteractionState_hook, 0x23fe, 0x52c2);
  if (!(F & FZ)) { CYCT(0x52c2, 0x52c4); goto run0; }
  CYC(0x52c2, 0x52c4);
  CYC(0x52c4, 0x52c6); A = 0x14;
  CALL_C(0x52c6, checkGlobalFlag_hook, 0x31f3, 0x52c9);
  if (!(F & FZ)) { CYCT(0x52c9, 0x52cc); interactionDelete_hook(gb); return; }
  CYC(0x52c9, 0x52cc);
  CYC(0x52cc, 0x52cf); push_effect(gb, 0x52cf); mustache_man_init(gb, sp0_);
run0:
  CALL_C(0x52cf, interactionRunScript_hook, 0x2552, 0x52d2);
  CYC(0x52d2, 0x52d5); interactionAnimateAsNpc_hook(gb);
  return;
subid1:
  CALL_C(0x52d5, checkInteractionState_hook, 0x23fe, 0x52d8);
  if (!(F & FZ)) { CYCT(0x52d8, 0x52da); goto run1; }
  CYC(0x52d8, 0x52da);
  CYC(0x52da, 0x52dc); E = 0x72;
  CYC(0x52dc, 0x52de); A = 2;
  CYC(0x52de, 0x52df); mem_wr(gb, DE, A);
  CYC(0x52df, 0x52e2); push_effect(gb, 0x52e2); mustache_man_init(gb, sp0_);
run1:
  CALL_C(0x52e2, interactionRunScript_hook, 0x2552, 0x52e5);
  CYC(0x52e5, 0x52e8); interactionAnimateAsNpc_hook(gb);
}
