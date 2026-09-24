#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/troy.s.
// INTERAC_TROY

static uint16_t troy_jump_table(GB *gb) {
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

static void troy_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// INTERAC_TROY
void s_interactionCodeca_hook(GB *gb) {
  BASE(interactionCodeca);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (troy_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+68) goto state1;
    if (jt_ == b_+79) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+12); A = 0x28;
  CALL_C(b_+12, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+15);
  if (F & FZ) { CYCT(b_+15, b_+18); TAIL(interactionDelete); }
  CYC(b_+15, b_+18);
  CALL_C(b_+18, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+21);
  CALL_C(b_+21, s_interactionIncState, SYM(interactionIncState), b_+24);
  CALL_C(b_+24, s_func_7aa7_hook, SYM(func_7aa7), b_+27);
  CYC(b_+27, b_+29); A = 0x4c;
  CALL_C(b_+29, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+32);
  CYC(b_+32, b_+34); A = 0x5a;
  CALL_C(b_+34, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+37);
  if (F & FZ) { CYCT(b_+37, b_+39); goto L_7971; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+42); SET_HL(SYM(troyScript_doneSecret));
  CYC(b_+42, b_+44);
  goto L_797e;
L_7971:
  CYC(b_+44, b_+46); A = 0x50;
  CALL_C(b_+46, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+49);
  CYC(b_+49, b_+52); SET_HL(SYM(troyScript_beginningSecret));
  if (F & FZ) { CYCT(b_+52, b_+54); goto L_797e; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+57); SET_HL(SYM(troyScript_beganSecret));
L_797e:
  CALL_C(b_+57, s_interactionSetScript, SYM(interactionSetScript), b_+60);
  CYC(b_+60, b_+62); A = 0x02;
  CALL_C(b_+62, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+65);
  CYC(b_+65, b_+68);
  TAIL(interactionAnimateAsNpc);
state1:
  CALL_C(b_+68, s_interactionRunScript, SYM(interactionRunScript), b_+71);
  CYC(b_+71, b_+73); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+75); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+75); return; }
  CYC(b_+75, b_+76);
  CYC(b_+76, b_+79);
  TAIL_SG(npcFaceLinkAndAnimate);
state2:
  CALL_C(b_+79, s_func_79df_hook, SYM(func_79df), b_+82);
  CALL_C(b_+82, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+85);
  if (!(F & FZ)) { CYCT(b_+85, b_+87); goto L_79a1; }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0xb4);
  CALL_C(b_+89, s_func_7a0d_hook, SYM(func_7a0d), b_+92);
L_79a1:
  CYC(b_+92, b_+95); SET_HL(wcce2);
  CYC(b_+95, b_+96); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+96, b_+98); alu_cp(gb, 0x30);
  if (!(F & FZ)) { CYCT(b_+98, b_+100); goto L_79b2; }
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+101); A = mem_rd(gb, HL);
  CYC(b_+101, b_+103); alu_cp(gb, 0x00);
  if (!(F & FZ)) { CYCT(b_+103, b_+105); goto L_79b2; }
  CYC(b_+103, b_+105);
  CYC(b_+105, b_+107); A = 0x01;
  CYC(b_+107, b_+109);
  goto L_79c0;
L_79b2:
  CYC(b_+109, b_+110); H = D;
  CYC(b_+110, b_+112); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+112, b_+113); A = mem_rd(gb, HL);
  CYC(b_+113, b_+115); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { RET_TAKEN(b_+115); return; }
  CYC(b_+115, b_+116);
  CYC(b_+116, b_+119); A = mem_rd(gb, wNumEnemies);
  CYC(b_+119, b_+120); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+120); return; }
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+123); A = 0x00;
L_79c0:
  CYC(b_+123, b_+124); H = D;
  CYC(b_+124, b_+126); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+126, b_+127); mem_wr(gb, HL, A);
  CYC(b_+127, b_+129); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+129, b_+131); mem_wr(gb, HL, 0x01);
  CYC(b_+131, b_+134); SET_HL(SYM(linkedFunc_15_6430));
  CYC(b_+134, b_+136); E = 0x15;
  CALL_C(b_+136, s_interBankCall, SYM(interBankCall), b_+139);
  CYC(b_+139, b_+142); SET_HL(SYM(troyScript_gameBegun));
  CALL_C(b_+142, s_interactionSetScript, SYM(interactionSetScript), b_+145);
  RET(b_+145); return;
}

// func_7a01+2
static void troy_func_7a01_2(GB *gb) {
  BASE(func_7a01);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+5); alu_add(gb, 0x01);
  CYC(b_+5, b_+6); alu_daa(gb);
  CYC(b_+6, b_+7); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto L_7a0b; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); alu_xor(gb, A);
L_7a0b:
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  RET(b_+11); return;
}

void s_func_79df_hook(GB *gb) {
  BASE(func_79df);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(wcce1);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+6); alu_cp(gb, 0x59);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto L_79f0; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+11); alu_cp(gb, 0x59);
  if (!(F & FZ)) { CYCT(b_+11, b_+13); goto L_79f0; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); A = mem_rd(gb, HL);
  CYC(b_+14, b_+16); alu_cp(gb, 0x99);
  if (F & FZ) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
L_79f0:
  CYC(b_+17, b_+20); SET_HL(wcce1);
  CALL_C(b_+20, s_func_7a01_hook, SYM(func_7a01), b_+23);
  if (!(F & FZ)) { RET_TAKEN(b_+23); return; }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); SET_HL(HL + 1);
  CALL_C(b_+25, s_func_7a01_hook, SYM(func_7a01), b_+28);
  if (!(F & FZ)) { RET_TAKEN(b_+28); return; }
  CYC(b_+28, b_+29);
  CYC(b_+29, b_+30); SET_HL(HL + 1);
  CYC(b_+30, b_+32); B = 0x00;
  CYC(b_+32, b_+34);
  troy_func_7a01_2(gb); return;
}

void s_func_7a01_hook(GB *gb) {
  BASE(func_7a01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x60;
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+5); alu_add(gb, 0x01);
  CYC(b_+5, b_+6); alu_daa(gb);
  CYC(b_+6, b_+7); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto L_7a0b; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); alu_xor(gb, A);
L_7a0b:
  CYC(b_+10, b_+11); mem_wr(gb, HL, A);
  RET(b_+11); return;
}

// func_7a3a+2
static void troy_func_7a3a_2(GB *gb) {
  BASE(func_7a3a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  RET(b_+2); return;
}

void s_func_7a0d_hook(GB *gb) {
  BASE(func_7a0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x04;
  CYC(b_+2, b_+5); SET_HL(wNumEnemies);
  CYC(b_+5, b_+6); alu_sub(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); mem_wr(gb, hFF8D, A);
  CALL_C(b_+9, s_getRandomNumber, SYM(getRandomNumber), b_+12);
  CYC(b_+12, b_+14); alu_and(gb, 0x03);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  CYC(b_+17, b_+18); alu_xor(gb, A);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+21); mem_wr(gb, hFF8B, A);
  CYC(b_+21, b_+22); H = D;
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+27, b_+29); troy_func_7a3a_2(gb); return; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+34); SET_HL(SYM(table_7a3d) - 1);
  CYC(b_+34, b_+35); troy_add_a_to_hl(gb, b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CALL_C(b_+36, s_func_7a49_hook, SYM(func_7a49), b_+39);
  CYC(b_+39, b_+41); A = mem_rd(gb, hFF8B);
  CYC(b_+41, b_+44); SET_HL(hFF8D);
  CYC(b_+44, b_+45); alu_cp(gb, mem_rd(gb, HL));
  s_func_7a3a_hook(gb); return; // falls through
}

// func_7a0d+18
static void troy_func_7a0d_18(GB *gb) {
  BASE(func_7a0d);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+21); mem_wr(gb, hFF8B, A);
  CYC(b_+21, b_+22); H = D;
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+27, b_+29); troy_func_7a3a_2(gb); return; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); A = alu_inc8(gb, A);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+34); SET_HL(SYM(table_7a3d) - 1);
  CYC(b_+34, b_+35); troy_add_a_to_hl(gb, b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CALL_C(b_+36, s_func_7a49_hook, SYM(func_7a49), b_+39);
  CYC(b_+39, b_+41); A = mem_rd(gb, hFF8B);
  CYC(b_+41, b_+44); SET_HL(hFF8D);
  CYC(b_+44, b_+45); alu_cp(gb, mem_rd(gb, HL));
  s_func_7a3a_hook(gb); return; // falls through
}

void s_func_7a3a_hook(GB *gb) {
  BASE(func_7a3a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(b_+0, b_+2); troy_func_7a0d_18(gb); return; }
  CYC(b_+0, b_+2);
  RET(b_+2); return;
}

void s_func_7a49_hook(GB *gb) {
  BASE(func_7a49);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_BC(SYM(table_7a76));
  CALL_C(b_+3, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+6);
  CALL_C(b_+6, s_getFreeEnemySlot, SYM(getFreeEnemySlot), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, BC);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+12, b_+13); SET_BC(BC + 1);
  CYC(b_+13, b_+14); A = mem_rd(gb, BC);
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); A = alu_inc8(gb, A);
  CYC(b_+19, b_+21); alu_and(gb, 0x03);
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CYC(b_+22, b_+25); SET_BC(SYM(table_7a86));
  CALL_C(b_+25, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+28);
  CYC(b_+28, b_+30); L = ENEMY_BASE + OBJ_YH;
  CYC(b_+30, b_+31); A = mem_rd(gb, BC);
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+33); SET_BC(BC + 1);
  CYC(b_+33, b_+35); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+35, b_+36); A = mem_rd(gb, BC);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);
  CYC(b_+37, b_+39); L = ENEMY_BASE + OBJ_ID;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); alu_cp(gb, 0x10);
  if (F & FZ) { RET_TAKEN(b_+42); return; }
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+45);
  TAIL_S(func_7a8e);
}

void s_func_7a8e_hook(GB *gb) {
  BASE(func_7a8e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_BC(SYM(table_7a86));
  CALL_C(b_+6, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+9);
  CALL_C(b_+9, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+12);
  if (!(F & FZ)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x05);
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+17, b_+18); A = mem_rd(gb, BC);
  CYC(b_+18, b_+19); mem_wr(gb, HL, A);
  CYC(b_+19, b_+20); SET_BC(BC + 1);
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+22, b_+23); A = mem_rd(gb, BC);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  RET(b_+24); return;
}

void s_func_7aa7_hook(GB *gb) {
  BASE(func_7aa7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x05;
  CALL_C(b_+2, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+5);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto nobleSword; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+9, b_+12); goto nobleSword; }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+14); alu_sub(gb, 0x01);
L_7ab5:
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A);
  RET(b_+17); return;
nobleSword:
  CYC(b_+18, b_+20); A = 0x01;
  CYC(b_+20, b_+22);
  goto L_7ab5;
}

