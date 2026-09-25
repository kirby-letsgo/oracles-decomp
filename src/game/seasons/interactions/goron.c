#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/goron.s.
// INTERAC_GORON

static uint16_t goron_jump_table(GB *gb) {
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

static void goron_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode3b@func_67fc
static void goron_func_67fc(GB *gb) {
  BASE(interactionCode3b);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+254, b_+256); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+256, b_+257); A = mem_rd(gb, DE);
  CYC(b_+257, b_+260); SET_HL(w1Link_xh);
  CYC(b_+260, b_+261); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+261, b_+263); A = 0x02;
  if (F & FC) { RET_TAKEN(b_+263); return; }
  CYC(b_+263, b_+264);
  CYC(b_+264, b_+265); alu_xor(gb, A);
  RET(b_+265); return;
}

// INTERAC_GORON
void s_interactionCode3b_hook(GB *gb) {
  BASE(interactionCode3b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+11); mem_wr(gb, hFF8D, A);
  CYC(b_+11, b_+13); A = 0x41;
  CALL_C(b_+13, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+16);
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto L_6718; }
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); alu_cp(gb, 0x05);
  if (F & FC) { CYCT(b_+20, b_+22); goto L_6718; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); A = 0x01;
  CYC(b_+24, b_+26); mem_wr(gb, hFF8D, A);
L_6718:
  CYC(b_+26, b_+27); H = D;
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); B = A;
  CYC(b_+31, b_+33); alu_and(gb, 0x0f);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+34, b_+35); C = A;
  CYC(b_+35, b_+36); A = B;
  CYC(b_+36, b_+38); A = alu_swap(gb, A);
  CYC(b_+38, b_+40); alu_and(gb, 0x0f);
  CYC(b_+40, b_+41); mem_wr(gb, HL, A);
  CYC(b_+41, b_+42); A = C;
  CYC(b_+42, b_+44); C = 0x37;
  CYC(b_+44, b_+46); alu_cp(gb, 0x07);
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto L_673a; }
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+51); A = mem_rd(gb, wIsLinkedGame);
  CYC(b_+51, b_+52); B = A;
  CYC(b_+52, b_+54); A = mem_rd(gb, hFF8D);
  CYC(b_+54, b_+55); alu_and(gb, B);
  if (F & FZ) { CYCT(b_+55, b_+58); TAIL(interactionDelete); }
  CYC(b_+55, b_+58);
  CYC(b_+58, b_+60); C = 0x53;
L_673a:
  CYC(b_+60, b_+61); A = C;
  CALL_C(b_+61, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+64);
  CALL_C(b_+64, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+67);
  CYC(b_+67, b_+70); SET_HL(b_+266 /* @biggoronColdNotHealed */);
  CYC(b_+70, b_+72); A = mem_rd(gb, hFF8D);
  CYC(b_+72, b_+73); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+73, b_+75); goto L_674c; }
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+78); SET_HL(b_+282 /* @biggoronColdHealed */);
L_674c:
  CYC(b_+78, b_+80); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+80, b_+81); A = mem_rd(gb, DE);
  CYC(b_+81, b_+82); goron_add_double_index(gb, b_+82);
  CYC(b_+82, b_+83); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+83, b_+84); H = mem_rd(gb, HL);
  CYC(b_+84, b_+85); L = A;
  CALL_C(b_+85, s_interactionSetScript, SYM(interactionSetScript), b_+88);
state1:
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+90, b_+91); A = mem_rd(gb, DE);
  CYC(b_+91, b_+92); push_effect(gb, b_+92);
  do { uint16_t jt_ = (goron_jump_table(gb));
    if (jt_ == b_+98) goto var03_00;
    if (jt_ == b_+104) goto var03_01;
    if (jt_ == b_+192) goto var03_02;
    HANDOFF(HL);
  } while (0);
var03_00:
  CALL_C(b_+98, s_interactionRunScript, SYM(interactionRunScript), b_+101);
  CYC(b_+101, b_+104);
  TAIL(npcFaceLinkAndAnimate);
var03_01:
  CALL_C(b_+104, s_interactionRunScript, SYM(interactionRunScript), b_+107);
  CYC(b_+107, b_+109); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+109, b_+110); A = mem_rd(gb, DE);
  CYC(b_+110, b_+111); push_effect(gb, b_+111);
  do { uint16_t jt_ = (goron_jump_table(gb));
    if (jt_ == b_+119) goto substate0;
    if (jt_ == b_+140) goto substate1;
    if (jt_ == b_+156) goto substate2;
    if (jt_ == b_+179) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+119, b_+121); C = 0x28;
  CALL_C(b_+121, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+124);
  if (!(F & FC)) { CYCT(b_+124, b_+126); goto L_6787; }
  CYC(b_+124, b_+126);
  CALL_C(b_+126, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+129);
  CALL_L(b_+129, goron_func_67fc, b_+132);
  CYC(b_+132, b_+134); alu_add(gb, 0x06);
  CALL_C(b_+134, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+137);
L_6787:
  CYC(b_+137, b_+140);
  TAIL(interactionAnimateAsNpc);
substate1:
  CYC(b_+140, b_+142); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+144); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+144, b_+146); goto L_6797; }
  CYC(b_+144, b_+146);
  CALL_C(b_+146, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+149);
  CYC(b_+149, b_+151); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+151, b_+153); mem_wr(gb, HL, 0xff);
L_6797:
  CYC(b_+153, b_+156);
  TAIL(interactionAnimateAsNpc);
substate2:
  CYC(b_+156, b_+158); C = 0x28;
  CALL_C(b_+158, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+161);
  if (F & FC) { CYCT(b_+161, b_+163); goto L_67ae; }
  CYC(b_+161, b_+163);
  CALL_C(b_+163, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+166);
  CALL_L(b_+166, goron_func_67fc, b_+169);
  CYC(b_+169, b_+171); alu_add(gb, 0x07);
  CALL_C(b_+171, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+174);
  CYC(b_+174, b_+176);
  goto animateAsNPC;
L_67ae:
  CYC(b_+176, b_+179);
  TAIL(npcFaceLinkAndAnimate);
substate3:
  CYC(b_+179, b_+181); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+181, b_+182); A = mem_rd(gb, DE);
  CYC(b_+182, b_+183); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+183, b_+185); goto animateAsNPC; }
  CYC(b_+183, b_+185);
  CYC(b_+185, b_+187); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+187, b_+188); alu_xor(gb, A);
  CYC(b_+188, b_+189); mem_wr(gb, DE, A);
animateAsNPC:
  CYC(b_+189, b_+192);
  TAIL(interactionAnimateAsNpc);
var03_02:
  CALL_C(b_+192, s_interactionAnimate, SYM(interactionAnimate), b_+195);
  CALL_C(b_+195, s_interactionAnimate, SYM(interactionAnimate), b_+198);
  CALL_C(b_+198, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+201);
  if (!(F & FZ)) { CYCT(b_+201, b_+203); goto func_67f0; }
  CYC(b_+201, b_+203);
  CYC(b_+203, b_+205); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+205, b_+206); A = mem_rd(gb, DE);
  CYC(b_+206, b_+207); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+207, b_+209); goto runScriptPushLinkAwayUpdateDrawPriority; }
  CYC(b_+207, b_+209);
  CYC(b_+209, b_+210); alu_xor(gb, A);
  CYC(b_+210, b_+211); mem_wr(gb, DE, A);
  CALL_C(b_+211, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+214);
  CYC(b_+214, b_+216); alu_add(gb, 0x04);
  CYC(b_+216, b_+217); alu_add(gb, A);
  CYC(b_+217, b_+219); A = alu_swap(gb, A);
  CYC(b_+219, b_+221); alu_and(gb, 0x03);
  CYC(b_+221, b_+223); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+223, b_+224); mem_wr(gb, DE, A);
  CALL_C(b_+224, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+227);
  CYC(b_+227, b_+230); SET_BC(0x3700);
  CALL_C(b_+230, s_showText, SYM(showText), b_+233);
  CALL_C(b_+233, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+236);
runScriptPushLinkAwayUpdateDrawPriority:
  CALL_C(b_+236, s_interactionRunScript, SYM(interactionRunScript), b_+239);
  CYC(b_+239, b_+242);
  TAIL(interactionPushLinkAwayAndUpdateDrawPriority);
func_67f0:
  CYC(b_+242, b_+244); E = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+244, b_+245); A = mem_rd(gb, DE);
  CALL_C(b_+245, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+248);
  CYC(b_+248, b_+250); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+250, b_+251); alu_xor(gb, A);
  CYC(b_+251, b_+252); mem_wr(gb, DE, A);
  CYC(b_+252, b_+254);
  goto runScriptPushLinkAwayUpdateDrawPriority;
}

