#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

#define raftDismountTileOffsets_bank06 0x590f
#define raftWallPositionOffsets_bank06 0x5978
#define raftValidTiles_bank06 0x5988

static uint16_t raft_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void raft_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void raftCalculateAdjacentWallsBitset(GB *gb, uint16_t sp0_) {
  CYC(0x5952, 0x5954); A = 0x01;
  CYC(0x5954, 0x5956); H8(hFF8B) = A;
  CYC(0x5956, 0x5959); SET_HL(raftWallPositionOffsets_bank06);
again:
  CYC(0x5959, 0x595a); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x595a, 0x595b); B = A;
  CYC(0x595b, 0x595c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x595c, 0x595d); C = A;
  CYC(0x595d, 0x595e); push_effect(gb, HL);
  CALL_C(0x595e, objectGetRelativeTile_hook, 0x1435, 0x5961);
  CYC(0x5961, 0x5962); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x5962, 0x5964);
  } else {
    CYC(0x5962, 0x5964);
    CYC(0x5964, 0x5965); E = A;
    CYC(0x5965, 0x5968); SET_HL(raftValidTiles_bank06);
    CALL_C(0x5968, findByteAtHl_hook, 0x1e17, 0x596b);
    CYC(0x596b, 0x596c); alu_ccf(gb);
  }
  CYC(0x596c, 0x596d); SET_HL(pop_effect(gb));
  CYC(0x596d, 0x596f); A = H8(hFF8B);
  CYC(0x596f, 0x5970); alu_rla(gb);
  CYC(0x5970, 0x5972); H8(hFF8B) = A;
  if (!(F & FC)) {
    CYCT(0x5972, 0x5974);
    goto again;
  }
  CYC(0x5972, 0x5974);
  CYC(0x5974, 0x5976); E = 0x33;
  CYC(0x5976, 0x5977); mem_wr(gb, DE, A);
  CYC(0x5977, 0x5978); ret_effect(gb);
}

static void raftTransferKnockbackToLink(GB *gb) {
  CYC(0x5990, 0x5993); SET_HL(w1Link_invincibilityCounter);
  CYC(0x5993, 0x5994); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x5994, 0x5995); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x5995, 0x5997);
    goto end;
  }
  CYC(0x5995, 0x5997);
  CYC(0x5997, 0x5998); E = L;
  CYC(0x5998, 0x5999); A = mem_rd(gb, DE);
  CYC(0x5999, 0x599a); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x599a, 0x599b); ret_effect(gb); return;
  }
  CYC(0x599a, 0x599b);
  CYC(0x599b, 0x599c); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x599c, 0x599d); E = alu_inc8(gb, E);
  CYC(0x599d, 0x599e); A = mem_rd(gb, DE);
  CYC(0x599e, 0x599f); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x599f, 0x59a0); E = alu_inc8(gb, E);
  CYC(0x59a0, 0x59a1); A = mem_rd(gb, DE);
  CYC(0x59a1, 0x59a2); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x59a2, 0x59a3); E = alu_inc8(gb, E);
  CYC(0x59a3, 0x59a4); A = mem_rd(gb, DE);
  CYC(0x59a4, 0x59a5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x59a5, 0x59a7); E = 0x25;
  CYC(0x59a7, 0x59a8); A = mem_rd(gb, DE);
  CYC(0x59a8, 0x59a9); L = E;
  CYC(0x59a9, 0x59aa); mem_wr(gb, HL, A);
end:
  CYC(0x59aa, 0x59ac); E = 0x2a;
  CYC(0x59ac, 0x59ad); alu_xor(gb, A);
  CYC(0x59ad, 0x59ae); mem_wr(gb, DE, A);
  CYC(0x59ae, 0x59af); E = alu_inc8(gb, E);
  CYC(0x59af, 0x59b0); mem_wr(gb, DE, A);
  CYC(0x59b0, 0x59b1); ret_effect(gb);
}

void specialObjectCode_raft_b06_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x57ef, 0x57f0); A = D;
  CYC(0x57f0, 0x57f3); W8(wLinkRidingObject) = A;
  CYC(0x57f3, 0x57f5); E = 0x04;
  CYC(0x57f5, 0x57f6); A = mem_rd(gb, DE);
  CYC(0x57f6, 0x57f7); push_effect(gb, 0x57f7);
  switch (raft_jump_table(gb)) {
    case 0x57ff: goto state0;
    case 0x5826: goto state1;
    case 0x5917: goto state2;
    case 0x592e: goto state3;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x57ff, 0x5802); SET_HL(0x41f7);
  CYC(0x5802, 0x5804); E = 0x05;
  CALL_C(0x5804, interBankCall_hook, 0x008a, 0x5807);
  CYC(0x5807, 0x5808); alu_xor(gb, A);
  CALL_C(0x5808, specialObjectSetAnimation_hook, 0x2b0a, 0x580b);
  CALL_C(0x580b, itemIncState_hook, 0x23ea, 0x580e);
  CYC(0x580e, 0x5810); L = 0x24;
  CYC(0x5810, 0x5812); A = 0x80;
  CYC(0x5812, 0x5813); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5813, 0x5814); L = alu_inc8(gb, L);
  CYC(0x5814, 0x5816); A = 0x06;
  CYC(0x5816, 0x5817); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5817, 0x5818); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5818, 0x581a); L = 0x06;
  CYC(0x581a, 0x581c); mem_wr(gb, HL, 0x0c);
  CYC(0x581c, 0x581d); A = D;
  CYC(0x581d, 0x5820); W8(wLinkObjectIndex) = A;
  CALL_C(0x5820, setCameraFocusedObjectToLink_hook, 0x12f0, 0x5823);
  CYC(0x5823, 0x5826);
  goto saveRaftPosition;

state1:
  CYC(0x5826, 0x5829); A = W8(wPaletteThread_mode);
  CYC(0x5829, 0x582a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x582a, 0x582b); ret_effect(gb); return; }
  CYC(0x582a, 0x582b);
  CALL_C(0x582b, retIfTextIsActive_hook, 0x1859, 0x582e);
  CYC(0x582e, 0x5831); A = W8(wScrollMode);
  CYC(0x5831, 0x5833); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x5833, 0x5834); ret_effect(gb); return; }
  CYC(0x5833, 0x5834);
  CYC(0x5834, 0x5837); A = W8(wDisabledObjects);
  CYC(0x5837, 0x5839); alu_and(gb, 0x81);
  if (!(F & FZ)) { CYCT(0x5839, 0x583a); ret_effect(gb); return; }
  CYC(0x5839, 0x583a);
  CYC(0x583a, 0x583d); A = W8(wLinkForceState);
  CYC(0x583d, 0x583f); alu_cp(gb, 0x02);
  if (F & FZ) {
    CYCT(0x583f, 0x5841);
    goto respawning;
  }
  CYC(0x583f, 0x5841);
  CYC(0x5841, 0x5844); A = W8(w1Link_state);
  CYC(0x5844, 0x5846); alu_cp(gb, 0x02);
  if (!(F & FZ)) {
    CYCT(0x5846, 0x5848);
    goto updateDirection;
  }
  CYC(0x5846, 0x5848);
respawning:
  CYC(0x5848, 0x584b); SET_HL(wLinkLocalRespawnY);
  CYC(0x584b, 0x584d); E = 0x0b;
  CYC(0x584d, 0x584e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x584e, 0x584f); mem_wr(gb, DE, A);
  CYC(0x584f, 0x5851); E = 0x0d;
  CYC(0x5851, 0x5852); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5852, 0x5853); mem_wr(gb, DE, A);
  CYC(0x5853, 0x5856); objectSetInvisible_hook(gb); return;

updateDirection:
  CALL_C(0x5856, updateCompanionDirectionFromAngle_hook, 0x2b5d, 0x5859);
  if (F & FC) {
    CYCT(0x5859, 0x585b);
    CALL_C(0x5860, specialObjectSetAnimation_hook, 0x2b0a, 0x5863);
  } else {
    CYC(0x5859, 0x585b);
    CALL_C(0x585b, specialObjectAnimate_hook, 0x2aef, 0x585e);
    CYC(0x585e, 0x5860);
  }
  CYC(0x5863, 0x5866); push_effect(gb, 0x5866); raftCalculateAdjacentWallsBitset(gb, sp0_);
  CYC(0x5866, 0x5869); push_effect(gb, 0x5869); raftTransferKnockbackToLink(gb);
  CYC(0x5869, 0x586c); SET_HL(w1Link_knockbackCounter);
  CYC(0x586c, 0x586d); A = mem_rd(gb, HL);
  CYC(0x586d, 0x586e); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x586e, 0x5870);
    goto updateMovement;
  }
  CYC(0x586e, 0x5870);
  CYC(0x5870, 0x5871); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x5871, 0x5872); L = alu_dec8(gb, L);
  CYC(0x5872, 0x5873); C = mem_rd(gb, HL);
  CYC(0x5873, 0x5875); B = 0x28;
  CYC(0x5875, 0x5878); SET_HL(0x5d9f);
  CYC(0x5878, 0x587a); E = 0x05;
  CALL_C(0x587a, interBankCall_hook, 0x008a, 0x587d);
  CYC(0x587d, 0x587f); A = 0x88;
  CYC(0x587f, 0x5882); W8(wcc92) = A;
  CYC(0x5882, 0x5884);
  goto notDismounting;

updateMovement:
  CYC(0x5884, 0x5886); E = 0x10;
  CYC(0x5886, 0x5888); A = 0x23;
  CYC(0x5888, 0x5889); mem_wr(gb, DE, A);
  CYC(0x5889, 0x588b); E = 0x09;
  CYC(0x588b, 0x588e); A = W8(wLinkAngle);
  CYC(0x588e, 0x588f); mem_wr(gb, DE, A);
  CYC(0x588f, 0x5891); alu_bit(gb, 7, A);
  if (!(F & FZ)) {
    CYCT(0x5891, 0x5893);
    goto notDismounting;
  }
  CYC(0x5891, 0x5893);
  CYC(0x5893, 0x5896); A = W8(wLinkImmobilized);
  CYC(0x5896, 0x5897); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x5897, 0x5899);
    goto notDismounting;
  }
  CYC(0x5897, 0x5899);
  CYC(0x5899, 0x589c); SET_HL(0x5d97);
  CYC(0x589c, 0x589e); E = 0x05;
  CALL_C(0x589e, interBankCall_hook, 0x008a, 0x58a1);
  CYC(0x58a1, 0x58a2); A = C;
  CYC(0x58a2, 0x58a3); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x58a3, 0x58a5);
    goto positionUnchanged;
  }
  CYC(0x58a3, 0x58a5);
  CYC(0x58a5, 0x58a7); A = 0x08;
  CYC(0x58a7, 0x58aa); W8(wcc92) = A;
notDismounting:
  CYC(0x58aa, 0x58ab); H = D;
  CYC(0x58ab, 0x58ad); L = 0x3e;
  CYC(0x58ad, 0x58af); A = 0xff;
  CYC(0x58af, 0x58b0); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x58b0, 0x58b2); mem_wr(gb, HL, 0x04);
  CYC(0x58b2, 0x58b3); ret_effect(gb); return;

positionUnchanged:
  CYC(0x58b3, 0x58b4); H = D;
  CYC(0x58b4, 0x58b6); E = 0x09;
  CYC(0x58b6, 0x58b7); A = mem_rd(gb, DE);
  CYC(0x58b7, 0x58b9); L = 0x3e;
  CYC(0x58b9, 0x58ba); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x58ba, 0x58bb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  if (!(F & FZ)) { CYCT(0x58bb, 0x58bc); ret_effect(gb); return; }
  CYC(0x58bb, 0x58bc);
  CYC(0x58bc, 0x58bd); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x58bd, 0x58be); ret_effect(gb); return; }
  CYC(0x58bd, 0x58be);
  CYC(0x58be, 0x58bf); E = alu_dec8(gb, E);
  CYC(0x58bf, 0x58c0); A = mem_rd(gb, DE);
  CYC(0x58c0, 0x58c3); SET_HL(raftDismountTileOffsets_bank06);
  CYC(0x58c3, 0x58c4); raft_add_double_index(gb, 0x58c4);
  CYC(0x58c4, 0x58c5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x58c5, 0x58c6); C = mem_rd(gb, HL);
  CYC(0x58c6, 0x58c7); H = D;
  CYC(0x58c7, 0x58c9); L = 0x0b;
  CYC(0x58c9, 0x58ca); alu_add(gb, mem_rd(gb, HL));
  CYC(0x58ca, 0x58cb); B = A;
  CYC(0x58cb, 0x58cd); L = 0x0d;
  CYC(0x58cd, 0x58ce); A = mem_rd(gb, HL);
  CYC(0x58ce, 0x58cf); alu_add(gb, C);
  CYC(0x58cf, 0x58d0); C = A;
  CALL_C(0x58d0, getTileAtPosition_hook, 0x1447, 0x58d3);
  CYC(0x58d3, 0x58d5); H = 0xce;
  CYC(0x58d5, 0x58d6); A = mem_rd(gb, HL);
  CYC(0x58d6, 0x58d7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x58d7, 0x58d9);
    goto checkDismount;
  }
  CYC(0x58d7, 0x58d9);
  CYC(0x58d9, 0x58db); alu_cp(gb, 0x18);
  if (!(F & FZ)) {
    CYCT(0x58db, 0x58dd);
    goto notDismounting;
  }
  CYC(0x58db, 0x58dd);
checkDismount:
  CYC(0x58dd, 0x58e0); SET_HL(0x5ea3);
  CYC(0x58e0, 0x58e2); E = 0x05;
  CALL_C(0x58e2, interBankCall_hook, 0x008a, 0x58e5);
  CYC(0x58e5, 0x58e7); A = H8(hFF8B);
  CYC(0x58e7, 0x58e8); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x58e8, 0x58ea);
    goto notDismounting;
  }
  CYC(0x58e8, 0x58ea);
  CYC(0x58ea, 0x58eb); A = alu_inc8(gb, A);
  CYC(0x58eb, 0x58ee); W8(wMenuDisabled) = A;
  CYC(0x58ee, 0x58f0); A = 0x0b;
  CYC(0x58f0, 0x58f3); W8(wLinkForceState) = A;
  CYC(0x58f3, 0x58f5); A = 0x0e;
  CYC(0x58f5, 0x58f8); W8(wLinkStateParameter) = A;
  CALL_C(0x58f8, itemUpdateAngle_hook, 0x2cf0, 0x58fb);
  CYC(0x58fb, 0x58fc); E = L;
  CYC(0x58fc, 0x58fe); H = 0xd0;
  CYC(0x58fe, 0x58ff); A = mem_rd(gb, DE);
  CYC(0x58ff, 0x5900); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5900, 0x5901); E = alu_inc8(gb, E);
  CYC(0x5901, 0x5902); A = mem_rd(gb, DE);
  CYC(0x5902, 0x5903); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5903, 0x5904); A = H;
  CYC(0x5904, 0x5907); W8(wLinkObjectIndex) = A;
  CALL_C(0x5907, setCameraFocusedObjectToLink_hook, 0x12f0, 0x590a);
  CALL_C(0x590a, itemIncState_hook, 0x23ea, 0x590d);
  CYC(0x590d, 0x590f);
  goto saveRaftPosition;

state2:
  CYC(0x5917, 0x5919); A = 0x80;
  CYC(0x5919, 0x591c); W8(wcc92) = A;
  CALL_C(0x591c, itemDecCounter1_hook, 0x23d6, 0x591f);
  if (!(F & FZ)) { CYCT(0x591f, 0x5920); ret_effect(gb); return; }
  CYC(0x591f, 0x5920);
  CYC(0x5920, 0x5921); alu_xor(gb, A);
  CYC(0x5921, 0x5924); W8(wMenuDisabled) = A;
  CYC(0x5924, 0x5926); E = 0x00;
  CYC(0x5926, 0x5927); A = alu_inc8(gb, A);
  CYC(0x5927, 0x5928); mem_wr(gb, DE, A);
  CALL_C(0x5928, updateLinkLocalRespawnPosition_hook, 0x113a, 0x592b);
  CALL_C(0x592b, itemIncState_hook, 0x23ea, 0x592e);
state3:
  CYC(0x592e, 0x5931); SET_BC(0xe602);
  CALL_C(0x5931, objectCreateInteraction_hook, 0x24c5, 0x5934);
  if (!(F & FZ)) { CYCT(0x5934, 0x5935); ret_effect(gb); return; }
  CYC(0x5934, 0x5935);
  CYC(0x5935, 0x5937); E = 0x08;
  CYC(0x5937, 0x5938); A = mem_rd(gb, DE);
  CYC(0x5938, 0x593a); L = 0x48;
  CYC(0x593a, 0x593b); mem_wr(gb, HL, A);
  CYC(0x593b, 0x593e); itemDelete_hook(gb); return;

saveRaftPosition:
  CYC(0x593e, 0x5941); SET_BC(wLastAnimalMountPointY);
  CYC(0x5941, 0x5942); H = D;
  CYC(0x5942, 0x5944); L = 0x0b;
  CYC(0x5944, 0x5945); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5945, 0x5946); mem_wr(gb, BC, A);
  CYC(0x5946, 0x5947); C = alu_inc8(gb, C);
  CYC(0x5947, 0x5948); L = alu_inc8(gb, L);
  CYC(0x5948, 0x5949); A = mem_rd(gb, HL);
  CYC(0x5949, 0x594a); mem_wr(gb, BC, A);
  CYC(0x594a, 0x594d); SET_HL(0x467c);
  CYC(0x594d, 0x594f); E = 0x05;
  CYC(0x594f, 0x5952); interBankCall_hook(gb);
}
