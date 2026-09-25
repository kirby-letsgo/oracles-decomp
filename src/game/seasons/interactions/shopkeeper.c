#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/interactions/shopkeeper.s.

// State 3: Seasons - block Link access
void s_shopkeeperState3_hook(GB *gb) {
  BASE(shopkeeperState3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(shopkeeperScript_blockLinkAccess));
  CYC(b_+3, b_+6);
  TAIL(shopkeeperLoadScript);
}

// State 4: Running a script (prompting whether to buy, playing chest game, etc...)
void s_shopkeeperState4_hook(GB *gb) {
  BASE(shopkeeperState4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x80);
  CYC(b_+5, b_+7); A = 0x0c;
  if (!(F & FZ)) CALL_C_CC(b_+7, s_shopkeeperGetItemPrice, SYM(shopkeeperGetItemPrice), b_+10);
  else CYC(b_+7, b_+10);
  CALL_C(b_+10, s_interactionRunScript, SYM(interactionRunScript), b_+13);
  if (!(F & FC)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+22, b_+24); goto notRing; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CALL_C(b_+27, s_getRandomRingOfGivenTier, SYM(getRandomRingOfGivenTier), b_+30);
  CYC(b_+30, b_+31); B = C;
  CYC(b_+31, b_+33); C = 0x00;
  CALL_C(b_+33, s_giveRingToLink, SYM(giveRingToLink), b_+36);
  CYC(b_+36, b_+38); A = 0x01;
  CYC(b_+38, b_+41); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+41, b_+43);
  TAIL(shopkeeperGotoState1);
notRing:
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+47, b_+49); TAIL(shopkeeperGotoState1); }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); A = alu_inc8(gb, A);
  CYC(b_+50, b_+52); C = 0x04;
  if (F & FZ) { CYCT(b_+52, b_+54); goto setItemState; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); C = 0x03;
  CYC(b_+56, b_+58); A = 0x81;
  CYC(b_+58, b_+61); mem_wr(gb, wDisabledObjects, A);
setItemState:
  CYC(b_+61, b_+62); alu_xor(gb, A);
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CYC(b_+63, b_+65); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+67); H = A;
  CYC(b_+67, b_+69); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+69, b_+70); mem_wr(gb, HL, C);
  CALL_C(b_+70, s_dropLinkHeldItem, SYM(dropLinkHeldItem), b_+73);
  s_shopkeeperGotoState1(gb); return; // falls through
}
