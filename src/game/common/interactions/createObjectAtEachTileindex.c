#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

void interactionCodec7__ret_hook(GB *gb) {
  CYC(0x494d, 0x494e); SET_BC(pop_effect(gb));
  CYC(0x494e, 0x494f); SET_HL(pop_effect(gb));
  CYC(0x494f, 0x4950); ret_effect(gb);
}

void interactionCodec7__spawnObjectType_hook(GB *gb) {
  CYC(0x4950, 0x4951); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4951, 0x4954); getFreeEnemySlot_hook(gb);
    return;
  }
  CYC(0x4951, 0x4954);
  CYC(0x4954, 0x4955); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x4955, 0x4958); getFreePartSlot_hook(gb);
    return;
  }
  CYC(0x4955, 0x4958);
  CYC(0x4958, 0x4959); A = alu_dec8(gb, A);
  if (F & FZ) {
    CYCT(0x4959, 0x495c); getFreeInteractionSlot_hook(gb);
    return;
  }
  CYC(0x4959, 0x495c);
  CYC(0x495c, 0x495d); ret_effect(gb);
}

void interactionCodec7__createObject_hook(GB *gb) {
  CYC(0x4921, 0x4922); push_effect(gb, HL);
  CYC(0x4922, 0x4923); push_effect(gb, BC);
  CYC(0x4923, 0x4924); B = L;
  CYC(0x4924, 0x4926); E = 0x4d;
  CYC(0x4926, 0x4927); A = mem_rd(gb, DE);
  CYC(0x4927, 0x4929); alu_and(gb, 0xf0);
  CYC(0x4929, 0x492b); A = alu_swap(gb, A);
  CYC(0x492b, 0x492e); push_effect(gb, 0x492e); interactionCodec7__spawnObjectType_hook(gb);
  if (!(F & FZ)) {
    CYCT(0x492e, 0x4930); interactionCodec7__ret_hook(gb);
    return;
  }
  CYC(0x492e, 0x4930);
  CYC(0x4930, 0x4932); E = 0x4b;
  CYC(0x4932, 0x4933); A = mem_rd(gb, DE);
  CYC(0x4933, 0x4934); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4934, 0x4936); E = 0x4d;
  CYC(0x4936, 0x4937); A = mem_rd(gb, DE);
  CYC(0x4937, 0x4939); alu_and(gb, 0x0f);
  CYC(0x4939, 0x493a); mem_wr(gb, HL, A);
  CYC(0x493a, 0x493b); A = L;
  CYC(0x493b, 0x493d); alu_add(gb, 0x09);
  CYC(0x493d, 0x493e); L = A;
  CYC(0x493e, 0x493f); A = B;
  CYC(0x493f, 0x4941); alu_and(gb, 0xf0);
  CYC(0x4941, 0x4943); alu_add(gb, 0x08);
  CYC(0x4943, 0x4944); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4944, 0x4945); L = alu_inc8(gb, L);
  CYC(0x4945, 0x4946); A = B;
  CYC(0x4946, 0x4948); alu_and(gb, 0x0f);
  CYC(0x4948, 0x494a); A = alu_swap(gb, A);
  CYC(0x494a, 0x494c); alu_add(gb, 0x08);
  CYC(0x494c, 0x494d); mem_wr(gb, HL, A);
  interactionCodec7__ret_hook(gb);
}

void interactionCodec7_hook(GB *gb) {
  CYC(0x490c, 0x490e); E = 0x42;
  CYC(0x490e, 0x490f); A = mem_rd(gb, DE);
  CYC(0x490f, 0x4910); C = A;
  CYC(0x4910, 0x4913); SET_HL(wRoomLayout);
  CYC(0x4913, 0x4915); B = 0xb0;
loop:
  CYC(0x4915, 0x4916); A = mem_rd(gb, HL);
  CYC(0x4916, 0x4917); alu_cp(gb, C);
  if (F & FZ) {
    CYCT(0x4917, 0x491a); push_effect(gb, 0x491a); interactionCodec7__createObject_hook(gb);
  } else CYC(0x4917, 0x491a);
  CYC(0x491a, 0x491b); L = alu_inc8(gb, L);
  CYC(0x491b, 0x491c); B = alu_dec8(gb, B);
  if (!(F & FZ)) {
    CYCT(0x491c, 0x491e); goto loop;
  }
  CYC(0x491c, 0x491e);
  CYC(0x491e, 0x4921); interactionDelete_hook(gb);
}
