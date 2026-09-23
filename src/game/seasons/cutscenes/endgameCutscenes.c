#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/code/seasons/cutscenes/endgameCutscenes.s, bank $03. wTmpcbc1 is the
// stage, wTmpcbc2 the state within it, wTmpcbb3 a countdown (16-bit with cbb4 where
// decHlRef16WithCap counts it).

static uint16_t endgame_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void endgame_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

static void endgame_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// endgameCutsceneHandler_09_stage0_body@state16Func0: once the text is gone, the next state
// with countdown e.
static void endgame_after_text(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0_body);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+787, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+790);
  if (!(F & FZ)) { RET_TAKEN(b_+790); return; }
  CYC(b_+790, b_+791);
  CALL_C(b_+791, s_incCbc2, SYM(incCbc2), b_+794);
  CYC(b_+794, b_+797); SET_HL(wTmpcbb3);
  CYC(b_+797, b_+798); mem_wr(gb, HL, E);
  RET(b_+798); return;
}

// endgameCutsceneHandler_09_stage1_body@seasonsFunc_03_5aa2: the credits' Link-and-Din
// sprites from bank $15 at ($30,$38), from the start of OAM.
static void endgame_draw_credits_sprites(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1_body);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+107, b_+110); SET_HL(SYM(oamData_15_4da3));
  CYC(b_+110, b_+112); E = 0x15; // :oamData_15_4da3
  CYC(b_+112, b_+115); SET_BC(0x3038);
  CYC(b_+115, b_+116); alu_xor(gb, A);
  CYC(b_+116, b_+118); mem_wr(gb, hOamTail, A);
  CYC(b_+118, b_+121);
  TAIL(addSpritesFromBankToOam_withOffset);
}

void s_endgameCutsceneHandler_0f_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wTmpcbc1);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == SYM(endgameCutsceneHandler_0f_stage0) && hook_is(gb, SYM(endgameCutsceneHandler_0f_stage0), s_endgameCutsceneHandler_0f_stage0_hook)) { s_endgameCutsceneHandler_0f_stage0_hook(gb); return; }
    else if (jt_ == SYM(endgameCutsceneHandler_0f_stage1) && hook_is(gb, SYM(endgameCutsceneHandler_0f_stage1), s_endgameCutsceneHandler_0f_stage1_hook)) { s_endgameCutsceneHandler_0f_stage1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void s_endgameCutsceneHandler_0f_stage0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_updateStatusBar, SYM(updateStatusBar), b_+3);
  CALL_C(b_+3, s_endgameCutsceneHandler_0f_stage0_body_hook, SYM(endgameCutsceneHandler_0f_stage0_body), b_+6);
  CYC(b_+6, b_+9);
  TAIL(updateAllObjects);
}

// Onox's castle collapsing after the fight: shaking, falling rubble, Din's text, then the
// energy swirl and a fade to white into stage 1.
void s_endgameCutsceneHandler_0f_stage0_body_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f_stage0_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wTmpcbc2);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+33) { goto state0; }
    else if (jt_ == b_+74) { goto state1; }
    else if (jt_ == b_+91) { goto state2; }
    else if (jt_ == b_+109) { goto state3; }
    else if (jt_ == b_+135) { goto state4; }
    else if (jt_ == b_+155) { goto state5; }
    else if (jt_ == b_+170) { goto state6; }
    else if (jt_ == b_+190) { goto state7; }
    else if (jt_ == b_+208) { goto state8; }
    else if (jt_ == b_+228) { goto state9; }
    else if (jt_ == b_+249) { goto stateA; }
    else if (jt_ == b_+254) { goto stateB; }
    else if (jt_ == b_+280) { goto stateC; }
    else if (jt_ == b_+298) { goto stateD; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+33, b_+35); A = 0x01;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+39); SET_HL(wActiveRing);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0xff);
  CYC(b_+41, b_+42); alu_xor(gb, A);
  CYC(b_+42, b_+44); mem_wr(gb, hActiveObjectType, A);
  CYC(b_+44, b_+47); SET_DE(w1Link);
  CYC(b_+47, b_+50); SET_BC(0xf8f0);
  CYC(b_+50, b_+52); A = 0x28;
  CALL_C(b_+52, s_objectCreateExclamationMark, SYM(objectCreateExclamationMark), b_+55);
  CYC(b_+55, b_+57); A = 0x28;
  CALL_C(b_+57, s_objectCreateExclamationMark, SYM(objectCreateExclamationMark), b_+60);
  CYC(b_+60, b_+62); L = 0x4b; // Interaction.yh
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x30);
  CYC(b_+64, b_+65); L = alu_inc8(gb, L);
  CYC(b_+65, b_+66); L = alu_inc8(gb, L);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x78);
  CYC(b_+68, b_+71); SET_HL(wTmpcbb3);
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x0a);
  RET(b_+73); return;

state1:
  CALL_C(b_+74, s_decCbb3, SYM(decCbb3), b_+77);
  if (!(F & FZ)) { RET_TAKEN(b_+77); return; }
  CYC(b_+77, b_+78);
  CYC(b_+78, b_+81); SET_HL(wTmpcbb3);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x1e);
  CYC(b_+83, b_+85); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+85, playSound_b00_hook, SYM(playSound_b00), b_+88);
  CYC(b_+88, b_+91);
  TAIL(incCbc2);

state2:
  CALL_C(b_+91, s_seasonsFunc_03_5cfb_hook, SYM(seasonsFunc_03_5cfb), b_+94);
  CALL_C(b_+94, s_decCbb3, SYM(decCbb3), b_+97);
  if (!(F & FZ)) { RET_TAKEN(b_+97); return; }
  CYC(b_+97, b_+98);
  CALL_C(b_+98, s_incCbc2, SYM(incCbc2), b_+101);
  CYC(b_+101, b_+104); SET_HL(wTmpcbb3);
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x96);
  CYC(b_+106, b_+109);
  s_seasonsFunc_03_5d0b_hook(gb); return;

state3:
  CALL_C(b_+109, s_seasonsFunc_03_5cfb_hook, SYM(seasonsFunc_03_5cfb), b_+112);
  CALL_C(b_+112, s_decCbb3, SYM(decCbb3), b_+115);
  if (!(F & FZ)) { RET_TAKEN(b_+115); return; }
  CYC(b_+115, b_+116);
  CALL_C(b_+116, s_incCbc2, SYM(incCbc2), b_+119);
  CYC(b_+119, b_+121); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(b_+121, playSound_b00_hook, SYM(playSound_b00), b_+124);
  CYC(b_+124, b_+127); SET_HL(wTmpcbb3);
  CYC(b_+127, b_+129); mem_wr(gb, HL, 0x3c);
  CYC(b_+129, b_+132); SET_BC(0x3d0e); // TX_3d0e
  CYC(b_+132, b_+135);
  TAIL(showText);

state4:
  CALL_C(b_+135, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+138);
  if (!(F & FZ)) { RET_TAKEN(b_+138); return; }
  CYC(b_+138, b_+139);
  CALL_C(b_+139, s_incCbc2, SYM(incCbc2), b_+142);
  CYC(b_+142, b_+144); A = 0x21; // MUS_DISASTER
  CALL_C(b_+144, playSound_b00_hook, SYM(playSound_b00), b_+147);
  CYC(b_+147, b_+150); SET_HL(wTmpcbb3);
  CYC(b_+150, b_+152); mem_wr(gb, HL, 0x3c);
  CYC(b_+152, b_+155);
  s_seasonsFunc_03_5d0b_hook(gb); return;

state5:
  CALL_C(b_+155, s_seasonsFunc_03_5cfb_hook, SYM(seasonsFunc_03_5cfb), b_+158);
  CALL_C(b_+158, s_decCbb3, SYM(decCbb3), b_+161);
  if (!(F & FZ)) { RET_TAKEN(b_+161); return; }
  CYC(b_+161, b_+162);
  CYC(b_+162, b_+165); SET_HL(wTmpcbb3);
  CYC(b_+165, b_+167); mem_wr(gb, HL, 0x5a);
  CYC(b_+167, b_+170);
  TAIL(incCbc2);

state6:
  CALL_C(b_+170, s_seasonsFunc_03_5cfb_hook, SYM(seasonsFunc_03_5cfb), b_+173);
  CALL_C(b_+173, s_decCbb3, SYM(decCbb3), b_+176);
  if (!(F & FZ)) { RET_TAKEN(b_+176); return; }
  CYC(b_+176, b_+177);
  CALL_C(b_+177, s_incCbc2, SYM(incCbc2), b_+180);
  CYC(b_+180, b_+183); SET_HL(wTmpcbb3);
  CYC(b_+183, b_+185); mem_wr(gb, HL, 0x3c);
  CYC(b_+185, b_+187); A = 0xf1; // SNDCTRL_STOPSFX
  CYC(b_+187, b_+190);
  TAIL(playSound_b00);

state7:
  CALL_C(b_+190, s_decCbb3, SYM(decCbb3), b_+193);
  if (!(F & FZ)) { RET_TAKEN(b_+193); return; }
  CYC(b_+193, b_+194);
  CALL_C(b_+194, s_incCbc2, SYM(incCbc2), b_+197);
  CYC(b_+197, b_+200); SET_HL(wTmpcbb3);
  CYC(b_+200, b_+202); mem_wr(gb, HL, 0x3c);
  CYC(b_+202, b_+205); SET_BC(0x3d0f); // TX_3d0f
  CYC(b_+205, b_+208);
  TAIL(showText);

state8:
  CALL_C(b_+208, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+211);
  if (!(F & FZ)) { RET_TAKEN(b_+211); return; }
  CYC(b_+211, b_+212);
  CALL_C(b_+212, s_incCbc2, SYM(incCbc2), b_+215);
  CYC(b_+215, b_+218); SET_HL(wTmpcbb3);
  CYC(b_+218, b_+220); mem_wr(gb, HL, 0x2c);
  CYC(b_+220, b_+221); SET_HL(HL + 1);
  CYC(b_+221, b_+223); mem_wr(gb, HL, 0x01);
  CYC(b_+223, b_+225); B = 0x03;
  CYC(b_+225, b_+228);
  s_seasonsFunc_03_5d12_hook(gb); return;

state9:
  CYC(b_+228, b_+231); SET_HL(wTmpcbb3);
  CALL_C(b_+231, s_decHlRef16WithCap, SYM(decHlRef16WithCap), b_+234);
  if (!(F & FZ)) { RET_TAKEN(b_+234); return; }
  CYC(b_+234, b_+235);
  CALL_C(b_+235, s_incCbc2, SYM(incCbc2), b_+238);
  CYC(b_+238, b_+241); SET_HL(wTmpcbb3);
  CYC(b_+241, b_+243); mem_wr(gb, HL, 0x3c);
  CYC(b_+243, b_+246); SET_BC(0x3d10); // TX_3d10
  CYC(b_+246, b_+249);
  TAIL(showText);

stateA:
  CYC(b_+249, b_+251); E = 0x1e;
  CYC(b_+251, b_+254);
  endgame_after_text(gb); return;

stateB:
  CALL_C(b_+254, s_seasonsFunc_03_5cfb_hook, SYM(seasonsFunc_03_5cfb), b_+257);
  CALL_C(b_+257, s_decCbb3, SYM(decCbb3), b_+260);
  if (!(F & FZ)) { RET_TAKEN(b_+260); return; }
  CYC(b_+260, b_+261);
  CALL_C(b_+261, s_incCbc2, SYM(incCbc2), b_+264);
  CALL_C(b_+264, s_seasonsFunc_03_5d0b_hook, SYM(seasonsFunc_03_5d0b), b_+267);
  CYC(b_+267, b_+269); A = 0x8c;
  CYC(b_+269, b_+272); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+272, b_+274); A = 0xff;
  CYC(b_+274, b_+277); SET_BC(0x4478);
  CYC(b_+277, b_+280);
  TAIL(createEnergySwirlGoingOut);

stateC:
  CALL_C(b_+280, s_seasonsFunc_03_5cfb_hook, SYM(seasonsFunc_03_5cfb), b_+283);
  CALL_C(b_+283, s_decCbb3, SYM(decCbb3), b_+286);
  if (!(F & FZ)) { RET_TAKEN(b_+286); return; }
  CYC(b_+286, b_+287);
  CALL_C(b_+287, s_incCbc2, SYM(incCbc2), b_+290);
  CYC(b_+290, b_+293); SET_HL(wTmpcbb3);
  CYC(b_+293, b_+295); mem_wr(gb, HL, 0x3c);
  CYC(b_+295, b_+298);
  s_seasonsFunc_03_5d0b_hook(gb); return;

stateD:
  CALL_C(b_+298, s_seasonsFunc_03_5cfb_hook, SYM(seasonsFunc_03_5cfb), b_+301);
  CALL_C(b_+301, s_decCbb3, SYM(decCbb3), b_+304);
  if (!(F & FZ)) { RET_TAKEN(b_+304); return; }
  CYC(b_+304, b_+305);
  CALL_C(b_+305, s_incCbc1, SYM(incCbc1), b_+308);
  CYC(b_+308, b_+309); L = alu_inc8(gb, L);
  CYC(b_+309, b_+310); alu_xor(gb, A);
  CYC(b_+310, b_+311); mem_wr(gb, HL, A);
  CYC(b_+311, b_+313); A = 0x03;
  CYC(b_+313, b_+316);
  TAIL(fadeoutToWhiteWithDelay);
}

// Shaking, with the rumble sound every 16 frames.
void s_seasonsFunc_03_5cfb_hook(GB *gb) {
  BASE(seasonsFunc_03_5cfb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CALL_C(b_+2, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+5);
  s_seasonsFunc_03_5d00_hook(gb); return;
}

void s_seasonsFunc_03_5d00_hook(GB *gb) {
  BASE(seasonsFunc_03_5d00);
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); A = 0xb8; // SND_RUMBLE2
  if (F & FZ) { CYCT(b_+7, b_+10); TAIL(playSound_b00); }
  CYC(b_+7, b_+10);
  RET(b_+10); return;
}

// Falling rubble (PART_48).
void s_seasonsFunc_03_5d0b_hook(GB *gb) {
  BASE(seasonsFunc_03_5d0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreePartSlot, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x48);
  RET(b_+6); return;
}

// INTERAC_48 subid 0 with var03 b.
void s_seasonsFunc_03_5d12_hook(GB *gb) {
  BASE(seasonsFunc_03_5d12);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x48);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x00);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); mem_wr(gb, HL, B);
  RET(b_+11); return;
}

void s_endgameCutsceneHandler_0f_stage1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_updateStatusBar, SYM(updateStatusBar), b_+3);
  CALL_C(b_+3, s_endgameCutsceneHandler_0f_stage1_body_hook, SYM(endgameCutsceneHandler_0f_stage1_body), b_+6);
  CYC(b_+6, b_+9);
  TAIL(updateAllObjects);
}

// Outside (room $22b): the energy swirl, the flash, then the credits sprites, the fade to
// black and the switch to the next cutscene.
void s_endgameCutsceneHandler_0f_stage1_body_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0f_stage1_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wTmpcbc2);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+27) { goto state0; }
    else if (jt_ == b_+88) { goto state1; }
    else if (jt_ == b_+108) { goto state2; }
    else if (jt_ == b_+123) { goto state3; }
    else if (jt_ == b_+150) { goto state4; }
    else if (jt_ == b_+172) { goto state5; }
    else if (jt_ == b_+192) { goto state6; }
    else if (jt_ == b_+217) { goto state7; }
    else if (jt_ == b_+231) { goto state8; }
    else if (jt_ == b_+276) { goto state9; }
    else if (jt_ == b_+296) { goto stateA; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+27, s_seasonsFunc_03_5cfb_hook, SYM(seasonsFunc_03_5cfb), b_+30);
  CYC(b_+30, b_+33); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+33, b_+34); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CALL_C(b_+35, s_incCbc2, SYM(incCbc2), b_+38);
  CYC(b_+38, b_+39); alu_xor(gb, A);
  CYC(b_+39, b_+42); SET_BC(0x022b); // ROOM_SEASONS_22b
  CALL_C(b_+42, s_disableLcdAndLoadRoom_body_hook, SYM(disableLcdAndLoadRoom_body), b_+45);
  CALL_C(b_+45, s_refreshObjectGfx, SYM(refreshObjectGfx), b_+48);
  CYC(b_+48, b_+50); B = 0x0c;
  CALL_C(b_+50, s_getEntryFromObjectTable1, SYM(getEntryFromObjectTable1), b_+53);
  CYC(b_+53, b_+54); D = H;
  CYC(b_+54, b_+55); E = L;
  CALL_C(b_+55, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+58);
  CYC(b_+58, b_+60); A = 0x04;
  CYC(b_+60, b_+62); B = 0x02;
  CALL_C(b_+62, s_seasonsFunc_03_642e_hook, SYM(seasonsFunc_03_642e), b_+65);
  CYC(b_+65, b_+67); A = 0xf1; // SNDCTRL_STOPSFX
  CALL_C(b_+67, playSound_b00_hook, SYM(playSound_b00), b_+70);
  CYC(b_+70, b_+72); A = 0xfa;
  CALL_C(b_+72, playSound_b00_hook, SYM(playSound_b00), b_+75);
  CYC(b_+75, b_+77); A = 0x02;
  CALL_C(b_+77, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+80);
  CYC(b_+80, b_+83); SET_HL(wTmpcbb3);
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x3c);
  CYC(b_+85, b_+88);
  TAIL(fadeinFromWhiteToRoom);

state1:
  CALL_C(b_+88, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+91);
  if (!(F & FZ)) { RET_TAKEN(b_+91); return; }
  CYC(b_+91, b_+92);
  CALL_C(b_+92, s_incCbc2, SYM(incCbc2), b_+95);
  CYC(b_+95, b_+97); A = 0x3c;
  CYC(b_+97, b_+100); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+100, b_+102); A = 0x64;
  CYC(b_+102, b_+105); SET_BC(0x5850);
  CYC(b_+105, b_+108);
  TAIL(createEnergySwirlGoingIn);

state2:
  CALL_C(b_+108, s_decCbb3, SYM(decCbb3), b_+111);
  if (!(F & FZ)) { RET_TAKEN(b_+111); return; }
  CYC(b_+111, b_+112);
  CYC(b_+112, b_+113); alu_xor(gb, A);
  CYC(b_+113, b_+116); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+116, b_+117); A = alu_dec8(gb, A);
  CYC(b_+117, b_+120); mem_wr(gb, wTmpcbba, A);
  CYC(b_+120, b_+123);
  TAIL(incCbc2);

state3:
  CYC(b_+123, b_+126); SET_HL(wTmpcbb3);
  CYC(b_+126, b_+128); B = 0x01;
  CALL_C(b_+128, s_flashScreen, SYM(flashScreen), b_+131);
  if (F & FZ) { RET_TAKEN(b_+131); return; }
  CYC(b_+131, b_+132);
  CALL_C(b_+132, s_incCbc2, SYM(incCbc2), b_+135);
  CYC(b_+135, b_+138); SET_HL(wTmpcbb3);
  CYC(b_+138, b_+140); mem_wr(gb, HL, 0x3c);
  CYC(b_+140, b_+142); A = 0x01;
  CYC(b_+142, b_+145); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+145, b_+147); A = 0x03;
  CYC(b_+147, b_+150);
  TAIL(fadeinFromWhiteWithDelay);

state4:
  CALL_C(b_+150, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+153);
  if (!(F & FZ)) { RET_TAKEN(b_+153); return; }
  CYC(b_+153, b_+154);
  CYC(b_+154, b_+156); A = 0x01;
  CYC(b_+156, b_+159); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+159, b_+161); A = 0x29;
  CALL_C(b_+161, playSound_b00_hook, SYM(playSound_b00), b_+164);
  CYC(b_+164, b_+167); SET_HL(wTmpcbb3);
  CYC(b_+167, b_+169); mem_wr(gb, HL, 0x3c);
  CYC(b_+169, b_+172);
  TAIL(incCbc2);

state5:
  CALL_C(b_+172, s_decCbb3, SYM(decCbb3), b_+175);
  if (!(F & FZ)) { RET_TAKEN(b_+175); return; }
  CYC(b_+175, b_+176);
  CALL_C(b_+176, s_incCbc2, SYM(incCbc2), b_+179);
  CYC(b_+179, b_+182); SET_HL(wTmpcbb3);
  CYC(b_+182, b_+184); mem_wr(gb, HL, 0x2c);
  CYC(b_+184, b_+185); SET_HL(HL + 1);
  CYC(b_+185, b_+187); mem_wr(gb, HL, 0x01);
  CYC(b_+187, b_+189); B = 0x00;
  CYC(b_+189, b_+192);
  s_seasonsFunc_03_5d12_hook(gb); return;

state6:
  CYC(b_+192, b_+195); SET_HL(wTmpcbb3);
  CALL_C(b_+195, s_decHlRef16WithCap, SYM(decHlRef16WithCap), b_+198);
  if (!(F & FZ)) { RET_TAKEN(b_+198); return; }
  CYC(b_+198, b_+199);
  CYC(b_+199, b_+201); A = 0x01;
  CYC(b_+201, b_+204); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+204, b_+207); SET_HL(wTmpcbb3);
  CYC(b_+207, b_+209); mem_wr(gb, HL, 0x3c);
  CYC(b_+209, b_+212); SET_HL(wTmpcfc0);
  CYC(b_+212, b_+214); mem_wr(gb, HL, 0x02);
  CYC(b_+214, b_+217);
  TAIL(incCbc2);

state7:
  CYC(b_+217, b_+220); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+220, b_+222); alu_cp(gb, 0x09);
  if (!(F & FZ)) { RET_TAKEN(b_+222); return; }
  CYC(b_+222, b_+223);
  CALL_C(b_+223, s_incCbc2, SYM(incCbc2), b_+226);
  CYC(b_+226, b_+228); A = 0x03;
  CYC(b_+228, b_+231);
  TAIL(fadeoutToWhiteWithDelay);

state8:
  CYC(b_+231, b_+234); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+234, b_+235); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+235); return; }
  CYC(b_+235, b_+236);
  CALL_C(b_+236, s_incCbc2, SYM(incCbc2), b_+239);
  CALL_C(b_+239, s_disableLcd, SYM(disableLcd), b_+242);
  CALL_C(b_+242, s_clearScreenVariablesAndWramBank1, SYM(clearScreenVariablesAndWramBank1), b_+245);
  CALL_C(b_+245, s_hideStatusBar, SYM(hideStatusBar), b_+248);
  CYC(b_+248, b_+250); A = 0x3c;
  CALL_C(b_+250, s_loadGfxHeader, SYM(loadGfxHeader), b_+253);
  CYC(b_+253, b_+255); A = 0xad; // PALH_ad
  CALL_C(b_+255, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+258);
  CYC(b_+258, b_+261); SET_HL(wTmpcbb3);
  CYC(b_+261, b_+263); mem_wr(gb, HL, 0xf0);
  CYC(b_+263, b_+265); A = 0x04;
  CALL_C(b_+265, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+268);
  CALL_L(b_+268, endgame_draw_credits_sprites, b_+271);
  CYC(b_+271, b_+273); A = 0x03;
  CYC(b_+273, b_+276);
  TAIL(fadeinFromWhiteWithDelay);

state9:
  CALL_L(b_+276, endgame_draw_credits_sprites, b_+279);
  CALL_C(b_+279, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+282);
  if (!(F & FZ)) { RET_TAKEN(b_+282); return; }
  CYC(b_+282, b_+283);
  CALL_C(b_+283, s_incCbc2, SYM(incCbc2), b_+286);
  CYC(b_+286, b_+289); SET_HL(wTmpcbb3);
  CYC(b_+289, b_+291); mem_wr(gb, HL, 0x10);
  CYC(b_+291, b_+293); A = 0x03;
  CYC(b_+293, b_+296);
  TAIL(fadeoutToBlackWithDelay);

stateA:
  CALL_L(b_+296, endgame_draw_credits_sprites, b_+299);
  CALL_C(b_+299, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+302);
  if (!(F & FZ)) { RET_TAKEN(b_+302); return; }
  CYC(b_+302, b_+303);
  CYC(b_+303, b_+305); A = 0x0a;
  CYC(b_+305, b_+308); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CALL_C(b_+308, s_seasonsFunc_03_646a_hook, SYM(seasonsFunc_03_646a), b_+311);
  CYC(b_+311, b_+314); SET_HL(wRoomLayout);
  CYC(b_+314, b_+317); SET_BC(0x00c0);
  CALL_C(b_+317, s_clearMemoryBc, SYM(clearMemoryBc), b_+320);
  CYC(b_+320, b_+323); SET_HL(wRoomCollisions);
  CYC(b_+323, b_+326); SET_BC(0x00c0);
  CALL_C(b_+326, s_clearMemoryBc, SYM(clearMemoryBc), b_+329);
  CYC(b_+329, b_+330); alu_xor(gb, A);
  CYC(b_+330, b_+332); mem_wr(gb, hCameraY, A);
  CYC(b_+332, b_+334); mem_wr(gb, hCameraX, A);
  CYC(b_+334, b_+337); SET_HL(wTmpcbb3);
  CYC(b_+337, b_+339); mem_wr(gb, HL, 0x3c);
  CYC(b_+339, b_+341); A = 0xfb;
  CYC(b_+341, b_+344);
  TAIL(playSound_b00);
}

// Room bc (group, room) with room state modifier a: LCD off, screen and Link variables
// cleared, then the room loaded.
void s_disableLcdAndLoadRoom_body_hook(GB *gb) {
  BASE(disableLcdAndLoadRoom_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); mem_wr(gb, wRoomStateModifier, A);
  CYC(b_+3, b_+4); A = B;
  CYC(b_+4, b_+7); mem_wr(gb, wActiveGroup, A);
  CYC(b_+7, b_+8); A = C;
  CYC(b_+8, b_+11); mem_wr(gb, wActiveRoom, A);
  CALL_C(b_+11, s_disableLcd, SYM(disableLcd), b_+14);
  CALL_C(b_+14, s_clearScreenVariablesAndWramBank1, SYM(clearScreenVariablesAndWramBank1), b_+17);
  CYC(b_+17, b_+20); SET_HL(wLinkInAir);
  CYC(b_+20, b_+22); B = 0x88; // wcce9-wLinkInAir
  CALL_C(b_+22, s_clearMemory, SYM(clearMemory), b_+25);
  s_seasonsFunc_03_63eb_hook(gb); return;
}

// The active room's music, tileset, graphics and layout, scroll mode 1, common graphics, OAM
// cleared.
void s_seasonsFunc_03_63eb_hook(GB *gb) {
  BASE(seasonsFunc_03_63eb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_initializeVramMaps, SYM(initializeVramMaps), b_+3);
  CALL_C(b_+3, s_loadScreenMusicAndSetRoomPack, SYM(loadScreenMusicAndSetRoomPack), b_+6);
  CALL_C(b_+6, s_loadTilesetData, SYM(loadTilesetData), b_+9);
  CALL_C(b_+9, s_loadTilesetGraphics, SYM(loadTilesetGraphics), b_+12);
  CALL_C(b_+12, s_func_131f, SYM(func_131f), b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+20); mem_wr(gb, wScrollMode, A);
  CALL_C(b_+20, s_loadCommonGraphics, SYM(loadCommonGraphics), b_+23);
  CYC(b_+23, b_+26);
  TAIL(clearOam);
}

// Object table 1 entry b (none for $ff) with its graphics; wTmpcbb4 5, 6 and 7 also load a
// set of object graphics.
void s_seasonsFunc_03_6405_hook(GB *gb) {
  BASE(seasonsFunc_03_6405);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = B;
  CYC(b_+1, b_+3); alu_cp(gb, 0xff);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); push_effect(gb, BC);
  CALL_C(b_+5, s_refreshObjectGfx, SYM(refreshObjectGfx), b_+8);
  CYC(b_+8, b_+9); SET_BC(pop_effect(gb));
  CALL_C(b_+9, s_getEntryFromObjectTable1, SYM(getEntryFromObjectTable1), b_+12);
  CYC(b_+12, b_+13); D = H;
  CYC(b_+13, b_+14); E = L;
  CALL_C(b_+14, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+17);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+21); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+21, b_+24); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+24, b_+26); alu_cp(gb, 0x05);
  if (F & FZ) {
    CYCT(b_+26, b_+28);
    CYC(b_+37, b_+39); A = 0x04;
    CYC(b_+39, b_+41); B = 0x03;
    s_seasonsFunc_03_642e_hook(gb); return;
  }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); alu_cp(gb, 0x06);
  if (F & FZ) {
    CYCT(b_+30, b_+32);
    CYC(b_+59, b_+61); A = 0x0f;
    CYC(b_+61, b_+63); B = 0x06;
    CYC(b_+63, b_+65);
    s_seasonsFunc_03_642e_hook(gb); return;
  }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); alu_cp(gb, 0x07);
  if (F & FZ) {
    CYCT(b_+34, b_+36);
    CYC(b_+65, b_+67); A = 0x13;
    CYC(b_+67, b_+69); B = 0x02;
    CYC(b_+69, b_+71);
    s_seasonsFunc_03_642e_hook(gb); return;
  }
  CYC(b_+34, b_+36);
  RET(b_+36); return;
}

void s_seasonsFunc_03_642e_hook(GB *gb) {
  BASE(seasonsFunc_03_642e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_seasonsFunc_03_6434_hook, SYM(seasonsFunc_03_6434), b_+3);
  CYC(b_+3, b_+6);
  TAIL(reloadObjectGfx_b00);
}

// b consecutive object graphics headers from a into wLoadedObjectGfx, each flagged loaded.
void s_seasonsFunc_03_6434_hook(GB *gb) {
  BASE(seasonsFunc_03_6434);
  CYC(b_+0, b_+3); SET_HL(wLoadedObjectGfx);
  for (;;) {
    CYC(b_+3, b_+4); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+4, b_+5); A = alu_inc8(gb, A);
    CYC(b_+5, b_+7); mem_wr(gb, HL, 0x01);
    CYC(b_+7, b_+8); L = alu_inc8(gb, L);
    CYC(b_+8, b_+9); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+9, b_+11); continue; }
    CYC(b_+9, b_+11);
    break;
  }
  RET(b_+11); return;
}

// Room state modifier a, the active room reloaded, gfx register state 2.
void s_seasonsFunc_03_644c_hook(GB *gb) {
  BASE(seasonsFunc_03_644c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); mem_wr(gb, wRoomStateModifier, A);
  CALL_C(b_+3, s_disableLcd, SYM(disableLcd), b_+6);
  CALL_C(b_+6, s_seasonsFunc_03_63eb_hook, SYM(seasonsFunc_03_63eb), b_+9);
  CYC(b_+9, b_+11); A = 0x02;
  CYC(b_+11, b_+14);
  TAIL(loadGfxRegisterStateIndex);
}

// NZ while text is showing, else decCbb3.
void s_seasonsFunc_03_645a_hook(GB *gb) {
  BASE(seasonsFunc_03_645a);
  CYC(b_+0, b_+3); A = mem_rd(gb, wTextIsActive);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8);
  TAIL(decCbb3);
}

// NZ while a fade runs, else decCbb3.
void s_seasonsFunc_03_6462_hook(GB *gb) {
  BASE(seasonsFunc_03_6462);
  CYC(b_+0, b_+3); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8);
  TAIL(decCbb3);
}

void s_seasonsFunc_03_646a_hook(GB *gb) {
  BASE(seasonsFunc_03_646a);
  CYC(b_+0, b_+3); SET_HL(wTmpcbb3);
  CYC(b_+3, b_+5); B = 0x10;
  CYC(b_+5, b_+8);
  TAIL(clearMemory);
}

// One heart of health, the B button item saved in wcc1f, and the active ring off.
void s_seasonsFunc_03_66dc_hook(GB *gb) {
  BASE(seasonsFunc_03_66dc);
  CYC(b_+0, b_+3); SET_HL(wLinkHealth);
  CYC(b_+3, b_+5); mem_wr(gb, HL, 0x04);
  CYC(b_+5, b_+7); L = 0x80; // <wInventoryB
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); B = mem_rd(gb, HL);
  CYC(b_+9, b_+12); SET_HL(wcc1f);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, B);
  CYC(b_+14, b_+17);
  TAIL(disableActiveRing);
}

// Full health, the saved B button item restored, and the active ring back on.
void s_seasonsFunc_03_66ed_hook(GB *gb) {
  BASE(seasonsFunc_03_66ed);
  CYC(b_+0, b_+3); SET_HL(wLinkMaxHealth);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  CYC(b_+5, b_+8); SET_HL(wcc1f);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); B = mem_rd(gb, HL);
  CYC(b_+10, b_+13); SET_HL(wInventoryB);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); mem_wr(gb, HL, B);
  CYC(b_+15, b_+18);
  TAIL(enableActiveRing);
}

// endgameCutsceneHandler_09_stage1_body from +127: sprites hl at (b,c) from the start of OAM.
static void endgame_sprites_from_oam_start(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1_body);
  CYC(b_+127, b_+128); alu_xor(gb, A);
  CYC(b_+128, b_+130); mem_wr(gb, hOamTail, A);
  CYC(b_+130, b_+133);
  TAIL(addSpritesToOam_withOffset);
}

// endgameCutsceneHandler_09_stage1_body@seasonsFunc_03_5ab0: Din's sprites at ($30,$38).
static void endgame_draw_din_sprites(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1_body);
  CYC(b_+121, b_+124); SET_HL(SYM(seasonsOamData_03_65a4));
  CYC(b_+124, b_+127); SET_BC(0x3038);
  endgame_sprites_from_oam_start(gb); return;
}

// endgameCutsceneHandler_09_stage1_body from +98: sprites hl at ($30, wTmpcbb5).
static void endgame_scrolled_sprites(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1_body);
  CYC(b_+98, b_+100); B = 0x30;
  CYC(b_+100, b_+103); SET_DE(wTmpcbb5);
  CYC(b_+103, b_+104); A = mem_rd(gb, DE);
  CYC(b_+104, b_+105); C = A;
  CYC(b_+105, b_+107);
  endgame_sprites_from_oam_start(gb); return;
}

// endgameCutsceneHandler_09_stage1_body@state4Func0: the scrolling seed sprites.
static void endgame_draw_seed_sprites(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1_body);
  CYC(b_+167, b_+170); SET_HL(SYM(seasonsOamData_03_650b));
  CYC(b_+170, b_+172);
  endgame_scrolled_sprites(gb); return;
}

// endgameCutsceneHandler_09_stage0_body@state0Func0: Din (subid c) in the room, Link placed
// by c, the camera, objects disabled, then the fade in.
static void endgame_din_room(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0_body);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+121, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+124);
  if (!(F & FZ)) { CYCT(b_+124, b_+126); goto link; }
  CYC(b_+124, b_+126);
  CYC(b_+126, b_+128); A = 0xa5; // INTERAC_DIN
  CYC(b_+128, b_+131); mem_wr(gb, wInteractionIDToLoadExtraGfx, A);
  CYC(b_+131, b_+132); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+132, b_+133); mem_wr(gb, HL, C);
  CYC(b_+133, b_+136); mem_wr(gb, wLoadedTreeGfxIndex, A);
link:
  CYC(b_+136, b_+137); A = C;
  CYC(b_+137, b_+140); SET_HL(w1Link_enabled);
  CYC(b_+140, b_+142); mem_wr(gb, HL, 0x03);
  CYC(b_+142, b_+145); SET_DE(b_+191); // @state0Table_03_55f3
  CALL_C(b_+145, s_addDoubleIndexToDe, SYM(addDoubleIndexToDe), b_+148);
  CYC(b_+148, b_+149); A = mem_rd(gb, DE);
  CYC(b_+149, b_+150); SET_DE(DE + 1);
  CYC(b_+150, b_+152); L = 0x0b; // w1Link.yh
  CYC(b_+152, b_+153); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+153, b_+154); L = alu_inc8(gb, L);
  CYC(b_+154, b_+155); A = mem_rd(gb, DE);
  CYC(b_+155, b_+156); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+156, b_+158); L = 0x08; // w1Link.direction
  CYC(b_+158, b_+160); mem_wr(gb, HL, 0x03);
  CYC(b_+160, b_+161); A = C;
  CYC(b_+161, b_+164); SET_BC(0x0050);
  CYC(b_+164, b_+165); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+165, b_+167); goto camera; }
  CYC(b_+165, b_+167);
  CYC(b_+167, b_+170); SET_BC(0x3050);
camera:
  CYC(b_+170, b_+173); SET_HL(hCameraY);
  CYC(b_+173, b_+174); mem_wr(gb, HL, B);
  CYC(b_+174, b_+177); SET_HL(hCameraX);
  CYC(b_+177, b_+178); mem_wr(gb, HL, C);
  CYC(b_+178, b_+180); A = 0x80;
  CYC(b_+180, b_+183); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+183, b_+185); A = 0x02;
  CALL_C(b_+185, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+188);
  CYC(b_+188, b_+191);
  TAIL(fadeinFromWhiteToRoom);
}

// endgameCutsceneHandler_09_stage0_body@stateBFunc0: text bc with textbox flags 1.
static void endgame_text_flags1(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0_body);
  CYC(b_+471, b_+473); A = 0x01;
  CYC(b_+473, b_+476); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+476, b_+479);
  TAIL(showText);
}

// endgameCutsceneHandler_09_stage0_body@stateDFunc0: once the fade is done, the next state,
// countdown e and text bc.
static void endgame_text_after_fade(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0_body);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+505, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+508);
  if (!(F & FZ)) { RET_TAKEN(b_+508); return; }
  CYC(b_+508, b_+509);
  CALL_C(b_+509, s_incCbc2, SYM(incCbc2), b_+512);
  CYC(b_+512, b_+513); A = E;
  CYC(b_+513, b_+516); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+516, b_+519);
  TAIL(showText);
}

// endgameCutsceneHandler_09_stage0_body@state13Func0: textbox flags 8 at position 3.
static void endgame_textbox_style(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0_body);
  CYC(b_+745, b_+747); A = 0x08;
  CYC(b_+747, b_+750); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+750, b_+752); A = 0x03;
  CYC(b_+752, b_+755); mem_wr(gb, wTextboxPosition, A);
  RET(b_+755); return;
}

void s_endgameCutsceneHandler_09_stage0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_updateStatusBar, SYM(updateStatusBar), b_+3);
  CALL_C(b_+3, s_endgameCutsceneHandler_09_stage0_body_hook, SYM(endgameCutsceneHandler_09_stage0_body), b_+6);
  CALL_C(b_+6, s_updateAllObjects, SYM(updateAllObjects), b_+9);
  CYC(b_+9, b_+12);
  TAIL(checkEnemyAndPartCollisionsIfTextInactive);
}

// The ending: Din thanks Link, the visits to the rescued characters (rooms from
// @state7Table0), Twinrova's ritual in the Room of Rites, the Maku seed, then the warp to the
// credits.
void s_endgameCutsceneHandler_09_stage0_body_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage0_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wTmpcbc2);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+85) { goto state0; }
    else if (jt_ == b_+195) { goto state1; }
    else if (jt_ == b_+219) { goto state2; }
    else if (jt_ == b_+228) { goto state3; }
    else if (jt_ == b_+246) { goto state4; }
    else if (jt_ == b_+261) { goto state5; }
    else if (jt_ == b_+282) { goto state6; }
    else if (jt_ == b_+292) { goto state7; }
    else if (jt_ == b_+386) { goto state8; }
    else if (jt_ == b_+393) { goto state9; }
    else if (jt_ == b_+403) { goto stateA; }
    else if (jt_ == b_+448) { goto stateB; }
    else if (jt_ == b_+479) { goto stateC; }
    else if (jt_ == b_+500) { goto stateD; }
    else if (jt_ == b_+519) { goto stateE; }
    else if (jt_ == b_+544) { goto stateF; }
    else if (jt_ == b_+613) { goto state10; }
    else if (jt_ == b_+636) { goto state11; }
    else if (jt_ == b_+652) { goto state12; }
    else if (jt_ == b_+734) { goto state13; }
    else if (jt_ == b_+756) { goto state14; }
    else if (jt_ == b_+778) { goto state15; }
    else if (jt_ == b_+785) { goto state16; }
    else if (jt_ == b_+799) { goto state17; }
    else if (jt_ == b_+853) { goto state18; }
    else if (jt_ == b_+891) { goto state19; }
    else if (jt_ == b_+909) { goto state1A; }
    else if (jt_ == b_+927) { goto state1B; }
    else if (jt_ == b_+960) { goto state1C; }
    else if (jt_ == b_+980) { goto state1D; }
    else if (jt_ == b_+995) { goto state1E; }
    else if (jt_ == b_+1099) { goto state1F; }
    else if (jt_ == b_+1131) { goto state20; }
    else if (jt_ == b_+1139) { goto state21; }
    else if (jt_ == b_+1151) { goto state22; }
    else if (jt_ == b_+1159) { goto state23; }
    else if (jt_ == b_+1164) { goto state24; }
    else if (jt_ == b_+1184) { goto state25; }
    else if (jt_ == b_+1207) { goto state26; }
    else if (jt_ == b_+1234) { goto state27; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+85, b_+88); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+88, b_+89); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+89); return; }
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+92); B = 0x20;
  CYC(b_+92, b_+95); SET_HL(wTmpcfc0);
  CALL_C(b_+95, s_clearMemory, SYM(clearMemory), b_+98);
  CALL_C(b_+98, s_incCbc2, SYM(incCbc2), b_+101);
  CYC(b_+101, b_+102); alu_xor(gb, A);
  CYC(b_+102, b_+105); SET_BC(0x0790); // ROOM_SEASONS_790
  CALL_C(b_+105, s_disableLcdAndLoadRoom_body_hook, SYM(disableLcdAndLoadRoom_body), b_+108);
  CYC(b_+108, b_+110); A = 0x0d;
  CALL_C(b_+110, playSound_b00_hook, SYM(playSound_b00), b_+113);
  CALL_C(b_+113, s_clearAllParentItems, SYM(clearAllParentItems), b_+116);
  CALL_C(b_+116, s_dropLinkHeldItem, SYM(dropLinkHeldItem), b_+119);
  CYC(b_+119, b_+121); C = 0x00;
  endgame_din_room(gb); return;

state1:
  CYC(b_+195, b_+198); SET_HL(wccd8);
  CYC(b_+198, b_+200); mem_wr(gb, HL, 0xff);
  CYC(b_+200, b_+203); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+203, b_+204); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+204); return; }
  CYC(b_+204, b_+205);
  CYC(b_+205, b_+208); A = mem_rd(gb, wTmpcfc0 + 0x1f);
  CYC(b_+208, b_+209); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+209); return; }
  CYC(b_+209, b_+210);
  CALL_C(b_+210, s_incCbc2, SYM(incCbc2), b_+213);
  CYC(b_+213, b_+216); SET_BC(0x3d00); // TX_3d00
  CYC(b_+216, b_+219);
  TAIL(showText);

state2:
  CALL_C(b_+219, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+222);
  CALL_C(b_+222, s_incCbc2, SYM(incCbc2), b_+225);
  CYC(b_+225, b_+228);
  TAIL(fastFadeoutToWhite);

state3:
  CYC(b_+228, b_+231); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+231, b_+232); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+232); return; }
  CYC(b_+232, b_+233);
  CALL_C(b_+233, s_incCbc2, SYM(incCbc2), b_+236);
  CYC(b_+236, b_+239); SET_HL(wTmpcbb3);
  CYC(b_+239, b_+241); mem_wr(gb, HL, 0x3c);
  CYC(b_+241, b_+242); L = alu_inc8(gb, L);
  CYC(b_+242, b_+244); mem_wr(gb, HL, 0x00);
  CYC(b_+244, b_+246);
  goto state7Func0;

state4:
  CALL_C(b_+246, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+249);
  if (!(F & FZ)) { RET_TAKEN(b_+249); return; }
  CYC(b_+249, b_+250);
  CALL_C(b_+250, s_incCbc2, SYM(incCbc2), b_+253);
  CYC(b_+253, b_+255); A = 0xc1;
  CALL_C(b_+255, playSound_b00_hook, SYM(playSound_b00), b_+258);
  CYC(b_+258, b_+261);
  TAIL(fadeoutToWhite);

state5:
  CYC(b_+261, b_+264); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+264, b_+265); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+265); return; }
  CYC(b_+265, b_+266);
  CALL_C(b_+266, s_incCbc2, SYM(incCbc2), b_+269);
  CYC(b_+269, b_+271); A = 0x00;
  CALL_C(b_+271, s_seasonsFunc_03_644c_hook, SYM(seasonsFunc_03_644c), b_+274);
  CYC(b_+274, b_+277); SET_HL(wTmpcbb3);
  CYC(b_+277, b_+279); mem_wr(gb, HL, 0x3c);
  CYC(b_+279, b_+282);
  TAIL(fadeinFromWhite);

state6:
  CALL_C(b_+282, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+285);
  if (!(F & FZ)) { RET_TAKEN(b_+285); return; }
  CYC(b_+285, b_+286);
  CALL_C(b_+286, s_incCbc2, SYM(incCbc2), b_+289);
  CYC(b_+289, b_+292);
  TAIL(fastFadeoutToWhite);

state7:
  CYC(b_+292, b_+295); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+295, b_+296); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+296); return; }
  CYC(b_+296, b_+297);
state7Func0:
  CALL_C(b_+297, s_clearDynamicInteractions, SYM(clearDynamicInteractions), b_+300);
  CYC(b_+300, b_+303); SET_HL(wTmpcbb3);
  CYC(b_+303, b_+305); mem_wr(gb, HL, 0x3c);
  CYC(b_+305, b_+306); L = alu_inc8(gb, L);
  CYC(b_+306, b_+307); A = mem_rd(gb, HL);
  CYC(b_+307, b_+310); SET_HL(b_+376); // @state7Table0
  CYC(b_+310, b_+311); endgame_add_double_index(gb, b_+311);
  CYC(b_+311, b_+312); C = mem_rd(gb, HL);
  CYC(b_+312, b_+313); SET_HL(HL + 1);
  CYC(b_+313, b_+314); B = mem_rd(gb, HL);
  CYC(b_+314, b_+316); A = 0x03;
  CALL_C(b_+316, s_disableLcdAndLoadRoom_body_hook, SYM(disableLcdAndLoadRoom_body), b_+319);
  CALL_C(b_+319, s_fastFadeinFromWhite, SYM(fastFadeinFromWhite), b_+322);
  CYC(b_+322, b_+325); SET_HL(wTmpcbb4);
  CYC(b_+325, b_+326); A = mem_rd(gb, HL);
  CYC(b_+326, b_+327); B = A;
  CYC(b_+327, b_+328); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+328, b_+330); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(b_+330, b_+332); goto lastVisit; }
  CYC(b_+330, b_+332);
  CYC(b_+332, b_+334); C = 0x04;
  CYC(b_+334, b_+335); push_effect(gb, BC);
  CYC(b_+335, b_+337); A = 0x02;
  CALL_C(b_+337, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+340);
  CALL_C(b_+340, s_resetCamera, SYM(resetCamera), b_+343);
  CYC(b_+343, b_+344); SET_BC(pop_effect(gb));
  CYC(b_+344, b_+346);
  goto nextState;
lastVisit:
  CYC(b_+346, b_+349); SET_HL(wTmpcbb3);
  CYC(b_+349, b_+351); mem_wr(gb, HL, 0x3c);
  CYC(b_+351, b_+352); push_effect(gb, BC);
  CYC(b_+352, b_+354); C = 0x01;
  CALL_L(b_+354, endgame_din_room, b_+357);
  CYC(b_+357, b_+358); SET_BC(pop_effect(gb));
  CYC(b_+358, b_+360); C = 0x08;
  CALL_C(b_+360, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+363);
  CYC(b_+363, b_+365); B = 0xff;
  if (F & FZ) { CYCT(b_+365, b_+367); goto nextState; }
  CYC(b_+365, b_+367);
  CYC(b_+367, b_+369); C = 0x0d;
nextState:
  CYC(b_+369, b_+372); SET_HL(wTmpcbc2);
  CYC(b_+372, b_+373); mem_wr(gb, HL, C);
  CYC(b_+373, b_+376);
  s_seasonsFunc_03_6405_hook(gb); return;

state8:
  CYC(b_+386, b_+388); E = 0x3c;
  CYC(b_+388, b_+391); SET_BC(0x3d01); // TX_3d01
  CYC(b_+391, b_+393);
  endgame_text_after_fade(gb); return;

state9:
  CALL_C(b_+393, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+396);
  if (!(F & FZ)) { RET_TAKEN(b_+396); return; }
  CYC(b_+396, b_+397);
  CALL_C(b_+397, s_incCbc2, SYM(incCbc2), b_+400);
  CYC(b_+400, b_+403);
  TAIL(fadeoutToWhite);

stateA:
  CYC(b_+403, b_+406); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+406, b_+407); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+407); return; }
  CYC(b_+407, b_+408);
  CALL_C(b_+408, s_incCbc2, SYM(incCbc2), b_+411);
  CYC(b_+411, b_+414); SET_HL(wTmpcbb3);
  CYC(b_+414, b_+416); mem_wr(gb, HL, 0x3c);
  CYC(b_+416, b_+418); A = 0xff;
  CYC(b_+418, b_+421); mem_wr(gb, wTilesetAnimation, A);
  CALL_C(b_+421, s_disableLcd, SYM(disableLcd), b_+424);
  CYC(b_+424, b_+426); A = 0x2b;
  CALL_C(b_+426, s_loadGfxHeader, SYM(loadGfxHeader), b_+429);
  CYC(b_+429, b_+431); A = 0x9d; // PALH_9d
  CALL_C(b_+431, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+434);
  CALL_C(b_+434, s_cutscene_clearObjects, SYM(cutscene_clearObjects), b_+437);
  CALL_L(b_+437, endgame_draw_din_sprites, b_+440);
  CYC(b_+440, b_+442); A = 0x04;
  CALL_C(b_+442, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+445);
  CYC(b_+445, b_+448);
  TAIL(fadeinFromWhite);

stateB:
  CALL_L(b_+448, endgame_draw_din_sprites, b_+451);
  CALL_C(b_+451, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+454);
  if (!(F & FZ)) { RET_TAKEN(b_+454); return; }
  CYC(b_+454, b_+455);
  CALL_C(b_+455, s_incCbc2, SYM(incCbc2), b_+458);
  CYC(b_+458, b_+461); SET_HL(wMenuDisabled);
  CYC(b_+461, b_+463); mem_wr(gb, HL, 0x01);
  CYC(b_+463, b_+466); SET_HL(wTmpcbb3);
  CYC(b_+466, b_+468); mem_wr(gb, HL, 0x3c);
  CYC(b_+468, b_+471); SET_BC(0x3d02); // TX_3d02
  endgame_text_flags1(gb); return;

stateC:
  CALL_L(b_+479, endgame_draw_din_sprites, b_+482);
  CALL_C(b_+482, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+485);
  if (!(F & FZ)) { RET_TAKEN(b_+485); return; }
  CYC(b_+485, b_+486);
  CALL_C(b_+486, s_seasonsFunc_03_646a_hook, SYM(seasonsFunc_03_646a), b_+489);
  CYC(b_+489, b_+491); A = 0x01;
  CYC(b_+491, b_+494); mem_wr(gb, wTmpcbc1, A);
  CALL_C(b_+494, s_disableActiveRing, SYM(disableActiveRing), b_+497);
  CYC(b_+497, b_+500);
  TAIL(fadeoutToWhite);

stateD:
  CYC(b_+500, b_+502); E = 0x3c;
  CYC(b_+502, b_+505); SET_BC(0x4f00); // TX_4f00
  endgame_text_after_fade(gb); return;

stateE:
  CALL_C(b_+519, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+522);
  if (!(F & FZ)) { RET_TAKEN(b_+522); return; }
  CYC(b_+522, b_+523);
  CYC(b_+523, b_+524); alu_xor(gb, A);
  CYC(b_+524, b_+527); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+527, b_+528); A = alu_dec8(gb, A);
  CYC(b_+528, b_+531); mem_wr(gb, wTmpcbba, A);
  CYC(b_+531, b_+533); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+533, playSound_b00_hook, SYM(playSound_b00), b_+536);
  CYC(b_+536, b_+538); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+538, playSound_b00_hook, SYM(playSound_b00), b_+541);
  CYC(b_+541, b_+544);
  TAIL(incCbc2);

stateF:
  CYC(b_+544, b_+547); SET_HL(wTmpcbb3);
  CYC(b_+547, b_+549); B = 0x02;
  CALL_C(b_+549, s_flashScreen, SYM(flashScreen), b_+552);
  if (F & FZ) { RET_TAKEN(b_+552); return; }
  CYC(b_+552, b_+553);
  CALL_C(b_+553, s_incCbc2, SYM(incCbc2), b_+556);
  CYC(b_+556, b_+557); alu_xor(gb, A);
  CYC(b_+557, b_+560); SET_BC(0x059a); // ROOM_SEASONS_59a
  CALL_C(b_+560, s_disableLcdAndLoadRoom_body_hook, SYM(disableLcdAndLoadRoom_body), b_+563);
  CYC(b_+563, b_+565); A = 0xac; // PALH_ac
  CALL_C(b_+565, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+568);
  CALL_C(b_+568, s_hideStatusBar, SYM(hideStatusBar), b_+571);
  CALL_C(b_+571, s_clearFadingPalettes2, SYM(clearFadingPalettes2), b_+574);
  CYC(b_+574, b_+576); B = 0x06;
  for (;;) {
    CALL_C(b_+576, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+579);
    if (!(F & FZ)) { CYCT(b_+579, b_+581); break; }
    CYC(b_+579, b_+581);
    CYC(b_+581, b_+583); mem_wr(gb, HL, 0xb0); // INTERAC_TWINROVA_FLAME
    CYC(b_+583, b_+584); L = alu_inc8(gb, L);
    CYC(b_+584, b_+585); B = alu_dec8(gb, B);
    CYC(b_+585, b_+586); mem_wr(gb, HL, B);
    if (!(F & FZ)) { CYCT(b_+586, b_+588); continue; }
    CYC(b_+586, b_+588);
    break;
  }
  CYC(b_+588, b_+591); SET_HL(wTmpcbb3);
  CYC(b_+591, b_+593); mem_wr(gb, HL, 0x1e);
  CYC(b_+593, b_+595); A = 0x13;
  CALL_C(b_+595, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+598);
  CYC(b_+598, b_+601); SET_HL(wGfxRegs1_SCY);
  CYC(b_+601, b_+602); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+602, b_+604); mem_wr(gb, hCameraY, A);
  CYC(b_+604, b_+605); A = mem_rd(gb, HL);
  CYC(b_+605, b_+607); mem_wr(gb, hCameraX, A);
  CYC(b_+607, b_+609); A = 0x00;
  CYC(b_+609, b_+612); mem_wr(gb, wScrollMode, A);
  RET(b_+612); return;

state10:
  CALL_C(b_+613, s_decCbb3, SYM(decCbb3), b_+616);
  if (!(F & FZ)) { RET_TAKEN(b_+616); return; }
  CYC(b_+616, b_+617);
  CALL_C(b_+617, s_incCbc2, SYM(incCbc2), b_+620);
  CYC(b_+620, b_+623); SET_HL(wTmpcbb3);
  CYC(b_+623, b_+625); mem_wr(gb, HL, 0x28);
  CYC(b_+625, b_+627); A = 0x04;
  CYC(b_+627, b_+630); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+630, b_+633); SET_BC(0x4f01); // TX_4f01
  CYC(b_+633, b_+636);
  TAIL(showText);

state11:
  CALL_C(b_+636, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+639);
  if (!(F & FZ)) { RET_TAKEN(b_+639); return; }
  CYC(b_+639, b_+640);
  CALL_C(b_+640, s_incCbc2, SYM(incCbc2), b_+643);
  CYC(b_+643, b_+645); A = 0x20;
  CYC(b_+645, b_+648); SET_HL(wTmpcbb3);
  CYC(b_+648, b_+649); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+649, b_+650); alu_xor(gb, A);
  CYC(b_+650, b_+651); mem_wr(gb, HL, A);
  RET(b_+651); return;

state12:
  CALL_C(b_+652, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+655);
  if (!(F & FZ)) { RET_TAKEN(b_+655); return; }
  CYC(b_+655, b_+656);
  CYC(b_+656, b_+659); SET_HL(wTmpcbb3);
  CYC(b_+659, b_+661); mem_wr(gb, HL, 0x20);
  CYC(b_+661, b_+662); SET_HL(HL + 1);
  CYC(b_+662, b_+663); A = mem_rd(gb, HL);
  CYC(b_+663, b_+665); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+665, b_+667); goto torchStep; }
  CYC(b_+665, b_+667);
  CYC(b_+667, b_+668); B = A;
  CYC(b_+668, b_+669); push_effect(gb, HL);
  CYC(b_+669, b_+671); A = 0x72; // SND_LIGHTTORCH
  CALL_C(b_+671, playSound_b00_hook, SYM(playSound_b00), b_+674);
  CYC(b_+674, b_+675); SET_HL(pop_effect(gb));
  CYC(b_+675, b_+676); A = B;
torchStep:
  CYC(b_+676, b_+677); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+677, b_+680); SET_HL(b_+728); // @state12Table0
  CYC(b_+680, b_+681); endgame_add_a_to_hl(gb, b_+681);
  CYC(b_+681, b_+682); A = mem_rd(gb, HL);
  CYC(b_+682, b_+683); alu_or(gb, A);
  CYC(b_+683, b_+684); B = A;
  if (!(F & FZ)) { CYCT(b_+684, b_+686); goto fadeInSome; }
  CYC(b_+684, b_+686);
  CALL_C(b_+686, s_fadeinFromBlack, SYM(fadeinFromBlack), b_+689);
  CYC(b_+689, b_+691); A = 0x01;
  CYC(b_+691, b_+694); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+694, b_+697); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+697, b_+700); SET_HL(wTmpcbb3);
  CYC(b_+700, b_+702); mem_wr(gb, HL, 0x3c);
  CYC(b_+702, b_+704); A = 0x21; // MUS_DISASTER
  CALL_C(b_+704, playSound_b00_hook, SYM(playSound_b00), b_+707);
  CYC(b_+707, b_+710);
  TAIL(incCbc2);
fadeInSome:
  CALL_C(b_+710, s_fastFadeinFromBlack, SYM(fastFadeinFromBlack), b_+713);
  CYC(b_+713, b_+714); A = B;
  CYC(b_+714, b_+717); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+717, b_+720); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+720, b_+721); alu_xor(gb, A);
  CYC(b_+721, b_+724); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+724, b_+727); mem_wr(gb, wFadeBgPaletteSources, A);
  RET(b_+727); return;

state13:
  CYC(b_+734, b_+736); E = 0x28;
  CYC(b_+736, b_+739); SET_BC(0x4f02); // TX_4f02
  CALL_L(b_+739, endgame_textbox_style, b_+742);
  CYC(b_+742, b_+745);
  endgame_text_after_fade(gb); return;

state14:
  CYC(b_+756, b_+758); E = 0x28;
  CYC(b_+758, b_+761); SET_BC(0x4f03); // TX_4f03
textAfterText:
  CALL_C(b_+761, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+764);
  if (!(F & FZ)) { RET_TAKEN(b_+764); return; }
  CYC(b_+764, b_+765);
  CALL_C(b_+765, s_incCbc2, SYM(incCbc2), b_+768);
  CYC(b_+768, b_+771); SET_HL(wTmpcbb3);
  CYC(b_+771, b_+772); mem_wr(gb, HL, E);
  CALL_L(b_+772, endgame_textbox_style, b_+775);
  CYC(b_+775, b_+778);
  TAIL(showText);

state15:
  CYC(b_+778, b_+780); E = 0x3c;
  CYC(b_+780, b_+783); SET_BC(0x4f04); // TX_4f04
  CYC(b_+783, b_+785);
  goto textAfterText;

state16:
  CYC(b_+785, b_+787); E = 0xb4;
  endgame_after_text(gb); return;

state17:
  CYC(b_+799, b_+802); SET_HL(wGfxRegs1_SCY);
  CYC(b_+802, b_+804); A = mem_rd(gb, hCameraY);
  CYC(b_+804, b_+805); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+805, b_+807); A = mem_rd(gb, hCameraX);
  CYC(b_+807, b_+808); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+808, b_+811); SET_HL(b_+845); // @state17Table0
  CYC(b_+811, b_+814); SET_DE(wGfxRegs1_SCY);
  CALL_C(b_+814, s_seasonsFunc_03_79cd_hook, SYM(seasonsFunc_03_79cd), b_+817);
  CYC(b_+817, b_+818); SET_DE(DE + 1);
  CALL_C(b_+818, s_seasonsFunc_03_79cd_hook, SYM(seasonsFunc_03_79cd), b_+821);
  CALL_C(b_+821, s_seasonsFunc_03_5d00_hook, SYM(seasonsFunc_03_5d00), b_+824);
  CALL_C(b_+824, s_decCbb3, SYM(decCbb3), b_+827);
  if (!(F & FZ)) { RET_TAKEN(b_+827); return; }
  CYC(b_+827, b_+828);
  CYC(b_+828, b_+829); A = alu_dec8(gb, A);
  CYC(b_+829, b_+832); mem_wr(gb, wTmpcbba, A);
  CYC(b_+832, b_+834); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+834, playSound_b00_hook, SYM(playSound_b00), b_+837);
  CYC(b_+837, b_+839); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+839, playSound_b00_hook, SYM(playSound_b00), b_+842);
  CYC(b_+842, b_+845);
  TAIL(incCbc2);

state18:
  CYC(b_+853, b_+856); SET_HL(wTmpcbb3);
  CYC(b_+856, b_+858); B = 0x01;
  CALL_C(b_+858, s_flashScreen, SYM(flashScreen), b_+861);
  if (F & FZ) { RET_TAKEN(b_+861); return; }
  CYC(b_+861, b_+862);
  CALL_C(b_+862, s_incCbc2, SYM(incCbc2), b_+865);
  CYC(b_+865, b_+868); SET_HL(wTmpcbb3);
  CYC(b_+868, b_+870); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+870, s_clearDynamicInteractions, SYM(clearDynamicInteractions), b_+873);
  CALL_C(b_+873, s_clearOam, SYM(clearOam), b_+876);
  CALL_C(b_+876, s_showStatusBar, SYM(showStatusBar), b_+879);
  CYC(b_+879, b_+880); alu_xor(gb, A);
  CYC(b_+880, b_+883); SET_BC(0x0790); // ROOM_SEASONS_790
  CALL_C(b_+883, s_disableLcdAndLoadRoom_body_hook, SYM(disableLcdAndLoadRoom_body), b_+886);
  CYC(b_+886, b_+888); C = 0x01;
  CYC(b_+888, b_+891);
  endgame_din_room(gb); return;

state19:
  CALL_C(b_+891, s_decCbb3, SYM(decCbb3), b_+894);
  if (!(F & FZ)) { RET_TAKEN(b_+894); return; }
  CYC(b_+894, b_+895);
  CALL_C(b_+895, s_incCbc2, SYM(incCbc2), b_+898);
  CYC(b_+898, b_+901); SET_HL(wTmpcbb3);
  CYC(b_+901, b_+903); mem_wr(gb, HL, 0x1e);
  CYC(b_+903, b_+906); SET_BC(0x3d17); // TX_3d17
  CYC(b_+906, b_+909);
  TAIL(showText);

state1A:
  CALL_C(b_+909, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+912);
  if (!(F & FZ)) { RET_TAKEN(b_+912); return; }
  CYC(b_+912, b_+913);
  CALL_C(b_+913, s_incCbc2, SYM(incCbc2), b_+916);
  CYC(b_+916, b_+919); SET_HL(wTmpcbb3);
  CYC(b_+919, b_+921); mem_wr(gb, HL, 0x1e);
  CYC(b_+921, b_+924); SET_BC(0x4f09); // TX_4f09
  CYC(b_+924, b_+927);
  TAIL(showText);

state1B:
  CALL_C(b_+927, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+930);
  if (!(F & FZ)) { RET_TAKEN(b_+930); return; }
  CYC(b_+930, b_+931);
  CALL_C(b_+931, s_incCbc2, SYM(incCbc2), b_+934);
  CYC(b_+934, b_+936); C = 0x40;
  CYC(b_+936, b_+938); A = 0x29;
  CALL_C(b_+938, s_giveTreasure, SYM(giveTreasure), b_+941);
  CYC(b_+941, b_+943); A = 0x08;
  CALL_C(b_+943, s_setLinkIDOverride, SYM(setLinkIDOverride), b_+946);
  CYC(b_+946, b_+948); L = 0x02; // w1Link.subid
  CYC(b_+948, b_+950); mem_wr(gb, HL, 0x07);
  CYC(b_+950, b_+953); SET_HL(wTmpcbb3);
  CYC(b_+953, b_+955); mem_wr(gb, HL, 0x5a);
  CYC(b_+955, b_+957); A = 0x4a;
  CYC(b_+957, b_+960);
  TAIL(playSound_b00);

state1C:
  CALL_C(b_+960, s_decCbb3, SYM(decCbb3), b_+963);
  if (!(F & FZ)) { RET_TAKEN(b_+963); return; }
  CYC(b_+963, b_+964);
  CALL_C(b_+964, s_incCbc2, SYM(incCbc2), b_+967);
  CYC(b_+967, b_+970); SET_HL(wTmpcbb3);
  CYC(b_+970, b_+972); mem_wr(gb, HL, 0xb4);
  CYC(b_+972, b_+975); SET_BC(0x90bd);
  CYC(b_+975, b_+977); A = 0xff;
  CYC(b_+977, b_+980);
  TAIL(createEnergySwirlGoingOut);

state1D:
  CALL_C(b_+980, s_decCbb3, SYM(decCbb3), b_+983);
  if (!(F & FZ)) { RET_TAKEN(b_+983); return; }
  CYC(b_+983, b_+984);
  CALL_C(b_+984, s_incCbc2, SYM(incCbc2), b_+987);
  CYC(b_+987, b_+990); SET_HL(wTmpcbb3);
  CYC(b_+990, b_+992); mem_wr(gb, HL, 0x3c);
  CYC(b_+992, b_+995);
  TAIL(fadeoutToWhite);

state1E:
  CALL_C(b_+995, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+998);
  if (!(F & FZ)) { RET_TAKEN(b_+998); return; }
  CYC(b_+998, b_+999);
  CALL_C(b_+999, s_incCbc2, SYM(incCbc2), b_+1002);
  CALL_C(b_+1002, s_disableLcd, SYM(disableLcd), b_+1005);
  CALL_C(b_+1005, s_clearOam, SYM(clearOam), b_+1008);
  CALL_C(b_+1008, s_clearDynamicInteractions, SYM(clearDynamicInteractions), b_+1011);
  CALL_C(b_+1011, s_refreshObjectGfx, SYM(refreshObjectGfx), b_+1014);
  CALL_C(b_+1014, s_hideStatusBar, SYM(hideStatusBar), b_+1017);
  CYC(b_+1017, b_+1019); A = 0x02;
  CYC(b_+1019, b_+1021); mem_wr(gb, IO_SVBK, A);
  CYC(b_+1021, b_+1024); SET_HL(w2TilesetBgPalettes + 0x10);
  CYC(b_+1024, b_+1026); B = 0x08;
  CYC(b_+1026, b_+1028); A = 0xff;
  CALL_C(b_+1028, s_fillMemory, SYM(fillMemory), b_+1031);
  CYC(b_+1031, b_+1032); alu_xor(gb, A);
  CYC(b_+1032, b_+1034); mem_wr(gb, IO_SVBK, A);
  CYC(b_+1034, b_+1036); A = 0x07;
  CYC(b_+1036, b_+1038); mem_wr(gb, hDirtyBgPalettes, A);
  CYC(b_+1038, b_+1040); B = 0x02;
  for (;;) {
    CALL_C(b_+1040, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+1043);
    if (!(F & FZ)) { CYCT(b_+1043, b_+1045); break; }
    CYC(b_+1043, b_+1045);
    CYC(b_+1045, b_+1047); mem_wr(gb, HL, 0xb0); // INTERAC_TWINROVA_FLAME
    CYC(b_+1047, b_+1048); L = alu_inc8(gb, L);
    CYC(b_+1048, b_+1050); A = 0x05;
    CYC(b_+1050, b_+1051); alu_add(gb, B);
    CYC(b_+1051, b_+1052); mem_wr(gb, HL, A);
    CYC(b_+1052, b_+1053); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+1053, b_+1055); continue; }
    CYC(b_+1053, b_+1055);
    break;
  }
  CYC(b_+1055, b_+1057); A = 0x02;
  CYC(b_+1057, b_+1060); mem_wr(gb, wOpenedMenuType, A);
  CALL_C(b_+1060, s_seasonsFunc_03_7a6b_hook, SYM(seasonsFunc_03_7a6b), b_+1063);
  CYC(b_+1063, b_+1065); A = 0x02;
  CALL_C(b_+1065, s_seasonsFunc_03_7a88_hook, SYM(seasonsFunc_03_7a88), b_+1068);
  CYC(b_+1068, b_+1071); SET_HL(wTmpcbb3);
  CYC(b_+1071, b_+1073); mem_wr(gb, HL, 0x1e);
  CYC(b_+1073, b_+1075); A = 0x04;
  CALL_C(b_+1075, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+1078);
  CYC(b_+1078, b_+1080); A = 0x10;
  CYC(b_+1080, b_+1082); mem_wr(gb, hCameraY, A);
  CYC(b_+1082, b_+1085); mem_wr(gb, wDeleteEnergyBeads, A);
  CYC(b_+1085, b_+1086); alu_xor(gb, A);
  CYC(b_+1086, b_+1088); mem_wr(gb, hCameraX, A);
  CYC(b_+1088, b_+1090); A = 0x00;
  CYC(b_+1090, b_+1093); mem_wr(gb, wScrollMode, A);
  CYC(b_+1093, b_+1096); SET_BC(0x4f05); // TX_4f05
  CYC(b_+1096, b_+1099);
  TAIL(showText);

state1F:
  CALL_C(b_+1099, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+1102);
  if (!(F & FZ)) { RET_TAKEN(b_+1102); return; }
  CYC(b_+1102, b_+1103);
  CALL_C(b_+1103, s_incCbc2, SYM(incCbc2), b_+1106);
  CYC(b_+1106, b_+1108); B = 0x02;
state1FFunc0:
  CALL_C(b_+1108, s_fadeinFromWhite, SYM(fadeinFromWhite), b_+1111);
  CYC(b_+1111, b_+1112); A = B;
  CYC(b_+1112, b_+1115); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+1115, b_+1118); mem_wr(gb, wFadeSprPaletteSources, A);
  CYC(b_+1118, b_+1119); alu_xor(gb, A);
  CYC(b_+1119, b_+1122); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+1122, b_+1125); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+1125, b_+1128); SET_HL(wTmpcbb3);
  CYC(b_+1128, b_+1130); mem_wr(gb, HL, 0x3c);
  RET(b_+1130); return;

state20:
  CYC(b_+1131, b_+1133); E = 0x1e;
  CYC(b_+1133, b_+1136); SET_BC(0x4f06); // TX_4f06
  CYC(b_+1136, b_+1139);
  endgame_text_after_fade(gb); return;

state21:
  CALL_C(b_+1139, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+1142);
  if (!(F & FZ)) { RET_TAKEN(b_+1142); return; }
  CYC(b_+1142, b_+1143);
  CALL_C(b_+1143, s_incCbc2, SYM(incCbc2), b_+1146);
  CYC(b_+1146, b_+1148); B = 0x14;
  CYC(b_+1148, b_+1151);
  goto state1FFunc0;

state22:
  CYC(b_+1151, b_+1153); E = 0x1e;
  CYC(b_+1153, b_+1156); SET_BC(0x4f07); // TX_4f07
  CYC(b_+1156, b_+1159);
  endgame_text_after_fade(gb); return;

state23:
  CYC(b_+1159, b_+1161); E = 0x3c;
  CYC(b_+1161, b_+1164);
  endgame_after_text(gb); return;

state24:
  CALL_C(b_+1164, s_decCbb3, SYM(decCbb3), b_+1167);
  if (!(F & FZ)) { RET_TAKEN(b_+1167); return; }
  CYC(b_+1167, b_+1168);
  CALL_C(b_+1168, s_incCbc2, SYM(incCbc2), b_+1171);
  CYC(b_+1171, b_+1174); SET_HL(wTmpcbb3);
  CYC(b_+1174, b_+1176); mem_wr(gb, HL, 0xf0);
  CYC(b_+1176, b_+1178); A = 0xff;
  CYC(b_+1178, b_+1181); SET_BC(0x4850);
  CYC(b_+1181, b_+1184);
  TAIL(createEnergySwirlGoingOut);

state25:
  CALL_C(b_+1184, s_decCbb3, SYM(decCbb3), b_+1187);
  if (!(F & FZ)) { RET_TAKEN(b_+1187); return; }
  CYC(b_+1187, b_+1188);
  CYC(b_+1188, b_+1191); SET_HL(wTmpcbb3);
  CYC(b_+1191, b_+1193); mem_wr(gb, HL, 0x5a);
  CALL_C(b_+1193, s_fadeoutToWhite, SYM(fadeoutToWhite), b_+1196);
  CYC(b_+1196, b_+1198); A = 0xfc;
  CYC(b_+1198, b_+1201); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+1201, b_+1204); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+1204, b_+1207);
  TAIL(incCbc2);

state26:
  CALL_C(b_+1207, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+1210);
  if (!(F & FZ)) { RET_TAKEN(b_+1210); return; }
  CYC(b_+1210, b_+1211);
  CALL_C(b_+1211, s_incCbc2, SYM(incCbc2), b_+1214);
  CALL_C(b_+1214, s_clearDynamicInteractions, SYM(clearDynamicInteractions), b_+1217);
  CALL_C(b_+1217, s_clearParts, SYM(clearParts), b_+1220);
  CALL_C(b_+1220, s_clearOam, SYM(clearOam), b_+1223);
  CYC(b_+1223, b_+1226); SET_HL(wTmpcbb3);
  CYC(b_+1226, b_+1228); mem_wr(gb, HL, 0x3c);
  CYC(b_+1228, b_+1231); SET_BC(0x4f08); // TX_4f08
  CYC(b_+1231, b_+1234);
  TAIL(showTextNonExitable);

state27:
  CYC(b_+1234, b_+1237); A = mem_rd(gb, wTextIsActive);
  CYC(b_+1237, b_+1238); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+1238); return; }
  CYC(b_+1238, b_+1239);
  CALL_C(b_+1239, s_decCbb3, SYM(decCbb3), b_+1242);
  if (!(F & FZ)) { RET_TAKEN(b_+1242); return; }
  CYC(b_+1242, b_+1243);
  CALL_C(b_+1243, s_showStatusBar, SYM(showStatusBar), b_+1246);
  CYC(b_+1246, b_+1247); alu_xor(gb, A);
  CYC(b_+1247, b_+1250); mem_wr(gb, wOpenedMenuType, A);
  CYC(b_+1250, b_+1251); A = alu_dec8(gb, A);
  CYC(b_+1251, b_+1254); mem_wr(gb, wActiveMusic, A);
  CYC(b_+1254, b_+1256); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+1256, playSound_b00_hook, SYM(playSound_b00), b_+1259);
  CYC(b_+1259, b_+1262); SET_HL(wWarpDestGroup);
  CYC(b_+1262, b_+1264); A = 0x85;
  CYC(b_+1264, b_+1265); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+1265, b_+1267); A = 0x9d;
  CYC(b_+1267, b_+1268); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+1268, b_+1270); A = 0x0f;
  CYC(b_+1270, b_+1271); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+1271, b_+1273); A = 0x57;
  CYC(b_+1273, b_+1274); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+1274, b_+1276); mem_wr(gb, HL, 0x03);
  RET(b_+1276); return;
}

// The credits: Din's sprites and text, the crystal and seed sprites scrolling, the flash, the
// final picture, then the fade to black.
void s_endgameCutsceneHandler_09_stage1_body_hook(GB *gb) {
  BASE(endgameCutsceneHandler_09_stage1_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wTmpcbc2);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+25) { goto state0; }
    else if (jt_ == b_+70) { goto state1; }
    else if (jt_ == b_+83) { goto state2; }
    else if (jt_ == b_+133) { goto state3; }
    else if (jt_ == b_+155) { goto state4; }
    else if (jt_ == b_+172) { goto state5; }
    else if (jt_ == b_+200) { goto state6; }
    else if (jt_ == b_+221) { goto state7; }
    else if (jt_ == b_+244) { goto state8; }
    else if (jt_ == b_+330) { goto state9; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_L(b_+25, endgame_draw_din_sprites, b_+28);
  CYC(b_+28, b_+31); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CALL_C(b_+33, s_incCbc2, SYM(incCbc2), b_+36);
  CYC(b_+36, b_+39); SET_HL(wTmpcbb3);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+41, s_disableLcd, SYM(disableLcd), b_+44);
  CALL_C(b_+44, s_clearOam, SYM(clearOam), b_+47);
  CYC(b_+47, b_+49); A = 0x2c;
  CALL_C(b_+49, s_loadGfxHeader, SYM(loadGfxHeader), b_+52);
  CYC(b_+52, b_+54); A = 0x9e; // PALH_9e
  CALL_C(b_+54, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+57);
  CYC(b_+57, b_+59); A = 0x04;
  CALL_C(b_+59, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+62);
  CYC(b_+62, b_+64); A = 0x21; // MUS_DISASTER
  CALL_C(b_+64, playSound_b00_hook, SYM(playSound_b00), b_+67);
  CYC(b_+67, b_+70);
  TAIL(fadeinFromWhite);

state1:
  CYC(b_+70, b_+72); A = 0x01;
  CYC(b_+72, b_+75); mem_wr(gb, wTextboxFlags, A);
  CYC(b_+75, b_+77); A = 0x3c;
  CYC(b_+77, b_+80); SET_BC(0x3d03); // TX_3d03
  CYC(b_+80, b_+83);
  endgame_text_after_fade(gb); return;

state2:
  CALL_C(b_+83, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+86);
  if (!(F & FZ)) { RET_TAKEN(b_+86); return; }
  CYC(b_+86, b_+87);
  CALL_C(b_+87, s_incCbc2, SYM(incCbc2), b_+90);
  CYC(b_+90, b_+93); SET_HL(wTmpcbb5);
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0xd0);
crystalSprites:
  CYC(b_+95, b_+98); SET_HL(SYM(seasonsOamData_03_6472));
  endgame_scrolled_sprites(gb); return;

state3:
  CYC(b_+133, b_+136); SET_HL(wTmpcbb5);
  CYC(b_+136, b_+137); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+137, b_+139); goto crystalSprites; }
  CYC(b_+137, b_+139);
  CALL_C(b_+139, s_clearOam, SYM(clearOam), b_+142);
  CYC(b_+142, b_+144); A = 0x0a; // UNCMP_GFXH_0a
  CALL_C(b_+144, s_loadUncompressedGfxHeader, SYM(loadUncompressedGfxHeader), b_+147);
  CYC(b_+147, b_+150); SET_HL(wTmpcbb3);
  CYC(b_+150, b_+152); mem_wr(gb, HL, 0x1e);
  CYC(b_+152, b_+155);
  TAIL(incCbc2);

state4:
  CALL_C(b_+155, s_decCbb3, SYM(decCbb3), b_+158);
  if (!(F & FZ)) { RET_TAKEN(b_+158); return; }
  CYC(b_+158, b_+159);
  CALL_C(b_+159, s_incCbc2, SYM(incCbc2), b_+162);
  CYC(b_+162, b_+165); SET_HL(wTmpcbb5);
  CYC(b_+165, b_+167); mem_wr(gb, HL, 0xd0);
  endgame_draw_seed_sprites(gb); return;

state5:
  CALL_L(b_+172, endgame_draw_seed_sprites, b_+175);
  CYC(b_+175, b_+178); SET_HL(wTmpcbb5);
  CYC(b_+178, b_+179); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+179, b_+180); A = mem_rd(gb, HL);
  CYC(b_+180, b_+182); alu_sub(gb, 0xa0);
  if (!(F & FZ)) { RET_TAKEN(b_+182); return; }
  CYC(b_+182, b_+183);
  CYC(b_+183, b_+186); mem_wr(gb, wScreenOffsetY, A);
  CYC(b_+186, b_+189); mem_wr(gb, wScreenOffsetX, A);
  CYC(b_+189, b_+191); A = 0x1e;
  CYC(b_+191, b_+194); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+194, b_+197); mem_wr(gb, wOpenedMenuType, A);
  CYC(b_+197, b_+200);
  TAIL(incCbc2);

state6:
  CALL_L(b_+200, endgame_draw_seed_sprites, b_+203);
  CALL_C(b_+203, s_decCbb3, SYM(decCbb3), b_+206);
  if (!(F & FZ)) { RET_TAKEN(b_+206); return; }
  CYC(b_+206, b_+207);
  CYC(b_+207, b_+210); SET_HL(wTmpcbb3);
  CYC(b_+210, b_+212); mem_wr(gb, HL, 0x14);
  CYC(b_+212, b_+215); SET_BC(0x3d04); // TX_3d04
  CALL_L(b_+215, endgame_text_flags1, b_+218);
  CYC(b_+218, b_+221);
  TAIL(incCbc2);

state7:
  CALL_L(b_+221, endgame_draw_seed_sprites, b_+224);
  CALL_C(b_+224, s_seasonsFunc_03_645a_hook, SYM(seasonsFunc_03_645a), b_+227);
  if (!(F & FZ)) { RET_TAKEN(b_+227); return; }
  CYC(b_+227, b_+228);
  CYC(b_+228, b_+229); alu_xor(gb, A);
  CYC(b_+229, b_+232); mem_wr(gb, wOpenedMenuType, A);
  CYC(b_+232, b_+233); A = alu_dec8(gb, A);
  CYC(b_+233, b_+236); mem_wr(gb, wTmpcbba, A);
  CYC(b_+236, b_+238); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+238, playSound_b00_hook, SYM(playSound_b00), b_+241);
  CYC(b_+241, b_+244);
  TAIL(incCbc2);

state8:
  CALL_L(b_+244, endgame_draw_seed_sprites, b_+247);
  CYC(b_+247, b_+250); SET_HL(wTmpcbb3);
  CYC(b_+250, b_+252); B = 0x02;
  CALL_C(b_+252, s_flashScreen, SYM(flashScreen), b_+255);
  if (F & FZ) { RET_TAKEN(b_+255); return; }
  CYC(b_+255, b_+256);
  CALL_C(b_+256, s_incCbc2, SYM(incCbc2), b_+259);
  CYC(b_+259, b_+262); SET_HL(wTmpcbb3);
  CYC(b_+262, b_+264); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+264, s_disableLcd, SYM(disableLcd), b_+267);
  CALL_C(b_+267, s_clearOam, SYM(clearOam), b_+270);
  CYC(b_+270, b_+271); alu_xor(gb, A);
  CYC(b_+271, b_+273); mem_wr(gb, IO_VBK, A);
  CYC(b_+273, b_+276); SET_HL(0x8000);
  CYC(b_+276, b_+279); SET_BC(0x2000);
  CALL_C(b_+279, s_clearMemoryBc, SYM(clearMemoryBc), b_+282);
  CYC(b_+282, b_+283); alu_xor(gb, A);
  CYC(b_+283, b_+285); mem_wr(gb, IO_VBK, A);
  CYC(b_+285, b_+288); SET_HL(0x9c00);
  CYC(b_+288, b_+291); SET_BC(0x0400);
  CALL_C(b_+291, s_clearMemoryBc, SYM(clearMemoryBc), b_+294);
  CYC(b_+294, b_+296); A = 0x01;
  CYC(b_+296, b_+298); mem_wr(gb, IO_VBK, A);
  CYC(b_+298, b_+301); SET_HL(0x9c00);
  CYC(b_+301, b_+304); SET_BC(0x0400);
  CALL_C(b_+304, s_clearMemoryBc, SYM(clearMemoryBc), b_+307);
  CYC(b_+307, b_+309); A = 0x2d;
  CALL_C(b_+309, s_loadGfxHeader, SYM(loadGfxHeader), b_+312);
  CYC(b_+312, b_+314); A = 0x9c; // PALH_9c
  CALL_C(b_+314, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+317);
  CYC(b_+317, b_+319); A = 0x04;
  CALL_C(b_+319, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+322);
  CYC(b_+322, b_+324); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+324, playSound_b00_hook, SYM(playSound_b00), b_+327);
  CYC(b_+327, b_+330);
  TAIL(clearPaletteFadeVariablesAndRefreshPalettes);

state9:
  CALL_C(b_+330, s_decCbb3, SYM(decCbb3), b_+333);
  if (!(F & FZ)) { RET_TAKEN(b_+333); return; }
  CYC(b_+333, b_+334);
  CYC(b_+334, b_+336); A = 0x0a;
  CYC(b_+336, b_+339); mem_wr(gb, wThreadStateBuffer + 0x0f, A);
  CALL_C(b_+339, s_seasonsFunc_03_646a_hook, SYM(seasonsFunc_03_646a), b_+342);
  CYC(b_+342, b_+345); SET_HL(wRoomLayout);
  CYC(b_+345, b_+348); SET_BC(0x00c0);
  CALL_C(b_+348, s_clearMemoryBc, SYM(clearMemoryBc), b_+351);
  CYC(b_+351, b_+354); SET_HL(wRoomCollisions);
  CYC(b_+354, b_+357); SET_BC(0x00c0);
  CALL_C(b_+357, s_clearMemoryBc, SYM(clearMemoryBc), b_+360);
  CYC(b_+360, b_+362); mem_wr(gb, hCameraY, A);
  CYC(b_+362, b_+364); mem_wr(gb, hCameraX, A);
  CYC(b_+364, b_+367); SET_HL(wTmpcbb3);
  CYC(b_+367, b_+369); mem_wr(gb, HL, 0x3c);
  CYC(b_+369, b_+371); A = 0x03;
  CYC(b_+371, b_+374);
  TAIL(fadeoutToBlackWithDelay);
}

void s_endgameCutsceneHandler_0a_body_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wTmpcbc1);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == SYM(endgameCutsceneHandler_0a_stage0) && hook_is(gb, SYM(endgameCutsceneHandler_0a_stage0), s_endgameCutsceneHandler_0a_stage0_hook)) { s_endgameCutsceneHandler_0a_stage0_hook(gb); return; }
    else if (jt_ == SYM(endgameCutsceneHandler_0a_stage1) && hook_is(gb, SYM(endgameCutsceneHandler_0a_stage1), s_endgameCutsceneHandler_0a_stage1_hook)) { s_endgameCutsceneHandler_0a_stage1_hook(gb); return; }
    else if (jt_ == SYM(endgameCutsceneHandler_0a_stage2) && hook_is(gb, SYM(endgameCutsceneHandler_0a_stage2), s_endgameCutsceneHandler_0a_stage2_hook)) { s_endgameCutsceneHandler_0a_stage2_hook(gb); return; }
    else if (jt_ == SYM(endgameCutsceneHandler_0a_stage3) && hook_is(gb, SYM(endgameCutsceneHandler_0a_stage3), s_endgameCutsceneHandler_0a_stage3_hook)) { s_endgameCutsceneHandler_0a_stage3_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

// The credits music over black, then the first credits palette; the number of scenes to show
// (4, or 8 in a linked game) goes to wTmpcbb4.
void s_endgameCutsceneHandler_0a_stage0_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a_stage0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wTmpcbc2);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+11) { goto state0; }
    else if (jt_ == b_+42) { goto state1; }
    else if (jt_ == b_+70) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+11, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+14);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CALL_C(b_+15, s_seasonsFunc_03_66dc_hook, SYM(seasonsFunc_03_66dc), b_+18);
  CALL_C(b_+18, s_incCbc2, SYM(incCbc2), b_+21);
  CALL_C(b_+21, s_clearOam, SYM(clearOam), b_+24);
  CYC(b_+24, b_+27); SET_HL(wTmpcbb3);
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0xb4);
  CYC(b_+29, b_+30); SET_HL(HL + 1);
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x00);
  CYC(b_+32, b_+35); SET_HL(wGfxRegs1_LCDC);
  CYC(b_+35, b_+37); mem_wr(gb, HL, mem_rd(gb, HL) | 0x08);
  CYC(b_+37, b_+39); A = 0x2a; // MUS_CREDITS_2
  CYC(b_+39, b_+42);
  TAIL(playSound_b00);

state1:
  CYC(b_+42, b_+45); SET_HL(wTmpcbb3);
  CALL_C(b_+45, s_decHlRef16WithCap, SYM(decHlRef16WithCap), b_+48);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; }
  CYC(b_+48, b_+49);
  CALL_C(b_+49, s_incCbc2, SYM(incCbc2), b_+52);
  CYC(b_+52, b_+55); SET_HL(wTmpcbb3);
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x48);
  CYC(b_+57, b_+58); SET_HL(HL + 1);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0x03);
  CYC(b_+60, b_+62); A = 0x04; // PALH_04
  CALL_C(b_+62, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+65);
  CYC(b_+65, b_+67); A = 0x06;
  CYC(b_+67, b_+70);
  TAIL(fadeinFromBlackWithDelay);

state2:
  CYC(b_+70, b_+73); SET_HL(wTmpcbb3);
  CALL_C(b_+73, s_decHlRef16WithCap, SYM(decHlRef16WithCap), b_+76);
  if (!(F & FZ)) { RET_TAKEN(b_+76); return; }
  CYC(b_+76, b_+77);
  CALL_C(b_+77, s_incCbc1, SYM(incCbc1), b_+80);
  CYC(b_+80, b_+81); L = alu_inc8(gb, L);
  CYC(b_+81, b_+82); mem_wr(gb, HL, A);
  CYC(b_+82, b_+84); B = 0x04;
  CALL_C(b_+84, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+87);
  if (F & FZ) { CYCT(b_+87, b_+89); goto count; }
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+91); B = 0x08;
count:
  CYC(b_+91, b_+94); SET_HL(wTmpcbb4);
  CYC(b_+94, b_+95); mem_wr(gb, HL, B);
  CYC(b_+95, b_+98);
  TAIL(fadeoutToWhite);
}

// One credits scene per pass: scene wTmpcbb4 - 4 loads its room (from @state0Table), graphics
// and palettes, its characters, then the scrolling credits picture, until the last scene moves
// on to stage 2.
void s_endgameCutsceneHandler_0a_stage1_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a_stage1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wTmpcbc2);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+15) { goto state0; }
    else if (jt_ == b_+194) { goto state1; }
    else if (jt_ == b_+215) { goto state2; }
    else if (jt_ == b_+285) { goto state3; }
    else if (jt_ == b_+313) { goto state4; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+18); mem_wr(gb, hOamTail, A);
  CYC(b_+18, b_+21); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CALL_C(b_+23, s_disableLcd, SYM(disableLcd), b_+26);
  CALL_C(b_+26, s_incCbc2, SYM(incCbc2), b_+29);
  CALL_C(b_+29, s_clearDynamicInteractions, SYM(clearDynamicInteractions), b_+32);
  CALL_C(b_+32, s_clearOam, SYM(clearOam), b_+35);
  CYC(b_+35, b_+37); A = 0x10;
  CYC(b_+37, b_+39); mem_wr(gb, hOamTail, A);
  CYC(b_+39, b_+42); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+42, b_+44); alu_sub(gb, 0x04);
  CYC(b_+44, b_+47); SET_HL(b_+178); // @state0Table
  CYC(b_+47, b_+48); endgame_add_double_index(gb, b_+48);
  CYC(b_+48, b_+49); B = mem_rd(gb, HL);
  CYC(b_+49, b_+50); SET_HL(HL + 1);
  CYC(b_+50, b_+51); A = mem_rd(gb, HL);
  CYC(b_+51, b_+52); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+52, b_+54); goto sceneGfx; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+55); C = A;
  CYC(b_+55, b_+57); A = 0x00;
  CALL_C(b_+57, s_forceLoadRoom, SYM(forceLoadRoom), b_+60);
  CYC(b_+60, b_+62); B = 0x2d;
  CYC(b_+62, b_+65); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+65, b_+67); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+67, b_+69); goto uncompressed; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+71); B = 0x0f;
uncompressed:
  CYC(b_+71, b_+72); A = B;
  CALL_C(b_+72, s_loadUncompressedGfxHeader, SYM(loadUncompressedGfxHeader), b_+75);
sceneGfx:
  CYC(b_+75, b_+78); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+78, b_+80); alu_sub(gb, 0x04);
  CYC(b_+80, b_+81); alu_add(gb, A);
  CYC(b_+81, b_+83); alu_add(gb, 0x85);
  CALL_C(b_+83, s_loadGfxHeader, SYM(loadGfxHeader), b_+86);
  CYC(b_+86, b_+88); A = 0x0f; // PALH_0f
  CALL_C(b_+88, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+91);
  CALL_C(b_+91, s_reloadObjectGfx, SYM(reloadObjectGfx_b00), b_+94);
  CALL_C(b_+94, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+97);
  if (!(F & FZ)) { CYCT(b_+97, b_+99); goto linkedPalette; }
  CYC(b_+97, b_+99);
  CYC(b_+99, b_+102); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+102, b_+104); B = 0x10;
  CYC(b_+104, b_+106); C = 0x00;
  CYC(b_+106, b_+108); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(b_+108, b_+110); goto palette; }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+112); B = 0x50;
  CYC(b_+112, b_+114); C = 0x0e;
  CYC(b_+114, b_+116);
  goto palette;
linkedPalette:
  CYC(b_+116, b_+119); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+119, b_+121); B = 0x10;
  CYC(b_+121, b_+123); C = 0x00;
  CYC(b_+123, b_+125); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+125, b_+127); goto palette; }
  CYC(b_+125, b_+127);
  CYC(b_+127, b_+129); B = 0xae;
  CYC(b_+129, b_+131); C = 0xff;
palette:
  CYC(b_+131, b_+132); A = B;
  CYC(b_+132, b_+133); push_effect(gb, BC);
  CALL_C(b_+133, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+136);
  CYC(b_+136, b_+137); SET_BC(pop_effect(gb));
  CYC(b_+137, b_+138); A = C;
  CYC(b_+138, b_+141); mem_wr(gb, wTilesetAnimation, A);
  CALL_C(b_+141, s_loadAnimationData, SYM(loadAnimationData), b_+144);
  CYC(b_+144, b_+146); A = 0x01;
  CYC(b_+146, b_+149); mem_wr(gb, wScrollMode, A);
  CYC(b_+149, b_+150); alu_xor(gb, A);
  CYC(b_+150, b_+152); mem_wr(gb, hCameraX, A);
  CYC(b_+152, b_+154); B = 0x20;
  CYC(b_+154, b_+157); SET_HL(wTmpcfc0);
  CALL_C(b_+157, s_clearMemory, SYM(clearMemory), b_+160);
  CYC(b_+160, b_+163); SET_HL(wTmpcbb3);
  CYC(b_+163, b_+165); mem_wr(gb, HL, 0xf0);
  CYC(b_+165, b_+166); L = alu_inc8(gb, L);
  CYC(b_+166, b_+167); B = mem_rd(gb, HL);
  CALL_C(b_+167, s_seasonsFunc_03_6405_hook, SYM(seasonsFunc_03_6405), b_+170);
  CYC(b_+170, b_+172); A = 0x04;
  CALL_C(b_+172, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+175);
  CYC(b_+175, b_+178);
  TAIL(fadeinFromWhite);

state1:
  CYC(b_+194, b_+197); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+197, b_+198); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+198); return; }
  CYC(b_+198, b_+199);
  CYC(b_+199, b_+202); A = mem_rd(gb, wTmpcfc0 + 0x1f);
  CYC(b_+202, b_+203); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+203); return; }
  CYC(b_+203, b_+204);
  CALL_C(b_+204, s_incCbc2, SYM(incCbc2), b_+207);
  CYC(b_+207, b_+209); A = 0xff;
  CYC(b_+209, b_+212); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+212, b_+215);
  TAIL(fadeoutToWhite);

state2:
  CYC(b_+215, b_+218); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+218, b_+219); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+219); return; }
  CYC(b_+219, b_+220);
  CALL_C(b_+220, s_incCbc2, SYM(incCbc2), b_+223);
  CALL_C(b_+223, s_disableLcd, SYM(disableLcd), b_+226);
  CALL_C(b_+226, s_clearWramBank1, SYM(clearWramBank1), b_+229);
  CYC(b_+229, b_+232); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+232, b_+234); alu_sub(gb, 0x04);
  CYC(b_+234, b_+235); alu_add(gb, A);
  CYC(b_+235, b_+237); alu_add(gb, 0x86);
  CALL_C(b_+237, s_loadGfxHeader, SYM(loadGfxHeader), b_+240);
  CYC(b_+240, b_+243); SET_HL(wTmpcbb3);
  CYC(b_+243, b_+245); mem_wr(gb, HL, 0x5a);
  CYC(b_+245, b_+246); L = alu_inc8(gb, L);
  CYC(b_+246, b_+247); A = mem_rd(gb, HL);
  CYC(b_+247, b_+249); alu_add(gb, 0x9d);
  CALL_C(b_+249, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+252);
  CYC(b_+252, b_+254); A = 0x04;
  CALL_C(b_+254, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+257);
  CYC(b_+257, b_+260); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+260, b_+262); alu_sub(gb, 0x04);
  CYC(b_+262, b_+265); SET_HL(b_+277); // @state2Table
  CYC(b_+265, b_+266); endgame_add_a_to_hl(gb, b_+266);
  CYC(b_+266, b_+267); A = mem_rd(gb, HL);
  CYC(b_+267, b_+270); mem_wr(gb, wGfxRegs1_SCX, A);
  CYC(b_+270, b_+272); A = 0x10;
  CYC(b_+272, b_+274); mem_wr(gb, hCameraX, A);
  CYC(b_+274, b_+277);
  TAIL(fadeinFromWhite);

state3:
  CYC(b_+285, b_+288); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+288, b_+289); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+289); return; }
  CYC(b_+289, b_+290);
  CALL_C(b_+290, s_decCbb3, SYM(decCbb3), b_+293);
  if (!(F & FZ)) { RET_TAKEN(b_+293); return; }
  CYC(b_+293, b_+294);
  CALL_C(b_+294, s_incCbc2, SYM(incCbc2), b_+297);
  CALL_C(b_+297, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+300);
  if (!(F & FZ)) { RET_TAKEN(b_+300); return; }
  CYC(b_+300, b_+301);
  CYC(b_+301, b_+303); mem_wr(gb, HL, 0xae); // INTERAC_CREDITS_TEXT_HORIZONTAL
  CYC(b_+303, b_+304); L = alu_inc8(gb, L);
  CYC(b_+304, b_+307); A = mem_rd(gb, wTmpcbb4);
  CYC(b_+307, b_+309); alu_sub(gb, 0x04);
  CYC(b_+309, b_+310); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+310, b_+312); mem_wr(gb, HL, 0x00);
  RET(b_+312); return;

state4:
  CYC(b_+313, b_+316); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+316, b_+317); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+317); return; }
  CYC(b_+317, b_+318);
  CYC(b_+318, b_+319); alu_xor(gb, A);
  CYC(b_+319, b_+321); mem_wr(gb, hOamTail, A);
  CYC(b_+321, b_+324); A = mem_rd(gb, wTmpcfc0 + 0x1e);
  CYC(b_+324, b_+325); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+325); return; }
  CYC(b_+325, b_+326);
  CYC(b_+326, b_+328); B = 0x07;
  CALL_C(b_+328, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+331);
  if (F & FZ) { CYCT(b_+331, b_+333); goto lastScene; }
  CYC(b_+331, b_+333);
  CYC(b_+333, b_+335); B = 0x0b;
lastScene:
  CYC(b_+335, b_+338); SET_HL(wTmpcbb4);
  CYC(b_+338, b_+339); A = mem_rd(gb, HL);
  CYC(b_+339, b_+340); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+340, b_+342); goto done; }
  CYC(b_+340, b_+342);
  CYC(b_+342, b_+343); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+343, b_+344); alu_xor(gb, A);
  CYC(b_+344, b_+347); mem_wr(gb, wTmpcbc2, A);
  CYC(b_+347, b_+349);
  goto fade;
done:
  CALL_C(b_+349, s_seasonsFunc_03_646a_hook, SYM(seasonsFunc_03_646a), b_+352);
  CALL_C(b_+352, s_enableActiveRing, SYM(enableActiveRing), b_+355);
  CYC(b_+355, b_+357); A = 0x02;
  CYC(b_+357, b_+360); mem_wr(gb, wTmpcbc1, A);
  CYC(b_+360, b_+363); SET_HL(wLinkMaxHealth);
  CYC(b_+363, b_+364); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+364, b_+365); mem_wr(gb, HL, A);
  CYC(b_+365, b_+366); alu_xor(gb, A);
  CYC(b_+366, b_+368); L = 0x80; // <wInventoryB
  CYC(b_+368, b_+369); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+369, b_+370); mem_wr(gb, HL, A);
  CYC(b_+370, b_+372); L = 0xc5; // <wActiveRing
  CYC(b_+372, b_+374); mem_wr(gb, HL, 0xff);
fade:
  CYC(b_+374, b_+377);
  TAIL(fadeoutToWhite);
}

// endgameCutsceneHandler_0a_stage2@seasonsFunc_03_616f: the credits' tree, sky and ground
// sprites placed against the scroll (wGfxRegs1_SCY), the upper ones only below line $60.
static void endgame_credits_scroll_sprites(GB *gb) {
  BASE(endgameCutsceneHandler_0a_stage2);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+258, b_+259); alu_xor(gb, A);
  CYC(b_+259, b_+261); mem_wr(gb, hOamTail, A);
  CYC(b_+261, b_+264); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+264, b_+266); alu_cp(gb, 0x60);
  if (!(F & FC)) { CYCT(b_+266, b_+268); goto lower; }
  CYC(b_+266, b_+268);
  CYC(b_+268, b_+269); alu_cpl(gb);
  CYC(b_+269, b_+270); A = alu_inc8(gb, A);
  CYC(b_+270, b_+271); B = A;
  CYC(b_+271, b_+274); A = mem_rd(gb, wFrameCounter);
  CYC(b_+274, b_+276); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+276, b_+278); goto lower; }
  CYC(b_+276, b_+278);
  CYC(b_+278, b_+279); C = A;
  CYC(b_+279, b_+282); SET_HL(SYM(seasonsOamData_03_6641));
  CALL_C(b_+282, s_addSpritesToOam_withOffset, SYM(addSpritesToOam_withOffset), b_+285);
lower:
  CYC(b_+285, b_+288); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+288, b_+289); alu_cpl(gb);
  CYC(b_+289, b_+290); A = alu_inc8(gb, A);
  CYC(b_+290, b_+292); B = 0xc7;
  CYC(b_+292, b_+293); alu_add(gb, B);
  CYC(b_+293, b_+294); B = A;
  CYC(b_+294, b_+296); C = 0x38;
  CYC(b_+296, b_+299); SET_HL(SYM(seasonsOamData_03_668a));
  CYC(b_+299, b_+300); push_effect(gb, BC);
  CALL_C(b_+300, s_addSpritesToOam_withOffset, SYM(addSpritesToOam_withOffset), b_+303);
  CYC(b_+303, b_+304); SET_BC(pop_effect(gb));
  CYC(b_+304, b_+307); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+307, b_+309); alu_cp(gb, 0x60);
  if (F & FC) { RET_TAKEN(b_+309); return; }
  CYC(b_+309, b_+310);
  CYC(b_+310, b_+313); SET_HL(SYM(seasonsOamData_03_66bf));
  CYC(b_+313, b_+316);
  TAIL(addSpritesToOam_withOffset);
}

// The last scene: the Maku tree picture scrolled up, then (linked games) the flash and the
// credits sprites scrolling down; unlinked games skip to stage 3.
void s_endgameCutsceneHandler_0a_stage2_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a_stage2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); mem_wr(gb, hOamTail, A);
  CYC(b_+3, b_+6); SET_DE(wTmpcbc2);
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); push_effect(gb, b_+8);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+26) { goto state0; }
    else if (jt_ == b_+80) { goto state1; }
    else if (jt_ == b_+96) { goto state2; }
    else if (jt_ == b_+161) { goto state3; }
    else if (jt_ == b_+189) { goto state4; }
    else if (jt_ == b_+201) { goto state5; }
    else if (jt_ == b_+316) { goto state6; }
    else if (jt_ == b_+331) { goto state7; }
    else if (jt_ == b_+377) { goto state8; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CYC(b_+26, b_+29); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+29, b_+30); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CALL_C(b_+31, s_incCbc2, SYM(incCbc2), b_+34);
  CALL_C(b_+34, s_disableLcd, SYM(disableLcd), b_+37);
  CALL_C(b_+37, s_clearDynamicInteractions, SYM(clearDynamicInteractions), b_+40);
  CALL_C(b_+40, s_clearOam, SYM(clearOam), b_+43);
  CYC(b_+43, b_+44); alu_xor(gb, A);
  CYC(b_+44, b_+47); mem_wr(gb, wTmpcfc0 + 0x1e, A);
  CYC(b_+47, b_+49); A = 0x95;
  CALL_C(b_+49, s_loadGfxHeader, SYM(loadGfxHeader), b_+52);
  CYC(b_+52, b_+54); A = 0xa0; // PALH_a0
  CALL_C(b_+54, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+57);
  CYC(b_+57, b_+59); A = 0x09;
  CALL_C(b_+59, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+62);
  CALL_C(b_+62, s_fadeinFromWhite, SYM(fadeinFromWhite), b_+65);
  CALL_C(b_+65, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+68);
  if (!(F & FZ)) { RET_TAKEN(b_+68); return; }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0xaf);
  CYC(b_+71, b_+73); L = 0x4b; // Interaction.yh
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0xe8);
  CYC(b_+75, b_+76); L = alu_inc8(gb, L);
  CYC(b_+76, b_+77); L = alu_inc8(gb, L);
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x50);
  RET(b_+79); return;

state1:
  CYC(b_+80, b_+83); A = mem_rd(gb, wTmpcfc0 + 0x1e);
  CYC(b_+83, b_+84); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+84); return; }
  CYC(b_+84, b_+85);
  CYC(b_+85, b_+88); SET_HL(wTmpcbb3);
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0xe0);
  CYC(b_+90, b_+91); SET_HL(HL + 1);
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x01);
  CYC(b_+93, b_+96);
  TAIL(incCbc2);

state2:
  CYC(b_+96, b_+99); SET_HL(wTmpcbb3);
  CALL_C(b_+99, s_decHlRef16WithCap, SYM(decHlRef16WithCap), b_+102);
  if (!(F & FZ)) { RET_TAKEN(b_+102); return; }
  CYC(b_+102, b_+103);
  CALL_C(b_+103, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+106);
  if (!(F & FZ)) { CYCT(b_+106, b_+108); goto linked; }
  CYC(b_+106, b_+108);
  CALL_C(b_+108, s_seasonsFunc_03_646a_hook, SYM(seasonsFunc_03_646a), b_+111);
  CYC(b_+111, b_+113); A = 0x03;
  CYC(b_+113, b_+116); mem_wr(gb, wTmpcbc1, A);
  CYC(b_+116, b_+118); A = 0x04;
  CYC(b_+118, b_+121);
  TAIL(fadeoutToWhiteWithDelay);
linked:
  CYC(b_+121, b_+123); A = 0x04;
  CYC(b_+123, b_+126); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+126, b_+129); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+129, b_+131); mem_wr(gb, hCameraY, A);
  CYC(b_+131, b_+133); A = 0x01;
  CALL_C(b_+133, s_loadUncompressedGfxHeader, SYM(loadUncompressedGfxHeader), b_+136);
  CYC(b_+136, b_+138); A = 0x0b; // PALH_0b
  CALL_C(b_+138, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+141);
  CYC(b_+141, b_+143); B = 0x03;
  for (;;) {
    CALL_C(b_+143, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+146);
    if (!(F & FZ)) { CYCT(b_+146, b_+148); break; }
    CYC(b_+146, b_+148);
    CYC(b_+148, b_+150); mem_wr(gb, HL, 0x4a);
    CYC(b_+150, b_+151); L = alu_inc8(gb, L);
    CYC(b_+151, b_+153); mem_wr(gb, HL, 0x09);
    CYC(b_+153, b_+154); L = alu_inc8(gb, L);
    CYC(b_+154, b_+155); B = alu_dec8(gb, B);
    CYC(b_+155, b_+156); mem_wr(gb, HL, B);
    if (!(F & FZ)) { CYCT(b_+156, b_+158); continue; }
    CYC(b_+156, b_+158);
    break;
  }
  CYC(b_+158, b_+161);
  TAIL(incCbc2);

state3:
  CYC(b_+161, b_+164); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+164, b_+165); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+165, b_+167); goto scrollUp; }
  CYC(b_+165, b_+167);
  CYC(b_+167, b_+169); A = 0x78;
  CYC(b_+169, b_+172); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+172, b_+175);
  TAIL(incCbc2);
scrollUp:
  CALL_C(b_+175, s_decCbb3, SYM(decCbb3), b_+178);
  if (!(F & FZ)) { RET_TAKEN(b_+178); return; }
  CYC(b_+178, b_+179);
  CYC(b_+179, b_+181); mem_wr(gb, HL, 0x04);
  CYC(b_+181, b_+184); SET_HL(wGfxRegs1_SCY);
  CYC(b_+184, b_+185); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+185, b_+186); A = mem_rd(gb, HL);
  CYC(b_+186, b_+188); mem_wr(gb, hCameraY, A);
  RET(b_+188); return;

state4:
  CALL_C(b_+189, s_decCbb3, SYM(decCbb3), b_+192);
  if (!(F & FZ)) { RET_TAKEN(b_+192); return; }
  CYC(b_+192, b_+193);
  CYC(b_+193, b_+195); A = 0xff;
  CYC(b_+195, b_+198); mem_wr(gb, wTmpcbba, A);
  CYC(b_+198, b_+201);
  TAIL(incCbc2);

state5:
  CYC(b_+201, b_+204); SET_HL(wTmpcbb3);
  CYC(b_+204, b_+206); B = 0x01;
  CALL_C(b_+206, s_flashScreen, SYM(flashScreen), b_+209);
  if (F & FZ) { RET_TAKEN(b_+209); return; }
  CYC(b_+209, b_+210);
  CALL_C(b_+210, s_disableLcd, SYM(disableLcd), b_+213);
  CYC(b_+213, b_+215); A = 0x9a;
  CALL_C(b_+215, s_loadGfxHeader, SYM(loadGfxHeader), b_+218);
  CYC(b_+218, b_+220); A = 0x9f; // PALH_9f
  CALL_C(b_+220, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+223);
  CALL_C(b_+223, s_clearDynamicInteractions, SYM(clearDynamicInteractions), b_+226);
  CYC(b_+226, b_+228); B = 0x03;
  for (;;) {
    CALL_C(b_+228, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+231);
    if (!(F & FZ)) { CYCT(b_+231, b_+233); break; }
    CYC(b_+231, b_+233);
    CYC(b_+233, b_+235); mem_wr(gb, HL, 0xcf);
    CYC(b_+235, b_+236); L = alu_inc8(gb, L);
    CYC(b_+236, b_+237); B = alu_dec8(gb, B);
    CYC(b_+237, b_+238); mem_wr(gb, HL, B);
    if (!(F & FZ)) { CYCT(b_+238, b_+240); continue; }
    CYC(b_+238, b_+240);
    break;
  }
  CYC(b_+240, b_+242); A = 0x04;
  CALL_C(b_+242, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+245);
  CYC(b_+245, b_+247); A = 0x04;
  CALL_C(b_+247, s_fadeinFromWhiteWithDelay, SYM(fadeinFromWhiteWithDelay), b_+250);
  CALL_C(b_+250, s_incCbc2, SYM(incCbc2), b_+253);
  CYC(b_+253, b_+255); A = 0xf0;
  CYC(b_+255, b_+258); mem_wr(gb, wTmpcbb3, A);
  endgame_credits_scroll_sprites(gb); return;

state6:
  CALL_L(b_+316, endgame_credits_scroll_sprites, b_+319);
  CALL_C(b_+319, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+322);
  if (!(F & FZ)) { RET_TAKEN(b_+322); return; }
  CYC(b_+322, b_+323);
  CYC(b_+323, b_+325); A = 0x04;
  CYC(b_+325, b_+328); mem_wr(gb, wTmpcbb3, A);
  CYC(b_+328, b_+331);
  TAIL(incCbc2);

state7:
  CYC(b_+331, b_+334); A = mem_rd(gb, wGfxRegs1_SCY);
  CYC(b_+334, b_+336); alu_cp(gb, 0x98);
  if (!(F & FZ)) { CYCT(b_+336, b_+338); goto scrollDown; }
  CYC(b_+336, b_+338);
  CYC(b_+338, b_+340); A = 0xf0;
  CYC(b_+340, b_+343); mem_wr(gb, wTmpcbb3, A);
  CALL_C(b_+343, s_incCbc2, SYM(incCbc2), b_+346);
  CYC(b_+346, b_+348);
  goto sprites;
scrollDown:
  CALL_C(b_+348, s_decCbb3, SYM(decCbb3), b_+351);
  if (!(F & FZ)) { CYCT(b_+351, b_+353); goto sprites; }
  CYC(b_+351, b_+353);
  CYC(b_+353, b_+355); mem_wr(gb, HL, 0x04);
  CYC(b_+355, b_+358); SET_HL(wGfxRegs1_SCY);
  CYC(b_+358, b_+359); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+359, b_+360); A = mem_rd(gb, HL);
  CYC(b_+360, b_+362); mem_wr(gb, hCameraY, A);
  CYC(b_+362, b_+364); alu_cp(gb, 0x60);
  if (!(F & FZ)) { CYCT(b_+364, b_+366); goto sprites; }
  CYC(b_+364, b_+366);
  CALL_C(b_+366, s_clearDynamicInteractions, SYM(clearDynamicInteractions), b_+369);
  CYC(b_+369, b_+371); A = 0x2c;
  CALL_C(b_+371, s_loadUncompressedGfxHeader, SYM(loadUncompressedGfxHeader), b_+374);
sprites:
  CYC(b_+374, b_+377);
  endgame_credits_scroll_sprites(gb); return;

state8:
  CALL_L(b_+377, endgame_credits_scroll_sprites, b_+380);
  CALL_C(b_+380, s_decCbb3, SYM(decCbb3), b_+383);
  if (!(F & FZ)) { RET_TAKEN(b_+383); return; }
  CYC(b_+383, b_+384);
  CALL_C(b_+384, s_seasonsFunc_03_646a_hook, SYM(seasonsFunc_03_646a), b_+387);
  CYC(b_+387, b_+389); A = 0x03;
  CYC(b_+389, b_+392); mem_wr(gb, wTmpcbc1, A);
  CYC(b_+392, b_+394); A = 0x04;
  CYC(b_+394, b_+397);
  TAIL(fadeoutToWhiteWithDelay);
}

// endgameCutsceneHandler_0a_stage3@state1Func0: in a linked game, a random twinkle sound
// every few frames (countdown in wTmpcbb4 from @state1Table).
static void endgame_twinkle(GB *gb) {
  BASE(endgameCutsceneHandler_0a_stage3);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+126, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+129);
  if (F & FZ) { RET_TAKEN(b_+129); return; }
  CYC(b_+129, b_+130);
  CYC(b_+130, b_+133); SET_HL(wTmpcbb4);
  CYC(b_+133, b_+134); A = mem_rd(gb, HL);
  CYC(b_+134, b_+135); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+135, b_+137); goto sound; }
  CYC(b_+135, b_+137);
  CYC(b_+137, b_+138); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+138); return;
sound:
  CYC(b_+139, b_+141); A = 0xaa;
  CALL_C(b_+141, playSound_b00_hook, SYM(playSound_b00), b_+144);
  CALL_C(b_+144, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+147);
  CYC(b_+147, b_+149); alu_and(gb, 0x03);
  CYC(b_+149, b_+152); SET_HL(b_+158); // @state1Table
  CYC(b_+152, b_+153); endgame_add_a_to_hl(gb, b_+153);
  CYC(b_+153, b_+154); A = mem_rd(gb, HL);
  CYC(b_+154, b_+157); mem_wr(gb, wTmpcbb4, A);
  RET(b_+157); return;
}

// endgameCutsceneHandler_0a_stage3@state9Func0: the "to be continued" sprites from bank $15.
static void endgame_to_be_continued_sprites(GB *gb) {
  BASE(endgameCutsceneHandler_0a_stage3);
  CYC(b_+423, b_+426); SET_HL(SYM(oamData_15_4e0c));
  CYC(b_+426, b_+428); E = 0x15; // :oamData_15_4e0c
  CYC(b_+428, b_+431); SET_BC(0x3038);
  CYC(b_+431, b_+432); alu_xor(gb, A);
  CYC(b_+432, b_+434); mem_wr(gb, hOamTail, A);
  CYC(b_+434, b_+437);
  TAIL(addSpritesFromBankToOam_withOffset);
}

// The end: "The End" (or the linked-game ending picture), the game transfer secret on the
// file select background, then "to be continued" and the reset.
void s_endgameCutsceneHandler_0a_stage3_hook(GB *gb) {
  BASE(endgameCutsceneHandler_0a_stage3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_DE(wTmpcbc2);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (endgame_jump_table(gb));
    if (jt_ == b_+29) { goto state0; }
    else if (jt_ == b_+118) { goto state1; }
    else if (jt_ == b_+162) { goto state2; }
    else if (jt_ == b_+172) { goto state3; }
    else if (jt_ == b_+195) { goto state4; }
    else if (jt_ == b_+285) { goto state5; }
    else if (jt_ == b_+308) { goto state6; }
    else if (jt_ == b_+334) { goto state7; }
    else if (jt_ == b_+366) { goto state8; }
    else if (jt_ == b_+407) { goto state9; }
    else if (jt_ == b_+437) { goto stateA; }
    else if (jt_ == b_+461) { goto stateB; }
    else { HANDOFF(HL); }
  } while (0);

state0:
  CALL_C(b_+29, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+32);
  if (!(F & FZ)) CALL_L_CC(b_+32, endgame_credits_scroll_sprites, b_+35);
  else CYC(b_+32, b_+35);
  CYC(b_+35, b_+38); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+39); return; }
  CYC(b_+39, b_+40);
  CALL_C(b_+40, s_disableLcd, SYM(disableLcd), b_+43);
  CALL_C(b_+43, s_incCbc2, SYM(incCbc2), b_+46);
  CALL_C(b_+46, s_seasonsFunc_03_66ed_hook, SYM(seasonsFunc_03_66ed), b_+49);
  CALL_C(b_+49, s_clearDynamicInteractions, SYM(clearDynamicInteractions), b_+52);
  CALL_C(b_+52, s_clearOam, SYM(clearOam), b_+55);
  CALL_C(b_+55, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+58);
  if (F & FZ) { CYCT(b_+58, b_+61); goto unlinked; }
  CYC(b_+58, b_+61);
  CYC(b_+61, b_+63); A = 0x99;
  CALL_C(b_+63, s_loadGfxHeader, SYM(loadGfxHeader), b_+66);
  CYC(b_+66, b_+68); A = 0xaa; // PALH_aa
  CALL_C(b_+68, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+71);
  CYC(b_+71, b_+74); SET_HL(SYM(objectData5887));
  CALL_C(b_+74, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+77);
  CYC(b_+77, b_+79);
  goto gfx;
unlinked:
  CYC(b_+79, b_+81); A = 0x98;
  CALL_C(b_+81, s_loadGfxHeader, SYM(loadGfxHeader), b_+84);
  CYC(b_+84, b_+86); A = 0xa9; // PALH_a9
  CALL_C(b_+86, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+89);
gfx:
  CYC(b_+89, b_+91); A = 0x04;
  CALL_C(b_+91, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+94);
  CYC(b_+94, b_+95); alu_xor(gb, A);
  CYC(b_+95, b_+98); SET_HL(hCameraY);
  CYC(b_+98, b_+99); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+99, b_+100); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+100, b_+101); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+101, b_+102); mem_wr(gb, HL, A);
  CYC(b_+102, b_+105); SET_HL(wTmpcbb3);
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0xf0);
  CYC(b_+107, b_+108); mem_wr(gb, HL, A);
  CYC(b_+108, b_+110); A = 0xfb;
  CALL_C(b_+110, playSound_b00_hook, SYM(playSound_b00), b_+113);
  CYC(b_+113, b_+115); A = 0x04;
  CYC(b_+115, b_+118);
  TAIL(fadeinFromWhiteWithDelay);

state1:
  CYC(b_+118, b_+121); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+121, b_+122); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+122); return; }
  CYC(b_+122, b_+123);
  CALL_C(b_+123, s_incCbc2, SYM(incCbc2), b_+126);
  endgame_twinkle(gb); return;

state2:
  CALL_L(b_+162, endgame_twinkle, b_+165);
  CALL_C(b_+165, s_decCbb3, SYM(decCbb3), b_+168);
  if (!(F & FZ)) { RET_TAKEN(b_+168); return; }
  CYC(b_+168, b_+169);
  CALL_C(b_+169, s_incCbc2, SYM(incCbc2), b_+172);
state3:
  CALL_L(b_+172, endgame_twinkle, b_+175);
  CYC(b_+175, b_+178); SET_HL(wFileIsLinkedGame);
  CYC(b_+178, b_+179); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+179, b_+180); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+180, b_+182); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+182); return; }
  CYC(b_+182, b_+183);
  CYC(b_+183, b_+186); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+186, b_+188); alu_and(gb, 0x0b);
  if (F & FZ) { RET_TAKEN(b_+188); return; }
  CYC(b_+188, b_+189);
  CALL_C(b_+189, s_incCbc2, SYM(incCbc2), b_+192);
  CYC(b_+192, b_+195);
  TAIL(fadeoutToWhite);

state4:
  CYC(b_+195, b_+198); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+198, b_+199); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+199); return; }
  CYC(b_+199, b_+200);
  CALL_C(b_+200, s_incCbc2, SYM(incCbc2), b_+203);
  CALL_C(b_+203, s_disableLcd, SYM(disableLcd), b_+206);
  CALL_C(b_+206, s_generateGameTransferSecret, SYM(generateGameTransferSecret), b_+209);
  CYC(b_+209, b_+211); A = 0xff;
  CYC(b_+211, b_+214); mem_wr(gb, wTmpcbba, A);
  CYC(b_+214, b_+216); A = mem_rd(gb, IO_SVBK);
  CYC(b_+216, b_+217); push_effect(gb, AF);
  CYC(b_+217, b_+219); A = 0x07;
  CYC(b_+219, b_+221); mem_wr(gb, IO_SVBK, A);
  CYC(b_+221, b_+224); SET_HL(w7SecretText1);
  CYC(b_+224, b_+227); SET_DE(w7d800);
  CYC(b_+227, b_+230); SET_BC(0x1800);
  for (;;) {
    CYC(b_+230, b_+231); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CALL_C(b_+231, s_copyTextCharacterGfx, SYM(copyTextCharacterGfx), b_+234);
    CYC(b_+234, b_+235); B = alu_dec8(gb, B);
    if (!(F & FZ)) { CYCT(b_+235, b_+237); continue; }
    CYC(b_+235, b_+237);
    break;
  }
  CYC(b_+237, b_+238); SET_AF(pop_effect(gb));
  CYC(b_+238, b_+240); mem_wr(gb, IO_SVBK, A);
  CYC(b_+240, b_+242); A = 0x97;
  CALL_C(b_+242, s_loadGfxHeader, SYM(loadGfxHeader), b_+245);
  CYC(b_+245, b_+247); A = 0x2b;
  CALL_C(b_+247, s_loadUncompressedGfxHeader, SYM(loadUncompressedGfxHeader), b_+250);
  CYC(b_+250, b_+252); A = 0x05; // PALH_05
  CALL_C(b_+252, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+255);
  CALL_C(b_+255, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+258);
  CYC(b_+258, b_+260); A = 0x84;
  if (!(F & FZ)) CALL_C_CC(b_+260, s_loadGfxHeader, SYM(loadGfxHeader), b_+263);
  else CYC(b_+260, b_+263);
  CALL_C(b_+263, s_clearDynamicInteractions, SYM(clearDynamicInteractions), b_+266);
  CALL_C(b_+266, s_clearOam, SYM(clearOam), b_+269);
  CYC(b_+269, b_+271); A = 0x04;
  CALL_C(b_+271, s_loadGfxRegisterStateIndex, SYM(loadGfxRegisterStateIndex), b_+274);
  CYC(b_+274, b_+277); SET_HL(wTmpcbb3);
  CYC(b_+277, b_+279); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+279, s_fileSelect_redrawDecorations, SYM(fileSelect_redrawDecorations), b_+282);
  CYC(b_+282, b_+285);
  TAIL(fadeinFromWhite);

state5:
  CALL_C(b_+285, s_fileSelect_redrawDecorations, SYM(fileSelect_redrawDecorations), b_+288);
  CALL_C(b_+288, s_seasonsFunc_03_6462_hook, SYM(seasonsFunc_03_6462), b_+291);
  if (!(F & FZ)) { RET_TAKEN(b_+291); return; }
  CYC(b_+291, b_+292);
  CYC(b_+292, b_+295); SET_HL(wTmpcbb3);
  CYC(b_+295, b_+297); B = 0x3c;
  CALL_C(b_+297, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+300);
  if (F & FZ) { CYCT(b_+300, b_+302); goto delay; }
  CYC(b_+300, b_+302);
  CYC(b_+302, b_+304); B = 0xb4;
delay:
  CYC(b_+304, b_+305); mem_wr(gb, HL, B);
  CYC(b_+305, b_+308);
  TAIL(incCbc2);

state6:
  CALL_C(b_+308, s_fileSelect_redrawDecorations, SYM(fileSelect_redrawDecorations), b_+311);
  CALL_C(b_+311, s_decCbb3, SYM(decCbb3), b_+314);
  if (!(F & FZ)) { RET_TAKEN(b_+314); return; }
  CYC(b_+314, b_+315);
  CALL_C(b_+315, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+318);
  if (!(F & FZ)) { CYCT(b_+318, b_+320); goto next6; }
  CYC(b_+318, b_+320);
  CALL_C(b_+320, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+323);
  if (!(F & FZ)) { CYCT(b_+323, b_+325); goto next6; }
  CYC(b_+323, b_+325);
  CYC(b_+325, b_+327); mem_wr(gb, HL, 0xd1);
  CYC(b_+327, b_+328); alu_xor(gb, A);
  CYC(b_+328, b_+331); mem_wr(gb, wTmpcfc0 + 0x1e, A);
next6:
  CYC(b_+331, b_+334);
  TAIL(incCbc2);

state7:
  CALL_C(b_+334, s_fileSelect_redrawDecorations, SYM(fileSelect_redrawDecorations), b_+337);
  CALL_C(b_+337, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+340);
  if (F & FZ) { CYCT(b_+340, b_+342); goto waitObject; }
  CYC(b_+340, b_+342);
  CYC(b_+342, b_+345); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+345, b_+347); alu_and(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+347, b_+349); goto leave; }
  CYC(b_+347, b_+349);
  RET(b_+349); return;
waitObject:
  CYC(b_+350, b_+353); A = mem_rd(gb, wTmpcfc0 + 0x1e);
  CYC(b_+353, b_+354); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+354); return; }
  CYC(b_+354, b_+355);
leave:
  CALL_C(b_+355, s_incCbc2, SYM(incCbc2), b_+358);
  CYC(b_+358, b_+360); A = 0xfa; // SNDCTRL_FAST_FADEOUT
  CALL_C(b_+360, playSound_b00_hook, SYM(playSound_b00), b_+363);
  CYC(b_+363, b_+366);
  TAIL(fadeoutToWhite);

state8:
  CALL_C(b_+366, s_fileSelect_redrawDecorations, SYM(fileSelect_redrawDecorations), b_+369);
  CYC(b_+369, b_+372); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+372, b_+373); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+373); return; }
  CYC(b_+373, b_+374);
  CALL_C(b_+374, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+377);
  if (!(F & FZ)) { CYCT(b_+377, b_+380); TAIL(resetGame); }
  CYC(b_+377, b_+380);
  CALL_C(b_+380, s_disableLcd, SYM(disableLcd), b_+383);
  CALL_C(b_+383, s_clearOam, SYM(clearOam), b_+386);
  CALL_C(b_+386, s_incCbc2, SYM(incCbc2), b_+389);
  CYC(b_+389, b_+391); A = 0x82; // GFXH_TO_BE_CONTINUED
  CALL_C(b_+391, s_loadGfxHeader, SYM(loadGfxHeader), b_+394);
  CYC(b_+394, b_+396); A = 0x8f; // PALH_SEASONS_8f
  CALL_C(b_+396, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+399);
  CALL_C(b_+399, s_fadeinFromWhite, SYM(fadeinFromWhite), b_+402);
  CYC(b_+402, b_+404); A = 0x04;
  CYC(b_+404, b_+407);
  TAIL(loadGfxRegisterStateIndex);

state9:
  CALL_L(b_+407, endgame_to_be_continued_sprites, b_+410);
  CYC(b_+410, b_+413); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+413, b_+414); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+414); return; }
  CYC(b_+414, b_+415);
  CYC(b_+415, b_+418); SET_HL(wTmpcbb3);
  CYC(b_+418, b_+420); mem_wr(gb, HL, 0xb4);
  CYC(b_+420, b_+423);
  TAIL(incCbc2);

stateA:
  CALL_L(b_+437, endgame_to_be_continued_sprites, b_+440);
  CYC(b_+440, b_+443); SET_HL(wTmpcbb3);
  CYC(b_+443, b_+444); A = mem_rd(gb, HL);
  CYC(b_+444, b_+445); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+445, b_+447); goto pressA; }
  CYC(b_+445, b_+447);
  CYC(b_+447, b_+448); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+448); return;
pressA:
  CYC(b_+449, b_+452); A = mem_rd(gb, wKeysJustPressed);
  CYC(b_+452, b_+454); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+454); return; }
  CYC(b_+454, b_+455);
  CALL_C(b_+455, s_incCbc2, SYM(incCbc2), b_+458);
  CYC(b_+458, b_+461);
  TAIL(fadeoutToWhite);

stateB:
  CALL_L(b_+461, endgame_to_be_continued_sprites, b_+464);
  CYC(b_+464, b_+467); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+467, b_+468); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+468); return; }
  CYC(b_+468, b_+469);
  CYC(b_+469, b_+472);
  TAIL(resetGame);
}
