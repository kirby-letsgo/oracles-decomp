#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/mothulaProjectile2.s.
// PART_MOTHULA_PROJECTILE_2

// PART_MOTHULA_PROJECTILE_2
void s_partCode42_hook(GB *gb) {
  BASE(partCode42);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_VAR2A;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); A = A & 0x7f;
  CYC(b_+7, b_+9); alu_cp(gb, 0x04);
  if (F & FC) { CYCT(b_+9, b_+11); goto delete; }
  CYC(b_+9, b_+11);
normalStatus:
  CYC(b_+11, b_+13); E = PART_BASE + OBJ_STATE;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+15, b_+17); goto state0; }
  CYC(b_+15, b_+17);
  CALL_C(b_+17, s_partCommon_checkTileCollisionOrOutOfBounds, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+20);
  if (F & FZ) { CYCT(b_+20, b_+22); goto delete; }
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+24); E = PART_BASE + OBJ_SUBID;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+27); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+27, b_+29); goto L_712c; }
  CYC(b_+27, b_+29);
  CALL_C(b_+29, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+32);
  if (!(F & FZ)) { CYCT(b_+32, b_+34); goto L_712c; }
  CYC(b_+32, b_+34);
  CYC(b_+34, b_+35); L = alu_inc8(gb, L);
  CYC(b_+35, b_+37); E = PART_BASE + OBJ_VAR30;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
  CYC(b_+39, b_+41); alu_and(gb, 0x01);
  CYC(b_+41, b_+42); mem_wr(gb, DE, A);
  CYC(b_+42, b_+43); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+43, b_+44); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+44, b_+45); mem_wr(gb, HL, A);
  CYC(b_+45, b_+47); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+47, b_+48); A = mem_rd(gb, HL);
  CYC(b_+48, b_+50); alu_add(gb, 0x02);
  CYC(b_+50, b_+52); alu_and(gb, 0x1f);
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
L_712c:
  CALL_C(b_+53, s_objectApplySpeed, SYM(objectApplySpeed), b_+56);
  CALL_C(b_+56, s_partAnimate, SYM(partAnimate), b_+59);
  CYC(b_+59, b_+61); E = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+63); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+63); return; }
  CYC(b_+63, b_+64);
delete:
  CYC(b_+64, b_+67);
  TAIL(partDelete);
state0:
  CALL_C(b_+67, s_func_7174_hook, SYM(func_7174), b_+70);
  if (!(F & FZ)) { RET_TAKEN(b_+70); return; }
  CYC(b_+70, b_+71);
  CALL_C(b_+71, s_objectSetVisible82, SYM(objectSetVisible82), b_+74);
  CYC(b_+74, b_+75); H = D;
  CYC(b_+75, b_+77); L = PART_BASE + OBJ_STATE;
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+78, b_+80); E = PART_BASE + OBJ_SUBID;
  CYC(b_+80, b_+81); A = mem_rd(gb, DE);
  CYC(b_+81, b_+83); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+83, b_+85); goto L_715a; }
  CYC(b_+83, b_+85);
  CYC(b_+85, b_+87); L = PART_BASE + OBJ_ZH;
  CYC(b_+87, b_+88); A = mem_rd(gb, HL);
  CYC(b_+88, b_+90); mem_wr(gb, HL, 0x00);
  CYC(b_+90, b_+92); L = PART_BASE + OBJ_YH;
  CYC(b_+92, b_+93); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+93, b_+94); mem_wr(gb, HL, A);
  CYC(b_+94, b_+96); L = PART_BASE + OBJ_SPEED;
  CYC(b_+96, b_+98); mem_wr(gb, HL, 0x32);
  RET(b_+98); return;
L_715a:
  CYC(b_+99, b_+101); L = PART_BASE + OBJ_ZH;
  CYC(b_+101, b_+102); A = mem_rd(gb, HL);
  CYC(b_+102, b_+104); mem_wr(gb, HL, 0xfa);
  CYC(b_+104, b_+106); alu_add(gb, 0x06);
  CYC(b_+106, b_+108); L = PART_BASE + OBJ_YH;
  CYC(b_+108, b_+109); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+109, b_+110); mem_wr(gb, HL, A);
  CYC(b_+110, b_+112); L = PART_BASE + OBJ_SPEED;
  CYC(b_+112, b_+114); mem_wr(gb, HL, 0x78);
  CYC(b_+114, b_+116); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+116, b_+118); A = 0x02;
  CYC(b_+118, b_+119); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+119, b_+120); mem_wr(gb, HL, A);
  CYC(b_+120, b_+122); A = 0x01;
  CYC(b_+122, b_+125);
  TAIL(partSetAnimation);
}

void s_func_7174_hook(GB *gb) {
  BASE(func_7174);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+5, b_+7); TAIL_S(func_71b5); }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); alu_rrca(gb);
  CYC(b_+8, b_+10); A = 0x04;
  CYC(b_+10, b_+13); SET_BC(0x0300);
  if (!(F & FC)) { CYCT(b_+13, b_+15); goto L_7188; }
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); A = 0x03;
  CYC(b_+17, b_+20); SET_BC(0x0503);
L_7188:
  CYC(b_+20, b_+22); E = PART_BASE + OBJ_ANGLE;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+25); E = PART_BASE + OBJ_SUBID;
  CYC(b_+25, b_+26); alu_xor(gb, A);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); push_effect(gb, BC);
  CALL_C(b_+28, s_checkBPartSlotsAvailable, SYM(checkBPartSlotsAvailable), b_+31);
  CYC(b_+31, b_+32); SET_BC(pop_effect(gb));
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+34); A = B;
  CYC(b_+34, b_+36); mem_wr(gb, hFF8B, A);
  CYC(b_+36, b_+37); A = C;
  CYC(b_+37, b_+40); SET_BC(SYM(table_71fc));
  CALL_C(b_+40, s_addAToBc, SYM(addAToBc), b_+43);
L_719f:
  CYC(b_+43, b_+44); push_effect(gb, BC);
  CALL_C(b_+44, s_getFreePartSlot, SYM(getFreePartSlot), b_+47);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0x42);
  CALL_C(b_+49, s_objectCopyPosition, SYM(objectCopyPosition), b_+52);
  CYC(b_+52, b_+53); SET_BC(pop_effect(gb));
  CYC(b_+53, b_+55); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+55, b_+56); A = mem_rd(gb, BC);
  CYC(b_+56, b_+57); mem_wr(gb, HL, A);
  CYC(b_+57, b_+58); SET_BC(BC + 1);
  CYC(b_+58, b_+61); SET_HL(hFF8B);
  CYC(b_+61, b_+62); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+62, b_+64); goto L_719f; }
  CYC(b_+62, b_+64);
  RET(b_+64); return;
}

void s_func_71b5_hook(GB *gb) {
  BASE(func_71b5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+1, b_+3); goto L_71ba; }
  CYC(b_+1, b_+3);
  CYC(b_+3, b_+4); alu_xor(gb, A);
  RET(b_+4); return;
L_71ba:
  CYC(b_+5, b_+7); B = 0x05;
  CALL_C(b_+7, s_checkBPartSlotsAvailable, SYM(checkBPartSlotsAvailable), b_+10);
  if (!(F & FZ)) { RET_TAKEN(b_+10); return; }
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); A = 0x09;
  CALL_C(b_+13, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+16);
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); alu_add(gb, 0x08);
  CYC(b_+19, b_+21); alu_and(gb, 0x1f);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+24); C = 0x02;
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+27); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+27, b_+28); alu_sub(gb, C);
  CYC(b_+28, b_+30); alu_and(gb, 0x1f);
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, b_+33); L = PART_BASE + OBJ_SUBID;
  CYC(b_+33, b_+34); mem_wr(gb, HL, C);
  CALL_C(b_+34, s_func_71e2_hook, SYM(func_71e2), b_+37);
  CYC(b_+37, b_+38); A = B;
  CYC(b_+38, b_+40); alu_add(gb, 0x0c);
  CYC(b_+40, b_+42); alu_and(gb, 0x1f);
  CYC(b_+42, b_+43); B = A;
  CYC(b_+43, b_+45); C = 0x03;
  s_func_71e2_hook(gb); return; // falls through
}

void s_func_71e2_hook(GB *gb) {
  BASE(func_71e2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
L_71e2:
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CALL_C(b_+1, s_getFreePartSlot, SYM(getFreePartSlot), b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x42);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x02);
  CALL_C(b_+9, s_objectCopyPosition, SYM(objectCopyPosition), b_+12);
  CYC(b_+12, b_+13); SET_BC(pop_effect(gb));
  CYC(b_+13, b_+15); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+15, b_+16); mem_wr(gb, HL, B);
  CYC(b_+16, b_+17); A = B;
  CYC(b_+17, b_+19); alu_add(gb, 0x02);
  CYC(b_+19, b_+21); alu_and(gb, 0x1f);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+23); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto L_71e2; }
  CYC(b_+23, b_+25);
  RET(b_+25); return;
}

