#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_BOY), bank 0x15.

static void boy15_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// @param a Duration
void createExclamationMark_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5854, 0x5857); SET_BC(0xf300);
  CYC(0x5857, 0x585a); objectCreateExclamationMark_hook(gb); return; // jp
}

void oscillateXRandomly_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x585a, 0x585d); SET_HL(0x5d87); // agesInteractionsBank08.interactionOscillateXRandomly
  CYC(0x585d, 0x585f); E = 0x08; // target bank
  CYC(0x585f, 0x5862); interBankCall_hook(gb); return; // jp
}

// Forces the next animation frame to be loaded; does something with var38 and cfd3.
// @param a ?
void loadNextAnimationFrameAndMore_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5862, 0x5863); H = D;
  CYC(0x5863, 0x5865); L = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(0x5865, 0x5867); mem_wr(gb, HL, 0x01);
  CYC(0x5867, 0x5869); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x5869, 0x586a); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x586a, 0x586d); mem_wr(gb, wTmpcfc0_genericCutscene_cfd3, A);
  CYC(0x586d, 0x5870); interactionAnimate_hook(gb); return; // jp
}

// Creates lightning for the cutscene where the boy's father turns to stone.
// @param a Index of lightning to make (0-1)
void boy_createLightning_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5870, 0x5871); B = A;
  CALL_C(0x5871, getFreePartSlot_hook, 0x3e8e, 0x5874);
  if (!(F & FZ)) { RET_TAKEN(0x5874); return; }
  CYC(0x5874, 0x5875);
  CYC(0x5875, 0x5877); mem_wr(gb, HL, 0x27); // PART_LIGHTNING
  CYC(0x5877, 0x5878); L = alu_inc8(gb, L);
  CYC(0x5878, 0x5879); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5879, 0x587a); L = alu_inc8(gb, L);
  CYC(0x587a, 0x587b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x587b, 0x587c); A = B;
  CYC(0x587c, 0x587d); alu_or(gb, A);
  CYC(0x587d, 0x5880); SET_BC(0x4838);
  if (F & FZ) { CYCT(0x5880, 0x5882); goto write_pos; } // jr z
  CYC(0x5880, 0x5882);
  CYC(0x5882, 0x5885); SET_BC(0x2878);

write_pos:
  CYC(0x5885, 0x5887); L = PART_BASE + OBJ_YH;
  CYC(0x5887, 0x5888); mem_wr(gb, HL, B);
  CYC(0x5888, 0x588a); L = PART_BASE + OBJ_XH;
  CYC(0x588a, 0x588b); mem_wr(gb, HL, C);
  RET(0x588b); return;
}

// Updates the funny joke cutscene by determining whether to update Link's animation.
// Uses var3f as a counter until Link proceeds to the next animation; var3e as the index of the
// current animation.
void boy_runFunnyJokeCutscene_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x588c, 0x588d); H = D;
  CYC(0x588d, 0x588f); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x588f, 0x5890); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x5890); return; }
  CYC(0x5890, 0x5891);
  CYC(0x5891, 0x5893); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x5893, 0x5894); A = mem_rd(gb, HL);
  CYC(0x5894, 0x5896); alu_cp(gb, 0x14);
  CALL_C(0x5896, writeFlagsTocddb_hook, 0x5118, 0x5899);
  if (F & FZ) { RET_TAKEN(0x5899); return; }
  CYC(0x5899, 0x589a);
  CYC(0x589a, 0x589b); A = mem_rd(gb, HL);
  CYC(0x589b, 0x589c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x589c, 0x589f); SET_HL(0x58a9); // @animations data table
  CYC(0x589f, 0x58a0); boy15_addDoubleIndexToHl_from_rst(gb, 0x58a0);
  CYC(0x58a0, 0x58a1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x58a1, 0x58a4); mem_wr(gb, wcc50, A); // set Link animation
  CYC(0x58a4, 0x58a5); A = mem_rd(gb, HL);
  CYC(0x58a5, 0x58a7); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x58a7, 0x58a8); mem_wr(gb, DE, A);
  RET(0x58a8); return;
}
