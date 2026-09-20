#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

void makuConfetti_subid0_hook(GB *gb);
void makuConfetti_subid1_hook(GB *gb);
void makuConfetti_updateSpeedY_hook(GB *gb);
void makuConfetti_updateSpeedX_hook(GB *gb);
void makuConfetti_updateSpeedYUsingSpeedZ_hook(GB *gb);
void makuConfetti_updateSpeedXUsingSpeedZ_hook(GB *gb);

static uint16_t maku_confetti_jump_table(GB *gb) {
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

static void maku_confetti_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void maku_confetti_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void maku_confetti_copy_acceleration_component(GB *gb) {
  CYC(0x717e, 0x717f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x717f, 0x7180); mem_wr(gb, DE, A);
  CYC(0x7180, 0x7181); E = alu_inc8(gb, E);
  CYC(0x7181, 0x7182); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x7182, 0x7183); mem_wr(gb, DE, A);
  CYC(0x7183, 0x7184); E = alu_inc8(gb, E);
  CYC(0x7184, 0x7185); ret_effect(gb);
}

static void maku_confetti_negate_bc(GB *gb) {
  CYC(0x7260, 0x7261); alu_xor(gb, A);
  CYC(0x7261, 0x7262); A = C;
  CYC(0x7262, 0x7263); alu_cpl(gb);
  CYC(0x7263, 0x7265); alu_add(gb, 1);
  CYC(0x7265, 0x7266); C = A;
  CYC(0x7266, 0x7267); A = B;
  CYC(0x7267, 0x7268); alu_cpl(gb);
  CYC(0x7268, 0x726a); alu_adc(gb, 0);
  CYC(0x726a, 0x726b); B = A;
  CYC(0x726b, 0x726c); ret_effect(gb);
}

static void maku_confetti_negate_word_at_de(GB *gb) {
  CYC(0x726c, 0x726d); alu_xor(gb, A);
  CYC(0x726d, 0x726e); A = mem_rd(gb, DE);
  CYC(0x726e, 0x726f); alu_cpl(gb);
  CYC(0x726f, 0x7271); alu_add(gb, 1);
  CYC(0x7271, 0x7272); mem_wr(gb, DE, A);
  CYC(0x7272, 0x7273); E = alu_inc8(gb, E);
  CYC(0x7273, 0x7274); A = mem_rd(gb, DE);
  CYC(0x7274, 0x7275); alu_cpl(gb);
  CYC(0x7275, 0x7277); alu_adc(gb, 0);
  CYC(0x7277, 0x7278); mem_wr(gb, DE, A);
  CYC(0x7278, 0x7279); ret_effect(gb);
}

static void maku_confetti_set_speed_component(GB *gb) {
  CYC(0x72d9, 0x72da); mem_wr(gb, HL, B);
  CYC(0x72da, 0x72db); L = alu_inc8(gb, L);
  CYC(0x72db, 0x72dc); mem_wr(gb, HL, C);
  CYC(0x72dc, 0x72dd); L = alu_inc8(gb, L);
  CYC(0x72dd, 0x72de); ret_effect(gb);
}

static void maku_confetti_make_sparkle(GB *gb, uint16_t sp0_) {
  CALL_C(0x7279, getFreeInteractionSlot_hook, 0x3aef, 0x727c);
  if (!(F & FZ)) { CYCT(0x727c, 0x727d); ret_effect(gb); return; }
  CYC(0x727c, 0x727d);
  CYC(0x727d, 0x727f); mem_wr(gb, HL, 0x84);
  CYC(0x727f, 0x7280); L = alu_inc8(gb, L);
  CYC(0x7280, 0x7282); mem_wr(gb, HL, 2);
  CYC(0x7282, 0x7285); objectCopyPosition_hook(gb);
}

static void maku_confetti_update_speed(GB *gb, uint16_t e, uint16_t l, uint16_t start, uint16_t jump_end, uint16_t sp0_) {
  CYC(start, start + 2); E = e;
  CYC(start + 2, start + 4); L = l;
  if (jump_end != start + 4) CYC(start + 4, jump_end);
  CYC(0x734d, 0x734e); H = D;
  CALL_C(0x734e, add16BitRefs_hook, 0x23a7, 0x7351);
  CYC(0x7351, 0x7352); ret_effect(gb);
}

void interactionCode62_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7132, 0x7134); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x7134, 0x7135); A = mem_rd(gb, DE);
  CYC(0x7135, 0x7136); push_effect(gb, 0x7136);
  switch (maku_confetti_jump_table(gb)) {
    case 0x713a: makuConfetti_subid0_hook(gb); return;
    case 0x7285: makuConfetti_subid1_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void makuConfetti_subid0_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x713a, 0x713c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x713c, 0x713d); A = mem_rd(gb, DE);
  CYC(0x713d, 0x713e); push_effect(gb, 0x713e);
  switch (maku_confetti_jump_table(gb)) {
    case 0x7144: goto state0;
    case 0x71a3: goto state1;
    case 0x71db: goto state2;
    default: HANDOFF(HL);
  }
state0:
  CYC(0x7144, 0x7145); H = D;
  CYC(0x7145, 0x7146); L = E;
  CYC(0x7146, 0x7147); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7147, 0x7149); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7149, 0x714a); A = mem_rd(gb, HL);
  CYC(0x714a, 0x714b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x714b, 0x714d); goto init_piece; }
  CYC(0x714b, 0x714d);
  CYC(0x714d, 0x7150); goto set_delay;
init_piece:
  CYC(0x7150, 0x7151); alu_add(gb, A);
  CYC(0x7151, 0x7152); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7152, 0x7155); SET_HL(0x717f);
  CYC(0x7155, 0x7156); maku_confetti_add_double_index(gb, 0x7156);
  CYC(0x7156, 0x7158); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x7158, 0x715a); A = mem_rd(gb, hCameraY);
  CYC(0x715a, 0x715b); alu_add(gb, mem_rd(gb, HL));
  CYC(0x715b, 0x715c); SET_HL(HL + 1);
  CYC(0x715c, 0x715d); mem_wr(gb, DE, A);
  CYC(0x715d, 0x715e); E = alu_inc8(gb, E);
  CYC(0x715e, 0x715f); E = alu_inc8(gb, E);
  CYC(0x715f, 0x7161); A = mem_rd(gb, hCameraX);
  CYC(0x7161, 0x7162); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7162, 0x7163); SET_HL(HL + 1);
  CYC(0x7163, 0x7164); mem_wr(gb, DE, A);
  CYC(0x7164, 0x7166); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x7166, 0x7169); push_effect(gb, 0x7169); maku_confetti_copy_acceleration_component(gb);
  CYC(0x7169, 0x716c); push_effect(gb, 0x716c); maku_confetti_copy_acceleration_component(gb);
  CYC(0x716c, 0x716d); H = D;
  CYC(0x716d, 0x716f); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x716f, 0x7170); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7170, 0x7172); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x7172, 0x7174); mem_wr(gb, HL, 0x10);
  CYC(0x7174, 0x7176); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x7176, 0x7178); mem_wr(gb, HL, 0);
  CALL_C(0x7178, interactionInitGraphics_hook, 0x15fb, 0x717b);
  CYC(0x717b, 0x717e); objectSetVisible80_hook(gb);
  return;
state1:
  CYC(0x71a3, 0x71a4); H = D;
  CYC(0x71a4, 0x71a6); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x71a6, 0x71a7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x71a7, 0x71a8); ret_effect(gb); return; }
  CYC(0x71a7, 0x71a8);
  CYC(0x71a8, 0x71aa); mem_wr(gb, HL, 1);
  CALL_C(0x71aa, getFreeInteractionSlot_hook, 0x3aef, 0x71ad);
  if (!(F & FZ)) { CYCT(0x71ad, 0x71ae); ret_effect(gb); return; }
  CYC(0x71ad, 0x71ae);
  CYC(0x71ae, 0x71b0); mem_wr(gb, HL, 0x62);
  CYC(0x71b0, 0x71b2); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x71b2, 0x71b3); A = mem_rd(gb, DE);
  CYC(0x71b3, 0x71b4); A = alu_inc8(gb, A);
  CYC(0x71b4, 0x71b5); mem_wr(gb, DE, A);
  CYC(0x71b5, 0x71b7); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x71b7, 0x71b8); mem_wr(gb, HL, A);
  CYC(0x71b8, 0x71ba); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x71ba, 0x71bc); mem_wr(gb, HL, 180);
  CYC(0x71bc, 0x71be); A = 0x83;
  CALL_C(0x71be, playSound_b00_hook, 0x0c98, 0x71c1);
  CYC(0x71c1, 0x71c3); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x71c3, 0x71c4); A = mem_rd(gb, DE);
  CYC(0x71c4, 0x71c6); alu_cp(gb, 5);
  if (F & FZ) { CYCT(0x71c6, 0x71c9); interactionDelete_hook(gb); return; }
  CYC(0x71c6, 0x71c9);
set_delay:
  CYC(0x71c9, 0x71cb); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x71cb, 0x71cc); A = mem_rd(gb, DE);
  CYC(0x71cc, 0x71cf); SET_HL(0x71d5);
  CYC(0x71cf, 0x71d0); maku_confetti_add_a_to_hl(gb, 0x71d0);
  CYC(0x71d0, 0x71d1); A = mem_rd(gb, HL);
  CYC(0x71d1, 0x71d3); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x71d3, 0x71d4); mem_wr(gb, DE, A);
  CYC(0x71d4, 0x71d5); ret_effect(gb);
  return;
state2:
  CALL_C(0x71db, interactionDecCounter2_hook, 0x23d1, 0x71de);
  if (!(F & FZ)) { CYCT(0x71de, 0x71e0); goto update_sparkle; }
  CYC(0x71de, 0x71e0);
  CYC(0x71e0, 0x71e2); mem_wr(gb, HL, 180);
  CYC(0x71e2, 0x71e4); A = 0x83;
  CALL_C(0x71e4, playSound_b00_hook, 0x0c98, 0x71e7);
update_sparkle:
  CYC(0x71e7, 0x71e8); H = D;
  CYC(0x71e8, 0x71ea); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x71ea, 0x71eb); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x71eb, 0x71ed); goto update_motion; }
  CYC(0x71eb, 0x71ed);
  CYC(0x71ed, 0x71ef); mem_wr(gb, HL, 0x18);
  CYC(0x71ef, 0x71f2); push_effect(gb, 0x71f2); maku_confetti_make_sparkle(gb, sp0_);
update_motion:
  CYC(0x71f2, 0x71f5); SET_HL(0x725e);
  CYC(0x71f5, 0x71f7); E = INTERACTION_BASE + OBJ_Y;
  CALL_C(0x71f7, add16BitRefs_hook, 0x23a7, 0x71fa);
  CALL_C(0x71fa, makuConfetti_updateSpeedY_hook, 0x7337, 0x71fd);
  CALL_C(0x71fd, makuConfetti_updateSpeedX_hook, 0x733d, 0x7200);
  CALL_C(0x7200, objectApplyComponentSpeed_hook, 0x2008, 0x7203);
  CYC(0x7203, 0x7205); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x7205, 0x7206); A = mem_rd(gb, DE);
  CYC(0x7206, 0x7208); alu_cp(gb, 0x88);
  if (F & FC) { CYCT(0x7208, 0x720b); goto adjust_y; }
  CYC(0x7208, 0x720b);
  CYC(0x720b, 0x720d); alu_cp(gb, 0xd8);
  if (F & FC) { CYCT(0x720d, 0x7210); interactionDelete_hook(gb); return; }
  CYC(0x720d, 0x7210);
adjust_y:
  CYC(0x7210, 0x7211); H = D;
  CYC(0x7211, 0x7213); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x7213, 0x7214); C = mem_rd(gb, HL);
  CYC(0x7214, 0x7215); L = alu_inc8(gb, L);
  CYC(0x7215, 0x7216); B = mem_rd(gb, HL);
  CYC(0x7216, 0x7218); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7218, 0x721a); goto compare_y; }
  CYC(0x7218, 0x721a);
  CYC(0x721a, 0x721d); push_effect(gb, 0x721d); maku_confetti_negate_bc(gb);
compare_y:
  CYC(0x721d, 0x7220); SET_HL(0x0100);
  CALL_C(0x7220, compareHlToBc_hook, 0x01d6, 0x7223);
  CYC(0x7223, 0x7225); alu_cp(gb, 1);
  if (F & FZ) { CYCT(0x7225, 0x7227); goto adjust_x; }
  CYC(0x7225, 0x7227);
  CYC(0x7227, 0x7229); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x7229, 0x722c); push_effect(gb, 0x722c); maku_confetti_negate_word_at_de(gb);
adjust_x:
  CYC(0x722c, 0x722d); H = D;
  CYC(0x722d, 0x722f); L = INTERACTION_BASE + OBJ_SPEED_X;
  CYC(0x722f, 0x7230); C = mem_rd(gb, HL);
  CYC(0x7230, 0x7231); L = alu_inc8(gb, L);
  CYC(0x7231, 0x7232); B = mem_rd(gb, HL);
  CYC(0x7232, 0x7234); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x7234, 0x7236); goto compare_x; }
  CYC(0x7234, 0x7236);
  CYC(0x7236, 0x7239); push_effect(gb, 0x7239); maku_confetti_negate_bc(gb);
compare_x:
  CYC(0x7239, 0x723c); SET_HL(0x0200);
  CALL_C(0x723c, compareHlToBc_hook, 0x01d6, 0x723f);
  CYC(0x723f, 0x7241); alu_cp(gb, 1);
  if (F & FZ) { CYCT(0x7241, 0x7243); goto update_direction; }
  CYC(0x7241, 0x7243);
  CYC(0x7243, 0x7245); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x7245, 0x7248); push_effect(gb, 0x7248); maku_confetti_negate_word_at_de(gb);
update_direction:
  CYC(0x7248, 0x7249); H = D;
  CYC(0x7249, 0x724b); L = INTERACTION_BASE + OBJ_SPEED_X + 1;
  CYC(0x724b, 0x724d); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(0x724d, 0x724f); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x724f, 0x7250); A = mem_rd(gb, HL);
  if (F & FZ) { CYCT(0x7250, 0x7252); goto positive_x; }
  CYC(0x7250, 0x7252);
  CYC(0x7252, 0x7253); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7253, 0x7254); ret_effect(gb); return; }
  CYC(0x7253, 0x7254);
  CYC(0x7254, 0x7256); goto invert_direction;
positive_x:
  CYC(0x7256, 0x7257); alu_or(gb, A);
  if (F & FZ) { CYCT(0x7257, 0x7258); ret_effect(gb); return; }
  CYC(0x7257, 0x7258);
invert_direction:
  CYC(0x7258, 0x725a); alu_xor(gb, 1);
  CYC(0x725a, 0x725b); mem_wr(gb, HL, A);
  CYC(0x725b, 0x725e); interactionSetAnimation_hook(gb);
}

void makuConfetti_subid1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7285, 0x7287); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7287, 0x7288); A = mem_rd(gb, DE);
  CYC(0x7288, 0x7289); push_effect(gb, 0x7289);
  switch (maku_confetti_jump_table(gb)) {
    case 0x728f: goto state0;
    case 0x72ea: goto state1;
    case 0x7329: goto state2;
    default: HANDOFF(HL);
  }
state0:
  CYC(0x728f, 0x7290); H = D;
  CYC(0x7290, 0x7291); L = E;
  CYC(0x7291, 0x7292); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7292, 0x7294); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x7294, 0x7295); A = mem_rd(gb, HL);
  CYC(0x7295, 0x7296); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7296, 0x7298); goto init_piece; }
  CYC(0x7296, 0x7298);
  CYC(0x7298, 0x729a); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x729a, 0x729c); mem_wr(gb, HL, 10);
  CYC(0x729c, 0x729f); goto set_delay;
init_piece:
  CYC(0x729f, 0x72a0); A = alu_dec8(gb, A);
  CYC(0x72a0, 0x72a2); alu_cp(gb, 6);
  if (F & FC) { CYCT(0x72a2, 0x72a4); goto position; }
  CYC(0x72a2, 0x72a4);
  CYC(0x72a4, 0x72a6); alu_sub(gb, 6);
position:
  CYC(0x72a6, 0x72a9); SET_HL(0x72de);
  CYC(0x72a9, 0x72aa); maku_confetti_add_double_index(gb, 0x72aa);
  CYC(0x72aa, 0x72ac); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x72ac, 0x72ae); A = mem_rd(gb, hCameraY);
  CYC(0x72ae, 0x72af); alu_add(gb, mem_rd(gb, HL));
  CYC(0x72af, 0x72b0); SET_HL(HL + 1);
  CYC(0x72b0, 0x72b1); mem_wr(gb, DE, A);
  CYC(0x72b1, 0x72b2); E = alu_inc8(gb, E);
  CYC(0x72b2, 0x72b3); E = alu_inc8(gb, E);
  CYC(0x72b3, 0x72b5); A = mem_rd(gb, hCameraX);
  CYC(0x72b5, 0x72b6); alu_add(gb, mem_rd(gb, HL));
  CYC(0x72b6, 0x72b7); SET_HL(HL + 1);
  CYC(0x72b7, 0x72b8); mem_wr(gb, DE, A);
  CYC(0x72b8, 0x72b9); H = D;
  CYC(0x72b9, 0x72bb); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x72bb, 0x72bd); B = 0x80;
  CYC(0x72bd, 0x72bf); C = 0xfd;
  CYC(0x72bf, 0x72c2); push_effect(gb, 0x72c2); maku_confetti_set_speed_component(gb);
  CYC(0x72c2, 0x72c4); B = 0;
  CYC(0x72c4, 0x72c6); C = 4;
  CYC(0x72c6, 0x72c9); push_effect(gb, 0x72c9); maku_confetti_set_speed_component(gb);
  CYC(0x72c9, 0x72cb); B = 0xf0;
  CYC(0x72cb, 0x72cd); C = 0xff;
  CYC(0x72cd, 0x72d0); push_effect(gb, 0x72d0); maku_confetti_set_speed_component(gb);
  CYC(0x72d0, 0x72d2); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x72d2, 0x72d3); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x72d3, interactionInitGraphics_hook, 0x15fb, 0x72d6);
  CYC(0x72d6, 0x72d9); objectSetVisible80_hook(gb);
  return;
state1:
  CALL_C(0x72ea, interactionDecCounter2_hook, 0x23d1, 0x72ed);
  if (!(F & FZ)) { CYCT(0x72ed, 0x72ef); goto spawn; }
  CYC(0x72ed, 0x72ef);
  CYC(0x72ef, 0x72f1); mem_wr(gb, HL, 45);
  CYC(0x72f1, 0x72f3); A = 0xce;
  CALL_C(0x72f3, playSound_b00_hook, 0x0c98, 0x72f6);
spawn:
  CYC(0x72f6, 0x72f7); H = D;
  CYC(0x72f7, 0x72f9); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x72f9, 0x72fa); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x72fa, 0x72fb); ret_effect(gb); return; }
  CYC(0x72fa, 0x72fb);
  CALL_C(0x72fb, getFreeInteractionSlot_hook, 0x3aef, 0x72fe);
  if (!(F & FZ)) { CYCT(0x72fe, 0x72ff); ret_effect(gb); return; }
  CYC(0x72fe, 0x72ff);
  CYC(0x72ff, 0x7301); mem_wr(gb, HL, 0x62);
  CYC(0x7301, 0x7302); L = alu_inc8(gb, L);
  CYC(0x7302, 0x7304); mem_wr(gb, HL, 1);
  CYC(0x7304, 0x7306); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7306, 0x7307); A = mem_rd(gb, DE);
  CYC(0x7307, 0x7308); A = alu_inc8(gb, A);
  CYC(0x7308, 0x7309); mem_wr(gb, DE, A);
  CYC(0x7309, 0x730b); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x730b, 0x730c); mem_wr(gb, HL, A);
  CYC(0x730c, 0x730e); alu_cp(gb, 12);
  if (F & FZ) { CYCT(0x730e, 0x7311); interactionDelete_hook(gb); return; }
  CYC(0x730e, 0x7311);
set_delay:
  CYC(0x7311, 0x7313); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7313, 0x7314); A = mem_rd(gb, DE);
  CYC(0x7314, 0x7317); SET_HL(0x731d);
  CYC(0x7317, 0x7318); maku_confetti_add_a_to_hl(gb, 0x7318);
  CYC(0x7318, 0x7319); A = mem_rd(gb, HL);
  CYC(0x7319, 0x731b); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(0x731b, 0x731c); mem_wr(gb, DE, A);
  CYC(0x731c, 0x731d); ret_effect(gb);
  return;
state2:
  CALL_C(0x7329, makuConfetti_updateSpeedXUsingSpeedZ_hook, 0x7349, 0x732c);
  CYC(0x732c, 0x732e); E = INTERACTION_BASE + OBJ_SPEED_X + 1;
  CYC(0x732e, 0x732f); A = mem_rd(gb, DE);
  CYC(0x732f, 0x7331); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(0x7331, 0x7334); interactionDelete_hook(gb); return; }
  CYC(0x7331, 0x7334);
  CYC(0x7334, 0x7337); objectApplyComponentSpeed_hook(gb);
}

void makuConfetti_updateSpeedY_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  maku_confetti_update_speed(gb, INTERACTION_BASE + OBJ_SPEED, INTERACTION_BASE + OBJ_VAR3C, 0x7337, 0x733d, sp0_);
}

void makuConfetti_updateSpeedX_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  maku_confetti_update_speed(gb, INTERACTION_BASE + OBJ_SPEED_X, INTERACTION_BASE + OBJ_VAR3E, 0x733d, 0x7343, sp0_);
}

void makuConfetti_updateSpeedYUsingSpeedZ_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  maku_confetti_update_speed(gb, INTERACTION_BASE + OBJ_SPEED, INTERACTION_BASE + OBJ_SPEED_Z, 0x7343, 0x7349, sp0_);
}

void makuConfetti_updateSpeedXUsingSpeedZ_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  maku_confetti_update_speed(gb, INTERACTION_BASE + OBJ_SPEED_X, INTERACTION_BASE + OBJ_SPEED_Z, 0x7349, 0x734d, sp0_);
}
