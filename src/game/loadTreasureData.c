#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x16, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x16, (from), (to), true)

void interactionLoadTreasureData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x451e, 0x4520); E = 0x42;
  CYC(0x4520, 0x4521); A = mem_rd(gb, DE);
  CYC(0x4521, 0x4523); E = 0x70;
  CYC(0x4523, 0x4524); mem_wr(gb, DE, A);
  CYC(0x4524, 0x4527); SET_HL(0x5332);
  for (;;) {
    CALL_C(0x4527, multiplyABy4_hook, 0x01c3, 0x452a);
    CYC(0x452a, 0x452b); alu_add_hl(gb, BC);
    CYC(0x452b, 0x452d); alu_bit(gb, 7, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(0x452d, 0x452f);
      break;
    }
    CYC(0x452d, 0x452f);
    CYC(0x452f, 0x4530); SET_HL(HL + 1);
    CYC(0x4530, 0x4531); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4531, 0x4532); H = mem_rd(gb, HL);
    CYC(0x4532, 0x4533); L = A;
    CYC(0x4533, 0x4535); E = 0x43;
    CYC(0x4535, 0x4536); A = mem_rd(gb, DE);
    CYC(0x4536, 0x4538);
  }
  CYC(0x4538, 0x4539); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4539, 0x453a); B = A;
  CYC(0x453a, 0x453c); A = alu_swap(gb, A);
  CYC(0x453c, 0x453e); alu_and(gb, 0x07);
  CYC(0x453e, 0x4540); E = 0x71;
  CYC(0x4540, 0x4541); mem_wr(gb, DE, A);
  CYC(0x4541, 0x4542); A = B;
  CYC(0x4542, 0x4544); alu_and(gb, 0x07);
  CYC(0x4544, 0x4545); E = alu_inc8(gb, E);
  CYC(0x4545, 0x4546); mem_wr(gb, DE, A);
  CYC(0x4546, 0x4547); A = B;
  CYC(0x4547, 0x4549); alu_and(gb, 0x08);
  CYC(0x4549, 0x454a); E = alu_inc8(gb, E);
  CYC(0x454a, 0x454b); mem_wr(gb, DE, A);
  CYC(0x454b, 0x454c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x454c, 0x454d); E = alu_inc8(gb, E);
  CYC(0x454d, 0x454e); mem_wr(gb, DE, A);
  CYC(0x454e, 0x454f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x454f, 0x4550); E = alu_inc8(gb, E);
  CYC(0x4550, 0x4551); mem_wr(gb, DE, A);
  CYC(0x4551, 0x4552); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4552, 0x4554); E = 0x42;
  CYC(0x4554, 0x4555); mem_wr(gb, DE, A);
  CYC(0x4555, 0x4556); ret_effect(gb);
}
