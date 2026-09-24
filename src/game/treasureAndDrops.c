#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void loseTreasure_helper_hook(GB *gb);
void realignUnappraisedRings__findNextFilledSlot_hook(GB *gb);
void addTreasureToInventory__addToInventory_hook(GB *gb);
void loadTreasureDisplayData__getTableIndices_b3f_hook(GB *gb);
void checkItemDropAvailable_body_hook(GB *gb);
void giveTreasure_body__findItemInTable_hook(GB *gb);
void giveTreasure_body__giveTreasure_hook(GB *gb);
void giveTreasure_body__applyParameter_hook(GB *gb);
void giveTreasure_body__modee_hook(GB *gb);
void giveTreasure_body__modef_hook(GB *gb);
void giveTreasure_body__mode9_hook(GB *gb);
void giveTreasure_body__removeOneDuplicateRing_hook(GB *gb);

static void treasure_add_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

static void treasure_add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static uint16_t treasure_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void giveTreasure_body_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(1)); push_effect(gb, HL);
  CYC(b_+O(1), b_+OE(2)); push_effect(gb, DE);
  CYC(b_+O(2), b_+OE(3)); A = B;
  CYC(b_+O(3), b_+OE(5)); H8(hFF8B) = A;
  CYC(b_+O(5), b_+OE(6)); push_effect(gb, BC);
  CYC(b_+O(6), b_+OE(9)); SET_HL(GV(b_+O(127), 0x4544));
  CALL_C(b_+O(9), giveTreasure_body__findItemInTable_hook, b_+O(40), b_+OE(12));
  if (F & FZ) {
    CYCT(b_+O(12), b_+OE(14));
  } else {
    CYC(b_+O(12), b_+OE(14));
    CALL_C(b_+O(14), loseTreasure_helper_hook, SYM(loseTreasure_helper), b_+OE(17));
    CYC(b_+O(17), b_+OE(18)); A = C;
    CALL_C(b_+O(18), loseTreasure_helper_hook, SYM(loseTreasure_helper), b_+OE(21));
  }
  CYC(b_+O(21), b_+OE(22)); SET_BC(pop_effect(gb));
  CYC(b_+O(22), b_+OE(23)); A = B;
  CALL_C(b_+O(23), giveTreasure_body__giveTreasure_hook, b_+O(57), b_+OE(26));
  CYC(b_+O(26), b_+OE(27)); push_effect(gb, AF);
  CYC(b_+O(27), b_+OE(30)); SET_HL(b_+O(114));
  CALL_C(b_+O(30), giveTreasure_body__findItemInTable_hook, b_+O(40), b_+OE(33));
  if (!(F & FZ)) {
    CALL_C_CC(b_+O(33), giveTreasure_body__giveTreasure_hook, b_+O(57), b_+OE(36));
  } else {
    CYC(b_+O(33), b_+OE(36));
  }
  CYC(b_+O(36), b_+OE(37)); SET_BC(pop_effect(gb));
  CYC(b_+O(37), b_+OE(38)); SET_DE(pop_effect(gb));
  CYC(b_+O(38), b_+OE(39)); SET_HL(pop_effect(gb));
  CYC(b_+O(39), b_+OE(40)); ret_effect(gb);
}

void giveTreasure_body__findItemInTable_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(40), b_+OE(42)); A = H8(hFF8B);
  CYC(b_+O(42), b_+OE(43)); C = A;
  for (;;) {
    CYC(b_+O(43), b_+OE(44)); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+O(44), b_+OE(45)); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(b_+O(45), b_+OE(47));
      break;
    }
    CYC(b_+O(45), b_+OE(47));
    CYC(b_+O(47), b_+OE(48)); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+O(48), b_+OE(49)); ret_effect(gb); return;
    }
    CYC(b_+O(48), b_+OE(49));
    CYC(b_+O(49), b_+OE(50)); SET_HL(HL + 1);
    CYC(b_+O(50), b_+OE(51)); SET_HL(HL + 1);
    CYC(b_+O(51), b_+OE(53));
  }
  CYC(b_+O(53), b_+OE(54)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(54), b_+OE(55)); C = mem_rd(gb, HL);
  CYC(b_+O(55), b_+OE(56)); alu_or(gb, A);
  CYC(b_+O(56), b_+OE(57)); ret_effect(gb);
}

void giveTreasure_body__giveTreasure_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(57), b_+OE(59)); H8(hFF8B) = A;
  CALL_C(b_+O(59), checkIncreaseGashaMaturityForGettingTreasure_hook, SYM(checkIncreaseGashaMaturityForGettingTreasure), b_+OE(62));
  CALL_C(b_+O(62), addTreasureToInventory_hook, SYM(addTreasureToInventory), b_+OE(65));
  CYC(b_+O(65), b_+OE(68)); SET_HL(wObtainedTreasureFlags);
  CYC(b_+O(68), b_+OE(70)); A = H8(hFF8B);
  CALL_C(b_+O(70), setFlag_hook, SYM(setFlag), b_+OE(73));
  CYC(b_+O(73), b_+OE(74)); push_effect(gb, BC);
  CYC(b_+O(74), b_+OE(76)); A = H8(hFF8B);
  CYC(b_+O(76), b_+OE(77)); C = A;
  CYC(b_+O(77), b_+OE(79)); B = 0x00;
  CYC(b_+O(79), b_+OE(82)); SET_HL(SYM(treasureCollectionBehaviourTable));
  CYC(b_+O(82), b_+OE(83)); alu_add_hl(gb, BC);
  CYC(b_+O(83), b_+OE(84)); alu_add_hl(gb, BC);
  CYC(b_+O(84), b_+OE(85)); alu_add_hl(gb, BC);
  CYC(b_+O(85), b_+OE(86)); SET_BC(pop_effect(gb));
  CYC(b_+O(86), b_+OE(88)); D = 0xc6;
  CYC(b_+O(88), b_+OE(89)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(89), b_+OE(90)); E = A;
  CYC(b_+O(90), b_+OE(91)); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(91), b_+OE(93));
  } else {
    CYC(b_+O(91), b_+OE(93));
    CYC(b_+O(93), b_+OE(95)); E = (uint8_t)wShortSecretIndex;
  }
  CYC(b_+O(95), b_+OE(96)); A = mem_rd(gb, HL);
  CYC(b_+O(96), b_+OE(98)); alu_and(gb, 0x0f);
  CYC(b_+O(98), b_+OE(99)); push_effect(gb, HL);
  CALL_C(b_+O(99), giveTreasure_body__applyParameter_hook, SYM(giveTreasure_body__applyParameter), b_+OE(102));
  CYC(b_+O(102), b_+OE(103)); SET_HL(pop_effect(gb));
  CYC(b_+O(103), b_+OE(105)); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+O(105), b_+OE(106)); SET_HL(HL + 1);
  CYC(b_+O(106), b_+OE(107)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FZ)) {
    CYCT(b_+O(107), b_+OE(109));
  } else {
    CYC(b_+O(107), b_+OE(109));
    CALL_C(b_+O(109), playSound_b00_hook, SYM(playSound_b00), b_+OE(112));
    CYC(b_+O(112), b_+OE(113)); alu_xor(gb, A);
  }
  CYC(b_+O(113), b_+OE(114)); ret_effect(gb);
}

void giveTreasure_body__applyParameter_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(128), b_+OE(129)); push_effect(gb, b_+OE(129));
  do { uint16_t jt_ = (treasure_jump_table(gb));
    if (jt_ == b_+O(113)) { CYC(b_+O(113), b_+OE(114)); ret_effect(gb); return; }
    else if (jt_ == b_+O(161)) { giveTreasure_body__modeb_hook(gb); return; }
    else if (jt_ == b_+O(168)) { giveTreasure_body__mode8_hook(gb); return; }
    else if (jt_ == b_+O(179)) { giveTreasure_body__mode5_hook(gb); return; }
    else if (jt_ == b_+O(182)) { giveTreasure_body__mode6_hook(gb); return; }
    else if (jt_ == b_+O(186)) { giveTreasure_body__mode1_hook(gb); return; }
    else if (jt_ == b_+O(192)) { giveTreasure_body__mode2_hook(gb); return; }
    else if (jt_ == b_+O(196)) { giveTreasure_body__mode3_hook(gb); return; }
    else if (jt_ == b_+O(198)) { giveTreasure_body__mode4_hook(gb); return; }
    else if (jt_ == b_+O(207)) { giveTreasure_body__mode7_hook(gb); return; }
    else if (jt_ == b_+O(220)) { giveTreasure_body__modea_hook(gb); return; }
    else if (jt_ == b_+O(224)) { giveTreasure_body__modec_hook(gb); return; }
    else if (jt_ == b_+O(244)) { giveTreasure_body__moded_hook(gb); return; }
    else if (jt_ == b_+O(255)) { giveTreasure_body__modee_hook(gb); return; }
    else if (jt_ == b_+O(310)) { giveTreasure_body__modef_hook(gb); return; }
    else if (jt_ == b_+O(332)) { giveTreasure_body__mode9_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void checkTreasureObtained_body_hook(GB *gb) {
  BASE(checkTreasureObtained_body);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); A = L;
  CYC(b_+1, b_+3); alu_cp(gb, 0x60);
  if (!(F & FC)) {
    CYCT(b_+3, b_+5);
    CYC(b_+39, b_+41); alu_and(gb, 0x07);
    CYC(b_+41, b_+44); SET_HL(wUpgradesObtained);
    CALL_C(b_+44, checkFlag_hook, SYM(checkFlag), b_+47);
    if (!(F & FZ)) {
      CYCT(b_+47, b_+49);
      CYC(b_+36, b_+38); H = 0x01;
      CYC(b_+38, b_+39); ret_effect(gb); return;
    }
    CYC(b_+47, b_+49);
    CYC(b_+49, b_+51); H = 0x00;
    CYC(b_+51, b_+52); ret_effect(gb); return;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); H8(hFF8B) = A;
  CYC(b_+7, b_+10); SET_HL(wObtainedTreasureFlags);
  CALL_C(b_+10, checkFlag_hook, SYM(checkFlag), b_+13);
  if (F & FZ) {
    CYCT(b_+13, b_+15);
    CYC(b_+49, b_+51); H = 0x00;
    CYC(b_+51, b_+52); ret_effect(gb); return;
  }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); push_effect(gb, BC);
  CYC(b_+16, b_+18); A = H8(hFF8B);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+21); B = 0x00;
  CYC(b_+21, b_+24); SET_HL(SYM(treasureCollectionBehaviourTable));
  CYC(b_+24, b_+25); alu_add_hl(gb, BC);
  CYC(b_+25, b_+26); alu_add_hl(gb, BC);
  CYC(b_+26, b_+27); alu_add_hl(gb, BC);
  CYC(b_+27, b_+28); SET_BC(pop_effect(gb));
  CYC(b_+28, b_+29); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+29, b_+30); L = A;
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+31, b_+33);
  } else {
    CYC(b_+31, b_+33);
    CYC(b_+33, b_+35); H = 0xc6;
    CYC(b_+35, b_+36); L = mem_rd(gb, HL);
  }
  CYC(b_+36, b_+38); H = 0x01;
  CYC(b_+38, b_+39); ret_effect(gb);
}

void loseTreasure_body_hook(GB *gb) {
  BASE(loseTreasure_body);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2); A = B;
  CALL_C(b_+2, loseTreasure_helper_hook, SYM(loseTreasure_helper), b_+5);
  CYC(b_+5, b_+6); SET_HL(pop_effect(gb));
  CYC(b_+6, b_+7); ret_effect(gb);
}

void loseTreasure_helper_hook(GB *gb) {
  BASE(loseTreasure_helper);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+4); SET_HL(wObtainedTreasureFlags);
  CALL_C(b_+4, unsetFlag_hook, SYM(unsetFlag), b_+7);
  CYC(b_+7, b_+8); A = B;
  CYC(b_+8, b_+10); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(b_+10, b_+11); ret_effect(gb); return;
  }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); SET_HL(wInventoryB);
  CYC(b_+14, b_+16); B = 0x12;
  for (;;) {
    CYC(b_+16, b_+17); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(b_+17, b_+19);
      break;
    }
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+20); L = alu_inc8(gb, L);
    CYC(b_+20, b_+21); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(b_+21, b_+23);
      CYC(b_+23, b_+24); ret_effect(gb); return;
    }
    CYCT(b_+21, b_+23);
  }
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x00);
  CYC(b_+26, b_+29); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+29, b_+31); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+31, b_+32); ret_effect(gb);
}

void getNumUnappraisedRings_hook(GB *gb) {
  BASE(getNumUnappraisedRings);
  uint16_t sp0_ = gb->sp;

  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+4); SET_HL(wUnappraisedRings);
  CYC(b_+4, b_+7); SET_DE(SYM(initGbaModePaletteData));
  do {
    CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+8, b_+10); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYCT(b_+10, b_+12);
    } else {
      CYC(b_+10, b_+12);
      CYC(b_+12, b_+13); E = alu_inc8(gb, E);
    }
    CYC(b_+13, b_+14); D = alu_dec8(gb, D);
    if (!(F & FZ)) CYCT(b_+14, b_+16);
    else CYC(b_+14, b_+16);
  } while (!(F & FZ));
  CYC(b_+16, b_+17); push_effect(gb, BC);
  CYC(b_+17, b_+18); A = E;
  CALL_C(b_+18, hexToDec_hook, SYM(hexToDec), b_+21);
  CYC(b_+21, b_+23); C = alu_swap(gb, C);
  CYC(b_+23, b_+24); alu_or(gb, C);
  CYC(b_+24, b_+27); W8(wNumUnappraisedRingsBcd) = A;
  CYC(b_+27, b_+28); SET_BC(pop_effect(gb));
  CYC(b_+28, b_+29); B = E;
  CYC(b_+29, b_+30); SET_DE(pop_effect(gb));
  CYC(b_+30, b_+31); ret_effect(gb);
}

void realignUnappraisedRings_hook(GB *gb) {
  BASE(realignUnappraisedRings);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wUnappraisedRings);
  for (;;) {
    CYC(b_+3, b_+4); A = mem_rd(gb, HL);
    CYC(b_+4, b_+6); alu_cp(gb, 0xff);
    if (F & FZ) {
      CYC(b_+6, b_+8);
      CYC(b_+8, b_+9); push_effect(gb, HL);
      CALL_C(b_+9, realignUnappraisedRings__findNextFilledSlot_hook, b_+24, b_+12);
      CYC(b_+12, b_+13); SET_HL(pop_effect(gb));
      if (!(F & FC)) {
        CYCT(b_+13, b_+15);
        break;
      }
      CYC(b_+13, b_+15);
      CYC(b_+15, b_+16); mem_wr(gb, HL, A);
    } else {
      CYCT(b_+6, b_+8);
    }
    CYC(b_+16, b_+17); L = alu_inc8(gb, L);
    CYC(b_+17, b_+18); A = L;
    CYC(b_+18, b_+20); alu_cp(gb, (uint8_t)wUnappraisedRingsEnd);
    if (F & FZ) {
      CYC(b_+20, b_+22);
      break;
    }
    CYCT(b_+20, b_+22);
  }
  CYC(b_+22, b_+24);
  TAIL(getNumUnappraisedRings);
}

void realignUnappraisedRings__findNextFilledSlot_hook(GB *gb) {
  BASE(realignUnappraisedRings);
  for (;;) {
    CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+25, b_+27); alu_cp(gb, 0xff);
    if (!(F & FZ)) {
      CYCT(b_+27, b_+29);
      break;
    }
    CYC(b_+27, b_+29);
    CYC(b_+29, b_+30); A = L;
    CYC(b_+30, b_+32); alu_cp(gb, (uint8_t)wUnappraisedRingsEnd);
    if (F & FZ) {
      CYC(b_+32, b_+34);
      CYC(b_+34, b_+35); ret_effect(gb);
      return;
    }
    CYCT(b_+32, b_+34);
  }
  CYC(b_+35, b_+36); SET_HL(HL - 1);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0xff);
  CYC(b_+38, b_+39); alu_scf(gb);
  CYC(b_+39, b_+40); ret_effect(gb);
}

void addTreasureToInventory_hook(GB *gb) {
  BASE(addTreasureToInventory);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = H8(hFF8B);
  CYC(b_+2, b_+4); alu_cp(gb, 0x20);
  if (!(F & FC)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); push_effect(gb, BC);
  CALL_C(b_+6, addTreasureToInventory__addToInventory_hook, b_+38, b_+9);
  CYC(b_+9, b_+10); SET_BC(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(b_+10, b_+11); ret_effect(gb);
    return;
  }
  CYC(b_+10, b_+11);
  if (F & FZ) {
    CYCT(b_+11, b_+14);
    TAIL(setStatusBarNeedsRefreshBit1);
  }
  CYC(b_+11, b_+14);
  CYC(b_+14, b_+15); push_effect(gb, BC);
  CYC(b_+15, b_+16); alu_cpl(gb);
  CYC(b_+16, b_+18); alu_add(gb, GV(0x88, 0x80));
  CYC(b_+18, b_+19); L = A;
  CYC(b_+19, b_+21); A = H8(hFF8B);
  CYC(b_+21, b_+22); C = A;
  CYC(b_+22, b_+24); alu_cp(gb, 0x0c);
  if (F & FZ) {
    CYC(b_+24, b_+26);
    CYC(b_+26, b_+27); A = mem_rd(gb, HL);
    CYC(b_+27, b_+28); mem_wr(gb, HL, C);
    CALL_C(b_+28, addTreasureToInventory__addToInventory_hook, b_+38, b_+31);
  } else {
    CYCT(b_+24, b_+26);
  }
  CYC(b_+31, b_+34); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+34, b_+36); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+36, b_+37); SET_BC(pop_effect(gb));
  CYC(b_+37, b_+38); ret_effect(gb);
}

void addTreasureToInventory__addToInventory_hook(GB *gb) {
  BASE(addTreasureToInventory);
  CYC(b_+38, b_+39); C = A;
  CYC(b_+39, b_+42); SET_HL(wInventoryB);
  CYC(b_+42, b_+44); B = 0x12;
  for (;;) {
    CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+45, b_+46); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(b_+46, b_+48);
      break;
    }
    CYC(b_+46, b_+48);
    CYC(b_+48, b_+49); B = alu_dec8(gb, B);
    if (F & FZ) {
      CYC(b_+49, b_+51);
      CYC(b_+51, b_+52); B = alu_dec8(gb, B);
      CYC(b_+52, b_+54); L = (uint8_t)wInventoryB;
      do {
        CYC(b_+54, b_+55); A = mem_rd(gb, HL); SET_HL(HL + 1);
        CYC(b_+55, b_+56); alu_or(gb, A);
        if (!(F & FZ)) CYCT(b_+56, b_+58);
        else CYC(b_+56, b_+58);
      } while (!(F & FZ));
      break;
    }
    CYCT(b_+49, b_+51);
  }
  CYC(b_+58, b_+59); L = alu_dec8(gb, L);
  CYC(b_+59, b_+60); mem_wr(gb, HL, C);
  CYC(b_+60, b_+61); A = L;
  CYC(b_+61, b_+63); alu_sub(gb, GV(0x8a, 0x82));
  CYC(b_+63, b_+65); alu_bit(gb, 7, B);
  CYC(b_+65, b_+66); ret_effect(gb);
}

void loadTreasureDisplayData_b3f_hook(GB *gb) {
  BASE(loadTreasureDisplayData_b3f);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = L;
  CYC(b_+1, b_+2); push_effect(gb, DE);
  CALL_C(b_+2, loadTreasureDisplayData__getTableIndices_b3f_hook, b_+51, b_+5);
  CYC(b_+5, b_+6); push_effect(gb, BC);
  CYC(b_+6, b_+9); SET_HL(0x0000);
  CYC(b_+9, b_+10); A = D;
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+11, b_+13);
  } else {
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+14); alu_cpl(gb);
    CYC(b_+14, b_+15); A = alu_inc8(gb, A);
    CYC(b_+15, b_+16); L = A;
    CYC(b_+16, b_+18); H = 0xff;
    CYC(b_+18, b_+19); A = D;
    CALL_C(b_+19, multiplyABy8_hook, SYM(multiplyABy8), b_+22);
    CYC(b_+22, b_+23); alu_add_hl(gb, BC);
  }
  CYC(b_+23, b_+24); push_effect(gb, HL);
  CYC(b_+24, b_+25); A = E;
  CYC(b_+25, b_+28); SET_HL(SYM(treasureDisplayData2));
  CYC(b_+28, b_+29); treasure_add_double_index_to_hl_from_rst(gb, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); H = mem_rd(gb, HL);
  CYC(b_+31, b_+32); L = A;
  CYC(b_+32, b_+33); SET_BC(pop_effect(gb));
  CYC(b_+33, b_+34); alu_add_hl(gb, BC);
  CYC(b_+34, b_+37); SET_DE(wTmpcec0);
  CYC(b_+37, b_+39); B = 0x07;
  do {
    CYC(b_+39, b_+40); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+40, b_+41); mem_wr(gb, DE, A);
    CYC(b_+41, b_+42); E = alu_inc8(gb, E);
    CYC(b_+42, b_+43); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+43, b_+45);
    else CYC(b_+43, b_+45);
  } while (!(F & FZ));
  CYC(b_+45, b_+48); SET_HL(wTmpcec0);
  CYC(b_+48, b_+49); SET_BC(pop_effect(gb));
  CYC(b_+49, b_+50); SET_DE(pop_effect(gb));
  CYC(b_+50, b_+51); ret_effect(gb);
}

void loadTreasureDisplayData__getTableIndices_b3f_hook(GB *gb) {
  BASE(loadTreasureDisplayData_b3f);
  CYC(b_+51, b_+52); D = A;
  CYC(b_+52, b_+55); SET_HL(SYM(treasureDisplayData1));
  for (;;) {
    CYC(b_+55, b_+56); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+56, b_+57); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+57, b_+59);
      break;
    }
    CYC(b_+57, b_+59);
    CYC(b_+59, b_+60); alu_cp(gb, D);
    if (F & FZ) {
      CYCT(b_+60, b_+62);
      break;
    }
    CYC(b_+60, b_+62);
    CYC(b_+62, b_+63); SET_HL(HL + 1);
    CYC(b_+63, b_+64); SET_HL(HL + 1);
    CYC(b_+64, b_+66);
  }
  CYC(b_+66, b_+67); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+67, b_+68); E = mem_rd(gb, HL);
  CYC(b_+68, b_+69); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+69, b_+71);
  } else {
    CYC(b_+69, b_+71);
    CYC(b_+71, b_+72); L = A;
    CYC(b_+72, b_+74); H = 0xc6;
    CYC(b_+74, b_+75); D = mem_rd(gb, HL);
  }
  CYC(b_+75, b_+76); ret_effect(gb);
}

void decideItemDrop_body_hook(GB *gb) {
  BASE(decideItemDrop_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(0), b_+OE(1)); A = C;
  CYC(b_+O(1), b_+OE(2)); alu_or(gb, A);
  CYC(b_+O(2), b_+OE(4)); A |= 0x80;
  if (!(F & FZ)) {
    CYCT(b_+O(4), b_+OE(6));
  } else {
    CYC(b_+O(4), b_+OE(6));
    CYC(b_+O(6), b_+OE(8)); A = H8(hActiveObjectType);
    CYC(b_+O(8), b_+OE(10)); alu_add(gb, 0x02);
    CYC(b_+O(10), b_+OE(11)); E = A;
    CYC(b_+O(11), b_+OE(12)); A = mem_rd(gb, DE);
  }
  CYC(b_+O(12), b_+OE(15)); SET_HL(SYM(itemDropTables));
  CYC(b_+O(15), b_+OE(16)); treasure_add_index_to_hl_from_rst(gb, b_+OE(16));
  CYC(b_+O(16), b_+OE(17)); A = mem_rd(gb, HL);
  CYC(b_+O(17), b_+OE(18)); C = A;
  CYC(b_+O(18), b_+OE(20)); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+O(20), b_+OE(22));
    goto unavailable;
  }
  CYC(b_+O(20), b_+OE(22));
  CYC(b_+O(22), b_+OE(24)); A = alu_swap(gb, A);
  CYC(b_+O(24), b_+OE(25)); alu_rrca(gb);
  CYC(b_+O(25), b_+OE(27)); alu_and(gb, 0x07);
  CYC(b_+O(27), b_+OE(30)); SET_HL(SYM(itemDropProbabilityTable));
  CYC(b_+O(30), b_+OE(31)); treasure_add_double_index_to_hl_from_rst(gb, b_+OE(31));
  CYC(b_+O(31), b_+OE(32)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(32), b_+OE(33)); H = mem_rd(gb, HL);
  CYC(b_+O(33), b_+OE(34)); L = A;
  CALL_C(b_+O(34), getRandomNumber_hook, SYM(getRandomNumber), b_+OE(37));
  CYC(b_+O(37), b_+OE(39)); alu_and(gb, 0x3f);
  CALL_C(b_+O(39), checkFlag_hook, SYM(checkFlag), b_+OE(42));
  if (F & FZ) {
    CYCT(b_+O(42), b_+OE(44));
    goto unavailable;
  }
  CYC(b_+O(42), b_+OE(44));
  CYC(b_+O(44), b_+OE(45)); A = C;
  CYC(b_+O(45), b_+OE(47)); alu_and(gb, 0x1f);
  CYC(b_+O(47), b_+OE(50)); SET_HL(SYM(itemDropSetTable));
  CYC(b_+O(50), b_+OE(51)); treasure_add_double_index_to_hl_from_rst(gb, b_+OE(51));
  CYC(b_+O(51), b_+OE(52)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(52), b_+OE(53)); H = mem_rd(gb, HL);
  CYC(b_+O(53), b_+OE(54)); L = A;
  CALL_C(b_+O(54), getRandomNumber_hook, SYM(getRandomNumber), b_+OE(57));
  CYC(b_+O(57), b_+OE(59)); alu_and(gb, 0x1f);
  CYC(b_+O(59), b_+OE(60)); treasure_add_index_to_hl_from_rst(gb, b_+OE(60));
  CYC(b_+O(60), b_+OE(61)); A = mem_rd(gb, HL);
  CYC(b_+O(61), b_+OE(62)); C = A;
  TAIL(checkItemDropAvailable_body);

unavailable:
  CYC(SYM(checkItemDropAvailable_body__done), (SYM(checkItemDropAvailable_body__done) + 2)); C = 0xff;
  CYC((SYM(checkItemDropAvailable_body__done) + 2), (SYM(checkItemDropAvailable_body__done) + 3)); ret_effect(gb);
}

void checkItemDropAvailable_body_hook(GB *gb) {
  BASE(checkItemDropAvailable_body);
  uint16_t sp0_ = gb->sp;
  if (game_seasons) {
    CYC(b_+S(0), b_+S(3)); A = W8(wMinimapGroup);
    CYC(b_+S(3), b_+S(4)); A = alu_dec8(gb, A);
    CYC(b_+S(4), b_+S(5)); A = C;
    if (!(F & FZ)) CYCT(b_+S(5), b_+S(7));
    else {
      CYC(b_+S(5), b_+S(7));
      CYC(b_+S(7), b_+S(10)); SET_HL(SYM(subrosiaDropSet));
      CYC(b_+S(10), b_+S(11)); treasure_add_index_to_hl_from_rst(gb, b_+S(11));
      CYC(b_+S(11), b_+S(12)); A = mem_rd(gb, HL);
      CYC(b_+S(12), b_+S(13)); C = A;
    }
  } else {
    CYC(b_+0, b_+1); A = C;
  }
  CYC(b_+O(1), b_+OE(4)); SET_HL(SYM(itemDropAvailabilityTable));
  CYC(b_+O(4), b_+OE(5)); treasure_add_double_index_to_hl_from_rst(gb, b_+OE(5));
  CYC(b_+O(5), b_+OE(6)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(6), b_+OE(7)); B = mem_rd(gb, HL);
  CYC(b_+O(7), b_+OE(8)); L = A;
  CYC(b_+O(8), b_+OE(10)); H = 0xc6;
  CYC(b_+O(10), b_+OE(11)); A = mem_rd(gb, HL);
  CYC(b_+O(11), b_+OE(12)); alu_and(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+O(12), b_+OE(13)); ret_effect(gb); return;
  }
  CYC(b_+O(12), b_+OE(13));
  CYC(b_+O(13), b_+OE(15)); C = 0xff;
  CYC(b_+O(15), b_+OE(16)); ret_effect(gb);
}

void giveTreasure_body__modeb_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(161), b_+OE(162)); A = C;
  CYC(b_+O(162), b_+OE(165)); SET_HL(wUpgradesObtained);
  CYC(b_+O(165), b_+OE(168)); TAIL(setFlag);
}

void giveTreasure_body__mode8_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(168), b_+OE(169)); A = mem_rd(gb, DE);
  CYC(b_+O(169), b_+OE(170)); alu_cp(gb, C);
  if (!(F & FC)) {
    CYCT(b_+O(170), b_+OE(171)); ret_effect(gb); return;
  }
  CYC(b_+O(170), b_+OE(171));
  CYC(b_+O(171), b_+OE(172)); A = C;
  CYC(b_+O(172), b_+OE(173)); mem_wr(gb, DE, A);
  CYC(b_+O(173), b_+OE(176)); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+O(176), b_+OE(178)); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+O(178), b_+OE(179)); ret_effect(gb);
}

void giveTreasure_body__mode5_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(179), b_+OE(180)); A = C;
  CYC(b_+O(180), b_+OE(181)); mem_wr(gb, DE, A);
  CYC(b_+O(181), b_+OE(182)); ret_effect(gb);
}

void giveTreasure_body__mode1_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(186), b_+OE(187)); A = C;
  CYC(b_+O(187), b_+OE(188)); H = D;
  CYC(b_+O(188), b_+OE(189)); L = E;
  CYC(b_+O(189), b_+OE(192)); TAIL(setFlag);
}

void giveTreasure_body__mode6_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(182), b_+OE(185)); A = W8(wDungeonIndex);
  CYC(b_+O(185), b_+OE(186)); C = A;
  TAIL(giveTreasure_body__mode1);
}

void giveTreasure_body__mode2_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(192), b_+OE(193)); A = mem_rd(gb, DE);
  CYC(b_+O(193), b_+OE(194)); A = alu_inc8(gb, A);
  CYC(b_+O(194), b_+OE(195)); mem_wr(gb, DE, A);
  CYC(b_+O(195), b_+OE(196)); ret_effect(gb);
}

void giveTreasure_body__mode4_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(198), b_+OE(199)); A = mem_rd(gb, DE);
  CYC(b_+O(199), b_+OE(200)); alu_add(gb, C);
  CYC(b_+O(200), b_+OE(201)); alu_daa(gb);
  if (!(F & FC)) {
    CYCT(b_+O(201), b_+OE(203));
  } else {
    CYC(b_+O(201), b_+OE(203));
    CYC(b_+O(203), b_+OE(205)); A = 0x99;
  }
  CYC(b_+O(205), b_+OE(206)); mem_wr(gb, DE, A);
  CYC(b_+O(206), b_+OE(207)); ret_effect(gb);
}

void giveTreasure_body__mode3_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(196), b_+OE(198)); C = 0x01;
  TAIL(giveTreasure_body__mode4);
}

void giveTreasure_body__mode7_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(207), b_+OE(210)); A = W8(wDungeonIndex);
  CYC(b_+O(210), b_+OE(211)); alu_add(gb, E);
  CYC(b_+O(211), b_+OE(212)); L = A;
  CYC(b_+O(212), b_+OE(213)); H = D;
  CYC(b_+O(213), b_+OE(214)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+O(214), b_+OE(217)); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+O(217), b_+OE(219)); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(b_+O(219), b_+OE(220)); ret_effect(gb);
}

void giveTreasure_body__modea_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(220), b_+OE(221)); A = mem_rd(gb, DE);
  CYC(b_+O(221), b_+OE(222)); alu_add(gb, C);
  CYC(b_+O(222), b_+OE(223)); mem_wr(gb, DE, A);
  CYC(b_+O(223), b_+OE(224)); ret_effect(gb);
}

void giveTreasure_body__modec_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+O(224), b_+OE(225)); H = D;
  CYC(b_+O(225), b_+OE(226)); L = E;
  CYC(b_+O(226), b_+OE(228)); A = (uint8_t)wLinkHealth;
  CYC(b_+O(228), b_+OE(229)); alu_cp(gb, E);
  CYC(b_+O(229), b_+OE(230)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FZ)) {
    CYCT(b_+O(230), b_+OE(232));
  } else {
    CYC(b_+O(230), b_+OE(232));
    CYC(b_+O(232), b_+OE(233)); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYC(b_+O(233), b_+OE(235));
      CYC(b_+O(235), b_+OE(237)); A = 0x57;
      CYC(b_+O(237), b_+OE(240)); playSound_b00_hook(gb);
      return;
    }
    CYCT(b_+O(233), b_+OE(235));
  }
  CYC(b_+O(240), b_+OE(241)); alu_add(gb, C);
  CYC(b_+O(241), b_+OE(242)); mem_wr(gb, DE, A);
  CYC(b_+O(242), b_+OE(244));
  CYC(b_+O(250), b_+OE(251)); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+O(251), b_+OE(252)); ret_effect(gb); return;
  }
  CYC(b_+O(251), b_+OE(252));
  CYC(b_+O(252), b_+OE(253)); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+O(253), b_+OE(254)); mem_wr(gb, HL, A);
  CYC(b_+O(254), b_+OE(255)); ret_effect(gb);
}

void giveTreasure_body__moded_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(244), giveTreasure_body__mode4_hook, b_+O(198), b_+OE(247));
  CYC(b_+O(247), b_+OE(248)); H = D;
  CYC(b_+O(248), b_+OE(249)); L = E;
  CYC(b_+O(249), b_+OE(250)); L = alu_inc8(gb, L);
  CYC(b_+O(250), b_+OE(251)); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+O(251), b_+OE(252)); ret_effect(gb); return;
  }
  CYC(b_+O(251), b_+OE(252));
  CYC(b_+O(252), b_+OE(253)); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+O(253), b_+OE(254)); mem_wr(gb, HL, A);
  CYC(b_+O(254), b_+OE(255)); ret_effect(gb);
}

void giveTreasure_body__modee_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(255), b_+OE(256)); A = C;
  CALL_C(b_+O(256), getRupeeValue_hook, SYM(getRupeeValue), b_+OE(259));
  CYC(b_+O(259), b_+OE(260)); A = E;
  CYC(b_+O(260), b_+OE(262)); alu_cp(gb, (uint8_t)wNumRupees);
  if (!(F & FZ)) {
    CYCT(b_+O(262), b_+OE(264));
  } else {
    CYC(b_+O(262), b_+OE(264));
    CYC(b_+O(264), b_+OE(266)); A = 0x01;
    CALL_C(b_+O(266), checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+OE(269));
    if (!(F & FZ)) {
      CYCT(b_+O(269), b_+OE(271));
    } else {
      CYC(b_+O(269), b_+OE(271));
      CYC(b_+O(271), b_+OE(272)); H = D;
      CYC(b_+O(272), b_+OE(274)); L = (uint8_t)wTotalRupeesCollected;
      CALL_C(b_+O(274), addDecimalToHlRef_hook, SYM(addDecimalToHlRef), b_+OE(277));
      if (!(F & FC)) {
        CYCT(b_+O(277), b_+OE(279));
      } else {
        CYC(b_+O(277), b_+OE(279));
        CYC(b_+O(279), b_+OE(281)); A = 0x01;
        CALL_C(b_+O(281), setGlobalFlag_hook, SYM(setGlobalFlag), b_+OE(284));
      }
    }
  }
  CYC(b_+O(284), b_+OE(285)); H = D;
  CYC(b_+O(285), b_+OE(286)); L = E;
  CALL_C(b_+O(286), addDecimalToHlRef_hook, SYM(addDecimalToHlRef), b_+OE(289));
  CYC(b_+O(289), b_+OE(290)); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+O(290), b_+OE(291)); H = mem_rd(gb, HL);
  CYC(b_+O(291), b_+OE(292)); L = A;
  CYC(b_+O(292), b_+OE(295)); SET_BC(0x0999);
  CALL_C(b_+O(295), compareHlToBc_hook, SYM(compareHlToBc), b_+OE(298));
  CYC(b_+O(298), b_+OE(299)); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+O(299), b_+OE(300)); ret_effect(gb); return;
  }
  CYC(b_+O(299), b_+OE(300));
  CYC(b_+O(300), b_+OE(301)); A = C;
  CYC(b_+O(301), b_+OE(302)); mem_wr(gb, DE, A);
  CYC(b_+O(302), b_+OE(303)); E = alu_inc8(gb, E);
  CYC(b_+O(303), b_+OE(304)); A = B;
  CYC(b_+O(304), b_+OE(305)); mem_wr(gb, DE, A);
  CYC(b_+O(305), b_+OE(307)); A = 0x61;
  CYC(b_+O(307), b_+OE(310)); TAIL(playSound_b00);
}

void giveTreasure_body__modef_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+O(310), giveTreasure_body__mode4_hook, b_+O(198), b_+OE(313));
  CALL_C(b_+O(313), setStatusBarNeedsRefreshBit1_hook, SYM(setStatusBarNeedsRefreshBit1), b_+OE(316));
  CYC(b_+O(316), b_+OE(319)); A = W8(wSeedSatchelLevel);
  CYC(b_+O(319), b_+OE(322)); SET_HL(GV(b_+O(328), 0x4616));
  CYC(b_+O(322), b_+OE(323)); treasure_add_index_to_hl_from_rst(gb, b_+OE(323));
  CYC(b_+O(323), b_+OE(324)); A = mem_rd(gb, DE);
  CYC(b_+O(324), b_+OE(325)); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+O(325), b_+OE(326)); ret_effect(gb); return;
  }
  CYC(b_+O(325), b_+OE(326));
  CYC(b_+O(326), b_+OE(327)); A = mem_rd(gb, HL);
  CYC(b_+O(327), b_+OE(328)); mem_wr(gb, DE, A);
  CYC(b_+O(328), b_+OE(329)); ret_effect(gb);
}

void giveTreasure_body__mode9_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(332), b_+OE(334)); C |= 0x40;
  CALL_C(b_+O(334), realignUnappraisedRings_hook, SYM(realignUnappraisedRings), b_+OE(337));
  CYC(b_+O(337), b_+OE(339)); alu_cp(gb, 0x64);
  if (F & FC) {
    CYCT(b_+O(339), b_+OE(341));
  } else {
    CYC(b_+O(339), b_+OE(341));
    CALL_C(b_+O(341), giveTreasure_body__removeOneDuplicateRing_hook, b_+O(353), b_+OE(344));
    CALL_C(b_+O(344), realignUnappraisedRings_hook, SYM(realignUnappraisedRings), b_+OE(347));
  }
  CYC(b_+O(347), b_+OE(348)); A = C;
  CYC(b_+O(348), b_+OE(351)); mem_wr(gb, wUnappraisedRingsEnd - 1, A);
  CYC(b_+O(351), b_+OE(353)); TAIL(realignUnappraisedRings);
}

void giveTreasure_body__removeOneDuplicateRing_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+O(353), b_+OE(355)); A = hram_rd(gb, R_SVBK);
  CYC(b_+O(355), b_+OE(356)); push_effect(gb, AF);
  CYC(b_+O(356), b_+OE(358)); A = 0x04;
  CYC(b_+O(358), b_+OE(360)); hram_wr(gb, R_SVBK, A);
  CYC(b_+O(360), b_+OE(363)); SET_HL(w4TmpRingBuffer);
  CYC(b_+O(363), b_+OE(365)); B = 0x40;
  CALL_C(b_+O(365), clearMemory_hook, SYM(clearMemory), b_+OE(368));
  CYC(b_+O(368), b_+OE(371)); SET_DE(wUnappraisedRings);
  CYC(b_+O(371), b_+OE(373)); B = 0x40;
  do {
    CYC(b_+O(373), b_+OE(374)); A = mem_rd(gb, DE);
    CYC(b_+O(374), b_+OE(376)); alu_and(gb, 0x3f);
    CYC(b_+O(376), b_+OE(379)); SET_HL(w4TmpRingBuffer);
    CYC(b_+O(379), b_+OE(380)); treasure_add_index_to_hl_from_rst(gb, b_+OE(380));
    CYC(b_+O(380), b_+OE(381)); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+O(381), b_+OE(382)); E = alu_inc8(gb, E);
    CYC(b_+O(382), b_+OE(383)); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+O(383), b_+OE(385));
    else CYC(b_+O(383), b_+OE(385));
  } while (!(F & FZ));
  CYC(b_+O(385), b_+OE(388)); SET_HL(w4TmpRingBuffer);
  CYC(b_+O(388), b_+OE(391)); SET_DE(0x0000);
  CYC(b_+O(391), b_+OE(393)); B = 0x40;
  do {
    CYC(b_+O(393), b_+OE(394)); A = mem_rd(gb, HL);
    CYC(b_+O(394), b_+OE(395)); alu_cp(gb, D);
    if (F & FC) {
      CYCT(b_+O(395), b_+OE(397));
    } else {
      CYC(b_+O(395), b_+OE(397));
      CYC(b_+O(397), b_+OE(398)); D = A;
      CYC(b_+O(398), b_+OE(399)); E = L;
    }
    CYC(b_+O(399), b_+OE(400)); L = alu_inc8(gb, L);
    CYC(b_+O(400), b_+OE(401)); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+O(401), b_+OE(403));
    else CYC(b_+O(401), b_+OE(403));
  } while (!(F & FZ));
  CYC(b_+O(403), b_+OE(404)); A = E;
  CYC(b_+O(404), b_+OE(406)); alu_sub(gb, (uint8_t)w4TmpRingBuffer);
  CYC(b_+O(406), b_+OE(408)); alu_or(gb, 0x40);
  CYC(b_+O(408), b_+OE(409)); E = A;
  CYC(b_+O(409), b_+OE(410)); SET_AF(pop_effect(gb));
  CYC(b_+O(410), b_+OE(412)); hram_wr(gb, R_SVBK, A);
  CYC(b_+O(412), b_+OE(415)); SET_HL(wUnappraisedRingsEnd - 1);
  do {
    CYC(b_+O(415), b_+OE(416)); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+O(416), b_+OE(417)); alu_cp(gb, E);
    if (!(F & FZ)) CYCT(b_+O(417), b_+OE(419));
    else CYC(b_+O(417), b_+OE(419));
  } while (!(F & FZ));
  CYC(b_+O(419), b_+OE(420)); SET_HL(HL + 1);
  CYC(b_+O(420), b_+OE(422)); mem_wr(gb, HL, 0xff);
  CYC(b_+O(422), b_+OE(423)); ret_effect(gb);
}

void checkIncreaseGashaMaturityForGettingTreasure_hook(GB *gb) {
  BASE(checkIncreaseGashaMaturityForGettingTreasure);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); B = A;
  CYC(b_+2, b_+5); SET_HL(b_+23);
  for (;;) {
    CYC(b_+5, b_+6); SET_HL(HL + 1);
    CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+7, b_+8); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+8, b_+10);
      break;
    }
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+11); alu_cp(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+11, b_+13);
      continue;
    }
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+15); alu_cp(gb, 0x29);
    CYC(b_+15, b_+16); A = C;
    if (F & FZ) {
      CYCT(b_+16, b_+18);
    } else {
      CYC(b_+16, b_+18);
      CYC(b_+18, b_+19); A = mem_rd(gb, HL);
    }
    CALL_C(b_+19, addToGashaMaturity_hook, SYM(addToGashaMaturity), b_+22);
    break;
  }
  CYC(b_+22, b_+23); SET_BC(pop_effect(gb));
  CYC(b_+23, b_+24); ret_effect(gb);
}
