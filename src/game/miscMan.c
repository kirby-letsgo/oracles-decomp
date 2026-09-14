#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t misc_man_jump_table(GB *gb) {
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

static void misc_man_init(GB *gb, uint16_t sp0_) {
  CALL_C(0x5285, interactionInitGraphics_hook, 0x15fb, 0x5288);
  CALL_C(0x5288, objectMarkSolidPosition_hook, 0x24f0, 0x528b);
  CYC(0x528b, 0x528e); interactionIncState_hook(gb);
}

static void misc_man_init_with_script(GB *gb, uint16_t sp0_) {
  CALL_C(0x528e, interactionInitGraphics_hook, 0x15fb, 0x5291);
  CALL_C(0x5291, objectMarkSolidPosition_hook, 0x24f0, 0x5294);
  CYC(0x5294, 0x5296); A = 0x26;
  CALL_C(0x5296, interactionSetHighTextIndex_hook, 0x253b, 0x5299);
  CYC(0x5299, 0x529b); E = 0x42;
  CYC(0x529b, 0x529c); A = mem_rd(gb, DE);
  CYC(0x529c, 0x529f); SET_HL(0x52a9);
  CYC(0x529f, 0x52a0); push_effect(gb, 0x52a0);
  CYC(0x0018, 0x0019); push_effect(gb, BC);
  CYC(0x0019, 0x001a); C = A;
  CYC(0x001a, 0x001c); B = 0;
  CYC(0x001c, 0x001d); alu_add_hl(gb, BC);
  CYC(0x001d, 0x001e); alu_add_hl(gb, BC);
  CYC(0x001e, 0x001f); SET_BC(pop_effect(gb));
  CYC(0x001f, 0x0020); ret_effect(gb);
  CYC(0x52a0, 0x52a1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x52a1, 0x52a2); H = mem_rd(gb, HL);
  CYC(0x52a2, 0x52a3); L = A;
  CALL_C(0x52a3, interactionSetScript_hook, 0x2544, 0x52a6);
  CYC(0x52a6, 0x52a9); interactionIncState_hook(gb);
}

void interactionCode41_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5223, 0x5225); E = 0x42;
  CYC(0x5225, 0x5226); A = mem_rd(gb, DE);
  CYC(0x5226, 0x5227); push_effect(gb, 0x5227);
  switch (misc_man_jump_table(gb)) {
    case 0x5235: goto subid0;
    case 0x5253: goto subid_nonzero;
    default: HANDOFF(HL);
  }
subid0:
  CALL_C(0x5235, checkInteractionState_hook, 0x23fe, 0x5238);
  if (!(F & FZ)) { CYCT(0x5238, 0x523a); goto initialized0; }
  CYC(0x5238, 0x523a);
  CYC(0x523a, 0x523c); A = 0x14;
  CALL_C(0x523c, checkGlobalFlag_hook, 0x31f3, 0x523f);
  if (!(F & FZ)) { CYCT(0x523f, 0x5242); interactionDelete_hook(gb); return; }
  CYC(0x523f, 0x5242);
  CYC(0x5242, 0x5244); A = 0x0b;
  CALL_C(0x5244, checkGlobalFlag_hook, 0x31f3, 0x5247);
  if (!(F & FZ)) { CYCT(0x5247, 0x524a); interactionDelete_hook(gb); return; }
  CYC(0x5247, 0x524a);
  CYC(0x524a, 0x524d); push_effect(gb, 0x524d); misc_man_init_with_script(gb, sp0_);
initialized0:
  CALL_C(0x524d, interactionRunScript_hook, 0x2552, 0x5250);
  CYC(0x5250, 0x5253); npcFaceLinkAndAnimate_hook(gb);
  return;
subid_nonzero:
  CALL_C(0x5253, checkInteractionState_hook, 0x23fe, 0x5256);
  if (!(F & FZ)) { CYCT(0x5256, 0x5258); goto initialized_nonzero; }
  CYC(0x5256, 0x5258);
  CYC(0x5258, 0x525a); A = 1;
  CYC(0x525a, 0x525c); E = 0x5c;
  CYC(0x525c, 0x525d); mem_wr(gb, DE, A);
  CYC(0x525d, 0x5260); SET_HL(0x552b);
  CYC(0x5260, 0x5262); E = 9;
  CALL_C(0x5262, interBankCall_hook, 0x008a, 0x5265);
  CYC(0x5265, 0x5267); E = 0x42;
  CYC(0x5267, 0x5268); A = mem_rd(gb, DE);
  CYC(0x5268, 0x5269); A = alu_dec8(gb, A);
  CYC(0x5269, 0x526a); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x526a, 0x526d); interactionDelete_hook(gb); return; }
  CYC(0x526a, 0x526d);
  CYC(0x526d, 0x5270); SET_HL(0x52ab);
  CYC(0x5270, 0x5271); push_effect(gb, 0x5271);
  CYC(0x0018, 0x0019); push_effect(gb, BC);
  CYC(0x0019, 0x001a); C = A;
  CYC(0x001a, 0x001c); B = 0;
  CYC(0x001c, 0x001d); alu_add_hl(gb, BC);
  CYC(0x001d, 0x001e); alu_add_hl(gb, BC);
  CYC(0x001e, 0x001f); SET_BC(pop_effect(gb));
  CYC(0x001f, 0x0020); ret_effect(gb);
  CYC(0x5271, 0x5272); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5272, 0x5273); H = mem_rd(gb, HL);
  CYC(0x5273, 0x5274); L = A;
  CALL_C(0x5274, interactionSetScript_hook, 0x2544, 0x5277);
  CYC(0x5277, 0x5279); A = 0x26;
  CALL_C(0x5279, interactionSetHighTextIndex_hook, 0x253b, 0x527c);
  CYC(0x527c, 0x527f); push_effect(gb, 0x527f); misc_man_init(gb, sp0_);
initialized_nonzero:
  CALL_C(0x527f, interactionRunScript_hook, 0x2552, 0x5282);
  CYC(0x5282, 0x5285); interactionAnimateAsNpc_hook(gb);
}
