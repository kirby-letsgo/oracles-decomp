#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/horonDog.s.
// INTERAC_HORON_DOG

static uint16_t horonDog_jump_table(GB *gb) {
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

// interactionCode82@func_7a09
static void horonDog_func_7a09(GB *gb) {
  BASE(interactionCode82);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+87, s_interactionAnimate, SYM(interactionAnimate), b_+90);
  CYC(b_+90, b_+93);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}

// interactionCode82@func_7a06
static void horonDog_func_7a06(GB *gb) {
  BASE(interactionCode82);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+84, s_func_7a93_hook, SYM(func_7a93), b_+87);
  CALL_C(b_+87, s_interactionAnimate, SYM(interactionAnimate), b_+90);
  CYC(b_+90, b_+93);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}

// INTERAC_HORON_DOG
void s_interactionCode82_hook(GB *gb) {
  BASE(interactionCode82);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CALL_C(b_+14, s_func_7a99_hook, SYM(func_7a99), b_+17);
  CYC(b_+17, b_+19); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x5a);
  CALL_C(b_+21, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+24);
  CYC(b_+24, b_+27);
  TAIL(objectSetVisible82);
state1:
  CYC(b_+27, b_+29); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); push_effect(gb, b_+31);
  do { uint16_t jt_ = (horonDog_jump_table(gb));
    if (jt_ == b_+43) goto substate0;
    if (jt_ == b_+93) goto substate1;
    if (jt_ == b_+111) goto substate2;
    if (jt_ == b_+143) goto substate3;
    if (jt_ == b_+176) goto substate4;
    if (jt_ == b_+189) goto substate5;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_L(b_+43, horonDog_func_7a09, b_+46);
  CALL_C(b_+46, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+49);
  CYC(b_+49, b_+51); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+51, b_+52); A = mem_rd(gb, HL);
  CYC(b_+52, b_+54); alu_add(gb, 0x08);
  CYC(b_+54, b_+55); B = A;
  CYC(b_+55, b_+56); E = L;
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+58); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+58, b_+60); goto func_79fc; }
  CYC(b_+58, b_+60);
  CALL_C(b_+60, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+63);
  CYC(b_+63, b_+65); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+65, b_+67); mem_wr(gb, HL, 0x14);
  CYC(b_+67, b_+69); A = 0x06;
  CALL_C(b_+69, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+72);
  CYC(b_+72, b_+74);
  goto func_7a06;
func_79fc:
  CYC(b_+74, b_+76); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+76, b_+77); A = mem_rd(gb, DE);
  CYC(b_+77, b_+78); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+78, b_+81); TAIL_S(func_7a93); }
  CYC(b_+78, b_+81);
  CYC(b_+81, b_+84);
  TAIL(interactionDecCounter1);
func_7a06:
  CALL_C(b_+84, s_func_7a93_hook, SYM(func_7a93), b_+87);
  CALL_C(b_+87, s_interactionAnimate, SYM(interactionAnimate), b_+90);
  CYC(b_+90, b_+93);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
substate1:
  CALL_L(b_+93, horonDog_func_7a06, b_+96);
  CALL_C(b_+96, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+99);
  if (!(F & FZ)) { RET_TAKEN(b_+99); return; }
  CYC(b_+99, b_+100);
  CYC(b_+100, b_+102); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+102, b_+104); mem_wr(gb, HL, 0x18);
  CYC(b_+104, b_+106); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+106, b_+108); mem_wr(gb, HL, 0x28);
  CYC(b_+108, b_+111);
  TAIL(interactionIncSubstate);
substate2:
  CALL_L(b_+111, horonDog_func_7a06, b_+114);
  CALL_C(b_+114, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+117);
  CYC(b_+117, b_+119); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+119, b_+120); A = mem_rd(gb, HL);
  CYC(b_+120, b_+122); alu_add(gb, 0x04);
  CALL_C(b_+122, s_func_7a9f_hook, SYM(func_7a9f), b_+125);
  if (!(F & FZ)) { CYCT(b_+125, b_+128); TAIL(objectApplySpeed); }
  CYC(b_+125, b_+128);
  CALL_C(b_+128, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+131);
  CYC(b_+131, b_+133); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+133, b_+135); mem_wr(gb, HL, 0x0c);
  CYC(b_+135, b_+137); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+137, b_+138); alu_xor(gb, A);
  CYC(b_+138, b_+139); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+139, b_+140); mem_wr(gb, HL, A);
  CYC(b_+140, b_+143);
  TAIL_S(func_7aa5);
substate3:
  CALL_L(b_+143, horonDog_func_7a09, b_+146);
  CALL_C(b_+146, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+149);
  if (F & FZ) { CYCT(b_+149, b_+152); goto func_7a4f; }
  CYC(b_+149, b_+152);
  CALL_C(b_+152, s_objectApplySpeed, SYM(objectApplySpeed), b_+155);
  CYC(b_+155, b_+157);
  goto L_7a5f;
func_7a4f:
  CALL_C(b_+157, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+160);
  CYC(b_+160, b_+162); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+162, b_+164); mem_wr(gb, HL, 0x1e);
  CYC(b_+164, b_+166); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+166, b_+168); mem_wr(gb, HL, 0x08);
  CYC(b_+168, b_+170); A = 0x05;
  CALL_C(b_+170, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+173);
L_7a5f:
  CYC(b_+173, b_+176);
  TAIL_S(func_7aa5);
substate4:
  CALL_L(b_+176, horonDog_func_7a09, b_+179);
  CALL_C(b_+179, s_func_7aa5_hook, SYM(func_7aa5), b_+182);
  CALL_C(b_+182, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+185);
  if (!(F & FZ)) { RET_TAKEN(b_+185); return; }
  CYC(b_+185, b_+186);
  CYC(b_+186, b_+189);
  TAIL(interactionIncSubstate);
substate5:
  CALL_L(b_+189, horonDog_func_7a06, b_+192);
  CALL_C(b_+192, s_func_7aa5_hook, SYM(func_7aa5), b_+195);
  CALL_C(b_+195, s_objectApplySpeed, SYM(objectApplySpeed), b_+198);
  CYC(b_+198, b_+200); E = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+200, b_+201); A = mem_rd(gb, DE);
  CALL_C(b_+201, s_func_7a9f_hook, SYM(func_7a9f), b_+204);
  if (!(F & FZ)) { RET_TAKEN(b_+204); return; }
  CYC(b_+204, b_+205);
  CYC(b_+205, b_+208); SET_HL(wccd4);
  CYC(b_+208, b_+210); mem_wr(gb, HL, 0x02);
  CYC(b_+210, b_+211); H = D;
  CYC(b_+211, b_+213); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+213, b_+215); mem_wr(gb, HL, 0x00);
  CYC(b_+215, b_+217); L = INTERACTION_BASE + OBJ_Z;
  CYC(b_+217, b_+218); alu_xor(gb, A);
  CYC(b_+218, b_+219); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+219, b_+220); mem_wr(gb, HL, A);
  CYC(b_+220, b_+222); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+222, b_+224); mem_wr(gb, HL, 0x3c);
  RET(b_+224); return;
}

void s_func_7a93_hook(GB *gb) {
  BASE(func_7a93);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  s_func_7a99_hook(gb); return; // falls through
}

void s_func_7a99_hook(GB *gb) {
  BASE(func_7a99);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(IO_LCDC);
  CYC(b_+3, b_+6);
  TAIL(objectSetSpeedZ);
}

void s_func_7a9f_hook(GB *gb) {
  BASE(func_7a9f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); alu_cp(gb, B);
  RET(b_+5); return;
}

void s_func_7aa5_hook(GB *gb) {
  BASE(func_7aa5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x40;
  CALL_C(b_+2, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x18);
  CYC(b_+10, b_+12); C = 0x07;
  if (!(F & FZ)) { CYCT(b_+12, b_+14); goto L_7ab5; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+16); C = 0xfb;
L_7ab5:
  CYC(b_+16, b_+18); B = 0xfe;
  CYC(b_+18, b_+21);
  TAIL(objectCopyPositionWithOffset);
}

