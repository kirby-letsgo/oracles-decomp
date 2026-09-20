#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x65e9, 0x65eb); E = 0xc4; // Part.state
  CYC(0x65eb, 0x65ec); A = mem_rd(gb, DE);
  CYC(0x65ec, 0x65ee); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x65ee, 0x65f0); goto L_65f9; } // jr nc
  CYC(0x65ee, 0x65f0);
  CYC(0x65f0, 0x65f2); A = 0x0d; // Object.xh
  CALL_C(0x65f2, objectGetRelatedObject1Var_hook, 0x2160, 0x65f5);
  CYC(0x65f5, 0x65f6); A = mem_rd(gb, HL);
  CYC(0x65f6, 0x65f8); E = 0xcd; // Part.xh
  CYC(0x65f8, 0x65f9); mem_wr(gb, DE, A);

L_65f9:
  CYC(0x65f9, 0x65fb); E = 0xc7; // Part.counter2
  CYC(0x65fb, 0x65fc); A = mem_rd(gb, DE);
  CYC(0x65fc, 0x65fd); A = alu_dec8(gb, A);
  CYC(0x65fd, 0x65fe); mem_wr(gb, DE, A);
  CYC(0x65fe, 0x6600); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x6600, 0x6602); goto L_6608; } // jr nz
  CYC(0x6600, 0x6602);
  CYC(0x6602, 0x6604); E = 0xdc; // Part.oamFlags
  CYC(0x6604, 0x6605); A = mem_rd(gb, DE);
  CYC(0x6605, 0x6607); alu_xor(gb, 0x01);
  CYC(0x6607, 0x6608); mem_wr(gb, DE, A);

L_6608:
  CYC(0x6608, 0x660a); E = 0xc4; // Part.state
  CYC(0x660a, 0x660b); A = mem_rd(gb, DE);
  {
    CYC(0x660b, 0x660c); push_effect(gb, 0x660c);
    uint16_t target = ramrockSeedFormLaser_jump_table(gb);
    if (target == 0x662c) goto state1;
    if (target == 0x665c) goto state2;
    if (target == 0x666f) goto state3;
    if (target == 0x3ea1) { partDelete_hook(gb); return; }
    goto state0;
  }

state0:
  CYC(0x6616, 0x6618); A = 0x01;
  CYC(0x6618, 0x6619); mem_wr(gb, DE, A);
  CYC(0x6619, 0x661a); H = D;
  CYC(0x661a, 0x661c); L = 0xd0; // Part.speed
  CYC(0x661c, 0x661e); mem_wr(gb, HL, 0x6e); // SPEED_2c0
  CYC(0x661e, 0x6620); L = 0xc9; // Part.angle
  CYC(0x6620, 0x6622); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(0x6622, 0x6624); L = 0xc6; // Part.counter1
  CYC(0x6624, 0x6626); A = 0x07;
  CYC(0x6626, 0x6627); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6627, 0x6629); mem_wr(gb, HL, 0x03); // Part.counter2
  CALL_C(0x6629, objectSetVisible80_hook, 0x1e57, 0x662c);

state1:
  CYC(0x662c, 0x662e); E = 0xc3; // Part.var03
  CYC(0x662e, 0x662f); A = mem_rd(gb, DE);
  CYC(0x662f, 0x6630); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6630, 0x6632); goto L_6638; } // jr z
  CYC(0x6630, 0x6632);
  CALL_C(0x6632, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x6635);
  if (!(F & FZ)) { CYCT(0x6635, 0x6638); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(0x6635, 0x6638);

L_6638:
  CYC(0x6638, 0x663a); E = 0xc3; // Part.var03
  CYC(0x663a, 0x663b); A = mem_rd(gb, DE);
  CYC(0x663b, 0x663d); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(0x663d, 0x663f); goto L_6657; } // jr z
  CYC(0x663d, 0x663f);
  CALL_C(0x663f, getFreePartSlot_hook, 0x3e8e, 0x6642);
  if (!(F & FZ)) { RET_TAKEN(0x6642); return; } // ret nz
  CYC(0x6642, 0x6643);
  CYC(0x6643, 0x6645); mem_wr(gb, HL, 0x34); // PART_RAMROCK_SEED_FORM_LASER
  CYC(0x6645, 0x6646); L = alu_inc8(gb, L);
  CYC(0x6646, 0x6648); mem_wr(gb, HL, 0x0e);
  CYC(0x6648, 0x6649); L = E;
  CYC(0x6649, 0x664a); A = mem_rd(gb, DE);
  CYC(0x664a, 0x664b); A = alu_inc8(gb, A);
  CYC(0x664b, 0x664c); mem_wr(gb, HL, A);
  CYC(0x664c, 0x664e); E = 0xd6; // Part.relatedObj1
  CYC(0x664e, 0x664f); L = E;
  CYC(0x664f, 0x6651); A = 0xc0; // Part
  CYC(0x6651, 0x6652); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6652, 0x6653); A = D;
  CYC(0x6653, 0x6654); mem_wr(gb, HL, A);
  CALL_C(0x6654, objectCopyPosition_hook, 0x2242, 0x6657);

L_6657:
  CYC(0x6657, 0x6659); E = 0xc4; // Part.state
  CYC(0x6659, 0x665b); A = 0x02;
  CYC(0x665b, 0x665c); mem_wr(gb, DE, A);

state2:
  CYC(0x665c, 0x665e); A = 0x02; // Object.subid
  CALL_C(0x665e, objectGetRelatedObject1Var_hook, 0x2160, 0x6661);
  CYC(0x6661, 0x6662); A = mem_rd(gb, HL);
  CYC(0x6662, 0x6664); alu_cp(gb, 0x0e);
  if (F & FZ) { RET_TAKEN(0x6664); return; } // ret z
  CYC(0x6664, 0x6665);
  CYC(0x6665, 0x6667); E = 0xc4; // Part.state
  CYC(0x6667, 0x6669); A = 0x03;
  CYC(0x6669, 0x666a); mem_wr(gb, DE, A);
  CYC(0x666a, 0x666c); E = 0xc6; // Part.counter1
  CYC(0x666c, 0x666e); A = 0x07;
  CYC(0x666e, 0x666f); mem_wr(gb, DE, A);

state3:
  CYC(0x666f, 0x6671); E = 0xc3; // Part.var03
  CYC(0x6671, 0x6672); A = mem_rd(gb, DE);
  CYC(0x6672, 0x6674); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(0x6674, 0x6677); partDelete_hook(gb); return; } // jp z
  CYC(0x6674, 0x6677);
  CALL_C(0x6677, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x667a);
  if (!(F & FZ)) { CYCT(0x667a, 0x667d); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(0x667a, 0x667d);
  CYC(0x667d, 0x667f); E = 0xc2; // Part.subid
  CYC(0x667f, 0x6680); mem_wr(gb, DE, A);
  CYC(0x6680, 0x6682); E = 0xc4; // Part.state
  CYC(0x6682, 0x6684); A = 0x04;
  CYC(0x6684, 0x6685); mem_wr(gb, DE, A);
  RET(0x6685); return; // ret
}
