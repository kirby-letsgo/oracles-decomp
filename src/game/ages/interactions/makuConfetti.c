#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(makuConfetti_subid0);
  CYC(b_+68, b_+69); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+69, b_+70); mem_wr(gb, DE, A);
  CYC(b_+70, b_+71); E = alu_inc8(gb, E);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+72, b_+73); mem_wr(gb, DE, A);
  CYC(b_+73, b_+74); E = alu_inc8(gb, E);
  CYC(b_+74, b_+75); ret_effect(gb);
}

static void maku_confetti_negate_bc(GB *gb) {
  BASE(makuConfetti_subid0);
  CYC(b_+294, b_+295); alu_xor(gb, A);
  CYC(b_+295, b_+296); A = C;
  CYC(b_+296, b_+297); alu_cpl(gb);
  CYC(b_+297, b_+299); alu_add(gb, 1);
  CYC(b_+299, b_+300); C = A;
  CYC(b_+300, b_+301); A = B;
  CYC(b_+301, b_+302); alu_cpl(gb);
  CYC(b_+302, b_+304); alu_adc(gb, 0);
  CYC(b_+304, b_+305); B = A;
  CYC(b_+305, b_+306); ret_effect(gb);
}

static void maku_confetti_negate_word_at_de(GB *gb) {
  BASE(makuConfetti_subid0);
  CYC(b_+306, b_+307); alu_xor(gb, A);
  CYC(b_+307, b_+308); A = mem_rd(gb, DE);
  CYC(b_+308, b_+309); alu_cpl(gb);
  CYC(b_+309, b_+311); alu_add(gb, 1);
  CYC(b_+311, b_+312); mem_wr(gb, DE, A);
  CYC(b_+312, b_+313); E = alu_inc8(gb, E);
  CYC(b_+313, b_+314); A = mem_rd(gb, DE);
  CYC(b_+314, b_+315); alu_cpl(gb);
  CYC(b_+315, b_+317); alu_adc(gb, 0);
  CYC(b_+317, b_+318); mem_wr(gb, DE, A);
  CYC(b_+318, b_+319); ret_effect(gb);
}

static void maku_confetti_set_speed_component(GB *gb) {
  BASE(makuConfetti_subid1);
  CYC(b_+84, b_+85); mem_wr(gb, HL, B);
  CYC(b_+85, b_+86); L = alu_inc8(gb, L);
  CYC(b_+86, b_+87); mem_wr(gb, HL, C);
  CYC(b_+87, b_+88); L = alu_inc8(gb, L);
  CYC(b_+88, b_+89); ret_effect(gb);
}

static void maku_confetti_make_sparkle(GB *gb, uint16_t sp0_) {
  BASE(makuConfetti_subid0);
  CALL_C(b_+319, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+322);
  if (!(F & FZ)) { CYCT(b_+322, b_+323); ret_effect(gb); return; }
  CYC(b_+322, b_+323);
  CYC(b_+323, b_+325); mem_wr(gb, HL, 0x84);
  CYC(b_+325, b_+326); L = alu_inc8(gb, L);
  CYC(b_+326, b_+328); mem_wr(gb, HL, 2);
  CYC(b_+328, b_+331); objectCopyPosition_hook(gb);
}

static void maku_confetti_update_speed(GB *gb, uint16_t e, uint16_t l, uint16_t start, uint16_t jump_end, uint16_t sp0_) {
  BASE(makuConfetti_add16BitRefs);
  CYC(start, start + 2); E = e;
  CYC(start + 2, start + 4); L = l;
  if (jump_end != start + 4) CYC(start + 4, jump_end);
  CYC(b_+0, b_+1); H = D;
  CALL_C(b_+1, add16BitRefs_hook, SYM(add16BitRefs), b_+4);
  CYC(b_+4, b_+5); ret_effect(gb);
}

void interactionCode62_hook(GB *gb) {
  BASE(interactionCode62);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (maku_confetti_jump_table(gb));
    if (jt_ == SYM(makuConfetti_subid0)) { makuConfetti_subid0_hook(gb); return; }
    else if (jt_ == SYM(makuConfetti_subid1)) { makuConfetti_subid1_hook(gb); return; }
    else { HANDOFF(HL); }
  } while (0);
}

void makuConfetti_subid0_hook(GB *gb) {
  BASE(makuConfetti_subid0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (maku_confetti_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+105) { goto state1; }
    else if (jt_ == b_+161) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);
state0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto init_piece; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+22); goto set_delay;
init_piece:
  CYC(b_+22, b_+23); alu_add(gb, A);
  CYC(b_+23, b_+24); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+24, b_+27); SET_HL(b_+69);
  CYC(b_+27, b_+28); maku_confetti_add_double_index(gb, b_+28);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+30, b_+32); A = mem_rd(gb, hCameraY);
  CYC(b_+32, b_+33); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+33, b_+34); SET_HL(HL + 1);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+36); E = alu_inc8(gb, E);
  CYC(b_+36, b_+37); E = alu_inc8(gb, E);
  CYC(b_+37, b_+39); A = mem_rd(gb, hCameraX);
  CYC(b_+39, b_+40); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+40, b_+41); SET_HL(HL + 1);
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+44, b_+47); push_effect(gb, b_+47); maku_confetti_copy_acceleration_component(gb);
  CYC(b_+47, b_+50); push_effect(gb, b_+50); maku_confetti_copy_acceleration_component(gb);
  CYC(b_+50, b_+51); H = D;
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+53, b_+54); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+54, b_+56); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x10);
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0);
  CALL_C(b_+62, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+65);
  CYC(b_+65, b_+68); objectSetVisible80_hook(gb);
  return;
state1:
  CYC(b_+105, b_+106); H = D;
  CYC(b_+106, b_+108); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+108, b_+109); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+109, b_+110); ret_effect(gb); return; }
  CYC(b_+109, b_+110);
  CYC(b_+110, b_+112); mem_wr(gb, HL, 1);
  CALL_C(b_+112, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+115);
  if (!(F & FZ)) { CYCT(b_+115, b_+116); ret_effect(gb); return; }
  CYC(b_+115, b_+116);
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x62);
  CYC(b_+118, b_+120); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+120, b_+121); A = mem_rd(gb, DE);
  CYC(b_+121, b_+122); A = alu_inc8(gb, A);
  CYC(b_+122, b_+123); mem_wr(gb, DE, A);
  CYC(b_+123, b_+125); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+125, b_+126); mem_wr(gb, HL, A);
  CYC(b_+126, b_+128); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+128, b_+130); mem_wr(gb, HL, 180);
  CYC(b_+130, b_+132); A = 0x83;
  CALL_C(b_+132, playSound_b00_hook, SYM(playSound_b00), b_+135);
  CYC(b_+135, b_+137); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+137, b_+138); A = mem_rd(gb, DE);
  CYC(b_+138, b_+140); alu_cp(gb, 5);
  if (F & FZ) { CYCT(b_+140, b_+143); interactionDelete_hook(gb); return; }
  CYC(b_+140, b_+143);
set_delay:
  CYC(b_+143, b_+145); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+149); SET_HL(b_+155);
  CYC(b_+149, b_+150); maku_confetti_add_a_to_hl(gb, b_+150);
  CYC(b_+150, b_+151); A = mem_rd(gb, HL);
  CYC(b_+151, b_+153); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+153, b_+154); mem_wr(gb, DE, A);
  CYC(b_+154, b_+155); ret_effect(gb);
  return;
state2:
  CALL_C(b_+161, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+164);
  if (!(F & FZ)) { CYCT(b_+164, b_+166); goto update_sparkle; }
  CYC(b_+164, b_+166);
  CYC(b_+166, b_+168); mem_wr(gb, HL, 180);
  CYC(b_+168, b_+170); A = 0x83;
  CALL_C(b_+170, playSound_b00_hook, SYM(playSound_b00), b_+173);
update_sparkle:
  CYC(b_+173, b_+174); H = D;
  CYC(b_+174, b_+176); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+176, b_+177); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+177, b_+179); goto update_motion; }
  CYC(b_+177, b_+179);
  CYC(b_+179, b_+181); mem_wr(gb, HL, 0x18);
  CYC(b_+181, b_+184); push_effect(gb, b_+184); maku_confetti_make_sparkle(gb, sp0_);
update_motion:
  CYC(b_+184, b_+187); SET_HL(b_+292);
  CYC(b_+187, b_+189); E = INTERACTION_BASE + OBJ_Y;
  CALL_C(b_+189, add16BitRefs_hook, SYM(add16BitRefs), b_+192);
  CALL_C(b_+192, makuConfetti_updateSpeedY_hook, SYM(makuConfetti_updateSpeedY), b_+195);
  CALL_C(b_+195, makuConfetti_updateSpeedX_hook, SYM(makuConfetti_updateSpeedX), b_+198);
  CALL_C(b_+198, objectApplyComponentSpeed_hook, SYM(objectApplyComponentSpeed), b_+201);
  CYC(b_+201, b_+203); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+203, b_+204); A = mem_rd(gb, DE);
  CYC(b_+204, b_+206); alu_cp(gb, 0x88);
  if (F & FC) { CYCT(b_+206, b_+209); goto adjust_y; }
  CYC(b_+206, b_+209);
  CYC(b_+209, b_+211); alu_cp(gb, 0xd8);
  if (F & FC) { CYCT(b_+211, b_+214); interactionDelete_hook(gb); return; }
  CYC(b_+211, b_+214);
adjust_y:
  CYC(b_+214, b_+215); H = D;
  CYC(b_+215, b_+217); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+217, b_+218); C = mem_rd(gb, HL);
  CYC(b_+218, b_+219); L = alu_inc8(gb, L);
  CYC(b_+219, b_+220); B = mem_rd(gb, HL);
  CYC(b_+220, b_+222); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+222, b_+224); goto compare_y; }
  CYC(b_+222, b_+224);
  CYC(b_+224, b_+227); push_effect(gb, b_+227); maku_confetti_negate_bc(gb);
compare_y:
  CYC(b_+227, b_+230); SET_HL(0x0100);
  CALL_C(b_+230, compareHlToBc_hook, SYM(compareHlToBc), b_+233);
  CYC(b_+233, b_+235); alu_cp(gb, 1);
  if (F & FZ) { CYCT(b_+235, b_+237); goto adjust_x; }
  CYC(b_+235, b_+237);
  CYC(b_+237, b_+239); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+239, b_+242); push_effect(gb, b_+242); maku_confetti_negate_word_at_de(gb);
adjust_x:
  CYC(b_+242, b_+243); H = D;
  CYC(b_+243, b_+245); L = INTERACTION_BASE + OBJ_SPEED_X;
  CYC(b_+245, b_+246); C = mem_rd(gb, HL);
  CYC(b_+246, b_+247); L = alu_inc8(gb, L);
  CYC(b_+247, b_+248); B = mem_rd(gb, HL);
  CYC(b_+248, b_+250); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+250, b_+252); goto compare_x; }
  CYC(b_+250, b_+252);
  CYC(b_+252, b_+255); push_effect(gb, b_+255); maku_confetti_negate_bc(gb);
compare_x:
  CYC(b_+255, b_+258); SET_HL(0x0200);
  CALL_C(b_+258, compareHlToBc_hook, SYM(compareHlToBc), b_+261);
  CYC(b_+261, b_+263); alu_cp(gb, 1);
  if (F & FZ) { CYCT(b_+263, b_+265); goto update_direction; }
  CYC(b_+263, b_+265);
  CYC(b_+265, b_+267); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+267, b_+270); push_effect(gb, b_+270); maku_confetti_negate_word_at_de(gb);
update_direction:
  CYC(b_+270, b_+271); H = D;
  CYC(b_+271, b_+273); L = INTERACTION_BASE + OBJ_SPEED_X + 1;
  CYC(b_+273, b_+275); alu_bit(gb, 7, mem_rd(gb, HL));
  CYC(b_+275, b_+277); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+277, b_+278); A = mem_rd(gb, HL);
  if (F & FZ) { CYCT(b_+278, b_+280); goto positive_x; }
  CYC(b_+278, b_+280);
  CYC(b_+280, b_+281); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+281, b_+282); ret_effect(gb); return; }
  CYC(b_+281, b_+282);
  CYC(b_+282, b_+284); goto invert_direction;
positive_x:
  CYC(b_+284, b_+285); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+285, b_+286); ret_effect(gb); return; }
  CYC(b_+285, b_+286);
invert_direction:
  CYC(b_+286, b_+288); alu_xor(gb, 1);
  CYC(b_+288, b_+289); mem_wr(gb, HL, A);
  CYC(b_+289, b_+292); interactionSetAnimation_hook(gb);
}

void makuConfetti_subid1_hook(GB *gb) {
  BASE(makuConfetti_subid1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (maku_confetti_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+101) { goto state1; }
    else if (jt_ == b_+164) { goto state2; }
    else { HANDOFF(HL); }
  } while (0);
state0:
  CYC(b_+10, b_+11); H = D;
  CYC(b_+11, b_+12); L = E;
  CYC(b_+12, b_+13); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+13, b_+15); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+15, b_+16); A = mem_rd(gb, HL);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto init_piece; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 10);
  CYC(b_+23, b_+26); goto set_delay;
init_piece:
  CYC(b_+26, b_+27); A = alu_dec8(gb, A);
  CYC(b_+27, b_+29); alu_cp(gb, 6);
  if (F & FC) { CYCT(b_+29, b_+31); goto position; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); alu_sub(gb, 6);
position:
  CYC(b_+33, b_+36); SET_HL(b_+89);
  CYC(b_+36, b_+37); maku_confetti_add_double_index(gb, b_+37);
  CYC(b_+37, b_+39); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+39, b_+41); A = mem_rd(gb, hCameraY);
  CYC(b_+41, b_+42); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+42, b_+43); SET_HL(HL + 1);
  CYC(b_+43, b_+44); mem_wr(gb, DE, A);
  CYC(b_+44, b_+45); E = alu_inc8(gb, E);
  CYC(b_+45, b_+46); E = alu_inc8(gb, E);
  CYC(b_+46, b_+48); A = mem_rd(gb, hCameraX);
  CYC(b_+48, b_+49); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+49, b_+50); SET_HL(HL + 1);
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  CYC(b_+51, b_+52); H = D;
  CYC(b_+52, b_+54); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+54, b_+56); B = 0x80;
  CYC(b_+56, b_+58); C = 0xfd;
  CYC(b_+58, b_+61); push_effect(gb, b_+61); maku_confetti_set_speed_component(gb);
  CYC(b_+61, b_+63); B = 0;
  CYC(b_+63, b_+65); C = 4;
  CYC(b_+65, b_+68); push_effect(gb, b_+68); maku_confetti_set_speed_component(gb);
  CYC(b_+68, b_+70); B = 0xf0;
  CYC(b_+70, b_+72); C = 0xff;
  CYC(b_+72, b_+75); push_effect(gb, b_+75); maku_confetti_set_speed_component(gb);
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+78, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+81);
  CYC(b_+81, b_+84); objectSetVisible80_hook(gb);
  return;
state1:
  CALL_C(b_+101, interactionDecCounter2_hook, SYM(interactionDecCounter2), b_+104);
  if (!(F & FZ)) { CYCT(b_+104, b_+106); goto spawn; }
  CYC(b_+104, b_+106);
  CYC(b_+106, b_+108); mem_wr(gb, HL, 45);
  CYC(b_+108, b_+110); A = 0xce;
  CALL_C(b_+110, playSound_b00_hook, SYM(playSound_b00), b_+113);
spawn:
  CYC(b_+113, b_+114); H = D;
  CYC(b_+114, b_+116); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+116, b_+117); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+117, b_+118); ret_effect(gb); return; }
  CYC(b_+117, b_+118);
  CALL_C(b_+118, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+121);
  if (!(F & FZ)) { CYCT(b_+121, b_+122); ret_effect(gb); return; }
  CYC(b_+121, b_+122);
  CYC(b_+122, b_+124); mem_wr(gb, HL, 0x62);
  CYC(b_+124, b_+125); L = alu_inc8(gb, L);
  CYC(b_+125, b_+127); mem_wr(gb, HL, 1);
  CYC(b_+127, b_+129); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+131); A = alu_inc8(gb, A);
  CYC(b_+131, b_+132); mem_wr(gb, DE, A);
  CYC(b_+132, b_+134); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+134, b_+135); mem_wr(gb, HL, A);
  CYC(b_+135, b_+137); alu_cp(gb, 12);
  if (F & FZ) { CYCT(b_+137, b_+140); interactionDelete_hook(gb); return; }
  CYC(b_+137, b_+140);
set_delay:
  CYC(b_+140, b_+142); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+142, b_+143); A = mem_rd(gb, DE);
  CYC(b_+143, b_+146); SET_HL(b_+152);
  CYC(b_+146, b_+147); maku_confetti_add_a_to_hl(gb, b_+147);
  CYC(b_+147, b_+148); A = mem_rd(gb, HL);
  CYC(b_+148, b_+150); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+150, b_+151); mem_wr(gb, DE, A);
  CYC(b_+151, b_+152); ret_effect(gb);
  return;
state2:
  CALL_C(b_+164, makuConfetti_updateSpeedXUsingSpeedZ_hook, SYM(makuConfetti_updateSpeedXUsingSpeedZ), b_+167);
  CYC(b_+167, b_+169); E = INTERACTION_BASE + OBJ_SPEED_X + 1;
  CYC(b_+169, b_+170); A = mem_rd(gb, DE);
  CYC(b_+170, b_+172); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+172, b_+175); interactionDelete_hook(gb); return; }
  CYC(b_+172, b_+175);
  CYC(b_+175, b_+178); objectApplyComponentSpeed_hook(gb);
}

void makuConfetti_updateSpeedY_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  maku_confetti_update_speed(gb, INTERACTION_BASE + OBJ_SPEED, INTERACTION_BASE + OBJ_VAR3C, SYM(makuConfetti_updateSpeedY), SYM(makuConfetti_updateSpeedX), sp0_);
}

void makuConfetti_updateSpeedX_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  maku_confetti_update_speed(gb, INTERACTION_BASE + OBJ_SPEED_X, INTERACTION_BASE + OBJ_VAR3E, SYM(makuConfetti_updateSpeedX), SYM(makuConfetti_updateSpeedYUsingSpeedZ), sp0_);
}

void makuConfetti_updateSpeedYUsingSpeedZ_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  maku_confetti_update_speed(gb, INTERACTION_BASE + OBJ_SPEED, INTERACTION_BASE + OBJ_SPEED_Z, SYM(makuConfetti_updateSpeedYUsingSpeedZ), SYM(makuConfetti_updateSpeedXUsingSpeedZ), sp0_);
}

void makuConfetti_updateSpeedXUsingSpeedZ_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  maku_confetti_update_speed(gb, INTERACTION_BASE + OBJ_SPEED_X, INTERACTION_BASE + OBJ_SPEED_Z, SYM(makuConfetti_updateSpeedXUsingSpeedZ), SYM(makuConfetti_add16BitRefs), sp0_);
}
