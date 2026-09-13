#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

static uint16_t punch_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void punch_body(GB *gb, uint16_t sp0_) {
  CYC(0x5f2a, 0x5f2c); E = 0x04;
  CYC(0x5f2c, 0x5f2d); A = mem_rd(gb, DE);
  CYC(0x5f2d, 0x5f2e); push_effect(gb, 0x5f2e);
  switch (punch_jump_table(gb)) {
    case 0x5f32:
      CALL_C(0x5f32, itemLoadAttributesAndGraphics_hook, 0x4993, 0x5f35);
      CYC(0x5f35, 0x5f37); C = 0xa6;
      CALL_C(0x5f37, itemIncState_hook, 0x23ea, 0x5f3a);
      CYC(0x5f3a, 0x5f3c); L = 0x06;
      CYC(0x5f3c, 0x5f3e); mem_wr(gb, HL, 0x04);
      CYC(0x5f3e, 0x5f40); L = 0x02;
      CYC(0x5f40, 0x5f42); alu_bit(gb, 0, mem_rd(gb, HL));
      if (F & FZ) {
        CYCT(0x5f42, 0x5f44);
      } else {
        CYC(0x5f42, 0x5f44);
        CYC(0x5f44, 0x5f46); L = 0x26;
        CYC(0x5f46, 0x5f48); A = 0x06;
        CYC(0x5f48, 0x5f49); mem_wr(gb, HL, A); SET_HL(HL + 1);
        CYC(0x5f49, 0x5f4a); mem_wr(gb, HL, A); SET_HL(HL + 1);
        CYC(0x5f4a, 0x5f4b); A = mem_rd(gb, HL);
        CYC(0x5f4b, 0x5f4d); alu_add(gb, 0xfd);
        CYC(0x5f4d, 0x5f4e); mem_wr(gb, HL, A);
        CYC(0x5f4e, 0x5f50); L = 0x24;
        CYC(0x5f50, 0x5f51); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
        CALL_C(0x5f51, tryBreakTileWithExpertsRing_hook, 0x618a, 0x5f54);
        CYC(0x5f54, 0x5f56); C = 0x6f;
      }
      CYC(0x5f56, 0x5f57); A = C;
      CYC(0x5f57, 0x5f5a); playSound_b00_hook(gb); return;
    case 0x5f5a:
      CALL_C(0x5f5a, itemDecCounter1_hook, 0x23d6, 0x5f5d);
      if (F & FZ) {
        CYCT(0x5f5d, 0x5f60); itemDelete_hook(gb); return;
      }
      CYC(0x5f5d, 0x5f60);
      CYC(0x5f60, 0x5f61); ret_effect(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
}

void itemCode00_hook(GB *gb) {
  punch_body(gb, gb->sp);
}

void itemCode02_hook(GB *gb) {
  punch_body(gb, gb->sp);
}
