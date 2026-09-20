#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(updatePirateShipRoom), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(updatePirateShipRoom), (from), (to), true)

void checkLoadPirateShip_hook(GB *gb);
void updatePirateShipChangedTile_hook(GB *gb);
void updatePirateShipPosition_hook(GB *gb);
void updatePirateShipRoom_hook(GB *gb);
void updatePirateShipAngle_hook(GB *gb);

static void pirateShip_addAToHl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  pop_effect(gb);
}

static uint16_t pirate_ship_jump_table(GB *gb) {
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

static void update_pirate_ship_room_value(GB *gb) {
  BASE(updatePirateShipRoom);
  CYC(b_+22, b_+23); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+23, b_+24); ret_effect(gb);
    return;
  }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); mem_wr(gb, HL, B);
  CYC(b_+25, b_+28); A = W8(wPirateShipRoom);
  CYC(b_+28, b_+29); alu_add(gb, C);
  CYC(b_+29, b_+32); W8(wPirateShipRoom) = A;
  CYC(b_+32, b_+33); ret_effect(gb);
}

void updatePirateShip_hook(GB *gb) {
  BASE(updatePirateShip);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x34;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CALL_C(b_+6, checkLoadPirateShip_hook, SYM(checkLoadPirateShip), b_+9);
  CALL_C(b_+9, updatePirateShipChangedTile_hook, SYM(updatePirateShipChangedTile), b_+12);
  CALL_C(b_+12, updatePirateShipAngle_hook, SYM(updatePirateShipAngle), b_+15);
  CALL_C(b_+15, updatePirateShipPosition_hook, SYM(updatePirateShipPosition), b_+18);
  CYC(b_+18, SYM(checkLoadPirateShip)); updatePirateShipRoom_hook(gb);
}

void checkLoadPirateShip_hook(GB *gb) {
  BASE(checkLoadPirateShip);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x34;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); A = W8(wTilesetFlags);
  CYC(b_+9, b_+10); B = A;
  CYC(b_+10, b_+12); alu_bit(gb, 0, B);
  if (F & FZ) {
    CYCT(b_+12, b_+13); ret_effect(gb);
    return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+15); alu_bit(gb, 6, B);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+16); ret_effect(gb);
    return;
  }
  CYC(b_+15, b_+16);
  CALL_C(b_+16, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+19);
  if (!(F & FZ)) {
    CYCT(b_+19, b_+21);
    goto linked_game;
  }
  CYC(b_+19, b_+21);
  CYC(b_+21, b_+23); alu_bit(gb, 7, B);
  if (F & FZ) {
    CYCT(b_+23, b_+24); ret_effect(gb);
    return;
  }
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+26);
  goto check_room;
linked_game:
  CYC(b_+26, b_+28); alu_bit(gb, 7, B);
  if (!(F & FZ)) {
    CYCT(b_+28, b_+29); ret_effect(gb);
    return;
  }
  CYC(b_+28, b_+29);
check_room:
  CYC(b_+29, b_+32); A = W8(wPirateShipRoom);
  CYC(b_+32, b_+33); B = A;
  CYC(b_+33, b_+36); A = W8(wActiveRoom);
  CYC(b_+36, b_+37); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(b_+37, b_+38); ret_effect(gb);
    return;
  }
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+41); SET_HL(w1ReservedInteraction1_enabled);
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  CYC(b_+42, b_+43); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+43, b_+44); ret_effect(gb);
    return;
  }
  CYC(b_+43, b_+44);
  CYC(b_+44, b_+46); mem_wr(gb, HL, 0x01);
  CYC(b_+46, b_+47); L = alu_inc8(gb, L);
  CYC(b_+47, b_+49); mem_wr(gb, HL, 0xc2);
  CYC(b_+49, b_+51); L = 0x4b;
  CYC(b_+51, b_+54); A = W8(wPirateShipY);
  CYC(b_+54, b_+55); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+55, b_+56); L = alu_inc8(gb, L);
  CYC(b_+56, b_+59); A = W8(wPirateShipX);
  CYC(b_+59, b_+60); mem_wr(gb, HL, A);
  CYC(b_+60, SYM(updatePirateShipChangedTile)); ret_effect(gb);
}

void updatePirateShipChangedTile_hook(GB *gb) {
  BASE(updatePirateShipChangedTile);
  CYC(b_+0, b_+3); A = W8(wPirateShipY);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); A = W8(wPirateShipX);
  CYC(b_+11, b_+13); alu_and(gb, 0x0f);
  CYC(b_+13, b_+15); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(b_+15, b_+16); ret_effect(gb);
    return;
  }
  CYC(b_+15, b_+16);
  CYC(b_+16, b_+19); A = W8(wPirateShipY);
  CYC(b_+19, b_+21); alu_and(gb, 0xf0);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+25); A = W8(wPirateShipX);
  CYC(b_+25, b_+27); A = alu_swap(gb, A);
  CYC(b_+27, b_+29); alu_and(gb, 0x0f);
  CYC(b_+29, b_+30); alu_or(gb, B);
  CYC(b_+30, b_+33); W8(wPirateShipChangedTile) = A;
  CYC(b_+33, SYM(updatePirateShipPosition)); ret_effect(gb);
}

void updatePirateShipPosition_hook(GB *gb) {
  BASE(updatePirateShipPosition);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+6); A = W8(wLinkPlayingInstrument);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(b_+7, b_+8); ret_effect(gb);
    return;
  }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); A = W8(wFrameCounter);
  CYC(b_+11, b_+12); alu_rrca(gb);
  if (F & FC) {
    CYCT(b_+12, b_+13); ret_effect(gb);
    return;
  }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); A = W8(wPirateShipAngle);
  CYC(b_+16, b_+18); alu_and(gb, 0x03);
  CYC(b_+18, b_+21); SET_DE(b_+35);
  CALL_C(b_+21, addDoubleIndexToDe_hook, 0x0072, b_+24);
  CYC(b_+24, b_+27); SET_HL(wPirateShipY);
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+29, b_+30); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+30, b_+31); SET_DE(DE + 1);
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+33); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+35); ret_effect(gb);
}

void updatePirateShipRoom_hook(GB *gb) {
  BASE(updatePirateShipRoom);
  CYC(b_+0, b_+3); A = W8(wPirateShipAngle);
  CYC(b_+3, b_+5); alu_and(gb, 0x03);
  CYC(b_+5, b_+6); push_effect(gb, b_+6);
  do { uint16_t jt_ = (pirate_ship_jump_table(gb));
    if (jt_ == b_+14) {
      CYC(b_+14, b_+17); SET_HL(wPirateShipY);
      CYC(b_+17, b_+20); SET_BC(0x80f0);
      CYC(b_+20, b_+22); A = 0xf8;
      update_pirate_ship_room_value(gb);
      return;
    }
    else if (jt_ == b_+33) {
      CYC(b_+33, b_+36); SET_HL(wPirateShipX);
      CYC(b_+36, b_+39); SET_BC(0x0001);
      CYC(b_+39, b_+41); A = 0x98;
      CYC(b_+41, b_+43);
      update_pirate_ship_room_value(gb);
      return;
    }
    else if (jt_ == b_+43) {
      CYC(b_+43, b_+46); SET_HL(wPirateShipY);
      CYC(b_+46, b_+49); SET_BC(0x0010);
      CYC(b_+49, b_+51); A = 0x88;
      CYC(b_+51, b_+53);
      update_pirate_ship_room_value(gb);
      return;
    }
    else if (jt_ == b_+53) {
      CYC(b_+53, b_+56); SET_HL(wPirateShipX);
      CYC(b_+56, b_+59); SET_BC(0xa0ff);
      CYC(b_+59, b_+61); A = 0xf8;
      CYC(b_+61, SYM(updatePirateShipAngle));
      update_pirate_ship_room_value(gb);
      return;
    }
    else {
      hook_handoff(gb, HL);
      return;
    }
  } while (0);
}

void updatePirateShipAngle_hook(GB *gb) {
  BASE(updatePirateShipAngle);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wPirateShipChangedTile);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+5); ret_effect(gb);
    return;
  }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); hram_wr(gb, 0x8b, A);
  CYC(b_+7, b_+8); alu_xor(gb, A);
  CYC(b_+8, b_+11); W8(wPirateShipChangedTile) = A;
  CYC(b_+11, b_+14); SET_HL(b_+88);
  CALL_C(b_+14, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+17);
  if (F & FZ) {
    CYCT(b_+17, b_+19);
  } else {
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+22); SET_HL(b_+51);
  }
next_direction:
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+24, b_+25); ret_effect(gb);
    return;
  }
  CYC(b_+24, b_+25);
  CYC(b_+25, b_+26); push_effect(gb, HL);
  CYC(b_+26, b_+29); A = W8(wPirateShipRoom);
  CYC(b_+29, b_+30); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+30, b_+32);
    goto next_entry;
  }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); SET_HL(HL + 1);
  CYC(b_+33, b_+35); A = hram_rd(gb, 0x8b);
  CYC(b_+35, b_+36); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(b_+36, b_+38);
    goto next_entry;
  }
  CYC(b_+36, b_+38);
  CYC(b_+38, b_+39); SET_HL(HL + 1);
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+43); W8(wPirateShipAngle) = A;
  CYC(b_+43, b_+44); SET_HL(pop_effect(gb));
  CYC(b_+44, b_+45); ret_effect(gb);
  return;
next_entry:
  CYC(b_+45, b_+46); SET_HL(pop_effect(gb));
  CYC(b_+46, b_+48); A = 0x03;
  CYC(b_+48, b_+49); pirateShip_addAToHl(gb, b_+49);
  CYC(b_+49, b_+51);
  goto next_direction;
}
