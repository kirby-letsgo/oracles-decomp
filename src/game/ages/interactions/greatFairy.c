#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(greatFairy_initialize), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(greatFairy_initialize), (from), (to), true)

void greatFairy_initialize_hook(GB *gb);
void greatFairy_initialize__afterCall7966_hook(GB *gb);
void greatFairy_subid0__afterCall7866_hook(GB *gb);
void greatFairy_subid1__afterCall78b2_hook(GB *gb);

static uint16_t great_fairy_jump_table(GB *gb) {
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

static void great_fairy_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void great_fairy_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

static void great_fairy_initialize_tail(GB *gb, uint16_t sp0_) {
  BASE(greatFairy_initialize);
  CALL_C(b_+3, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+12); SET_HL(b_+22);
  CYC(b_+12, b_+13); great_fairy_add_double_index(gb, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+14, b_+15); H = mem_rd(gb, HL);
  CYC(b_+15, b_+16); L = A;
  CALL_C(b_+16, interactionSetScript_hook, SYM(interactionSetScript), b_+19);
  CYC(b_+19, b_+22); interactionIncState_hook(gb);
}

void greatFairy_initialize__afterCall7966_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  great_fairy_initialize_tail(gb, sp0_);
}

void greatFairy_initialize_hook(GB *gb) {
  BASE(greatFairy_initialize);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  greatFairy_initialize__afterCall7966_hook(gb);
}

static void great_fairy_subid0_state1(GB *gb, uint16_t sp0_) {
  BASE(greatFairy_subid0);
  CALL_C(b_+22, returnIfScrollMode01Unset_hook, SYM(returnIfScrollMode01Unset), b_+25);
  CALL_C(b_+25, interactionRunScript_hook, SYM(interactionRunScript), b_+28);
  if (F & FC) {
    CYCT(b_+28, b_+31); interactionDeleteAndUnmarkSolidPosition_hook(gb);
    return;
  }
  CYC(b_+28, b_+31);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+35, b_+36); ret_effect(gb);
    return;
  }
  CYC(b_+35, b_+36);
  CALL_C(b_+36, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+39);
  CYC(b_+39, b_+42); A = W8(wFrameCounter);
  CYC(b_+42, b_+44); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+44, b_+45); ret_effect(gb);
    return;
  }
  CYC(b_+44, b_+45);
  CYC(b_+45, b_+48); A = W8(wFrameCounter);
  CYC(b_+48, b_+50); alu_and(gb, 0x38);
  CYC(b_+50, b_+52); A = alu_swap(gb, A);
  CYC(b_+52, b_+53); alu_rlca(gb);
  CYC(b_+53, b_+56); SET_HL(b_+63);
  CYC(b_+56, b_+57); great_fairy_add_a_to_hl(gb, b_+57);
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+61); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);
  CYC(b_+62, b_+63); ret_effect(gb);
}

void greatFairy_subid0__afterCall7866_hook(GB *gb) {
  BASE(greatFairy_subid0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+8, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+11);
  CYC(b_+11, b_+13); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0xf0);
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x06);
  CALL_C(b_+19, interactionRunScript_hook, SYM(interactionRunScript), b_+22);
  great_fairy_subid0_state1(gb, sp0_);
}

void greatFairy_subid0_hook(GB *gb) {
  BASE(greatFairy_subid0);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); great_fairy_subid0_state1(gb, sp0_);
    return;
  }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, greatFairy_initialize_hook, SYM(greatFairy_initialize), b_+8);
  greatFairy_subid0__afterCall7866_hook(gb);
}

static void great_fairy_update_sparkles_and_sound(GB *gb, uint16_t sp0_) {
  BASE(greatFairy_subid1);
  CYC(b_+126, b_+129); A = W8(wFrameCounter);
  CYC(b_+129, b_+131); alu_and(gb, 0x07);
  if (!(F & FZ)) {
    CYCT(b_+131, b_+132); ret_effect(gb);
    return;
  }
  CYC(b_+131, b_+132);
  CYC(b_+132, b_+135); SET_BC(0x8402);
  CALL_C(b_+135, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+138);
  CYC(b_+138, b_+141); A = W8(wFrameCounter);
  CYC(b_+141, b_+143); alu_and(gb, 0x1f);
  CYC(b_+143, b_+145); A = 0x83;
  if (F & FZ) CALL_C_CC(b_+145, playSound_b00_hook, SYM(playSound_b00), b_+148);
  else CYC(b_+145, b_+148);
  CYC(b_+148, b_+149); ret_effect(gb);
}

static void great_fairy_subid1_state1(GB *gb, uint16_t sp0_) {
  BASE(greatFairy_subid1);
  CALL_C(b_+62, interactionAnimate_hook, SYM(interactionAnimate), b_+65);
  CYC(b_+65, b_+67); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  CYC(b_+68, b_+69); push_effect(gb, b_+69);
  do { uint16_t jt_ = (great_fairy_jump_table(gb));
    if (jt_ == b_+77) { goto substate0; }
    else if (jt_ == b_+100) { goto substate1; }
    else if (jt_ == b_+149) { goto substate2; }
    else if (jt_ == b_+168) { goto substate3; }
    else { HANDOFF(HL); }
  } while (0);

substate0:
  CALL_C(b_+77, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+80);
  if (!(F & FZ)) {
    CYCT(b_+80, b_+81); ret_effect(gb);
    return;
  }
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+83); mem_wr(gb, HL, 0x40);
  CYC(b_+83, b_+85); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+85, b_+87); mem_wr(gb, HL, 0x08);
  CYC(b_+87, b_+89); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+89, b_+91); mem_wr(gb, HL, 0x78);
  CYC(b_+91, b_+94); SET_BC((SYM(interactionCodeac) + 17));
  CALL_C(b_+94, showText_hook, SYM(showText), b_+97);
  CYC(b_+97, b_+100); interactionIncSubstate_hook(gb);
  return;

substate1:
  CALL_C(b_+100, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+103);
  CALL_C(b_+103, objectApplySpeed_hook, SYM(objectApplySpeed), b_+106);
  CALL_C(b_+106, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+109);
  if (!(F & FZ)) {
    CYCT(b_+109, b_+111);
    goto update_sparkles;
  }
  CYC(b_+109, b_+111);
  CYC(b_+111, b_+113); mem_wr(gb, HL, 0x02);
  CYC(b_+113, b_+115); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+115, b_+116); A = mem_rd(gb, HL);
  CYC(b_+116, b_+117); A = alu_inc8(gb, A);
  CYC(b_+117, b_+119); alu_and(gb, 0x1f);
  CYC(b_+119, b_+120); mem_wr(gb, HL, A);
  CALL_C(b_+120, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+123);
  if (F & FZ) {
    CYCT(b_+123, b_+126); interactionIncSubstate_hook(gb);
    return;
  }
  CYC(b_+123, b_+126);

update_sparkles:
  great_fairy_update_sparkles_and_sound(gb, sp0_);
  return;

substate2:
  CYC(b_+149, b_+152); push_effect(gb, b_+152);
  great_fairy_update_sparkles_and_sound(gb, sp0_);
  CYC(b_+152, b_+153); H = D;
  CYC(b_+153, b_+155); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+155, b_+156); A = mem_rd(gb, HL);
  CYC(b_+156, b_+158); alu_sub(gb, 0x02);
  CYC(b_+158, b_+159); mem_wr(gb, HL, A);
  CYC(b_+159, b_+161); alu_cp(gb, 0xb0);
  if (!(F & FC)) {
    CYCT(b_+161, b_+162); ret_effect(gb);
    return;
  }
  CYC(b_+161, b_+162);
  CALL_C(b_+162, fadeoutToWhite_hook, SYM(fadeoutToWhite), b_+165);
  CYC(b_+165, b_+168); interactionIncSubstate_hook(gb);
  return;

substate3:
  CYC(b_+168, b_+171); A = W8(wPaletteThread_mode);
  CYC(b_+171, b_+172); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+172, b_+173); ret_effect(gb);
    return;
  }
  CYC(b_+172, b_+173);
  CYC(b_+173, b_+175); A = 0x1e;
  CYC(b_+175, b_+178); W8(wCutsceneTrigger) = A;
  CYC(b_+178, b_+181); interactionDelete_hook(gb);
}

void greatFairy_subid1__afterCall78b2_hook(GB *gb) {
  BASE(greatFairy_subid1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+13, objectSetVisiblec1_hook, SYM(objectSetVisiblec1), b_+16);
  CALL_C(b_+16, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+19);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xf0);
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+25, b_+27); A = 0xb4;
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x02);
  CYC(b_+30, b_+33); SET_BC(0x8404);
  CALL_C(b_+33, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+36);
  CYC(b_+36, b_+38); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x78);
  CYC(b_+40, b_+42); B = 0x00;

  do {
    CYC(b_+42, b_+43); push_effect(gb, BC);
    CYC(b_+43, b_+46); SET_BC(0x840a);
    CALL_C(b_+46, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+49);
    CYC(b_+49, b_+50); SET_BC(pop_effect(gb));
    CYC(b_+50, b_+52); L = INTERACTION_BASE + OBJ_ANGLE;
    CYC(b_+52, b_+53); mem_wr(gb, HL, B);
    CYC(b_+53, b_+54); A = B;
    CYC(b_+54, b_+56); alu_add(gb, 0x04);
    CYC(b_+56, b_+57); B = A;
    CYC(b_+57, b_+59); alu_bit(gb, 5, A);
    if (!(F & FZ)) {
      CYC(b_+59, b_+61);
      break;
    }
    CYCT(b_+59, b_+61);
  } while (true);
  CYC(b_+61, b_+62); ret_effect(gb);
}

void greatFairy_subid1_hook(GB *gb) {
  BASE(greatFairy_subid1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) {
    CYCT(b_+3, b_+5); great_fairy_subid1_state1(gb, sp0_);
    return;
  }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x8a;
  CALL_C(b_+7, playSound_b00_hook, SYM(playSound_b00), b_+10);
  CALL_C(b_+10, greatFairy_initialize_hook, SYM(greatFairy_initialize), b_+13);
  greatFairy_subid1__afterCall78b2_hook(gb);
}

void interactionCoded5_hook(GB *gb) {
  BASE(interactionCodecd);
  CYC(b_+61, b_+63); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+63, b_+64); A = mem_rd(gb, DE);
  CYC(b_+64, b_+65); push_effect(gb, b_+65);
  uint16_t target = great_fairy_jump_table(gb);
  if (target == SYM(greatFairy_subid0)) { greatFairy_subid0_hook(gb); return; }
  greatFairy_subid1_hook(gb); return; // target == 0x78a5
}
