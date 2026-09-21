#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode41);
  CALL_C(b_+98, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+101);
  CALL_C(b_+101, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+104);
  CYC(b_+104, b_+107); interactionIncState_hook(gb);
}

static void misc_man_init_with_script(GB *gb, uint16_t sp0_) {
  BASE(interactionCode41);
  CALL_C(b_+107, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+110);
  CALL_C(b_+110, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+113);
  CYC(b_+113, b_+115); A = 0x26;
  CALL_C(b_+115, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+118);
  CYC(b_+118, b_+120); E = 0x42;
  CYC(b_+120, b_+121); A = mem_rd(gb, DE);
  CYC(b_+121, b_+124); SET_HL(b_+134);
  CYC(b_+124, b_+125); push_effect(gb, b_+125);
  CYC(0x0018, 0x0019); push_effect(gb, BC);
  CYC(0x0019, 0x001a); C = A;
  CYC(0x001a, 0x001c); B = 0;
  CYC(0x001c, 0x001d); alu_add_hl(gb, BC);
  CYC(0x001d, 0x001e); alu_add_hl(gb, BC);
  CYC(0x001e, 0x001f); SET_BC(pop_effect(gb));
  CYC(0x001f, 0x0020); ret_effect(gb);
  CYC(b_+125, b_+126); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+126, b_+127); H = mem_rd(gb, HL);
  CYC(b_+127, b_+128); L = A;
  CALL_C(b_+128, interactionSetScript_hook, SYM(interactionSetScript), b_+131);
  CYC(b_+131, b_+134); interactionIncState_hook(gb);
}

void interactionCode41_hook(GB *gb) {
  BASE(interactionCode41);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (misc_man_jump_table(gb));
    if (jt_ == b_+18) { goto subid0; }
    else if (jt_ == b_+48) { goto subid_nonzero; }
    else { HANDOFF(HL); }
  } while (0);
subid0:
  CALL_C(b_+18, checkInteractionState_hook, SYM(checkInteractionState), b_+21);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto initialized0; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); A = 0x14;
  CALL_C(b_+25, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+28);
  if (!(F & FZ)) { CYCT(b_+28, b_+31); interactionDelete_hook(gb); return; }
  CYC(b_+28, b_+31);
  CYC(b_+31, b_+33); A = 0x0b;
  CALL_C(b_+33, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+36);
  if (!(F & FZ)) { CYCT(b_+36, b_+39); interactionDelete_hook(gb); return; }
  CYC(b_+36, b_+39);
  CYC(b_+39, b_+42); push_effect(gb, b_+42); misc_man_init_with_script(gb, sp0_);
initialized0:
  CALL_C(b_+42, interactionRunScript_hook, SYM(interactionRunScript), b_+45);
  CYC(b_+45, b_+48); npcFaceLinkAndAnimate_hook(gb);
  return;
subid_nonzero:
  CALL_C(b_+48, checkInteractionState_hook, SYM(checkInteractionState), b_+51);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto initialized_nonzero; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); A = 1;
  CYC(b_+55, b_+57); E = 0x5c;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+61); SET_HL(SYM(getGameProgress_1));
  CYC(b_+61, b_+63); E = 9;
  CALL_C(b_+63, interBankCall_hook, 0x008a, b_+66);
  CYC(b_+66, b_+68); E = 0x42;
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+70); A = alu_dec8(gb, A);
  CYC(b_+70, b_+71); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+71, b_+74); interactionDelete_hook(gb); return; }
  CYC(b_+71, b_+74);
  CYC(b_+74, b_+77); SET_HL(b_+136);
  CYC(b_+77, b_+78); push_effect(gb, b_+78);
  CYC(0x0018, 0x0019); push_effect(gb, BC);
  CYC(0x0019, 0x001a); C = A;
  CYC(0x001a, 0x001c); B = 0;
  CYC(0x001c, 0x001d); alu_add_hl(gb, BC);
  CYC(0x001d, 0x001e); alu_add_hl(gb, BC);
  CYC(0x001e, 0x001f); SET_BC(pop_effect(gb));
  CYC(0x001f, 0x0020); ret_effect(gb);
  CYC(b_+78, b_+79); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+79, b_+80); H = mem_rd(gb, HL);
  CYC(b_+80, b_+81); L = A;
  CALL_C(b_+81, interactionSetScript_hook, SYM(interactionSetScript), b_+84);
  CYC(b_+84, b_+86); A = 0x26;
  CALL_C(b_+86, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+89);
  CYC(b_+89, b_+92); push_effect(gb, b_+92); misc_man_init(gb, sp0_);
initialized_nonzero:
  CALL_C(b_+92, interactionRunScript_hook, SYM(interactionRunScript), b_+95);
  CYC(b_+95, b_+98); TAIL(interactionAnimateAsNpc);
}
