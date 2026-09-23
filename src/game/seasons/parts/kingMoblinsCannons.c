#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/kingMoblinsCannons.s.
// PART_KING_MOBLINS_CANNONS

static uint16_t kingMoblinsCannons_jump_table(GB *gb) {
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

static void kingMoblinsCannons_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// PART_KING_MOBLINS_CANNONS
void s_partCode2d_hook(GB *gb) {
  BASE(partCode2d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = PART_BASE + OBJ_VAR30;
  CYC(b_+5, b_+7); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+7, b_+10); TAIL_S(seasonsFunc_10_67cc); }
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); L = PART_BASE + OBJ_HEALTH;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  CYC(b_+15, b_+17); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x41);
  CYC(b_+19, b_+22);
  TAIL(objectSetInvisible);
normalStatus:
  CYC(b_+22, b_+24); E = PART_BASE + OBJ_SUBID;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+27); A = alu_srl(gb, A);
  CYC(b_+27, b_+29); E = PART_BASE + OBJ_STATE;
  CYC(b_+29, b_+30); push_effect(gb, b_+30);
  do { uint16_t jt_ = (kingMoblinsCannons_jump_table(gb));
    if (jt_ == b_+34) goto subid0;
    if (jt_ == b_+82) goto subid1;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+36, b_+38); goto func_6775; }
  CYC(b_+36, b_+38);
func_6759:
  CYC(b_+38, b_+39); H = D;
  CYC(b_+39, b_+40); L = E;
  CYC(b_+40, b_+41); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+41, b_+43); L = PART_BASE + OBJ_YH;
  CYC(b_+43, b_+45); mem_wr(gb, HL, mem_rd(gb, HL) & 0xf7);
  CYC(b_+45, b_+47); L = PART_BASE + OBJ_XH;
  CYC(b_+47, b_+49); mem_wr(gb, HL, mem_rd(gb, HL) & 0xf7);
  CYC(b_+49, b_+51); A = 0x16;
  CALL_C(b_+51, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+54);
  if (!(F & FZ)) { CYCT(b_+54, b_+57); TAIL(partDelete); }
  CYC(b_+54, b_+57);
  CYC(b_+57, b_+59); E = PART_BASE + OBJ_SUBID;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CALL_C(b_+60, s_partSetAnimation, SYM(partSetAnimation), b_+63);
  CYC(b_+63, b_+66);
  TAIL(objectSetVisible82);
func_6775:
  CALL_C(b_+66, s_partAnimate, SYM(partAnimate), b_+69);
  CYC(b_+69, b_+71); E = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+71, b_+72); A = mem_rd(gb, DE);
  CYC(b_+72, b_+73); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+73); return; }
  CYC(b_+73, b_+74);
  CYC(b_+74, b_+77); SET_BC(0xfa13);
  CYC(b_+77, b_+78); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+78, b_+80); TAIL_S(func_67a4); }
  CYC(b_+78, b_+80);
  CYC(b_+80, b_+82);
  TAIL_S(func_6797);
subid1:
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+84); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+84, b_+86); goto func_6759; }
  CYC(b_+84, b_+86);
  CALL_C(b_+86, s_partAnimate, SYM(partAnimate), b_+89);
  CYC(b_+89, b_+91); E = PART_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+91, b_+92); A = mem_rd(gb, DE);
  CYC(b_+92, b_+93); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+93); return; }
  CYC(b_+93, b_+94);
  CYC(b_+94, b_+97); SET_BC(0xfaed);
  CYC(b_+97, b_+98); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+98, b_+100); TAIL_S(func_67a4); }
  CYC(b_+98, b_+100);
  s_func_6797_hook(gb); return; // falls through
}

void s_func_6797_hook(GB *gb) {
  BASE(func_6797);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreePartSlot, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x3f);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+8, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+11);
  CYC(b_+11, b_+13);
  TAIL_S(func_67b7);
}

void s_func_67a4_hook(GB *gb) {
  BASE(func_67a4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); mem_wr(gb, DE, A);
  CYC(b_+1, b_+3); A = 0x81;
  CALL_C(b_+3, s_playSound, SYM(playSound_b00), b_+6);
  CALL_C(b_+6, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+9);
  if (!(F & FZ)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x05);
  CYC(b_+12, b_+14); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x80);
  CYC(b_+16, b_+19);
  TAIL(objectCopyPositionWithOffset);
}

void s_func_67b7_hook(GB *gb) {
  BASE(func_67b7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_bit(gb, 1, A);
  CYC(b_+5, b_+7); B = 0x04;
  if (F & FZ) { CYCT(b_+7, b_+9); goto L_67c2; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); B = 0x12;
L_67c2:
  CALL_C(b_+11, s_getRandomNumber, SYM(getRandomNumber), b_+14);
  CYC(b_+14, b_+16); alu_and(gb, 0x06);
  CYC(b_+16, b_+17); alu_add(gb, B);
  CYC(b_+17, b_+19); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+19, b_+20); mem_wr(gb, HL, A);
  RET(b_+20); return;
}

void s_seasonsFunc_10_67cc_hook(GB *gb) {
  BASE(seasonsFunc_10_67cc);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+3);
  if (F & FZ) { CYCT(b_+3, b_+6); TAIL(partDelete); }
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); alu_cp(gb, 0x35);
  if (F & FZ) { CYCT(b_+9, b_+11); TAIL_S(func_67f8); }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); alu_and(gb, 0x0f);
  if (!(F & FZ)) { RET_TAKEN(b_+13); return; }
  CYC(b_+13, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+17); alu_and(gb, 0xf0);
  CYC(b_+17, b_+19); A = alu_swap(gb, A);
  CYC(b_+19, b_+20); A = alu_dec8(gb, A);
  CYC(b_+20, b_+23); SET_HL(SYM(table_67f0));
  CYC(b_+23, b_+24); kingMoblinsCannons_add_double_index(gb, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+25, b_+26); C = mem_rd(gb, HL);
  CYC(b_+26, b_+27); B = A;
  CALL_C(b_+27, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+30);
  if (!(F & FZ)) { RET_TAKEN(b_+30); return; }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x56);
  CYC(b_+33, b_+36);
  TAIL(objectCopyPositionWithOffset);
}

void s_func_67f8_hook(GB *gb) {
  BASE(func_67f8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_YH;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x08);
  CYC(b_+5, b_+7); alu_and(gb, 0xf0);
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+10); E = PART_BASE + OBJ_XH;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_sub(gb, 0x08);
  CYC(b_+13, b_+15); alu_and(gb, 0xf0);
  CYC(b_+15, b_+17); A = alu_swap(gb, A);
  CYC(b_+17, b_+18); alu_or(gb, B);
  CYC(b_+18, b_+19); C = A;
  CYC(b_+19, b_+21); B = 0xa2;
  CYC(b_+21, b_+23); E = PART_BASE + OBJ_SUBID;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+26, b_+28); goto L_6816; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); B = 0xa6;
L_6816:
  CYC(b_+30, b_+31); push_effect(gb, BC);
  CYC(b_+31, b_+32); A = B;
  CALL_C(b_+32, s_setTile, SYM(setTile), b_+35);
  CYC(b_+35, b_+36); SET_BC(pop_effect(gb));
  CYC(b_+36, b_+37); A = B;
  CYC(b_+37, b_+38); A = alu_inc8(gb, A);
  CYC(b_+38, b_+39); C = alu_inc8(gb, C);
  CYC(b_+39, b_+42);
  TAIL(setTile);
}

