#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t seaEffects_jump_table(GB *gb) {
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

static void seaEffects_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

static void seaEffects_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// PART_SEA_EFFECTS
// When this object exists, it applies the effects of whirlpool and pollution tiles.
void partCode2e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x62de, 0x62e0); E = 0xc4; // Part.state
  CYC(0x62e0, 0x62e1); A = mem_rd(gb, DE);
  CYC(0x62e1, 0x62e2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x62e2, 0x62e4); goto initialized; } // jr nz
  CYC(0x62e2, 0x62e4);
  CYC(0x62e4, 0x62e5); A = alu_inc8(gb, A);
  CYC(0x62e5, 0x62e6); mem_wr(gb, DE, A);
  CYC(0x62e6, 0x62e9); goto setCounter1To20; // jp

initialized:
  CYC(0x62e9, 0x62ec); A = mem_rd(gb, w1Link_state);
  CYC(0x62ec, 0x62ee); alu_sub(gb, 0x01); // LINK_STATE_NORMAL
  if (!(F & FZ)) { RET_TAKEN(0x62ee); return; } // ret nz
  CYC(0x62ee, 0x62ef);
  CYC(0x62ef, 0x62f2); mem_wr(gb, wDisableScreenTransitions, A);
  CALL_C(0x62f2, checkLinkCollisionsEnabled_hook, 0x1d32, 0x62f5);
  if (F & FC) { CYCT(0x62f5, 0x62f7); goto afterSwimCheck; } // jr c
  CYC(0x62f5, 0x62f7);
  CYC(0x62f7, 0x62fa); A = mem_rd(gb, wLinkSwimmingState);
  CYC(0x62fa, 0x62fb); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(0x62fb); return; } // ret nc
  CYC(0x62fb, 0x62fc);

afterSwimCheck:
  CYC(0x62fc, 0x62ff); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x62ff, 0x6300); H = A;
  CYC(0x6300, 0x6302); L = 0x00; // SpecialObject.start
  CALL_C(0x6302, objectTakePosition_hook, 0x2274, 0x6305);
  CYC(0x6305, 0x6307); L = 0x01; // SpecialObject.id
  CYC(0x6307, 0x6308); A = mem_rd(gb, HL);
  CYC(0x6308, 0x630a); alu_cp(gb, 0x13); // SPECIALOBJECT_RAFT
  CYC(0x630a, 0x630c); A = 0x05;
  if (!(F & FZ)) { CYCT(0x630c, 0x630e); goto notRaft; } // jr nz
  CYC(0x630c, 0x630e);
  CYC(0x630e, 0x630f); alu_add(gb, A);

notRaft:
  CYC(0x630f, 0x6310); H = D;
  CYC(0x6310, 0x6312); L = 0xc7; // Part.counter2
  CYC(0x6312, 0x6313); mem_wr(gb, HL, A);
  CYC(0x6313, 0x6315); L = 0xcb; // Part.yh
  CYC(0x6315, 0x6316); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6316, 0x6317); B = A;
  CYC(0x6317, 0x6318); L = alu_inc8(gb, L);
  CYC(0x6318, 0x6319); C = mem_rd(gb, HL);
  CYC(0x6319, 0x631c); SET_HL(0xff8b);
  CYC(0x631c, 0x631e); mem_wr(gb, HL, 0x06);

waterLoop:
  CYC(0x631e, 0x6321); SET_HL(0xff8b);
  CYC(0x6321, 0x6322); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(0x6322, 0x6324); goto applyCurrentsDirection; } // jr z
  CYC(0x6322, 0x6324);
  CYC(0x6324, 0x6325); H = D;
  CYC(0x6325, 0x6327); L = 0xc7; // Part.counter2
  CYC(0x6327, 0x6328); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(0x6328, 0x6329); A = mem_rd(gb, HL);
  CYC(0x6329, 0x632c); SET_HL(0x63fc); // @positionOffsets
  CYC(0x632c, 0x632d); seaEffects_addDoubleIndexToHl_from_rst(gb, 0x632d);
  CYC(0x632d, 0x632e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x632e, 0x632f); alu_add(gb, B);
  CYC(0x632f, 0x6330); B = A;
  CYC(0x6330, 0x6331); A = mem_rd(gb, HL);
  CYC(0x6331, 0x6332); alu_add(gb, C);
  CYC(0x6332, 0x6333); C = A;
  CALL_C(0x6333, getTileAtPosition_hook, 0x1447, 0x6336);
  CYC(0x6336, 0x6337); E = A;
  CYC(0x6337, 0x6338); A = L;
  CYC(0x6338, 0x633a); mem_wr(gb, 0xff8a, A);
  CYC(0x633a, 0x633d); SET_HL(0x6418); // harmfulWaterTilesCollisionTable
  CALL_C(0x633d, lookupCollisionTable_paramE_hook, 0x1e20, 0x6340);
  if (!(F & FC)) { CYCT(0x6340, 0x6342); goto waterLoop; } // jr nc
  CYC(0x6340, 0x6342);
  {
    CYC(0x6342, 0x6343); push_effect(gb, 0x6343);
    uint16_t target = seaEffects_jump_table(gb);
    if (target == 0x6367) goto collision0;
    if (target == 0x6398) goto collision1;
    goto collision2;
  }

applyCurrentsDirection:
  CYC(0x6349, 0x634c); A = mem_rd(gb, wTilesetFlags);
  CYC(0x634c, 0x634e); alu_and(gb, 0x01); // TILESETFLAG_OUTDOORS
  if (F & FZ) { CYCT(0x634e, 0x6350); goto setCounter1To20; } // jr z
  CYC(0x634e, 0x6350);
  CALL_C(0x6350, objectGetTileAtPosition_hook, 0x1444, 0x6353);
  CYC(0x6353, 0x6356); SET_HL(0x6437); // currentsCollisionTable
  CALL_C(0x6356, lookupCollisionTable_hook, 0x1e1f, 0x6359);
  if (!(F & FC)) { CYCT(0x6359, 0x635b); goto setCounter1To20; } // jr nc
  CYC(0x6359, 0x635b);
  CYC(0x635b, 0x635c); C = A;
  CYC(0x635c, 0x635e); B = 0x3c;
  CALL_C(0x635e, updateLinkPositionGivenVelocity_hook, 0x231e, 0x6361);

setCounter1To20:
  CYC(0x6361, 0x6363); E = 0xc6; // Part.counter1
  CYC(0x6363, 0x6365); A = 0x20;
  CYC(0x6365, 0x6366); mem_wr(gb, DE, A);
  RET(0x6366); return; // ret

collision0:
  CYC(0x6367, 0x6369); A = mem_rd(gb, 0xff8a);
  CALL_C(0x6369, convertShortToLongPosition_hook, 0x20cb, 0x636c);
  CYC(0x636c, 0x636e); E = 0xcb; // Part.yh
  CYC(0x636e, 0x636f); A = mem_rd(gb, DE);
  CYC(0x636f, 0x6371); mem_wr(gb, 0xff8f, A);
  CYC(0x6371, 0x6373); E = 0xcd; // Part.xh
  CYC(0x6373, 0x6374); A = mem_rd(gb, DE);
  CYC(0x6374, 0x6376); mem_wr(gb, 0xff8e, A);
  CALL_C(0x6376, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x6379);
  CYC(0x6379, 0x637b); alu_xor(gb, 0x10);
  CYC(0x637b, 0x637c); B = A;
  CYC(0x637c, 0x637f); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x637f, 0x6380); H = A;
  CYC(0x6380, 0x6382); L = 0x2b;
  CYC(0x6382, 0x6383); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x6383, 0x6384); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(0x6384); return; } // ret nz
  CYC(0x6384, 0x6385);
  CYC(0x6385, 0x6387); mem_wr(gb, HL, 0x01);
  CYC(0x6387, 0x6388); L = alu_inc8(gb, L);
  CYC(0x6388, 0x638a); mem_wr(gb, HL, 0x18);
  CYC(0x638a, 0x638b); L = alu_inc8(gb, L);
  CYC(0x638b, 0x638c); mem_wr(gb, HL, B);
  CYC(0x638c, 0x638d); L = alu_inc8(gb, L);
  CYC(0x638d, 0x638f); mem_wr(gb, HL, 0x0c);
  CYC(0x638f, 0x6391); L = 0x25;
  CYC(0x6391, 0x6393); mem_wr(gb, HL, 0xfe);
  CYC(0x6393, 0x6395); A = 0x5f; // SND_DAMAGE_LINK
  CYC(0x6395, 0x6398); playSound_b00_hook(gb); return; // jp

collision1:
  CYC(0x6398, 0x639b); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x639b, 0x639c); alu_rrca(gb);
  if (F & FC) { CYCT(0x639c, 0x639e); goto collision2; } // jr c
  CYC(0x639c, 0x639e);
  CYC(0x639e, 0x63a1); A = mem_rd(gb, wLinkSwimmingState);
  CYC(0x63a1, 0x63a2); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x63a2); return; } // ret z
  CYC(0x63a2, 0x63a3);

collision2:
  CYC(0x63a3, 0x63a5); A = mem_rd(gb, 0xff8a);
  CALL_C(0x63a5, convertShortToLongPosition_hook, 0x20cb, 0x63a8);
  CYC(0x63a8, 0x63aa); E = 0xcb; // Part.yh
  CYC(0x63aa, 0x63ab); A = mem_rd(gb, DE);
  CYC(0x63ab, 0x63ad); mem_wr(gb, 0xff8f, A);
  CYC(0x63ad, 0x63af); E = 0xcd; // Part.xh
  CYC(0x63af, 0x63b0); A = mem_rd(gb, DE);
  CYC(0x63b0, 0x63b2); mem_wr(gb, 0xff8e, A);
  CYC(0x63b2, 0x63b3); alu_sub(gb, C);
  CYC(0x63b3, 0x63b4); A = alu_inc8(gb, A);
  CYC(0x63b4, 0x63b6); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x63b6, 0x63b8); goto func_63d6; } // jr nc
  CYC(0x63b6, 0x63b8);
  CYC(0x63b8, 0x63ba); A = mem_rd(gb, 0xff8f);
  CYC(0x63ba, 0x63bb); alu_sub(gb, B);
  CYC(0x63bb, 0x63bc); A = alu_inc8(gb, A);
  CYC(0x63bc, 0x63be); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x63be, 0x63c0); goto func_63d6; } // jr nc
  CYC(0x63be, 0x63c0);
  CYC(0x63c0, 0x63c3); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x63c3, 0x63c4); H = A;
  CYC(0x63c4, 0x63c6); L = 0x0b;
  CYC(0x63c6, 0x63c7); mem_wr(gb, HL, B);
  CYC(0x63c7, 0x63c9); L = 0x0d;
  CYC(0x63c9, 0x63ca); mem_wr(gb, HL, C);
  CYC(0x63ca, 0x63cc); A = 0x02;
  CYC(0x63cc, 0x63cf); mem_wr(gb, wLinkForceState, A);
  CYC(0x63cf, 0x63d0); alu_xor(gb, A);
  CYC(0x63d0, 0x63d3); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x63d3, 0x63d6); clearAllParentItems_hook(gb); return; // jp

func_63d6:
  CYC(0x63d6, 0x63d8); A = 0xff;
  CYC(0x63d8, 0x63db); mem_wr(gb, wDisableScreenTransitions, A);
  CALL_C(0x63db, objectGetRelativeAngleWithTempVars_hook, 0x1eb1, 0x63de);
  CYC(0x63de, 0x63df); C = A;
  CALL_C(0x63df, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x63e2);
  CYC(0x63e2, 0x63e3); A = mem_rd(gb, HL);
  CYC(0x63e3, 0x63e5); alu_and(gb, 0x1c);
  CYC(0x63e5, 0x63e6); alu_rrca(gb);
  CYC(0x63e6, 0x63e7); alu_rrca(gb);
  CYC(0x63e7, 0x63ea); SET_HL(0x6410); // @speedValues
  CYC(0x63ea, 0x63eb); seaEffects_addAToHl_from_rst(gb, 0x63eb);
  CYC(0x63eb, 0x63ec); A = mem_rd(gb, HL);
  CYC(0x63ec, 0x63ed); B = A;
  CYC(0x63ed, 0x63ef); alu_cp(gb, 0x19);
  if (F & FC) { CYCT(0x63ef, 0x63f1); goto applyVelocity; } // jr c
  CYC(0x63ef, 0x63f1);
  CYC(0x63f1, 0x63f4); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x63f4, 0x63f5); H = A;
  CYC(0x63f5, 0x63f7); L = 0x10; // Object.speed
  CYC(0x63f7, 0x63f9); mem_wr(gb, HL, 0x00);

applyVelocity:
  CYC(0x63f9, 0x63fc); updateLinkPositionGivenVelocity_hook(gb); return; // jp
}
