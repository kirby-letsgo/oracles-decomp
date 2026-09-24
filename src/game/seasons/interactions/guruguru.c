#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/guruguru.s.
// INTERAC_GURU_GURU

static uint16_t guruguru_jump_table(GB *gb) {
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

// INTERAC_GURU_GURU
void s_interactionCode58_hook(GB *gb) {
  BASE(interactionCode58);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (guruguru_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+53) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x01);
  CYC(b_+16, b_+18); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x01);
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x78);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x01);
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x01);
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x0f);
  CALL_C(b_+36, s_func_7e20_hook, SYM(func_7e20), b_+39);
  CYC(b_+39, b_+41); A = 0x0b;
  CALL_C(b_+41, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+44);
  CYC(b_+44, b_+47); SET_HL(SYM(guruGuruScript));
  CALL_C(b_+47, s_interactionSetScript, SYM(interactionSetScript), b_+50);
  CYC(b_+50, b_+53);
  TAIL_S(func_7ddc);
state1:
  CALL_C(b_+53, s_interactionRunScript, SYM(interactionRunScript), b_+56);
  CALL_C(b_+56, s_func_7deb_hook, SYM(func_7deb), b_+59);
  CYC(b_+59, b_+61);
  TAIL_S(func_7ddc);
}

void s_func_7ddc_hook(GB *gb) {
  BASE(func_7ddc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto L_7de5; }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, s_interactionAnimate, SYM(interactionAnimate), b_+9);
L_7de5:
  CALL_C(b_+9, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+12);
  CYC(b_+12, b_+15);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}

void s_func_7deb_hook(GB *gb) {
  BASE(func_7deb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (guruguru_jump_table(gb));
    if (jt_ == b_+8) goto var38_00;
    if (jt_ == SYM(func_7e38) && hook_is(gb, SYM(func_7e38), s_func_7e38_hook)) { s_func_7e38_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
var38_00:
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+12, b_+14); TAIL_S(func_7e0f); }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x78);
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_xor(gb, 0x10);
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); alu_xor(gb, 0x02);
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+36);
  TAIL(interactionSetAnimation);
}

void s_func_7e0f_hook(GB *gb) {
  BASE(func_7e0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x01);
  CYC(b_+5, b_+7); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+12); A = mem_rd(gb, w1Link_xh);
  CYC(b_+12, b_+14); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+14, b_+15); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+15, b_+17); TAIL_S(func_7e2c); }
  CYC(b_+15, b_+17);
  s_func_7e20_hook(gb); return; // falls through
}

void s_func_7e20_hook(GB *gb) {
  BASE(func_7e20);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x18);
  CYC(b_+4, b_+6); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+6, b_+8); A = 0x03;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+12);
  TAIL(interactionSetAnimation);
}

void s_func_7e2c_hook(GB *gb) {
  BASE(func_7e2c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0x08);
  CYC(b_+4, b_+6); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+6, b_+8); A = 0x01;
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+12);
  TAIL(interactionSetAnimation);
}

void s_func_7e38_hook(GB *gb) {
  BASE(func_7e38);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x00);
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x01);
  CYC(b_+14, b_+16); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x78);
  RET(b_+18); return;
}

