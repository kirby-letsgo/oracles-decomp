#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x4fca, 0x4fcc); A = 1;
  CYC(0x4fcc, 0x4fcd); mem_wr(gb, DE, A);
  CALL_C(0x4fcd, getThisRoomFlags_hook, 0x197d, 0x4fd0);
  CYC(0x4fd0, 0x4fd2); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(0x4fd2, 0x4fd5); interactionDelete_hook(gb); return; }
  CYC(0x4fd2, 0x4fd5);
  CYC(0x4fd5, 0x4fd7); A = 0xac;
  CYC(0x4fd7, 0x4fda); loadPaletteHeader_hook(gb);
}

void interactionCodeda__state1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4fda, checkLinkVulnerable_hook, 0x1d28, 0x4fdd);
  if (!(F & FC)) { CYCT(0x4fdd, 0x4fde); ret_effect(gb); return; }
  CYC(0x4fdd, 0x4fde);
  CYC(0x4fde, 0x4fe1); A = mem_rd(gb, 0xcd00);
  CYC(0x4fe1, 0x4fe3); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x4fe3, 0x4fe4); ret_effect(gb); return; }
  CYC(0x4fe3, 0x4fe4);
  CYC(0x4fe4, 0x4fe7); SET_HL(0xd00b);
  CYC(0x4fe7, 0x4fe9); E = 0x4b;
  CYC(0x4fe9, 0x4fea); A = mem_rd(gb, DE);
  CYC(0x4fea, 0x4feb); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x4feb, 0x4fec); ret_effect(gb); return; }
  CYC(0x4feb, 0x4fec);
  CYC(0x4fec, 0x4fee); L = 0x0d;
  CYC(0x4fee, 0x4ff0); E = 0x4d;
  CYC(0x4ff0, 0x4ff1); A = mem_rd(gb, DE);
  CYC(0x4ff1, 0x4ff2); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) CYC(0x4ff2, 0x4ff4);
  else {
    CYCT(0x4ff2, 0x4ff4);
    CYC(0x4ff4, 0x4ff5); alu_cpl(gb);
    CYC(0x4ff5, 0x4ff6); A = alu_inc8(gb, A);
  }
  CYC(0x4ff6, 0x4ff8); alu_cp(gb, 9);
  if (!(F & FC)) { CYCT(0x4ff8, 0x4ff9); ret_effect(gb); return; }
  CYC(0x4ff8, 0x4ff9);
  CYC(0x4ff9, 0x4ffb); A = 0x17;
  CYC(0x4ffb, 0x4ffe); mem_wr(gb, 0xcc04, A);
  CYC(0x4ffe, 0x5001); mem_wr(gb, 0xcc02, A);
  CYC(0x5001, 0x5004); SET_HL(0xd240);
  for (;;) {
    CYC(0x5004, 0x5006); L = 0x40;
    CYC(0x5006, 0x5007); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5007, 0x5008); alu_or(gb, A);
    if (F & FZ) { CYCT(0x5008, 0x500a); goto next_interaction; }
    CYC(0x5008, 0x500a); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x500a, 0x500b); alu_cp(gb, 0xa9);
    if (!(F & FZ)) { CYCT(0x500d, 0x500f); goto next_interaction; }
    CYC(0x500d, 0x500f);
    CYC(0x500f, 0x5011); L = 0x5a;
    CYC(0x5011, 0x5013); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
next_interaction:
    CYC(0x5013, 0x5014); H = alu_inc8(gb, H);
    CYC(0x5014, 0x5015); A = H;
    CYC(0x5015, 0x5017); alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x5017, 0x5019); continue; }
    CYC(0x5017, 0x5019);
    CYC(0x5019, 0x501c); interactionDelete_hook(gb); return;
  }
}

void interactionCodeda_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4fc2, 0x4fc4); E = 0x44;
  CYC(0x4fc4, 0x4fc5); A = mem_rd(gb, DE);
  CYC(0x4fc5, 0x4fc6); push_effect(gb, 0x4fc6);
  switch (zelda_approach_jump_table(gb)) {
    case 0x4fca: interactionCodeda__state0_hook(gb); return;
    case 0x4fda: interactionCodeda__state1_hook(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}
