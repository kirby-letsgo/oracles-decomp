#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void symmetryNpc_getTuniNutState_hook(GB *gb);

void symmetryNpc_getTuniNutState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x77ef, 0x77f1); A = 0x4c;
  CALL_C(0x77f1, checkTreasureObtained_hook, 0x1748, 0x77f4);
  CYC(0x77f4, 0x77f6); B = 0x00;
  if (!(F & FC)) { CYCT(0x77f6, 0x77f8); goto store; }
  CYC(0x77f6, 0x77f8);
  CYC(0x77f8, 0x77f9); B = alu_inc8(gb, B);
  CYC(0x77f9, 0x77fa); alu_or(gb, A);
  if (F & FZ) { CYCT(0x77fa, 0x77fc); goto store; }
  CYC(0x77fa, 0x77fc);
  CYC(0x77fc, 0x77fd); B = alu_inc8(gb, B);
store:
  CYC(0x77fd, 0x77fe); A = B;
  CYC(0x77fe, 0x7801); mem_wr(gb, 0xcfc1, A);
  CYC(0x7801, 0x7802); ret_effect(gb);
}

void symmetryNpc_setRoomFlagIfTalkedToRightSister_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7802, getThisRoomFlags_hook, 0x197d, 0x7805);
  CYC(0x7805, 0x7807); E = 0x42;
  CYC(0x7807, 0x7808); A = mem_rd(gb, DE);
  CYC(0x7808, 0x780a); alu_sub(gb, 0x08);
  CYC(0x780a, 0x780b); alu_or(gb, mem_rd(gb, HL));
  CYC(0x780b, 0x780c); mem_wr(gb, HL, A);
  CYC(0x780c, 0x780d); ret_effect(gb);
}

void symmetryNpc_getTuniNutStateForSister_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x780d, symmetryNpc_getTuniNutState_hook, 0x77ef, 0x7810);
  CYC(0x7810, 0x7813); A = mem_rd(gb, 0xcfc1);
  CYC(0x7813, 0x7814); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7814); return; }
  CYC(0x7814, 0x7815);
  CYC(0x7815, 0x7817); E = 0x42;
  CYC(0x7817, 0x7818); A = mem_rd(gb, DE);
  CYC(0x7818, 0x781a); alu_sub(gb, 0x08);
  CYC(0x781a, 0x781b); B = A;
  CALL_C(0x781b, getThisRoomFlags_hook, 0x197d, 0x781e);
  CYC(0x781e, 0x7820); alu_and(gb, 0x0f);
  CYC(0x7820, 0x7821); alu_cp(gb, B);
  CYC(0x7821, 0x7823); C = 0x00;
  if (F & FZ) { CYCT(0x7823, 0x7825); goto store; }
  CYC(0x7823, 0x7825);
  CYC(0x7825, 0x7827); C = 0x03;
store:
  CYC(0x7827, 0x7828); A = C;
  CYC(0x7828, 0x782b); mem_wr(gb, 0xcfc1, A);
  CYC(0x782b, 0x782c); ret_effect(gb);
}

void symmetryNpc_getUpgradeCapacityForText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x782c, 0x782e); A = 0x2c;
  CALL_C(0x782e, checkTreasureObtained_hook, 0x1748, 0x7831);
  if (F & FC) { CYCT(0x7831, 0x7833); goto haveRingBox; }
  CYC(0x7831, 0x7833);
  CYC(0x7833, 0x7835); C = 0x03;
  CYC(0x7835, 0x7837); goto finish;
haveRingBox:
  CYC(0x7837, 0x783a); A = mem_rd(gb, wRingBoxLevel);
  CYC(0x783a, 0x783b); A = alu_dec8(gb, A);
  CYC(0x783b, 0x783d); C = 0x03;
  if (F & FZ) { CYCT(0x783d, 0x783f); goto finish; }
  CYC(0x783d, 0x783f);
  CYC(0x783f, 0x7841); C = 0x05;
finish:
  CYC(0x7841, 0x7844); SET_HL(wTextNumberSubstitution);
  CYC(0x7844, 0x7845); mem_wr(gb, HL, C);
  CYC(0x7845, 0x7846); SET_HL(HL + 1);
  CYC(0x7846, 0x7848); mem_wr(gb, HL, 0x00);
  CYC(0x7848, 0x7849); ret_effect(gb);
}
