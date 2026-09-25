#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/oldManWithJewel.s.
// INTERAC_OLD_MAN_WITH_JEWEL
// Variables:
// var35: $01 if Link has at least 5 essences

static uint16_t oldManWithJewel_jump_table(GB *gb) {
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

// interactionCode8f@checkHaveEssences
static void oldManWithJewel_checkHaveEssences(GB *gb) {
  BASE(interactionCode8f);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+41, b_+44); A = mem_rd(gb, wEssencesObtained);
  CALL_C(b_+44, s_getNumSetBits, SYM(getNumSetBits), b_+47);
  CYC(b_+47, b_+48); H = D;
  CYC(b_+48, b_+50); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+50, b_+52); alu_cp(gb, 0x05);
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x00);
  if (F & FC) { RET_TAKEN(b_+54); return; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+56); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+56); return;
}

// INTERAC_OLD_MAN_WITH_JEWEL
// Variables:
// var35: $01 if Link has at least 5 essences
void s_interactionCode8f_hook(GB *gb) {
  BASE(interactionCode8f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (oldManWithJewel_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+35) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); A = 0x36;
  CALL_C(b_+16, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+19);
  CYC(b_+19, b_+22); SET_HL(SYM(oldManWithJewelScript));
  CALL_C(b_+22, s_interactionSetScript, SYM(interactionSetScript), b_+25);
  CALL_L(b_+25, oldManWithJewel_checkHaveEssences, b_+28);
  CYC(b_+28, b_+30); A = 0x02;
  CALL_C(b_+30, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+33);
  CYC(b_+33, b_+35);
  goto state1;
state1:
  CALL_C(b_+35, s_interactionRunScript, SYM(interactionRunScript), b_+38);
  CYC(b_+38, b_+41);
  TAIL(npcFaceLinkAndAnimate);
}
