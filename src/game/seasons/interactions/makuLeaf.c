#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/makuLeaf.s.
// INTERAC_MAKU_LEAF
// leaves during maku tree cutscenes
// Variables:
// var03: pointer to another interactionCode48
// var3a:
// var3b:
// var3c:

static uint16_t makuLeaf_jump_table(GB *gb) {
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

static void makuLeaf_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void makuLeaf_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode48@func_7018
static void makuLeaf_func_7018(GB *gb) {
  BASE(interactionCode48);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto func_7018;
func_7012:
  CYC(b_+185, b_+187); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+187, b_+189); A = 0x0b;
  CYC(b_+189, b_+190); mem_wr(gb, DE, A);
  RET(b_+190); return;
func_7018:
  CYC(b_+191, b_+194); SET_BC(0x8402);
  CALL_C(b_+194, s_objectCreateInteraction, SYM(objectCreateInteraction), b_+197);
  CYC(b_+197, b_+199);
  goto func_7012;
}

// interactionCode48@func_7012
static void makuLeaf_func_7012(GB *gb) {
  BASE(interactionCode48);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+185, b_+187); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+187, b_+189); A = 0x0b;
  CYC(b_+189, b_+190); mem_wr(gb, DE, A);
  RET(b_+190); return;
}

// interactionCode48@func_6fee
static void makuLeaf_func_6fee(GB *gb) {
  BASE(interactionCode48);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+149, b_+151); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+151, b_+152); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+152, b_+153); mem_wr(gb, DE, A);
  CYC(b_+153, b_+155); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+155, b_+156); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+156, b_+157); mem_wr(gb, DE, A);
  CYC(b_+157, b_+159); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+159, b_+160); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+160, b_+161); mem_wr(gb, DE, A);
  CYC(b_+161, b_+163); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+163, b_+164); A = L;
  CYC(b_+164, b_+165); mem_wr(gb, DE, A);
  CYC(b_+165, b_+166); E = alu_inc8(gb, E);
  CYC(b_+166, b_+167); A = H;
  CYC(b_+167, b_+168); mem_wr(gb, DE, A);
  RET(b_+168); return;
}

// INTERAC_MAKU_LEAF
// leaves during maku tree cutscenes
// Variables:
// var03: pointer to another interactionCode48
// var3a:
// var3b:
// var3c:
void s_interactionCode48_hook(GB *gb) {
  BASE(interactionCode48);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (makuLeaf_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == b_+30) goto state1;
    if (jt_ == b_+61) goto state2;
    if (jt_ == b_+103) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+14); L = E;
  CYC(b_+14, b_+15); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x01);
  CYC(b_+19, b_+21); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+26, b_+27); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+27, b_+30);
  TAIL(interactionInitGraphics);
state1:
  CYC(b_+30, b_+31); H = D;
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+33, b_+34); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+34); return; }
  CYC(b_+34, b_+35);
  CALL_C(b_+35, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+38);
  if (!(F & FZ)) { RET_TAKEN(b_+38); return; }
  CYC(b_+38, b_+39);
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x48);
  CYC(b_+41, b_+43); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+45); L = E;
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+47); L = alu_dec8(gb, L);
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+51); A = alu_inc8(gb, A);
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
  CYC(b_+53, b_+55); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+55, b_+58); TAIL(interactionDelete); }
  CYC(b_+55, b_+58);
  CYC(b_+58, b_+61);
  goto func_7002;
state2:
  CYC(b_+61, b_+63); A = 0x03;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CYC(b_+64, b_+65); H = D;
  CYC(b_+65, b_+67); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+67, b_+68); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+68, b_+69); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+69, b_+72); SET_HL(b_+197 /* @table_701e */);
  CYC(b_+72, b_+73); makuLeaf_add_double_index(gb, b_+73);
  CYC(b_+73, b_+75); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+75, b_+76); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+76, b_+77); mem_wr(gb, DE, A);
  CYC(b_+77, b_+79); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+79, b_+80); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  CALL_L(b_+81, makuLeaf_func_7012, b_+84);
  CYC(b_+84, b_+87); SET_HL(b_+211 /* @table_702c */);
  CALL_L(b_+87, makuLeaf_func_6fee, b_+90);
  CYC(b_+90, b_+92); A = 0x83;
  CALL_C(b_+92, s_playSound, SYM(playSound_b00), b_+95);
  CYC(b_+95, b_+97); A = 0x70;
  CYC(b_+97, b_+99); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  CYC(b_+100, b_+103);
  TAIL(objectSetVisible80);
state3:
  CALL_C(b_+103, s_objectApplySpeed, SYM(objectApplySpeed), b_+106);
  CYC(b_+106, b_+107); H = D;
  CYC(b_+107, b_+109); L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+109, b_+110); A = mem_rd(gb, HL);
  CYC(b_+110, b_+111); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+111, b_+113); goto L_6fd0; }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+114); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+114, b_+116); A = 0x83;
  if (F & FZ) CALL_C_CC(b_+116, s_playSound, SYM(playSound_b00), b_+119);
  else CYC(b_+116, b_+119);
L_6fd0:
  CYC(b_+119, b_+121); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+121, b_+122); A = mem_rd(gb, HL);
  CYC(b_+122, b_+124); alu_and(gb, 0xf0);
  CYC(b_+124, b_+126); alu_cp(gb, 0xf0);
  if (F & FZ) { CYCT(b_+126, b_+129); TAIL(interactionDelete); }
  CYC(b_+126, b_+129);
  CYC(b_+129, b_+131); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+131, b_+132); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) CALL_L_CC(b_+132, makuLeaf_func_7018, b_+135);
  else CYC(b_+132, b_+135);
  CALL_C(b_+135, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+138);
  if (!(F & FZ)) { RET_TAKEN(b_+138); return; }
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+141); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+141, b_+142); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+142, b_+143); H = mem_rd(gb, HL);
  CYC(b_+143, b_+144); L = A;
  CYC(b_+144, b_+145); A = mem_rd(gb, HL);
  CYC(b_+145, b_+146); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+146, b_+149); TAIL(interactionDelete); }
  CYC(b_+146, b_+149);
  CYC(b_+149, b_+151); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+151, b_+152); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+152, b_+153); mem_wr(gb, DE, A);
  CYC(b_+153, b_+155); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+155, b_+156); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+156, b_+157); mem_wr(gb, DE, A);
  CYC(b_+157, b_+159); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+159, b_+160); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+160, b_+161); mem_wr(gb, DE, A);
  CYC(b_+161, b_+163); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+163, b_+164); A = L;
  CYC(b_+164, b_+165); mem_wr(gb, DE, A);
  CYC(b_+165, b_+166); E = alu_inc8(gb, E);
  CYC(b_+166, b_+167); A = H;
  CYC(b_+167, b_+168); mem_wr(gb, DE, A);
  RET(b_+168); return;
func_7002:
  CYC(b_+169, b_+171); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+171, b_+172); A = mem_rd(gb, DE);
  CYC(b_+172, b_+175); SET_HL(b_+181 /* @table_700e */);
  CYC(b_+175, b_+176); makuLeaf_add_a_to_hl(gb, b_+176);
  CYC(b_+176, b_+177); A = mem_rd(gb, HL);
  CYC(b_+177, b_+179); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+179, b_+180); mem_wr(gb, DE, A);
  RET(b_+180); return;
}

