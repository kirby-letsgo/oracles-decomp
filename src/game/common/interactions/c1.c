#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodec1), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodec1), (from), (to), true)

static uint16_t c1_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

void interactionCodec1__afterCall71ce_hook(GB *gb) {
  BASE(interactionCodec1);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = 0x46;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x86);
  CYC(b_+19, b_+20); L = alu_inc8(gb, L);
  CYC(b_+20, b_+22); mem_wr(gb, HL, 1);
  CYC(b_+22, b_+24); L = 0x76;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 6);
  CYC(b_+26, b_+28); L = 0x49;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x15);
  CYC(b_+30, b_+32); L = 0x50;
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0x78);
  CYC(b_+34, b_+37); objectSetVisible82_hook(gb);
}

void interactionCodec1__state0_hook(GB *gb) {
  BASE(interactionCodec1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  interactionCodec1__afterCall71ce_hook(gb);
}

void interactionCodec1__ret_hook(GB *gb) {
  BASE(interactionCodec1);
  CYC(b_+99, b_+100); ret_effect(gb);
}

void interactionCodec1__updateSparkles_hook(GB *gb) {
  BASE(interactionCodec1);
  CYC(b_+111, b_+112); H = D;
  CYC(b_+112, b_+114); L = 0x76;
  CYC(b_+114, b_+115); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+115, b_+116); ret_effect(gb); return; }
  CYC(b_+115, b_+116);
  CYC(b_+116, b_+118); mem_wr(gb, HL, 6);
  CYC(b_+118, b_+121); SET_BC(0x8409);
  CYC(b_+121, SYM(interactionCodec2)); objectCreateInteraction_hook(gb);
}

void interactionCodec1__updateMovementAndSparkles_hook(GB *gb) {
  BASE(interactionCodec1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+90, interactionCodec1__updateSparkles_hook, b_+111, b_+93);
  CALL_C(b_+93, objectApplySpeed_hook, SYM(objectApplySpeed), b_+96);
  CYC(b_+96, b_+99); interactionDecCounter1_hook(gb);
}

void interactionCodec1__substate0_hook(GB *gb) {
  BASE(interactionCodec1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+47, b_+48); H = D;
  CYC(b_+48, b_+50); L = 0x46;
  CALL_C(b_+50, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+53);
  if (!(F & FZ)) { CYCT(b_+53, b_+54); ret_effect(gb); return; }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+56); L = 0x46;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x28);
  CYC(b_+58, b_+61); interactionIncSubstate_hook(gb);
}

void interactionCodec1__substate1_hook(GB *gb) {
  BASE(interactionCodec1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+61, interactionCodec1__updateMovementAndSparkles_hook, b_+90, b_+64);
  if (!(F & FZ)) { CYCT(b_+64, b_+66); interactionCodec1__ret_hook(gb); return; }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); L = 0x60;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 1);
  CYC(b_+70, b_+73); interactionIncSubstate_hook(gb);
}

void interactionCodec1__substate2_hook(GB *gb) {
  BASE(interactionCodec1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+73, interactionAnimate_hook, SYM(interactionAnimate), b_+76);
  CALL_C(b_+76, interactionCodec1__updateSparkles_hook, b_+111, b_+79);
  CALL_C(b_+79, objectApplySpeed_hook, SYM(objectApplySpeed), b_+82);
  CYC(b_+82, b_+84); E = 0x61;
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+86); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+86, b_+89); interactionDelete_hook(gb); return; }
  CYC(b_+86, b_+89);
  CYC(b_+89, b_+90); ret_effect(gb);
}

void interactionCodec1__state1_hook(GB *gb) {
  BASE(interactionCodec1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+37, b_+39); E = 0x45;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); push_effect(gb, b_+41);
  do { uint16_t jt_ = (c1_jump_table(gb));
    if (jt_ == b_+47) { interactionCodec1__substate0_hook(gb); return; }
    else if (jt_ == b_+61) { interactionCodec1__substate1_hook(gb); return; }
    else if (jt_ == b_+73) { interactionCodec1__substate2_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void interactionCodec1__func_7224_hook(GB *gb) {
  BASE(interactionCodec1);
  CYC(b_+100, b_+103); A = mem_rd(gb, wFrameCounter);
  CYC(b_+103, b_+105); alu_and(gb, 1);
  if (F & FZ) { CYCT(b_+105, b_+108); objectSetInvisible_hook(gb); return; }
  CYC(b_+105, b_+108);
  CYC(b_+108, b_+111); objectSetVisible_hook(gb);
}

void interactionCodec1_hook(GB *gb) {
  BASE(interactionCodec1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (c1_jump_table(gb));
    if (jt_ == b_+8) { interactionCodec1__state0_hook(gb); return; }
    else if (jt_ == b_+37) { interactionCodec1__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
