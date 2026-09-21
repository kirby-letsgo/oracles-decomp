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
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2); push_effect(gb, DE);
  CYC(b_+2, b_+3); A = B;
  CYC(b_+3, b_+5); H8(hFF8B) = A;
  CYC(b_+5, b_+6); push_effect(gb, BC);
  CYC(b_+6, b_+9); SET_HL(GV(b_+127, 0x4544));
  CALL_C(b_+9, giveTreasure_body__findItemInTable_hook, b_+40, b_+12);
  if (F & FZ) {
    CYCT(b_+12, b_+14);
  } else {
    CYC(b_+12, b_+14);
    CALL_C(b_+14, loseTreasure_helper_hook, SYM(loseTreasure_helper), b_+17);
    CYC(b_+17, b_+18); A = C;
    CALL_C(b_+18, loseTreasure_helper_hook, SYM(loseTreasure_helper), b_+21);
  }
  CYC(b_+21, b_+22); SET_BC(pop_effect(gb));
  CYC(b_+22, b_+23); A = B;
  CALL_C(b_+23, giveTreasure_body__giveTreasure_hook, b_+57, b_+26);
  CYC(b_+26, b_+27); push_effect(gb, AF);
  CYC(b_+27, b_+30); SET_HL(b_+114);
  CALL_C(b_+30, giveTreasure_body__findItemInTable_hook, b_+40, b_+33);
  if (!(F & FZ)) {
    CALL_C_CC(b_+33, giveTreasure_body__giveTreasure_hook, b_+57, b_+36);
  } else {
    CYC(b_+33, b_+36);
  }
  CYC(b_+36, b_+37); SET_BC(pop_effect(gb));
  CYC(b_+37, b_+38); SET_DE(pop_effect(gb));
  CYC(b_+38, b_+39); SET_HL(pop_effect(gb));
  CYC(b_+39, b_+40); ret_effect(gb);
}

void giveTreasure_body__findItemInTable_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+40, b_+42); A = H8(hFF8B);
  CYC(b_+42, b_+43); C = A;
  for (;;) {
    CYC(b_+43, b_+44); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+44, b_+45); alu_cp(gb, C);
    if (F & FZ) {
      CYCT(b_+45, b_+47);
      break;
    }
    CYC(b_+45, b_+47);
    CYC(b_+47, b_+48); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+48, b_+49); ret_effect(gb); return;
    }
    CYC(b_+48, b_+49);
    CYC(b_+49, b_+50); SET_HL(HL + 1);
    CYC(b_+50, b_+51); SET_HL(HL + 1);
    CYC(b_+51, b_+53);
  }
  CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+54, b_+55); C = mem_rd(gb, HL);
  CYC(b_+55, b_+56); alu_or(gb, A);
  CYC(b_+56, b_+57); ret_effect(gb);
}

void giveTreasure_body__giveTreasure_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+57, b_+59); H8(hFF8B) = A;
  CALL_C(b_+59, checkIncreaseGashaMaturityForGettingTreasure_hook, SYM(checkIncreaseGashaMaturityForGettingTreasure), b_+62);
  CALL_C(b_+62, addTreasureToInventory_hook, SYM(addTreasureToInventory), b_+65);
  CYC(b_+65, b_+68); SET_HL(wObtainedTreasureFlags);
  CYC(b_+68, b_+70); A = H8(hFF8B);
  CALL_C(b_+70, setFlag_hook, SYM(setFlag), b_+73);
  CYC(b_+73, b_+74); push_effect(gb, BC);
  CYC(b_+74, b_+76); A = H8(hFF8B);
  CYC(b_+76, b_+77); C = A;
  CYC(b_+77, b_+79); B = 0x00;
  CYC(b_+79, b_+82); SET_HL(SYM(treasureCollectionBehaviourTable));
  CYC(b_+82, b_+83); alu_add_hl(gb, BC);
  CYC(b_+83, b_+84); alu_add_hl(gb, BC);
  CYC(b_+84, b_+85); alu_add_hl(gb, BC);
  CYC(b_+85, b_+86); SET_BC(pop_effect(gb));
  CYC(b_+86, b_+88); D = 0xc6;
  CYC(b_+88, b_+89); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+89, b_+90); E = A;
  CYC(b_+90, b_+91); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+91, b_+93);
  } else {
    CYC(b_+91, b_+93);
    CYC(b_+93, b_+95); E = (uint8_t)wShortSecretIndex;
  }
  CYC(b_+95, b_+96); A = mem_rd(gb, HL);
  CYC(b_+96, b_+98); alu_and(gb, 0x0f);
  CYC(b_+98, b_+99); push_effect(gb, HL);
  CALL_C(b_+99, giveTreasure_body__applyParameter_hook, b_+128, b_+102);
  CYC(b_+102, b_+103); SET_HL(pop_effect(gb));
  CYC(b_+103, b_+105); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+105, b_+106); SET_HL(HL + 1);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FZ)) {
    CYCT(b_+107, b_+109);
  } else {
    CYC(b_+107, b_+109);
    CALL_C(b_+109, playSound_b00_hook, SYM(playSound_b00), b_+112);
    CYC(b_+112, b_+113); alu_xor(gb, A);
  }
  CYC(b_+113, b_+114); ret_effect(gb);
}

void giveTreasure_body__applyParameter_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+128, b_+129); push_effect(gb, b_+129);
  do { uint16_t jt_ = (treasure_jump_table(gb));
    if (jt_ == b_+113) { CYC(b_+113, b_+114); ret_effect(gb); return; }
    else if (jt_ == b_+161) { giveTreasure_body__modeb_hook(gb); return; }
    else if (jt_ == b_+168) { giveTreasure_body__mode8_hook(gb); return; }
    else if (jt_ == b_+179) { giveTreasure_body__mode5_hook(gb); return; }
    else if (jt_ == b_+182) { giveTreasure_body__mode6_hook(gb); return; }
    else if (jt_ == b_+186) { giveTreasure_body__mode1_hook(gb); return; }
    else if (jt_ == b_+192) { giveTreasure_body__mode2_hook(gb); return; }
    else if (jt_ == b_+196) { giveTreasure_body__mode3_hook(gb); return; }
    else if (jt_ == b_+198) { giveTreasure_body__mode4_hook(gb); return; }
    else if (jt_ == b_+207) { giveTreasure_body__mode7_hook(gb); return; }
    else if (jt_ == b_+220) { giveTreasure_body__modea_hook(gb); return; }
    else if (jt_ == b_+224) { giveTreasure_body__modec_hook(gb); return; }
    else if (jt_ == b_+244) { giveTreasure_body__moded_hook(gb); return; }
    else if (jt_ == b_+255) { giveTreasure_body__modee_hook(gb); return; }
    else if (jt_ == b_+310) { giveTreasure_body__modef_hook(gb); return; }
    else if (jt_ == b_+332) { giveTreasure_body__mode9_hook(gb); return; }
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
  getNumUnappraisedRings_hook(gb);
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
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+2); alu_or(gb, A);
  CYC(b_+2, b_+4); A |= 0x80;
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
  } else {
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+8); A = H8(hActiveObjectType);
    CYC(b_+8, b_+10); alu_add(gb, 0x02);
    CYC(b_+10, b_+11); E = A;
    CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  }
  CYC(b_+12, b_+15); SET_HL(SYM(itemDropTables));
  CYC(b_+15, b_+16); treasure_add_index_to_hl_from_rst(gb, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+18); C = A;
  CYC(b_+18, b_+20); alu_cp(gb, 0xff);
  if (F & FZ) {
    CYCT(b_+20, b_+22);
    goto unavailable;
  }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = alu_swap(gb, A);
  CYC(b_+24, b_+25); alu_rrca(gb);
  CYC(b_+25, b_+27); alu_and(gb, 0x07);
  CYC(b_+27, b_+30); SET_HL(SYM(itemDropProbabilityTable));
  CYC(b_+30, b_+31); treasure_add_double_index_to_hl_from_rst(gb, b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+32, b_+33); H = mem_rd(gb, HL);
  CYC(b_+33, b_+34); L = A;
  CALL_C(b_+34, getRandomNumber_hook, SYM(getRandomNumber), b_+37);
  CYC(b_+37, b_+39); alu_and(gb, 0x3f);
  CALL_C(b_+39, checkFlag_hook, SYM(checkFlag), b_+42);
  if (F & FZ) {
    CYCT(b_+42, b_+44);
    goto unavailable;
  }
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+45); A = C;
  CYC(b_+45, b_+47); alu_and(gb, 0x1f);
  CYC(b_+47, b_+50); SET_HL(SYM(itemDropSetTable));
  CYC(b_+50, b_+51); treasure_add_double_index_to_hl_from_rst(gb, b_+51);
  CYC(b_+51, b_+52); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+52, b_+53); H = mem_rd(gb, HL);
  CYC(b_+53, b_+54); L = A;
  CALL_C(b_+54, getRandomNumber_hook, SYM(getRandomNumber), b_+57);
  CYC(b_+57, b_+59); alu_and(gb, 0x1f);
  CYC(b_+59, b_+60); treasure_add_index_to_hl_from_rst(gb, b_+60);
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+62); C = A;
  TAIL(checkItemDropAvailable_body);

unavailable:
  CYC(SYM(checkItemDropAvailable_body__done), (SYM(checkItemDropAvailable_body__done) + 2)); C = 0xff;
  CYC((SYM(checkItemDropAvailable_body__done) + 2), (SYM(checkItemDropAvailable_body__done) + 3)); ret_effect(gb);
}

void checkItemDropAvailable_body_hook(GB *gb) {
  BASE(checkItemDropAvailable_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+4); SET_HL(SYM(itemDropAvailabilityTable));
  CYC(b_+4, b_+5); treasure_add_double_index_to_hl_from_rst(gb, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); B = mem_rd(gb, HL);
  CYC(b_+7, b_+8); L = A;
  CYC(b_+8, b_+10); H = 0xc6;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); alu_and(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+13); ret_effect(gb); return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); C = 0xff;
  CYC(b_+15, b_+16); ret_effect(gb);
}

void giveTreasure_body__modeb_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+161, b_+162); A = C;
  CYC(b_+162, b_+165); SET_HL(wUpgradesObtained);
  CYC(b_+165, b_+168); setFlag_hook(gb);
}

void giveTreasure_body__mode8_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+168, b_+169); A = mem_rd(gb, DE);
  CYC(b_+169, b_+170); alu_cp(gb, C);
  if (!(F & FC)) {
    CYCT(b_+170, b_+171); ret_effect(gb); return;
  }
  CYC(b_+170, b_+171);
  CYC(b_+171, b_+172); A = C;
  CYC(b_+172, b_+173); mem_wr(gb, DE, A);
  CYC(b_+173, b_+176); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+176, b_+178); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+178, b_+179); ret_effect(gb);
}

void giveTreasure_body__mode5_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+179, b_+180); A = C;
  CYC(b_+180, b_+181); mem_wr(gb, DE, A);
  CYC(b_+181, b_+182); ret_effect(gb);
}

void giveTreasure_body__mode1_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+186, b_+187); A = C;
  CYC(b_+187, b_+188); H = D;
  CYC(b_+188, b_+189); L = E;
  CYC(b_+189, b_+192); setFlag_hook(gb);
}

void giveTreasure_body__mode6_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+182, b_+185); A = W8(wDungeonIndex);
  CYC(b_+185, b_+186); C = A;
  giveTreasure_body__mode1_hook(gb);
}

void giveTreasure_body__mode2_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+192, b_+193); A = mem_rd(gb, DE);
  CYC(b_+193, b_+194); A = alu_inc8(gb, A);
  CYC(b_+194, b_+195); mem_wr(gb, DE, A);
  CYC(b_+195, b_+196); ret_effect(gb);
}

void giveTreasure_body__mode4_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+198, b_+199); A = mem_rd(gb, DE);
  CYC(b_+199, b_+200); alu_add(gb, C);
  CYC(b_+200, b_+201); alu_daa(gb);
  if (!(F & FC)) {
    CYCT(b_+201, b_+203);
  } else {
    CYC(b_+201, b_+203);
    CYC(b_+203, b_+205); A = 0x99;
  }
  CYC(b_+205, b_+206); mem_wr(gb, DE, A);
  CYC(b_+206, b_+207); ret_effect(gb);
}

void giveTreasure_body__mode3_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+196, b_+198); C = 0x01;
  giveTreasure_body__mode4_hook(gb);
}

void giveTreasure_body__mode7_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+207, b_+210); A = W8(wDungeonIndex);
  CYC(b_+210, b_+211); alu_add(gb, E);
  CYC(b_+211, b_+212); L = A;
  CYC(b_+212, b_+213); H = D;
  CYC(b_+213, b_+214); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+214, b_+217); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+217, b_+219); mem_wr(gb, HL, mem_rd(gb, HL) | 0x10);
  CYC(b_+219, b_+220); ret_effect(gb);
}

void giveTreasure_body__modea_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+220, b_+221); A = mem_rd(gb, DE);
  CYC(b_+221, b_+222); alu_add(gb, C);
  CYC(b_+222, b_+223); mem_wr(gb, DE, A);
  CYC(b_+223, b_+224); ret_effect(gb);
}

void giveTreasure_body__modec_hook(GB *gb) {
  BASE(giveTreasure_body);
  CYC(b_+224, b_+225); H = D;
  CYC(b_+225, b_+226); L = E;
  CYC(b_+226, b_+228); A = (uint8_t)wLinkHealth;
  CYC(b_+228, b_+229); alu_cp(gb, E);
  CYC(b_+229, b_+230); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (!(F & FZ)) {
    CYCT(b_+230, b_+232);
  } else {
    CYC(b_+230, b_+232);
    CYC(b_+232, b_+233); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYC(b_+233, b_+235);
      CYC(b_+235, b_+237); A = 0x57;
      CYC(b_+237, b_+240); playSound_b00_hook(gb);
      return;
    }
    CYCT(b_+233, b_+235);
  }
  CYC(b_+240, b_+241); alu_add(gb, C);
  CYC(b_+241, b_+242); mem_wr(gb, DE, A);
  CYC(b_+242, b_+244);
  CYC(b_+250, b_+251); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+251, b_+252); ret_effect(gb); return;
  }
  CYC(b_+251, b_+252);
  CYC(b_+252, b_+253); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+253, b_+254); mem_wr(gb, HL, A);
  CYC(b_+254, b_+255); ret_effect(gb);
}

void giveTreasure_body__moded_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+244, giveTreasure_body__mode4_hook, b_+198, b_+247);
  CYC(b_+247, b_+248); H = D;
  CYC(b_+248, b_+249); L = E;
  CYC(b_+249, b_+250); L = alu_inc8(gb, L);
  CYC(b_+250, b_+251); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+251, b_+252); ret_effect(gb); return;
  }
  CYC(b_+251, b_+252);
  CYC(b_+252, b_+253); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+253, b_+254); mem_wr(gb, HL, A);
  CYC(b_+254, b_+255); ret_effect(gb);
}

void giveTreasure_body__modee_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+255, b_+256); A = C;
  CALL_C(b_+256, getRupeeValue_hook, SYM(getRupeeValue), b_+259);
  CYC(b_+259, b_+260); A = E;
  CYC(b_+260, b_+262); alu_cp(gb, (uint8_t)wNumRupees);
  if (!(F & FZ)) {
    CYCT(b_+262, b_+264);
  } else {
    CYC(b_+262, b_+264);
    CYC(b_+264, b_+266); A = 0x01;
    CALL_C(b_+266, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+269);
    if (!(F & FZ)) {
      CYCT(b_+269, b_+271);
    } else {
      CYC(b_+269, b_+271);
      CYC(b_+271, b_+272); H = D;
      CYC(b_+272, b_+274); L = (uint8_t)wTotalRupeesCollected;
      CALL_C(b_+274, addDecimalToHlRef_hook, SYM(addDecimalToHlRef), b_+277);
      if (!(F & FC)) {
        CYCT(b_+277, b_+279);
      } else {
        CYC(b_+277, b_+279);
        CYC(b_+279, b_+281); A = 0x01;
        CALL_C(b_+281, setGlobalFlag_hook, SYM(setGlobalFlag), b_+284);
      }
    }
  }
  CYC(b_+284, b_+285); H = D;
  CYC(b_+285, b_+286); L = E;
  CALL_C(b_+286, addDecimalToHlRef_hook, SYM(addDecimalToHlRef), b_+289);
  CYC(b_+289, b_+290); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+290, b_+291); H = mem_rd(gb, HL);
  CYC(b_+291, b_+292); L = A;
  CYC(b_+292, b_+295); SET_BC(0x0999);
  CALL_C(b_+295, compareHlToBc_hook, SYM(compareHlToBc), b_+298);
  CYC(b_+298, b_+299); A = alu_dec8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+299, b_+300); ret_effect(gb); return;
  }
  CYC(b_+299, b_+300);
  CYC(b_+300, b_+301); A = C;
  CYC(b_+301, b_+302); mem_wr(gb, DE, A);
  CYC(b_+302, b_+303); E = alu_inc8(gb, E);
  CYC(b_+303, b_+304); A = B;
  CYC(b_+304, b_+305); mem_wr(gb, DE, A);
  CYC(b_+305, b_+307); A = 0x61;
  CYC(b_+307, b_+310); playSound_b00_hook(gb);
}

void giveTreasure_body__modef_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+310, giveTreasure_body__mode4_hook, b_+198, b_+313);
  CALL_C(b_+313, setStatusBarNeedsRefreshBit1_hook, SYM(setStatusBarNeedsRefreshBit1), b_+316);
  CYC(b_+316, b_+319); A = W8(wSeedSatchelLevel);
  CYC(b_+319, b_+322); SET_HL(GV(b_+328, 0x4616));
  CYC(b_+322, b_+323); treasure_add_index_to_hl_from_rst(gb, b_+323);
  CYC(b_+323, b_+324); A = mem_rd(gb, DE);
  CYC(b_+324, b_+325); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) {
    CYCT(b_+325, b_+326); ret_effect(gb); return;
  }
  CYC(b_+325, b_+326);
  CYC(b_+326, b_+327); A = mem_rd(gb, HL);
  CYC(b_+327, b_+328); mem_wr(gb, DE, A);
  CYC(b_+328, b_+329); ret_effect(gb);
}

void giveTreasure_body__mode9_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+332, b_+334); C |= 0x40;
  CALL_C(b_+334, realignUnappraisedRings_hook, SYM(realignUnappraisedRings), b_+337);
  CYC(b_+337, b_+339); alu_cp(gb, 0x64);
  if (F & FC) {
    CYCT(b_+339, b_+341);
  } else {
    CYC(b_+339, b_+341);
    CALL_C(b_+341, giveTreasure_body__removeOneDuplicateRing_hook, b_+353, b_+344);
    CALL_C(b_+344, realignUnappraisedRings_hook, SYM(realignUnappraisedRings), b_+347);
  }
  CYC(b_+347, b_+348); A = C;
  CYC(b_+348, b_+351); mem_wr(gb, wUnappraisedRingsEnd - 1, A);
  CYC(b_+351, b_+353); realignUnappraisedRings_hook(gb);
}

void giveTreasure_body__removeOneDuplicateRing_hook(GB *gb) {
  BASE(giveTreasure_body);
  uint16_t sp0_ = gb->sp;
  CYC(b_+353, b_+355); A = hram_rd(gb, R_SVBK);
  CYC(b_+355, b_+356); push_effect(gb, AF);
  CYC(b_+356, b_+358); A = 0x04;
  CYC(b_+358, b_+360); hram_wr(gb, R_SVBK, A);
  CYC(b_+360, b_+363); SET_HL(w4TmpRingBuffer);
  CYC(b_+363, b_+365); B = 0x40;
  CALL_C(b_+365, clearMemory_hook, SYM(clearMemory), b_+368);
  CYC(b_+368, b_+371); SET_DE(wUnappraisedRings);
  CYC(b_+371, b_+373); B = 0x40;
  do {
    CYC(b_+373, b_+374); A = mem_rd(gb, DE);
    CYC(b_+374, b_+376); alu_and(gb, 0x3f);
    CYC(b_+376, b_+379); SET_HL(w4TmpRingBuffer);
    CYC(b_+379, b_+380); treasure_add_index_to_hl_from_rst(gb, b_+380);
    CYC(b_+380, b_+381); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    CYC(b_+381, b_+382); E = alu_inc8(gb, E);
    CYC(b_+382, b_+383); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+383, b_+385);
    else CYC(b_+383, b_+385);
  } while (!(F & FZ));
  CYC(b_+385, b_+388); SET_HL(w4TmpRingBuffer);
  CYC(b_+388, b_+391); SET_DE(0x0000);
  CYC(b_+391, b_+393); B = 0x40;
  do {
    CYC(b_+393, b_+394); A = mem_rd(gb, HL);
    CYC(b_+394, b_+395); alu_cp(gb, D);
    if (F & FC) {
      CYCT(b_+395, b_+397);
    } else {
      CYC(b_+395, b_+397);
      CYC(b_+397, b_+398); D = A;
      CYC(b_+398, b_+399); E = L;
    }
    CYC(b_+399, b_+400); L = alu_inc8(gb, L);
    CYC(b_+400, b_+401); B = alu_dec8(gb, B);
    if (!(F & FZ)) CYCT(b_+401, b_+403);
    else CYC(b_+401, b_+403);
  } while (!(F & FZ));
  CYC(b_+403, b_+404); A = E;
  CYC(b_+404, b_+406); alu_sub(gb, (uint8_t)w4TmpRingBuffer);
  CYC(b_+406, b_+408); alu_or(gb, 0x40);
  CYC(b_+408, b_+409); E = A;
  CYC(b_+409, b_+410); SET_AF(pop_effect(gb));
  CYC(b_+410, b_+412); hram_wr(gb, R_SVBK, A);
  CYC(b_+412, b_+415); SET_HL(wUnappraisedRingsEnd - 1);
  do {
    CYC(b_+415, b_+416); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+416, b_+417); alu_cp(gb, E);
    if (!(F & FZ)) CYCT(b_+417, b_+419);
    else CYC(b_+417, b_+419);
  } while (!(F & FZ));
  CYC(b_+419, b_+420); SET_HL(HL + 1);
  CYC(b_+420, b_+422); mem_wr(gb, HL, 0xff);
  CYC(b_+422, b_+423); ret_effect(gb);
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
