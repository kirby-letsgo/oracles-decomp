#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode32), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode32), (from), (to), true)

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
  BASE(partCode32);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = subterrorDirt_jump_table(gb);
    if (target == b_+8) goto state0;
    goto state1;
  }

state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0xa9; // SND_DIG
  CALL_C(b_+13, playSound_b00_hook, SYM(playSound_b00), b_+16);

state1:
  CALL_C(b_+16, partAnimate_hook, SYM(partAnimate), b_+19);
  CYC(b_+19, b_+21); E = 0xe1; // Part.animParameter
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); E = 0xda; // Part.visible
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+26); return; } // ret nz
  CYC(b_+26, b_+27);
  CYC(b_+27, SYM(partCode33)); partDelete_hook(gb); return; // jp
}
