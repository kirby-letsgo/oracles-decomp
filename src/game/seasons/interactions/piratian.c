#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/piratian.s.
// INTERAC_PIRATIAN
// INTERAC_PIRATIAN_CAPTAIN

static uint16_t piratian_jump_table(GB *gb) {
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

static void piratian_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void piratian_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode41@func_6b91
static void piratian_func_6b91(GB *gb) {
  BASE(interactionCode40);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+116, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+119);
  CYC(b_+119, b_+120); H = D;
  CYC(b_+120, b_+122); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+122, b_+124); mem_wr(gb, HL, 0x01);
  CYC(b_+124, b_+126); A = 0x3a;
  CALL_C(b_+126, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+129);
  CALL_C(b_+129, s_func_6c29_hook, SYM(func_6c29), b_+132);
  CYC(b_+132, b_+134); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+134, b_+135); A = mem_rd(gb, DE);
  CYC(b_+135, b_+138); SET_HL(SYM(table_6cbf));
  CYC(b_+138, b_+139); piratian_add_double_index(gb, b_+139);
  CYC(b_+139, b_+140); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+140, b_+141); H = mem_rd(gb, HL);
  CYC(b_+141, b_+142); L = A;
  CALL_C(b_+142, s_interactionSetScript, SYM(interactionSetScript), b_+145);
  CYC(b_+145, b_+148);
  TAIL(interactionRunScript);
}

// INTERAC_PIRATIAN
// INTERAC_PIRATIAN_CAPTAIN
void s_interactionCode40_hook(GB *gb) {
  BASE(interactionCode40);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (piratian_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+148) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (piratian_jump_table(gb));
    if (jt_ == b_+42) goto state0_subid0;
    if (jt_ == b_+45) goto state0_subid1;
    if (jt_ == b_+58) goto state0_subid7;
    if (jt_ == b_+97) goto state0_subidA;
    if (jt_ == b_+65) goto state0_subidB;
    HANDOFF(HL);
  } while (0);
state0_subid0:
  CALL_C(b_+42, s_func_6c3c_hook, SYM(func_6c3c), b_+45);
state0_subid1:
  CYC(b_+45, b_+47); A = 0x13;
  CALL_C(b_+47, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+50);
  if (!(F & FZ)) { CYCT(b_+50, b_+53); TAIL(interactionDelete); }
  CYC(b_+50, b_+53);
  CALL_C(b_+53, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+56);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x00);
state0_subid7:
  CALL_L(b_+58, piratian_func_6b91, b_+61);
  CYC(b_+61, b_+63); A = 0x04;
  CYC(b_+63, b_+65);
  goto func_6b8b;
state0_subidB:
  CYC(b_+65, b_+67); A = 0x13;
  CALL_C(b_+67, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+70);
  if (F & FZ) { CYCT(b_+70, b_+73); TAIL(interactionDelete); }
  CYC(b_+70, b_+73);
  CYC(b_+73, b_+75); A = 0x17;
  CALL_C(b_+75, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+78);
  if (!(F & FZ)) { CYCT(b_+78, b_+81); TAIL(interactionDelete); }
  CYC(b_+78, b_+81);
  CALL_L(b_+81, piratian_func_6b91, b_+84);
  CYC(b_+84, b_+86); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+86, b_+87); A = mem_rd(gb, DE);
  CYC(b_+87, b_+89); alu_cp(gb, 0x0d);
  CYC(b_+89, b_+91); A = 0x00;
  if (F & FZ) { CYCT(b_+91, b_+93); goto L_6b7c; }
  CYC(b_+91, b_+93);
  CYC(b_+93, b_+95); A = 0x04;
L_6b7c:
  CYC(b_+95, b_+97);
  goto func_6b8b;
state0_subidA:
  CALL_C(b_+97, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+100);
  CYC(b_+100, b_+102); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+102, b_+105); TAIL(interactionDelete); }
  CYC(b_+102, b_+105);
  CALL_L(b_+105, piratian_func_6b91, b_+108);
  CYC(b_+108, b_+110); A = 0x04;
func_6b8b:
  CALL_C(b_+110, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+113);
  CYC(b_+113, b_+116);
  TAIL_S(func_6bc4);

state1:
  CYC(b_+148, b_+150); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+150, b_+151); A = mem_rd(gb, DE);
  CYC(b_+151, b_+153); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+153, b_+155); goto L_6bbb; }
  CYC(b_+153, b_+155);
  CALL_C(b_+155, s_func_6c51_hook, SYM(func_6c51), b_+158);
L_6bbb:
  CALL_C(b_+158, s_interactionRunScript, SYM(interactionRunScript), b_+161);
  if (F & FC) { CYCT(b_+161, b_+164); TAIL(interactionDelete); }
  CYC(b_+161, b_+164);
  CYC(b_+164, b_+167);
  TAIL_S(func_6bc4);
}

void s_func_6bc4_hook(GB *gb) {
  BASE(func_6bc4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_interactionAnimate, SYM(interactionAnimate), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); TAIL_S(func_6be9); }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto L_6be3; }
  CYC(b_+13, b_+15);
  CALL_C(b_+15, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0x1f);
  CYC(b_+20, b_+23); SET_HL(SYM(table_6c09));
  CYC(b_+23, b_+24); piratian_add_a_to_hl(gb, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+26); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+26, b_+28); goto L_6be3; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); E = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
L_6be3:
  CALL_C(b_+31, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+34);
  CYC(b_+34, b_+37);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}

void s_func_6be9_hook(GB *gb) {
  BASE(func_6be9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x28);
  if (F & FZ) { CYCT(b_+5, b_+7); goto L_6bf5; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 0x50);
  if (F & FZ) { CYCT(b_+9, b_+11); goto L_6bfd; }
  CYC(b_+9, b_+11);
  RET(b_+11); return;
L_6bf5:
  CYC(b_+12, b_+14); E = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+17); alu_cp(gb, 0x09);
  if (!(F & FZ)) { RET_TAKEN(b_+17); return; }
  CYC(b_+17, b_+18);
  CYC(b_+18, b_+20);
  goto L_6c03;
L_6bfd:
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+25); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { RET_TAKEN(b_+25); return; }
  CYC(b_+25, b_+26);
L_6c03:
  CYC(b_+26, b_+28); E = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  CYC(b_+28, b_+30); A = 0x01;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  RET(b_+31); return;
}

void s_func_6c29_hook(GB *gb) {
  BASE(func_6c29);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x40;
  CALL_C(b_+2, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); goto L_6c31; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
L_6c31:
  CYC(b_+8, b_+10); alu_cp(gb, 0x20);
  CYC(b_+10, b_+12); A = 0x01;
  if (!(F & FC)) { CYCT(b_+12, b_+14); goto L_6c38; }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); alu_xor(gb, A);
L_6c38:
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+17, b_+18); mem_wr(gb, DE, A);
  RET(b_+18); return;
}

void s_func_6c3c_hook(GB *gb) {
  BASE(func_6c3c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x4a;
  CALL_C(b_+2, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); goto L_6c46; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+10);
  goto L_6c4d;
L_6c46:
  CYC(b_+10, b_+11); alu_or(gb, A);
  CYC(b_+11, b_+13); A = 0x01;
  if (F & FZ) { CYCT(b_+13, b_+15); goto L_6c4d; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); A = 0x02;
L_6c4d:
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
}

void s_func_6c51_hook(GB *gb) {
  BASE(func_6c51);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_func_6c8b_hook, SYM(func_6c8b), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); goto L_6c6a; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+8, b_+10); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(b_+10, b_+12); goto L_6c64; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+14); C = 0x01;
  CYC(b_+14, b_+16); B = 0xdb;
  CYC(b_+16, b_+19);
  TAIL_S(func_6c78);
L_6c64:
  CYC(b_+19, b_+22); A = mem_rd(gb, wGameKeysPressed);
  CYC(b_+22, b_+24); alu_bit(gb, 6, A);
  if (!(F & FZ)) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
L_6c6a:
  CYC(b_+25, b_+26); H = D;
  CYC(b_+26, b_+28); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+28, b_+30); A = 0x00;
  CYC(b_+30, b_+31); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+31); return; }
  CYC(b_+31, b_+32);
  CYC(b_+32, b_+34); C = 0x00;
  CYC(b_+34, b_+36); B = 0xd9;
  CYC(b_+36, b_+39);
  TAIL_S(func_6c78);
}

void s_func_6c78_hook(GB *gb) {
  BASE(func_6c78);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+3, b_+4); mem_wr(gb, HL, C);
  CYC(b_+4, b_+6); A = 0x05;
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+8, b_+9); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+11); A = B;
  CALL_C(b_+11, s_setTile, SYM(setTile), b_+14);
  CYC(b_+14, b_+16); A = 0x70;
  CYC(b_+16, b_+19);
  TAIL(playSound_b00);
}

void s_func_6c8b_hook(GB *gb) {
  BASE(func_6c8b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(table_6cb2));
  CYC(b_+3, b_+6); A = mem_rd(gb, w1Link_yh);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+10); A = mem_rd(gb, w1Link_xh);
  CYC(b_+10, b_+11); B = A;
L_6c96:
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); alu_add(gb, 0x04);
  CYC(b_+16, b_+17); alu_sub(gb, C);
  CYC(b_+17, b_+19); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+19, b_+21); goto L_6cae; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+24); alu_add(gb, 0x03);
  CYC(b_+24, b_+25); alu_sub(gb, B);
  CYC(b_+25, b_+27); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(b_+27, b_+29); goto L_6caf; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+34); alu_or(gb, D);
  RET(b_+34); return;
L_6cae:
  CYC(b_+35, b_+36); SET_HL(HL + 1);
L_6caf:
  CYC(b_+36, b_+37); SET_HL(HL + 1);
  CYC(b_+37, b_+39);
  goto L_6c96;
}

