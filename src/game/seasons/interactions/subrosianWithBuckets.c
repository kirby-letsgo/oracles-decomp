#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/subrosianWithBuckets.s.
// INTERAC_SUBROSIAN_WITH_BUCKETS

static uint16_t subrosianWithBuckets_jump_table(GB *gb) {
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

static void subrosianWithBuckets_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_SUBROSIAN_WITH_BUCKETS
void s_interactionCode32_hook(GB *gb) {
  BASE(interactionCode32);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosianWithBuckets_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+47) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+14);
  CYC(b_+14, b_+15); H = D;
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_INVINCIBILITY_COUNTER;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x00);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0xff);
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+29); SET_HL(b_+83 /* @scriptTable */);
  CYC(b_+29, b_+30); subrosianWithBuckets_add_double_index(gb, b_+30);
  CYC(b_+30, b_+31); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+31, b_+32); H = mem_rd(gb, HL);
  CYC(b_+32, b_+33); L = A;
  CALL_C(b_+33, s_interactionSetScript, SYM(interactionSetScript), b_+36);
  CALL_C(b_+36, s_interactionRunScript, SYM(interactionRunScript), b_+39);
  CALL_C(b_+39, s_interactionRunScript, SYM(interactionRunScript), b_+42);
  if (F & FC) { CYCT(b_+42, b_+45); TAIL(interactionDelete); }
  CYC(b_+42, b_+45);
  CYC(b_+45, b_+47);
  goto animateAsNpc;
state1:
  CYC(b_+47, b_+50); A = mem_rd(gb, wActiveGroup);
  CYC(b_+50, b_+51); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto L_6350; }
  CYC(b_+51, b_+53);
  CALL_C(b_+53, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+56);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x00);
L_6350:
  CALL_C(b_+58, s_interactionRunScript, SYM(interactionRunScript), b_+61);
  CYC(b_+61, b_+63); C = 0x28;
  CALL_C(b_+63, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+66);
  if (F & FC) { CYCT(b_+66, b_+68); goto L_635c; }
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+70); A = 0x04;
L_635c:
  CYC(b_+70, b_+72); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+72, b_+73); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+73, b_+75); goto animateAsNpc; }
  CYC(b_+73, b_+75);
  CYC(b_+75, b_+76); mem_wr(gb, HL, A);
  CYC(b_+76, b_+77); alu_rrca(gb);
  CALL_C(b_+77, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+80);
animateAsNpc:
  CYC(b_+80, b_+83);
  TAIL(interactionAnimateAsNpc);
}

