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

void displayNextTextCharacter_readSubsequentLineBuffers_hook(GB *gb);
void displayNextTextCharacter_playSound_hook(GB *gb);
void displayNextTextCharacter_checkCanAdvanceWithAB_hook(GB *gb);
void label_3f_155_hook(GB *gb);
void label_3f_157_hook(GB *gb);
void label_3f_158_hook(GB *gb);
void label_3f_159_hook(GB *gb);
void label_3f_160_hook(GB *gb);

void displayNextTextCharacter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
start:
  CYC(0x51db, 0x51dd); E = 0xd3;
  CYC(0x51dd, 0x51de); A = mem_rd(gb, DE);
  CYC(0x51de, 0x51df); C = A;
  CYC(0x51df, 0x51e1); alu_cp(gb, 0x40);
  CYC(0x51e1, 0x51e3); B = 0x00;
  if (F & FZ) { CYCT(0x51e3, 0x51e5); goto display_character; }
  CYC(0x51e3, 0x51e5);
  CYC(0x51e5, 0x51e7); B = 0x40;
display_character:
  CYC(0x51e7, 0x51e9); E = 0xd0;
  CYC(0x51e9, 0x51ea); A = mem_rd(gb, DE);
  CYC(0x51ea, 0x51eb); L = A;
  CYC(0x51eb, 0x51ed); E = 0xcc;
  CYC(0x51ed, 0x51ee); A = mem_rd(gb, DE);
  CYC(0x51ee, 0x51f0); alu_add(gb, 0x02);
  CYC(0x51f0, 0x51f1); alu_add(gb, L);
  CYC(0x51f1, 0x51f3); alu_and(gb, 0x1f);
  CYC(0x51f3, 0x51f4); alu_add(gb, B);
  CYC(0x51f4, 0x51f5); E = A;
  CYC(0x51f5, 0x51f7); H = 0xd4;
  CYC(0x51f7, 0x51f8); A = mem_rd(gb, HL);
  CYC(0x51f8, 0x51f9); alu_or(gb, A);
  if (F & FZ) { CYCT(0x51f9, 0x51fb); goto end_line; }
  CYC(0x51f9, 0x51fb);
  CYC(0x51fb, 0x51fd); mem_wr(gb, 0xff8b, A);
  CYC(0x51fd, 0x51ff); D = 0xd0;
  CYC(0x51ff, 0x5200); A = L;
  CYC(0x5200, 0x5201); alu_add(gb, A);
  CYC(0x5201, 0x5202); alu_add(gb, C);
  CYC(0x5202, 0x5203); B = A;
  CYC(0x5203, 0x5204); B = alu_inc8(gb, B);
  CYC(0x5204, 0x5205); mem_wr(gb, DE, A);
  CYC(0x5205, 0x5206); A = E;
  CYC(0x5206, 0x5208); alu_add(gb, 0x20);
  CYC(0x5208, 0x5209); E = A;
  CYC(0x5209, 0x520a); A = B;
  CYC(0x520a, 0x520b); mem_wr(gb, DE, A);
  CYC(0x520b, 0x520c); D = alu_inc8(gb, D);
  CYC(0x520c, 0x520d); A = L;
  CYC(0x520d, 0x520f); alu_add(gb, 0x10);
  CYC(0x520f, 0x5210); L = A;
  CYC(0x5210, 0x5211); A = mem_rd(gb, HL);
  CYC(0x5211, 0x5212); mem_wr(gb, DE, A);
  CYC(0x5212, 0x5213); A = E;
  CYC(0x5213, 0x5215); alu_sub(gb, 0x20);
  CYC(0x5215, 0x5216); E = A;
  CYC(0x5216, 0x5217); A = mem_rd(gb, HL);
  CYC(0x5217, 0x5218); mem_wr(gb, DE, A);
  CYC(0x5218, 0x521a); D = 0xd0;
  CYC(0x521a, 0x521c); E = 0xd0;
  CYC(0x521c, 0x521d); A = mem_rd(gb, DE);
  CYC(0x521d, 0x521e); A = alu_inc8(gb, A);
  CYC(0x521e, 0x521f); mem_wr(gb, DE, A);
  CYC(0x521f, 0x5221); alu_cp(gb, 0x10);
  if (F & FZ) { CYCT(0x5221, 0x5223); goto end_line; }
  CYC(0x5221, 0x5223);
  CYC(0x5223, 0x5226); push_effect(gb, 0x5226);
  displayNextTextCharacter_checkCanAdvanceWithAB_hook(gb);
  if (!(F & FZ)) { CYCT(0x5226, 0x5228); goto read_subsequent_buffers; }
  CYC(0x5226, 0x5228);
  CYC(0x5228, 0x522a); E = 0xc1;
  CYC(0x522a, 0x522b); A = mem_rd(gb, DE);
  CYC(0x522b, 0x522d); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x522d, 0x522f); goto start; }
  CYC(0x522d, 0x522f);
read_subsequent_buffers:
  CYC(0x522f, 0x5232); push_effect(gb, 0x5232);
  displayNextTextCharacter_readSubsequentLineBuffers_hook(gb);
  CYC(0x5232, 0x5233); alu_or(gb, D);
  CYC(0x5233, 0x5234); ret_effect(gb);
  return;
end_line:
  CYC(0x5234, 0x5235); H = D;
  CYC(0x5235, 0x5237); L = 0xc1;
  CYC(0x5237, 0x5239); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5239, 0x523a); ret_effect(gb); return; }
  CYC(0x5239, 0x523a);
  CYC(0x523a, 0x523c); L = 0xee;
  CYC(0x523c, 0x523d); A = mem_rd(gb, HL);
  CYC(0x523d, 0x523e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x523e, 0x5240); goto clear_a; }
  CYC(0x523e, 0x5240);
  CYC(0x5240, 0x5242); mem_wr(gb, HL, 0x04);
  CYC(0x5242, 0x5244); L = 0xc4;
  CYC(0x5244, 0x5245); A = mem_rd(gb, HL);
  CALL_C(0x5245, playSound_b00_hook, 0x0c98, 0x5248);
clear_a:
  CYC(0x5248, 0x5249); alu_xor(gb, A);
  CYC(0x5249, 0x524a); ret_effect(gb);
}

void displayNextTextCharacter_readSubsequentLineBuffers_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x524a, 0x524b); A = L;
  CYC(0x524b, 0x524d); alu_add(gb, 0x10);
  CYC(0x524d, 0x524e); L = A;
  CYC(0x524e, 0x524f); A = mem_rd(gb, HL);
  CYC(0x524f, 0x5251); E = 0xc6;
  CYC(0x5251, 0x5252); mem_wr(gb, DE, A);
  CYC(0x5252, 0x5253); A = L;
  CYC(0x5253, 0x5255); alu_add(gb, 0x10);
  CYC(0x5255, 0x5256); L = A;
  CYC(0x5256, 0x5257); A = mem_rd(gb, HL);
  CYC(0x5257, 0x5258); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5258, 0x525a); goto read_sound_effect; }
  CYC(0x5258, 0x525a);
  CYC(0x525a, 0x525b); B = A;
  CYC(0x525b, 0x525d); A = mem_rd(gb, 0xff8b);
  CYC(0x525d, 0x525f); alu_cp(gb, 0x20);
  if (F & FZ) { CYCT(0x525f, 0x5261); goto read_sound_effect; }
  CYC(0x525f, 0x5261);
  CYC(0x5261, 0x5263); E = 0xee;
  CYC(0x5263, 0x5264); A = mem_rd(gb, DE);
  CYC(0x5264, 0x5265); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x5265, 0x5267); goto read_sound_effect; }
  CYC(0x5265, 0x5267);
  CYC(0x5267, 0x5269); A = 0x04;
  CYC(0x5269, 0x526a); mem_wr(gb, DE, A);
  CYC(0x526a, 0x526b); A = B;
  CYC(0x526b, 0x526e); push_effect(gb, 0x526e);
  displayNextTextCharacter_playSound_hook(gb);
read_sound_effect:
  CYC(0x526e, 0x526f); A = L;
  CYC(0x526f, 0x5271); alu_add(gb, 0x10);
  CYC(0x5271, 0x5272); L = A;
  CYC(0x5272, 0x5273); A = mem_rd(gb, HL);
  CYC(0x5273, 0x5274); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5274, 0x5275); ret_effect(gb); return; }
  CYC(0x5274, 0x5275);
  displayNextTextCharacter_playSound_hook(gb);
}

void displayNextTextCharacter_playSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5275, 0x5276); push_effect(gb, HL);
  CALL_C(0x5276, playSound_b00_hook, 0x0c98, 0x5279);
  CYC(0x5279, 0x527a); SET_HL(pop_effect(gb));
  CYC(0x527a, 0x527b); ret_effect(gb);
}

void displayNextTextCharacter_checkCanAdvanceWithAB_hook(GB *gb) {
  CYC(0x527b, 0x527c); push_effect(gb, HL);
  CYC(0x527c, 0x527e); E = 0xd0;
  CYC(0x527e, 0x527f); A = mem_rd(gb, DE);
  CYC(0x527f, 0x5281); alu_add(gb, 0x50);
  CYC(0x5281, 0x5282); L = A;
  CYC(0x5282, 0x5284); H = 0xd4;
  CYC(0x5284, 0x5286); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(0x5286, 0x5287); SET_HL(pop_effect(gb));
  CYC(0x5287, 0x5288); ret_effect(gb);
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

void func_5296_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5296, 0x5297); H = D;
  CYC(0x5297, 0x5299); L = 0xc1;
  CYC(0x5299, 0x529a); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x529a, 0x529c); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(0x529c, 0x529e); goto choose_option; }
  CYC(0x529c, 0x529e);
  CYC(0x529e, 0x52a0); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(0x52a0, 0x52a2); label_3f_155_hook(gb); return; }
  CYC(0x52a0, 0x52a2);
  CYC(0x52a2, 0x52a4); alu_bit(gb, 4, A);
  if (F & FZ) { CYCT(0x52a4, 0x52a5); ret_effect(gb); return; }
  CYC(0x52a4, 0x52a5);
  CALL_C(0x52a5, readNextTextByte_hook, 0x52fd, 0x52a8);
  CYC(0x52a8, 0x52aa); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x52aa, 0x52ac); label_3f_158_hook(gb); return; }
  CYC(0x52aa, 0x52ac);
  CYC(0x52ac, 0x52af); mem_wr(gb, 0xcba2, A);
  CALL_C(0x52af, checkInitialTextCommands, 0x4ff5, 0x52b2);
  CYC(0x52b2, 0x52b4); E = 0xc1;
  CYC(0x52b4, 0x52b5); alu_xor(gb, A);
  CYC(0x52b5, 0x52b6); mem_wr(gb, DE, A);
  CYC(0x52b6, 0x52b7); E = alu_inc8(gb, E);
  CYC(0x52b7, 0x52b8); A = alu_inc8(gb, A);
  CYC(0x52b8, 0x52b9); mem_wr(gb, DE, A);
  CYC(0x52b9, 0x52ba); ret_effect(gb);
  return;
choose_option:
  CYC(0x52ba, 0x52bc); E = 0xc2;
  CYC(0x52bc, 0x52bd); A = mem_rd(gb, DE);
  CYC(0x52bd, 0x52be); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x52be, 0x52c0); label_3f_159_hook(gb); return; }
  CYC(0x52be, 0x52c0);
  CYC(0x52c0, 0x52c1); mem_wr(gb, HL, A);
  CYC(0x52c1, 0x52c3); A = 0x01;
  CYC(0x52c3, 0x52c6); mem_wr(gb, 0xcba1, A);
  CYC(0x52c6, 0x52c7); alu_or(gb, H);
  CYC(0x52c7, 0x52c8); ret_effect(gb);
}

void label_3f_155_hook(GB *gb) {
  CYC(0x52c8, 0x52ca); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(0x52ca, 0x52cc); goto check_input; }
  CYC(0x52ca, 0x52cc);
  CYC(0x52cc, 0x52cd); L = alu_inc8(gb, L);
  CYC(0x52cd, 0x52cf); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x01);
  CYC(0x52cf, 0x52d1); label_3f_157_hook(gb);
  return;
check_input:
  CYC(0x52d1, 0x52d4); A = mem_rd(gb, 0xc482);
  CYC(0x52d4, 0x52d6); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(0x52d6, 0x52d8); label_3f_157_hook(gb); return; }
  CYC(0x52d6, 0x52d8);
  CYC(0x52d8, 0x52da); mem_wr(gb, HL, 0x00);
  CYC(0x52da, 0x52dc); L = 0xc1;
  CYC(0x52dc, 0x52de); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x02);
  CYC(0x52de, 0x52df); SET_HL(pop_effect(gb));
  CYC(0x52df, 0x52e1); A = 0x89;
  CYC(0x52e1, 0x52e4); playSound_b00_hook(gb);
}

void label_3f_157_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x52e4, updateTextboxArrow_hook, 0x5314, 0x52e7);
  CYC(0x52e7, 0x52e8); alu_or(gb, H);
  CYC(0x52e8, 0x52e9); ret_effect(gb);
}

void label_3f_158_hook(GB *gb) {
  CYC(0x52e9, 0x52ea); alu_xor(gb, A);
  CYC(0x52ea, 0x52ed); mem_wr(gb, 0x00c2, A);
  CYC(0x52ed, 0x52ee); ret_effect(gb);
}

void label_3f_159_hook(GB *gb) {
  CYC(0x52ee, 0x52f1); SET_HL(0xd0e0);
  label_3f_160_hook(gb);
}

void label_3f_160_hook(GB *gb) {
  for (;;) {
    CYC(0x52f1, 0x52f2); A = mem_rd(gb, HL);
    CYC(0x52f2, 0x52f3); alu_or(gb, A);
    if (F & FZ) { CYCT(0x52f3, 0x52f4); ret_effect(gb); return; }
    CYC(0x52f3, 0x52f4);
    CYC(0x52f4, 0x52f6); alu_xor(gb, 0x20);
    CYC(0x52f6, 0x52f7); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x52f7, 0x52f8); A = L;
    CYC(0x52f8, 0x52fa); alu_and(gb, 0x07);
    if (!(F & FZ)) { CYCT(0x52fa, 0x52fc); continue; }
    CYC(0x52fa, 0x52fc); ret_effect(gb);
    return;
  }
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

void func_53eb_func_hook(GB *gb);
void func_53eb_dmaHeartPieceDisplay_hook(GB *gb);

void func_53dd_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x53dd, 0x53de); H = D;
  CYC(0x53de, 0x53e0); L = 0xc1;
  CYC(0x53e0, 0x53e2); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x02);
  CALL_C(0x53e2, saveTilesUnderTextbox_hook, 0x50dd, 0x53e5);
  CALL_C(0x53e5, initTextboxMapping_hook, 0x514a, 0x53e8);
  CYC(0x53e8, 0x53eb); dmaTextboxMap_hook(gb);
}

void func_53eb_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x53eb, 0x53ec); H = D;
  CYC(0x53ec, 0x53ee); L = 0xc1;
  CYC(0x53ee, 0x53f0); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x53f0, 0x53f2); goto initialize_heart_piece; }
  CYC(0x53f0, 0x53f2);
  CYC(0x53f2, 0x53f4); L = 0xea;
  CYC(0x53f4, 0x53f5); A = mem_rd(gb, HL);
  CYC(0x53f5, 0x53f6); alu_or(gb, A);
  if (F & FZ) { CYCT(0x53f6, 0x53f7); ret_effect(gb); return; }
  CYC(0x53f6, 0x53f7);
  CYC(0x53f7, 0x53f8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x53f8, 0x53f9); ret_effect(gb); return; }
  CYC(0x53f8, 0x53f9);
  CYC(0x53f9, 0x53fb); B = 0x00;
  CYC(0x53fb, 0x53fe); push_effect(gb, 0x53fe);
  func_53eb_func_hook(gb);
  CYC(0x53fe, 0x5400); A = 0x89;
  CALL_C(0x5400, playSound_b00_hook, 0x0c98, 0x5403);
  CYC(0x5403, 0x5404); alu_xor(gb, A);
  CYC(0x5404, 0x5405); ret_effect(gb);
  return;
initialize_heart_piece:
  CYC(0x5405, 0x5407); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x20);
  CYC(0x5407, 0x5409); L = 0xea;
  CYC(0x5409, 0x540b); mem_wr(gb, HL, 0x1e);
  CYC(0x540b, 0x540d); L = 0xef;
  CYC(0x540d, 0x540f); mem_wr(gb, HL, 0x01);
  CYC(0x540f, 0x5412); push_effect(gb, 0x5412);
  func_53eb_dmaHeartPieceDisplay_hook(gb);
  CYC(0x5412, 0x5414); B = 0xff;
  func_53eb_func_hook(gb);
}

void func_53eb_func_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5414, 0x5417); A = mem_rd(gb, 0xc6ac);
  CYC(0x5417, 0x5418); alu_add(gb, B);
  CYC(0x5418, 0x5419); alu_add(gb, A);
  CYC(0x5419, 0x541a); push_effect(gb, AF);
  CYC(0x541a, 0x541c); alu_sub(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x541c, 0x541e); goto select_heart_piece; }
  CYC(0x541c, 0x541e);
  CYC(0x541e, 0x5421); mem_wr(gb, 0xc6ac, A);
  CYC(0x5421, 0x5422); A = alu_dec8(gb, A);
  CYC(0x5422, 0x5425); mem_wr(gb, 0xcbe9, A);
  CYC(0x5425, 0x5428); mem_wr(gb, 0xd0ef, A);
select_heart_piece:
  CYC(0x5428, 0x5429); SET_AF(pop_effect(gb));
  CYC(0x5429, 0x542c); SET_HL(0x5465);
  CYC(0x542c, 0x542d); add_double_index_to_hl(gb, 0x542d);
  CYC(0x542d, 0x542f); D = 0xd0;
  CYC(0x542f, 0x5432); A = mem_rd(gb, 0xd0cc);
  CYC(0x5432, 0x5434); alu_add(gb, 0x11);
  CYC(0x5434, 0x5436); alu_and(gb, 0x1f);
  CYC(0x5436, 0x5437); C = A;
  CYC(0x5437, 0x5438); A = alu_dec8(gb, A);
  CYC(0x5438, 0x543a); alu_and(gb, 0x1f);
  CYC(0x543a, 0x543b); B = A;
  CYC(0x543b, 0x543d); alu_add(gb, 0x20);
  CYC(0x543d, 0x543e); E = A;
  CYC(0x543e, 0x543f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x543f, 0x5440); mem_wr(gb, DE, A);
  CYC(0x5440, 0x5441); A = B;
  CYC(0x5441, 0x5443); alu_add(gb, 0x40);
  CYC(0x5443, 0x5444); E = A;
  CYC(0x5444, 0x5445); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5445, 0x5446); mem_wr(gb, DE, A);
  CYC(0x5446, 0x5447); A = C;
  CYC(0x5447, 0x5449); alu_add(gb, 0x20);
  CYC(0x5449, 0x544a); E = A;
  CYC(0x544a, 0x544b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x544b, 0x544c); mem_wr(gb, DE, A);
  CYC(0x544c, 0x544d); A = C;
  CYC(0x544d, 0x544f); alu_add(gb, 0x40);
  CYC(0x544f, 0x5450); E = A;
  CYC(0x5450, 0x5451); A = mem_rd(gb, HL);
  CYC(0x5451, 0x5452); mem_wr(gb, DE, A);
  CYC(0x5452, 0x5454); D = 0xd1;
  CYC(0x5454, 0x5455); A = mem_rd(gb, DE);
  CYC(0x5455, 0x5457); alu_or(gb, 0x20);
  CYC(0x5457, 0x5458); mem_wr(gb, DE, A);
  CYC(0x5458, 0x5459); A = C;
  CYC(0x5459, 0x545b); alu_add(gb, 0x20);
  CYC(0x545b, 0x545c); E = A;
  CYC(0x545c, 0x545d); A = mem_rd(gb, DE);
  CYC(0x545d, 0x545f); alu_or(gb, 0x20);
  CYC(0x545f, 0x5460); mem_wr(gb, DE, A);
  CALL_C(0x5460, dmaTextboxMap_hook, 0x5173, 0x5463);
  CYC(0x5463, 0x5464); alu_or(gb, D);
  CYC(0x5464, 0x5465); ret_effect(gb);
}

void func_53eb_dmaHeartPieceDisplay_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5479, 0x547c); SET_HL(0x5720);
  CYC(0x547c, 0x547f); SET_DE(0x95d0);
  CYC(0x547f, 0x5482); SET_BC(0x001c);
  CALL_C(0x5482, queueDmaTransfer_hook, 0x058a, 0x5485);
  CYC(0x5485, 0x5488); SET_HL(0x5730);
  CYC(0x5488, 0x548a); E = 0xf0;
  CALL_C(0x548a, queueDmaTransfer_hook, 0x058a, 0x548d);
  CYC(0x548d, 0x5490); SET_HL(0x5740);
  CYC(0x5490, 0x5493); SET_DE(0x97c0);
  CALL_C(0x5493, queueDmaTransfer_hook, 0x058a, 0x5496);
  CYC(0x5496, 0x5499); SET_HL(0x5750);
  CYC(0x5499, 0x549b); E = 0xe0;
  CYC(0x549b, 0x549e); queueDmaTransfer_hook(gb);
}

void shiftTextGfxBufferLeft_hook(GB *gb) {
  CYC(0x557f, 0x5582); SET_HL(0xd200);
  CYC(0x5582, 0x5585); SET_DE(0xd220);
  CYC(0x5585, 0x5588); SET_BC(0x01e0);
  for (;;) {
    CYC(0x5588, 0x5589); A = mem_rd(gb, DE);
    CYC(0x5589, 0x558a); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x558a, 0x558b); SET_DE(DE + 1);
    CYC(0x558b, 0x558c); SET_BC(BC - 1);
    CYC(0x558c, 0x558d); A = C;
    CYC(0x558d, 0x558e); alu_or(gb, B);
    if (!(F & FZ)) { CYCT(0x558e, 0x5590); continue; }
    CYC(0x558e, 0x5590);
    break;
  }
  CYC(0x5590, 0x5593); SET_HL(0xd0d5);
  CYC(0x5593, 0x5594); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5594, 0x5595); H = mem_rd(gb, HL);
  CYC(0x5595, 0x5596); L = A;
  CYC(0x5596, 0x5597); ret_effect(gb);
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

void handleTextControlCodeWithSpecialCase_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x55a0, 0x55a2); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(0x55a2, 0x55a4); goto cmd6; }
  CYC(0x55a2, 0x55a4);
  CALL_C(0x55a4, handleTextControlCode, 0x56e4, 0x55a7);
  CYC(0x55a7, 0x55a8); alu_or(gb, D);
  CYC(0x55a8, 0x55a9); ret_effect(gb);
  return;
cmd6:
  CYC(0x55a9, 0x55ac); SET_BC(0xd3e0);
  CYC(0x55ac, 0x55af); SET_DE(0xd5e0);
  CALL_C(0x55af, handleTextControlCode, 0x56e4, 0x55b2);
  CYC(0x55b2, 0x55b3); alu_xor(gb, A);
  CYC(0x55b3, 0x55b4); ret_effect(gb);
}

void updateSelectedTextPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x55b4, getSelectedTextOptionAddress_hook, 0x55c8, 0x55b7);
  CYC(0x55b7, 0x55b9); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(0x55b9, 0x55bb); B = 0x60;
  if (!(F & FZ)) { CYCT(0x55bb, 0x55bd); goto set_position; }
  CYC(0x55bb, 0x55bd);
  CYC(0x55bd, 0x55bf); B = 0x20;
set_position:
  CYC(0x55bf, 0x55c0); A = mem_rd(gb, HL);
  CYC(0x55c0, 0x55c1); E = alu_inc8(gb, E);
  CYC(0x55c1, 0x55c2); mem_wr(gb, DE, A);
  CALL_C(0x55c2, getAddressInTextboxMap_hook, 0x55d0, 0x55c5);
  CYC(0x55c5, 0x55c7); mem_wr(gb, HL, 0x04);
  CYC(0x55c7, 0x55c8); ret_effect(gb);
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

void textOptionCode_checkDirectionButtons_updateSelectedTextOption_hook(GB *gb);
void updateSelectedTextPositionAndDmaTextboxMap_hook(GB *gb);

void moveSelectedTextOptionLeft_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(0x5602, 0x5604); E = 0xe8;
    CYC(0x5604, 0x5605); A = mem_rd(gb, DE);
    CYC(0x5605, 0x5606); A = alu_dec8(gb, A);
    CYC(0x5606, 0x5608); alu_and(gb, 0x07);
    CYC(0x5608, 0x5609); mem_wr(gb, DE, A);
    CALL_C(0x5609, getSelectedTextOptionAddress_hook, 0x55c8, 0x560c);
    CYC(0x560c, 0x560d); A = mem_rd(gb, HL);
    CYC(0x560d, 0x560e); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(0x560e, 0x560f); ret_effect(gb); return; }
    CYC(0x560e, 0x560f);
    CYCT(0x560f, 0x5611);
  }
}

void textOptionCode_checkDirectionButtons_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5611, 0x5614); A = mem_rd(gb, 0xc482);
  CYC(0x5614, 0x5616); alu_and(gb, 0xf0);
  if (F & FZ) { CYCT(0x5616, 0x5617); ret_effect(gb); return; }
  CYC(0x5616, 0x5617);
  CYC(0x5617, 0x5619); A = 0x84;
  CALL_C(0x5619, playSound_b00_hook, 0x0c98, 0x561c);
  CALL_C(0x561c, removeCursorFromSelectedTextPosition_hook, 0x55e1, 0x561f);
  CYC(0x561f, 0x5622); push_effect(gb, 0x5622);
  textOptionCode_checkDirectionButtons_updateSelectedTextOption_hook(gb);
  CYC(0x5622, 0x5624); updateSelectedTextPositionAndDmaTextboxMap_hook(gb);
}

void textOptionCode_checkDirectionButtons_updateSelectedTextOption_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5624, 0x5627); A = mem_rd(gb, 0xc482);
  CALL_C(0x5627, getHighestSetBit_hook, 0x01ea, 0x562a);
  CYC(0x562a, 0x562c); alu_sub(gb, 0x04);
  if (F & FZ) { CYCT(0x562c, 0x562e); moveSelectedTextOptionRight_hook(gb); return; }
  CYC(0x562c, 0x562e);
  CYC(0x562e, 0x562f); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x562f, 0x5631); moveSelectedTextOptionLeft_hook(gb); return; }
  CYC(0x562f, 0x5631);
  CALL_C(0x5631, getSelectedTextOptionAddress_hook, 0x55c8, 0x5634);
  CYC(0x5634, 0x5635); B = mem_rd(gb, HL);
  CYC(0x5635, 0x5637); C = 0xff;
  CYC(0x5637, 0x5639); L = 0xe0;
  CYC(0x5639, 0x563a); E = L;
  for (;;) {
    CYC(0x563a, 0x563b); A = mem_rd(gb, HL);
    CYC(0x563b, 0x563c); alu_or(gb, A);
    if (F & FZ) { CYCT(0x563c, 0x563e); goto update_end; }
    CYC(0x563c, 0x563e);
    CYC(0x563e, 0x563f); alu_sub(gb, B);
    if (!(F & FC)) { CYCT(0x563f, 0x5641); goto compare_row; }
    CYC(0x563f, 0x5641);
    CYC(0x5641, 0x5642); alu_cpl(gb);
    CYC(0x5642, 0x5643); A = alu_inc8(gb, A);
compare_row:
    CYC(0x5643, 0x5645); alu_sub(gb, 0x20);
    if (!(F & FC)) { CYCT(0x5645, 0x5647); goto compare_distance; }
    CYC(0x5645, 0x5647);
    CYC(0x5647, 0x5648); alu_cpl(gb);
    CYC(0x5648, 0x5649); A = alu_inc8(gb, A);
compare_distance:
    CYC(0x5649, 0x564a); alu_cp(gb, C);
    if (!(F & FC)) { CYCT(0x564a, 0x564c); goto next_option; }
    CYC(0x564a, 0x564c);
    CYC(0x564c, 0x564d); C = A;
    CYC(0x564d, 0x564e); E = L;
next_option:
    CYC(0x564e, 0x564f); L = alu_inc8(gb, L);
    CYCT(0x564f, 0x5651);
  }
update_end:
  CYC(0x5651, 0x5652); A = C;
  CYC(0x5652, 0x5654); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(0x5654, 0x5655); ret_effect(gb); return; }
  CYC(0x5654, 0x5655);
  CYC(0x5655, 0x5656); A = E;
  CYC(0x5656, 0x5658); alu_sub(gb, 0xe0);
  CYC(0x5658, 0x565a); E = 0xe8;
  CYC(0x565a, 0x565b); mem_wr(gb, DE, A);
  CYC(0x565b, 0x565c); ret_effect(gb);
}

void updateSelectedTextPositionAndDmaTextboxMap_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x565c, updateSelectedTextPosition_hook, 0x55b4, 0x565f);
  CYC(0x565f, 0x5662); dmaTextboxMap_hook(gb);
}

void textOptionCode_checkBButton_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5662, 0x5664); alu_and(gb, 0x02);
  if (F & FZ) { CYCT(0x5664, 0x5665); ret_effect(gb); return; }
  CYC(0x5664, 0x5665);
  CYC(0x5665, 0x5666); H = D;
  CYC(0x5666, 0x5668); L = 0xe0;
  for (;;) {
    CYC(0x5668, 0x5669); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x5669, 0x566a); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(0x566a, 0x566c); continue; }
    CYC(0x566a, 0x566c);
    break;
  }
  CYC(0x566c, 0x566d); A = L;
  CYC(0x566d, 0x566f); alu_sub(gb, 0xe2);
  CYC(0x566f, 0x5671); L = 0xe8;
  CYC(0x5671, 0x5672); mem_wr(gb, HL, A);
  CYC(0x5672, 0x5674); A = 0x84;
  CALL_C(0x5674, playSound_b00_hook, 0x0c98, 0x5677);
  CALL_C(0x5677, removeCursorFromSelectedTextPosition_hook, 0x55e1, 0x567a);
  CALL_C(0x567a, updateSelectedTextPositionAndDmaTextboxMap_hook, 0x565c, 0x567d);
  CYC(0x567d, 0x567e); alu_or(gb, D);
  CYC(0x567e, 0x567f); ret_effect(gb);
}

void pushToTextStack_hook(GB *gb) {
  CYC(0x567f, 0x5680); push_effect(gb, DE);
  CYC(0x5680, 0x5681); push_effect(gb, BC);
  CYC(0x5681, 0x5682); push_effect(gb, HL);
  CYC(0x5682, 0x5685); SET_HL(0xd1db);
  CYC(0x5685, 0x5688); SET_DE(0xd1df);
  CYC(0x5688, 0x568a); B = 0x1c;
  for (;;) {
    CYC(0x568a, 0x568b); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(0x568b, 0x568c); mem_wr(gb, DE, A);
    CYC(0x568c, 0x568d); E = alu_dec8(gb, E);
    CYC(0x568d, 0x568e); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x568e, 0x5690); continue; }
    CYC(0x568e, 0x5690);
    break;
  }
  CYC(0x5690, 0x5691); L = alu_inc8(gb, L);
  CYC(0x5691, 0x5694); SET_DE(0xd0d4);
  CYC(0x5694, 0x5695); A = mem_rd(gb, DE);
  CYC(0x5695, 0x5696); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5696, 0x5697); SET_DE(pop_effect(gb));
  CYC(0x5697, 0x5698); mem_wr(gb, HL, E);
  CYC(0x5698, 0x5699); L = alu_inc8(gb, L);
  CYC(0x5699, 0x569a); mem_wr(gb, HL, D);
  CYC(0x569a, 0x569b); L = alu_inc8(gb, L);
  CYC(0x569b, 0x569e); A = mem_rd(gb, 0xcba3);
  CYC(0x569e, 0x569f); mem_wr(gb, HL, A);
  CYC(0x569f, 0x56a0); H = D;
  CYC(0x56a0, 0x56a1); L = E;
  CYC(0x56a1, 0x56a2); SET_BC(pop_effect(gb));
  CYC(0x56a2, 0x56a3); SET_DE(pop_effect(gb));
  CYC(0x56a3, 0x56a4); ret_effect(gb);
}

void popFromTextStack_hook(GB *gb) {
  CYC(0x56a4, 0x56a5); push_effect(gb, DE);
  CYC(0x56a5, 0x56a6); push_effect(gb, BC);
  CYC(0x56a6, 0x56a9); SET_HL(0xd1c3);
  CYC(0x56a9, 0x56aa); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x56aa, 0x56ad); mem_wr(gb, 0xcba3, A);
  CYC(0x56ad, 0x56ae); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x56ae, 0x56b1); SET_DE(0xd0d6);
  CYC(0x56b1, 0x56b2); mem_wr(gb, DE, A);
  CYC(0x56b2, 0x56b3); B = A;
  CYC(0x56b3, 0x56b4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x56b4, 0x56b5); E = alu_dec8(gb, E);
  CYC(0x56b5, 0x56b6); mem_wr(gb, DE, A);
  CYC(0x56b6, 0x56b7); C = A;
  CYC(0x56b7, 0x56b8); A = mem_rd(gb, HL);
  CYC(0x56b8, 0x56b9); E = alu_dec8(gb, E);
  CYC(0x56b9, 0x56ba); mem_wr(gb, DE, A);
  CYC(0x56ba, 0x56bb); push_effect(gb, BC);
  CYC(0x56bb, 0x56be); SET_DE(0xd1c4);
  CYC(0x56be, 0x56c0); B = 0x1c;
  for (;;) {
    CYC(0x56c0, 0x56c1); A = mem_rd(gb, DE);
    CYC(0x56c1, 0x56c2); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x56c2, 0x56c3); E = alu_inc8(gb, E);
    CYC(0x56c3, 0x56c4); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(0x56c4, 0x56c6); continue; }
    CYC(0x56c4, 0x56c6);
    break;
  }
  CYC(0x56c6, 0x56c7); alu_xor(gb, A);
  CYC(0x56c7, 0x56c8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x56c8, 0x56c9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x56c9, 0x56ca); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x56ca, 0x56cb); mem_wr(gb, HL, A);
  CYC(0x56cb, 0x56cc); SET_HL(pop_effect(gb));
  CYC(0x56cc, 0x56cd); SET_BC(pop_effect(gb));
  CYC(0x56cd, 0x56ce); SET_DE(pop_effect(gb));
  CYC(0x56ce, 0x56cf); ret_effect(gb);
}
