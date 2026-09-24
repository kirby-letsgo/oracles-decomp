#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/getRodOfSeasons.s.
// INTERAC_GET_ROD_OF_SEASONS
// Variables:
// var03:    Index of a seasons' sparkle from 0 to 3
// var3b:    Initial time for each seasons' sparkle to start dropping sparkles
// $cceb:    Set to 1 when Rod disappears, to remove its aura, and continue cutscene

static uint16_t getRodOfSeasons_jump_table(GB *gb) {
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

static void getRodOfSeasons_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void getRodOfSeasons_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_GET_ROD_OF_SEASONS
// Variables:
// var03:    Index of a seasons' sparkle from 0 to 3
// var3b:    Initial time for each seasons' sparkle to start dropping sparkles
// $cceb:    Set to 1 when Rod disappears, to remove its aura, and continue cutscene
void s_interactionCodee6_hook(GB *gb) {
  BASE(interactionCodee6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (getRodOfSeasons_jump_table(gb));
    if (jt_ == SYM(interactionCodee6_state0) && hook_is(gb, SYM(interactionCodee6_state0), s_interactionCodee6_state0_hook)) { s_interactionCodee6_state0_hook(gb); return; }
    if (jt_ == SYM(interactionCodee6_state1) && hook_is(gb, SYM(interactionCodee6_state1), s_interactionCodee6_state1_hook)) { s_interactionCodee6_state1_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_interactionCodee6_state0_hook(GB *gb) {
  BASE(interactionCodee6_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CALL_C(b_+3, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); push_effect(gb, b_+10);
  do { uint16_t jt_ = (getRodOfSeasons_jump_table(gb));
    if (jt_ == b_+18) goto subid0;
    if (jt_ == b_+36) goto sparkles;
    if (jt_ == b_+88) goto rodOfSeasons;
    if (jt_ == b_+124) goto rodOfSeasonsAura;
    HANDOFF(HL);
  } while (0);
subid0:
  CALL_C(b_+18, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+21);
  CYC(b_+21, b_+23); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+23, b_+26); TAIL(interactionDelete); }
  CYC(b_+23, b_+26);
  CYC(b_+26, b_+27); alu_xor(gb, A);
  CYC(b_+27, b_+30); mem_wr(gb, wccd4, A);
  CYC(b_+30, b_+33); SET_HL(SYM(gettingRodOfSeasonsScript));
  CYC(b_+33, b_+36);
  TAIL(interactionSetScript);
sparkles:
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+40); alu_rlca(gb);
  CYC(b_+40, b_+43); SET_HL(b_+72 /* @sparklesData */);
  CYC(b_+43, b_+44); getRodOfSeasons_add_double_index(gb, b_+44);
  CYC(b_+44, b_+45); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+45, b_+47); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+53); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+53, b_+55); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+55, b_+56); mem_wr(gb, DE, A);
  CYC(b_+56, b_+57); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
  CYC(b_+60, b_+61); H = D;
  CYC(b_+61, b_+63); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+63, b_+65); mem_wr(gb, HL, 0x3c);
  CYC(b_+65, b_+67); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x5a);
  CYC(b_+69, b_+72);
  TAIL(objectSetVisible80);

rodOfSeasons:
  CYC(b_+88, b_+90); A = 0x04;
  CALL_C(b_+90, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+93);
  CYC(b_+93, b_+94); H = D;
  CYC(b_+94, b_+96); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0xf0);
  CYC(b_+98, b_+100); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0x00);
  CYC(b_+102, b_+104); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x30);
  CYC(b_+106, b_+109); SET_BC(0xe603);
  CALL_C(b_+109, s_objectCreateInteraction, SYM(objectCreateInteraction), b_+112);
  if (!(F & FZ)) { RET_TAKEN(b_+112); return; }
  CYC(b_+112, b_+113);
  CYC(b_+113, b_+115); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+115, b_+117); A = mem_rd(gb, hActiveObjectType);
  CYC(b_+117, b_+118); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+118, b_+120); A = mem_rd(gb, hActiveObject);
  CYC(b_+120, b_+121); mem_wr(gb, HL, A);
  CYC(b_+121, b_+124);
  TAIL(objectSetVisible81);
rodOfSeasonsAura:
  CALL_C(b_+124, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+127);
  CYC(b_+127, b_+130);
  TAIL(objectSetVisible82);
}

void s_interactionCodee6_state1_hook(GB *gb) {
  BASE(interactionCodee6_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (getRodOfSeasons_jump_table(gb));
    if (jt_ == b_+12) goto subid0;
    if (jt_ == b_+19) goto sparkles;
    if (jt_ == b_+50) goto rodOfSeasons;
    if (jt_ == b_+280) goto rodOfSeasonsAura;
    HANDOFF(HL);
  } while (0);
subid0:
  CALL_C(b_+12, s_interactionRunScript, SYM(interactionRunScript), b_+15);
  if (F & FC) { CYCT(b_+15, b_+18); TAIL(interactionDelete); }
  CYC(b_+15, b_+18);
  RET(b_+18); return;
sparkles:
  CALL_C(b_+19, s_interactionAnimate, SYM(interactionAnimate), b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); push_effect(gb, b_+26);
  do { uint16_t jt_ = (getRodOfSeasons_jump_table(gb));
    if (jt_ == b_+30) goto sparkles_waitToMove;
    if (jt_ == b_+37) goto sparkles_move;
    HANDOFF(HL);
  } while (0);
sparkles_waitToMove:
  CALL_C(b_+30, s_interactionDecCounter2, SYM(interactionDecCounter2), b_+33);
  if (!(F & FZ)) { RET_TAKEN(b_+33); return; }
  CYC(b_+33, b_+34);
  CALL_C(b_+34, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+37);
sparkles_move:
  CALL_C(b_+37, s_dropSparkles_hook, SYM(dropSparkles), b_+40);
  CALL_C(b_+40, s_objectApplySpeed, SYM(objectApplySpeed), b_+43);
  CALL_C(b_+43, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+46);
  if (F & FZ) { CYCT(b_+46, b_+49); TAIL(interactionDelete); }
  CYC(b_+46, b_+49);
  RET(b_+49); return;
rodOfSeasons:
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CYC(b_+53, b_+54); push_effect(gb, b_+54);
  do { uint16_t jt_ = (getRodOfSeasons_jump_table(gb));
    if (jt_ == b_+68) goto rodOfSeasons_substate0;
    if (jt_ == b_+135) goto rodOfSeasons_substate1;
    if (jt_ == b_+156) goto rodOfSeasons_substate2;
    if (jt_ == b_+175) goto rodOfSeasons_substate3;
    if (jt_ == b_+237) goto rodOfSeasons_substate4;
    if (jt_ == b_+249) goto rodOfSeasons_substate5;
    if (jt_ == b_+263) goto rodOfSeasons_substate6;
    HANDOFF(HL);
  } while (0);
rodOfSeasons_substate0:
  CYC(b_+68, b_+71); A = mem_rd(gb, wFrameCounter);
  CYC(b_+71, b_+73); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+73); return; }
  CYC(b_+73, b_+74);
  CYC(b_+74, b_+75); H = D;
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+78, b_+79); A = mem_rd(gb, HL);
  CYC(b_+79, b_+81); alu_and(gb, 0x0f);
  CYC(b_+81, b_+84); SET_HL(b_+119 /* @rodOfSeasons@seasonsTable_15_705f */);
  CYC(b_+84, b_+85); getRodOfSeasons_add_a_to_hl(gb, b_+85);
  CYC(b_+85, b_+86); A = mem_rd(gb, HL);
  CYC(b_+86, b_+88); alu_add(gb, 0xf0);
  CYC(b_+88, b_+90); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CYC(b_+91, b_+92); H = D;
  CYC(b_+92, b_+94); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+94, b_+95); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+95); return; }
  CYC(b_+95, b_+96);
  CALL_C(b_+96, s_clearAllParentItems, SYM(clearAllParentItems), b_+99);
  CYC(b_+99, b_+102); SET_HL(w1Link_direction);
  CYC(b_+102, b_+104); mem_wr(gb, HL, 0x00);
  CALL_C(b_+104, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+107);
  CYC(b_+107, b_+108); H = D;
  CYC(b_+108, b_+110); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+110, b_+111); mem_wr(gb, HL, A);
  CYC(b_+111, b_+113); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+113, b_+115); mem_wr(gb, HL, 0x14);
  CYC(b_+115, b_+117); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+117, b_+118); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+118); return;

rodOfSeasons_substate1:
  CALL_C(b_+135, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+138);
  CYC(b_+138, b_+140); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+140, b_+141); mem_wr(gb, DE, A);
  CALL_C(b_+141, s_objectApplySpeed, SYM(objectApplySpeed), b_+144);
  CALL_C(b_+144, s_objectCheckCollidedWithLink_ignoreZ, SYM(objectCheckCollidedWithLink_ignoreZ), b_+147);
  if (!(F & FC)) { RET_TAKEN(b_+147); return; }
  CYC(b_+147, b_+148);
  CYC(b_+148, b_+150); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+150, b_+152); A = 0x06;
  CYC(b_+152, b_+153); mem_wr(gb, DE, A);
  CYC(b_+153, b_+156);
  TAIL(interactionIncSubstate);
rodOfSeasons_substate2:
  CYC(b_+156, b_+158); C = 0x08;
  CALL_C(b_+158, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+161);
  if (F & FZ) { CYCT(b_+161, b_+163); goto L_708f; }
  CYC(b_+161, b_+163);
  CALL_C(b_+163, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+166);
  if (!(F & FC)) { RET_TAKEN(b_+166); return; }
  CYC(b_+166, b_+167);
L_708f:
  CYC(b_+167, b_+168); H = D;
  CYC(b_+168, b_+170); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+170, b_+172); mem_wr(gb, HL, 0x1e);
  CYC(b_+172, b_+175);
  TAIL(interactionIncSubstate);
rodOfSeasons_substate3:
  CALL_C(b_+175, s_interactionDecCounter2, SYM(interactionDecCounter2), b_+178);
  if (!(F & FZ)) { RET_TAKEN(b_+178); return; }
  CYC(b_+178, b_+179);
  CYC(b_+179, b_+181); A = 0x04;
  CYC(b_+181, b_+184); mem_wr(gb, wLinkForceState, A);
  CYC(b_+184, b_+185); alu_xor(gb, A);
  CYC(b_+185, b_+188); mem_wr(gb, wcc50, A);
  CALL_C(b_+188, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+191);
  CYC(b_+191, b_+194); A = mem_rd(gb, w1Link_yh);
  CYC(b_+194, b_+196); alu_sub(gb, 0x0e);
  CYC(b_+196, b_+198); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+198, b_+199); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+199, b_+200); L = alu_inc8(gb, L);
  CYC(b_+200, b_+203); A = mem_rd(gb, w1Link_xh);
  CYC(b_+203, b_+205); alu_sub(gb, 0x04);
  CYC(b_+205, b_+206); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+206, b_+207); L = alu_inc8(gb, L);
  CYC(b_+207, b_+208); alu_xor(gb, A);
  CYC(b_+208, b_+209); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+209, b_+210); mem_wr(gb, HL, A);
  CYC(b_+210, b_+212); B = 0x00;
  CYC(b_+212, b_+214); C = 0x71;
  CALL_C(b_+214, s_showText, SYM(showText), b_+217);
  CALL_C(b_+217, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+220);
  CYC(b_+220, b_+222); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+222, b_+224); A = 0x06;
  CALL_C(b_+224, s_playSound, SYM(playSound_b00), b_+227);
  CYC(b_+227, b_+229); C = 0x07;
  CYC(b_+229, b_+231); A = 0x07;
  CALL_C(b_+231, s_giveTreasure, SYM(giveTreasure), b_+234);
  CYC(b_+234, b_+237);
  TAIL(darkenRoom);
rodOfSeasons_substate4:
  CALL_C(b_+237, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+240);
  CALL_C(b_+240, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+243);
  CYC(b_+243, b_+246); SET_HL(SYM(gettingRodOfSeasonsScript_setCounter1To32));
  CYC(b_+246, b_+249);
  TAIL(interactionSetScript);
rodOfSeasons_substate5:
  CALL_C(b_+249, s_interactionRunScript, SYM(interactionRunScript), b_+252);
  if (!(F & FC)) { RET_TAKEN(b_+252); return; }
  CYC(b_+252, b_+253);
  CALL_C(b_+253, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+256);
  CYC(b_+256, b_+258); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+258, b_+260); mem_wr(gb, HL, 0x14);
  CYC(b_+260, b_+263);
  TAIL(brightenRoom);
rodOfSeasons_substate6:
  CYC(b_+263, b_+266); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+266, b_+267); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+267); return; }
  CYC(b_+267, b_+268);
  CALL_C(b_+268, s_interactionDecCounter2, SYM(interactionDecCounter2), b_+271);
  if (!(F & FZ)) { RET_TAKEN(b_+271); return; }
  CYC(b_+271, b_+272);
  CYC(b_+272, b_+274); A = 0x01;
  CYC(b_+274, b_+277); mem_wr(gb, wccd4, A);
  CYC(b_+277, b_+280);
  TAIL(interactionDelete);
rodOfSeasonsAura:
  CYC(b_+280, b_+283); A = mem_rd(gb, wccd4);
  CYC(b_+283, b_+284); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+284, b_+287); TAIL(interactionDelete); }
  CYC(b_+284, b_+287);
  CYC(b_+287, b_+289); A = 0x00;
  CALL_C(b_+289, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+292);
  CALL_C(b_+292, s_objectTakePosition, SYM(objectTakePosition), b_+295);
  CALL_C(b_+295, s_interactionAnimate, SYM(interactionAnimate), b_+298);
  CYC(b_+298, b_+299); H = D;
  CYC(b_+299, b_+301); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+301, b_+302); A = mem_rd(gb, HL);
  CYC(b_+302, b_+303); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+303); return; }
  CYC(b_+303, b_+304);
  CYC(b_+304, b_+306); mem_wr(gb, HL, 0x00);
  CYC(b_+306, b_+308); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+308, b_+310); A = 0x80;
  CYC(b_+310, b_+311); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+311, b_+312); mem_wr(gb, HL, A);
  RET(b_+312); return;
}

void s_dropSparkles_hook(GB *gb) {
  BASE(dropSparkles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x10);
  CYC(b_+9, b_+12); SET_BC(0x8401);
  CYC(b_+12, b_+15);
  TAIL(objectCreateInteraction);
}

void s_forceLinksDirection_hook(GB *gb) {
  BASE(forceLinksDirection);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_direction);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+6); A = 0x80;
  CYC(b_+6, b_+9);
  TAIL(setLinkForceStateToState08_withParam);
}

void s_spawnRodOfSeasonsSparkles_hook(GB *gb) {
  BASE(spawnRodOfSeasonsSparkles);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(b_+37 /* @spawnCoordinates */);
  CYC(b_+3, b_+4); alu_xor(gb, A);
L_713d:
  CYC(b_+4, b_+6); mem_wr(gb, hFF8B, A);
  CALL_C(b_+6, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0xe6);
  CYC(b_+12, b_+13); L = alu_inc8(gb, L);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x01);
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);
  CYC(b_+16, b_+18); A = mem_rd(gb, hFF8B);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+21, b_+22); A = mem_rd(gb, BC);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+24); SET_BC(BC + 1);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+26, b_+27); A = mem_rd(gb, BC);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+29); SET_BC(BC + 1);
  CYC(b_+29, b_+31); A = mem_rd(gb, hFF8B);
  CYC(b_+31, b_+32); A = alu_inc8(gb, A);
  CYC(b_+32, b_+34); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+34, b_+36); goto L_713d; }
  CYC(b_+34, b_+36);
  RET(b_+36); return;
}

