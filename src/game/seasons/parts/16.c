#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/parts/16.s.
// PART_16

static uint16_t part16_jump_table(GB *gb) {
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

// partCode16@state1
static void part16_state1(GB *gb) {
  BASE(partCode16);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+86, b_+89); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+89, b_+90); H = A;
  CYC(b_+90, b_+92); L = 0x00;
  CALL_C(b_+92, s_preventObjectHFromPassingObjectD, SYM(preventObjectHFromPassingObjectD), b_+95);
  CYC(b_+95, b_+98);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
}

// PART_16
void s_partCode16_hook(GB *gb) {
  BASE(partCode16);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; }
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = PART_BASE + OBJ_STATE;
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x02);
  CYC(b_+7, b_+9); L = PART_BASE + OBJ_COUNTER1;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x14);
  CYC(b_+11, b_+13); L = PART_BASE + OBJ_COLLISION_TYPE;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x00);
  CALL_C(b_+15, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+18);
  CYC(b_+18, b_+20); alu_and(gb, 0xc0);
  CYC(b_+20, b_+22); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+22, b_+24); goto L_6493; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+27); A = mem_rd(gb, wScrollMode);
  CYC(b_+27, b_+29); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+29, b_+31); goto L_6493; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+34); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+34, b_+35); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+35, b_+38); goto normalStatus; }
  CYC(b_+35, b_+38);
  CALL_C(b_+38, s_func_6515_hook, SYM(func_6515), b_+41);
  CYC(b_+41, b_+42); A = alu_inc8(gb, A);
  CYC(b_+42, b_+45); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+45, b_+48); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+48, b_+51); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
  CYC(b_+51, b_+52); A = alu_inc8(gb, A);
  CYC(b_+52, b_+55); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+55, b_+57); A = 0x08;
  CYC(b_+57, b_+60); mem_wr(gb, wTmpcfc0, A);
L_6493:
  CYC(b_+60, b_+62); A = 0x01;
  CYC(b_+62, b_+65); mem_wr(gb, wcdd7, A);
normalStatus:
  CYC(b_+65, b_+68); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+68, b_+69); A = mem_rd(gb, HL);
  CYC(b_+69, b_+70); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+70, b_+73); TAIL(partDelete); }
  CYC(b_+70, b_+73);
  CYC(b_+73, b_+75); E = PART_BASE + OBJ_STATE;
  CYC(b_+75, b_+76); A = mem_rd(gb, DE);
  CYC(b_+76, b_+77); push_effect(gb, b_+77);
  do { uint16_t jt_ = (part16_jump_table(gb));
    if (jt_ == b_+83) goto state0;
    if (jt_ == b_+86) goto state1;
    if (jt_ == b_+98) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+83, b_+85); A = 0x01;
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
state1:
  CYC(b_+86, b_+89); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+89, b_+90); H = A;
  CYC(b_+90, b_+92); L = 0x00;
  CALL_C(b_+92, s_preventObjectHFromPassingObjectD, SYM(preventObjectHFromPassingObjectD), b_+95);
  CYC(b_+95, b_+98);
  TAIL(objectSetPriorityRelativeToLink_withTerrainEffects);
state2:
  CALL_L(b_+98, part16_state1, b_+101);
  CYC(b_+101, b_+104); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  CYC(b_+105, b_+107); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+107); return; }
  CYC(b_+107, b_+108);
  CYC(b_+108, b_+110); E = PART_BASE + OBJ_SUBSTATE;
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+112, b_+114); goto L_64d9; }
  CYC(b_+112, b_+114);
  CALL_C(b_+114, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+117);
  if (!(F & FZ)) { RET_TAKEN(b_+117); return; }
  CYC(b_+117, b_+118);
  CYC(b_+118, b_+120); mem_wr(gb, HL, 0xb4);
  CYC(b_+120, b_+121); L = alu_inc8(gb, L);
  CYC(b_+121, b_+123); mem_wr(gb, HL, 0x08);
  CYC(b_+123, b_+125); L = PART_BASE + OBJ_VAR30;
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x08);
  CYC(b_+127, b_+128); L = E;
  CYC(b_+128, b_+129); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+129); return;
L_64d9:
  CALL_C(b_+130, s_partCommon_decCounter1IfNonzero, SYM(partCommon_decCounter1IfNonzero), b_+133);
  if (!(F & FZ)) { CYCT(b_+133, b_+135); goto L_64fc; }
  CYC(b_+133, b_+135);
  CYC(b_+135, b_+138); A = mem_rd(gb, wScrollMode);
  CYC(b_+138, b_+140); alu_and(gb, 0x01);
  if (F & FZ) { CYCT(b_+140, b_+142); goto L_64fc; }
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+145); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+145, b_+146); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+146, b_+149); goto L_64fc; }
  CYC(b_+146, b_+149);
  CALL_C(b_+149, s_func_6515_hook, SYM(func_6515), b_+152);
  CYC(b_+152, b_+153); A = alu_inc8(gb, A);
  CYC(b_+153, b_+156); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+156, b_+159); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+159, b_+162); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+162, b_+165); mem_wr(gb, wDisableLinkCollisionsAndMenu, A);
L_64fc:
  CYC(b_+165, b_+166); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+166, b_+168); alu_cp(gb, 0x5a);
  if (!(F & FZ)) { CYCT(b_+168, b_+170); goto L_6506; }
  CYC(b_+168, b_+170);
  CYC(b_+170, b_+172); E = PART_BASE + OBJ_VAR30;
  CYC(b_+172, b_+174); A = 0x04;
  CYC(b_+174, b_+175); mem_wr(gb, DE, A);
L_6506:
  CYC(b_+175, b_+176); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+176); return; }
  CYC(b_+176, b_+177);
  CYC(b_+177, b_+179); E = PART_BASE + OBJ_VAR30;
  CYC(b_+179, b_+180); A = mem_rd(gb, DE);
  CYC(b_+180, b_+181); mem_wr(gb, HL, A);
  CYC(b_+181, b_+183); L = PART_BASE + OBJ_OAM_FLAGS;
  CYC(b_+183, b_+184); A = mem_rd(gb, HL);
  CYC(b_+184, b_+185); A = alu_dec8(gb, A);
  CYC(b_+185, b_+187); alu_xor(gb, 0x01);
  CYC(b_+187, b_+188); A = alu_inc8(gb, A);
  CYC(b_+188, b_+189); mem_wr(gb, HL, A);
  RET(b_+189); return;
}

void s_func_6515_hook(GB *gb) {
  BASE(func_6515);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); C = 0x2b;
  CYC(b_+6, b_+8); A = 0x80;
  CYC(b_+8, b_+9); mem_wr(gb, BC, A);
  CYC(b_+9, b_+11); C = 0x2d;
  CYC(b_+11, b_+12); alu_xor(gb, A);
  CYC(b_+12, b_+13); mem_wr(gb, BC, A);
  RET(b_+13); return;
}

