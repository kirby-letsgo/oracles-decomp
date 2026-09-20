#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode57), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode57), (from), (to), true)

static uint16_t veranAcidPool_jump_table(GB *gb) {
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

// PART_VERAN_ACID_POOL
void partCode57_hook(GB *gb) {
  BASE(partCode57);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = veranAcidPool_jump_table(gb);
    if (target == b_+48) goto state1;
    if (target == b_+65) goto state2;
    if (target == b_+91) goto state3;
    if (target == b_+99) goto state4;
    if (target == b_+106) goto state5;
    if (target == b_+164) goto state6;
    goto state0;
  }

state0:
  CALL_C(b_+18, objectCenterOnTile_hook, SYM(objectCenterOnTile), b_+21);
  CALL_C(b_+21, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+24);
  CYC(b_+24, b_+26); E = 0xf0; // Part.var30
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+29); E = 0xc6; // Part.counter1
  CYC(b_+29, b_+31); A = 0x04;
  CYC(b_+31, b_+32); mem_wr(gb, DE, A);
  CYC(b_+32, b_+34); A = 0x68; // SND_UNKNOWN3
  CALL_C(b_+34, playSound_b00_hook, SYM(playSound_b00), b_+37);
  CYC(b_+37, b_+40); SET_HL(b_+44); // table_7d98
  CYC(b_+40, b_+42); A = 0x60;
  CYC(b_+42, b_+44); goto func_7de1; // jr

state1:
  CALL_C(b_+48, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+51);
  if (!(F & FZ)) { RET_TAKEN(b_+51); return; } // ret nz
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+54); mem_wr(gb, HL, 0x04);
  CYC(b_+54, b_+57); SET_HL(b_+61); // table_7da9
  CYC(b_+57, b_+59); A = 0x60;
  CYC(b_+59, b_+61); goto func_7de1; // jr

state2:
  CALL_C(b_+65, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+68);
  if (!(F & FZ)) { RET_TAKEN(b_+68); return; } // ret nz
  CYC(b_+68, b_+69);
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x2d);
  CYC(b_+71, b_+72); L = E;
  CYC(b_+72, b_+73); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+73, b_+75); L = 0x60;

func_7db7:
  CYC(b_+75, b_+77); E = 0xf0; // Part.var30
  CYC(b_+77, b_+78); A = mem_rd(gb, DE);
  CYC(b_+78, b_+79); C = A;
  CYC(b_+79, b_+81); B = 0xcf;
  CYC(b_+81, b_+82); A = mem_rd(gb, BC);
  CYC(b_+82, b_+84); alu_sub(gb, 0x02);
  CYC(b_+84, b_+86); alu_cp(gb, 0x03);
  if (F & FC) { // ret c
    RET_TAKEN(b_+86);
    if (gb->pc == b_+173 && gb->sp == sp0_) goto state6_afterFunc7db7;
    return;
  }
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+88); A = L;
  CYC(b_+88, b_+91); setTile_hook(gb); // jp (own burn; local, multi-entry tail-chain)
  if (gb->pc == b_+173 && gb->sp == sp0_) goto state6_afterFunc7db7;
  return;

state3:
  CALL_C(b_+91, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+94);
  if (!(F & FZ)) { RET_TAKEN(b_+94); return; } // ret nz
  CYC(b_+94, b_+95);
  CYC(b_+95, b_+97); mem_wr(gb, HL, 0x04);
  CYC(b_+97, b_+98); L = E;
  CYC(b_+98, b_+99); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

state4:
  CYC(b_+99, b_+102); SET_HL(b_+61); // table_7da9
  CYC(b_+102, b_+104); A = 0xa0;
  CYC(b_+104, b_+106); goto func_7de1; // jr

state5:
  CALL_C(b_+106, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+109);
  if (!(F & FZ)) { RET_TAKEN(b_+109); return; } // ret nz
  CYC(b_+109, b_+110);
  CYC(b_+110, b_+112); mem_wr(gb, HL, 0x04);
  CYC(b_+112, b_+115); SET_HL(b_+44); // table_7d98
  CYC(b_+115, b_+117); A = 0xa0;

func_7de1:
  CYC(b_+117, b_+119); mem_wr(gb, hFF8B, A); // hFF8B
  CYC(b_+119, b_+121); E = 0xf0; // Part.var30
  CYC(b_+121, b_+122); A = mem_rd(gb, DE);
  CYC(b_+122, b_+123); C = A;
  CYC(b_+123, b_+125); B = 0x04;

L_7de9:
  CYC(b_+125, b_+126); push_effect(gb, BC);
  CYC(b_+126, b_+127); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+127, b_+128); alu_add(gb, C);
  CYC(b_+128, b_+129); C = A;
  CYC(b_+129, b_+131); B = 0xcf;
  CYC(b_+131, b_+132); A = mem_rd(gb, BC);
  CYC(b_+132, b_+134); alu_cp(gb, 0xda);
  if (F & FZ) { CYCT(b_+134, b_+136); goto L_7e00; } // jr z
  CYC(b_+134, b_+136);
  CYC(b_+136, b_+138); alu_sub(gb, 0x02);
  CYC(b_+138, b_+140); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+140, b_+142); goto L_7e07; } // jr c
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+144); B = 0xce;
  CYC(b_+144, b_+145); A = mem_rd(gb, BC);
  CYC(b_+145, b_+146); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+146, b_+148); goto L_7e07; } // jr nz
  CYC(b_+146, b_+148);

L_7e00:
  CYC(b_+148, b_+150); A = mem_rd(gb, hFF8B);
  CYC(b_+150, b_+151); push_effect(gb, HL);
  CALL_C(b_+151, setTile_hook, SYM(setTile), b_+154);
  CYC(b_+154, b_+155); SET_HL(pop_effect(gb));

L_7e07:
  CYC(b_+155, b_+156); SET_BC(pop_effect(gb));
  CYC(b_+156, b_+157); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+157, b_+159); goto L_7de9; } // jr nz
  CYC(b_+157, b_+159);
  CYC(b_+159, b_+160); H = D;
  CYC(b_+160, b_+162); L = 0xc4; // Part.state
  CYC(b_+162, b_+163); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+163); return; // ret

state6:
  CALL_C(b_+164, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+167);
  if (!(F & FZ)) { RET_TAKEN(b_+167); return; } // ret nz
  CYC(b_+167, b_+168);
  CYC(b_+168, b_+170); L = 0xa0;
  CYC(b_+170, b_+173); push_effect(gb, b_+173); goto func_7db7; // call $7db7 (own burn; local, inlined)

state6_afterFunc7db7:
  CYC(b_+173, SYM(partCode58)); partDelete_hook(gb); return; // jp
}
