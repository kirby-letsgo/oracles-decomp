#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void enemyCode5a_addSeed_hook(GB *gb);

static void seedsOnTree_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

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
  CYC(b_+O(0), b_+OE(2)); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+O(2), b_+OE(3)); A = mem_rd(gb, DE);
  CYC(b_+O(3), b_+OE(4)); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+O(4), b_+OE(6)); goto state1; } // jr nz
  CYC(b_+O(4), b_+OE(6));

  CYC(b_+O(6), b_+OE(8)); A = 0x01;
  CYC(b_+O(8), b_+OE(9)); mem_wr(gb, DE, A);
  if (!game_seasons) {
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
  } else {
    CYC(b_+S(9), b_+S(11)); E = ENEMY_BASE + OBJ_SUBID;
    CYC(b_+S(11), b_+S(12)); A = mem_rd(gb, DE);
    CYC(b_+S(12), b_+S(13)); B = A;
    CYC(b_+S(13), b_+S(14)); alu_add(gb, A);
    CYC(b_+S(14), b_+S(15)); alu_add(gb, B);
    CYC(b_+S(15), b_+S(18)); SET_HL(b_+S(97)); // @treeDataTable
    CYC(b_+S(18), b_+S(19)); seedsOnTree_addAToHl_from_rst(gb, b_+S(19));
    CYC(b_+S(19), b_+S(20)); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+S(20), b_+S(22)); hram_wr(gb, (uint8_t)hFF8B, A);
    CYC(b_+S(22), b_+S(23)); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+S(23), b_+S(24)); B = A;
    CYC(b_+S(24), b_+S(27)); A = W8(wRoomStateModifier);
    CYC(b_+S(27), b_+S(28)); alu_cp(gb, B);
    if (!(F & FZ)) { CYCT(b_+S(28), b_+S(31)); TAIL(enemyDelete); }
    CYC(b_+S(28), b_+S(31));
    CYC(b_+S(31), b_+S(32)); A = mem_rd(gb, HL);
    CYC(b_+S(32), b_+S(33)); alu_cpl(gb);
    CYC(b_+S(33), b_+S(35)); E = ENEMY_BASE + OBJ_DIRECTION;
    CYC(b_+S(35), b_+S(36)); mem_wr(gb, DE, A);
    CYC(b_+S(36), b_+S(39)); A = W8(wSeedTreeRefilledBitset);
    CYC(b_+S(39), b_+S(40)); alu_and(gb, mem_rd(gb, HL));
    if (F & FZ) { CYCT(b_+S(40), b_+S(43)); TAIL(enemyDelete); }
    CYC(b_+S(40), b_+S(43));
  }
  CYC(b_+O(51), b_+OE(52)); alu_xor(gb, A);
  CALL_L(b_+O(52), enemyCode5a_addSeed_hook, b_+OE(55));
  CYC(b_+O(55), b_+OE(57)); A = 0x01;
  CALL_L(b_+O(57), enemyCode5a_addSeed_hook, b_+OE(60));
  CYC(b_+O(60), b_+OE(62)); A = 0x02;
  enemyCode5a_addSeed_hook(gb); return; // fallthrough

state1:
  CYC(b_+O(105), b_+OE(107)); E = ENEMY_BASE + OBJ_VAR03;
  CYC(b_+O(107), b_+OE(108)); A = mem_rd(gb, DE);
  CYC(b_+O(108), b_+OE(109)); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+O(109)); return; } // ret z
  CYC(b_+O(109), b_+OE(110));
  if (!game_seasons) {
    CYC(b_+110, b_+112); E = ENEMY_BASE + OBJ_SUBID;
    CYC(b_+112, b_+113); A = mem_rd(gb, DE);
    CYC(b_+113, b_+115); alu_and(gb, 0x0f);
    CYC(b_+115, b_+118); SET_HL(wSeedTreeRefilledBitset);
    CALL_C(b_+118, unsetFlag_hook, SYM(unsetFlag), b_+121);
  } else {
    CYC(b_+S(120), b_+S(122)); E = ENEMY_BASE + OBJ_DIRECTION;
    CYC(b_+S(122), b_+S(123)); A = mem_rd(gb, DE);
    CYC(b_+S(123), b_+S(126)); SET_HL(wSeedTreeRefilledBitset);
    CYC(b_+S(126), b_+S(127)); alu_and(gb, mem_rd(gb, HL));
    CYC(b_+S(127), b_+S(128)); mem_wr(gb, HL, A);
  }
  CYC(b_+O(121), b_+OE(124)); TAIL(enemyDelete); // jp
}

// 0e:68d1, bare local (no exported symbol); called twice via genuine call/ret from
// enemyCode5a, also reached the third time by fallthrough.
// @param a Seed index (0-2)
void enemyCode5a_addSeed_hook(GB *gb) {
  BASE(enemyCode5a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(62), b_+OE(65)); SET_HL(b_+O(99)); // @seedOffsets
  CYC(b_+O(65), b_+OE(66)); seedsOnTree_addDoubleIndexToHl_from_rst(gb, b_+OE(66));
  CYC(b_+O(66), b_+OE(68)); E = ENEMY_BASE + OBJ_YH;
  CYC(b_+O(68), b_+OE(69)); A = mem_rd(gb, DE);
  CYC(b_+O(69), b_+OE(70)); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+O(70), b_+OE(71)); SET_HL(HL + 1); // inc hl
  CYC(b_+O(71), b_+OE(72)); B = A;
  CYC(b_+O(72), b_+OE(74)); E = ENEMY_BASE + OBJ_XH;
  CYC(b_+O(74), b_+OE(75)); A = mem_rd(gb, DE);
  CYC(b_+O(75), b_+OE(76)); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+O(76), b_+OE(77)); C = A;
  CALL_C(b_+O(77), getFreePartSlot_hook, SYM(getFreePartSlot), b_+OE(80));
  CYC(b_+O(80), b_+OE(82)); mem_wr(gb, HL, 0x10); // PART_SEED_ON_TREE
  CYC(b_+O(82), b_+OE(83)); L = alu_inc8(gb, L);
  CYC(b_+O(83), b_+OE(85)); A = hram_rd(gb, (uint8_t)hFF8B);
  CYC(b_+O(85), b_+OE(86)); mem_wr(gb, HL, A);
  CYC(b_+O(86), b_+OE(88)); L = PART_BASE + OBJ_YH;
  CYC(b_+O(88), b_+OE(89)); mem_wr(gb, HL, B);
  CYC(b_+O(89), b_+OE(91)); L = PART_BASE + OBJ_XH;
  CYC(b_+O(91), b_+OE(92)); mem_wr(gb, HL, C);
  CYC(b_+O(92), b_+OE(94)); L = PART_BASE + OBJ_RELATED2;
  CYC(b_+O(94), b_+OE(96)); mem_wr(gb, HL, PART_BASE); // Part.start
  CYC(b_+O(96), b_+OE(97)); L = alu_inc8(gb, L);
  CYC(b_+O(97), b_+OE(98)); mem_wr(gb, HL, D);
  RET(b_+O(98)); return; // ret
}
