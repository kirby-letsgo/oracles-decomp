#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/44.s.
// PART_44

static uint16_t part44_jump_table(GB *gb) {
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

static void part44_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// partCode44@func_73ae
static void part44_func_73ae(GB *gb) {
  BASE(partCode44);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+92, b_+93); C = A;
  CYC(b_+93, b_+95); B = 0x46;
  CYC(b_+95, b_+97); A = 0x02;
  CYC(b_+97, b_+100);
  TAIL(objectSetComponentSpeedByScaledVelocity);
}

// PART_44
void s_partCode44_hook(GB *gb) {
  BASE(partCode44);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_VAR2A;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x83);
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_7370; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = 0x01;
  CALL_C(b_+11, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_cp(gb, 0x7f);
  if (!(F & FZ)) { CYCT(b_+17, b_+19); goto L_7369; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); L = ENEMY_BASE + OBJ_VAR36;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x01);
L_7369:
  CYC(b_+23, b_+25); A = 0x13;
  CYC(b_+25, b_+28); mem_wr(gb, wLinkForceState, A);
  CYC(b_+28, b_+30);
  TAIL_S(func_73db);
L_7370:
  CYC(b_+30, b_+32); E = PART_BASE + OBJ_STATE;
  CYC(b_+32, b_+34); A = 0x02;
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+37); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+40); alu_xor(gb, 0x10);
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);
  CALL_L(b_+41, part44_func_73ae, b_+44);
  CALL_C(b_+44, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+47);
  CYC(b_+47, b_+50); mem_wr(gb, w1Link_knockbackAngle, A);
  CYC(b_+50, b_+52); A = 0x18;
  CYC(b_+52, b_+55); mem_wr(gb, w1Link_knockbackCounter, A);
  CYC(b_+55, b_+57); A = 0x52;
  CALL_C(b_+57, s_playSound, SYM(playSound_b00), b_+60);
normalStatus:
  CALL_C(b_+60, s_partCommon_checkOutOfBounds, SYM(partCommon_checkOutOfBounds), b_+63);
  if (F & FZ) { CYCT(b_+63, b_+65); TAIL_S(func_73db); }
  CYC(b_+63, b_+65);
  CYC(b_+65, b_+67); E = PART_BASE + OBJ_STATE;
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  CYC(b_+68, b_+69); push_effect(gb, b_+69);
  do { uint16_t jt_ = (part44_jump_table(gb));
    if (jt_ == b_+75) goto state0;
    if (jt_ == b_+100) goto state1;
    if (jt_ == b_+106) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+75, b_+77); A = 0x01;
  CYC(b_+77, b_+78); mem_wr(gb, DE, A);
  CALL_C(b_+78, s_objectSetVisible82, SYM(objectSetVisible82), b_+81);
  CYC(b_+81, b_+83); E = PART_BASE + OBJ_SUBID;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+87); SET_HL(SYM(table_73de));
  CYC(b_+87, b_+88); part44_add_a_to_hl(gb, b_+88);
  CYC(b_+88, b_+90); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+90, b_+91); A = mem_rd(gb, HL);
  CYC(b_+91, b_+92); mem_wr(gb, DE, A);
  CYC(b_+92, b_+93); C = A;
  CYC(b_+93, b_+95); B = 0x46;
  CYC(b_+95, b_+97); A = 0x02;
  CYC(b_+97, b_+100);
  TAIL(objectSetComponentSpeedByScaledVelocity);
state1:
  CALL_C(b_+100, s_objectApplyComponentSpeed, SYM(objectApplyComponentSpeed), b_+103);
  CYC(b_+103, b_+106);
  TAIL(partAnimate);
state2:
  CYC(b_+106, b_+108); A = 0x00;
  CALL_C(b_+108, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+111);
  CALL_C(b_+111, s_checkObjectsCollided, SYM(checkObjectsCollided), b_+114);
  if (!(F & FC)) { CYCT(b_+114, b_+116); goto state1; }
  CYC(b_+114, b_+116);
  CYC(b_+116, b_+118); L = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+118, b_+120); mem_wr(gb, HL, 0x78);
  CYC(b_+120, b_+121); L = alu_dec8(gb, L);
  CYC(b_+121, b_+123); mem_wr(gb, HL, 0x18);
  CYC(b_+123, b_+124); push_effect(gb, HL);
  CALL_C(b_+124, s_objectGetAngleTowardEnemyTarget, SYM(objectGetAngleTowardEnemyTarget), b_+127);
  CYC(b_+127, b_+128); SET_HL(pop_effect(gb));
  CYC(b_+128, b_+129); L = alu_dec8(gb, L);
  CYC(b_+129, b_+131); alu_xor(gb, 0x10);
  CYC(b_+131, b_+132); mem_wr(gb, HL, A);
  CYC(b_+132, b_+134); A = 0x4e;
  CALL_C(b_+134, s_playSound, SYM(playSound_b00), b_+137);
  s_func_73db_hook(gb); return; // falls through
}

void s_func_73db_hook(GB *gb) {
  BASE(func_73db);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3);
  TAIL(partDelete);
}

