#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t raftwreck_jump_table(GB *gb) {
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

static void raftwreck_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void raftwreck_load_angle_and_counter_preset(GB *gb, uint16_t sp0_) {
  BASE(interactionCode64);
  CYC(b_+33, b_+35); B = 3;
  CYC(b_+35, b_+38); SET_HL((SYM(goronSubid00__state3Substate2) + 1));
  CYC(b_+38, b_+40); E = 0x0a;
  CALL_C(b_+40, interBankCall_hook, 0x008a, b_+43);
  CYC(b_+43, b_+44); A = B;
  CYC(b_+44, b_+45); alu_or(gb, A);
  CYC(b_+45, b_+46); ret_effect(gb);
}

static void raftwreck_read_spawn_data(GB *gb) {
  BASE(interactionCode64);
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); alu_add(gb, A);
  CYC(b_+51, b_+52); raftwreck_add_double_index(gb, b_+52);
  CYC(b_+52, b_+53); B = mem_rd(gb, HL);
  CYC(b_+53, b_+54); SET_HL(HL + 1);
  CYC(b_+54, b_+55); C = mem_rd(gb, HL);
  CYC(b_+55, b_+56); SET_HL(HL + 1);
  CYC(b_+56, b_+57); E = mem_rd(gb, HL);
  CYC(b_+57, b_+58); SET_HL(HL + 1);
  CYC(b_+58, b_+59); A = mem_rd(gb, HL);
  CYC(b_+59, b_+60); H = D;
  CYC(b_+60, b_+62); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+62, b_+63); mem_wr(gb, HL, A);
  CYC(b_+63, b_+64); ret_effect(gb);
}

static void raftwreck_finish_initialization(GB *gb, uint16_t sp0_) {
  BASE(interactionCode64);
  CALL_C(b_+30, objectSetVisible82_hook, SYM(objectSetVisible82), b_+33);
  raftwreck_load_angle_and_counter_preset(gb, sp0_);
}

void interactionCode64__afterCall73ec_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  raftwreck_finish_initialization(gb, sp0_);
}

static void raftwreck_state0(GB *gb, uint16_t sp0_) {
  BASE(interactionCode64);
  CALL_C(b_+8, interactionIncState_hook, SYM(interactionIncState), b_+11);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); push_effect(gb, b_+15);
  do { uint16_t jt_ = (raftwreck_jump_table(gb));
    if (jt_ == b_+27) {
      CALL_C(b_+27, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+30);
      raftwreck_finish_initialization(gb, sp0_);
      return;
    }
    else if (jt_ == b_+46) {
      CYC(b_+46, b_+47); ret_effect(gb);
      return;
    }
    else {
      hook_continue(gb, HL, sp0_);
      return;
    }
  } while (0);
}

static void raftwreck_run_subid00(GB *gb, uint16_t sp0_) {
  BASE(interactionCode64);
  CALL_C(b_+80, interactionAnimate_hook, SYM(interactionAnimate), b_+83);
  CALL_C(b_+83, objectApplySpeed_hook, SYM(objectApplySpeed), b_+86);
  CYC(b_+86, b_+88); alu_cp(gb, 0xf0);
  if (!(F & FC)) {
    CYCT(b_+88, b_+91); interactionDelete_hook(gb);
    return;
  }
  CYC(b_+88, b_+91);
  CALL_C(b_+91, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+94);
  if (F & FZ) {
    CYCT(b_+94, b_+97); push_effect(gb, b_+97);
    raftwreck_load_angle_and_counter_preset(gb, sp0_);
  } else {
    CYC(b_+94, b_+97);
  }
  if (F & FZ) {
    CYCT(b_+97, b_+100); interactionDelete_hook(gb);
    return;
  }
  CYC(b_+97, b_+100);
  CYC(b_+100, b_+101); ret_effect(gb);
}

static void raftwreck_run_subid03(GB *gb, uint16_t sp0_) {
  BASE(interactionCode64);
  CYC(b_+101, b_+102); H = D;
  CYC(b_+102, b_+104); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  CYC(b_+105, b_+106); alu_or(gb, A);
  if (!(F & FZ)) {
    CYC(b_+106, b_+108);
    CYC(b_+108, b_+109); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+109, b_+110); ret_effect(gb);
    return;
  }
  CYCT(b_+106, b_+108);
  CYC(b_+110, b_+113); SET_HL(b_+167);
  CYC(b_+113, b_+115); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+115, b_+116); A = mem_rd(gb, DE);
  CYC(b_+116, b_+118); alu_cp(gb, 3);
  if (F & FZ) {
    CYCT(b_+118, b_+120);
  } else {
    CYC(b_+118, b_+120);
    CYC(b_+120, b_+123); SET_HL(b_+187);
  }
  CYC(b_+123, b_+126); push_effect(gb, b_+126);
  raftwreck_read_spawn_data(gb);
  CALL_C(b_+126, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+129);
  if (!(F & FZ)) {
    CYCT(b_+129, b_+130); ret_effect(gb);
    return;
  }
  CYC(b_+129, b_+130);
  CYC(b_+130, b_+132); mem_wr(gb, HL, 0x64);
  CYC(b_+132, b_+133); L = alu_inc8(gb, L);
  CYC(b_+133, b_+134); mem_wr(gb, HL, E);
  CYC(b_+134, b_+135); L = alu_inc8(gb, L);
  CYC(b_+135, b_+137); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+137, b_+138); A = mem_rd(gb, DE);
  CYC(b_+138, b_+139); mem_wr(gb, HL, A);
  CYC(b_+139, b_+141); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+144); alu_cp(gb, 3);
  CYC(b_+144, b_+146); A = 0x50;
  if (F & FZ) {
    CYCT(b_+146, b_+148);
  } else {
    CYC(b_+146, b_+148);
    CYC(b_+148, b_+150); A = 0x78;
  }
  CYC(b_+150, b_+152); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+152, b_+153); mem_wr(gb, HL, A);
  CALL_C(b_+153, interactionHSetPosition_hook, SYM(interactionHSetPosition), b_+156);
  CYC(b_+156, b_+157); H = D;
  CYC(b_+157, b_+159); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+159, b_+160); A = mem_rd(gb, HL);
  CYC(b_+160, b_+161); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+161, b_+164); interactionDelete_hook(gb);
    return;
  }
  CYC(b_+161, b_+164);
  CYC(b_+164, b_+165); L = alu_inc8(gb, L);
  CYC(b_+165, b_+166); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+166, b_+167); ret_effect(gb);
}

static void raftwreck_run_subid05(GB *gb, uint16_t sp0_) {
  BASE(interactionCode64);
  CYC(b_+251, b_+252); H = D;
  CYC(b_+252, b_+254); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+254, b_+255); A = mem_rd(gb, HL);
  CYC(b_+255, b_+256); alu_or(gb, A);
  if (!(F & FZ)) {
    CYC(b_+256, b_+258);
    CYC(b_+258, b_+259); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    CYC(b_+259, b_+260); ret_effect(gb);
    return;
  }
  CYCT(b_+256, b_+258);
  CYC(b_+260, b_+263); SET_HL(b_+300);
  CYC(b_+263, b_+266); push_effect(gb, b_+266);
  raftwreck_read_spawn_data(gb);
  CALL_C(b_+266, getFreePartSlot_hook, SYM(getFreePartSlot), b_+269);
  if (!(F & FZ)) {
    CYCT(b_+269, b_+270); ret_effect(gb);
    return;
  }
  CYC(b_+269, b_+270);
  CYC(b_+270, b_+272); mem_wr(gb, HL, 0x27);
  CYC(b_+272, b_+273); L = alu_inc8(gb, L);
  CYC(b_+273, b_+274); mem_wr(gb, HL, E);
  CYC(b_+274, b_+275); L = alu_inc8(gb, L);
  CYC(b_+275, b_+276); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+276, b_+278); L = PART_BASE + OBJ_YH;
  CYC(b_+278, b_+279); mem_wr(gb, HL, B);
  CYC(b_+279, b_+281); L = PART_BASE + OBJ_XH;
  CYC(b_+281, b_+282); mem_wr(gb, HL, C);
  CYC(b_+282, b_+283); H = D;
  CYC(b_+283, b_+285); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+285, b_+286); A = mem_rd(gb, HL);
  CYC(b_+286, b_+287); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+287, b_+289);
    CYC(b_+292, b_+294); A = 3;
    CYC(b_+294, b_+297); W8(wTmpcfc0) = A;
    CYC(b_+297, b_+300); interactionDelete_hook(gb);
    return;
  }
  CYC(b_+287, b_+289);
  CYC(b_+289, b_+290); L = alu_inc8(gb, L);
  CYC(b_+290, b_+291); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+291, b_+292); ret_effect(gb);
}

static void raftwreck_state1(GB *gb, uint16_t sp0_) {
  BASE(interactionCode64);
  CYC(b_+64, b_+66); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+68); push_effect(gb, b_+68);
  do { uint16_t jt_ = (raftwreck_jump_table(gb));
    if (jt_ == b_+80) { raftwreck_run_subid00(gb, sp0_); return; }
    else if (jt_ == b_+101) { raftwreck_run_subid03(gb, sp0_); return; }
    else if (jt_ == b_+251) { raftwreck_run_subid05(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}

void interactionCode64_hook(GB *gb) {
  BASE(interactionCode64);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (raftwreck_jump_table(gb));
    if (jt_ == b_+8) { raftwreck_state0(gb, sp0_); return; }
    else if (jt_ == b_+64) { raftwreck_state1(gb, sp0_); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
