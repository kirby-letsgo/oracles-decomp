#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(ralph_createLinkedSwordAnimation);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x5e); // INTERAC_SWORD
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_RELATED1 + 1;
  CYC(b_+8, b_+9); A = D;
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+13); TAIL(objectCopyPosition); // jp
}

void ralph_faceLinkAndCreateExclamationMark_hook(GB *gb) {
  BASE(ralph_faceLinkAndCreateExclamationMark);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, objectGetAngleTowardEnemyTarget_hook, SYM(objectGetAngleTowardEnemyTarget), b_+3);
  CYC(b_+3, b_+5); alu_add(gb, 0x04);
  CYC(b_+5, b_+7); alu_and(gb, 0x18);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+10); alu_rlca(gb);
  CALL_C(b_+10, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+13);
  CYC(b_+13, b_+15); A = 0x1e;
  TAIL(ralph_createExclamationMarkShiftedRight); // fallthrough
}

void ralph_createExclamationMarkShiftedRight_hook(GB *gb) {
  BASE(ralph_createExclamationMarkShiftedRight);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0xf30d);
  CYC(b_+3, b_+6); TAIL(objectCreateExclamationMark); // jp
}

void ralph_beginHighJump_hook(GB *gb) {
  BASE(ralph_beginHighJump);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x00);
  CYC(b_+5, b_+6); SET_HL(HL + 1);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0xfc);
  CYC(b_+8, b_+10); A = 0x53; // SND_JUMP
  CYC(b_+10, b_+13); TAIL(playSound_b00); // jp
}

void ralph_updateGravity_hook(GB *gb) {
  BASE(ralph_updateGravity);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); C = 0xc0;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CYC(b_+5, b_+8); TAIL(writeFlagsTocddb); // jp
}

void ralph_restoreMusic_hook(GB *gb) {
  BASE(ralph_restoreMusic);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); A = 0x03; // MUS_OVERWORLD
  CYC(b_+2, b_+5); mem_wr(gb, wActiveMusic2, A);
  CYC(b_+5, b_+8); mem_wr(gb, wActiveMusic, A);
  CYC(b_+8, b_+11); TAIL(playSound_b00); // jp
}

// ralph_flashScreen@func (15:5682) plus its @thing0-@thing4/@inccfde sub-labels: only reached via
// the single `call` at 15:567c. The caller pushes the real return address (15:567f) before
// invoking this as a plain C function call; every internal path here ends in a real `ret`
// (modeled with ret_effect, which pops that exact value).
static void ralph_flashScreen_func(GB *gb, uint16_t sp0_) {
  BASE(ralph_flashScreen);
  CYC(b_+6, b_+9); A = W8(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  uint16_t target = ralph15_jump_table(gb);
  if (target == b_+33) goto thing1;
  if (target == b_+49) goto thing3;
  if (target == b_+60) goto thing4;
  // target == 0x5690 falls through to @thing0

  CYC(b_+20, b_+22); A = 0x0a;
  CYC(b_+22, b_+25); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CALL_C(b_+25, clearFadingPalettes_hook, SYM(clearFadingPalettes), b_+28);

inccfde: // ralph_flashScreen@inccfde
  CYC(b_+28, b_+31); SET_HL(wTmpcfc0_genericCutscene_cfde);
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+32); return;

thing1: // ralph_flashScreen@thing1 / @thing2 (aliased)
  CYC(b_+33, b_+36); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+37); return; }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); A = 0x0a;
  CYC(b_+40, b_+43); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CALL_C(b_+43, fastFadeoutToWhite_hook, SYM(fastFadeoutToWhite), b_+46);
  CYC(b_+46, b_+49); goto inccfde; // jp

thing3: // ralph_flashScreen@thing3
  CYC(b_+49, b_+51); A = 0x14;
  CYC(b_+51, b_+54); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CALL_C(b_+54, clearFadingPalettes_hook, SYM(clearFadingPalettes), b_+57);
  CYC(b_+57, b_+60); goto inccfde; // jp

thing4: // ralph_flashScreen@thing4
  CYC(b_+60, b_+63); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+64); return;
}

void ralph_flashScreen_hook(GB *gb) {
  BASE(ralph_flashScreen);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); push_effect(gb, b_+3); ralph_flashScreen_func(gb, sp0_); // call
  CYC(b_+3, b_+6); TAIL(writeFlagsTocddb); // jp
}

void ralph_flickerVisibility_hook(GB *gb) {
  BASE(ralph_flickerVisibility);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); B = 0x01;
  CYC(b_+2, b_+5); TAIL(objectFlickerVisibility); // jp
}

void ralph_decVar3f_hook(GB *gb) {
  BASE(ralph_decVar3f);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+4, b_+7); TAIL(writeFlagsTocddb); // jp
}
