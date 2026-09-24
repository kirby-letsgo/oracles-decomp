#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/subrosiansHiding.s.
// INTERAC_ROSA_HIDING

static uint16_t subrosiansHiding_jump_table(GB *gb) {
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

static void subrosiansHiding_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_ROSA_HIDING
void s_interactionCode6c_hook(GB *gb) {
  BASE(interactionCode6c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosiansHiding_jump_table(gb));
    if (jt_ == SYM(rosaSubId0) && hook_is(gb, SYM(rosaSubId0), s_rosaSubId0_hook)) { s_rosaSubId0_hook(gb); return; }
    if (jt_ == SYM(rosaSubId1) && hook_is(gb, SYM(rosaSubId1), s_rosaSubId1_hook)) { s_rosaSubId1_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

// INTERAC_STRANGE_BROTHERS_HIDING
void s_interactionCode6d_hook(GB *gb) {
  BASE(interactionCode6d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosiansHiding_jump_table(gb));
    if (jt_ == SYM(strangeBrothersSubId0) && hook_is(gb, SYM(strangeBrothersSubId0), s_strangeBrothersSubId0_hook)) { s_strangeBrothersSubId0_hook(gb); return; }
    if (jt_ == SYM(strangeBrothersSubId1) && hook_is(gb, SYM(strangeBrothersSubId1), s_strangeBrothersSubId1_hook)) { s_strangeBrothersSubId1_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_rosaSubId0_hook(GB *gb) {
  BASE(rosaSubId0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosiansHiding_jump_table(gb));
    if (jt_ == b_+8) goto substate0;
    if (jt_ == b_+127) goto substate1;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x40;
  CALL_C(b_+13, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+16);
  CYC(b_+16, b_+18); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+18, b_+21); TAIL(interactionDelete); }
  CYC(b_+18, b_+21);
  CALL_C(b_+21, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+24);
  CYC(b_+24, b_+26); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+26, b_+29); TAIL(interactionDelete); }
  CYC(b_+26, b_+29);
  CALL_C(b_+29, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+32);
  CALL_C(b_+32, s_objectSetReservedBit1, SYM(objectSetReservedBit1), b_+35);
  CYC(b_+35, b_+37); A = 0x01;
  CYC(b_+37, b_+40); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+40, b_+43); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+45, b_+48); A = mem_rd(gb, wActiveRoom);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); alu_xor(gb, A);
  CYC(b_+50, b_+53); mem_wr(gb, wActiveMusic, A);
  CYC(b_+53, b_+55); A = 0x0b;
  CALL_C(b_+55, s_playSound, SYM(playSound_b00), b_+58);
func_67d1:
  CYC(b_+58, b_+60); A = 0x80;
  CYC(b_+60, b_+63); mem_wr(gb, wcc85, A);
  CYC(b_+63, b_+65); A = 0x01;
  CYC(b_+65, b_+68); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+68, b_+71); SET_BC(0x016c);
  CALL_C(b_+71, s_spawnHider_hook, SYM(spawnHider), b_+74);
  CYC(b_+74, b_+75); E = A;
  CYC(b_+75, b_+78); SET_BC(b_+90 /* @table_67f1 */);
  CALL_C(b_+78, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+81);
  CALL_C(b_+81, s_func_69ac_hook, SYM(func_69ac), b_+84);
  CYC(b_+84, b_+85); A = E;
  CYC(b_+85, b_+87); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+87, b_+89); goto func_680c; }
  CYC(b_+87, b_+89);
  RET(b_+89); return;

func_67fb:
  CYC(b_+100, b_+103); A = mem_rd(gb, wActiveRoom);
  CYC(b_+103, b_+105); alu_cp(gb, 0xcb);
  if (F & FZ) { CYCT(b_+105, b_+108); TAIL(interactionDelete); }
  CYC(b_+105, b_+108);
  CYC(b_+108, b_+111); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+111, b_+114); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+114, s_playSound, SYM(playSound_b00), b_+117);
func_680c:
  CYC(b_+117, b_+118); alu_xor(gb, A);
  CYC(b_+118, b_+121); mem_wr(gb, wcc85, A);
  CYC(b_+121, b_+124); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+124, b_+127);
  TAIL(interactionDelete);
substate1:
  CYC(b_+127, b_+129); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+131); push_effect(gb, b_+131);
  do { uint16_t jt_ = (subrosiansHiding_jump_table(gb));
    if (jt_ == b_+135) goto var38_00;
    if (jt_ == b_+154) goto var38_01;
    HANDOFF(HL);
  } while (0);
var38_00:
  CYC(b_+135, b_+138); A = mem_rd(gb, wActiveRoom);
  CYC(b_+138, b_+140); alu_cp(gb, 0xcb);
  if (!(F & FZ)) { CYCT(b_+140, b_+142); goto func_67fb; }
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+145); A = mem_rd(gb, wcc84);
  CYC(b_+145, b_+147); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+147); return; }
  CYC(b_+147, b_+148);
  CYC(b_+148, b_+150); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+150, b_+152); A = 0x01;
  CYC(b_+152, b_+153); mem_wr(gb, DE, A);
  RET(b_+153); return;
var38_01:
  CYC(b_+154, b_+157); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+157, b_+158); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+158, b_+160); goto L_6840; }
  CYC(b_+158, b_+160);
  CYC(b_+160, b_+162); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+162, b_+164); A = 0x01;
  CYC(b_+164, b_+165); mem_wr(gb, DE, A);
  CYC(b_+165, b_+166); alu_xor(gb, A);
  CYC(b_+166, b_+169); mem_wr(gb, wDisableScreenTransitions, A);
L_6840:
  CYC(b_+169, b_+171); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+171, b_+172); A = mem_rd(gb, DE);
  CYC(b_+172, b_+173); B = A;
  CYC(b_+173, b_+176); A = mem_rd(gb, wActiveRoom);
  CYC(b_+176, b_+177); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+177); return; }
  CYC(b_+177, b_+178);
  CYC(b_+178, b_+179); mem_wr(gb, DE, A);
  CYC(b_+179, b_+181); alu_cp(gb, 0xcb);
  if (F & FZ) { CYCT(b_+181, b_+183); goto func_67fb; }
  CYC(b_+181, b_+183);
  CYC(b_+183, b_+185); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+185, b_+186); A = mem_rd(gb, DE);
  CYC(b_+186, b_+187); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+187, b_+189); goto func_67fb; }
  CYC(b_+187, b_+189);
  CYC(b_+189, b_+190); alu_xor(gb, A);
  CYC(b_+190, b_+191); mem_wr(gb, DE, A);
  CYC(b_+191, b_+192); H = D;
  CYC(b_+192, b_+194); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+194, b_+195); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+195, b_+196); A = mem_rd(gb, HL);
  CYC(b_+196, b_+199); SET_BC(b_+213 /* @table_686c */);
  CALL_C(b_+199, s_addAToBc, SYM(addAToBc), b_+202);
  CYC(b_+202, b_+203); A = mem_rd(gb, BC);
  CYC(b_+203, b_+204); B = A;
  CYC(b_+204, b_+207); A = mem_rd(gb, wActiveRoom);
  CYC(b_+207, b_+208); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+208, b_+210); goto func_67fb; }
  CYC(b_+208, b_+210);
  CYC(b_+210, b_+213);
  goto func_67d1;
}

// rosaSubId1@func_68a2
static void subrosiansHiding_func_68a2(GB *gb) {
  BASE(rosaSubId1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+49, b_+51); C = 0x20;
  CYC(b_+51, b_+54);
  TAIL(objectUpdateSpeedZ_paramC);
}

void s_rosaSubId1_hook(GB *gb) {
  BASE(rosaSubId1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosiansHiding_jump_table(gb));
    if (jt_ == b_+16) goto substate0;
    if (jt_ == b_+43) goto substate1;
    if (jt_ == b_+54) goto substate2;
    if (jt_ == b_+72) goto substate3;
    if (jt_ == b_+86) goto substate4;
    if (jt_ == b_+103) goto substate5;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+16, b_+18); A = 0x01;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CALL_C(b_+19, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+28); SET_HL(SYM(table_6931));
  CYC(b_+28, b_+29); subrosiansHiding_add_double_index(gb, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); H = mem_rd(gb, HL);
  CYC(b_+31, b_+32); L = A;
  CALL_C(b_+32, s_interactionSetScript, SYM(interactionSetScript), b_+35);
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+37, b_+39); A = 0x08;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CALL_C(b_+40, s_objectSetVisiblec2, SYM(objectSetVisiblec2), b_+43);
substate1:
  CALL_C(b_+43, s_interactionRunScript, SYM(interactionRunScript), b_+46);
  if (F & FC) { CYCT(b_+46, b_+49); TAIL(interactionDelete); }
  CYC(b_+46, b_+49);
  CYC(b_+49, b_+51); C = 0x20;
  CYC(b_+51, b_+54);
  TAIL(objectUpdateSpeedZ_paramC);
substate2:
  CALL_C(b_+54, s_interactionRunScript, SYM(interactionRunScript), b_+57);
  if (F & FC) { CYCT(b_+57, b_+60); TAIL(interactionDelete); }
  CYC(b_+57, b_+60);
  CYC(b_+60, b_+62); C = 0x20;
  CALL_C(b_+62, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+65);
  if (!(F & FZ)) { RET_TAKEN(b_+65); return; }
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+69); SET_BC(0xfe40);
  CYC(b_+69, b_+72);
  TAIL(objectSetSpeedZ);
substate3:
  CALL_C(b_+72, s_interactionAnimate, SYM(interactionAnimate), b_+75);
  CALL_L(b_+75, subrosiansHiding_func_68a2, b_+78);
  if (!(F & FZ)) { RET_TAKEN(b_+78); return; }
  CYC(b_+78, b_+79);
  CALL_C(b_+79, s_interactionRunScript, SYM(interactionRunScript), b_+82);
  if (F & FC) { CYCT(b_+82, b_+85); TAIL(interactionDelete); }
  CYC(b_+82, b_+85);
  RET(b_+85); return;
substate4:
  CYC(b_+86, b_+88); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+90, b_+92); goto L_68cf; }
  CYC(b_+90, b_+92);
  CYC(b_+92, b_+94);
  goto substate3;
L_68cf:
  CYC(b_+94, b_+97); SET_HL(SYM(rosaHidingScript_caught));
  CALL_C(b_+97, s_interactionSetScript, SYM(interactionSetScript), b_+100);
  CYC(b_+100, b_+103);
  TAIL(interactionRunScript);
substate5:
  CALL_C(b_+103, s_interactionAnimate, SYM(interactionAnimate), b_+106);
  CALL_L(b_+106, subrosiansHiding_func_68a2, b_+109);
  if (!(F & FZ)) { RET_TAKEN(b_+109); return; }
  CYC(b_+109, b_+110);
  CYC(b_+110, b_+112); A = 0x09;
  CALL_C(b_+112, s_objectGetShortPosition_withYOffset, SYM(objectGetShortPosition_withYOffset), b_+115);
  CYC(b_+115, b_+116); C = A;
  CYC(b_+116, b_+118); B = 0xce;
  CYC(b_+118, b_+119); A = mem_rd(gb, BC);
  CYC(b_+119, b_+121); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+121, b_+123); goto L_68ef; }
  CYC(b_+121, b_+123);
  CYC(b_+123, b_+124); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+124, b_+126); goto L_68f3; }
  CYC(b_+124, b_+126);
L_68ef:
  CYC(b_+126, b_+128); A = 0x10;
  CYC(b_+128, b_+130);
  TAIL_S(func_6919);
L_68f3:
  CYC(b_+130, b_+132); E = INTERACTION_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+132, b_+133); A = mem_rd(gb, DE);
  CYC(b_+133, b_+135); mem_wr(gb, hFF8B, A);
  CYC(b_+135, b_+137); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CALL_C(b_+138, s_convertAngleDeToDirection, SYM(convertAngleDeToDirection), b_+141);
  CYC(b_+141, b_+143); alu_xor(gb, 0x02);
  CYC(b_+143, b_+145); alu_sub(gb, 0x02);
  CYC(b_+145, b_+146); alu_add(gb, C);
  CYC(b_+146, b_+147); C = A;
  CYC(b_+147, b_+148); A = mem_rd(gb, BC);
  CYC(b_+148, b_+149); alu_or(gb, A);
  CYC(b_+149, b_+151); A = mem_rd(gb, hFF8B);
  if (F & FZ) { CYCT(b_+151, b_+153); TAIL_S(func_6919); }
  CYC(b_+151, b_+153);
  CYC(b_+153, b_+155); E = INTERACTION_BASE + OBJ_KNOCKBACK_COUNTER;
  CYC(b_+155, b_+156); A = mem_rd(gb, DE);
  CYC(b_+156, b_+158); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+158, b_+160); goto L_6916; }
  CYC(b_+158, b_+160);
  CYC(b_+160, b_+162); A = 0x08;
  CYC(b_+162, b_+163); mem_wr(gb, DE, A);
  CYC(b_+163, b_+165);
  TAIL_S(func_6919);
L_6916:
  CYC(b_+165, b_+167); A = 0x18;
  CYC(b_+167, b_+168); mem_wr(gb, DE, A);
  s_func_6919_hook(gb); return; // falls through
}

void s_func_6919_hook(GB *gb) {
  BASE(func_6919);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CALL_C(b_+3, s_objectApplySpeed, SYM(objectApplySpeed), b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_cp(gb, 0x90);
  if (!(F & FC)) { CYCT(b_+11, b_+13); goto L_6927; }
  CYC(b_+11, b_+13);
  RET(b_+13); return;
L_6927:
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CYC(b_+15, b_+18); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+18, b_+21); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+21, b_+24);
  TAIL(interactionDelete);
}

void s_strangeBrothersSubId0_hook(GB *gb) {
  BASE(strangeBrothersSubId0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosiansHiding_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == SYM(strangeBrothersSubId0State1) && hook_is(gb, SYM(strangeBrothersSubId0State1), s_strangeBrothersSubId0State1_hook)) { s_strangeBrothersSubId0State1_hook(gb); return; }
    if (jt_ == SYM(strangeBrothersSubId0State2) && hook_is(gb, SYM(strangeBrothersSubId0State2), s_strangeBrothersSubId0State2_hook)) { s_strangeBrothersSubId0State2_hook(gb); return; }
    if (jt_ == SYM(strangeBrothersSubId0State3) && hook_is(gb, SYM(strangeBrothersSubId0State3), s_strangeBrothersSubId0State3_hook)) { s_strangeBrothersSubId0State3_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); A = 0x10;
  CALL_C(b_+17, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+20);
  if (!(F & FZ)) { CYCT(b_+20, b_+23); TAIL(interactionDelete); }
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+25); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+25, b_+27); A = 0x83;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+31); A = mem_rd(gb, wActiveRoom);
  CYC(b_+31, b_+34); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CYC(b_+34, b_+36); A = 0x10;
  CALL_C(b_+36, s_setGlobalFlag, SYM(setGlobalFlag), b_+39);
  s_func_6964_hook(gb); return; // falls through
}

void s_func_6964_hook(GB *gb) {
  BASE(func_6964);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+5); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+5, b_+8); SET_BC(0x016d);
  CALL_C(b_+8, s_spawnHider_hook, SYM(spawnHider), b_+11);
  CYC(b_+11, b_+14); SET_BC(SYM(pollInput));
  CALL_C(b_+14, s_spawnHider_hook, SYM(spawnHider), b_+17);
  CYC(b_+17, b_+19); A = 0x17;
  CALL_C(b_+19, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+22);
  CYC(b_+22, b_+24); A = 0x01;
  if (!(F & FC)) { CYCT(b_+24, b_+26); goto L_6983; }
  CYC(b_+24, b_+26);
  CALL_C(b_+26, s_getRandomNumber, SYM(getRandomNumber), b_+29);
  CYC(b_+29, b_+31); alu_and(gb, 0x01);
L_6983:
  CYC(b_+31, b_+34); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+34, b_+36); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+39); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+39, b_+42); TAIL_S(func_6995); }
  CYC(b_+39, b_+42);
  RET(b_+42); return;
}

void s_func_698f_hook(GB *gb) {
  BASE(func_698f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveMusic);
  CALL_C(b_+3, s_playSound, SYM(playSound_b00), b_+6);
  s_func_6995_hook(gb); return; // falls through
}

void s_func_6995_hook(GB *gb) {
  BASE(func_6995);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+4); mem_wr(gb, wcc84, A);
  CYC(b_+4, b_+7);
  TAIL(interactionDelete);
}

// @param[out]	b	subid
// @param[out]	c	id
void s_spawnHider_hook(GB *gb) {
  BASE(spawnHider);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+3);
  CYC(b_+3, b_+4); L = alu_dec8(gb, L);
  CYC(b_+4, b_+6); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, C);
  CYC(b_+8, b_+9); L = alu_inc8(gb, L);
  CYC(b_+9, b_+10); mem_wr(gb, HL, B);
  CYC(b_+10, b_+11); L = alu_inc8(gb, L);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  RET(b_+15); return;
}

void s_func_69ac_hook(GB *gb) {
  BASE(func_69ac);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, BC);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); SET_BC(BC + 1);
  CYC(b_+6, b_+7); A = mem_rd(gb, BC);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A);
  RET(b_+8); return;
}

void s_strangeBrothersSubId0State1_hook(GB *gb) {
  BASE(strangeBrothersSubId0State1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScrollMode);
  CYC(b_+3, b_+5); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wcc84);
  CYC(b_+9, b_+11); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+16); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+18, b_+20); A = 0x02;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+23); A = 0x11;
  CALL_C(b_+23, s_unsetGlobalFlag, SYM(unsetGlobalFlag), b_+26);
  CYC(b_+26, b_+28); A = 0x0b;
  CYC(b_+28, b_+31);
  TAIL(playSound_b00);
}

void s_strangeBrothersSubId0State2_hook(GB *gb) {
  BASE(strangeBrothersSubId0State2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+3, b_+5); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+5, b_+7); TAIL_S(func_6a23); }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_and(gb, 0x03);
  CYC(b_+9, b_+11); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto L_69eb; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  CYC(b_+17, b_+20); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+20, b_+23); mem_wr(gb, wTmpcfc0, A);
L_69eb:
  CYC(b_+23, b_+26); A = mem_rd(gb, wActiveRoom);
  CYC(b_+26, b_+27); B = A;
  CYC(b_+27, b_+30); A = mem_rd(gb, wTmpcfc0 + 0x11);
  CYC(b_+30, b_+31); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+33); A = B;
  CYC(b_+33, b_+36); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CYC(b_+36, b_+38); alu_cp(gb, 0x51);
  if (F & FZ) { CYCT(b_+38, b_+40); TAIL_S(func_698f); }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+42); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+45); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); TAIL_S(func_698f); }
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+48); alu_xor(gb, A);
  CYC(b_+48, b_+49); mem_wr(gb, DE, A);
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+52, b_+53); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+53, b_+54); A = mem_rd(gb, HL);
  CYC(b_+54, b_+57); SET_BC(b_+72 /* @table_6a1c */);
  CALL_C(b_+57, s_addAToBc, SYM(addAToBc), b_+60);
  CYC(b_+60, b_+61); A = mem_rd(gb, BC);
  CYC(b_+61, b_+62); B = A;
  CYC(b_+62, b_+65); A = mem_rd(gb, wActiveRoom);
  CYC(b_+65, b_+66); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+66, b_+69); TAIL_S(func_698f); }
  CYC(b_+66, b_+69);
  CYC(b_+69, b_+72);
  TAIL_S(func_6964);
}

void s_func_6a23_hook(GB *gb) {
  BASE(func_6a23);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); A = 0x03;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+10); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+10, b_+13); SET_BC(0x2804);
  CYC(b_+13, b_+16);
  TAIL(showText);
}

void s_strangeBrothersSubId0State3_hook(GB *gb) {
  BASE(strangeBrothersSubId0State3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0x11;
  CALL_C(b_+7, s_setGlobalFlag, SYM(setGlobalFlag), b_+10);
  CYC(b_+10, b_+12); A = 0x10;
  CALL_C(b_+12, s_unsetGlobalFlag, SYM(unsetGlobalFlag), b_+15);
  CYC(b_+15, b_+16); alu_xor(gb, A);
  CYC(b_+16, b_+19); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+19, b_+22); SET_HL(b_+28 /* @warpDestVariables */);
  CALL_C(b_+22, s_setWarpDestVariables, SYM(setWarpDestVariables), b_+25);
  CYC(b_+25, b_+28);
  TAIL(interactionDelete);
}

// strangeBrothersSubId2@func_6ad1
static void subrosiansHiding_func_6ad1(GB *gb) {
  BASE(strangeBrothersSubId1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+125, b_+127); C = 0x60;
  CALL_C(b_+127, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+130);
  if (!(F & FZ)) { RET_TAKEN(b_+130); return; }
  CYC(b_+130, b_+131);
  CYC(b_+131, b_+134); SET_BC(0xfe00);
  CYC(b_+134, b_+137);
  TAIL(objectSetSpeedZ);
}

void s_strangeBrothersSubId1_hook(GB *gb) {
  BASE(strangeBrothersSubId1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosiansHiding_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+94) goto state1;
    if (jt_ == b_+171) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+16);
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+18, b_+20); A = 0x28;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+25); B = A;
  CYC(b_+25, b_+26); A = alu_dec8(gb, A);
  CYC(b_+26, b_+28); A = 0x02;
  if (F & FZ) { CYCT(b_+28, b_+30); goto L_6a73; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); A = alu_dec8(gb, A);
L_6a73:
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  CYC(b_+34, b_+35); A = B;
  CYC(b_+35, b_+36); A = alu_dec8(gb, A);
  CYC(b_+36, b_+39); SET_HL(b_+62 /* @table_6a92 */);
  CYC(b_+39, b_+40); subrosiansHiding_add_double_index(gb, b_+40);
  CYC(b_+40, b_+41); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+41, b_+42); H = mem_rd(gb, HL);
  CYC(b_+42, b_+43); L = A;
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); subrosiansHiding_add_double_index(gb, b_+47);
  CYC(b_+47, b_+48); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+48, b_+49); H = mem_rd(gb, HL);
  CYC(b_+49, b_+50); L = A;
  CALL_C(b_+50, s_interactionSetScript, SYM(interactionSetScript), b_+53);
  CALL_C(b_+53, s_interactionRunScript, SYM(interactionRunScript), b_+56);
  CALL_C(b_+56, s_interactionRunScript, SYM(interactionRunScript), b_+59);
  CYC(b_+59, b_+62);
  TAIL(objectSetVisiblec2);

state1:
  CYC(b_+94, b_+97); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+97, b_+98); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+98, b_+100); goto func_6add; }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);
  CYC(b_+103, b_+104); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+104, b_+106); goto func_6add; }
  CYC(b_+104, b_+106);
  CYC(b_+106, b_+109); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+109, b_+111); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+111, b_+113); goto func_6add; }
  CYC(b_+111, b_+113);
  CALL_C(b_+113, s_interactionAnimate, SYM(interactionAnimate), b_+116);
  CALL_C(b_+116, s_interactionAnimate, SYM(interactionAnimate), b_+119);
  CALL_C(b_+119, s_interactionRunScript, SYM(interactionRunScript), b_+122);
  if (F & FC) { CYCT(b_+122, b_+125); TAIL(interactionDelete); }
  CYC(b_+122, b_+125);
  CYC(b_+125, b_+127); C = 0x60;
  CALL_C(b_+127, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+130);
  if (!(F & FZ)) { RET_TAKEN(b_+130); return; }
  CYC(b_+130, b_+131);
  CYC(b_+131, b_+134); SET_BC(0xfe00);
  CYC(b_+134, b_+137);
  TAIL(objectSetSpeedZ);
func_6add:
  CYC(b_+137, b_+139); A = 0xff;
  CYC(b_+139, b_+142); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+142, b_+144); A = 0x01;
  CYC(b_+144, b_+147); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+147, b_+148); H = D;
  CYC(b_+148, b_+150); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+150, b_+151); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+151, b_+153); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+153, b_+155); mem_wr(gb, HL, 0x64);
  CALL_C(b_+155, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+158);
  CYC(b_+158, b_+160); alu_add(gb, 0x10);
  CYC(b_+160, b_+162); alu_and(gb, 0x1f);
  CYC(b_+162, b_+164); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+164, b_+165); mem_wr(gb, DE, A);
  CALL_C(b_+165, s_convertAngleDeToDirection, SYM(convertAngleDeToDirection), b_+168);
  CYC(b_+168, b_+171);
  TAIL(interactionSetAnimation);
state2:
  CALL_C(b_+171, s_interactionAnimate, SYM(interactionAnimate), b_+174);
  CALL_C(b_+174, s_interactionAnimate, SYM(interactionAnimate), b_+177);
  CALL_L(b_+177, subrosiansHiding_func_6ad1, b_+180);
  CALL_C(b_+180, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+183);
  CALL_C(b_+183, s_objectApplySpeed, SYM(objectApplySpeed), b_+186);
  CALL_C(b_+186, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+189);
  if (F & FC) { RET_TAKEN(b_+189); return; }
  CYC(b_+189, b_+190);
  CALL_C(b_+190, s_objectSetInvisible, SYM(objectSetInvisible), b_+193);
  CYC(b_+193, b_+194); H = D;
  CYC(b_+194, b_+196); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+196, b_+197); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+197, b_+199);
  goto L_6b1b;
L_6b1b:
  CYC(b_+199, b_+200); alu_xor(gb, A);
  CYC(b_+200, b_+203); mem_wr(gb, wTmpcfc0, A);
  CYC(b_+203, b_+206);
  TAIL(interactionDelete);
}

