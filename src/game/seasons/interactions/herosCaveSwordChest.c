#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/herosCaveSwordChest.s.
// INTERAC_HEROS_CAVE_SWORD_CHEST

static uint16_t herosCaveSwordChest_jump_table(GB *gb) {
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

// INTERAC_HEROS_CAVE_SWORD_CHEST
void s_interactionCodec6_hook(GB *gb) {
  BASE(interactionCodec6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (herosCaveSwordChest_jump_table(gb));
    if (jt_ == b_+14) goto state0;
    if (jt_ == b_+23) goto state1;
    if (jt_ == b_+53) goto state2;
    if (jt_ == b_+80) goto state3;
    if (jt_ == b_+124) goto state4;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+20); mem_wr(gb, wcca1, A);
  CYC(b_+20, b_+23);
  TAIL(interactionInitGraphics);
state1:
  CYC(b_+23, b_+26); A = mem_rd(gb, wcca2);
  CYC(b_+26, b_+27); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+27); return; }
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); A = 0x81;
  CYC(b_+30, b_+33); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+33, b_+36); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+36, s_interactionIncState, SYM(interactionIncState), b_+39);
  CALL_C(b_+39, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+42);
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x0a);
  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x20);
  CYC(b_+50, b_+53);
  TAIL(objectSetVisible80);
state2:
  CALL_C(b_+53, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+56);
  if (!(F & FZ)) { CYCT(b_+56, b_+59); TAIL(objectApplySpeed); }
  CYC(b_+56, b_+59);
  CALL_C(b_+59, s_interactionIncState, SYM(interactionIncState), b_+62);
  CYC(b_+62, b_+64); A = 0x05;
  CYC(b_+64, b_+66); C = 0x01;
  CALL_C(b_+66, s_giveTreasure, SYM(giveTreasure), b_+69);
  CYC(b_+69, b_+71); A = 0x4c;
  CALL_C(b_+71, s_playSound, SYM(playSound_b00), b_+74);
  CYC(b_+74, b_+77); SET_BC(0x001c);
  CYC(b_+77, b_+80);
  TAIL(showText);
state3:
  CYC(b_+80, b_+83); A = mem_rd(gb, wTextIsActive);
  CYC(b_+83, b_+84); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+84); return; }
  CYC(b_+84, b_+85);
  CALL_C(b_+85, s_interactionIncState, SYM(interactionIncState), b_+88);
  CALL_C(b_+88, s_objectSetInvisible, SYM(objectSetInvisible), b_+91);
  CYC(b_+91, b_+93); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+93, b_+95); A = 0x5a;
  CYC(b_+95, b_+96); mem_wr(gb, DE, A);
  CALL_C(b_+96, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+99);
  if (!(F & FZ)) { RET_TAKEN(b_+99); return; }
  CYC(b_+99, b_+100);
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0x60);
  CYC(b_+102, b_+103); L = alu_inc8(gb, L);
  CYC(b_+103, b_+105); mem_wr(gb, HL, 0x05);
  CYC(b_+105, b_+106); L = alu_inc8(gb, L);
  CYC(b_+106, b_+108); mem_wr(gb, HL, 0x03);
  CYC(b_+108, b_+111); A = mem_rd(gb, w1Link_yh);
  CYC(b_+111, b_+113); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+113, b_+114); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+114, b_+115); L = alu_inc8(gb, L);
  CYC(b_+115, b_+118); A = mem_rd(gb, w1Link_xh);
  CYC(b_+118, b_+119); mem_wr(gb, HL, A);
  CYC(b_+119, b_+121); A = 0xfb;
  CYC(b_+121, b_+124);
  TAIL(playSound_b00);
state4:
  CALL_C(b_+124, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+127);
  if (!(F & FZ)) { RET_TAKEN(b_+127); return; }
  CYC(b_+127, b_+128);
  CALL_C(b_+128, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+131);
  CYC(b_+131, b_+133); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);
  CYC(b_+133, b_+136); SET_HL(b_+147 /* @warpDestVariables */);
  CALL_C(b_+136, s_setWarpDestVariables, SYM(setWarpDestVariables), b_+139);
  CYC(b_+139, b_+141); A = 0xb4;
  CALL_C(b_+141, s_playSound, SYM(playSound_b00), b_+144);
  CYC(b_+144, b_+147);
  TAIL(interactionDelete);
}

