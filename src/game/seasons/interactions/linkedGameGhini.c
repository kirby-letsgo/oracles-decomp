#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/linkedGameGhini.s.
// INTERAC_LINKED_GAME_GHINI

static uint16_t linkedGameGhini_jump_table(GB *gb) {
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

static void linkedGameGhini_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// interactionCodecb@func_7be1
static void linkedGameGhini_func_7be1(GB *gb) {
  BASE(interactionCodecb);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+292, b_+293); H = D;
  CYC(b_+293, b_+295); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+295, b_+296); A = mem_rd(gb, HL);
  CYC(b_+296, b_+297); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+297, b_+299); goto L_7bf0; }
  CYC(b_+297, b_+299);
  CYC(b_+299, b_+300); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+300, b_+303); A = mem_rd(gb, wFrameCounter);
  CYC(b_+303, b_+304); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+304, b_+307); TAIL(objectSetInvisible); }
  CYC(b_+304, b_+307);
L_7bf0:
  CYC(b_+307, b_+310);
  TAIL(objectSetVisible);
}

// INTERAC_LINKED_GAME_GHINI
void s_interactionCodecb_hook(GB *gb) {
  BASE(interactionCodecb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkedGameGhini_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == b_+179) goto state1;
    if (jt_ == b_+258) goto state2;
    if (jt_ == b_+310) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (linkedGameGhini_jump_table(gb));
    if (jt_ == b_+24) goto state0_subid0;
    if (jt_ == b_+78) goto state0_subid1;
    if (jt_ == b_+148) goto state0_subid3;
    HANDOFF(HL);
  } while (0);
state0_subid0:
  CYC(b_+24, b_+26); A = 0x28;
  CALL_C(b_+26, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+29);
  if (F & FZ) { CYCT(b_+29, b_+32); TAIL(interactionDelete); }
  CYC(b_+29, b_+32);
  CALL_C(b_+32, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+35);
  CALL_C(b_+35, s_interactionIncState, SYM(interactionIncState), b_+38);
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x78);
  CYC(b_+42, b_+44); A = 0x4c;
  CALL_C(b_+44, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+47);
  CYC(b_+47, b_+49); A = 0x5b;
  CALL_C(b_+49, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+52);
  if (F & FZ) { CYCT(b_+52, b_+54); goto state0_notDoneSecret; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+57); SET_HL(SYM(linkedGhiniScript_doneSecret));
  CYC(b_+57, b_+59);
  goto state0_setScript;
state0_notDoneSecret:
  CYC(b_+59, b_+61); A = 0x51;
  CALL_C(b_+61, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+64);
  CYC(b_+64, b_+67); SET_HL(SYM(linkedGhiniScript_beginningSecret));
  if (F & FZ) { CYCT(b_+67, b_+69); goto state0_setScript; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+72); SET_HL(SYM(linkedGhiniScript_begunSecret));
state0_setScript:
  CALL_C(b_+72, s_interactionSetScript, SYM(interactionSetScript), b_+75);
  CYC(b_+75, b_+78);
  TAIL(objectSetVisible81);
state0_subid1:
  CALL_C(b_+78, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+81);
  CYC(b_+81, b_+82); H = D;
  CYC(b_+82, b_+84); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+87); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+87, b_+88); mem_wr(gb, HL, A);
  CYC(b_+88, b_+90); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x02);
  CYC(b_+92, b_+94); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+94, b_+96); mem_wr(gb, HL, 0x1e);
  CYC(b_+96, b_+98); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+98, b_+99); A = mem_rd(gb, HL);
  CYC(b_+99, b_+101); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+101, b_+102); mem_wr(gb, HL, A);
  CYC(b_+102, b_+104); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  CYC(b_+105, b_+107); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+107, b_+108); mem_wr(gb, HL, A);
  CALL_C(b_+108, s_getRandomNumber, SYM(getRandomNumber), b_+111);
  CYC(b_+111, b_+113); alu_and(gb, 0x02);
  CYC(b_+113, b_+114); A = alu_dec8(gb, A);
  CYC(b_+114, b_+116); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+116, b_+117); mem_wr(gb, DE, A);
  CALL_C(b_+117, s_getRandomNumber, SYM(getRandomNumber), b_+120);
  CYC(b_+120, b_+122); alu_and(gb, 0x1f);
  CYC(b_+122, b_+124); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+124, b_+125); mem_wr(gb, DE, A);
  CALL_C(b_+125, s_getRandomNumber, SYM(getRandomNumber), b_+128);
  CYC(b_+128, b_+130); alu_and(gb, 0x03);
  CYC(b_+130, b_+133); SET_HL(b_+144 /* @state0@table_7b4d */);
  CYC(b_+133, b_+134); linkedGameGhini_add_a_to_hl(gb, b_+134);
  CYC(b_+134, b_+135); A = mem_rd(gb, HL);
  CYC(b_+135, b_+137); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CALL_C(b_+138, s_func_7c3f_hook, SYM(func_7c3f), b_+141);
  CYC(b_+141, b_+144);
  TAIL(objectSetVisible81);

state0_subid3:
  CALL_C(b_+148, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+151);
  if (F & FZ) { CYCT(b_+151, b_+154); TAIL(interactionDelete); }
  CYC(b_+151, b_+154);
  CALL_C(b_+154, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+157);
  CYC(b_+157, b_+158); H = D;
  CYC(b_+158, b_+160); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+160, b_+162); mem_wr(gb, HL, 0x02);
  CYC(b_+162, b_+164); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+164, b_+166); mem_wr(gb, HL, 0x03);
  CYC(b_+166, b_+168); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+168, b_+170); mem_wr(gb, HL, 0x04);
  CYC(b_+170, b_+173); SET_HL((SYM(ganon_decideNextMove) + 43) /* @choice0 */);
  CALL_C(b_+173, s_interactionSetScript, SYM(interactionSetScript), b_+176);
  CYC(b_+176, b_+179);
  TAIL(interactionAnimateAsNpc);
state1:
  CYC(b_+179, b_+181); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+181, b_+182); A = mem_rd(gb, DE);
  CYC(b_+182, b_+183); push_effect(gb, b_+183);
  do { uint16_t jt_ = (linkedGameGhini_jump_table(gb));
    if (jt_ == b_+191) goto state1_substate0;
    if (jt_ == b_+207) goto state1_substate1;
    if (jt_ == b_+219) goto state1_substate2;
    if (jt_ == b_+232) goto state1_substate3;
    HANDOFF(HL);
  } while (0);
state1_substate0:
  CALL_C(b_+191, s_interactionAnimate, SYM(interactionAnimate), b_+194);
  CALL_C(b_+194, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+197);
  CALL_C(b_+197, s_interactionRunScript, SYM(interactionRunScript), b_+200);
  if (!(F & FC)) { RET_TAKEN(b_+200); return; }
  CYC(b_+200, b_+201);
  CALL_C(b_+201, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+204);
  CYC(b_+204, b_+207);
  TAIL_S(func_7c0f);
state1_substate1:
  CALL_C(b_+207, s_func_7bf9_hook, SYM(func_7bf9), b_+210);
  if (!(F & FZ)) { RET_TAKEN(b_+210); return; }
  CYC(b_+210, b_+211);
  CYC(b_+211, b_+213); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+213, b_+214); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+214, b_+216); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+216, b_+218); mem_wr(gb, HL, 0x3c);
  RET(b_+218); return;
state1_substate2:
  CALL_C(b_+219, s_func_7bf9_hook, SYM(func_7bf9), b_+222);
  if (!(F & FZ)) { RET_TAKEN(b_+222); return; }
  CYC(b_+222, b_+223);
  CYC(b_+223, b_+225); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+225, b_+226); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+226, b_+229); SET_HL(SYM(linkedGhiniScript_startRound));
  CALL_C(b_+229, s_interactionSetScript, SYM(interactionSetScript), b_+232);
state1_substate3:
  CALL_C(b_+232, s_interactionAnimate, SYM(interactionAnimate), b_+235);
  CALL_C(b_+235, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+238);
  CALL_C(b_+238, s_interactionRunScript, SYM(interactionRunScript), b_+241);
  if (!(F & FC)) { RET_TAKEN(b_+241); return; }
  CYC(b_+241, b_+242);
  CYC(b_+242, b_+243); H = D;
  CYC(b_+243, b_+245); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+245, b_+247); mem_wr(gb, HL, 0x01);
  CYC(b_+247, b_+249); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+249, b_+250); A = mem_rd(gb, HL);
  CYC(b_+250, b_+252); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+252, b_+255); TAIL_S(func_71c5); }
  CYC(b_+252, b_+255);
  CYC(b_+255, b_+258);
  TAIL_S(func_7c0f);
state2:
  CALL_C(b_+258, s_interactionAnimate, SYM(interactionAnimate), b_+261);
  CALL_L(b_+261, linkedGameGhini_func_7be1, b_+264);
  CALL_C(b_+264, s_func_7bf9_hook, SYM(func_7bf9), b_+267);
  if (F & FZ) { CYCT(b_+267, b_+270); TAIL(interactionDelete); }
  CYC(b_+267, b_+270);
  CYC(b_+270, b_+272); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+272, b_+273); A = mem_rd(gb, HL);
  CYC(b_+273, b_+274); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+274); return; }
  CYC(b_+274, b_+275);
  CYC(b_+275, b_+277); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+277, b_+278); A = mem_rd(gb, HL);
  CYC(b_+278, b_+280); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+280, b_+281); B = mem_rd(gb, HL);
  CYC(b_+281, b_+283); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+283, b_+284); C = mem_rd(gb, HL);
  CYC(b_+284, b_+286); E = INTERACTION_BASE + OBJ_VAR3F;
  CALL_C(b_+286, s_objectSetPositionInCircleArc, SYM(objectSetPositionInCircleArc), b_+289);
  CYC(b_+289, b_+292);
  TAIL_S(func_7bfe);

state3:
  CALL_C(b_+310, s_interactionRunScript, SYM(interactionRunScript), b_+313);
  CYC(b_+313, b_+316);
  TAIL(interactionAnimateAsNpc);
}

void s_func_7bf9_hook(GB *gb) {
  BASE(func_7bf9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

void s_func_7bfe_hook(GB *gb) {
  BASE(func_7bfe);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+4); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+8, b_+9); B = mem_rd(gb, HL);
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+13); alu_add(gb, B);
  CYC(b_+13, b_+15); alu_and(gb, 0x1f);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  RET(b_+16); return;
}

// func_71c5+10
static void linkedGameGhini_func_71c5_10(GB *gb) {
  BASE(func_71c5);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+10, s_func_7c3f_hook, SYM(func_7c3f), b_+13);
  CALL_C(b_+13, s_getRandomNumber, SYM(getRandomNumber), b_+16);
  CYC(b_+16, b_+18); alu_and(gb, 0x01);
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); push_effect(gb, DE);
  CALL_C(b_+22, s_clearEnemies, SYM(clearEnemies), b_+25);
  CALL_C(b_+25, s_clearItems, SYM(clearItems), b_+28);
  CALL_C(b_+28, s_clearParts, SYM(clearParts), b_+31);
  CYC(b_+31, b_+32); SET_DE(pop_effect(gb));
  CYC(b_+32, b_+33); alu_xor(gb, A);
  CYC(b_+33, b_+36); mem_wr(gb, wNumEnemies, A);
  CALL_C(b_+36, s_func_7c50_hook, SYM(func_7c50), b_+39);
  CYC(b_+39, b_+42);
  TAIL_S(func_7cce);
}

void s_func_7c0f_hook(GB *gb) {
  BASE(func_7c0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+6);
  linkedGameGhini_func_71c5_10(gb); return;
}

void s_func_71c5_hook(GB *gb) {
  BASE(func_71c5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); A = alu_inc8(gb, A);
  CYC(b_+4, b_+6); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+6, b_+8); goto L_7c1e; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); alu_xor(gb, A);
L_7c1e:
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CALL_C(b_+10, s_func_7c3f_hook, SYM(func_7c3f), b_+13);
  CALL_C(b_+13, s_getRandomNumber, SYM(getRandomNumber), b_+16);
  CYC(b_+16, b_+18); alu_and(gb, 0x01);
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); push_effect(gb, DE);
  CALL_C(b_+22, s_clearEnemies, SYM(clearEnemies), b_+25);
  CALL_C(b_+25, s_clearItems, SYM(clearItems), b_+28);
  CALL_C(b_+28, s_clearParts, SYM(clearParts), b_+31);
  CYC(b_+31, b_+32); SET_DE(pop_effect(gb));
  CYC(b_+32, b_+33); alu_xor(gb, A);
  CYC(b_+33, b_+36); mem_wr(gb, wNumEnemies, A);
  CALL_C(b_+36, s_func_7c50_hook, SYM(func_7c50), b_+39);
  CYC(b_+39, b_+42);
  TAIL_S(func_7cce);
}

void s_func_7c3f_hook(GB *gb) {
  BASE(func_7c3f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_BC(SYM(table_7c4d));
  CALL_C(b_+6, s_addAToBc, SYM(addAToBc), b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, BC);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;
}

void s_func_7c50_hook(GB *gb) {
  BASE(func_7c50);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wWizzrobePositionReservations);
  CYC(b_+3, b_+4); alu_xor(gb, A);
L_7c54:
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); A = alu_inc8(gb, A);
  CYC(b_+6, b_+8); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+8, b_+10); goto L_7c54; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  RET(b_+17); return;
}

void s_func_7c62_hook(GB *gb) {
  BASE(func_7c62);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+5); A = alu_dec8(gb, A);
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CALL_C(b_+6, s_getRandomNumber, SYM(getRandomNumber), b_+9);
L_7c6b:
  CYC(b_+9, b_+10); alu_sub(gb, B);
  if (!(F & FC)) { CYCT(b_+10, b_+12); goto L_7c6b; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); alu_add(gb, B);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+17); SET_HL(wWizzrobePositionReservations);
  CYC(b_+17, b_+18); linkedGameGhini_add_a_to_hl(gb, b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+23); push_effect(gb, DE);
  CYC(b_+23, b_+24); D = C;
  CYC(b_+24, b_+25); E = B;
  CYC(b_+25, b_+26); E = alu_dec8(gb, E);
  CYC(b_+26, b_+27); B = H;
  CYC(b_+27, b_+28); C = L;
L_7c7e:
  CYC(b_+28, b_+29); A = D;
  CYC(b_+29, b_+30); alu_cp(gb, E);
  if (F & FZ) { CYCT(b_+30, b_+32); goto L_7c88; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); SET_BC(BC + 1);
  CYC(b_+33, b_+34); A = mem_rd(gb, BC);
  CYC(b_+34, b_+35); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+35, b_+36); D = alu_inc8(gb, D);
  CYC(b_+36, b_+38);
  goto L_7c7e;
L_7c88:
  CYC(b_+38, b_+39); SET_DE(pop_effect(gb));
  RET(b_+39); return;
}

void s_func_7c8a_hook(GB *gb) {
  BASE(func_7c8a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+8, b_+9); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+12); SET_BC(SYM(table_7c9e));
  CALL_C(b_+12, s_addAToBc, SYM(addAToBc), b_+15);
  CYC(b_+15, b_+16); A = mem_rd(gb, BC);
  CYC(b_+16, b_+18); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+18, b_+19); alu_xor(gb, mem_rd(gb, HL));
  RET(b_+19); return;
}

void s_func_7cce_hook(GB *gb) {
  BASE(func_7cce);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_7cce:
  CALL_C(b_+0, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xcb);
  CYC(b_+6, b_+7); SET_HL(HL + 1);
  CYC(b_+7, b_+8); push_effect(gb, HL);
  CALL_C(b_+8, s_func_7c8a_hook, SYM(func_7c8a), b_+11);
  CYC(b_+11, b_+12); SET_HL(pop_effect(gb));
  CYC(b_+12, b_+13); A = alu_inc8(gb, A);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  CYC(b_+20, b_+21); push_effect(gb, HL);
  CALL_C(b_+21, s_func_7c62_hook, SYM(func_7c62), b_+24);
  CYC(b_+24, b_+25); SET_HL(pop_effect(gb));
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+31); SET_BC(SYM(table_7d03));
  CALL_C(b_+31, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+34);
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+36, b_+37); A = mem_rd(gb, BC);
  CYC(b_+37, b_+38); mem_wr(gb, HL, A);
  CYC(b_+38, b_+39); SET_BC(BC + 1);
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+41, b_+42); A = mem_rd(gb, BC);
  CYC(b_+42, b_+43); mem_wr(gb, HL, A);
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+45, b_+46); A = mem_rd(gb, DE);
  CYC(b_+46, b_+47); A = alu_inc8(gb, A);
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+50); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto L_7cce; }
  CYC(b_+50, b_+52);
  RET(b_+52); return;
}

