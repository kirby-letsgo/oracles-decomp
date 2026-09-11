#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x02, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x02, (from), (to), true)

static uint16_t function_caller_jump_table(GB *gb) {
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

static void swap_de_hl_memory(GB *gb) {
  CYC(0x7856, 0x7857); A = mem_rd(gb, DE);
  CYC(0x7857, 0x7858); C = A;
  CYC(0x7858, 0x7859); A = mem_rd(gb, HL);
  CYC(0x7859, 0x785a); mem_wr(gb, HL, C);
  CYC(0x785a, 0x785b); mem_wr(gb, DE, A);
  CYC(0x785b, 0x785c); ret_effect(gb);
}

void functionCaller_b02_hook(GB *gb) {
  CYC(0x77b2, 0x77b3); C = L;
  CYC(0x77b3, 0x77b4); A = H;
  CYC(0x77b4, 0x77b5); push_effect(gb, 0x77b5);
  hook_handoff(gb, function_caller_jump_table(gb));
}

void addRoomToEnemiesKilledList_b02_hook(GB *gb) {
  CYC(0x77c3, 0x77c6); SET_HL(0xcdc0);
  CYC(0x77c6, 0x77c9); A = mem_rd(gb, 0xcc30);
  CYC(0x77c9, 0x77cb); B = 0x08;
  for (;;) {
    CYC(0x77cb, 0x77cc); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x77cc, 0x77ce); break; }
    CYC(0x77cc, 0x77ce);
    CYC(0x77ce, 0x77cf); L = alu_inc8(gb, L);
    CYC(0x77cf, 0x77d0); L = alu_inc8(gb, L);
    CYC(0x77d0, 0x77d1); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x77d1, 0x77d3); continue; }
    CYC(0x77d1, 0x77d3);
    CYC(0x77d3, 0x77d6); A = mem_rd(gb, 0xcdd0);
    CYC(0x77d6, 0x77d7); B = A;
    CYC(0x77d7, 0x77d8); A = alu_inc8(gb, A);
    CYC(0x77d8, 0x77d9); A = alu_inc8(gb, A);
    CYC(0x77d9, 0x77db); alu_and(gb, 0x0f);
    CYC(0x77db, 0x77de); mem_wr(gb, 0xcdd0, A);
    CYC(0x77de, 0x77df); A = B;
    CYC(0x77df, 0x77e1); alu_and(gb, 0x0f);
    CYC(0x77e1, 0x77e3); alu_add(gb, 0xc0);
    CYC(0x77e3, 0x77e4); L = A;
    CYC(0x77e4, 0x77e7); A = mem_rd(gb, 0xcc30);
    CYC(0x77e7, 0x77e8); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x77e8, 0x77e9); alu_xor(gb, A);
    CYC(0x77e9, 0x77ea); mem_wr(gb, HL, A);
    CYC(0x77ea, 0x77ed); mem_wr(gb, 0xcec9, A);
    CYC(0x77ed, 0x77ee); ret_effect(gb);
    return;
  }
  CYC(0x77ee, 0x77ef); L = alu_inc8(gb, L);
  CYC(0x77ef, 0x77f0); A = mem_rd(gb, HL);
  CYC(0x77f0, 0x77f3); mem_wr(gb, 0xcec9, A);
  CYC(0x77f3, 0x77f4); ret_effect(gb);
}

void markEnemyAsKilledInRoom_b02_hook(GB *gb) {
  CYC(0x77f5, 0x77f8); SET_HL(0xcdc0);
  CYC(0x77f8, 0x77fa); B = 0x08;
  CYC(0x77fa, 0x77fd); A = mem_rd(gb, 0xcc30);
  for (;;) {
    CYC(0x77fd, 0x77fe); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(0x77fe, 0x7800); break; }
    CYC(0x77fe, 0x7800);
    CYC(0x7800, 0x7801); L = alu_inc8(gb, L);
    CYC(0x7801, 0x7802); L = alu_inc8(gb, L);
    CYC(0x7802, 0x7803); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x7803, 0x7805); continue; }
    CYC(0x7803, 0x7805);
    CYC(0x7805, 0x7806); ret_effect(gb);
    return;
  }
  CYC(0x7806, 0x7807); L = alu_inc8(gb, L);
  CYC(0x7807, 0x7809); E = 0x80;
  CYC(0x7809, 0x780a); A = mem_rd(gb, DE);
  CYC(0x780a, 0x780c); alu_and(gb, 0x70);
  CYC(0x780c, 0x780e); A = alu_swap(gb, A);
  CYC(0x780e, 0x7811); SET_BC(0x00f8);
  CYC(0x7811, 0x7812); alu_add(gb, C);
  CYC(0x7812, 0x7813); C = A;
  CYC(0x7813, 0x7814); A = mem_rd(gb, BC);
  CYC(0x7814, 0x7815); alu_or(gb, mem_rd(gb, HL));
  CYC(0x7815, 0x7816); mem_wr(gb, HL, A);
  CYC(0x7816, 0x7817); ret_effect(gb);
}

void clearEnemiesKilledList_b02_hook(GB *gb) {
  CYC(0x7817, 0x7818); alu_xor(gb, A);
  CYC(0x7818, 0x781b); mem_wr(gb, 0xcdd0, A);
  CYC(0x781b, 0x781e); SET_HL(0xcdc0);
  CYC(0x781e, 0x7820); B = 0x10;
  CYC(0x7820, 0x7823); clearMemory_hook(gb);
}

void generateRandomBuffer_b02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7823, 0x7824); push_effect(gb, DE);
  CYC(0x7824, 0x7826); A = 0x04;
  CYC(0x7826, 0x7828); mem_wr(gb, 0xff70, A);
  CYC(0x7828, 0x782b); SET_HL(0xd000);
  CYC(0x782b, 0x782d); B = 0;
  do {
    CYC(0x782d, 0x782e); A = B;
    CYC(0x782e, 0x782f); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x782f, 0x7830); B = alu_inc8(gb, B);
    if (!(F & FZ)) CYCT(0x7830, 0x7832); else CYC(0x7830, 0x7832);
  } while (!(F & FZ));
  CYC(0x7832, 0x7835); SET_HL(0xd0ff);
  CYC(0x7835, 0x7836); D = H;
  CALL_C(0x7836, getRandomNumber_hook, 0x043e, 0x7839);
  CYC(0x7839, 0x783a); E = A;
  CYC(0x783a, 0x783d); push_effect(gb, 0x783d); swap_de_hl_memory(gb);
  CYC(0x783d, 0x783f); B = 0xff;
  do {
    CALL_C(0x783f, getRandomNumber_hook, 0x043e, 0x7842);
    CYC(0x7842, 0x7843); C = L;
    CALL_C(0x7843, multiplyAByC_hook, 0x019d, 0x7846);
    CYC(0x7846, 0x7847); E = H;
    CYC(0x7847, 0x7848); L = C;
    CYC(0x7848, 0x784a); H = 0xd0;
    CYC(0x784a, 0x784d); push_effect(gb, 0x784d); swap_de_hl_memory(gb);
    CYC(0x784d, 0x784e); L = alu_dec8(gb, L);
    if (!(F & FZ)) CYCT(0x784e, 0x7850); else CYC(0x784e, 0x7850);
  } while (!(F & FZ));
  CYC(0x7850, 0x7852); A = 0x01;
  CYC(0x7852, 0x7854); mem_wr(gb, 0xff70, A);
  CYC(0x7854, 0x7855); SET_DE(pop_effect(gb));
  CYC(0x7855, 0x7856); ret_effect(gb);
}

void getRandomPositionForEnemy_b02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x785c, 0x785e); A = 0x40;
  CYC(0x785e, 0x7861); mem_wr(gb, 0xcecf, A);
  for (;;) {
    CYC(0x7861, 0x7864); SET_HL(0xcecf);
    CYC(0x7864, 0x7865); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (F & FZ) { CYCT(0x7865, 0x7867); CYC(0x7880, 0x7881); alu_scf(gb); CYC(0x7881, 0x7882); ret_effect(gb); return; }
    CYC(0x7865, 0x7867);
    CALL_C(0x7867, getCandidatePositionForEnemy, 0x796b, 0x786a);
    CYC(0x786a, 0x786d); mem_wr(gb, 0xcec2, A);
    CYC(0x786d, 0x786e); C = A;
    CALL_C(0x786e, checkPositionValidForEnemySpawn, 0x7894, 0x7871);
    if (F & FC) { CYCT(0x7871, 0x7873); continue; }
    CYC(0x7871, 0x7873);
    CYC(0x7873, 0x7875); A = mem_rd(gb, 0xff8b);
    CYC(0x7875, 0x7877); alu_and(gb, 0x04);
    if (F & FZ) {
      CYC(0x7877, 0x7879);
      CALL_C(0x7879, checkTileValidForEnemySpawn, 0x7882, 0x787c);
      if (F & FC) { CYCT(0x787c, 0x787e); continue; }
      CYC(0x787c, 0x787e);
    } else CYCT(0x7877, 0x7879);
    CYC(0x787e, 0x787f); alu_xor(gb, A);
    CYC(0x787f, 0x7880); ret_effect(gb);
    return;
  }
}

void checkSpawnTimeportalInteraction_b02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x79be, 0x79bf); alu_xor(gb, A);
  CYC(0x79bf, 0x79c2); mem_wr(gb, 0xcddd, A);
  CYC(0x79c2, 0x79c5); SET_HL(0xc63e);
  CYC(0x79c5, 0x79c8); A = mem_rd(gb, 0xcc2d);
  CYC(0x79c8, 0x79c9); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x79c9, 0x79ca); ret_effect(gb); return; }
  CYC(0x79c9, 0x79ca);
  CYC(0x79ca, 0x79cb); L = alu_inc8(gb, L);
  CYC(0x79cb, 0x79ce); A = mem_rd(gb, 0xcc30);
  CYC(0x79ce, 0x79cf); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x79cf, 0x79d0); ret_effect(gb); return; }
  CYC(0x79cf, 0x79d0);
  CYC(0x79d0, 0x79d1); L = alu_inc8(gb, L);
  CYC(0x79d1, 0x79d2); C = mem_rd(gb, HL);
  CALL_C(0x79d2, getFreeInteractionSlot_hook, 0x3aef, 0x79d5);
  if (!(F & FZ)) { CYCT(0x79d5, 0x79d6); ret_effect(gb); return; }
  CYC(0x79d5, 0x79d6);
  CYC(0x79d6, 0x79d8); mem_wr(gb, HL, 0xde);
  CYC(0x79d8, 0x79da); A = 0x01;
  CYC(0x79da, 0x79dd); mem_wr(gb, 0xcddd, A);
  CYC(0x79dd, 0x79df); L = 0x4b;
  CYC(0x79df, 0x79e2); setShortPosition_paramC_hook(gb);
}
