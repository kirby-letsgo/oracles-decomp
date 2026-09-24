#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/trampoline.s.
// INTERAC_TRAMPOLINE

static uint16_t trampoline_jump_table(GB *gb) {
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

// INTERAC_TRAMPOLINE
void s_interactionCode7c_hook(GB *gb) {
  BASE(interactionCode7c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectSetPriorityRelativeToLink, SYM(objectSetPriorityRelativeToLink), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, b_+7);
  do { uint16_t jt_ = (trampoline_jump_table(gb));
    if (jt_ == b_+17) goto state0;
    if (jt_ == b_+36) goto state1;
    if (jt_ == b_+47) goto state2;
    if (jt_ == b_+185) goto state3;
    if (jt_ == b_+204) goto state4;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+17, b_+19); A = 0x01;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CALL_C(b_+20, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+23);
  CYC(b_+23, b_+25); A = 0x07;
  CALL_C(b_+25, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+28);
  CYC(b_+28, b_+30); A = 0x14;
  CYC(b_+30, b_+32); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+32, b_+33); mem_wr(gb, HL, A);
  CYC(b_+33, b_+36);
  TAIL(objectSetVisible82);
state1:
  CALL_C(b_+36, s_returnIfScrollMode01Unset, SYM(returnIfScrollMode01Unset), b_+39);
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+41, b_+43); A = 0x02;
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+47);
  TAIL_S(func_76d4);
state2:
  CYC(b_+47, b_+50); A = mem_rd(gb, w1Link_zh);
  CYC(b_+50, b_+51); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto func_7677; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+54); alu_xor(gb, A);
  CYC(b_+54, b_+56); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+56, b_+57); mem_wr(gb, DE, A);
  CYC(b_+57, b_+59); A = 0x07;
  CALL_C(b_+59, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+62);
  CALL_C(b_+62, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+65);
  if (!(F & FC)) { CYCT(b_+65, b_+67); goto func_7671; }
  CYC(b_+65, b_+67);
  CALL_C(b_+67, s_objectCheckLinkPushingAgainstCenter, SYM(objectCheckLinkPushingAgainstCenter), b_+70);
  if (!(F & FC)) { CYCT(b_+70, b_+72); goto func_7671; }
  CYC(b_+70, b_+72);
  CYC(b_+72, b_+74); A = 0x01;
  CYC(b_+74, b_+77); mem_wr(gb, wForceLinkPushAnimation, A);
  CALL_C(b_+77, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+80);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; }
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+83); C = 0x28;
  CALL_C(b_+83, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+86);
  CYC(b_+86, b_+88); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+88, b_+90); alu_xor(gb, 0x04);
  CYC(b_+90, b_+91); mem_wr(gb, DE, A);
  CALL_C(b_+91, s_interactionCheckAdjacentTileIsSolid_viaDirection, SYM(interactionCheckAdjacentTileIsSolid_viaDirection), b_+94);
  if (!(F & FZ)) { RET_TAKEN(b_+94); return; }
  CYC(b_+94, b_+95);
  CYC(b_+95, b_+96); H = D;
  CYC(b_+96, b_+98); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+98, b_+99); A = mem_rd(gb, HL);
  CYC(b_+99, b_+100); alu_add(gb, A);
  CYC(b_+100, b_+101); alu_add(gb, A);
  CYC(b_+101, b_+103); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+103, b_+104); mem_wr(gb, HL, A);
  CYC(b_+104, b_+106); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+106, b_+108); mem_wr(gb, HL, 0x20);
  CYC(b_+108, b_+110); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+110, b_+111); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+111, s_func_76e0_hook, SYM(func_76e0), b_+114);
  CYC(b_+114, b_+116); A = 0x71;
  CYC(b_+116, b_+119);
  TAIL(playSound_b00);
func_7671:
  CYC(b_+119, b_+121); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+121, b_+123); A = 0x1e;
  CYC(b_+123, b_+124); mem_wr(gb, DE, A);
  RET(b_+124); return;
func_7677:
  CYC(b_+125, b_+127); A = 0x0a;
  CALL_C(b_+127, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+130);
  CYC(b_+130, b_+133); A = mem_rd(gb, w1Link_yh);
  CYC(b_+133, b_+134); B = A;
  CYC(b_+134, b_+137); A = mem_rd(gb, w1Link_xh);
  CYC(b_+137, b_+138); C = A;
  CALL_C(b_+138, s_interactionCheckContainsPoint, SYM(interactionCheckContainsPoint), b_+141);
  if (!(F & FC)) { RET_TAKEN(b_+141); return; }
  CYC(b_+141, b_+142);
  CYC(b_+142, b_+145); A = mem_rd(gb, w1Link_zh);
  CYC(b_+145, b_+146); B = A;
  CYC(b_+146, b_+148); alu_cp(gb, 0xe8);
  if (!(F & FC)) { CYCT(b_+148, b_+150); goto L_7693; }
  CYC(b_+148, b_+150);
  CYC(b_+150, b_+152); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+152, b_+153); mem_wr(gb, DE, A);
L_7693:
  CYC(b_+153, b_+154); A = B;
  CYC(b_+154, b_+156); alu_cp(gb, 0xfc);
  if (F & FC) { RET_TAKEN(b_+156); return; }
  CYC(b_+156, b_+157);
  CYC(b_+157, b_+159); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+159, b_+160); A = mem_rd(gb, DE);
  CYC(b_+160, b_+161); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+161, b_+163); goto L_76a5; }
  CYC(b_+161, b_+163);
  CYC(b_+163, b_+166); SET_HL(SYM(trampoline_bounce));
  CYC(b_+166, b_+168); E = 0x15;
  CALL_C(b_+168, s_interBankCall, SYM(interBankCall), b_+171);
L_76a5:
  CYC(b_+171, b_+173); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+173, b_+175); A = 0x04;
  CYC(b_+175, b_+176); mem_wr(gb, DE, A);
  CYC(b_+176, b_+177); alu_xor(gb, A);
  CALL_C(b_+177, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+180);
  CYC(b_+180, b_+182); A = 0x53;
  CYC(b_+182, b_+185);
  TAIL(playSound_b00);
state3:
  CALL_C(b_+185, s_objectApplySpeed, SYM(objectApplySpeed), b_+188);
  CALL_C(b_+188, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+191);
  CALL_C(b_+191, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+194);
  if (!(F & FZ)) { RET_TAKEN(b_+194); return; }
  CYC(b_+194, b_+195);
  CYC(b_+195, b_+197); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+197, b_+198); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+198, b_+200); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+200, b_+202); mem_wr(gb, HL, 0x1e);
  CYC(b_+202, b_+204);
  TAIL_S(func_76d4);
state4:
  CALL_C(b_+204, s_interactionAnimate, SYM(interactionAnimate), b_+207);
  CYC(b_+207, b_+209); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+209, b_+210); A = mem_rd(gb, DE);
  CYC(b_+210, b_+211); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+211); return; }
  CYC(b_+211, b_+212);
  CYC(b_+212, b_+214); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+214, b_+216); A = 0x02;
  CYC(b_+216, b_+217); mem_wr(gb, DE, A);
  RET(b_+217); return;
}

void s_func_76d4_hook(GB *gb) {
  BASE(func_76d4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x07);
  CYC(b_+8, b_+9); H = alu_dec8(gb, H);
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x14);
  RET(b_+11); return;
}

void s_func_76e0_hook(GB *gb) {
  BASE(func_76e0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  CYC(b_+7, b_+8); H = alu_dec8(gb, H);
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x00);
  RET(b_+10); return;
}

