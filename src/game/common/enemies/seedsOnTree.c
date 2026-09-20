#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void enemyCode5a_addSeed_hook(GB *gb);

static void seedsOnTree_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// ENEMY_SEEDS_ON_TREE
//
// Variables:
//   var03: Child "PART_SEED_ON_TREE" objects write here when Link touches them?
// ==================================================================================================
void enemyCode5a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6893, 0x6895); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6895, 0x6896); A = mem_rd(gb, DE);
  CYC(0x6896, 0x6897); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6897, 0x6899); goto state1; } // jr nz
  CYC(0x6897, 0x6899);

  CYC(0x6899, 0x689b); A = 0x01;
  CYC(0x689b, 0x689c); mem_wr(gb, DE, A);
  CYC(0x689c, 0x689e); A = 0x6e; // TILEINDEX_MYSTICAL_TREE_TL
  CALL_C(0x689e, findTileInRoom_hook, 0x15cc, 0x68a1);
  if (!(F & FZ)) { CYCT(0x68a1, 0x68a4); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x68a1, 0x68a4);
  CYC(0x68a4, 0x68a5); C = L;
  CYC(0x68a5, 0x68a6); H = D;
  CYC(0x68a6, 0x68a8); L = ENEMY_BASE + OBJ_YH;
  CALL_C(0x68a8, setShortPosition_paramC_hook, 0x20b9, 0x68ab);
  CYC(0x68ab, 0x68ae); SET_BC(0x0808);
  CALL_C(0x68ae, objectCopyPositionWithOffset_hook, 0x225a, 0x68b1);
  CYC(0x68b1, 0x68b3); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x68b3, 0x68b4); A = mem_rd(gb, DE);
  CYC(0x68b4, 0x68b6); alu_and(gb, 0x0f);
  CYC(0x68b6, 0x68b9); SET_HL(wSeedTreeRefilledBitset);
  CALL_C(0x68b9, checkFlag_hook, 0x0205, 0x68bc);
  if (F & FZ) { CYCT(0x68bc, 0x68bf); interactionDelete_hook(gb); return; } // jp z
  CYC(0x68bc, 0x68bf);
  CYC(0x68bf, 0x68c0); A = mem_rd(gb, DE);
  CYC(0x68c0, 0x68c2); A = alu_swap(gb, A);
  CYC(0x68c2, 0x68c4); alu_and(gb, 0x0f);
  CYC(0x68c4, 0x68c6); hram_wr(gb, (uint8_t)hFF8B, A);
  CYC(0x68c6, 0x68c7); alu_xor(gb, A);
  CALL_C(0x68c7, enemyCode5a_addSeed_hook, 0x68d1, 0x68ca);
  CYC(0x68ca, 0x68cc); A = 0x01;
  CALL_C(0x68cc, enemyCode5a_addSeed_hook, 0x68d1, 0x68cf);
  CYC(0x68cf, 0x68d1); A = 0x02;
  enemyCode5a_addSeed_hook(gb); return; // fallthrough

state1:
  CYC(0x68fc, 0x68fe); E = ENEMY_BASE + OBJ_VAR03;
  CYC(0x68fe, 0x68ff); A = mem_rd(gb, DE);
  CYC(0x68ff, 0x6900); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6900); return; } // ret z
  CYC(0x6900, 0x6901);
  CYC(0x6901, 0x6903); E = ENEMY_BASE + OBJ_SUBID;
  CYC(0x6903, 0x6904); A = mem_rd(gb, DE);
  CYC(0x6904, 0x6906); alu_and(gb, 0x0f);
  CYC(0x6906, 0x6909); SET_HL(wSeedTreeRefilledBitset);
  CALL_C(0x6909, unsetFlag_hook, 0x0218, 0x690c);
  CYC(0x690c, 0x690f); enemyDelete_hook(gb); return; // jp
}

// 0e:68d1, bare local (no exported symbol); called twice via genuine call/ret from
// enemyCode5a, also reached the third time by fallthrough.
// @param a Seed index (0-2)
void enemyCode5a_addSeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x68d1, 0x68d4); SET_HL(0x68f6); // @seedOffsets
  CYC(0x68d4, 0x68d5); seedsOnTree_addDoubleIndexToHl_from_rst(gb, 0x68d5);
  CYC(0x68d5, 0x68d7); E = ENEMY_BASE + OBJ_YH;
  CYC(0x68d7, 0x68d8); A = mem_rd(gb, DE);
  CYC(0x68d8, 0x68d9); alu_add(gb, mem_rd(gb, HL));
  CYC(0x68d9, 0x68da); SET_HL(HL + 1); // inc hl
  CYC(0x68da, 0x68db); B = A;
  CYC(0x68db, 0x68dd); E = ENEMY_BASE + OBJ_XH;
  CYC(0x68dd, 0x68de); A = mem_rd(gb, DE);
  CYC(0x68de, 0x68df); alu_add(gb, mem_rd(gb, HL));
  CYC(0x68df, 0x68e0); C = A;
  CALL_C(0x68e0, getFreePartSlot_hook, 0x3e8e, 0x68e3);
  CYC(0x68e3, 0x68e5); mem_wr(gb, HL, 0x10); // PART_SEED_ON_TREE
  CYC(0x68e5, 0x68e6); L = alu_inc8(gb, L);
  CYC(0x68e6, 0x68e8); A = hram_rd(gb, (uint8_t)hFF8B);
  CYC(0x68e8, 0x68e9); mem_wr(gb, HL, A);
  CYC(0x68e9, 0x68eb); L = PART_BASE + OBJ_YH;
  CYC(0x68eb, 0x68ec); mem_wr(gb, HL, B);
  CYC(0x68ec, 0x68ee); L = PART_BASE + OBJ_XH;
  CYC(0x68ee, 0x68ef); mem_wr(gb, HL, C);
  CYC(0x68ef, 0x68f1); L = PART_BASE + OBJ_RELATED2;
  CYC(0x68f1, 0x68f3); mem_wr(gb, HL, PART_BASE); // Part.start
  CYC(0x68f3, 0x68f4); L = alu_inc8(gb, L);
  CYC(0x68f4, 0x68f5); mem_wr(gb, HL, D);
  RET(0x68f5); return; // ret
}
