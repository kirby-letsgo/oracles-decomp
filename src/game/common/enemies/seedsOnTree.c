#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode5a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto state1; } // jr nz
  CYC(b_+4, b_+6);

  CYC(b_+6, b_+8); A = 0x01;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+11); A = 0x6e; // TILEINDEX_MYSTICAL_TREE_TL
  CALL_C(b_+11, findTileInRoom_hook, SYM(findTileInRoom), b_+14);
  if (!(F & FZ)) { CYCT(b_+14, b_+17); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+14, b_+17);
  CYC(b_+17, b_+18); C = L;
  CYC(b_+18, b_+19); H = D;
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_YH;
  CALL_C(b_+21, setShortPosition_paramC_hook, SYM(setShortPosition_paramC), b_+24);
  CYC(b_+24, b_+27); SET_BC(0x0808);
  CALL_C(b_+27, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+30);
  CYC(b_+30, b_+32); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+35); alu_and(gb, 0x0f);
  CYC(b_+35, b_+38); SET_HL(wSeedTreeRefilledBitset);
  CALL_C(b_+38, checkFlag_hook, SYM(checkFlag), b_+41);
  if (F & FZ) { CYCT(b_+41, b_+44); interactionDelete_hook(gb); return; } // jp z
  CYC(b_+41, b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+47); A = alu_swap(gb, A);
  CYC(b_+47, b_+49); alu_and(gb, 0x0f);
  CYC(b_+49, b_+51); hram_wr(gb, (uint8_t)hFF8B, A);
  CYC(b_+51, b_+52); alu_xor(gb, A);
  CALL_L(b_+52, enemyCode5a_addSeed_hook, b_+55);
  CYC(b_+55, b_+57); A = 0x01;
  CALL_L(b_+57, enemyCode5a_addSeed_hook, b_+60);
  CYC(b_+60, b_+62); A = 0x02;
  enemyCode5a_addSeed_hook(gb); return; // fallthrough

state1:
  CYC(b_+105, b_+107); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  CYC(b_+108, b_+109); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+109); return; } // ret z
  CYC(b_+109, b_+110);
  CYC(b_+110, b_+112); E = ENEMY_BASE + OBJ_SUBID;
  CYC(b_+112, b_+113); A = mem_rd(gb, DE);
  CYC(b_+113, b_+115); alu_and(gb, 0x0f);
  CYC(b_+115, b_+118); SET_HL(wSeedTreeRefilledBitset);
  CALL_C(b_+118, unsetFlag_hook, SYM(unsetFlag), b_+121);
  CYC(b_+121, b_+124); TAIL(enemyDelete); // jp
}

// 0e:68d1, bare local (no exported symbol); called twice via genuine call/ret from
// enemyCode5a, also reached the third time by fallthrough.
// @param a Seed index (0-2)
void enemyCode5a_addSeed_hook(GB *gb) {
  BASE(enemyCode5a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+62, b_+65); SET_HL(b_+99); // @seedOffsets
  CYC(b_+65, b_+66); seedsOnTree_addDoubleIndexToHl_from_rst(gb, b_+66);
  CYC(b_+66, b_+68); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+70); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+70, b_+71); SET_HL(HL + 1); // inc hl
  CYC(b_+71, b_+72); B = A;
  CYC(b_+72, b_+74); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+74, b_+75); A = mem_rd(gb, DE);
  CYC(b_+75, b_+76); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+76, b_+77); C = A;
  CALL_C(b_+77, getFreePartSlot_hook, SYM(getFreePartSlot), b_+80);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x10); // PART_SEED_ON_TREE
  CYC(b_+82, b_+83); L = alu_inc8(gb, L);
  CYC(b_+83, b_+85); A = hram_rd(gb, (uint8_t)hFF8B);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A);
  CYC(b_+86, b_+88); L = PART_BASE + OBJ_YH;
  CYC(b_+88, b_+89); mem_wr(gb, HL, B);
  CYC(b_+89, b_+91); L = PART_BASE + OBJ_XH;
  CYC(b_+91, b_+92); mem_wr(gb, HL, C);
  CYC(b_+92, b_+94); L = PART_BASE + OBJ_RELATED2;
  CYC(b_+94, b_+96); mem_wr(gb, HL, PART_BASE); // Part.start
  CYC(b_+96, b_+97); L = alu_inc8(gb, L);
  CYC(b_+97, b_+98); mem_wr(gb, HL, D);
  RET(b_+98); return; // ret
}
