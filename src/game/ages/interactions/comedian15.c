#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_COMEDIAN), bank 0x15.

// Set var3f to: $00 before beating d2, $01 after beating d2, $02 after beating moonlit grotto.
void comedian_checkGameProgress_hook(GB *gb) {
  BASE(comedian_checkGameProgress);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); A = mem_rd(gb, wEssencesObtained);
  CALL_C(b_+3, getHighestSetBit_hook, SYM(getHighestSetBit), b_+6);
  CYC(b_+6, b_+8); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+8, b_+10); goto write_progress; } // jr c
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); A = 0x02;

write_progress:
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  RET(b_+15); return;
}

void checkEssenceNotObtained_hook(GB *gb);

// @param a Essence to check for
// @param[out] zflag z if essence obtained
void checkEssenceObtained_hook(GB *gb) {
  BASE(checkEssenceObtained);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkEssenceNotObtained_hook, SYM(checkEssenceNotObtained), b_+3);
  CYC(b_+3, b_+4); alu_cpl(gb);
  CYC(b_+4, b_+7); mem_wr(gb, wcddb, A);
  RET(b_+7); return;
}

// @param a Essence to check for
// @param[out] zflag z if essence not obtained
void checkEssenceNotObtained_hook(GB *gb) {
  BASE(checkEssenceNotObtained);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wEssencesObtained);
  CALL_C(b_+3, checkFlag_hook, SYM(checkFlag), b_+6);
  CYC(b_+6, b_+9); TAIL(writeFlagsTocddb); // jp
}

void comedian_enableMustache_hook(GB *gb) {
  BASE(comedian_disableMustache);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(SYM(comedian_enableMustache), (SYM(comedian_enableMustache) + 2)); A = 0x04;
  CYC((SYM(comedian_enableMustache) + 2), (SYM(comedian_enableMustache) + 4)); // jr $6280 -- shared tail duplicated below (matches comedian_disableMustache)
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = INTERACTION_BASE + OBJ_VAR37; // animation base
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_VAR3E; // force animation refresh next time
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0xff);
  RET(b_+10); return;
}

void comedian_disableMustache_hook(GB *gb) {
  BASE(comedian_disableMustache);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+2); A = 0x00;
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+5, b_+6); mem_wr(gb, HL, A);
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0xff);
  RET(b_+10); return;
}

// Turn to face link, accounting for the fact that he only faces left and right.
void comedian_turnToFaceLink_hook(GB *gb) {
  BASE(comedian_turnToFaceLink);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+3, b_+6); A = W8(w1Link_xh);
  CYC(b_+6, b_+7); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+9); A = 0x01;
  if (!(F & FC)) { CYCT(b_+9, b_+11); goto compare_var3e; } // jr nc
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); alu_xor(gb, A);

compare_var3e:
  CYC(b_+12, b_+14); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+14, b_+15); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+15); return; }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_VAR37; // "animation base"?
  CYC(b_+19, b_+20); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+23); TAIL(interactionSetAnimation); // jp
}
