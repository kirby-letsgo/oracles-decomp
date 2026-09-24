#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/shootingDragonHead.s.
// PART_SHOOTING_DRAGON_HEAD

static uint16_t shootingDragonHead_jump_table(GB *gb) {
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

// PART_SHOOTING_DRAGON_HEAD
void s_partCode24_hook(GB *gb) {
  BASE(partCode24);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = PART_BASE + OBJ_VAR2A;
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+7, b_+10); TAIL(partDelete); }
  CYC(b_+7, b_+10);
normalStatus:
  CYC(b_+10, b_+12); E = PART_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); E = PART_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); push_effect(gb, b_+16);
  do { uint16_t jt_ = (shootingDragonHead_jump_table(gb));
    if (jt_ == b_+22) goto subid0;
    if (jt_ == b_+79) goto subid2;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+24, b_+26); goto func_656b; }
  CYC(b_+24, b_+26);
  CALL_C(b_+26, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+29);
  if (!(F & FZ)) { RET_TAKEN(b_+29); return; }
  CYC(b_+29, b_+30);
  CYC(b_+30, b_+32); L = PART_BASE + OBJ_SUBID;
  CYC(b_+32, b_+34); alu_bit(gb, 0, mem_rd(gb, HL));
  CYC(b_+34, b_+36); L = PART_BASE + OBJ_XH;
  CYC(b_+36, b_+38); A = mem_rd(gb, hEnemyTargetX);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto func_654f; }
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+41); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { RET_TAKEN(b_+41); return; }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+44);
  goto L_6551;
func_654f:
  CYC(b_+44, b_+45); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+45); return; }
  CYC(b_+45, b_+46);
L_6551:
  CALL_C(b_+46, s_func_65b8_hook, SYM(func_65b8), b_+49);
  if (!(F & FC)) { RET_TAKEN(b_+49); return; }
  CYC(b_+49, b_+50);
  CALL_C(b_+50, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+53);
  CYC(b_+53, b_+55); alu_cp(gb, 0x50);
  if (!(F & FC)) { RET_TAKEN(b_+55); return; }
  CYC(b_+55, b_+56);
  CALL_C(b_+56, s_func_65a6_hook, SYM(func_65a6), b_+59);
  if (!(F & FZ)) { RET_TAKEN(b_+59); return; }
  CYC(b_+59, b_+60);
  CYC(b_+60, b_+62); L = PART_BASE + OBJ_ANGLE;
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x08);
  CYC(b_+64, b_+66); E = PART_BASE + OBJ_SUBID;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+68); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+68); return; }
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x18);
  RET(b_+71); return;
func_656b:
  CYC(b_+72, b_+73); H = D;
  CYC(b_+73, b_+74); L = E;
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+75, b_+77); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+77, b_+78); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+78); return;
subid2:
  CYC(b_+79, b_+80); A = mem_rd(gb, DE);
  CYC(b_+80, b_+81); push_effect(gb, b_+81);
  do { uint16_t jt_ = (shootingDragonHead_jump_table(gb));
    if (jt_ == b_+87) goto state0;
    if (jt_ == b_+101) goto state1;
    if (jt_ == b_+108) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+87, b_+88); H = D;
  CYC(b_+88, b_+89); L = E;
  CYC(b_+89, b_+90); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+90, b_+92); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+92, b_+94); mem_wr(gb, HL, 0x10);
  CYC(b_+94, b_+96); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+96, b_+98); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+98, b_+101);
  TAIL(objectSetVisible81);
state1:
  CALL_C(b_+101, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+104);
  if (!(F & FZ)) { CYCT(b_+104, b_+106); goto L_6595; }
  CYC(b_+104, b_+106);
  CYC(b_+106, b_+107); L = E;
  CYC(b_+107, b_+108); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
state2:
  CALL_C(b_+108, s_partCommon_checkTileCollisionOrOutOfBounds, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+111);
  if (F & FC) { CYCT(b_+111, b_+114); TAIL(partDelete); }
  CYC(b_+111, b_+114);
L_6595:
  CALL_C(b_+114, s_objectApplySpeed, SYM(objectApplySpeed), b_+117);
  CYC(b_+117, b_+120); A = mem_rd(gb, wFrameCounter);
  CYC(b_+120, b_+122); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+122); return; }
  CYC(b_+122, b_+123);
  CYC(b_+123, b_+125); E = PART_BASE + OBJ_OAM_FLAGS;
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+127); A = alu_inc8(gb, A);
  CYC(b_+127, b_+129); alu_and(gb, 0x07);
  CYC(b_+129, b_+130); mem_wr(gb, DE, A);
  RET(b_+130); return;
}

void s_func_65a6_hook(GB *gb) {
  BASE(func_65a6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_getFreePartSlot, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x24);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x02);
  CALL_C(b_+9, s_objectCopyPosition, SYM(objectCopyPosition), b_+12);
  CYC(b_+12, b_+14); L = PART_BASE + OBJ_SPEED;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x3c);
  CYC(b_+16, b_+17); alu_xor(gb, A);
  RET(b_+17); return;
}

void s_func_65b8_hook(GB *gb) {
  BASE(func_65b8);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = PART_BASE + OBJ_YH;
  CYC(b_+2, b_+4); A = mem_rd(gb, hEnemyTargetY);
  CYC(b_+4, b_+5); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+5, b_+7); alu_add(gb, 0x10);
  CYC(b_+7, b_+9); alu_cp(gb, 0x21);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); E = PART_BASE + OBJ_COUNTER1;
  CYC(b_+12, b_+14); A = 0x1e;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  RET(b_+15); return;
}

