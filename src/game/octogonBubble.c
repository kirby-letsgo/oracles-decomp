#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t octogonBubble_jump_table(GB *gb) {
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

static void octogonBubble_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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
  ret_effect(gb);
}

// PART_OCTOGON_BUBBLE
void partCode55_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x7b1f, 0x7b21); goto normalStatus; } // jr z
  CYC(0x7b1f, 0x7b21);
  CYC(0x7b21, 0x7b23); E = 0xea; // Part.var2a
  CYC(0x7b23, 0x7b24); A = mem_rd(gb, DE);
  CYC(0x7b24, 0x7b26); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { CYCT(0x7b26, 0x7b29); goto gotoState2; } // jp nz
  CYC(0x7b26, 0x7b29);
  CALL_C(0x7b29, checkLinkVulnerable_hook, 0x1d28, 0x7b2c);
  if (!(F & FC)) { CYCT(0x7b2c, 0x7b2e); goto normalStatus; } // jr nc
  CYC(0x7b2c, 0x7b2e);
  CYC(0x7b2e, 0x7b31); SET_HL(wLinkForceState);
  CYC(0x7b31, 0x7b33); A = 0x14; // LINK_STATE_COLLAPSED
  CYC(0x7b33, 0x7b34); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x7b34, 0x7b36); mem_wr(gb, HL, 0x01); // wcc50
  CYC(0x7b36, 0x7b37); H = D;
  CYC(0x7b37, 0x7b39); L = 0xc4; // Part.state
  CYC(0x7b39, 0x7b3b); mem_wr(gb, HL, 0x03);
  CYC(0x7b3b, 0x7b3d); L = 0xcf; // Part.zh
  CYC(0x7b3d, 0x7b3f); mem_wr(gb, HL, 0x00);
  CYC(0x7b3f, 0x7b41); L = 0xe4; // Part.collisionType
  CYC(0x7b41, 0x7b43); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(0x7b43, objectSetVisible81_hook, 0x1e60, 0x7b46);

normalStatus:
  CYC(0x7b46, 0x7b48); E = 0xc4; // Part.state
  CYC(0x7b48, 0x7b49); A = mem_rd(gb, DE);
  {
    CYC(0x7b49, 0x7b4a); push_effect(gb, 0x7b4a);
    uint16_t target = octogonBubble_jump_table(gb);
    if (target == 0x7b60) goto state1;
    if (target == 0x7b7f) goto state2;
    if (target == 0x7b8a) goto state3;
    goto state0;
  }

state0:
  CYC(0x7b52, 0x7b53); H = D;
  CYC(0x7b53, 0x7b54); L = E;
  CYC(0x7b54, 0x7b55); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x7b55, 0x7b57); L = 0xd0; // Part.speed
  CYC(0x7b57, 0x7b59); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x7b59, 0x7b5b); L = 0xc6; // Part.counter1
  CYC(0x7b5b, 0x7b5d); mem_wr(gb, HL, 0xb4); // 180
  CYC(0x7b5d, 0x7b60); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(0x7b60, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7b63);
  if (F & FZ) { CYCT(0x7b63, 0x7b65); goto gotoState2; } // jr z
  CYC(0x7b63, 0x7b65);
  CYC(0x7b65, 0x7b68); A = mem_rd(gb, wFrameCounter);
  CYC(0x7b68, 0x7b6a); alu_and(gb, 0x18);
  CYC(0x7b6a, 0x7b6b); alu_rlca(gb);
  CYC(0x7b6b, 0x7b6d); A = alu_swap(gb, A);
  CYC(0x7b6d, 0x7b70); SET_HL(0x7b7b); // table_7b7b (zPositions)
  CYC(0x7b70, 0x7b71); octogonBubble_addAToHl_from_rst(gb, 0x7b71);
  CYC(0x7b71, 0x7b73); E = 0xcf; // Part.zh
  CYC(0x7b73, 0x7b74); A = mem_rd(gb, HL);
  CYC(0x7b74, 0x7b75); mem_wr(gb, DE, A);
  CALL_C(0x7b75, objectApplySpeed_hook, 0x201d, 0x7b78);

animate:
  CYC(0x7b78, 0x7b7b); partAnimate_hook(gb); return; // jp

state2:
  CALL_C(0x7b7f, partAnimate_hook, 0x2978, 0x7b82);
  CYC(0x7b82, 0x7b84); E = 0xe1; // Part.animParameter
  CYC(0x7b84, 0x7b85); A = mem_rd(gb, DE);
  CYC(0x7b85, 0x7b86); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x7b86); return; } // ret nz
  CYC(0x7b86, 0x7b87);
  CYC(0x7b87, 0x7b8a); partDelete_hook(gb); return; // jp

state3:
  CYC(0x7b8a, 0x7b8d); SET_HL(w1Link);
  CALL_C(0x7b8d, objectTakePosition_hook, 0x2274, 0x7b90);
  CYC(0x7b90, 0x7b93); A = mem_rd(gb, wLinkForceState);
  CYC(0x7b93, 0x7b95); alu_cp(gb, 0x14); // LINK_STATE_COLLAPSED
  if (F & FZ) { RET_TAKEN(0x7b95); return; } // ret z
  CYC(0x7b95, 0x7b96);
  CYC(0x7b96, 0x7b98); L = 0x04; // <w1Link.state
  CYC(0x7b98, 0x7b99); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x7b99, 0x7b9b); alu_cp(gb, 0x14); // LINK_STATE_COLLAPSED
  if (F & FZ) { CYCT(0x7b9b, 0x7b9d); goto animate; } // jr z
  CYC(0x7b9b, 0x7b9d);

gotoState2:
  CYC(0x7b9d, 0x7b9e); H = D;
  CYC(0x7b9e, 0x7ba0); L = 0xc4; // Part.state
  CYC(0x7ba0, 0x7ba2); mem_wr(gb, HL, 0x02);
  CYC(0x7ba2, 0x7ba4); L = 0xe4; // Part.collisionType
  CYC(0x7ba4, 0x7ba6); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(0x7ba6, 0x7ba8); A = 0x01;
  CYC(0x7ba8, 0x7bab); partSetAnimation_hook(gb); return; // jp
}
