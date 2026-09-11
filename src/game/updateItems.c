#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

static uint16_t item_post_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0004, false); alu_add(gb, L); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else { burn_rom(gb, 0x00, 0x0004, 0x0007, false); H = alu_inc8(gb, H); }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  L = A;
  burn_rom(gb, 0x00, 0x0009, 0x000b, false);
  return HL;
}

static uint16_t item_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0004, false); alu_add(gb, L); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else { burn_rom(gb, 0x00, 0x0004, 0x0007, false); H = alu_inc8(gb, H); }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  L = A;
  burn_rom(gb, 0x00, 0x0009, 0x000b, false);
  return HL;
}

static void update_item_hook(GB *gb) {
  CYC(0x48be, 0x48c0); E = 0x01;
  CYC(0x48c0, 0x48c1); A = mem_rd(gb, DE);
  CYC(0x48c1, 0x48c2); push_effect(gb, 0x48c2);
  hook_handoff(gb, item_jump_table(gb));
}

void updateItems_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4872, 0x4874); B = 0;
  CYC(0x4874, 0x4877); A = mem_rd(gb, 0xcd00);
  CYC(0x4877, 0x4879); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(0x4879, 0x487b); goto dont_update; }
  CYC(0x4879, 0x487b);
  CYC(0x487b, 0x487e); A = mem_rd(gb, 0xcc8a);
  CYC(0x487e, 0x4880); alu_and(gb, 0x90);
  if (!(F & FZ)) { CYCT(0x4880, 0x4882); goto dont_update; }
  CYC(0x4880, 0x4882);
  CYC(0x4882, 0x4885); A = mem_rd(gb, 0xc4ab);
  CYC(0x4885, 0x4886); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4886, 0x4888); goto dont_update; }
  CYC(0x4886, 0x4888);
  CYC(0x4888, 0x488b); A = mem_rd(gb, 0xcba0);
  CYC(0x488b, 0x488c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x488c, 0x488e); goto setup; }
  CYC(0x488c, 0x488e);
dont_update:
  CYC(0x488e, 0x488f); B = alu_inc8(gb, B);
setup:
  CYC(0x488f, 0x4892); SET_HL(0xcc8b);
  CYC(0x4892, 0x4893); A = mem_rd(gb, HL);
  CYC(0x4893, 0x4895); alu_and(gb, 0xfe);
  CYC(0x4895, 0x4896); alu_or(gb, B);
  CYC(0x4896, 0x4897); mem_wr(gb, HL, A);
  CYC(0x4897, 0x4898); alu_xor(gb, A);
  CYC(0x4898, 0x489b); mem_wr(gb, 0xccd9, A);
  CYC(0x489b, 0x489d); A = 0;
  CYC(0x489d, 0x489f); H8(hActiveObjectType) = A;
  CYC(0x489f, 0x48a1); D = 0xd6;
  CYC(0x48a1, 0x48a2); A = D;
loop:
  CYC(0x48a2, 0x48a4); H8(hActiveObject) = A;
  CYC(0x48a4, 0x48a6); E = 0;
  CYC(0x48a6, 0x48a7); A = mem_rd(gb, DE);
  CYC(0x48a7, 0x48a8); alu_or(gb, A);
  if (F & FZ) { CYCT(0x48a8, 0x48aa); goto next; }
  CYC(0x48a8, 0x48aa);
  CYC(0x48aa, 0x48ac); E = 0x04;
  CYC(0x48ac, 0x48ad); A = mem_rd(gb, DE);
  CYC(0x48ad, 0x48ae); alu_or(gb, A);
  if (F & FZ) { CYCT(0x48ae, 0x48b0); goto update; }
  CYC(0x48ae, 0x48b0);
  CYC(0x48b0, 0x48b3); A = mem_rd(gb, 0xcc8b);
  CYC(0x48b3, 0x48b4); alu_or(gb, A);
update:
  if (F & FZ) CALL_C_CC(0x48b4, update_item_hook, 0x48be, 0x48b7);
  else CYC(0x48b4, 0x48b7);
next:
  CYC(0x48b7, 0x48b8); D = alu_inc8(gb, D);
  CYC(0x48b8, 0x48b9); A = D;
  CYC(0x48b9, 0x48bb); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(0x48bb, 0x48bd); goto loop; }
  CYC(0x48bb, 0x48bd);
  CYC(0x48bd, 0x48be);
  ret_effect(gb);
}

void itemCodeNilPost_hook(GB *gb) {
  CYC(0x492f, 0x4930);
  ret_effect(gb);
}

void updateItemPost_hook(GB *gb) {
  CYC(0x4930, 0x4932); E = 0x01;
  CYC(0x4932, 0x4933); A = mem_rd(gb, DE);
  CYC(0x4933, 0x4934); push_effect(gb, 0x4934);
  hook_handoff(gb, item_post_jump_table(gb));
}

void updateItemsPost_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x491a, 0x491b); alu_xor(gb, A);
  CYC(0x491b, 0x491d); H8(hActiveObjectType) = A;
  CYC(0x491d, 0x491f); D = 0xd6;
  CYC(0x491f, 0x4920); A = D;
loop:
  CYC(0x4920, 0x4922); H8(hActiveObject) = A;
  CYC(0x4922, 0x4924); E = 0;
  CYC(0x4924, 0x4925); A = mem_rd(gb, DE);
  CYC(0x4925, 0x4926); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(0x4926, updateItemPost_hook, 0x4930, 0x4929);
  else CYC(0x4926, 0x4929);
  CYC(0x4929, 0x492a); D = alu_inc8(gb, D);
  CYC(0x492a, 0x492b); A = D;
  CYC(0x492b, 0x492d); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(0x492d, 0x492f); goto loop; }
  CYC(0x492d, 0x492f);
  itemCodeNilPost_hook(gb);
}
