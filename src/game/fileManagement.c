#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x07, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x07, (from), (to), true)

void clearFileAtHl_hook(GB *gb);
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

void clearFileAtHl_hook(GB *gb) {
  CYC(0x40b6, 0x40b9); SET_BC(0x0550);
  CYC(0x40b9, 0x40bc);
  clearMemoryBc_hook(gb);
}

static void verify_file_copies_both_valid(GB *gb) {
  CYC(0x40e9, 0x40ea); alu_xor(gb, A);
  CYC(0x40ea, 0x40eb); ret_effect(gb);
}

static void verify_file_copies_copy2_invalid(GB *gb, uint16_t sp0_) {
  CALL_C(0x40dc, getFileAddress2_hook, 0x415b, 0x40df);
  CYC(0x40df, 0x40e0); E = C;
  CYC(0x40e0, 0x40e1); D = B;
  CALL_C(0x40e1, getFileAddress1_hook, 0x4157, 0x40e4);
  CYC(0x40e4, 0x40e5); L = C;
  CYC(0x40e5, 0x40e6); H = B;
  CALL_C(0x40e6, copyFileFromHlToDe_hook, 0x40fe, 0x40e9);
  verify_file_copies_both_valid(gb);
}

static void verify_file_copies_copy1_invalid(GB *gb, uint16_t sp0_) {
  CALL_C(0x40eb, getFileAddress1_hook, 0x4157, 0x40ee);
  CYC(0x40ee, 0x40ef); E = C;
  CYC(0x40ef, 0x40f0); D = B;
  CALL_C(0x40f0, getFileAddress2_hook, 0x415b, 0x40f3);
  CYC(0x40f3, 0x40f4); L = C;
  CYC(0x40f4, 0x40f5); H = B;
  CALL_C(0x40f5, copyFileFromHlToDe_hook, 0x40fe, 0x40f8);
  CYC(0x40f8, 0x40fa); A = 0x01;
  CYC(0x40fa, 0x40fb); ret_effect(gb);
}

static void verify_file_copies_both_invalid(GB *gb) {
  CYC(0x40fb, 0x40fd); A = 0xff;
  CYC(0x40fd, 0x40fe); ret_effect(gb);
}

void verifyFileCopies_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x40bc, getFileAddress2_hook, 0x415b, 0x40bf);
  CYC(0x40bf, 0x40c0); L = C;
  CYC(0x40c0, 0x40c1); H = B;
  CALL_C(0x40c1, verifyFileAtHl_hook, 0x4110, 0x40c4);
  CYC(0x40c4, 0x40c6); alu_and(gb, 0x01);
  CYC(0x40c6, 0x40c7); push_effect(gb, AF);
  CALL_C(0x40c7, getFileAddress1_hook, 0x4157, 0x40ca);
  CYC(0x40ca, 0x40cb); L = C;
  CYC(0x40cb, 0x40cc); H = B;
  CALL_C(0x40cc, verifyFileAtHl_hook, 0x4110, 0x40cf);
  CYC(0x40cf, 0x40d0); SET_BC(pop_effect(gb));
  CYC(0x40d0, 0x40d2); B = alu_rl(gb, B);
  CYC(0x40d2, 0x40d3); A = B;
  CYC(0x40d3, 0x40d4); push_effect(gb, 0x40d4);
  switch (jump_table_from_rst(gb)) {
    case 0x40dc: verify_file_copies_copy2_invalid(gb, sp0_); return;
    case 0x40e9: verify_file_copies_both_valid(gb); return;
    case 0x40eb: verify_file_copies_copy1_invalid(gb, sp0_); return;
    case 0x40fb: verify_file_copies_both_invalid(gb); return;
    default: hook_continue(gb, HL, sp0_); return;
  }
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
  uint16_t sp0_ = gb->sp;
  CYC(0x40fe, 0x40ff); push_effect(gb, HL);
  CYC(0x40ff, 0x4101); A = 0x0a;
  CYC(0x4101, 0x4104); mem_wr(gb, 0x1111, A);
  CYC(0x4104, 0x4107); SET_BC(0x0550);
  CALL_C(0x4107, copyMemoryBc_hook, 0x0496, 0x410a);
  CYC(0x410a, 0x410b); alu_xor(gb, A);
  CYC(0x410b, 0x410e); mem_wr(gb, 0x1111, A);
  CYC(0x410e, 0x410f); SET_HL(pop_effect(gb));
  CYC(0x410f, 0x4110); ret_effect(gb);
}

static void verify_file_done(GB *gb) {
  CYC(0x412f, 0x4130); alu_xor(gb, A);
  CYC(0x4130, 0x4133); mem_wr(gb, 0x1111, A);
  CYC(0x4133, 0x4134); SET_HL(pop_effect(gb));
  CYC(0x4134, 0x4135); A = B;
  CYC(0x4135, 0x4136); alu_rrca(gb);
  CYC(0x4136, 0x4137); ret_effect(gb);
}

static void verify_file_failed(GB *gb, uint16_t sp0_) {
  CYC(0x4137, 0x4138); SET_HL(pop_effect(gb));
  CYC(0x4138, 0x4139); push_effect(gb, HL);
  CALL_C(0x4139, clearFileAtHl_hook, 0x40b6, 0x413c);
  CYC(0x413c, 0x413e); B = 0xff;
  CYC(0x413e, 0x4140);
  verify_file_done(gb);
}

static void verify_file_characters(GB *gb, uint16_t sp0_) {
  for (;;) {
    CYC(0x4126, 0x4127); A = mem_rd(gb, DE);
    CYC(0x4127, 0x4128); alu_cp(gb, mem_rd(gb, HL));
    if (!(F & FZ)) {
      CYCT(0x4128, 0x412a);
      verify_file_failed(gb, sp0_);
      return;
    }
    CYC(0x4128, 0x412a);
    CYC(0x412a, 0x412b); SET_DE(DE + 1);
    CYC(0x412b, 0x412c); SET_HL(HL + 1);
    CYC(0x412c, 0x412d); B = alu_dec8(gb, B);
    if (!(F & FZ)) {
      CYCT(0x412d, 0x412f);
      continue;
    }
    CYC(0x412d, 0x412f);
    verify_file_done(gb);
    return;
  }
}

void verifyFileAtHl_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4110, 0x4111); push_effect(gb, HL);
  CYC(0x4111, 0x4113); A = 0x0a;
  CYC(0x4113, 0x4116); mem_wr(gb, 0x1111, A);
  CALL_C(0x4116, calculateFileChecksum_hook, 0x4140, 0x4119);
  CYC(0x4119, 0x411a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x411a, 0x411b); alu_cp(gb, E);
  if (!(F & FZ)) {
    CYCT(0x411b, 0x411d);
    verify_file_failed(gb, sp0_);
    return;
  }
  CYC(0x411b, 0x411d);
  CYC(0x411d, 0x411e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x411e, 0x411f); alu_cp(gb, D);
  if (!(F & FZ)) {
    CYCT(0x411f, 0x4121);
    verify_file_failed(gb, sp0_);
    return;
  }
  CYC(0x411f, 0x4121);
  CYC(0x4121, 0x4124); SET_DE(0x41c9);
  CYC(0x4124, 0x4126); B = 0x08;
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
  CYC(0x4140, 0x4141); push_effect(gb, HL);
  CYC(0x4141, 0x4143); A = 0x02;
  CYC(0x4143, 0x4144); push_effect(gb, 0x4144);
  add_a_to_hl_from_rst(gb);
  CYC(0x4144, 0x4147); SET_BC(0x02a7);
  CYC(0x4147, 0x414a); SET_DE(0x0000);
  for (;;) {
    CYC(0x414a, 0x414b); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x414b, 0x414c); alu_add(gb, E);
    CYC(0x414c, 0x414d); E = A;
    CYC(0x414d, 0x414e); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x414e, 0x414f); alu_adc(gb, D);
    CYC(0x414f, 0x4150); D = A;
    CYC(0x4150, 0x4151); SET_BC(BC - 1);
    CYC(0x4151, 0x4152); A = B;
    CYC(0x4152, 0x4153); alu_or(gb, C);
    if (BC) {
      CYCT(0x4153, 0x4155);
      continue;
    }
    CYC(0x4153, 0x4155);
    break;
  }
  CYC(0x4155, 0x4156); SET_HL(pop_effect(gb));
  CYC(0x4156, 0x4157); ret_effect(gb);
}

static void get_file_address(GB *gb) {
  CYC(0x415d, 0x415e); push_effect(gb, HL);
  CYC(0x415e, 0x4160); A = H8(hActiveFileSlot);
  CYC(0x4160, 0x4161); alu_add(gb, C);
  CYC(0x4161, 0x4164); SET_HL(0x416a);
  CYC(0x4164, 0x4165); push_effect(gb, 0x4165);
  add_double_index_to_hl_from_rst(gb);
  CYC(0x4165, 0x4166); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4166, 0x4167); B = mem_rd(gb, HL);
  CYC(0x4167, 0x4168); C = A;
  CYC(0x4168, 0x4169); SET_HL(pop_effect(gb));
  CYC(0x4169, 0x416a); ret_effect(gb);
}

void getFileAddress1_hook(GB *gb) {
  CYC(0x4157, 0x4159); C = 0x00;
  CYC(0x4159, 0x415b);
  get_file_address(gb);
}

void getFileAddress2_hook(GB *gb) {
  CYC(0x415b, 0x415d); C = 0x03;
  get_file_address(gb);
}

void initializeFileVariables_hook(GB *gb) {
  CYC(0x4176, 0x4178); D = 0xc6;
  for (;;) {
    CYC(0x4178, 0x4179); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x4179, 0x417a); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x417a, 0x417c);
      break;
    }
    CYC(0x417a, 0x417c);
    CYC(0x417c, 0x417d); E = A;
    CYC(0x417d, 0x417e); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x417e, 0x417f); mem_wr(gb, DE, A);
    CYC(0x417f, 0x4181);
  }
  CYC(0x4181, 0x4182); ret_effect(gb);
}
