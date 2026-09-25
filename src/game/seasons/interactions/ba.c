#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/ba.s.
// INTERAC_ba

static uint16_t ba_jump_table(GB *gb) {
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

// interactionCodebb@subid0
static void ba_subid0(GB *gb) {
  BASE(interactionCodebb);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (ba_jump_table(gb));
    if (jt_ == b_+24) goto subid0_state0;
    if (jt_ == b_+33) goto subid0_state1;
    if (jt_ == b_+51) goto subid0_state2;
    if (jt_ == b_+71) goto subid0_state3;
    if (jt_ == b_+100) goto subid0_state4;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CALL_C(b_+24, s_func_7867_hook, SYM(func_7867), b_+27);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CALL_C(b_+30, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+33);
subid0_state1:
  CALL_C(b_+33, s_interactionRunScript, SYM(interactionRunScript), b_+36);
  CALL_C(b_+36, s_func_7886_hook, SYM(func_7886), b_+39);
  CYC(b_+39, b_+42); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+42, b_+44); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+44); return; }
  CYC(b_+44, b_+45);
  CALL_C(b_+45, s_func_788e_hook, SYM(func_788e), b_+48);
  CYC(b_+48, b_+51);
  TAIL(interactionIncState);
subid0_state2:
  CALL_C(b_+51, s_interactionRunScript, SYM(interactionRunScript), b_+54);
  CALL_C(b_+54, s_func_7886_hook, SYM(func_7886), b_+57);
  CALL_C(b_+57, s_decVar3c_hook, SYM(decVar3c), b_+60);
  if (!(F & FZ)) { RET_TAKEN(b_+60); return; }
  CYC(b_+60, b_+61);
  CYC(b_+61, b_+63); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+63, b_+64); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+64, b_+66); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x0a);
  CYC(b_+68, b_+71);
  TAIL_S(func_78c3);
subid0_state3:
  CALL_C(b_+71, s_interactionRunScript, SYM(interactionRunScript), b_+74);
  CALL_C(b_+74, s_func_7886_hook, SYM(func_7886), b_+77);
  CALL_C(b_+77, s_decVar3c_hook, SYM(decVar3c), b_+80);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; }
  CYC(b_+80, b_+81);
  CYC(b_+81, b_+83); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+83, b_+84); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+84, b_+86); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x28);
  CYC(b_+88, b_+90); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x58);
  CALL_C(b_+92, s_func_78b3_hook, SYM(func_78b3), b_+95);
  CYC(b_+95, b_+97); A = 0xd2;
  CYC(b_+97, b_+100);
  TAIL(playSound_b00);
subid0_state4:
  CALL_C(b_+100, s_decVar3c_hook, SYM(decVar3c), b_+103);
  if (F & FZ) { CYCT(b_+103, b_+106); TAIL(interactionDelete); }
  CYC(b_+103, b_+106);
  CALL_C(b_+106, s_objectApplySpeed, SYM(objectApplySpeed), b_+109);
  CALL_C(b_+109, s_interactionRunScript, SYM(interactionRunScript), b_+112);
  CYC(b_+112, b_+115);
  TAIL_S(func_7886);
}

// INTERAC_ba
void s_interactionCodeba_hook(GB *gb) {
  BASE(interactionCodeba);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (ba_jump_table(gb));
    if (jt_ == SYM(interactionCodebb__subid0)) { ba_subid0(gb); return; }
    if (jt_ == b_+12) goto subid1;
    if (jt_ == b_+35) goto subid3;
    HANDOFF(HL);
  } while (0);
subid1:
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (ba_jump_table(gb));
    if (jt_ == b_+20) goto subid2_state0;
    if (jt_ == b_+29) goto subid2_state1;
    HANDOFF(HL);
  } while (0);
subid2_state0:
  CALL_C(b_+20, s_func_7867_hook, SYM(func_7867), b_+23);
  CYC(b_+23, b_+25); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CALL_C(b_+26, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+29);
subid2_state1:
  CALL_C(b_+29, s_interactionRunScript, SYM(interactionRunScript), b_+32);
  CYC(b_+32, b_+35);
  TAIL_S(func_7886);
subid3:
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); push_effect(gb, b_+39);
  do { uint16_t jt_ = (ba_jump_table(gb));
    if (jt_ == b_+43) goto subid3_state0;
    if (jt_ == b_+46) goto subid3_state1;
    HANDOFF(HL);
  } while (0);
subid3_state0:
  CALL_C(b_+43, s_func_7867_hook, SYM(func_7867), b_+46);
subid3_state1:
  CALL_C(b_+46, s_interactionRunScript, SYM(interactionRunScript), b_+49);
  if (F & FC) { CYCT(b_+49, b_+52); TAIL(interactionDelete); }
  CYC(b_+49, b_+52);
  CYC(b_+52, b_+55);
  TAIL_S(func_7886);
}

