#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static uint16_t harp_parent_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void harp_parent_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
    ret_effect(gb);
  }
}

void parentItemCode_flute_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4d73, 0x4d75); E = 0x04;
  CYC(0x4d75, 0x4d76); A = mem_rd(gb, DE);
  CYC(0x4d76, 0x4d77); push_effect(gb, 0x4d77);
  switch (harp_parent_jump_table(gb)) {
    case 0x4d7b: goto state0;
    case 0x4db4: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x4d7b, checkLinkOnGround_hook, 0x54c4, 0x4d7e);
  if (!(F & FZ)) { CYCT(0x4d7e, 0x4d81); goto clear_self; }
  CYC(0x4d7e, 0x4d81);
  CYC(0x4d81, 0x4d84); A = W8(wInstrumentsDisabledCounter);
  CYC(0x4d84, 0x4d85); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4d85, 0x4d88); goto clear_self; }
  CYC(0x4d85, 0x4d88);
  CALL_C(0x4d88, isLinkInHole_hook, 0x54d8, 0x4d8b);
  if (F & FC) { CYCT(0x4d8b, 0x4d8e); goto clear_self; }
  CYC(0x4d8b, 0x4d8e);
  CALL_C(0x4d8e, checkNoOtherParentItemsInUse_hook, 0x5366, 0x4d91);
  if (!(F & FZ)) { CYCT(0x4d91, 0x4d94); goto clear_self; }
  CYC(0x4d91, 0x4d94);
  CYC(0x4d94, 0x4d96); A = 0x80;
  CYC(0x4d96, 0x4d99); W8(wcc95) = A;
  CYC(0x4d99, 0x4d9b); A = 0x7e;
  CYC(0x4d9b, 0x4d9e); W8(wDisabledObjects) = A;
  CALL_C(0x4d9e, parentItemLoadAnimationAndIncState_hook, 0x5378, 0x4da1);
  CYC(0x4da1, 0x4da3); B = 0x00;
  CALL_ROM(0x4da3, 0x4e5a);
  if (F & FZ) CYCT(0x4da6, 0x4da8);
  else { CYC(0x4da6, 0x4da8); CYC(0x4da8, 0x4daa); B = 0x03; }
  CYC(0x4daa, 0x4dab); A = mem_rd(gb, HL);
  CYC(0x4dab, 0x4dac); alu_add(gb, B);
  CYC(0x4dac, 0x4daf); SET_HL(0x4e53);
  CYC(0x4daf, 0x4db0); push_effect(gb, 0x4db0); harp_parent_add_a_to_hl(gb);
  CYC(0x4db0, 0x4db1); A = mem_rd(gb, HL);
  CALL_C(0x4db1, playSound_b00_hook, 0x0c98, 0x4db4);

state1:
  CYC(0x4db4, 0x4db7); SET_HL(w1Link_collisionType);
  CYC(0x4db7, 0x4db9); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  CALL_C(0x4db9, itemDecCounter1_hook, 0x23d6, 0x4dbc);
  CYC(0x4dbc, 0x4dbd); A = mem_rd(gb, HL);
  CYC(0x4dbd, 0x4dbf); alu_and(gb, 0x1f);
  if (!(F & FZ)) CYCT(0x4dbf, 0x4dc1);
  else {
    CYC(0x4dbf, 0x4dc1);
    CYC(0x4dc1, 0x4dc3); L = 0x21;
    CYC(0x4dc3, 0x4dc5); alu_bit(gb, 0, mem_rd(gb, HL));
    CYC(0x4dc5, 0x4dc8); SET_BC(0xfcf8);
    if (F & FZ) CYCT(0x4dc8, 0x4dca);
    else { CYC(0x4dc8, 0x4dca); CYC(0x4dca, 0x4dcc); C = 0x08; }
    CALL_C(0x4dcc, getRandomNumber_hook, 0x043e, 0x4dcf);
    CYC(0x4dcf, 0x4dd1); alu_and(gb, 0x01);
    CYC(0x4dd1, 0x4dd2); push_effect(gb, DE);
    CYC(0x4dd2, 0x4dd4); D = 0xd0;
    CALL_C(0x4dd4, objectCreateFloatingMusicNote_hook, 0x27fe, 0x4dd7);
    CYC(0x4dd7, 0x4dd8); SET_DE(pop_effect(gb));
  }
  CALL_C(0x4dd8, specialObjectAnimate_optimized_hook, 0x441e, 0x4ddb);
  CALL_ROM(0x4ddb, 0x4e5a);
  CYC(0x4dde, 0x4de0); A = 0xff;
  if (F & FZ) CYCT(0x4de0, 0x4de2);
  else { CYC(0x4de0, 0x4de2); CYC(0x4de2, 0x4de3); A = mem_rd(gb, HL); }
  CYC(0x4de3, 0x4de6); W8(wLinkPlayingInstrument) = A;
  CYC(0x4de6, 0x4de9); W8(wLinkRidingObject) = A;
  CYC(0x4de9, 0x4deb); C = 0x80;
  if (!(F & FZ)) CYCT(0x4deb, 0x4ded);
  else {
    CYC(0x4deb, 0x4ded);
    CYC(0x4ded, 0x4dee); A = mem_rd(gb, HL);
    CYC(0x4dee, 0x4def); alu_or(gb, A);
    if (!(F & FZ)) CYCT(0x4def, 0x4df1);
    else { CYC(0x4def, 0x4df1); CYC(0x4df1, 0x4df3); C = 0x40; }
  }
  CYC(0x4df3, 0x4df5); E = 0x21;
  CYC(0x4df5, 0x4df6); A = mem_rd(gb, DE);
  CYC(0x4df6, 0x4df7); alu_and(gb, C);
  if (F & FZ) { CYCT(0x4df7, 0x4df8); ret_effect(gb); return; }
  CYC(0x4df7, 0x4df8);
  CYC(0x4df8, 0x4dfb); SET_HL(w1Link_collisionType);
  CYC(0x4dfb, 0x4dfd); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_ROM(0x4dfd, 0x4e5a);
  if (!(F & FZ)) { CYCT(0x4e00, 0x4e02); goto harp; }
  CYC(0x4e00, 0x4e02);
  CYC(0x4e02, 0x4e05); SET_BC(0x6780);
  CALL_C(0x4e05, objectCreateInteraction_hook, 0x24c5, 0x4e08);

clear_self:
  CYC(0x4e08, 0x4e09); alu_xor(gb, A);
  CYC(0x4e09, 0x4e0c); W8(wDisabledObjects) = A;
  CYC(0x4e0c, 0x4e0f); W8(wcc95) = A;
  CYC(0x4e0f, 0x4e12); clearParentItem_hook(gb); return;

echoes_in_vain:
  CYC(0x4e12, 0x4e15); SET_BC(0x5110);
  CALL_C(0x4e15, showText_hook, 0x1872, 0x4e18);
  CYC(0x4e18, 0x4e1a); goto clear_self;

harp:
  CYC(0x4e1a, 0x4e1d); A = W8(wTilesetFlags);
  CYC(0x4e1d, 0x4e1f); alu_and(gb, 0x7e);
  if (!(F & FZ)) { CYCT(0x4e1f, 0x4e21); goto clear_self; }
  CYC(0x4e1f, 0x4e21);
  CYC(0x4e21, 0x4e22); A = mem_rd(gb, HL);
  CYC(0x4e22, 0x4e23); push_effect(gb, 0x4e23);
  switch (harp_parent_jump_table(gb)) {
    case 0x4e08: goto clear_self;
    case 0x4e2b: goto tune_echoes;
    case 0x4e34: goto tune_currents;
    case 0x4e3a: goto tune_ages;
    default: hook_continue(gb, HL, sp0_); return;
  }

tune_echoes:
  CALL_C(0x4e2b, getThisRoomFlags_hook, 0x197d, 0x4e2e);
  CYC(0x4e2e, 0x4e30); alu_bit(gb, 3, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4e30, 0x4e32); goto clear_self; }
  CYC(0x4e30, 0x4e32);
  CYC(0x4e32, 0x4e34); goto echoes_in_vain;

tune_currents:
  CYC(0x4e34, 0x4e37); A = W8(wTilesetFlags);
  CYC(0x4e37, 0x4e38); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x4e38, 0x4e3a); goto echoes_in_vain; }
  CYC(0x4e38, 0x4e3a);

tune_ages:
  CALL_C(0x4e3a, restartSound_hook, 0x0cb2, 0x4e3d);
  CYC(0x4e3d, 0x4e3f); A = 0x1b;
  CYC(0x4e3f, 0x4e42); W8(wCutsceneTrigger) = A;
  CYC(0x4e42, 0x4e44); A = 0x6d;
  CYC(0x4e44, 0x4e47); W8(wDisabledObjects) = A;
  CYC(0x4e47, 0x4e4a); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(0x4e4a, 0x4e4d); W8(wcde0) = A;
  CALL_C(0x4e4d, clearAllItemsAndPutLinkOnGround_hook, 0x19ad, 0x4e50);
  CYC(0x4e50, 0x4e53); specialObjectAnimate_optimized_hook(gb);
}
