#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

void specialObjectGetTileAtOffset_hook(GB *gb);

static void add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void resetPushingAgainstTileCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x420c, 0x420e); A = 0x14;
  CYC(0x420e, 0x4211); W8(wPushingAgainstTileCounter) = A;
  CYC(0x4211, 0x4212); ret_effect(gb);
}

void decPushingAgainstTileCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4212, 0x4215); SET_HL(wPushingAgainstTileCounter);
  CYC(0x4215, 0x4216); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x4216, 0x4217); ret_effect(gb);
}

void specialObjectGetTileInFront_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4373, 0x4375); E = 0x08;
  CYC(0x4375, 0x4376); A = mem_rd(gb, DE);
  CYC(0x4376, 0x4379); SET_HL(0x4390);
  CYC(0x4379, 0x437a); add_double_index_to_hl_from_rst(gb, 0x437a);
  specialObjectGetTileAtOffset_hook(gb);
}

void specialObjectGetTileAtOffset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x437a, 0x437c); E = 0x0b;
  CYC(0x437c, 0x437d); A = mem_rd(gb, DE);
  CYC(0x437d, 0x437e); alu_add(gb, mem_rd(gb, HL));
  CYC(0x437e, 0x4380); alu_and(gb, 0xf0);
  CYC(0x4380, 0x4381); C = A;
  CYC(0x4381, 0x4382); SET_HL(HL + 1);
  CYC(0x4382, 0x4384); E = 0x0d;
  CYC(0x4384, 0x4385); A = mem_rd(gb, DE);
  CYC(0x4385, 0x4386); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4386, 0x4388); A = alu_swap(gb, A);
  CYC(0x4388, 0x438a); alu_and(gb, 0x0f);
  CYC(0x438a, 0x438b); alu_or(gb, C);
  CYC(0x438b, 0x438c); C = A;
  CYC(0x438c, 0x438e); B = 0xcf;
  CYC(0x438e, 0x438f); A = mem_rd(gb, BC);
  CYC(0x438f, 0x4390); ret_effect(gb);
}

void checkTileAfterNext_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4398, 0x439b); A = W8(wLinkPushingDirection);
  CYC(0x439b, 0x439e); SET_HL(0x43aa);
  CYC(0x439e, 0x439f); add_double_index_to_hl_from_rst(gb, 0x439f);
  CALL_C(0x439f, specialObjectGetTileAtOffset_hook, 0x437a, 0x43a2);
  CYC(0x43a2, 0x43a4); B = 0xce;
  CYC(0x43a4, 0x43a5); A = mem_rd(gb, BC);
  CYC(0x43a5, 0x43a7); alu_and(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x43a7, 0x43a8); ret_effect(gb); return;
  }
  CYC(0x43a7, 0x43a8);
  CYC(0x43a8, 0x43a9); alu_scf(gb);
  CYC(0x43a9, 0x43aa); ret_effect(gb);
}
