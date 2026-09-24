#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/goldenBeastOldMan.s.
// INTERAC_GOLDEN_BEAST_OLD_MAN

// INTERAC_GOLDEN_BEAST_OLD_MAN
void s_interactionCodedd_hook(GB *gb) {
  BASE(interactionCodedd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto L_6aa3; }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, s_interactionRunScript, SYM(interactionRunScript), b_+9);
  if (F & FC) { CYCT(b_+9, b_+12); TAIL(interactionDelete); }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+15);
  TAIL(npcFaceLinkAndAnimate);
L_6aa3:
  CALL_C(b_+15, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+20, b_+23); TAIL(interactionDelete); }
  CYC(b_+20, b_+23);
  CALL_C(b_+23, s_interactionIncState, SYM(interactionIncState), b_+26);
  CALL_C(b_+26, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+29);
  CALL_C(b_+29, s_objectSetVisible82, SYM(objectSetVisible82), b_+32);
  CYC(b_+32, b_+34); A = 0x1f;
  CALL_C(b_+34, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+37);
  CYC(b_+37, b_+40); SET_HL(SYM(goldenBeastOldManScript));
  CYC(b_+40, b_+43);
  TAIL(interactionSetScript);
}

void s_checkGoldenBeastsKilled_hook(GB *gb) {
  BASE(checkGoldenBeastsKilled);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); SET_HL(wTextNumberSubstitution);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+6, b_+9); A = mem_rd(gb, wKilledGoldenEnemies);
  CYC(b_+9, b_+11); alu_and(gb, 0x0f);
  CALL_C(b_+11, s_getNumSetBits, SYM(getNumSetBits), b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+17); alu_cp(gb, 0x04);
  CYC(b_+17, b_+19); A = 0x01;
  if (F & FZ) { CYCT(b_+19, b_+21); goto L_6ad5; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); A = alu_dec8(gb, A);
L_6ad5:
  CYC(b_+22, b_+25); mem_wr(gb, wTmpcfc0 + 0x01, A);
  RET(b_+25); return;
}

void s_giveRedRing_hook(GB *gb) {
  BASE(giveRedRing);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0x0700);
  CYC(b_+3, b_+6);
  TAIL(giveRingToLink);
}

