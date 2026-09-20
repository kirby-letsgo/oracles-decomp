#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x08, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x08, (from), (to), true)

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
  CYC(0x52da, 0x52dc); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x52dc, 0x52dd); A = mem_rd(gb, DE);
  CYC(0x52dd, 0x52df); A = alu_swap(gb, A);
  CYC(0x52df, 0x52e1); alu_and(gb, 0x07);
  CYC(0x52e1, 0x52e4); SET_HL(bitTable);
  CYC(0x52e4, 0x52e5); alu_add(gb, L);
  CYC(0x52e5, 0x52e6); L = A;
  CYC(0x52e6, 0x52e7); A = C;
  CYC(0x52e7, 0x52e8); alu_and(gb, mem_rd(gb, HL));
  CYC(0x52e8, 0x52e9); B = A;
  CYC(0x52e9, 0x52ea); A = mem_rd(gb, HL);
  CYC(0x52ea, 0x52eb); alu_cpl(gb);
  CYC(0x52eb, 0x52ec); C = A;
  CYC(0x52ec, 0x52ef); A = mem_rd(gb, wActiveTriggers);
  CYC(0x52ef, 0x52f0); alu_and(gb, C);
  CYC(0x52f0, 0x52f1); alu_or(gb, B);
  CYC(0x52f1, 0x52f4); mem_wr(gb, wActiveTriggers, A);
  CYC(0x52f4, 0x52f5); ret_effect(gb);
}

// Subid 0: control a bit in wActiveTriggers based on wToggleBlocksState.
static void triggerTranslator_subid0(GB *gb) {
  CYC(0x52d6, 0x52d9); A = mem_rd(gb, wToggleBlocksState);
  CYC(0x52d9, 0x52da); C = A;
  triggerTranslator_setTriggerBitFromC(gb);
}

// Subid 1: control a bit in wActiveTriggers based on wSwitchState.
static void triggerTranslator_subid1(GB *gb) {
  CYC(0x52f5, 0x52f8); A = mem_rd(gb, wSwitchState);
  CYC(0x52f8, 0x52f9); C = A;
  CYC(0x52f9, 0x52fb);
  triggerTranslator_setTriggerBitFromC(gb);
}

// Subid 2: set the trigger bits in xh if [wNumTorchesLit] == yh, clear them otherwise.
static void triggerTranslator_subid2(GB *gb) {
  CYC(0x52fb, 0x52fd); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x52fd, 0x52fe); A = mem_rd(gb, DE);
  CYC(0x52fe, 0x52ff); B = A;
  CYC(0x52ff, 0x5301); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5301, 0x5302); A = mem_rd(gb, DE);
  CYC(0x5302, 0x5303); C = A;
  CYC(0x5303, 0x5306); A = mem_rd(gb, wNumTorchesLit);
  CYC(0x5306, 0x5307); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x5307, 0x5309); goto clearBits;
  }
  CYC(0x5307, 0x5309);
  CYC(0x5309, 0x530c); A = mem_rd(gb, wActiveTriggers);
  CYC(0x530c, 0x530d); alu_or(gb, C);
  CYC(0x530d, 0x5310); mem_wr(gb, wActiveTriggers, A);
  CYC(0x5310, 0x5311); ret_effect(gb);
  return;

clearBits:
  CYC(0x5311, 0x5312); A = C;
  CYC(0x5312, 0x5313); alu_cpl(gb);
  CYC(0x5313, 0x5314); C = A;
  CYC(0x5314, 0x5317); A = mem_rd(gb, wActiveTriggers);
  CYC(0x5317, 0x5318); alu_and(gb, C);
  CYC(0x5318, 0x531b); mem_wr(gb, wActiveTriggers, A);
  CYC(0x531b, 0x531c); ret_effect(gb);
}

// INTERAC_TRIGGER_TRANSLATOR: mirrors some game state (toggle blocks, switches, lit
// torches) into bits of wActiveTriggers.
void interactionCode24_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x52c7, interactionDeleteAndRetIfEnabled02_hook, 0x26ec, 0x52ca);
  CYC(0x52ca, 0x52cc); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x52cc, 0x52cd); A = mem_rd(gb, DE);
  CYC(0x52cd, 0x52cf); alu_and(gb, 0x0f);
  CYC(0x52cf, 0x52d0); push_effect(gb, 0x52d0);
  switch (triggerTranslator_jumpTable(gb)) {
    case 0x52d6: triggerTranslator_subid0(gb); return;
    case 0x52f5: triggerTranslator_subid1(gb); return;
    case 0x52fb: triggerTranslator_subid2(gb); return;
    default: HANDOFF(HL);
  }
}
