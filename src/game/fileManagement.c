#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void clearFileAtHl_hook(GB *gb);
void fileManagementFunction_hook(GB *gb);
void initializeFile_b07_hook(GB *gb);
void saveFile_b07_hook(GB *gb);
void loadFile_b07_hook(GB *gb);
void eraseFile_b07_hook(GB *gb);
void eraseFile__clearFile_b07_hook(GB *gb);
void verifyFileCopies_hook(GB *gb);
void verifyFileCopies__copy2Invalid_hook(GB *gb);
void verifyFileCopies__bothCopiesValid_hook(GB *gb);
void verifyFileCopies__copy1Invalid_hook(GB *gb);
void verifyFileCopies__bothCopiesInvalid_hook(GB *gb);
void copyFileFromHlToDe_hook(GB *gb);
void verifyFileAtHl_hook(GB *gb);
void verifyFileAtHl__nextChar_hook(GB *gb);
void verifyFileAtHl__verifyDone_hook(GB *gb);
void verifyFileAtHl__verifyFailed_hook(GB *gb);
void calculateFileChecksum_hook(GB *gb);
void getFileAddress1_hook(GB *gb);
void getFileAddress2_hook(GB *gb);
void initializeFileVariables_hook(GB *gb);

static uint16_t jump_table_from_rst(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void add_a_to_hl_from_rst(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  }
  pop_effect(gb);
}

static void add_double_index_to_hl_from_rst(GB *gb) {
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void fileManagementFunction_hook(GB *gb) {
  BASE(fileManagementFunction);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+2); push_effect(gb, b_+2);
  do { uint16_t jt_ = (jump_table_from_rst(gb));
    if (jt_ == SYM(initializeFile_b07) && hook_enabled_at(gb, SYM(initializeFile_b07))) { initializeFile_b07_hook(gb); return; }
    else if (jt_ == SYM(saveFile_b07) && hook_enabled_at(gb, SYM(saveFile_b07))) { saveFile_b07_hook(gb); return; }
    else if (jt_ == SYM(loadFile_b07) && hook_enabled_at(gb, SYM(loadFile_b07))) { loadFile_b07_hook(gb); return; }
    else if (jt_ == SYM(eraseFile_b07) && hook_enabled_at(gb, SYM(eraseFile_b07))) { eraseFile_b07_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void initializeFile_b07_hook(GB *gb) {
  BASE(initializeFile_b07);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(SYM(initialFileVariables));
  CALL_C(b_+3, initializeFileVariables_hook, SYM(initializeFileVariables), b_+6);
  CYC(b_+6, b_+9); SET_HL(wFileIsHeroGame);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+10, b_+11); alu_add(gb, A);
  CYC(b_+11, b_+12); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+12, b_+13); push_effect(gb, AF);
  CYC(b_+13, b_+16); SET_HL(SYM(initialFileVariablesTable));
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  add_double_index_to_hl_from_rst(gb);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+18, b_+19); H = mem_rd(gb, HL);
  CYC(b_+19, b_+20); L = A;
  CALL_C(b_+20, initializeFileVariables_hook, SYM(initializeFileVariables), b_+23);
  CYC(b_+23, b_+24); SET_AF(pop_effect(gb));
  CYC(b_+24, b_+25); C = A;
  CYC(b_+25, b_+28); SET_HL(wUnappraisedRings);
  CYC(b_+28, b_+30); B = 0x40;
  CYC(b_+30, b_+32); A = 0xff;
  CALL_C(b_+32, fillMemory_hook, SYM(fillMemory), b_+35);
  CYC(b_+35, b_+38); SET_HL(wRingBoxContents);
  CYC(b_+38, b_+40); B = 0x06;
  CYC(b_+40, b_+42); A = 0xff;
  CALL_C(b_+42, fillMemory_hook, SYM(fillMemory), b_+45);
  CYC(b_+45, b_+46); A = C;
  CYC(b_+46, b_+48); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+48, b_+50);
  } else {
    CYC(b_+48, b_+50);
    CYC(b_+50, b_+53); SET_HL(wObtainedTreasureFlags);
    CYC(b_+53, b_+55); A = 0x2d;
    CALL_C(b_+55, setFlag_hook, SYM(setFlag), b_+58);
    CYC(b_+58, b_+60); A = 0x76;
    CYC(b_+60, b_+63); mem_wr(gb, wUnappraisedRings, A);
  }
  CYC(b_+63, b_+66); SET_HL((SYM(getFileAddress2) + 1));
  CYC(b_+66, b_+68); E = 0x0b;
  CALL_C(b_+68, interBankCall_hook, 0x008a, b_+71);
  CYC(b_+71, b_+74); SET_HL((SYM(enemyActiveCollisions) + 59));
  CYC(b_+74, b_+76); E = 0x04;
  CALL_C(b_+76, interBankCall_hook, 0x008a, SYM(saveFile_b07));
  TAIL(saveFile_b07);
}

void saveFile_b07_hook(GB *gb) {
  BASE(saveFile_b07);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wWhichGame);
  CYC(b_+3, b_+5); mem_wr(gb, HL, GV(0x01, 0x00));
  CYC(b_+5, b_+8); SET_HL(wSavefileString);
  CYC(b_+8, b_+11); SET_DE(SYM(saveVerificationString));
  CYC(b_+11, b_+13); B = 0x08;
  CALL_C(b_+13, copyMemoryReverse_hook, SYM(copyMemoryReverse), b_+16);
  CYC(b_+16, b_+18); L = (uint8_t)wFileStart;
  CALL_C(b_+18, calculateFileChecksum_hook, SYM(calculateFileChecksum), b_+21);
  CYC(b_+21, b_+22); mem_wr(gb, HL, E);
  CYC(b_+22, b_+23); L = alu_inc8(gb, L);
  CYC(b_+23, b_+24); mem_wr(gb, HL, D);
  CYC(b_+24, b_+26); L = (uint8_t)wFileStart;
  CALL_C(b_+26, getFileAddress1_hook, SYM(getFileAddress1), b_+29);
  CYC(b_+29, b_+30); E = C;
  CYC(b_+30, b_+31); D = B;
  CALL_C(b_+31, copyFileFromHlToDe_hook, SYM(copyFileFromHlToDe), b_+34);
  CALL_C(b_+34, getFileAddress2_hook, SYM(getFileAddress2), b_+37);
  CYC(b_+37, b_+38); E = C;
  CYC(b_+38, b_+39); D = B;
  CALL_C(b_+39, copyFileFromHlToDe_hook, SYM(copyFileFromHlToDe), b_+42);
  CYC(b_+42, b_+44);
  TAIL(verifyFileCopies);
}

void loadFile_b07_hook(GB *gb) {
  BASE(loadFile_b07);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, verifyFileCopies_hook, SYM(verifyFileCopies), b_+3);
  CYC(b_+3, b_+4); push_effect(gb, AF);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYC(b_+5, b_+7);
    CALL_C(b_+7, getFileAddress1_hook, SYM(getFileAddress1), b_+10);
    CYC(b_+10, b_+12);
  } else {
    CYCT(b_+5, b_+7);
    CALL_C(b_+12, getFileAddress2_hook, SYM(getFileAddress2), b_+15);
  }
  CYC(b_+15, b_+16); L = C;
  CYC(b_+16, b_+17); H = B;
  CYC(b_+17, b_+20); SET_DE(wFileStart);
  CALL_C(b_+20, copyFileFromHlToDe_hook, SYM(copyFileFromHlToDe), b_+23);
  CYC(b_+23, b_+24); SET_AF(pop_effect(gb));
  CYC(b_+24, b_+25); ret_effect(gb);
}

void eraseFile_b07_hook(GB *gb) {
  BASE(eraseFile_b07);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFileAddress1_hook, SYM(getFileAddress1), b_+3);
  CALL_C(b_+3, eraseFile__clearFile_b07_hook, b_+9, b_+6);
  CALL_C(b_+6, getFileAddress2_hook, SYM(getFileAddress2), b_+9);
  TAIL(eraseFile__clearFile_b07);
}

void eraseFile__clearFile_b07_hook(GB *gb) {
  BASE(eraseFile_b07);
  uint16_t sp0_ = gb->sp;
  CYC(b_+9, b_+11); A = 0x0a;
  CYC(b_+11, b_+14); mem_wr(gb, 0x1111, A);
  CYC(b_+14, b_+15); L = C;
  CYC(b_+15, b_+16); H = B;
  CALL_C(b_+16, clearFileAtHl_hook, SYM(clearFileAtHl), b_+19);
  CYC(b_+19, b_+20); alu_xor(gb, A);
  CYC(b_+20, b_+23); mem_wr(gb, 0x1111, A);
  CYC(b_+23, b_+24); ret_effect(gb);
}

void clearFileAtHl_hook(GB *gb) {
  BASE(clearFileAtHl);
  CYC(b_+0, b_+3); SET_BC(0x0550);
  CYC(b_+3, b_+6);
  TAIL(clearMemoryBc);
}

static void verify_file_copies_both_valid(GB *gb) {
  BASE(verifyFileCopies);
  CYC(b_+45, b_+46); alu_xor(gb, A);
  CYC(b_+46, b_+47); ret_effect(gb);
}

static void verify_file_copies_copy2_invalid(GB *gb, uint16_t sp0_) {
  BASE(verifyFileCopies);
  CALL_C(b_+32, getFileAddress2_hook, SYM(getFileAddress2), b_+35);
  CYC(b_+35, b_+36); E = C;
  CYC(b_+36, b_+37); D = B;
  CALL_C(b_+37, getFileAddress1_hook, SYM(getFileAddress1), b_+40);
  CYC(b_+40, b_+41); L = C;
  CYC(b_+41, b_+42); H = B;
  CALL_C(b_+42, copyFileFromHlToDe_hook, SYM(copyFileFromHlToDe), b_+45);
  verify_file_copies_both_valid(gb);
}

static void verify_file_copies_copy1_invalid(GB *gb, uint16_t sp0_) {
  BASE(verifyFileCopies);
  CALL_C(b_+47, getFileAddress1_hook, SYM(getFileAddress1), b_+50);
  CYC(b_+50, b_+51); E = C;
  CYC(b_+51, b_+52); D = B;
  CALL_C(b_+52, getFileAddress2_hook, SYM(getFileAddress2), b_+55);
  CYC(b_+55, b_+56); L = C;
  CYC(b_+56, b_+57); H = B;
  CALL_C(b_+57, copyFileFromHlToDe_hook, SYM(copyFileFromHlToDe), b_+60);
  CYC(b_+60, b_+62); A = 0x01;
  CYC(b_+62, b_+63); ret_effect(gb);
}

static void verify_file_copies_both_invalid(GB *gb) {
  BASE(verifyFileCopies);
  CYC(b_+63, b_+65); A = 0xff;
  CYC(b_+65, b_+66); ret_effect(gb);
}

void verifyFileCopies_hook(GB *gb) {
  BASE(verifyFileCopies);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFileAddress2_hook, SYM(getFileAddress2), b_+3);
  CYC(b_+3, b_+4); L = C;
  CYC(b_+4, b_+5); H = B;
  CALL_C(b_+5, verifyFileAtHl_hook, SYM(verifyFileAtHl), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x01);
  CYC(b_+10, b_+11); push_effect(gb, AF);
  CALL_C(b_+11, getFileAddress1_hook, SYM(getFileAddress1), b_+14);
  CYC(b_+14, b_+15); L = C;
  CYC(b_+15, b_+16); H = B;
  CALL_C(b_+16, verifyFileAtHl_hook, SYM(verifyFileAtHl), b_+19);
  CYC(b_+19, b_+20); SET_BC(pop_effect(gb));
  CYC(b_+20, b_+22); B = alu_rl(gb, B);
  CYC(b_+22, b_+23); A = B;
  CYC(b_+23, b_+24); push_effect(gb, b_+24);
  do { uint16_t jt_ = (jump_table_from_rst(gb));
    if (jt_ == b_+32) { verify_file_copies_copy2_invalid(gb, sp0_); return; }
    else if (jt_ == b_+45) { verify_file_copies_both_valid(gb); return; }
    else if (jt_ == b_+47) { verify_file_copies_copy1_invalid(gb, sp0_); return; }
    else if (jt_ == b_+63) { verify_file_copies_both_invalid(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void verifyFileCopies__copy2Invalid_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  verify_file_copies_copy2_invalid(gb, sp0_);
}

void verifyFileCopies__bothCopiesValid_hook(GB *gb) {
  verify_file_copies_both_valid(gb);
}

void verifyFileCopies__copy1Invalid_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  verify_file_copies_copy1_invalid(gb, sp0_);
}

void verifyFileCopies__bothCopiesInvalid_hook(GB *gb) {
  verify_file_copies_both_invalid(gb);
}

void copyFileFromHlToDe_hook(GB *gb) {
  BASE(copyFileFromHlToDe);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+3); A = 0x0a;
  CYC(b_+3, b_+6); mem_wr(gb, 0x1111, A);
  CYC(b_+6, b_+9); SET_BC(0x0550);
  CALL_C(b_+9, copyMemoryBc_hook, SYM(copyMemoryBc), b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+16); mem_wr(gb, 0x1111, A);
  CYC(b_+16, b_+17); SET_HL(pop_effect(gb));
  CYC(b_+17, b_+18); ret_effect(gb);
}

static void verify_file_done(GB *gb) {
  BASE(verifyFileAtHl);
  CYC(b_+31, b_+32); alu_xor(gb, A);
  CYC(b_+32, b_+35); mem_wr(gb, 0x1111, A);
  CYC(b_+35, b_+36); SET_HL(pop_effect(gb));
  CYC(b_+36, b_+37); A = B;
  CYC(b_+37, b_+38); alu_rrca(gb);
  CYC(b_+38, b_+39); ret_effect(gb);
}

static void verify_file_failed(GB *gb, uint16_t sp0_) {
  BASE(verifyFileAtHl);
  CYC(b_+39, b_+40); SET_HL(pop_effect(gb));
  CYC(b_+40, b_+41); push_effect(gb, HL);
  CALL_C(b_+41, clearFileAtHl_hook, SYM(clearFileAtHl), b_+44);
  CYC(b_+44, b_+46); B = 0xff;
  CYC(b_+46, b_+48);
  verify_file_done(gb);
}

static void verify_file_characters(GB *gb, uint16_t sp0_) {
  BASE(verifyFileAtHl);
  for (;;) {
    CYC(b_+22, b_+23); A = mem_rd(gb, DE);
    CYC(b_+23, b_+24); alu_cp(gb, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYCT(b_+24, b_+26);
      verify_file_failed(gb, sp0_);
      return;
    }
    CYC(b_+24, b_+26);
    CYC(b_+26, b_+27); SET_DE(DE + 1);
    CYC(b_+27, b_+28); SET_HL(HL + 1);
    CYC(b_+28, b_+29); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(b_+29, b_+31);
      continue;
    }
    CYC(b_+29, b_+31);
    verify_file_done(gb);
    return;
  }
}

void verifyFileAtHl_hook(GB *gb) {
  BASE(verifyFileAtHl);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+3); A = 0x0a;
  CYC(b_+3, b_+6); mem_wr(gb, 0x1111, A);
  CALL_C(b_+6, calculateFileChecksum_hook, SYM(calculateFileChecksum), b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); alu_cp(gb, E);
  if (!(F & FZ)) {
    CYCT(b_+11, b_+13);
    verify_file_failed(gb, sp0_);
    return;
  }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); alu_cp(gb, D);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+17);
    verify_file_failed(gb, sp0_);
    return;
  }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+20); SET_DE(SYM(saveVerificationString));
  CYC(b_+20, b_+22); B = 0x08;
  verify_file_characters(gb, sp0_);
}

void verifyFileAtHl__nextChar_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  verify_file_characters(gb, sp0_);
}

void verifyFileAtHl__verifyDone_hook(GB *gb) {
  verify_file_done(gb);
}

void verifyFileAtHl__verifyFailed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  verify_file_failed(gb, sp0_);
}

void calculateFileChecksum_hook(GB *gb) {
  BASE(calculateFileChecksum);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+3); A = 0x02;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  add_a_to_hl_from_rst(gb);
  CYC(b_+4, b_+7); SET_BC(0x02a7);
  CYC(b_+7, b_+10); SET_DE(0x0000);
  for (;;) {
    CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+11, b_+12); alu_add(gb, E);
    CYC(b_+12, b_+13); E = A;
    CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+14, b_+15); alu_adc(gb, D);
    CYC(b_+15, b_+16); D = A;
    CYC(b_+16, b_+17); SET_BC(BC - 1);
    CYC(b_+17, b_+18); A = B;
    CYC(b_+18, b_+19); alu_or(gb, C);
    if (BC) {
      CYCT(b_+19, b_+21);
      continue;
    }
    CYC(b_+19, b_+21);
    break;
  }
  CYC(b_+21, b_+22); SET_HL(pop_effect(gb));
  CYC(b_+22, b_+23); ret_effect(gb);
}

static void get_file_address(GB *gb) {
  BASE(getFileAddress2);
  CYC(b_+2, b_+3); push_effect(gb, HL);
  CYC(b_+3, b_+5); A = H8(hActiveFileSlot);
  CYC(b_+5, b_+6); alu_add(gb, C);
  CYC(b_+6, b_+9); SET_HL(b_+15);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  add_double_index_to_hl_from_rst(gb);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); B = mem_rd(gb, HL);
  CYC(b_+12, b_+13); C = A;
  CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
  CYC(b_+14, b_+15); ret_effect(gb);
}

void getFileAddress1_hook(GB *gb) {
  BASE(getFileAddress1);
  CYC(b_+0, b_+2); C = 0x00;
  CYC(b_+2, b_+4);
  get_file_address(gb);
}

void getFileAddress2_hook(GB *gb) {
  BASE(getFileAddress2);
  CYC(b_+0, b_+2); C = 0x03;
  get_file_address(gb);
}

void initializeFileVariables_hook(GB *gb) {
  BASE(initializeFileVariables);
  CYC(b_+0, b_+2); D = 0xc6;
  for (;;) {
    CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+3, b_+4); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+4, b_+6);
      break;
    }
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+7); E = A;
    CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+8, b_+9); mem_wr(gb, DE, A);
    CYC(b_+9, b_+11);
  }
  CYC(b_+11, b_+12); ret_effect(gb);
}
