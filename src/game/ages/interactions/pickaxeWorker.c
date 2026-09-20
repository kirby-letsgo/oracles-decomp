#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t bank09_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false); return HL;
}

void interactionCode57_hook(GB *gb);
void interactionCode57__subid00_hook(GB *gb);
void interactionCode57__subid03__state0_hook(GB *gb);
void interactionCode57__afterCall693f_hook(GB *gb);
void interactionCode57__subid03__state1_hook(GB *gb);
void interactionCode57__subid01_hook(GB *gb);
void interactionCode57__subid1And2State0_hook(GB *gb);
void interactionCode57__subid1State1_hook(GB *gb);
void interactionCode57__subid1And2Substate0_hook(GB *gb);
void interactionCode57__label_09_221_hook(GB *gb);
void interactionCode57__nextObject_hook(GB *gb);
void interactionCode57__doneSpawningObjects_hook(GB *gb);
void interactionCode57__subid1Substate1_hook(GB *gb);
void interactionCode57__updateAnimationAndRunScript_hook(GB *gb);
void interactionCode57__subid1Substate2_hook(GB *gb);
void interactionCode57__subid1Substate3_hook(GB *gb);
void interactionCode57__subid02_hook(GB *gb);
void interactionCode57__subid2Substate1_hook(GB *gb);
void interactionCode57__subid2Substate2_hook(GB *gb);
void interactionCode57__unusedFunc_6a80_hook(GB *gb);
void interactionCode57__loadScriptAndInitGraphics_hook(GB *gb);
void interactionCode57__createDirtChips_hook(GB *gb);
void interactionCode57__next_hook(GB *gb);

void interactionCode57_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+12) { goto L_6937; }
    else if (jt_ == b_+53) { goto L_6960; }
    else if (jt_ == b_+281) { goto L_6a44; }
    else { HANDOFF(HL); }
  } while (0);
L_6937:
  CALL_C(b_+12, checkInteractionState_hook, SYM(checkInteractionState), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto L_6942; } CYC(b_+15, b_+17);
L_693c:
  CALL_C(b_+17, interactionCode57__loadScriptAndInitGraphics_hook, b_+350, b_+20);
L_693f:
  CALL_C(b_+20, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+23);
L_6942:
  CALL_C(b_+23, interactionRunScript_hook, SYM(interactionRunScript), b_+26);
  if ((F & FC)) { CYCT(b_+26, b_+29); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+26, b_+29);
  CALL_C(b_+29, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+32);
  CYC(b_+32, b_+34); E = 0x61;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+36, b_+37); ret_effect(gb); return; } CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); A = 0x50;
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);
  CYC(b_+42, b_+45); A = mem_rd(gb, wScreenVariables);
  CYC(b_+45, b_+47); alu_and(gb, 0x01);
  if ((F & FZ)) { CYCT(b_+47, b_+48); ret_effect(gb); return; } CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); A = 0x03;
  CYC(b_+50, b_+53); goto L_6aa4;
L_6960:
  CALL_C(b_+53, checkInteractionState_hook, SYM(checkInteractionState), b_+56);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto L_6975; } CYC(b_+56, b_+58);
L_6965:
  CYC(b_+58, b_+60); E = 0x42;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); A = alu_dec8(gb, A);
  CYC(b_+62, b_+64); A = 0x0c;
  if ((F & FZ)) { CYCT(b_+64, b_+66); goto L_696f; } CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); A = 0xf4;
L_696f:
  CYC(b_+68, b_+70); E = 0x78;
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CALL_C(b_+71, interactionCode57__loadScriptAndInitGraphics_hook, b_+350, b_+74);
L_6975:
  CYC(b_+74, b_+76); E = 0x45;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+78); push_effect(gb, b_+78); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+88) { goto L_6983; }
    else if (jt_ == b_+171) { goto L_69d6; }
    else if (jt_ == b_+183) { goto L_69e2; }
    else if (jt_ == b_+193) { goto L_69ec; }
    else if (jt_ == b_+209) { goto L_69fc; }
    else { HANDOFF(HL); }
  } while (0);
L_6983:
  CYC(b_+88, b_+91); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+91, b_+93); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+93, b_+95); goto L_699b; } CYC(b_+93, b_+95);
  CALL_C(b_+95, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+98);
  CYC(b_+98, b_+100); L = 0x42;
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+102); A = alu_dec8(gb, A);
  CYC(b_+102, b_+105); SET_HL(b_+441);
  CYC(b_+105, b_+106); push_effect(gb, b_+106);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+107, b_+108); H = mem_rd(gb, HL);
  CYC(b_+108, b_+109); L = A;
  CYC(b_+109, b_+112); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
L_699b:
  CALL_C(b_+112, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+115);
  CALL_C(b_+115, interactionRunScript_hook, SYM(interactionRunScript), b_+118);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = 0x61;
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+123); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+123, b_+125); goto L_69cb; } CYC(b_+123, b_+125);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x00);
  CYC(b_+127, b_+129); B = 0x04;
L_69ac:
  CALL_C(b_+129, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+132);
  if (!(F & FZ)) { CYCT(b_+132, b_+133); ret_effect(gb); return; } CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x99);
  CYC(b_+135, b_+136); L = alu_inc8(gb, L);
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x02);
  CYC(b_+138, b_+139); L = alu_inc8(gb, L);
  CYC(b_+139, b_+140); mem_wr(gb, HL, B);
  CYC(b_+140, b_+142); E = 0x5a;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+145); L = 0x78;
  CYC(b_+145, b_+146); mem_wr(gb, HL, A);
  PUSH(b_+146, BC);
  CYC(b_+147, b_+149); E = 0x78;
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); B = 0x00;
  CYC(b_+152, b_+153); C = A;
  CALL_C(b_+153, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+156);
  SET_BC(POP(b_+156));
  CYC(b_+157, b_+158); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto L_69ac; } CYC(b_+158, b_+160);
L_69cb:
  CYC(b_+160, b_+162); L = 0x4b;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+165, b_+168); if (hook_enabled_at(gb, SYM(objectSetVisiblec1))) { objectSetVisiblec1_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec1)); } CYC(b_+165, b_+168);
  CYC(b_+168, b_+171); if (hook_enabled_at(gb, SYM(objectSetVisiblec3))) { objectSetVisiblec3_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec3));
L_69d6:
  CALL_C(b_+171, interactionCode57__updateAnimationAndRunScript_hook, b_+183, b_+174);
  if (!(F & FC)) { CYCT(b_+174, b_+175); ret_effect(gb); return; } CYC(b_+174, b_+175);
  CALL_C(b_+175, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+178);
  CYC(b_+178, b_+180); L = 0x46;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 0xd2);
  CYC(b_+182, b_+183); ret_effect(gb); return;
L_69e2:
  CYC(b_+183, b_+185); E = 0x7f;
  CYC(b_+185, b_+186); A = mem_rd(gb, DE);
  CYC(b_+186, b_+187); alu_or(gb, A);
  if ((F & FZ)) { CALL_C_CC(b_+187, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+190); } else CYC(b_+187, b_+190);
  CYC(b_+190, b_+193); if (hook_enabled_at(gb, SYM(interactionRunScript))) { interactionRunScript_hook(gb); return; } HANDOFF(SYM(interactionRunScript));
L_69ec:
  CALL_C(b_+193, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+196);
  CALL_C(b_+196, objectApplySpeed_hook, SYM(objectApplySpeed), b_+199);
  CALL_C(b_+199, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+202);
  if (!(F & FZ)) { CYCT(b_+202, b_+203); ret_effect(gb); return; } CYC(b_+202, b_+203);
  CALL_C(b_+203, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+206);
  CYC(b_+206, b_+209); if (hook_enabled_at(gb, SYM(fadeoutToWhite))) { fadeoutToWhite_hook(gb); return; } HANDOFF(SYM(fadeoutToWhite));
L_69fc:
  CYC(b_+209, b_+212); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+212, b_+213); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+213, b_+214); ret_effect(gb); return; } CYC(b_+213, b_+214);
  CALL_C(b_+214, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+217);
  CYC(b_+217, b_+219); A = 0x06;
  CYC(b_+219, b_+222); mem_wr(gb, wRoomLayoutEnd, A);
  CALL_C(b_+222, disableLcd_hook, SYM(disableLcd), b_+225);
  PUSH(b_+225, DE);
  CYC(b_+226, b_+229); SET_BC(0x0138);
  CYC(b_+229, b_+231); A = 0x00;
  CALL_C(b_+231, forceLoadRoom_hook, SYM(forceLoadRoom), b_+234);
  CYC(b_+234, b_+236); A = 0x2d;
  CALL_C(b_+236, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+239);
  CYC(b_+239, b_+241); A = 0x30;
  CALL_C(b_+241, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+244);
  CYC(b_+244, b_+246); A = 0x84;
  CALL_C(b_+246, loadGfxHeader_hook, SYM(loadGfxHeader), b_+249);
  CYC(b_+249, b_+251); A = 0xff;
  CYC(b_+251, b_+254); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+254, b_+256); A = 0x04;
  CALL_C(b_+256, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+259);
  SET_DE(POP(b_+259));
  CYC(b_+260, b_+263); SET_BC((SYM(shopkeeperCheckLinkHasItemAlready) + 15));
  CALL_C(b_+263, interactionSetPosition_hook, SYM(interactionSetPosition), b_+266);
  CYC(b_+266, b_+268); A = 0x02;
  CYC(b_+268, b_+271); SET_HL(b_+441);
  CYC(b_+271, b_+272); push_effect(gb, b_+272);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+272, b_+273); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+273, b_+274); H = mem_rd(gb, HL);
  CYC(b_+274, b_+275); L = A;
  CALL_C(b_+275, interactionSetScript_hook, SYM(interactionSetScript), b_+278);
  CYC(b_+278, b_+281); if (hook_enabled_at(gb, SYM(fadeinFromWhite))) { fadeinFromWhite_hook(gb); return; } HANDOFF(SYM(fadeinFromWhite));
L_6a44:
  CALL_C(b_+281, checkInteractionState_hook, SYM(checkInteractionState), b_+284);
  if (!(F & FZ)) { CYCT(b_+284, b_+286); goto L_6a4c; } CYC(b_+284, b_+286);
  CYC(b_+286, b_+289); goto L_6965;
L_6a4c:
  CYC(b_+289, b_+291); E = 0x45;
  CYC(b_+291, b_+292); A = mem_rd(gb, DE);
  CYC(b_+292, b_+293); push_effect(gb, b_+293); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+88) { goto L_6983; }
    else if (jt_ == b_+183) { goto L_69e2; }
    else if (jt_ == b_+301) { goto L_6a58; }
    else if (jt_ == b_+308) { goto L_6a5f; }
    else { HANDOFF(HL); }
  } while (0);
L_6a58:
  CALL_C(b_+301, interactionCode57__updateAnimationAndRunScript_hook, b_+183, b_+304);
  if (!(F & FC)) { CYCT(b_+304, b_+305); ret_effect(gb); return; } CYC(b_+304, b_+305);
  CALL_C(b_+305, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+308);
L_6a5f:
  CALL_C(b_+308, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+311);
  CALL_C(b_+311, objectApplySpeed_hook, SYM(objectApplySpeed), b_+314);
  CYC(b_+314, b_+317); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+317, b_+319); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+319, b_+320); ret_effect(gb); return; } CYC(b_+319, b_+320);
  CALL_C(b_+320, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+323);
  CYC(b_+323, b_+326); SET_BC(0x388a);
  CALL_C(b_+326, interactionSetPosition_hook, SYM(interactionSetPosition), b_+329);
  CYC(b_+329, b_+331); A = 0x03;
  CYC(b_+331, b_+334); SET_HL(b_+441);
  CYC(b_+334, b_+335); push_effect(gb, b_+335);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+335, b_+336); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+336, b_+337); H = mem_rd(gb, HL);
  CYC(b_+337, b_+338); L = A;
  CYC(b_+338, b_+341); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
L_6aa4:
  CYC(b_+377, b_+378); C = A;
  CYC(b_+378, b_+380); B = 0x02;
L_6aa7:
  CALL_C(b_+380, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+383);
  if (!(F & FZ)) { CYCT(b_+383, b_+384); ret_effect(gb); return; } CYC(b_+383, b_+384);
  CYC(b_+384, b_+386); mem_wr(gb, HL, 0x92);
  CYC(b_+386, b_+387); L = alu_inc8(gb, L);
  CYC(b_+387, b_+389); mem_wr(gb, HL, 0x06);
  CYC(b_+389, b_+390); L = alu_inc8(gb, L);
  CYC(b_+390, b_+391); mem_wr(gb, HL, C);
  CYC(b_+391, b_+393); E = 0x5a;
  CYC(b_+393, b_+394); A = mem_rd(gb, DE);
  CYC(b_+394, b_+396); alu_and(gb, 0x03);
  CYC(b_+396, b_+398); L = 0x47;
  CYC(b_+398, b_+399); mem_wr(gb, HL, A);
  CYC(b_+399, b_+401); L = 0x49;
  CYC(b_+401, b_+402); mem_wr(gb, HL, B);
  CYC(b_+402, b_+403); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  PUSH(b_+403, BC);
  CALL_C(b_+404, objectCopyPosition_hook, SYM(objectCopyPosition), b_+407);
  SET_BC(POP(b_+407));
  CYC(b_+408, b_+410); L = 0x4b;
  CYC(b_+410, b_+411); A = mem_rd(gb, HL);
  CYC(b_+411, b_+413); alu_add(gb, 0x04);
  CYC(b_+413, b_+414); mem_wr(gb, HL, A);
  CYC(b_+414, b_+416); E = 0x61;
  CYC(b_+416, b_+417); A = mem_rd(gb, DE);
  CYC(b_+417, b_+419); alu_cp(gb, 0x01);
  CYC(b_+419, b_+421); L = 0x4d;
  CYC(b_+421, b_+422); A = mem_rd(gb, HL);
  if ((F & FZ)) { CYCT(b_+422, b_+424); goto L_6ad5; } CYC(b_+422, b_+424);
  CYC(b_+424, b_+426); alu_add(gb, 0x1c);
L_6ad5:
  CYC(b_+426, b_+428); alu_sub(gb, 0x0e);
  CYC(b_+428, b_+429); mem_wr(gb, HL, A);
  CYC(b_+429, b_+430); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+430, b_+432); goto L_6aa7; } CYC(b_+430, b_+432);
  CYC(b_+432, b_+433); ret_effect(gb); return;
}

void interactionCode57__subid00_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6937:
  CALL_C(b_+12, checkInteractionState_hook, SYM(checkInteractionState), b_+15);
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto L_6942; } CYC(b_+15, b_+17);
L_693c:
  CALL_C(b_+17, interactionCode57__loadScriptAndInitGraphics_hook, b_+350, b_+20);
L_693f:
  CALL_C(b_+20, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+23);
L_6942:
  CALL_C(b_+23, interactionRunScript_hook, SYM(interactionRunScript), b_+26);
  if ((F & FC)) { CYCT(b_+26, b_+29); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+26, b_+29);
  CALL_C(b_+29, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+32);
  CYC(b_+32, b_+34); E = 0x61;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+36, b_+37); ret_effect(gb); return; } CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); A = 0x50;
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);
  CYC(b_+42, b_+45); A = mem_rd(gb, wScreenVariables);
  CYC(b_+45, b_+47); alu_and(gb, 0x01);
  if ((F & FZ)) { CYCT(b_+47, b_+48); ret_effect(gb); return; } CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); A = 0x03;
  CYC(b_+50, b_+53); goto L_6aa4;
L_6aa4:
  CYC(b_+377, b_+378); C = A;
  CYC(b_+378, b_+380); B = 0x02;
L_6aa7:
  CALL_C(b_+380, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+383);
  if (!(F & FZ)) { CYCT(b_+383, b_+384); ret_effect(gb); return; } CYC(b_+383, b_+384);
  CYC(b_+384, b_+386); mem_wr(gb, HL, 0x92);
  CYC(b_+386, b_+387); L = alu_inc8(gb, L);
  CYC(b_+387, b_+389); mem_wr(gb, HL, 0x06);
  CYC(b_+389, b_+390); L = alu_inc8(gb, L);
  CYC(b_+390, b_+391); mem_wr(gb, HL, C);
  CYC(b_+391, b_+393); E = 0x5a;
  CYC(b_+393, b_+394); A = mem_rd(gb, DE);
  CYC(b_+394, b_+396); alu_and(gb, 0x03);
  CYC(b_+396, b_+398); L = 0x47;
  CYC(b_+398, b_+399); mem_wr(gb, HL, A);
  CYC(b_+399, b_+401); L = 0x49;
  CYC(b_+401, b_+402); mem_wr(gb, HL, B);
  CYC(b_+402, b_+403); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  PUSH(b_+403, BC);
  CALL_C(b_+404, objectCopyPosition_hook, SYM(objectCopyPosition), b_+407);
  SET_BC(POP(b_+407));
  CYC(b_+408, b_+410); L = 0x4b;
  CYC(b_+410, b_+411); A = mem_rd(gb, HL);
  CYC(b_+411, b_+413); alu_add(gb, 0x04);
  CYC(b_+413, b_+414); mem_wr(gb, HL, A);
  CYC(b_+414, b_+416); E = 0x61;
  CYC(b_+416, b_+417); A = mem_rd(gb, DE);
  CYC(b_+417, b_+419); alu_cp(gb, 0x01);
  CYC(b_+419, b_+421); L = 0x4d;
  CYC(b_+421, b_+422); A = mem_rd(gb, HL);
  if ((F & FZ)) { CYCT(b_+422, b_+424); goto L_6ad5; } CYC(b_+422, b_+424);
  CYC(b_+424, b_+426); alu_add(gb, 0x1c);
L_6ad5:
  CYC(b_+426, b_+428); alu_sub(gb, 0x0e);
  CYC(b_+428, b_+429); mem_wr(gb, HL, A);
  CYC(b_+429, b_+430); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+430, b_+432); goto L_6aa7; } CYC(b_+430, b_+432);
  CYC(b_+432, b_+433); ret_effect(gb); return;
}

void interactionCode57__subid03__state0_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_693c:
  CALL_C(b_+17, interactionCode57__loadScriptAndInitGraphics_hook, b_+350, b_+20);
L_693f:
  CALL_C(b_+20, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+23);
L_6942:
  CALL_C(b_+23, interactionRunScript_hook, SYM(interactionRunScript), b_+26);
  if ((F & FC)) { CYCT(b_+26, b_+29); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+26, b_+29);
  CALL_C(b_+29, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+32);
  CYC(b_+32, b_+34); E = 0x61;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+36, b_+37); ret_effect(gb); return; } CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); A = 0x50;
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);
  CYC(b_+42, b_+45); A = mem_rd(gb, wScreenVariables);
  CYC(b_+45, b_+47); alu_and(gb, 0x01);
  if ((F & FZ)) { CYCT(b_+47, b_+48); ret_effect(gb); return; } CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); A = 0x03;
  CYC(b_+50, b_+53); goto L_6aa4;
L_6aa4:
  CYC(b_+377, b_+378); C = A;
  CYC(b_+378, b_+380); B = 0x02;
L_6aa7:
  CALL_C(b_+380, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+383);
  if (!(F & FZ)) { CYCT(b_+383, b_+384); ret_effect(gb); return; } CYC(b_+383, b_+384);
  CYC(b_+384, b_+386); mem_wr(gb, HL, 0x92);
  CYC(b_+386, b_+387); L = alu_inc8(gb, L);
  CYC(b_+387, b_+389); mem_wr(gb, HL, 0x06);
  CYC(b_+389, b_+390); L = alu_inc8(gb, L);
  CYC(b_+390, b_+391); mem_wr(gb, HL, C);
  CYC(b_+391, b_+393); E = 0x5a;
  CYC(b_+393, b_+394); A = mem_rd(gb, DE);
  CYC(b_+394, b_+396); alu_and(gb, 0x03);
  CYC(b_+396, b_+398); L = 0x47;
  CYC(b_+398, b_+399); mem_wr(gb, HL, A);
  CYC(b_+399, b_+401); L = 0x49;
  CYC(b_+401, b_+402); mem_wr(gb, HL, B);
  CYC(b_+402, b_+403); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  PUSH(b_+403, BC);
  CALL_C(b_+404, objectCopyPosition_hook, SYM(objectCopyPosition), b_+407);
  SET_BC(POP(b_+407));
  CYC(b_+408, b_+410); L = 0x4b;
  CYC(b_+410, b_+411); A = mem_rd(gb, HL);
  CYC(b_+411, b_+413); alu_add(gb, 0x04);
  CYC(b_+413, b_+414); mem_wr(gb, HL, A);
  CYC(b_+414, b_+416); E = 0x61;
  CYC(b_+416, b_+417); A = mem_rd(gb, DE);
  CYC(b_+417, b_+419); alu_cp(gb, 0x01);
  CYC(b_+419, b_+421); L = 0x4d;
  CYC(b_+421, b_+422); A = mem_rd(gb, HL);
  if ((F & FZ)) { CYCT(b_+422, b_+424); goto L_6ad5; } CYC(b_+422, b_+424);
  CYC(b_+424, b_+426); alu_add(gb, 0x1c);
L_6ad5:
  CYC(b_+426, b_+428); alu_sub(gb, 0x0e);
  CYC(b_+428, b_+429); mem_wr(gb, HL, A);
  CYC(b_+429, b_+430); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+430, b_+432); goto L_6aa7; } CYC(b_+430, b_+432);
  CYC(b_+432, b_+433); ret_effect(gb); return;
}

void interactionCode57__afterCall693f_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_693f:
  CALL_C(b_+20, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+23);
L_6942:
  CALL_C(b_+23, interactionRunScript_hook, SYM(interactionRunScript), b_+26);
  if ((F & FC)) { CYCT(b_+26, b_+29); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+26, b_+29);
  CALL_C(b_+29, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+32);
  CYC(b_+32, b_+34); E = 0x61;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+36, b_+37); ret_effect(gb); return; } CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); A = 0x50;
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);
  CYC(b_+42, b_+45); A = mem_rd(gb, wScreenVariables);
  CYC(b_+45, b_+47); alu_and(gb, 0x01);
  if ((F & FZ)) { CYCT(b_+47, b_+48); ret_effect(gb); return; } CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); A = 0x03;
  CYC(b_+50, b_+53); goto L_6aa4;
L_6aa4:
  CYC(b_+377, b_+378); C = A;
  CYC(b_+378, b_+380); B = 0x02;
L_6aa7:
  CALL_C(b_+380, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+383);
  if (!(F & FZ)) { CYCT(b_+383, b_+384); ret_effect(gb); return; } CYC(b_+383, b_+384);
  CYC(b_+384, b_+386); mem_wr(gb, HL, 0x92);
  CYC(b_+386, b_+387); L = alu_inc8(gb, L);
  CYC(b_+387, b_+389); mem_wr(gb, HL, 0x06);
  CYC(b_+389, b_+390); L = alu_inc8(gb, L);
  CYC(b_+390, b_+391); mem_wr(gb, HL, C);
  CYC(b_+391, b_+393); E = 0x5a;
  CYC(b_+393, b_+394); A = mem_rd(gb, DE);
  CYC(b_+394, b_+396); alu_and(gb, 0x03);
  CYC(b_+396, b_+398); L = 0x47;
  CYC(b_+398, b_+399); mem_wr(gb, HL, A);
  CYC(b_+399, b_+401); L = 0x49;
  CYC(b_+401, b_+402); mem_wr(gb, HL, B);
  CYC(b_+402, b_+403); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  PUSH(b_+403, BC);
  CALL_C(b_+404, objectCopyPosition_hook, SYM(objectCopyPosition), b_+407);
  SET_BC(POP(b_+407));
  CYC(b_+408, b_+410); L = 0x4b;
  CYC(b_+410, b_+411); A = mem_rd(gb, HL);
  CYC(b_+411, b_+413); alu_add(gb, 0x04);
  CYC(b_+413, b_+414); mem_wr(gb, HL, A);
  CYC(b_+414, b_+416); E = 0x61;
  CYC(b_+416, b_+417); A = mem_rd(gb, DE);
  CYC(b_+417, b_+419); alu_cp(gb, 0x01);
  CYC(b_+419, b_+421); L = 0x4d;
  CYC(b_+421, b_+422); A = mem_rd(gb, HL);
  if ((F & FZ)) { CYCT(b_+422, b_+424); goto L_6ad5; } CYC(b_+422, b_+424);
  CYC(b_+424, b_+426); alu_add(gb, 0x1c);
L_6ad5:
  CYC(b_+426, b_+428); alu_sub(gb, 0x0e);
  CYC(b_+428, b_+429); mem_wr(gb, HL, A);
  CYC(b_+429, b_+430); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+430, b_+432); goto L_6aa7; } CYC(b_+430, b_+432);
  CYC(b_+432, b_+433); ret_effect(gb); return;
}

void interactionCode57__subid03__state1_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6942:
  CALL_C(b_+23, interactionRunScript_hook, SYM(interactionRunScript), b_+26);
  if ((F & FC)) { CYCT(b_+26, b_+29); if (hook_enabled_at(gb, SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+26, b_+29);
  CALL_C(b_+29, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+32);
  CYC(b_+32, b_+34); E = 0x61;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+36, b_+37); ret_effect(gb); return; } CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); A = 0x50;
  CALL_C(b_+39, playSound_b00_hook, SYM(playSound_b00), b_+42);
  CYC(b_+42, b_+45); A = mem_rd(gb, wScreenVariables);
  CYC(b_+45, b_+47); alu_and(gb, 0x01);
  if ((F & FZ)) { CYCT(b_+47, b_+48); ret_effect(gb); return; } CYC(b_+47, b_+48);
  CYC(b_+48, b_+50); A = 0x03;
  CYC(b_+50, b_+53); goto L_6aa4;
L_6aa4:
  CYC(b_+377, b_+378); C = A;
  CYC(b_+378, b_+380); B = 0x02;
L_6aa7:
  CALL_C(b_+380, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+383);
  if (!(F & FZ)) { CYCT(b_+383, b_+384); ret_effect(gb); return; } CYC(b_+383, b_+384);
  CYC(b_+384, b_+386); mem_wr(gb, HL, 0x92);
  CYC(b_+386, b_+387); L = alu_inc8(gb, L);
  CYC(b_+387, b_+389); mem_wr(gb, HL, 0x06);
  CYC(b_+389, b_+390); L = alu_inc8(gb, L);
  CYC(b_+390, b_+391); mem_wr(gb, HL, C);
  CYC(b_+391, b_+393); E = 0x5a;
  CYC(b_+393, b_+394); A = mem_rd(gb, DE);
  CYC(b_+394, b_+396); alu_and(gb, 0x03);
  CYC(b_+396, b_+398); L = 0x47;
  CYC(b_+398, b_+399); mem_wr(gb, HL, A);
  CYC(b_+399, b_+401); L = 0x49;
  CYC(b_+401, b_+402); mem_wr(gb, HL, B);
  CYC(b_+402, b_+403); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  PUSH(b_+403, BC);
  CALL_C(b_+404, objectCopyPosition_hook, SYM(objectCopyPosition), b_+407);
  SET_BC(POP(b_+407));
  CYC(b_+408, b_+410); L = 0x4b;
  CYC(b_+410, b_+411); A = mem_rd(gb, HL);
  CYC(b_+411, b_+413); alu_add(gb, 0x04);
  CYC(b_+413, b_+414); mem_wr(gb, HL, A);
  CYC(b_+414, b_+416); E = 0x61;
  CYC(b_+416, b_+417); A = mem_rd(gb, DE);
  CYC(b_+417, b_+419); alu_cp(gb, 0x01);
  CYC(b_+419, b_+421); L = 0x4d;
  CYC(b_+421, b_+422); A = mem_rd(gb, HL);
  if ((F & FZ)) { CYCT(b_+422, b_+424); goto L_6ad5; } CYC(b_+422, b_+424);
  CYC(b_+424, b_+426); alu_add(gb, 0x1c);
L_6ad5:
  CYC(b_+426, b_+428); alu_sub(gb, 0x0e);
  CYC(b_+428, b_+429); mem_wr(gb, HL, A);
  CYC(b_+429, b_+430); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+430, b_+432); goto L_6aa7; } CYC(b_+430, b_+432);
  CYC(b_+432, b_+433); ret_effect(gb); return;
}

void interactionCode57__subid01_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6960:
  CALL_C(b_+53, checkInteractionState_hook, SYM(checkInteractionState), b_+56);
  if (!(F & FZ)) { CYCT(b_+56, b_+58); goto L_6975; } CYC(b_+56, b_+58);
L_6965:
  CYC(b_+58, b_+60); E = 0x42;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); A = alu_dec8(gb, A);
  CYC(b_+62, b_+64); A = 0x0c;
  if ((F & FZ)) { CYCT(b_+64, b_+66); goto L_696f; } CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); A = 0xf4;
L_696f:
  CYC(b_+68, b_+70); E = 0x78;
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CALL_C(b_+71, interactionCode57__loadScriptAndInitGraphics_hook, b_+350, b_+74);
L_6975:
  CYC(b_+74, b_+76); E = 0x45;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+78); push_effect(gb, b_+78); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+88) { goto L_6983; }
    else if (jt_ == b_+171) { goto L_69d6; }
    else if (jt_ == b_+183) { goto L_69e2; }
    else if (jt_ == b_+193) { goto L_69ec; }
    else if (jt_ == b_+209) { goto L_69fc; }
    else { HANDOFF(HL); }
  } while (0);
L_6983:
  CYC(b_+88, b_+91); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+91, b_+93); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+93, b_+95); goto L_699b; } CYC(b_+93, b_+95);
  CALL_C(b_+95, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+98);
  CYC(b_+98, b_+100); L = 0x42;
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+102); A = alu_dec8(gb, A);
  CYC(b_+102, b_+105); SET_HL(b_+441);
  CYC(b_+105, b_+106); push_effect(gb, b_+106);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+107, b_+108); H = mem_rd(gb, HL);
  CYC(b_+108, b_+109); L = A;
  CYC(b_+109, b_+112); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
L_699b:
  CALL_C(b_+112, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+115);
  CALL_C(b_+115, interactionRunScript_hook, SYM(interactionRunScript), b_+118);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = 0x61;
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+123); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+123, b_+125); goto L_69cb; } CYC(b_+123, b_+125);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x00);
  CYC(b_+127, b_+129); B = 0x04;
L_69ac:
  CALL_C(b_+129, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+132);
  if (!(F & FZ)) { CYCT(b_+132, b_+133); ret_effect(gb); return; } CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x99);
  CYC(b_+135, b_+136); L = alu_inc8(gb, L);
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x02);
  CYC(b_+138, b_+139); L = alu_inc8(gb, L);
  CYC(b_+139, b_+140); mem_wr(gb, HL, B);
  CYC(b_+140, b_+142); E = 0x5a;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+145); L = 0x78;
  CYC(b_+145, b_+146); mem_wr(gb, HL, A);
  PUSH(b_+146, BC);
  CYC(b_+147, b_+149); E = 0x78;
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); B = 0x00;
  CYC(b_+152, b_+153); C = A;
  CALL_C(b_+153, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+156);
  SET_BC(POP(b_+156));
  CYC(b_+157, b_+158); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto L_69ac; } CYC(b_+158, b_+160);
L_69cb:
  CYC(b_+160, b_+162); L = 0x4b;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+165, b_+168); if (hook_enabled_at(gb, SYM(objectSetVisiblec1))) { objectSetVisiblec1_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec1)); } CYC(b_+165, b_+168);
  CYC(b_+168, b_+171); if (hook_enabled_at(gb, SYM(objectSetVisiblec3))) { objectSetVisiblec3_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec3));
L_69d6:
  CALL_C(b_+171, interactionCode57__updateAnimationAndRunScript_hook, b_+183, b_+174);
  if (!(F & FC)) { CYCT(b_+174, b_+175); ret_effect(gb); return; } CYC(b_+174, b_+175);
  CALL_C(b_+175, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+178);
  CYC(b_+178, b_+180); L = 0x46;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 0xd2);
  CYC(b_+182, b_+183); ret_effect(gb); return;
L_69e2:
  CYC(b_+183, b_+185); E = 0x7f;
  CYC(b_+185, b_+186); A = mem_rd(gb, DE);
  CYC(b_+186, b_+187); alu_or(gb, A);
  if ((F & FZ)) { CALL_C_CC(b_+187, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+190); } else CYC(b_+187, b_+190);
  CYC(b_+190, b_+193); if (hook_enabled_at(gb, SYM(interactionRunScript))) { interactionRunScript_hook(gb); return; } HANDOFF(SYM(interactionRunScript));
L_69ec:
  CALL_C(b_+193, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+196);
  CALL_C(b_+196, objectApplySpeed_hook, SYM(objectApplySpeed), b_+199);
  CALL_C(b_+199, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+202);
  if (!(F & FZ)) { CYCT(b_+202, b_+203); ret_effect(gb); return; } CYC(b_+202, b_+203);
  CALL_C(b_+203, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+206);
  CYC(b_+206, b_+209); if (hook_enabled_at(gb, SYM(fadeoutToWhite))) { fadeoutToWhite_hook(gb); return; } HANDOFF(SYM(fadeoutToWhite));
L_69fc:
  CYC(b_+209, b_+212); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+212, b_+213); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+213, b_+214); ret_effect(gb); return; } CYC(b_+213, b_+214);
  CALL_C(b_+214, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+217);
  CYC(b_+217, b_+219); A = 0x06;
  CYC(b_+219, b_+222); mem_wr(gb, wRoomLayoutEnd, A);
  CALL_C(b_+222, disableLcd_hook, SYM(disableLcd), b_+225);
  PUSH(b_+225, DE);
  CYC(b_+226, b_+229); SET_BC(0x0138);
  CYC(b_+229, b_+231); A = 0x00;
  CALL_C(b_+231, forceLoadRoom_hook, SYM(forceLoadRoom), b_+234);
  CYC(b_+234, b_+236); A = 0x2d;
  CALL_C(b_+236, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+239);
  CYC(b_+239, b_+241); A = 0x30;
  CALL_C(b_+241, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+244);
  CYC(b_+244, b_+246); A = 0x84;
  CALL_C(b_+246, loadGfxHeader_hook, SYM(loadGfxHeader), b_+249);
  CYC(b_+249, b_+251); A = 0xff;
  CYC(b_+251, b_+254); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+254, b_+256); A = 0x04;
  CALL_C(b_+256, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+259);
  SET_DE(POP(b_+259));
  CYC(b_+260, b_+263); SET_BC((SYM(shopkeeperCheckLinkHasItemAlready) + 15));
  CALL_C(b_+263, interactionSetPosition_hook, SYM(interactionSetPosition), b_+266);
  CYC(b_+266, b_+268); A = 0x02;
  CYC(b_+268, b_+271); SET_HL(b_+441);
  CYC(b_+271, b_+272); push_effect(gb, b_+272);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+272, b_+273); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+273, b_+274); H = mem_rd(gb, HL);
  CYC(b_+274, b_+275); L = A;
  CALL_C(b_+275, interactionSetScript_hook, SYM(interactionSetScript), b_+278);
  CYC(b_+278, b_+281); if (hook_enabled_at(gb, SYM(fadeinFromWhite))) { fadeinFromWhite_hook(gb); return; } HANDOFF(SYM(fadeinFromWhite));
}

void interactionCode57__subid1And2State0_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6965:
  CYC(b_+58, b_+60); E = 0x42;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); A = alu_dec8(gb, A);
  CYC(b_+62, b_+64); A = 0x0c;
  if ((F & FZ)) { CYCT(b_+64, b_+66); goto L_696f; } CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); A = 0xf4;
L_696f:
  CYC(b_+68, b_+70); E = 0x78;
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CALL_C(b_+71, interactionCode57__loadScriptAndInitGraphics_hook, b_+350, b_+74);
L_6975:
  CYC(b_+74, b_+76); E = 0x45;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+78); push_effect(gb, b_+78); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+88) { goto L_6983; }
    else if (jt_ == b_+171) { goto L_69d6; }
    else if (jt_ == b_+183) { goto L_69e2; }
    else if (jt_ == b_+193) { goto L_69ec; }
    else if (jt_ == b_+209) { goto L_69fc; }
    else { HANDOFF(HL); }
  } while (0);
L_6983:
  CYC(b_+88, b_+91); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+91, b_+93); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+93, b_+95); goto L_699b; } CYC(b_+93, b_+95);
  CALL_C(b_+95, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+98);
  CYC(b_+98, b_+100); L = 0x42;
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+102); A = alu_dec8(gb, A);
  CYC(b_+102, b_+105); SET_HL(b_+441);
  CYC(b_+105, b_+106); push_effect(gb, b_+106);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+107, b_+108); H = mem_rd(gb, HL);
  CYC(b_+108, b_+109); L = A;
  CYC(b_+109, b_+112); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
L_699b:
  CALL_C(b_+112, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+115);
  CALL_C(b_+115, interactionRunScript_hook, SYM(interactionRunScript), b_+118);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = 0x61;
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+123); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+123, b_+125); goto L_69cb; } CYC(b_+123, b_+125);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x00);
  CYC(b_+127, b_+129); B = 0x04;
L_69ac:
  CALL_C(b_+129, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+132);
  if (!(F & FZ)) { CYCT(b_+132, b_+133); ret_effect(gb); return; } CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x99);
  CYC(b_+135, b_+136); L = alu_inc8(gb, L);
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x02);
  CYC(b_+138, b_+139); L = alu_inc8(gb, L);
  CYC(b_+139, b_+140); mem_wr(gb, HL, B);
  CYC(b_+140, b_+142); E = 0x5a;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+145); L = 0x78;
  CYC(b_+145, b_+146); mem_wr(gb, HL, A);
  PUSH(b_+146, BC);
  CYC(b_+147, b_+149); E = 0x78;
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); B = 0x00;
  CYC(b_+152, b_+153); C = A;
  CALL_C(b_+153, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+156);
  SET_BC(POP(b_+156));
  CYC(b_+157, b_+158); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto L_69ac; } CYC(b_+158, b_+160);
L_69cb:
  CYC(b_+160, b_+162); L = 0x4b;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+165, b_+168); if (hook_enabled_at(gb, SYM(objectSetVisiblec1))) { objectSetVisiblec1_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec1)); } CYC(b_+165, b_+168);
  CYC(b_+168, b_+171); if (hook_enabled_at(gb, SYM(objectSetVisiblec3))) { objectSetVisiblec3_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec3));
L_69d6:
  CALL_C(b_+171, interactionCode57__updateAnimationAndRunScript_hook, b_+183, b_+174);
  if (!(F & FC)) { CYCT(b_+174, b_+175); ret_effect(gb); return; } CYC(b_+174, b_+175);
  CALL_C(b_+175, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+178);
  CYC(b_+178, b_+180); L = 0x46;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 0xd2);
  CYC(b_+182, b_+183); ret_effect(gb); return;
L_69e2:
  CYC(b_+183, b_+185); E = 0x7f;
  CYC(b_+185, b_+186); A = mem_rd(gb, DE);
  CYC(b_+186, b_+187); alu_or(gb, A);
  if ((F & FZ)) { CALL_C_CC(b_+187, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+190); } else CYC(b_+187, b_+190);
  CYC(b_+190, b_+193); if (hook_enabled_at(gb, SYM(interactionRunScript))) { interactionRunScript_hook(gb); return; } HANDOFF(SYM(interactionRunScript));
L_69ec:
  CALL_C(b_+193, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+196);
  CALL_C(b_+196, objectApplySpeed_hook, SYM(objectApplySpeed), b_+199);
  CALL_C(b_+199, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+202);
  if (!(F & FZ)) { CYCT(b_+202, b_+203); ret_effect(gb); return; } CYC(b_+202, b_+203);
  CALL_C(b_+203, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+206);
  CYC(b_+206, b_+209); if (hook_enabled_at(gb, SYM(fadeoutToWhite))) { fadeoutToWhite_hook(gb); return; } HANDOFF(SYM(fadeoutToWhite));
L_69fc:
  CYC(b_+209, b_+212); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+212, b_+213); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+213, b_+214); ret_effect(gb); return; } CYC(b_+213, b_+214);
  CALL_C(b_+214, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+217);
  CYC(b_+217, b_+219); A = 0x06;
  CYC(b_+219, b_+222); mem_wr(gb, wRoomLayoutEnd, A);
  CALL_C(b_+222, disableLcd_hook, SYM(disableLcd), b_+225);
  PUSH(b_+225, DE);
  CYC(b_+226, b_+229); SET_BC(0x0138);
  CYC(b_+229, b_+231); A = 0x00;
  CALL_C(b_+231, forceLoadRoom_hook, SYM(forceLoadRoom), b_+234);
  CYC(b_+234, b_+236); A = 0x2d;
  CALL_C(b_+236, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+239);
  CYC(b_+239, b_+241); A = 0x30;
  CALL_C(b_+241, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+244);
  CYC(b_+244, b_+246); A = 0x84;
  CALL_C(b_+246, loadGfxHeader_hook, SYM(loadGfxHeader), b_+249);
  CYC(b_+249, b_+251); A = 0xff;
  CYC(b_+251, b_+254); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+254, b_+256); A = 0x04;
  CALL_C(b_+256, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+259);
  SET_DE(POP(b_+259));
  CYC(b_+260, b_+263); SET_BC((SYM(shopkeeperCheckLinkHasItemAlready) + 15));
  CALL_C(b_+263, interactionSetPosition_hook, SYM(interactionSetPosition), b_+266);
  CYC(b_+266, b_+268); A = 0x02;
  CYC(b_+268, b_+271); SET_HL(b_+441);
  CYC(b_+271, b_+272); push_effect(gb, b_+272);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+272, b_+273); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+273, b_+274); H = mem_rd(gb, HL);
  CYC(b_+274, b_+275); L = A;
  CALL_C(b_+275, interactionSetScript_hook, SYM(interactionSetScript), b_+278);
  CYC(b_+278, b_+281); if (hook_enabled_at(gb, SYM(fadeinFromWhite))) { fadeinFromWhite_hook(gb); return; } HANDOFF(SYM(fadeinFromWhite));
}

void interactionCode57__subid1State1_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6975:
  CYC(b_+74, b_+76); E = 0x45;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+78); push_effect(gb, b_+78); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+88) { goto L_6983; }
    else if (jt_ == b_+171) { goto L_69d6; }
    else if (jt_ == b_+183) { goto L_69e2; }
    else if (jt_ == b_+193) { goto L_69ec; }
    else if (jt_ == b_+209) { goto L_69fc; }
    else { HANDOFF(HL); }
  } while (0);
L_6983:
  CYC(b_+88, b_+91); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+91, b_+93); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+93, b_+95); goto L_699b; } CYC(b_+93, b_+95);
  CALL_C(b_+95, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+98);
  CYC(b_+98, b_+100); L = 0x42;
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+102); A = alu_dec8(gb, A);
  CYC(b_+102, b_+105); SET_HL(b_+441);
  CYC(b_+105, b_+106); push_effect(gb, b_+106);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+107, b_+108); H = mem_rd(gb, HL);
  CYC(b_+108, b_+109); L = A;
  CYC(b_+109, b_+112); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
L_699b:
  CALL_C(b_+112, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+115);
  CALL_C(b_+115, interactionRunScript_hook, SYM(interactionRunScript), b_+118);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = 0x61;
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+123); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+123, b_+125); goto L_69cb; } CYC(b_+123, b_+125);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x00);
  CYC(b_+127, b_+129); B = 0x04;
L_69ac:
  CALL_C(b_+129, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+132);
  if (!(F & FZ)) { CYCT(b_+132, b_+133); ret_effect(gb); return; } CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x99);
  CYC(b_+135, b_+136); L = alu_inc8(gb, L);
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x02);
  CYC(b_+138, b_+139); L = alu_inc8(gb, L);
  CYC(b_+139, b_+140); mem_wr(gb, HL, B);
  CYC(b_+140, b_+142); E = 0x5a;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+145); L = 0x78;
  CYC(b_+145, b_+146); mem_wr(gb, HL, A);
  PUSH(b_+146, BC);
  CYC(b_+147, b_+149); E = 0x78;
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); B = 0x00;
  CYC(b_+152, b_+153); C = A;
  CALL_C(b_+153, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+156);
  SET_BC(POP(b_+156));
  CYC(b_+157, b_+158); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto L_69ac; } CYC(b_+158, b_+160);
L_69cb:
  CYC(b_+160, b_+162); L = 0x4b;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+165, b_+168); if (hook_enabled_at(gb, SYM(objectSetVisiblec1))) { objectSetVisiblec1_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec1)); } CYC(b_+165, b_+168);
  CYC(b_+168, b_+171); if (hook_enabled_at(gb, SYM(objectSetVisiblec3))) { objectSetVisiblec3_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec3));
L_69d6:
  CALL_C(b_+171, interactionCode57__updateAnimationAndRunScript_hook, b_+183, b_+174);
  if (!(F & FC)) { CYCT(b_+174, b_+175); ret_effect(gb); return; } CYC(b_+174, b_+175);
  CALL_C(b_+175, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+178);
  CYC(b_+178, b_+180); L = 0x46;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 0xd2);
  CYC(b_+182, b_+183); ret_effect(gb); return;
L_69e2:
  CYC(b_+183, b_+185); E = 0x7f;
  CYC(b_+185, b_+186); A = mem_rd(gb, DE);
  CYC(b_+186, b_+187); alu_or(gb, A);
  if ((F & FZ)) { CALL_C_CC(b_+187, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+190); } else CYC(b_+187, b_+190);
  CYC(b_+190, b_+193); if (hook_enabled_at(gb, SYM(interactionRunScript))) { interactionRunScript_hook(gb); return; } HANDOFF(SYM(interactionRunScript));
L_69ec:
  CALL_C(b_+193, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+196);
  CALL_C(b_+196, objectApplySpeed_hook, SYM(objectApplySpeed), b_+199);
  CALL_C(b_+199, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+202);
  if (!(F & FZ)) { CYCT(b_+202, b_+203); ret_effect(gb); return; } CYC(b_+202, b_+203);
  CALL_C(b_+203, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+206);
  CYC(b_+206, b_+209); if (hook_enabled_at(gb, SYM(fadeoutToWhite))) { fadeoutToWhite_hook(gb); return; } HANDOFF(SYM(fadeoutToWhite));
L_69fc:
  CYC(b_+209, b_+212); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+212, b_+213); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+213, b_+214); ret_effect(gb); return; } CYC(b_+213, b_+214);
  CALL_C(b_+214, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+217);
  CYC(b_+217, b_+219); A = 0x06;
  CYC(b_+219, b_+222); mem_wr(gb, wRoomLayoutEnd, A);
  CALL_C(b_+222, disableLcd_hook, SYM(disableLcd), b_+225);
  PUSH(b_+225, DE);
  CYC(b_+226, b_+229); SET_BC(0x0138);
  CYC(b_+229, b_+231); A = 0x00;
  CALL_C(b_+231, forceLoadRoom_hook, SYM(forceLoadRoom), b_+234);
  CYC(b_+234, b_+236); A = 0x2d;
  CALL_C(b_+236, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+239);
  CYC(b_+239, b_+241); A = 0x30;
  CALL_C(b_+241, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+244);
  CYC(b_+244, b_+246); A = 0x84;
  CALL_C(b_+246, loadGfxHeader_hook, SYM(loadGfxHeader), b_+249);
  CYC(b_+249, b_+251); A = 0xff;
  CYC(b_+251, b_+254); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+254, b_+256); A = 0x04;
  CALL_C(b_+256, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+259);
  SET_DE(POP(b_+259));
  CYC(b_+260, b_+263); SET_BC((SYM(shopkeeperCheckLinkHasItemAlready) + 15));
  CALL_C(b_+263, interactionSetPosition_hook, SYM(interactionSetPosition), b_+266);
  CYC(b_+266, b_+268); A = 0x02;
  CYC(b_+268, b_+271); SET_HL(b_+441);
  CYC(b_+271, b_+272); push_effect(gb, b_+272);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+272, b_+273); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+273, b_+274); H = mem_rd(gb, HL);
  CYC(b_+274, b_+275); L = A;
  CALL_C(b_+275, interactionSetScript_hook, SYM(interactionSetScript), b_+278);
  CYC(b_+278, b_+281); if (hook_enabled_at(gb, SYM(fadeinFromWhite))) { fadeinFromWhite_hook(gb); return; } HANDOFF(SYM(fadeinFromWhite));
}

void interactionCode57__subid1And2Substate0_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6983:
  CYC(b_+88, b_+91); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+91, b_+93); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+93, b_+95); goto L_699b; } CYC(b_+93, b_+95);
  CALL_C(b_+95, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+98);
  CYC(b_+98, b_+100); L = 0x42;
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+102); A = alu_dec8(gb, A);
  CYC(b_+102, b_+105); SET_HL(b_+441);
  CYC(b_+105, b_+106); push_effect(gb, b_+106);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+107, b_+108); H = mem_rd(gb, HL);
  CYC(b_+108, b_+109); L = A;
  CYC(b_+109, b_+112); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
L_699b:
  CALL_C(b_+112, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+115);
  CALL_C(b_+115, interactionRunScript_hook, SYM(interactionRunScript), b_+118);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = 0x61;
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+123); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+123, b_+125); goto L_69cb; } CYC(b_+123, b_+125);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x00);
  CYC(b_+127, b_+129); B = 0x04;
L_69ac:
  CALL_C(b_+129, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+132);
  if (!(F & FZ)) { CYCT(b_+132, b_+133); ret_effect(gb); return; } CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x99);
  CYC(b_+135, b_+136); L = alu_inc8(gb, L);
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x02);
  CYC(b_+138, b_+139); L = alu_inc8(gb, L);
  CYC(b_+139, b_+140); mem_wr(gb, HL, B);
  CYC(b_+140, b_+142); E = 0x5a;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+145); L = 0x78;
  CYC(b_+145, b_+146); mem_wr(gb, HL, A);
  PUSH(b_+146, BC);
  CYC(b_+147, b_+149); E = 0x78;
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); B = 0x00;
  CYC(b_+152, b_+153); C = A;
  CALL_C(b_+153, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+156);
  SET_BC(POP(b_+156));
  CYC(b_+157, b_+158); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto L_69ac; } CYC(b_+158, b_+160);
L_69cb:
  CYC(b_+160, b_+162); L = 0x4b;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+165, b_+168); if (hook_enabled_at(gb, SYM(objectSetVisiblec1))) { objectSetVisiblec1_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec1)); } CYC(b_+165, b_+168);
  CYC(b_+168, b_+171); if (hook_enabled_at(gb, SYM(objectSetVisiblec3))) { objectSetVisiblec3_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec3));
}

void interactionCode57__label_09_221_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_699b:
  CALL_C(b_+112, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+115);
  CALL_C(b_+115, interactionRunScript_hook, SYM(interactionRunScript), b_+118);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = 0x61;
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+123); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+123, b_+125); goto L_69cb; } CYC(b_+123, b_+125);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x00);
  CYC(b_+127, b_+129); B = 0x04;
L_69ac:
  CALL_C(b_+129, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+132);
  if (!(F & FZ)) { CYCT(b_+132, b_+133); ret_effect(gb); return; } CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x99);
  CYC(b_+135, b_+136); L = alu_inc8(gb, L);
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x02);
  CYC(b_+138, b_+139); L = alu_inc8(gb, L);
  CYC(b_+139, b_+140); mem_wr(gb, HL, B);
  CYC(b_+140, b_+142); E = 0x5a;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+145); L = 0x78;
  CYC(b_+145, b_+146); mem_wr(gb, HL, A);
  PUSH(b_+146, BC);
  CYC(b_+147, b_+149); E = 0x78;
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); B = 0x00;
  CYC(b_+152, b_+153); C = A;
  CALL_C(b_+153, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+156);
  SET_BC(POP(b_+156));
  CYC(b_+157, b_+158); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto L_69ac; } CYC(b_+158, b_+160);
L_69cb:
  CYC(b_+160, b_+162); L = 0x4b;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+165, b_+168); if (hook_enabled_at(gb, SYM(objectSetVisiblec1))) { objectSetVisiblec1_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec1)); } CYC(b_+165, b_+168);
  CYC(b_+168, b_+171); if (hook_enabled_at(gb, SYM(objectSetVisiblec3))) { objectSetVisiblec3_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec3));
}

void interactionCode57__nextObject_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_69ac:
  CALL_C(b_+129, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+132);
  if (!(F & FZ)) { CYCT(b_+132, b_+133); ret_effect(gb); return; } CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x99);
  CYC(b_+135, b_+136); L = alu_inc8(gb, L);
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x02);
  CYC(b_+138, b_+139); L = alu_inc8(gb, L);
  CYC(b_+139, b_+140); mem_wr(gb, HL, B);
  CYC(b_+140, b_+142); E = 0x5a;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+145); L = 0x78;
  CYC(b_+145, b_+146); mem_wr(gb, HL, A);
  PUSH(b_+146, BC);
  CYC(b_+147, b_+149); E = 0x78;
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); B = 0x00;
  CYC(b_+152, b_+153); C = A;
  CALL_C(b_+153, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+156);
  SET_BC(POP(b_+156));
  CYC(b_+157, b_+158); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto L_69ac; } CYC(b_+158, b_+160);
L_69cb:
  CYC(b_+160, b_+162); L = 0x4b;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+165, b_+168); if (hook_enabled_at(gb, SYM(objectSetVisiblec1))) { objectSetVisiblec1_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec1)); } CYC(b_+165, b_+168);
  CYC(b_+168, b_+171); if (hook_enabled_at(gb, SYM(objectSetVisiblec3))) { objectSetVisiblec3_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec3));
}

void interactionCode57__doneSpawningObjects_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_69cb:
  CYC(b_+160, b_+162); L = 0x4b;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+165, b_+168); if (hook_enabled_at(gb, SYM(objectSetVisiblec1))) { objectSetVisiblec1_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec1)); } CYC(b_+165, b_+168);
  CYC(b_+168, b_+171); if (hook_enabled_at(gb, SYM(objectSetVisiblec3))) { objectSetVisiblec3_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec3));
}

void interactionCode57__subid1Substate1_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_69d6:
  CALL_C(b_+171, interactionCode57__updateAnimationAndRunScript_hook, b_+183, b_+174);
  if (!(F & FC)) { CYCT(b_+174, b_+175); ret_effect(gb); return; } CYC(b_+174, b_+175);
  CALL_C(b_+175, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+178);
  CYC(b_+178, b_+180); L = 0x46;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 0xd2);
  CYC(b_+182, b_+183); ret_effect(gb); return;
}

void interactionCode57__updateAnimationAndRunScript_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_69e2:
  CYC(b_+183, b_+185); E = 0x7f;
  CYC(b_+185, b_+186); A = mem_rd(gb, DE);
  CYC(b_+186, b_+187); alu_or(gb, A);
  if ((F & FZ)) { CALL_C_CC(b_+187, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+190); } else CYC(b_+187, b_+190);
  CYC(b_+190, b_+193); if (hook_enabled_at(gb, SYM(interactionRunScript))) { interactionRunScript_hook(gb); return; } HANDOFF(SYM(interactionRunScript));
}

void interactionCode57__subid1Substate2_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_69ec:
  CALL_C(b_+193, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+196);
  CALL_C(b_+196, objectApplySpeed_hook, SYM(objectApplySpeed), b_+199);
  CALL_C(b_+199, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+202);
  if (!(F & FZ)) { CYCT(b_+202, b_+203); ret_effect(gb); return; } CYC(b_+202, b_+203);
  CALL_C(b_+203, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+206);
  CYC(b_+206, b_+209); if (hook_enabled_at(gb, SYM(fadeoutToWhite))) { fadeoutToWhite_hook(gb); return; } HANDOFF(SYM(fadeoutToWhite));
}

void interactionCode57__subid1Substate3_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_69fc:
  CYC(b_+209, b_+212); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+212, b_+213); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+213, b_+214); ret_effect(gb); return; } CYC(b_+213, b_+214);
  CALL_C(b_+214, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+217);
  CYC(b_+217, b_+219); A = 0x06;
  CYC(b_+219, b_+222); mem_wr(gb, wRoomLayoutEnd, A);
  CALL_C(b_+222, disableLcd_hook, SYM(disableLcd), b_+225);
  PUSH(b_+225, DE);
  CYC(b_+226, b_+229); SET_BC(0x0138);
  CYC(b_+229, b_+231); A = 0x00;
  CALL_C(b_+231, forceLoadRoom_hook, SYM(forceLoadRoom), b_+234);
  CYC(b_+234, b_+236); A = 0x2d;
  CALL_C(b_+236, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+239);
  CYC(b_+239, b_+241); A = 0x30;
  CALL_C(b_+241, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+244);
  CYC(b_+244, b_+246); A = 0x84;
  CALL_C(b_+246, loadGfxHeader_hook, SYM(loadGfxHeader), b_+249);
  CYC(b_+249, b_+251); A = 0xff;
  CYC(b_+251, b_+254); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+254, b_+256); A = 0x04;
  CALL_C(b_+256, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+259);
  SET_DE(POP(b_+259));
  CYC(b_+260, b_+263); SET_BC((SYM(shopkeeperCheckLinkHasItemAlready) + 15));
  CALL_C(b_+263, interactionSetPosition_hook, SYM(interactionSetPosition), b_+266);
  CYC(b_+266, b_+268); A = 0x02;
  CYC(b_+268, b_+271); SET_HL(b_+441);
  CYC(b_+271, b_+272); push_effect(gb, b_+272);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+272, b_+273); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+273, b_+274); H = mem_rd(gb, HL);
  CYC(b_+274, b_+275); L = A;
  CALL_C(b_+275, interactionSetScript_hook, SYM(interactionSetScript), b_+278);
  CYC(b_+278, b_+281); if (hook_enabled_at(gb, SYM(fadeinFromWhite))) { fadeinFromWhite_hook(gb); return; } HANDOFF(SYM(fadeinFromWhite));
}

void interactionCode57__subid02_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  goto L_6a44;
L_6965:
  CYC(b_+58, b_+60); E = 0x42;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); A = alu_dec8(gb, A);
  CYC(b_+62, b_+64); A = 0x0c;
  if ((F & FZ)) { CYCT(b_+64, b_+66); goto L_696f; } CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); A = 0xf4;
L_696f:
  CYC(b_+68, b_+70); E = 0x78;
  CYC(b_+70, b_+71); mem_wr(gb, DE, A);
  CALL_C(b_+71, interactionCode57__loadScriptAndInitGraphics_hook, b_+350, b_+74);
L_6975:
  CYC(b_+74, b_+76); E = 0x45;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+78); push_effect(gb, b_+78); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+88) { goto L_6983; }
    else if (jt_ == b_+171) { goto L_69d6; }
    else if (jt_ == b_+183) { goto L_69e2; }
    else if (jt_ == b_+193) { goto L_69ec; }
    else if (jt_ == b_+209) { goto L_69fc; }
    else { HANDOFF(HL); }
  } while (0);
L_6983:
  CYC(b_+88, b_+91); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+91, b_+93); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+93, b_+95); goto L_699b; } CYC(b_+93, b_+95);
  CALL_C(b_+95, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+98);
  CYC(b_+98, b_+100); L = 0x42;
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+102); A = alu_dec8(gb, A);
  CYC(b_+102, b_+105); SET_HL(b_+441);
  CYC(b_+105, b_+106); push_effect(gb, b_+106);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+107, b_+108); H = mem_rd(gb, HL);
  CYC(b_+108, b_+109); L = A;
  CYC(b_+109, b_+112); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
L_699b:
  CALL_C(b_+112, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+115);
  CALL_C(b_+115, interactionRunScript_hook, SYM(interactionRunScript), b_+118);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = 0x61;
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+123); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+123, b_+125); goto L_69cb; } CYC(b_+123, b_+125);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x00);
  CYC(b_+127, b_+129); B = 0x04;
L_69ac:
  CALL_C(b_+129, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+132);
  if (!(F & FZ)) { CYCT(b_+132, b_+133); ret_effect(gb); return; } CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x99);
  CYC(b_+135, b_+136); L = alu_inc8(gb, L);
  CYC(b_+136, b_+138); mem_wr(gb, HL, 0x02);
  CYC(b_+138, b_+139); L = alu_inc8(gb, L);
  CYC(b_+139, b_+140); mem_wr(gb, HL, B);
  CYC(b_+140, b_+142); E = 0x5a;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+145); L = 0x78;
  CYC(b_+145, b_+146); mem_wr(gb, HL, A);
  PUSH(b_+146, BC);
  CYC(b_+147, b_+149); E = 0x78;
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); B = 0x00;
  CYC(b_+152, b_+153); C = A;
  CALL_C(b_+153, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+156);
  SET_BC(POP(b_+156));
  CYC(b_+157, b_+158); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto L_69ac; } CYC(b_+158, b_+160);
L_69cb:
  CYC(b_+160, b_+162); L = 0x4b;
  CYC(b_+162, b_+163); A = mem_rd(gb, HL);
  CYC(b_+163, b_+165); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+165, b_+168); if (hook_enabled_at(gb, SYM(objectSetVisiblec1))) { objectSetVisiblec1_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec1)); } CYC(b_+165, b_+168);
  CYC(b_+168, b_+171); if (hook_enabled_at(gb, SYM(objectSetVisiblec3))) { objectSetVisiblec3_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec3));
L_69d6:
  CALL_C(b_+171, interactionCode57__updateAnimationAndRunScript_hook, b_+183, b_+174);
  if (!(F & FC)) { CYCT(b_+174, b_+175); ret_effect(gb); return; } CYC(b_+174, b_+175);
  CALL_C(b_+175, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+178);
  CYC(b_+178, b_+180); L = 0x46;
  CYC(b_+180, b_+182); mem_wr(gb, HL, 0xd2);
  CYC(b_+182, b_+183); ret_effect(gb); return;
L_69e2:
  CYC(b_+183, b_+185); E = 0x7f;
  CYC(b_+185, b_+186); A = mem_rd(gb, DE);
  CYC(b_+186, b_+187); alu_or(gb, A);
  if ((F & FZ)) { CALL_C_CC(b_+187, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+190); } else CYC(b_+187, b_+190);
  CYC(b_+190, b_+193); if (hook_enabled_at(gb, SYM(interactionRunScript))) { interactionRunScript_hook(gb); return; } HANDOFF(SYM(interactionRunScript));
L_69ec:
  CALL_C(b_+193, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+196);
  CALL_C(b_+196, objectApplySpeed_hook, SYM(objectApplySpeed), b_+199);
  CALL_C(b_+199, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+202);
  if (!(F & FZ)) { CYCT(b_+202, b_+203); ret_effect(gb); return; } CYC(b_+202, b_+203);
  CALL_C(b_+203, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+206);
  CYC(b_+206, b_+209); if (hook_enabled_at(gb, SYM(fadeoutToWhite))) { fadeoutToWhite_hook(gb); return; } HANDOFF(SYM(fadeoutToWhite));
L_69fc:
  CYC(b_+209, b_+212); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+212, b_+213); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+213, b_+214); ret_effect(gb); return; } CYC(b_+213, b_+214);
  CALL_C(b_+214, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+217);
  CYC(b_+217, b_+219); A = 0x06;
  CYC(b_+219, b_+222); mem_wr(gb, wRoomLayoutEnd, A);
  CALL_C(b_+222, disableLcd_hook, SYM(disableLcd), b_+225);
  PUSH(b_+225, DE);
  CYC(b_+226, b_+229); SET_BC(0x0138);
  CYC(b_+229, b_+231); A = 0x00;
  CALL_C(b_+231, forceLoadRoom_hook, SYM(forceLoadRoom), b_+234);
  CYC(b_+234, b_+236); A = 0x2d;
  CALL_C(b_+236, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+239);
  CYC(b_+239, b_+241); A = 0x30;
  CALL_C(b_+241, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+244);
  CYC(b_+244, b_+246); A = 0x84;
  CALL_C(b_+246, loadGfxHeader_hook, SYM(loadGfxHeader), b_+249);
  CYC(b_+249, b_+251); A = 0xff;
  CYC(b_+251, b_+254); mem_wr(gb, wTilesetAnimation, A);
  CYC(b_+254, b_+256); A = 0x04;
  CALL_C(b_+256, loadGfxRegisterStateIndex_hook, SYM(loadGfxRegisterStateIndex), b_+259);
  SET_DE(POP(b_+259));
  CYC(b_+260, b_+263); SET_BC((SYM(shopkeeperCheckLinkHasItemAlready) + 15));
  CALL_C(b_+263, interactionSetPosition_hook, SYM(interactionSetPosition), b_+266);
  CYC(b_+266, b_+268); A = 0x02;
  CYC(b_+268, b_+271); SET_HL(b_+441);
  CYC(b_+271, b_+272); push_effect(gb, b_+272);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+272, b_+273); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+273, b_+274); H = mem_rd(gb, HL);
  CYC(b_+274, b_+275); L = A;
  CALL_C(b_+275, interactionSetScript_hook, SYM(interactionSetScript), b_+278);
  CYC(b_+278, b_+281); if (hook_enabled_at(gb, SYM(fadeinFromWhite))) { fadeinFromWhite_hook(gb); return; } HANDOFF(SYM(fadeinFromWhite));
L_6a44:
  CALL_C(b_+281, checkInteractionState_hook, SYM(checkInteractionState), b_+284);
  if (!(F & FZ)) { CYCT(b_+284, b_+286); goto L_6a4c; } CYC(b_+284, b_+286);
  CYC(b_+286, b_+289); goto L_6965;
L_6a4c:
  CYC(b_+289, b_+291); E = 0x45;
  CYC(b_+291, b_+292); A = mem_rd(gb, DE);
  CYC(b_+292, b_+293); push_effect(gb, b_+293); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+88) { goto L_6983; }
    else if (jt_ == b_+183) { goto L_69e2; }
    else if (jt_ == b_+301) { goto L_6a58; }
    else if (jt_ == b_+308) { goto L_6a5f; }
    else { HANDOFF(HL); }
  } while (0);
L_6a58:
  CALL_C(b_+301, interactionCode57__updateAnimationAndRunScript_hook, b_+183, b_+304);
  if (!(F & FC)) { CYCT(b_+304, b_+305); ret_effect(gb); return; } CYC(b_+304, b_+305);
  CALL_C(b_+305, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+308);
L_6a5f:
  CALL_C(b_+308, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+311);
  CALL_C(b_+311, objectApplySpeed_hook, SYM(objectApplySpeed), b_+314);
  CYC(b_+314, b_+317); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+317, b_+319); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+319, b_+320); ret_effect(gb); return; } CYC(b_+319, b_+320);
  CALL_C(b_+320, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+323);
  CYC(b_+323, b_+326); SET_BC(0x388a);
  CALL_C(b_+326, interactionSetPosition_hook, SYM(interactionSetPosition), b_+329);
  CYC(b_+329, b_+331); A = 0x03;
  CYC(b_+331, b_+334); SET_HL(b_+441);
  CYC(b_+334, b_+335); push_effect(gb, b_+335);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+335, b_+336); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+336, b_+337); H = mem_rd(gb, HL);
  CYC(b_+337, b_+338); L = A;
  CYC(b_+338, b_+341); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
}

void interactionCode57__subid2Substate1_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6a58:
  CALL_C(b_+301, interactionCode57__updateAnimationAndRunScript_hook, b_+183, b_+304);
  if (!(F & FC)) { CYCT(b_+304, b_+305); ret_effect(gb); return; } CYC(b_+304, b_+305);
  CALL_C(b_+305, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+308);
L_6a5f:
  CALL_C(b_+308, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+311);
  CALL_C(b_+311, objectApplySpeed_hook, SYM(objectApplySpeed), b_+314);
  CYC(b_+314, b_+317); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+317, b_+319); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+319, b_+320); ret_effect(gb); return; } CYC(b_+319, b_+320);
  CALL_C(b_+320, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+323);
  CYC(b_+323, b_+326); SET_BC(0x388a);
  CALL_C(b_+326, interactionSetPosition_hook, SYM(interactionSetPosition), b_+329);
  CYC(b_+329, b_+331); A = 0x03;
  CYC(b_+331, b_+334); SET_HL(b_+441);
  CYC(b_+334, b_+335); push_effect(gb, b_+335);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+335, b_+336); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+336, b_+337); H = mem_rd(gb, HL);
  CYC(b_+337, b_+338); L = A;
  CYC(b_+338, b_+341); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
}

void interactionCode57__subid2Substate2_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6a5f:
  CALL_C(b_+308, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+311);
  CALL_C(b_+311, objectApplySpeed_hook, SYM(objectApplySpeed), b_+314);
  CYC(b_+314, b_+317); A = mem_rd(gb, wRoomLayoutEnd);
  CYC(b_+317, b_+319); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+319, b_+320); ret_effect(gb); return; } CYC(b_+319, b_+320);
  CALL_C(b_+320, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+323);
  CYC(b_+323, b_+326); SET_BC(0x388a);
  CALL_C(b_+326, interactionSetPosition_hook, SYM(interactionSetPosition), b_+329);
  CYC(b_+329, b_+331); A = 0x03;
  CYC(b_+331, b_+334); SET_HL(b_+441);
  CYC(b_+334, b_+335); push_effect(gb, b_+335);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+335, b_+336); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+336, b_+337); H = mem_rd(gb, HL);
  CYC(b_+337, b_+338); L = A;
  CYC(b_+338, b_+341); if (hook_enabled_at(gb, SYM(interactionSetScript))) { interactionSetScript_hook(gb); return; } HANDOFF(SYM(interactionSetScript));
}

void interactionCode57__unusedFunc_6a80_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6a80:
  CALL_C(b_+341, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+344);
  CALL_C(b_+344, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+347);
  CYC(b_+347, b_+350); if (hook_enabled_at(gb, SYM(interactionIncState))) { interactionIncState_hook(gb); return; } HANDOFF(SYM(interactionIncState));
}

void interactionCode57__loadScriptAndInitGraphics_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6a89:
  CALL_C(b_+350, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+353);
  CALL_C(b_+353, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+356);
  CYC(b_+356, b_+358); A = 0x1b;
  CALL_C(b_+358, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+361);
  CYC(b_+361, b_+363); E = 0x42;
  CYC(b_+363, b_+364); A = mem_rd(gb, DE);
  CYC(b_+364, b_+367); SET_HL(b_+433);
  CYC(b_+367, b_+368); push_effect(gb, b_+368);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+368, b_+369); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+369, b_+370); H = mem_rd(gb, HL);
  CYC(b_+370, b_+371); L = A;
  CALL_C(b_+371, interactionSetScript_hook, SYM(interactionSetScript), b_+374);
  CYC(b_+374, b_+377); if (hook_enabled_at(gb, SYM(interactionIncState))) { interactionIncState_hook(gb); return; } HANDOFF(SYM(interactionIncState));
}

void interactionCode57__createDirtChips_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6aa4:
  CYC(b_+377, b_+378); C = A;
  CYC(b_+378, b_+380); B = 0x02;
L_6aa7:
  CALL_C(b_+380, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+383);
  if (!(F & FZ)) { CYCT(b_+383, b_+384); ret_effect(gb); return; } CYC(b_+383, b_+384);
  CYC(b_+384, b_+386); mem_wr(gb, HL, 0x92);
  CYC(b_+386, b_+387); L = alu_inc8(gb, L);
  CYC(b_+387, b_+389); mem_wr(gb, HL, 0x06);
  CYC(b_+389, b_+390); L = alu_inc8(gb, L);
  CYC(b_+390, b_+391); mem_wr(gb, HL, C);
  CYC(b_+391, b_+393); E = 0x5a;
  CYC(b_+393, b_+394); A = mem_rd(gb, DE);
  CYC(b_+394, b_+396); alu_and(gb, 0x03);
  CYC(b_+396, b_+398); L = 0x47;
  CYC(b_+398, b_+399); mem_wr(gb, HL, A);
  CYC(b_+399, b_+401); L = 0x49;
  CYC(b_+401, b_+402); mem_wr(gb, HL, B);
  CYC(b_+402, b_+403); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  PUSH(b_+403, BC);
  CALL_C(b_+404, objectCopyPosition_hook, SYM(objectCopyPosition), b_+407);
  SET_BC(POP(b_+407));
  CYC(b_+408, b_+410); L = 0x4b;
  CYC(b_+410, b_+411); A = mem_rd(gb, HL);
  CYC(b_+411, b_+413); alu_add(gb, 0x04);
  CYC(b_+413, b_+414); mem_wr(gb, HL, A);
  CYC(b_+414, b_+416); E = 0x61;
  CYC(b_+416, b_+417); A = mem_rd(gb, DE);
  CYC(b_+417, b_+419); alu_cp(gb, 0x01);
  CYC(b_+419, b_+421); L = 0x4d;
  CYC(b_+421, b_+422); A = mem_rd(gb, HL);
  if ((F & FZ)) { CYCT(b_+422, b_+424); goto L_6ad5; } CYC(b_+422, b_+424);
  CYC(b_+424, b_+426); alu_add(gb, 0x1c);
L_6ad5:
  CYC(b_+426, b_+428); alu_sub(gb, 0x0e);
  CYC(b_+428, b_+429); mem_wr(gb, HL, A);
  CYC(b_+429, b_+430); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+430, b_+432); goto L_6aa7; } CYC(b_+430, b_+432);
  CYC(b_+432, b_+433); ret_effect(gb); return;
}

void interactionCode57__next_hook(GB *gb) {
  BASE(interactionCode57);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_6aa7:
  CALL_C(b_+380, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+383);
  if (!(F & FZ)) { CYCT(b_+383, b_+384); ret_effect(gb); return; } CYC(b_+383, b_+384);
  CYC(b_+384, b_+386); mem_wr(gb, HL, 0x92);
  CYC(b_+386, b_+387); L = alu_inc8(gb, L);
  CYC(b_+387, b_+389); mem_wr(gb, HL, 0x06);
  CYC(b_+389, b_+390); L = alu_inc8(gb, L);
  CYC(b_+390, b_+391); mem_wr(gb, HL, C);
  CYC(b_+391, b_+393); E = 0x5a;
  CYC(b_+393, b_+394); A = mem_rd(gb, DE);
  CYC(b_+394, b_+396); alu_and(gb, 0x03);
  CYC(b_+396, b_+398); L = 0x47;
  CYC(b_+398, b_+399); mem_wr(gb, HL, A);
  CYC(b_+399, b_+401); L = 0x49;
  CYC(b_+401, b_+402); mem_wr(gb, HL, B);
  CYC(b_+402, b_+403); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  PUSH(b_+403, BC);
  CALL_C(b_+404, objectCopyPosition_hook, SYM(objectCopyPosition), b_+407);
  SET_BC(POP(b_+407));
  CYC(b_+408, b_+410); L = 0x4b;
  CYC(b_+410, b_+411); A = mem_rd(gb, HL);
  CYC(b_+411, b_+413); alu_add(gb, 0x04);
  CYC(b_+413, b_+414); mem_wr(gb, HL, A);
  CYC(b_+414, b_+416); E = 0x61;
  CYC(b_+416, b_+417); A = mem_rd(gb, DE);
  CYC(b_+417, b_+419); alu_cp(gb, 0x01);
  CYC(b_+419, b_+421); L = 0x4d;
  CYC(b_+421, b_+422); A = mem_rd(gb, HL);
  if ((F & FZ)) { CYCT(b_+422, b_+424); goto L_6ad5; } CYC(b_+422, b_+424);
  CYC(b_+424, b_+426); alu_add(gb, 0x1c);
L_6ad5:
  CYC(b_+426, b_+428); alu_sub(gb, 0x0e);
  CYC(b_+428, b_+429); mem_wr(gb, HL, A);
  CYC(b_+429, b_+430); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+430, b_+432); goto L_6aa7; } CYC(b_+430, b_+432);
  CYC(b_+432, b_+433); ret_effect(gb); return;
}
