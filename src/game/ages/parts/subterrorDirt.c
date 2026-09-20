#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t subterrorDirt_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

// PART_SUBTERROR_DIRT
void partCode32_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x64bf, 0x64c1); E = 0xc4; // Part.state
  CYC(0x64c1, 0x64c2); A = mem_rd(gb, DE);
  {
    CYC(0x64c2, 0x64c3); push_effect(gb, 0x64c3);
    uint16_t target = subterrorDirt_jump_table(gb);
    if (target == 0x64c7) goto state0;
    goto state1;
  }

state0:
  CYC(0x64c7, 0x64c9); A = 0x01;
  CYC(0x64c9, 0x64ca); mem_wr(gb, DE, A);
  CYC(0x64ca, 0x64cc); A = 0xa9; // SND_DIG
  CALL_C(0x64cc, playSound_b00_hook, 0x0c98, 0x64cf);

state1:
  CALL_C(0x64cf, partAnimate_hook, 0x2978, 0x64d2);
  CYC(0x64d2, 0x64d4); E = 0xe1; // Part.animParameter
  CYC(0x64d4, 0x64d5); A = mem_rd(gb, DE);
  CYC(0x64d5, 0x64d7); E = 0xda; // Part.visible
  CYC(0x64d7, 0x64d8); mem_wr(gb, DE, A);
  CYC(0x64d8, 0x64d9); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x64d9); return; } // ret nz
  CYC(0x64d9, 0x64da);
  CYC(0x64da, 0x64dd); partDelete_hook(gb); return; // jp
}
