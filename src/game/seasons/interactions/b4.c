#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/b4.s.
// INTERAC_b4

static uint16_t b4_jump_table(GB *gb) {
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

static void b4_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_b4
void s_interactionCodeb4_hook(GB *gb) {
  BASE(interactionCodeb4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (b4_jump_table(gb));
    if (jt_ == SYM(twinrovaWitches_state0) && hook_is(gb, SYM(twinrovaWitches_state0), s_twinrovaWitches_state0_hook)) { s_twinrovaWitches_state0_hook(gb); return; }
    if (jt_ == SYM(twinrovaWitches_state1) && hook_is(gb, SYM(twinrovaWitches_state1), s_twinrovaWitches_state1_hook)) { s_twinrovaWitches_state1_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_twinrovaWitches_state0_hook(GB *gb) {
  BASE(twinrovaWitches_state0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (b4_jump_table(gb));
    if (jt_ == b_+20) goto subid0;
    if (jt_ == b_+32) goto subid2;
    if (jt_ == b_+48) goto subid4;
    if (jt_ == b_+68) goto subid5;
    if (jt_ == b_+84) goto subid6;
    if (jt_ == b_+99) goto subid7;
    HANDOFF(HL);
  } while (0);
subid0:
  CALL_C(b_+20, s_twinrovaWitches_state0Init_hook, SYM(twinrovaWitches_state0Init), b_+23);
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CALL_C(b_+26, s_func_7266_hook, SYM(func_7266), b_+29);
  CYC(b_+29, b_+32);
  TAIL_S(twinrovaWitches_state1);
subid2:
  CALL_C(b_+32, s_twinrovaWitches_state0Init_hook, SYM(twinrovaWitches_state0Init), b_+35);
  CYC(b_+35, b_+37); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0xfb);
  CYC(b_+39, b_+41); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  CALL_C(b_+42, s_func_7266_hook, SYM(func_7266), b_+45);
  CYC(b_+45, b_+48);
  TAIL_S(twinrovaWitches_state1);
subid4:
  CALL_C(b_+48, s_twinrovaWitches_state0Init_hook, SYM(twinrovaWitches_state0Init), b_+51);
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0xf0);
  CYC(b_+55, b_+57); A = 0x04;
  CALL_C(b_+57, s_func_7266_hook, SYM(func_7266), b_+60);
  CYC(b_+60, b_+62); A = 0x04;
  CALL_C(b_+62, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+65);
  CYC(b_+65, b_+68);
  TAIL_S(twinrovaWitches_state1);
subid5:
  CALL_C(b_+68, s_twinrovaWitches_state0Init_hook, SYM(twinrovaWitches_state0Init), b_+71);
  CYC(b_+71, b_+73); A = 0x04;
  CALL_C(b_+73, s_func_7266_hook, SYM(func_7266), b_+76);
  CYC(b_+76, b_+78); A = 0x01;
  CALL_C(b_+78, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+81);
  CYC(b_+81, b_+84);
  TAIL_S(twinrovaWitches_state1);
subid6:
  CALL_C(b_+84, s_twinrovaWitches_state0Init_hook, SYM(twinrovaWitches_state0Init), b_+87);
  CYC(b_+87, b_+89); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x00);
  CYC(b_+91, b_+93); A = 0x05;
  CALL_C(b_+93, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+96);
  CYC(b_+96, b_+99);
  TAIL_S(twinrovaWitches_state1);
subid7:
  CALL_C(b_+99, s_twinrovaWitches_state0Init_hook, SYM(twinrovaWitches_state0Init), b_+102);
  CYC(b_+102, b_+104); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+104, b_+106); mem_wr(gb, HL, 0x00);
  CYC(b_+106, b_+108); A = 0x06;
  CALL_C(b_+108, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+111);
  CYC(b_+111, b_+114);
  TAIL_S(twinrovaWitches_state1);
}

void s_twinrovaWitches_state0Init_hook(GB *gb) {
  BASE(twinrovaWitches_state0Init);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, s_objectSetVisiblec0, SYM(objectSetVisiblec0), b_+6);
  CALL_C(b_+6, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+9);
  CALL_C(b_+9, s_twinrovaWitches_getOamFlags_hook, SYM(twinrovaWitches_getOamFlags), b_+12);
  CALL_C(b_+12, s_interactionIncState, SYM(interactionIncState), b_+15);
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x50);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xf8);
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0xff);
  RET(b_+27); return;
}

// twinrovaWitches_state1@animate
static void b4_animate(GB *gb) {
  BASE(twinrovaWitches_state1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+247, b_+250);
  TAIL(interactionAnimate);
}

// twinrovaWitches_state1@subid3@func_7126
static void b4_subid3_func_7126(GB *gb) {
  BASE(twinrovaWitches_state1);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+102, b_+105); SET_HL(wTmpcfc0 + 0x06);
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+106, b_+107); A = mem_rd(gb, HL);
  CYC(b_+107, b_+109); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+109); return; }
  CYC(b_+109, b_+110);
  CYC(b_+110, b_+112); mem_wr(gb, HL, 0x00);
  CYC(b_+112, b_+115); SET_HL(wTmpcfc0);
  CYC(b_+115, b_+117); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  RET(b_+117); return;
}

void s_twinrovaWitches_state1_hook(GB *gb) {
  BASE(twinrovaWitches_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (b4_jump_table(gb));
    if (jt_ == b_+20) goto subid0;
    if (jt_ == b_+214) goto subid4;
    if (jt_ == b_+244) goto subid6;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); push_effect(gb, b_+24);
  do { uint16_t jt_ = (b4_jump_table(gb));
    if (jt_ == b_+34) goto subid3_substate0;
    if (jt_ == b_+85) goto subid3_substate1;
    if (jt_ == b_+118) goto subid3_substate2;
    if (jt_ == b_+149) goto subid3_substate3;
    if (jt_ == b_+211) goto subid3_substate4;
    HANDOFF(HL);
  } while (0);
subid3_substate0:
  CALL_C(b_+34, s_func_71ee_hook, SYM(func_71ee), b_+37);
  CALL_C(b_+37, s_func_7220_hook, SYM(func_7220), b_+40);
  CALL_C(b_+40, s_func_720a_hook, SYM(func_720a), b_+43);
  if (F & FC) CALL_C_CC(b_+43, s_func_7232_hook, SYM(func_7232), b_+46);
  else CYC(b_+43, b_+46);
  if (!(F & FC)) { CYCT(b_+46, b_+49); goto animate; }
  CYC(b_+46, b_+49);
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x01);
  CYC(b_+54, b_+56); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x28);
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+60, b_+61); A = mem_rd(gb, HL);
  CYC(b_+61, b_+63); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+63, b_+65); goto subid3_func_7105; }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); A = 0x00;
  CYC(b_+67, b_+69);
  goto L_710f;
subid3_func_7105:
  CYC(b_+69, b_+71); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+71, b_+73); goto subid3_func_710d; }
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+75); A = 0x01;
  CYC(b_+75, b_+77);
  goto L_710f;
subid3_func_710d:
  CYC(b_+77, b_+79); A = 0x02;
L_710f:
  CALL_C(b_+79, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+82);
  CYC(b_+82, b_+85);
  goto animate;
subid3_substate1:
  CALL_C(b_+85, s_seasonsFunc_0a_71ce_hook, SYM(seasonsFunc_0a_71ce), b_+88);
  CALL_L(b_+88, b4_animate, b_+91);
  CALL_C(b_+91, s_interactionDecCounter2, SYM(interactionDecCounter2), b_+94);
  if (!(F & FZ)) { RET_TAKEN(b_+94); return; }
  CYC(b_+94, b_+95);
  CYC(b_+95, b_+97); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+97, b_+98); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+98, b_+100); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0x28);
  CYC(b_+102, b_+105); SET_HL(wTmpcfc0 + 0x06);
  CYC(b_+105, b_+106); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+106, b_+107); A = mem_rd(gb, HL);
  CYC(b_+107, b_+109); alu_cp(gb, 0x02);
  if (!(F & FZ)) { RET_TAKEN(b_+109); return; }
  CYC(b_+109, b_+110);
  CYC(b_+110, b_+112); mem_wr(gb, HL, 0x00);
  CYC(b_+112, b_+115); SET_HL(wTmpcfc0);
  CYC(b_+115, b_+117); mem_wr(gb, HL, mem_rd(gb, HL) | 0x01);
  RET(b_+117); return;
subid3_substate2:
  CALL_C(b_+118, s_seasonsFunc_0a_71ce_hook, SYM(seasonsFunc_0a_71ce), b_+121);
  CALL_L(b_+121, b4_animate, b_+124);
  CYC(b_+124, b_+127); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+127, b_+129); alu_bit(gb, 0, A);
  if (!(F & FZ)) { RET_TAKEN(b_+129); return; }
  CYC(b_+129, b_+130);
  CALL_C(b_+130, s_interactionDecCounter2, SYM(interactionDecCounter2), b_+133);
  if (!(F & FZ)) { RET_TAKEN(b_+133); return; }
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+136); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+136, b_+137); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+137, b_+139); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+139, b_+141); mem_wr(gb, HL, 0xff);
  CYC(b_+141, b_+143); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+143, b_+144); A = mem_rd(gb, HL);
  CYC(b_+144, b_+146); alu_add(gb, 0x04);
  CYC(b_+146, b_+149);
  TAIL_S(func_7266);
subid3_substate3:
  CYC(b_+149, b_+151); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+151, b_+152); A = mem_rd(gb, DE);
  CYC(b_+152, b_+154); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+154, b_+156); goto L_7167; }
  CYC(b_+154, b_+156);
subid3_func_715c:
  CALL_C(b_+156, s_func_71ee_hook, SYM(func_71ee), b_+159);
  CALL_C(b_+159, s_func_720a_hook, SYM(func_720a), b_+162);
  if (F & FC) CALL_C_CC(b_+162, s_func_7232_hook, SYM(func_7232), b_+165);
  else CYC(b_+162, b_+165);
  if (F & FC) { CYCT(b_+165, b_+167); goto L_717a; }
  CYC(b_+165, b_+167);
L_7167:
  CALL_C(b_+167, s_func_71ee_hook, SYM(func_71ee), b_+170);
  CYC(b_+170, b_+172); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+172, b_+173); A = mem_rd(gb, DE);
  CYC(b_+173, b_+175); alu_cp(gb, 0x04);
  if (!(F & FZ)) CALL_C_CC(b_+175, s_func_7220_hook, SYM(func_7220), b_+178);
  else CYC(b_+175, b_+178);
  CALL_C(b_+178, s_func_720a_hook, SYM(func_720a), b_+181);
  if (F & FC) CALL_C_CC(b_+181, s_func_7232_hook, SYM(func_7232), b_+184);
  else CYC(b_+181, b_+184);
  if (!(F & FC)) { CYCT(b_+184, b_+186); goto animate; }
  CYC(b_+184, b_+186);
L_717a:
  CYC(b_+186, b_+188); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+188, b_+189); A = mem_rd(gb, DE);
  CYC(b_+189, b_+191); alu_cp(gb, 0x02);
  if (F & FC) { CYCT(b_+191, b_+193); goto L_7185; }
  CYC(b_+191, b_+193);
  CYC(b_+193, b_+195); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+195, b_+197); goto L_718b; }
  CYC(b_+195, b_+197);
L_7185:
  CALL_L(b_+197, b4_subid3_func_7126, b_+200);
  CYC(b_+200, b_+203);
  TAIL(interactionDelete);
L_718b:
  CALL_L(b_+203, b4_subid3_func_7126, b_+206);
  CYC(b_+206, b_+207); H = D;
  CYC(b_+207, b_+209); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+209, b_+210); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+210); return;
subid3_substate4:
  CYC(b_+211, b_+214);
  goto animate;
subid4:
  CYC(b_+214, b_+216); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+216, b_+217); A = mem_rd(gb, DE);
  CYC(b_+217, b_+218); push_effect(gb, b_+218);
  do { uint16_t jt_ = (b4_jump_table(gb));
    if (jt_ == b_+222) goto subid5_substate0;
    if (jt_ == b_+242) goto subid5_substate1;
    HANDOFF(HL);
  } while (0);
subid5_substate0:
  CALL_C(b_+222, s_seasonsFunc_0a_71ce_hook, SYM(seasonsFunc_0a_71ce), b_+225);
  CALL_L(b_+225, b4_animate, b_+228);
  CYC(b_+228, b_+231); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+231, b_+233); alu_bit(gb, 0, A);
  if (F & FZ) { RET_TAKEN(b_+233); return; }
  CYC(b_+233, b_+234);
  CALL_C(b_+234, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+237);
  CYC(b_+237, b_+239); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+239, b_+241); mem_wr(gb, HL, 0xff);
  RET(b_+241); return;
subid5_substate1:
  CYC(b_+242, b_+244);
  goto subid3_func_715c;
subid6:
  CYC(b_+244, b_+247);
  goto animate;
animate:
  CYC(b_+247, b_+250);
  TAIL(interactionAnimate);
}

void s_twinrovaWitches_getOamFlags_hook(GB *gb) {
  BASE(twinrovaWitches_getOamFlags);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+12 /* @oamFlagsData */);
  CYC(b_+6, b_+7); b4_add_a_to_hl(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  RET(b_+11); return;
}

void s_seasonsFunc_0a_71ce_hook(GB *gb) {
  BASE(seasonsFunc_0a_71ce);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = mem_rd(gb, wFrameCounter);
  CYC(b_+9, b_+11); alu_and(gb, 0x38);
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+14); alu_rlca(gb);
  CYC(b_+14, b_+17); SET_HL(SYM(table_71e6));
  CYC(b_+17, b_+18); b4_add_a_to_hl(gb, b_+18);
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  RET(b_+23); return;
}

void s_func_71ee_hook(GB *gb) {
  BASE(func_71ee);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); H = A;
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+16); b4_add_a_to_hl(gb, b_+16);
  CYC(b_+16, b_+17); B = mem_rd(gb, HL);
  CYC(b_+17, b_+18); SET_HL(HL + 1);
  CYC(b_+18, b_+19); C = mem_rd(gb, HL);
  CALL_C(b_+19, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+28);
  TAIL(objectApplySpeed);
}

void s_func_720a_hook(GB *gb) {
  BASE(func_720a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_func_7253_hook, SYM(func_7253), b_+3);
  CYC(b_+3, b_+5); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+5, b_+6); A = mem_rd(gb, BC);
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+9); alu_add(gb, 0x01);
  CYC(b_+9, b_+11); alu_cp(gb, 0x05);
  if (!(F & FC)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); SET_BC(BC + 1);
  CYC(b_+13, b_+15); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+15, b_+16); A = mem_rd(gb, BC);
  CYC(b_+16, b_+17); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+17, b_+19); alu_add(gb, 0x01);
  CYC(b_+19, b_+21); alu_cp(gb, 0x05);
  RET(b_+21); return;
}

void s_func_7220_hook(GB *gb) {
  BASE(func_7220);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+8); alu_and(gb, 0x01);
  CYC(b_+8, b_+10); alu_xor(gb, 0x01);
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+12, b_+13); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A);
  CYC(b_+15, b_+18);
  TAIL(interactionSetAnimation);
}

void s_func_7232_hook(GB *gb) {
  BASE(func_7232);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_func_7242_hook, SYM(func_7242), b_+3);
  CYC(b_+3, b_+4); H = D;
  CYC(b_+4, b_+6); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+9, b_+10); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+10, b_+11); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+11); return; }
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x00);
  CYC(b_+14, b_+15); alu_scf(gb);
  RET(b_+15); return;
}

void s_func_7242_hook(GB *gb) {
  BASE(func_7242);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_func_7253_hook, SYM(func_7253), b_+3);
  CYC(b_+3, b_+5); L = INTERACTION_BASE + OBJ_Y;
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8); A = mem_rd(gb, BC);
  CYC(b_+8, b_+9); mem_wr(gb, HL, A);
  CYC(b_+9, b_+10); SET_BC(BC + 1);
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_X;
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); A = mem_rd(gb, BC);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  RET(b_+16); return;
}

void s_func_7253_hook(GB *gb) {
  BASE(func_7253);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); push_effect(gb, AF);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); B = A;
  CYC(b_+14, b_+15); SET_AF(pop_effect(gb));
  CALL_C(b_+15, s_addAToBc, SYM(addAToBc), b_+18);
  RET(b_+18); return;
}

void s_func_7266_hook(GB *gb) {
  BASE(func_7266);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_add(gb, A);
  CYC(b_+1, b_+2); alu_add(gb, A);
  CYC(b_+2, b_+5); SET_HL(SYM(table_7279));
  CYC(b_+5, b_+6); b4_add_a_to_hl(gb, b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  RET(b_+18); return;
}

