#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(init), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(init), (from), (to), true)

void generateGameTransferSecret_hook(GB *gb);
void secretFunctionCaller_body_hook(GB *gb);
void generateSecret_hook(GB *gb);
void insertBitsIntoSecretGenerationBuffer_hook(GB *gb);
void unpackSecret_hook(GB *gb);
void verifyUnpackedSecretGameID_hook(GB *gb);
void loadUnpackedSecretData_hook(GB *gb);
void loadUnpackedSecretData__type0_hook(GB *gb);
void loadUnpackedSecretData__type1_hook(GB *gb);
void loadUnpackedSecretData__type3_hook(GB *gb);
void loadUnpackedSecretData__type2_hook(GB *gb);
void generateGameIDIfNeeded_hook(GB *gb);
void generateSecret__determineXorCipher_hook(GB *gb);
void generateSecret__ret_hook(GB *gb);
void encodeSecretData_paramC_hook(GB *gb);
void encodeSecretData_hook(GB *gb);
void unpackSecret__unpackSecretData_hook(GB *gb);
void unpackSecret__unpack_hook(GB *gb);
void unpackSecret__unpack__nextEntry_hook(GB *gb);
void unpackSecret__readBits_hook(GB *gb);
void unpackSecret__readBits__end_hook(GB *gb);
void unpackSecret__fail_hook(GB *gb);
void convertSecretBufferToText_hook(GB *gb);
void loadSecretBufferFromText_hook(GB *gb);
void loadSecretBufferFromText__textCharacterToByte_hook(GB *gb);
void loadSecretBufferFromText__end_hook(GB *gb);
void runXorCipherOnSecretBuffer_hook(GB *gb);
void getSecretBufferChecksum_hook(GB *gb);
void shiftSecretBufferContentsToFront_hook(GB *gb);
void andCWith3_hook(GB *gb);
void getNumCharactersForSecretType_hook(GB *gb);

static uint16_t secret_function_jump_table(GB *gb) {
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

static void add_a_to_hl_from_rst(GB *gb, uint16_t return_address) {
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

static void add_double_index_to_hl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void init_hook(GB *gb) {
  BASE(init);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); gb->ime = false; gb->ime_delay = false; gb->ime_writes++;
  CYC(b_+1, b_+2); alu_xor(gb, A);
  CYC(b_+2, b_+4); mem_wr(gb, IO_IF, A);
  CYC(b_+4, b_+6); mem_wr(gb, IO_IE, A);
  CYC(b_+6, b_+8); mem_wr(gb, IO_STAT, A);
  CYC(b_+8, b_+10); mem_wr(gb, IO_TAC, A);
  CYC(b_+10, b_+12); mem_wr(gb, IO_SC, A);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+16); mem_wr(gb, (SYM(setDeathRespawnPoint) + 17), A);
  CALL_C(b_+16, disableLcd_hook, SYM(disableLcd), b_+19);
  CYC(b_+19, b_+21); A = H8(hGameboyType);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+22, b_+24);
  } else {
    CYC(b_+22, b_+24);
    CYC(b_+24, b_+25); alu_xor(gb, A);
    CYC(b_+25, b_+27); mem_wr(gb, IO_RP, A);
    CYC(b_+27, b_+29); mem_wr(gb, IO_SVBK, A);
    CYC(b_+29, b_+31); mem_wr(gb, IO_VBK, A);
    CALL_C(b_+31, setCpuToDoubleSpeed, SYM(setCpuToDoubleSpeed), b_+34);
  }
  CYC(b_+34, b_+37); SET_HL(hActiveFileSlot);
  CYC(b_+37, b_+39); B = 0x26;
  CALL_C(b_+39, clearMemory_hook, SYM(clearMemory), b_+42);
  CYC(b_+42, b_+45); SET_HL(wThread3StackTop);
  CYC(b_+45, b_+48); SET_BC((SYM(checkLinkCollisionsEnabled) + 13));
  CALL_C(b_+48, clearMemoryBc_hook, SYM(clearMemoryBc), b_+51);
  CALL_C(b_+51, clearVram_hook, SYM(clearVram), b_+54);
  CYC(b_+54, b_+57); SET_HL(SYM(oamDmaFunction));
  CYC(b_+57, b_+60); SET_DE(hOamFunc);
  CYC(b_+60, b_+62); B = 0x0a;
  CALL_C(b_+62, copyMemory_hook, SYM(copyMemory), b_+65);
  CYC(b_+65, b_+67); A = 0xe4;
  CYC(b_+67, b_+69); mem_wr(gb, IO_BGP, A);
  CYC(b_+69, b_+71); mem_wr(gb, IO_OBP0, A);
  CYC(b_+71, b_+73); A = 0x6c;
  CYC(b_+73, b_+75); mem_wr(gb, IO_OBP1, A);
  CALL_C(b_+75, initSound_b00_hook, SYM(initSound_b00), b_+78);
  CYC(b_+78, b_+80); A = 0xc7;
  CYC(b_+80, b_+82); mem_wr(gb, IO_LYC, A);
  CYC(b_+82, b_+84); A = 0x40;
  CYC(b_+84, b_+86); mem_wr(gb, IO_STAT, A);
  CYC(b_+86, b_+87); alu_xor(gb, A);
  CYC(b_+87, b_+89); mem_wr(gb, IO_IF, A);
  CYC(b_+89, b_+91); A = 0x0f;
  CYC(b_+91, b_+93); mem_wr(gb, IO_IE, A);
  CYC(b_+93, b_+96); SET_HL(b_+0);
  CYC(b_+96, b_+98); E = 0x3f;
  CALL_C(b_+98, interBankCall_hook, 0x008a, b_+101);
  CYC(b_+101, b_+102); gb->ime_delay = true; gb->ime_writes++;
  CYC(b_+102, b_+105); SET_HL(b_+0);
  CYC(b_+105, b_+107); E = 0x02;
  CALL_C(b_+107, interBankCall_hook, 0x008a, b_+110);
  CYC(b_+110, SYM(setCpuToDoubleSpeed));
  startGame_hook(gb);
}

void unpackSecret__fail_hook(GB *gb) {
  BASE(unpackSecret);
  CYC(b_+50, b_+52); B = 0x01;
  CYC(b_+52, b_+53); ret_effect(gb);
}

static void verifyUnpackedSecretGameID_fail(GB *gb) {
  BASE(verifyUnpackedSecretGameID);
  CYC(b_+20, b_+22); B = 0x01;
  CYC(b_+22, b_+23); ret_effect(gb);
}

static void verifyUnpackedSecretGameID_success(GB *gb) {
  BASE(verifyUnpackedSecretGameID);
  CYC(b_+23, b_+25); B = 0x00;
  CYC(b_+25, SYM(generateGameIDIfNeeded)); ret_effect(gb);
}

void generateGameTransferSecret_hook(GB *gb) {
  BASE(generateGameTransferSecret);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wFileIsLinkedGame);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); B = mem_rd(gb, HL);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+7); push_effect(gb, BC);
  CYC(b_+7, b_+8); alu_or(gb, B);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+9, b_+11); alu_xor(gb, 0x01);
  CYC(b_+11, b_+12); alu_or(gb, B);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A);
  CYC(b_+13, b_+16); SET_BC(0x0000);
  CALL_C(b_+16, secretFunctionCaller_body_hook, SYM(secretFunctionCaller_body), b_+19);
  CYC(b_+19, b_+20); SET_BC(pop_effect(gb));
  CYC(b_+20, b_+23); SET_HL(wFileIsLinkedGame);
  CYC(b_+23, b_+24); mem_wr(gb, HL, C);
  CYC(b_+24, b_+25); L = alu_inc8(gb, L);
  CYC(b_+25, b_+26); mem_wr(gb, HL, B);
  CYC(b_+26, SYM(secretFunctionCaller_body)); ret_effect(gb);
}

void secretFunctionCaller_body_hook(GB *gb) {
  BASE(secretFunctionCaller_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+3); A = hram_rd(gb, 0x70);
  CYC(b_+3, b_+4); push_effect(gb, AF);
  CYC(b_+4, b_+6); A = 0x07;
  CYC(b_+6, b_+8); hram_wr(gb, 0x70, A);
  CYC(b_+8, b_+11); push_effect(gb, b_+11);
  CYC(b_+16, b_+17); A = B;
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (secret_function_jump_table(gb));
    if (jt_ == SYM(generateSecret)) { generateSecret_hook(gb); break; }
    else if (jt_ == SYM(unpackSecret)) { unpackSecret_hook(gb); break; }
    else if (jt_ == SYM(verifyUnpackedSecretGameID)) { verifyUnpackedSecretGameID_hook(gb); break; }
    else if (jt_ == SYM(generateGameIDIfNeeded)) { generateGameIDIfNeeded_hook(gb); break; }
    else if (jt_ == SYM(loadUnpackedSecretData)) { loadUnpackedSecretData_hook(gb); break; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
  CYC(b_+11, b_+12); SET_AF(pop_effect(gb));
  CYC(b_+12, b_+14); hram_wr(gb, 0x70, A);
  CYC(b_+14, b_+15); SET_DE(pop_effect(gb));
  CYC(b_+15, b_+16); ret_effect(gb);
}

void generateSecret_hook(GB *gb) {
  BASE(generateSecret);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w7SecretText1);
  CYC(b_+3, b_+5); B = 0x40;
  CALL_C(b_+5, clearMemory_hook, SYM(clearMemory), b_+8);
  CALL_C(b_+8, andCWith3_hook, SYM(andCWith3), b_+11);
  CALL_C(b_+11, generateGameIDIfNeeded_hook, SYM(generateGameIDIfNeeded), b_+14);
  CALL_C(b_+14, generateSecret__determineXorCipher_hook, b_+53, b_+17);
  CYC(b_+17, b_+20); SET_HL(wSecretXorCipherIndex);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); mem_wr(gb, HL, C);
  CYC(b_+22, b_+24); A = 0x04;
  CALL_C(b_+24, encodeSecretData_hook, SYM(encodeSecretData), b_+27);
  CALL_C(b_+27, encodeSecretData_paramC_hook, SYM(encodeSecretData_paramC), b_+30);
  CYC(b_+30, b_+32); B = 0x04;
  CYC(b_+32, b_+33); alu_xor(gb, A);
  CALL_C(b_+33, insertBitsIntoSecretGenerationBuffer_hook, SYM(insertBitsIntoSecretGenerationBuffer), b_+36);
  CALL_C(b_+36, getSecretBufferChecksum_hook, SYM(getSecretBufferChecksum), b_+39);
  CYC(b_+39, b_+42); SET_HL(w7SecretGenerationBuffer + 19);
  CYC(b_+42, b_+43); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  CALL_C(b_+44, shiftSecretBufferContentsToFront_hook, SYM(shiftSecretBufferContentsToFront), b_+47);
  CALL_C(b_+47, runXorCipherOnSecretBuffer_hook, SYM(runXorCipherOnSecretBuffer), b_+50);
  CYC(b_+50, b_+53); convertSecretBufferToText_hook(gb);
}

void generateSecret__ret_hook(GB *gb) {
  BASE(generateSecret);
  CYC(b_+81, b_+83); alu_and(gb, 0x07);
  CYC(b_+83, b_+84); SET_BC(pop_effect(gb));
  CYC(b_+84, SYM(encodeSecretData_paramC)); ret_effect(gb);
}

void generateSecret__determineXorCipher_hook(GB *gb) {
  BASE(generateSecret);
  CYC(b_+53, b_+54); push_effect(gb, BC);
  CYC(b_+54, b_+57); SET_HL(wGameID);
  CYC(b_+57, b_+58); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+58, b_+59); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+59, b_+60); B = A;
  CYC(b_+60, b_+61); A = C;
  CYC(b_+61, b_+63); alu_cp(gb, 0x03);
  CYC(b_+63, b_+64); A = B;
  if (!(F & FZ)) {
    CYCT(b_+64, b_+66);
    generateSecret__ret_hook(gb);
    return;
  }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); L = 0xfb;
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  CYC(b_+69, b_+71); A = alu_swap(gb, A);
  CYC(b_+71, b_+73); alu_and(gb, 0x0f);
  CYC(b_+73, b_+74); alu_add(gb, B);
  CYC(b_+74, b_+75); B = A;
  CYC(b_+75, b_+76); A = mem_rd(gb, HL);
  CYC(b_+76, b_+78); alu_and(gb, 0x01);
  CYC(b_+78, b_+79); alu_rlca(gb);
  CYC(b_+79, b_+80); alu_rlca(gb);
  CYC(b_+80, b_+81); alu_xor(gb, B);
  generateSecret__ret_hook(gb);
}

void encodeSecretData_paramC_hook(GB *gb) {
  BASE(encodeSecretData_paramC);
  CYC(b_+0, SYM(encodeSecretData)); A = C;
  encodeSecretData_hook(gb);
}

void encodeSecretData_hook(GB *gb) {
  BASE(encodeSecretData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+4); SET_HL(SYM(secretDataToEncodeTable));
  CYC(b_+4, b_+5); add_double_index_to_hl_from_rst(gb, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); H = mem_rd(gb, HL);
  CYC(b_+7, b_+8); L = A;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); C = A;
  for (;;) {
    CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+11, b_+12); E = A;
    CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+13, b_+14); B = A;
    CYC(b_+14, b_+16); D = 0xc6;
    CYC(b_+16, b_+17); A = mem_rd(gb, DE);
    CALL_C(b_+17, insertBitsIntoSecretGenerationBuffer_hook, SYM(insertBitsIntoSecretGenerationBuffer), b_+20);
    CYC(b_+20, b_+21); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+21, b_+23); continue; }
    CYC(b_+21, b_+23);
    break;
  }
  CYC(b_+23, b_+24); SET_BC(pop_effect(gb));
  CYC(b_+24, SYM(insertBitsIntoSecretGenerationBuffer)); ret_effect(gb);
}

void insertBitsIntoSecretGenerationBuffer_hook(GB *gb) {
  BASE(insertBitsIntoSecretGenerationBuffer);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2); push_effect(gb, BC);
  CYC(b_+2, b_+3); C = A;
  for (;;) {
    CYC(b_+3, b_+6); SET_HL(w7SecretGenerationBuffer + 19);
    CYC(b_+6, b_+8); E = 0x14;
    CYC(b_+8, b_+10); C = alu_srl(gb, C);
    for (;;) {
      CYC(b_+10, b_+11); A = mem_rd(gb, HL);
      CYC(b_+11, b_+12); alu_rla(gb);
      CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(b_+13, b_+14); alu_rla(gb);
      CYC(b_+14, b_+15); alu_rla(gb);
      CYC(b_+15, b_+16); E = alu_dec8(gb, E);
      if (!(F & FZ)) { CYCT(b_+16, b_+18); continue; }
      CYC(b_+16, b_+18);
      break;
    }
    CYC(b_+18, b_+19); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+19, b_+21); continue; }
    CYC(b_+19, b_+21);
    break;
  }
  CYC(b_+21, b_+24); SET_HL(w7SecretGenerationBuffer);
  CYC(b_+24, b_+27); SET_DE((SYM(func_3ee4) + 48));
  for (;;) {
    CYC(b_+27, b_+28); A = mem_rd(gb, HL);
    CYC(b_+28, b_+29); alu_and(gb, D);
    CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+30, b_+31); E = alu_dec8(gb, E);
    if (!(F & FZ)) { CYCT(b_+31, b_+33); continue; }
    CYC(b_+31, b_+33);
    break;
  }
  CYC(b_+33, b_+34); SET_BC(pop_effect(gb));
  CYC(b_+34, b_+35); SET_HL(pop_effect(gb));
  CYC(b_+35, SYM(unpackSecret)); ret_effect(gb);
}

void unpackSecret_hook(GB *gb) {
  BASE(unpackSecret);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w7SecretText1);
  CYC(b_+3, b_+5); B = 0x40;
  CALL_C(b_+5, clearMemory_hook, SYM(clearMemory), b_+8);
  CALL_C(b_+8, andCWith3_hook, SYM(andCWith3), b_+11);
  CALL_C(b_+11, loadSecretBufferFromText_hook, SYM(loadSecretBufferFromText), b_+14);
  if (F & FC) {
    CYCT(b_+14, b_+16);
    unpackSecret__fail_hook(gb);
    return;
  }
  CYC(b_+14, b_+16);
  CALL_C(b_+16, runXorCipherOnSecretBuffer_hook, SYM(runXorCipherOnSecretBuffer), b_+19);
  CALL_C(b_+19, getNumCharactersForSecretType_hook, SYM(getNumCharactersForSecretType), b_+22);
  CYC(b_+22, b_+25); SET_HL(w7SecretGenerationBuffer - 1);
  CYC(b_+25, b_+26); add_a_to_hl_from_rst(gb, b_+26);
  CYC(b_+26, b_+27); A = mem_rd(gb, HL);
  CYC(b_+27, b_+29); alu_and(gb, 0x0f);
  CYC(b_+29, b_+30); E = A;
  CYC(b_+30, b_+31); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CALL_C(b_+32, getSecretBufferChecksum_hook, SYM(getSecretBufferChecksum), b_+35);
  CYC(b_+35, b_+36); alu_cp(gb, E);
  if (!(F & FZ)) {
    CYCT(b_+36, b_+38);
    unpackSecret__fail_hook(gb);
    return;
  }
  CYC(b_+36, b_+38);
  CALL_C(b_+38, unpackSecret__unpackSecretData_hook, b_+53, b_+41);
  CYC(b_+41, b_+44); A = mem_rd(gb, wTmpcec0 + 1);
  CYC(b_+44, b_+45); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(b_+45, b_+47);
    unpackSecret__fail_hook(gb);
    return;
  }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); B = 0x00;
  CYC(b_+49, b_+50); ret_effect(gb);
}

void unpackSecret__unpackSecretData_hook(GB *gb) {
  BASE(unpackSecret);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+53, b_+56); SET_DE(wTmpcec0);
  CYC(b_+56, b_+58); A = 0x04;
  CALL_C(b_+58, unpackSecret__unpack_hook, b_+62, b_+61);
  CYC(b_+61, b_+62); A = C;
  unpackSecret__unpack_hook(gb);
}

void unpackSecret__unpack_hook(GB *gb) {
  BASE(unpackSecret);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+62, b_+65); SET_HL(SYM(secretDataToEncodeTable));
  CYC(b_+65, b_+66); add_double_index_to_hl_from_rst(gb, b_+66);
  CYC(b_+66, b_+67); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+67, b_+68); H = mem_rd(gb, HL);
  CYC(b_+68, b_+69); L = A;
  CYC(b_+69, b_+70); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+70, b_+71); B = A;
  unpackSecret__unpack__nextEntry_hook(gb);
}

void unpackSecret__unpack__nextEntry_hook(GB *gb) {
  BASE(unpackSecret);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+71, b_+72); SET_HL(HL + 1);
    CYC(b_+72, b_+73); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CALL_C(b_+73, unpackSecret__readBits_hook, b_+82, b_+76);
    CYC(b_+76, b_+77); mem_wr(gb, DE, A);
    CYC(b_+77, b_+78); SET_DE(DE + 1);
    CYC(b_+78, b_+79); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+79, b_+81); continue; }
    CYC(b_+79, b_+81);
    break;
  }
  CYC(b_+81, b_+82); ret_effect(gb);
}

void unpackSecret__readBits_hook(GB *gb) {
  BASE(unpackSecret);
  CYC(b_+82, b_+83); push_effect(gb, BC);
  CYC(b_+83, b_+84); push_effect(gb, DE);
  CYC(b_+84, b_+85); push_effect(gb, HL);
  CYC(b_+85, b_+86); B = A;
  CYC(b_+86, b_+87); C = A;
  CYC(b_+87, b_+89); D = 0x00;
  for (;;) {
    CYC(b_+89, b_+92); SET_HL(w7SecretGenerationBuffer + 19);
    CYC(b_+92, b_+94); E = 0x14;
    for (;;) {
      CYC(b_+94, b_+96); mem_wr(gb, HL, alu_rl(gb, mem_rd(gb, HL)));
      CYC(b_+96, b_+97); A = mem_rd(gb, HL);
      CYC(b_+97, b_+98); alu_rla(gb);
      CYC(b_+98, b_+99); alu_rla(gb);
      CYC(b_+99, b_+100); SET_HL(HL - 1);
      CYC(b_+100, b_+101); E = alu_dec8(gb, E);
      if (!(F & FZ)) { CYCT(b_+101, b_+103); continue; }
      CYC(b_+101, b_+103);
      break;
    }
    CYC(b_+103, b_+105); D = alu_rr(gb, D);
    CYC(b_+105, b_+106); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+106, b_+108); continue; }
    CYC(b_+106, b_+108);
    break;
  }
  CYC(b_+108, b_+110); A = 0x08;
  CYC(b_+110, b_+111); alu_sub(gb, C);
  CYC(b_+111, b_+112); B = A;
  CYC(b_+112, b_+113); A = D;
  if (F & FZ) {
    CYCT(b_+113, b_+115);
  } else {
    CYC(b_+113, b_+115);
    for (;;) {
      CYC(b_+115, b_+116); alu_rrca(gb);
      CYC(b_+116, b_+117); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+117, b_+119); continue; }
      CYC(b_+117, b_+119);
      break;
    }
  }
  unpackSecret__readBits__end_hook(gb);
}

void unpackSecret__readBits__end_hook(GB *gb) {
  BASE(unpackSecret);
  CYC(b_+119, b_+120); SET_HL(pop_effect(gb));
  CYC(b_+120, b_+121); SET_DE(pop_effect(gb));
  CYC(b_+121, b_+122); SET_BC(pop_effect(gb));
  CYC(b_+122, SYM(loadUnpackedSecretData)); ret_effect(gb);
}

void loadUnpackedSecretData_hook(GB *gb) {
  BASE(loadUnpackedSecretData);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, andCWith3_hook, SYM(andCWith3), b_+3);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); secret_function_jump_table(gb);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+12) { loadUnpackedSecretData__type0_hook(gb); return; }
    else if (jt_ == b_+44) { loadUnpackedSecretData__type3_hook(gb); return; }
    else if (jt_ == b_+45) { loadUnpackedSecretData__type2_hook(gb); return; }
    else { hook_handoff(gb, HL); return; }
  } while (0);
}

void loadUnpackedSecretData__type0_hook(GB *gb) {
  BASE(loadUnpackedSecretData);
  CYC(b_+12, b_+15); SET_HL(SYM(secretDataToEncodeTable__entry0));
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+16, b_+17); B = A;
  CYC(b_+17, b_+20); SET_DE(wTmpcec0 + 4);
  for (;;) {
    CYC(b_+20, b_+21); A = mem_rd(gb, DE);
    CYC(b_+21, b_+22); push_effect(gb, DE);
    CYC(b_+22, b_+23); E = mem_rd(gb, HL);
    CYC(b_+23, b_+25); D = 0xc6;
    CYC(b_+25, b_+26); mem_wr(gb, DE, A);
    CYC(b_+26, b_+27); SET_DE(pop_effect(gb));
    CYC(b_+27, b_+28); SET_DE(DE + 1);
    CYC(b_+28, b_+29); SET_HL(HL + 1);
    CYC(b_+29, b_+30); SET_HL(HL + 1);
    CYC(b_+30, b_+31); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+31, b_+33); continue; }
    CYC(b_+31, b_+33);
    break;
  }
  CYC(b_+33, b_+36); SET_HL(wGameID);
  CYC(b_+36, b_+39); A = mem_rd(gb, wTmpcec0 + 2);
  CYC(b_+39, b_+40); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+40, b_+43); A = mem_rd(gb, wTmpcec0 + 3);
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  loadUnpackedSecretData__type3_hook(gb);
}

void loadUnpackedSecretData__type1_hook(GB *gb) {
  loadUnpackedSecretData__type0_hook(gb);
}

void loadUnpackedSecretData__type3_hook(GB *gb) {
  BASE(loadUnpackedSecretData);
  CYC(b_+44, b_+45); ret_effect(gb);
}

void loadUnpackedSecretData__type2_hook(GB *gb) {
  BASE(loadUnpackedSecretData);
  CYC(b_+45, b_+48); SET_HL((SYM(secretDataToEncodeTable__entry2) + 1));
  CYC(b_+48, b_+50); B = 0x08;
  CYC(b_+50, b_+53); SET_DE(wTmpcec0 + 4);
  for (;;) {
    CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+54, b_+55); push_effect(gb, HL);
    CYC(b_+55, b_+56); L = A;
    CYC(b_+56, b_+58); H = 0xc6;
    CYC(b_+58, b_+59); A = mem_rd(gb, DE);
    CYC(b_+59, b_+60); alu_or(gb, mem_rd(gb, HL));
    CYC(b_+60, b_+61); mem_wr(gb, HL, A);
    CYC(b_+61, b_+62); SET_HL(pop_effect(gb));
    CYC(b_+62, b_+63); SET_DE(DE + 1);
    CYC(b_+63, b_+64); SET_HL(HL + 1);
    CYC(b_+64, b_+65); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+65, b_+67); continue; }
    CYC(b_+65, b_+67);
    break;
  }
  CYC(b_+67, SYM(verifyUnpackedSecretGameID)); ret_effect(gb);
}

void verifyUnpackedSecretGameID_hook(GB *gb) {
  BASE(verifyUnpackedSecretGameID);
  CYC(b_+0, b_+3); SET_HL(wTmpcec0 + 2);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); D = mem_rd(gb, HL);
  CYC(b_+5, b_+6); E = A;
  CYC(b_+6, b_+7); alu_or(gb, D);
  if (F & FZ) {
    CYCT(b_+7, b_+9);
    verifyUnpackedSecretGameID_success(gb);
    return;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_HL(wGameID);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); alu_cp(gb, E);
  if (!(F & FZ)) {
    CYCT(b_+14, b_+16);
    verifyUnpackedSecretGameID_fail(gb);
    return;
  }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); alu_cp(gb, D);
  if (F & FZ) {
    CYCT(b_+18, b_+20);
    verifyUnpackedSecretGameID_success(gb);
    return;
  }
  CYC(b_+18, b_+20);
  verifyUnpackedSecretGameID_fail(gb);
}

void generateGameIDIfNeeded_hook(GB *gb) {
  BASE(generateGameIDIfNeeded);
  CYC(b_+0, b_+3); SET_HL(wGameID);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+5); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); L = 0x23;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+9, b_+11); alu_and(gb, 0x7f);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  if (!(F & FZ)) {
    CYCT(b_+13, b_+15);
  } else {
    CYC(b_+13, b_+15);
    for (;;) {
      CYC(b_+15, b_+16); alu_or(gb, A);
      if (!(F & FZ)) {
        CYCT(b_+16, b_+18);
        break;
      }
      CYC(b_+16, b_+18);
      CYC(b_+18, b_+20); A = hram_rd(gb, 0x04);
      CYC(b_+20, b_+22);
    }
  }
  CYC(b_+22, b_+24); L = 0x00;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+26); mem_wr(gb, HL, B);
  CYC(b_+26, SYM(convertSecretBufferToText)); ret_effect(gb);
}

void convertSecretBufferToText_hook(GB *gb) {
  BASE(convertSecretBufferToText);
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+4); SET_HL(b_+41);
  CYC(b_+4, b_+5); add_double_index_to_hl_from_rst(gb, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); B = mem_rd(gb, HL);
  CYC(b_+7, b_+8); C = A;
  CYC(b_+8, b_+11); SET_DE(w7SecretGenerationBuffer);
  CYC(b_+11, b_+14); SET_HL(w7SecretText1);
  for (;;) {
    CYC(b_+14, b_+15); A = mem_rd(gb, BC);
    CYC(b_+15, b_+17); alu_and(gb, 0x0f);
    if (F & FZ) {
      CYCT(b_+17, b_+18); ret_effect(gb);
      return;
    }
    CYC(b_+17, b_+18);
    CYC(b_+18, b_+19); push_effect(gb, BC);
    CYC(b_+19, b_+20); B = A;
    for (;;) {
      CYC(b_+20, b_+21); A = mem_rd(gb, DE);
      CYC(b_+21, b_+22); push_effect(gb, HL);
      CYC(b_+22, b_+25); SET_HL(0x00a1);
      CYC(b_+25, b_+26); add_a_to_hl_from_rst(gb, b_+26);
      CYC(b_+26, b_+27); A = mem_rd(gb, HL);
      CYC(b_+27, b_+28); SET_HL(pop_effect(gb));
      CYC(b_+28, b_+29); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+29, b_+30); SET_DE(DE + 1);
      CYC(b_+30, b_+31); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+31, b_+33); continue; }
      CYC(b_+31, b_+33);
      break;
    }
    CYC(b_+33, b_+34); SET_BC(pop_effect(gb));
    CYC(b_+34, b_+35); A = mem_rd(gb, BC);
    CYC(b_+35, b_+37); alu_and(gb, 0xf0);
    CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+38, b_+39); SET_BC(BC + 1);
    CYC(b_+39, b_+41);
  }
}

void loadSecretBufferFromText_hook(GB *gb) {
  BASE(loadSecretBufferFromText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getNumCharactersForSecretType_hook, SYM(getNumCharactersForSecretType), b_+3);
  CYC(b_+3, b_+6); SET_HL(wTmpcec0);
  CYC(b_+6, b_+9); SET_DE(w7SecretGenerationBuffer);
  for (;;) {
    CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CALL_C(b_+10, loadSecretBufferFromText__textCharacterToByte_hook, b_+20, b_+13);
    if (F & FC) {
      CYCT(b_+13, b_+14); ret_effect(gb);
      return;
    }
    CYC(b_+13, b_+14);
    CYC(b_+14, b_+15); mem_wr(gb, DE, A);
    CYC(b_+15, b_+16); SET_DE(DE + 1);
    CYC(b_+16, b_+17); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+17, b_+19); continue; }
    CYC(b_+17, b_+19);
    break;
  }
  CYC(b_+19, b_+20); ret_effect(gb);
}

void loadSecretBufferFromText__textCharacterToByte_hook(GB *gb) {
  BASE(loadSecretBufferFromText);
  CYC(b_+20, b_+21); push_effect(gb, HL);
  CYC(b_+21, b_+22); push_effect(gb, BC);
  CYC(b_+22, b_+25); SET_HL(0x00a1);
  CYC(b_+25, b_+28); SET_BC(SYM(init));
  for (;;) {
    CYC(b_+28, b_+29); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(b_+29, b_+31);
      loadSecretBufferFromText__end_hook(gb);
      return;
    }
    CYC(b_+29, b_+31);
    CYC(b_+31, b_+32); SET_HL(HL + 1);
    CYC(b_+32, b_+33); C = alu_inc8(gb, C);
    CYC(b_+33, b_+34); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+34, b_+36); continue; }
    CYC(b_+34, b_+36);
    CYC(b_+36, b_+37); alu_scf(gb);
    loadSecretBufferFromText__end_hook(gb);
    return;
  }
}

void loadSecretBufferFromText__end_hook(GB *gb) {
  BASE(loadSecretBufferFromText);
  CYC(b_+37, b_+38); A = C;
  CYC(b_+38, b_+39); SET_BC(pop_effect(gb));
  CYC(b_+39, b_+40); SET_HL(pop_effect(gb));
  CYC(b_+40, SYM(runXorCipherOnSecretBuffer)); ret_effect(gb);
}

void runXorCipherOnSecretBuffer_hook(GB *gb) {
  BASE(runXorCipherOnSecretBuffer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getNumCharactersForSecretType_hook, SYM(getNumCharactersForSecretType), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, w7SecretGenerationBuffer);
  CYC(b_+6, b_+8); alu_and(gb, 0x38);
  CYC(b_+8, b_+9); alu_rrca(gb);
  CYC(b_+9, b_+12); SET_DE(SYM(secretXorCipher));
  CALL_C(b_+12, addAToDe_hook, 0x0068, b_+15);
  CYC(b_+15, b_+18); SET_HL(w7SecretGenerationBuffer);
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_and(gb, 0x07);
  for (;;) {
    CYC(b_+21, b_+22); alu_xor(gb, mem_rd(gb, HL));
    CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+23, b_+24); SET_DE(DE + 1);
    CYC(b_+24, b_+25); A = mem_rd(gb, DE);
    CYC(b_+25, b_+26); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+26, b_+28); continue; }
    CYC(b_+26, b_+28);
    break;
  }
  CYC(b_+28, SYM(getSecretBufferChecksum)); ret_effect(gb);
}

void getSecretBufferChecksum_hook(GB *gb) {
  BASE(getSecretBufferChecksum);
  CYC(b_+0, b_+3); SET_HL(w7SecretGenerationBuffer);
  CYC(b_+3, b_+5); B = 0x14;
  CYC(b_+5, b_+6); alu_xor(gb, A);
  for (;;) {
    CYC(b_+6, b_+7); alu_add(gb, mem_rd(gb, HL));
    CYC(b_+7, b_+8); SET_HL(HL + 1);
    CYC(b_+8, b_+9); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+9, b_+11); continue; }
    CYC(b_+9, b_+11);
    break;
  }
  CYC(b_+11, b_+13); alu_and(gb, 0x0f);
  CYC(b_+13, SYM(shiftSecretBufferContentsToFront)); ret_effect(gb);
}

void shiftSecretBufferContentsToFront_hook(GB *gb) {
  BASE(shiftSecretBufferContentsToFront);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getNumCharactersForSecretType_hook, SYM(getNumCharactersForSecretType), b_+3);
  CYC(b_+3, b_+5); A = 0x14;
  CYC(b_+5, b_+6); alu_sub(gb, B);
  if (F & FZ) {
    CYCT(b_+6, b_+7); ret_effect(gb);
    return;
  }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+10); SET_DE(w7SecretGenerationBuffer);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); add_a_to_hl_from_rst(gb, b_+13);
  for (;;) {
    CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+14, b_+15); mem_wr(gb, DE, A);
    CYC(b_+15, b_+16); SET_DE(DE + 1);
    CYC(b_+16, b_+17); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+17, b_+19); continue; }
    CYC(b_+17, b_+19);
    break;
  }
  CYC(b_+19, SYM(andCWith3)); ret_effect(gb);
}

void andCWith3_hook(GB *gb) {
  BASE(andCWith3);
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); alu_and(gb, 0x03);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, SYM(secretDataToEncodeTable)); ret_effect(gb);
}

void getNumCharactersForSecretType_hook(GB *gb) {
  BASE(getNumCharactersForSecretType);
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+4); SET_HL(b_+8);
  CYC(b_+4, b_+5); add_a_to_hl_from_rst(gb, b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+8); ret_effect(gb);
}
