#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/specialObjects/linkInCutscene.s, bank $06.

// Animates and applies the z speed with gravity $20; on landing, animation 0 (Z set).
// Otherwise, once the animation's parameter reaches 3 (bit 7 clear), it restarts the animation
// at state 3 and returns NZ.
void s_seasonsFunc_06_7178_hook(GB *gb) {
  BASE(seasonsFunc_06_7178);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+3);
  CYC(b_+3, b_+5); C = 0x20;
  CALL_C(b_+5, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+8);
  if (F & FZ) { CYCT(b_+8, b_+10); goto landed; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+13); L = 0x15; // SpecialObject.animParameter
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+19); alu_cp(gb, 0x03);
  if (F & FC) { RET_TAKEN(b_+19); return; }
  CYC(b_+19, b_+20);
  CYC(b_+20, b_+22); L = 0x14; // SpecialObject.animCounter
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+24, b_+26); A = 0x03;
  CYC(b_+26, b_+27); mem_wr(gb, HL, A);
  CYC(b_+27, b_+28); alu_or(gb, A);
  RET(b_+28); return;
landed:
  CYC(b_+29, b_+31); A = 0x00;
  CYC(b_+31, b_+34);
  TAIL(specialObjectSetAnimation);
}

// Sets animation a on the interaction in relatedObj2.
void s_setRelatedObj2Animation_hook(GB *gb) {
  BASE(setRelatedObj2Animation);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+2); push_effect(gb, DE);
  CYC(b_+2, b_+4); E = 0x19; // SpecialObject.relatedObj2
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+6); D = A;
  CYC(b_+6, b_+7); A = B;
  CALL_C(b_+7, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+10);
  CYC(b_+10, b_+11); SET_DE(pop_effect(gb));
  RET(b_+11); return;
}

static uint16_t linkInCutscene_d_jump_table(GB *gb) {
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

static void linkInCutscene_d_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void linkInCutscene_d_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void s_linkCutscene_oscillateZ_hook(GB *gb) {
  BASE(linkCutscene_oscillateZ);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcbb7);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); alu_and(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto L_6edd; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = B;
  CYC(b_+9, b_+11); alu_and(gb, 0x38);
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+14); alu_rlca(gb);
  CYC(b_+14, b_+15); linkInCutscene_d_add_a_to_hl(gb, b_+15);
  CYC(b_+15, b_+17); E = 0x0f;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
L_6edd:
  CYC(b_+20, b_+23);
  TAIL(specialObjectAnimate);
}

// linkCutscene1@seasonsFunc_06_6f71
static void linkInCutscene_d_seasonsFunc_06_6f71(GB *gb) {
  BASE(linkCutscene1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+96, b_+98); A = 0x07;
  CALL_C(b_+98, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+101);
  CYC(b_+101, b_+103); A = 0x08;
  CYC(b_+103, b_+106);
  TAIL_S(setRelatedObj2Animation);
}

// Dancing with Din
void s_linkCutscene1_hook(GB *gb) {
  BASE(linkCutscene1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+11) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+11);
  TAIL(linkCutscene_initOam_setVisible_incState);
state1:
  CYC(b_+11, b_+13); E = 0x05;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); push_effect(gb, b_+15);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+59) goto substate0;
    if (jt_ == b_+106) goto substate1;
    if (jt_ == b_+124) goto substate2;
    if (jt_ == b_+143) goto substate3;
    if (jt_ == b_+165) goto substate4;
    if (jt_ == b_+182) goto substate5;
    if (jt_ == b_+195) goto substate6;
    if (jt_ == b_+208) goto substate7;
    if (jt_ == b_+229) goto substate8;
    if (jt_ == b_+238) goto substate9;
    if (jt_ == b_+258) goto substateA;
    if (jt_ == b_+270) goto substateB;
    if (jt_ == b_+289) goto substateC;
    if (jt_ == b_+336) goto substateD;
    if (jt_ == b_+357) goto substateE;
    if (jt_ == b_+369) goto substateF;
    if (jt_ == b_+399) goto substate10;
    if (jt_ == b_+422) goto substate11;
    if (jt_ == b_+439) goto substate12;
    if (jt_ == b_+452) goto substate13;
    if (jt_ == b_+483) goto substate14;
    if (jt_ == b_+335) goto ret;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+59, b_+62); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+62, b_+63); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+63); return; }
  CYC(b_+63, b_+64);
  CALL_C(b_+64, s_itemIncSubstate, SYM(itemIncSubstate), b_+67);
  CYC(b_+67, b_+69); L = 0x06;
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0xaa);
  CYC(b_+71, b_+73); L = 0x0b;
  CYC(b_+73, b_+75); A = 0x30;
  CYC(b_+75, b_+76); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+76, b_+77); L = alu_inc8(gb, L);
  CYC(b_+77, b_+79); A = 0x50;
  CYC(b_+79, b_+80); mem_wr(gb, HL, A);
  CYC(b_+80, b_+82); L = 0x19;
  CYC(b_+82, b_+83); H = mem_rd(gb, HL);
  CYC(b_+83, b_+85); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+85, b_+87); A = 0x30;
  CYC(b_+87, b_+88); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+88, b_+89); L = alu_inc8(gb, L);
  CYC(b_+89, b_+91); A = 0x60;
  CYC(b_+91, b_+92); mem_wr(gb, HL, A);
  CYC(b_+92, b_+94); E = 0x08;
  CYC(b_+94, b_+95); alu_xor(gb, A);
  CYC(b_+95, b_+96); mem_wr(gb, DE, A);
seasonsFunc_06_6f71:
  CYC(b_+96, b_+98); A = 0x07;
  CALL_C(b_+98, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+101);
  CYC(b_+101, b_+103); A = 0x08;
  CYC(b_+103, b_+106);
  TAIL_S(setRelatedObj2Animation);
substate1:
  CALL_C(b_+106, s_itemDecCounter1, SYM(itemDecCounter1), b_+109);
  if (!(F & FZ)) { CYCT(b_+109, b_+111); goto animateSelfAndRelatedObj2; }
  CYC(b_+109, b_+111);
  CYC(b_+111, b_+113); mem_wr(gb, HL, 0x1e);
  CALL_C(b_+113, s_itemIncSubstate, SYM(itemIncSubstate), b_+116);
  CYC(b_+116, b_+118);
  goto seasonsFunc_06_6f71;
animateSelfAndRelatedObj2:
  CALL_C(b_+118, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+121);
  CYC(b_+121, b_+124);
  TAIL_S(animateRelatedObj2);
substate2:
  CALL_C(b_+124, s_itemDecCounter1, SYM(itemDecCounter1), b_+127);
  if (!(F & FZ)) { RET_TAKEN(b_+127); return; }
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0x28);
  CYC(b_+130, b_+132); A = 0x10;
  CALL_C(b_+132, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+135);
  CYC(b_+135, b_+137); A = 0x0d;
  CALL_C(b_+137, s_setRelatedObj2Animation_hook, SYM(setRelatedObj2Animation), b_+140);
  CYC(b_+140, b_+143);
  TAIL(itemIncSubstate);
substate3:
  CALL_C(b_+143, s_itemDecCounter1, SYM(itemDecCounter1), b_+146);
  if (!(F & FZ)) { RET_TAKEN(b_+146); return; }
  CYC(b_+146, b_+147);
  CYC(b_+147, b_+149); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+149, s_itemIncSubstate, SYM(itemIncSubstate), b_+152);
  CYC(b_+152, b_+155); SET_BC(0x0c17);
  CALL_C(b_+155, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+158);
  if (F & FZ) { CYCT(b_+158, b_+160); goto L_6fb3; }
  CYC(b_+158, b_+160);
  CYC(b_+160, b_+162); C = 0x18;
L_6fb3:
  CYC(b_+162, b_+165);
  TAIL(showText);
substate4:
  CYC(b_+165, b_+168); A = mem_rd(gb, wTextIsActive);
  CYC(b_+168, b_+169); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+169); return; }
  CYC(b_+169, b_+170);
  CALL_C(b_+170, s_itemDecCounter1, SYM(itemDecCounter1), b_+173);
  if (!(F & FZ)) { RET_TAKEN(b_+173); return; }
  CYC(b_+173, b_+174);
  CYC(b_+174, b_+176); mem_wr(gb, HL, 0x96);
  CALL_L(b_+176, linkInCutscene_d_seasonsFunc_06_6f71, b_+179);
  CYC(b_+179, b_+182);
  TAIL(itemIncSubstate);
substate5:
  CALL_C(b_+182, s_itemDecCounter1, SYM(itemDecCounter1), b_+185);
  if (!(F & FZ)) { CYCT(b_+185, b_+187); goto animateSelfAndRelatedObj2; }
  CYC(b_+185, b_+187);
  CYC(b_+187, b_+189); A = 0x02;
  CYC(b_+189, b_+192); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+192, b_+195);
  TAIL(itemIncSubstate);
substate6:
  CYC(b_+195, b_+198); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+198, b_+200); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+200, b_+202); goto animateSelfAndRelatedObj2; }
  CYC(b_+200, b_+202);
  CALL_L(b_+202, linkInCutscene_d_seasonsFunc_06_6f71, b_+205);
  CYC(b_+205, b_+208);
  TAIL(itemIncSubstate);
substate7:
  CYC(b_+208, b_+211); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+211, b_+213); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+213); return; }
  CYC(b_+213, b_+214);
  CALL_C(b_+214, s_itemIncSubstate, SYM(itemIncSubstate), b_+217);
  CYC(b_+217, b_+219); L = 0x06;
  CYC(b_+219, b_+221); mem_wr(gb, HL, 0x5a);
  CYC(b_+221, b_+223); L = 0x08;
  CYC(b_+223, b_+225); mem_wr(gb, HL, 0x03);
  CYC(b_+225, b_+226); alu_xor(gb, A);
  CYC(b_+226, b_+229);
  TAIL(specialObjectSetAnimation);
substate8:
  CALL_C(b_+229, s_itemDecCounter1, SYM(itemDecCounter1), b_+232);
  if (!(F & FZ)) { RET_TAKEN(b_+232); return; }
  CYC(b_+232, b_+233);
  CYC(b_+233, b_+235); mem_wr(gb, HL, 0x12);
  CYC(b_+235, b_+238);
  TAIL(itemIncSubstate);
substate9:
  CALL_C(b_+238, s_itemDecCounter1, SYM(itemDecCounter1), b_+241);
  if (!(F & FZ)) { CYCT(b_+241, b_+243); goto L_700d; }
  CYC(b_+241, b_+243);
  CYC(b_+243, b_+245); mem_wr(gb, HL, 0x46);
  CYC(b_+245, b_+246); alu_xor(gb, A);
  CALL_C(b_+246, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+249);
  CYC(b_+249, b_+252);
  TAIL(itemIncSubstate);
L_700d:
  CYC(b_+252, b_+254); L = 0x0d;
  CYC(b_+254, b_+255); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+255, b_+258);
  TAIL(specialObjectAnimate);
substateA:
  CALL_C(b_+258, s_itemDecCounter1, SYM(itemDecCounter1), b_+261);
  if (!(F & FZ)) { RET_TAKEN(b_+261); return; }
  CYC(b_+261, b_+262);
  CYC(b_+262, b_+265); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+265, b_+267); mem_wr(gb, HL, 0x05);
  CYC(b_+267, b_+270);
  TAIL(itemIncSubstate);
substateB:
  CYC(b_+270, b_+273); SET_HL(wTmpcfc0 + 0x11);
  CYC(b_+273, b_+275); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+275); return; }
  CYC(b_+275, b_+276);
seasonsFunc_06_7025:
  CYC(b_+276, b_+278); A = 0x14;
  CYC(b_+278, b_+280); E = 0x06;
  CYC(b_+280, b_+281); mem_wr(gb, DE, A);
  CYC(b_+281, b_+283); E = 0x0d;
  CYC(b_+283, b_+284); A = mem_rd(gb, DE);
  CYC(b_+284, b_+285); E = alu_dec8(gb, E);
  CYC(b_+285, b_+286); mem_wr(gb, DE, A);
  CYC(b_+286, b_+289);
  TAIL(itemIncSubstate);
substateC:
  CALL_C(b_+289, s_itemDecCounter1, SYM(itemDecCounter1), b_+292);
  if (!(F & FZ)) { CYCT(b_+292, b_+294); goto seasonsFunc_06_7052; }
  CYC(b_+292, b_+294);
  CYC(b_+294, b_+295); H = D;
  CYC(b_+295, b_+297); L = 0x10;
  CYC(b_+297, b_+299); mem_wr(gb, HL, 0x50);
  CYC(b_+299, b_+301); L = 0x09;
  CYC(b_+301, b_+303); mem_wr(gb, HL, 0x0e);
  CYC(b_+303, b_+305); L = 0x0d;
  CYC(b_+305, b_+307); mem_wr(gb, HL, 0x40);
  CYC(b_+307, b_+309); A = 0x08;
  CALL_C(b_+309, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+312);
  CYC(b_+312, b_+315); SET_BC(0xfe80);
  CALL_C(b_+315, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+318);
  CYC(b_+318, b_+321);
  TAIL(itemIncSubstate);
seasonsFunc_06_7052:
  CALL_C(b_+321, s_getRandomNumber, SYM(getRandomNumber), b_+324);
  CYC(b_+324, b_+326); alu_and(gb, 0x0f);
  CYC(b_+326, b_+328); alu_sub(gb, 0x08);
  CYC(b_+328, b_+329); B = A;
  CYC(b_+329, b_+331); E = 0x0c;
  CYC(b_+331, b_+332); A = mem_rd(gb, DE);
  CYC(b_+332, b_+333); E = alu_inc8(gb, E);
  CYC(b_+333, b_+334); alu_add(gb, B);
  CYC(b_+334, b_+335); mem_wr(gb, DE, A);
ret:
  RET(b_+335); return;
substateD:
  CALL_C(b_+336, s_objectApplySpeed, SYM(objectApplySpeed), b_+339);
  CYC(b_+339, b_+341); C = 0x20;
  CALL_C(b_+341, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+344);
  if (!(F & FZ)) { RET_TAKEN(b_+344); return; }
  CYC(b_+344, b_+345);
  CALL_C(b_+345, s_itemIncSubstate, SYM(itemIncSubstate), b_+348);
  CYC(b_+348, b_+350); L = 0x06;
  CYC(b_+350, b_+352); mem_wr(gb, HL, 0x28);
  CYC(b_+352, b_+354); A = 0x14;
  CYC(b_+354, b_+357);
  TAIL(specialObjectSetAnimation);
substateE:
  CALL_C(b_+357, s_itemDecCounter1, SYM(itemDecCounter1), b_+360);
  if (!(F & FZ)) { RET_TAKEN(b_+360); return; }
  CYC(b_+360, b_+361);
  CYC(b_+361, b_+363); A = 0x07;
  CYC(b_+363, b_+366); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+366, b_+369);
  TAIL(itemIncSubstate);
substateF:
  CYC(b_+369, b_+372); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+372, b_+374); alu_cp(gb, 0x09);
  if (!(F & FZ)) { RET_TAKEN(b_+374); return; }
  CYC(b_+374, b_+375);
  CALL_C(b_+375, s_itemIncSubstate, SYM(itemIncSubstate), b_+378);
  CYC(b_+378, b_+380); L = 0x14;
  CYC(b_+380, b_+382); mem_wr(gb, HL, 0xf0);
  CYC(b_+382, b_+383); L = alu_inc8(gb, L);
  CYC(b_+383, b_+385); mem_wr(gb, HL, 0xfd);
  CYC(b_+385, b_+387); L = 0x08;
  CYC(b_+387, b_+389); mem_wr(gb, HL, 0x02);
  CYC(b_+389, b_+391); A = 0x0a;
  CALL_C(b_+391, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+394);
  CYC(b_+394, b_+396); A = 0x53;
  CYC(b_+396, b_+399);
  TAIL(playSound_b00);
substate10:
  CALL_C(b_+399, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+402);
  CYC(b_+402, b_+404); C = 0x20;
  CALL_C(b_+404, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+407);
  if (!(F & FZ)) { RET_TAKEN(b_+407); return; }
  CYC(b_+407, b_+408);
  CALL_C(b_+408, s_itemIncSubstate, SYM(itemIncSubstate), b_+411);
  CYC(b_+411, b_+413); L = 0x06;
  CYC(b_+413, b_+415); mem_wr(gb, HL, 0x1e);
  CYC(b_+415, b_+416); alu_xor(gb, A);
  CYC(b_+416, b_+418); L = 0x08;
  CYC(b_+418, b_+419); mem_wr(gb, HL, A);
  CYC(b_+419, b_+422);
  TAIL(specialObjectSetAnimation);
substate11:
  CALL_C(b_+422, s_itemDecCounter1, SYM(itemDecCounter1), b_+425);
  if (!(F & FZ)) { RET_TAKEN(b_+425); return; }
  CYC(b_+425, b_+426);
  CYC(b_+426, b_+428); mem_wr(gb, HL, 0x19);
  CYC(b_+428, b_+430); L = 0x10;
  CYC(b_+430, b_+432); mem_wr(gb, HL, 0x50);
  CYC(b_+432, b_+434); L = 0x09;
  CYC(b_+434, b_+436); mem_wr(gb, HL, 0x02);
  CYC(b_+436, b_+439);
  TAIL(itemIncSubstate);
substate12:
  CALL_C(b_+439, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+442);
  CALL_C(b_+442, s_objectApplySpeed, SYM(objectApplySpeed), b_+445);
  CALL_C(b_+445, s_itemDecCounter1, SYM(itemDecCounter1), b_+448);
  if (!(F & FZ)) { RET_TAKEN(b_+448); return; }
  CYC(b_+448, b_+449);
  CYC(b_+449, b_+452);
  goto seasonsFunc_06_7025;
substate13:
  CALL_C(b_+452, s_itemDecCounter1, SYM(itemDecCounter1), b_+455);
  if (!(F & FZ)) { CYCT(b_+455, b_+458); goto seasonsFunc_06_7052; }
  CYC(b_+455, b_+458);
  CYC(b_+458, b_+460); E = 0x10;
  CYC(b_+460, b_+462); A = 0x78;
  CYC(b_+462, b_+463); mem_wr(gb, DE, A);
  CYC(b_+463, b_+465); E = 0x09;
  CYC(b_+465, b_+467); A = 0x19;
  CYC(b_+467, b_+468); mem_wr(gb, DE, A);
  CYC(b_+468, b_+470); E = 0x08;
  CYC(b_+470, b_+471); alu_xor(gb, A);
  CYC(b_+471, b_+472); mem_wr(gb, DE, A);
  CYC(b_+472, b_+475); mem_wr(gb, wScrollMode, A);
  CYC(b_+475, b_+477); A = 0x08;
  CALL_C(b_+477, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+480);
  CYC(b_+480, b_+483);
  TAIL(itemIncSubstate);
substate14:
  CALL_C(b_+483, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+486);
  CALL_C(b_+486, s_objectApplySpeed, SYM(objectApplySpeed), b_+489);
  CALL_C(b_+489, s_objectApplySpeed, SYM(objectApplySpeed), b_+492);
  CALL_C(b_+492, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+495);
  if (F & FC) { RET_TAKEN(b_+495); return; }
  CYC(b_+495, b_+496);
  CYC(b_+496, b_+498); A = 0x0a;
  CYC(b_+498, b_+501); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+501, b_+504);
  TAIL(itemIncSubstate);
}

void s_linkCutscene2_hook(GB *gb) {
  BASE(linkCutscene2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+16) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_linkCutscene_initOam_setVisible_incState, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); A = 0x09;
  CALL_C(b_+13, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+16);
state1:
  CYC(b_+16, b_+18); E = 0x05;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); push_effect(gb, b_+20);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+28) goto substate0;
    if (jt_ == b_+67) goto substate1;
    if (jt_ == b_+79) goto substate2;
    if (jt_ == b_+100) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+28, b_+31); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL);
  CYC(b_+32, b_+34); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CALL_C(b_+35, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+38);
  CYC(b_+38, b_+40); E = 0x21;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+42); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+42); return; }
  CYC(b_+42, b_+43);
  CALL_C(b_+43, s_itemIncSubstate, SYM(itemIncSubstate), b_+46);
  CYC(b_+46, b_+48); L = 0x14;
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0xf0);
  CYC(b_+50, b_+51); L = alu_inc8(gb, L);
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0xfd);
  CYC(b_+53, b_+55); L = 0x08;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x02);
  CYC(b_+57, b_+59); A = 0x0a;
  CALL_C(b_+59, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+62);
  CYC(b_+62, b_+64); A = 0x53;
  CALL_C(b_+64, s_playSound, SYM(playSound_b00), b_+67);
substate1:
  CALL_C(b_+67, s_seasonsFunc_06_7178_hook, SYM(seasonsFunc_06_7178), b_+70);
  if (!(F & FZ)) { RET_TAKEN(b_+70); return; }
  CYC(b_+70, b_+71);
  CALL_C(b_+71, s_itemIncSubstate, SYM(itemIncSubstate), b_+74);
  CYC(b_+74, b_+76); L = 0x06;
  CYC(b_+76, b_+78); mem_wr(gb, HL, 0x1e);
  RET(b_+78); return;
substate2:
  CALL_C(b_+79, s_itemDecCounter1, SYM(itemDecCounter1), b_+82);
  if (!(F & FZ)) { RET_TAKEN(b_+82); return; }
  CYC(b_+82, b_+83);
  CYC(b_+83, b_+86); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x02);
  CALL_C(b_+88, s_itemIncSubstate, SYM(itemIncSubstate), b_+91);
  CYC(b_+91, b_+93); L = 0x08;
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x03);
  CYC(b_+95, b_+97); A = 0x00;
  CYC(b_+97, b_+100);
  TAIL(specialObjectSetAnimation);
substate3:
  CYC(b_+100, b_+103); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+103, b_+105); alu_cp(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+105); return; }
  CYC(b_+105, b_+106);
  CYC(b_+106, b_+108); A = 0x00;
  CYC(b_+108, b_+111);
  TAIL(setLinkIDOverride);
}

void s_animateRelatedObj2_hook(GB *gb) {
  BASE(animateRelatedObj2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+3); E = 0x19;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); D = A;
  CALL_C(b_+5, s_interactionAnimate, SYM(interactionAnimate), b_+8);
  CYC(b_+8, b_+9); SET_DE(pop_effect(gb));
  RET(b_+9); return;
}

void s_linkCutscene3_hook(GB *gb) {
  BASE(linkCutscene3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+20) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_linkCutscene_initOam_setVisible_incState, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); L = 0x06;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xa8);
  CYC(b_+15, b_+17); A = 0x0c;
  CYC(b_+17, b_+20);
  TAIL(specialObjectSetAnimation);
state1:
  CYC(b_+20, b_+22); E = 0x05;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); push_effect(gb, b_+24);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+32) goto substate0;
    if (jt_ == b_+54) goto substate1;
    if (jt_ == b_+68) goto substate2;
    if (jt_ == b_+82) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+32, s_itemDecCounter1, SYM(itemDecCounter1), b_+35);
  if (!(F & FZ)) { CYCT(b_+35, b_+37); goto L_71e3; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+39); A = 0x80;
  CYC(b_+39, b_+42); mem_wr(gb, wTmpcfc0, A);
  CALL_C(b_+42, s_itemIncSubstate, SYM(itemIncSubstate), b_+45);
  CYC(b_+45, b_+48); SET_BC(IO_P1);
  CALL_C(b_+48, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+51);
L_71e3:
  CYC(b_+51, b_+54);
  TAIL(specialObjectAnimate);
substate1:
  CYC(b_+54, b_+56); C = 0x20;
  CALL_C(b_+56, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+59);
  if (!(F & FZ)) { RET_TAKEN(b_+59); return; }
  CYC(b_+59, b_+60);
  CALL_C(b_+60, s_itemIncSubstate, SYM(itemIncSubstate), b_+63);
  CYC(b_+63, b_+65); L = 0x06;
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x0a);
  RET(b_+67); return;
substate2:
  CALL_C(b_+68, s_itemDecCounter1, SYM(itemDecCounter1), b_+71);
  if (!(F & FZ)) { RET_TAKEN(b_+71); return; }
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x78);
  CALL_C(b_+74, s_itemIncSubstate, SYM(itemIncSubstate), b_+77);
  CYC(b_+77, b_+79); A = 0x0c;
  CYC(b_+79, b_+82);
  TAIL(specialObjectSetAnimation);
substate3:
  CALL_C(b_+82, s_itemDecCounter1, SYM(itemDecCounter1), b_+85);
  if (!(F & FZ)) { RET_TAKEN(b_+85); return; }
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+88); A = 0x01;
  CYC(b_+88, b_+91); mem_wr(gb, wTmpcfc0 + 0x1f, A);
  RET(b_+91); return;
}

void s_linkCutscene4_hook(GB *gb) {
  BASE(linkCutscene4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+28) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_linkCutscene_initOam_setVisible_incState, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); L = 0x09;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  CYC(b_+15, b_+17); L = 0x10;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x28);
  CYC(b_+19, b_+21); L = 0x06;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x80);
  CYC(b_+23, b_+25); A = 0x00;
  CYC(b_+25, b_+28);
  TAIL(specialObjectSetAnimation);
state1:
  CYC(b_+28, b_+30); E = 0x05;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+44) goto substate0;
    if (jt_ == b_+64) goto substate1;
    if (jt_ == b_+78) goto substate2;
    if (jt_ == b_+90) goto substate3;
    if (jt_ == b_+114) goto substate4;
    if (jt_ == b_+135) goto substate5;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+44, b_+47); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+47, b_+48); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+48); return; }
  CYC(b_+48, b_+49);
  CALL_C(b_+49, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+52);
  CALL_C(b_+52, s_objectApplySpeed, SYM(objectApplySpeed), b_+55);
  CALL_C(b_+55, s_itemDecCounter1, SYM(itemDecCounter1), b_+58);
  if (!(F & FZ)) { RET_TAKEN(b_+58); return; }
  CYC(b_+58, b_+59);
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x06);
  CYC(b_+61, b_+64);
  TAIL(itemIncSubstate);
substate1:
  CALL_C(b_+64, s_itemDecCounter1, SYM(itemDecCounter1), b_+67);
  if (!(F & FZ)) { RET_TAKEN(b_+67); return; }
  CYC(b_+67, b_+68);
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x78);
  CALL_C(b_+70, s_itemIncSubstate, SYM(itemIncSubstate), b_+73);
  CYC(b_+73, b_+75); A = 0x03;
  CYC(b_+75, b_+78);
  TAIL(specialObjectSetAnimation);
substate2:
  CALL_C(b_+78, s_itemDecCounter1, SYM(itemDecCounter1), b_+81);
  if (!(F & FZ)) { RET_TAKEN(b_+81); return; }
  CYC(b_+81, b_+82);
  CYC(b_+82, b_+85); SET_HL(wTmpcfc0);
  CYC(b_+85, b_+87); mem_wr(gb, HL, 0x01);
  CYC(b_+87, b_+90);
  TAIL(itemIncSubstate);
substate3:
  CYC(b_+90, b_+93); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+93, b_+95); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+95); return; }
  CYC(b_+95, b_+96);
  CALL_C(b_+96, s_itemIncSubstate, SYM(itemIncSubstate), b_+99);
  CYC(b_+99, b_+101); L = 0x09;
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x10);
  CYC(b_+103, b_+106); SET_BC(IO_P1);
  CALL_C(b_+106, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+109);
  CYC(b_+109, b_+111); A = 0x0d;
  CYC(b_+111, b_+114);
  TAIL(specialObjectSetAnimation);
substate4:
  CALL_C(b_+114, s_objectApplySpeed, SYM(objectApplySpeed), b_+117);
  CYC(b_+117, b_+119); C = 0x20;
  CALL_C(b_+119, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+122);
  if (!(F & FZ)) { RET_TAKEN(b_+122); return; }
  CYC(b_+122, b_+123);
  CALL_C(b_+123, s_itemIncSubstate, SYM(itemIncSubstate), b_+126);
  CYC(b_+126, b_+128); L = 0x06;
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0x78);
  CYC(b_+130, b_+132); L = 0x20;
  CYC(b_+132, b_+134); mem_wr(gb, HL, 0x01);
  RET(b_+134); return;
substate5:
  CALL_C(b_+135, s_itemDecCounter1, SYM(itemDecCounter1), b_+138);
  if (!(F & FZ)) { CYCT(b_+138, b_+141); TAIL(specialObjectAnimate); }
  CYC(b_+138, b_+141);
  CYC(b_+141, b_+144); SET_HL(wTmpcfc0 + 0x1f);
  CYC(b_+144, b_+146); mem_wr(gb, HL, 0x01);
  RET(b_+146); return;
}

// Sokra?
void s_linkCutscene5_hook(GB *gb) {
  BASE(linkCutscene5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+20) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_linkCutscene_initOam_setVisible_incState, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); L = 0x06;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xf0);
  CYC(b_+15, b_+17); A = 0x03;
  CYC(b_+17, b_+20);
  TAIL(specialObjectSetAnimation);
state1:
  CYC(b_+20, b_+22); E = 0x05;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); push_effect(gb, b_+24);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+28) goto substate0;
    if (jt_ == b_+66) goto substate1;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+28, b_+31); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CALL_C(b_+33, s_itemDecCounter1, SYM(itemDecCounter1), b_+36);
  if (!(F & FZ)) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+39); L = 0x06;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+41, s_itemIncSubstate, SYM(itemIncSubstate), b_+44);
  CYC(b_+44, b_+47); SET_HL(wTmpcfc0);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x01);
  CYC(b_+49, b_+52); SET_BC(0xf804);
  CYC(b_+52, b_+54); A = 0xff;
  CALL_C(b_+54, s_objectCreateExclamationMark, SYM(objectCreateExclamationMark), b_+57);
  CYC(b_+57, b_+59); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x01);
  CYC(b_+61, b_+63); A = 0x0e;
  CYC(b_+63, b_+66);
  TAIL(specialObjectSetAnimation);
substate1:
  CALL_C(b_+66, s_itemDecCounter1, SYM(itemDecCounter1), b_+69);
  if (!(F & FZ)) { RET_TAKEN(b_+69); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+73); SET_HL(wTmpcfc0 + 0x1f);
  CYC(b_+73, b_+75); mem_wr(gb, HL, 0x01);
  RET(b_+75); return;
}

// linkCutscene6@checkShieldEquipped
static void linkInCutscene_d_checkShieldEquipped(GB *gb) {
  BASE(linkCutscene6);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+26, b_+29); SET_HL(wInventoryB);
  CYC(b_+29, b_+31); A = 0x01;
  CYC(b_+31, b_+32); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+34); L = alu_inc8(gb, L);
  CYC(b_+34, b_+35); alu_cp(gb, mem_rd(gb, HL));
  RET(b_+35); return;
}

// Link being kissed by Zelda in ending cutscene - cutsceneA in ages
void s_linkCutscene6_hook(GB *gb) {
  BASE(linkCutscene6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+36) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_linkCutscene_initOam_setVisible_incState, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CALL_C(b_+11, s_objectSetInvisible, SYM(objectSetInvisible), b_+14);
  CALL_L(b_+14, linkInCutscene_d_checkShieldEquipped, b_+17);
  CYC(b_+17, b_+19); A = 0x0b;
  if (!(F & FZ)) { CYCT(b_+19, b_+21); goto L_7302; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); A = 0x0f;
L_7302:
  CYC(b_+23, b_+26);
  TAIL(specialObjectSetAnimation);

state1:
  CYC(b_+36, b_+38); E = 0x05;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); push_effect(gb, b_+40);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+48) goto substate0;
    if (jt_ == b_+60) goto substate1;
    if (jt_ == b_+80) goto substate2;
    if (jt_ == b_+120) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+48, b_+51); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+51, b_+53); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CALL_C(b_+54, s_itemIncSubstate, SYM(itemIncSubstate), b_+57);
  CYC(b_+57, b_+60);
  TAIL(objectSetVisible);
substate1:
  CYC(b_+60, b_+63); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+63, b_+65); alu_cp(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CALL_C(b_+66, s_itemIncSubstate, SYM(itemIncSubstate), b_+69);
  CALL_L(b_+69, linkInCutscene_d_checkShieldEquipped, b_+72);
  CYC(b_+72, b_+74); A = 0x10;
  if (!(F & FZ)) { CYCT(b_+74, b_+76); goto L_7338; }
  CYC(b_+74, b_+76);
  CYC(b_+76, b_+77); A = alu_inc8(gb, A);
L_7338:
  CYC(b_+77, b_+80);
  TAIL(specialObjectSetAnimation);
substate2:
  CYC(b_+80, b_+83); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+83, b_+85); alu_cp(gb, 0x08);
  if (!(F & FZ)) { RET_TAKEN(b_+85); return; }
  CYC(b_+85, b_+86);
  CALL_C(b_+86, s_itemIncSubstate, SYM(itemIncSubstate), b_+89);
  CYC(b_+89, b_+91); L = 0x06;
  CYC(b_+91, b_+93); mem_wr(gb, HL, 0x68);
  CYC(b_+93, b_+94); L = alu_inc8(gb, L);
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x01);
  CYC(b_+96, b_+98); B = 0x02;
L_734d:
  CALL_C(b_+98, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+101);
  if (!(F & FZ)) { CYCT(b_+101, b_+103); goto substate2_setAnimation; }
  CYC(b_+101, b_+103);
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0xb7);
  CYC(b_+105, b_+106); L = alu_inc8(gb, L);
  CYC(b_+106, b_+107); A = B;
  CYC(b_+107, b_+108); A = alu_dec8(gb, A);
  CYC(b_+108, b_+109); mem_wr(gb, HL, A);
  CALL_C(b_+109, s_objectCopyPosition, SYM(objectCopyPosition), b_+112);
  CYC(b_+112, b_+113); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+113, b_+115); goto L_734d; }
  CYC(b_+113, b_+115);
substate2_setAnimation:
  CYC(b_+115, b_+117); A = 0x12;
  CYC(b_+117, b_+120);
  TAIL(specialObjectSetAnimation);
substate3:
  CALL_C(b_+120, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+123);
  CYC(b_+123, b_+124); H = D;
  CYC(b_+124, b_+126); L = 0x06;
  CALL_C(b_+126, s_decHlRef16WithCap, SYM(decHlRef16WithCap), b_+129);
  if (!(F & FZ)) { RET_TAKEN(b_+129); return; }
  CYC(b_+129, b_+130);
  CYC(b_+130, b_+133); SET_HL(wTmpcfc0);
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x09);
  RET(b_+135); return;
}

// linkCutscene5@ret
static void linkInCutscene_d_ret(GB *gb) {
  BASE(linkCutscene5);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  RET(b_+75); return;
}

// linkCutscene5@seasonsFunc_06_72d0
static void linkInCutscene_d_seasonsFunc_06_72d0(GB *gb) {
  BASE(linkCutscene5);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+49, b_+52); SET_BC(0xf804);
  CYC(b_+52, b_+54); A = 0xff;
  CALL_C(b_+54, s_objectCreateExclamationMark, SYM(objectCreateExclamationMark), b_+57);
  CYC(b_+57, b_+59); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x01);
  CYC(b_+61, b_+63); A = 0x0e;
  CYC(b_+63, b_+66);
  TAIL(specialObjectSetAnimation);
}

// Sokra?
void s_linkCutscene7_hook(GB *gb) {
  BASE(linkCutscene7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == SYM(linkCutscene5__ret)) { linkInCutscene_d_ret(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_linkCutscene_initOam_setVisible_incState, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+14);
  linkInCutscene_d_seasonsFunc_06_72d0(gb); return;
}

void s_linkCutscene8_hook(GB *gb) {
  BASE(linkCutscene8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+20) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_linkCutscene_initOam_setVisible_incState, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+13); L = 0x10;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x28);
  CYC(b_+15, b_+17); A = 0x00;
  CALL_C(b_+17, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+20);
state1:
  CALL_C(b_+20, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+23);
  CALL_C(b_+23, s_angleToY48X50_hook, SYM(angleToY48X50), b_+26);
  CALL_C(b_+26, s_moveToAngleSnapToGrid_hook, SYM(moveToAngleSnapToGrid), b_+29);
  CALL_C(b_+29, s_checkCloseToY48X50_hook, SYM(checkCloseToY48X50), b_+32);
  if (!(F & FC)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); A = 0x00;
  CYC(b_+35, b_+38);
  TAIL(setLinkIDOverride);
}

void s_linkCutscene9_hook(GB *gb) {
  BASE(linkCutscene9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+21) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_linkCutscene_initOam_setVisible_incState, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CYC(b_+11, b_+12); push_effect(gb, DE);
  CALL_C(b_+12, s_clearItems, SYM(clearItems), b_+15);
  CYC(b_+15, b_+16); SET_DE(pop_effect(gb));
  CYC(b_+16, b_+18); A = 0x13;
  CYC(b_+18, b_+21);
  TAIL(specialObjectSetAnimation);
state1:
  CYC(b_+21, b_+23); E = 0x05;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); push_effect(gb, b_+25);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+39) goto substate0;
    if (jt_ == b_+60) goto substate1;
    if (jt_ == b_+71) goto substate2;
    if (jt_ == b_+97) goto substate3;
    if (jt_ == b_+120) goto substate4;
    if (jt_ == b_+134) goto substate5;
    if (jt_ == b_+156) goto ret;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+39, b_+42); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+42, b_+43); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+43); return; }
  CYC(b_+43, b_+44);
  CALL_C(b_+44, s_itemIncSubstate, SYM(itemIncSubstate), b_+47);
  CYC(b_+47, b_+49); L = 0x06;
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x28);
  CYC(b_+51, b_+53); L = 0x10;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0x05);
  CYC(b_+55, b_+57); L = 0x09;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x10);
  RET(b_+59); return;
substate1:
  CALL_C(b_+60, s_itemDecCounter1, SYM(itemDecCounter1), b_+63);
  if (!(F & FZ)) { CYCT(b_+63, b_+66); TAIL(objectApplySpeed); }
  CYC(b_+63, b_+66);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x19);
  CYC(b_+68, b_+71);
  TAIL(itemIncSubstate);
substate2:
  CALL_C(b_+71, s_itemDecCounter1, SYM(itemDecCounter1), b_+74);
  if (!(F & FZ)) { RET_TAKEN(b_+74); return; }
  CYC(b_+74, b_+75);
  CALL_C(b_+75, s_itemIncSubstate, SYM(itemIncSubstate), b_+78);
  CYC(b_+78, b_+80); L = 0x10;
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x78);
  CYC(b_+82, b_+84); L = 0x09;
  CYC(b_+84, b_+85); alu_xor(gb, A);
  CYC(b_+85, b_+86); mem_wr(gb, HL, A);
  CYC(b_+86, b_+88); L = 0x0f;
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0xfa);
animate:
  CYC(b_+90, b_+92); L = 0x20;
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x01);
  CYC(b_+94, b_+97);
  TAIL(specialObjectAnimate);
substate3:
  CALL_C(b_+97, s_objectApplySpeed, SYM(objectApplySpeed), b_+100);
  CYC(b_+100, b_+102); E = 0x0b;
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);
  CYC(b_+103, b_+105); alu_cp(gb, 0x10);
  if (!(F & FC)) { RET_TAKEN(b_+105); return; }
  CYC(b_+105, b_+106);
  CYC(b_+106, b_+108); A = 0x82;
  CALL_C(b_+108, s_playSound, SYM(playSound_b00), b_+111);
  CALL_C(b_+111, s_itemIncSubstate, SYM(itemIncSubstate), b_+114);
  CYC(b_+114, b_+116); L = 0x06;
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x1e);
  CYC(b_+118, b_+120);
  goto animate;
substate4:
  CALL_C(b_+120, s_itemDecCounter1, SYM(itemDecCounter1), b_+123);
  if (!(F & FZ)) { CYCT(b_+123, b_+125); goto L_7446; }
  CYC(b_+123, b_+125);
  CALL_C(b_+125, s_itemIncSubstate, SYM(itemIncSubstate), b_+128);
  CYC(b_+128, b_+131); SET_BC(IO_LCDC);
  CYC(b_+131, b_+134);
  TAIL(objectSetSpeedZ);
substate5:
  CYC(b_+134, b_+136); C = 0x10;
  CALL_C(b_+136, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+139);
  if (!(F & FZ)) { RET_TAKEN(b_+139); return; }
  CYC(b_+139, b_+140);
  CALL_C(b_+140, s_itemIncSubstate, SYM(itemIncSubstate), b_+143);
  CYC(b_+143, b_+145);
  goto animate;
L_7446:
  CYC(b_+145, b_+148); A = mem_rd(gb, wFrameCounter);
  CYC(b_+148, b_+150); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+150); return; }
  CYC(b_+150, b_+151);
  CYC(b_+151, b_+153); A = 0x04;
  CYC(b_+153, b_+156); mem_wr(gb, wScreenShakeCounterY, A);
ret:
  RET(b_+156); return;
}

// Cutscene played on starting a new game ("accept our quest, hero") - cutsceneA in ages
void s_linkCutsceneA_hook(GB *gb) {
  BASE(linkCutsceneA);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+52) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_linkCutscene_initOam_setVisible_incState, SYM(linkCutscene_initOam_setVisible_incState), b_+11);
  CALL_C(b_+11, s_objectSetVisible81, SYM(objectSetVisible81), b_+14);
  CYC(b_+14, b_+16); L = 0x06;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x2c);
  CYC(b_+18, b_+19); SET_HL(HL + 1);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x01);
  CYC(b_+21, b_+23); L = 0x0b;
  CYC(b_+23, b_+25); mem_wr(gb, HL, 0xd0);
  CYC(b_+25, b_+27); L = 0x0d;
  CYC(b_+27, b_+29); mem_wr(gb, HL, 0x50);
  CYC(b_+29, b_+31); A = 0x08;
  CALL_C(b_+31, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+34);
  CYC(b_+34, b_+35); alu_xor(gb, A);
  CYC(b_+35, b_+38); mem_wr(gb, wTmpcbb9, A);
  CYC(b_+38, b_+41); SET_BC(0x8409);
  CALL_C(b_+41, s_objectCreateInteraction, SYM(objectCreateInteraction), b_+44);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); goto state1; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+48, b_+50); A = 0x00;
  CYC(b_+50, b_+51); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+51, b_+52); mem_wr(gb, HL, D);
state1:
  CYC(b_+52, b_+55); A = mem_rd(gb, wFrameCounter);
  CYC(b_+55, b_+58); mem_wr(gb, wTmpcbb7, A);
  CYC(b_+58, b_+60); E = 0x05;
  CYC(b_+60, b_+61); A = mem_rd(gb, DE);
  CYC(b_+61, b_+62); push_effect(gb, b_+62);
  do { uint16_t jt_ = (linkInCutscene_d_jump_table(gb));
    if (jt_ == b_+70) goto substate0;
    if (jt_ == b_+85) goto substate1;
    if (jt_ == b_+101) goto substate2;
    if (jt_ == b_+125) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+70, s_linkCutscene_oscillateZ_2, SYM(linkCutscene_oscillateZ_2), b_+73);
  CYC(b_+73, b_+76); SET_HL(w1Link_counter1);
  CALL_C(b_+76, s_decHlRef16WithCap, SYM(decHlRef16WithCap), b_+79);
  if (!(F & FZ)) { RET_TAKEN(b_+79); return; }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x3c);
  CYC(b_+82, b_+85);
  TAIL(itemIncSubstate);
substate1:
  CALL_C(b_+85, s_linkCutscene_oscillateZ_2, SYM(linkCutscene_oscillateZ_2), b_+88);
  CALL_C(b_+88, s_itemDecCounter1, SYM(itemDecCounter1), b_+91);
  if (!(F & FZ)) { RET_TAKEN(b_+91); return; }
  CYC(b_+91, b_+92);
  CALL_C(b_+92, s_itemIncSubstate, SYM(itemIncSubstate), b_+95);
  CYC(b_+95, b_+98); SET_BC(0x0c16);
  CYC(b_+98, b_+101);
  TAIL(showText);
substate2:
  CYC(b_+101, b_+104); SET_HL(SYM(linkCutscene_zOscillation1));
  CALL_C(b_+104, s_linkCutscene_oscillateZ_hook, SYM(linkCutscene_oscillateZ), b_+107);
  CYC(b_+107, b_+110); A = mem_rd(gb, wTextIsActive);
  CYC(b_+110, b_+111); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+111); return; }
  CYC(b_+111, b_+112);
  CYC(b_+112, b_+114); A = 0x06;
  CYC(b_+114, b_+117); mem_wr(gb, wTmpcbb9, A);
  CYC(b_+117, b_+119); A = 0x91;
  CALL_C(b_+119, s_playSound, SYM(playSound_b00), b_+122);
  CYC(b_+122, b_+125);
  TAIL(linkCutscene_createGlowingOrb);
substate3:
  CYC(b_+125, b_+127); E = 0x21;
  CYC(b_+127, b_+128); A = mem_rd(gb, DE);
  CYC(b_+128, b_+129); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+129, b_+131); goto L_74db; }
  CYC(b_+129, b_+131);
  CYC(b_+131, b_+133); A = 0x07;
  CYC(b_+133, b_+136); mem_wr(gb, wTmpcbb9, A);
  RET(b_+136); return;
L_74db:
  CALL_C(b_+137, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+140);
  CYC(b_+140, b_+143); A = mem_rd(gb, wFrameCounter);
  CYC(b_+143, b_+144); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+144, b_+147); TAIL(objectSetInvisible); }
  CYC(b_+144, b_+147);
  CYC(b_+147, b_+150);
  TAIL(objectSetVisible);
}

void s_angleToY48X50_hook(GB *gb) {
  BASE(angleToY48X50);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL((SYM(checkCloseToY48X50) + 29) /* @destination */);
  CYC(b_+6, b_+7); linkInCutscene_d_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); B = mem_rd(gb, HL);
  CYC(b_+8, b_+9); SET_HL(HL + 1);
  CYC(b_+9, b_+10); C = mem_rd(gb, HL);
  CALL_C(b_+10, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+13);
  CYC(b_+13, b_+15); E = 0x09;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+19);
  TAIL(objectApplySpeed);
}

void s_checkCloseToY48X50_hook(GB *gb) {
  BASE(checkCloseToY48X50);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x03;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_BC(b_+29 /* @destination */);
  CALL_C(b_+6, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = 0x0b;
  CYC(b_+12, b_+13); A = mem_rd(gb, BC);
  CYC(b_+13, b_+14); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+14, b_+16); alu_add(gb, 0x01);
  CYC(b_+16, b_+18); alu_cp(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); SET_BC(BC + 1);
  CYC(b_+20, b_+22); L = 0x0d;
  CYC(b_+22, b_+23); A = mem_rd(gb, BC);
  CYC(b_+23, b_+24); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+24, b_+26); alu_add(gb, 0x01);
  CYC(b_+26, b_+28); alu_cp(gb, 0x03);
  RET(b_+28); return;
}

void s_moveToAngleSnapToGrid_hook(GB *gb) {
  BASE(moveToAngleSnapToGrid);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0x09;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+12); SET_HL(SYM(angleToDirectionTable));
  CYC(b_+12, b_+13); linkInCutscene_d_add_a_to_hl(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); E = 0x08;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  RET(b_+17); return;
}

