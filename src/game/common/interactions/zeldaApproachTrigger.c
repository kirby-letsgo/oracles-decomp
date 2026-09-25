#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t zelda_approach_jump_table(GB *gb) {
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

void interactionCodeda__state0_hook(GB *gb) {
  BASE(interactionCodeda);
  uint16_t sp0_ = gb->sp;
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); interactionDelete_hook(gb); return; }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); A = 0xac;
  CYC(b_+21, b_+24); TAIL(loadPaletteHeader);
}

void interactionCodeda__state1_hook(GB *gb) {
  BASE(interactionCodeda);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+24, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+27);
  if (!(F & FC)) { CYCT(b_+27, b_+28); ret_effect(gb); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+31); A = mem_rd(gb, wScreenVariables);
  CYC(b_+31, b_+33); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+33, b_+34); ret_effect(gb); return; }
  CYC(b_+33, b_+34);
  CYC(b_+34, b_+37); SET_HL(w1Link_yh);
  CYC(b_+37, b_+39); E = 0x4b;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+41, b_+42); ret_effect(gb); return; }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+44); L = 0x0d;
  CYC(b_+44, b_+46); E = 0x4d;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYC(b_+48, b_+50);
  else {
    CYCT(b_+48, b_+50);
    CYC(b_+50, b_+51); alu_cpl(gb);
    CYC(b_+51, b_+52); A = alu_inc8(gb, A);
  }
  CYC(b_+52, b_+54); alu_cp(gb, 9);
  if (!(F & FC)) { CYCT(b_+54, b_+55); ret_effect(gb); return; }
  CYC(b_+54, b_+55);
  CYC(b_+55, b_+57); A = 0x17;
  CYC(b_+57, b_+60); mem_wr(gb, wCutsceneTrigger, A);
  CYC(b_+60, b_+63); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+63, b_+66); SET_HL(w4StatusBarTileMap);
  for (;;) {
    CYC(b_+66, b_+68); L = 0x40;
    CYC(b_+68, b_+69); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+69, b_+70); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+70, b_+72); goto next_interaction; }
    CYC(b_+70, b_+72);
    CYC(b_+72, b_+73); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+73, b_+75); alu_cp(gb, GV(0xa9, 0xb0));
    if (!(F & FZ)) { CYCT(b_+75, b_+77); goto next_interaction; }
    CYC(b_+75, b_+77);
    CYC(b_+77, b_+79); L = 0x5a;
    CYC(b_+79, b_+81); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
next_interaction:
    CYC(b_+81, b_+82); H = alu_inc8(gb, H);
    CYC(b_+82, b_+83); A = H;
    CYC(b_+83, b_+85); alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+85, b_+87); continue; }
    CYC(b_+85, b_+87);
    CYC(b_+87, b_+90); TAIL(interactionDelete);
  }
}

void interactionCodeda_hook(GB *gb) {
  BASE(interactionCodeda);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (zelda_approach_jump_table(gb));
    if (jt_ == b_+8) { interactionCodeda__state0_hook(gb); return; }
    else if (jt_ == b_+24) { interactionCodeda__state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
