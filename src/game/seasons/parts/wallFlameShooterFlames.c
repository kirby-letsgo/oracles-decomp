#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/wallFlameShooterFlames.s.
// PART_WALL_FLAME_SHOOTERS_FLAMES

static uint16_t wallFlameShooterFlames_jump_table(GB *gb) {
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

static void wallFlameShooterFlames_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// PART_WALL_FLAME_SHOOTERS_FLAMES
void s_partCode26_hook(GB *gb) {
  BASE(partCode26);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_VAR2A;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); A = A & 0x7f;
  CYC(b_+7, b_+9); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+9, b_+12); TAIL_S(seasonsFunc_10_670c); }
  CYC(b_+9, b_+12);
normalStatus:
  CALL_C(b_+12, s_func_66e7_hook, SYM(func_66e7), b_+15);
  if (F & FC) { CYCT(b_+15, b_+18); TAIL_S(seasonsFunc_10_670c); }
  CYC(b_+15, b_+18);
  CYC(b_+18, b_+20); E = PART_BASE + OBJ_STATE;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); push_effect(gb, b_+22);
  do { uint16_t jt_ = (wallFlameShooterFlames_jump_table(gb));
    if (jt_ == b_+28) goto state0;
    if (jt_ == b_+51) goto state1;
    if (jt_ == b_+100) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+30); L = E;
  CYC(b_+30, b_+31); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+31, b_+33); L = PART_BASE + OBJ_SPEED;
  CYC(b_+33, b_+35); mem_wr(gb, HL, 0x46);
  CYC(b_+35, b_+37); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+37, b_+39); mem_wr(gb, HL, 0x16);
  CYC(b_+39, b_+41); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x10);
  CYC(b_+43, b_+45); A = 0x73;
  CALL_C(b_+45, s_playSound, SYM(playSound_b00), b_+48);
  CYC(b_+48, b_+51);
  TAIL(objectSetVisible82);
state1:
  CALL_C(b_+51, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+54);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto L_66af; }
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x10);
  CYC(b_+58, b_+59); L = E;
  CYC(b_+59, b_+60); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+60, b_+62);
  goto state2;
L_66af:
  CYC(b_+62, b_+63); A = mem_rd(gb, HL);
  CYC(b_+63, b_+64); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+64, b_+66); goto func_66bd; }
  CYC(b_+64, b_+66);
  CYC(b_+66, b_+68); L = PART_BASE + OBJ_SPEED;
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  CYC(b_+69, b_+71); alu_cp(gb, 0x78);
  if (F & FZ) { CYCT(b_+71, b_+73); goto func_66bd; }
  CYC(b_+71, b_+73);
  CYC(b_+73, b_+75); alu_add(gb, 0x05);
  CYC(b_+75, b_+76); mem_wr(gb, HL, A);
func_66bd:
  CALL_C(b_+76, s_objectApplySpeed, SYM(objectApplySpeed), b_+79);
  CALL_C(b_+79, s_partAnimate, SYM(partAnimate), b_+82);
  CYC(b_+82, b_+84); E = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+84, b_+85); A = mem_rd(gb, DE);
  CYC(b_+85, b_+88); SET_HL(b_+97 /* @table_66d2 */);
  CYC(b_+88, b_+89); wallFlameShooterFlames_add_a_to_hl(gb, b_+89);
  CYC(b_+89, b_+91); E = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+91, b_+92); A = mem_rd(gb, HL);
  CYC(b_+92, b_+93); mem_wr(gb, DE, A);
  CYC(b_+93, b_+94); E = alu_inc8(gb, E);
  CYC(b_+94, b_+95); A = mem_rd(gb, HL);
  CYC(b_+95, b_+96); mem_wr(gb, DE, A);
  RET(b_+96); return;

state2:
  CALL_C(b_+100, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+103);
  if (F & FZ) { CYCT(b_+103, b_+106); TAIL(partDelete); }
  CYC(b_+103, b_+106);
  CYC(b_+106, b_+107); A = mem_rd(gb, HL);
  CYC(b_+107, b_+108); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+108, b_+110); goto func_66bd; }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+112); L = PART_BASE + OBJ_SPEED;
  CYC(b_+112, b_+113); A = mem_rd(gb, HL);
  CYC(b_+113, b_+115); alu_sub(gb, 0x0a);
  CYC(b_+115, b_+116); mem_wr(gb, HL, A);
  CYC(b_+116, b_+118);
  goto func_66bd;
}

void s_func_66e7_hook(GB *gb) {
  BASE(func_66e7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_add(gb, 0x09);
  CYC(b_+5, b_+6); B = A;
  CYC(b_+6, b_+8); E = PART_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_add(gb, 0x09);
  CYC(b_+11, b_+12); C = A;
  CYC(b_+12, b_+15); SET_HL(w1MagnetBall_yh);
  CYC(b_+15, b_+17); E = PART_BASE + OBJ_YH;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+19); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+19, b_+20); alu_add(gb, B);
  CYC(b_+20, b_+22); B = alu_sla(gb, B);
  CYC(b_+22, b_+23); B = alu_inc8(gb, B);
  CYC(b_+23, b_+24); alu_cp(gb, B);
  if (!(F & FC)) { RET_TAKEN(b_+24); return; }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+27); L = 0x0d;
  CYC(b_+27, b_+29); E = PART_BASE + OBJ_XH;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+31, b_+32); alu_add(gb, C);
  CYC(b_+32, b_+34); C = alu_sla(gb, C);
  CYC(b_+34, b_+35); C = alu_inc8(gb, C);
  CYC(b_+35, b_+36); alu_cp(gb, C);
  RET(b_+36); return;
}

void s_seasonsFunc_10_670c_hook(GB *gb) {
  BASE(seasonsFunc_10_670c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_objectCreatePuff, SYM(objectCreatePuff), b_+3);
  CYC(b_+3, b_+6);
  TAIL(partDelete);
}

