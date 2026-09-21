#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void getExtraTextIndex_hook(GB *gb);
void handleTextControlCode_hook(GB *gb);
void updateTextbox__updateText_hook(GB *gb);
void updateTextbox__textOption_hook(GB *gb);
void updateTextbox__inventoryText_hook(GB *gb);
void updateTextbox__standardText_hook(GB *gb);
void standardTextState0_hook(GB *gb);
void standardTextState1_hook(GB *gb);
void standardTextState2_hook(GB *gb);
void standardTextState2__textFinished_hook(GB *gb);
void standardTextState3_hook(GB *gb);
void standardTextState5_hook(GB *gb);
void standardTextState6_hook(GB *gb);
void standardTextState7_hook(GB *gb);
void standardTextState8_hook(GB *gb);
void standardTextStateB_hook(GB *gb);
void standardTextStateE_hook(GB *gb);
void standardTextStateF_hook(GB *gb);
void standardTextStateF__label_3f_096_hook(GB *gb);
void standardTextStateF__checkShouldExit_hook(GB *gb);
void standardTextStateF__checkShouldExit__nonExitable_hook(GB *gb);
void standardTextStateF__checkShouldExit__end_hook(GB *gb);
void standardTextState10_hook(GB *gb);
void textOptionCode_hook(GB *gb);
void textOptionCode__state01_hook(GB *gb);
void textOptionCode__state02_hook(GB *gb);
void textOptionCode__state03_hook(GB *gb);
void textOptionCode__state04_hook(GB *gb);
void inventoryTextCode_hook(GB *gb);
void inventoryTextCode__state00__stopText_hook(GB *gb);
void inventoryTextCode__state00__end_hook(GB *gb);
void inventoryTextCode__state01_hook(GB *gb);
void inventoryTextCode__state02_hook(GB *gb);
void inventoryTextCode__drawSpaceWithoutSavingTextAddress_hook(GB *gb);
void inventoryTextCode__state03_hook(GB *gb);
void inventoryTextCode__insertSpace_hook(GB *gb);
void inventoryTextCode__drawSpace_hook(GB *gb);
void inventoryTextCode__drawCharacter_hook(GB *gb);
void inventoryTextCode__saveTextAddressAndDmaTextGfxBuffer_hook(GB *gb);
void inventoryTextCode__dmaTextGfxBuffer_hook(GB *gb);
void inventoryTextCode__state04_hook(GB *gb);
void inventoryTextCode__state05_hook(GB *gb);
void inventoryTextCode__state06_hook(GB *gb);
void inventoryTextCode__state07_hook(GB *gb);
void initTextboxStuff_hook(GB *gb);
static void add_double_index_to_hl(GB *gb, uint16_t return_address);
static void add_a_to_hl(GB *gb);

static uint16_t textbox_jump_table(GB *gb) {
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

void updateTextbox_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x07;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+6); D = 0xd0;
  CYC(b_+6, b_+9); A = mem_rd(gb, wTextIsActive);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+10, b_+12);
  } else {
    CYC(b_+10, b_+12);
    CYC(b_+12, b_+15); mem_wr(gb, wTextDisplayMode, A);
    CYC(b_+15, b_+16); H = D;
    CYC(b_+16, b_+18); L = 0xc0;
    CYC(b_+18, b_+20); mem_wr(gb, HL, 0x0f);
    CYC(b_+20, b_+21); L = alu_inc8(gb, L);
    CYC(b_+21, b_+23); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
  }
  CALL_C(b_+23, updateTextbox__updateText_hook, b_+37, b_+26);
  CYC(b_+26, b_+29); A = mem_rd(gb, wTextIsActive);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+30, b_+31); ret_effect(gb);
    return;
  }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+34, b_+37);
  stubThreadStart_hook(gb);
}

void updateTextbox__updateText_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp;
  CYC(b_+37, b_+40); A = mem_rd(gb, wTextIsActive);
  CYC(b_+40, b_+42); alu_cp(gb, 0x80);
  if (F & FZ) {
    CYCT(b_+42, b_+43); ret_effect(gb);
    return;
  }
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+45); E = 0xc0;
  CYC(b_+45, b_+48); A = mem_rd(gb, wTextDisplayMode);
  CYC(b_+48, b_+49); push_effect(gb, b_+49);
  do { uint16_t jt_ = (textbox_jump_table(gb));
    if (jt_ == b_+55) { updateTextbox__standardText_hook(gb); return; }
    else if (jt_ == b_+91) { updateTextbox__textOption_hook(gb); return; }
    else if (jt_ == b_+103) { updateTextbox__inventoryText_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void updateTextbox__textOption_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp;
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+93); push_effect(gb, b_+93);
  do { uint16_t jt_ = (textbox_jump_table(gb));
    if (jt_ == SYM(textOptionCode) && hook_enabled_at(gb, SYM(textOptionCode))) { textOptionCode_hook(gb); return; }
    else if (jt_ == SYM(textOptionCode__state01) && hook_enabled_at(gb, SYM(textOptionCode__state01))) { textOptionCode__state01_hook(gb); return; }
    else if (jt_ == SYM(textOptionCode__state02) && hook_enabled_at(gb, SYM(textOptionCode__state02))) { textOptionCode__state02_hook(gb); return; }
    else if (jt_ == SYM(textOptionCode__state03) && hook_enabled_at(gb, SYM(textOptionCode__state03))) { textOptionCode__state03_hook(gb); return; }
    else if (jt_ == SYM(textOptionCode__state04) && hook_enabled_at(gb, SYM(textOptionCode__state04))) { textOptionCode__state04_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void updateTextbox__inventoryText_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp;
  CYC(b_+103, b_+104); A = mem_rd(gb, DE);
  CYC(b_+104, b_+105); push_effect(gb, b_+105);
  do { uint16_t jt_ = (textbox_jump_table(gb));
    if (jt_ == SYM(inventoryTextCode) && hook_enabled_at(gb, SYM(inventoryTextCode))) { inventoryTextCode_hook(gb); return; }
    else if (jt_ == SYM(inventoryTextCode__state01) && hook_enabled_at(gb, SYM(inventoryTextCode__state01))) { inventoryTextCode__state01_hook(gb); return; }
    else if (jt_ == SYM(inventoryTextCode__state02) && hook_enabled_at(gb, SYM(inventoryTextCode__state02))) { inventoryTextCode__state02_hook(gb); return; }
    else if (jt_ == SYM(inventoryTextCode__state03) && hook_enabled_at(gb, SYM(inventoryTextCode__state03))) { inventoryTextCode__state03_hook(gb); return; }
    else if (jt_ == SYM(inventoryTextCode__state04) && hook_enabled_at(gb, SYM(inventoryTextCode__state04))) { inventoryTextCode__state04_hook(gb); return; }
    else if (jt_ == SYM(inventoryTextCode__state05) && hook_enabled_at(gb, SYM(inventoryTextCode__state05))) { inventoryTextCode__state05_hook(gb); return; }
    else if (jt_ == SYM(inventoryTextCode__state06) && hook_enabled_at(gb, SYM(inventoryTextCode__state06))) { inventoryTextCode__state06_hook(gb); return; }
    else if (jt_ == SYM(inventoryTextCode__state07) && hook_enabled_at(gb, SYM(inventoryTextCode__state07))) { inventoryTextCode__state07_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void updateTextbox__standardText_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp;
  CYC(b_+55, b_+56); A = mem_rd(gb, DE);
  CYC(b_+56, b_+57); push_effect(gb, b_+57);
  do { uint16_t jt_ = (textbox_jump_table(gb));
    if (jt_ == b_+121) { standardTextState0_hook(gb); return; }
    else if (jt_ == b_+133) { standardTextState1_hook(gb); return; }
    else if (jt_ == b_+151) { standardTextState2_hook(gb); return; }
    else if (jt_ == b_+200) { standardTextState3_hook(gb); return; }
    else if (jt_ == b_+225) { standardTextState5_hook(gb); return; }
    else if (jt_ == b_+245) { standardTextState6_hook(gb); return; }
    else if (jt_ == b_+251) { standardTextState7_hook(gb); return; }
    else if (jt_ == b_+260) { standardTextState8_hook(gb); return; }
    else if (jt_ == b_+274) { standardTextStateB_hook(gb); return; }
    else if (jt_ == b_+297) { standardTextStateE_hook(gb); return; }
    else if (jt_ == b_+312) { standardTextStateF_hook(gb); return; }
    else if (jt_ == b_+418) { standardTextState10_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void standardTextState0_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+121, b_+123); A = 0x01;
  CYC(b_+123, b_+124); mem_wr(gb, DE, A);
  CALL_C(b_+124, saveTilesUnderTextbox_hook, SYM(saveTilesUnderTextbox), b_+127);
  CALL_C(b_+127, initTextboxMapping_hook, SYM(initTextboxMapping), b_+130);
  CYC(b_+130, b_+133);
  dmaTextboxMap_hook(gb);
}

void standardTextState1_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+133, b_+134); H = D;
  CYC(b_+134, b_+136); L = 0xc0;
  CYC(b_+136, b_+137); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+137, b_+139); L = 0xd3;
  CYC(b_+139, b_+141); mem_wr(gb, HL, 0x40);
  CYC(b_+141, b_+143); L = 0xc5;
  CYC(b_+143, b_+144); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+144, b_+145); mem_wr(gb, HL, A);
  CALL_C(b_+145, drawLineOfText_hook, SYM(drawLineOfText), b_+148);
  CYC(b_+148, b_+151);
  dmaTextGfxBuffer_hook(gb);
}

void standardTextState2_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+151, getNextCharacterToDisplay_hook, SYM(getNextCharacterToDisplay), b_+154);
  if (F & FZ) CYCT(b_+154, b_+156);
  else {
    CYC(b_+154, b_+156);
    CALL_C(b_+156, updateCharacterDisplayTimer_hook, SYM(updateCharacterDisplayTimer), b_+159);
    if (!(F & FZ)) { CYCT(b_+159, b_+160); ret_effect(gb); return; }
    CYC(b_+159, b_+160);
    CALL_C(b_+160, displayNextTextCharacter_hook, SYM(displayNextTextCharacter), b_+163);
    CALL_C(b_+163, dmaTextboxMap_hook, SYM(dmaTextboxMap), b_+166);
    CYC(b_+166, b_+168); D = 0xd0;
    CALL_C(b_+168, getNextCharacterToDisplay_hook, SYM(getNextCharacterToDisplay), b_+171);
    if (!(F & FZ)) { CYCT(b_+171, b_+172); ret_effect(gb); return; }
    CYC(b_+171, b_+172);
  }
  CALL_C(b_+172, func_53eb_hook, SYM(func_53eb), b_+175);
  if (!(F & FZ)) { CYCT(b_+175, b_+176); ret_effect(gb); return; }
  CYC(b_+175, b_+176);
  CYC(b_+176, b_+178); D = 0xd0;
  CALL_C(b_+178, func_5296_hook, SYM(func_5296), b_+181);
  if (!(F & FZ)) { CYCT(b_+181, b_+182); ret_effect(gb); return; }
  CYC(b_+181, b_+182);
  CYC(b_+182, b_+183); H = D;
  CYC(b_+183, b_+185); L = 0xc2;
  CYC(b_+185, b_+186); A = mem_rd(gb, HL);
  CYC(b_+186, b_+187); alu_or(gb, A);
  CYC(b_+187, b_+189); L = 0xc0;
  if (F & FZ) {
    CYCT(b_+189, b_+191);
    standardTextState2__textFinished_hook(gb);
    return;
  }
  CYC(b_+189, b_+191);
  CYC(b_+191, b_+192); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+192, b_+194); L = 0xc5;
  CYC(b_+194, b_+195); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+195, b_+196); mem_wr(gb, HL, A);
  CYC(b_+196, b_+197); ret_effect(gb);
}

void standardTextState2__textFinished_hook(GB *gb) {
  BASE(updateTextbox);
  CYC(b_+197, b_+199); mem_wr(gb, HL, 0x0f);
  CYC(b_+199, b_+200); ret_effect(gb);
}

void standardTextState3_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+200, updateCharacterDisplayTimer_hook, SYM(updateCharacterDisplayTimer), b_+203);
  if (!(F & FZ)) { CYCT(b_+203, b_+204); ret_effect(gb); return; }
  CYC(b_+203, b_+204);
  CALL_C(b_+204, drawLineOfText_hook, SYM(drawLineOfText), b_+207);
  CYC(b_+207, b_+209); A = 0x02;
  CALL_C(b_+209, dmaTextGfxBuffer_hook, SYM(dmaTextGfxBuffer), b_+212);
  CYC(b_+212, b_+215); SET_HL(w7TextDisplayState);
  CYC(b_+215, b_+216); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+216, b_+218); L = 0xd3;
  CYC(b_+218, b_+220); mem_wr(gb, HL, 0x60);
  CYC(b_+220, b_+222); L = 0xc5;
  CYC(b_+222, b_+223); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+223, b_+224); mem_wr(gb, HL, A);
  CYC(b_+224, b_+225); ret_effect(gb);
}

void standardTextState5_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+225, b_+228); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+228, b_+230); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(b_+230, b_+233); updateTextboxArrow_hook(gb); return; }
  CYC(b_+230, b_+233);
  CYC(b_+233, b_+235); A = 0x89;
  CALL_C(b_+235, playSound_b00_hook, SYM(playSound_b00), b_+238);
  CYC(b_+238, b_+239); H = D;
  CYC(b_+239, b_+241); L = 0xc1;
  CYC(b_+241, b_+243); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x01);
  CYC(b_+243, b_+245);
  standardTextStateB_hook(gb);
}

void standardTextState6_hook(GB *gb) {
  BASE(updateTextbox);
  CYC(b_+245, b_+246); H = D;
  CYC(b_+246, b_+247); L = E;
  CYC(b_+247, b_+248); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+248, b_+251);
  dmaTextboxMap_hook(gb);
}

void standardTextState7_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+251, b_+252); H = D;
  CYC(b_+252, b_+253); L = E;
  CYC(b_+253, b_+254); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+254, shiftTextboxMapUp_hook, SYM(shiftTextboxMapUp), b_+257);
  CYC(b_+257, b_+260);
  subFirstRowOfTextMapBy20_hook(gb);
}

void standardTextState8_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+260, b_+261); H = D;
  CYC(b_+261, b_+262); L = E;
  CYC(b_+262, b_+263); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+263, b_+265); L = 0xc5;
  CYC(b_+265, b_+266); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+266, b_+267); mem_wr(gb, HL, A);
  CALL_C(b_+267, dmaTextboxMap_hook, SYM(dmaTextboxMap), b_+270);
  CYC(b_+270, b_+271); alu_xor(gb, A);
  CYC(b_+271, b_+274);
  dmaTextGfxBuffer_hook(gb);
}

void standardTextStateB_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+274, b_+275); H = D;
  CYC(b_+275, b_+276); L = E;
  CYC(b_+276, b_+277); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+277, b_+279); L = 0xcc;
  CYC(b_+279, b_+280); A = mem_rd(gb, HL);
  CYC(b_+280, b_+282); alu_add(gb, 0x12);
  CYC(b_+282, b_+284); alu_and(gb, 0x1f);
  CYC(b_+284, b_+286); alu_add(gb, 0x80);
  CYC(b_+286, b_+287); L = A;
  CYC(b_+287, b_+289); H = 0xd0;
  CYC(b_+289, b_+291); mem_wr(gb, HL, 0x02);
  CALL_C(b_+291, shiftTextboxMapUp_hook, SYM(shiftTextboxMapUp), b_+294);
  CYC(b_+294, b_+297);
  clearTopRowOfTextMap_hook(gb);
}

void standardTextStateE_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+297, b_+298); H = D;
  CYC(b_+298, b_+299); L = E;
  CYC(b_+299, b_+301); mem_wr(gb, HL, 0x03);
  CYC(b_+301, b_+303); L = 0xc5;
  CYC(b_+303, b_+304); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+304, b_+305); mem_wr(gb, HL, A);
  CALL_C(b_+305, dmaTextboxMap_hook, SYM(dmaTextboxMap), b_+308);
  CYC(b_+308, b_+309); alu_xor(gb, A);
  CYC(b_+309, b_+312);
  dmaTextGfxBuffer_hook(gb);
}

void standardTextStateF_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+312, b_+313); H = D;
  CYC(b_+313, b_+315); L = 0xef;
  CYC(b_+315, b_+317); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+317, b_+319);
    standardTextStateF__label_3f_096_hook(gb);
    return;
  }
  CYC(b_+317, b_+319);
  CYC(b_+319, b_+322); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+322, b_+324); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(b_+324, b_+325); ret_effect(gb); return; }
  CYC(b_+324, b_+325);
  CYC(b_+325, b_+327); mem_wr(gb, HL, 0x00);
  CYC(b_+327, b_+328); L = E;
  CYC(b_+328, b_+330); mem_wr(gb, HL, 0x00);
  CYC(b_+330, b_+332); A = GV(GV(0x49, 0x24), 0x24);
  CYC(b_+332, b_+335); mem_wr(gb, wTextIndexL, A);
  CYC(b_+335, b_+337); A = 0x00;
  CYC(b_+337, b_+339); alu_add(gb, 0x04);
  CYC(b_+339, b_+342); mem_wr(gb, wTextIndexH, A);
  CALL_C(b_+342, checkInitialTextCommands_hook, SYM(checkInitialTextCommands), b_+345);
  CYC(b_+345, b_+347); A = 0x8b;
  CALL_C(b_+347, playSound_b00_hook, SYM(playSound_b00), b_+350);
  CYC(b_+350, b_+352); A = 0x2a;
  CYC(b_+352, b_+354); C = 0x04;
  CYC(b_+354, b_+357);
  giveTreasure_hook(gb);
}

void standardTextStateF__label_3f_096_hook(GB *gb) {
  BASE(updateTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+357, b_+359); L = 0xc1;
  CYC(b_+359, b_+361); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) CYCT(b_+361, b_+363);
  else {
    CYC(b_+361, b_+363);
    CALL_C(b_+363, standardTextStateF__checkShouldExit_hook, b_+385, b_+366);
    if (F & FZ) { CYCT(b_+366, b_+367); ret_effect(gb); return; }
    CYC(b_+366, b_+367);
  }
  CYC(b_+367, b_+368); L = E;
  CYC(b_+368, b_+369); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+369, b_+371); L = 0xef;
  CYC(b_+371, b_+373); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) CYCT(b_+373, b_+375);
  else {
    CYC(b_+373, b_+375);
    CYC(b_+375, b_+377); A = 0x29;
    CYC(b_+377, b_+379); C = 0x40;
    CALL_C(b_+379, giveTreasure_hook, SYM(giveTreasure), b_+382);
  }
  CYC(b_+382, b_+385);
  saveTilesUnderTextbox_hook(gb);
}

void standardTextStateF__checkShouldExit_hook(GB *gb) {
  BASE(updateTextbox);
  CYC(b_+385, b_+388); A = mem_rd(gb, wTextboxFlags);
  CYC(b_+388, b_+390); alu_bit(gb, 1, A);
  if (!(F & FZ)) {
    CYCT(b_+390, b_+392);
    standardTextStateF__checkShouldExit__nonExitable_hook(gb);
    return;
  }
  CYC(b_+390, b_+392);
  CYC(b_+392, b_+394); L = 0xeb;
  CYC(b_+394, b_+395); A = mem_rd(gb, HL);
  CYC(b_+395, b_+396); alu_or(gb, A);
  if (F & FZ) CYCT(b_+396, b_+398);
  else {
    CYC(b_+396, b_+398);
    CYC(b_+398, b_+399); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (F & FZ) {
      CYCT(b_+399, b_+401);
      standardTextStateF__checkShouldExit__end_hook(gb);
      return;
    }
    CYC(b_+399, b_+401);
  }
  CYC(b_+401, b_+404); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+404, b_+405); alu_or(gb, A);
  CYC(b_+405, b_+406); ret_effect(gb);
}

void standardTextStateF__checkShouldExit__nonExitable_hook(GB *gb) {
  BASE(updateTextbox);
  CYC(b_+406, b_+408); A &= ~0x02;
  CYC(b_+408, b_+411); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+411, b_+413); A = 0x80;
  CYC(b_+413, b_+416); mem_wr(gb, wTextIsActive, A);
  standardTextStateF__checkShouldExit__end_hook(gb);
}

void standardTextStateF__checkShouldExit__end_hook(GB *gb) {
  BASE(updateTextbox);
  CYC(b_+416, b_+417); alu_or(gb, D);
  CYC(b_+417, b_+418); ret_effect(gb);
}

void standardTextState10_hook(GB *gb) {
  BASE(updateTextbox);
  CYC(b_+418, b_+419); alu_xor(gb, A);
  CYC(b_+419, b_+422); mem_wr(gb, wTextIsActive, A);
  CYC(b_+422, b_+425);
  dmaTextboxMap_hook(gb);
}

void textOptionCode_hook(GB *gb) {
  BASE(textOptionCode);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+6); A = mem_rd(gb, wTextSpeed);
  CYC(b_+6, b_+9); SET_HL(b_+15);
  CYC(b_+9, b_+10); push_effect(gb, b_+10); add_a_to_hl(gb);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); E = 0xc6;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void textOptionCode__state01_hook(GB *gb) {
  BASE(textOptionCode);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = 0xc6;
  CYC(b_+23, b_+24); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+24, b_+25); ret_effect(gb); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); L = E;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+30);
  updateSelectedTextPositionAndDmaTextboxMap_hook(gb);
}

void textOptionCode__state02_hook(GB *gb) {
  BASE(textOptionCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+30, b_+33); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+33, b_+35); alu_and(gb, 0x03);
  if (F & FZ) {
    CYCT(b_+35, b_+38);
    TAIL(textOptionCode_checkDirectionButtons);
  }
  CYC(b_+35, b_+38);
  CALL_C(b_+38, textOptionCode_checkBButton_hook, SYM(textOptionCode_checkBButton), b_+41);
  if (!(F & FZ)) { CYCT(b_+41, b_+42); ret_effect(gb); return; }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+44); A = 0x56;
  CALL_C(b_+44, playSound_b00_hook, SYM(playSound_b00), b_+47);
  CYC(b_+47, b_+50); SET_HL(w7TextDisplayState);
  CYC(b_+50, b_+51); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+51, b_+53); L = 0xe8;
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+57); mem_wr(gb, wSelectedTextOption, A);
  CYC(b_+57, b_+60); A = mem_rd(gb, wTextboxFlags);
  CYC(b_+60, b_+62); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+62, b_+63); ret_effect(gb); return; }
  CYC(b_+62, b_+63);
  CYC(b_+63, b_+65); A &= ~0x02;
  CYC(b_+65, b_+68); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+68, b_+70); A = 0x80;
  CYC(b_+70, b_+73); mem_wr(gb, wTextIsActive, A);
  CYC(b_+73, b_+74); ret_effect(gb);
}

void textOptionCode__state03_hook(GB *gb) {
  BASE(textOptionCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+74, b_+75); H = D;
  CYC(b_+75, b_+76); L = E;
  CYC(b_+76, b_+77); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+77, b_+78); L = alu_inc8(gb, L);
  CYC(b_+78, b_+80); alu_bit(gb, 4, mem_rd(gb, HL));
  if (F & FZ) CYCT(b_+80, b_+82);
  else {
    CYC(b_+80, b_+82);
    CYC(b_+82, b_+83); push_effect(gb, HL);
    CALL_C(b_+83, readNextTextByte_hook, SYM(readNextTextByte), b_+86);
    CYC(b_+86, b_+87); SET_HL(pop_effect(gb));
    CYC(b_+87, b_+89); alu_cp(gb, 0xff);
    if (F & FZ) CYCT(b_+89, b_+92);
    else {
      CYC(b_+89, b_+92);
      CYC(b_+92, b_+95); mem_wr(gb, wTextIndexL, A);
      CALL_C(b_+95, checkInitialTextCommands_hook, SYM(checkInitialTextCommands), b_+98);
      CYC(b_+98, b_+101);
      TAIL(func_53dd);
    }
  }
  CYC(b_+101, b_+103); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
  CYC(b_+103, b_+104); L = alu_inc8(gb, L);
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x00);
  CYC(b_+106, b_+108); L = 0xc0;
  CYC(b_+108, b_+110); mem_wr(gb, HL, 0x0f);
  CYC(b_+110, b_+112); A = 0x00;
  CYC(b_+112, b_+115); mem_wr(gb, wTextDisplayMode, A);
  CYC(b_+115, b_+116); ret_effect(gb);
}

void textOptionCode__state04_hook(GB *gb) {
  BASE(textOptionCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+116, b_+118); A = 0x00;
  CYC(b_+118, b_+121); mem_wr(gb, wTextDisplayMode, A);
  CYC(b_+121, b_+122); H = D;
  CYC(b_+122, b_+123); L = E;
  CYC(b_+123, b_+125); mem_wr(gb, HL, 0x02);
  CYC(b_+125, b_+126); L = alu_inc8(gb, L);
  CYC(b_+126, b_+128); mem_wr(gb, HL, 0x00);
  CYC(b_+128, b_+130); L = 0xc5;
  CYC(b_+130, b_+131); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+131, b_+132); mem_wr(gb, HL, A);
  CYC(b_+132, b_+134); L = 0xd3;
  CYC(b_+134, b_+136); mem_wr(gb, HL, 0x40);
  CYC(b_+136, b_+138); L = 0xe0;
  CYC(b_+138, b_+140); B = 0x0a;
  CALL_C(b_+140, clearMemory_hook, SYM(clearMemory), b_+143);
  CALL_C(b_+143, drawLineOfText_hook, SYM(drawLineOfText), b_+146);
  CYC(b_+146, b_+149);
  dmaTextGfxBuffer_hook(gb);
}

void inventoryTextCode_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); L = E;
  CYC(b_+2, b_+3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+3, b_+5); L = 0xec;
  CYC(b_+5, b_+8); A = mem_rd(gb, wTextIndexL);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+11); L = 0xde;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x28);
  CYC(b_+13, b_+15); L = 0xed;
  CYC(b_+15, b_+17); A = 0xff;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+20); L = 0xc2;
  CYC(b_+20, b_+21); mem_wr(gb, HL, A);
  CALL_C(b_+21, doInventoryTextFirstPass_hook, SYM(doInventoryTextFirstPass), b_+24);
  CYC(b_+24, b_+26); D = 0xd0;
  if (F & FZ) CYCT(b_+26, b_+28);
  else {
    CYC(b_+26, b_+28);
    CYC(b_+28, b_+30); E = 0xd5;
    CYC(b_+30, b_+31); A = L;
    CYC(b_+31, b_+32); mem_wr(gb, DE, A);
    CYC(b_+32, b_+33); E = alu_inc8(gb, E);
    CYC(b_+33, b_+34); A = H;
    CYC(b_+34, b_+35); mem_wr(gb, DE, A);
    CYC(b_+35, b_+37); E = 0xed;
    CYC(b_+37, b_+38); A = mem_rd(gb, DE);
    CYC(b_+38, b_+39); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+39, b_+41);
      goto check_inventory_text_status;
    }
    else {
      CYC(b_+39, b_+41);
      CYC(b_+41, b_+42); A = alu_inc8(gb, A);
      CYC(b_+42, b_+44); A = alu_srl(gb, A);
      CYC(b_+44, b_+45); mem_wr(gb, DE, A);
    }
  }
  CYC(b_+45, b_+47); E = 0xed;
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); A = alu_inc8(gb, A);
  if (F & FZ) {
    CYCT(b_+49, b_+51);
    TAIL(inventoryTextCode__state00__stopText);
  }
  CYC(b_+49, b_+51);
check_inventory_text_status:
  CYC(b_+51, b_+53); E = 0xc2;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+55); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+55, b_+57);
    TAIL(inventoryTextCode__state00__end);
  }
  CYC(b_+55, b_+57);
  inventoryTextCode__state00__stopText_hook(gb);
}

void inventoryTextCode__state00__stopText_hook(GB *gb) {
  BASE(inventoryTextCode);
  CYC(b_+57, b_+60); mem_wr(gb, wTextIsActive, A);
  inventoryTextCode__state00__end_hook(gb);
}

void inventoryTextCode__state00__end_hook(GB *gb) {
  BASE(inventoryTextCode);
  CYC(b_+60, b_+62); A = 0x17;
  CYC(b_+62, b_+65);
  loadUncompressedGfxHeader_hook(gb);
}

void inventoryTextCode__state01_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+65, decInvTextScrollTimer_hook, SYM(decInvTextScrollTimer), b_+68);
  if (!(F & FZ)) { CYCT(b_+68, b_+69); ret_effect(gb); return; }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x01);
  CYC(b_+71, b_+72); L = E;
  CYC(b_+72, b_+73); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+73, b_+75); L = 0xc2;
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0xff);
  CYC(b_+77, b_+78); ret_effect(gb);
}

void inventoryTextCode__state02_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+78, decInvTextScrollTimer_hook, SYM(decInvTextScrollTimer), b_+81);
  if (!(F & FZ)) { CYCT(b_+81, b_+82); ret_effect(gb); return; }
  CYC(b_+81, b_+82);
  CALL_C(b_+82, shiftTextGfxBufferLeft_hook, SYM(shiftTextGfxBufferLeft), b_+85);
  for (;;) {
    CALL_C(b_+85, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+88);
    CYC(b_+88, b_+90); alu_cp(gb, 0x10);
    if (!(F & FC)) {
      CYCT(b_+90, b_+92);
      TAIL(inventoryTextCode__drawCharacter);
    }
    CYC(b_+90, b_+92);
    CYC(b_+92, b_+94); alu_cp(gb, 0x01);
    if (F & FZ) {
      CYCT(b_+94, b_+96);
      TAIL(inventoryTextCode__drawSpace);
    }
    CYC(b_+94, b_+96);
    CALL_C(b_+96, handleTextControlCodeWithSpecialCase_hook, SYM(handleTextControlCodeWithSpecialCase), b_+99);
    if (F & FZ) {
      CYCT(b_+99, b_+101);
      TAIL(inventoryTextCode__saveTextAddressAndDmaTextGfxBuffer);
    }
    CYC(b_+99, b_+101);
    CYC(b_+101, b_+104); A = mem_rd(gb, w7TextStatus);
    CYC(b_+104, b_+105); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+105, b_+107); continue; }
    CYC(b_+105, b_+107);
    break;
  }
  CYC(b_+107, b_+108); A = L;
  CYC(b_+108, b_+109); B = H;
  CYC(b_+109, b_+112); SET_HL(w7TextStatus);
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0xff);
  CYC(b_+114, b_+116); L = 0xdf;
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x10);
  CYC(b_+118, b_+120); L = 0xc0;
  CYC(b_+120, b_+121); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+121, b_+123); L = 0xd5;
  CYC(b_+123, b_+124); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+124, b_+125); mem_wr(gb, HL, B);
  inventoryTextCode__drawSpaceWithoutSavingTextAddress_hook(gb);
}

void inventoryTextCode__drawSpaceWithoutSavingTextAddress_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+125, b_+127); A = 0x20;
  CYC(b_+127, b_+130); SET_BC(w7TextGfxBuffer + 0x1e0);
  CALL_C(b_+130, retrieveTextCharacter_hook, SYM(retrieveTextCharacter), b_+133);
  CYC(b_+133, b_+135);
  inventoryTextCode__dmaTextGfxBuffer_hook(gb);
}

void inventoryTextCode__state03_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+135, decInvTextScrollTimer_hook, SYM(decInvTextScrollTimer), b_+138);
  if (!(F & FZ)) { CYCT(b_+138, b_+139); ret_effect(gb); return; }
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+140); L = alu_inc8(gb, L);
  CYC(b_+140, b_+141); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+141, b_+143);
    TAIL(inventoryTextCode__insertSpace);
  }
  CYC(b_+141, b_+143);
  CYC(b_+143, b_+144); L = E;
  CYC(b_+144, b_+145); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+145, b_+147); L = 0xec;
  CYC(b_+147, b_+148); A = mem_rd(gb, HL);
  CYC(b_+148, b_+151); mem_wr(gb, wTextIndexL, A);
  CYC(b_+151, b_+154); A = mem_rd(gb, wTextIndexH_backup);
  CYC(b_+154, b_+157); mem_wr(gb, wTextIndexH, A);
  CALL_C(b_+157, checkInitialTextCommands_hook, SYM(checkInitialTextCommands), b_+160);
  inventoryTextCode__insertSpace_hook(gb);
}

void inventoryTextCode__insertSpace_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+160, shiftTextGfxBufferLeft_hook, SYM(shiftTextGfxBufferLeft), b_+163);
  inventoryTextCode__drawSpace_hook(gb);
}

void inventoryTextCode__drawSpace_hook(GB *gb) {
  BASE(inventoryTextCode);
  CYC(b_+163, b_+165); A = 0x20;
  inventoryTextCode__drawCharacter_hook(gb);
}

void inventoryTextCode__drawCharacter_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+165, b_+168); SET_BC(w7TextGfxBuffer + 0x1e0);
  CALL_C(b_+168, retrieveTextCharacter_hook, SYM(retrieveTextCharacter), b_+171);
  inventoryTextCode__saveTextAddressAndDmaTextGfxBuffer_hook(gb);
}

void inventoryTextCode__saveTextAddressAndDmaTextGfxBuffer_hook(GB *gb) {
  BASE(inventoryTextCode);
  CYC(b_+171, b_+172); A = L;
  CYC(b_+172, b_+175); mem_wr(gb, w7TextAddress, A);
  CYC(b_+175, b_+176); A = H;
  CYC(b_+176, b_+179); mem_wr(gb, w7TextAddress + 1, A);
  inventoryTextCode__dmaTextGfxBuffer_hook(gb);
}

void inventoryTextCode__dmaTextGfxBuffer_hook(GB *gb) {
  BASE(inventoryTextCode);
  CYC(b_+179, b_+181); A = 0x17;
  CYC(b_+181, b_+184);
  loadUncompressedGfxHeader_hook(gb);
}

void inventoryTextCode__state04_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+184, decInvTextScrollTimer_hook, SYM(decInvTextScrollTimer), b_+187);
  if (!(F & FZ)) { CYCT(b_+187, b_+188); ret_effect(gb); return; }
  CYC(b_+187, b_+188);
  CALL_C(b_+188, shiftTextGfxBufferLeft_hook, SYM(shiftTextGfxBufferLeft), b_+191);
  for (;;) {
    CALL_C(b_+191, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+194);
    CYC(b_+194, b_+196); alu_cp(gb, 0x10);
    if (!(F & FC)) {
      CYCT(b_+196, b_+198);
      TAIL(inventoryTextCode__drawCharacter);
    }
    CYC(b_+196, b_+198);
    CYC(b_+198, b_+200); alu_cp(gb, 0x01);
    if (F & FZ) {
      CYC(b_+200, b_+202);
      CYC(b_+202, b_+203); A = L;
      CYC(b_+203, b_+204); B = H;
      CYC(b_+204, b_+207); SET_HL(w7TextAddress);
      CYC(b_+207, b_+209); L = 0xd5;
      CYC(b_+209, b_+210); mem_wr(gb, HL, A); SET_HL(HL + 1);
      CYC(b_+210, b_+211); mem_wr(gb, HL, B);
      CYC(b_+211, b_+213); L = 0xed;
      CYC(b_+213, b_+214); A = mem_rd(gb, HL);
      CYC(b_+214, b_+216); L = 0xdf;
      CYC(b_+216, b_+217); mem_wr(gb, HL, A);
      CYC(b_+217, b_+219); L = 0xc0;
      CYC(b_+219, b_+220); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+220, b_+221); alu_or(gb, A);
      if (!(F & FZ)) {
        CYCT(b_+221, b_+223);
        TAIL(inventoryTextCode__drawSpaceWithoutSavingTextAddress);
      }
      CYC(b_+221, b_+223);
      CYC(b_+223, b_+224); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
      CYC(b_+224, b_+225); ret_effect(gb);
      return;
    }
    CYCT(b_+200, b_+202);
    CALL_C(b_+225, handleTextControlCodeWithSpecialCase_hook, SYM(handleTextControlCodeWithSpecialCase), b_+228);
    if (F & FZ) {
      CYCT(b_+228, b_+230);
      TAIL(inventoryTextCode__saveTextAddressAndDmaTextGfxBuffer);
    }
    CYC(b_+228, b_+230);
    CYC(b_+230, b_+232);
  }
}

void inventoryTextCode__state05_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+232, decInvTextScrollTimer_hook, SYM(decInvTextScrollTimer), b_+235);
  if (!(F & FZ)) { CYCT(b_+235, b_+236); ret_effect(gb); return; }
  CYC(b_+235, b_+236);
  CYC(b_+236, b_+237); L = alu_inc8(gb, L);
  CYC(b_+237, b_+238); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+238, b_+240);
    TAIL(inventoryTextCode__insertSpace);
  }
  CYC(b_+238, b_+240);
  CYC(b_+240, b_+241); L = alu_dec8(gb, L);
  CYC(b_+241, b_+243); mem_wr(gb, HL, 0x28);
  CYC(b_+243, b_+244); L = E;
  CYC(b_+244, b_+246); mem_wr(gb, HL, 0x01);
  CYC(b_+246, b_+247); ret_effect(gb);
}

void inventoryTextCode__state06_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+247, decInvTextScrollTimer_hook, SYM(decInvTextScrollTimer), b_+250);
  if (!(F & FZ)) { CYCT(b_+250, b_+251); ret_effect(gb); return; }
  CYC(b_+250, b_+251);
  CYC(b_+251, b_+253); mem_wr(gb, HL, 0x28);
  CYC(b_+253, b_+254); L = E;
  CYC(b_+254, b_+255); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+255, b_+256); ret_effect(gb);
}

void inventoryTextCode__state07_hook(GB *gb) {
  BASE(inventoryTextCode);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+256, decInvTextScrollTimer_hook, SYM(decInvTextScrollTimer), b_+259);
  if (!(F & FZ)) { CYCT(b_+259, b_+260); ret_effect(gb); return; }
  CYC(b_+259, b_+260);
  CYC(b_+260, b_+262); mem_wr(gb, HL, 0x08);
  CYC(b_+262, b_+263); L = E;
  CYC(b_+263, b_+265); mem_wr(gb, HL, 0x02);
  CYC(b_+265, b_+267); L = 0xc2;
  CYC(b_+267, b_+269); mem_wr(gb, HL, 0xff);
  CYC(b_+269, b_+271); L = 0xd5;
  CYC(b_+271, b_+272); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+272, b_+273); H = mem_rd(gb, HL);
  CYC(b_+273, b_+274); L = A;
  CYC(b_+274, b_+277);
  inventoryTextCode__drawSpace_hook(gb);
}

void initTextbox_hook(GB *gb) {
  BASE(initTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wTextboxFlags);
  CYC(b_+3, b_+5); alu_bit(gb, 3, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto clear_textbox_wram; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = mem_rd(gb, hCameraY);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+13); A = W8(w1Link_yh);
  CYC(b_+13, b_+14); alu_sub(gb, B);
  CYC(b_+14, b_+16); alu_cp(gb, 0x48);
  CYC(b_+16, b_+18); A = 0x02;
  if (F & FC) { CYCT(b_+18, b_+20); goto save_position; }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); alu_xor(gb, A);
save_position:
  CYC(b_+21, b_+24); W8(wTextboxPosition) = A;
clear_textbox_wram:
  CYC(b_+24, b_+26); A = 0x07;
  CYC(b_+26, b_+28); hram_wr(gb, IO_SVBK - 0xff00, A);
  CYC(b_+28, b_+31); SET_HL(w1Link);
  CYC(b_+31, b_+34); SET_BC(0x0460);
  CALL_C(b_+34, clearMemoryBc_hook, SYM(clearMemoryBc), b_+37);
  CYC(b_+37, b_+40); initTextboxStuff_hook(gb);
}

void initTextboxStuff_hook(GB *gb) {
  BASE(initTextboxStuff);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wActiveLanguage);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); alu_add(gb, B);
  CYC(b_+6, b_+9); SET_HL(SYM(textTableTable));
  CYC(b_+9, b_+10); push_effect(gb, b_+10); add_a_to_hl(gb);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+14); W8(w7TextTableAddr) = A;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+18); WP(w7TextTableAddr)[1] = A;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+22); W8(w7TextTableBank) = A;
  CALL_C(b_+22, checkInitialTextCommands_hook, SYM(checkInitialTextCommands), b_+25);
  CYC(b_+25, b_+28); SET_HL(w7TextSound);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x66);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CALL_C(b_+31, getCharacterDisplayLength_hook, SYM(getCharacterDisplayLength), b_+34);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+35, b_+36); L = alu_inc8(gb, L);
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x80);
  CYC(b_+38, b_+39); L = alu_inc8(gb, L);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x03);
  CYC(b_+41, b_+42); L = alu_inc8(gb, L);
  CYC(b_+42, b_+45); SET_DE(w3VramTiles);
  CYC(b_+45, b_+47); mem_wr(gb, HL, w3VramTiles_BANK);
  CYC(b_+47, b_+50); A = W8(wOpenedMenuType);
  CYC(b_+50, b_+51); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+51, b_+53); goto set_textbox_position; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+56); SET_DE(w4TileMap);
  CYC(b_+56, b_+58); mem_wr(gb, HL, w4TileMap_BANK);
set_textbox_position:
  CYC(b_+58, b_+61); A = W8(wTextboxPosition);
  CYC(b_+61, b_+64); SET_HL(b_+217);
  CYC(b_+64, b_+65); add_double_index_to_hl(gb, b_+65);
  CYC(b_+65, b_+66); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+66, b_+67); H = mem_rd(gb, HL);
  CYC(b_+67, b_+68); L = A;
  CYC(b_+68, b_+69); push_effect(gb, HL);
  CYC(b_+69, b_+70); alu_add(gb, E);
  CYC(b_+70, b_+71); L = A;
  CYC(b_+71, b_+72); A = D;
  CYC(b_+72, b_+73); alu_adc(gb, H);
  CYC(b_+73, b_+74); H = A;
  CYC(b_+74, b_+77); SET_DE(0x0020);
  CYC(b_+77, b_+79); A = H8(hCameraY);
  CYC(b_+79, b_+81); alu_add(gb, 0x04);
  CYC(b_+81, b_+83); alu_and(gb, 0xf8);
  if (F & FZ) { CYCT(b_+83, b_+85); goto adjust_x; }
  CYC(b_+83, b_+85);
  CYC(b_+85, b_+87); A = alu_swap(gb, A);
  CYC(b_+87, b_+88); alu_rlca(gb);
  do {
    CYC(b_+88, b_+89); alu_add_hl(gb, DE);
    CYC(b_+89, b_+90); A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(b_+90, b_+92); continue; }
    CYC(b_+90, b_+92);
    break;
  } while (true);
adjust_x:
  CYC(b_+92, b_+94); A = H8(hCameraX);
  CYC(b_+94, b_+96); alu_add(gb, 0x04);
  CYC(b_+96, b_+98); alu_and(gb, 0xf8);
  CYC(b_+98, b_+100); A = alu_swap(gb, A);
  CYC(b_+100, b_+101); alu_rlca(gb);
  CYC(b_+101, b_+102); alu_add(gb, L);
  CYC(b_+102, b_+105); W8(w7TextboxPos) = A;
  CYC(b_+105, b_+106); A = H;
  CYC(b_+106, b_+109); WP(w7TextboxPos)[1] = A;
  CYC(b_+109, b_+110); SET_HL(pop_effect(gb));
  CYC(b_+110, b_+112); A = H8(hCameraY);
  CYC(b_+112, b_+113); B = A;
  CYC(b_+113, b_+116); A = W8(wScreenOffsetY);
  CYC(b_+116, b_+117); alu_add(gb, B);
  CYC(b_+117, b_+119); alu_add(gb, 0x04);
  CYC(b_+119, b_+121); alu_and(gb, 0xf8);
  if (F & FZ) { CYCT(b_+121, b_+123); goto calculate_vram_position; }
  CYC(b_+121, b_+123);
  CYC(b_+123, b_+125); A = alu_swap(gb, A);
  CYC(b_+125, b_+126); alu_rlca(gb);
  do {
    CYC(b_+126, b_+127); alu_add_hl(gb, DE);
    CYC(b_+127, b_+128); A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(b_+128, b_+130); continue; }
    CYC(b_+128, b_+130);
    break;
  } while (true);
calculate_vram_position:
  CYC(b_+130, b_+131); A = H;
  CYC(b_+131, b_+133); alu_and(gb, 0x03);
  CYC(b_+133, b_+134); H = A;
  CYC(b_+134, b_+137); A = W8(wTextMapAddress);
  CYC(b_+137, b_+138); B = A;
  CYC(b_+138, b_+140); C = 0x00;
  CYC(b_+140, b_+141); alu_add_hl(gb, BC);
  CYC(b_+141, b_+142); A = L;
  CYC(b_+142, b_+145); W8(w7TextboxVramPos) = A;
  CYC(b_+145, b_+146); A = H;
  CYC(b_+146, b_+149); WP(w7TextboxVramPos)[1] = A;
  CYC(b_+149, b_+152); A = W8(wScreenOffsetX);
  CYC(b_+152, b_+153); B = A;
  CYC(b_+153, b_+155); A = H8(hCameraX);
  CYC(b_+155, b_+157); alu_add(gb, 0x04);
  CYC(b_+157, b_+158); alu_add(gb, B);
  CYC(b_+158, b_+160); alu_and(gb, 0xf8);
  CYC(b_+160, b_+162); A = alu_swap(gb, A);
  CYC(b_+162, b_+163); alu_rlca(gb);
  CYC(b_+163, b_+166); W8(w7d0cc) = A;
  CYC(b_+166, b_+168); alu_sub(gb, 0x20);
  CYC(b_+168, b_+169); alu_cpl(gb);
  CYC(b_+169, b_+170); A = alu_dec8(gb, A);
  CYC(b_+170, b_+172); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+172, b_+174); goto set_textbox_width; }
  CYC(b_+172, b_+174);
  CYC(b_+174, b_+176); A = 0x10;
set_textbox_width:
  CYC(b_+176, b_+179); W8(w7d0cd) = A;
  CYC(b_+179, b_+180); B = A;
  CYC(b_+180, b_+182); A = 0x10;
  CYC(b_+182, b_+183); alu_sub(gb, B);
  CYC(b_+183, b_+186); W8(w7d0ce) = A;
  CYC(b_+186, b_+189); A = W8(wTextboxFlags);
  CYC(b_+189, b_+191); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+191, b_+192); ret_effect(gb); return; }
  CYC(b_+191, b_+192);
  CYC(b_+192, b_+194); alu_and(gb, 0x14);
  CYC(b_+194, b_+196); A = 0x0e;
  if (F & FZ) { CYCT(b_+196, b_+198); goto load_palette; }
  CYC(b_+196, b_+198);
  CYC(b_+198, b_+201); A = W8(wTextboxFlags);
  CYC(b_+201, b_+203); alu_and(gb, 0x10);
  CYC(b_+203, b_+205); A = 0xbd;
  if (!(F & FZ)) { CYCT(b_+205, b_+207); goto load_palette; }
  CYC(b_+205, b_+207);
  CYC(b_+207, b_+209); A = 0x81;
  CYC(b_+209, b_+212); W8(w7TextAttribute) = A;
  CYC(b_+212, b_+214); A = 0x0d;
load_palette:
  CYC(b_+214, b_+217); loadPaletteHeader_hook(gb);
}

// handleTextControlCode's handlers. On entry the stack holds bc, hl and the return address the
// caller pushed; each handler pops what the assembly pops and ends in ret_effect or in the hook
// it tail-jumps to.
static void text_cc_pop_bc_and_ret(GB *gb) {
  BASE(handleTextControlCode__popBcAndRet);
  CYC(b_+0, b_+1); SET_BC(pop_effect(gb));
  CYC(b_+1, b_+2); ret_effect(gb);
}

static void text_cc_0(GB *gb) {
  BASE(handleTextControlCode__controlCode0);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_BC(pop_effect(gb));
  CALL_C(b_+2, popFromTextStack_hook, SYM(popFromTextStack), b_+5);
  A = H;
  alu_or(gb, A);
  CYC(b_+5, b_+7);
  if (!(F & FZ)) { CYCT(b_+7, b_+8); ret_effect(gb); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); mem_wr(gb, w7TextStatus, A);
  CYC(b_+11, b_+12); ret_effect(gb);
}

static void text_cc_1(GB *gb) {
  BASE(handleTextControlCode__controlCode1);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_BC(pop_effect(gb));
  A = 0x01;
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+7); mem_wr(gb, w7TextStatus, A);
  CYC(b_+7, b_+8); ret_effect(gb);
}

static void text_cc_6(GB *gb) {
  BASE(handleTextControlCode__controlCode6);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  B = A;
  alu_cp(gb, 0x80);
  CYC(b_+4, b_+7);
  if (F & FC) {
    CYCT(b_+7, b_+9);
    A = 0x01;
    CYC(b_+30, b_+32);
    CYC(b_+32, b_+35); mem_wr(gb, w7TextGfxSource, A);
    A = B;
    CYC(b_+35, b_+36);
  } else {
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+12); A = mem_rd(gb, wTextGfxColorIndex);
    alu_swap_a(gb);
    alu_or(gb, 0x03);
    CYC(b_+12, b_+16);
    CYC(b_+16, b_+19); mem_wr(gb, wTextGfxColorIndex, A);
    A = 0x02;
    CYC(b_+19, b_+21);
    CYC(b_+21, b_+24); mem_wr(gb, w7TextGfxSource, A);
    A = B;
    alu_sub(gb, 0x80);
    alu_add(gb, A);
    CYC(b_+24, b_+28);
    CYCT(b_+28, b_+30);
  }
  CYC(b_+36, b_+37); SET_BC(pop_effect(gb));
  CYC(b_+37, b_+38); push_effect(gb, AF);
  A = 0x06;
  CYC(b_+38, b_+40);
  CALL_C(b_+40, setLineTextBuffers_hook, SYM(setLineTextBuffers), b_+43);
  CYC(b_+43, b_+44); SET_AF(pop_effect(gb));
  CYC(b_+44, b_+47);
  retrieveTextCharacter_hook(gb);
}

// @controlCode2..5 select dictionary a and share this tail (from @controlCode5+2).
static void text_cc_dictionary_tail(GB *gb) {
  BASE(handleTextControlCode__controlCode5);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+2, b_+4); H8(hFF8B) = A;
  CYC(b_+4, b_+5); SET_HL(pop_effect(gb));
  CALL_C(b_+5, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+8);
  CYC(b_+8, b_+11); mem_wr(gb, wTextIndexL, A);
  CALL_C(b_+11, pushToTextStack_hook, SYM(pushToTextStack), b_+14);
  CYC(b_+14, b_+16); A = H8(hFF8B);
  CYC(b_+16, b_+19); mem_wr(gb, wTextIndexH, A);
  CALL_C(b_+19, getTextAddress_hook, SYM(getTextAddress), b_+22);
  CYCT(b_+22, b_+24);
  text_cc_pop_bc_and_ret(gb);
}

static void text_cc_2(GB *gb) {
  BASE(handleTextControlCode__controlCode2);
  alu_xor(gb, A);
  CYC(b_+0, b_+1);
  CYCT(b_+1, b_+3);
  text_cc_dictionary_tail(gb);
}

static void text_cc_3(GB *gb) {
  BASE(handleTextControlCode__controlCode3);
  A = 0x01;
  CYC(b_+0, b_+2);
  CYCT(b_+2, b_+4);
  text_cc_dictionary_tail(gb);
}

static void text_cc_4(GB *gb) {
  BASE(handleTextControlCode__controlCode4);
  A = 0x02;
  CYC(b_+0, b_+2);
  CYCT(b_+2, b_+4);
  text_cc_dictionary_tail(gb);
}

static void text_cc_5(GB *gb) {
  BASE(handleTextControlCode__controlCode5);
  A = 0x03;
  CYC(b_+0, b_+2);
  text_cc_dictionary_tail(gb);
}

static void text_cc_f(GB *gb) {
  BASE(handleTextControlCode__controlCodeF);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  alu_cp(gb, 0xfc);
  CYC(b_+4, b_+6);
  if (F & FC) CYCT(b_+6, b_+8);
  else {
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+9); push_effect(gb, HL);
    alu_cpl(gb);
    SET_HL(wTextSubstitutions);
    CYC(b_+9, b_+13);
    CYC(b_+13, b_+14); push_effect(gb, b_+14); add_a_to_hl(gb);
    CYC(b_+14, b_+15); A = mem_rd(gb, HL);
    CYC(b_+15, b_+16); SET_HL(pop_effect(gb));
  }
  CYC(b_+16, b_+19); mem_wr(gb, wTextIndexL, A);
  CYC(b_+19, b_+22); A = mem_rd(gb, wTextIndexH_backup);
  CYC(b_+22, b_+25); mem_wr(gb, wTextIndexH, A);
  CALL_C(b_+25, pushToTextStack_hook, SYM(pushToTextStack), b_+28);
  CALL_C(b_+28, checkInitialTextCommands_hook, SYM(checkInitialTextCommands), b_+31);
  CYCT(b_+31, b_+33);
  text_cc_pop_bc_and_ret(gb);
}

static void text_cc_7(GB *gb) {
  BASE(handleTextControlCode__controlCode7);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  CYC(b_+4, b_+7); mem_wr(gb, wTextIndexL, A);
  CALL_C(b_+7, checkInitialTextCommands_hook, SYM(checkInitialTextCommands), b_+10);
  CYCT(b_+10, b_+12);
  text_cc_pop_bc_and_ret(gb);
}

static void text_cc_8(GB *gb) {
  BASE(handleTextControlCode__controlCode8);
  CYC(b_+0, b_+3); A = mem_rd(gb, w7d0c1);
  alu_or(gb, 0x10);
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); mem_wr(gb, w7d0c1, A);
  alu_xor(gb, A);
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); mem_wr(gb, w7TextStatus, A);
  CYC(b_+12, b_+13); SET_HL(pop_effect(gb));
  CYCT(b_+13, b_+15);
  text_cc_pop_bc_and_ret(gb);
}

static void text_cc_9(GB *gb) {
  BASE(handleTextControlCode__controlCode9);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+4); A = mem_rd(gb, wTextboxFlags);
  alu_rrca(gb);
  CYC(b_+4, b_+5);
  if (F & FC) {
    CYCT(b_+5, b_+7);
    CALL_C(b_+36, incHlAndUpdateBank_hook, SYM(incHlAndUpdateBank), b_+39);
    text_cc_pop_bc_and_ret(gb);
    return;
  }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+10);
  alu_bit(gb, 7, A);
  CYC(b_+10, b_+12);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+14);
    CYC(b_+31, b_+34); mem_wr(gb, w7TextAttribute, A);
    CYCT(b_+34, b_+36);
    text_cc_pop_bc_and_ret(gb);
    return;
  }
  CYC(b_+12, b_+14);
  SET_BC(SYM(handleTextControlCode__textColorData));
  CYC(b_+14, b_+17);
  CALL_C(b_+17, addDoubleIndexToBc_hook, SYM(addDoubleIndexToBc), b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, BC);
  CYC(b_+21, b_+24); mem_wr(gb, w7TextAttribute, A);
  SET_BC(BC + 1);
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); A = mem_rd(gb, BC);
  CYC(b_+26, b_+29); mem_wr(gb, wTextGfxColorIndex, A);
  CYC(b_+29, b_+30); SET_BC(pop_effect(gb));
  CYC(b_+30, b_+31); ret_effect(gb);
}

static void text_cc_a(GB *gb) {
  BASE(handleTextControlCode__controlCodeA);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_BC(pop_effect(gb));
  CALL_C(b_+2, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+5);
  CYC(b_+5, b_+6); push_effect(gb, HL);
  SET_HL(SYM(nameAddressTable));
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+10); add_double_index_to_hl(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+12, b_+13);
  for (;;) {
    CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_or(gb, A);
    CYC(b_+14, b_+15);
    if (F & FZ) { CYCT(b_+15, b_+17); break; }
    CYC(b_+15, b_+17);
    CALL_C(b_+17, setLineTextBuffers_hook, SYM(setLineTextBuffers), b_+20);
    CALL_C(b_+20, retrieveTextCharacter_hook, SYM(retrieveTextCharacter), b_+23);
    CYCT(b_+23, b_+25);
  }
  CYC(b_+25, b_+26); SET_HL(pop_effect(gb));
  CYC(b_+26, b_+27); ret_effect(gb);
}

static void text_cc_e(GB *gb) {
  BASE(handleTextControlCode__controlCodeE);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  CYC(b_+4, b_+7); mem_wr(gb, w7SoundEffect, A);
  CYCT(b_+7, b_+9);
  text_cc_pop_bc_and_ret(gb);
}

static void text_cc_b(GB *gb) {
  BASE(handleTextControlCode__controlCodeB);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  CYC(b_+4, b_+7); mem_wr(gb, w7TextSound, A);
  CYCT(b_+7, b_+9);
  text_cc_pop_bc_and_ret(gb);
}

static void text_cc_d(GB *gb) {
  BASE(handleTextControlCode__controlCodeD);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  CYC(b_+4, b_+7); mem_wr(gb, w7TextboxTimer, A);
  CYCT(b_+7, b_+9);
  text_cc_pop_bc_and_ret(gb);
}

static void text_cc_c(GB *gb) {
  BASE(handleTextControlCode__controlCodeC);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  CYC(b_+4, b_+5); push_effect(gb, HL);
  B = A;
  alu_and(gb, 0x03);
  C = A;
  A = B;
  alu_swap_a(gb);
  alu_rlca(gb);
  alu_and(gb, 0x1f);
  CYC(b_+5, b_+15);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (textbox_jump_table(gb));
    if (jt_ == SYM(textControlCodeC_0) && hook_enabled_at(gb, SYM(textControlCodeC_0))) { textControlCodeC_0_hook(gb); return; }
    else if (jt_ == SYM(textControlCodeC_1) && hook_enabled_at(gb, SYM(textControlCodeC_1))) { textControlCodeC_1_hook(gb); return; }
    else if (jt_ == SYM(textControlCodeC_2) && hook_enabled_at(gb, SYM(textControlCodeC_2))) { textControlCodeC_2_hook(gb); return; }
    else if (jt_ == SYM(textControlCodeC_3) && hook_enabled_at(gb, SYM(textControlCodeC_3))) { textControlCodeC_3_hook(gb); return; }
    else if (jt_ == SYM(textControlCodeC_ret) && hook_enabled_at(gb, SYM(textControlCodeC_ret))) { textControlCodeC_ret_hook(gb); return; }
    else if (jt_ == SYM(textControlCodeC_5) && hook_enabled_at(gb, SYM(textControlCodeC_5))) { textControlCodeC_5_hook(gb); return; }
    else if (jt_ == SYM(textControlCodeC_6) && hook_enabled_at(gb, SYM(textControlCodeC_6))) { textControlCodeC_6_hook(gb); return; }
    else if (jt_ == SYM(textControlCodeC_7) && hook_enabled_at(gb, SYM(textControlCodeC_7))) { textControlCodeC_7_hook(gb); return; }
    else { HANDOFF(jt_); }
  } while (0);
}

void handleTextControlCode_hook(GB *gb) {
  BASE(handleTextControlCode);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); push_effect(gb, HL);
  CYC(b_+2, b_+3); push_effect(gb, b_+3);
  do { uint16_t jt_ = (textbox_jump_table(gb));
    if (jt_ == SYM(handleTextControlCode__controlCode0)) { text_cc_0(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCode1)) { text_cc_1(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCode2)) { text_cc_2(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCode3)) { text_cc_3(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCode4)) { text_cc_4(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCode5)) { text_cc_5(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCode6)) { text_cc_6(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCode7)) { text_cc_7(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCode8)) { text_cc_8(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCode9)) { text_cc_9(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCodeA)) { text_cc_a(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCodeB)) { text_cc_b(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCodeC)) { text_cc_c(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCodeD)) { text_cc_d(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCodeE)) { text_cc_e(gb); return; }
    else if (jt_ == SYM(handleTextControlCode__controlCodeF)) { text_cc_f(gb); return; }
    else { HANDOFF(jt_); }
  } while (0);
}

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
  BASE(clearTextGfxBuffer);
  CYC(b_+0, b_+3); SET_HL(w1ParentItem2);
  CYC(b_+3, b_+6); SET_BC(0x0200);
  CYC(b_+6, b_+8); A = 0xff;
  CYC(b_+8, b_+11);
  fillMemoryBc_hook(gb);
}

void clearLineTextBuffer_hook(GB *gb) {
  BASE(clearLineTextBuffer);
  CYC(b_+0, b_+3); SET_HL(w1ParentItem4);
  CYC(b_+3, b_+4); D = H;
  CYC(b_+4, b_+5); E = L;
  CYC(b_+5, b_+7); B = 0x10;
  CYC(b_+7, b_+10);
  clearMemory_hook(gb);
}

void setLineTextBuffers_hook(GB *gb) {
  BASE(setLineTextBuffers);
  CYC(b_+0, b_+1); mem_wr(gb, DE, A);
  CYC(b_+1, b_+2); push_effect(gb, DE);
  CYC(b_+2, b_+3); push_effect(gb, HL);

  CYC(b_+3, b_+6); SET_HL(w7TextAttribute);
  CYC(b_+6, b_+7); A = E;
  CYC(b_+7, b_+9); alu_add(gb, 0x10);
  CYC(b_+9, b_+10); E = A;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);

  CYC(b_+12, b_+13); A = E;
  CYC(b_+13, b_+15); alu_add(gb, 0x10);
  CYC(b_+15, b_+16); E = A;
  CYC(b_+16, b_+17); L = alu_dec8(gb, L);
  CYC(b_+17, b_+18); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);

  CYC(b_+19, b_+20); A = E;
  CYC(b_+20, b_+22); alu_add(gb, 0x10);
  CYC(b_+22, b_+23); E = A;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);

  CYC(b_+25, b_+26); A = E;
  CYC(b_+26, b_+28); alu_add(gb, 0x10);
  CYC(b_+28, b_+29); E = A;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x00);

  CYC(b_+33, b_+34); SET_HL(pop_effect(gb));
  CYC(b_+34, b_+35); SET_DE(pop_effect(gb));
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); E = alu_inc8(gb, E);
  CYC(b_+37, b_+38); ret_effect(gb);
}

void drawLineOfText_hook(GB *gb) {
  BASE(drawLineOfText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc2;
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0xff);
  CYC(b_+5, b_+7); L = 0xd5;
  CYC(b_+7, b_+8); push_effect(gb, HL);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); H = mem_rd(gb, HL);
  CYC(b_+10, b_+11); L = A;
  CYC(b_+11, b_+12); push_effect(gb, HL);
  CALL_C(b_+12, clearTextGfxBuffer_hook, SYM(clearTextGfxBuffer), b_+15);
  CALL_C(b_+15, clearLineTextBuffer_hook, SYM(clearLineTextBuffer), b_+18);
  CYC(b_+18, b_+19); SET_HL(pop_effect(gb));
  CYC(b_+19, b_+22); SET_BC(w1ParentItem2);
  for (;;) {
    CALL_C(b_+22, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+25);
    CYC(b_+25, b_+27); alu_cp(gb, 0x10);
    if (!(F & FC)) {
      CYCT(b_+27, b_+29);
      CALL_C(b_+41, setLineTextBuffers_hook, SYM(setLineTextBuffers), b_+44);
      CALL_C(b_+44, retrieveTextCharacter_hook, SYM(retrieveTextCharacter), b_+47);
      CYC(b_+47, b_+49);
      continue;
    }
    CYC(b_+27, b_+29);
    CALL_C(b_+29, handleTextControlCode_hook, SYM(handleTextControlCode), b_+32);
    CYC(b_+32, b_+35); A = W8(w7TextStatus);
    CYC(b_+35, b_+37); alu_cp(gb, 0x02);
    if (!(F & FC)) { CYCT(b_+37, b_+39); continue; }
    CYC(b_+37, b_+39);
    CYC(b_+39, b_+41);
    break;
  }
  CYC(b_+49, b_+50); SET_DE(pop_effect(gb));
  CYC(b_+50, b_+51); A = L;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+53); E = alu_inc8(gb, E);
  CYC(b_+53, b_+54); A = H;
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+57); E = 0xd0;
  CYC(b_+57, b_+58); alu_xor(gb, A);
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  CYC(b_+59, b_+60); ret_effect(gb);
}

void dmaTextGfxBuffer_hook(GB *gb) {
  BASE(dmaTextGfxBuffer);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_add(gb, 0x94);
  CYC(b_+2, b_+3); D = A;
  CYC(b_+3, b_+5); E = 0x00;
  CYC(b_+5, b_+8); SET_HL(w7TextGfxBuffer);
  CYC(b_+8, b_+11); SET_BC(0x1f07);
  CYC(b_+11, b_+12); push_effect(gb, HL);
  CALL_C(b_+12, queueDmaTransfer_hook, SYM(queueDmaTransfer), b_+15);
  CYC(b_+15, b_+16); SET_HL(pop_effect(gb));
  CYC(b_+16, b_+17); ret_effect(gb);
}

void saveTilesUnderTextbox_copyTileMap_hook(GB *gb) {
  BASE(saveTilesUnderTextbox);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+25, b_+27); A = 0x03;
  for (;;) {
    CYC(b_+27, b_+28); push_effect(gb, AF);
    CYC(b_+28, b_+29); push_effect(gb, HL);
    CYC(b_+29, b_+32); A = mem_rd(gb, w7TextboxPosBank);
    CYC(b_+32, b_+34); mem_wr(gb, 0xff70, A);
    CYC(b_+34, b_+37); SET_HL(wTmpVramBuffer);
    CYC(b_+37, b_+39); A = 0x02;
    for (;;) {
      CYC(b_+39, b_+40); push_effect(gb, AF);
      CYC(b_+40, b_+41); A = E;
      CYC(b_+41, b_+43); alu_and(gb, 0xe0);
      CYC(b_+43, b_+44); C = A;
      CYC(b_+44, b_+46); B = 0x20;
      for (;;) {
        CYC(b_+46, b_+47); A = mem_rd(gb, DE);
        CYC(b_+47, b_+48); mem_wr(gb, HL, A); SET_HL(HL + 1);
        CYC(b_+48, b_+49); A = E;
        CYC(b_+49, b_+50); A = alu_inc8(gb, A);
        CYC(b_+50, b_+52); alu_and(gb, 0x1f);
        CYC(b_+52, b_+53); alu_or(gb, C);
        CYC(b_+53, b_+54); E = A;
        CYC(b_+54, b_+55); B = alu_dec8(gb, B);
        if (!(F & FZ)) { CYCT(b_+55, b_+57); continue; }
        CYC(b_+55, b_+57);
        break;
      }
      CYC(b_+57, b_+59); A = 0x20;
      CALL_C(b_+59, addAToDe_hook, 0x0068, b_+62);
      CYC(b_+62, b_+63); SET_AF(pop_effect(gb));
      CYC(b_+63, b_+64); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(b_+64, b_+66); continue; }
      CYC(b_+64, b_+66);
      break;
    }
    CYC(b_+66, b_+68); A = 0x07;
    CYC(b_+68, b_+70); mem_wr(gb, 0xff70, A);
    CYC(b_+70, b_+71); SET_HL(pop_effect(gb));
    CYC(b_+71, b_+72); push_effect(gb, DE);
    CYC(b_+72, b_+75); SET_DE(wTmpVramBuffer);
    CYC(b_+75, b_+77); A = 0x02;
    for (;;) {
      CYC(b_+77, b_+78); push_effect(gb, AF);
      CYC(b_+78, b_+79); A = L;
      CYC(b_+79, b_+81); alu_and(gb, 0xe0);
      CYC(b_+81, b_+82); C = A;
      CYC(b_+82, b_+84); B = 0x20;
      for (;;) {
        CYC(b_+84, b_+85); A = mem_rd(gb, DE);
        CYC(b_+85, b_+86); mem_wr(gb, HL, A);
        CYC(b_+86, b_+87); E = alu_inc8(gb, E);
        CYC(b_+87, b_+88); A = L;
        CYC(b_+88, b_+89); A = alu_inc8(gb, A);
        CYC(b_+89, b_+91); alu_and(gb, 0x1f);
        CYC(b_+91, b_+92); alu_or(gb, C);
        CYC(b_+92, b_+93); L = A;
        CYC(b_+93, b_+94); B = alu_dec8(gb, B);
        if (!(F & FZ)) { CYCT(b_+94, b_+96); continue; }
        CYC(b_+94, b_+96);
        break;
      }
      CYC(b_+96, b_+98); A = 0x20;
      CYC(b_+98, b_+99); push_effect(gb, b_+99);
      add_a_to_hl(gb);
      CYC(b_+99, b_+100); SET_AF(pop_effect(gb));
      CYC(b_+100, b_+101); A = alu_dec8(gb, A);
      if (!(F & FZ)) { CYCT(b_+101, b_+103); continue; }
      CYC(b_+101, b_+103);
      break;
    }
    CYC(b_+103, b_+104); SET_DE(pop_effect(gb));
    CYC(b_+104, b_+105); SET_AF(pop_effect(gb));
    CYC(b_+105, b_+106); A = alu_dec8(gb, A);
    if (!(F & FZ)) { CYCT(b_+106, b_+108); continue; }
    CYC(b_+106, b_+108);
    break;
  }
  CYC(b_+108, b_+109); ret_effect(gb);
}

void saveTilesUnderTextbox_hook(GB *gb) {
  BASE(saveTilesUnderTextbox);
  CYC(b_+0, b_+3); SET_HL(w7TextboxPos);
  CYC(b_+3, b_+4); E = mem_rd(gb, HL);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); D = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); L = mem_rd(gb, HL);
  CYC(b_+8, b_+10); H = 0xd0;
  CYC(b_+10, b_+13); push_effect(gb, b_+13);
  saveTilesUnderTextbox_copyTileMap_hook(gb);
  CYC(b_+13, b_+16); SET_HL(w7TextboxPos);
  CYC(b_+16, b_+17); E = mem_rd(gb, HL);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+21); alu_add(gb, 0x04);
  CYC(b_+21, b_+22); D = A;
  CYC(b_+22, b_+23); L = mem_rd(gb, HL);
  CYC(b_+23, b_+25); H = 0xd1;
  saveTilesUnderTextbox_copyTileMap_hook(gb);
}

void initTextboxMapping_hook(GB *gb) {
  BASE(initTextboxMapping);
  CYC(b_+0, b_+3); A = mem_rd(gb, w7d0cc);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x1f);
  CYC(b_+6, b_+7); L = A;
  CYC(b_+7, b_+9); E = 0x05;
  for (;;) {
    CYC(b_+9, b_+11); B = 0x12;
    CYC(b_+11, b_+12); A = L;
    CYC(b_+12, b_+13); D = A;
    CYC(b_+13, b_+15); alu_and(gb, 0xe0);
    CYC(b_+15, b_+16); C = A;
    for (;;) {
      CYC(b_+16, b_+18); H = 0xd0;
      CYC(b_+18, b_+20); mem_wr(gb, HL, 0x02);
      CYC(b_+20, b_+22); H = 0xd1;
      CYC(b_+22, b_+24); mem_wr(gb, HL, 0x80);
      CYC(b_+24, b_+25); A = L;
      CYC(b_+25, b_+26); A = alu_inc8(gb, A);
      CYC(b_+26, b_+28); alu_and(gb, 0x1f);
      CYC(b_+28, b_+29); alu_or(gb, C);
      CYC(b_+29, b_+30); L = A;
      CYC(b_+30, b_+31); B = alu_dec8(gb, B);
      if (!(F & FZ)) {
        CYCT(b_+31, b_+33);
        continue;
      }
      CYC(b_+31, b_+33);
      break;
    }
    CYC(b_+33, b_+34); A = D;
    CYC(b_+34, b_+36); alu_add(gb, 0x20);
    CYC(b_+36, b_+37); L = A;
    CYC(b_+37, b_+38); E = alu_dec8(gb, E);
    if (!(F & FZ)) {
      CYCT(b_+38, b_+40);
      continue;
    }
    CYC(b_+38, b_+40);
    break;
  }
  CYC(b_+40, b_+41); ret_effect(gb);
}

void getTextAddress_hook(GB *gb) {
  BASE(getTextAddress);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+4); A = W8(w7TextTableAddr);
  CYC(b_+4, b_+5); L = A;
  CYC(b_+5, b_+8); A = WP(w7TextTableAddr)[1];
  CYC(b_+8, b_+9); H = A;
  CYC(b_+9, b_+10); push_effect(gb, HL);
  CYC(b_+10, b_+13); A = W8(wTextIndexH);
  CYC(b_+13, b_+14); add_double_index_to_hl(gb, b_+14);
  CALL_C(b_+14, readByteFromW7TextTableBank_hook, SYM(readByteFromW7TextTableBank), b_+17);
  CYC(b_+17, b_+18); C = A;
  CALL_C(b_+18, readByteFromW7TextTableBank_hook, SYM(readByteFromW7TextTableBank), b_+21);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+23); SET_HL(pop_effect(gb));
  CYC(b_+23, b_+24); alu_add_hl(gb, BC);
  CYC(b_+24, b_+27); A = W8(wTextIndexL);
  CYC(b_+27, b_+28); add_double_index_to_hl(gb, b_+28);
  CALL_C(b_+28, readByteFromW7TextTableBank_hook, SYM(readByteFromW7TextTableBank), b_+31);
  CYC(b_+31, b_+32); C = A;
  CALL_C(b_+32, readByteFromW7TextTableBank_hook, SYM(readByteFromW7TextTableBank), b_+35);
  CYC(b_+35, b_+36); B = A;
  CYC(b_+36, b_+39); A = W8(wActiveLanguage);
  CYC(b_+39, b_+40); alu_add(gb, A);
  CYC(b_+40, b_+43); SET_HL(SYM(textOffset1Table));
  CYC(b_+43, b_+44); add_double_index_to_hl(gb, b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+46); E = A;
  CYC(b_+46, b_+47); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+47, b_+48); H = mem_rd(gb, HL);
  CYC(b_+48, b_+49); L = A;
  CYC(b_+49, b_+52); A = W8(wTextIndexH);
  CYC(b_+52, b_+54); alu_cp(gb, 0x2c);
  if (F & FC) { CYCT(b_+54, b_+56); goto calculate_address; }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+59); A = W8(wActiveLanguage);
  CYC(b_+59, b_+60); alu_add(gb, A);
  CYC(b_+60, b_+63); SET_HL(SYM(textOffset2Table));
  CYC(b_+63, b_+64); add_double_index_to_hl(gb, b_+64);
  CYC(b_+64, b_+65); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+65, b_+66); E = A;
  CYC(b_+66, b_+67); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+67, b_+68); H = mem_rd(gb, HL);
  CYC(b_+68, b_+69); L = A;
calculate_address:
  CYC(b_+69, b_+70); A = E;
  CYC(b_+70, b_+72); alu_add(gb, 0x04);
  CYC(b_+72, b_+73); alu_add_hl(gb, BC);
  if (F & FC) { CYCT(b_+73, b_+75); goto save_address; }
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+76); A = H;
  CYC(b_+76, b_+78); alu_and(gb, 0xc0);
  CYC(b_+78, b_+79); alu_rlca(gb);
  CYC(b_+79, b_+80); alu_rlca(gb);
  CYC(b_+80, b_+81); alu_add(gb, E);
save_address:
  CYC(b_+81, b_+84); W8(w7ActiveBank) = A;
  CYC(b_+84, b_+86); H &= (uint8_t)~0x80;
  CYC(b_+86, b_+88); H |= 0x40;
  CYC(b_+88, b_+89); SET_DE(pop_effect(gb));
  CYC(b_+89, b_+90); ret_effect(gb);
}

void checkInitialTextCommands_hook(GB *gb) {
  BASE(checkInitialTextCommands);
  uint16_t sp0_ = gb->sp; (void)sp0_;
again:
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CALL_C(b_+1, getTextAddress_hook, SYM(getTextAddress), b_+4);
  CALL_C(b_+4, readByteFromW7ActiveBank_hook, SYM(readByteFromW7ActiveBank), b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+9, b_+11); goto cmd8; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto end; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); D = H;
  CYC(b_+16, b_+17); E = L;
  CALL_C(b_+17, incHlAndUpdateBank_hook, SYM(incHlAndUpdateBank), b_+20);
  CALL_C(b_+20, readByteFromW7ActiveBank_hook, SYM(readByteFromW7ActiveBank), b_+23);
  CYC(b_+23, b_+24); B = A;
  CYC(b_+24, b_+26); alu_and(gb, 0xfc);
  CYC(b_+26, b_+28); alu_cp(gb, 0x20);
  if (F & FZ) { CYCT(b_+28, b_+30); goto position; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); H = D;
  CYC(b_+31, b_+32); L = E;
  CYC(b_+32, b_+34); goto end;
position:
  CYC(b_+34, b_+37); A = W8(wTextboxFlags);
  CYC(b_+37, b_+39); alu_bit(gb, 3, A);
  if (!(F & FZ)) { CYCT(b_+39, b_+41); goto advance; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); A = B;
  CYC(b_+42, b_+44); alu_and(gb, 0x07);
  CYC(b_+44, b_+47); W8(wTextboxPosition) = A;
advance:
  CALL_C(b_+47, incHlAndUpdateBank_hook, SYM(incHlAndUpdateBank), b_+50);
end:
  CYC(b_+50, b_+51); A = L;
  CYC(b_+51, b_+54); W8(w7TextAddress) = A;
  CYC(b_+54, b_+55); A = H;
  CYC(b_+55, b_+58); WP(w7TextAddress)[1] = A;
  CYC(b_+58, b_+59); SET_DE(pop_effect(gb));
  CYC(b_+59, b_+60); ret_effect(gb);
  return;
cmd8:
  CALL_C(b_+60, incHlAndUpdateBank_hook, SYM(incHlAndUpdateBank), b_+63);
  CALL_C(b_+63, readByteFromW7ActiveBank_hook, SYM(readByteFromW7ActiveBank), b_+66);
  CALL_C(b_+66, getExtraTextIndex_hook, SYM(getExtraTextIndex), b_+69);
  CYC(b_+69, b_+71); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+71, b_+74); goto no_extra; }
  CYC(b_+71, b_+74);
  CYC(b_+74, b_+77); W8(wTextIndexL) = A;
  CYC(b_+77, b_+79); goto again;
no_extra:
  CYC(b_+79, b_+81); A = 0;
  CYC(b_+81, b_+84); W8(wTextIsActive) = A;
  CYC(b_+84, b_+87); SET_HL(w7TextDisplayState);
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x0f);
  CYC(b_+89, b_+90); L = alu_inc8(gb, L);
  CYC(b_+90, b_+92); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
  CYC(b_+92, b_+93); L = alu_inc8(gb, L);
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0);
  CYC(b_+95, b_+96); ret_effect(gb);
}

// doInventoryTextFirstPass@controlCode and its handlers: advance e by the number of characters
// a control code produces, reading the text stream the way the second pass does. Each handler
// ends in the routine that pops the return address the caller pushed.
static void inventory_text_cc_dictionary(GB *gb) {
  BASE(doInventoryTextFirstPass__dictionary);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  CYC(b_+4, b_+7); mem_wr(gb, wTextIndexL, A);
  CALL_C(b_+7, pushToTextStack_hook, SYM(pushToTextStack), b_+10);
  A = B;
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+14); mem_wr(gb, wTextIndexH, A);
  CYC(b_+14, b_+17);
  getTextAddress_hook(gb);
}

static void inventory_text_cc_nil(GB *gb) {
  BASE(doInventoryTextFirstPass__controlCodeNil);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+4);
  incHlAndUpdateBank_hook(gb);
}

static void inventory_text_cc_6(GB *gb) {
  BASE(doInventoryTextFirstPass__controlCode6);
  E = alu_inc8(gb, E);
  CYC(b_+0, b_+1);
  inventory_text_cc_nil(gb);
}

static void inventory_text_cc_7(GB *gb) {
  BASE(doInventoryTextFirstPass__controlCode7);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  CYC(b_+4, b_+7); mem_wr(gb, wTextIndexL, A);
  CYC(b_+7, b_+10);
  checkInitialTextCommands_hook(gb);
}

static void inventory_text_cc_8(GB *gb) {
  BASE(doInventoryTextFirstPass__controlCode8);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2);
  ret_effect(gb);
}

static void inventory_text_cc_a(GB *gb) {
  BASE(doInventoryTextFirstPass__controlCodeA);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  CYC(b_+4, b_+5); push_effect(gb, HL);
  SET_HL(SYM(nameAddressTable));
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); add_double_index_to_hl(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); H = mem_rd(gb, HL);
  L = A;
  CYC(b_+11, b_+12);
  for (;;) {
    CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
    alu_or(gb, A);
    CYC(b_+13, b_+14);
    if (F & FZ) { CYCT(b_+14, b_+16); break; }
    CYC(b_+14, b_+16);
    E = alu_inc8(gb, E);
    CYC(b_+16, b_+17);
    CYCT(b_+17, b_+19);
  }
  inventory_text_cc_8(gb);
}

static void inventory_text_cc_f(GB *gb) {
  BASE(doInventoryTextFirstPass__controlCodeF);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CALL_C(b_+1, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+4);
  alu_cp(gb, 0xfc);
  CYC(b_+4, b_+6);
  if (F & FC) CYCT(b_+6, b_+8);
  else {
    CYC(b_+6, b_+8);
    CYC(b_+8, b_+9); push_effect(gb, HL);
    alu_cpl(gb);
    SET_HL(wTextSubstitutions);
    CYC(b_+9, b_+13);
    CYC(b_+13, b_+14); push_effect(gb, b_+14); add_a_to_hl(gb);
    CYC(b_+14, b_+15); A = mem_rd(gb, HL);
    CYC(b_+15, b_+16); SET_HL(pop_effect(gb));
  }
  CYC(b_+16, b_+19); mem_wr(gb, wTextIndexL, A);
  CALL_C(b_+19, pushToTextStack_hook, SYM(pushToTextStack), b_+22);
  CYC(b_+22, b_+25);
  checkInitialTextCommands_hook(gb);
}

static void inventory_text_control_code(GB *gb) {
  BASE(doInventoryTextFirstPass__controlCode);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  alu_sub(gb, 0x02);
  B = A;
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+4); push_effect(gb, HL);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (textbox_jump_table(gb));
    if (jt_ == SYM(doInventoryTextFirstPass__dictionary)) { inventory_text_cc_dictionary(gb); return; }
    else if (jt_ == SYM(doInventoryTextFirstPass__controlCode6)) { inventory_text_cc_6(gb); return; }
    else if (jt_ == SYM(doInventoryTextFirstPass__controlCode7)) { inventory_text_cc_7(gb); return; }
    else if (jt_ == SYM(doInventoryTextFirstPass__controlCode8)) { inventory_text_cc_8(gb); return; }
    else if (jt_ == SYM(doInventoryTextFirstPass__controlCodeNil)) { inventory_text_cc_nil(gb); return; }
    else if (jt_ == SYM(doInventoryTextFirstPass__controlCodeA)) { inventory_text_cc_a(gb); return; }
    else if (jt_ == SYM(doInventoryTextFirstPass__controlCodeF)) { inventory_text_cc_f(gb); return; }
    else { HANDOFF(jt_); }
  } while (0);
}

void doInventoryTextFirstPass_hook(GB *gb) {
  BASE(doInventoryTextFirstPass);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, clearTextGfxBuffer_hook, SYM(clearTextGfxBuffer), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = 0xd4;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+7, b_+9); mem_wr(gb, hFF8A, A);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); H = mem_rd(gb, HL);
  CYC(b_+11, b_+12); L = A;
  CYC(b_+12, b_+13); push_effect(gb, HL);
  CYC(b_+13, b_+15); E = 0;
  for (;;) {
    CALL_C(b_+15, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+18);
    CYC(b_+18, b_+20); alu_cp(gb, 0);
    if (F & FZ) { CYCT(b_+20, b_+22); goto null_terminator; }
    CYC(b_+20, b_+22);
    CYC(b_+22, b_+24); alu_cp(gb, 1);
    if (F & FZ) { CYCT(b_+24, b_+26); goto line_end; }
    CYC(b_+24, b_+26);
    CYC(b_+26, b_+28); alu_cp(gb, 0x10);
    if (!(F & FC)) { CYCT(b_+28, b_+30); goto not_control; }
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+33); push_effect(gb, b_+33);
    inventory_text_control_code(gb);
    CYC(b_+33, b_+35);
    continue;
null_terminator:
    CALL_C(b_+42, popFromTextStack_hook, SYM(popFromTextStack), b_+45);
    CYC(b_+45, b_+46); A = H;
    CYC(b_+46, b_+47); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+47, b_+49); continue; }
    CYC(b_+47, b_+49);
    goto line_end;
not_control:
    CYC(b_+35, b_+36); E = alu_inc8(gb, E);
    CYC(b_+36, b_+38); alu_bit(gb, 4, E);
    if (F & FZ) { CYCT(b_+38, b_+40); continue; }
    CYC(b_+38, b_+40);
    CYC(b_+40, b_+42);
line_end:
    CALL_C(b_+49, popFromTextStack_hook, SYM(popFromTextStack), b_+52);
    CYC(b_+52, b_+53); SET_BC(pop_effect(gb));
    CYC(b_+53, b_+56); SET_HL(w7TextAddress - 1);
    CYC(b_+56, b_+58); A = mem_rd(gb, hFF8A);
    CYC(b_+58, b_+59); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+59, b_+60); mem_wr(gb, HL, C);
    CYC(b_+60, b_+61); L = alu_inc8(gb, L);
    CYC(b_+61, b_+62); mem_wr(gb, HL, B);
    CYC(b_+62, b_+63); A = E;
    CYC(b_+63, b_+64); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+64, b_+65); ret_effect(gb); return; }
    CYC(b_+64, b_+65);
    CYC(b_+65, b_+66); push_effect(gb, BC);
    CYC(b_+66, b_+68); alu_sub(gb, 0x11);
    CYC(b_+68, b_+69); A = (uint8_t)~A;
    CYC(b_+69, b_+71); L = 0xed;
    CYC(b_+71, b_+72); mem_wr(gb, HL, A);
    CYC(b_+72, b_+74); alu_and(gb, 0x0e);
    CYC(b_+74, b_+76); A = alu_swap(gb, A);
    CYC(b_+76, b_+78); alu_add(gb, 0);
    CYC(b_+78, b_+79); C = A;
    CALL_C(b_+79, clearLineTextBuffer_hook, SYM(clearLineTextBuffer), b_+82);
    CYC(b_+82, b_+84); B = 0xd2;
    CYC(b_+84, b_+85); SET_HL(pop_effect(gb));
    for (;;) {
      CALL_C(b_+85, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), b_+88);
      CYC(b_+88, b_+90); alu_cp(gb, 0x10);
      if (F & FC) { CYCT(b_+90, b_+92); goto control; }
      CYC(b_+90, b_+92);
      CALL_C(b_+92, setLineTextBuffers_hook, SYM(setLineTextBuffers), b_+95);
      CALL_C(b_+95, retrieveTextCharacter_hook, SYM(retrieveTextCharacter), b_+98);
      CYC(b_+98, b_+100); alu_bit(gb, 4, E);
      if (F & FZ) { CYCT(b_+100, b_+102); continue; }
      CYC(b_+100, b_+102);
      CYC(b_+102, b_+103); ret_effect(gb); return;
control:
      CALL_C(b_+103, handleTextControlCode_hook, SYM(handleTextControlCode), b_+106);
      CYC(b_+106, b_+109); A = W8(w7TextStatus);
      CYC(b_+109, b_+111); alu_cp(gb, 2);
      if (!(F & FC)) { CYCT(b_+111, b_+113); continue; }
      CYC(b_+111, b_+113);
      CYC(b_+113, b_+114); ret_effect(gb); return;
    }
  }
}

void dmaTextboxMap_func_hook(GB *gb) {
  BASE(dmaTextboxMap);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+25, b_+27); C = 0x07;
  CYC(b_+27, b_+28); push_effect(gb, HL);
  CALL_C(b_+28, queueDmaTransfer_hook, SYM(queueDmaTransfer), b_+31);
  CYC(b_+31, b_+32); SET_HL(pop_effect(gb));
  CYC(b_+32, b_+33); E = alu_inc8(gb, E);
  CYC(b_+33, b_+34); H = alu_inc8(gb, H);
  CYC(b_+34, b_+37); queueDmaTransfer_hook(gb);
}

void dmaTextboxMap_hook(GB *gb) {
  BASE(dmaTextboxMap);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTextMapAddress);
  CYC(b_+3, b_+5); alu_add(gb, 0x03);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+9); SET_HL(w7TextboxVramPos);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); E = A;
  CYC(b_+11, b_+13); alu_cp(gb, 0x61);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); D = A;
  if (F & FC) { CYCT(b_+15, b_+17); goto run_func; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); alu_cp(gb, C);
  if (F & FZ) { CYCT(b_+18, b_+20); goto alternate; }
  CYC(b_+18, b_+20);
run_func:
  CYC(b_+20, b_+22); B = 0x09;
  CYC(b_+22, b_+25); SET_HL(w1Link);
  dmaTextboxMap_func_hook(gb);
  return;
alternate:
  CYC(b_+37, b_+38); alu_xor(gb, A);
  CYC(b_+38, b_+39); alu_sub(gb, E);
  CYC(b_+39, b_+40); C = A;
  CYC(b_+40, b_+42); A = alu_swap(gb, A);
  CYC(b_+42, b_+43); A = alu_dec8(gb, A);
  CYC(b_+43, b_+44); B = A;
  CYC(b_+44, b_+47); SET_HL(w1Link);
  CYC(b_+47, b_+48); push_effect(gb, BC);
  CYC(b_+48, b_+51); push_effect(gb, b_+51);
  dmaTextboxMap_func_hook(gb);
  CYC(b_+51, b_+52); SET_BC(pop_effect(gb));
  CYC(b_+52, b_+55); A = mem_rd(gb, wTextMapAddress);
  CYC(b_+55, b_+56); D = A;
  CYC(b_+56, b_+58); E = 0x00;
  CYC(b_+58, b_+59); L = C;
  CYC(b_+59, b_+61); H = 0xd0;
  CYC(b_+61, b_+63); A = 0xa0;
  CYC(b_+63, b_+64); alu_sub(gb, C);
  CYC(b_+64, b_+66); A = alu_swap(gb, A);
  CYC(b_+66, b_+67); A = alu_dec8(gb, A);
  CYC(b_+67, b_+68); B = A;
  CYC(b_+68, b_+70); dmaTextboxMap_func_hook(gb);
}

void updateCharacterDisplayTimer_hook(GB *gb) {
  BASE(updateCharacterDisplayTimer);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xee;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto timer_check; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
timer_check:
  CYC(b_+8, b_+10); L = 0xd7;
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+12, b_+14); goto check_input; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto countdown; }
  CYC(b_+15, b_+17);
check_input:
  CYC(b_+17, b_+20); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+20, b_+22); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto skip_to_line_end; }
  CYC(b_+22, b_+24);
countdown:
  CYC(b_+24, b_+26); L = 0xc6;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+28); ret_effect(gb);
  return;
skip_to_line_end:
  CYC(b_+28, b_+30); L = 0xc1;
  CYC(b_+30, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  CYC(b_+32, b_+33); alu_xor(gb, A);
  CYC(b_+33, b_+34); ret_effect(gb);
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
  BASE(displayNextTextCharacter);
  uint16_t sp0_ = gb->sp; (void)sp0_;
start:
  CYC(b_+0, b_+2); E = 0xd3;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); C = A;
  CYC(b_+4, b_+6); alu_cp(gb, 0x40);
  CYC(b_+6, b_+8); B = 0x00;
  if (F & FZ) { CYCT(b_+8, b_+10); goto display_character; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); B = 0x40;
display_character:
  CYC(b_+12, b_+14); E = 0xd0;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); L = A;
  CYC(b_+16, b_+18); E = 0xcc;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_add(gb, 0x02);
  CYC(b_+21, b_+22); alu_add(gb, L);
  CYC(b_+22, b_+24); alu_and(gb, 0x1f);
  CYC(b_+24, b_+25); alu_add(gb, B);
  CYC(b_+25, b_+26); E = A;
  CYC(b_+26, b_+28); H = 0xd4;
  CYC(b_+28, b_+29); A = mem_rd(gb, HL);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+30, b_+32); goto end_line; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, hFF8B, A);
  CYC(b_+34, b_+36); D = 0xd0;
  CYC(b_+36, b_+37); A = L;
  CYC(b_+37, b_+38); alu_add(gb, A);
  CYC(b_+38, b_+39); alu_add(gb, C);
  CYC(b_+39, b_+40); B = A;
  CYC(b_+40, b_+41); B = alu_inc8(gb, B);
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+43); A = E;
  CYC(b_+43, b_+45); alu_add(gb, 0x20);
  CYC(b_+45, b_+46); E = A;
  CYC(b_+46, b_+47); A = B;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); D = alu_inc8(gb, D);
  CYC(b_+49, b_+50); A = L;
  CYC(b_+50, b_+52); alu_add(gb, 0x10);
  CYC(b_+52, b_+53); L = A;
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+56); A = E;
  CYC(b_+56, b_+58); alu_sub(gb, 0x20);
  CYC(b_+58, b_+59); E = A;
  CYC(b_+59, b_+60); A = mem_rd(gb, HL);
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+63); D = 0xd0;
  CYC(b_+63, b_+65); E = 0xd0;
  CYC(b_+65, b_+66); A = mem_rd(gb, DE);
  CYC(b_+66, b_+67); A = alu_inc8(gb, A);
  CYC(b_+67, b_+68); mem_wr(gb, DE, A);
  CYC(b_+68, b_+70); alu_cp(gb, 0x10);
  if (F & FZ) { CYCT(b_+70, b_+72); goto end_line; }
  CYC(b_+70, b_+72);
  CYC(b_+72, b_+75); push_effect(gb, b_+75);
  displayNextTextCharacter_checkCanAdvanceWithAB_hook(gb);
  if (!(F & FZ)) { CYCT(b_+75, b_+77); goto read_subsequent_buffers; }
  CYC(b_+75, b_+77);
  CYC(b_+77, b_+79); E = 0xc1;
  CYC(b_+79, b_+80); A = mem_rd(gb, DE);
  CYC(b_+80, b_+82); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+82, b_+84); goto start; }
  CYC(b_+82, b_+84);
read_subsequent_buffers:
  CYC(b_+84, b_+87); push_effect(gb, b_+87);
  displayNextTextCharacter_readSubsequentLineBuffers_hook(gb);
  CYC(b_+87, b_+88); alu_or(gb, D);
  CYC(b_+88, b_+89); ret_effect(gb);
  return;
end_line:
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); L = 0xc1;
  CYC(b_+92, b_+94); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+94, b_+95); ret_effect(gb); return; }
  CYC(b_+94, b_+95);
  CYC(b_+95, b_+97); L = 0xee;
  CYC(b_+97, b_+98); A = mem_rd(gb, HL);
  CYC(b_+98, b_+99); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+99, b_+101); goto clear_a; }
  CYC(b_+99, b_+101);
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x04);
  CYC(b_+103, b_+105); L = 0xc4;
  CYC(b_+105, b_+106); A = mem_rd(gb, HL);
  CALL_C(b_+106, playSound_b00_hook, SYM(playSound_b00), b_+109);
clear_a:
  CYC(b_+109, b_+110); alu_xor(gb, A);
  CYC(b_+110, b_+111); ret_effect(gb);
}

void displayNextTextCharacter_readSubsequentLineBuffers_hook(GB *gb) {
  BASE(displayNextTextCharacter);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+111, b_+112); A = L;
  CYC(b_+112, b_+114); alu_add(gb, 0x10);
  CYC(b_+114, b_+115); L = A;
  CYC(b_+115, b_+116); A = mem_rd(gb, HL);
  CYC(b_+116, b_+118); E = 0xc6;
  CYC(b_+118, b_+119); mem_wr(gb, DE, A);
  CYC(b_+119, b_+120); A = L;
  CYC(b_+120, b_+122); alu_add(gb, 0x10);
  CYC(b_+122, b_+123); L = A;
  CYC(b_+123, b_+124); A = mem_rd(gb, HL);
  CYC(b_+124, b_+125); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+125, b_+127); goto read_sound_effect; }
  CYC(b_+125, b_+127);
  CYC(b_+127, b_+128); B = A;
  CYC(b_+128, b_+130); A = mem_rd(gb, hFF8B);
  CYC(b_+130, b_+132); alu_cp(gb, 0x20);
  if (F & FZ) { CYCT(b_+132, b_+134); goto read_sound_effect; }
  CYC(b_+132, b_+134);
  CYC(b_+134, b_+136); E = 0xee;
  CYC(b_+136, b_+137); A = mem_rd(gb, DE);
  CYC(b_+137, b_+138); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+138, b_+140); goto read_sound_effect; }
  CYC(b_+138, b_+140);
  CYC(b_+140, b_+142); A = 0x04;
  CYC(b_+142, b_+143); mem_wr(gb, DE, A);
  CYC(b_+143, b_+144); A = B;
  CYC(b_+144, b_+147); push_effect(gb, b_+147);
  displayNextTextCharacter_playSound_hook(gb);
read_sound_effect:
  CYC(b_+147, b_+148); A = L;
  CYC(b_+148, b_+150); alu_add(gb, 0x10);
  CYC(b_+150, b_+151); L = A;
  CYC(b_+151, b_+152); A = mem_rd(gb, HL);
  CYC(b_+152, b_+153); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+153, b_+154); ret_effect(gb); return; }
  CYC(b_+153, b_+154);
  displayNextTextCharacter_playSound_hook(gb);
}

void displayNextTextCharacter_playSound_hook(GB *gb) {
  BASE(displayNextTextCharacter);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+154, b_+155); push_effect(gb, HL);
  CALL_C(b_+155, playSound_b00_hook, SYM(playSound_b00), b_+158);
  CYC(b_+158, b_+159); SET_HL(pop_effect(gb));
  CYC(b_+159, b_+160); ret_effect(gb);
}

void displayNextTextCharacter_checkCanAdvanceWithAB_hook(GB *gb) {
  BASE(displayNextTextCharacter);
  CYC(b_+160, b_+161); push_effect(gb, HL);
  CYC(b_+161, b_+163); E = 0xd0;
  CYC(b_+163, b_+164); A = mem_rd(gb, DE);
  CYC(b_+164, b_+166); alu_add(gb, 0x50);
  CYC(b_+166, b_+167); L = A;
  CYC(b_+167, b_+169); H = 0xd4;
  CYC(b_+169, b_+171); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+171, b_+172); SET_HL(pop_effect(gb));
  CYC(b_+172, b_+173); ret_effect(gb);
}

void getNextCharacterToDisplay_hook(GB *gb) {
  BASE(getNextCharacterToDisplay);
  CYC(b_+0, b_+2); E = 0xd0;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x10);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_add(gb, 0x00);
  CYC(b_+8, b_+9); L = A;
  CYC(b_+9, b_+11); H = 0xd4;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); alu_or(gb, A);
  CYC(b_+13, b_+14); ret_effect(gb);
}

void func_5296_hook(GB *gb) {
  BASE(func_5296);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc1;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+6); alu_bit(gb, 2, A);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto choose_option; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); label_3f_155_hook(gb); return; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); alu_bit(gb, 4, A);
  if (F & FZ) { CYCT(b_+14, b_+15); ret_effect(gb); return; }
  CYC(b_+14, b_+15);
  CALL_C(b_+15, readNextTextByte_hook, SYM(readNextTextByte), b_+18);
  CYC(b_+18, b_+20); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+20, b_+22); label_3f_158_hook(gb); return; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+25); mem_wr(gb, wTextIndex, A);
  CALL_C(b_+25, checkInitialTextCommands_hook, SYM(checkInitialTextCommands), b_+28);
  CYC(b_+28, b_+30); E = 0xc1;
  CYC(b_+30, b_+31); alu_xor(gb, A);
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+33); E = alu_inc8(gb, E);
  CYC(b_+33, b_+34); A = alu_inc8(gb, A);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); ret_effect(gb);
  return;
choose_option:
  CYC(b_+36, b_+38); E = 0xc2;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+40, b_+42); label_3f_159_hook(gb); return; }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A);
  CYC(b_+43, b_+45); A = 0x01;
  CYC(b_+45, b_+48); mem_wr(gb, wTextDisplayMode, A);
  CYC(b_+48, b_+49); alu_or(gb, H);
  CYC(b_+49, b_+50); ret_effect(gb);
}

void label_3f_155_hook(GB *gb) {
  BASE(label_3f_155);
  CYC(b_+0, b_+2); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+2, b_+4); goto check_input; }
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+7); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x01);
  CYC(b_+7, b_+9); label_3f_157_hook(gb);
  return;
check_input:
  CYC(b_+9, b_+12); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+12, b_+14); alu_and(gb, 0x03);
  if (F & FZ) { CYCT(b_+14, b_+16); label_3f_157_hook(gb); return; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x00);
  CYC(b_+18, b_+20); L = 0xc1;
  CYC(b_+20, b_+22); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x02);
  CYC(b_+22, b_+23); SET_HL(pop_effect(gb));
  CYC(b_+23, b_+25); A = 0x89;
  CYC(b_+25, b_+28); playSound_b00_hook(gb);
}

void label_3f_157_hook(GB *gb) {
  BASE(label_3f_157);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, updateTextboxArrow_hook, SYM(updateTextboxArrow), b_+3);
  CYC(b_+3, b_+4); alu_or(gb, H);
  CYC(b_+4, b_+5); ret_effect(gb);
}

void label_3f_158_hook(GB *gb) {
  BASE(label_3f_158);
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, 0x00c2, A);
  CYC(b_+4, b_+5); ret_effect(gb);
}

void label_3f_159_hook(GB *gb) {
  BASE(label_3f_159);
  CYC(b_+0, b_+3); SET_HL(w7TextboxOptionPositions);
  label_3f_160_hook(gb);
}

void label_3f_160_hook(GB *gb) {
  BASE(label_3f_160);
  for (;;) {
    CYC(b_+0, b_+1); A = mem_rd(gb, HL);
    CYC(b_+1, b_+2); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+2, b_+3); ret_effect(gb); return; }
    CYC(b_+2, b_+3);
    CYC(b_+3, b_+5); alu_xor(gb, 0x20);
    CYC(b_+5, b_+6); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+6, b_+7); A = L;
    CYC(b_+7, b_+9); alu_and(gb, 0x07);
    if (!(F & FZ)) { CYCT(b_+9, b_+11); continue; }
    CYC(b_+9, b_+11); RET(b_+11);
    return;
  }
}

void readNextTextByte_hook(GB *gb) {
  BASE(readNextTextByte);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0xd5;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+3, b_+4); H = mem_rd(gb, HL);
  CYC(b_+4, b_+5); L = A;
  CALL_C(b_+5, readByteFromW7ActiveBankAndIncHl_hook, SYM(readByteFromW7ActiveBankAndIncHl), SYM(getExtraTextIndex));
  getExtraTextIndex_hook(gb);
}

void getExtraTextIndex_hook(GB *gb) {
  BASE(getExtraTextIndex);
  CYC(b_+0, b_+3); SET_HL(SYM(extraTextIndices));
  CYC(b_+3, b_+4); add_double_index_to_hl(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); H = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = A;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); C = A;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+12); A = mem_rd(gb, BC);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  add_a_to_hl(gb);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+15); ret_effect(gb);
}

void updateTextboxArrow_hook(GB *gb) {
  BASE(updateTextboxArrow);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0xcc;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_add(gb, 0x12);
  CYC(b_+11, b_+13); alu_and(gb, 0x1f);
  CYC(b_+13, b_+15); alu_add(gb, 0x80);
  CYC(b_+15, b_+16); L = A;
  CYC(b_+16, b_+18); H = 0xd0;
  CYC(b_+18, b_+20); E = 0xc8;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_cp(gb, 0x03);
  CYC(b_+23, b_+25); A = 0x02;
  if (F & FZ) { CYCT(b_+25, b_+27); goto update_arrow; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); A = 0x03;
update_arrow:
  CYC(b_+29, b_+30); mem_wr(gb, DE, A);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+34); A = mem_rd(gb, wTextMapAddress);
  CYC(b_+34, b_+36); alu_add(gb, 0x04);
  CYC(b_+36, b_+37); C = A;
  CYC(b_+37, b_+39); L = 0x80;
  CYC(b_+39, b_+41); H = 0xd0;
  CYC(b_+41, b_+43); E = 0xd8;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+45); alu_add(gb, L);
  CYC(b_+45, b_+46); B = A;
  CYC(b_+46, b_+47); E = alu_inc8(gb, E);
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+50); alu_adc(gb, 0x00);
  CYC(b_+50, b_+51); alu_cp(gb, C);
  if (F & FC) { CYCT(b_+51, b_+53); goto queue_dma; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+56); A = mem_rd(gb, wTextMapAddress);
queue_dma:
  CYC(b_+56, b_+57); D = A;
  CYC(b_+57, b_+58); E = B;
  CYC(b_+58, b_+61); SET_BC(0x0107);
  CYC(b_+61, b_+64); queueDmaTransfer_hook(gb);
}

void clearTopRowOfTextMap_func_hook(GB *gb) {
  BASE(clearTopRowOfTextMap);
  CYC(b_+25, b_+26); C = A;
  CYC(b_+26, b_+27); A = L;
  CYC(b_+27, b_+29); alu_and(gb, 0xe0);
  CYC(b_+29, b_+30); E = A;
  for (;;) {
    CYC(b_+30, b_+31); mem_wr(gb, HL, C);
    CYC(b_+31, b_+32); A = L;
    CYC(b_+32, b_+33); A = alu_inc8(gb, A);
    CYC(b_+33, b_+35); alu_and(gb, 0x1f);
    CYC(b_+35, b_+36); alu_or(gb, E);
    CYC(b_+36, b_+37); L = A;
    CYC(b_+37, b_+38); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+38, b_+40); continue; }
    CYC(b_+38, b_+40);
    break;
  }
  CYC(b_+40, b_+41); ret_effect(gb);
}

void clearTopRowOfTextMap_hook(GB *gb) {
  BASE(clearTopRowOfTextMap);
  CYC(b_+0, b_+2); H = 0xd0;
  CYC(b_+2, b_+5); A = mem_rd(gb, w7d0cc);
  CYC(b_+5, b_+7); alu_add(gb, 0x02);
  CYC(b_+7, b_+9); alu_and(gb, 0x1f);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+13); B = 0x10;
  CYC(b_+13, b_+15); A = 0x02;
  CYC(b_+15, b_+16); push_effect(gb, BC);
  CYC(b_+16, b_+19); push_effect(gb, b_+19);
  clearTopRowOfTextMap_func_hook(gb);
  CYC(b_+19, b_+20); SET_BC(pop_effect(gb));
  CYC(b_+20, b_+22); H = 0xd1;
  CYC(b_+22, b_+23); L = C;
  CYC(b_+23, b_+25); A = 0x80;
  clearTopRowOfTextMap_func_hook(gb);
}

void shiftTextboxMapUp_func_hook(GB *gb) {
  BASE(shiftTextboxMapUp);
  CYC(b_+7, b_+8); D = H;
  CYC(b_+8, b_+11); A = mem_rd(gb, w7d0cc);
  CYC(b_+11, b_+13); alu_add(gb, 0x02);
  CYC(b_+13, b_+15); alu_and(gb, 0x1f);
  CYC(b_+15, b_+16); E = A;
  CYC(b_+16, b_+18); alu_add(gb, 0x20);
  CYC(b_+18, b_+19); L = A;
  CYC(b_+19, b_+20); B = A;
  CYC(b_+20, b_+22); C = 0x04;
  for (;;) {
    CYC(b_+22, b_+23); push_effect(gb, BC);
    CYC(b_+23, b_+24); A = E;
    CYC(b_+24, b_+26); alu_and(gb, 0xe0);
    CYC(b_+26, b_+27); C = A;
    CYC(b_+27, b_+29); B = 0x10;
    for (;;) {
      CYC(b_+29, b_+30); A = mem_rd(gb, HL);
      CYC(b_+30, b_+31); mem_wr(gb, DE, A);
      CYC(b_+31, b_+32); A = E;
      CYC(b_+32, b_+33); A = alu_inc8(gb, A);
      CYC(b_+33, b_+35); alu_and(gb, 0x1f);
      CYC(b_+35, b_+36); alu_or(gb, C);
      CYC(b_+36, b_+37); E = A;
      CYC(b_+37, b_+39); alu_add(gb, 0x20);
      CYC(b_+39, b_+40); L = A;
      CYC(b_+40, b_+41); B = alu_dec8(gb, B);
      if (!(F & FZ)) { CYCT(b_+41, b_+43); continue; }
      CYC(b_+41, b_+43);
      break;
    }
    CYC(b_+43, b_+44); SET_BC(pop_effect(gb));
    CYC(b_+44, b_+45); E = B;
    CYC(b_+45, b_+46); A = B;
    CYC(b_+46, b_+48); alu_add(gb, 0x20);
    CYC(b_+48, b_+49); L = A;
    CYC(b_+49, b_+50); B = A;
    CYC(b_+50, b_+51); C = alu_dec8(gb, C);
    if (!(F & FZ)) { CYCT(b_+51, b_+53); continue; }
    CYC(b_+51, b_+53);
    break;
  }
  CYC(b_+53, b_+54); ret_effect(gb);
}

void shiftTextboxMapUp_hook(GB *gb) {
  BASE(shiftTextboxMapUp);
  CYC(b_+0, b_+2); H = 0xd0;
  CYC(b_+2, b_+5); push_effect(gb, b_+5);
  shiftTextboxMapUp_func_hook(gb);
  CYC(b_+5, b_+7); H = 0xd1;
  shiftTextboxMapUp_func_hook(gb);
}

void subFirstRowOfTextMapBy20_func_hook(GB *gb) {
  BASE(subFirstRowOfTextMapBy20);
  CYC(b_+9, b_+12); A = mem_rd(gb, w7d0cc);
  CYC(b_+12, b_+14); alu_add(gb, 0x02);
  CYC(b_+14, b_+16); alu_and(gb, 0x1f);
  CYC(b_+16, b_+17); alu_add(gb, B);
  CYC(b_+17, b_+18); L = A;
  CYC(b_+18, b_+20); alu_and(gb, 0xe0);
  CYC(b_+20, b_+21); C = A;
  CYC(b_+21, b_+23); B = 0x10;
  for (;;) {
    CYC(b_+23, b_+24); A = mem_rd(gb, HL);
    CYC(b_+24, b_+26); alu_and(gb, 0x60);
    CYC(b_+26, b_+28); alu_cp(gb, 0x60);
    if (!(F & FZ)) { CYCT(b_+28, b_+30); goto next_tile; }
    CYC(b_+28, b_+30);
    CYC(b_+30, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x20);
next_tile:
    CYC(b_+32, b_+33); A = L;
    CYC(b_+33, b_+34); A = alu_inc8(gb, A);
    CYC(b_+34, b_+36); alu_and(gb, 0x1f);
    CYC(b_+36, b_+37); alu_or(gb, C);
    CYC(b_+37, b_+38); L = A;
    CYC(b_+38, b_+39); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+39, b_+41); continue; }
    CYC(b_+39, b_+41);
    break;
  }
  CYC(b_+41, b_+42); ret_effect(gb);
}

void subFirstRowOfTextMapBy20_hook(GB *gb) {
  BASE(subFirstRowOfTextMapBy20);
  CYC(b_+0, b_+2); H = 0xd0;
  CYC(b_+2, b_+4); B = 0x00;
  CYC(b_+4, b_+7); push_effect(gb, b_+7);
  subFirstRowOfTextMapBy20_func_hook(gb);
  CYC(b_+7, b_+9); B = 0x20;
  subFirstRowOfTextMapBy20_func_hook(gb);
}

void func_53eb_func_hook(GB *gb);
void func_53eb_dmaHeartPieceDisplay_hook(GB *gb);

void func_53dd_hook(GB *gb) {
  BASE(func_53dd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc1;
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x02);
  CALL_C(b_+5, saveTilesUnderTextbox_hook, SYM(saveTilesUnderTextbox), b_+8);
  CALL_C(b_+8, initTextboxMapping_hook, SYM(initTextboxMapping), b_+11);
  CYC(b_+11, b_+14); dmaTextboxMap_hook(gb);
}

void func_53eb_hook(GB *gb) {
  BASE(func_53eb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc1;
  CYC(b_+3, b_+5); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto initialize_heart_piece; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); L = 0xea;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+12); ret_effect(gb); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+13, b_+14); ret_effect(gb); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); B = 0x00;
  CYC(b_+16, b_+19); push_effect(gb, b_+19);
  func_53eb_func_hook(gb);
  CYC(b_+19, b_+21); A = 0x89;
  CALL_C(b_+21, playSound_b00_hook, SYM(playSound_b00), b_+24);
  CYC(b_+24, b_+25); alu_xor(gb, A);
  CYC(b_+25, b_+26); ret_effect(gb);
  return;
initialize_heart_piece:
  CYC(b_+26, b_+28); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x20);
  CYC(b_+28, b_+30); L = 0xea;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x1e);
  CYC(b_+32, b_+34); L = 0xef;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x01);
  CYC(b_+36, b_+39); push_effect(gb, b_+39);
  func_53eb_dmaHeartPieceDisplay_hook(gb);
  CYC(b_+39, b_+41); B = 0xff;
  func_53eb_func_hook(gb);
}

void func_53eb_func_hook(GB *gb) {
  BASE(func_53eb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+41, b_+44); A = mem_rd(gb, wNumHeartPieces);
  CYC(b_+44, b_+45); alu_add(gb, B);
  CYC(b_+45, b_+46); alu_add(gb, A);
  CYC(b_+46, b_+47); push_effect(gb, AF);
  CYC(b_+47, b_+49); alu_sub(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+49, b_+51); goto select_heart_piece; }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+54); mem_wr(gb, wNumHeartPieces, A);
  CYC(b_+54, b_+55); A = alu_dec8(gb, A);
  CYC(b_+55, b_+58); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+58, b_+61); mem_wr(gb, w7d0ef, A);
select_heart_piece:
  CYC(b_+61, b_+62); SET_AF(pop_effect(gb));
  CYC(b_+62, b_+65); SET_HL(b_+122);
  CYC(b_+65, b_+66); add_double_index_to_hl(gb, b_+66);
  CYC(b_+66, b_+68); D = 0xd0;
  CYC(b_+68, b_+71); A = mem_rd(gb, w7d0cc);
  CYC(b_+71, b_+73); alu_add(gb, 0x11);
  CYC(b_+73, b_+75); alu_and(gb, 0x1f);
  CYC(b_+75, b_+76); C = A;
  CYC(b_+76, b_+77); A = alu_dec8(gb, A);
  CYC(b_+77, b_+79); alu_and(gb, 0x1f);
  CYC(b_+79, b_+80); B = A;
  CYC(b_+80, b_+82); alu_add(gb, 0x20);
  CYC(b_+82, b_+83); E = A;
  CYC(b_+83, b_+84); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+84, b_+85); mem_wr(gb, DE, A);
  CYC(b_+85, b_+86); A = B;
  CYC(b_+86, b_+88); alu_add(gb, 0x40);
  CYC(b_+88, b_+89); E = A;
  CYC(b_+89, b_+90); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CYC(b_+91, b_+92); A = C;
  CYC(b_+92, b_+94); alu_add(gb, 0x20);
  CYC(b_+94, b_+95); E = A;
  CYC(b_+95, b_+96); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+96, b_+97); mem_wr(gb, DE, A);
  CYC(b_+97, b_+98); A = C;
  CYC(b_+98, b_+100); alu_add(gb, 0x40);
  CYC(b_+100, b_+101); E = A;
  CYC(b_+101, b_+102); A = mem_rd(gb, HL);
  CYC(b_+102, b_+103); mem_wr(gb, DE, A);
  CYC(b_+103, b_+105); D = 0xd1;
  CYC(b_+105, b_+106); A = mem_rd(gb, DE);
  CYC(b_+106, b_+108); alu_or(gb, 0x20);
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+110); A = C;
  CYC(b_+110, b_+112); alu_add(gb, 0x20);
  CYC(b_+112, b_+113); E = A;
  CYC(b_+113, b_+114); A = mem_rd(gb, DE);
  CYC(b_+114, b_+116); alu_or(gb, 0x20);
  CYC(b_+116, b_+117); mem_wr(gb, DE, A);
  CALL_C(b_+117, dmaTextboxMap_hook, SYM(dmaTextboxMap), b_+120);
  CYC(b_+120, b_+121); alu_or(gb, D);
  CYC(b_+121, b_+122); ret_effect(gb);
}

void func_53eb_dmaHeartPieceDisplay_hook(GB *gb) {
  BASE(func_53eb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+142, b_+145); SET_HL((SYM(handleTextControlCode__controlCode6) + 2));
  CYC(b_+145, b_+148); SET_DE(0x95d0);
  CYC(b_+148, b_+151); SET_BC(0x001c);
  CALL_C(b_+151, queueDmaTransfer_hook, SYM(queueDmaTransfer), b_+154);
  CYC(b_+154, b_+157); SET_HL((SYM(handleTextControlCode__tradeItem) + 9));
  CYC(b_+157, b_+159); E = 0xf0;
  CALL_C(b_+159, queueDmaTransfer_hook, SYM(queueDmaTransfer), b_+162);
  CYC(b_+162, b_+165); SET_HL((SYM(handleTextControlCode__kanji) + 4));
  CYC(b_+165, b_+168); SET_DE(0x97c0);
  CALL_C(b_+168, queueDmaTransfer_hook, SYM(queueDmaTransfer), b_+171);
  CYC(b_+171, b_+174); SET_HL(SYM(handleTextControlCode__controlCode3));
  CYC(b_+174, b_+176); E = 0xe0;
  CYC(b_+176, b_+179); queueDmaTransfer_hook(gb);
}

void shiftTextGfxBufferLeft_hook(GB *gb) {
  BASE(shiftTextGfxBufferLeft);
  CYC(b_+0, b_+3); SET_HL(w1ParentItem2);
  CYC(b_+3, b_+6); SET_DE(w1ParentItem2_animCounter);
  CYC(b_+6, b_+9); SET_BC(0x01e0);
  for (;;) {
    CYC(b_+9, b_+10); A = mem_rd(gb, DE);
    CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+11, b_+12); SET_DE(DE + 1);
    CYC(b_+12, b_+13); SET_BC(BC - 1);
    CYC(b_+13, b_+14); A = C;
    CYC(b_+14, b_+15); alu_or(gb, B);
    if (!(F & FZ)) { CYCT(b_+15, b_+17); continue; }
    CYC(b_+15, b_+17);
    break;
  }
  CYC(b_+17, b_+20); SET_HL(w7TextAddress);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+21, b_+22); H = mem_rd(gb, HL);
  CYC(b_+22, b_+23); L = A;
  CYC(b_+23, b_+24); ret_effect(gb);
}

void decInvTextScrollTimer_hook(GB *gb) {
  BASE(decInvTextScrollTimer);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xde;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x08);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void handleTextControlCodeWithSpecialCase_hook(GB *gb) {
  BASE(handleTextControlCodeWithSpecialCase);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+2, b_+4); goto cmd6; }
  CYC(b_+2, b_+4);
  CALL_C(b_+4, handleTextControlCode_hook, SYM(handleTextControlCode), b_+7);
  CYC(b_+7, b_+8); alu_or(gb, D);
  CYC(b_+8, b_+9); ret_effect(gb);
  return;
cmd6:
  CYC(b_+9, b_+12); SET_BC(w4SubscreenTextIndices);
  CYC(b_+12, b_+15); SET_DE((w1ParentItem5_var3f + 161));
  CALL_C(b_+15, handleTextControlCode_hook, SYM(handleTextControlCode), b_+18);
  CYC(b_+18, b_+19); alu_xor(gb, A);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void updateSelectedTextPosition_hook(GB *gb) {
  BASE(updateSelectedTextPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getSelectedTextOptionAddress_hook, SYM(getSelectedTextOptionAddress), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 5, mem_rd(gb, HL));
  CYC(b_+5, b_+7); B = 0x60;
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto set_position; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); B = 0x20;
set_position:
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); E = alu_inc8(gb, E);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CALL_C(b_+14, getAddressInTextboxMap_hook, SYM(getAddressInTextboxMap), b_+17);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x04);
  CYC(b_+19, b_+20); ret_effect(gb);
}

void getSelectedTextOptionAddress_hook(GB *gb) {
  BASE(getSelectedTextOptionAddress);
  CYC(b_+0, b_+2); E = 0xe8;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_add(gb, 0xe0);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+8); ret_effect(gb);
}

void getAddressInTextboxMap_hook(GB *gb) {
  BASE(getAddressInTextboxMap);
  CYC(b_+0, b_+2); alu_and(gb, 0x1e);
  CYC(b_+2, b_+3); alu_rrca(gb);
  CYC(b_+3, b_+4); L = A;
  CYC(b_+4, b_+6); E = 0xcc;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+9); alu_add(gb, 0x02);
  CYC(b_+9, b_+10); alu_add(gb, L);
  CYC(b_+10, b_+12); alu_and(gb, 0x1f);
  CYC(b_+12, b_+13); alu_add(gb, B);
  CYC(b_+13, b_+14); L = A;
  CYC(b_+14, b_+16); H = 0xd0;
  CYC(b_+16, b_+17); ret_effect(gb);
}

void removeCursorFromSelectedTextPosition_hook(GB *gb) {
  BASE(removeCursorFromSelectedTextPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x60;
  CYC(b_+2, b_+4); E = 0xe9;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); C = A;
  CYC(b_+6, b_+8); alu_bit(gb, 5, A);
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); B = 0x20;
  }
  CALL_C(b_+12, getAddressInTextboxMap_hook, SYM(getAddressInTextboxMap), b_+15);
  CYC(b_+15, b_+16); mem_wr(gb, HL, C);
  CYC(b_+16, b_+17); ret_effect(gb);
}

void moveSelectedTextOptionRight_hook(GB *gb) {
  BASE(moveSelectedTextOptionRight);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xe8;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x07);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  CALL_C(b_+7, getSelectedTextOptionAddress_hook, SYM(getSelectedTextOptionAddress), b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+12, b_+13); ret_effect(gb);
    return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void textOptionCode_checkDirectionButtons_updateSelectedTextOption_hook(GB *gb);
void updateSelectedTextPositionAndDmaTextboxMap_hook(GB *gb);

void moveSelectedTextOptionLeft_hook(GB *gb) {
  BASE(moveSelectedTextOptionLeft);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  for (;;) {
    CYC(b_+0, b_+2); E = 0xe8;
    CYC(b_+2, b_+3); A = mem_rd(gb, DE);
    CYC(b_+3, b_+4); A = alu_dec8(gb, A);
    CYC(b_+4, b_+6); alu_and(gb, 0x07);
    CYC(b_+6, b_+7); mem_wr(gb, DE, A);
    CALL_C(b_+7, getSelectedTextOptionAddress_hook, SYM(getSelectedTextOptionAddress), b_+10);
    CYC(b_+10, b_+11); A = mem_rd(gb, HL);
    CYC(b_+11, b_+12); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+12, b_+13); ret_effect(gb); return; }
    CYC(b_+12, b_+13);
    CYCT(b_+13, b_+15);
  }
}

void textOptionCode_checkDirectionButtons_hook(GB *gb) {
  BASE(textOptionCode_checkDirectionButtons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+3, b_+5); alu_and(gb, 0xf0);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x84;
  CALL_C(b_+8, playSound_b00_hook, SYM(playSound_b00), b_+11);
  CALL_C(b_+11, removeCursorFromSelectedTextPosition_hook, SYM(removeCursorFromSelectedTextPosition), b_+14);
  CYC(b_+14, b_+17); push_effect(gb, b_+17);
  textOptionCode_checkDirectionButtons_updateSelectedTextOption_hook(gb);
  CYC(b_+17, b_+19); updateSelectedTextPositionAndDmaTextboxMap_hook(gb);
}

void textOptionCode_checkDirectionButtons_updateSelectedTextOption_hook(GB *gb) {
  BASE(textOptionCode_checkDirectionButtons);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+19, b_+22); A = mem_rd(gb, wKeysJustPressed);
  CALL_C(b_+22, getHighestSetBit_hook, SYM(getHighestSetBit), b_+25);
  CYC(b_+25, b_+27); alu_sub(gb, 0x04);
  if (F & FZ) { CYCT(b_+27, b_+29); moveSelectedTextOptionRight_hook(gb); return; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+30, b_+32); moveSelectedTextOptionLeft_hook(gb); return; }
  CYC(b_+30, b_+32);
  CALL_C(b_+32, getSelectedTextOptionAddress_hook, SYM(getSelectedTextOptionAddress), b_+35);
  CYC(b_+35, b_+36); B = mem_rd(gb, HL);
  CYC(b_+36, b_+38); C = 0xff;
  CYC(b_+38, b_+40); L = 0xe0;
  CYC(b_+40, b_+41); E = L;
  for (;;) {
    CYC(b_+41, b_+42); A = mem_rd(gb, HL);
    CYC(b_+42, b_+43); alu_or(gb, A);
    if (F & FZ) { CYCT(b_+43, b_+45); goto update_end; }
    CYC(b_+43, b_+45);
    CYC(b_+45, b_+46); alu_sub(gb, B);
    if (!(F & FC)) { CYCT(b_+46, b_+48); goto compare_row; }
    CYC(b_+46, b_+48);
    CYC(b_+48, b_+49); alu_cpl(gb);
    CYC(b_+49, b_+50); A = alu_inc8(gb, A);
compare_row:
    CYC(b_+50, b_+52); alu_sub(gb, 0x20);
    if (!(F & FC)) { CYCT(b_+52, b_+54); goto compare_distance; }
    CYC(b_+52, b_+54);
    CYC(b_+54, b_+55); alu_cpl(gb);
    CYC(b_+55, b_+56); A = alu_inc8(gb, A);
compare_distance:
    CYC(b_+56, b_+57); alu_cp(gb, C);
    if (!(F & FC)) { CYCT(b_+57, b_+59); goto next_option; }
    CYC(b_+57, b_+59);
    CYC(b_+59, b_+60); C = A;
    CYC(b_+60, b_+61); E = L;
next_option:
    CYC(b_+61, b_+62); L = alu_inc8(gb, L);
    CYCT(b_+62, b_+64);
  }
update_end:
  CYC(b_+64, b_+65); A = C;
  CYC(b_+65, b_+67); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+67, b_+68); ret_effect(gb); return; }
  CYC(b_+67, b_+68);
  CYC(b_+68, b_+69); A = E;
  CYC(b_+69, b_+71); alu_sub(gb, 0xe0);
  CYC(b_+71, b_+73); E = 0xe8;
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CYC(b_+74, b_+75); ret_effect(gb);
}

void updateSelectedTextPositionAndDmaTextboxMap_hook(GB *gb) {
  BASE(updateSelectedTextPositionAndDmaTextboxMap);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, updateSelectedTextPosition_hook, SYM(updateSelectedTextPosition), b_+3);
  CYC(b_+3, b_+6); dmaTextboxMap_hook(gb);
}

void textOptionCode_checkBButton_hook(GB *gb) {
  BASE(textOptionCode_checkBButton);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_and(gb, 0x02);
  if (F & FZ) { CYCT(b_+2, b_+3); ret_effect(gb); return; }
  CYC(b_+2, b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = 0xe0;
  for (;;) {
    CYC(b_+6, b_+7); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+7, b_+8); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+8, b_+10); continue; }
    CYC(b_+8, b_+10);
    break;
  }
  CYC(b_+10, b_+11); A = L;
  CYC(b_+11, b_+13); alu_sub(gb, 0xe2);
  CYC(b_+13, b_+15); L = 0xe8;
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+18); A = 0x84;
  CALL_C(b_+18, playSound_b00_hook, SYM(playSound_b00), b_+21);
  CALL_C(b_+21, removeCursorFromSelectedTextPosition_hook, SYM(removeCursorFromSelectedTextPosition), b_+24);
  CALL_C(b_+24, updateSelectedTextPositionAndDmaTextboxMap_hook, SYM(updateSelectedTextPositionAndDmaTextboxMap), b_+27);
  CYC(b_+27, b_+28); alu_or(gb, D);
  CYC(b_+28, b_+29); ret_effect(gb);
}

void pushToTextStack_hook(GB *gb) {
  BASE(pushToTextStack);
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, BC);
  CYC(b_+2, b_+3); push_effect(gb, HL);
  CYC(b_+3, b_+6); SET_HL((w7TextStack + 27));
  CYC(b_+6, b_+9); SET_DE((w7TextStack + 31));
  CYC(b_+9, b_+11); B = 0x1c;
  for (;;) {
    CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL - 1);
    CYC(b_+12, b_+13); mem_wr(gb, DE, A);
    CYC(b_+13, b_+14); E = alu_dec8(gb, E);
    CYC(b_+14, b_+15); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+15, b_+17); continue; }
    CYC(b_+15, b_+17);
    break;
  }
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+21); SET_DE(w7ActiveBank);
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+23, b_+24); SET_DE(pop_effect(gb));
  CYC(b_+24, b_+25); mem_wr(gb, HL, E);
  CYC(b_+25, b_+26); L = alu_inc8(gb, L);
  CYC(b_+26, b_+27); mem_wr(gb, HL, D);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+31); A = mem_rd(gb, wTextIndexH);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+33); H = D;
  CYC(b_+33, b_+34); L = E;
  CYC(b_+34, b_+35); SET_BC(pop_effect(gb));
  CYC(b_+35, b_+36); SET_DE(pop_effect(gb));
  CYC(b_+36, b_+37); ret_effect(gb);
}

void popFromTextStack_hook(GB *gb) {
  BASE(popFromTextStack);
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+2); push_effect(gb, BC);
  CYC(b_+2, b_+5); SET_HL((w7TextStack + 3));
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+6, b_+9); mem_wr(gb, wTextIndexH, A);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+10, b_+13); SET_DE((w7TextAddress + 1));
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+15); B = A;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+16, b_+17); E = alu_dec8(gb, E);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL);
  CYC(b_+20, b_+21); E = alu_dec8(gb, E);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); push_effect(gb, BC);
  CYC(b_+23, b_+26); SET_DE((w7TextStack + 4));
  CYC(b_+26, b_+28); B = 0x1c;
  for (;;) {
    CYC(b_+28, b_+29); A = mem_rd(gb, DE);
    CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+30, b_+31); E = alu_inc8(gb, E);
    CYC(b_+31, b_+32); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+32, b_+34); continue; }
    CYC(b_+32, b_+34);
    break;
  }
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CYC(b_+35, b_+36); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+37, b_+38); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+38, b_+39); mem_wr(gb, HL, A);
  CYC(b_+39, b_+40); SET_HL(pop_effect(gb));
  CYC(b_+40, b_+41); SET_BC(pop_effect(gb));
  CYC(b_+41, b_+42); SET_DE(pop_effect(gb));
  CYC(b_+42, b_+43); ret_effect(gb);
}

void incHlAndUpdateBank_hook(GB *gb);
void textControlCodeC_ret_hook(GB *gb);
void textControlCodeC_3_hook(GB *gb);
void textControlCodeC_1_hook(GB *gb);
void textControlCodeC_1_drawDigit_hook(GB *gb);

void readByteFromW7ActiveBankAndIncHl_hook(GB *gb) {
  BASE(readByteFromW7ActiveBankAndIncHl);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, readByteFromW7ActiveBank_hook, SYM(readByteFromW7ActiveBank), SYM(incHlAndUpdateBank));
  incHlAndUpdateBank_hook(gb);
}

void incHlAndUpdateBank_hook(GB *gb) {
  BASE(incHlAndUpdateBank);
  CYC(b_+0, b_+1); L = alu_inc8(gb, L);
  if (!(F & FZ)) { CYCT(b_+1, b_+2); ret_effect(gb); return; }
  CYC(b_+1, b_+2);
  CYC(b_+2, b_+3); H = alu_inc8(gb, H);
  CYC(b_+3, b_+5); alu_bit(gb, 7, H);
  if (F & FZ) { CYCT(b_+5, b_+6); ret_effect(gb); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); H = alu_rrc(gb, H);
  CYC(b_+8, b_+9); push_effect(gb, AF);
  CYC(b_+9, b_+12); A = mem_rd(gb, w7ActiveBank);
  CYC(b_+12, b_+13); A = alu_inc8(gb, A);
  CYC(b_+13, b_+16); mem_wr(gb, w7ActiveBank, A);
  CYC(b_+16, b_+17); SET_AF(pop_effect(gb));
  CYC(b_+17, b_+18); ret_effect(gb);
}

void getCharacterDisplayLength_hook(GB *gb) {
  BASE(getCharacterDisplayLength);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+4); A = mem_rd(gb, wTextSpeed);
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+7); alu_rrca(gb);
  CYC(b_+7, b_+10); SET_HL((SYM(textSpeedData) + 2));
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  add_a_to_hl(gb);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); SET_HL(pop_effect(gb));
  CYC(b_+13, b_+14); ret_effect(gb);
}

void textControlCodeC_0_hook(GB *gb) {
  BASE(textControlCodeC_0);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTextSpeed);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); alu_rrca(gb);
  CYC(b_+6, b_+7); alu_add(gb, C);
  CYC(b_+7, b_+10); SET_HL(SYM(textSpeedData));
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  add_a_to_hl(gb);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+15); mem_wr(gb, w7CharacterDisplayLength, A);
  CYC(b_+15, b_+17); textControlCodeC_ret_hook(gb);
}

void textControlCodeC_7_hook(GB *gb) {
  BASE(textControlCodeC_7);
  CYC(b_+0, b_+2); A = 0x78;
  CYC(b_+2, b_+5); mem_wr(gb, w7TextSlowdownTimer, A);
  CYC(b_+5, b_+7); textControlCodeC_ret_hook(gb);
}

void textControlCodeC_5_hook(GB *gb) {
  BASE(textControlCodeC_5);
  CYC(b_+0, b_+3); SET_HL(w7d0c1);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  textControlCodeC_3_hook(gb);
}

void textControlCodeC_3_hook(GB *gb) {
  BASE(textControlCodeC_3);
  CYC(b_+0, b_+3); SET_HL(w7d0c1);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02);
  textControlCodeC_ret_hook(gb);
}

void textControlCodeC_ret_hook(GB *gb) {
  BASE(textControlCodeC_ret);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_BC(pop_effect(gb));
  CYC(b_+2, b_+3); ret_effect(gb);
}

void textControlCodeC_6_hook(GB *gb) {
  BASE(textControlCodeC_6);
  CYC(b_+0, b_+3); A = mem_rd(gb, (wcbaa + 1));
  CYC(b_+3, b_+6); mem_wr(gb, (wTextNumberSubstitution + 1), A);
  CYC(b_+6, b_+9); A = mem_rd(gb, wcbaa);
  CYC(b_+9, b_+12); mem_wr(gb, wTextNumberSubstitution, A);
  textControlCodeC_1_hook(gb);
}

void textControlCodeC_1_hook(GB *gb) {
  BASE(textControlCodeC_1);
  CYC(b_+0, b_+1); SET_HL(pop_effect(gb));
  CYC(b_+1, b_+2); SET_BC(pop_effect(gb));
  CYC(b_+2, b_+5); A = mem_rd(gb, (wTextNumberSubstitution + 1));
  CYC(b_+5, b_+6); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+6, b_+8); goto no_hundreds; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+11); push_effect(gb, b_+11);
  textControlCodeC_1_drawDigit_hook(gb);
  CYC(b_+11, b_+14); A = mem_rd(gb, wTextNumberSubstitution);
  CYC(b_+14, b_+16); alu_and(gb, 0xf0);
  CYC(b_+16, b_+18); goto draw_tens;
no_hundreds:
  CYC(b_+18, b_+21); A = mem_rd(gb, wTextNumberSubstitution);
  CYC(b_+21, b_+23); alu_and(gb, 0xf0);
  if (F & FZ) { CYCT(b_+23, b_+25); goto draw_ones; }
  CYC(b_+23, b_+25);
draw_tens:
  CYC(b_+25, b_+27); A = alu_swap(gb, A);
  CYC(b_+27, b_+30); push_effect(gb, b_+30);
  textControlCodeC_1_drawDigit_hook(gb);
draw_ones:
  CYC(b_+30, b_+33); A = mem_rd(gb, wTextNumberSubstitution);
  CYC(b_+33, b_+35); alu_and(gb, 0x0f);
  textControlCodeC_1_drawDigit_hook(gb);
}

void textControlCodeC_1_drawDigit_hook(GB *gb) {
  BASE(textControlCodeC_1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+35, b_+37); alu_add(gb, 0x30);
  CALL_C(b_+37, setLineTextBuffers_hook, SYM(setLineTextBuffers), b_+40);
  CYC(b_+40, b_+43); retrieveTextCharacter_hook(gb);
}

static void textControlCodeC_2_getNextTextboxOptionPosition(GB *gb) {
  BASE(textControlCodeC_2);
  CYC(b_+28, b_+31); SET_HL(w7TextboxOptionPositions);
  for (;;) {
    CYC(b_+31, b_+32); A = mem_rd(gb, HL);
    CYC(b_+32, b_+33); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+33, b_+34); ret_effect(gb);
      return;
    }
    CYC(b_+33, b_+34);
    CYC(b_+34, b_+35); L = alu_inc8(gb, L);
    CYC(b_+35, b_+37);
  }
}

void textControlCodeC_2_hook(GB *gb) {
  BASE(textControlCodeC_2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); push_effect(gb, b_+3);
  textControlCodeC_2_getNextTextboxOptionPosition(gb);
  CYC(b_+3, b_+6); A = mem_rd(gb, w7d0c1);
  CYC(b_+6, b_+8); alu_or(gb, 0x04);
  CYC(b_+8, b_+11); mem_wr(gb, w7d0c1, A);
  CYC(b_+11, b_+12); A = E;
  CYC(b_+12, b_+13); alu_add(gb, A);
  CYC(b_+13, b_+15); alu_or(gb, 0x60);
  CYC(b_+15, b_+16); B = A;
  CYC(b_+16, b_+17); B = alu_inc8(gb, B);
  CYC(b_+17, b_+18); mem_wr(gb, HL, B);
  CYC(b_+18, b_+19); SET_HL(pop_effect(gb));
  CYC(b_+19, b_+20); SET_BC(pop_effect(gb));
  CYC(b_+20, b_+22); A = 0x20;
  CALL_C(b_+22, setLineTextBuffers_hook, SYM(setLineTextBuffers), b_+25);
  CYC(b_+25, b_+28); retrieveTextCharacter_hook(gb);
}
