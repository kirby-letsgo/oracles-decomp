#include "game/game.h"
#include "game/gen.h"
#undef CYC
#undef CYCT
#define CYC(a,b) burn_rom(gb,SYMBANK(interactionCode60),(a),(b),false)
#define CYCT(a,b) burn_rom(gb,SYMBANK(interactionCode60),(a),(b),true)

void interactionCode60__giveTreasure_hook(GB *gb);
void interactionCode60__checkLinkTouched_hook(GB *gb);
void interactionCode60__setVisibleIfWithinScreenBoundary_hook(GB *gb);
void interactionCode60__setLinkAnimationAndDeleteIfTextClosed_hook(GB *gb);
void interactionCode60_hook(GB *gb) {
  BASE(interactionCode60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0,b_+2); E = 0x44;
  CYC(b_+2,b_+3); A = mem_rd(gb, DE);
  CYC(b_+3,b_+3+1);push_effect(gb,b_+4);
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+14) { goto L_4981; }
    else if (jt_ == b_+54) { goto L_49a9; }
    else if (jt_ == b_+514) { goto L_4b75; }
    else if (jt_ == b_+524) { goto L_4b7f; }
    else { HANDOFF(HL); }
  } while (0);
L_4981:
  CYC(b_+14,b_+16); A = 0x01;
  CYC(b_+16,b_+17); mem_wr(gb, DE, A);
  CYC(b_+17,b_+20); SET_HL((SYM(shopItemTreasureToGive) + 39));
  CYC(b_+20,b_+22); E = 0x16;
  CALL_C(b_+22, interBankCall_hook, 0x008a, b_+25);
  CYC(b_+25,b_+27); A = 0x06;
  CALL_C(b_+27, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+30);
  CYC(b_+30,b_+32); L = 0x78;
  CYC(b_+32,b_+33); A = mem_rd(gb, HL);
  CYC(b_+33,b_+34); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+34,b_+36); goto L_499e; } CYC(b_+34,b_+36);
  CYC(b_+36,b_+38); alu_cp(gb, 0xff);
  if ((F & FZ)) { CYCT(b_+38,b_+40); goto L_499e; } CYC(b_+38,b_+40);
  CYC(b_+40,b_+42); L = 0x74;
  CYC(b_+42,b_+43); mem_wr(gb, HL, A);
L_499e:
  CALL_C(b_+43, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+46);
L_49a1:
  CYC(b_+46,b_+48); E = 0x71;
  CYC(b_+48,b_+49); A = mem_rd(gb, DE);
  CYC(b_+49,b_+50); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+50,b_+50+1);ret_effect(gb); return; } CYC(b_+50,b_+51);
  CYC(b_+51,b_+54); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
L_49a9:
  CYC(b_+54,b_+56); E = 0x71;
  CYC(b_+56,b_+57); A = mem_rd(gb, DE);
  CYC(b_+57,b_+57+1);push_effect(gb,b_+58);
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+72) { goto L_49bb; }
    else if (jt_ == b_+89) { goto L_49cc; }
    else if (jt_ == b_+113) { goto L_49e4; }
    else if (jt_ == b_+260) { goto L_4a77; }
    else if (jt_ == b_+336) { goto L_4ac3; }
    else if (jt_ == b_+399) { goto L_4b02; }
    else if (jt_ == b_+425) { goto L_4b1c; }
    else { HANDOFF(HL); }
  } while (0);
L_49bb:
  CYC(b_+72,b_+73); H = D;
  CYC(b_+73,b_+75); L = 0x44;
  CYC(b_+75,b_+77); mem_wr(gb, HL, 0x02);
  CYC(b_+77,b_+78); L = alu_inc8(gb, L);
  CYC(b_+78,b_+80); mem_wr(gb, HL, 0x00);
  CALL_C(b_+80, interactionCode60__checkLinkTouched_hook, b_+789, b_+83);
  if ((F & FC)) { CYCT(b_+83,b_+86); goto L_4a50; } CYC(b_+83,b_+86);
  CYC(b_+86,b_+89); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
L_49cc:
  CYC(b_+89,b_+91); E = 0x45;
  CYC(b_+91,b_+92); A = mem_rd(gb, DE);
  CYC(b_+92,b_+93); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+93,b_+95); goto L_49de; } CYC(b_+93,b_+95);
  CYC(b_+95,b_+97); A = 0x01;
  CYC(b_+97,b_+98); mem_wr(gb, DE, A);
  CYC(b_+98,b_+100); E = 0x46;
  CYC(b_+100,b_+102); A = 0x1e;
  CYC(b_+102,b_+103); mem_wr(gb, DE, A);
  CALL_C(b_+103, objectCreatePuff_hook, SYM(objectCreatePuff), b_+106);
  if (!(F & FZ)) { CYCT(b_+106,b_+106+1);ret_effect(gb); return; } CYC(b_+106,b_+107);
L_49de:
  CALL_C(b_+107, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+110);
  if (!(F & FZ)) { CYCT(b_+110,b_+110+1);ret_effect(gb); return; } CYC(b_+110,b_+111);
  CYC(b_+111,b_+113); goto L_49bb;
L_49e4:
  CYC(b_+113,b_+115); E = 0x45;
  CYC(b_+115,b_+116); A = mem_rd(gb, DE);
  CYC(b_+116,b_+116+1);push_effect(gb,b_+117);
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+123) { goto L_49ee; }
    else if (jt_ == b_+136) { goto L_49fb; }
    else if (jt_ == b_+161) { goto L_4a14; }
    else { HANDOFF(HL); }
  } while (0);
L_49ee:
  CYC(b_+123,b_+125); A = 0x01;
  CYC(b_+125,b_+126); mem_wr(gb, DE, A);
  CYC(b_+126,b_+127); H = D;
  CYC(b_+127,b_+129); L = 0x46;
  CYC(b_+129,b_+131); mem_wr(gb, HL, 0x28);
  CYC(b_+131,b_+133); A = 0x4d;
  CYC(b_+133,b_+136); if (hook_enabled_at(SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));
L_49fb:
  CALL_C(b_+136, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+139);
  if (!(F & FZ)) { CYCT(b_+139,b_+140);ret_effect(gb); return; } CYC(b_+139,b_+140);
  CYC(b_+140,b_+142); mem_wr(gb, HL, 0x02);
  CYC(b_+142,b_+143); L = alu_inc8(gb, L);
  CYC(b_+143,b_+145); mem_wr(gb, HL, 0x02);
  CYC(b_+145,b_+147); L = 0x45;
  CYC(b_+147,b_+148); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+148, objectGetZAboveScreen_hook, SYM(objectGetZAboveScreen), b_+151);
  CYC(b_+151,b_+152); H = D;
  CYC(b_+152,b_+154); L = 0x4f;
  CYC(b_+154,b_+155); mem_wr(gb, HL, A);
  CALL_C(b_+155, objectSetVisiblec0_hook, SYM(objectSetVisiblec0), b_+158);
  CYC(b_+158,b_+161); goto L_4a47;
L_4a14:
  CALL_C(b_+161, interactionCode60__checkLinkTouched_hook, b_+789, b_+164);
  if ((F & FC)) { CYCT(b_+164,b_+166); goto L_4a50; } CYC(b_+164,b_+166);
  CALL_C(b_+166, interactionCode60__setVisibleIfWithinScreenBoundary_hook, b_+212, b_+169);
  CYC(b_+169,b_+171); C = 0x10;
  CALL_C(b_+171, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+174);
  if (!(F & FZ)) { CYCT(b_+174,b_+174+1);ret_effect(gb); return; } CYC(b_+174,b_+175);
  CALL_C(b_+175, objectCheckIsOnHazard_hook, SYM(objectCheckIsOnHazard), b_+178);
  if (!(F & FC)) { CYCT(b_+178,b_+180); goto L_4a2d; } CYC(b_+178,b_+180);
  CYC(b_+180,b_+181); A = alu_dec8(gb, A);
  if ((F & FZ)) { CYCT(b_+181,b_+183); goto L_4a62; } CYC(b_+181,b_+183);
  CYC(b_+183,b_+186); if (hook_enabled_at(SYM(objectReplaceWithFallingDownHoleInteraction))) { objectReplaceWithFallingDownHoleInteraction_hook(gb); return; } HANDOFF(SYM(objectReplaceWithFallingDownHoleInteraction));
L_4a2d:
  CYC(b_+186,b_+188); A = 0x77;
  CALL_C(b_+188, playSound_b00_hook, SYM(playSound_b00), b_+191);
  CALL_C(b_+191, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+194);
  if ((F & FZ)) { CYCT(b_+194,b_+196); goto L_4a3d; } CYC(b_+194,b_+196);
  CYC(b_+196,b_+199); SET_BC(0xff56);
  CYC(b_+199,b_+202); if (hook_enabled_at(SYM(objectSetSpeedZ))) { objectSetSpeedZ_hook(gb); return; } HANDOFF(SYM(objectSetSpeedZ));
L_4a3d:
  CALL_C(b_+202, objectSetVisible_hook, SYM(objectSetVisible), b_+205);
  CALL_C(b_+205, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+208);
  CYC(b_+208,b_+210); A = 0x02;
  CYC(b_+210,b_+212); goto L_4a55;
L_4a47:
  CALL_C(b_+212, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+215);
  if (!(F & FC)) { CYCT(b_+215,b_+218); if (hook_enabled_at(SYM(objectSetInvisible))) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible)); } CYC(b_+215,b_+218);
  CYC(b_+218,b_+221); if (hook_enabled_at(SYM(objectSetVisible))) { objectSetVisible_hook(gb); return; } HANDOFF(SYM(objectSetVisible));
L_4a50:
  CALL_C(b_+221, interactionCode60__giveTreasure_hook, b_+703, b_+224);
  CYC(b_+224,b_+226); A = 0x03;
L_4a55:
  CYC(b_+226,b_+227); H = D;
  CYC(b_+227,b_+229); L = 0x44;
  CYC(b_+229,b_+230); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+230,b_+231); alu_xor(gb, A);
  CYC(b_+231,b_+232); mem_wr(gb, HL, A);
  CYC(b_+232,b_+234); L = 0x4e;
  CYC(b_+234,b_+235); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+235,b_+236); mem_wr(gb, HL, A);
  CYC(b_+236,b_+239); if (hook_enabled_at(SYM(interactionSetAlwaysUpdateBit))) { interactionSetAlwaysUpdateBit_hook(gb); return; } HANDOFF(SYM(interactionSetAlwaysUpdateBit));
L_4a62:
  CYC(b_+239,b_+240); H = D;
  CYC(b_+240,b_+242); L = 0x70;
  CYC(b_+242,b_+243); A = mem_rd(gb, HL);
  CYC(b_+243,b_+245); L = 0x42;
  CYC(b_+245,b_+246); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+246,b_+247); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+247,b_+248); L = alu_inc8(gb, L);
  CYC(b_+248,b_+249); alu_xor(gb, A);
  CYC(b_+249,b_+250); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+250,b_+251); mem_wr(gb, HL, A);
  CYC(b_+251,b_+253); L = 0x5a;
  CYC(b_+253,b_+255); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+255,b_+257); B = 0x03;
  CYC(b_+257,b_+260); if (hook_enabled_at(SYM(objectCreateInteractionWithSubid00))) { objectCreateInteractionWithSubid00_hook(gb); return; } HANDOFF(SYM(objectCreateInteractionWithSubid00));
L_4a77:
  CYC(b_+260,b_+262); A = 0x80;
  CYC(b_+262,b_+265); mem_wr(gb, wForceLinkPushAnimation, A);
  CYC(b_+265,b_+267); E = 0x45;
  CYC(b_+267,b_+268); A = mem_rd(gb, DE);
  CYC(b_+268,b_+268+1);push_effect(gb,b_+269);
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+275) { goto L_4a86; }
    else if (jt_ == b_+295) { goto L_4a9a; }
    else if (jt_ == b_+317) { goto L_4ab0; }
    else { HANDOFF(HL); }
  } while (0);
L_4a86:
  CYC(b_+275,b_+277); A = 0x01;
  CYC(b_+277,b_+278); mem_wr(gb, DE, A);
  CYC(b_+278,b_+281); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CALL_C(b_+281, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+284);
  CYC(b_+284,b_+286); L = 0x50;
  CYC(b_+286,b_+288); mem_wr(gb, HL, 0x0a);
  CYC(b_+288,b_+290); L = 0x46;
  CYC(b_+290,b_+292); mem_wr(gb, HL, 0x20);
  CYC(b_+292,b_+295); if (hook_enabled_at(SYM(objectSetVisible80))) { objectSetVisible80_hook(gb); return; } HANDOFF(SYM(objectSetVisible80));
L_4a9a:
  CALL_C(b_+295, objectApplySpeed_hook, SYM(objectApplySpeed), b_+298);
  CALL_C(b_+298, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+301);
  if (!(F & FZ)) { CYCT(b_+301,b_+301+1);ret_effect(gb); return; } CYC(b_+301,b_+302);
  CYC(b_+302,b_+304); L = 0x45;
  CYC(b_+304,b_+305); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+305,b_+307); L = 0x79;
  CYC(b_+307,b_+308); A = mem_rd(gb, HL);
  CYC(b_+308,b_+309); alu_or(gb, A);
  if ((F & FZ)) { CALL_C_CC(b_+309, interactionCode60__giveTreasure_hook, b_+703, b_+312); } else CYC(b_+309,b_+312);
  CYC(b_+312,b_+314); A = 0x4c;
  CALL_C(b_+314, playSound_b00_hook, SYM(playSound_b00), b_+317);
L_4ab0:
  CYC(b_+317,b_+320); A = mem_rd(gb, wOamEnd);
  CYC(b_+320,b_+322); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(b_+322,b_+322+1);ret_effect(gb); return; } CYC(b_+322,b_+323);
  CYC(b_+323,b_+324); alu_xor(gb, A);
  CYC(b_+324,b_+327); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+327,b_+329); E = 0x79;
  CYC(b_+329,b_+330); A = mem_rd(gb, DE);
  CYC(b_+330,b_+333); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+333,b_+336); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete));
L_4ac3:
  CYC(b_+336,b_+338); E = 0x45;
  CYC(b_+338,b_+339); A = mem_rd(gb, DE);
  CYC(b_+339,b_+339+1);push_effect(gb,b_+340);
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+346) { goto L_4acd; }
    else if (jt_ == b_+359) { goto L_4ada; }
    else if (jt_ == b_+383) { goto L_4af2; }
    else { HANDOFF(HL); }
  } while (0);
L_4acd:
  CYC(b_+346,b_+348); A = 0x01;
  CYC(b_+348,b_+349); mem_wr(gb, DE, A);
  CYC(b_+349,b_+352); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CALL_C(b_+352, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+355);
  CYC(b_+355,b_+357); L = 0x46;
  CYC(b_+357,b_+359); mem_wr(gb, HL, 0x0f);
L_4ada:
  CALL_C(b_+359, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+362);
  if (!(F & FZ)) { CYCT(b_+362,b_+362+1);ret_effect(gb); return; } CYC(b_+362,b_+363);
  CALL_C(b_+363, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+366);
  CALL_C(b_+366, objectSetVisible80_hook, SYM(objectSetVisible80), b_+369);
  CALL_C(b_+369, interactionCode60__giveTreasure_hook, b_+703, b_+372);
  CYC(b_+372,b_+375); SET_BC(0x8100);
  CALL_C(b_+375, interactionCode60__setLinkAnimationAndDeleteIfTextClosed_hook, b_+556, b_+378);
  CYC(b_+378,b_+380); A = 0x4c;
  CYC(b_+380,b_+383); if (hook_enabled_at(SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));
L_4af2:
  CYC(b_+383,b_+386); A = mem_rd(gb, wOamEnd);
  CYC(b_+386,b_+388); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(b_+388,b_+388+1);ret_effect(gb); return; } CYC(b_+388,b_+389);
  CYC(b_+389,b_+390); alu_xor(gb, A);
  CYC(b_+390,b_+393); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+393,b_+396); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+396,b_+399); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete));
L_4b02:
  CALL_C(b_+399, interactionCode60__checkLinkTouched_hook, b_+789, b_+402);
  if (!(F & FC)) { CYCT(b_+402,b_+402+1);ret_effect(gb); return; } CYC(b_+402,b_+403);
  CYC(b_+403,b_+406); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+406,b_+408); alu_bit(gb, 7, A);
  if ((F & FZ)) { CYCT(b_+408,b_+408+1);ret_effect(gb); return; } CYC(b_+408,b_+409);
  CALL_C(b_+409, objectSetVisible82_hook, SYM(objectSetVisible82), b_+412);
  CALL_C(b_+412, interactionCode60__giveTreasure_hook, b_+703, b_+415);
  CYC(b_+415,b_+417); A = 0x4c;
  CALL_C(b_+417, playSound_b00_hook, SYM(playSound_b00), b_+420);
  CYC(b_+420,b_+422); A = 0x03;
  CYC(b_+422,b_+425); goto L_4a55;
L_4b1c:
  CYC(b_+425,b_+427); E = 0x45;
  CYC(b_+427,b_+428); A = mem_rd(gb, DE);
  CYC(b_+428,b_+428+1);push_effect(gb,b_+429);
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+435) { goto L_4b26; }
    else if (jt_ == b_+445) { goto L_4b30; }
    else if (jt_ == b_+483) { goto L_4b56; }
    else { HANDOFF(HL); }
  } while (0);
L_4b26:
  CYC(b_+435,b_+437); A = 0x01;
  CYC(b_+437,b_+438); mem_wr(gb, DE, A);
  CALL_C(b_+438, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+441);
  CYC(b_+441,b_+444); mem_wr(gb, wccaa, A);
  CYC(b_+444,b_+444+1);ret_effect(gb); return;
L_4b30:
  CYC(b_+445,b_+448); A = mem_rd(gb, wScreenVariables);
  CYC(b_+448,b_+450); alu_and(gb, 0x0c);
  if (!(F & FZ)) { CYCT(b_+450,b_+453); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+450,b_+453);
  CYC(b_+453,b_+456); A = mem_rd(gb, wccaa);
  CYC(b_+456,b_+457); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+457,b_+457+1);ret_effect(gb); return; } CYC(b_+457,b_+458);
  CYC(b_+458,b_+461); SET_BC(0xff00);
  CALL_C(b_+461, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+464);
  CYC(b_+464,b_+466); L = 0x45;
  CYC(b_+466,b_+467); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+467,b_+470); A = mem_rd(gb, w1Link_direction);
  CYC(b_+470,b_+472); A = alu_swap(gb, A);
  CYC(b_+472,b_+473); alu_rrca(gb);
  CYC(b_+473,b_+475); L = 0x49;
  CYC(b_+475,b_+476); mem_wr(gb, HL, A);
  CYC(b_+476,b_+478); L = 0x50;
  CYC(b_+478,b_+480); mem_wr(gb, HL, 0x14);
  CYC(b_+480,b_+483); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
L_4b56:
  CALL_C(b_+483, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+486);
  if (!(F & FC)) { CALL_C_CC(b_+486, objectApplySpeed_hook, SYM(objectApplySpeed), b_+489); } else CYC(b_+486,b_+489);
  CYC(b_+489,b_+491); C = 0x10;
  CALL_C(b_+491, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+494);
  if (!(F & FZ)) { CYCT(b_+494,b_+494+1);ret_effect(gb); return; } CYC(b_+494,b_+495);
  CYC(b_+495,b_+495+1);push_effect(gb,AF);
  CALL_C(b_+496, objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+499);
  CYC(b_+499,b_+499+1);SET_BC(pop_effect(gb));
  if ((F & FC)) { CYCT(b_+500,b_+503); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+500,b_+503);
  CYC(b_+503,b_+505); A = 0x77;
  CALL_C(b_+505, playSound_b00_hook, SYM(playSound_b00), b_+508);
  CYC(b_+508,b_+510); alu_bit(gb, 4, C);
  if ((F & FZ)) { CYCT(b_+510,b_+510+1);ret_effect(gb); return; } CYC(b_+510,b_+511);
  CYC(b_+511,b_+514); goto L_4a3d;
L_4b75:
  CALL_C(b_+514, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+517);
  CALL_C(b_+517, interactionCode60__checkLinkTouched_hook, b_+789, b_+520);
  if (!(F & FC)) { CYCT(b_+520,b_+520+1);ret_effect(gb); return; } CYC(b_+520,b_+521);
  CYC(b_+521,b_+524); goto L_4a50;
L_4b7f:
  CYC(b_+524,b_+526); E = 0x72;
  CYC(b_+526,b_+527); A = mem_rd(gb, DE);
  CYC(b_+527,b_+527+1);push_effect(gb,b_+528);
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+540) { goto L_4b8f; }
    else if (jt_ == b_+545) { goto L_4b94; }
    else if (jt_ == b_+602) { goto L_4bcd; }
    else { HANDOFF(HL); }
  } while (0);
L_4b8f:
  CYC(b_+540,b_+543); SET_BC(0x80fc);
  CYC(b_+543,b_+545); goto L_4b97;
L_4b94:
  CYC(b_+545,b_+548); SET_BC(0x8100);
L_4b97:
  CYC(b_+548,b_+550); E = 0x45;
  CYC(b_+550,b_+551); A = mem_rd(gb, DE);
  CYC(b_+551,b_+552); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+552,b_+554); goto L_4bbd; } CYC(b_+552,b_+554);
  CYC(b_+554,b_+555); A = alu_inc8(gb, A);
  CYC(b_+555,b_+556); mem_wr(gb, DE, A);
L_4b9f:
  CYC(b_+556,b_+558); A = 0x04;
  CYC(b_+558,b_+561); mem_wr(gb, wLinkForceState, A);
  CYC(b_+561,b_+562); A = B;
  CYC(b_+562,b_+565); mem_wr(gb, wcc50, A);
  CYC(b_+565,b_+568); SET_HL(wDisabledObjects);
  CYC(b_+568,b_+570); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0)));
  CYC(b_+570,b_+573); SET_HL(w1Link);
  CYC(b_+573,b_+575); B = 0xf2;
  CALL_C(b_+575, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+578);
  CALL_C(b_+578, objectSetVisible80_hook, SYM(objectSetVisible80), b_+581);
  CYC(b_+581,b_+583); A = 0x4c;
  CALL_C(b_+583, playSound_b00_hook, SYM(playSound_b00), b_+586);
L_4bbd:
  CALL_C(b_+586, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+589);
  CYC(b_+589,b_+592); SET_HL(wDisabledObjects);
  CYC(b_+592,b_+594); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 0)));
  CYC(b_+594,b_+596); A = 0x0f;
  CYC(b_+596,b_+599); mem_wr(gb, wInstrumentsDisabledCounter, A);
  CYC(b_+599,b_+602); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete));
L_4bcd:
  CYC(b_+602,b_+604); A = 0x78;
  CYC(b_+604,b_+607); mem_wr(gb, wInstrumentsDisabledCounter, A);
  CYC(b_+607,b_+609); E = 0x45;
  CYC(b_+609,b_+610); A = mem_rd(gb, DE);
  CYC(b_+610,b_+610+1);push_effect(gb,b_+611);
  CYC(0x0000,0x0001); alu_add(gb, A); CYC(0x0001,0x0002); SET_HL(pop_effect(gb)); CYC(0x0002,0x0003); alu_add(gb, L); CYC(0x0003,0x0004); L = A;
  if (!(F & FC)) CYCT(0x0004,0x0006); else { CYC(0x0004,0x0006); CYC(0x0006,0x0007); H = alu_inc8(gb, H); }
  CYC(0x0007,0x0008); A = mem_rd(gb, HL); SET_HL(HL + 1); CYC(0x0008,0x0009); H = mem_rd(gb, HL); CYC(0x0009,0x000a); L = A; CYC(0x000a,0x000b);
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+619) { goto L_4bde; }
    else if (jt_ == b_+642) { goto L_4bf5; }
    else if (jt_ == b_+655) { goto L_4c02; }
    else if (jt_ == b_+695) { goto L_4c2a; }
    else { HANDOFF(HL); }
  } while (0);
L_4bde:
  CYC(b_+619,b_+621); A = 0x01;
  CYC(b_+621,b_+622); mem_wr(gb, DE, A);
  CYC(b_+622,b_+623); E = alu_inc8(gb, E);
  CYC(b_+623,b_+625); A = 0x04;
  CYC(b_+625,b_+626); mem_wr(gb, DE, A);
  CYC(b_+626,b_+628); A = 0x81;
  CYC(b_+628,b_+631); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+631,b_+633); A = 0xff;
  CALL_C(b_+633, setLinkForceStateToState08_withParam_hook, SYM(setLinkForceStateToState08_withParam), b_+636);
  CYC(b_+636,b_+639); SET_HL(wLinkForceState);
  CYC(b_+639,b_+642); if (hook_enabled_at(SYM(objectSetInvisible))) { objectSetInvisible_hook(gb); return; } HANDOFF(SYM(objectSetInvisible));
L_4bf5:
  CALL_C(b_+642, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+645);
  if (!(F & FZ)) { CYCT(b_+645,b_+645+1);ret_effect(gb); return; } CYC(b_+645,b_+646);
  CYC(b_+646,b_+648); L = 0x45;
  CYC(b_+648,b_+649); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+649,b_+651); A = 0xff;
  CYC(b_+651,b_+654); mem_wr(gb, wcc63, A);
  CYC(b_+654,b_+654+1);ret_effect(gb); return;
L_4c02:
  CYC(b_+655,b_+658); A = mem_rd(gb, wcc63);
  CYC(b_+658,b_+659); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+659,b_+659+1);ret_effect(gb); return; } CYC(b_+659,b_+660);
  CYC(b_+660,b_+662); A = 0x0e;
  CYC(b_+662,b_+665); mem_wr(gb, wcc50, A);
  CYC(b_+665,b_+667); E = 0x4b;
  CYC(b_+667,b_+670); A = mem_rd(gb, w1Link_yh);
  CYC(b_+670,b_+672); alu_sub(gb, 0x0e);
  CYC(b_+672,b_+673); mem_wr(gb, DE, A);
  CYC(b_+673,b_+675); E = 0x4d;
  CYC(b_+675,b_+678); A = mem_rd(gb, w1Link_xh);
  CYC(b_+678,b_+680); alu_sub(gb, 0x04);
  CYC(b_+680,b_+681); mem_wr(gb, DE, A);
  CALL_C(b_+681, objectSetVisible_hook, SYM(objectSetVisible), b_+684);
  CALL_C(b_+684, objectSetVisible80_hook, SYM(objectSetVisible80), b_+687);
  CALL_C(b_+687, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+690);
  CYC(b_+690,b_+692); A = 0xab;
  CYC(b_+692,b_+695); if (hook_enabled_at(SYM(playSound_b00))) { playSound_b00_hook(gb); return; } HANDOFF(SYM(playSound_b00));
L_4c2a:
  CYC(b_+695,b_+698); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+698,b_+699); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+699,b_+700);ret_effect(gb); return; } CYC(b_+699,b_+700);
  CYC(b_+700,b_+703); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete));
}

void interactionCode60__giveTreasure_hook(GB *gb) {
  BASE(interactionCode60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4c32:
  CYC(b_+703,b_+705); E = 0x74;
  CYC(b_+705,b_+706); A = mem_rd(gb, DE);
  CYC(b_+706,b_+707); C = A;
  CYC(b_+707,b_+709); E = 0x70;
  CYC(b_+709,b_+710); A = mem_rd(gb, DE);
  CYC(b_+710,b_+711); B = A;
  CYC(b_+711,b_+713); alu_cp(gb, 0x37);
  if (!(F & FZ)) { CYCT(b_+713,b_+715); goto L_4c4d; } CYC(b_+713,b_+715);
  CYC(b_+715,b_+717); A = 0x26;
  CALL_C(b_+717, cpActiveRing_hook, SYM(cpActiveRing), b_+720);
  if ((F & FZ)) { CYCT(b_+720,b_+722); goto L_4c4c; } CYC(b_+720,b_+722);
  CYC(b_+722,b_+724); A = 0x27;
  CALL_C(b_+724, cpActiveRing_hook, SYM(cpActiveRing), b_+727);
  if (!(F & FZ)) { CYCT(b_+727,b_+729); goto L_4c4d; } CYC(b_+727,b_+729);
L_4c4c:
  CYC(b_+729,b_+730); C = alu_inc8(gb, C);
L_4c4d:
  CYC(b_+730,b_+731); A = B;
  CALL_C(b_+731, giveTreasure_hook, SYM(giveTreasure), b_+734);
  CYC(b_+734,b_+735); B = A;
  CYC(b_+735,b_+737); E = 0x72;
  CYC(b_+737,b_+738); A = mem_rd(gb, DE);
  CYC(b_+738,b_+740); alu_cp(gb, 0x03);
  if ((F & FZ)) { CYCT(b_+740,b_+742); goto L_4c5d; } CYC(b_+740,b_+742);
  CYC(b_+742,b_+743); A = B;
  CALL_C(b_+743, playSound_b00_hook, SYM(playSound_b00), b_+746);
L_4c5d:
  CYC(b_+746,b_+748); E = 0x75;
  CYC(b_+748,b_+749); A = mem_rd(gb, DE);
  CYC(b_+749,b_+751); alu_cp(gb, 0xff);
  if ((F & FZ)) { CYCT(b_+751,b_+753); goto L_4c7d; } CYC(b_+751,b_+753);
  CYC(b_+753,b_+754); C = A;
  CYC(b_+754,b_+756); B = 0x00;
  CALL_C(b_+756, showText_hook, SYM(showText), b_+759);
  CYC(b_+759,b_+761); A = mem_rd(gb, hCameraY);
  CYC(b_+761,b_+762); B = A;
  CYC(b_+762,b_+765); A = mem_rd(gb, w1Link_yh);
  CYC(b_+765,b_+766); alu_sub(gb, B);
  CYC(b_+766,b_+768); alu_sub(gb, 0x10);
  CYC(b_+768,b_+770); alu_cp(gb, 0x48);
  CYC(b_+770,b_+772); A = 0x02;
  if ((F & FC)) { CYCT(b_+772,b_+774); goto L_4c7a; } CYC(b_+772,b_+774);
  CYC(b_+774,b_+775); alu_xor(gb, A);
L_4c7a:
  CYC(b_+775,b_+778); mem_wr(gb, wTextboxPosition, A);
L_4c7d:
  CYC(b_+778,b_+780); E = 0x73;
  CYC(b_+780,b_+781); A = mem_rd(gb, DE);
  CYC(b_+781,b_+782); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+782,b_+782+1);ret_effect(gb); return; } CYC(b_+782,b_+783);
  CALL_C(b_+783, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+786);
  CYC(b_+786,b_+788); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5)));
  CYC(b_+788,b_+788+1);ret_effect(gb); return;
}

void interactionCode60__checkLinkTouched_hook(GB *gb) {
  BASE(interactionCode60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_4c88:
  CYC(b_+789,b_+792); A = mem_rd(gb, wLinkForceState);
  CYC(b_+792,b_+793); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+793,b_+793+1);ret_effect(gb); return; } CYC(b_+793,b_+794);
  CYC(b_+794,b_+797); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+797,b_+798); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+798,b_+798+1);ret_effect(gb); return; } CYC(b_+798,b_+799);
  CYC(b_+799,b_+802); A = mem_rd(gb, w1Link_state);
  CYC(b_+802,b_+804); alu_cp(gb, 0x01);
  if ((F & FZ)) { CYCT(b_+804,b_+806); goto L_4c9d; } CYC(b_+804,b_+806);
  CYC(b_+806,b_+808); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+808,b_+810); goto L_4cac; } CYC(b_+808,b_+810);
L_4c9d:
  CYC(b_+810,b_+813); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+813,b_+814); alu_rrca(gb);
  if ((F & FC)) { CYCT(b_+814,b_+816); goto L_4cac; } CYC(b_+814,b_+816);
  CYC(b_+816,b_+818); E = 0x6a;
  CYC(b_+818,b_+819); A = mem_rd(gb, DE);
  CYC(b_+819,b_+820); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+820,b_+823); if (hook_enabled_at(SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing))) { objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook(gb); return; } HANDOFF(SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing)); } CYC(b_+820,b_+823);
  CYC(b_+823,b_+824); alu_scf(gb);
  CYC(b_+824,b_+824+1);ret_effect(gb); return;
L_4cac:
  CYC(b_+825,b_+826); alu_xor(gb, A);
  CYC(b_+826,b_+826+1);ret_effect(gb); return;
}

void interactionCode60__setVisibleIfWithinScreenBoundary_hook(GB *gb) {
  BASE(interactionCode60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+212, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+215);
  if (!(F & FC)) { CYCT(b_+215, b_+218); objectSetInvisible_hook(gb); return; }
  CYC(b_+215, b_+218);
  CYC(b_+218, b_+221); objectSetVisible_hook(gb);
}

void interactionCode60__setLinkAnimationAndDeleteIfTextClosed_hook(GB *gb) {
  BASE(interactionCode60);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+556, b_+558); A = 0x04;
  CYC(b_+558, b_+561); mem_wr(gb, wLinkForceState, A);
  CYC(b_+561, b_+562); A = B;
  CYC(b_+562, b_+565); mem_wr(gb, wcc50, A);
  CYC(b_+565, b_+568); SET_HL(wDisabledObjects);
  CYC(b_+568, b_+570); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 1));
  CYC(b_+570, b_+573); SET_HL(w1Link);
  CYC(b_+573, b_+575); B = 0xf2;
  CALL_C(b_+575, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+578);
  CALL_C(b_+578, objectSetVisible80_hook, SYM(objectSetVisible80), b_+581);
  CYC(b_+581, b_+583); A = 0x4c;
  CALL_C(b_+583, playSound_b00_hook, SYM(playSound_b00), b_+586);
  CALL_C(b_+586, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+589);
  CYC(b_+589, b_+592); SET_HL(wDisabledObjects);
  CYC(b_+592, b_+594); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~1));
  CYC(b_+594, b_+596); A = 0x0f;
  CYC(b_+596, b_+599); mem_wr(gb, wInstrumentsDisabledCounter, A);
  CYC(b_+599, b_+602); interactionDelete_hook(gb);
}
