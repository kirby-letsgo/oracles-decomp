#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode70), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode70), (from), (to), true)

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
  BASE(interactionCode70);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = wildTokayController_jump_table(gb);
    if (target == b_+51) goto state1;
  }

  CYC(b_+8, b_+9); alu_xor(gb, A);
  CYC(b_+9, b_+12); SET_HL(wTmpcfc0_wildTokay_cfde);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CALL_C(b_+14, interactionIncState_hook, SYM(interactionIncState), b_+17);
  CYC(b_+17, b_+20); A = W8(wWildTokayGameLevel);
  CYC(b_+20, b_+21); B = A;
  CYC(b_+21, b_+24); A = W8(wTmpcfc0_wildTokay_inPresent);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+25, b_+27); goto l5705; } // jr z
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); B = 0x02;

l5705:
  CYC(b_+29, b_+30); A = B;
  CYC(b_+30, b_+33); W8(wTmpcfc0_wildTokay_cfdc) = A;
  CYC(b_+33, b_+36); SET_BC(b_+46); // @var3bValues
  CALL_C(b_+36, addAToBc_hook, 0x006d, b_+39);
  CYC(b_+39, b_+40); A = mem_rd(gb, BC);
  CYC(b_+40, b_+42); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+42, b_+43); mem_wr(gb, DE, A);
  CYC(b_+43, b_+46); goto getRandomVar39Value; // jp

state1:
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  {
    CYC(b_+54, b_+55); push_effect(gb, b_+55);
    uint16_t target = wildTokayController_jump_table(gb);
    if (target == b_+144) goto substate1;
    if (target == b_+163) goto substate2;
    if (target == b_+180) goto substate3;
    if (target == b_+203) goto substate4;
    if (target == b_+242) goto substate5;
    if (target == b_+272) goto substate6;
  }

  CYC(b_+69, b_+72); A = W8(wPaletteThread_mode);
  CYC(b_+72, b_+73); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+73); return; } // ret nz
  CYC(b_+73, b_+74);
  // Save Link's equipped items
  CYC(b_+74, b_+77); SET_HL(wInventoryB);
  CYC(b_+77, b_+79); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+79, b_+80); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+80, b_+81); mem_wr(gb, DE, A);
  CYC(b_+81, b_+82); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+82, b_+83); E = alu_inc8(gb, E);
  CYC(b_+83, b_+84); mem_wr(gb, DE, A);
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x00); // ITEM_NONE
  CYC(b_+86, b_+87); L = alu_inc8(gb, L);
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x16); // ITEM_BRACELET
  // Replace tiles to start game
  CYC(b_+89, b_+91); B = 0x06;
  CYC(b_+91, b_+94); SET_HL(b_+132); // @tilesToReplaceOnStart

nextTile:
  CYC(b_+94, b_+95); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+95, b_+96); C = mem_rd(gb, HL);
  CYC(b_+96, b_+97); SET_HL(HL + 1);
  CYC(b_+97, b_+98); push_effect(gb, BC);
  CYC(b_+98, b_+99); push_effect(gb, HL);
  CALL_C(b_+99, setTile_hook, SYM(setTile), b_+102);
  CYC(b_+102, b_+103); SET_HL(pop_effect(gb));
  CYC(b_+103, b_+104); SET_BC(pop_effect(gb));
  CYC(b_+104, b_+105); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+105, b_+107); goto nextTile; } // jr nz
  CYC(b_+105, b_+107);
  CALL_C(b_+107, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+110);
  CYC(b_+110, b_+112); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+112, b_+114); mem_wr(gb, HL, 30);
  CYC(b_+114, b_+117); SET_HL(w1Link_yh); // w1Link.yh
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x48);
  CYC(b_+119, b_+121); L = 0x0d; // <w1Link.xh
  CYC(b_+121, b_+123); mem_wr(gb, HL, 0x50);
  CYC(b_+123, b_+124); alu_xor(gb, A);
  CYC(b_+124, b_+126); L = 0x08; // <w1Link.direction
  CYC(b_+126, b_+127); mem_wr(gb, HL, A);
  CYC(b_+127, b_+128); A = alu_dec8(gb, A);
  CYC(b_+128, b_+131); W8(wStatusBarNeedsRefresh) = A;
  RET(b_+131); return; // ret

substate1:
  CALL_C(b_+144, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+147);
  if (!(F & FZ)) { RET_TAKEN(b_+147); return; } // ret nz
  CYC(b_+147, b_+148);
  CALL_C(b_+148, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+151);
  CYC(b_+151, b_+153); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+153, b_+155); mem_wr(gb, HL, 0x0a);
  CYC(b_+155, b_+157); A = 0x02; // MUS_MINIGAME
  CALL_C(b_+157, playSound_b00_hook, SYM(playSound_b00), b_+160);
  CYC(b_+160, b_+163); fadeinFromWhite_hook(gb); return; // jp

substate2:
  CALL_C(b_+163, interactionDecCounter1IfPaletteNotFading_hook, SYM(interactionDecCounter1IfPaletteNotFading), b_+166);
  if (!(F & FZ)) { RET_TAKEN(b_+166); return; } // ret nz
  CYC(b_+166, b_+167);
  CALL_C(b_+167, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+170);
  CYC(b_+170, b_+171); alu_xor(gb, A);
  CYC(b_+171, b_+174); W8(wDisabledObjects) = A;
  CYC(b_+174, b_+177); SET_BC((SYM(vblankInterrupt) + 30)); // TX_0a16
  CYC(b_+177, b_+180); showText_hook(gb); return; // jp

substate3:
  CYC(b_+180, b_+183); A = W8(wTextIsActive);
  CYC(b_+183, b_+184); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+184); return; } // ret nz
  CYC(b_+184, b_+185);
  CALL_C(b_+185, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+188);
  CYC(b_+188, b_+190); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+190, b_+192); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+192, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+195);
  if (!(F & FZ)) { RET_TAKEN(b_+195); return; } // ret nz
  CYC(b_+195, b_+196);
  CYC(b_+196, b_+198); mem_wr(gb, HL, 0x8c); // INTERAC_TOKAY_MEAT
  CYC(b_+198, b_+200); A = 0xcc; // SND_WHISTLE
  CYC(b_+200, b_+203); playSound_b00_hook(gb); return; // jp

substate4:
  CYC(b_+203, b_+206); A = W8(wTmpcfc0_wildTokay_cfde);
  CYC(b_+206, b_+207); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+207, b_+210); goto checkSpawnNextTokay; } // jp z
  CYC(b_+207, b_+210);
  CYC(b_+210, b_+213); SET_HL(wDisabledObjects);
  CYC(b_+213, b_+215); mem_wr(gb, HL, 0x01); // DISABLE_LINK
  CYC(b_+215, b_+216); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+216, b_+218); goto lostGame; } // jr z
  CYC(b_+216, b_+218);

// interactionCode70@substate4@wonGame
  CYC(b_+218, b_+220); A = 0x8b; // SND_FILLED_HEART_CONTAINER
  CALL_C(b_+220, playSound_b00_hook, SYM(playSound_b00), b_+223);
  CYC(b_+223, b_+225); goto l57d2; // jr

lostGame:
  CYC(b_+225, b_+226); A = alu_dec8(gb, A);
  CYC(b_+226, b_+228); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+228, b_+229); mem_wr(gb, DE, A);
  CYC(b_+229, b_+231); A = 0x5a; // SND_ERROR
  CALL_C(b_+231, playSound_b00_hook, SYM(playSound_b00), b_+234);

l57d2:
  CALL_C(b_+234, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+237);
  CYC(b_+237, b_+239); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+239, b_+241); mem_wr(gb, HL, 0x1e);
  RET(b_+241); return; // ret

substate5:
  CALL_C(b_+242, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+245);
  if (!(F & FZ)) { RET_TAKEN(b_+245); return; } // ret nz
  CYC(b_+245, b_+246);
  CYC(b_+246, b_+248); mem_wr(gb, HL, 0x3c);
  CALL_C(b_+248, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+251);
  CYC(b_+251, b_+254); A = W8(wTmpcfc0_wildTokay_inPresent);
  CYC(b_+254, b_+255); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+255); return; } // ret nz
  CYC(b_+255, b_+256);
  CYC(b_+256, b_+257); H = D;
  CYC(b_+257, b_+259); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+259, b_+261); mem_wr(gb, HL, 0x14);
  CYC(b_+261, b_+264); SET_BC((SYM(vblankInterrupt) + 32)); // TX_0a18
  CYC(b_+264, b_+266); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+266, b_+267); A = mem_rd(gb, HL);
  CYC(b_+267, b_+268); alu_add(gb, C);
  CYC(b_+268, b_+269); C = A;
  CYC(b_+269, b_+272); showText_hook(gb); return; // jp

substate6:
  CYC(b_+272, b_+275); A = W8(wTmpcfc0_wildTokay_inPresent);
  CYC(b_+275, b_+276); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+276, b_+278); goto l5804; } // jr z
  CYC(b_+276, b_+278);
  CALL_C(b_+278, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+281);
  if (!(F & FZ)) { RET_TAKEN(b_+281); return; } // ret nz
  CYC(b_+281, b_+282);
  CYC(b_+282, b_+284); goto l5808; // jr

l5804:
  CALL_C(b_+284, interactionDecCounter1IfTextNotActive_hook, SYM(interactionDecCounter1IfTextNotActive), b_+287);
  if (!(F & FZ)) { RET_TAKEN(b_+287); return; } // ret nz
  CYC(b_+287, b_+288);

l5808:
  // Restore inventory
  CYC(b_+288, b_+291); SET_HL(wInventoryB);
  CYC(b_+291, b_+293); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+293, b_+294); A = mem_rd(gb, DE);
  CYC(b_+294, b_+295); E = alu_inc8(gb, E);
  CYC(b_+295, b_+296); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+296, b_+297); A = mem_rd(gb, DE);
  CYC(b_+297, b_+298); mem_wr(gb, HL, A);
  CALL_C(b_+298, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+301);
  CYC(b_+301, b_+303); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40); // set 6,(hl)
  CYC(b_+303, b_+305); A = 0xff;
  CYC(b_+305, b_+308); W8(wActiveMusic) = A;
  CYC(b_+308, b_+311); SET_HL(b_+323); // @@pastWarpDest
  CYC(b_+311, b_+314); A = W8(wTmpcfc0_wildTokay_inPresent);
  CYC(b_+314, b_+315); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+315, b_+317); goto l5828; } // jr z
  CYC(b_+315, b_+317);
  CYC(b_+317, b_+320); SET_HL(b_+328); // @@presentWarpDest

l5828:
  CYC(b_+320, b_+323); setWarpDestVariables_hook(gb); return; // jp

checkSpawnNextTokay:
  CALL_C(b_+333, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+336);
  if (!(F & FZ)) { RET_TAKEN(b_+336); return; } // ret nz
  CYC(b_+336, b_+337);
  CYC(b_+337, b_+339); mem_wr(gb, HL, 0x3c);
  CYC(b_+339, b_+341); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+341, b_+342); A = mem_rd(gb, HL);
  CYC(b_+342, b_+343); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+343); return; } // ret z
  CYC(b_+343, b_+344);
  CYC(b_+344, b_+346); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+346, b_+347); A = mem_rd(gb, HL);
  CYC(b_+347, b_+348); alu_add(gb, A);
  CYC(b_+348, b_+351); SET_BC(b_+432); // @data_5898
  CALL_C(b_+351, addDoubleIndexToBc_hook, 0x007e, b_+354);
  CYC(b_+354, b_+356); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+356, b_+357); A = mem_rd(gb, HL);
  CYC(b_+357, b_+359); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+359, b_+361); goto decVar3b; } // jr z
  CYC(b_+359, b_+361);
  CYC(b_+361, b_+362); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CALL_C(b_+362, addAToBc_hook, 0x006d, b_+365);
  CYC(b_+365, b_+366); A = mem_rd(gb, BC);
  CYC(b_+366, b_+367); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+367); return; } // ret z
  CYC(b_+367, b_+368);
  CYC(b_+368, b_+369); C = A;
  CYC(b_+369, b_+371); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+371, b_+372); A = mem_rd(gb, HL);
  CYC(b_+372, b_+373); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+373, b_+375); goto loadTokay; } // jr nz
  CYC(b_+373, b_+375);
  CYC(b_+375, b_+377); L = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+377, b_+378); A = mem_rd(gb, HL);
  CYC(b_+378, b_+380); B = 0x03;
  CYC(b_+380, b_+382); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+382, b_+384); goto l586d; } // jr z
  CYC(b_+382, b_+384);
  CYC(b_+384, b_+386); alu_cp(gb, 0x06);
  if (F & FZ) { CYCT(b_+386, b_+388); goto l586d; } // jr z
  CYC(b_+386, b_+388);
  CYC(b_+388, b_+389); B = alu_inc8(gb, B);

l586d:
  CYC(b_+389, b_+391); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+391, b_+392); A = mem_rd(gb, HL);
  CYC(b_+392, b_+393); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+393, b_+395); goto loadTokay; } // jr nz
  CYC(b_+393, b_+395);
  CYC(b_+395, b_+398); SET_HL(wTmpcfc0_wildTokay_cfdf);
  CYC(b_+398, b_+399); mem_wr(gb, HL, B);

loadTokay:
  CYC(b_+399, b_+400); B = C;
  CALL_C(b_+400, getWildTokayObjectDataIndex_hook, SYM(getWildTokayObjectDataIndex), b_+403);
  CYC(b_+403, b_+406); parseGivenObjectData_b00_hook(gb); return; // jp

decVar3b:
  CYC(b_+406, b_+408); mem_wr(gb, HL, 0x00);
  CYC(b_+408, b_+410); L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+410, b_+411); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)

getRandomVar39Value:
  CYC(b_+411, b_+414); SET_HL(wTmpcfc0_wildTokay_cfdc);
  CYC(b_+414, b_+415); A = mem_rd(gb, HL);
  CYC(b_+415, b_+417); alu_swap_a(gb);
  CYC(b_+417, b_+420); SET_HL(b_+464); // @table
  CYC(b_+420, b_+421); wildTokayController_addAToHl_from_rst(gb, b_+421);
  CALL_C(b_+421, getRandomNumber_hook, SYM(getRandomNumber), b_+424);
  CYC(b_+424, b_+426); alu_and(gb, 0x0f);
  CYC(b_+426, b_+427); wildTokayController_addAToHl_from_rst(gb, b_+427);
  CYC(b_+427, b_+428); A = mem_rd(gb, HL);
  CYC(b_+428, b_+430); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+430, b_+431); mem_wr(gb, DE, A);
  RET(b_+431); return; // ret
}
