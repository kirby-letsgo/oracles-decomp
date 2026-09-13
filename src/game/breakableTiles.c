#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

#define breakableTileCollisionTable_bank06 0x7818
#define breakableTileModes_bank06 0x78dd

void makeInteractionForBreakableTile_hook(GB *gb);

static void breakable_tiles_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void breakable_tiles_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void tryToBreakTile_deleteSomariaBlock(GB *gb) {
  CYC(0x47fe, 0x4800); L = 0x04;
  CYC(0x4800, 0x4801); A = mem_rd(gb, HL);
  CYC(0x4801, 0x4803); alu_cp(gb, 0x03);
  if (!(F & FZ)) {
    CYCT(0x4803, 0x4804); ret_effect(gb); return;
  }
  CYC(0x4803, 0x4804);
  CYC(0x4804, 0x4806); L = 0x2f;
  CYC(0x4806, 0x4808); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 5));
  CYC(0x4808, 0x4809); ret_effect(gb);
}

void tryToBreakTile_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4734, 0x4735); A = B;
  CYC(0x4735, 0x4737); alu_and(gb, 0xf0);
  CYC(0x4737, 0x4739); alu_or(gb, 0x08);
  CYC(0x4739, 0x473b); H8(hFF90) = A;
  CYC(0x473b, 0x473c); A = C;
  CYC(0x473c, 0x473e); alu_and(gb, 0xf0);
  CYC(0x473e, 0x4740); alu_or(gb, 0x08);
  CYC(0x4740, 0x4742); H8(hFF91) = A;
  CALL_C(0x4742, getTileAtPosition_hook, 0x1447, 0x4745);
  CYC(0x4745, 0x4747); H8(hFF92) = A;
  CYC(0x4747, 0x4748); E = A;
  CYC(0x4748, 0x4749); A = L;
  CYC(0x4749, 0x474b); H8(hFF93) = A;
  CYC(0x474b, 0x474e); SET_HL(breakableTileCollisionTable_bank06);
  CALL_C(0x474e, lookupCollisionTable_paramE_hook, 0x1e20, 0x4751);
  if (!(F & FC)) {
    CYCT(0x4751, 0x4752); ret_effect(gb); return;
  }
  CYC(0x4751, 0x4752);
  CYC(0x4752, 0x4753); E = A;
  CYC(0x4753, 0x4754); alu_add(gb, A);
  CYC(0x4754, 0x4757); SET_HL(breakableTileModes_bank06);
  CYC(0x4757, 0x4758); breakable_tiles_add_double_index(gb, 0x4758);
  CYC(0x4758, 0x4759); A = E;
  CYC(0x4759, 0x475a); push_effect(gb, 0x475a); breakable_tiles_add_a_to_hl(gb);
  CYC(0x475a, 0x475c); A = H8(hFF8F);
  CYC(0x475c, 0x475d); E = A;
  CYC(0x475d, 0x475f); alu_and(gb, 0x1f);
  CALL_C(0x475f, checkFlag_hook, 0x0205, 0x4762);
  if (F & FZ) {
    CYCT(0x4762, 0x4763); ret_effect(gb); return;
  }
  CYC(0x4762, 0x4763);
  CYC(0x4763, 0x4765); E = alu_rl(gb, E);
  if (F & FC) {
    CYCT(0x4765, 0x4766); ret_effect(gb); return;
  }
  CYC(0x4765, 0x4766);
  CYC(0x4766, 0x4767); SET_HL(HL + 1);
  CYC(0x4767, 0x4768); SET_HL(HL + 1);
  CYC(0x4768, 0x4769); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4769, 0x476b); A = alu_swap(gb, A);
  CYC(0x476b, 0x476d); alu_and(gb, 0x0f);
  CYC(0x476d, 0x476f); H8(hFF8D) = A;
  CYC(0x476f, 0x4770); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4770, 0x4772); H8(hFF8E) = A;
  CYC(0x4772, 0x4773); push_effect(gb, DE);
  CYC(0x4773, 0x4774); A = mem_rd(gb, HL);
  CYC(0x4774, 0x4775); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4775, 0x4777);
    goto doneSettingTile;
  }
  CYC(0x4775, 0x4777);
  CYC(0x4777, 0x4779); A = H8(hFF92);
  CYC(0x4779, 0x477b); alu_cp(gb, 0xdb);
  if (F & FZ) {
    CYCT(0x477b, 0x477d);
    goto useOriginalLayout;
  }
  CYC(0x477b, 0x477d);
  CYC(0x477d, 0x4780); A = W8(wActiveCollisions);
  CYC(0x4780, 0x4782); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x4782, 0x4784);
    goto activeCollisions1Or2;
  }
  CYC(0x4782, 0x4784);
  CYC(0x4784, 0x4786); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    CYCT(0x4786, 0x4788);
    goto useGivenValue;
  }
  CYC(0x4786, 0x4788);
activeCollisions1Or2:
  CYC(0x4788, 0x478a); A = H8(hFF92);
  CYC(0x478a, 0x478c); alu_cp(gb, 0x10);
  if (!(F & FZ)) {
    CYCT(0x478c, 0x478e);
    goto useGivenValue;
  }
  CYC(0x478c, 0x478e);
useOriginalLayout:
  CYC(0x478e, 0x4790); A = H8(hFF93);
  CYC(0x4790, 0x4791); push_effect(gb, HL);
  CALL_C(0x4791, getTileIndexFromRoomLayoutBuffer_hook, 0x15d7, 0x4794);
  CYC(0x4794, 0x4795); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(0x4795, 0x4797);
    goto setTile;
  }
  CYC(0x4795, 0x4797);
useGivenValue:
  CYC(0x4797, 0x4799); A = H8(hFF93);
  CYC(0x4799, 0x479a); C = A;
  CYC(0x479a, 0x479b); B = mem_rd(gb, HL);
  CALL_C(0x479b, setTileInRoomLayoutBuffer_hook, 0x1426, 0x479e);
  CYC(0x479e, 0x479f); A = mem_rd(gb, HL);
setTile:
  CALL_C(0x479f, setTile_hook, 0x3a9c, 0x47a2);
doneSettingTile:
  CYC(0x47a2, 0x47a4); A = H8(hFF92);
  CYC(0x47a4, 0x47a6); alu_cp(gb, 0xda);
  if (F & FZ) {
    CYCT(0x47a6, 0x47a8);
    goto somariaBlock;
  }
  CYC(0x47a6, 0x47a8);
  CYC(0x47a8, 0x47aa); alu_cp(gb, 0xf2);
  CYC(0x47aa, 0x47ad); SET_HL(wTotalSignsDestroyed);
  if (F & FZ) CALL_C_CC(0x47ad, incHlRefWithCap_hook, 0x0245, 0x47b0);
  else CYC(0x47ad, 0x47b0);
  CYC(0x47b0, 0x47b2); A = H8(hFF8E);
  CYC(0x47b2, 0x47b3); alu_rlca(gb);
  CYC(0x47b3, 0x47b5); A = H8(hFF92);
  if (F & FC) CALL_C_CC(0x47b5, updateRoomFlagsForBrokenTile_hook, 0x1151, 0x47b8);
  else CYC(0x47b5, 0x47b8);
  CYC(0x47b8, 0x47ba); A = H8(hFF8E);
  CYC(0x47ba, 0x47bc); alu_bit(gb, 6, A);
  CYC(0x47bc, 0x47be); A = 0x4d;
  if (!(F & FZ)) CALL_C_CC(0x47be, playSound_b00_hook, 0x0c98, 0x47c1);
  else CYC(0x47be, 0x47c1);
  CYC(0x47c1, 0x47c4); SET_HL(wccaa);
  CYC(0x47c4, 0x47c6); A = H8(hFF93);
  CYC(0x47c6, 0x47c7); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x47c7, 0x47c9);
    CYC(0x47cd, 0x47cf); A = H8(hFF8D);
    CYC(0x47cf, 0x47d0); alu_or(gb, A);
    if (!(F & FZ)) CALL_C_CC(0x47d0, decideItemDropForBrokenTile_hook, 0x483d, 0x47d3);
    else CYC(0x47d0, 0x47d3);
  } else {
    CYC(0x47c7, 0x47c9);
    CYC(0x47c9, 0x47cb); mem_wr(gb, HL, 0xff);
    CYC(0x47cb, 0x47cd);
  }
  CYC(0x47d3, 0x47d5); A = H8(hFF8F);
  CYC(0x47d5, 0x47d6); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x47d6, 0x47d8);
    goto done;
  }
  CYC(0x47d6, 0x47d8);
  CYC(0x47d8, 0x47da); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYCT(0x47da, 0x47dc);
    goto done;
  }
  CYC(0x47da, 0x47dc);
  CYC(0x47dc, 0x47de); alu_cp(gb, 0x08);
  if (F & FZ) {
    CYCT(0x47de, 0x47e0);
    goto done;
  }
  CYC(0x47de, 0x47e0);
  CYC(0x47e0, 0x47e2); alu_cp(gb, 0x12);
  CYC(0x47e2, 0x47e4); A = H8(hFF8E);
  if (!(F & FZ)) CALL_C_CC(0x47e4, makeInteractionForBreakableTile_hook, 0x4816, 0x47e7);
  else CYC(0x47e4, 0x47e7);
done:
  CYC(0x47e7, 0x47e8); SET_DE(pop_effect(gb));
  CYC(0x47e8, 0x47e9); alu_scf(gb);
  CYC(0x47e9, 0x47ea); ret_effect(gb); return;
somariaBlock:
  CYC(0x47ea, 0x47ec); C = 0x18;
  CALL_C(0x47ec, findItemWithID_hook, 0x22b9, 0x47ef);
  if (!(F & FZ)) {
    CYCT(0x47ef, 0x47f1);
    goto done;
  }
  CYC(0x47ef, 0x47f1);
  CYC(0x47f1, 0x47f4); push_effect(gb, 0x47f4); tryToBreakTile_deleteSomariaBlock(gb);
  CALL_C(0x47f4, findItemWithID_startingAfterH_hook, 0x22c0, 0x47f7);
  if (!(F & FZ)) {
    CYCT(0x47f7, 0x47f9);
    goto done;
  }
  CYC(0x47f7, 0x47f9);
  CYC(0x47f9, 0x47fc); push_effect(gb, 0x47fc); tryToBreakTile_deleteSomariaBlock(gb);
  CYC(0x47fc, 0x47fe);
  goto done;
}

void itemMakeInteractionForBreakableTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4809, 0x480a); H = D;
  CYC(0x480a, 0x480c); L = 0x0b;
  CYC(0x480c, 0x480d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x480d, 0x480f); H8(hFF90) = A;
  CYC(0x480f, 0x4810); L = alu_inc8(gb, L);
  CYC(0x4810, 0x4811); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4811, 0x4813); H8(hFF91) = A;
  CYC(0x4813, 0x4815); L = 0x03;
  CYC(0x4815, 0x4816); A = mem_rd(gb, HL);
  makeInteractionForBreakableTile_hook(gb);
}

void makeInteractionForBreakableTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4816, 0x4818); alu_and(gb, 0x1f);
  CYC(0x4818, 0x481a); alu_cp(gb, 0x1f);
  if (F & FZ) {
    CYCT(0x481a, 0x481b); ret_effect(gb); return;
  }
  CYC(0x481a, 0x481b);
  CYC(0x481b, 0x481c); C = A;
  CALL_C(0x481c, getFreeInteractionSlot_hook, 0x3aef, 0x481f);
  if (!(F & FZ)) {
    CYCT(0x481f, 0x4820); ret_effect(gb); return;
  }
  CYC(0x481f, 0x4820);
  CYC(0x4820, 0x4821); A = C;
  CYC(0x4821, 0x4823); alu_and(gb, 0x0f);
  CYC(0x4823, 0x4824); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4824, 0x4825); A = C;
  CYC(0x4825, 0x4827); alu_and(gb, 0x10);
  CYC(0x4827, 0x4829); A = alu_swap(gb, A);
  CYC(0x4829, 0x482a); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x482a, 0x482d); A = W8(w1Link_direction);
  CYC(0x482d, 0x482f); L = 0x48;
  CYC(0x482f, 0x4830); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4830, 0x4832); A = alu_swap(gb, A);
  CYC(0x4832, 0x4833); alu_rrca(gb);
  CYC(0x4833, 0x4834); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4834, 0x4835); L = alu_inc8(gb, L);
  CYC(0x4835, 0x4837); A = H8(hFF90);
  CYC(0x4837, 0x4838); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4838, 0x4839); L = alu_inc8(gb, L);
  CYC(0x4839, 0x483b); A = H8(hFF91);
  CYC(0x483b, 0x483c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x483c, 0x483d); ret_effect(gb);
}

void decideItemDropForBrokenTile_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x483d, 0x483e); push_effect(gb, HL);
  CALL_C(0x483e, decideItemDrop_hook, 0x16eb, 0x4841);
  if (F & FZ) {
    CYCT(0x4841, 0x4843);
    goto done;
  }
  CYC(0x4841, 0x4843);
  CALL_C(0x4843, getFreePartSlot_hook, 0x3e8e, 0x4846);
  if (!(F & FZ)) {
    CYCT(0x4846, 0x4848);
    goto done;
  }
  CYC(0x4846, 0x4848);
  CYC(0x4848, 0x484a); mem_wr(gb, HL, 0x01);
  CYC(0x484a, 0x484b); L = alu_inc8(gb, L);
  CYC(0x484b, 0x484c); mem_wr(gb, HL, C);
  CYC(0x484c, 0x484e); L = 0xcb;
  CYC(0x484e, 0x4850); A = H8(hFF90);
  CYC(0x4850, 0x4851); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4851, 0x4852); L = alu_inc8(gb, L);
  CYC(0x4852, 0x4854); A = H8(hFF91);
  CYC(0x4854, 0x4855); mem_wr(gb, HL, A);
  CYC(0x4855, 0x4858); A = W8(w1Link_direction);
  CYC(0x4858, 0x485a); A = alu_swap(gb, A);
  CYC(0x485a, 0x485b); alu_rrca(gb);
  CYC(0x485b, 0x485d); L = 0xc9;
  CYC(0x485d, 0x485e); mem_wr(gb, HL, A);
  CYC(0x485e, 0x4860); L = 0xc3;
  CYC(0x4860, 0x4861); A = C;
  CYC(0x4861, 0x4863); alu_cp(gb, 0x0f);
  if (!(F & FZ)) {
    CYCT(0x4863, 0x4865);
  } else {
    CYC(0x4863, 0x4865);
    CYC(0x4865, 0x4867); mem_wr(gb, HL, 0x02);
  }
  CYC(0x4867, 0x4869); A = H8(hFF8F);
  CYC(0x4869, 0x486b); alu_cp(gb, 0x06);
  if (!(F & FZ)) {
    CYCT(0x486b, 0x486d);
  } else {
    CYC(0x486b, 0x486d);
    CYC(0x486d, 0x486e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  }
done:
  CYC(0x486e, 0x486f); SET_HL(pop_effect(gb));
  CYC(0x486f, 0x4870); ret_effect(gb);
}
