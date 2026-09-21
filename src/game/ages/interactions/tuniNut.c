#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void interactionCodeb1_hook(GB *gb) {
  BASE(interactionCodeb1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL((SYM(interactiond7_makuSeed__state4Substate1) + 5));
  CYC(b_+3, b_+5); E = 0x3f;
  CYC(b_+5, b_+8); interBankCall_hook(gb); return;
}

void interactionCodeb2__state0_hook(GB *gb) {
  BASE(interactionCodeb2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+60, b_+61); A = alu_inc8(gb, A);
  CYC(b_+61, b_+63); mem_wr(gb, DE, A);
  CYC(b_+62, b_+66); mem_wr(gb, wScreenShakeMagnitude, A);
  CYC(b_+65, b_+68); SET_HL(b_+117);
  CYC(b_+68, b_+72); TAIL(interactionSetMiniScript);
}

// interactionCodeb2@setRandomCounter1: counter1 = (random & var30) + var31.
static void volcano_set_random_counter1(GB *gb) {
  BASE(interactionCodeb2__setRandomCounter1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  H = D;
  L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); alu_and(gb, mem_rd(gb, HL));
  L = alu_inc8(gb, L);
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); alu_add(gb, mem_rd(gb, HL));
  L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A);
  CYC(b_+12, b_+13); ret_effect(gb);
}

// interactionCodeb2@runScript: reads the next screen-shake entry from the mini script,
// restarting it at $ff, then falls into @setRandomCounter1.
static void volcano_run_script(GB *gb) {
  BASE(interactionCodeb2__runScript);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  for (;;) {
    CALL_C(b_+0, interactionGetMiniScript_hook, SYM(interactionGetMiniScript), b_+3);
    CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_cp(gb, 0xff);
    CYC(b_+4, b_+6);
    if (!(F & FZ)) { CYCT(b_+6, b_+8); break; }
    CYC(b_+6, b_+8);
    SET_HL(SYM(interactionCodeb2__script));
    CYC(b_+8, b_+11);
    CALL_C(b_+11, interactionSetMiniScript_hook, SYM(interactionSetMiniScript), b_+14);
    CYCT(b_+14, b_+16);
  }
  CYC(b_+16, b_+19); mem_wr(gb, wScreenShakeCounterY, A);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+23); mem_wr(gb, wScreenShakeCounterX, A);
  E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  E = alu_inc8(gb, E);
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CALL_C(b_+30, interactionSetMiniScript_hook, SYM(interactionSetMiniScript), b_+33);
  volcano_set_random_counter1(gb);
}

void interactionCodeb2__state1_hook(GB *gb) {
  BASE(interactionCodeb2);
  uint16_t sp0_ = gb->sp;
  CYC(b_+5, b_+8); A = mem_rd(gb, wFrameCounter);
  CYC(b_+8, b_+10); alu_and(gb, 0x0f);
  CYC(b_+10, b_+12); A = 0xb3;
  if (F & FZ) CALL_C_CC(b_+12, playSound_b00_hook, SYM(playSound_b00), b_+15); else CYC(b_+12, b_+15);
  CYC(b_+15, b_+18); A = mem_rd(gb, wScreenShakeCounterY);
  CYC(b_+18, b_+19); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto update; }
  CYC(b_+19, b_+21); A = mem_rd(gb, wScreenShakeCounterX);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+25, b_+28); push_effect(gb, b_+28); volcano_run_script(gb); } else CYC(b_+25, b_+28);
update:
  CALL_C(b_+28, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+32); ret_effect(gb); return; }
  CYC(b_+32, b_+35); push_effect(gb, b_+35); volcano_set_random_counter1(gb);
  CYC(b_+35, b_+37); C = 0x0f;
  CALL_C(b_+37, getRandomNumber_hook, SYM(getRandomNumber), b_+40);
  CYC(b_+40, b_+41); alu_and(gb, C);
  CYC(b_+41, b_+43); C = alu_srl(gb, C);
  CYC(b_+43, b_+44); C = alu_inc8(gb, C);
  CYC(b_+44, b_+45); alu_sub(gb, C);
  CYC(b_+45, b_+46); C = A;
  CALL_C(b_+46, getFreePartSlot_hook, SYM(getFreePartSlot), b_+49);
  if (!(F & FZ)) { CYCT(b_+49, b_+50); ret_effect(gb); return; }
  CYC(b_+50, b_+53); mem_wr(gb, HL, 0x11);
  CYC(b_+52, b_+53); L = alu_inc8(gb, L);
  CYC(b_+53, b_+56); mem_wr(gb, HL, 1);
  CYC(b_+55, b_+57); B = 0;
  CYC(b_+57, b_+60); TAIL(objectCopyPositionWithOffset);
}

void interactionCodeb2_hook(GB *gb) {
  BASE(interactionCodeb2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); interactionCodeb2__state0_hook(gb); return; }
  CYC(b_+3, b_+5); interactionCodeb2__state1_hook(gb);
}
