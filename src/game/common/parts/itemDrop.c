#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// object_code/common/parts/itemDrop.s (PART_ITEM_DROP). Part.* fields are read/written through
// D/E or H/L exactly as the ROM does; D already holds the active part's WRAM page (set by the
// part dispatcher before calling in), so only the low-byte field offset needs to appear here.

static uint16_t itemDrop_jump_table(GB *gb) {
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

static void itemDrop_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void itemDrop_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode01_hook(GB *gb);
void itemDrop_initGfx_hook(GB *gb);
void itemDrop_countdownToDisappear_hook(GB *gb);
void itemDrop_initSpeed_hook(GB *gb);
void itemDrop_updateSpeed_hook(GB *gb);
void itemDrop_spawnEnemy_hook(GB *gb);
void itemDrop_checkSidescrollingConditions_hook(GB *gb);
void itemDrop_checkHitGround_hook(GB *gb);
void itemDrop_checkOnHazard_hook(GB *gb);
void itemDrop_updateFairyMovement_hook(GB *gb);
void itemDrop_chooseRandomFairyMovement_hook(GB *gb);
void itemDrop_moveTowardPoint_hook(GB *gb);
void itemDrop_applySpeed_hook(GB *gb);

void partCode01__linkCollectedItem_hook(GB *gb);
void partCode01__getRelatedObj1ID_hook(GB *gb);
void partCode01__checkCollidedWithLink_hook(GB *gb);
void partCode01__afterCollisionCheck_hook(GB *gb);

void partCode01_hook(GB *gb) {
  BASE(partCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) {
    CYCT(b_+0, b_+2); // jr z,@normalStatus
  } else {
    CYC(b_+0, b_+2);
    CYC(b_+2, b_+4); alu_cp(gb, 0x02); // cp PARTSTATUS_DEAD
    if (F & FZ) {
      CYCT(b_+4, b_+7); // jp z,@linkCollectedItem
      TAIL(partCode01__linkCollectedItem);
    }
    CYC(b_+4, b_+7);
    CYC(b_+7, b_+9); E = 0xc4; // Part.state
    CYC(b_+9, b_+11); A = 0x03;
    CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  }

  // @normalStatus
  CYC(b_+12, b_+15); push_effect(gb, b_+15);
  partCode01__checkCollidedWithLink_hook(gb);
}

void partCode01__afterCollisionCheck_hook(GB *gb) {
  BASE(partCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+15, b_+17); E = 0xc4; // Part.state
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  {
    CYC(b_+18, b_+19); push_effect(gb, b_+19);
    uint16_t target = itemDrop_jump_table(gb);
    if (target == b_+27) goto state0;
    if (target == b_+104) goto state1;
    if (target == b_+161) goto state2;
    goto state3;
  }

state0:
  CYC(b_+27, b_+30); A = W8(wIsMaplePresent);
  CYC(b_+30, b_+31); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+31, b_+34); partDelete_hook(gb); return; } // jp nz
  CYC(b_+31, b_+34);
  CYC(b_+34, b_+36); E = 0xc2; // Part.subid
  CYC(b_+36, b_+37); A = mem_rd(gb, DE);
  CYC(b_+37, b_+39); alu_cp(gb, 0x0f); // ITEM_DROP_100_RUPEES_OR_ENEMY
  if (!(F & FZ)) {
    CYCT(b_+39, b_+41); // jr nz,@normalItem
  } else {
    CYC(b_+39, b_+41);
    CALL_C(b_+41, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+44);
    CYC(b_+44, b_+46); alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(b_+46, b_+49); itemDrop_spawnEnemy_hook(gb); return; } // jp c
    CYC(b_+46, b_+49);
  }

  // @normalItem
  CALL_C(b_+49, itemDrop_initGfx_hook, SYM(itemDrop_initGfx), b_+52);
  CYC(b_+52, b_+53); H = D;
  CYC(b_+53, b_+55); L = 0xd4; // Part.speedZ
  CYC(b_+55, b_+57); A = 0xa0; // <(-$160)
  CYC(b_+57, b_+58); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+58, b_+60); mem_wr(gb, HL, 0xfe); // >(-$160)
  CYC(b_+60, b_+62); L = 0xc4; // Part.state
  CYC(b_+62, b_+63); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+63, b_+66); A = W8(wTilesetFlags);
  CYC(b_+66, b_+68); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (F & FZ) {
    CYCT(b_+68, b_+70); // jr z,@label_11_008
    goto label_11_008;
  }
  CYC(b_+68, b_+70);
  CYC(b_+70, b_+71); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 2
  CYC(b_+71, b_+73); L = 0xe4; // Part.collisionType
  CYC(b_+73, b_+75); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+75, b_+77); L = 0xc6; // Part.counter1
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0xf0);
  CALL_C(b_+79, objectCheckIsOnHazard_hook, SYM(objectCheckIsOnHazard), b_+82);
  if (!(F & FC)) { CYCT(b_+82, b_+84); goto label_11_008; } // jr nc
  CYC(b_+82, b_+84);
  CYC(b_+84, b_+85); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+85, b_+87); goto label_11_008; } // jr nc
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+89); E = 0xf4; // Part.var34
  CYC(b_+89, b_+91); A = 0x01;
  CYC(b_+91, b_+92); mem_wr(gb, DE, A);

label_11_008:
  CYC(b_+92, b_+94); E = 0xc2; // Part.subid
  CYC(b_+94, b_+95); A = mem_rd(gb, DE);
  CALL_C(b_+95, itemDrop_initSpeed_hook, SYM(itemDrop_initSpeed), b_+98);
  CYC(b_+98, b_+100); E = 0xc2; // Part.subid
  CYC(b_+100, b_+101); A = mem_rd(gb, DE);
  CYC(b_+101, b_+104); TAIL(partSetAnimation); // jp

state1:
  CALL_C(b_+104, partCommon_getTileCollisionInFront_allowHoles_hook, SYM(partCommon_getTileCollisionInFront_allowHoles), b_+107);
  if (!(F & FC)) {
    CALL_C_CC(b_+107, itemDrop_updateSpeed_hook, SYM(itemDrop_updateSpeed), b_+110); // call nc
  } else {
    CYC(b_+107, b_+110);
  }
  CYC(b_+110, b_+112); C = 0x20;
  CALL_C(b_+112, objectUpdateSpeedZAndBounce_hook, SYM(objectUpdateSpeedZAndBounce), b_+115);
  if (F & FC) { CYCT(b_+115, b_+117); goto doneBouncing; } // jr c
  CYC(b_+115, b_+117);
  CALL_C(b_+117, itemDrop_checkHitGround_hook, SYM(itemDrop_checkHitGround), b_+120);
  if (!(F & FC)) { CYCT(b_+120, b_+122); goto label_11_010; } // jr nc
  CYC(b_+120, b_+122);

doneBouncing:
  CYC(b_+122, b_+123); H = D;
  CYC(b_+123, b_+125); L = 0xc4; // Part.state
  CYC(b_+125, b_+126); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+126, b_+128); L = 0xc6; // Part.counter1
  CYC(b_+128, b_+130); mem_wr(gb, HL, 0xf0);
  CALL_C(b_+130, objectSetVisiblec3_hook, SYM(objectSetVisiblec3), b_+133);

label_11_010:
  CALL_C(b_+133, itemDrop_checkOnHazard_hook, SYM(itemDrop_checkOnHazard), b_+136);
  if (F & FC) { RET_TAKEN(b_+136); return; } // ret c
  CYC(b_+136, b_+137);
  CYC(b_+137, b_+139); E = 0xcf; // Part.zh
  CYC(b_+139, b_+140); A = mem_rd(gb, DE);
  CYC(b_+140, b_+141); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+141); return; } // ret c
  CYC(b_+141, b_+142);
  CYC(b_+142, b_+145); SET_BC(0x0500);
  CALL_C(b_+145, objectGetRelativeTile_hook, SYM(objectGetRelativeTile), b_+148);
  CYC(b_+148, b_+151); SET_HL(SYM(itemDropConveyorTilesTable)); // itemDropConveyorTilesTable
  CALL_C(b_+151, lookupCollisionTable_hook, SYM(lookupCollisionTable), b_+154);
  if (!(F & FC)) { RET_TAKEN(b_+154); return; } // ret nc
  CYC(b_+154, b_+155);
  CYC(b_+155, b_+156); C = A;
  CYC(b_+156, b_+158); B = 0x14; // SPEED_80
  CYC(b_+158, b_+161); TAIL(itemDrop_applySpeed); // jp

state2:
  CALL_C(b_+161, itemDrop_checkSidescrollingConditions_hook, SYM(itemDrop_checkSidescrollingConditions), b_+164);
  CALL_C(b_+164, itemDrop_moveTowardPoint_hook, SYM(itemDrop_moveTowardPoint), b_+167);
  if (F & FC) { CYCT(b_+167, b_+170); goto reachedPoint; } // jp c
  CYC(b_+167, b_+170);
  CALL_C(b_+170, itemDrop_countdownToDisappear_hook, SYM(itemDrop_countdownToDisappear), b_+173);
  if (F & FC) { CYCT(b_+173, b_+176); partDelete_hook(gb); return; } // jp c
  CYC(b_+173, b_+176);
  CYC(b_+176, b_+178); E = 0xc2; // Part.subid
  CYC(b_+178, b_+179); A = mem_rd(gb, DE);
  CYC(b_+179, b_+180); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+180, b_+182); goto label_11_010; } // jr nz
  CYC(b_+180, b_+182);
  CYC(b_+182, b_+185); TAIL(itemDrop_updateFairyMovement); // jp

reachedPoint:
  CYC(b_+185, b_+186); H = D;
  CYC(b_+186, b_+188); L = 0xf1; // Part.var31
  CYC(b_+188, b_+189); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+189, b_+190); C = mem_rd(gb, HL); // Part.var32
  CYC(b_+190, b_+192); L = 0xcb; // Part.yh
  mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+192, b_+193);
  CYC(b_+193, b_+194); L = L + 1; // Part.xh
  CYC(b_+194, b_+195); mem_wr(gb, HL, C);
  CYC(b_+195, b_+198); TAIL(partDelete); // jp

state3:
  CYC(b_+198, b_+200); E = 0xc5; // Part.substate
  CYC(b_+200, b_+201); A = mem_rd(gb, DE);
  CYC(b_+201, b_+202); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+202, b_+205); // call z
    push_effect(gb, b_+205);
    partCode01__getRelatedObj1ID_hook(gb);
  } else {
    CYC(b_+202, b_+205);
  }
  CALL_C(b_+205, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+208);
  if (F & FC) { CYCT(b_+208, b_+211); partCode01__linkCollectedItem_hook(gb); return; } // jp c
  CYC(b_+208, b_+211);
  CYC(b_+211, b_+213); A = 0x00; // Object.enabled
  CALL_C(b_+213, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+216);
  CYC(b_+216, b_+217); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+217, b_+218); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+218, b_+220); goto label_11_006; } // jr z
  CYC(b_+218, b_+220);
  CYC(b_+220, b_+222); E = 0xf0; // Part.var30
  CYC(b_+222, b_+223); A = mem_rd(gb, DE);
  CYC(b_+223, b_+224); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+224, b_+227); objectTakePosition_hook(gb); return; } // jp z
  CYC(b_+224, b_+227);

label_11_006:
  CYC(b_+227, b_+230); TAIL(partDelete); // jp
}

void partCode01__getRelatedObj1ID_hook(GB *gb) {
  BASE(partCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+230, b_+231); H = D;
  CYC(b_+231, b_+232); L = E;
  CYC(b_+232, b_+233); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(b_+233, b_+235); L = 0xcf; // Part.zh
  CYC(b_+235, b_+237); mem_wr(gb, HL, 0x00);
  CYC(b_+237, b_+239); A = 0x01; // Object.id
  CALL_C(b_+239, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+242);
  CYC(b_+242, b_+243); A = mem_rd(gb, HL);
  CYC(b_+243, b_+245); E = 0xf0; // Part.var30
  CYC(b_+245, b_+246); mem_wr(gb, DE, A);
  CYC(b_+246, b_+249); objectSetVisible80_hook(gb); // jp, real ret happens inside its chain
}

void partCode01__checkCollidedWithLink_hook(GB *gb) {
  BASE(partCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+249, b_+251); E = 0xe4; // Part.collisionType
  CYC(b_+251, b_+252); A = mem_rd(gb, DE);
  CYC(b_+252, b_+253); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+253); partCode01__afterCollisionCheck_hook(gb); return; } // ret nc
  CYC(b_+253, b_+254);
  CALL_C(b_+254, objectCheckCollidedWithLink_hook, SYM(objectCheckCollidedWithLink), b_+257);
  if (!(F & FC)) { RET_TAKEN(b_+257); partCode01__afterCollisionCheck_hook(gb); return; } // ret nc
  CYC(b_+257, b_+258);
  CYC(b_+258, b_+259); SET_HL(pop_effect(gb)); // pop hl (discard return address)
  partCode01__linkCollectedItem_hook(gb);
}

void partCode01__linkCollectedItem_hook(GB *gb) {
  BASE(partCode01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+259, b_+262); A = W8(wLinkDeathTrigger);
  CYC(b_+262, b_+263); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+263, b_+265); goto deleteSelf; } // jr nz
  CYC(b_+263, b_+265);
  CYC(b_+265, b_+267); E = 0xc2; // Part.subid
  CYC(b_+267, b_+268); A = mem_rd(gb, DE);
  CYC(b_+268, b_+269); alu_add(gb, A);
  CYC(b_+269, b_+272); SET_HL(b_+315); // @itemDropTreasureTable
  CYC(b_+272, b_+273); itemDrop_addDoubleIndexToHl_from_rst(gb, b_+273);
  CYC(b_+273, b_+274); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+274, b_+275); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+275, b_+277); goto deleteSelf; } // jr z
  CYC(b_+275, b_+277);
  CYC(b_+277, b_+278); B = A;
  CYC(b_+278, b_+280); A = 0x26; // GOLD_JOY_RING
  CALL_C(b_+280, cpActiveRing_hook, SYM(cpActiveRing), b_+283);
  CYC(b_+283, b_+284); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (F & FZ) { CYCT(b_+284, b_+286); goto doubleDrop; } // jr z
  CYC(b_+284, b_+286);
  CYC(b_+286, b_+287); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+287, b_+289); goto giveDrop; } // jr z
  CYC(b_+287, b_+289);
  CALL_C(b_+289, cpActiveRing_hook, SYM(cpActiveRing), b_+292);
  if (!(F & FZ)) { CYCT(b_+292, b_+294); goto giveDrop; } // jr nz
  CYC(b_+292, b_+294);

doubleDrop:
  CYC(b_+294, b_+295); SET_HL(HL + 1);

giveDrop:
  CYC(b_+295, b_+296); C = mem_rd(gb, HL);
  CYC(b_+296, b_+297); A = B;
  CALL_C(b_+297, giveTreasure_hook, SYM(giveTreasure), b_+300);
  CYC(b_+300, b_+302); E = 0xc2; // Part.subid
  CYC(b_+302, b_+303); A = mem_rd(gb, DE);
  CYC(b_+303, b_+305); alu_cp(gb, 0x0e); // ITEM_DROP_50_ORE_CHUNKS
  if (!(F & FZ)) { CYCT(b_+305, b_+307); goto deleteSelf; } // jr nz
  CYC(b_+305, b_+307);
  CALL_C(b_+307, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+310);
  CYC(b_+310, b_+312); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);

deleteSelf:
  CYC(b_+312, b_+315); TAIL(partDelete); // jp
}

void itemDrop_initGfx_hook(GB *gb) {
  BASE(itemDrop_initGfx);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+21); // @spriteData
  CYC(b_+6, b_+7); itemDrop_addDoubleIndexToHl_from_rst(gb, b_+7);
  CYC(b_+7, b_+9); E = 0xdd; // Part.oamTileIndexBase
  CYC(b_+9, b_+10); A = mem_rd(gb, DE);
  CYC(b_+10, b_+11); A = A + mem_rd(gb, HL);
  CYC(b_+11, b_+12); mem_wr(gb, DE, A);
  CYC(b_+12, b_+13); SET_HL(HL + 1);
  CYC(b_+13, b_+14); E = 0xdc; // Part.oamFlags
  CYC(b_+14, b_+15); A = mem_rd(gb, HL);
  CYC(b_+15, b_+16); mem_wr(gb, DE, A); // [oamFlags]
  CYC(b_+16, b_+17); E = 0xdb; // Part.oamFlagsBackup
  CYC(b_+17, b_+18); mem_wr(gb, DE, A); // [oamFlagsBackup]
  CYC(b_+18, b_+21); TAIL(objectSetVisiblec1); // jp
}

void itemDrop_countdownToDisappear_hook(GB *gb) {
  BASE(itemDrop_countdownToDisappear);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wFrameCounter);
  CYC(b_+3, b_+4); A = A ^ D;
  CYC(b_+4, b_+5); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; } // ret nc
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = 0xf3; // Part.var33
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+11, b_+13); // jr z,L_42d6
  } else {
    CYC(b_+11, b_+13);
    CYC(b_+13, b_+14); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (!(F & FZ)) { RET_TAKEN(b_+14); return; } // ret nz
    CYC(b_+14, b_+15);
    CYC(b_+15, b_+17); L = 0xe4; // Part.collisionType
    CYC(b_+17, b_+19); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  }
  CALL_C(b_+19, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+22);
  if (F & FZ) { CYCT(b_+22, b_+24); goto disappear; } // jr z
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); alu_cp(gb, 60);
  if (!(F & FC)) { RET_TAKEN(b_+27); return; } // ret nc
  CYC(b_+27, b_+28);
  CYC(b_+28, b_+30); L = 0xda; // Part.visible
  CYC(b_+30, b_+31); A = mem_rd(gb, HL);
  CYC(b_+31, b_+33); A = A ^ 0x80;
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+35); ret_effect(gb); return;

disappear:
  CYC(b_+35, b_+36); alu_scf(gb);
  CYC(b_+36, b_+37); ret_effect(gb); return;
}

void itemDrop_initSpeed_hook(GB *gb) {
  BASE(itemDrop_initSpeed);
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+2, b_+4); goto fairy; } // jr z
  CYC(b_+2, b_+4);
  CYC(b_+4, b_+6); E = 0xc3; // Part.var03
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+8); return; } // ret nc
  CYC(b_+8, b_+9);
  CYC(b_+9, b_+11); L = 0xd0; // Part.speed
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x19); // SPEED_a0
  CYC(b_+13, b_+14); ret_effect(gb); return;

fairy:
  CYC(b_+14, b_+16); L = 0xcf; // Part.zh
  CYC(b_+16, b_+17); A = mem_rd(gb, HL);
  CYC(b_+17, b_+19); mem_wr(gb, HL, 0x00);
  CYC(b_+19, b_+21); L = 0xcb; // Part.yh
  CYC(b_+21, b_+22); A = A + mem_rd(gb, HL);
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+26); TAIL(itemDrop_chooseRandomFairyMovement); // jp
}

void itemDrop_updateSpeed_hook(GB *gb) {
  BASE(itemDrop_updateSpeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCheckTileCollision_allowHoles_hook, SYM(objectCheckTileCollision_allowHoles), b_+3);
  if (F & FC) { RET_TAKEN(b_+3); return; } // ret c
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); TAIL(objectApplySpeed); // jp
}

void itemDrop_spawnEnemy_hook(GB *gb) {
  BASE(itemDrop_spawnEnemy);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); C = A;
  CYC(b_+1, b_+4); A = W8(wDiggingUpEnemiesForbidden);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto delete_; } // jr nz
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); A = C;
  CYC(b_+8, b_+10); alu_and(gb, 0x07);
  CYC(b_+10, b_+13); SET_HL(b_+33); // @enemiesToSpawn
  CYC(b_+13, b_+14); itemDrop_addAToHl_from_rst(gb, b_+14);
  CYC(b_+14, b_+15); B = mem_rd(gb, HL);
  CALL_C(b_+15, getFreeEnemySlot_hook, SYM(getFreeEnemySlot), b_+18);
  if (!(F & FZ)) { CYCT(b_+18, b_+20); goto delete_; } // jr nz
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+21); mem_wr(gb, HL, B);
  CALL_C(b_+21, objectCopyPosition_hook, SYM(objectCopyPosition), b_+24);
  CYC(b_+24, b_+26); E = 0xc3; // Part.var03
  CYC(b_+26, b_+27); A = mem_rd(gb, DE);
  CYC(b_+27, b_+29); L = 0x82; // Enemy.subid
  CYC(b_+29, b_+30); mem_wr(gb, HL, A);

delete_:
  CYC(b_+30, b_+33); TAIL(partDelete); // jp
}

void itemDrop_checkSidescrollingConditions_hook(GB *gb) {
  BASE(itemDrop_checkSidescrollingConditions);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = W8(wTilesetFlags);
  CYC(b_+3, b_+5); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (F & FZ) { RET_TAKEN(b_+5); return; } // ret z
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = 0xc2; // Part.subid
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+10); return; } // ret z
  CYC(b_+10, b_+11);
  CYC(b_+11, b_+13); A = 0x20;
  CALL_C(b_+13, objectUpdateSpeedZ_sidescroll_hook, SYM(objectUpdateSpeedZ_sidescroll), b_+16);
  if (F & FC) { CYCT(b_+16, b_+18); goto checkY; } // jr c
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); E = 0xf4; // Part.var34
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+22, b_+24); goto checkY; } // jr nc
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+26); B = 0x01;
  CYC(b_+26, b_+27); A = mem_rd(gb, HL); // [speedZ+1]
  CYC(b_+27, b_+29); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+29, b_+31); goto after_sign; } // jr z
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); B = 0xff;
  CYC(b_+33, b_+34); A = alu_inc8(gb, A);

after_sign:
  CYC(b_+34, b_+36); alu_cp(gb, 0x01);
  if (F & FC) { RET_TAKEN(b_+36); return; } // ret c
  CYC(b_+36, b_+37);
  CYC(b_+37, b_+38); mem_wr(gb, HL, B); // [speedZ+1]
  CYC(b_+38, b_+39); L = L - 1;
  CYC(b_+39, b_+41); mem_wr(gb, HL, 0x00); // [speedZ]

checkY:
  CYC(b_+41, b_+43); E = 0xcb; // Part.yh
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+46); alu_cp(gb, 0xb0);
  if (F & FC) { RET_TAKEN(b_+46); return; } // ret c
  CYC(b_+46, b_+47);
  CYC(b_+47, b_+48); SET_HL(pop_effect(gb)); // pop hl (discard return address)
  CYC(b_+48, b_+51); TAIL(partDelete); // jp
}

void itemDrop_checkHitGround_hook(GB *gb) {
  BASE(itemDrop_checkHitGround);
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+6); goto fairy; } // jr z
  CYC(b_+4, b_+6);
  CYC(b_+6, b_+8); E = 0xd5; // Part.speedZ+1
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+11); alu_and(gb, 0x80);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; } // ret nz
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+13); H = D;
  CYC(b_+13, b_+15); L = 0xe4; // Part.collisionType
  CYC(b_+15, b_+17); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+17, b_+18); ret_effect(gb); return;

fairy:
  CYC(b_+18, b_+20); E = 0xcf; // Part.zh
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+23); alu_cp(gb, 0xfa);
  if (!(F & FC)) { RET_TAKEN(b_+23); return; } // ret nc
  CYC(b_+23, b_+24);
  CYC(b_+24, b_+25); H = D;
  CYC(b_+25, b_+26); L = E;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0xfa); // [Part.zh]
  CYC(b_+28, b_+30); L = 0xf3; // Part.var33
  CYC(b_+30, b_+32); mem_wr(gb, HL, 0x05);
  CYC(b_+32, b_+33); ret_effect(gb); return;
}

void itemDrop_checkOnHazard_hook(GB *gb) {
  BASE(itemDrop_checkOnHazard);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectCheckIsOnHazard_hook, SYM(objectCheckIsOnHazard), b_+3);
  if (F & FC) { CYCT(b_+3, b_+5); goto onHazard; } // jr c
  CYC(b_+3, b_+5);
  CYC(b_+5, b_+7); E = 0xf4; // Part.var34
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+9); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(b_+9); return; } // ret nc
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); B = 0x03; // INTERAC_SPLASH
  CYC(b_+12, b_+13); alu_xor(gb, A);
  CYC(b_+13, b_+15); goto onWaterSidescrolling; // jr (unconditional)

onHazard:
  CYC(b_+15, b_+16); alu_rrca(gb);
  if (F & FC) { CYCT(b_+16, b_+18); goto onWater; } // jr c
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+19); alu_rrca(gb);
  CYC(b_+19, b_+21); B = 0x04; // INTERAC_LAVASPLASH
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto replaceWithAnimation; } // jr nc
  CYC(b_+21, b_+23);
  CALL_C(b_+23, objectCreateFallingDownHoleInteraction_hook, SYM(objectCreateFallingDownHoleInteraction), b_+26);
  CYC(b_+26, b_+28); goto delete_; // jr (unconditional)

replaceWithAnimation:
  CALL_C(b_+28, objectCreateInteractionWithSubid00_hook, SYM(objectCreateInteractionWithSubid00), b_+31);

delete_:
  CALL_C(b_+31, partDelete_hook, SYM(partDelete), b_+34);
  CYC(b_+34, b_+35); alu_scf(gb);
  CYC(b_+35, b_+36); ret_effect(gb); return;

onWater:
  CYC(b_+36, b_+38); B = 0x03; // INTERAC_SPLASH
  CYC(b_+38, b_+41); A = W8(wTilesetFlags);
  CYC(b_+41, b_+43); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (F & FZ) { CYCT(b_+43, b_+45); goto replaceWithAnimation; } // jr z
  CYC(b_+43, b_+45);
  CYC(b_+45, b_+47); E = 0xf4; // Part.var34
  CYC(b_+47, b_+48); A = mem_rd(gb, DE);
  CYC(b_+48, b_+49); alu_rrca(gb);
  CYC(b_+49, b_+50); alu_ccf(gb);
  if (!(F & FC)) { RET_TAKEN(b_+50); return; } // ret nc
  CYC(b_+50, b_+51);
  CYC(b_+51, b_+53); A = 0x01;

onWaterSidescrolling:
  CYC(b_+53, b_+54); mem_wr(gb, DE, A);
  CYC(b_+54, b_+57); TAIL(objectCreateInteractionWithSubid00); // jp
}

void itemDrop_updateFairyMovement_hook(GB *gb) {
  BASE(itemDrop_updateFairyMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0xc7; // Part.counter2
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+4, b_+6); itemDrop_chooseRandomFairyMovement_hook(gb); return; } // jr z
  CYC(b_+4, b_+6);
  CALL_C(b_+6, partCommon_getTileCollisionInFront_hook, SYM(partCommon_getTileCollisionInFront), b_+9);
  CYC(b_+9, b_+10); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+10, b_+13); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(b_+10, b_+13);
  TAIL(itemDrop_chooseRandomFairyMovement); // fallthrough
}

void itemDrop_chooseRandomFairyMovement_hook(GB *gb) {
  BASE(itemDrop_chooseRandomFairyMovement);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+3);
  CYC(b_+3, b_+5); alu_and(gb, 0x3e);
  CYC(b_+5, b_+7); A = A + 0x08;
  CYC(b_+7, b_+9); E = 0xc7; // Part.counter2
  CYC(b_+9, b_+10); mem_wr(gb, DE, A);
  CALL_C(b_+10, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+13);
  CYC(b_+13, b_+15); alu_and(gb, 0x03);
  CYC(b_+15, b_+18); SET_HL(b_+49); // @speedTable
  CYC(b_+18, b_+19); itemDrop_addAToHl_from_rst(gb, b_+19);
  CYC(b_+19, b_+21); E = 0xd0; // Part.speed
  CYC(b_+21, b_+22); A = mem_rd(gb, HL);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+26);
  CYC(b_+26, b_+28); alu_and(gb, 0x1e);
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+31); L = 0xc9; // Part.angle
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+34); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(b_+34); return; } // ret z
  CYC(b_+34, b_+35);
  CYC(b_+35, b_+37); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(b_+37, b_+39); L = 0xdb; // Part.oamFlagsBackup
  CYC(b_+39, b_+40); A = mem_rd(gb, HL);
  CYC(b_+40, b_+42); A = A & ~0x20;
  if (!(F & FZ)) { CYCT(b_+42, b_+44); goto set_flags; } // jr nz
  CYC(b_+42, b_+44);
  CYC(b_+44, b_+46); A = A | 0x20;

set_flags:
  CYC(b_+46, b_+47); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A);
  CYC(b_+48, b_+49); ret_effect(gb); return;
}

void itemDrop_moveTowardPoint_hook(GB *gb) {
  BASE(itemDrop_moveTowardPoint);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); L = 0xf1; // Part.var31
  CYC(b_+2, b_+3); H = D;
  CYC(b_+3, b_+4); alu_xor(gb, A);
  CYC(b_+4, b_+5); B = mem_rd(gb, HL); // [var31]
  mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); C = mem_rd(gb, HL); // [var32]
  mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+9); alu_or(gb, B);
  if (F & FZ) { RET_TAKEN(b_+9); return; } // ret z
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); push_effect(gb, BC);
  CALL_C(b_+11, objectCheckContainsPoint_hook, SYM(objectCheckContainsPoint), b_+14);
  CYC(b_+14, b_+15); SET_BC(pop_effect(gb));
  if (F & FC) { RET_TAKEN(b_+15); return; } // ret c
  CYC(b_+15, b_+16);
  CALL_C(b_+16, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+19);
  CYC(b_+19, b_+20); C = A;
  CYC(b_+20, b_+22); B = 0x0a; // SPEED_40
  CYC(b_+22, b_+24); E = 0xc9; // Part.angle
  CALL_C(b_+24, objectApplyGivenSpeed_hook, SYM(objectApplyGivenSpeed), b_+27);
  CYC(b_+27, b_+28); alu_xor(gb, A);
  CYC(b_+28, b_+29); ret_effect(gb); return;
}

void itemDrop_applySpeed_hook(GB *gb) {
  BASE(itemDrop_applySpeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, BC);
  CYC(b_+1, b_+2); A = C;
  CALL_C(b_+2, partCommon_getTileCollisionAtAngle_allowHoles_hook, SYM(partCommon_getTileCollisionAtAngle_allowHoles), b_+5);
  CYC(b_+5, b_+6); SET_BC(pop_effect(gb));
  if (F & FC) { RET_TAKEN(b_+6); return; } // ret c
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); E = 0xc9; // Part.angle
  CALL_C(b_+9, objectApplyGivenSpeed_hook, SYM(objectApplyGivenSpeed), b_+12);
  CYC(b_+12, b_+13); alu_scf(gb);
  CYC(b_+13, b_+14); ret_effect(gb); return;
}
