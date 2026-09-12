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

void unpackSecret__fail_hook(GB *gb) {
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
    case 0x49be: generateGameIDIfNeeded_hook(gb); break;
    case 0x4960: loadUnpackedSecretData_hook(gb); break;
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
  CALL_C(0x485a, andCWith3_hook, 0x4a7d, 0x485d);
  CALL_C(0x485d, generateGameIDIfNeeded_hook, 0x49be, 0x4860);
  CALL_C(0x4860, generateSecret__determineXorCipher_hook, 0x4887, 0x4863);
  CYC(0x4863, 0x4866); SET_HL(wSecretXorCipherIndex);
  CYC(0x4866, 0x4867); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4867, 0x4868); mem_wr(gb, HL, C);
  CYC(0x4868, 0x486a); A = 0x04;
  CALL_C(0x486a, encodeSecretData_hook, 0x48a8, 0x486d);
  CALL_C(0x486d, encodeSecretData_paramC_hook, 0x48a7, 0x4870);
  CYC(0x4870, 0x4872); B = 0x04;
  CYC(0x4872, 0x4873); alu_xor(gb, A);
  CALL_C(0x4873, insertBitsIntoSecretGenerationBuffer_hook, 0x48c1, 0x4876);
  CALL_C(0x4876, getSecretBufferChecksum_hook, 0x4a5b, 0x4879);
  CYC(0x4879, 0x487c); SET_HL(w7SecretGenerationBuffer + 19);
  CYC(0x487c, 0x487d); alu_or(gb, mem_rd(gb, HL));
  CYC(0x487d, 0x487e); mem_wr(gb, HL, A);
  CALL_C(0x487e, shiftSecretBufferContentsToFront_hook, 0x4a69, 0x4881);
  CALL_C(0x4881, runXorCipherOnSecretBuffer_hook, 0x4a3e, 0x4884);
  CYC(0x4884, 0x4887); convertSecretBufferToText_hook(gb);
}

void generateSecret__ret_hook(GB *gb) {
  CYC(0x48a3, 0x48a5); alu_and(gb, 0x07);
  CYC(0x48a5, 0x48a6); SET_BC(pop_effect(gb));
  CYC(0x48a6, 0x48a7); ret_effect(gb);
}

void generateSecret__determineXorCipher_hook(GB *gb) {
  CYC(0x4887, 0x4888); push_effect(gb, BC);
  CYC(0x4888, 0x488b); SET_HL(wGameID);
  CYC(0x488b, 0x488c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x488c, 0x488d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x488d, 0x488e); B = A;
  CYC(0x488e, 0x488f); A = C;
  CYC(0x488f, 0x4891); alu_cp(gb, 0x03);
  CYC(0x4891, 0x4892); A = B;
  if (!(F & FZ)) {
    CYCT(0x4892, 0x4894);
    generateSecret__ret_hook(gb);
    return;
  }
  CYC(0x4892, 0x4894);
  CYC(0x4894, 0x4896); L = 0xfb;
  CYC(0x4896, 0x4897); A = mem_rd(gb, HL);
  CYC(0x4897, 0x4899); A = alu_swap(gb, A);
  CYC(0x4899, 0x489b); alu_and(gb, 0x0f);
  CYC(0x489b, 0x489c); alu_add(gb, B);
  CYC(0x489c, 0x489d); B = A;
  CYC(0x489d, 0x489e); A = mem_rd(gb, HL);
  CYC(0x489e, 0x48a0); alu_and(gb, 0x01);
  CYC(0x48a0, 0x48a1); alu_rlca(gb);
  CYC(0x48a1, 0x48a2); alu_rlca(gb);
  CYC(0x48a2, 0x48a3); alu_xor(gb, B);
  generateSecret__ret_hook(gb);
}

void encodeSecretData_paramC_hook(GB *gb) {
  CYC(0x48a7, 0x48a8); A = C;
  encodeSecretData_hook(gb);
}

void encodeSecretData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x48a8, 0x48a9); push_effect(gb, BC);
  CYC(0x48a9, 0x48ac); SET_HL(0x4a82);
  CYC(0x48ac, 0x48ad); add_double_index_to_hl_from_rst(gb, 0x48ad);
  CYC(0x48ad, 0x48ae); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x48ae, 0x48af); H = mem_rd(gb, HL);
  CYC(0x48af, 0x48b0); L = A;
  CYC(0x48b0, 0x48b1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x48b1, 0x48b2); C = A;
  for (;;) {
    CYC(0x48b2, 0x48b3); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x48b3, 0x48b4); E = A;
    CYC(0x48b4, 0x48b5); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x48b5, 0x48b6); B = A;
    CYC(0x48b6, 0x48b8); D = 0xc6;
    CYC(0x48b8, 0x48b9); A = mem_rd(gb, DE);
    CALL_C(0x48b9, insertBitsIntoSecretGenerationBuffer_hook, 0x48c1, 0x48bc);
    CYC(0x48bc, 0x48bd); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(0x48bd, 0x48bf); continue; }
    CYC(0x48bd, 0x48bf);
    break;
  }
  CYC(0x48bf, 0x48c0); SET_BC(pop_effect(gb));
  CYC(0x48c0, 0x48c1); ret_effect(gb);
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
  CALL_C(0x48ed, andCWith3_hook, 0x4a7d, 0x48f0);
  CALL_C(0x48f0, loadSecretBufferFromText_hook, 0x4a15, 0x48f3);
  if (F & FC) {
    CYCT(0x48f3, 0x48f5);
    unpackSecret__fail_hook(gb);
    return;
  }
  CYC(0x48f3, 0x48f5);
  CALL_C(0x48f5, runXorCipherOnSecretBuffer_hook, 0x4a3e, 0x48f8);
  CALL_C(0x48f8, getNumCharactersForSecretType_hook, 0x4ace, 0x48fb);
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
    unpackSecret__fail_hook(gb);
    return;
  }
  CYC(0x4909, 0x490b);
  CALL_C(0x490b, unpackSecret__unpackSecretData_hook, 0x491a, 0x490e);
  CYC(0x490e, 0x4911); A = mem_rd(gb, wTmpcec0 + 1);
  CYC(0x4911, 0x4912); alu_cp(gb, C);
  if (!(F & FZ)) {
    CYCT(0x4912, 0x4914);
    unpackSecret__fail_hook(gb);
    return;
  }
  CYC(0x4912, 0x4914);
  CYC(0x4914, 0x4916); B = 0x00;
  CYC(0x4916, 0x4917); ret_effect(gb);
}

void unpackSecret__unpackSecretData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x491a, 0x491d); SET_DE(wTmpcec0);
  CYC(0x491d, 0x491f); A = 0x04;
  CALL_C(0x491f, unpackSecret__unpack_hook, 0x4923, 0x4922);
  CYC(0x4922, 0x4923); A = C;
  unpackSecret__unpack_hook(gb);
}

void unpackSecret__unpack_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4923, 0x4926); SET_HL(0x4a82);
  CYC(0x4926, 0x4927); add_double_index_to_hl_from_rst(gb, 0x4927);
  CYC(0x4927, 0x4928); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4928, 0x4929); H = mem_rd(gb, HL);
  CYC(0x4929, 0x492a); L = A;
  CYC(0x492a, 0x492b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x492b, 0x492c); B = A;
  unpackSecret__unpack__nextEntry_hook(gb);
}

void unpackSecret__unpack__nextEntry_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x492c, 0x492d); SET_HL(HL + 1);
    CYC(0x492d, 0x492e); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CALL_C(0x492e, unpackSecret__readBits_hook, 0x4937, 0x4931);
    CYC(0x4931, 0x4932); mem_wr(gb, DE, A);
    CYC(0x4932, 0x4933); SET_DE(DE + 1);
    CYC(0x4933, 0x4934); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x4934, 0x4936); continue; }
    CYC(0x4934, 0x4936);
    break;
  }
  CYC(0x4936, 0x4937); ret_effect(gb);
}

void unpackSecret__readBits_hook(GB *gb) {
  CYC(0x4937, 0x4938); push_effect(gb, BC);
  CYC(0x4938, 0x4939); push_effect(gb, DE);
  CYC(0x4939, 0x493a); push_effect(gb, HL);
  CYC(0x493a, 0x493b); B = A;
  CYC(0x493b, 0x493c); C = A;
  CYC(0x493c, 0x493e); D = 0x00;
  for (;;) {
    CYC(0x493e, 0x4941); SET_HL(w7SecretGenerationBuffer + 19);
    CYC(0x4941, 0x4943); E = 0x14;
    for (;;) {
      CYC(0x4943, 0x4945); mem_wr(gb, HL, alu_rl(gb, mem_rd(gb, HL)));
      CYC(0x4945, 0x4946); A = mem_rd(gb, HL);
      CYC(0x4946, 0x4947); alu_rla(gb);
      CYC(0x4947, 0x4948); alu_rla(gb);
      CYC(0x4948, 0x4949); SET_HL(HL - 1);
      CYC(0x4949, 0x494a); E = alu_dec8(gb, E);
      if (!(F & FZ)) { CYCT(0x494a, 0x494c); continue; }
      CYC(0x494a, 0x494c);
      break;
    }
    CYC(0x494c, 0x494e); D = alu_rr(gb, D);
    CYC(0x494e, 0x494f); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x494f, 0x4951); continue; }
    CYC(0x494f, 0x4951);
    break;
  }
  CYC(0x4951, 0x4953); A = 0x08;
  CYC(0x4953, 0x4954); alu_sub(gb, C);
  CYC(0x4954, 0x4955); B = A;
  CYC(0x4955, 0x4956); A = D;
  if (F & FZ) {
    CYCT(0x4956, 0x4958);
  } else {
    CYC(0x4956, 0x4958);
    for (;;) {
      CYC(0x4958, 0x4959); alu_rrca(gb);
      CYC(0x4959, 0x495a); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(0x495a, 0x495c); continue; }
      CYC(0x495a, 0x495c);
      break;
    }
  }
  unpackSecret__readBits__end_hook(gb);
}

void unpackSecret__readBits__end_hook(GB *gb) {
  CYC(0x495c, 0x495d); SET_HL(pop_effect(gb));
  CYC(0x495d, 0x495e); SET_DE(pop_effect(gb));
  CYC(0x495e, 0x495f); SET_BC(pop_effect(gb));
  CYC(0x495f, 0x4960); ret_effect(gb);
}

void loadUnpackedSecretData_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4960, andCWith3_hook, 0x4a7d, 0x4963);
  CYC(0x4963, 0x4964); push_effect(gb, 0x4964); secret_function_jump_table(gb);
  switch (HL) {
    case 0x496c: loadUnpackedSecretData__type0_hook(gb); return;
    case 0x498c: loadUnpackedSecretData__type3_hook(gb); return;
    case 0x498d: loadUnpackedSecretData__type2_hook(gb); return;
    default: hook_handoff(gb, HL); return;
  }
}

void loadUnpackedSecretData__type0_hook(GB *gb) {
  CYC(0x496c, 0x496f); SET_HL(0x4a95);
  CYC(0x496f, 0x4970); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4970, 0x4971); B = A;
  CYC(0x4971, 0x4974); SET_DE(wTmpcec0 + 4);
  for (;;) {
    CYC(0x4974, 0x4975); A = mem_rd(gb, DE);
    CYC(0x4975, 0x4976); push_effect(gb, DE);
    CYC(0x4976, 0x4977); E = mem_rd(gb, HL);
    CYC(0x4977, 0x4979); D = 0xc6;
    CYC(0x4979, 0x497a); mem_wr(gb, DE, A);
    CYC(0x497a, 0x497b); SET_DE(pop_effect(gb));
    CYC(0x497b, 0x497c); SET_DE(DE + 1);
    CYC(0x497c, 0x497d); SET_HL(HL + 1);
    CYC(0x497d, 0x497e); SET_HL(HL + 1);
    CYC(0x497e, 0x497f); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x497f, 0x4981); continue; }
    CYC(0x497f, 0x4981);
    break;
  }
  CYC(0x4981, 0x4984); SET_HL(wGameID);
  CYC(0x4984, 0x4987); A = mem_rd(gb, wTmpcec0 + 2);
  CYC(0x4987, 0x4988); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4988, 0x498b); A = mem_rd(gb, wTmpcec0 + 3);
  CYC(0x498b, 0x498c); mem_wr(gb, HL, A);
  loadUnpackedSecretData__type3_hook(gb);
}

void loadUnpackedSecretData__type1_hook(GB *gb) {
  loadUnpackedSecretData__type0_hook(gb);
}

void loadUnpackedSecretData__type3_hook(GB *gb) {
  CYC(0x498c, 0x498d); ret_effect(gb);
}

void loadUnpackedSecretData__type2_hook(GB *gb) {
  CYC(0x498d, 0x4990); SET_HL(0x4ab9);
  CYC(0x4990, 0x4992); B = 0x08;
  CYC(0x4992, 0x4995); SET_DE(wTmpcec0 + 4);
  for (;;) {
    CYC(0x4995, 0x4996); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4996, 0x4997); push_effect(gb, HL);
    CYC(0x4997, 0x4998); L = A;
    CYC(0x4998, 0x499a); H = 0xc6;
    CYC(0x499a, 0x499b); A = mem_rd(gb, DE);
    CYC(0x499b, 0x499c); alu_or(gb, mem_rd(gb, HL));
    CYC(0x499c, 0x499d); mem_wr(gb, HL, A);
    CYC(0x499d, 0x499e); SET_HL(pop_effect(gb));
    CYC(0x499e, 0x499f); SET_DE(DE + 1);
    CYC(0x499f, 0x49a0); SET_HL(HL + 1);
    CYC(0x49a0, 0x49a1); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x49a1, 0x49a3); continue; }
    CYC(0x49a1, 0x49a3);
    break;
  }
  CYC(0x49a3, 0x49a4); ret_effect(gb);
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

void generateGameIDIfNeeded_hook(GB *gb) {
  CYC(0x49be, 0x49c1); SET_HL(wGameID);
  CYC(0x49c1, 0x49c2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49c2, 0x49c3); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x49c3, 0x49c4); ret_effect(gb);
    return;
  }
  CYC(0x49c3, 0x49c4);
  CYC(0x49c4, 0x49c6); L = 0x23;
  CYC(0x49c6, 0x49c7); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x49c7, 0x49c9); alu_and(gb, 0x7f);
  CYC(0x49c9, 0x49ca); B = A;
  CYC(0x49ca, 0x49cb); A = mem_rd(gb, HL);
  if (!(F & FZ)) {
    CYCT(0x49cb, 0x49cd);
  } else {
    CYC(0x49cb, 0x49cd);
    for (;;) {
      CYC(0x49cd, 0x49ce); alu_or(gb, A);
      if (!(F & FZ)) {
        CYCT(0x49ce, 0x49d0);
        break;
      }
      CYC(0x49ce, 0x49d0);
      CYC(0x49d0, 0x49d2); A = hram_rd(gb, 0x04);
      CYC(0x49d2, 0x49d4);
    }
  }
  CYC(0x49d4, 0x49d6); L = 0x00;
  CYC(0x49d6, 0x49d7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x49d7, 0x49d8); mem_wr(gb, HL, B);
  CYC(0x49d8, 0x49d9); ret_effect(gb);
}

void convertSecretBufferToText_hook(GB *gb) {
  CYC(0x49d9, 0x49da); A = C;
  CYC(0x49da, 0x49dd); SET_HL(0x4a02);
  CYC(0x49dd, 0x49de); add_double_index_to_hl_from_rst(gb, 0x49de);
  CYC(0x49de, 0x49df); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x49df, 0x49e0); B = mem_rd(gb, HL);
  CYC(0x49e0, 0x49e1); C = A;
  CYC(0x49e1, 0x49e4); SET_DE(w7SecretGenerationBuffer);
  CYC(0x49e4, 0x49e7); SET_HL(w7SecretText1);
  for (;;) {
    CYC(0x49e7, 0x49e8); A = mem_rd(gb, BC);
    CYC(0x49e8, 0x49ea); alu_and(gb, 0x0f);
    if (F & FZ) {
      CYCT(0x49ea, 0x49eb); ret_effect(gb);
      return;
    }
    CYC(0x49ea, 0x49eb);
    CYC(0x49eb, 0x49ec); push_effect(gb, BC);
    CYC(0x49ec, 0x49ed); B = A;
    for (;;) {
      CYC(0x49ed, 0x49ee); A = mem_rd(gb, DE);
      CYC(0x49ee, 0x49ef); push_effect(gb, HL);
      CYC(0x49ef, 0x49f2); SET_HL(0x00a1);
      CYC(0x49f2, 0x49f3); add_a_to_hl_from_rst(gb, 0x49f3);
      CYC(0x49f3, 0x49f4); A = mem_rd(gb, HL);
      CYC(0x49f4, 0x49f5); SET_HL(pop_effect(gb));
      CYC(0x49f5, 0x49f6); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(0x49f6, 0x49f7); SET_DE(DE + 1);
      CYC(0x49f7, 0x49f8); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(0x49f8, 0x49fa); continue; }
      CYC(0x49f8, 0x49fa);
      break;
    }
    CYC(0x49fa, 0x49fb); SET_BC(pop_effect(gb));
    CYC(0x49fb, 0x49fc); A = mem_rd(gb, BC);
    CYC(0x49fc, 0x49fe); alu_and(gb, 0xf0);
    CYC(0x49fe, 0x49ff); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x49ff, 0x4a00); SET_BC(BC + 1);
    CYC(0x4a00, 0x4a02);
  }
}

void loadSecretBufferFromText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4a15, getNumCharactersForSecretType_hook, 0x4ace, 0x4a18);
  CYC(0x4a18, 0x4a1b); SET_HL(wTmpcec0);
  CYC(0x4a1b, 0x4a1e); SET_DE(w7SecretGenerationBuffer);
  for (;;) {
    CYC(0x4a1e, 0x4a1f); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CALL_C(0x4a1f, loadSecretBufferFromText__textCharacterToByte_hook, 0x4a29, 0x4a22);
    if (F & FC) {
      CYCT(0x4a22, 0x4a23); ret_effect(gb);
      return;
    }
    CYC(0x4a22, 0x4a23);
    CYC(0x4a23, 0x4a24); mem_wr(gb, DE, A);
    CYC(0x4a24, 0x4a25); SET_DE(DE + 1);
    CYC(0x4a25, 0x4a26); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x4a26, 0x4a28); continue; }
    CYC(0x4a26, 0x4a28);
    break;
  }
  CYC(0x4a28, 0x4a29); ret_effect(gb);
}

void loadSecretBufferFromText__textCharacterToByte_hook(GB *gb) {
  CYC(0x4a29, 0x4a2a); push_effect(gb, HL);
  CYC(0x4a2a, 0x4a2b); push_effect(gb, BC);
  CYC(0x4a2b, 0x4a2e); SET_HL(0x00a1);
  CYC(0x4a2e, 0x4a31); SET_BC(0x4000);
  for (;;) {
    CYC(0x4a31, 0x4a32); alu_cp(gb, mem_rd(gb, HL));
    if (F & FZ) {
      CYCT(0x4a32, 0x4a34);
      loadSecretBufferFromText__end_hook(gb);
      return;
    }
    CYC(0x4a32, 0x4a34);
    CYC(0x4a34, 0x4a35); SET_HL(HL + 1);
    CYC(0x4a35, 0x4a36); C = alu_inc8(gb, C);
    CYC(0x4a36, 0x4a37); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x4a37, 0x4a39); continue; }
    CYC(0x4a37, 0x4a39);
    CYC(0x4a39, 0x4a3a); alu_scf(gb);
    loadSecretBufferFromText__end_hook(gb);
    return;
  }
}

void loadSecretBufferFromText__end_hook(GB *gb) {
  CYC(0x4a3a, 0x4a3b); A = C;
  CYC(0x4a3b, 0x4a3c); SET_BC(pop_effect(gb));
  CYC(0x4a3c, 0x4a3d); SET_HL(pop_effect(gb));
  CYC(0x4a3d, 0x4a3e); ret_effect(gb);
}

void runXorCipherOnSecretBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4a3e, getNumCharactersForSecretType_hook, 0x4ace, 0x4a41);
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

void shiftSecretBufferContentsToFront_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4a69, getNumCharactersForSecretType_hook, 0x4ace, 0x4a6c);
  CYC(0x4a6c, 0x4a6e); A = 0x14;
  CYC(0x4a6e, 0x4a6f); alu_sub(gb, B);
  if (F & FZ) {
    CYCT(0x4a6f, 0x4a70); ret_effect(gb);
    return;
  }
  CYC(0x4a6f, 0x4a70);
  CYC(0x4a70, 0x4a73); SET_DE(w7SecretGenerationBuffer);
  CYC(0x4a73, 0x4a74); H = D;
  CYC(0x4a74, 0x4a75); L = E;
  CYC(0x4a75, 0x4a76); add_a_to_hl_from_rst(gb, 0x4a76);
  for (;;) {
    CYC(0x4a76, 0x4a77); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4a77, 0x4a78); mem_wr(gb, DE, A);
    CYC(0x4a78, 0x4a79); SET_DE(DE + 1);
    CYC(0x4a79, 0x4a7a); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x4a7a, 0x4a7c); continue; }
    CYC(0x4a7a, 0x4a7c);
    break;
  }
  CYC(0x4a7c, 0x4a7d); ret_effect(gb);
}

void andCWith3_hook(GB *gb) {
  CYC(0x4a7d, 0x4a7e); A = C;
  CYC(0x4a7e, 0x4a80); alu_and(gb, 0x03);
  CYC(0x4a80, 0x4a81); C = A;
  CYC(0x4a81, 0x4a82); ret_effect(gb);
}

void getNumCharactersForSecretType_hook(GB *gb) {
  CYC(0x4ace, 0x4acf); A = C;
  CYC(0x4acf, 0x4ad2); SET_HL(0x4ad6);
  CYC(0x4ad2, 0x4ad3); add_a_to_hl_from_rst(gb, 0x4ad3);
  CYC(0x4ad3, 0x4ad4); A = mem_rd(gb, HL);
  CYC(0x4ad4, 0x4ad5); B = A;
  CYC(0x4ad5, 0x4ad6); ret_effect(gb);
}
