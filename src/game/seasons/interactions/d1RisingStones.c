#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/d1RisingStones.s.
// INTERAC_D1_RISING_STONES

static uint16_t d1RisingStones_jump_table(GB *gb) {
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

static void d1RisingStones_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_D1_RISING_STONES
void s_interactionCode4b_hook(GB *gb) {
  BASE(interactionCode4b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (d1RisingStones_jump_table(gb));
    if (jt_ == b_+18) goto state0;
    if (jt_ == b_+29) goto state1;
    if (jt_ == b_+40) goto state2;
    if (jt_ == b_+50) goto state3;
    if (jt_ == b_+121) goto state4;
    if (jt_ == b_+134) goto state5;
    if (jt_ == b_+140) goto state6;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_add(gb, A);
  CYC(b_+22, b_+23); A = alu_inc8(gb, A);
  CYC(b_+23, b_+25); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+29);
  TAIL(interactionInitGraphics);
state1:
  CYC(b_+29, b_+31); A = 0x02;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34); A = 0x6f;
  CALL_C(b_+34, s_playSound, SYM(playSound_b00), b_+37);
  CYC(b_+37, b_+40);
  TAIL(objectSetVisible81);
state2:
  CYC(b_+40, b_+42); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+42, b_+43); A = mem_rd(gb, DE);
  CYC(b_+43, b_+44); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+44, b_+47); TAIL(interactionDelete); }
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+50);
  TAIL(interactionAnimate);
state3:
  CYC(b_+50, b_+52); A = 0x04;
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CALL_C(b_+53, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+56);
  CYC(b_+56, b_+57); B = A;
  CYC(b_+57, b_+59); alu_and(gb, 0x60);
  CYC(b_+59, b_+61); A = alu_swap(gb, A);
  CYC(b_+61, b_+64); SET_HL(b_+149 /* @table_7260 */);
  CYC(b_+64, b_+65); d1RisingStones_add_a_to_hl(gb, b_+65);
  CYC(b_+65, b_+67); E = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+67, b_+68); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
  CYC(b_+69, b_+70); E = alu_inc8(gb, E);
  CYC(b_+70, b_+71); A = mem_rd(gb, HL);
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+73); A = B;
  CYC(b_+73, b_+75); alu_and(gb, 0x03);
  CYC(b_+75, b_+78); SET_HL(b_+157 /* @table_7268 */);
  CYC(b_+78, b_+79); d1RisingStones_add_a_to_hl(gb, b_+79);
  CYC(b_+79, b_+81); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+81, b_+82); A = mem_rd(gb, HL);
  CYC(b_+82, b_+83); mem_wr(gb, DE, A);
  CALL_C(b_+83, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+86);
  CYC(b_+86, b_+87); B = A;
  CYC(b_+87, b_+89); alu_and(gb, 0x30);
  CYC(b_+89, b_+91); A = alu_swap(gb, A);
  CYC(b_+91, b_+94); SET_HL(b_+161 /* @table_726c */);
  CYC(b_+94, b_+95); d1RisingStones_add_a_to_hl(gb, b_+95);
  CYC(b_+95, b_+97); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+97, b_+98); A = mem_rd(gb, HL);
  CYC(b_+98, b_+99); mem_wr(gb, DE, A);
  CYC(b_+99, b_+100); A = B;
  CYC(b_+100, b_+102); alu_and(gb, 0x0f);
  CYC(b_+102, b_+105); SET_HL(b_+165 /* @table_7270 */);
  CYC(b_+105, b_+106); d1RisingStones_add_a_to_hl(gb, b_+106);
  CYC(b_+106, b_+108); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+108, b_+109); A = mem_rd(gb, HL);
  CYC(b_+109, b_+110); mem_wr(gb, DE, A);
  CYC(b_+110, b_+111); A = alu_inc8(gb, A);
  CYC(b_+111, b_+113); alu_and(gb, 0x07);
  CYC(b_+113, b_+115); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+115, b_+118); TAIL(objectSetVisible82); }
  CYC(b_+115, b_+118);
  CYC(b_+118, b_+121);
  TAIL(objectSetVisible80);
state4:
  CALL_C(b_+121, s_objectApplySpeed, SYM(objectApplySpeed), b_+124);
  CYC(b_+124, b_+126); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+126, b_+127); A = mem_rd(gb, DE);
  CALL_C(b_+127, s_objectUpdateSpeedZ, SYM(objectUpdateSpeedZ), b_+130);
  if (!(F & FZ)) { RET_TAKEN(b_+130); return; }
  CYC(b_+130, b_+131);
  CYC(b_+131, b_+134);
  TAIL(interactionDelete);
state5:
  CYC(b_+134, b_+136); A = 0x06;
  CYC(b_+136, b_+137); mem_wr(gb, DE, A);
  CYC(b_+137, b_+140);
  TAIL(objectSetVisible81);
state6:
  CALL_C(b_+140, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+143);
  if (F & FZ) { CYCT(b_+143, b_+146); TAIL(interactionDelete); }
  CYC(b_+143, b_+146);
  CYC(b_+146, b_+149);
  TAIL(interactionAnimate);
}

