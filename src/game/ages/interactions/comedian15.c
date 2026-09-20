#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_COMEDIAN), bank 0x15.

// Set var3f to: $00 before beating d2, $01 after beating d2, $02 after beating moonlit grotto.
void comedian_checkGameProgress_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6259, 0x625c); A = mem_rd(gb, wEssencesObtained);
  CALL_C(0x625c, getHighestSetBit_hook, 0x01ea, 0x625f);
  CYC(0x625f, 0x6261); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x6261, 0x6263); goto write_progress; } // jr c
  CYC(0x6261, 0x6263);
  CYC(0x6263, 0x6265); A = 0x02;

write_progress:
  CYC(0x6265, 0x6267); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x6267, 0x6268); mem_wr(gb, DE, A);
  RET(0x6268); return;
}

void checkEssenceNotObtained_hook(GB *gb);

// @param a Essence to check for
// @param[out] zflag z if essence obtained
void checkEssenceObtained_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6269, checkEssenceNotObtained_hook, 0x6271, 0x626c);
  CYC(0x626c, 0x626d); alu_cpl(gb);
  CYC(0x626d, 0x6270); mem_wr(gb, wcddb, A);
  RET(0x6270); return;
}

// @param a Essence to check for
// @param[out] zflag z if essence not obtained
void checkEssenceNotObtained_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x6271, 0x6274); SET_HL(wEssencesObtained);
  CALL_C(0x6274, checkFlag_hook, 0x0205, 0x6277);
  CYC(0x6277, 0x627a); writeFlagsTocddb_hook(gb); return; // jp
}

void comedian_enableMustache_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x627a, 0x627c); A = 0x04;
  CYC(0x627c, 0x627e); // jr $6280 -- shared tail duplicated below (matches comedian_disableMustache)
  CYC(0x6280, 0x6281); H = D;
  CYC(0x6281, 0x6283); L = INTERACTION_BASE + OBJ_VAR37; // animation base
  CYC(0x6283, 0x6284); mem_wr(gb, HL, A);
  CYC(0x6284, 0x6286); L = INTERACTION_BASE + OBJ_VAR3E; // force animation refresh next time
  CYC(0x6286, 0x6288); mem_wr(gb, HL, 0xff);
  RET(0x6288); return;
}

void comedian_disableMustache_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x627e, 0x6280); A = 0x00;
  CYC(0x6280, 0x6281); H = D;
  CYC(0x6281, 0x6283); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x6283, 0x6284); mem_wr(gb, HL, A);
  CYC(0x6284, 0x6286); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x6286, 0x6288); mem_wr(gb, HL, 0xff);
  RET(0x6288); return;
}

// Turn to face link, accounting for the fact that he only faces left and right.
void comedian_turnToFaceLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6289, 0x628a); H = D;
  CYC(0x628a, 0x628c); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x628c, 0x628f); A = W8(w1Link_xh);
  CYC(0x628f, 0x6290); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x6290, 0x6292); A = 0x01;
  if (!(F & FC)) { CYCT(0x6292, 0x6294); goto compare_var3e; } // jr nc
  CYC(0x6292, 0x6294);
  CYC(0x6294, 0x6295); alu_xor(gb, A);

compare_var3e:
  CYC(0x6295, 0x6297); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x6297, 0x6298); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(0x6298); return; }
  CYC(0x6298, 0x6299);
  CYC(0x6299, 0x629a); mem_wr(gb, HL, A);
  CYC(0x629a, 0x629c); L = INTERACTION_BASE + OBJ_VAR37; // "animation base"?
  CYC(0x629c, 0x629d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x629d, 0x62a0); interactionSetAnimation_hook(gb); return; // jp
}
