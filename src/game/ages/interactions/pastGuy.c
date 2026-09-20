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

void interactionCode43_hook(GB *gb);
void interactionCode43__subid0_hook(GB *gb);
void interactionCode43__afterCall5348_hook(GB *gb);
void interactionCode43__subid1_hook(GB *gb);
void interactionCode43__afterCall5378_hook(GB *gb);
void interactionCode43__label_09_117_hook(GB *gb);
void interactionCode43__subid3_hook(GB *gb);
void interactionCode43__afterCall538b_hook(GB *gb);
void interactionCode43__subid4_hook(GB *gb);
void interactionCode43__afterCall53a1_hook(GB *gb);
void interactionCode43__subid5_hook(GB *gb);
void interactionCode43__afterCall53b0_hook(GB *gb);
void interactionCode43__subid5__initialized_hook(GB *gb);
void interactionCode43__subid5__substate0_hook(GB *gb);
void interactionCode43__subid5__substate1_hook(GB *gb);
void interactionCode43__subid5__substate2_hook(GB *gb);
void interactionCode43__subid6_hook(GB *gb);
void interactionCode43__afterCall5410_hook(GB *gb);
void interactionCode43__subid6__initAndLoadScript_hook(GB *gb);
void interactionCode43__initialized_hook(GB *gb);
void interactionCode43__subid7_hook(GB *gb);
void interactionCode43__afterCall543b_hook(GB *gb);
void interactionCode43__initGraphicsAndIncState_hook(GB *gb);
void interactionCode43__initGraphicsIncStateAndLoadScript_hook(GB *gb);

void interactionCode43_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+20) { goto L_5324; }
    else if (jt_ == b_+62) { goto L_534e; }
    else if (jt_ == b_+115) { goto L_5383; }
    else if (jt_ == b_+138) { goto L_539a; }
    else if (jt_ == b_+152) { goto L_53a8; }
    else if (jt_ == b_+234) { goto L_53fa; }
    else if (jt_ == b_+289) { goto L_5431; }
    else { HANDOFF(HL); }
  } while (0);
L_5324:
  CYC(b_+20, b_+22); A = 0x14;
  CALL_C(b_+22, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+25);
  if (!(F & FZ)) { CYCT(b_+25, b_+28); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+25, b_+28);
  CYC(b_+28, b_+30); A = 0x0b;
  CALL_C(b_+30, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+33);
  CYC(b_+33, b_+35); E = 0x43;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto L_533c; } CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+39, b_+42); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+39, b_+42);
  CYC(b_+42, b_+44); goto L_5340;
L_533c:
  CYC(b_+44, b_+45); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+45, b_+48); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+45, b_+48);
L_5340:
  CALL_C(b_+48, checkInteractionState_hook, SYM(checkInteractionState), b_+51);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto L_5348; } CYC(b_+51, b_+53);
  CALL_C(b_+53, interactionCode43__initGraphicsIncStateAndLoadScript_hook, b_+315, b_+56);
L_5348:
  CALL_C(b_+56, interactionRunScript_hook, SYM(interactionRunScript), b_+59);
  CYC(b_+59, b_+62); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
L_534e:
  CALL_C(b_+62, checkInteractionState_hook, SYM(checkInteractionState), b_+65);
  if (!(F & FZ)) { CYCT(b_+65, b_+67); goto L_537d; } CYC(b_+65, b_+67);
  CYC(b_+67, b_+70); SET_HL(SYM(getGameProgress_2));
  CYC(b_+70, b_+72); E = 0x09;
  CALL_C(b_+72, interBankCall_hook, 0x008a, b_+75);
  CYC(b_+75, b_+77); C = 0x01;
  CYC(b_+77, b_+79); A = 0x05;
  CALL_C(b_+79, checkNpcShouldExistAtGameStage_hook, SYM(checkNpcShouldExistAtGameStage), b_+82);
  if (!(F & FZ)) { CYCT(b_+82, b_+85); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+82, b_+85);
  CYC(b_+85, b_+86); A = B;
  CYC(b_+86, b_+89); SET_HL(b_+356);
  CYC(b_+89, b_+90); push_effect(gb, b_+90);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+90, b_+91); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+91, b_+92); H = mem_rd(gb, HL);
  CYC(b_+92, b_+93); L = A;
  CALL_C(b_+93, interactionSetScript_hook, SYM(interactionSetScript), b_+96);
  CYC(b_+96, b_+98); A = 0x17;
  CALL_C(b_+98, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+101);
  CALL_C(b_+101, interactionCode43__initGraphicsAndIncState_hook, b_+306, b_+104);
L_5378:
  CYC(b_+104, b_+106); A = 0x03;
  CYC(b_+106, b_+108); E = 0x5c;
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
L_537d:
  CALL_C(b_+109, interactionRunScript_hook, SYM(interactionRunScript), b_+112);
  CYC(b_+112, b_+115); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
L_5383:
  CALL_C(b_+115, checkInteractionState_hook, SYM(checkInteractionState), b_+118);
  if (!(F & FZ)) { CYCT(b_+118, b_+120); goto L_538e; } CYC(b_+118, b_+120);
  CALL_C(b_+120, interactionCode43__initGraphicsIncStateAndLoadScript_hook, b_+315, b_+123);
L_538b:
  CYC(b_+123, b_+126); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
L_538e:
  CALL_C(b_+126, interactionRunScript_hook, SYM(interactionRunScript), b_+129);
  CYC(b_+129, b_+132); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CYC(b_+132, b_+134); alu_cp(gb, 0x02);
  if ((F & FC)) { CYCT(b_+134, b_+137); if (hook_enabled_at(SYM(interactionAnimate))) { interactionAnimate_hook(gb); return; } HANDOFF(SYM(interactionAnimate)); } CYC(b_+134, b_+137);
  CYC(b_+137, b_+138); ret_effect(gb); return;
L_539a:
  CALL_C(b_+138, checkInteractionState_hook, SYM(checkInteractionState), b_+141);
  if (!(F & FZ)) { CYCT(b_+141, b_+142); ret_effect(gb); return; } CYC(b_+141, b_+142);
  CALL_C(b_+142, interactionCode43__initGraphicsAndIncState_hook, b_+306, b_+145);
L_53a1:
  CYC(b_+145, b_+147); L = 0x5c;
  CYC(b_+147, b_+149); mem_wr(gb, HL, 0x06);
  CYC(b_+149, b_+152); if (hook_enabled_at(SYM(objectSetVisible82))) { objectSetVisible82_hook(gb); return; } HANDOFF(SYM(objectSetVisible82));
L_53a8:
  CALL_C(b_+152, checkInteractionState_hook, SYM(checkInteractionState), b_+155);
  if (!(F & FZ)) { CYCT(b_+155, b_+157); goto L_53b7; } CYC(b_+155, b_+157);
  CALL_C(b_+157, interactionCode43__initGraphicsIncStateAndLoadScript_hook, b_+315, b_+160);
L_53b0:
  CYC(b_+160, b_+162); L = 0x5c;
  CYC(b_+162, b_+164); mem_wr(gb, HL, 0x06);
  CYC(b_+164, b_+167); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
L_53b7:
  CYC(b_+167, b_+169); E = 0x45;
  CYC(b_+169, b_+170); A = mem_rd(gb, DE);
  CYC(b_+170, b_+171); push_effect(gb, b_+171); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+177) { goto L_53c1; }
    else if (jt_ == b_+191) { goto L_53cf; }
    else if (jt_ == b_+219) { goto L_53eb; }
    else { HANDOFF(HL); }
  } while (0);
L_53c1:
  CYC(b_+177, b_+180); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CYC(b_+180, b_+182); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+182, b_+183); ret_effect(gb); return; } CYC(b_+182, b_+183);
  CYC(b_+183, b_+186); SET_HL((SYM(goronDance_sequenceData__platinum) + 98));
  CYC(b_+186, b_+188); E = 0x08;
  CYC(b_+188, b_+191); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53cf:
  CALL_C(b_+191, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+194);
  if ((F & FZ)) { CYCT(b_+194, b_+196); goto L_53dc; } CYC(b_+194, b_+196);
  CYC(b_+196, b_+199); SET_HL((SYM(goronDance_sequenceData__platinum) + 126));
  CYC(b_+199, b_+201); E = 0x08;
  CYC(b_+201, b_+204); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53dc:
  CALL_C(b_+204, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+207);
  CYC(b_+207, b_+209); L = 0x5c;
  CYC(b_+209, b_+211); mem_wr(gb, HL, 0x02);
  CYC(b_+211, b_+213); L = 0x46;
  CYC(b_+213, b_+215); mem_wr(gb, HL, 0xa4);
  CYC(b_+215, b_+216); L = alu_inc8(gb, L);
  CYC(b_+216, b_+218); mem_wr(gb, HL, 0x01);
  CYC(b_+218, b_+219); ret_effect(gb); return;
L_53eb:
  CALL_C(b_+219, interactionAnimate_hook, SYM(interactionAnimate), b_+222);
  CYC(b_+222, b_+224); L = 0x46;
  CALL_C(b_+224, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+227);
  if (!(F & FZ)) { CYCT(b_+227, b_+228); ret_effect(gb); return; } CYC(b_+227, b_+228);
  CYC(b_+228, b_+230); A = 0xff;
  CYC(b_+230, b_+233); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(b_+233, b_+234); ret_effect(gb); return;
L_53fa:
  CALL_C(b_+234, checkInteractionState_hook, SYM(checkInteractionState), b_+237);
  if (!(F & FZ)) { CYCT(b_+237, b_+239); goto L_5421; } CYC(b_+237, b_+239);
  CYC(b_+239, b_+242); SET_HL((wGroup4RoomFlags + 252));
  CYC(b_+242, b_+244); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+244, b_+246); goto L_541e; } CYC(b_+244, b_+246);
  CYC(b_+246, b_+249); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+249, b_+251); alu_bit(gb, 6, A);
  if ((F & FZ)) { CYCT(b_+251, b_+253); goto L_541e; } CYC(b_+251, b_+253);
  CALL_C(b_+253, interactionCode43__initGraphicsAndIncState_hook, b_+306, b_+256);
L_5410:
  CYC(b_+256, b_+258); L = 0x5c;
  CYC(b_+258, b_+260); mem_wr(gb, HL, 0x06);
  CYC(b_+260, b_+262); L = 0x43;
  CYC(b_+262, b_+263); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+263, b_+265); A = 0x06;
  CALL_C(b_+265, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+268);
  CYC(b_+268, b_+270); goto L_5421;
L_541e:
  CALL_C(b_+270, interactionCode43__initGraphicsIncStateAndLoadScript_hook, b_+315, b_+273);
L_5421:
  CYC(b_+273, b_+275); E = 0x43;
  CYC(b_+275, b_+276); A = mem_rd(gb, DE);
  CYC(b_+276, b_+277); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+277, b_+280); if (hook_enabled_at(SYM(interactionPushLinkAwayAndUpdateDrawPriority))) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; } HANDOFF(SYM(interactionPushLinkAwayAndUpdateDrawPriority)); } CYC(b_+277, b_+280);
  CALL_C(b_+280, interactionRunScript_hook, SYM(interactionRunScript), b_+283);
  if ((F & FC)) { CYCT(b_+283, b_+286); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+283, b_+286);
  CYC(b_+286, b_+289); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
L_5431:
  CALL_C(b_+289, checkInteractionState_hook, SYM(checkInteractionState), b_+292);
  if ((F & FZ)) { CYCT(b_+292, b_+295); goto L_5438; } CYC(b_+292, b_+295);
  CYC(b_+295, b_+296); ret_effect(gb); return;
L_5438:
  CALL_C(b_+296, interactionCode43__initGraphicsAndIncState_hook, b_+306, b_+299);
L_543b:
  CYC(b_+299, b_+301); L = 0x5c;
  CYC(b_+301, b_+303); mem_wr(gb, HL, 0x06);
  CYC(b_+303, b_+306); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
}

void interactionCode43__subid0_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5324:
  CYC(b_+20, b_+22); A = 0x14;
  CALL_C(b_+22, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+25);
  if (!(F & FZ)) { CYCT(b_+25, b_+28); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+25, b_+28);
  CYC(b_+28, b_+30); A = 0x0b;
  CALL_C(b_+30, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+33);
  CYC(b_+33, b_+35); E = 0x43;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto L_533c; } CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+39, b_+42); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+39, b_+42);
  CYC(b_+42, b_+44); goto L_5340;
L_533c:
  CYC(b_+44, b_+45); alu_or(gb, A);
  if ((F & FZ)) { CYCT(b_+45, b_+48); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+45, b_+48);
L_5340:
  CALL_C(b_+48, checkInteractionState_hook, SYM(checkInteractionState), b_+51);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto L_5348; } CYC(b_+51, b_+53);
  CALL_C(b_+53, interactionCode43__initGraphicsIncStateAndLoadScript_hook, b_+315, b_+56);
L_5348:
  CALL_C(b_+56, interactionRunScript_hook, SYM(interactionRunScript), b_+59);
  CYC(b_+59, b_+62); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode43__afterCall5348_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5348:
  CALL_C(b_+56, interactionRunScript_hook, SYM(interactionRunScript), b_+59);
  CYC(b_+59, b_+62); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode43__subid1_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_534e:
  CALL_C(b_+62, checkInteractionState_hook, SYM(checkInteractionState), b_+65);
  if (!(F & FZ)) { CYCT(b_+65, b_+67); goto L_537d; } CYC(b_+65, b_+67);
  CYC(b_+67, b_+70); SET_HL(SYM(getGameProgress_2));
  CYC(b_+70, b_+72); E = 0x09;
  CALL_C(b_+72, interBankCall_hook, 0x008a, b_+75);
  CYC(b_+75, b_+77); C = 0x01;
  CYC(b_+77, b_+79); A = 0x05;
  CALL_C(b_+79, checkNpcShouldExistAtGameStage_hook, SYM(checkNpcShouldExistAtGameStage), b_+82);
  if (!(F & FZ)) { CYCT(b_+82, b_+85); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+82, b_+85);
  CYC(b_+85, b_+86); A = B;
  CYC(b_+86, b_+89); SET_HL(b_+356);
  CYC(b_+89, b_+90); push_effect(gb, b_+90);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+90, b_+91); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+91, b_+92); H = mem_rd(gb, HL);
  CYC(b_+92, b_+93); L = A;
  CALL_C(b_+93, interactionSetScript_hook, SYM(interactionSetScript), b_+96);
  CYC(b_+96, b_+98); A = 0x17;
  CALL_C(b_+98, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+101);
  CALL_C(b_+101, interactionCode43__initGraphicsAndIncState_hook, b_+306, b_+104);
L_5378:
  CYC(b_+104, b_+106); A = 0x03;
  CYC(b_+106, b_+108); E = 0x5c;
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
L_537d:
  CALL_C(b_+109, interactionRunScript_hook, SYM(interactionRunScript), b_+112);
  CYC(b_+112, b_+115); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode43__afterCall5378_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5378:
  CYC(b_+104, b_+106); A = 0x03;
  CYC(b_+106, b_+108); E = 0x5c;
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
L_537d:
  CALL_C(b_+109, interactionRunScript_hook, SYM(interactionRunScript), b_+112);
  CYC(b_+112, b_+115); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode43__label_09_117_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_537d:
  CALL_C(b_+109, interactionRunScript_hook, SYM(interactionRunScript), b_+112);
  CYC(b_+112, b_+115); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode43__subid3_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5383:
  CALL_C(b_+115, checkInteractionState_hook, SYM(checkInteractionState), b_+118);
  if (!(F & FZ)) { CYCT(b_+118, b_+120); goto L_538e; } CYC(b_+118, b_+120);
  CALL_C(b_+120, interactionCode43__initGraphicsIncStateAndLoadScript_hook, b_+315, b_+123);
L_538b:
  CYC(b_+123, b_+126); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
L_538e:
  CALL_C(b_+126, interactionRunScript_hook, SYM(interactionRunScript), b_+129);
  CYC(b_+129, b_+132); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CYC(b_+132, b_+134); alu_cp(gb, 0x02);
  if ((F & FC)) { CYCT(b_+134, b_+137); if (hook_enabled_at(SYM(interactionAnimate))) { interactionAnimate_hook(gb); return; } HANDOFF(SYM(interactionAnimate)); } CYC(b_+134, b_+137);
  CYC(b_+137, b_+138); ret_effect(gb); return;
}

void interactionCode43__afterCall538b_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_538b:
  CYC(b_+123, b_+126); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
}

void interactionCode43__subid4_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_539a:
  CALL_C(b_+138, checkInteractionState_hook, SYM(checkInteractionState), b_+141);
  if (!(F & FZ)) { CYCT(b_+141, b_+142); ret_effect(gb); return; } CYC(b_+141, b_+142);
  CALL_C(b_+142, interactionCode43__initGraphicsAndIncState_hook, b_+306, b_+145);
L_53a1:
  CYC(b_+145, b_+147); L = 0x5c;
  CYC(b_+147, b_+149); mem_wr(gb, HL, 0x06);
  CYC(b_+149, b_+152); if (hook_enabled_at(SYM(objectSetVisible82))) { objectSetVisible82_hook(gb); return; } HANDOFF(SYM(objectSetVisible82));
}

void interactionCode43__afterCall53a1_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53a1:
  CYC(b_+145, b_+147); L = 0x5c;
  CYC(b_+147, b_+149); mem_wr(gb, HL, 0x06);
  CYC(b_+149, b_+152); if (hook_enabled_at(SYM(objectSetVisible82))) { objectSetVisible82_hook(gb); return; } HANDOFF(SYM(objectSetVisible82));
}

void interactionCode43__subid5_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53a8:
  CALL_C(b_+152, checkInteractionState_hook, SYM(checkInteractionState), b_+155);
  if (!(F & FZ)) { CYCT(b_+155, b_+157); goto L_53b7; } CYC(b_+155, b_+157);
  CALL_C(b_+157, interactionCode43__initGraphicsIncStateAndLoadScript_hook, b_+315, b_+160);
L_53b0:
  CYC(b_+160, b_+162); L = 0x5c;
  CYC(b_+162, b_+164); mem_wr(gb, HL, 0x06);
  CYC(b_+164, b_+167); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
L_53b7:
  CYC(b_+167, b_+169); E = 0x45;
  CYC(b_+169, b_+170); A = mem_rd(gb, DE);
  CYC(b_+170, b_+171); push_effect(gb, b_+171); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+177) { goto L_53c1; }
    else if (jt_ == b_+191) { goto L_53cf; }
    else if (jt_ == b_+219) { goto L_53eb; }
    else { HANDOFF(HL); }
  } while (0);
L_53c1:
  CYC(b_+177, b_+180); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CYC(b_+180, b_+182); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+182, b_+183); ret_effect(gb); return; } CYC(b_+182, b_+183);
  CYC(b_+183, b_+186); SET_HL((SYM(goronDance_sequenceData__platinum) + 98));
  CYC(b_+186, b_+188); E = 0x08;
  CYC(b_+188, b_+191); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53cf:
  CALL_C(b_+191, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+194);
  if ((F & FZ)) { CYCT(b_+194, b_+196); goto L_53dc; } CYC(b_+194, b_+196);
  CYC(b_+196, b_+199); SET_HL((SYM(goronDance_sequenceData__platinum) + 126));
  CYC(b_+199, b_+201); E = 0x08;
  CYC(b_+201, b_+204); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53dc:
  CALL_C(b_+204, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+207);
  CYC(b_+207, b_+209); L = 0x5c;
  CYC(b_+209, b_+211); mem_wr(gb, HL, 0x02);
  CYC(b_+211, b_+213); L = 0x46;
  CYC(b_+213, b_+215); mem_wr(gb, HL, 0xa4);
  CYC(b_+215, b_+216); L = alu_inc8(gb, L);
  CYC(b_+216, b_+218); mem_wr(gb, HL, 0x01);
  CYC(b_+218, b_+219); ret_effect(gb); return;
L_53eb:
  CALL_C(b_+219, interactionAnimate_hook, SYM(interactionAnimate), b_+222);
  CYC(b_+222, b_+224); L = 0x46;
  CALL_C(b_+224, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+227);
  if (!(F & FZ)) { CYCT(b_+227, b_+228); ret_effect(gb); return; } CYC(b_+227, b_+228);
  CYC(b_+228, b_+230); A = 0xff;
  CYC(b_+230, b_+233); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(b_+233, b_+234); ret_effect(gb); return;
}

void interactionCode43__afterCall53b0_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53b0:
  CYC(b_+160, b_+162); L = 0x5c;
  CYC(b_+162, b_+164); mem_wr(gb, HL, 0x06);
  CYC(b_+164, b_+167); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
}

void interactionCode43__subid5__initialized_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53b7:
  CYC(b_+167, b_+169); E = 0x45;
  CYC(b_+169, b_+170); A = mem_rd(gb, DE);
  CYC(b_+170, b_+171); push_effect(gb, b_+171); SET_HL(bank09_jump_table(gb));
  do { uint16_t jt_ = (HL);
    if (jt_ == b_+177) { goto L_53c1; }
    else if (jt_ == b_+191) { goto L_53cf; }
    else if (jt_ == b_+219) { goto L_53eb; }
    else { HANDOFF(HL); }
  } while (0);
L_53c1:
  CYC(b_+177, b_+180); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CYC(b_+180, b_+182); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+182, b_+183); ret_effect(gb); return; } CYC(b_+182, b_+183);
  CYC(b_+183, b_+186); SET_HL((SYM(goronDance_sequenceData__platinum) + 98));
  CYC(b_+186, b_+188); E = 0x08;
  CYC(b_+188, b_+191); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53cf:
  CALL_C(b_+191, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+194);
  if ((F & FZ)) { CYCT(b_+194, b_+196); goto L_53dc; } CYC(b_+194, b_+196);
  CYC(b_+196, b_+199); SET_HL((SYM(goronDance_sequenceData__platinum) + 126));
  CYC(b_+199, b_+201); E = 0x08;
  CYC(b_+201, b_+204); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53dc:
  CALL_C(b_+204, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+207);
  CYC(b_+207, b_+209); L = 0x5c;
  CYC(b_+209, b_+211); mem_wr(gb, HL, 0x02);
  CYC(b_+211, b_+213); L = 0x46;
  CYC(b_+213, b_+215); mem_wr(gb, HL, 0xa4);
  CYC(b_+215, b_+216); L = alu_inc8(gb, L);
  CYC(b_+216, b_+218); mem_wr(gb, HL, 0x01);
  CYC(b_+218, b_+219); ret_effect(gb); return;
L_53eb:
  CALL_C(b_+219, interactionAnimate_hook, SYM(interactionAnimate), b_+222);
  CYC(b_+222, b_+224); L = 0x46;
  CALL_C(b_+224, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+227);
  if (!(F & FZ)) { CYCT(b_+227, b_+228); ret_effect(gb); return; } CYC(b_+227, b_+228);
  CYC(b_+228, b_+230); A = 0xff;
  CYC(b_+230, b_+233); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(b_+233, b_+234); ret_effect(gb); return;
}

void interactionCode43__subid5__substate0_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53c1:
  CYC(b_+177, b_+180); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CYC(b_+180, b_+182); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+182, b_+183); ret_effect(gb); return; } CYC(b_+182, b_+183);
  CYC(b_+183, b_+186); SET_HL((SYM(goronDance_sequenceData__platinum) + 98));
  CYC(b_+186, b_+188); E = 0x08;
  CYC(b_+188, b_+191); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
}

void interactionCode43__subid5__substate1_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53cf:
  CALL_C(b_+191, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+194);
  if ((F & FZ)) { CYCT(b_+194, b_+196); goto L_53dc; } CYC(b_+194, b_+196);
  CYC(b_+196, b_+199); SET_HL((SYM(goronDance_sequenceData__platinum) + 126));
  CYC(b_+199, b_+201); E = 0x08;
  CYC(b_+201, b_+204); if (hook_enabled_at(0x008a)) { interBankCall_hook(gb); return; } HANDOFF(0x008a);
L_53dc:
  CALL_C(b_+204, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+207);
  CYC(b_+207, b_+209); L = 0x5c;
  CYC(b_+209, b_+211); mem_wr(gb, HL, 0x02);
  CYC(b_+211, b_+213); L = 0x46;
  CYC(b_+213, b_+215); mem_wr(gb, HL, 0xa4);
  CYC(b_+215, b_+216); L = alu_inc8(gb, L);
  CYC(b_+216, b_+218); mem_wr(gb, HL, 0x01);
  CYC(b_+218, b_+219); ret_effect(gb); return;
}

void interactionCode43__subid5__substate2_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53eb:
  CALL_C(b_+219, interactionAnimate_hook, SYM(interactionAnimate), b_+222);
  CYC(b_+222, b_+224); L = 0x46;
  CALL_C(b_+224, decHlRef16WithCap_hook, SYM(decHlRef16WithCap), b_+227);
  if (!(F & FZ)) { CYCT(b_+227, b_+228); ret_effect(gb); return; } CYC(b_+227, b_+228);
  CYC(b_+228, b_+230); A = 0xff;
  CYC(b_+230, b_+233); mem_wr(gb, wTmpcfc0_genericCutscene_cfdf, A);
  CYC(b_+233, b_+234); ret_effect(gb); return;
}

void interactionCode43__subid6_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_53fa:
  CALL_C(b_+234, checkInteractionState_hook, SYM(checkInteractionState), b_+237);
  if (!(F & FZ)) { CYCT(b_+237, b_+239); goto L_5421; } CYC(b_+237, b_+239);
  CYC(b_+239, b_+242); SET_HL((wGroup4RoomFlags + 252));
  CYC(b_+242, b_+244); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+244, b_+246); goto L_541e; } CYC(b_+244, b_+246);
  CYC(b_+246, b_+249); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+249, b_+251); alu_bit(gb, 6, A);
  if ((F & FZ)) { CYCT(b_+251, b_+253); goto L_541e; } CYC(b_+251, b_+253);
  CALL_C(b_+253, interactionCode43__initGraphicsAndIncState_hook, b_+306, b_+256);
L_5410:
  CYC(b_+256, b_+258); L = 0x5c;
  CYC(b_+258, b_+260); mem_wr(gb, HL, 0x06);
  CYC(b_+260, b_+262); L = 0x43;
  CYC(b_+262, b_+263); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+263, b_+265); A = 0x06;
  CALL_C(b_+265, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+268);
  CYC(b_+268, b_+270); goto L_5421;
L_541e:
  CALL_C(b_+270, interactionCode43__initGraphicsIncStateAndLoadScript_hook, b_+315, b_+273);
L_5421:
  CYC(b_+273, b_+275); E = 0x43;
  CYC(b_+275, b_+276); A = mem_rd(gb, DE);
  CYC(b_+276, b_+277); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+277, b_+280); if (hook_enabled_at(SYM(interactionPushLinkAwayAndUpdateDrawPriority))) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; } HANDOFF(SYM(interactionPushLinkAwayAndUpdateDrawPriority)); } CYC(b_+277, b_+280);
  CALL_C(b_+280, interactionRunScript_hook, SYM(interactionRunScript), b_+283);
  if ((F & FC)) { CYCT(b_+283, b_+286); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+283, b_+286);
  CYC(b_+286, b_+289); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode43__afterCall5410_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5410:
  CYC(b_+256, b_+258); L = 0x5c;
  CYC(b_+258, b_+260); mem_wr(gb, HL, 0x06);
  CYC(b_+260, b_+262); L = 0x43;
  CYC(b_+262, b_+263); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+263, b_+265); A = 0x06;
  CALL_C(b_+265, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+268);
  CYC(b_+268, b_+270); goto L_5421;
L_5421:
  CYC(b_+273, b_+275); E = 0x43;
  CYC(b_+275, b_+276); A = mem_rd(gb, DE);
  CYC(b_+276, b_+277); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+277, b_+280); if (hook_enabled_at(SYM(interactionPushLinkAwayAndUpdateDrawPriority))) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; } HANDOFF(SYM(interactionPushLinkAwayAndUpdateDrawPriority)); } CYC(b_+277, b_+280);
  CALL_C(b_+280, interactionRunScript_hook, SYM(interactionRunScript), b_+283);
  if ((F & FC)) { CYCT(b_+283, b_+286); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+283, b_+286);
  CYC(b_+286, b_+289); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode43__subid6__initAndLoadScript_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_541e:
  CALL_C(b_+270, interactionCode43__initGraphicsIncStateAndLoadScript_hook, b_+315, b_+273);
L_5421:
  CYC(b_+273, b_+275); E = 0x43;
  CYC(b_+275, b_+276); A = mem_rd(gb, DE);
  CYC(b_+276, b_+277); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+277, b_+280); if (hook_enabled_at(SYM(interactionPushLinkAwayAndUpdateDrawPriority))) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; } HANDOFF(SYM(interactionPushLinkAwayAndUpdateDrawPriority)); } CYC(b_+277, b_+280);
  CALL_C(b_+280, interactionRunScript_hook, SYM(interactionRunScript), b_+283);
  if ((F & FC)) { CYCT(b_+283, b_+286); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+283, b_+286);
  CYC(b_+286, b_+289); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode43__initialized_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5421:
  CYC(b_+273, b_+275); E = 0x43;
  CYC(b_+275, b_+276); A = mem_rd(gb, DE);
  CYC(b_+276, b_+277); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+277, b_+280); if (hook_enabled_at(SYM(interactionPushLinkAwayAndUpdateDrawPriority))) { interactionPushLinkAwayAndUpdateDrawPriority_hook(gb); return; } HANDOFF(SYM(interactionPushLinkAwayAndUpdateDrawPriority)); } CYC(b_+277, b_+280);
  CALL_C(b_+280, interactionRunScript_hook, SYM(interactionRunScript), b_+283);
  if ((F & FC)) { CYCT(b_+283, b_+286); if (hook_enabled_at(SYM(interactionDelete))) { interactionDelete_hook(gb); return; } HANDOFF(SYM(interactionDelete)); } CYC(b_+283, b_+286);
  CYC(b_+286, b_+289); if (hook_enabled_at(SYM(interactionAnimateAsNpc))) { interactionAnimateAsNpc_hook(gb); return; } HANDOFF(SYM(interactionAnimateAsNpc));
}

void interactionCode43__subid7_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5431:
  CALL_C(b_+289, checkInteractionState_hook, SYM(checkInteractionState), b_+292);
  if ((F & FZ)) { CYCT(b_+292, b_+295); goto L_5438; } CYC(b_+292, b_+295);
  CYC(b_+295, b_+296); ret_effect(gb); return;
L_5438:
  CALL_C(b_+296, interactionCode43__initGraphicsAndIncState_hook, b_+306, b_+299);
L_543b:
  CYC(b_+299, b_+301); L = 0x5c;
  CYC(b_+301, b_+303); mem_wr(gb, HL, 0x06);
  CYC(b_+303, b_+306); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
}

void interactionCode43__afterCall543b_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_543b:
  CYC(b_+299, b_+301); L = 0x5c;
  CYC(b_+301, b_+303); mem_wr(gb, HL, 0x06);
  CYC(b_+303, b_+306); if (hook_enabled_at(SYM(objectSetVisiblec2))) { objectSetVisiblec2_hook(gb); return; } HANDOFF(SYM(objectSetVisiblec2));
}

void interactionCode43__initGraphicsAndIncState_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_5442:
  CALL_C(b_+306, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+309);
  CALL_C(b_+309, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+312);
  CYC(b_+312, b_+315); if (hook_enabled_at(SYM(interactionIncState))) { interactionIncState_hook(gb); return; } HANDOFF(SYM(interactionIncState));
}

void interactionCode43__initGraphicsIncStateAndLoadScript_hook(GB *gb) {
  BASE(interactionCode43);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_544b:
  CALL_C(b_+315, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+318);
  CALL_C(b_+318, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+321);
  CYC(b_+321, b_+323); A = 0x17;
  CALL_C(b_+323, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+326);
  CYC(b_+326, b_+328); E = 0x42;
  CYC(b_+328, b_+329); A = mem_rd(gb, DE);
  CYC(b_+329, b_+332); SET_HL(b_+342);
  CYC(b_+332, b_+333); push_effect(gb, b_+333);
  PUSH(0x0018, BC); CYC(0x0019, 0x001a); C = A; CYC(0x001a, 0x001c); B = 0x00; CYC(0x001c, 0x001d); alu_add_hl(gb, BC); CYC(0x001d, 0x001e); alu_add_hl(gb, BC); SET_BC(POP(0x001e)); CYC(0x001f, 0x0020); pop_effect(gb);
  CYC(b_+333, b_+334); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+334, b_+335); H = mem_rd(gb, HL);
  CYC(b_+335, b_+336); L = A;
  CALL_C(b_+336, interactionSetScript_hook, SYM(interactionSetScript), b_+339);
  CYC(b_+339, b_+342); if (hook_enabled_at(SYM(interactionIncState))) { interactionIncState_hook(gb); return; } HANDOFF(SYM(interactionIncState));
}
