#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x01, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x01, (from), (to), true)

void checkLoadPirateShip_hook(GB *gb);
void updatePirateShipChangedTile_hook(GB *gb);
void updatePirateShipPosition_hook(GB *gb);
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

void updatePirateShip_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7dcc, 0x7dce); A = 0x34;
  CALL_C(0x7dce, checkGlobalFlag_hook, 0x31f3, 0x7dd1);
  if (!(F & FZ)) {
    CYCT(0x7dd1, 0x7dd2); ret_effect(gb);
    return;
  }
  CYC(0x7dd1, 0x7dd2);
  CALL_C(0x7dd2, checkLoadPirateShip_hook, 0x7de1, 0x7dd5);
  CALL_C(0x7dd5, updatePirateShipChangedTile_hook, 0x7e1e, 0x7dd8);
  CALL_C(0x7dd8, updatePirateShipAngle_hook, 0x7eaa, 0x7ddb);
  CALL_C(0x7ddb, updatePirateShipPosition_hook, 0x7e40, 0x7dde);
  CYC(0x7dde, 0x7de1); updatePirateShipRoom(gb);
}

void checkLoadPirateShip_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7de1, 0x7de3); A = 0x34;
  CALL_C(0x7de3, checkGlobalFlag_hook, 0x31f3, 0x7de6);
  if (!(F & FZ)) {
    CYCT(0x7de6, 0x7de7); ret_effect(gb);
    return;
  }
  CYC(0x7de6, 0x7de7);
  CYC(0x7de7, 0x7dea); A = W8(wTilesetFlags);
  CYC(0x7dea, 0x7deb); B = A;
  CYC(0x7deb, 0x7ded); alu_bit(gb, 0, B);
  if (F & FZ) {
    CYCT(0x7ded, 0x7dee); ret_effect(gb);
    return;
  }
  CYC(0x7ded, 0x7dee);
  CYC(0x7dee, 0x7df0); alu_bit(gb, 6, B);
  if (!(F & FZ)) {
    CYCT(0x7df0, 0x7df1); ret_effect(gb);
    return;
  }
  CYC(0x7df0, 0x7df1);
  CALL_C(0x7df1, checkIsLinkedGame_hook, 0x1992, 0x7df4);
  if (!(F & FZ)) {
    CYCT(0x7df4, 0x7df6);
    goto linked_game;
  }
  CYC(0x7df4, 0x7df6);
  CYC(0x7df6, 0x7df8); alu_bit(gb, 7, B);
  if (F & FZ) {
    CYCT(0x7df8, 0x7df9); ret_effect(gb);
    return;
  }
  CYC(0x7df8, 0x7df9);
  CYC(0x7df9, 0x7dfb);
  goto check_room;
linked_game:
  CYC(0x7dfb, 0x7dfd); alu_bit(gb, 7, B);
  if (!(F & FZ)) {
    CYCT(0x7dfd, 0x7dfe); ret_effect(gb);
    return;
  }
  CYC(0x7dfd, 0x7dfe);
check_room:
  CYC(0x7dfe, 0x7e01); A = W8(wPirateShipRoom);
  CYC(0x7e01, 0x7e02); B = A;
  CYC(0x7e02, 0x7e05); A = W8(wActiveRoom);
  CYC(0x7e05, 0x7e06); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x7e06, 0x7e07); ret_effect(gb);
    return;
  }
  CYC(0x7e06, 0x7e07);
  CYC(0x7e07, 0x7e0a); SET_HL(w1ReservedInteraction1_enabled);
  CYC(0x7e0a, 0x7e0b); A = mem_rd(gb, HL);
  CYC(0x7e0b, 0x7e0c); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7e0c, 0x7e0d); ret_effect(gb);
    return;
  }
  CYC(0x7e0c, 0x7e0d);
  CYC(0x7e0d, 0x7e0f); mem_wr(gb, HL, 0x01);
  CYC(0x7e0f, 0x7e10); L = alu_inc8(gb, L);
  CYC(0x7e10, 0x7e12); mem_wr(gb, HL, 0xc2);
  CYC(0x7e12, 0x7e14); L = 0x4b;
  CYC(0x7e14, 0x7e17); A = W8(wPirateShipY);
  CYC(0x7e17, 0x7e18); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7e18, 0x7e19); L = alu_inc8(gb, L);
  CYC(0x7e19, 0x7e1c); A = W8(wPirateShipX);
  CYC(0x7e1c, 0x7e1d); mem_wr(gb, HL, A);
  CYC(0x7e1d, 0x7e1e); ret_effect(gb);
}

void updatePirateShipChangedTile_hook(GB *gb) {
  CYC(0x7e1e, 0x7e21); A = W8(wPirateShipY);
  CYC(0x7e21, 0x7e23); alu_and(gb, 0x0f);
  CYC(0x7e23, 0x7e25); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x7e25, 0x7e26); ret_effect(gb);
    return;
  }
  CYC(0x7e25, 0x7e26);
  CYC(0x7e26, 0x7e29); A = W8(wPirateShipX);
  CYC(0x7e29, 0x7e2b); alu_and(gb, 0x0f);
  CYC(0x7e2b, 0x7e2d); alu_cp(gb, 0x08);
  if (!(F & FZ)) {
    CYCT(0x7e2d, 0x7e2e); ret_effect(gb);
    return;
  }
  CYC(0x7e2d, 0x7e2e);
  CYC(0x7e2e, 0x7e31); A = W8(wPirateShipY);
  CYC(0x7e31, 0x7e33); alu_and(gb, 0xf0);
  CYC(0x7e33, 0x7e34); B = A;
  CYC(0x7e34, 0x7e37); A = W8(wPirateShipX);
  CYC(0x7e37, 0x7e39); A = alu_swap(gb, A);
  CYC(0x7e39, 0x7e3b); alu_and(gb, 0x0f);
  CYC(0x7e3b, 0x7e3c); alu_or(gb, B);
  CYC(0x7e3c, 0x7e3f); W8(wPirateShipChangedTile) = A;
  CYC(0x7e3f, 0x7e40); ret_effect(gb);
}

void updatePirateShipPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7e40, retIfTextIsActive_hook, 0x1859, 0x7e43);
  CYC(0x7e43, 0x7e46); A = W8(wLinkPlayingInstrument);
  CYC(0x7e46, 0x7e47); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x7e47, 0x7e48); ret_effect(gb);
    return;
  }
  CYC(0x7e47, 0x7e48);
  CYC(0x7e48, 0x7e4b); A = W8(wFrameCounter);
  CYC(0x7e4b, 0x7e4c); alu_rrca(gb);
  if (F & FC) {
    CYCT(0x7e4c, 0x7e4d); ret_effect(gb);
    return;
  }
  CYC(0x7e4c, 0x7e4d);
  CYC(0x7e4d, 0x7e50); A = W8(wPirateShipAngle);
  CYC(0x7e50, 0x7e52); alu_and(gb, 0x03);
  CYC(0x7e52, 0x7e55); SET_DE(0x7e63);
  CALL_C(0x7e55, addDoubleIndexToDe_hook, 0x0072, 0x7e58);
  CYC(0x7e58, 0x7e5b); SET_HL(wPirateShipY);
  CYC(0x7e5b, 0x7e5c); A = mem_rd(gb, DE);
  CYC(0x7e5c, 0x7e5d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7e5d, 0x7e5e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x7e5e, 0x7e5f); SET_DE(DE + 1);
  CYC(0x7e5f, 0x7e60); A = mem_rd(gb, DE);
  CYC(0x7e60, 0x7e61); alu_add(gb, mem_rd(gb, HL));
  CYC(0x7e61, 0x7e62); mem_wr(gb, HL, A);
  CYC(0x7e62, 0x7e63); ret_effect(gb);
}

void updatePirateShipAngle_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7eaa, 0x7ead); A = W8(wPirateShipChangedTile);
  CYC(0x7ead, 0x7eae); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7eae, 0x7eaf); ret_effect(gb);
    return;
  }
  CYC(0x7eae, 0x7eaf);
  CYC(0x7eaf, 0x7eb1); hram_wr(gb, 0x8b, A);
  CYC(0x7eb1, 0x7eb2); alu_xor(gb, A);
  CYC(0x7eb2, 0x7eb5); W8(wPirateShipChangedTile) = A;
  CYC(0x7eb5, 0x7eb8); SET_HL(0x7f02);
  CALL_C(0x7eb8, checkIsLinkedGame_hook, 0x1992, 0x7ebb);
  if (F & FZ) {
    CYCT(0x7ebb, 0x7ebd);
  } else {
    CYC(0x7ebb, 0x7ebd);
    CYC(0x7ebd, 0x7ec0); SET_HL(0x7edd);
  }
next_direction:
  CYC(0x7ec0, 0x7ec1); A = mem_rd(gb, HL);
  CYC(0x7ec1, 0x7ec2); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x7ec2, 0x7ec3); ret_effect(gb);
    return;
  }
  CYC(0x7ec2, 0x7ec3);
  CYC(0x7ec3, 0x7ec4); push_effect(gb, HL);
  CYC(0x7ec4, 0x7ec7); A = W8(wPirateShipRoom);
  CYC(0x7ec7, 0x7ec8); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x7ec8, 0x7eca);
    goto next_entry;
  }
  CYC(0x7ec8, 0x7eca);
  CYC(0x7eca, 0x7ecb); SET_HL(HL + 1);
  CYC(0x7ecb, 0x7ecd); A = hram_rd(gb, 0x8b);
  CYC(0x7ecd, 0x7ece); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x7ece, 0x7ed0);
    goto next_entry;
  }
  CYC(0x7ece, 0x7ed0);
  CYC(0x7ed0, 0x7ed1); SET_HL(HL + 1);
  CYC(0x7ed1, 0x7ed2); A = mem_rd(gb, HL);
  CYC(0x7ed2, 0x7ed5); W8(wPirateShipAngle) = A;
  CYC(0x7ed5, 0x7ed6); SET_HL(pop_effect(gb));
  CYC(0x7ed6, 0x7ed7); ret_effect(gb);
  return;
next_entry:
  CYC(0x7ed7, 0x7ed8); SET_HL(pop_effect(gb));
  CYC(0x7ed8, 0x7eda); A = 0x03;
  CYC(0x7eda, 0x7edb); pirateShip_addAToHl(gb, 0x7edb);
  CYC(0x7edb, 0x7edd);
  goto next_direction;
}
