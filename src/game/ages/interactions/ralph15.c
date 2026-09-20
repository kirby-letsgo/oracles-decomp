#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_RALPH), bank 0x15.

void ralph_createExclamationMarkShiftedRight_hook(GB *gb);

static uint16_t ralph15_jump_table(GB *gb) {
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

void ralph_createLinkedSwordAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x563a, getFreeInteractionSlot_hook, 0x3aef, 0x563d);
  if (!(F & FZ)) { RET_TAKEN(0x563d); return; }
  CYC(0x563d, 0x563e);
  CYC(0x563e, 0x5640); mem_wr(gb, HL, 0x5e); // INTERAC_SWORD
  CYC(0x5640, 0x5642); L = INTERACTION_BASE + OBJ_RELATED1 + 1;
  CYC(0x5642, 0x5643); A = D;
  CYC(0x5643, 0x5644); mem_wr(gb, HL, A);
  CYC(0x5644, 0x5647); objectCopyPosition_hook(gb); return; // jp
}

void ralph_faceLinkAndCreateExclamationMark_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5647, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x564a);
  CYC(0x564a, 0x564c); alu_add(gb, 0x04);
  CYC(0x564c, 0x564e); alu_and(gb, 0x18);
  CYC(0x564e, 0x5650); A = alu_swap(gb, A);
  CYC(0x5650, 0x5651); alu_rlca(gb);
  CALL_C(0x5651, interactionSetAnimation_hook, 0x262e, 0x5654);
  CYC(0x5654, 0x5656); A = 0x1e;
  ralph_createExclamationMarkShiftedRight_hook(gb); return; // fallthrough
}

void ralph_createExclamationMarkShiftedRight_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5656, 0x5659); SET_BC(0xf30d);
  CYC(0x5659, 0x565c); objectCreateExclamationMark_hook(gb); return; // jp
}

void ralph_beginHighJump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x565c, 0x565d); H = D;
  CYC(0x565d, 0x565f); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(0x565f, 0x5661); mem_wr(gb, HL, 0x00);
  CYC(0x5661, 0x5662); SET_HL(HL + 1);
  CYC(0x5662, 0x5664); mem_wr(gb, HL, 0xfc);
  CYC(0x5664, 0x5666); A = 0x53; // SND_JUMP
  CYC(0x5666, 0x5669); playSound_b00_hook(gb); return; // jp
}

void ralph_updateGravity_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5669, 0x566b); C = 0xc0;
  CALL_C(0x566b, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x566e);
  CYC(0x566e, 0x5671); writeFlagsTocddb_hook(gb); return; // jp
}

void ralph_restoreMusic_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5671, 0x5673); A = 0x03; // MUS_OVERWORLD
  CYC(0x5673, 0x5676); mem_wr(gb, wActiveMusic2, A);
  CYC(0x5676, 0x5679); mem_wr(gb, wActiveMusic, A);
  CYC(0x5679, 0x567c); playSound_b00_hook(gb); return; // jp
}

// ralph_flashScreen@func (15:5682) plus its @thing0-@thing4/@inccfde sub-labels: only reached via
// the single `call` at 15:567c. The caller pushes the real return address (15:567f) before
// invoking this as a plain C function call; every internal path here ends in a real `ret`
// (modeled with ret_effect, which pops that exact value).
static void ralph_flashScreen_func(GB *gb, uint16_t sp0_) {
  CYC(0x5682, 0x5685); A = W8(wTmpcfc0_genericCutscene_cfde);
  CYC(0x5685, 0x5686); push_effect(gb, 0x5686);
  uint16_t target = ralph15_jump_table(gb);
  if (target == 0x569d) goto thing1;
  if (target == 0x56ad) goto thing3;
  if (target == 0x56b8) goto thing4;
  // target == 0x5690 falls through to @thing0

  CYC(0x5690, 0x5692); A = 0x0a;
  CYC(0x5692, 0x5695); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CALL_C(0x5695, clearFadingPalettes_hook, 0x2d5f, 0x5698);

inccfde: // ralph_flashScreen@inccfde
  CYC(0x5698, 0x569b); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(0x569b, 0x569c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(0x569c); return;

thing1: // ralph_flashScreen@thing1 / @thing2 (aliased)
  CYC(0x569d, 0x56a0); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(0x56a0, 0x56a1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x56a1); return; }
  CYC(0x56a1, 0x56a2);
  CYC(0x56a2, 0x56a4); A = 0x0a;
  CYC(0x56a4, 0x56a7); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CALL_C(0x56a7, fastFadeoutToWhite_hook, 0x3263, 0x56aa);
  CYC(0x56aa, 0x56ad); goto inccfde; // jp

thing3: // ralph_flashScreen@thing3
  CYC(0x56ad, 0x56af); A = 0x14;
  CYC(0x56af, 0x56b2); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CALL_C(0x56b2, clearFadingPalettes_hook, 0x2d5f, 0x56b5);
  CYC(0x56b5, 0x56b8); goto inccfde; // jp

thing4: // ralph_flashScreen@thing4
  CYC(0x56b8, 0x56bb); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(0x56bb, 0x56bc); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x56bc); return;
}

void ralph_flashScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x567c, 0x567f); push_effect(gb, 0x567f); ralph_flashScreen_func(gb, sp0_); // call
  CYC(0x567f, 0x5682); writeFlagsTocddb_hook(gb); return; // jp
}

void ralph_flickerVisibility_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x56bd, 0x56bf); B = 0x01;
  CYC(0x56bf, 0x56c2); objectFlickerVisibility_hook(gb); return; // jp
}

void ralph_decVar3f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x56c2, 0x56c3); H = D;
  CYC(0x56c3, 0x56c5); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x56c5, 0x56c6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x56c6, 0x56c9); writeFlagsTocddb_hook(gb); return; // jp
}
