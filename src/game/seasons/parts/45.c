#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/45.s.
// PART_45

// PART_45
void s_partCode45_hook(GB *gb) {
  BASE(partCode45);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto delete; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); E = PART_BASE + OBJ_VAR2A;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto delete; }
  CYC(b_+10, b_+12);
normalStatus:
  CYC(b_+12, b_+14); E = PART_BASE + OBJ_SUBID;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_or(gb, A);
  CYC(b_+16, b_+18); E = PART_BASE + OBJ_STATE;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  if (F & FZ) { CYCT(b_+19, b_+21); goto func_742e; }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+22); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+22, b_+24); goto L_740f; }
  CYC(b_+22, b_+24);
  CALL_C(b_+24, s_objectCheckSimpleCollision, SYM(objectCheckSimpleCollision), b_+27);
  if (F & FZ) { CYCT(b_+27, b_+30); TAIL(objectApplyComponentSpeed); }
  CYC(b_+27, b_+30);
delete:
  CYC(b_+30, b_+33);
  TAIL(partDelete);
L_740f:
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+35); L = E;
  CYC(b_+35, b_+36); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+36, b_+38); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+38, b_+40); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+40, b_+42); A = 0x0b;
  CALL_C(b_+42, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+45);
  CYC(b_+45, b_+48); SET_BC(0x0f00);
  CALL_C(b_+48, s_objectTakePositionWithOffset, SYM(objectTakePositionWithOffset), b_+51);
  CYC(b_+51, b_+52); alu_xor(gb, A);
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  CYC(b_+53, b_+56); SET_BC(0x5010);
  CYC(b_+56, b_+58); A = 0x08;
  CALL_C(b_+58, s_objectSetComponentSpeedByScaledVelocity, SYM(objectSetComponentSpeedByScaledVelocity), b_+61);
  CYC(b_+61, b_+64);
  TAIL(objectSetVisible82);
func_742e:
  CYC(b_+64, b_+65); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+65, b_+67); goto L_7433; }
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+68); A = alu_inc8(gb, A);
  CYC(b_+68, b_+69); mem_wr(gb, DE, A);
L_7433:
  CYC(b_+69, b_+71); A = 0x29;
  CALL_C(b_+71, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+74);
  CYC(b_+74, b_+75); A = mem_rd(gb, HL);
  CYC(b_+75, b_+76); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+76, b_+78); goto delete; }
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+80); L = ENEMY_BASE + OBJ_STUN_COUNTER;
  CYC(b_+80, b_+81); A = mem_rd(gb, HL);
  CYC(b_+81, b_+82); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+82); return; }
  CYC(b_+82, b_+83);
  CALL_C(b_+83, s_getFreePartSlot, SYM(getFreePartSlot), b_+86);
  if (!(F & FZ)) { RET_TAKEN(b_+86); return; }
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x45);
  CYC(b_+89, b_+90); L = alu_inc8(gb, L);
  CYC(b_+90, b_+91); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+91, b_+93); L = PART_BASE + OBJ_RELATED1;
  CYC(b_+93, b_+94); E = L;
  CYC(b_+94, b_+95); A = mem_rd(gb, DE);
  CYC(b_+95, b_+96); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+96, b_+97); E = L;
  CYC(b_+97, b_+98); A = mem_rd(gb, DE);
  CYC(b_+98, b_+99); mem_wr(gb, HL, A);
  RET(b_+99); return;
}

