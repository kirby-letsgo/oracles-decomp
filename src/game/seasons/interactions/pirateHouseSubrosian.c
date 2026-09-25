#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/pirateHouseSubrosian.s.
// INTERAC_PIRATE_HOUSE_SUBROSIAN

static uint16_t pirateHouseSubrosian_jump_table(GB *gb) {
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

static void pirateHouseSubrosian_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_PIRATE_HOUSE_SUBROSIAN
void s_interactionCode42_hook(GB *gb) {
  BASE(interactionCode42);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (pirateHouseSubrosian_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+49) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x13;
  CALL_C(b_+10, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+13);
  CYC(b_+13, b_+15); B = 0x00;
  if (!(F & FZ)) { CYCT(b_+15, b_+17); goto L_6cef; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); B = alu_inc8(gb, B);
L_6cef:
  CYC(b_+18, b_+20); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_cp(gb, B);
  if (F & FZ) { CYCT(b_+22, b_+25); TAIL(interactionDelete); }
  CYC(b_+22, b_+25);
  CALL_C(b_+25, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+28);
  CALL_C(b_+28, s_interactionIncState, SYM(interactionIncState), b_+31);
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+33, b_+34); A = mem_rd(gb, HL);
  CYC(b_+34, b_+37); SET_HL(SYM(table_6d14));
  CYC(b_+37, b_+38); pirateHouseSubrosian_add_double_index(gb, b_+38);
  CYC(b_+38, b_+39); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+39, b_+40); H = mem_rd(gb, HL);
  CYC(b_+40, b_+41); L = A;
  CALL_C(b_+41, s_interactionSetScript, SYM(interactionSetScript), b_+44);
  CYC(b_+44, b_+46); A = 0x02;
  CALL_C(b_+46, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+49);
state1:
  CALL_C(b_+49, s_interactionRunScript, SYM(interactionRunScript), b_+52);
  CYC(b_+52, b_+55);
  TAIL(npcFaceLinkAndAnimate);
}

