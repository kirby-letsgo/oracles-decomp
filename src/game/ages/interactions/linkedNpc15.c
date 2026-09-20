#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(linkedNpc_checkShouldSpawn), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(linkedNpc_checkShouldSpawn), (from), (to), true)

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
  CYC(b_+5, b_+8); writeFlagsTocddb_hook(gb); return;
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
  CYC(b_+34, b_+37); checkEssenceNotObtained_hook(gb); return;
checkd1:
  CYC(b_+37, b_+39); A = 0x00;
  CYC(b_+39, b_+42); checkEssenceNotObtained_hook(gb); return;
checkd2:
  CYC(b_+42, b_+44); A = 0x01;
  CYC(b_+44, b_+47); checkEssenceNotObtained_hook(gb); return;
checkd2_2:
  CYC(b_+47, b_+49); A = 0x01;
  CYC(b_+49, b_+52); checkEssenceNotObtained_hook(gb); return;
always:
  CYC(b_+52, b_+53); alu_or(gb, D);
  CYC(b_+53, SYM(linkedNpc_checkHasExtraTextBox)); writeFlagsTocddb_hook(gb);
}

void linkedNpc_checkHasExtraTextBox_hook(GB *gb) {
  BASE(linkedNpc_checkHasExtraTextBox);
  CYC(b_+0, b_+2); E = 0x7f;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+12);
  CYC(b_+6, b_+7); linkedNpc_add_a_to_hl(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  CYC(b_+9, b_+12); writeFlagsTocddb_hook(gb);
}

void linkedNpc_generateSecret_hook(GB *gb) {
  BASE(linkedNpc_generateSecret);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7f;
  CYC(b_+3, b_+4); B = mem_rd(gb, HL);
  CYC(b_+4, b_+6); A = 0x50;
  CYC(b_+6, b_+7); alu_add(gb, B);
  CALL_C(b_+7, setGlobalFlag_hook, SYM(setGlobalFlag), b_+10);
  CYC(b_+10, b_+12); A = 0x20;
  CYC(b_+12, b_+13); alu_add(gb, B);
  CYC(b_+13, b_+16); mem_wr(gb, wShortSecretIndex, A);
  CYC(b_+16, b_+19); SET_BC(0x0003);
  CYC(b_+19, SYM(linkedNpc_initHighTextIndex)); secretFunctionCaller_hook(gb);
}

void linkedNpc_initHighTextIndex_hook(GB *gb) {
  BASE(linkedNpc_initHighTextIndex);
  CYC(b_+0, b_+2); A = 0x4d;
  CYC(b_+2, SYM(linkedNpc_calcLowTextIndex)); interactionSetHighTextIndex_hook(gb);
}

void linkedNpc_calcLowTextIndex_hook(GB *gb) {
  BASE(linkedNpc_calcLowTextIndex);
  CYC(b_+0, b_+2); alu_add(gb, 0x00);
  CYC(b_+2, b_+3); C = A;
  CYC(b_+3, b_+5); E = 0x7f;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); B = A;
  CYC(b_+7, b_+8); alu_add(gb, A);
  CYC(b_+8, b_+9); alu_add(gb, A);
  CYC(b_+9, b_+10); alu_add(gb, B);
  CYC(b_+10, b_+11); alu_add(gb, C);
  CYC(b_+11, b_+13); E = 0x72;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, SYM(plenSubid0Script_b15)); ret_effect(gb);
}
