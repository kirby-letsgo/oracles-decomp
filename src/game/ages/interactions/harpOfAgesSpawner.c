#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t harp_of_ages_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 4, 6, false);
    burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

void interactionCodeb3__state0_hook(GB *gb) {
  BASE(interactionCodeb3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+14, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+17);
  CYC(b_+17, b_+19); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+19, b_+22); interactionDelete_hook(gb); return; }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+23); alu_xor(gb, A);
  CYC(b_+23, b_+26); mem_wr(gb, wRoomLayoutEnd, A);
  CALL_C(b_+26, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+29);
  if (!(F & FZ)) { CYCT(b_+29, b_+30); ret_effect(gb); return; }
  CYC(b_+29, b_+32); mem_wr(gb, HL, 0x60);
  CYC(b_+32, b_+33); L = alu_inc8(gb, L);
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x11);
  CYC(b_+35, b_+37); L = 0x4b;
  CYC(b_+37, b_+40); mem_wr(gb, HL, 0x38);
  CYC(b_+39, b_+41); L = 0x4d;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x58);
  CYC(b_+43, b_+44); B = H;
  CALL_C(b_+44, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+47);
  if (!(F & FZ)) { CYCT(b_+47, b_+49); goto inc_state; }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+51); mem_wr(gb, HL, 0x84);
  CYC(b_+51, b_+52); L = alu_inc8(gb, L);
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x0c);
  CYC(b_+54, b_+56); L = 0x56;
  CYC(b_+56, b_+58); A = 0x40;
  CYC(b_+58, b_+59); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+59, b_+60); mem_wr(gb, HL, B);
inc_state:
  CALL_C(b_+60, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+63);
  CYC(b_+63, b_+66); interactionIncState_hook(gb); return;
}

void interactionCodeb3__incState_hook(GB *gb) {
  BASE(interactionCodeb3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+60, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+63);
  CYC(b_+63, b_+66); interactionIncState_hook(gb); return;
}

void interactionCodeb3__state1_hook(GB *gb) {
  BASE(interactionCodeb3);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+66, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+69);
  CYC(b_+69, b_+71); alu_bit(gb, 5, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+71, b_+72); ret_effect(gb); return; }
  CYC(b_+71, b_+72);
  CYC(b_+72, b_+74); A = 0xf0;
  CALL_C(b_+74, playSound_b00_hook, SYM(playSound_b00), b_+77);
  CYC(b_+77, b_+79); A = 0x80;
  CYC(b_+79, b_+82); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+82, b_+85); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+85, interactionIncState_hook, SYM(interactionIncState), b_+88);
}

void interactionCodeb3__state2_hook(GB *gb) {
  BASE(interactionCodeb3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+88, b_+91); A = mem_rd(gb, wOamEnd);
  CYC(b_+91, b_+92); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+92, b_+93); ret_effect(gb); return; }
  CYC(b_+92, b_+93);
  CYC(b_+93, b_+94); alu_xor(gb, A);
  CYC(b_+94, b_+97); mem_wr(gb, w1Link_direction, A);
  CYC(b_+97, b_+100); interactionIncState_hook(gb); return;
}

void interactionCodeb3__state3_hook(GB *gb) {
  BASE(interactionCodeb3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+100, b_+103); A = mem_rd(gb, wOamEnd);
  CYC(b_+103, b_+104); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+104, b_+105); ret_effect(gb); return; }
  CYC(b_+104, b_+105);
  CYC(b_+105, b_+108); SET_HL(wRoomLayoutEnd);
  CYC(b_+108, b_+110); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 1));
  CALL_C(b_+110, interactionIncState_hook, SYM(interactionIncState), b_+113);
  CYC(b_+113, b_+115); L = 0x46;
  CYC(b_+115, b_+118); mem_wr(gb, HL, 0x28);
  CYC(b_+117, b_+119); A = 0x02;
  CALL_C(b_+119, fadeoutToBlackWithDelay_hook, SYM(fadeoutToBlackWithDelay), b_+122);
  CYC(b_+122, b_+124); A = 0xff;
  CYC(b_+124, b_+127); mem_wr(gb, wDirtyFadeBgPalettes, A);
  CYC(b_+127, b_+130); mem_wr(gb, wFadeBgPaletteSources, A);
  CYC(b_+130, b_+132); A = 1;
  CYC(b_+132, b_+135); mem_wr(gb, wDirtyFadeSprPalettes, A);
  CYC(b_+135, b_+137); A = 0xfe;
  CYC(b_+137, b_+140); mem_wr(gb, wFadeSprPaletteSources, A);
  CALL_C(b_+140, hideStatusBar_hook, SYM(hideStatusBar), b_+143);
  CYC(b_+143, b_+145); A = mem_rd(gb, hActiveObject);
  CYC(b_+145, b_+146); D = A;
  CYC(b_+146, b_+147); ret_effect(gb);
}

void interactionCodeb3__state4_hook(GB *gb) {
  BASE(interactionCodeb3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+147, b_+150); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+150, b_+151); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+151, b_+152); ret_effect(gb); return; }
  CYC(b_+151, b_+152);
  CALL_C(b_+152, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+155);
  if (!(F & FZ)) { CYCT(b_+155, b_+156); ret_effect(gb); return; }
  CYC(b_+155, b_+156);
  CYC(b_+156, b_+157); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+157, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+160);
  if (!(F & FZ)) { CYCT(b_+160, b_+161); ret_effect(gb); return; }
  CYC(b_+160, b_+161);
  CYC(b_+161, b_+163); mem_wr(gb, HL, 0x36);
  CYC(b_+163, b_+164); L = alu_inc8(gb, L);
  CYC(b_+164, b_+166); mem_wr(gb, HL, 0x07);
  CALL_C(b_+166, objectCopyPosition_hook, SYM(objectCopyPosition), b_+169);
  CYC(b_+169, b_+172); interactionDelete_hook(gb); return;
}

void interactionCodeb3_hook(GB *gb) {
  BASE(interactionCodeb3);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+4); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (harp_of_ages_jump_table(gb));
    if (jt_ == b_+14) { interactionCodeb3__state0_hook(gb); return; }
    else if (jt_ == b_+66) { interactionCodeb3__state1_hook(gb); return; }
    else if (jt_ == b_+88) { interactionCodeb3__state2_hook(gb); return; }
    else if (jt_ == b_+100) { interactionCodeb3__state3_hook(gb); return; }
    else if (jt_ == b_+147) { interactionCodeb3__state4_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
