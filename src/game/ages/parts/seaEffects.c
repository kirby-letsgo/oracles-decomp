#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode2e), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode2e), (from), (to), true)

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
  BASE(partCode2e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto initialized; } // jr nz
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+7); A = alu_inc8(gb, A);
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CYC(b_+8, b_+11); goto setCounter1To20; // jp

initialized:
  CYC(b_+11, b_+14); A = mem_rd(gb, w1Link_state);
  CYC(b_+14, b_+16); alu_sub(gb, 0x01); // LINK_STATE_NORMAL
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; } // ret nz
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+20); mem_wr(gb, wDisableScreenTransitions, A);
  CALL_C(b_+20, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+23);
  if (F & FC) { CYCT(b_+23, b_+25); goto afterSwimCheck; } // jr c
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+28); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+28, b_+29); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+29); return; } // ret nc
  CYC(b_+29, b_+30);

afterSwimCheck:
  CYC(b_+30, b_+33); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+33, b_+34); H = A;
  CYC(b_+34, b_+36); L = 0x00; // SpecialObject.start
  CALL_C(b_+36, objectTakePosition_hook, SYM(objectTakePosition), b_+39);
  CYC(b_+39, b_+41); L = 0x01; // SpecialObject.id
  CYC(b_+41, b_+42); A = mem_rd(gb, HL);
  CYC(b_+42, b_+44); alu_cp(gb, 0x13); // SPECIALOBJECT_RAFT
  CYC(b_+44, b_+46); A = 0x05;
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto notRaft; } // jr nz
  CYC(b_+46, b_+48);
  CYC(b_+48, b_+49); alu_add(gb, A);

notRaft:
  CYC(b_+49, b_+50); H = D;
  CYC(b_+50, b_+52); L = 0xc7; // Part.counter2
  CYC(b_+52, b_+53); mem_wr(gb, HL, A);
  CYC(b_+53, b_+55); L = 0xcb; // Part.yh
  CYC(b_+55, b_+56); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+56, b_+57); B = A;
  CYC(b_+57, b_+58); L = alu_inc8(gb, L);
  CYC(b_+58, b_+59); C = mem_rd(gb, HL);
  CYC(b_+59, b_+62); SET_HL(hFF8B);
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x06);

waterLoop:
  CYC(b_+64, b_+67); SET_HL(hFF8B);
  CYC(b_+67, b_+68); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+68, b_+70); goto applyCurrentsDirection; } // jr z
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+71); H = D;
  CYC(b_+71, b_+73); L = 0xc7; // Part.counter2
  CYC(b_+73, b_+74); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  CYC(b_+74, b_+75); A = mem_rd(gb, HL);
  CYC(b_+75, b_+78); SET_HL(b_+286); // @positionOffsets
  CYC(b_+78, b_+79); seaEffects_addDoubleIndexToHl_from_rst(gb, b_+79);
  CYC(b_+79, b_+80); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+80, b_+81); alu_add(gb, B);
  CYC(b_+81, b_+82); B = A;
  CYC(b_+82, b_+83); A = mem_rd(gb, HL);
  CYC(b_+83, b_+84); alu_add(gb, C);
  CYC(b_+84, b_+85); C = A;
  CALL_C(b_+85, getTileAtPosition_hook, SYM(getTileAtPosition), b_+88);
  CYC(b_+88, b_+89); E = A;
  CYC(b_+89, b_+90); A = L;
  CYC(b_+90, b_+92); mem_wr(gb, hFF8A, A);
  CYC(b_+92, b_+95); SET_HL(SYM(harmfulWaterTilesCollisionTable)); // harmfulWaterTilesCollisionTable
  CALL_C(b_+95, lookupCollisionTable_paramE_hook, SYM(lookupCollisionTable_paramE), b_+98);
  if (!(F & FC)) { CYCT(b_+98, b_+100); goto waterLoop; } // jr nc
  CYC(b_+98, b_+100);
  {
    CYC(b_+100, b_+101); push_effect(gb, b_+101);
    uint16_t target = seaEffects_jump_table(gb);
    if (target == b_+137) goto collision0;
    if (target == b_+186) goto collision1;
    goto collision2;
  }

applyCurrentsDirection:
  CYC(b_+107, b_+110); A = mem_rd(gb, wTilesetFlags);
  CYC(b_+110, b_+112); alu_and(gb, 0x01); // TILESETFLAG_OUTDOORS
  if (F & FZ) { CYCT(b_+112, b_+114); goto setCounter1To20; } // jr z
  CYC(b_+112, b_+114);
  CALL_C(b_+114, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+117);
  CYC(b_+117, b_+120); SET_HL(SYM(currentsCollisionTable)); // currentsCollisionTable
  CALL_C(b_+120, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+123);
  if (!(F & FC)) { CYCT(b_+123, b_+125); goto setCounter1To20; } // jr nc
  CYC(b_+123, b_+125);
  CYC(b_+125, b_+126); C = A;
  CYC(b_+126, b_+128); B = 0x3c;
  CALL_C(b_+128, updateLinkPositionGivenVelocity_hook, SYM(updateLinkPositionGivenVelocity), b_+131);

setCounter1To20:
  CYC(b_+131, b_+133); E = 0xc6; // Part.counter1
  CYC(b_+133, b_+135); A = 0x20;
  CYC(b_+135, b_+136); mem_wr(gb, DE, A);
  RET(b_+136); return; // ret

collision0:
  CYC(b_+137, b_+139); A = mem_rd(gb, hFF8A);
  CALL_C(b_+139, convertShortToLongPosition_hook, SYM(convertShortToLongPosition), b_+142);
  CYC(b_+142, b_+144); E = 0xcb; // Part.yh
  CYC(b_+144, b_+145); A = mem_rd(gb, DE);
  CYC(b_+145, b_+147); mem_wr(gb, hFF8F, A);
  CYC(b_+147, b_+149); E = 0xcd; // Part.xh
  CYC(b_+149, b_+150); A = mem_rd(gb, DE);
  CYC(b_+150, b_+152); mem_wr(gb, hFF8E, A);
  CALL_C(b_+152, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+155);
  CYC(b_+155, b_+157); alu_xor(gb, 0x10);
  CYC(b_+157, b_+158); B = A;
  CYC(b_+158, b_+161); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+161, b_+162); H = A;
  CYC(b_+162, b_+164); L = 0x2b;
  CYC(b_+164, b_+165); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+165, b_+166); alu_or(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+166); return; } // ret nz
  CYC(b_+166, b_+167);
  CYC(b_+167, b_+169); mem_wr(gb, HL, 0x01);
  CYC(b_+169, b_+170); L = alu_inc8(gb, L);
  CYC(b_+170, b_+172); mem_wr(gb, HL, 0x18);
  CYC(b_+172, b_+173); L = alu_inc8(gb, L);
  CYC(b_+173, b_+174); mem_wr(gb, HL, B);
  CYC(b_+174, b_+175); L = alu_inc8(gb, L);
  CYC(b_+175, b_+177); mem_wr(gb, HL, 0x0c);
  CYC(b_+177, b_+179); L = 0x25;
  CYC(b_+179, b_+181); mem_wr(gb, HL, 0xfe);
  CYC(b_+181, b_+183); A = 0x5f; // SND_DAMAGE_LINK
  CYC(b_+183, b_+186); playSound_b00_hook(gb); return; // jp

collision1:
  CYC(b_+186, b_+189); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+189, b_+190); alu_rrca(gb);
  if (F & FC) { CYCT(b_+190, b_+192); goto collision2; } // jr c
  CYC(b_+190, b_+192);
  CYC(b_+192, b_+195); A = mem_rd(gb, wLinkSwimmingState);
  CYC(b_+195, b_+196); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+196); return; } // ret z
  CYC(b_+196, b_+197);

collision2:
  CYC(b_+197, b_+199); A = mem_rd(gb, hFF8A);
  CALL_C(b_+199, convertShortToLongPosition_hook, SYM(convertShortToLongPosition), b_+202);
  CYC(b_+202, b_+204); E = 0xcb; // Part.yh
  CYC(b_+204, b_+205); A = mem_rd(gb, DE);
  CYC(b_+205, b_+207); mem_wr(gb, hFF8F, A);
  CYC(b_+207, b_+209); E = 0xcd; // Part.xh
  CYC(b_+209, b_+210); A = mem_rd(gb, DE);
  CYC(b_+210, b_+212); mem_wr(gb, hFF8E, A);
  CYC(b_+212, b_+213); alu_sub(gb, C);
  CYC(b_+213, b_+214); A = alu_inc8(gb, A);
  CYC(b_+214, b_+216); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+216, b_+218); goto func_63d6; } // jr nc
  CYC(b_+216, b_+218);
  CYC(b_+218, b_+220); A = mem_rd(gb, hFF8F);
  CYC(b_+220, b_+221); alu_sub(gb, B);
  CYC(b_+221, b_+222); A = alu_inc8(gb, A);
  CYC(b_+222, b_+224); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(b_+224, b_+226); goto func_63d6; } // jr nc
  CYC(b_+224, b_+226);
  CYC(b_+226, b_+229); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+229, b_+230); H = A;
  CYC(b_+230, b_+232); L = 0x0b;
  CYC(b_+232, b_+233); mem_wr(gb, HL, B);
  CYC(b_+233, b_+235); L = 0x0d;
  CYC(b_+235, b_+236); mem_wr(gb, HL, C);
  CYC(b_+236, b_+238); A = 0x02;
  CYC(b_+238, b_+241); mem_wr(gb, wLinkForceState, A);
  CYC(b_+241, b_+242); alu_xor(gb, A);
  CYC(b_+242, b_+245); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+245, b_+248); clearAllParentItems_hook(gb); return; // jp

func_63d6:
  CYC(b_+248, b_+250); A = 0xff;
  CYC(b_+250, b_+253); mem_wr(gb, wDisableScreenTransitions, A);
  CALL_C(b_+253, objectGetRelativeAngleWithTempVars_hook, SYM(objectGetRelativeAngleWithTempVars), b_+256);
  CYC(b_+256, b_+257); C = A;
  CALL_C(b_+257, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+260);
  CYC(b_+260, b_+261); A = mem_rd(gb, HL);
  CYC(b_+261, b_+263); alu_and(gb, 0x1c);
  CYC(b_+263, b_+264); alu_rrca(gb);
  CYC(b_+264, b_+265); alu_rrca(gb);
  CYC(b_+265, b_+268); SET_HL(b_+306); // @speedValues
  CYC(b_+268, b_+269); seaEffects_addAToHl_from_rst(gb, b_+269);
  CYC(b_+269, b_+270); A = mem_rd(gb, HL);
  CYC(b_+270, b_+271); B = A;
  CYC(b_+271, b_+273); alu_cp(gb, 0x19);
  if (F & FC) { CYCT(b_+273, b_+275); goto applyVelocity; } // jr c
  CYC(b_+273, b_+275);
  CYC(b_+275, b_+278); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+278, b_+279); H = A;
  CYC(b_+279, b_+281); L = 0x10; // Object.speed
  CYC(b_+281, b_+283); mem_wr(gb, HL, 0x00);

applyVelocity:
  CYC(b_+283, b_+286); updateLinkPositionGivenVelocity_hook(gb); return; // jp
}
