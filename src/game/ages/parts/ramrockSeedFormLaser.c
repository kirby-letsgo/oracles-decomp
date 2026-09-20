#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode34), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode34), (from), (to), true)

static uint16_t ramrockSeedFormLaser_jump_table(GB *gb) {
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

// PART_RAMROCK_SEED_FORM_LASER
void partCode34_hook(GB *gb) {
  BASE(partCode34);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+5, b_+7); goto L_65f9; } // jr nc
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+9); A = 0x0d; // Object.xh
  CALL_C(b_+9, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, HL);
  CYC(b_+13, b_+15); E = 0xcd; // Part.xh
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);

L_65f9:
  CYC(b_+16, b_+18); E = 0xc7; // Part.counter2
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); A = alu_dec8(gb, A);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+23); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+23, b_+25); goto L_6608; } // jr nz
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+27); E = 0xdc; // Part.oamFlags
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+30); alu_xor(gb, 0x01);
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);

L_6608:
  CYC(b_+31, b_+33); E = 0xc4; // Part.state
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  {
    CYC(b_+34, b_+35); push_effect(gb, b_+35);
    uint16_t target = ramrockSeedFormLaser_jump_table(gb);
    if (target == b_+67) goto state1;
    if (target == b_+115) goto state2;
    if (target == b_+134) goto state3;
    if (target == SYM(partDelete)) { partDelete_hook(gb); return; }
    goto state0;
  }

state0:
  CYC(b_+45, b_+47); A = 0x01;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+49); H = D;
  CYC(b_+49, b_+51); L = 0xd0; // Part.speed
  CYC(b_+51, b_+53); mem_wr(gb, HL, 0x6e); // SPEED_2c0
  CYC(b_+53, b_+55); L = 0xc9; // Part.angle
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+57, b_+59); L = 0xc6; // Part.counter1
  CYC(b_+59, b_+61); A = 0x07;
  CYC(b_+61, b_+62); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x03); // Part.counter2
  CALL_C(b_+64, objectSetVisible80_hook, SYM(objectSetVisible80), b_+67);

state1:
  CYC(b_+67, b_+69); E = 0xc3; // Part.var03
  CYC(b_+69, b_+70); A = mem_rd(gb, DE);
  CYC(b_+70, b_+71); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+71, b_+73); goto L_6638; } // jr z
  CYC(b_+71, b_+73);
  CALL_C(b_+73, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+76);
  if (!(F & FZ)) { CYCT(b_+76, b_+79); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(b_+76, b_+79);

L_6638:
  CYC(b_+79, b_+81); E = 0xc3; // Part.var03
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+84); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+84, b_+86); goto L_6657; } // jr z
  CYC(b_+84, b_+86);
  CALL_C(b_+86, getFreePartSlot_hook, SYM(getFreePartSlot), b_+89);
  if (!(F & FZ)) { RET_TAKEN(b_+89); return; } // ret nz
  CYC(b_+89, b_+90);
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x34); // PART_RAMROCK_SEED_FORM_LASER
  CYC(b_+92, b_+93); L = alu_inc8(gb, L);
  CYC(b_+93, b_+95); mem_wr(gb, HL, 0x0e);
  CYC(b_+95, b_+96); L = E;
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+98); A = alu_inc8(gb, A);
  CYC(b_+98, b_+99); mem_wr(gb, HL, A);
  CYC(b_+99, b_+101); E = 0xd6; // Part.relatedObj1
  CYC(b_+101, b_+102); L = E;
  CYC(b_+102, b_+104); A = 0xc0; // Part
  CYC(b_+104, b_+105); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+105, b_+106); A = D;
  CYC(b_+106, b_+107); mem_wr(gb, HL, A);
  CALL_C(b_+107, objectCopyPosition_hook, SYM(objectCopyPosition), b_+110);

L_6657:
  CYC(b_+110, b_+112); E = 0xc4; // Part.state
  CYC(b_+112, b_+114); A = 0x02;
  CYC(b_+114, b_+115); mem_wr(gb, DE, A);

state2:
  CYC(b_+115, b_+117); A = 0x02; // Object.subid
  CALL_C(b_+117, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+120);
  CYC(b_+120, b_+121); A = mem_rd(gb, HL);
  CYC(b_+121, b_+123); alu_cp(gb, 0x0e);
  if (F & FZ) { RET_TAKEN(b_+123); return; } // ret z
  CYC(b_+123, b_+124);
  CYC(b_+124, b_+126); E = 0xc4; // Part.state
  CYC(b_+126, b_+128); A = 0x03;
  CYC(b_+128, b_+129); mem_wr(gb, DE, A);
  CYC(b_+129, b_+131); E = 0xc6; // Part.counter1
  CYC(b_+131, b_+133); A = 0x07;
  CYC(b_+133, b_+134); mem_wr(gb, DE, A);

state3:
  CYC(b_+134, b_+136); E = 0xc3; // Part.var03
  CYC(b_+136, b_+137); A = mem_rd(gb, DE);
  CYC(b_+137, b_+139); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+139, b_+142); partDelete_hook(gb); return; } // jp z
  CYC(b_+139, b_+142);
  CALL_C(b_+142, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+145);
  if (!(F & FZ)) { CYCT(b_+145, b_+148); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(b_+145, b_+148);
  CYC(b_+148, b_+150); E = 0xc2; // Part.subid
  CYC(b_+150, b_+151); mem_wr(gb, DE, A);
  CYC(b_+151, b_+153); E = 0xc4; // Part.state
  CYC(b_+153, b_+155); A = 0x04;
  CYC(b_+155, b_+156); mem_wr(gb, DE, A);
  RET(b_+156); return; // ret
}
