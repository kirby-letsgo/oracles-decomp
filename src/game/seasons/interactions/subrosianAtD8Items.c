#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/subrosianAtD8Items.s.
// INTERAC_SUBROSIAN_AT_VOLCANO_ITEMS

static uint16_t subrosianAtD8Items_jump_table(GB *gb) {
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

static void subrosianAtD8Items_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_SUBROSIAN_AT_VOLCANO_ITEMS
void s_interactionCode54_hook(GB *gb) {
  BASE(interactionCode54);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosianAtD8Items_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+62) goto state1;
    if (jt_ == SYM(objectSetVisible82) && hook_is(gb, SYM(objectSetVisible82), s_objectSetVisible82)) { s_objectSetVisible82(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, s_getRandomNumber, SYM(getRandomNumber), b_+16);
  CYC(b_+16, b_+18); alu_and(gb, 0x0f);
  CYC(b_+18, b_+21); SET_HL(b_+46 /* @table_7c0a */);
  CYC(b_+21, b_+22); subrosianAtD8Items_add_a_to_hl(gb, b_+22);
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  CYC(b_+26, b_+29); SET_BC(0xfe40);
  CALL_C(b_+29, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+32);
  CYC(b_+32, b_+34); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x28);
  CYC(b_+36, b_+38); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x08);
  CALL_C(b_+40, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+43);
  CYC(b_+43, b_+46);
  TAIL(objectSetVisiblec1);

state1:
  CALL_C(b_+62, s_objectApplySpeed, SYM(objectApplySpeed), b_+65);
  CYC(b_+65, b_+67); C = 0x20;
  CALL_C(b_+67, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+70);
  if (!(F & FZ)) { RET_TAKEN(b_+70); return; }
  CYC(b_+70, b_+71);
  CYC(b_+71, b_+73); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+73, b_+75); A = 0x02;
  CYC(b_+75, b_+76); mem_wr(gb, DE, A);
  CYC(b_+76, b_+79);
  TAIL(objectReplaceWithAnimationIfOnHazard);
}

