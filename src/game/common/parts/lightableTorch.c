#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x4550, 0x4552); goto normalStatus; } // jr z
  CYC(0x4550, 0x4552);

  CYC(0x4552, 0x4553); H = D;
  CYC(0x4553, 0x4555); L = 0xc2; // Part.subid
  CYC(0x4555, 0x4556); A = mem_rd(gb, HL);
  CYC(0x4556, 0x4558); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(0x4558, 0x455a); goto normalStatus; } // jr z
  CYC(0x4558, 0x455a);
  CYC(0x455a, 0x455c); L = 0xc7; // Part.counter2
  CYC(0x455c, 0x455d); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x455d, 0x455e); mem_wr(gb, HL, A); // [counter1] = [counter2]
  CYC(0x455e, 0x4560); L = 0xc4; // Part.state
  CYC(0x4560, 0x4562); mem_wr(gb, HL, 0x02);

normalStatus:
  CYC(0x4562, 0x4564); E = 0xc2; // Part.subid
  CYC(0x4564, 0x4565); A = mem_rd(gb, DE);
  {
    CYC(0x4565, 0x4566); push_effect(gb, 0x4566);
    uint16_t target = lightableTorch_jump_table(gb);
    if (target == 0x456c) goto subid0;
    if (target == 0x4597) goto subid1;
    goto subid2;
  }

subid0:
  CYC(0x456c, 0x456e); E = 0xc4; // Part.state
  CYC(0x456e, 0x456f); A = mem_rd(gb, DE);
  {
    CYC(0x456f, 0x4570); push_effect(gb, 0x4570);
    uint16_t target = lightableTorch_jump_table(gb);
    if (target == 0x4576) goto gotoState1;
    if (target == 0x4579) goto ret_;
    goto subid0State2;
  }

gotoState1:
  CYC(0x4576, 0x4578); A = 0x01;
  CYC(0x4578, 0x4579); mem_wr(gb, DE, A);

ret_:
  RET(0x4579); return; // ret

subid0State2:
  CYC(0x457a, 0x457d); SET_HL(0xcc8f); // wNumTorchesLit
  CYC(0x457d, 0x457e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x457e, 0x4580); A = 0x72; // SND_LIGHTTORCH
  CALL_C(0x4580, playSound_b00_hook, 0x0c98, 0x4583);
  CALL_C(0x4583, objectGetShortPosition_hook, 0x2096, 0x4586);
  CYC(0x4586, 0x4587); C = A;
  CYC(0x4587, 0x458a); A = mem_rd(gb, 0xcc2d); // wActiveGroup
  CYC(0x458a, 0x458b); alu_or(gb, A);
  CYC(0x458b, 0x458d); A = 0xa1; // TILEINDEX_OVERWORLD_LIT_TORCH
  if (F & FZ) { CYCT(0x458d, 0x458f); goto L_4591; } // jr z
  CYC(0x458d, 0x458f);
  CYC(0x458f, 0x4591); A = 0x09; // TILEINDEX_LIT_TORCH

L_4591:
  CALL_C(0x4591, setTile_hook, 0x3a9c, 0x4594);
  CYC(0x4594, 0x4597); partDelete_hook(gb); return; // jp

subid1:
  CYC(0x4597, 0x4599); E = 0xc4; // Part.state
  CYC(0x4599, 0x459a); A = mem_rd(gb, DE);
  {
    CYC(0x459a, 0x459b); push_effect(gb, 0x459b);
    uint16_t target = lightableTorch_jump_table(gb);
    if (target == 0x4576) goto gotoState1;
    if (target == 0x4579) goto ret_;
    if (target == 0x45a3) goto subid1State2;
    goto subid1State3;
  }

subid1State2:
  CYC(0x45a3, 0x45a4); H = D;
  CYC(0x45a4, 0x45a5); L = E;
  CYC(0x45a5, 0x45a6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 3
  CYC(0x45a6, 0x45a8); L = 0xe4; // Part.collisionType
  CYC(0x45a8, 0x45aa); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x45aa, 0x45ac); L = 0xc7; // Part.counter2
  CYC(0x45ac, 0x45ad); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x45ad, 0x45ae); mem_wr(gb, HL, A); // [counter1] = [counter2]
  CYC(0x45ae, 0x45b1); SET_HL(0xcc8f); // wNumTorchesLit
  CYC(0x45b1, 0x45b2); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x45b2, 0x45b4); A = 0x72; // SND_LIGHTTORCH
  CALL_C(0x45b4, playSound_b00_hook, 0x0c98, 0x45b7);
  CALL_C(0x45b7, objectGetShortPosition_hook, 0x2096, 0x45ba);
  CYC(0x45ba, 0x45bb); C = A;
  CYC(0x45bb, 0x45bd); A = 0x09; // TILEINDEX_LIT_TORCH
  CYC(0x45bd, 0x45bf); goto setTile_; // jr

subid1State3:
  CYC(0x45bf, 0x45c2); A = mem_rd(gb, 0xcc00); // wFrameCounter
  CYC(0x45c2, 0x45c4); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(0x45c4); return; } // ret nz
  CYC(0x45c4, 0x45c5);
  CALL_C(0x45c5, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x45c8);
  if (!(F & FZ)) { RET_TAKEN(0x45c8); return; } // ret nz
  CYC(0x45c8, 0x45c9);
  CYC(0x45c9, 0x45cb); L = 0xe4; // Part.collisionType
  CYC(0x45cb, 0x45cd); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x45cd, 0x45cf); L = 0xc4; // Part.state
  CYC(0x45cf, 0x45d1); mem_wr(gb, HL, 0x01);
  CYC(0x45d1, 0x45d4); SET_HL(0xcc8f); // wNumTorchesLit
  CYC(0x45d4, 0x45d5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x45d5, objectGetShortPosition_hook, 0x2096, 0x45d8);
  CYC(0x45d8, 0x45d9); C = A;
  CYC(0x45d9, 0x45db); A = 0x08; // TILEINDEX_UNLIT_TORCH
  CYC(0x45db, 0x45dd); goto setTile_; // jr

subid2:
  CYC(0x45dd, 0x45df); E = 0xc4; // Part.state
  CYC(0x45df, 0x45e0); A = mem_rd(gb, DE);
  {
    CYC(0x45e0, 0x45e1); push_effect(gb, 0x45e1);
    uint16_t target = lightableTorch_jump_table(gb);
    if (target == 0x4576) goto gotoState1;
    if (target == 0x45eb) goto subid2State1;
    if (target == 0x45fa) goto subid2State2;
    if (target == 0x460f) goto subid2State3;
    goto subid2State4;
  }

subid2State1:
  CYC(0x45eb, 0x45ee); push_effect(gb, 0x45ee); lightableTorch_getTileAtRelatedObjPosition_hook(gb);
  CYC(0x45ee, 0x45f0); alu_cp(gb, 0x09); // TILEINDEX_LIT_TORCH
  if (F & FZ) { RET_TAKEN(0x45f0); return; } // ret z
  CYC(0x45f0, 0x45f1);
  CYC(0x45f1, 0x45f2); H = D;
  CYC(0x45f2, 0x45f4); L = 0xc4; // Part.state
  CYC(0x45f4, 0x45f5); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x45f5, 0x45f7); L = 0xc6; // Part.counter1
  CYC(0x45f7, 0x45f9); mem_wr(gb, HL, 0xf0);
  RET(0x45f9); return; // ret

subid2State2:
  CALL_C(0x45fa, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x45fd);
  if (!(F & FZ)) { CYCT(0x45fd, 0x4600); goto gotoState1IfTileAtRelatedObjPositionIsNotLit; } // jp nz
  CYC(0x45fd, 0x4600);
  CYC(0x4600, 0x4601); L = E;
  CYC(0x4601, 0x4602); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 3
  CYC(0x4602, 0x4605); SET_HL(0xcc8f); // wNumTorchesLit
  CYC(0x4605, 0x4606); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CALL_C(0x4606, objectGetShortPosition_hook, 0x2096, 0x4609);
  CYC(0x4609, 0x460a); C = A;
  CYC(0x460a, 0x460c); A = 0x08; // TILEINDEX_UNLIT_TORCH

setTile_:
  CYC(0x460c, 0x460f); setTile_hook(gb); return; // jp

subid2State3:
  CYC(0x460f, 0x4612); push_effect(gb, 0x4612); lightableTorch_getTileAtRelatedObjPosition_hook(gb);
  CYC(0x4612, 0x4614); alu_cp(gb, 0x08); // TILEINDEX_UNLIT_TORCH
  if (F & FZ) { RET_TAKEN(0x4614); return; } // ret z
  CYC(0x4614, 0x4615);
  CYC(0x4615, 0x4617); E = 0xc4; // Part.state
  CYC(0x4617, 0x4619); A = 0x04;
  CYC(0x4619, 0x461a); mem_wr(gb, DE, A);
  RET(0x461a); return; // ret

subid2State4:
  CYC(0x461b, 0x461d); A = 0x01;
  CYC(0x461d, 0x461e); mem_wr(gb, DE, A); // [state]
  CYC(0x461e, 0x4621); SET_HL(0xcc8f); // wNumTorchesLit
  CYC(0x4621, 0x4622); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(0x4622, objectGetShortPosition_hook, 0x2096, 0x4625);
  CYC(0x4625, 0x4626); C = A;
  CYC(0x4626, 0x4628); A = 0x09; // TILEINDEX_LIT_TORCH
  CYC(0x4628, 0x462a); goto setTile_; // jr

gotoState1IfTileAtRelatedObjPositionIsNotLit:
  CYC(0x4636, 0x4639); push_effect(gb, 0x4639); lightableTorch_getTileAtRelatedObjPosition_hook(gb);
  CYC(0x4639, 0x463b); alu_cp(gb, 0x09); // TILEINDEX_LIT_TORCH
  if (!(F & FZ)) { RET_TAKEN(0x463b); return; } // ret nz
  CYC(0x463b, 0x463c);
  CYC(0x463c, 0x463e); E = 0xc4; // Part.state
  CYC(0x463e, 0x4640); A = 0x01;
  CYC(0x4640, 0x4641); mem_wr(gb, DE, A);
  RET(0x4641); return; // ret
}

void lightableTorch_getTileAtRelatedObjPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x462a, 0x462c); A = 0x0b; // Object.yh
  CALL_C(0x462c, objectGetRelatedObject2Var_hook, 0x2164, 0x462f);
  CYC(0x462f, 0x4630); B = mem_rd(gb, HL);
  CYC(0x4630, 0x4632); L = 0xcd; // xh
  CYC(0x4632, 0x4633); C = mem_rd(gb, HL);
  CYC(0x4633, 0x4636); getTileAtPosition_hook(gb); return; // jp
}
