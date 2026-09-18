#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t wildTokayController_jump_table(GB *gb) {
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

static void wildTokayController_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// INTERAC_WILD_TOKAY_CONTROLLER
//
// Variables:
//   var03: Set to $ff when the game is lost?
//   var38: ?
//   var39: ?
//   var3b: ?
//   var3e/3f: Link's B/A button items, saved
void interactionCode70_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x56e8, 0x56ea); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x56ea, 0x56eb); A = mem_rd(gb, DE);
  {
    CYC(0x56eb, 0x56ec); push_effect(gb, 0x56ec);
    uint16_t target = wildTokayController_jump_table(gb);
    if (target == 0x571b) goto state1;
  }

  CYC(0x56f0, 0x56f1); alu_xor(gb, A);
  CYC(0x56f1, 0x56f4); SET_HL(wTmpcfc0_wildTokay_cfde);
  CYC(0x56f4, 0x56f5); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x56f5, 0x56f6); mem_wr(gb, HL, A);
  CALL_C(0x56f6, interactionIncState_hook, 0x23e0, 0x56f9);
  CYC(0x56f9, 0x56fc); A = W8(wWildTokayGameLevel);
  CYC(0x56fc, 0x56fd); B = A;
  CYC(0x56fd, 0x5700); A = W8(wTmpcfc0_wildTokay_inPresent);
  CYC(0x5700, 0x5701); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5701, 0x5703); goto l5705; } // jr z
  CYC(0x5701, 0x5703);
  CYC(0x5703, 0x5705); B = 0x02;

l5705:
  CYC(0x5705, 0x5706); A = B;
  CYC(0x5706, 0x5709); W8(wTmpcfc0_wildTokay_cfdc) = A;
  CYC(0x5709, 0x570c); SET_BC(0x5716); // @var3bValues
  CALL_C(0x570c, addAToBc_hook, 0x006d, 0x570f);
  CYC(0x570f, 0x5710); A = mem_rd(gb, BC);
  CYC(0x5710, 0x5712); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x5712, 0x5713); mem_wr(gb, DE, A);
  CYC(0x5713, 0x5716); goto getRandomVar39Value; // jp

state1:
  CYC(0x571b, 0x571d); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x571d, 0x571e); A = mem_rd(gb, DE);
  {
    CYC(0x571e, 0x571f); push_effect(gb, 0x571f);
    uint16_t target = wildTokayController_jump_table(gb);
    if (target == 0x5778) goto substate1;
    if (target == 0x578b) goto substate2;
    if (target == 0x579c) goto substate3;
    if (target == 0x57b3) goto substate4;
    if (target == 0x57da) goto substate5;
    if (target == 0x57f8) goto substate6;
  }

  CYC(0x572d, 0x5730); A = W8(wPaletteThread_mode);
  CYC(0x5730, 0x5731); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x5731); return; } // ret nz
  CYC(0x5731, 0x5732);
  // Save Link's equipped items
  CYC(0x5732, 0x5735); SET_HL(wInventoryB);
  CYC(0x5735, 0x5737); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x5737, 0x5738); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5738, 0x5739); mem_wr(gb, DE, A);
  CYC(0x5739, 0x573a); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(0x573a, 0x573b); E = alu_inc8(gb, E);
  CYC(0x573b, 0x573c); mem_wr(gb, DE, A);
  CYC(0x573c, 0x573e); mem_wr(gb, HL, 0x00); // ITEM_NONE
  CYC(0x573e, 0x573f); L = alu_inc8(gb, L);
  CYC(0x573f, 0x5741); mem_wr(gb, HL, 0x16); // ITEM_BRACELET
  // Replace tiles to start game
  CYC(0x5741, 0x5743); B = 0x06;
  CYC(0x5743, 0x5746); SET_HL(0x576c); // @tilesToReplaceOnStart

nextTile:
  CYC(0x5746, 0x5747); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x5747, 0x5748); C = mem_rd(gb, HL);
  CYC(0x5748, 0x5749); SET_HL(HL + 1);
  CYC(0x5749, 0x574a); push_effect(gb, BC);
  CYC(0x574a, 0x574b); push_effect(gb, HL);
  CALL_C(0x574b, setTile_hook, 0x3a9c, 0x574e);
  CYC(0x574e, 0x574f); SET_HL(pop_effect(gb));
  CYC(0x574f, 0x5750); SET_BC(pop_effect(gb));
  CYC(0x5750, 0x5751); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x5751, 0x5753); goto nextTile; } // jr nz
  CYC(0x5751, 0x5753);
  CALL_C(0x5753, interactionIncSubstate_hook, 0x23e5, 0x5756);
  CYC(0x5756, 0x5758); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5758, 0x575a); mem_wr(gb, HL, 30);
  CYC(0x575a, 0x575d); SET_HL(0xd00b); // w1Link.yh
  CYC(0x575d, 0x575f); mem_wr(gb, HL, 0x48);
  CYC(0x575f, 0x5761); L = 0x0d; // <w1Link.xh
  CYC(0x5761, 0x5763); mem_wr(gb, HL, 0x50);
  CYC(0x5763, 0x5764); alu_xor(gb, A);
  CYC(0x5764, 0x5766); L = 0x08; // <w1Link.direction
  CYC(0x5766, 0x5767); mem_wr(gb, HL, A);
  CYC(0x5767, 0x5768); A = alu_dec8(gb, A);
  CYC(0x5768, 0x576b); W8(wStatusBarNeedsRefresh) = A;
  RET(0x576b); return; // ret

substate1:
  CALL_C(0x5778, interactionDecCounter1_hook, 0x23cc, 0x577b);
  if (!(F & FZ)) { RET_TAKEN(0x577b); return; } // ret nz
  CYC(0x577b, 0x577c);
  CALL_C(0x577c, interactionIncSubstate_hook, 0x23e5, 0x577f);
  CYC(0x577f, 0x5781); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x5781, 0x5783); mem_wr(gb, HL, 0x0a);
  CYC(0x5783, 0x5785); A = 0x02; // MUS_MINIGAME
  CALL_C(0x5785, playSound_b00_hook, 0x0c98, 0x5788);
  CYC(0x5788, 0x578b); fadeinFromWhite_hook(gb); return; // jp

substate2:
  CALL_C(0x578b, interactionDecCounter1IfPaletteNotFading_hook, 0x2744, 0x578e);
  if (!(F & FZ)) { RET_TAKEN(0x578e); return; } // ret nz
  CYC(0x578e, 0x578f);
  CALL_C(0x578f, interactionIncSubstate_hook, 0x23e5, 0x5792);
  CYC(0x5792, 0x5793); alu_xor(gb, A);
  CYC(0x5793, 0x5796); W8(wDisabledObjects) = A;
  CYC(0x5796, 0x5799); SET_BC(0x0a16); // TX_0a16
  CYC(0x5799, 0x579c); showText_hook(gb); return; // jp

substate3:
  CYC(0x579c, 0x579f); A = W8(wTextIsActive);
  CYC(0x579f, 0x57a0); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x57a0); return; } // ret nz
  CYC(0x57a0, 0x57a1);
  CALL_C(0x57a1, interactionIncSubstate_hook, 0x23e5, 0x57a4);
  CYC(0x57a4, 0x57a6); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x57a6, 0x57a8); mem_wr(gb, HL, 0x3c);
  CALL_C(0x57a8, getFreeInteractionSlot_hook, 0x3aef, 0x57ab);
  if (!(F & FZ)) { RET_TAKEN(0x57ab); return; } // ret nz
  CYC(0x57ab, 0x57ac);
  CYC(0x57ac, 0x57ae); mem_wr(gb, HL, 0x8c); // INTERAC_TOKAY_MEAT
  CYC(0x57ae, 0x57b0); A = 0xcc; // SND_WHISTLE
  CYC(0x57b0, 0x57b3); playSound_b00_hook(gb); return; // jp

substate4:
  CYC(0x57b3, 0x57b6); A = W8(wTmpcfc0_wildTokay_cfde);
  CYC(0x57b6, 0x57b7); alu_or(gb, A);
  if (F & FZ) { CYCT(0x57b7, 0x57ba); goto checkSpawnNextTokay; } // jp z
  CYC(0x57b7, 0x57ba);
  CYC(0x57ba, 0x57bd); SET_HL(wDisabledObjects);
  CYC(0x57bd, 0x57bf); mem_wr(gb, HL, 0x01); // DISABLE_LINK
  CYC(0x57bf, 0x57c0); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x57c0, 0x57c2); goto lostGame; } // jr z
  CYC(0x57c0, 0x57c2);

// interactionCode70@substate4@wonGame
  CYC(0x57c2, 0x57c4); A = 0x8b; // SND_FILLED_HEART_CONTAINER
  CALL_C(0x57c4, playSound_b00_hook, 0x0c98, 0x57c7);
  CYC(0x57c7, 0x57c9); goto l57d2; // jr

lostGame:
  CYC(0x57c9, 0x57ca); A = alu_dec8(gb, A);
  CYC(0x57ca, 0x57cc); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x57cc, 0x57cd); mem_wr(gb, DE, A);
  CYC(0x57cd, 0x57cf); A = 0x5a; // SND_ERROR
  CALL_C(0x57cf, playSound_b00_hook, 0x0c98, 0x57d2);

l57d2:
  CALL_C(0x57d2, interactionIncSubstate_hook, 0x23e5, 0x57d5);
  CYC(0x57d5, 0x57d7); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x57d7, 0x57d9); mem_wr(gb, HL, 0x1e);
  RET(0x57d9); return; // ret

substate5:
  CALL_C(0x57da, interactionDecCounter1_hook, 0x23cc, 0x57dd);
  if (!(F & FZ)) { RET_TAKEN(0x57dd); return; } // ret nz
  CYC(0x57dd, 0x57de);
  CYC(0x57de, 0x57e0); mem_wr(gb, HL, 0x3c);
  CALL_C(0x57e0, interactionIncSubstate_hook, 0x23e5, 0x57e3);
  CYC(0x57e3, 0x57e6); A = W8(wTmpcfc0_wildTokay_inPresent);
  CYC(0x57e6, 0x57e7); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x57e7); return; } // ret nz
  CYC(0x57e7, 0x57e8);
  CYC(0x57e8, 0x57e9); H = D;
  CYC(0x57e9, 0x57eb); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x57eb, 0x57ed); mem_wr(gb, HL, 0x14);
  CYC(0x57ed, 0x57f0); SET_BC(0x0a18); // TX_0a18
  CYC(0x57f0, 0x57f2); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x57f2, 0x57f3); A = mem_rd(gb, HL);
  CYC(0x57f3, 0x57f4); alu_add(gb, C);
  CYC(0x57f4, 0x57f5); C = A;
  CYC(0x57f5, 0x57f8); showText_hook(gb); return; // jp

substate6:
  CYC(0x57f8, 0x57fb); A = W8(wTmpcfc0_wildTokay_inPresent);
  CYC(0x57fb, 0x57fc); alu_or(gb, A);
  if (F & FZ) { CYCT(0x57fc, 0x57fe); goto l5804; } // jr z
  CYC(0x57fc, 0x57fe);
  CALL_C(0x57fe, interactionDecCounter1_hook, 0x23cc, 0x5801);
  if (!(F & FZ)) { RET_TAKEN(0x5801); return; } // ret nz
  CYC(0x5801, 0x5802);
  CYC(0x5802, 0x5804); goto l5808; // jr

l5804:
  CALL_C(0x5804, interactionDecCounter1IfTextNotActive_hook, 0x273c, 0x5807);
  if (!(F & FZ)) { RET_TAKEN(0x5807); return; } // ret nz
  CYC(0x5807, 0x5808);

l5808:
  // Restore inventory
  CYC(0x5808, 0x580b); SET_HL(wInventoryB);
  CYC(0x580b, 0x580d); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x580d, 0x580e); A = mem_rd(gb, DE);
  CYC(0x580e, 0x580f); E = alu_inc8(gb, E);
  CYC(0x580f, 0x5810); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x5810, 0x5811); A = mem_rd(gb, DE);
  CYC(0x5811, 0x5812); mem_wr(gb, HL, A);
  CALL_C(0x5812, getThisRoomFlags_hook, 0x197d, 0x5815);
  CYC(0x5815, 0x5817); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40); // set 6,(hl)
  CYC(0x5817, 0x5819); A = 0xff;
  CYC(0x5819, 0x581c); W8(wActiveMusic) = A;
  CYC(0x581c, 0x581f); SET_HL(0x582b); // @@pastWarpDest
  CYC(0x581f, 0x5822); A = W8(wTmpcfc0_wildTokay_inPresent);
  CYC(0x5822, 0x5823); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5823, 0x5825); goto l5828; } // jr z
  CYC(0x5823, 0x5825);
  CYC(0x5825, 0x5828); SET_HL(0x5830); // @@presentWarpDest

l5828:
  CYC(0x5828, 0x582b); setWarpDestVariables_hook(gb); return; // jp

checkSpawnNextTokay:
  CALL_C(0x5835, interactionDecCounter1_hook, 0x23cc, 0x5838);
  if (!(F & FZ)) { RET_TAKEN(0x5838); return; } // ret nz
  CYC(0x5838, 0x5839);
  CYC(0x5839, 0x583b); mem_wr(gb, HL, 0x3c);
  CYC(0x583b, 0x583d); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x583d, 0x583e); A = mem_rd(gb, HL);
  CYC(0x583e, 0x583f); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x583f); return; } // ret z
  CYC(0x583f, 0x5840);
  CYC(0x5840, 0x5842); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5842, 0x5843); A = mem_rd(gb, HL);
  CYC(0x5843, 0x5844); alu_add(gb, A);
  CYC(0x5844, 0x5847); SET_BC(0x5898); // @data_5898
  CALL_C(0x5847, addDoubleIndexToBc_hook, 0x007e, 0x584a);
  CYC(0x584a, 0x584c); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x584c, 0x584d); A = mem_rd(gb, HL);
  CYC(0x584d, 0x584f); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(0x584f, 0x5851); goto decVar3b; } // jr z
  CYC(0x584f, 0x5851);
  CYC(0x5851, 0x5852); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CALL_C(0x5852, addAToBc_hook, 0x006d, 0x5855);
  CYC(0x5855, 0x5856); A = mem_rd(gb, BC);
  CYC(0x5856, 0x5857); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x5857); return; } // ret z
  CYC(0x5857, 0x5858);
  CYC(0x5858, 0x5859); C = A;
  CYC(0x5859, 0x585b); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x585b, 0x585c); A = mem_rd(gb, HL);
  CYC(0x585c, 0x585d); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x585d, 0x585f); goto loadTokay; } // jr nz
  CYC(0x585d, 0x585f);
  CYC(0x585f, 0x5861); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5861, 0x5862); A = mem_rd(gb, HL);
  CYC(0x5862, 0x5864); B = 0x03;
  CYC(0x5864, 0x5866); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(0x5866, 0x5868); goto l586d; } // jr z
  CYC(0x5866, 0x5868);
  CYC(0x5868, 0x586a); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(0x586a, 0x586c); goto l586d; } // jr z
  CYC(0x586a, 0x586c);
  CYC(0x586c, 0x586d); B = alu_inc8(gb, B);

l586d:
  CYC(0x586d, 0x586f); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x586f, 0x5870); A = mem_rd(gb, HL);
  CYC(0x5870, 0x5871); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x5871, 0x5873); goto loadTokay; } // jr nz
  CYC(0x5871, 0x5873);
  CYC(0x5873, 0x5876); SET_HL(wTmpcfc0_wildTokay_cfdf);
  CYC(0x5876, 0x5877); mem_wr(gb, HL, B);

loadTokay:
  CYC(0x5877, 0x5878); B = C;
  CALL_C(0x5878, getWildTokayObjectDataIndex_hook, 0x3e0f, 0x587b);
  CYC(0x587b, 0x587e); parseGivenObjectData_b00_hook(gb); return; // jp

decVar3b:
  CYC(0x587e, 0x5880); mem_wr(gb, HL, 0x00);
  CYC(0x5880, 0x5882); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x5882, 0x5883); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)

getRandomVar39Value:
  CYC(0x5883, 0x5886); SET_HL(wTmpcfc0_wildTokay_cfdc);
  CYC(0x5886, 0x5887); A = mem_rd(gb, HL);
  CYC(0x5887, 0x5889); alu_swap_a(gb);
  CYC(0x5889, 0x588c); SET_HL(0x58b8); // @table
  CYC(0x588c, 0x588d); wildTokayController_addAToHl_from_rst(gb, 0x588d);
  CALL_C(0x588d, getRandomNumber_hook, 0x043e, 0x5890);
  CYC(0x5890, 0x5892); alu_and(gb, 0x0f);
  CYC(0x5892, 0x5893); wildTokayController_addAToHl_from_rst(gb, 0x5893);
  CYC(0x5893, 0x5894); A = mem_rd(gb, HL);
  CYC(0x5894, 0x5896); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5896, 0x5897); mem_wr(gb, DE, A);
  RET(0x5897); return; // ret
}
