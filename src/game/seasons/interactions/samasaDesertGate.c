#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/samasaDesertGate.s.
// INTERAC_SAMASA_DESERT_GATE

static uint16_t samasaDesertGate_jump_table(GB *gb) {
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

// INTERAC_SAMASA_DESERT_GATE
void s_interactionCode9e_hook(GB *gb) {
  BASE(interactionCode9e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (samasaDesertGate_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+31) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+8, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+11);
  CYC(b_+11, b_+13); alu_bit(gb, 7, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+13, b_+16); TAIL(interactionDelete); }
  CYC(b_+13, b_+16);
  CALL_C(b_+16, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+19);
  CALL_C(b_+19, s_interactionIncState, SYM(interactionIncState), b_+22);
  CYC(b_+22, b_+25); SET_BC(SYM(table_604d));
  CYC(b_+25, b_+27); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+27, b_+28); mem_wr(gb, HL, B);
  CYC(b_+28, b_+29); SET_HL(HL + 1);
  CYC(b_+29, b_+30); mem_wr(gb, HL, C);
  RET(b_+30); return;
state1:
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CYC(b_+34, b_+35); push_effect(gb, b_+35);
  do { uint16_t jt_ = (samasaDesertGate_jump_table(gb));
    if (jt_ == b_+45) goto substate0;
    if (jt_ == b_+77) goto substate1;
    if (jt_ == b_+93) goto substate2;
    if (jt_ == b_+108) goto substate3;
    if (jt_ == b_+141) goto substate4;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+45, s_func_5f8c_hook, SYM(func_5f8c), b_+48);
  CYC(b_+48, b_+50); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+53); alu_cp(gb, 0xff);
  if (!(F & FZ)) { RET_TAKEN(b_+53); return; }
  CYC(b_+53, b_+54);
  CALL_C(b_+54, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+57);
  CYC(b_+57, b_+59); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x28);
  CYC(b_+61, b_+63); A = 0x81;
  CYC(b_+63, b_+66); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+66, b_+68); A = 0x80;
  CYC(b_+68, b_+71); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+71, b_+74); SET_HL(SYM(script7556));
  CYC(b_+74, b_+77);
  TAIL(interactionSetScript);
substate1:
  CALL_C(b_+77, s_func_5f87_hook, SYM(func_5f87), b_+80);
  if (!(F & FZ)) { RET_TAKEN(b_+80); return; }
  CYC(b_+80, b_+81);
  CALL_C(b_+81, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+84);
  CYC(b_+84, b_+86); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x78);
  CYC(b_+88, b_+90); A = 0xb8;
  CYC(b_+90, b_+93);
  TAIL(playSound_b00);
substate2:
  CALL_C(b_+93, s_func_606a_hook, SYM(func_606a), b_+96);
  CALL_C(b_+96, s_func_5f87_hook, SYM(func_5f87), b_+99);
  if (!(F & FZ)) { RET_TAKEN(b_+99); return; }
  CYC(b_+99, b_+100);
  CALL_C(b_+100, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+103);
  CYC(b_+103, b_+105); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+105, b_+107); mem_wr(gb, HL, 0x01);
  RET(b_+107); return;
substate3:
  CALL_C(b_+108, s_interactionRunScript, SYM(interactionRunScript), b_+111);
  CALL_C(b_+111, s_func_606a_hook, SYM(func_606a), b_+114);
  CALL_C(b_+114, s_func_5f87_hook, SYM(func_5f87), b_+117);
  if (!(F & FZ)) { RET_TAKEN(b_+117); return; }
  CYC(b_+117, b_+118);
  CALL_C(b_+118, s_func_602c_hook, SYM(func_602c), b_+121);
  if (F & FZ) { CYCT(b_+121, b_+123); goto func_5f67; }
  CYC(b_+121, b_+123);
  CYC(b_+123, b_+124); H = D;
  CYC(b_+124, b_+126); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+126, b_+128); mem_wr(gb, HL, 0x32);
  CYC(b_+128, b_+130); A = 0x6f;
  CYC(b_+130, b_+133);
  TAIL(playSound_b00);
func_5f67:
  CALL_C(b_+133, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+136);
  CYC(b_+136, b_+138); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+138, b_+140); mem_wr(gb, HL, 0x28);
  RET(b_+140); return;
substate4:
  CALL_C(b_+141, s_func_5f87_hook, SYM(func_5f87), b_+144);
  if (!(F & FZ)) { RET_TAKEN(b_+144); return; }
  CYC(b_+144, b_+145);
  CYC(b_+145, b_+146); alu_xor(gb, A);
  CYC(b_+146, b_+149); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+149, b_+152); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+152, b_+154); A = 0x4d;
  CALL_C(b_+154, s_playSound, SYM(playSound_b00), b_+157);
  CALL_C(b_+157, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+160);
  CYC(b_+160, b_+162); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+162, b_+165);
  TAIL(interactionDelete);
}

void s_func_5f87_hook(GB *gb) {
  BASE(func_5f87);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+4); return;
}

// func_5fa3+6
static void samasaDesertGate_func_5fa3_6(GB *gb) {
  BASE(func_5fa3);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+9, b_+11); A = 0x00;
  CYC(b_+11, b_+12); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); C = 0x00;
  CYC(b_+15, b_+17); B = 0xb1;
  CALL_C(b_+17, s_func_5fba_hook, SYM(func_5fba), b_+20);
  CYC(b_+20, b_+23);
  TAIL_S(func_6001);
}

void s_func_5f8c_hook(GB *gb) {
  BASE(func_5f8c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_func_5fcd_hook, SYM(func_5fcd), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+5); samasaDesertGate_func_5fa3_6(gb); return; }
  CYC(b_+3, b_+5);
  CALL_C(b_+5, s_checkLinkID0AndControlNormal, SYM(checkLinkID0AndControlNormal), b_+8);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+12); A = mem_rd(gb, wGameKeysJustPressed);
  CYC(b_+12, b_+14); alu_bit(gb, 6, A);
  if (F & FZ) { CYCT(b_+14, b_+16); TAIL_S(func_5fa3); }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+18); C = 0x01;
  CYC(b_+18, b_+20); B = 0xb0;
  CYC(b_+20, b_+23);
  TAIL_S(func_5fba);
}

void s_func_5fa3_hook(GB *gb) {
  BASE(func_5fa3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wGameKeysPressed);
  CYC(b_+3, b_+5); alu_bit(gb, 6, A);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+9, b_+11); A = 0x00;
  CYC(b_+11, b_+12); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); C = 0x00;
  CYC(b_+15, b_+17); B = 0xb1;
  CALL_C(b_+17, s_func_5fba_hook, SYM(func_5fba), b_+20);
  CYC(b_+20, b_+23);
  TAIL_S(func_6001);
}

void s_func_5fba_hook(GB *gb) {
  BASE(func_5fba);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+3, b_+4); mem_wr(gb, HL, C);
  CYC(b_+4, b_+6); A = 0x13;
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+8, b_+9); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+10); C = A;
  CYC(b_+10, b_+11); A = B;
  CALL_C(b_+11, s_setTile, SYM(setTile), b_+14);
  CYC(b_+14, b_+16); A = 0x70;
  CYC(b_+16, b_+19);
  TAIL(playSound_b00);
}

void s_func_5fcd_hook(GB *gb) {
  BASE(func_5fcd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(table_5ff4));
  CYC(b_+3, b_+6); A = mem_rd(gb, w1Link_yh);
  CYC(b_+6, b_+7); C = A;
  CYC(b_+7, b_+10); A = mem_rd(gb, w1Link_xh);
  CYC(b_+10, b_+11); B = A;
L_5fd8:
  CYC(b_+11, b_+12); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+12, b_+13); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+16); alu_add(gb, 0x04);
  CYC(b_+16, b_+17); alu_sub(gb, C);
  CYC(b_+17, b_+19); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+19, b_+21); goto L_5ff0; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+22, b_+24); alu_add(gb, 0x03);
  CYC(b_+24, b_+25); alu_sub(gb, B);
  CYC(b_+25, b_+27); alu_cp(gb, 0x07);
  if (!(F & FC)) { CYCT(b_+27, b_+29); goto L_5ff1; }
  CYC(b_+27, b_+29);
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+32); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CYC(b_+33, b_+34); alu_or(gb, D);
  RET(b_+34); return;
L_5ff0:
  CYC(b_+35, b_+36); SET_HL(HL + 1);
L_5ff1:
  CYC(b_+36, b_+37); SET_HL(HL + 1);
  CYC(b_+37, b_+39);
  goto L_5fd8;
}

void s_func_6001_hook(GB *gb) {
  BASE(func_6001);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+7); SET_BC(SYM(table_6024));
  CALL_C(b_+7, s_addAToBc, SYM(addAToBc), b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, BC);
  CYC(b_+11, b_+12); B = A;
  CYC(b_+12, b_+14); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); L = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+17, b_+18); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); TAIL_S(func_601c); }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+23); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+23, b_+25); TAIL_S(func_601f); }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+26); return;
}

void s_func_601c_hook(GB *gb) {
  BASE(func_601c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x00);
  RET(b_+2); return;
}

void s_func_601f_hook(GB *gb) {
  BASE(func_601f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+2, b_+4); mem_wr(gb, HL, 0xff);
  RET(b_+4); return;
}

void s_func_602c_hook(GB *gb) {
  BASE(func_602c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+5); SET_DE(DE + 1);
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); L = A;
L_6033:
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); goto func_6044; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+13, b_+15); goto ret; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+16); C = A;
  CYC(b_+16, b_+17); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+17, b_+18); push_effect(gb, HL);
  CALL_C(b_+18, s_setTile, SYM(setTile), b_+21);
  CYC(b_+21, b_+22); SET_HL(pop_effect(gb));
  CYC(b_+22, b_+24);
  goto L_6033;
func_6044:
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+26, b_+27); A = H;
  CYC(b_+27, b_+28); mem_wr(gb, DE, A);
  CYC(b_+28, b_+29); E = alu_inc8(gb, E);
  CYC(b_+29, b_+30); A = L;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
  CYC(b_+31, b_+32); alu_or(gb, D);
ret:
  RET(b_+32); return;
}

void s_func_606a_hook(GB *gb) {
  BASE(func_606a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wFrameCounter);
  CYC(b_+3, b_+5); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 0x02;
  CYC(b_+8, b_+11);
  TAIL(setScreenShakeCounter);
}

