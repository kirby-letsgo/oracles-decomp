#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/specialWarp.s.
// INTERAC_SPECIAL_WARP

static uint16_t specialWarp_jump_table(GB *gb) {
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

static void specialWarp_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_SPECIAL_WARP
void s_interactionCode1f_hook(GB *gb) {
  BASE(interactionCode1f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (specialWarp_jump_table(gb));
    if (jt_ == SYM(specialWarp_subid0) && hook_is(gb, SYM(specialWarp_subid0), s_specialWarp_subid0_hook)) { s_specialWarp_subid0_hook(gb); return; }
    if (jt_ == SYM(specialWarp_subid2) && hook_is(gb, SYM(specialWarp_subid2), s_specialWarp_subid2_hook)) { s_specialWarp_subid2_hook(gb); return; }
    if (jt_ == SYM(specialWarp_subid5) && hook_is(gb, SYM(specialWarp_subid5), s_specialWarp_subid5_hook)) { s_specialWarp_subid5_hook(gb); return; }
    if (jt_ == SYM(specialWarp_subid8) && hook_is(gb, SYM(specialWarp_subid8), s_specialWarp_subid8_hook)) { s_specialWarp_subid8_hook(gb); return; }
    if (jt_ == SYM(specialWarp_subidD) && hook_is(gb, SYM(specialWarp_subidD), s_specialWarp_subidD_hook)) { s_specialWarp_subidD_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
}

void s_specialWarp_subid0_hook(GB *gb) {
  BASE(specialWarp_subid0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto L_519f; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wScrollMode);
  CYC(b_+8, b_+10); alu_and(gb, 0x01);
  if (F & FZ) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CALL_C(b_+14, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+17);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x20);
L_519f:
  CYC(b_+19, b_+22); A = mem_rd(gb, wLinkInAir);
  CYC(b_+22, b_+23); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CALL_C(b_+24, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+27);
  CYC(b_+27, b_+30); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+30, b_+31); alu_cp(gb, L);
  if (!(F & FZ)) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+34); mem_wr(gb, HL, 0xeb);
  CYC(b_+34, b_+36); A = 0x81;
  CYC(b_+36, b_+39); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+39, b_+42);
  TAIL(interactionDelete);
}

void s_specialWarp_subid2_hook(GB *gb) {
  BASE(specialWarp_subid2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (specialWarp_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+71) goto state1;
    if (jt_ == b_+117) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_sub(gb, 0x02);
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+19); SET_HL(b_+47 /* @table_51e5 */);
  CYC(b_+19, b_+20); specialWarp_add_double_index(gb, b_+20);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+22, b_+24); B = 0x03;
  CALL_C(b_+24, s_copyMemory, SYM(copyMemory), b_+27);
  CYC(b_+27, b_+29); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); E = alu_dec8(gb, E);
  CYC(b_+32, b_+34); A = 0x0a;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CALL_C(b_+35, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+38);
  CYC(b_+38, b_+40); A = 0x01;
  if (!(F & FC)) { CYCT(b_+40, b_+42); goto L_51e1; }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); A = alu_inc8(gb, A);
L_51e1:
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  RET(b_+46); return;

state1:
  CYC(b_+71, b_+72); A = D;
  CYC(b_+72, b_+75); mem_wr(gb, wDisableWarpTiles, A);
  CYC(b_+75, b_+78); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+78, b_+80); alu_cp(gb, 0xd1);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; }
  CYC(b_+80, b_+81);
  CALL_C(b_+81, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+84);
  if (!(F & FC)) { RET_TAKEN(b_+84); return; }
  CYC(b_+84, b_+85);
  CYC(b_+85, b_+86); alu_xor(gb, A);
  CYC(b_+86, b_+89); mem_wr(gb, wWarpTransition, A);
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+92, b_+93); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+93, b_+96); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+96, b_+97); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+97, b_+100); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+104); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+104, b_+106); A = 0x03;
  CYC(b_+106, b_+109); mem_wr(gb, wWarpTransition2, A);
  CYC(b_+109, b_+111); A = 0x01;
  CYC(b_+111, b_+114); mem_wr(gb, wScrollMode, A);
  CYC(b_+114, b_+117);
  TAIL(interactionDelete);
state2:
  CALL_C(b_+117, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+120);
  if (F & FC) { RET_TAKEN(b_+120); return; }
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+123); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+123, b_+125); A = 0x01;
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  RET(b_+126); return;
}

// specialWarp_subid4@setWarpVariables
static void specialWarp_setWarpVariables(GB *gb) {
  BASE(specialWarp_subid2);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+92, b_+93); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+93, b_+96); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+96, b_+97); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+97, b_+100); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+104); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+104, b_+106); A = 0x03;
  CYC(b_+106, b_+109); mem_wr(gb, wWarpTransition2, A);
  CYC(b_+109, b_+111); A = 0x01;
  CYC(b_+111, b_+114); mem_wr(gb, wScrollMode, A);
  CYC(b_+114, b_+117);
  TAIL(interactionDelete);
}

// specialWarp_subid4@state0
static void specialWarp_state0(GB *gb) {
  BASE(specialWarp_subid2);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_sub(gb, 0x02);
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+19); SET_HL(b_+47 /* @table_51e5 */);
  CYC(b_+19, b_+20); specialWarp_add_double_index(gb, b_+20);
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+22, b_+24); B = 0x03;
  CALL_C(b_+24, s_copyMemory, SYM(copyMemory), b_+27);
  CYC(b_+27, b_+29); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); E = alu_dec8(gb, E);
  CYC(b_+32, b_+34); A = 0x0a;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CALL_C(b_+35, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+38);
  CYC(b_+38, b_+40); A = 0x01;
  if (!(F & FC)) { CYCT(b_+40, b_+42); goto L_51e1; }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); A = alu_inc8(gb, A);
L_51e1:
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+45, b_+46); mem_wr(gb, DE, A);
  RET(b_+46); return;
}

void s_specialWarp_subid5_hook(GB *gb) {
  BASE(specialWarp_subid5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (specialWarp_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+20) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_L(b_+10, specialWarp_state0, b_+13);
  CYC(b_+13, b_+14); alu_xor(gb, A);
  CYC(b_+14, b_+17); mem_wr(gb, wActiveMusic, A);
  CYC(b_+17, b_+20);
  TAIL(interactionSetAlwaysUpdateBit);
state1:
  CYC(b_+20, b_+21); A = D;
  CYC(b_+21, b_+24); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+24, b_+27); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+27, b_+29); alu_cp(gb, 0xd1);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+31); alu_xor(gb, A);
  CYC(b_+31, b_+34); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+34, b_+37); A = mem_rd(gb, wScrollMode);
  CYC(b_+37, b_+39); alu_and(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+39); return; }
  CYC(b_+39, b_+40);
  CYC(b_+40, b_+41); alu_xor(gb, A);
  CYC(b_+41, b_+44); mem_wr(gb, wScrollMode, A);
  CYC(b_+44, b_+46); A = 0xff;
  CYC(b_+46, b_+49); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+49, b_+52); mem_wr(gb, wActiveMusic, A);
  CYC(b_+52, b_+54);
  specialWarp_setWarpVariables(gb); return;
}

void s_specialWarp_subid8_hook(GB *gb) {
  BASE(specialWarp_subid8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto L_5278; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); A = 0x02;
  CALL_C(b_+10, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+13);
L_5278:
  CYC(b_+13, b_+16); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+16, b_+17); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CALL_C(b_+18, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+21);
  if (!(F & FC)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+27); alu_sub(gb, 0x08);
  CYC(b_+27, b_+30); SET_HL(SYM(table_52a4));
  CYC(b_+30, b_+31); specialWarp_add_double_index(gb, b_+31);
  CYC(b_+31, b_+32); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+32, b_+35); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+39); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+39, b_+41); A = 0x87;
  CYC(b_+41, b_+44); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+44, b_+46); A = 0x01;
  CYC(b_+46, b_+49); mem_wr(gb, wWarpTransition, A);
  CYC(b_+49, b_+51); A = 0x03;
  CYC(b_+51, b_+54); mem_wr(gb, wWarpTransition2, A);
  CYC(b_+54, b_+57);
  TAIL(interactionDelete);
}

// specialWarp_subidC@fadeoutTransition
static void specialWarp_fadeoutTransition(GB *gb) {
  BASE(specialWarp_subid8);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+49, b_+51); A = 0x03;
  CYC(b_+51, b_+54); mem_wr(gb, wWarpTransition2, A);
  CYC(b_+54, b_+57);
  TAIL(interactionDelete);
}

void s_specialWarp_subidD_hook(GB *gb) {
  BASE(specialWarp_subidD);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkInteractionState, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto L_52bb; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+10); A = 0x02;
  CALL_C(b_+10, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+13);
L_52bb:
  CYC(b_+13, b_+16); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+16, b_+17); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CALL_C(b_+18, s_objectCheckCollidedWithLink_notDeadAndNotGrabbing, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+21);
  if (!(F & FC)) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+25); SET_HL(wWarpDestGroup);
  CYC(b_+25, b_+27); mem_wr(gb, HL, 0x85);
  CYC(b_+27, b_+28); L = alu_inc8(gb, L);
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x12);
  CYC(b_+30, b_+31); L = alu_inc8(gb, L);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x05);
  CYC(b_+33, b_+34); L = alu_inc8(gb, L);
  CYC(b_+34, b_+36); mem_wr(gb, HL, 0x29);
  CYC(b_+36, b_+38);
  specialWarp_fadeoutTransition(gb); return;
}

