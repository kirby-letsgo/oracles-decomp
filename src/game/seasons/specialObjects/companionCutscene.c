#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/specialObjects/companionCutscene.s.

static uint16_t companionCutscene_jump_table(GB *gb) {
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

static void companionCutscene_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void companionCutscene_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// rickyState1
static void companionCutscene_rickyState1(GB *gb) {
  BASE(incState);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+14, b_+16); E = 0x02;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+22) goto subid0;
    if (jt_ == b_+94) goto subid1;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+22, b_+24); E = 0x05;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); push_effect(gb, b_+26);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+30) goto subid0_substate0;
    if (jt_ == b_+61) goto subid0_substate1;
    HANDOFF(HL);
  } while (0);
subid0_substate0:
  CALL_C(b_+30, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+33);
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = 0x21;
  CYC(b_+36, b_+37); A = mem_rd(gb, HL);
  CYC(b_+37, b_+38); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+38, b_+40); goto L_6a37; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); A = 0x01;
  CYC(b_+42, b_+45); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+45, b_+47); L = 0x05;
  CYC(b_+47, b_+48); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_6a37:
  CYC(b_+48, b_+50); C = 0x20;
  CALL_C(b_+50, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+53);
  if (!(F & FZ)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CYC(b_+54, b_+55); H = D;
  CYC(b_+55, b_+58); SET_BC(0xff20);
  CYC(b_+58, b_+61);
  TAIL(objectSetSpeedZ);
subid0_substate1:
  CALL_C(b_+61, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+64);
  CYC(b_+64, b_+65); H = D;
  CYC(b_+65, b_+67); L = 0x21;
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+69); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+69); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+72); mem_wr(gb, HL, 0x00);
  CYC(b_+72, b_+73); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+73, b_+75); goto L_6a5e; }
  CYC(b_+73, b_+75);
clink:
  CALL_C(b_+75, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+78);
  if (!(F & FZ)) { RET_TAKEN(b_+78); return; }
  CYC(b_+78, b_+79);
  CYC(b_+79, b_+81); mem_wr(gb, HL, 0x07);
  CYC(b_+81, b_+84); SET_BC(0xf812);
  CYC(b_+84, b_+87);
  TAIL(objectCopyPositionWithOffset);
L_6a5e:
  CYC(b_+87, b_+89); L = 0x05;
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x00);
  CYC(b_+91, b_+94);
  TAIL_S(func_69fe);
subid1:
  CYC(b_+94, b_+96); E = 0x05;
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+98); push_effect(gb, b_+98);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+120) goto subid1_substate0;
    if (jt_ == b_+123) goto subid1_substate1;
    if (jt_ == b_+173) goto subid1_substate2;
    if (jt_ == b_+182) goto subid1_substate3;
    if (jt_ == b_+195) goto subid1_substate4;
    if (jt_ == b_+214) goto subid1_substate5;
    if (jt_ == b_+227) goto subid1_substate6;
    if (jt_ == b_+267) goto subid1_substate7;
    if (jt_ == b_+285) goto subid1_substate8;
    if (jt_ == b_+306) goto subid1_substate9;
    if (jt_ == b_+339) goto subid1_substateA;
    HANDOFF(HL);
  } while (0);
subid1_substate0:
  CYC(b_+120, b_+122); L = 0x05;
  CYC(b_+122, b_+123); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
subid1_substate1:
  CALL_C(b_+123, s_objectApplySpeed, SYM(objectApplySpeed), b_+126);
  CYC(b_+126, b_+128); E = 0x0d;
  CYC(b_+128, b_+129); A = mem_rd(gb, DE);
  CYC(b_+129, b_+131); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+131, b_+133); goto L_6aa4; }
  CYC(b_+131, b_+133);
  CYC(b_+133, b_+136); SET_HL(w1Link_xh);
  CYC(b_+136, b_+137); B = mem_rd(gb, HL);
  CYC(b_+137, b_+139); alu_add(gb, 0x18);
  CYC(b_+139, b_+140); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+140, b_+142); goto L_6aa4; }
  CYC(b_+140, b_+142);
  CALL_C(b_+142, s_itemIncSubstate, SYM(itemIncSubstate), b_+145);
  CYC(b_+145, b_+146); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+146, b_+147); L = alu_inc8(gb, L);
  CYC(b_+147, b_+149); mem_wr(gb, HL, 0x3c);
  CYC(b_+149, b_+151); L = 0x0e;
  CYC(b_+151, b_+152); alu_xor(gb, A);
  CYC(b_+152, b_+153); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+153, b_+154); mem_wr(gb, HL, A);
  CYC(b_+154, b_+157);
  TAIL(specialObjectAnimate);
L_6aa4:
  CYC(b_+157, b_+159); C = 0x40;
  CALL_C(b_+159, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+162);
  if (!(F & FZ)) { RET_TAKEN(b_+162); return; }
  CYC(b_+162, b_+163);
  CALL_C(b_+163, s_itemIncSubstate, SYM(itemIncSubstate), b_+166);
  CYC(b_+166, b_+168); L = 0x06;
  CYC(b_+168, b_+170); mem_wr(gb, HL, 0x08);
  CYC(b_+170, b_+173);
  TAIL(specialObjectAnimate);
subid1_substate2:
  CALL_C(b_+173, s_itemDecCounter1, SYM(itemDecCounter1), b_+176);
  if (!(F & FZ)) { RET_TAKEN(b_+176); return; }
  CYC(b_+176, b_+177);
  CYC(b_+177, b_+178); L = alu_dec8(gb, L);
  CYC(b_+178, b_+179); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+179, b_+182);
  TAIL_S(func_69f3);
subid1_substate3:
  CALL_C(b_+182, s_itemDecCounter1, SYM(itemDecCounter1), b_+185);
  if (!(F & FZ)) { RET_TAKEN(b_+185); return; }
  CYC(b_+185, b_+186);
  CYC(b_+186, b_+188); mem_wr(gb, HL, 0x5a);
  CYC(b_+188, b_+189); L = alu_dec8(gb, L);
  CYC(b_+189, b_+190); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+190, b_+192); A = 0x14;
  CYC(b_+192, b_+195);
  TAIL(specialObjectSetAnimation);
subid1_substate4:
  CALL_C(b_+195, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+198);
  CALL_C(b_+198, s_itemDecCounter1, SYM(itemDecCounter1), b_+201);
  if (!(F & FZ)) { RET_TAKEN(b_+201); return; }
  CYC(b_+201, b_+202);
  CYC(b_+202, b_+204); mem_wr(gb, HL, 0x0c);
  CYC(b_+204, b_+205); L = alu_dec8(gb, L);
  CYC(b_+205, b_+206); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+206, b_+208); A = 0x1f;
  CALL_C(b_+208, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+211);
  CYC(b_+211, b_+214);
  goto clink;
subid1_substate5:
  CALL_C(b_+214, s_itemDecCounter1, SYM(itemDecCounter1), b_+217);
  if (!(F & FZ)) { RET_TAKEN(b_+217); return; }
  CYC(b_+217, b_+218);
  CYC(b_+218, b_+220); mem_wr(gb, HL, 0x3c);
  CYC(b_+220, b_+221); L = alu_dec8(gb, L);
  CYC(b_+221, b_+222); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+222, b_+224); A = 0x1e;
  CYC(b_+224, b_+227);
  TAIL(specialObjectSetAnimation);
subid1_substate6:
  CALL_C(b_+227, s_itemDecCounter1, SYM(itemDecCounter1), b_+230);
  if (!(F & FZ)) { RET_TAKEN(b_+230); return; }
  CYC(b_+230, b_+231);
  CYC(b_+231, b_+233); mem_wr(gb, HL, 0x1e);
  CYC(b_+233, b_+234); L = alu_dec8(gb, L);
  CYC(b_+234, b_+235); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+235, b_+238); SET_HL(wActiveRing);
  CYC(b_+238, b_+240); mem_wr(gb, HL, 0xff);
  CYC(b_+240, b_+242); A = 0x81;
  CYC(b_+242, b_+245); mem_wr(gb, wLinkInAir, A);
  CYC(b_+245, b_+248); SET_HL(w1Link_speed);
  CYC(b_+248, b_+250); mem_wr(gb, HL, 0x14);
  CYC(b_+250, b_+252); L = 0x14;
  CYC(b_+252, b_+254); mem_wr(gb, HL, 0x00);
  CYC(b_+254, b_+255); L = alu_inc8(gb, L);
  CYC(b_+255, b_+257); mem_wr(gb, HL, 0xfe);
  CYC(b_+257, b_+259); A = 0x18;
  CYC(b_+259, b_+262); mem_wr(gb, w1Link_angle, A);
  CYC(b_+262, b_+264); A = 0x53;
  CYC(b_+264, b_+267);
  TAIL(playSound_b00);
subid1_substate7:
  CALL_C(b_+267, s_itemDecCounter1, SYM(itemDecCounter1), b_+270);
  if (!(F & FZ)) { RET_TAKEN(b_+270); return; }
  CYC(b_+270, b_+271);
  CYC(b_+271, b_+273); mem_wr(gb, HL, 0x14);
  CYC(b_+273, b_+274); L = alu_dec8(gb, L);
  CYC(b_+274, b_+275); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+275, b_+276); alu_xor(gb, A);
  CYC(b_+276, b_+279); SET_HL(w1Link_visible);
  CYC(b_+279, b_+280); mem_wr(gb, HL, A);
  CYC(b_+280, b_+281); A = alu_inc8(gb, A);
  CYC(b_+281, b_+284); mem_wr(gb, wDisabledObjects, A);
  RET(b_+284); return;
subid1_substate8:
  CALL_C(b_+285, s_itemDecCounter1, SYM(itemDecCounter1), b_+288);
  if (!(F & FZ)) { RET_TAKEN(b_+288); return; }
  CYC(b_+288, b_+289);
  CYC(b_+289, b_+290); L = alu_dec8(gb, L);
  CYC(b_+290, b_+291); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+291, b_+293); L = 0x09;
  CYC(b_+293, b_+295); mem_wr(gb, HL, 0x18);
subid1_func_6b2e:
  CYC(b_+295, b_+297); A = 0x1c;
  CALL_C(b_+297, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+300);
  CYC(b_+300, b_+303); SET_BC(0xfe00);
  CYC(b_+303, b_+306);
  TAIL(objectSetSpeedZ);
subid1_substate9:
  CALL_C(b_+306, s_objectApplySpeed, SYM(objectApplySpeed), b_+309);
  CYC(b_+309, b_+311); E = 0x0d;
  CYC(b_+311, b_+312); A = mem_rd(gb, DE);
  CYC(b_+312, b_+314); alu_sub(gb, 0x10);
  CYC(b_+314, b_+315); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+315, b_+317); goto L_6b4a; }
  CYC(b_+315, b_+317);
  CYC(b_+317, b_+320); SET_HL(wTmpcfc0 + 0x1f);
  CYC(b_+320, b_+322); mem_wr(gb, HL, 0x01);
  RET(b_+322); return;
L_6b4a:
  CYC(b_+323, b_+325); C = 0x40;
  CALL_C(b_+325, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+328);
  if (!(F & FZ)) { RET_TAKEN(b_+328); return; }
  CYC(b_+328, b_+329);
  CALL_C(b_+329, s_itemIncSubstate, SYM(itemIncSubstate), b_+332);
  CYC(b_+332, b_+334); L = 0x06;
  CYC(b_+334, b_+336); mem_wr(gb, HL, 0x08);
  CYC(b_+336, b_+339);
  TAIL(specialObjectAnimate);
subid1_substateA:
  CALL_C(b_+339, s_itemDecCounter1, SYM(itemDecCounter1), b_+342);
  if (!(F & FZ)) { RET_TAKEN(b_+342); return; }
  CYC(b_+342, b_+343);
  CYC(b_+343, b_+345); L = 0x05;
  CYC(b_+345, b_+346); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+346, b_+349);
  goto subid1_func_6b2e;
}

void s_rickyCutscenes_hook(GB *gb) {
  BASE(rickyCutscenes);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+9) goto state0;
    if (jt_ == (b_ + 61)) { companionCutscene_rickyState1(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+9, s_incState_hook, SYM(incState), b_+12);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = 0x02;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+17, b_+19); TAIL_S(func_69fe); }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); L = 0x10;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x50);
  CYC(b_+23, b_+25); L = 0x09;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x08);
  s_func_69f3_hook(gb); return; // falls through
}

void s_func_69f3_hook(GB *gb) {
  BASE(func_69f3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(0xfe00);
  CALL_C(b_+3, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+6);
  CYC(b_+6, b_+8); A = 0x02;
  CYC(b_+8, b_+11);
  TAIL(specialObjectSetAnimation);
}

void s_func_69fe_hook(GB *gb) {
  BASE(func_69fe);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wTmpcbb5, A);
  CYC(b_+4, b_+6); A = 0x1e;
  CYC(b_+6, b_+9);
  TAIL(specialObjectSetAnimation);
}

void s_incState_hook(GB *gb) {
  BASE(incState);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, b_+6); SET_HL(SYM(specialObjectSetOamVariables));
  CYC(b_+6, b_+8); E = 0x05;
  CALL_C(b_+8, s_interBankCall, SYM(interBankCall), b_+11);
  CYC(b_+11, b_+14);
  TAIL(objectSetVisiblec0);
}

void s_mooshCutscenes_hook(GB *gb) {
  BASE(mooshCutscenes);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+50) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_incState_hook, SYM(incState), b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x06;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x5a);
  CYC(b_+16, b_+18); L = 0x10;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x37);
  CYC(b_+20, b_+22); L = 0x36;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x05);
  CYC(b_+24, b_+26); L = 0x09;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x10);
  CYC(b_+28, b_+30); L = 0x0e;
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0xff);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0xe0);
  CALL_C(b_+35, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+38);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto L_6b91; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0xc0);
  CYC(b_+42, b_+44); L = 0x57;
  CYC(b_+44, b_+45); mem_wr(gb, HL, D);
L_6b91:
  CYC(b_+45, b_+47); A = 0x07;
  CYC(b_+47, b_+50);
  TAIL(specialObjectSetAnimation);
state1:
  CYC(b_+50, b_+52); E = 0x05;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+54, b_+56); goto L_6ba2; }
  CYC(b_+54, b_+56);
  CALL_C(b_+56, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+59);
  CALL_C(b_+59, s_objectApplySpeed, SYM(objectApplySpeed), b_+62);
L_6ba2:
  CYC(b_+62, b_+64); E = 0x05;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  CYC(b_+65, b_+66); push_effect(gb, b_+66);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+76) goto state1_substate0;
    if (jt_ == b_+86) goto state1_substate1;
    if (jt_ == b_+98) goto state1_substate2;
    if (jt_ == b_+122) goto state1_substate3;
    if (jt_ == b_+138) goto state1_substate4;
    HANDOFF(HL);
  } while (0);
state1_substate0:
  CALL_C(b_+76, s_itemDecCounter1, SYM(itemDecCounter1), b_+79);
  if (!(F & FZ)) { RET_TAKEN(b_+79); return; }
  CYC(b_+79, b_+80);
  CYC(b_+80, b_+82); mem_wr(gb, HL, 0x48);
  CYC(b_+82, b_+84); L = 0x05;
  CYC(b_+84, b_+85); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+85); return;
state1_substate1:
  CALL_C(b_+86, s_itemDecCounter1, SYM(itemDecCounter1), b_+89);
  if (!(F & FZ)) { RET_TAKEN(b_+89); return; }
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x06);
  CYC(b_+92, b_+94); L = 0x05;
  CYC(b_+94, b_+95); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+95, b_+98);
  TAIL_S(seasonsFunc_06_6d89);
state1_substate2:
  CYC(b_+98, b_+99); H = D;
  CYC(b_+99, b_+101); L = 0x09;
  CYC(b_+101, b_+102); A = mem_rd(gb, HL);
  CYC(b_+102, b_+104); alu_cp(gb, 0x10);
  if (F & FZ) { CYCT(b_+104, b_+106); goto state1_func_6bd2; }
  CYC(b_+104, b_+106);
  CYC(b_+106, b_+108); L = 0x05;
  CYC(b_+108, b_+109); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+109); return;
state1_func_6bd2:
  CYC(b_+110, b_+112); L = 0x06;
  CYC(b_+112, b_+113); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+113); return; }
  CYC(b_+113, b_+114);
  CALL_C(b_+114, s_seasonsFunc_06_6da0_hook, SYM(seasonsFunc_06_6da0), b_+117);
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x06);
  CYC(b_+119, b_+122);
  TAIL_S(seasonsFunc_06_6d89);
state1_substate3:
  CYC(b_+122, b_+123); H = D;
  CYC(b_+123, b_+125); L = 0x09;
  CYC(b_+125, b_+126); A = mem_rd(gb, HL);
  CYC(b_+126, b_+128); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(b_+128, b_+130); goto state1_func_6bd2; }
  CYC(b_+128, b_+130);
  CYC(b_+130, b_+132); L = 0x05;
  CYC(b_+132, b_+133); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+133, b_+135); A = 0x07;
  CYC(b_+135, b_+138);
  TAIL(specialObjectSetAnimation);
state1_substate4:
  CYC(b_+138, b_+140); E = 0x0b;
  CYC(b_+140, b_+141); A = mem_rd(gb, DE);
  CYC(b_+141, b_+143); alu_cp(gb, 0xb0);
  if (F & FC) { RET_TAKEN(b_+143); return; }
  CYC(b_+143, b_+144);
  CYC(b_+144, b_+147); SET_HL(w1Companion_id);
  CYC(b_+147, b_+149); B = 0x3f;
  CALL_C(b_+149, s_clearMemory, SYM(clearMemory), b_+152);
  CYC(b_+152, b_+155); SET_HL(w1Companion_id);
  CYC(b_+155, b_+157); mem_wr(gb, HL, 0x10);
  CYC(b_+157, b_+159); L = 0x0b;
  CYC(b_+159, b_+161); mem_wr(gb, HL, 0xe8);
  CYC(b_+161, b_+162); L = alu_inc8(gb, L);
  CYC(b_+162, b_+163); L = alu_inc8(gb, L);
  CYC(b_+163, b_+165); mem_wr(gb, HL, 0x28);
  RET(b_+165); return;
}

void s_dimitriCutscenes_hook(GB *gb) {
  BASE(dimitriCutscenes);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+28) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_incState_hook, SYM(incState), b_+11);
  CYC(b_+11, b_+12); H = D;
  CYC(b_+12, b_+14); L = 0x10;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x28);
  CYC(b_+16, b_+18); L = 0x0e;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0xe0);
  CYC(b_+20, b_+21); L = alu_inc8(gb, L);
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xff);
  CYC(b_+23, b_+25); A = 0x19;
  CYC(b_+25, b_+28);
  TAIL(specialObjectSetAnimation);
state1:
  CYC(b_+28, b_+30); E = 0x05;
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  CYC(b_+31, b_+32); push_effect(gb, b_+32);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+46) goto state1_substate0;
    if (jt_ == b_+90) goto state1_substate1;
    if (jt_ == b_+125) goto state1_substate2;
    if (jt_ == b_+141) goto state1_substate3;
    if (jt_ == b_+153) goto state1_substate4;
    if (jt_ == b_+176) goto state1_substate5;
    if (jt_ == b_+188) goto state1_substate6;
    HANDOFF(HL);
  } while (0);
state1_substate0:
  CYC(b_+46, b_+47); H = D;
  CYC(b_+47, b_+49); L = 0x05;
  CYC(b_+49, b_+50); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+50, b_+52); L = 0x07;
  CYC(b_+52, b_+53); A = mem_rd(gb, HL);
  CYC(b_+53, b_+55); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+55, b_+57); goto L_6c4a; }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+58); push_effect(gb, AF);
  CYC(b_+58, b_+60); A = 0x1a;
  CALL_C(b_+60, s_specialObjectSetAnimation, SYM(specialObjectSetAnimation), b_+63);
  CYC(b_+63, b_+64); SET_AF(pop_effect(gb));
L_6c4a:
  CYC(b_+64, b_+65); B = A;
  CYC(b_+65, b_+66); alu_add(gb, A);
  CYC(b_+66, b_+67); alu_add(gb, B);
  CYC(b_+67, b_+70); SET_HL(b_+81 /* @state1@table_6c5b */);
  CYC(b_+70, b_+71); companionCutscene_add_a_to_hl(gb, b_+71);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+72, b_+74); E = 0x09;
  CYC(b_+74, b_+75); mem_wr(gb, DE, A);
  CYC(b_+75, b_+76); C = mem_rd(gb, HL);
  CYC(b_+76, b_+77); SET_HL(HL + 1);
  CYC(b_+77, b_+78); B = mem_rd(gb, HL);
  CYC(b_+78, b_+81);
  TAIL(objectSetSpeedZ);

state1_substate1:
  CALL_C(b_+90, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+93);
  CALL_C(b_+93, s_objectApplySpeed, SYM(objectApplySpeed), b_+96);
  CYC(b_+96, b_+98); C = 0x18;
  CALL_C(b_+98, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+101);
  if (!(F & FZ)) { RET_TAKEN(b_+101); return; }
  CYC(b_+101, b_+102);
  CYC(b_+102, b_+103); H = D;
  CYC(b_+103, b_+105); L = 0x07;
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+106, b_+107); A = mem_rd(gb, HL);
  CYC(b_+107, b_+109); L = 0x05;
  CYC(b_+109, b_+111); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+111, b_+113); goto L_6c81; }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+114); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+114, b_+116); L = 0x06;
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x08);
  RET(b_+118); return;
L_6c81:
  CYC(b_+119, b_+120); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+120, b_+122); L = 0x06;
  CYC(b_+122, b_+124); mem_wr(gb, HL, 0x06);
  RET(b_+124); return;
state1_substate2:
  CALL_C(b_+125, s_itemDecCounter1, SYM(itemDecCounter1), b_+128);
  if (!(F & FZ)) { RET_TAKEN(b_+128); return; }
  CYC(b_+128, b_+129);
  CYC(b_+129, b_+131); L = 0x05;
  CYC(b_+131, b_+132); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+132, b_+134); L = 0x06;
  CYC(b_+134, b_+136); mem_wr(gb, HL, 0x14);
  CYC(b_+136, b_+138); A = 0x27;
  CYC(b_+138, b_+141);
  TAIL(specialObjectSetAnimation);
state1_substate3:
  CALL_C(b_+141, s_itemDecCounter1, SYM(itemDecCounter1), b_+144);
  if (!(F & FZ)) { RET_TAKEN(b_+144); return; }
  CYC(b_+144, b_+145);
  CYC(b_+145, b_+147); L = 0x05;
  CYC(b_+147, b_+148); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+148, b_+150); L = 0x06;
  CYC(b_+150, b_+152); mem_wr(gb, HL, 0x78);
  RET(b_+152); return;
state1_substate4:
  CALL_C(b_+153, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+156);
  CALL_C(b_+156, s_itemDecCounter1, SYM(itemDecCounter1), b_+159);
  if (!(F & FZ)) { RET_TAKEN(b_+159); return; }
  CYC(b_+159, b_+160);
  CYC(b_+160, b_+162); L = 0x05;
  CYC(b_+162, b_+163); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+163, b_+165); L = 0x06;
  CYC(b_+165, b_+167); mem_wr(gb, HL, 0x3c);
  CYC(b_+167, b_+169); L = 0x09;
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x0b);
  CYC(b_+171, b_+173); L = 0x10;
  CYC(b_+173, b_+175); mem_wr(gb, HL, 0x14);
  RET(b_+175); return;
state1_substate5:
  CALL_C(b_+176, s_itemDecCounter1, SYM(itemDecCounter1), b_+179);
  if (!(F & FZ)) { RET_TAKEN(b_+179); return; }
  CYC(b_+179, b_+180);
  CYC(b_+180, b_+182); L = 0x05;
  CYC(b_+182, b_+183); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+183, b_+185); A = 0x26;
  CYC(b_+185, b_+188);
  TAIL(specialObjectSetAnimation);
state1_substate6:
  CALL_C(b_+188, s_specialObjectAnimate, SYM(specialObjectAnimate), b_+191);
  CALL_C(b_+191, s_objectApplySpeed, SYM(objectApplySpeed), b_+194);
  CYC(b_+194, b_+196); E = 0x0d;
  CYC(b_+196, b_+197); A = mem_rd(gb, DE);
  CYC(b_+197, b_+199); alu_cp(gb, 0x78);
  if (!(F & FZ)) { CYCT(b_+199, b_+201); goto L_6cd8; }
  CYC(b_+199, b_+201);
  CYC(b_+201, b_+203); A = 0x05;
  CYC(b_+203, b_+206);
  TAIL(specialObjectSetAnimation);
L_6cd8:
  CYC(b_+206, b_+208); alu_cp(gb, 0xb0);
  if (F & FC) { RET_TAKEN(b_+208); return; }
  CYC(b_+208, b_+209);
  CYC(b_+209, b_+212); SET_HL(w1Companion_id);
  CYC(b_+212, b_+214); B = 0x3f;
  CALL_C(b_+214, s_clearMemory, SYM(clearMemory), b_+217);
  CYC(b_+217, b_+220); SET_HL(w1Companion_id);
  CYC(b_+220, b_+222); mem_wr(gb, HL, 0x0f);
  CYC(b_+222, b_+223); L = alu_inc8(gb, L);
  CYC(b_+223, b_+225); mem_wr(gb, HL, 0x01);
  CYC(b_+225, b_+227); L = 0x0b;
  CYC(b_+227, b_+229); mem_wr(gb, HL, 0x48);
  CYC(b_+229, b_+230); L = alu_inc8(gb, L);
  CYC(b_+230, b_+231); L = alu_inc8(gb, L);
  CYC(b_+231, b_+233); mem_wr(gb, HL, 0xd8);
  RET(b_+233); return;
}

void s_mapleCutscenes_hook(GB *gb) {
  BASE(mapleCutscenes);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); push_effect(gb, b_+5);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+9) goto state0;
    if (jt_ == b_+48) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+9, s_incState_hook, SYM(incState), b_+12);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = 0x10;
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x32);
  CYC(b_+17, b_+19); L = 0x36;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x04);
  CYC(b_+21, b_+23); L = 0x02;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); alu_or(gb, A);
  CYC(b_+25, b_+27); A = 0xf0;
  if (F & FZ) { CYCT(b_+27, b_+29); goto L_6d17; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); A = D;
  CYC(b_+30, b_+33); mem_wr(gb, wLinkObjectIndex, A);
  CYC(b_+33, b_+35); A = 0xd0;
L_6d17:
  CYC(b_+35, b_+37); L = 0x0f;
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);
  CYC(b_+38, b_+40); L = 0x09;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x18);
  CYC(b_+42, b_+44); L = 0x02;
  CYC(b_+44, b_+45); A = mem_rd(gb, HL);
  CYC(b_+45, b_+48);
  TAIL_S(seasonsFunc_06_6d78);
state1:
  CYC(b_+48, b_+50); E = 0x02;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); push_effect(gb, b_+52);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+94) goto state1_subid0;
    if (jt_ == b_+56) goto state1_subid1;
    HANDOFF(HL);
  } while (0);
state1_subid1:
  CYC(b_+56, b_+58); E = 0x05;
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); push_effect(gb, b_+60);
  do { uint16_t jt_ = (companionCutscene_jump_table(gb));
    if (jt_ == b_+66) goto state1_subid1_substate0;
    if (jt_ == SYM(seasonsFunc_06_6d62) && hook_is(gb, SYM(seasonsFunc_06_6d62), s_seasonsFunc_06_6d62_hook)) { s_seasonsFunc_06_6d62_hook(gb); return; }
    if (jt_ == SYM(ret) && hook_is(gb, SYM(ret), s_ret_hook)) { s_ret_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state1_subid1_substate0:
  CYC(b_+66, b_+69); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+69, b_+70); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+70, b_+72); goto state1_subid0; }
  CYC(b_+70, b_+72);
  CALL_C(b_+72, s_itemIncSubstate, SYM(itemIncSubstate), b_+75);
  CYC(b_+75, b_+78); SET_BC(IO_P1);
  CALL_C(b_+78, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+81);
  CYC(b_+81, b_+83); L = 0x09;
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x0e);
  CYC(b_+85, b_+87); L = 0x10;
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x14);
  CYC(b_+89, b_+91); A = 0x1b;
  CYC(b_+91, b_+94);
  TAIL(specialObjectSetAnimation);
state1_subid0:
  CYC(b_+94, b_+95); H = D;
  CYC(b_+95, b_+97); L = 0x02;
  CYC(b_+97, b_+98); A = mem_rd(gb, HL);
  CYC(b_+98, b_+100); L = 0x06;
  CYC(b_+100, b_+101); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) CALL_C_CC(b_+101, s_seasonsFunc_06_6d78_hook, SYM(seasonsFunc_06_6d78), b_+104);
  else CYC(b_+101, b_+104);
  CALL_C(b_+104, s_objectApplySpeed, SYM(objectApplySpeed), b_+107);
  CYC(b_+107, b_+110);
  TAIL(specialObjectAnimate);
}

void s_seasonsFunc_06_6d62_hook(GB *gb) {
  BASE(seasonsFunc_06_6d62);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectApplySpeed, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+5); C = 0x20;
  CALL_C(b_+5, s_objectUpdateSpeedZAndBounce, SYM(objectUpdateSpeedZAndBounce), b_+8);
  if (!(F & FC)) { CYCT(b_+8, b_+11); TAIL_S(seasonsFunc_06_6d74); }
  CYC(b_+8, b_+11);
  CALL_C(b_+11, s_itemIncSubstate, SYM(itemIncSubstate), b_+14);
  CYC(b_+14, b_+16); L = 0x20;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x01);
  s_seasonsFunc_06_6d74_hook(gb); return; // falls through
}

void s_seasonsFunc_06_6d74_hook(GB *gb) {
  BASE(seasonsFunc_06_6d74);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(specialObjectAnimate);
}

void s_ret_hook(GB *gb) {
  BASE(ret);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  RET(b_+0); return;
}

void s_seasonsFunc_06_6d78_hook(GB *gb) {
  BASE(seasonsFunc_06_6d78);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(seasonsTable_06_6da8));
  CYC(b_+3, b_+4); companionCutscene_add_double_index(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); H = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = A;
  CALL_C(b_+7, s_seasonsFunc_06_6da0_hook, SYM(seasonsFunc_06_6da0), b_+10);
  CYC(b_+10, b_+11); B = A;
  CYC(b_+11, b_+12); companionCutscene_add_a_to_hl(gb, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); E = 0x06;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); A = B;
  s_seasonsFunc_06_6d89_hook(gb); return; // falls through
}

void s_seasonsFunc_06_6d89_hook(GB *gb) {
  BASE(seasonsFunc_06_6d89);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); alu_sub(gb, 0x04);
  CYC(b_+2, b_+4); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = 0x09;
  CALL_C(b_+7, s_convertAngleDeToDirection, SYM(convertAngleDeToDirection), b_+10);
  CYC(b_+10, b_+11); A = alu_dec8(gb, A);
  CYC(b_+11, b_+13); alu_and(gb, 0x03);
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+16); L = 0x08;
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); L = 0x36;
  CYC(b_+19, b_+20); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+20, b_+23);
  TAIL(specialObjectSetAnimation);
}

void s_seasonsFunc_06_6da0_hook(GB *gb) {
  BASE(seasonsFunc_06_6da0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x09;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_dec8(gb, A);
  CYC(b_+4, b_+6); alu_and(gb, 0x1f);
  CYC(b_+6, b_+7); mem_wr(gb, DE, A);
  RET(b_+7); return;
}

