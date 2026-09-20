#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode13), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode13), (from), (to), true)

static uint16_t owlStatue_jump_table(GB *gb) {
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

static void owlStatue_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode13_hook(GB *gb) {
  BASE(partCode13);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x9a);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto normalStatus; } // jr nz
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = 0xc4; // Part.state
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+15, b_+17); goto normalStatus; } // jr nc
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+18); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+18, b_+20); L = 0xc6; // Part.counter1
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x32);

normalStatus:
  CYC(b_+22, b_+24); E = 0xc4; // Part.state
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  {
    CYC(b_+25, b_+26); push_effect(gb, b_+26);
    uint16_t target = owlStatue_jump_table(gb);
    if (target == b_+34) goto state0;
    if (target == b_+51) goto stateStub;
    if (target == b_+52) goto state2;
    goto state3;
  }

state0:
  CYC(b_+34, b_+35); H = D;
  CYC(b_+35, b_+36); L = E;
  CYC(b_+36, b_+37); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+37, b_+39); L = 0xff; // Part.var3f
  CYC(b_+39, b_+41); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 5)));
  CALL_C(b_+41, objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+44);
  CYC(b_+44, b_+46); H = 0xcf; // >wRoomLayout
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x00);
  CYC(b_+48, b_+51); objectSetVisible83_hook(gb); return; // jp

stateStub:
  RET(b_+51); return; // ret

state2:
  CALL_C(b_+52, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+55);
  if (!(F & FZ)) { CYCT(b_+55, b_+57); goto L_4cd6; } // jr nz
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x1e);
  CYC(b_+59, b_+60); L = E;
  CYC(b_+60, b_+61); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+61, b_+63); A = 0x01;
  CYC(b_+63, b_+66); partSetAnimation_hook(gb); return; // jp

L_4cd6:
  CYC(b_+66, b_+67); A = mem_rd(gb, HL);
  CYC(b_+67, b_+69); alu_and(gb, 0x07);
  if (!(F & FZ)) { RET_TAKEN(b_+69); return; } // ret nz
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+71); A = mem_rd(gb, HL);
  CYC(b_+71, b_+72); alu_rrca(gb);
  CYC(b_+72, b_+73); alu_rrca(gb);
  CYC(b_+73, b_+75); alu_sub(gb, 0x02);
  CYC(b_+75, b_+78); SET_HL(b_+91); // @owlStatueSparkleOffset
  CYC(b_+78, b_+79); owlStatue_addAToHl_from_rst(gb, b_+79);
  CYC(b_+79, b_+80); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+80, b_+81); B = A;
  CYC(b_+81, b_+82); C = mem_rd(gb, HL);
  CALL_C(b_+82, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+85);
  if (!(F & FZ)) { RET_TAKEN(b_+85); return; } // ret nz
  CYC(b_+85, b_+86);
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x84); // INTERAC_SPARKLE
  CYC(b_+88, b_+91); objectCopyPositionWithOffset_hook(gb); return; // jp

state3:
  CALL_C(b_+103, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+106);
  if (!(F & FZ)) { CYCT(b_+106, b_+108); goto L_4d07; } // jr nz
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+109); L = E;
  CYC(b_+109, b_+111); mem_wr(gb, HL, 0x01);
  CYC(b_+111, b_+112); alu_xor(gb, A);
  CYC(b_+112, b_+115); partSetAnimation_hook(gb); return; // jp

L_4d07:
  CYC(b_+115, b_+116); A = mem_rd(gb, HL);
  CYC(b_+116, b_+118); alu_cp(gb, 0x16);
  if (!(F & FZ)) { RET_TAKEN(b_+118); return; } // ret nz
  CYC(b_+118, b_+119);
  CYC(b_+119, b_+121); L = 0xc2; // Part.subid
  CYC(b_+121, b_+122); C = mem_rd(gb, HL);
  CYC(b_+122, b_+124); B = 0x39;
  CYC(b_+124, SYM(partCode14)); showText_hook(gb); return; // jp
}
