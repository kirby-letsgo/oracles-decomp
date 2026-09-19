#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t tokayShopItem_jump_table(GB *gb) {
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

static void tokayShopItem_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// INTERAC_TOKAY_SHOP_ITEM
void interactionCode81_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6218, 0x621a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x621a, 0x621b); A = mem_rd(gb, DE);
  {
    CYC(0x621b, 0x621c); push_effect(gb, 0x621c);
    uint16_t target = tokayShopItem_jump_table(gb);
    if (target == 0x6277) goto state1;
  }

  // interactionCode81@state0
  CYC(0x6220, 0x6222); A = 0x01;
  CYC(0x6222, 0x6223); mem_wr(gb, DE, A);
  CYC(0x6223, 0x6225); A = 0x0a; // >TX_0a00
  CALL_C(0x6225, interactionSetHighTextIndex_hook, 0x253b, 0x6228);
  CALL_C(0x6228, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x622b);
  CYC(0x622b, 0x622d); A = 0x06;
  CALL_C(0x622d, objectSetCollideRadius_hook, 0x24a1, 0x6230);
  CYC(0x6230, 0x6232); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6232, 0x6233); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6233, 0x6234); mem_wr(gb, HL, A); // [var03] = [subid]
  CYC(0x6234, 0x6236); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x6236, 0x6238); goto initializeItem; } // jr nz
  CYC(0x6236, 0x6238);

  // This is the shield; only appears if all other items retrieved.
  CYC(0x6238, 0x623a); A = 0x37; // GLOBALFLAG_BOUGHT_BRACELET_FROM_TOKAY
  CALL_C(0x623a, checkGlobalFlag_hook, 0x31f3, 0x623d);
  if (F & FZ) { CYCT(0x623d, 0x6240); interactionDelete_hook(gb); return; } // jp z
  CYC(0x623d, 0x6240);

  CYC(0x6240, 0x6243); A = W8(wShieldLevel);
  CYC(0x6243, 0x6244); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6244, 0x6246); goto initializeItem; } // jr z
  CYC(0x6244, 0x6246);

  // [subid] = [var03] = [wShieldLevel] + [subid] - 1
  CYC(0x6246, 0x6248); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6248, 0x6249); C = A;
  CYC(0x6249, 0x624a); C = alu_dec8(gb, C);
  CYC(0x624a, 0x624b); A = mem_rd(gb, DE);
  CYC(0x624b, 0x624c); alu_add(gb, C);
  CYC(0x624c, 0x624d); mem_wr(gb, DE, A);
  CYC(0x624d, 0x624e); E = alu_inc8(gb, E);
  CYC(0x624e, 0x624f); mem_wr(gb, DE, A);

initializeItem:
  CYC(0x624f, 0x6252); push_effect(gb, 0x6252); goto checkTransformItem;
after1_checkTransformItem:
  if (!(F & FZ)) { CYCT(0x6252, 0x6255); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x6252, 0x6255);
  CYC(0x6255, 0x6258); SET_HL(0x7613); // mainScripts.tokayShopItemScript
  CALL_C(0x6258, interactionSetScript_hook, 0x2544, 0x625b);
  CYC(0x625b, 0x625d); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(0x625d, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x6260);
  CALL_C(0x6260, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6263);
  CYC(0x6263, 0x6266); objectSetVisible82_hook(gb); return; // jp

state1:
  CALL_C(0x6277, interactionAnimateAsNpc_hook, 0x26db, 0x627a);
  CYC(0x627a, 0x627d); push_effect(gb, 0x627d); goto checkTransformItem;
after2_checkTransformItem:
  if (!(F & FZ)) CALL_C_CC(0x627d, objectSetInvisible_hook, 0x1e7b, 0x6280); else CYC(0x627d, 0x6280); // call nz
  CALL_C(0x6280, interactionRunScript_hook, 0x2552, 0x6283);
  if (!(F & FC)) { RET_TAKEN(0x6283); return; } // ret nc
  CYC(0x6283, 0x6284);
  CYC(0x6284, 0x6285); alu_xor(gb, A);
  CYC(0x6285, 0x6288); W8(wDisabledObjects) = A;
  CYC(0x6288, 0x628b); interactionDelete_hook(gb); return; // jp

  // This checks whether to replace the feather/bracelet with the shovel, changing the subid
  // accordingly and initializing the graphics after doing so. Returns nz if item should be deleted.
checkTransformItem:
  CYC(0x628b, 0x628c); H = D;
  CYC(0x628c, 0x628e); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x628e, 0x628f); alu_xor(gb, A);
  CYC(0x628f, 0x6290); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6290, 0x6291); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6291, 0x6292); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6292, 0x6293); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a

  CYC(0x6293, 0x6295); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6295, 0x6296); A = mem_rd(gb, DE);
  CYC(0x6296, 0x6297); C = A;
  CYC(0x6297, 0x6299); A = 0x19; // TREASURE_SEED_SATCHEL
  CALL_C(0x6299, checkTreasureObtained_hook, 0x1748, 0x629c);
  if (!(F & FC)) { CYCT(0x629c, 0x629e); goto checkReplaceWithShovel; } // jr nc
  CYC(0x629c, 0x629e);

  // Seed satchel obtained; set var38/var39 based on whether Link can buy the item.
  CYC(0x629e, 0x629f); A = C;
  CYC(0x629f, 0x62a2); SET_HL(0x6268); // @seedsNeededToBuyItems
  CYC(0x62a2, 0x62a3); tokayShopItem_addAToHl_from_rst(gb, 0x62a3);
  CYC(0x62a3, 0x62a4); A = mem_rd(gb, HL);
  CALL_C(0x62a4, checkTreasureObtained_hook, 0x1748, 0x62a7);
  if (!(F & FC)) { CYCT(0x62a7, 0x62a9); goto checkReplaceWithShovel; } // jr nc
  CYC(0x62a7, 0x62a9);

  CYC(0x62a9, 0x62aa); A = alu_inc8(gb, A);
  CYC(0x62aa, 0x62ac); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x62ac, 0x62ad); mem_wr(gb, DE, A);
  CYC(0x62ad, 0x62af); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(0x62af, 0x62b1); goto checkReplaceWithShovel; } // jr c
  CYC(0x62af, 0x62b1);

  CYC(0x62b1, 0x62b3); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x62b3, 0x62b4); mem_wr(gb, DE, A);

checkReplaceWithShovel:
  CYC(0x62b4, 0x62b6); A = 0x15; // TREASURE_SHOVEL
  CALL_C(0x62b6, checkTreasureObtained_hook, 0x1748, 0x62b9);
  if (!(F & FC)) { CYCT(0x62b9, 0x62bb); goto afterShovelCheck; } // jr nc
  CYC(0x62b9, 0x62bb);
  CYC(0x62bb, 0x62bd); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(0x62bd, 0x62bf); A = 0x01;
  CYC(0x62bf, 0x62c0); mem_wr(gb, DE, A);

afterShovelCheck:
  CYC(0x62c0, 0x62c2); A = 0x01; // TREASURE_SHIELD
  CALL_C(0x62c2, checkTreasureObtained_hook, 0x1748, 0x62c5);
  if (!(F & FC)) { CYCT(0x62c5, 0x62c7); goto afterShieldCheck; } // jr nc
  CYC(0x62c5, 0x62c7);
  CYC(0x62c7, 0x62c9); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x62c9, 0x62cb); A = 0x01;
  CYC(0x62cb, 0x62cc); mem_wr(gb, DE, A);

afterShieldCheck:
  CYC(0x62cc, 0x62cd); A = C;
  CYC(0x62cd, 0x62cf); alu_cp(gb, 0x04);
  if (!(F & FC)) { CYCT(0x62cf, 0x62d1); goto setSubidAndInitGraphics; } // jr nc
  CYC(0x62cf, 0x62d1);

  // The item is the feather or the bracelet.
  // If we've bought the item, it should be deleted.
  CYC(0x62d1, 0x62d2); A = C;
  CYC(0x62d2, 0x62d5); SET_HL(0x62f4); // @boughtItemGlobalflags
  CYC(0x62d5, 0x62d6); tokayShopItem_addAToHl_from_rst(gb, 0x62d6);
  CYC(0x62d6, 0x62d7); A = mem_rd(gb, HL);
  CALL_C(0x62d7, checkGlobalFlag_hook, 0x31f3, 0x62da);
  if (!(F & FZ)) {
    RET_TAKEN(0x62da);
    if (gb->pc == 0x6252 && gb->sp == sp0_) goto after1_checkTransformItem;
    if (gb->pc == 0x627d && gb->sp == sp0_) goto after2_checkTransformItem;
    return;
  } // ret nz
  CYC(0x62da, 0x62db);

  // Otherwise, if Link has the item, it should be replaced with the shovel.
  CYC(0x62db, 0x62dc); A = C;
  CYC(0x62dc, 0x62df); SET_HL(0x6266); // @initialShopTreasures
  CYC(0x62df, 0x62e0); tokayShopItem_addAToHl_from_rst(gb, 0x62e0);
  CYC(0x62e0, 0x62e1); A = mem_rd(gb, HL);
  CYC(0x62e1, 0x62e3); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x62e3, 0x62e4); mem_wr(gb, DE, A);

  CALL_C(0x62e4, checkTreasureObtained_hook, 0x1748, 0x62e7);
  if (!(F & FC)) { CYCT(0x62e7, 0x62e9); goto setSubidAndInitGraphics; } // jr nc
  CYC(0x62e7, 0x62e9);

  // Increment subid by 2, making it a shovel
  CYC(0x62e9, 0x62ea); C = alu_inc8(gb, C);
  CYC(0x62ea, 0x62eb); C = alu_inc8(gb, C);

setSubidAndInitGraphics:
  CYC(0x62eb, 0x62ed); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x62ed, 0x62ee); A = C;
  CYC(0x62ee, 0x62ef); mem_wr(gb, DE, A);
  CALL_C(0x62ef, interactionInitGraphics_hook, 0x15fb, 0x62f2);
  CYC(0x62f2, 0x62f3); alu_xor(gb, A);
  RET(0x62f3);
  if (gb->pc == 0x6252 && gb->sp == sp0_) goto after1_checkTransformItem;
  if (gb->pc == 0x627d && gb->sp == sp0_) goto after2_checkTransformItem;
  return; // ret
}
