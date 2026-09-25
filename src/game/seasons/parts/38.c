#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/38.s.
// PART_38

static uint16_t i38_jump_table(GB *gb) {
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

// PART_38
void s_partCode38_hook(GB *gb) {
  BASE(partCode38);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xd7;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+7); TAIL(partDelete); }
  CYC(b_+4, b_+7);
  CYC(b_+7, b_+9); E = PART_BASE + OBJ_STATE;
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); push_effect(gb, b_+11);
  do { uint16_t jt_ = (i38_jump_table(gb));
    if (jt_ == b_+19) goto state0;
    if (jt_ == b_+43) goto state1;
    if (jt_ == b_+52) goto state2;
    if (jt_ == b_+98) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+21); L = E;
  CYC(b_+21, b_+22); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+22, b_+24); L = PART_BASE + OBJ_SPEED;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x50);
  CYC(b_+26, b_+28); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x14);
  CYC(b_+30, b_+32); A = 0x08;
  CALL_C(b_+32, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+35);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+37, b_+40); TAIL(objectSetVisible82); }
  CYC(b_+37, b_+40);
  CYC(b_+40, b_+43);
  TAIL(objectSetVisible81);
state1:
  CALL_C(b_+43, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+46);
  if (!(F & FZ)) { RET_TAKEN(b_+46); return; }
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+48); L = E;
  CYC(b_+48, b_+49); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+49, s_objectSetVisible81, SYM(objectSetVisible81), b_+52);
state2:
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = PART_BASE + OBJ_VAR30;
  CYC(b_+55, b_+56); B = mem_rd(gb, HL);
  CYC(b_+56, b_+57); L = alu_inc8(gb, L);
  CYC(b_+57, b_+58); C = mem_rd(gb, HL);
  CALL_C(b_+58, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+61);
  CYC(b_+61, b_+63); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CYC(b_+64, b_+65); H = D;
  CYC(b_+65, b_+67); L = PART_BASE + OBJ_VAR30;
  CYC(b_+67, b_+69); E = PART_BASE + OBJ_YH;
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+71); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+71, b_+73); alu_add(gb, 0x08);
  CYC(b_+73, b_+75); alu_cp(gb, 0x11);
  if (!(F & FC)) { CYCT(b_+75, b_+77); goto applySpeedAndAnimate; }
  CYC(b_+75, b_+77);
  CYC(b_+77, b_+79); L = PART_BASE + OBJ_VAR31;
  CYC(b_+79, b_+81); E = PART_BASE + OBJ_XH;
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+83); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+83, b_+85); alu_add(gb, 0x08);
  CYC(b_+85, b_+87); alu_cp(gb, 0x11);
  if (!(F & FC)) { CYCT(b_+87, b_+89); goto applySpeedAndAnimate; }
  CYC(b_+87, b_+89);
  CYC(b_+89, b_+91); L = PART_BASE + OBJ_STATE;
  CYC(b_+91, b_+92); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
applySpeedAndAnimate:
  CALL_C(b_+92, s_objectApplySpeed, SYM(objectApplySpeed), b_+95);
  CYC(b_+95, b_+98);
  TAIL(partAnimate);
state3:
  CYC(b_+98, b_+100); A = 0x0b;
  CALL_C(b_+100, s_objectGetRelatedObject2Var, SYM(objectGetRelatedObject2Var), b_+103);
  CYC(b_+103, b_+104); push_effect(gb, HL);
  CYC(b_+104, b_+105); B = mem_rd(gb, HL);
  CYC(b_+105, b_+107); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+107, b_+108); C = mem_rd(gb, HL);
  CALL_C(b_+108, s_objectGetRelativeAngle, SYM(objectGetRelativeAngle), b_+111);
  CYC(b_+111, b_+113); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+113, b_+114); mem_wr(gb, DE, A);
  CYC(b_+114, b_+115); SET_HL(pop_effect(gb));
  CYC(b_+115, b_+117); E = PART_BASE + OBJ_YH;
  CYC(b_+117, b_+118); A = mem_rd(gb, DE);
  CYC(b_+118, b_+119); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+119, b_+121); alu_add(gb, 0x04);
  CYC(b_+121, b_+123); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+123, b_+125); goto applySpeedAndAnimate; }
  CYC(b_+123, b_+125);
  CYC(b_+125, b_+127); L = ENEMY_BASE + OBJ_XH;
  CYC(b_+127, b_+129); E = PART_BASE + OBJ_XH;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+131); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+131, b_+133); alu_add(gb, 0x04);
  CYC(b_+133, b_+135); alu_cp(gb, 0x09);
  if (!(F & FC)) { CYCT(b_+135, b_+137); goto applySpeedAndAnimate; }
  CYC(b_+135, b_+137);
  CYC(b_+137, b_+139); A = 0x18;
  CALL_C(b_+139, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+142);
  CYC(b_+142, b_+143); alu_xor(gb, A);
  CYC(b_+143, b_+144); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+144, b_+145); mem_wr(gb, HL, A);
  CYC(b_+145, b_+148);
  TAIL(partDelete);
}

