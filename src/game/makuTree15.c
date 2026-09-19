#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void label_15_203_hook(GB *gb);
void makuTree_func_709c_hook(GB *gb);
void makuTree_func_70a2_hook(GB *gb);
void makuTree_checkLinkedAndUpdateMapText_hook(GB *gb);
void makuTree_modifyTextIndexForLinked_hook(GB *gb);

static void makuTree_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) { burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb); return; }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void makuTree_setAnimation_hook(GB *gb) {
  CYC(0x707c, 0x707e); E = 0x7b;
  CYC(0x707e, 0x707f); mem_wr(gb, DE, A);
  CYC(0x707f, 0x7082); interactionSetAnimation_hook(gb);
}

void makuTree_showTextWithOffsetAndUpdateMapText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7082, makuTree_func_70a2_hook, 0x70a2, 0x7085);
  CYC(0x7085, 0x7087); label_15_203_hook(gb);
}

void makuTree_showTextWithOffset_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7087, makuTree_func_709c_hook, 0x709c, 0x708a);
  CYC(0x708a, 0x708c); label_15_203_hook(gb);
}

void makuTree_showTextAndUpdateMapText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x708c, makuTree_checkLinkedAndUpdateMapText_hook, 0x70a6, 0x708f);
  CYC(0x708f, 0x7091); label_15_203_hook(gb);
}

void makuTree_showText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7091, makuTree_modifyTextIndexForLinked_hook, 0x70b2, 0x7094);
  CYC(0x7094, 0x7097); label_15_203_hook(gb);
}

void label_15_203_hook(GB *gb) {
  CYC(0x7097, 0x7099); B = 0x05;
  CYC(0x7099, 0x709c); showText_hook(gb);
}

void makuTree_func_709c_hook(GB *gb) {
  CYC(0x709c, 0x709d); H = D;
  CYC(0x709d, 0x709f); L = 0x7f;
  CYC(0x709f, 0x70a0); alu_add(gb, mem_rd(gb, HL));
  CYC(0x70a0, 0x70a2); makuTree_modifyTextIndexForLinked_hook(gb);
}

void makuTree_func_70a2_hook(GB *gb) {
  CYC(0x70a2, 0x70a3); H = D;
  CYC(0x70a3, 0x70a5); L = 0x7f;
  CYC(0x70a5, 0x70a6); alu_add(gb, mem_rd(gb, HL));
  makuTree_checkLinkedAndUpdateMapText_hook(gb);
}

void makuTree_checkLinkedAndUpdateMapText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x70a6, makuTree_modifyTextIndexForLinked_hook, 0x70b2, 0x70a9);
  CYC(0x70a9, 0x70ab); E = 0x7d;
  CYC(0x70ab, 0x70ac); A = mem_rd(gb, DE);
  CYC(0x70ac, 0x70af); SET_HL(wMakuMapTextPresent);
  CYC(0x70af, 0x70b0); makuTree_add_a_to_hl(gb, 0x70b0);
  CYC(0x70b0, 0x70b1); mem_wr(gb, HL, C);
  CYC(0x70b1, 0x70b2); ret_effect(gb);
}

void makuTree_modifyTextIndexForLinked_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x70b2, 0x70b3); C = A;
  CALL_C(0x70b3, checkIsLinkedGame_hook, 0x1992, 0x70b6);
  if (F & FZ) { RET_TAKEN(0x70b6); return; }
  CYC(0x70b6, 0x70b7);
  CYC(0x70b7, 0x70ba); push_effect(gb, 0x70ba); asm_call(gb, 0x70c2, 0x70ba);
  CYC(0x70ba, 0x70bd); SET_HL(0x70ce);
  CYC(0x70bd, 0x70be); makuTree_add_a_to_hl(gb, 0x70be);
  CYC(0x70be, 0x70bf); A = mem_rd(gb, HL);
  CYC(0x70bf, 0x70c0); alu_add(gb, C);
  CYC(0x70c0, 0x70c1); C = A;
  CYC(0x70c1, 0x70c2); ret_effect(gb);
}

void makuTree_dropSeedSatchel_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x70d1, getThisRoomFlags_hook, 0x197d, 0x70d4);
  CYC(0x70d4, 0x70d6); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(0x70d6); return; }
  CYC(0x70d6, 0x70d7);
  CYC(0x70d7, 0x70d9); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(0x70d9, getFreeInteractionSlot_hook, 0x3aef, 0x70dc);
  CYC(0x70dc, 0x70de); mem_wr(gb, HL, 0x60);
  CYC(0x70de, 0x70df); L = alu_inc8(gb, L);
  CYC(0x70df, 0x70e1); mem_wr(gb, HL, 0x19);
  CYC(0x70e1, 0x70e2); L = alu_inc8(gb, L);
  CYC(0x70e2, 0x70e4); mem_wr(gb, HL, 0x02);
  CYC(0x70e4, 0x70e6); L = 0x4b;
  CYC(0x70e6, 0x70e8); mem_wr(gb, HL, 0x60);
  CYC(0x70e8, 0x70eb); A = mem_rd(gb, w1Link_xh);
  CYC(0x70eb, 0x70ed); B = 0x50;
  CYC(0x70ed, 0x70ef); alu_cp(gb, 0x64);
  if (!(F & FC)) { CYCT(0x70ef, 0x70f1); goto setX; }
  CYC(0x70ef, 0x70f1);
  CYC(0x70f1, 0x70f3); alu_cp(gb, 0x3c);
  if (F & FC) { CYCT(0x70f3, 0x70f5); goto setX; }
  CYC(0x70f3, 0x70f5);
  CYC(0x70f5, 0x70f7); B = 0x40;
  CYC(0x70f7, 0x70f9); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(0x70f9, 0x70fb); goto setX; }
  CYC(0x70f9, 0x70fb);
  CYC(0x70fb, 0x70fd); B = 0x60;
setX:
  CYC(0x70fd, 0x70ff); L = 0x4d;
  CYC(0x70ff, 0x7100); mem_wr(gb, HL, B);
  CYC(0x7100, 0x7101); A = B;
  CYC(0x7101, 0x7104); mem_wr(gb, wMakuTreeSeedSatchelXPosition, A);
  CYC(0x7104, 0x7105); ret_effect(gb);
}

void makuTree_checkSpawnSeedSatchel_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x7105, getThisRoomFlags_hook, 0x197d, 0x7108);
  CYC(0x7108, 0x710a); alu_bit(gb, 5, A);
  if (!(F & FZ)) { RET_TAKEN(0x710a); return; }
  CYC(0x710a, 0x710b);
  CYC(0x710b, 0x710d); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(0x710d); return; }
  CYC(0x710d, 0x710e);
  CALL_C(0x710e, getFreeInteractionSlot_hook, 0x3aef, 0x7111);
  CYC(0x7111, 0x7113); mem_wr(gb, HL, 0x60);
  CYC(0x7113, 0x7114); L = alu_inc8(gb, L);
  CYC(0x7114, 0x7116); mem_wr(gb, HL, 0x19);
  CYC(0x7116, 0x7117); L = alu_inc8(gb, L);
  CYC(0x7117, 0x7119); mem_wr(gb, HL, 0x03);
  CYC(0x7119, 0x711b); L = 0x4b;
  CYC(0x711b, 0x711d); A = 0x58;
  CYC(0x711d, 0x711e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x711e, 0x7121); A = mem_rd(gb, wMakuTreeSeedSatchelXPosition);
  CYC(0x7121, 0x7123); L = 0x4d;
  CYC(0x7123, 0x7124); mem_wr(gb, HL, A);
  CYC(0x7124, 0x7125); ret_effect(gb);
}

void makuTree_spawnMakuSeed_hook(GB *gb) {
  CYC(0x7125, 0x7128); SET_BC(0xa600);
  CYC(0x7128, 0x712b); objectCreateInteraction_hook(gb);
}

void makuTree_chooseTextAfterSeeingTwinrova_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x712b, 0x712d); C = 0x5b;
  CALL_C(0x712d, checkIsLinkedGame_hook, 0x1992, 0x7130);
  if (F & FZ) { CYCT(0x7130, 0x7132); goto setE; }
  CYC(0x7130, 0x7132);
  CYC(0x7132, 0x7134); C = 0x5f;
setE:
  CYC(0x7134, 0x7136); E = 0x72;
  CYC(0x7136, 0x7137); A = C;
  CYC(0x7137, 0x7138); mem_wr(gb, DE, A);
  CYC(0x7138, 0x7139); ret_effect(gb);
}
