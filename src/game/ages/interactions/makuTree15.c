#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(makuTree_setAnimation), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(makuTree_setAnimation), (from), (to), true)

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
  BASE(makuTree_setAnimation);
  CYC(b_+0, b_+2); E = 0x7b;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A);
  CYC(b_+3, SYM(makuTree_showTextWithOffsetAndUpdateMapText)); interactionSetAnimation_hook(gb);
}

void makuTree_showTextWithOffsetAndUpdateMapText_hook(GB *gb) {
  BASE(makuTree_showTextWithOffsetAndUpdateMapText);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, makuTree_func_70a2_hook, SYM(makuTree_func_70a2), b_+3);
  CYC(b_+3, SYM(makuTree_showTextWithOffset)); label_15_203_hook(gb);
}

void makuTree_showTextWithOffset_hook(GB *gb) {
  BASE(makuTree_showTextWithOffset);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, makuTree_func_709c_hook, SYM(makuTree_func_709c), b_+3);
  CYC(b_+3, SYM(makuTree_showTextAndUpdateMapText)); label_15_203_hook(gb);
}

void makuTree_showTextAndUpdateMapText_hook(GB *gb) {
  BASE(makuTree_showTextAndUpdateMapText);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, makuTree_checkLinkedAndUpdateMapText_hook, SYM(makuTree_checkLinkedAndUpdateMapText), b_+3);
  CYC(b_+3, SYM(makuTree_showText)); label_15_203_hook(gb);
}

void makuTree_showText_hook(GB *gb) {
  BASE(makuTree_showText);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, makuTree_modifyTextIndexForLinked_hook, SYM(makuTree_modifyTextIndexForLinked), b_+3);
  CYC(b_+3, SYM(label_15_203)); label_15_203_hook(gb);
}

void label_15_203_hook(GB *gb) {
  BASE(label_15_203);
  CYC(b_+0, b_+2); B = 0x05;
  CYC(b_+2, SYM(makuTree_func_709c)); showText_hook(gb);
}

void makuTree_func_709c_hook(GB *gb) {
  BASE(makuTree_func_709c);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7f;
  CYC(b_+3, b_+4); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+4, SYM(makuTree_func_70a2)); makuTree_modifyTextIndexForLinked_hook(gb);
}

void makuTree_func_70a2_hook(GB *gb) {
  BASE(makuTree_func_70a2);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x7f;
  CYC(b_+3, SYM(makuTree_checkLinkedAndUpdateMapText)); alu_add(gb, mem_rd(gb, HL));
  makuTree_checkLinkedAndUpdateMapText_hook(gb);
}

void makuTree_checkLinkedAndUpdateMapText_hook(GB *gb) {
  BASE(makuTree_checkLinkedAndUpdateMapText);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, makuTree_modifyTextIndexForLinked_hook, SYM(makuTree_modifyTextIndexForLinked), b_+3);
  CYC(b_+3, b_+5); E = 0x7d;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+9); SET_HL(wMakuMapTextPresent);
  CYC(b_+9, b_+10); makuTree_add_a_to_hl(gb, b_+10);
  CYC(b_+10, b_+11); mem_wr(gb, HL, C);
  CYC(b_+11, SYM(makuTree_modifyTextIndexForLinked)); ret_effect(gb);
}

void makuTree_modifyTextIndexForLinked_hook(GB *gb) {
  BASE(makuTree_modifyTextIndexForLinked);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); C = A;
  CALL_C(b_+1, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+4);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); push_effect(gb, b_+8); asm_call(gb, b_+16, b_+8);
  CYC(b_+8, b_+11); SET_HL(SYM(makuTree_textOffsetsForLinked));
  CYC(b_+11, b_+12); makuTree_add_a_to_hl(gb, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+14); alu_add(gb, C);
  CYC(b_+14, b_+15); C = A;
  CYC(b_+15, b_+16); ret_effect(gb);
}

void makuTree_dropSeedSatchel_hook(GB *gb) {
  BASE(makuTree_dropSeedSatchel);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CALL_C(b_+8, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+11);
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x60);
  CYC(b_+13, b_+14); L = alu_inc8(gb, L);
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x19);
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x02);
  CYC(b_+19, b_+21); L = 0x4b;
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x60);
  CYC(b_+23, b_+26); A = mem_rd(gb, w1Link_xh);
  CYC(b_+26, b_+28); B = 0x50;
  CYC(b_+28, b_+30); alu_cp(gb, 0x64);
  if (!(F & FC)) { CYCT(b_+30, b_+32); goto setX; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); alu_cp(gb, 0x3c);
  if (F & FC) { CYCT(b_+34, b_+36); goto setX; }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+38); B = 0x40;
  CYC(b_+38, b_+40); alu_cp(gb, 0x50);
  if (!(F & FC)) { CYCT(b_+40, b_+42); goto setX; }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+44); B = 0x60;
setX:
  CYC(b_+44, b_+46); L = 0x4d;
  CYC(b_+46, b_+47); mem_wr(gb, HL, B);
  CYC(b_+47, b_+48); A = B;
  CYC(b_+48, b_+51); mem_wr(gb, wMakuTreeSeedSatchelXPosition, A);
  CYC(b_+51, SYM(makuTree_checkSpawnSeedSatchel)); ret_effect(gb);
}

void makuTree_checkSpawnSeedSatchel_hook(GB *gb) {
  BASE(makuTree_checkSpawnSeedSatchel);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+3);
  CYC(b_+3, b_+5); alu_bit(gb, 5, A);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+8); return; }
  CYC(b_+8, b_+9);
  CALL_C(b_+9, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+12);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x60);
  CYC(b_+14, b_+15); L = alu_inc8(gb, L);
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x19);
  CYC(b_+17, b_+18); L = alu_inc8(gb, L);
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x03);
  CYC(b_+20, b_+22); L = 0x4b;
  CYC(b_+22, b_+24); A = 0x58;
  CYC(b_+24, b_+25); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+25, b_+28); A = mem_rd(gb, wMakuTreeSeedSatchelXPosition);
  CYC(b_+28, b_+30); L = 0x4d;
  CYC(b_+30, b_+31); mem_wr(gb, HL, A);
  CYC(b_+31, SYM(makuTree_spawnMakuSeed)); ret_effect(gb);
}

void makuTree_spawnMakuSeed_hook(GB *gb) {
  BASE(makuTree_spawnMakuSeed);
  CYC(b_+0, b_+3); SET_BC(0xa600);
  CYC(b_+3, SYM(makuTree_chooseTextAfterSeeingTwinrova)); objectCreateInteraction_hook(gb);
}

void makuTree_chooseTextAfterSeeingTwinrova_hook(GB *gb) {
  BASE(makuTree_chooseTextAfterSeeingTwinrova);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); C = 0x5b;
  CALL_C(b_+2, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+5);
  if (F & FZ) { CYCT(b_+5, b_+7); goto setE; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); C = 0x5f;
setE:
  CYC(b_+9, b_+11); E = 0x72;
  CYC(b_+11, b_+12); A = C;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, SYM(makuTree_subid00Script_body)); ret_effect(gb);
}
