#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode06), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode06), (from), (to), true)

static uint16_t lightableTorch_jump_table(GB *gb) {
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

void partCode06_hook(GB *gb);
void lightableTorch_getTileAtRelatedObjPosition_hook(GB *gb);

void partCode06_hook(GB *gb) {
  BASE(partCode06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);

  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+5); L = 0xc2; // Part.subid
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+8); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+8, b_+10); goto normalStatus; } // jr z
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); L = 0xc7; // Part.counter2
  CYC(b_+12, b_+13); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); // [counter1] = [counter2]
  CYC(b_+14, b_+16); L = 0xc4; // Part.state
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x02);

normalStatus:
  CYC(b_+18, b_+20); E = 0xc2; // Part.subid
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  {
    CYC(b_+21, b_+22); push_effect(gb, b_+22);
    uint16_t target = lightableTorch_jump_table(gb);
    if (target == b_+28) goto subid0;
    if (target == b_+71) goto subid1;
    goto subid2;
  }

subid0:
  CYC(b_+28, b_+30); E = 0xc4; // Part.state
  CYC(b_+30, b_+31); A = mem_rd(gb, DE);
  {
    CYC(b_+31, b_+32); push_effect(gb, b_+32);
    uint16_t target = lightableTorch_jump_table(gb);
    if (target == b_+38) goto gotoState1;
    if (target == b_+41) goto ret_;
    goto subid0State2;
  }

gotoState1:
  CYC(b_+38, b_+40); A = 0x01;
  CYC(b_+40, b_+41); mem_wr(gb, DE, A);

ret_:
  RET(b_+41); return; // ret

subid0State2:
  CYC(b_+42, b_+45); SET_HL(wNumTorchesLit); // wNumTorchesLit
  CYC(b_+45, b_+46); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+46, b_+48); A = 0x72; // SND_LIGHTTORCH
  CALL_C(b_+48, playSound_b00_hook, SYM(playSound_b00), b_+51);
  CALL_C(b_+51, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+54);
  CYC(b_+54, b_+55); C = A;
  CYC(b_+55, b_+58); A = mem_rd(gb, wActiveGroup); // wActiveGroup
  CYC(b_+58, b_+59); alu_or(gb, A);
  CYC(b_+59, b_+61); A = 0xa1; // TILEINDEX_OVERWORLD_LIT_TORCH
  if (F & FZ) { CYCT(b_+61, b_+63); goto L_4591; } // jr z
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+65); A = 0x09; // TILEINDEX_LIT_TORCH

L_4591:
  CALL_C(b_+65, setTile_hook, SYM(setTile), b_+68);
  CYC(b_+68, b_+71); partDelete_hook(gb); return; // jp

subid1:
  CYC(b_+71, b_+73); E = 0xc4; // Part.state
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  {
    CYC(b_+74, b_+75); push_effect(gb, b_+75);
    uint16_t target = lightableTorch_jump_table(gb);
    if (target == b_+38) goto gotoState1;
    if (target == b_+41) goto ret_;
    if (target == b_+83) goto subid1State2;
    goto subid1State3;
  }

subid1State2:
  CYC(b_+83, b_+84); H = D;
  CYC(b_+84, b_+85); L = E;
  CYC(b_+85, b_+86); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 3
  CYC(b_+86, b_+88); L = 0xe4; // Part.collisionType
  CYC(b_+88, b_+90); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+90, b_+92); L = 0xc7; // Part.counter2
  CYC(b_+92, b_+93); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(b_+93, b_+94); mem_wr(gb, HL, A); // [counter1] = [counter2]
  CYC(b_+94, b_+97); SET_HL(wNumTorchesLit); // wNumTorchesLit
  CYC(b_+97, b_+98); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+98, b_+100); A = 0x72; // SND_LIGHTTORCH
  CALL_C(b_+100, playSound_b00_hook, SYM(playSound_b00), b_+103);
  CALL_C(b_+103, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+106);
  CYC(b_+106, b_+107); C = A;
  CYC(b_+107, b_+109); A = 0x09; // TILEINDEX_LIT_TORCH
  CYC(b_+109, b_+111); goto setTile_; // jr

subid1State3:
  CYC(b_+111, b_+114); A = mem_rd(gb, wFrameCounter); // wFrameCounter
  CYC(b_+114, b_+116); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+116); return; } // ret nz
  CYC(b_+116, b_+117);
  CALL_C(b_+117, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+120);
  if (!(F & FZ)) { RET_TAKEN(b_+120); return; } // ret nz
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+123); L = 0xe4; // Part.collisionType
  CYC(b_+123, b_+125); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+125, b_+127); L = 0xc4; // Part.state
  CYC(b_+127, b_+129); mem_wr(gb, HL, 0x01);
  CYC(b_+129, b_+132); SET_HL(wNumTorchesLit); // wNumTorchesLit
  CYC(b_+132, b_+133); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+133, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+136);
  CYC(b_+136, b_+137); C = A;
  CYC(b_+137, b_+139); A = 0x08; // TILEINDEX_UNLIT_TORCH
  CYC(b_+139, b_+141); goto setTile_; // jr

subid2:
  CYC(b_+141, b_+143); E = 0xc4; // Part.state
  CYC(b_+143, b_+144); A = mem_rd(gb, DE);
  {
    CYC(b_+144, b_+145); push_effect(gb, b_+145);
    uint16_t target = lightableTorch_jump_table(gb);
    if (target == b_+38) goto gotoState1;
    if (target == b_+155) goto subid2State1;
    if (target == b_+170) goto subid2State2;
    if (target == b_+191) goto subid2State3;
    goto subid2State4;
  }

subid2State1:
  CYC(b_+155, b_+158); push_effect(gb, b_+158); lightableTorch_getTileAtRelatedObjPosition_hook(gb);
  CYC(b_+158, b_+160); alu_cp(gb, 0x09); // TILEINDEX_LIT_TORCH
  if (F & FZ) { RET_TAKEN(b_+160); return; } // ret z
  CYC(b_+160, b_+161);
  CYC(b_+161, b_+162); H = D;
  CYC(b_+162, b_+164); L = 0xc4; // Part.state
  CYC(b_+164, b_+165); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+165, b_+167); L = 0xc6; // Part.counter1
  CYC(b_+167, b_+169); mem_wr(gb, HL, 0xf0);
  RET(b_+169); return; // ret

subid2State2:
  CALL_C(b_+170, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+173);
  if (!(F & FZ)) { CYCT(b_+173, b_+176); goto gotoState1IfTileAtRelatedObjPositionIsNotLit; } // jp nz
  CYC(b_+173, b_+176);
  CYC(b_+176, b_+177); L = E;
  CYC(b_+177, b_+178); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 3
  CYC(b_+178, b_+181); SET_HL(wNumTorchesLit); // wNumTorchesLit
  CYC(b_+181, b_+182); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(b_+182, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+185);
  CYC(b_+185, b_+186); C = A;
  CYC(b_+186, b_+188); A = 0x08; // TILEINDEX_UNLIT_TORCH

setTile_:
  CYC(b_+188, b_+191); setTile_hook(gb); return; // jp

subid2State3:
  CYC(b_+191, b_+194); push_effect(gb, b_+194); lightableTorch_getTileAtRelatedObjPosition_hook(gb);
  CYC(b_+194, b_+196); alu_cp(gb, 0x08); // TILEINDEX_UNLIT_TORCH
  if (F & FZ) { RET_TAKEN(b_+196); return; } // ret z
  CYC(b_+196, b_+197);
  CYC(b_+197, b_+199); E = 0xc4; // Part.state
  CYC(b_+199, b_+201); A = 0x04;
  CYC(b_+201, b_+202); mem_wr(gb, DE, A);
  RET(b_+202); return; // ret

subid2State4:
  CYC(b_+203, b_+205); A = 0x01;
  CYC(b_+205, b_+206); mem_wr(gb, DE, A); // [state]
  CYC(b_+206, b_+209); SET_HL(wNumTorchesLit); // wNumTorchesLit
  CYC(b_+209, b_+210); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+210, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+213);
  CYC(b_+213, b_+214); C = A;
  CYC(b_+214, b_+216); A = 0x09; // TILEINDEX_LIT_TORCH
  CYC(b_+216, b_+218); goto setTile_; // jr

gotoState1IfTileAtRelatedObjPositionIsNotLit:
  CYC(b_+230, b_+233); push_effect(gb, b_+233); lightableTorch_getTileAtRelatedObjPosition_hook(gb);
  CYC(b_+233, b_+235); alu_cp(gb, 0x09); // TILEINDEX_LIT_TORCH
  if (!(F & FZ)) { RET_TAKEN(b_+235); return; } // ret nz
  CYC(b_+235, b_+236);
  CYC(b_+236, b_+238); E = 0xc4; // Part.state
  CYC(b_+238, b_+240); A = 0x01;
  CYC(b_+240, b_+241); mem_wr(gb, DE, A);
  RET(b_+241); return; // ret
}

void lightableTorch_getTileAtRelatedObjPosition_hook(GB *gb) {
  BASE(partCode06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+218, b_+220); A = 0x0b; // Object.yh
  CALL_C(b_+220, objectGetRelatedObject2Var_hook, SYM(objectGetRelatedObject2Var), b_+223);
  CYC(b_+223, b_+224); B = mem_rd(gb, HL);
  CYC(b_+224, b_+226); L = 0xcd; // xh
  CYC(b_+226, b_+227); C = mem_rd(gb, HL);
  CYC(b_+227, b_+230); getTileAtPosition_hook(gb); return; // jp
}
