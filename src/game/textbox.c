#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x3f, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x3f, (from), (to), true)

void getExtraTextIndex_hook(GB *gb);

static void add_double_index_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L); L = A;
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void clearTextGfxBuffer_hook(GB *gb) {
  CYC(0x5091, 0x5094); SET_HL(0xd200);
  CYC(0x5094, 0x5097); SET_BC(0x0200);
  CYC(0x5097, 0x5099); A = 0xff;
  CYC(0x5099, 0x509c);
  fillMemoryBc_hook(gb);
}

void clearLineTextBuffer_hook(GB *gb) {
  CYC(0x509c, 0x509f); SET_HL(0xd400);
  CYC(0x509f, 0x50a0); D = H;
  CYC(0x50a0, 0x50a1); E = L;
  CYC(0x50a1, 0x50a3); B = 0x10;
  CYC(0x50a3, 0x50a6);
  clearMemory_hook(gb);
}

void setLineTextBuffers_hook(GB *gb) {
  CYC(0x50a6, 0x50a7); mem_wr(gb, DE, A);
  CYC(0x50a7, 0x50a8); push_effect(gb, DE);
  CYC(0x50a8, 0x50a9); push_effect(gb, HL);

  CYC(0x50a9, 0x50ac); SET_HL(0xd0c7);
  CYC(0x50ac, 0x50ad); A = E;
  CYC(0x50ad, 0x50af); alu_add(gb, 0x10);
  CYC(0x50af, 0x50b0); E = A;
  CYC(0x50b0, 0x50b1); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x50b1, 0x50b2); mem_wr(gb, DE, A);

  CYC(0x50b2, 0x50b3); A = E;
  CYC(0x50b3, 0x50b5); alu_add(gb, 0x10);
  CYC(0x50b5, 0x50b6); E = A;
  CYC(0x50b6, 0x50b7); L = alu_dec8(gb, L);
  CYC(0x50b7, 0x50b8); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x50b8, 0x50b9); mem_wr(gb, DE, A);

  CYC(0x50b9, 0x50ba); A = E;
  CYC(0x50ba, 0x50bc); alu_add(gb, 0x10);
  CYC(0x50bc, 0x50bd); E = A;
  CYC(0x50bd, 0x50be); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x50be, 0x50bf); mem_wr(gb, DE, A);

  CYC(0x50bf, 0x50c0); A = E;
  CYC(0x50c0, 0x50c2); alu_add(gb, 0x10);
  CYC(0x50c2, 0x50c3); E = A;
  CYC(0x50c3, 0x50c4); A = mem_rd(gb, HL);
  CYC(0x50c4, 0x50c5); mem_wr(gb, DE, A);
  CYC(0x50c5, 0x50c7); mem_wr(gb, HL, 0x00);

  CYC(0x50c7, 0x50c8); SET_HL(pop_effect(gb));
  CYC(0x50c8, 0x50c9); SET_DE(pop_effect(gb));
  CYC(0x50c9, 0x50ca); A = mem_rd(gb, DE);
  CYC(0x50ca, 0x50cb); E = alu_inc8(gb, E);
  CYC(0x50cb, 0x50cc); ret_effect(gb);
}

void dmaTextGfxBuffer_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x50cc, 0x50ce); alu_add(gb, 0x94);
  CYC(0x50ce, 0x50cf); D = A;
  CYC(0x50cf, 0x50d1); E = 0x00;
  CYC(0x50d1, 0x50d4); SET_HL(w7TextGfxBuffer);
  CYC(0x50d4, 0x50d7); SET_BC(0x1f07);
  CYC(0x50d7, 0x50d8); push_effect(gb, HL);
  CALL_C(0x50d8, queueDmaTransfer_hook, 0x058a, 0x50db);
  CYC(0x50db, 0x50dc); SET_HL(pop_effect(gb));
  CYC(0x50dc, 0x50dd); ret_effect(gb);
}

void saveTilesUnderTextbox_copyTileMap_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x50f6, 0x50f8); A = 0x03;
  for (;;) {
    CYC(0x50f8, 0x50f9); push_effect(gb, AF);
    CYC(0x50f9, 0x50fa); push_effect(gb, HL);
    CYC(0x50fa, 0x50fd); A = mem_rd(gb, 0xd0c9);
    CYC(0x50fd, 0x50ff); mem_wr(gb, 0xff70, A);
    CYC(0x50ff, 0x5102); SET_HL(0xcd40);
    CYC(0x5102, 0x5104); A = 0x02;
    for (;;) {
      CYC(0x5104, 0x5105); push_effect(gb, AF);
      CYC(0x5105, 0x5106); A = E;
      CYC(0x5106, 0x5108); alu_and(gb, 0xe0);
      CYC(0x5108, 0x5109); C = A;
      CYC(0x5109, 0x510b); B = 0x20;
      for (;;) {
        CYC(0x510b, 0x510c); A = mem_rd(gb, DE);
        CYC(0x510c, 0x510d); mem_wr(gb, HL, A); SET_HL(HL + 1);
        CYC(0x510d, 0x510e); A = E;
        CYC(0x510e, 0x510f); A = alu_inc8(gb, A);
        CYC(0x510f, 0x5111); alu_and(gb, 0x1f);
        CYC(0x5111, 0x5112); alu_or(gb, C);
        CYC(0x5112, 0x5113); E = A;
        CYC(0x5113, 0x5114); B = alu_dec8(gb, B);
        if (!(F & FZ)) { CYCT(0x5114, 0x5116); continue; }
        CYC(0x5114, 0x5116);
        break;
      }
      CYC(0x5116, 0x5118); A = 0x20;
      CALL_C(0x5118, addAToDe_hook, 0x0068, 0x511b);
      CYC(0x511b, 0x511c); SET_AF(pop_effect(gb));
      CYC(0x511c, 0x511d); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(0x511d, 0x511f); continue; }
      CYC(0x511d, 0x511f);
      break;
    }
    CYC(0x511f, 0x5121); A = 0x07;
    CYC(0x5121, 0x5123); mem_wr(gb, 0xff70, A);
    CYC(0x5123, 0x5124); SET_HL(pop_effect(gb));
    CYC(0x5124, 0x5125); push_effect(gb, DE);
    CYC(0x5125, 0x5128); SET_DE(0xcd40);
    CYC(0x5128, 0x512a); A = 0x02;
    for (;;) {
      CYC(0x512a, 0x512b); push_effect(gb, AF);
      CYC(0x512b, 0x512c); A = L;
      CYC(0x512c, 0x512e); alu_and(gb, 0xe0);
      CYC(0x512e, 0x512f); C = A;
      CYC(0x512f, 0x5131); B = 0x20;
      for (;;) {
        CYC(0x5131, 0x5132); A = mem_rd(gb, DE);
        CYC(0x5132, 0x5133); mem_wr(gb, HL, A);
        CYC(0x5133, 0x5134); E = alu_inc8(gb, E);
        CYC(0x5134, 0x5135); A = L;
        CYC(0x5135, 0x5136); A = alu_inc8(gb, A);
        CYC(0x5136, 0x5138); alu_and(gb, 0x1f);
        CYC(0x5138, 0x5139); alu_or(gb, C);
        CYC(0x5139, 0x513a); L = A;
        CYC(0x513a, 0x513b); B = alu_dec8(gb, B);
        if (!(F & FZ)) { CYCT(0x513b, 0x513d); continue; }
        CYC(0x513b, 0x513d);
        break;
      }
      CYC(0x513d, 0x513f); A = 0x20;
      CYC(0x513f, 0x5140); push_effect(gb, 0x5140);
      add_a_to_hl(gb);
      CYC(0x5140, 0x5141); SET_AF(pop_effect(gb));
      CYC(0x5141, 0x5142); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(0x5142, 0x5144); continue; }
      CYC(0x5142, 0x5144);
      break;
    }
    CYC(0x5144, 0x5145); SET_DE(pop_effect(gb));
    CYC(0x5145, 0x5146); SET_AF(pop_effect(gb));
    CYC(0x5146, 0x5147); A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(0x5147, 0x5149); continue; }
    CYC(0x5147, 0x5149);
    break;
  }
  CYC(0x5149, 0x514a); ret_effect(gb);
}

void saveTilesUnderTextbox_hook(GB *gb) {
  CYC(0x50dd, 0x50e0); SET_HL(0xd0ca);
  CYC(0x50e0, 0x50e1); E = mem_rd(gb, HL);
  CYC(0x50e1, 0x50e2); L = alu_inc8(gb, L);
  CYC(0x50e2, 0x50e3); D = mem_rd(gb, HL);
  CYC(0x50e3, 0x50e4); L = alu_inc8(gb, L);
  CYC(0x50e4, 0x50e5); L = mem_rd(gb, HL);
  CYC(0x50e5, 0x50e7); H = 0xd0;
  CYC(0x50e7, 0x50ea); push_effect(gb, 0x50ea);
  saveTilesUnderTextbox_copyTileMap_hook(gb);
  CYC(0x50ea, 0x50ed); SET_HL(0xd0ca);
  CYC(0x50ed, 0x50ee); E = mem_rd(gb, HL);
  CYC(0x50ee, 0x50ef); L = alu_inc8(gb, L);
  CYC(0x50ef, 0x50f0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x50f0, 0x50f2); alu_add(gb, 0x04);
  CYC(0x50f2, 0x50f3); D = A;
  CYC(0x50f3, 0x50f4); L = mem_rd(gb, HL);
  CYC(0x50f4, 0x50f6); H = 0xd1;
  saveTilesUnderTextbox_copyTileMap_hook(gb);
}

void initTextboxMapping_hook(GB *gb) {
  CYC(0x514a, 0x514d); A = mem_rd(gb, w7d0cc);
  CYC(0x514d, 0x514e); A = alu_inc8(gb, A);
  CYC(0x514e, 0x5150); alu_and(gb, 0x1f);
  CYC(0x5150, 0x5151); L = A;
  CYC(0x5151, 0x5153); E = 0x05;
  for (;;) {
    CYC(0x5153, 0x5155); B = 0x12;
    CYC(0x5155, 0x5156); A = L;
    CYC(0x5156, 0x5157); D = A;
    CYC(0x5157, 0x5159); alu_and(gb, 0xe0);
    CYC(0x5159, 0x515a); C = A;
    for (;;) {
      CYC(0x515a, 0x515c); H = 0xd0;
      CYC(0x515c, 0x515e); mem_wr(gb, HL, 0x02);
      CYC(0x515e, 0x5160); H = 0xd1;
      CYC(0x5160, 0x5162); mem_wr(gb, HL, 0x80);
      CYC(0x5162, 0x5163); A = L;
      CYC(0x5163, 0x5164); A = alu_inc8(gb, A);
      CYC(0x5164, 0x5166); alu_and(gb, 0x1f);
      CYC(0x5166, 0x5167); alu_or(gb, C);
      CYC(0x5167, 0x5168); L = A;
      CYC(0x5168, 0x5169); B = alu_dec8(gb, B);
      if (!(F & FZ)) {
        CYCT(0x5169, 0x516b);
        continue;
      }
      CYC(0x5169, 0x516b);
      break;
    }
    CYC(0x516b, 0x516c); A = D;
    CYC(0x516c, 0x516e); alu_add(gb, 0x20);
    CYC(0x516e, 0x516f); L = A;
    CYC(0x516f, 0x5170); E = alu_dec8(gb, E);
    if (!(F & FZ)) {
      CYCT(0x5170, 0x5172);
      continue;
    }
    CYC(0x5170, 0x5172);
    break;
  }
  CYC(0x5172, 0x5173); ret_effect(gb);
}

void dmaTextboxMap_func_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x518c, 0x518e); C = 0x07;
  CYC(0x518e, 0x518f); push_effect(gb, HL);
  CALL_C(0x518f, queueDmaTransfer_hook, 0x058a, 0x5192);
  CYC(0x5192, 0x5193); SET_HL(pop_effect(gb));
  CYC(0x5193, 0x5194); E = alu_inc8(gb, E);
  CYC(0x5194, 0x5195); H = alu_inc8(gb, H);
  CYC(0x5195, 0x5198); queueDmaTransfer_hook(gb);
}

void dmaTextboxMap_hook(GB *gb) {
  CYC(0x5173, 0x5176); A = mem_rd(gb, 0xcba7);
  CYC(0x5176, 0x5178); alu_add(gb, 0x03);
  CYC(0x5178, 0x5179); C = A;
  CYC(0x5179, 0x517c); SET_HL(0xd0d8);
  CYC(0x517c, 0x517d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x517d, 0x517e); E = A;
  CYC(0x517e, 0x5180); alu_cp(gb, 0x61);
  CYC(0x5180, 0x5181); A = mem_rd(gb, HL);
  CYC(0x5181, 0x5182); D = A;
  if (F & FC) { CYCT(0x5182, 0x5184); goto run_func; }
  CYC(0x5182, 0x5184);
  CYC(0x5184, 0x5185); alu_cp(gb, C);
  if (F & FZ) { CYCT(0x5185, 0x5187); goto alternate; }
  CYC(0x5185, 0x5187);
run_func:
  CYC(0x5187, 0x5189); B = 0x09;
  CYC(0x5189, 0x518c); SET_HL(0xd000);
  dmaTextboxMap_func_hook(gb);
  return;
alternate:
  CYC(0x5198, 0x5199); alu_xor(gb, A);
  CYC(0x5199, 0x519a); alu_sub(gb, E);
  CYC(0x519a, 0x519b); C = A;
  CYC(0x519b, 0x519d); A = alu_swap(gb, A);
  CYC(0x519d, 0x519e); A = alu_dec8(gb, A);
  CYC(0x519e, 0x519f); B = A;
  CYC(0x519f, 0x51a2); SET_HL(0xd000);
  CYC(0x51a2, 0x51a3); push_effect(gb, BC);
  CYC(0x51a3, 0x51a6); push_effect(gb, 0x51a6);
  dmaTextboxMap_func_hook(gb);
  CYC(0x51a6, 0x51a7); SET_BC(pop_effect(gb));
  CYC(0x51a7, 0x51aa); A = mem_rd(gb, 0xcba7);
  CYC(0x51aa, 0x51ab); D = A;
  CYC(0x51ab, 0x51ad); E = 0x00;
  CYC(0x51ad, 0x51ae); L = C;
  CYC(0x51ae, 0x51b0); H = 0xd0;
  CYC(0x51b0, 0x51b2); A = 0xa0;
  CYC(0x51b2, 0x51b3); alu_sub(gb, C);
  CYC(0x51b3, 0x51b5); A = alu_swap(gb, A);
  CYC(0x51b5, 0x51b6); A = alu_dec8(gb, A);
  CYC(0x51b6, 0x51b7); B = A;
  CYC(0x51b7, 0x51b9); dmaTextboxMap_func_hook(gb);
}

void updateCharacterDisplayTimer_hook(GB *gb) {
  CYC(0x51b9, 0x51ba); H = D;
  CYC(0x51ba, 0x51bc); L = 0xee;
  CYC(0x51bc, 0x51bd); A = mem_rd(gb, HL);
  CYC(0x51bd, 0x51be); alu_or(gb, A);
  if (F & FZ) { CYCT(0x51be, 0x51c0); goto timer_check; }
  CYC(0x51be, 0x51c0);
  CYC(0x51c0, 0x51c1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
timer_check:
  CYC(0x51c1, 0x51c3); L = 0xd7;
  CYC(0x51c3, 0x51c4); A = mem_rd(gb, HL);
  CYC(0x51c4, 0x51c5); alu_or(gb, A);
  if (F & FZ) { CYCT(0x51c5, 0x51c7); goto check_input; }
  CYC(0x51c5, 0x51c7);
  CYC(0x51c7, 0x51c8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x51c8, 0x51ca); goto countdown; }
  CYC(0x51c8, 0x51ca);
check_input:
  CYC(0x51ca, 0x51cd); A = mem_rd(gb, 0xc482);
  CYC(0x51cd, 0x51cf); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x51cf, 0x51d1); goto skip_to_line_end; }
  CYC(0x51cf, 0x51d1);
countdown:
  CYC(0x51d1, 0x51d3); L = 0xc6;
  CYC(0x51d3, 0x51d4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x51d4, 0x51d5); ret_effect(gb);
  return;
skip_to_line_end:
  CYC(0x51d5, 0x51d7); L = 0xc1;
  CYC(0x51d7, 0x51d9); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(0x51d9, 0x51da); alu_xor(gb, A);
  CYC(0x51da, 0x51db); ret_effect(gb);
}

void getNextCharacterToDisplay_hook(GB *gb) {
  CYC(0x5288, 0x528a); E = 0xd0;
  CYC(0x528a, 0x528b); A = mem_rd(gb, DE);
  CYC(0x528b, 0x528d); alu_cp(gb, 0x10);
  if (F & FZ) { CYCT(0x528d, 0x528e); ret_effect(gb); return; }
  CYC(0x528d, 0x528e);
  CYC(0x528e, 0x5290); alu_add(gb, 0x00);
  CYC(0x5290, 0x5291); L = A;
  CYC(0x5291, 0x5293); H = 0xd4;
  CYC(0x5293, 0x5294); A = mem_rd(gb, HL);
  CYC(0x5294, 0x5295); alu_or(gb, A);
  CYC(0x5295, 0x5296); ret_effect(gb);
}

void readNextTextByte_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x52fd, 0x52ff); L = 0xd5;
  CYC(0x52ff, 0x5300); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5300, 0x5301); H = mem_rd(gb, HL);
  CYC(0x5301, 0x5302); L = A;
  CALL_C(0x5302, readByteFromW7ActiveBankAndIncHl, 0x56cf, 0x5305);
  getExtraTextIndex_hook(gb);
}

void getExtraTextIndex_hook(GB *gb) {
  CYC(0x5305, 0x5308); SET_HL(0x5915);
  CYC(0x5308, 0x5309); add_double_index_to_hl(gb, 0x5309);
  CYC(0x5309, 0x530a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x530a, 0x530b); H = mem_rd(gb, HL);
  CYC(0x530b, 0x530c); L = A;
  CYC(0x530c, 0x530d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x530d, 0x530e); C = A;
  CYC(0x530e, 0x530f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x530f, 0x5310); B = A;
  CYC(0x5310, 0x5311); A = mem_rd(gb, BC);
  CYC(0x5311, 0x5312); push_effect(gb, 0x5312);
  add_a_to_hl(gb);
  CYC(0x5312, 0x5313); A = mem_rd(gb, HL);
  CYC(0x5313, 0x5314); ret_effect(gb);
}

void updateTextboxArrow_hook(GB *gb) {
  CYC(0x5314, 0x5317); A = mem_rd(gb, 0xcc00);
  CYC(0x5317, 0x5319); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x5319, 0x531a); ret_effect(gb); return; }
  CYC(0x5319, 0x531a);
  CYC(0x531a, 0x531c); E = 0xcc;
  CYC(0x531c, 0x531d); A = mem_rd(gb, DE);
  CYC(0x531d, 0x531f); alu_add(gb, 0x12);
  CYC(0x531f, 0x5321); alu_and(gb, 0x1f);
  CYC(0x5321, 0x5323); alu_add(gb, 0x80);
  CYC(0x5323, 0x5324); L = A;
  CYC(0x5324, 0x5326); H = 0xd0;
  CYC(0x5326, 0x5328); E = 0xc8;
  CYC(0x5328, 0x5329); A = mem_rd(gb, DE);
  CYC(0x5329, 0x532b); alu_cp(gb, 0x03);
  CYC(0x532b, 0x532d); A = 0x02;
  if (F & FZ) { CYCT(0x532d, 0x532f); goto update_arrow; }
  CYC(0x532d, 0x532f);
  CYC(0x532f, 0x5331); A = 0x03;
update_arrow:
  CYC(0x5331, 0x5332); mem_wr(gb, DE, A);
  CYC(0x5332, 0x5333); mem_wr(gb, HL, A);
  CYC(0x5333, 0x5336); A = mem_rd(gb, 0xcba7);
  CYC(0x5336, 0x5338); alu_add(gb, 0x04);
  CYC(0x5338, 0x5339); C = A;
  CYC(0x5339, 0x533b); L = 0x80;
  CYC(0x533b, 0x533d); H = 0xd0;
  CYC(0x533d, 0x533f); E = 0xd8;
  CYC(0x533f, 0x5340); A = mem_rd(gb, DE);
  CYC(0x5340, 0x5341); alu_add(gb, L);
  CYC(0x5341, 0x5342); B = A;
  CYC(0x5342, 0x5343); E = alu_inc8(gb, E);
  CYC(0x5343, 0x5344); A = mem_rd(gb, DE);
  CYC(0x5344, 0x5346); alu_adc(gb, 0x00);
  CYC(0x5346, 0x5347); alu_cp(gb, C);
  if (F & FC) { CYCT(0x5347, 0x5349); goto queue_dma; }
  CYC(0x5347, 0x5349);
  CYC(0x5349, 0x534c); A = mem_rd(gb, 0xcba7);
queue_dma:
  CYC(0x534c, 0x534d); D = A;
  CYC(0x534d, 0x534e); E = B;
  CYC(0x534e, 0x5351); SET_BC(0x0107);
  CYC(0x5351, 0x5354); queueDmaTransfer_hook(gb);
}

void clearTopRowOfTextMap_func_hook(GB *gb) {
  CYC(0x536d, 0x536e); C = A;
  CYC(0x536e, 0x536f); A = L;
  CYC(0x536f, 0x5371); alu_and(gb, 0xe0);
  CYC(0x5371, 0x5372); E = A;
  for (;;) {
    CYC(0x5372, 0x5373); mem_wr(gb, HL, C);
    CYC(0x5373, 0x5374); A = L;
    CYC(0x5374, 0x5375); A = alu_inc8(gb, A);
    CYC(0x5375, 0x5377); alu_and(gb, 0x1f);
    CYC(0x5377, 0x5378); alu_or(gb, E);
    CYC(0x5378, 0x5379); L = A;
    CYC(0x5379, 0x537a); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x537a, 0x537c); continue; }
    CYC(0x537a, 0x537c);
    break;
  }
  CYC(0x537c, 0x537d); ret_effect(gb);
}

void clearTopRowOfTextMap_hook(GB *gb) {
  CYC(0x5354, 0x5356); H = 0xd0;
  CYC(0x5356, 0x5359); A = mem_rd(gb, 0xd0cc);
  CYC(0x5359, 0x535b); alu_add(gb, 0x02);
  CYC(0x535b, 0x535d); alu_and(gb, 0x1f);
  CYC(0x535d, 0x535e); L = A;
  CYC(0x535e, 0x535f); C = A;
  CYC(0x535f, 0x5361); B = 0x10;
  CYC(0x5361, 0x5363); A = 0x02;
  CYC(0x5363, 0x5364); push_effect(gb, BC);
  CYC(0x5364, 0x5367); push_effect(gb, 0x5367);
  clearTopRowOfTextMap_func_hook(gb);
  CYC(0x5367, 0x5368); SET_BC(pop_effect(gb));
  CYC(0x5368, 0x536a); H = 0xd1;
  CYC(0x536a, 0x536b); L = C;
  CYC(0x536b, 0x536d); A = 0x80;
  clearTopRowOfTextMap_func_hook(gb);
}

void shiftTextboxMapUp_func_hook(GB *gb) {
  CYC(0x5384, 0x5385); D = H;
  CYC(0x5385, 0x5388); A = mem_rd(gb, 0xd0cc);
  CYC(0x5388, 0x538a); alu_add(gb, 0x02);
  CYC(0x538a, 0x538c); alu_and(gb, 0x1f);
  CYC(0x538c, 0x538d); E = A;
  CYC(0x538d, 0x538f); alu_add(gb, 0x20);
  CYC(0x538f, 0x5390); L = A;
  CYC(0x5390, 0x5391); B = A;
  CYC(0x5391, 0x5393); C = 0x04;
  for (;;) {
    CYC(0x5393, 0x5394); push_effect(gb, BC);
    CYC(0x5394, 0x5395); A = E;
    CYC(0x5395, 0x5397); alu_and(gb, 0xe0);
    CYC(0x5397, 0x5398); C = A;
    CYC(0x5398, 0x539a); B = 0x10;
    for (;;) {
      CYC(0x539a, 0x539b); A = mem_rd(gb, HL);
      CYC(0x539b, 0x539c); mem_wr(gb, DE, A);
      CYC(0x539c, 0x539d); A = E;
      CYC(0x539d, 0x539e); A = alu_inc8(gb, A);
      CYC(0x539e, 0x53a0); alu_and(gb, 0x1f);
      CYC(0x53a0, 0x53a1); alu_or(gb, C);
      CYC(0x53a1, 0x53a2); E = A;
      CYC(0x53a2, 0x53a4); alu_add(gb, 0x20);
      CYC(0x53a4, 0x53a5); L = A;
      CYC(0x53a5, 0x53a6); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(0x53a6, 0x53a8); continue; }
      CYC(0x53a6, 0x53a8);
      break;
    }
    CYC(0x53a8, 0x53a9); SET_BC(pop_effect(gb));
    CYC(0x53a9, 0x53aa); E = B;
    CYC(0x53aa, 0x53ab); A = B;
    CYC(0x53ab, 0x53ad); alu_add(gb, 0x20);
    CYC(0x53ad, 0x53ae); L = A;
    CYC(0x53ae, 0x53af); B = A;
    CYC(0x53af, 0x53b0); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(0x53b0, 0x53b2); continue; }
    CYC(0x53b0, 0x53b2);
    break;
  }
  CYC(0x53b2, 0x53b3); ret_effect(gb);
}

void shiftTextboxMapUp_hook(GB *gb) {
  CYC(0x537d, 0x537f); H = 0xd0;
  CYC(0x537f, 0x5382); push_effect(gb, 0x5382);
  shiftTextboxMapUp_func_hook(gb);
  CYC(0x5382, 0x5384); H = 0xd1;
  shiftTextboxMapUp_func_hook(gb);
}

void subFirstRowOfTextMapBy20_func_hook(GB *gb) {
  CYC(0x53bc, 0x53bf); A = mem_rd(gb, 0xd0cc);
  CYC(0x53bf, 0x53c1); alu_add(gb, 0x02);
  CYC(0x53c1, 0x53c3); alu_and(gb, 0x1f);
  CYC(0x53c3, 0x53c4); alu_add(gb, B);
  CYC(0x53c4, 0x53c5); L = A;
  CYC(0x53c5, 0x53c7); alu_and(gb, 0xe0);
  CYC(0x53c7, 0x53c8); C = A;
  CYC(0x53c8, 0x53ca); B = 0x10;
  for (;;) {
    CYC(0x53ca, 0x53cb); A = mem_rd(gb, HL);
    CYC(0x53cb, 0x53cd); alu_and(gb, 0x60);
    CYC(0x53cd, 0x53cf); alu_cp(gb, 0x60);
    if (!(F & FZ)) { CYCT(0x53cf, 0x53d1); goto next_tile; }
    CYC(0x53cf, 0x53d1);
    CYC(0x53d1, 0x53d3); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x20);
next_tile:
    CYC(0x53d3, 0x53d4); A = L;
    CYC(0x53d4, 0x53d5); A = alu_inc8(gb, A);
    CYC(0x53d5, 0x53d7); alu_and(gb, 0x1f);
    CYC(0x53d7, 0x53d8); alu_or(gb, C);
    CYC(0x53d8, 0x53d9); L = A;
    CYC(0x53d9, 0x53da); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x53da, 0x53dc); continue; }
    CYC(0x53da, 0x53dc);
    break;
  }
  CYC(0x53dc, 0x53dd); ret_effect(gb);
}

void subFirstRowOfTextMapBy20_hook(GB *gb) {
  CYC(0x53b3, 0x53b5); H = 0xd0;
  CYC(0x53b5, 0x53b7); B = 0x00;
  CYC(0x53b7, 0x53ba); push_effect(gb, 0x53ba);
  subFirstRowOfTextMapBy20_func_hook(gb);
  CYC(0x53ba, 0x53bc); B = 0x20;
  subFirstRowOfTextMapBy20_func_hook(gb);
}

void decInvTextScrollTimer_hook(GB *gb) {
  CYC(0x5597, 0x5598); H = D;
  CYC(0x5598, 0x559a); L = 0xde;
  CYC(0x559a, 0x559b); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x559b, 0x559c); ret_effect(gb);
    return;
  }
  CYC(0x559b, 0x559c);
  CYC(0x559c, 0x559e); mem_wr(gb, HL, 0x08);
  CYC(0x559e, 0x559f); alu_xor(gb, A);
  CYC(0x559f, 0x55a0); ret_effect(gb);
}

void getSelectedTextOptionAddress_hook(GB *gb) {
  CYC(0x55c8, 0x55ca); E = 0xe8;
  CYC(0x55ca, 0x55cb); A = mem_rd(gb, DE);
  CYC(0x55cb, 0x55cd); alu_add(gb, 0xe0);
  CYC(0x55cd, 0x55ce); L = A;
  CYC(0x55ce, 0x55cf); H = D;
  CYC(0x55cf, 0x55d0); ret_effect(gb);
}

void getAddressInTextboxMap_hook(GB *gb) {
  CYC(0x55d0, 0x55d2); alu_and(gb, 0x1e);
  CYC(0x55d2, 0x55d3); alu_rrca(gb);
  CYC(0x55d3, 0x55d4); L = A;
  CYC(0x55d4, 0x55d6); E = 0xcc;
  CYC(0x55d6, 0x55d7); A = mem_rd(gb, DE);
  CYC(0x55d7, 0x55d9); alu_add(gb, 0x02);
  CYC(0x55d9, 0x55da); alu_add(gb, L);
  CYC(0x55da, 0x55dc); alu_and(gb, 0x1f);
  CYC(0x55dc, 0x55dd); alu_add(gb, B);
  CYC(0x55dd, 0x55de); L = A;
  CYC(0x55de, 0x55e0); H = 0xd0;
  CYC(0x55e0, 0x55e1); ret_effect(gb);
}

void removeCursorFromSelectedTextPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55e1, 0x55e3); B = 0x60;
  CYC(0x55e3, 0x55e5); E = 0xe9;
  CYC(0x55e5, 0x55e6); A = mem_rd(gb, DE);
  CYC(0x55e6, 0x55e7); C = A;
  CYC(0x55e7, 0x55e9); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(0x55e9, 0x55eb);
  } else {
    CYC(0x55e9, 0x55eb);
    CYC(0x55eb, 0x55ed); B = 0x20;
  }
  CALL_C(0x55ed, getAddressInTextboxMap_hook, 0x55d0, 0x55f0);
  CYC(0x55f0, 0x55f1); mem_wr(gb, HL, C);
  CYC(0x55f1, 0x55f2); ret_effect(gb);
}

void moveSelectedTextOptionRight_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55f2, 0x55f4); E = 0xe8;
  CYC(0x55f4, 0x55f5); A = mem_rd(gb, DE);
  CYC(0x55f5, 0x55f6); A = alu_inc8(gb, A);
  CYC(0x55f6, 0x55f8); alu_and(gb, 0x07);
  CYC(0x55f8, 0x55f9); mem_wr(gb, DE, A);
  CALL_C(0x55f9, getSelectedTextOptionAddress_hook, 0x55c8, 0x55fc);
  CYC(0x55fc, 0x55fd); A = mem_rd(gb, HL);
  CYC(0x55fd, 0x55fe); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x55fe, 0x55ff); ret_effect(gb);
    return;
  }
  CYC(0x55fe, 0x55ff);
  CYC(0x55ff, 0x5600); alu_xor(gb, A);
  CYC(0x5600, 0x5601); mem_wr(gb, DE, A);
  CYC(0x5601, 0x5602); ret_effect(gb);
}
