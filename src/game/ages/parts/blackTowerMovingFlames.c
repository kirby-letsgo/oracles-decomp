#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode59), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode59), (from), (to), true)

static uint16_t blackTowerMovingFlames_jump_table(GB *gb) {
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

static void blackTowerMovingFlames_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_BLACK_TOWER_MOVING_FLAMES
void partCode59_hook(GB *gb) {
  BASE(partCode59);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = blackTowerMovingFlames_jump_table(gb);
    if (target == b_+39) goto state1;
    if (target == b_+66) goto state2;
    if (target == b_+77) goto state3;
    if (target == b_+105) goto state4;
    if (target == b_+150) goto state5;
    goto state0;
  }

state0:
  CYC(b_+16, b_+17); H = D;
  CYC(b_+17, b_+18); L = E;
  CYC(b_+18, b_+19); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+19, b_+21); L = 0xd0; // Part.speed
  CYC(b_+21, b_+23); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+23, b_+25); L = 0xc2; // Part.subid
  CYC(b_+25, b_+26); A = mem_rd(gb, HL);
  CYC(b_+26, b_+29); SET_HL(b_+35); // table_7ebd
  CYC(b_+29, b_+30); blackTowerMovingFlames_addAToHl_from_rst(gb, b_+30);
  CYC(b_+30, b_+32); E = 0xc6; // Part.counter1
  CYC(b_+32, b_+33); A = mem_rd(gb, HL);
  CYC(b_+33, b_+34); mem_wr(gb, DE, A);
  RET(b_+34); return; // ret

state1:
  CALL_C(b_+39, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+42);
  if (!(F & FZ)) { RET_TAKEN(b_+42); return; } // ret nz
  CYC(b_+42, b_+43);
  CYC(b_+43, b_+44); L = E;
  CYC(b_+44, b_+45); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+45, b_+47); L = 0xc2; // Part.subid
  CYC(b_+47, b_+48); A = mem_rd(gb, HL);
  CYC(b_+48, b_+50); alu_xor(gb, 0x03);
  CYC(b_+50, b_+53); SET_HL(b_+35); // table_7ebd
  CYC(b_+53, b_+54); blackTowerMovingFlames_addAToHl_from_rst(gb, b_+54);
  CYC(b_+54, b_+56); E = 0xc6; // Part.counter1
  CYC(b_+56, b_+57); A = mem_rd(gb, HL);
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
  CYC(b_+58, b_+60); A = 0x72; // SND_LIGHTTORCH
  CALL_C(b_+60, playSound_b00_hook, SYM(playSound_b00), b_+63);
  CYC(b_+63, b_+66); objectSetVisible83_hook(gb); return; // jp

state2:
  CALL_C(b_+66, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+69);
  if (!(F & FZ)) { CYCT(b_+69, b_+71); goto animate; } // jr nz
  CYC(b_+69, b_+71);
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x14);
  CYC(b_+73, b_+74); L = E;
  CYC(b_+74, b_+75); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+75, b_+77); goto animate; // jr

state3:
  CALL_C(b_+77, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+80);
  if (!(F & FZ)) { CYCT(b_+80, b_+82); goto animate; } // jr nz
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+85); SET_HL((SYM(partCode3a__func_6dba) + 2)); // blackTower_getMovingFlamesNextTileCoords
  CYC(b_+85, b_+87); E = 0x10; // bank 0x10
  CALL_C(b_+87, interBankCall_hook, 0x008a, b_+90);
  CYC(b_+90, b_+91); H = D;
  CYC(b_+91, b_+93); L = 0xc4; // Part.state
  CYC(b_+93, b_+94); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+94, b_+95); A = B;
  CYC(b_+95, b_+96); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+96, b_+98); goto animate; } // jr nz
  CYC(b_+96, b_+98);
  CYC(b_+98, b_+99); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+99, b_+101); L = 0xc6; // Part.counter1
  CYC(b_+101, b_+103); mem_wr(gb, HL, 0x10);
  CYC(b_+103, b_+105); goto animate; // jr

state4:
  CYC(b_+105, b_+106); H = D;
  CYC(b_+106, b_+108); L = 0xf0; // Part.var30
  CYC(b_+108, b_+109); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+109, b_+110); B = A;
  CYC(b_+110, b_+111); C = mem_rd(gb, HL);
  CYC(b_+111, b_+113); L = 0xcb; // Part.yh
  CYC(b_+113, b_+114); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+114, b_+116); mem_wr(gb, hFF8F, A);
  CYC(b_+116, b_+117); L = alu_inc8(gb, L);
  CYC(b_+117, b_+118); A = mem_rd(gb, HL);
  CYC(b_+118, b_+120); mem_wr(gb, hFF8E, A);
  CYC(b_+120, b_+121); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+121, b_+123); goto moveToBC; } // jr nz
  CYC(b_+121, b_+123);
  CYC(b_+123, b_+125); A = mem_rd(gb, hFF8F);
  CYC(b_+125, b_+126); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+126, b_+128); goto moveToBC; } // jr nz
  CYC(b_+126, b_+128);
  CYC(b_+128, b_+129); L = E;
  CYC(b_+129, b_+130); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+130, b_+132); L = 0xc6; // Part.counter1
  CYC(b_+132, b_+134); mem_wr(gb, HL, 0x10);
  CYC(b_+134, b_+135); L = alu_inc8(gb, L);
  CYC(b_+135, b_+136); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+136, b_+138); goto animate; // jr

moveToBC:
  CALL_C(b_+138, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+141);
  CYC(b_+141, b_+143); E = 0xc9; // Part.angle
  CYC(b_+143, b_+144); mem_wr(gb, DE, A);
  CALL_C(b_+144, objectApplySpeed_hook, SYM(objectApplySpeed), b_+147);

animate:
  CYC(b_+147, b_+150); partAnimate_hook(gb); return; // jp

state5:
  CALL_C(b_+150, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+153);
  if (!(F & FZ)) { CYCT(b_+153, b_+155); goto animate; } // jr nz
  CYC(b_+153, b_+155);
  CALL_C(b_+155, objectCreatePuff_hook, SYM(objectCreatePuff), b_+158);
  CYC(b_+158, SYM(partCode5a)); partDelete_hook(gb); return; // jp
}
