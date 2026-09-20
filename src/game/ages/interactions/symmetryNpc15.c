#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void symmetryNpc_getTuniNutState_hook(GB *gb);

void symmetryNpc_getTuniNutState_hook(GB *gb) {
  BASE(symmetryNpc_getTuniNutState);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x4c;
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  CYC(b_+5, b_+7); B = 0x00;
  if (!(F & FC)) { CYCT(b_+7, b_+9); goto store; }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); B = alu_inc8(gb, B);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+11, b_+13); goto store; }
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); B = alu_inc8(gb, B);
store:
  CYC(b_+14, b_+15); A = B;
  CYC(b_+15, b_+18); mem_wr(gb, wTmpcfc0_bigBangGame_filler1, A);
  CYC(b_+18, b_+19); ret_effect(gb);
}

void symmetryNpc_setRoomFlagIfTalkedToRightSister_hook(GB *gb) {
  BASE(symmetryNpc_setRoomFlagIfTalkedToRightSister);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); E = 0x42;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_sub(gb, 0x08);
  CYC(b_+8, b_+9); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+9, b_+10); mem_wr(gb, HL, A);
  CYC(b_+10, b_+11); ret_effect(gb);
}

void symmetryNpc_getTuniNutStateForSister_hook(GB *gb) {
  BASE(symmetryNpc_getTuniNutStateForSister);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, symmetryNpc_getTuniNutState_hook, SYM(symmetryNpc_getTuniNutState), b_+3);
  CYC(b_+3, b_+6); A = mem_rd(gb, wTmpcfc0_bigBangGame_filler1);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); E = 0x42;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_sub(gb, 0x08);
  CYC(b_+13, b_+14); B = A;
  CALL_C(b_+14, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+17);
  CYC(b_+17, b_+19); alu_and(gb, 0x0f);
  CYC(b_+19, b_+20); alu_cp(gb, B);
  CYC(b_+20, b_+22); C = 0x00;
  if (F & FZ) { CYCT(b_+22, b_+24); goto store; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); C = 0x03;
store:
  CYC(b_+26, b_+27); A = C;
  CYC(b_+27, b_+30); mem_wr(gb, wTmpcfc0_bigBangGame_filler1, A);
  CYC(b_+30, b_+31); ret_effect(gb);
}

void symmetryNpc_getUpgradeCapacityForText_hook(GB *gb) {
  BASE(symmetryNpc_getUpgradeCapacityForText);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x2c;
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  if (F & FC) { CYCT(b_+5, b_+7); goto haveRingBox; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); C = 0x03;
  CYC(b_+9, b_+11); goto finish;
haveRingBox:
  CYC(b_+11, b_+14); A = mem_rd(gb, wRingBoxLevel);
  CYC(b_+14, b_+15); A = alu_dec8(gb, A);
  CYC(b_+15, b_+17); C = 0x03;
  if (F & FZ) { CYCT(b_+17, b_+19); goto finish; }
  CYC(b_+17, b_+19);
  CYC(b_+19, b_+21); C = 0x05;
finish:
  CYC(b_+21, b_+24); SET_HL(wTextNumberSubstitution);
  CYC(b_+24, b_+25); mem_wr(gb, HL, C);
  CYC(b_+25, b_+26); SET_HL(HL + 1);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x00);
  CYC(b_+28, b_+29); ret_effect(gb);
}
