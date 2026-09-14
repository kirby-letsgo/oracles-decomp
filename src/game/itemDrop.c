#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

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
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) {
    CYCT(0x4113, 0x4115); // jr z,@normalStatus
  } else {
    CYC(0x4113, 0x4115);
    CYC(0x4115, 0x4117); alu_cp(gb, 0x02); // cp PARTSTATUS_DEAD
    if (F & FZ) {
      CYCT(0x4117, 0x411a); // jp z,@linkCollectedItem
      partCode01__linkCollectedItem_hook(gb);
      return;
    }
    CYC(0x4117, 0x411a);
    CYC(0x411a, 0x411c); E = 0xc4; // Part.state
    CYC(0x411c, 0x411e); A = 0x03;
    CYC(0x411e, 0x411f); mem_wr(gb, DE, A);
  }

  // @normalStatus
  CYC(0x411f, 0x4122); push_effect(gb, 0x4122);
  partCode01__checkCollidedWithLink_hook(gb);
}

void partCode01__afterCollisionCheck_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4122, 0x4124); E = 0xc4; // Part.state
  CYC(0x4124, 0x4125); A = mem_rd(gb, DE);
  {
    CYC(0x4125, 0x4126); push_effect(gb, 0x4126);
    uint16_t target = itemDrop_jump_table(gb);
    if (target == 0x412e) goto state0;
    if (target == 0x417b) goto state1;
    if (target == 0x41b4) goto state2;
    goto state3;
  }

state0:
  CYC(0x412e, 0x4131); A = W8(wIsMaplePresent);
  CYC(0x4131, 0x4132); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4132, 0x4135); partDelete_hook(gb); return; } // jp nz
  CYC(0x4132, 0x4135);
  CYC(0x4135, 0x4137); E = 0xc2; // Part.subid
  CYC(0x4137, 0x4138); A = mem_rd(gb, DE);
  CYC(0x4138, 0x413a); alu_cp(gb, 0x0f); // ITEM_DROP_100_RUPEES_OR_ENEMY
  if (!(F & FZ)) {
    CYCT(0x413a, 0x413c); // jr nz,@normalItem
  } else {
    CYC(0x413a, 0x413c);
    CALL_C(0x413c, getRandomNumber_noPreserveVars_hook, 0x0453, 0x413f);
    CYC(0x413f, 0x4141); alu_cp(gb, 0xe0);
    if (F & FC) { CYCT(0x4141, 0x4144); itemDrop_spawnEnemy_hook(gb); return; } // jp c
    CYC(0x4141, 0x4144);
  }

  // @normalItem
  CALL_C(0x4144, itemDrop_initGfx_hook, 0x428e, 0x4147);
  CYC(0x4147, 0x4148); H = D;
  CYC(0x4148, 0x414a); L = 0xd4; // Part.speedZ
  CYC(0x414a, 0x414c); A = 0xa0; // <(-$160)
  CYC(0x414c, 0x414d); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x414d, 0x414f); mem_wr(gb, HL, 0xfe); // >(-$160)
  CYC(0x414f, 0x4151); L = 0xc4; // Part.state
  CYC(0x4151, 0x4152); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4152, 0x4155); A = W8(wTilesetFlags);
  CYC(0x4155, 0x4157); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (F & FZ) {
    CYCT(0x4157, 0x4159); // jr z,@label_11_008
    goto label_11_008;
  }
  CYC(0x4157, 0x4159);
  CYC(0x4159, 0x415a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 2
  CYC(0x415a, 0x415c); L = 0xe4; // Part.collisionType
  CYC(0x415c, 0x415e); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x415e, 0x4160); L = 0xc6; // Part.counter1
  CYC(0x4160, 0x4162); mem_wr(gb, HL, 0xf0);
  CALL_C(0x4162, objectCheckIsOnHazard_hook, 0x220d, 0x4165);
  if (!(F & FC)) { CYCT(0x4165, 0x4167); goto label_11_008; } // jr nc
  CYC(0x4165, 0x4167);
  CYC(0x4167, 0x4168); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x4168, 0x416a); goto label_11_008; } // jr nc
  CYC(0x4168, 0x416a);
  CYC(0x416a, 0x416c); E = 0xf4; // Part.var34
  CYC(0x416c, 0x416e); A = 0x01;
  CYC(0x416e, 0x416f); mem_wr(gb, DE, A);

label_11_008:
  CYC(0x416f, 0x4171); E = 0xc2; // Part.subid
  CYC(0x4171, 0x4172); A = mem_rd(gb, DE);
  CALL_C(0x4172, itemDrop_initSpeed_hook, 0x42e8, 0x4175);
  CYC(0x4175, 0x4177); E = 0xc2; // Part.subid
  CYC(0x4177, 0x4178); A = mem_rd(gb, DE);
  CYC(0x4178, 0x417b); partSetAnimation_hook(gb); return; // jp

state1:
  CALL_C(0x417b, partCommon_getTileCollisionInFront_allowHoles, 0x4030, 0x417e);
  if (!(F & FC)) {
    CALL_C_CC(0x417e, itemDrop_updateSpeed_hook, 0x4302, 0x4181); // call nc
  } else {
    CYC(0x417e, 0x4181);
  }
  CYC(0x4181, 0x4183); C = 0x20;
  CALL_C(0x4183, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x4186);
  if (F & FC) { CYCT(0x4186, 0x4188); goto doneBouncing; } // jr c
  CYC(0x4186, 0x4188);
  CALL_C(0x4188, itemDrop_checkHitGround_hook, 0x4365, 0x418b);
  if (!(F & FC)) { CYCT(0x418b, 0x418d); goto label_11_010; } // jr nc
  CYC(0x418b, 0x418d);

doneBouncing:
  CYC(0x418d, 0x418e); H = D;
  CYC(0x418e, 0x4190); L = 0xc4; // Part.state
  CYC(0x4190, 0x4191); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4191, 0x4193); L = 0xc6; // Part.counter1
  CYC(0x4193, 0x4195); mem_wr(gb, HL, 0xf0);
  CALL_C(0x4195, objectSetVisiblec3_hook, 0x1e4e, 0x4198);

label_11_010:
  CALL_C(0x4198, itemDrop_checkOnHazard_hook, 0x4386, 0x419b);
  if (F & FC) { RET_TAKEN(0x419b); return; } // ret c
  CYC(0x419b, 0x419c);
  CYC(0x419c, 0x419e); E = 0xcf; // Part.zh
  CYC(0x419e, 0x419f); A = mem_rd(gb, DE);
  CYC(0x419f, 0x41a0); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(0x41a0); return; } // ret c
  CYC(0x41a0, 0x41a1);
  CYC(0x41a1, 0x41a4); SET_BC(0x0500);
  CALL_C(0x41a4, objectGetRelativeTile_hook, 0x1435, 0x41a7);
  CYC(0x41a7, 0x41aa); SET_HL(0x442c); // itemDropConveyorTilesTable
  CALL_C(0x41aa, lookupCollisionTable_hook, 0x1e1f, 0x41ad);
  if (!(F & FC)) { RET_TAKEN(0x41ad); return; } // ret nc
  CYC(0x41ad, 0x41ae);
  CYC(0x41ae, 0x41af); C = A;
  CYC(0x41af, 0x41b1); B = 0x14; // SPEED_80
  CYC(0x41b1, 0x41b4); itemDrop_applySpeed_hook(gb); return; // jp

state2:
  CALL_C(0x41b4, itemDrop_checkSidescrollingConditions_hook, 0x4332, 0x41b7);
  CALL_C(0x41b7, itemDrop_moveTowardPoint_hook, 0x4401, 0x41ba);
  if (F & FC) { CYCT(0x41ba, 0x41bd); goto reachedPoint; } // jp c
  CYC(0x41ba, 0x41bd);
  CALL_C(0x41bd, itemDrop_countdownToDisappear_hook, 0x42c3, 0x41c0);
  if (F & FC) { CYCT(0x41c0, 0x41c3); partDelete_hook(gb); return; } // jp c
  CYC(0x41c0, 0x41c3);
  CYC(0x41c3, 0x41c5); E = 0xc2; // Part.subid
  CYC(0x41c5, 0x41c6); A = mem_rd(gb, DE);
  CYC(0x41c6, 0x41c7); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x41c7, 0x41c9); goto label_11_010; } // jr nz
  CYC(0x41c7, 0x41c9);
  CYC(0x41c9, 0x41cc); itemDrop_updateFairyMovement_hook(gb); return; // jp

reachedPoint:
  CYC(0x41cc, 0x41cd); H = D;
  CYC(0x41cd, 0x41cf); L = 0xf1; // Part.var31
  CYC(0x41cf, 0x41d0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x41d0, 0x41d1); C = mem_rd(gb, HL); // Part.var32
  CYC(0x41d1, 0x41d3); L = 0xcb; // Part.yh
  mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x41d3, 0x41d4);
  CYC(0x41d4, 0x41d5); L = L + 1; // Part.xh
  CYC(0x41d5, 0x41d6); mem_wr(gb, HL, C);
  CYC(0x41d6, 0x41d9); partDelete_hook(gb); return; // jp

state3:
  CYC(0x41d9, 0x41db); E = 0xc5; // Part.substate
  CYC(0x41db, 0x41dc); A = mem_rd(gb, DE);
  CYC(0x41dc, 0x41dd); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x41dd, 0x41e0); // call z
    push_effect(gb, 0x41e0);
    partCode01__getRelatedObj1ID_hook(gb);
  } else {
    CYC(0x41dd, 0x41e0);
  }
  CALL_C(0x41e0, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x41e3);
  if (F & FC) { CYCT(0x41e3, 0x41e6); partCode01__linkCollectedItem_hook(gb); return; } // jp c
  CYC(0x41e3, 0x41e6);
  CYC(0x41e6, 0x41e8); A = 0x00; // Object.enabled
  CALL_C(0x41e8, objectGetRelatedObject1Var_hook, 0x2160, 0x41eb);
  CYC(0x41eb, 0x41ec); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x41ec, 0x41ed); alu_or(gb, A);
  if (F & FZ) { CYCT(0x41ed, 0x41ef); goto label_11_006; } // jr z
  CYC(0x41ed, 0x41ef);
  CYC(0x41ef, 0x41f1); E = 0xf0; // Part.var30
  CYC(0x41f1, 0x41f2); A = mem_rd(gb, DE);
  CYC(0x41f2, 0x41f3); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x41f3, 0x41f6); objectTakePosition_hook(gb); return; } // jp z
  CYC(0x41f3, 0x41f6);

label_11_006:
  CYC(0x41f6, 0x41f9); partDelete_hook(gb); return; // jp
}

void partCode01__getRelatedObj1ID_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x41f9, 0x41fa); H = D;
  CYC(0x41fa, 0x41fb); L = E;
  CYC(0x41fb, 0x41fc); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [substate]
  CYC(0x41fc, 0x41fe); L = 0xcf; // Part.zh
  CYC(0x41fe, 0x4200); mem_wr(gb, HL, 0x00);
  CYC(0x4200, 0x4202); A = 0x01; // Object.id
  CALL_C(0x4202, objectGetRelatedObject1Var_hook, 0x2160, 0x4205);
  CYC(0x4205, 0x4206); A = mem_rd(gb, HL);
  CYC(0x4206, 0x4208); E = 0xf0; // Part.var30
  CYC(0x4208, 0x4209); mem_wr(gb, DE, A);
  CYC(0x4209, 0x420c); objectSetVisible80_hook(gb); // jp, real ret happens inside its chain
}

void partCode01__checkCollidedWithLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x420c, 0x420e); E = 0xe4; // Part.collisionType
  CYC(0x420e, 0x420f); A = mem_rd(gb, DE);
  CYC(0x420f, 0x4210); alu_rlca(gb);
  if (!(F & FC)) { RET_TAKEN(0x4210); partCode01__afterCollisionCheck_hook(gb); return; } // ret nc
  CYC(0x4210, 0x4211);
  CALL_C(0x4211, objectCheckCollidedWithLink_hook, 0x1c41, 0x4214);
  if (!(F & FC)) { RET_TAKEN(0x4214); partCode01__afterCollisionCheck_hook(gb); return; } // ret nc
  CYC(0x4214, 0x4215);
  CYC(0x4215, 0x4216); SET_HL(pop_effect(gb)); // pop hl (discard return address)
  partCode01__linkCollectedItem_hook(gb);
}

void partCode01__linkCollectedItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4216, 0x4219); A = W8(wLinkDeathTrigger);
  CYC(0x4219, 0x421a); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x421a, 0x421c); goto deleteSelf; } // jr nz
  CYC(0x421a, 0x421c);
  CYC(0x421c, 0x421e); E = 0xc2; // Part.subid
  CYC(0x421e, 0x421f); A = mem_rd(gb, DE);
  CYC(0x421f, 0x4220); alu_add(gb, A);
  CYC(0x4220, 0x4223); SET_HL(0x424e); // @itemDropTreasureTable
  CYC(0x4223, 0x4224); itemDrop_addDoubleIndexToHl_from_rst(gb, 0x4224);
  CYC(0x4224, 0x4225); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4225, 0x4226); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4226, 0x4228); goto deleteSelf; } // jr z
  CYC(0x4226, 0x4228);
  CYC(0x4228, 0x4229); B = A;
  CYC(0x4229, 0x422b); A = 0x26; // GOLD_JOY_RING
  CALL_C(0x422b, cpActiveRing_hook, 0x23b0, 0x422e);
  CYC(0x422e, 0x422f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  if (F & FZ) { CYCT(0x422f, 0x4231); goto doubleDrop; } // jr z
  CYC(0x422f, 0x4231);
  CYC(0x4231, 0x4232); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4232, 0x4234); goto giveDrop; } // jr z
  CYC(0x4232, 0x4234);
  CALL_C(0x4234, cpActiveRing_hook, 0x23b0, 0x4237);
  if (!(F & FZ)) { CYCT(0x4237, 0x4239); goto giveDrop; } // jr nz
  CYC(0x4237, 0x4239);

doubleDrop:
  CYC(0x4239, 0x423a); SET_HL(HL + 1);

giveDrop:
  CYC(0x423a, 0x423b); C = mem_rd(gb, HL);
  CYC(0x423b, 0x423c); A = B;
  CALL_C(0x423c, giveTreasure_hook, 0x171c, 0x423f);
  CYC(0x423f, 0x4241); E = 0xc2; // Part.subid
  CYC(0x4241, 0x4242); A = mem_rd(gb, DE);
  CYC(0x4242, 0x4244); alu_cp(gb, 0x0e); // ITEM_DROP_50_ORE_CHUNKS
  if (!(F & FZ)) { CYCT(0x4244, 0x4246); goto deleteSelf; } // jr nz
  CYC(0x4244, 0x4246);
  CALL_C(0x4246, getThisRoomFlags_hook, 0x197d, 0x4249);
  CYC(0x4249, 0x424b); mem_wr(gb, HL, mem_rd(gb, HL) | 0x20);

deleteSelf:
  CYC(0x424b, 0x424e); partDelete_hook(gb); return; // jp
}

void itemDrop_initGfx_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x428e, 0x4290); E = 0xc2; // Part.subid
  CYC(0x4290, 0x4291); A = mem_rd(gb, DE);
  CYC(0x4291, 0x4294); SET_HL(0x42a3); // @spriteData
  CYC(0x4294, 0x4295); itemDrop_addDoubleIndexToHl_from_rst(gb, 0x4295);
  CYC(0x4295, 0x4297); E = 0xdd; // Part.oamTileIndexBase
  CYC(0x4297, 0x4298); A = mem_rd(gb, DE);
  CYC(0x4298, 0x4299); A = A + mem_rd(gb, HL);
  CYC(0x4299, 0x429a); mem_wr(gb, DE, A);
  CYC(0x429a, 0x429b); SET_HL(HL + 1);
  CYC(0x429b, 0x429c); E = 0xdc; // Part.oamFlags
  CYC(0x429c, 0x429d); A = mem_rd(gb, HL);
  CYC(0x429d, 0x429e); mem_wr(gb, DE, A); // [oamFlags]
  CYC(0x429e, 0x429f); E = 0xdb; // Part.oamFlagsBackup
  CYC(0x429f, 0x42a0); mem_wr(gb, DE, A); // [oamFlagsBackup]
  CYC(0x42a0, 0x42a3); objectSetVisiblec1_hook(gb); return; // jp
}

void itemDrop_countdownToDisappear_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42c3, 0x42c6); A = W8(wFrameCounter);
  CYC(0x42c6, 0x42c7); A = A ^ D;
  CYC(0x42c7, 0x42c8); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x42c8); return; } // ret nc
  CYC(0x42c8, 0x42c9);
  CYC(0x42c9, 0x42ca); H = D;
  CYC(0x42ca, 0x42cc); L = 0xf3; // Part.var33
  CYC(0x42cc, 0x42cd); A = mem_rd(gb, HL);
  CYC(0x42cd, 0x42ce); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x42ce, 0x42d0); // jr z,L_42d6
  } else {
    CYC(0x42ce, 0x42d0);
    CYC(0x42d0, 0x42d1); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
    if (!(F & FZ)) { RET_TAKEN(0x42d1); return; } // ret nz
    CYC(0x42d1, 0x42d2);
    CYC(0x42d2, 0x42d4); L = 0xe4; // Part.collisionType
    CYC(0x42d4, 0x42d6); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  }
  CALL_C(0x42d6, partCommon_decCounter1IfNonzero, 0x40a7, 0x42d9);
  if (F & FZ) { CYCT(0x42d9, 0x42db); goto disappear; } // jr z
  CYC(0x42d9, 0x42db);
  CYC(0x42db, 0x42dc); A = mem_rd(gb, HL);
  CYC(0x42dc, 0x42de); alu_cp(gb, 60);
  if (!(F & FC)) { RET_TAKEN(0x42de); return; } // ret nc
  CYC(0x42de, 0x42df);
  CYC(0x42df, 0x42e1); L = 0xda; // Part.visible
  CYC(0x42e1, 0x42e2); A = mem_rd(gb, HL);
  CYC(0x42e2, 0x42e4); A = A ^ 0x80;
  CYC(0x42e4, 0x42e5); mem_wr(gb, HL, A);
  CYC(0x42e5, 0x42e6); ret_effect(gb); return;

disappear:
  CYC(0x42e6, 0x42e7); alu_scf(gb);
  CYC(0x42e7, 0x42e8); ret_effect(gb); return;
}

void itemDrop_initSpeed_hook(GB *gb) {
  CYC(0x42e8, 0x42e9); H = D;
  CYC(0x42e9, 0x42ea); alu_or(gb, A);
  if (F & FZ) { CYCT(0x42ea, 0x42ec); goto fairy; } // jr z
  CYC(0x42ea, 0x42ec);
  CYC(0x42ec, 0x42ee); E = 0xc3; // Part.var03
  CYC(0x42ee, 0x42ef); A = mem_rd(gb, DE);
  CYC(0x42ef, 0x42f0); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x42f0); return; } // ret nc
  CYC(0x42f0, 0x42f1);
  CYC(0x42f1, 0x42f3); L = 0xd0; // Part.speed
  CYC(0x42f3, 0x42f5); mem_wr(gb, HL, 0x19); // SPEED_a0
  CYC(0x42f5, 0x42f6); ret_effect(gb); return;

fairy:
  CYC(0x42f6, 0x42f8); L = 0xcf; // Part.zh
  CYC(0x42f8, 0x42f9); A = mem_rd(gb, HL);
  CYC(0x42f9, 0x42fb); mem_wr(gb, HL, 0x00);
  CYC(0x42fb, 0x42fd); L = 0xcb; // Part.yh
  CYC(0x42fd, 0x42fe); A = A + mem_rd(gb, HL);
  CYC(0x42fe, 0x42ff); mem_wr(gb, HL, A);
  CYC(0x42ff, 0x4302); itemDrop_chooseRandomFairyMovement_hook(gb); return; // jp
}

void itemDrop_updateSpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4302, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x4305);
  if (F & FC) { RET_TAKEN(0x4305); return; } // ret c
  CYC(0x4305, 0x4306);
  CYC(0x4306, 0x4309); objectApplySpeed_hook(gb); return; // jp
}

void itemDrop_spawnEnemy_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4309, 0x430a); C = A;
  CYC(0x430a, 0x430d); A = W8(wDiggingUpEnemiesForbidden);
  CYC(0x430d, 0x430e); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x430e, 0x4310); goto delete_; } // jr nz
  CYC(0x430e, 0x4310);
  CYC(0x4310, 0x4311); A = C;
  CYC(0x4311, 0x4313); alu_and(gb, 0x07);
  CYC(0x4313, 0x4316); SET_HL(0x432a); // @enemiesToSpawn
  CYC(0x4316, 0x4317); itemDrop_addAToHl_from_rst(gb, 0x4317);
  CYC(0x4317, 0x4318); B = mem_rd(gb, HL);
  CALL_C(0x4318, getFreeEnemySlot_hook, 0x2e27, 0x431b);
  if (!(F & FZ)) { CYCT(0x431b, 0x431d); goto delete_; } // jr nz
  CYC(0x431b, 0x431d);
  CYC(0x431d, 0x431e); mem_wr(gb, HL, B);
  CALL_C(0x431e, objectCopyPosition_hook, 0x2242, 0x4321);
  CYC(0x4321, 0x4323); E = 0xc3; // Part.var03
  CYC(0x4323, 0x4324); A = mem_rd(gb, DE);
  CYC(0x4324, 0x4326); L = 0x82; // Enemy.subid
  CYC(0x4326, 0x4327); mem_wr(gb, HL, A);

delete_:
  CYC(0x4327, 0x432a); partDelete_hook(gb); return; // jp
}

void itemDrop_checkSidescrollingConditions_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4332, 0x4335); A = W8(wTilesetFlags);
  CYC(0x4335, 0x4337); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (F & FZ) { RET_TAKEN(0x4337); return; } // ret z
  CYC(0x4337, 0x4338);
  CYC(0x4338, 0x433a); E = 0xc2; // Part.subid
  CYC(0x433a, 0x433b); A = mem_rd(gb, DE);
  CYC(0x433b, 0x433c); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x433c); return; } // ret z
  CYC(0x433c, 0x433d);
  CYC(0x433d, 0x433f); A = 0x20;
  CALL_C(0x433f, objectUpdateSpeedZ_sidescroll_hook, 0x1f66, 0x4342);
  if (F & FC) { CYCT(0x4342, 0x4344); goto checkY; } // jr c
  CYC(0x4342, 0x4344);
  CYC(0x4344, 0x4346); E = 0xf4; // Part.var34
  CYC(0x4346, 0x4347); A = mem_rd(gb, DE);
  CYC(0x4347, 0x4348); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x4348, 0x434a); goto checkY; } // jr nc
  CYC(0x4348, 0x434a);
  CYC(0x434a, 0x434c); B = 0x01;
  CYC(0x434c, 0x434d); A = mem_rd(gb, HL); // [speedZ+1]
  CYC(0x434d, 0x434f); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(0x434f, 0x4351); goto after_sign; } // jr z
  CYC(0x434f, 0x4351);
  CYC(0x4351, 0x4353); B = 0xff;
  CYC(0x4353, 0x4354); A = alu_inc8(gb, A);

after_sign:
  CYC(0x4354, 0x4356); alu_cp(gb, 0x01);
  if (F & FC) { RET_TAKEN(0x4356); return; } // ret c
  CYC(0x4356, 0x4357);
  CYC(0x4357, 0x4358); mem_wr(gb, HL, B); // [speedZ+1]
  CYC(0x4358, 0x4359); L = L - 1;
  CYC(0x4359, 0x435b); mem_wr(gb, HL, 0x00); // [speedZ]

checkY:
  CYC(0x435b, 0x435d); E = 0xcb; // Part.yh
  CYC(0x435d, 0x435e); A = mem_rd(gb, DE);
  CYC(0x435e, 0x4360); alu_cp(gb, 0xb0);
  if (F & FC) { RET_TAKEN(0x4360); return; } // ret c
  CYC(0x4360, 0x4361);
  CYC(0x4361, 0x4362); SET_HL(pop_effect(gb)); // pop hl (discard return address)
  CYC(0x4362, 0x4365); partDelete_hook(gb); return; // jp
}

void itemDrop_checkHitGround_hook(GB *gb) {
  CYC(0x4365, 0x4367); E = 0xc2; // Part.subid
  CYC(0x4367, 0x4368); A = mem_rd(gb, DE);
  CYC(0x4368, 0x4369); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4369, 0x436b); goto fairy; } // jr z
  CYC(0x4369, 0x436b);
  CYC(0x436b, 0x436d); E = 0xd5; // Part.speedZ+1
  CYC(0x436d, 0x436e); A = mem_rd(gb, DE);
  CYC(0x436e, 0x4370); alu_and(gb, 0x80);
  if (!(F & FZ)) { RET_TAKEN(0x4370); return; } // ret nz
  CYC(0x4370, 0x4371);
  CYC(0x4371, 0x4372); H = D;
  CYC(0x4372, 0x4374); L = 0xe4; // Part.collisionType
  CYC(0x4374, 0x4376); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(0x4376, 0x4377); ret_effect(gb); return;

fairy:
  CYC(0x4377, 0x4379); E = 0xcf; // Part.zh
  CYC(0x4379, 0x437a); A = mem_rd(gb, DE);
  CYC(0x437a, 0x437c); alu_cp(gb, 0xfa);
  if (!(F & FC)) { RET_TAKEN(0x437c); return; } // ret nc
  CYC(0x437c, 0x437d);
  CYC(0x437d, 0x437e); H = D;
  CYC(0x437e, 0x437f); L = E;
  CYC(0x437f, 0x4381); mem_wr(gb, HL, 0xfa); // [Part.zh]
  CYC(0x4381, 0x4383); L = 0xf3; // Part.var33
  CYC(0x4383, 0x4385); mem_wr(gb, HL, 0x05);
  CYC(0x4385, 0x4386); ret_effect(gb); return;
}

void itemDrop_checkOnHazard_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4386, objectCheckIsOnHazard_hook, 0x220d, 0x4389);
  if (F & FC) { CYCT(0x4389, 0x438b); goto onHazard; } // jr c
  CYC(0x4389, 0x438b);
  CYC(0x438b, 0x438d); E = 0xf4; // Part.var34
  CYC(0x438d, 0x438e); A = mem_rd(gb, DE);
  CYC(0x438e, 0x438f); alu_rrca(gb);
  if (!(F & FC)) { RET_TAKEN(0x438f); return; } // ret nc
  CYC(0x438f, 0x4390);
  CYC(0x4390, 0x4392); B = 0x03; // INTERAC_SPLASH
  CYC(0x4392, 0x4393); alu_xor(gb, A);
  CYC(0x4393, 0x4395); goto onWaterSidescrolling; // jr (unconditional)

onHazard:
  CYC(0x4395, 0x4396); alu_rrca(gb);
  if (F & FC) { CYCT(0x4396, 0x4398); goto onWater; } // jr c
  CYC(0x4396, 0x4398);
  CYC(0x4398, 0x4399); alu_rrca(gb);
  CYC(0x4399, 0x439b); B = 0x04; // INTERAC_LAVASPLASH
  if (!(F & FC)) { CYCT(0x439b, 0x439d); goto replaceWithAnimation; } // jr nc
  CYC(0x439b, 0x439d);
  CALL_C(0x439d, objectCreateFallingDownHoleInteraction_hook, 0x24d1, 0x43a0);
  CYC(0x43a0, 0x43a2); goto delete_; // jr (unconditional)

replaceWithAnimation:
  CALL_C(0x43a2, objectCreateInteractionWithSubid00_hook, 0x24c3, 0x43a5);

delete_:
  CALL_C(0x43a5, partDelete_hook, 0x3ea1, 0x43a8);
  CYC(0x43a8, 0x43a9); alu_scf(gb);
  CYC(0x43a9, 0x43aa); ret_effect(gb); return;

onWater:
  CYC(0x43aa, 0x43ac); B = 0x03; // INTERAC_SPLASH
  CYC(0x43ac, 0x43af); A = W8(wTilesetFlags);
  CYC(0x43af, 0x43b1); alu_and(gb, 0x20); // TILESETFLAG_SIDESCROLL
  if (F & FZ) { CYCT(0x43b1, 0x43b3); goto replaceWithAnimation; } // jr z
  CYC(0x43b1, 0x43b3);
  CYC(0x43b3, 0x43b5); E = 0xf4; // Part.var34
  CYC(0x43b5, 0x43b6); A = mem_rd(gb, DE);
  CYC(0x43b6, 0x43b7); alu_rrca(gb);
  CYC(0x43b7, 0x43b8); alu_ccf(gb);
  if (!(F & FC)) { RET_TAKEN(0x43b8); return; } // ret nc
  CYC(0x43b8, 0x43b9);
  CYC(0x43b9, 0x43bb); A = 0x01;

onWaterSidescrolling:
  CYC(0x43bb, 0x43bc); mem_wr(gb, DE, A);
  CYC(0x43bc, 0x43bf); objectCreateInteractionWithSubid00_hook(gb); return; // jp
}

void itemDrop_updateFairyMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x43bf, 0x43c0); H = D;
  CYC(0x43c0, 0x43c2); L = 0xc7; // Part.counter2
  CYC(0x43c2, 0x43c3); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(0x43c3, 0x43c5); itemDrop_chooseRandomFairyMovement_hook(gb); return; } // jr z
  CYC(0x43c3, 0x43c5);
  CALL_C(0x43c5, partCommon_getTileCollisionInFront, 0x4000, 0x43c8);
  CYC(0x43c8, 0x43c9); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(0x43c9, 0x43cc); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(0x43c9, 0x43cc);
  itemDrop_chooseRandomFairyMovement_hook(gb); return; // fallthrough
}

void itemDrop_chooseRandomFairyMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x43cc, getRandomNumber_noPreserveVars_hook, 0x0453, 0x43cf);
  CYC(0x43cf, 0x43d1); alu_and(gb, 0x3e);
  CYC(0x43d1, 0x43d3); A = A + 0x08;
  CYC(0x43d3, 0x43d5); E = 0xc7; // Part.counter2
  CYC(0x43d5, 0x43d6); mem_wr(gb, DE, A);
  CALL_C(0x43d6, getRandomNumber_noPreserveVars_hook, 0x0453, 0x43d9);
  CYC(0x43d9, 0x43db); alu_and(gb, 0x03);
  CYC(0x43db, 0x43de); SET_HL(0x43fd); // @speedTable
  CYC(0x43de, 0x43df); itemDrop_addAToHl_from_rst(gb, 0x43df);
  CYC(0x43df, 0x43e1); E = 0xd0; // Part.speed
  CYC(0x43e1, 0x43e2); A = mem_rd(gb, HL);
  CYC(0x43e2, 0x43e3); mem_wr(gb, DE, A);
  CALL_C(0x43e3, getRandomNumber_noPreserveVars_hook, 0x0453, 0x43e6);
  CYC(0x43e6, 0x43e8); alu_and(gb, 0x1e);
  CYC(0x43e8, 0x43e9); H = D;
  CYC(0x43e9, 0x43eb); L = 0xc9; // Part.angle
  CYC(0x43eb, 0x43ec); mem_wr(gb, HL, A);
  CYC(0x43ec, 0x43ee); alu_and(gb, 0x0f);
  if (F & FZ) { RET_TAKEN(0x43ee); return; } // ret z
  CYC(0x43ee, 0x43ef);
  CYC(0x43ef, 0x43f1); alu_bit(gb, 4, mem_rd(gb, HL));
  CYC(0x43f1, 0x43f3); L = 0xdb; // Part.oamFlagsBackup
  CYC(0x43f3, 0x43f4); A = mem_rd(gb, HL);
  CYC(0x43f4, 0x43f6); A = A & ~0x20;
  if (!(F & FZ)) { CYCT(0x43f6, 0x43f8); goto set_flags; } // jr nz
  CYC(0x43f6, 0x43f8);
  CYC(0x43f8, 0x43fa); A = A | 0x20;

set_flags:
  CYC(0x43fa, 0x43fb); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x43fb, 0x43fc); mem_wr(gb, HL, A);
  CYC(0x43fc, 0x43fd); ret_effect(gb); return;
}

void itemDrop_moveTowardPoint_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4401, 0x4403); L = 0xf1; // Part.var31
  CYC(0x4403, 0x4404); H = D;
  CYC(0x4404, 0x4405); alu_xor(gb, A);
  CYC(0x4405, 0x4406); B = mem_rd(gb, HL); // [var31]
  mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4406, 0x4407);
  CYC(0x4407, 0x4408); C = mem_rd(gb, HL); // [var32]
  mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4408, 0x4409);
  CYC(0x4409, 0x440a); alu_or(gb, B);
  if (F & FZ) { RET_TAKEN(0x440a); return; } // ret z
  CYC(0x440a, 0x440b);
  CYC(0x440b, 0x440c); push_effect(gb, BC);
  CALL_C(0x440c, objectCheckContainsPoint_hook, 0x1bdb, 0x440f);
  CYC(0x440f, 0x4410); SET_BC(pop_effect(gb));
  if (F & FC) { RET_TAKEN(0x4410); return; } // ret c
  CYC(0x4410, 0x4411);
  CALL_C(0x4411, objectGetRelativeAngle_hook, 0x1ea4, 0x4414);
  CYC(0x4414, 0x4415); C = A;
  CYC(0x4415, 0x4417); B = 0x0a; // SPEED_40
  CYC(0x4417, 0x4419); E = 0xc9; // Part.angle
  CALL_C(0x4419, objectApplyGivenSpeed_hook, 0x2029, 0x441c);
  CYC(0x441c, 0x441d); alu_xor(gb, A);
  CYC(0x441d, 0x441e); ret_effect(gb); return;
}

void itemDrop_applySpeed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x441e, 0x441f); push_effect(gb, BC);
  CYC(0x441f, 0x4420); A = C;
  CALL_C(0x4420, partCommon_getTileCollisionAtAngle_allowHoles, 0x402a, 0x4423);
  CYC(0x4423, 0x4424); SET_BC(pop_effect(gb));
  if (F & FC) { RET_TAKEN(0x4424); return; } // ret c
  CYC(0x4424, 0x4425);
  CYC(0x4425, 0x4427); E = 0xc9; // Part.angle
  CALL_C(0x4427, objectApplyGivenSpeed_hook, 0x2029, 0x442a);
  CYC(0x442a, 0x442b); alu_scf(gb);
  CYC(0x442b, 0x442c); ret_effect(gb); return;
}
