#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x03, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x03, (from), (to), true)

void generateGameTransferSecret_hook(GB *gb);
void secretFunctionCaller_body_hook(GB *gb);
void generateSecret_hook(GB *gb);
void insertBitsIntoSecretGenerationBuffer_hook(GB *gb);
void unpackSecret_hook(GB *gb);
void verifyUnpackedSecretGameID_hook(GB *gb);
void runXorCipherOnSecretBuffer_hook(GB *gb);
void getSecretBufferChecksum_hook(GB *gb);

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

static void unpackSecret_fail(GB *gb) {
  CYC(0x4917, 0x4919); B = 0x01;
  CYC(0x4919, 0x491a); ret_effect(gb);
}

static void verifyUnpackedSecretGameID_fail(GB *gb) {
  CYC(0x49b8, 0x49ba); B = 0x01;
  CYC(0x49ba, 0x49bb); ret_effect(gb);
}

static void verifyUnpackedSecretGameID_success(GB *gb) {
  CYC(0x49bb, 0x49bd); B = 0x00;
  CYC(0x49bd, 0x49be); ret_effect(gb);
}

void generateGameTransferSecret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x481b, 0x481e); SET_HL(wFileIsLinkedGame);
  CYC(0x481e, 0x481f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x481f, 0x4820); B = mem_rd(gb, HL);
  CYC(0x4820, 0x4821); C = A;
  CYC(0x4821, 0x4822); push_effect(gb, BC);
  CYC(0x4822, 0x4823); alu_or(gb, B);
  CYC(0x4823, 0x4824); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x4824, 0x4826); alu_xor(gb, 0x01);
  CYC(0x4826, 0x4827); alu_or(gb, B);
  CYC(0x4827, 0x4828); mem_wr(gb, HL, A);
  CYC(0x4828, 0x482b); SET_BC(0x0000);
  CALL_C(0x482b, secretFunctionCaller_body_hook, 0x4836, 0x482e);
  CYC(0x482e, 0x482f); SET_BC(pop_effect(gb));
  CYC(0x482f, 0x4832); SET_HL(wFileIsLinkedGame);
  CYC(0x4832, 0x4833); mem_wr(gb, HL, C);
  CYC(0x4833, 0x4834); L = alu_inc8(gb, L);
  CYC(0x4834, 0x4835); mem_wr(gb, HL, B);
  CYC(0x4835, 0x4836); ret_effect(gb);
}

void secretFunctionCaller_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4836, 0x4837); push_effect(gb, DE);
  CYC(0x4837, 0x4839); A = hram_rd(gb, 0x70);
  CYC(0x4839, 0x483a); push_effect(gb, AF);
  CYC(0x483a, 0x483c); A = 0x07;
  CYC(0x483c, 0x483e); hram_wr(gb, 0x70, A);
  CYC(0x483e, 0x4841); push_effect(gb, 0x4841);
  CYC(0x4846, 0x4847); A = B;
  CYC(0x4847, 0x4848); push_effect(gb, 0x4848);
  switch (secret_function_jump_table(gb)) {
    case 0x4852: generateSecret_hook(gb); break;
    case 0x48e5: unpackSecret_hook(gb); break;
    case 0x49a4: verifyUnpackedSecretGameID_hook(gb); break;
    case 0x49be: generateGameIDIfNeeded(gb); break;
    case 0x4960: loadUnpackedSecretData(gb); break;
    default: hook_handoff(gb, HL); return;
  }
  CYC(0x4841, 0x4842); SET_AF(pop_effect(gb));
  CYC(0x4842, 0x4844); hram_wr(gb, 0x70, A);
  CYC(0x4844, 0x4845); SET_DE(pop_effect(gb));
  CYC(0x4845, 0x4846); ret_effect(gb);
}

void generateSecret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4852, 0x4855); SET_HL(w7SecretText1);
  CYC(0x4855, 0x4857); B = 0x40;
  CALL_C(0x4857, clearMemory_hook, 0x046f, 0x485a);
  CALL_C(0x485a, andCWith3, 0x4a7d, 0x485d);
  CALL_C(0x485d, generateGameIDIfNeeded, 0x49be, 0x4860);
  CALL_ROM(0x4860, 0x4887);
  CYC(0x4863, 0x4866); SET_HL(wSecretXorCipherIndex);
  CYC(0x4866, 0x4867); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4867, 0x4868); mem_wr(gb, HL, C);
  CYC(0x4868, 0x486a); A = 0x04;
  CALL_C(0x486a, encodeSecretData, 0x48a8, 0x486d);
  CALL_C(0x486d, encodeSecretData_paramC, 0x48a7, 0x4870);
  CYC(0x4870, 0x4872); B = 0x04;
  CYC(0x4872, 0x4873); alu_xor(gb, A);
  CALL_C(0x4873, insertBitsIntoSecretGenerationBuffer_hook, 0x48c1, 0x4876);
  CALL_C(0x4876, getSecretBufferChecksum_hook, 0x4a5b, 0x4879);
  CYC(0x4879, 0x487c); SET_HL(w7SecretGenerationBuffer + 19);
  CYC(0x487c, 0x487d); alu_or(gb, mem_rd(gb, HL));
  CYC(0x487d, 0x487e); mem_wr(gb, HL, A);
  CALL_C(0x487e, shiftSecretBufferContentsToFront, 0x4a69, 0x4881);
  CALL_C(0x4881, runXorCipherOnSecretBuffer_hook, 0x4a3e, 0x4884);
  CYC(0x4884, 0x4887); convertSecretBufferToText(gb);
}

void insertBitsIntoSecretGenerationBuffer_hook(GB *gb) {
  CYC(0x48c1, 0x48c2); push_effect(gb, HL);
  CYC(0x48c2, 0x48c3); push_effect(gb, BC);
  CYC(0x48c3, 0x48c4); C = A;
  for (;;) {
    CYC(0x48c4, 0x48c7); SET_HL(w7SecretGenerationBuffer + 19);
    CYC(0x48c7, 0x48c9); E = 0x14;
    CYC(0x48c9, 0x48cb); C = alu_srl(gb, C);
    for (;;) {
      CYC(0x48cb, 0x48cc); A = mem_rd(gb, HL);
      CYC(0x48cc, 0x48cd); alu_rla(gb);
      CYC(0x48cd, 0x48ce); mem_wr(gb, HL, A); SET_HL(HL - 1);
      CYC(0x48ce, 0x48cf); alu_rla(gb);
      CYC(0x48cf, 0x48d0); alu_rla(gb);
      CYC(0x48d0, 0x48d1); E = alu_dec8(gb, E);
      if (!(F & FZ)) { CYCT(0x48d1, 0x48d3); continue; }
      CYC(0x48d1, 0x48d3);
      break;
    }
    CYC(0x48d3, 0x48d4); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x48d4, 0x48d6); continue; }
    CYC(0x48d4, 0x48d6);
    break;
  }
  CYC(0x48d6, 0x48d9); SET_HL(w7SecretGenerationBuffer);
  CYC(0x48d9, 0x48dc); SET_DE(0x3f14);
  for (;;) {
    CYC(0x48dc, 0x48dd); A = mem_rd(gb, HL);
    CYC(0x48dd, 0x48de); alu_and(gb, D);
    CYC(0x48de, 0x48df); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x48df, 0x48e0); E = alu_dec8(gb, E);
    if (!(F & FZ)) { CYCT(0x48e0, 0x48e2); continue; }
    CYC(0x48e0, 0x48e2);
    break;
  }
  CYC(0x48e2, 0x48e3); SET_BC(pop_effect(gb));
  CYC(0x48e3, 0x48e4); SET_HL(pop_effect(gb));
  CYC(0x48e4, 0x48e5); ret_effect(gb);
}

void unpackSecret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x48e5, 0x48e8); SET_HL(w7SecretText1);
  CYC(0x48e8, 0x48ea); B = 0x40;
  CALL_C(0x48ea, clearMemory_hook, 0x046f, 0x48ed);
  CALL_C(0x48ed, andCWith3, 0x4a7d, 0x48f0);
  CALL_C(0x48f0, loadSecretBufferFromText, 0x4a15, 0x48f3);
  if (F & FC) {
    CYCT(0x48f3, 0x48f5);
    unpackSecret_fail(gb);
    return;
  }
  CYC(0x48f3, 0x48f5);
  CALL_C(0x48f5, runXorCipherOnSecretBuffer_hook, 0x4a3e, 0x48f8);
  CALL_C(0x48f8, getNumCharactersForSecretType, 0x4ace, 0x48fb);
  CYC(0x48fb, 0x48fe); SET_HL(w7SecretGenerationBuffer - 1);
  CYC(0x48fe, 0x48ff); add_a_to_hl_from_rst(gb, 0x48ff);
  CYC(0x48ff, 0x4900); A = mem_rd(gb, HL);
  CYC(0x4900, 0x4902); alu_and(gb, 0x0f);
  CYC(0x4902, 0x4903); E = A;
  CYC(0x4903, 0x4904); alu_xor(gb, mem_rd(gb, HL));
  CYC(0x4904, 0x4905); mem_wr(gb, HL, A);
  CALL_C(0x4905, getSecretBufferChecksum_hook, 0x4a5b, 0x4908);
  CYC(0x4908, 0x4909); alu_cp(gb, E);
  if (!(F & FZ)) {
    CYCT(0x4909, 0x490b);
    unpackSecret_fail(gb);
    return;
  }
  CYC(0x4909, 0x490b);
  CALL_ROM(0x490b, 0x491a);
  CYC(0x490e, 0x4911); A = mem_rd(gb, wTmpcec0 + 1);
  CYC(0x4911, 0x4912); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(0x4912, 0x4914);
    unpackSecret_fail(gb);
    return;
  }
  CYC(0x4912, 0x4914);
  CYC(0x4914, 0x4916); B = 0x00;
  CYC(0x4916, 0x4917); ret_effect(gb);
}

void verifyUnpackedSecretGameID_hook(GB *gb) {
  CYC(0x49a4, 0x49a7); SET_HL(wTmpcec0 + 2);
  CYC(0x49a7, 0x49a8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49a8, 0x49a9); D = mem_rd(gb, HL);
  CYC(0x49a9, 0x49aa); E = A;
  CYC(0x49aa, 0x49ab); alu_or(gb, D);
  if (F & FZ) {
    CYCT(0x49ab, 0x49ad);
    verifyUnpackedSecretGameID_success(gb);
    return;
  }
  CYC(0x49ab, 0x49ad);
  CYC(0x49ad, 0x49b0); SET_HL(wGameID);
  CYC(0x49b0, 0x49b1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49b1, 0x49b2); alu_cp(gb, E);
  if (!(F & FZ)) {
    CYCT(0x49b2, 0x49b4);
    verifyUnpackedSecretGameID_fail(gb);
    return;
  }
  CYC(0x49b2, 0x49b4);
  CYC(0x49b4, 0x49b5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49b5, 0x49b6); alu_cp(gb, D);
  if (F & FZ) {
    CYCT(0x49b6, 0x49b8);
    verifyUnpackedSecretGameID_success(gb);
    return;
  }
  CYC(0x49b6, 0x49b8);
  verifyUnpackedSecretGameID_fail(gb);
}

void runXorCipherOnSecretBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4a3e, getNumCharactersForSecretType, 0x4ace, 0x4a41);
  CYC(0x4a41, 0x4a44); A = mem_rd(gb, w7SecretGenerationBuffer);
  CYC(0x4a44, 0x4a46); alu_and(gb, 0x38);
  CYC(0x4a46, 0x4a47); alu_rrca(gb);
  CYC(0x4a47, 0x4a4a); SET_DE(0x4ada);
  CALL_C(0x4a4a, addAToDe_hook, 0x0068, 0x4a4d);
  CYC(0x4a4d, 0x4a50); SET_HL(w7SecretGenerationBuffer);
  CYC(0x4a50, 0x4a51); A = mem_rd(gb, DE);
  CYC(0x4a51, 0x4a53); alu_and(gb, 0x07);
  for (;;) {
    CYC(0x4a53, 0x4a54); alu_xor(gb, mem_rd(gb, HL));
    CYC(0x4a54, 0x4a55); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x4a55, 0x4a56); SET_DE(DE + 1);
    CYC(0x4a56, 0x4a57); A = mem_rd(gb, DE);
    CYC(0x4a57, 0x4a58); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x4a58, 0x4a5a); continue; }
    CYC(0x4a58, 0x4a5a);
    break;
  }
  CYC(0x4a5a, 0x4a5b); ret_effect(gb);
}

void getSecretBufferChecksum_hook(GB *gb) {
  CYC(0x4a5b, 0x4a5e); SET_HL(w7SecretGenerationBuffer);
  CYC(0x4a5e, 0x4a60); B = 0x14;
  CYC(0x4a60, 0x4a61); alu_xor(gb, A);
  for (;;) {
    CYC(0x4a61, 0x4a62); alu_add(gb, mem_rd(gb, HL));
    CYC(0x4a62, 0x4a63); SET_HL(HL + 1);
    CYC(0x4a63, 0x4a64); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x4a64, 0x4a66); continue; }
    CYC(0x4a64, 0x4a66);
    break;
  }
  CYC(0x4a66, 0x4a68); alu_and(gb, 0x0f);
  CYC(0x4a68, 0x4a69); ret_effect(gb);
}
