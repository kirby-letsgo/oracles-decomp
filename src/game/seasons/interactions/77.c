#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/77.s.
// INTERAC_77

static uint16_t i77_jump_table(GB *gb) {
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

// INTERAC_77
void s_interactionCode77_hook(GB *gb) {
  BASE(interactionCode77);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i77_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+30) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CALL_C(b_+17, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+20);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+24, b_+27); TAIL(objectSetVisible82); }
  CYC(b_+24, b_+27);
  CYC(b_+27, b_+30);
  TAIL(objectSetVisible83);
state1:
  CYC(b_+30, b_+33); SET_HL(wTmpcfc0 + 0x13);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+36); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+36, b_+39); TAIL(objectSetInvisible); }
  CYC(b_+36, b_+39);
  CALL_C(b_+39, s_objectSetVisible, SYM(objectSetVisible), b_+42);
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+46); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto func_74dd; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+51); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+51, b_+52); B = A;
  CYC(b_+52, b_+54); A = 0x7d;
  CYC(b_+54, b_+55); alu_sub(gb, B);
  CYC(b_+55, b_+57); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+61); A = mem_rd(gb, wGfxRegs1_SCX);
  CYC(b_+61, b_+62); B = A;
  CYC(b_+62, b_+64); A = 0x54;
  CYC(b_+64, b_+65); alu_sub(gb, B);
  CYC(b_+65, b_+67); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+67, b_+68); mem_wr(gb, DE, A);
  RET(b_+68); return;
func_74dd:
  CYC(b_+69, b_+72); A = mem_rd(gb, wGfxRegs1_WINY);
  CYC(b_+72, b_+73); B = A;
  CYC(b_+73, b_+75); A = 0xe9;
  CYC(b_+75, b_+76); alu_add(gb, B);
  CYC(b_+76, b_+78); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+78, b_+79); mem_wr(gb, DE, A);
  CYC(b_+79, b_+82); A = mem_rd(gb, wGfxRegs1_WINX);
  CYC(b_+82, b_+83); B = A;
  CYC(b_+83, b_+85); A = 0x19;
  CYC(b_+85, b_+86); alu_add(gb, B);
  CYC(b_+86, b_+88); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+88, b_+89); mem_wr(gb, DE, A);
  RET(b_+89); return;
}

