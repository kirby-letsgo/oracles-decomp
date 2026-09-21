#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(partCode55);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80); // $80|ITEMCOLLISION_LINK
  if (!(F & FZ)) { CYCT(b_+7, b_+10); goto gotoState2; } // jp nz
  CYC(b_+7, b_+10);
  CALL_C(b_+10, checkLinkVulnerable_hook, SYM(checkLinkVulnerable), b_+13);
  if (!(F & FC)) { CYCT(b_+13, b_+15); goto normalStatus; } // jr nc
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+18); SET_HL(wLinkForceState);
  CYC(b_+18, b_+20); A = 0x14; // LINK_STATE_COLLAPSED
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x01); // wcc50
  CYC(b_+23, b_+24); H = D;
  CYC(b_+24, b_+26); L = 0xc4; // Part.state
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x03);
  CYC(b_+28, b_+30); L = 0xcf; // Part.zh
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x00);
  CYC(b_+32, b_+34); L = 0xe4; // Part.collisionType
  CYC(b_+34, b_+36); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(b_+36, objectSetVisible81_hook, SYM(objectSetVisible81), b_+39);

normalStatus:
  CYC(b_+39, b_+41); E = 0xc4; // Part.state
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  {
    CYC(b_+42, b_+43); push_effect(gb, b_+43);
    uint16_t target = octogonBubble_jump_table(gb);
    if (target == b_+65) goto state1;
    if (target == b_+96) goto state2;
    if (target == b_+107) goto state3;
    goto state0;
  }

state0:
  CYC(b_+51, b_+52); H = D;
  CYC(b_+52, b_+53); L = E;
  CYC(b_+53, b_+54); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+54, b_+56); L = 0xd0; // Part.speed
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+58, b_+60); L = 0xc6; // Part.counter1
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0xb4); // 180
  CYC(b_+62, b_+65); TAIL(objectSetVisible82); // jp

state1:
  CALL_C(b_+65, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+68);
  if (F & FZ) { CYCT(b_+68, b_+70); goto gotoState2; } // jr z
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+73); A = mem_rd(gb, wFrameCounter);
  CYC(b_+73, b_+75); alu_and(gb, 0x18);
  CYC(b_+75, b_+76); alu_rlca(gb);
  CYC(b_+76, b_+78); A = alu_swap(gb, A);
  CYC(b_+78, b_+81); SET_HL(b_+92); // table_7b7b (zPositions)
  CYC(b_+81, b_+82); octogonBubble_addAToHl_from_rst(gb, b_+82);
  CYC(b_+82, b_+84); E = 0xcf; // Part.zh
  CYC(b_+84, b_+85); A = mem_rd(gb, HL);
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CALL_C(b_+86, objectApplySpeed_hook, SYM(objectApplySpeed), b_+89);

animate:
  CYC(b_+89, b_+92); TAIL(partAnimate); // jp

state2:
  CALL_C(b_+96, partAnimate_hook, SYM(partAnimate), b_+99);
  CYC(b_+99, b_+101); E = 0xe1; // Part.animParameter
  CYC(b_+101, b_+102); A = mem_rd(gb, DE);
  CYC(b_+102, b_+103); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+103); return; } // ret nz
  CYC(b_+103, b_+104);
  CYC(b_+104, b_+107); TAIL(partDelete); // jp

state3:
  CYC(b_+107, b_+110); SET_HL(w1Link);
  CALL_C(b_+110, objectTakePosition_hook, SYM(objectTakePosition), b_+113);
  CYC(b_+113, b_+116); A = mem_rd(gb, wLinkForceState);
  CYC(b_+116, b_+118); alu_cp(gb, 0x14); // LINK_STATE_COLLAPSED
  if (F & FZ) { RET_TAKEN(b_+118); return; } // ret z
  CYC(b_+118, b_+119);
  CYC(b_+119, b_+121); L = 0x04; // <w1Link.state
  CYC(b_+121, b_+122); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+122, b_+124); alu_cp(gb, 0x14); // LINK_STATE_COLLAPSED
  if (F & FZ) { CYCT(b_+124, b_+126); goto animate; } // jr z
  CYC(b_+124, b_+126);

gotoState2:
  CYC(b_+126, b_+127); H = D;
  CYC(b_+127, b_+129); L = 0xc4; // Part.state
  CYC(b_+129, b_+131); mem_wr(gb, HL, 0x02);
  CYC(b_+131, b_+133); L = 0xe4; // Part.collisionType
  CYC(b_+133, b_+135); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+135, b_+137); A = 0x01;
  CYC(b_+137, b_+140); TAIL(partSetAnimation); // jp
}
