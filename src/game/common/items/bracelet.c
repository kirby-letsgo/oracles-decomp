#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void braceletCheckBreakable_hook(GB *gb);
void braceletCheckDeleteSelfWhileThrowing_hook(GB *gb);
void itemBeginThrow_hook(GB *gb);
void itemUpdateThrowingLaterally_hook(GB *gb);

static uint16_t bracelet_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void itemCode16_hook(GB *gb) {
  BASE(itemCode16);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (bracelet_jump_table(gb));
    if (jt_ == b_+12) { goto state0; }
    else if (jt_ == b_+36) { goto held; }
    else if (jt_ == b_+102) { goto thrown; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+12, itemLoadAttributesAndGraphics_hook, SYM(itemLoadAttributesAndGraphics), b_+15);
  CYC(b_+15, b_+16); H = D;
  CYC(b_+16, b_+18); L = 0x00;
  CYC(b_+18, b_+20); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1)));
  CYC(b_+20, b_+22); L = 0x02;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+24, b_+26);
    goto not_tile;
  }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+28); L = 0x04;
  CYC(b_+28, b_+30); mem_wr(gb, HL, 0x02);
  CALL_C(b_+30, itemMimicBgTile_hook, SYM(itemMimicBgTile), b_+33);
  CYC(b_+33, b_+36);
  TAIL(objectSetVisiblec0);

held:
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+39); L = 0x05;
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+41); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+41, b_+42); ret_effect(gb); return; }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+44); L = 0x27;
  CYC(b_+44, b_+46); A = 0x06;
  CYC(b_+46, b_+47); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+48, b_+49); L = alu_dec8(gb, L);
  CYC(b_+49, b_+51); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+51, b_+53);
  goto throw_item;

not_tile:
  CALL_C(b_+53, braceletCheckDeleteSelfWhileThrowing_hook, SYM(braceletCheckDeleteSelfWhileThrowing), b_+56);
  CYC(b_+56, b_+57); A = H;
  CYC(b_+57, b_+59); alu_cp(gb, 0xd1);
  if (F & FZ) {
    CYCT(b_+59, b_+61);
    goto copy_collisions;
  }
  CYC(b_+59, b_+61);
  CYC(b_+61, b_+62); A = L;
  CYC(b_+62, b_+64); alu_cp(gb, 0x40);
  if (F & FC) {
    CYCT(b_+64, b_+66);
    goto throw_item;
  }
  CYC(b_+64, b_+66);

copy_collisions:
  CYC(b_+66, b_+68); A = 0x09;
  CALL_C(b_+68, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+71);
  CYC(b_+71, b_+73); E = 0x09;
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+75); mem_wr(gb, HL, A);
  CYC(b_+75, b_+76); A = L;
  CYC(b_+76, b_+78); alu_add(gb, 0x1d);
  CYC(b_+78, b_+79); L = A;
  CYC(b_+79, b_+81); E = 0x26;
  CYC(b_+81, b_+82); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+82, b_+83); mem_wr(gb, DE, A);
  CYC(b_+83, b_+84); E = alu_inc8(gb, E);
  CYC(b_+84, b_+85); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+87); H = D;
  CYC(b_+87, b_+89); L = 0x24;
  CYC(b_+89, b_+91); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));

throw_item:
  CALL_C(b_+91, itemBeginThrow_hook, SYM(itemBeginThrow), b_+94);
  CYC(b_+94, b_+95); H = D;
  CYC(b_+95, b_+97); L = 0x04;
  CYC(b_+97, b_+99); mem_wr(gb, HL, 0x03);
  CYC(b_+99, b_+100); L = alu_inc8(gb, L);
  CYC(b_+100, b_+102); mem_wr(gb, HL, 0x00);

thrown:
  CALL_C(b_+102, braceletCheckDeleteSelfWhileThrowing_hook, SYM(braceletCheckDeleteSelfWhileThrowing), b_+105);
  CALL_C(b_+105, itemUpdateThrowingLaterally_hook, SYM(itemUpdateThrowingLaterally), b_+108);
  if (F & FZ) {
    CYCT(b_+108, b_+110);
    goto destroy;
  }
  CYC(b_+108, b_+110);
  CYC(b_+110, b_+112); E = 0x39;
  CYC(b_+112, b_+113); A = mem_rd(gb, DE);
  CYC(b_+113, b_+114); C = A;
  CALL_C(b_+114, itemUpdateThrowingVertically_hook, SYM(itemUpdateThrowingVertically), b_+117);
  if (!(F & FC)) {
    CYCT(b_+117, b_+119);
    goto no_collision;
  }
  CYC(b_+117, b_+119);
  CALL_C(b_+119, braceletCheckBreakable_hook, SYM(braceletCheckBreakable), b_+122);
  if (!(F & FZ)) {
    CYCT(b_+122, b_+124);
    goto destroy;
  }
  CYC(b_+122, b_+124);
  CALL_C(b_+124, itemBounce_hook, SYM(itemBounce), b_+127);
  if (F & FC) {
    CYCT(b_+127, b_+129);
    goto release;
  }
  CYC(b_+127, b_+129);

no_collision:
  CYC(b_+129, b_+131); E = 0x02;
  CYC(b_+131, b_+132); A = mem_rd(gb, DE);
  CYC(b_+132, b_+133); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+133, b_+134); ret_effect(gb); return; }
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+136); A = 0x0b;
  CALL_C(b_+136, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+139);
  CYC(b_+139, b_+142);
  TAIL(objectCopyPosition);

release:
  CYC(b_+142, b_+144); A = 0x05;
  CALL_C(b_+144, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+147);
  CYC(b_+147, b_+149); mem_wr(gb, HL, 0x03);
  CYC(b_+149, b_+152);
  TAIL(itemDelete);

destroy:
  CALL_C(b_+152, objectReplaceWithAnimationIfOnHazard_hook, SYM(objectReplaceWithAnimationIfOnHazard), b_+155);
  if (F & FC) { CYCT(b_+155, b_+156); ret_effect(gb); return; }
  CYC(b_+155, b_+156);
  CYC(b_+156, b_+159); SET_HL((SYM(applyDamageToLink) + 41));
  CYC(b_+159, b_+161); E = 0x06;
  CALL_C(b_+161, interBankCall_hook, 0x008a, b_+164);
  CYC(b_+164, b_+167);
  itemDelete_hook(gb);
}

void braceletCheckBreakable_hook(GB *gb) {
  BASE(braceletCheckBreakable);
  CYC(b_+0, b_+2); E = 0x02;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); alu_scf(gb);
  CYC(b_+6, b_+7); ret_effect(gb);
}

void braceletCheckDeleteSelfWhileThrowing_hook(GB *gb) {
  BASE(braceletCheckDeleteSelfWhileThrowing);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x02;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+4, b_+6);
    goto throwing_tile;
  }
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CALL_C(b_+7, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+10);
  CYC(b_+10, b_+12); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(b_+12, b_+14);
    goto delete_self;
  }
  CYC(b_+12, b_+14);
  CYC(b_+14, b_+15); A = L;
  CYC(b_+15, b_+17); alu_add(gb, 0x04);
  CYC(b_+17, b_+18); L = A;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+21); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(b_+21, b_+23);
    goto delete_self;
  }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+26); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+26, b_+27); ret_effect(gb); return; }
  CYC(b_+26, b_+27);

delete_self:
  CYC(b_+27, b_+28); SET_AF(pop_effect(gb));
  CYC(b_+28, b_+31);
  TAIL(itemDelete);

throwing_tile:
  CALL_C(b_+31, objectCheckWithinRoomBoundary_hook, SYM(objectCheckWithinRoomBoundary), b_+34);
  if (!(F & FC)) {
    CYCT(b_+34, b_+36);
    goto delete_self;
  }
  CYC(b_+34, b_+36);
  CYC(b_+36, b_+37); H = D;
  CYC(b_+37, b_+39); L = 0x05;
  CYC(b_+39, b_+40); ret_effect(gb);
}
