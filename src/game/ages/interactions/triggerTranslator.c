#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t triggerTranslator_jumpTable(GB *gb) {
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

// @label_08_081: C holds a source bitmask; copy bit (subid >> 4) of it into the same bit of
// wActiveTriggers.
static void triggerTranslator_setTriggerBitFromC(GB *gb) {
  BASE(interactionCode24);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); A = alu_swap(gb, A);
  CYC(b_+24, b_+26); alu_and(gb, 0x07);
  CYC(b_+26, b_+29); SET_HL(bitTable);
  CYC(b_+29, b_+30); alu_add(gb, L);
  CYC(b_+30, b_+31); L = A;
  CYC(b_+31, b_+32); A = C;
  CYC(b_+32, b_+33); alu_and(gb, mem_rd(gb, HL));
  CYC(b_+33, b_+34); B = A;
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); alu_cpl(gb);
  CYC(b_+36, b_+37); C = A;
  CYC(b_+37, b_+40); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+40, b_+41); alu_and(gb, C);
  CYC(b_+41, b_+42); alu_or(gb, B);
  CYC(b_+42, b_+45); mem_wr(gb, wActiveTriggers, A);
  CYC(b_+45, b_+46); ret_effect(gb);
}

// Subid 0: control a bit in wActiveTriggers based on wToggleBlocksState.
static void triggerTranslator_subid0(GB *gb) {
  BASE(interactionCode24);
  CYC(b_+15, b_+18); A = mem_rd(gb, wToggleBlocksState);
  CYC(b_+18, b_+19); C = A;
  triggerTranslator_setTriggerBitFromC(gb);
}

// Subid 1: control a bit in wActiveTriggers based on wSwitchState.
static void triggerTranslator_subid1(GB *gb) {
  BASE(interactionCode24);
  CYC(b_+46, b_+49); A = mem_rd(gb, wSwitchState);
  CYC(b_+49, b_+50); C = A;
  CYC(b_+50, b_+52);
  triggerTranslator_setTriggerBitFromC(gb);
}

// Subid 2: set the trigger bits in xh if [wNumTorchesLit] == yh, clear them otherwise.
static void triggerTranslator_subid2(GB *gb) {
  BASE(interactionCode24);
  CYC(b_+52, b_+54); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+56); B = A;
  CYC(b_+56, b_+58); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); C = A;
  CYC(b_+60, b_+63); A = mem_rd(gb, wNumTorchesLit);
  CYC(b_+63, b_+64); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+64, b_+66); goto clearBits;
  }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+69); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+69, b_+70); alu_or(gb, C);
  CYC(b_+70, b_+73); mem_wr(gb, wActiveTriggers, A);
  CYC(b_+73, b_+74); ret_effect(gb);
  return;

clearBits:
  CYC(b_+74, b_+75); A = C;
  CYC(b_+75, b_+76); alu_cpl(gb);
  CYC(b_+76, b_+77); C = A;
  CYC(b_+77, b_+80); A = mem_rd(gb, wActiveTriggers);
  CYC(b_+80, b_+81); alu_and(gb, C);
  CYC(b_+81, b_+84); mem_wr(gb, wActiveTriggers, A);
  CYC(b_+84, b_+85); ret_effect(gb);
}

// INTERAC_TRIGGER_TRANSLATOR: mirrors some game state (toggle blocks, switches, lit
// torches) into bits of wActiveTriggers.
void interactionCode24_hook(GB *gb) {
  BASE(interactionCode24);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDeleteAndRetIfEnabled02_hook, SYM(interactionDeleteAndRetIfEnabled02), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_and(gb, 0x0f);
  CYC(b_+8, b_+9); push_effect(gb, b_+9);
  do { uint16_t jt_ = (triggerTranslator_jumpTable(gb));
    if (jt_ == b_+15) { triggerTranslator_subid0(gb); return; }
    else if (jt_ == b_+46) { triggerTranslator_subid1(gb); return; }
    else if (jt_ == b_+52) { triggerTranslator_subid2(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}
