#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7a54, checkIsLinkedGame_hook, 0x1992, 0x7a57);
  if (!(F & FZ)) { CYCT(0x7a57, 0x7a59); goto notLinked; }
  CYC(0x7a57, 0x7a59);
  CYC(0x7a59, 0x7a5c); writeFlagsTocddb_hook(gb); return;
notLinked:
  CYC(0x7a5c, 0x7a5e); E = 0x7f;
  CYC(0x7a5e, 0x7a5f); A = mem_rd(gb, DE);
  CYC(0x7a5f, 0x7a60); push_effect(gb, 0x7a60);
  switch (linkedNpc_checkShouldSpawn_jump_table(gb)) {
    case 0x7a74: goto checkd4;
    case 0x7a79: goto checkd1;
    case 0x7a7e: goto checkd2;
    case 0x7a83: goto checkd2_2;
    case 0x7a88: goto always;
    default: HANDOFF(HL);
  }
checkd4:
  CYC(0x7a74, 0x7a76); A = 0x03;
  CYC(0x7a76, 0x7a79); checkEssenceNotObtained_hook(gb); return;
checkd1:
  CYC(0x7a79, 0x7a7b); A = 0x00;
  CYC(0x7a7b, 0x7a7e); checkEssenceNotObtained_hook(gb); return;
checkd2:
  CYC(0x7a7e, 0x7a80); A = 0x01;
  CYC(0x7a80, 0x7a83); checkEssenceNotObtained_hook(gb); return;
checkd2_2:
  CYC(0x7a83, 0x7a85); A = 0x01;
  CYC(0x7a85, 0x7a88); checkEssenceNotObtained_hook(gb); return;
always:
  CYC(0x7a88, 0x7a89); alu_or(gb, D);
  CYC(0x7a89, 0x7a8c); writeFlagsTocddb_hook(gb);
}

void linkedNpc_checkHasExtraTextBox_hook(GB *gb) {
  CYC(0x7a8c, 0x7a8e); E = 0x7f;
  CYC(0x7a8e, 0x7a8f); A = mem_rd(gb, DE);
  CYC(0x7a8f, 0x7a92); SET_HL(0x7a98);
  CYC(0x7a92, 0x7a93); linkedNpc_add_a_to_hl(gb, 0x7a93);
  CYC(0x7a93, 0x7a94); A = mem_rd(gb, HL);
  CYC(0x7a94, 0x7a95); alu_or(gb, A);
  CYC(0x7a95, 0x7a98); writeFlagsTocddb_hook(gb);
}

void linkedNpc_generateSecret_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7aa2, 0x7aa3); H = D;
  CYC(0x7aa3, 0x7aa5); L = 0x7f;
  CYC(0x7aa5, 0x7aa6); B = mem_rd(gb, HL);
  CYC(0x7aa6, 0x7aa8); A = 0x50;
  CYC(0x7aa8, 0x7aa9); alu_add(gb, B);
  CALL_C(0x7aa9, setGlobalFlag_hook, 0x31f9, 0x7aac);
  CYC(0x7aac, 0x7aae); A = 0x20;
  CYC(0x7aae, 0x7aaf); alu_add(gb, B);
  CYC(0x7aaf, 0x7ab2); mem_wr(gb, wShortSecretIndex, A);
  CYC(0x7ab2, 0x7ab5); SET_BC(0x0003);
  CYC(0x7ab5, 0x7ab8); secretFunctionCaller_hook(gb);
}

void linkedNpc_initHighTextIndex_hook(GB *gb) {
  CYC(0x7ab8, 0x7aba); A = 0x4d;
  CYC(0x7aba, 0x7abd); interactionSetHighTextIndex_hook(gb);
}

void linkedNpc_calcLowTextIndex_hook(GB *gb) {
  CYC(0x7abd, 0x7abf); alu_add(gb, 0x00);
  CYC(0x7abf, 0x7ac0); C = A;
  CYC(0x7ac0, 0x7ac2); E = 0x7f;
  CYC(0x7ac2, 0x7ac3); A = mem_rd(gb, DE);
  CYC(0x7ac3, 0x7ac4); B = A;
  CYC(0x7ac4, 0x7ac5); alu_add(gb, A);
  CYC(0x7ac5, 0x7ac6); alu_add(gb, A);
  CYC(0x7ac6, 0x7ac7); alu_add(gb, B);
  CYC(0x7ac7, 0x7ac8); alu_add(gb, C);
  CYC(0x7ac8, 0x7aca); E = 0x72;
  CYC(0x7aca, 0x7acb); mem_wr(gb, DE, A);
  CYC(0x7acb, 0x7acc); ret_effect(gb);
}
