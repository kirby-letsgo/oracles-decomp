#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(createExclamationMark);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xf300);
  CYC(b_+3, b_+6); TAIL(objectCreateExclamationMark); // jp
}

void oscillateXRandomly_hook(GB *gb) {
  BASE(oscillateXRandomly);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); SET_HL((SYM(oldManScript_givesShieldUpgrade__validSecret_b15) + 16)); // agesInteractionsBank08.interactionOscillateXRandomly
  CYC(b_+3, b_+5); E = 0x08; // target bank
  CYC(b_+5, b_+8); interBankCall_hook(gb); return; // jp
}

// Forces the next animation frame to be loaded; does something with var38 and cfd3.
// @param a ?
void loadNextAnimationFrameAndMore_hook(GB *gb) {
  BASE(loadNextAnimationFrameAndMore);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x01);
  CYC(b_+5, b_+7); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+8, b_+11); mem_wr(gb, wTmpcfc0_genericCutscene_cfd3, A);
  CYC(b_+11, b_+14); TAIL(interactionAnimate); // jp
}

// Creates lightning for the cutscene where the boy's father turns to stone.
// @param a Index of lightning to make (0-1)
void boy_createLightning_hook(GB *gb) {
  BASE(boy_createLightning);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); B = A;
  CALL_C(b_+1, getFreePartSlot_hook, SYM(getFreePartSlot), b_+4);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x27); // PART_LIGHTNING
  CYC(b_+7, b_+8); L = alu_inc8(gb, L);
  CYC(b_+8, b_+9); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+12); A = B;
  CYC(b_+12, b_+13); alu_or(gb, A);
  CYC(b_+13, b_+16); SET_BC((SYM(group2ObjectDataTable) + 253));
  if (F & FZ) { CYCT(b_+16, b_+18); goto write_pos; } // jr z
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+21); SET_BC(0x2878);

write_pos:
  CYC(b_+21, b_+23); L = PART_BASE + OBJ_YH;
  CYC(b_+23, b_+24); mem_wr(gb, HL, B);
  CYC(b_+24, b_+26); L = PART_BASE + OBJ_XH;
  CYC(b_+26, b_+27); mem_wr(gb, HL, C);
  RET(b_+27); return;
}

// Updates the funny joke cutscene by determining whether to update Link's animation.
// Uses var3f as a counter until Link proceeds to the next animation; var3e as the index of the
// current animation.
void boy_runFunnyJokeCutscene_hook(GB *gb) {
  BASE(boy_runFunnyJokeCutscene);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); alu_cp(gb, 0x14);
  CALL_C(b_+10, writeFlagsTocddb_hook, SYM(writeFlagsTocddb), b_+13);
  if (F & FZ) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+19); SET_HL(b_+29); // @animations data table
  CYC(b_+19, b_+20); boy15_addDoubleIndexToHl_from_rst(gb, b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+24); mem_wr(gb, wcc50, A); // set Link animation
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  RET(b_+28); return;
}
