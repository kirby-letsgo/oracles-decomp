#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

void objectCreateExclamationMark_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x406d, 0x406f); H8(0xff8b) = A;
  CALL_C(0x406f, getFreeInteractionSlot_hook, 0x3aef, 0x4072);
  if (!(F & FZ)) {
    CYCT(0x4072, 0x4073); ret_effect(gb);
    return;
  }
  CYC(0x4072, 0x4073);
  CYC(0x4073, 0x4075); mem_wr(gb, HL, 0x9f);
  CYC(0x4075, 0x4077); L = 0x46;
  CYC(0x4077, 0x4079); A = H8(0xff8b);
  CYC(0x4079, 0x407a); mem_wr(gb, HL, A);
  CALL_C(0x407a, objectCopyPositionWithOffset_hook, 0x225a, 0x407d);
  CYC(0x407d, 0x407e); push_effect(gb, HL);
  CYC(0x407e, 0x4080); A = 0x50;
  CALL_ROM(0x4080, 0x0c98);
  CYC(0x4083, 0x4084); SET_HL(pop_effect(gb));
  CYC(0x4084, 0x4085); ret_effect(gb);
}

void objectCreateFloatingImage_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4085, getFreeInteractionSlot_hook, 0x3aef, 0x4088);
  if (!(F & FZ)) {
    CYCT(0x4088, 0x4089); ret_effect(gb);
    return;
  }
  CYC(0x4088, 0x4089);
  CYC(0x4089, 0x408b); mem_wr(gb, HL, 0xa0);
  CYC(0x408b, 0x408c); L = alu_inc8(gb, L);
  CYC(0x408c, 0x408e); A = H8(0xff8d);
  CYC(0x408e, 0x408f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x408f, 0x4091); A = H8(0xff8b);
  CYC(0x4091, 0x4092); mem_wr(gb, HL, A);
  CYC(0x4092, 0x4095); objectCopyPositionWithOffset_hook(gb);
}
