#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

void loseTreasure_helper_hook(GB *gb);

void checkTreasureObtained_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x446d, 0x446e); A = L;
  CYC(0x446e, 0x4470); alu_cp(gb, 0x60);
  if (!(F & FC)) {
    CYCT(0x4470, 0x4472);
    CYC(0x4494, 0x4496); alu_and(gb, 0x07);
    CYC(0x4496, 0x4499); SET_HL(wUpgradesObtained);
    CALL_C(0x4499, checkFlag_hook, 0x0205, 0x449c);
    if (!(F & FZ)) {
      CYCT(0x449c, 0x449e);
      CYC(0x4491, 0x4493); H = 0x01;
      CYC(0x4493, 0x4494); ret_effect(gb); return;
    }
    CYC(0x449c, 0x449e);
    CYC(0x449e, 0x44a0); H = 0x00;
    CYC(0x44a0, 0x44a1); ret_effect(gb); return;
  }
  CYC(0x4470, 0x4472);
  CYC(0x4472, 0x4474); H8(hFF8B) = A;
  CYC(0x4474, 0x4477); SET_HL(wObtainedTreasureFlags);
  CALL_C(0x4477, checkFlag_hook, 0x0205, 0x447a);
  if (F & FZ) {
    CYCT(0x447a, 0x447c);
    CYC(0x449e, 0x44a0); H = 0x00;
    CYC(0x44a0, 0x44a1); ret_effect(gb); return;
  }
  CYC(0x447a, 0x447c);
  CYC(0x447c, 0x447d); push_effect(gb, BC);
  CYC(0x447d, 0x447f); A = H8(hFF8B);
  CYC(0x447f, 0x4480); C = A;
  CYC(0x4480, 0x4482); B = 0x00;
  CYC(0x4482, 0x4485); SET_HL(0x6c09);
  CYC(0x4485, 0x4486); alu_add_hl(gb, BC);
  CYC(0x4486, 0x4487); alu_add_hl(gb, BC);
  CYC(0x4487, 0x4488); alu_add_hl(gb, BC);
  CYC(0x4488, 0x4489); SET_BC(pop_effect(gb));
  CYC(0x4489, 0x448a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x448a, 0x448b); L = A;
  CYC(0x448b, 0x448c); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x448c, 0x448e);
  } else {
    CYC(0x448c, 0x448e);
    CYC(0x448e, 0x4490); H = 0xc6;
    CYC(0x4490, 0x4491); L = mem_rd(gb, HL);
  }
  CYC(0x4491, 0x4493); H = 0x01;
  CYC(0x4493, 0x4494); ret_effect(gb);
}

void loseTreasure_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x44a1, 0x44a2); push_effect(gb, HL);
  CYC(0x44a2, 0x44a3); A = B;
  CALL_C(0x44a3, loseTreasure_helper_hook, 0x44a8, 0x44a6);
  CYC(0x44a6, 0x44a7); SET_HL(pop_effect(gb));
  CYC(0x44a7, 0x44a8); ret_effect(gb);
}

void loseTreasure_helper_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x44a8, 0x44a9); B = A;
  CYC(0x44a9, 0x44ac); SET_HL(wObtainedTreasureFlags);
  CALL_C(0x44ac, unsetFlag_hook, 0x0218, 0x44af);
  CYC(0x44af, 0x44b0); A = B;
  CYC(0x44b0, 0x44b2); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(0x44b2, 0x44b3); ret_effect(gb); return;
  }
  CYC(0x44b2, 0x44b3);
  CYC(0x44b3, 0x44b6); SET_HL(wInventoryB);
  CYC(0x44b6, 0x44b8); B = 0x12;
  for (;;) {
    CYC(0x44b8, 0x44b9); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(0x44b9, 0x44bb);
      break;
    }
    CYC(0x44b9, 0x44bb);
    CYC(0x44bb, 0x44bc); L = alu_inc8(gb, L);
    CYC(0x44bc, 0x44bd); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(0x44bd, 0x44bf);
      CYC(0x44bf, 0x44c0); ret_effect(gb); return;
    }
    CYCT(0x44bd, 0x44bf);
  }
  CYC(0x44c0, 0x44c2); mem_wr(gb, HL, 0x00);
  CYC(0x44c2, 0x44c5); SET_HL(wStatusBarNeedsRefresh);
  CYC(0x44c5, 0x44c7); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x44c7, 0x44c8); ret_effect(gb);
}

void getNumUnappraisedRings_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;

  CYC(0x4697, 0x4698); push_effect(gb, DE);
  CYC(0x4698, 0x469b); SET_HL(wUnappraisedRings);
  CYC(0x469b, 0x469e); SET_DE(0x4000);
  do {
    CYC(0x469e, 0x469f); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x469f, 0x46a1); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYCT(0x46a1, 0x46a3);
    } else {
      CYC(0x46a1, 0x46a3);
      CYC(0x46a3, 0x46a4); E = alu_inc8(gb, E);
    }
    CYC(0x46a4, 0x46a5); D = alu_dec8(gb, D);
    if (!(F & FZ)) CYCT(0x46a5, 0x46a7);
    else CYC(0x46a5, 0x46a7);
  } while (!(F & FZ));
  CYC(0x46a7, 0x46a8); push_effect(gb, BC);
  CYC(0x46a8, 0x46a9); A = E;
  CALL_C(0x46a9, hexToDec_hook, 0x0259, 0x46ac);
  CYC(0x46ac, 0x46ae); C = alu_swap(gb, C);
  CYC(0x46ae, 0x46af); alu_or(gb, C);
  CYC(0x46af, 0x46b2); W8(wNumUnappraisedRingsBcd) = A;
  CYC(0x46b2, 0x46b3); SET_BC(pop_effect(gb));
  CYC(0x46b3, 0x46b4); B = E;
  CYC(0x46b4, 0x46b5); SET_DE(pop_effect(gb));
  CYC(0x46b5, 0x46b6); ret_effect(gb);
}
