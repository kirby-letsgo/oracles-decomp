#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void checkEssenceNotObtained_hook(GB *gb);
void checkEssenceObtained_hook(GB *gb);

void writeFlagsTocddb_hook(GB *gb);

static uint16_t linkedNpc_checkShouldSpawn_jump_table(GB *gb) {
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

static void linkedNpc_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) { burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb); return; }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void linkedNpc_checkShouldSpawn_hook(GB *gb) {
  BASE(linkedNpc_checkShouldSpawn);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto notLinked; }
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+8); TAIL(writeFlagsTocddb);
notLinked:
  CYC(b_+8, b_+10); E = 0x7f;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); push_effect(gb, b_+12);
  do { uint16_t jt_ = (linkedNpc_checkShouldSpawn_jump_table(gb));
    if (jt_ == b_+32) { goto checkd4; }
    else if (jt_ == b_+37) { goto checkd1; }
    else if (jt_ == b_+42) { goto checkd2; }
    else if (jt_ == b_+47) { goto checkd2_2; }
    else if (jt_ == b_+52) { goto always; }
    else { HANDOFF(HL); }
  } while (0);
checkd4:
  CYC(b_+32, b_+34); A = 0x03;
  CYC(b_+34, b_+37); TAIL(checkEssenceNotObtained);
checkd1:
  CYC(b_+37, b_+39); A = 0x00;
  CYC(b_+39, b_+42); TAIL(checkEssenceNotObtained);
checkd2:
  CYC(b_+42, b_+44); A = 0x01;
  CYC(b_+44, b_+47); TAIL(checkEssenceNotObtained);
checkd2_2:
  CYC(b_+47, b_+49); A = 0x01;
  CYC(b_+49, b_+52); TAIL(checkEssenceNotObtained);
always:
  CYC(b_+52, b_+53); alu_or(gb, D);
  CYC(b_+53, b_+56); TAIL(writeFlagsTocddb);
}

void linkedNpc_checkHasExtraTextBox_hook(GB *gb) {
  BASE(linkedNpc_checkHasExtraTextBox);
  CYC(b_+0, b_+2); E = 0x7f;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+12);
  CYC(b_+6, b_+7); linkedNpc_add_a_to_hl(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  CYC(b_+9, b_+12); TAIL(writeFlagsTocddb);
}

void linkedNpc_generateSecret_hook(GB *gb) {
  BASE(linkedNpc_generateSecret);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = GV(0x7f, 0x7e);
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+6); A = GV(0x50, 0x64);
  CYC(b_+6, b_+7); alu_add(gb, B);
  CALL_C(b_+7, setGlobalFlag_hook, SYM(setGlobalFlag), b_+10);
  CYC(b_+10, b_+12); A = GV(0x20, 0x00);
  CYC(b_+12, b_+13); alu_add(gb, B);
  CYC(b_+13, b_+16); mem_wr(gb, wShortSecretIndex, A);
  CYC(b_+16, b_+19); SET_BC(0x0003);
  CYC(b_+19, b_+22); TAIL(secretFunctionCaller);
}

void linkedNpc_initHighTextIndex_hook(GB *gb) {
  BASE(linkedNpc_initHighTextIndex);
  CYC(b_+0, b_+2); A = 0x4d;
  CYC(b_+2, b_+5); TAIL(interactionSetHighTextIndex);
}

void linkedNpc_calcLowTextIndex_hook(GB *gb) {
  BASE(linkedNpc_calcLowTextIndex);
  CYC(b_+O(0), b_+OE(2)); alu_add(gb, 0x00);
  CYC(b_+O(2), b_+OE(3)); C = A;
  CYC(b_+O(3), b_+OE(5)); E = GV(0x7f, 0x7e);
  CYC(b_+O(5), b_+OE(6)); A = mem_rd(gb, DE);
  CYC(b_+O(6), b_+OE(7)); B = A;
  CYC(b_+O(7), b_+OE(8)); alu_add(gb, A);
  if (game_seasons) {
    CYC(b_+S(8), b_+S(9)); alu_add(gb, B);
    CYC(b_+S(9), b_+S(10)); alu_add(gb, A);
  } else {
    CYC(b_+8, b_+9); alu_add(gb, A);
    CYC(b_+9, b_+10); alu_add(gb, B);
  }
  CYC(b_+O(10), b_+OE(11)); alu_add(gb, C);
  CYC(b_+O(11), b_+OE(13)); E = 0x72;
  CYC(b_+O(13), b_+OE(14)); mem_wr(gb, DE, A);
  CYC(b_+O(14), b_+OE(15)); ret_effect(gb);
}
