#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodece_jump_table(GB *gb) {
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

// rst_addAToHl (rst $10): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCodece_addAToHl(GB *gb, uint16_t return_address) {
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

// rst_addDoubleIndex (rst $18): self-contained push+compute+ret_effect, balanced regardless of caller.
static void interactionCodece_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_BUSINESS_SCRUB
//
// Variables:
//   var38: Number of rupees to spend (1-byte value, converted with "rupeeValue" methods)
//   var39: Set when Link is close to the scrub (he pops out of his bush)
//
// Every internal label below is reached only via jr/jp within this one routine (the ages build
// strips the ROM_SEASONS-only block at the top of @state0 entirely) -- none are entered via a
// real `call`, so this whole interaction is a single C function with goto labels.
// ==================================================================================================
void interactionCodece_hook(GB *gb) {
  BASE(interactionCodece);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodece_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+117) { goto state1; }
    else if (jt_ == b_+218) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CALL_C(b_+13, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+16);
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+21); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto mimicBush; } // jr nz
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); alu_cp(gb, 0x00);
  if (F & FZ) { CYCT(b_+25, b_+27); goto sellingShield; } // jr z
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+29, b_+31); goto sellingShield; } // jr z
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); alu_cp(gb, 0x06);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto afterSellingShield; } // jr nz
  CYC(b_+33, b_+35);

sellingShield:
  CYC(b_+35, b_+36); C = A;
  CYC(b_+36, b_+39); A = mem_rd(gb, wShieldLevel);
  CYC(b_+39, b_+40); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+40, b_+42); goto sellingShieldSetPrice; } // jr z
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); A = alu_dec8(gb, A);

sellingShieldSetPrice:
  CYC(b_+43, b_+44); alu_add(gb, C);
  CYC(b_+44, b_+45); mem_wr(gb, DE, A);
  CYC(b_+45, b_+48); SET_HL(b_+412); // @itemPrices
  CYC(b_+48, b_+49); interactionCodece_addDoubleIndex(gb, b_+49);
  CYC(b_+49, b_+50); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+50, b_+51); B = mem_rd(gb, HL);
  CYC(b_+51, b_+54); SET_HL(wTextNumberSubstitution);
  CYC(b_+54, b_+55); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+55, b_+56); mem_wr(gb, HL, B);

afterSellingShield:
  CYC(b_+56, b_+58); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+58, b_+60); A = 0x06;
  CYC(b_+60, b_+61); mem_wr(gb, DE, A);
  CYC(b_+61, b_+62); E = alu_inc8(gb, E);
  CYC(b_+62, b_+63); mem_wr(gb, DE, A);
  CALL_C(b_+63, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+66);
  CALL_C(b_+66, objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+69);
  CYC(b_+69, b_+71); H = 0xcf; // >wRoomLayout
  CYC(b_+71, b_+73); mem_wr(gb, HL, 0x00);
  CALL_C(b_+73, objectSetVisible80_hook, SYM(objectSetVisible80), b_+76);
  CYC(b_+76, b_+78); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+78, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+81);
  CALL_C(b_+81, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+84);
  CYC(b_+84, b_+86); A = 0xce; // INTERAC_BUSINESS_SCRUB
  CYC(b_+86, b_+87); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+87, b_+89); A = 0x80;
  CYC(b_+89, b_+90); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+90, b_+92); L = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+92, b_+93); mem_wr(gb, HL, D);
  CYC(b_+93, b_+96); TAIL(objectCopyPosition); // jp

// Subid $80 initialization (the bush above the scrub)
mimicBush:
  CYC(b_+96, b_+99); A = mem_rd(gb, wActiveGroup);
  CYC(b_+99, b_+100); alu_or(gb, A);
  CYC(b_+100, b_+102); A = 0xc5; // TILEINDEX_OVERWORLD_BUSH_1
  if (F & FZ) { CYCT(b_+102, b_+104); goto mimicBushSetTile; } // jr z
  CYC(b_+102, b_+104);
  CYC(b_+104, b_+106); A = 0xc5; // TILEINDEX_OVERWORLD_BUSH_1

mimicBushSetTile:
  CALL_C(b_+106, objectMimicBgTile_hook, SYM(objectMimicBgTile), b_+109);
  CYC(b_+109, b_+111); A = 0x05;
  CALL_C(b_+111, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+114);
  CYC(b_+114, b_+117); TAIL(objectSetVisible80); // jp

state1:
  CYC(b_+117, b_+120); A = mem_rd(gb, wScrollMode);
  CYC(b_+120, b_+122); alu_and(gb, 0x0e); // SCROLLMODE_08 | SCROLLMODE_04 | SCROLLMODE_02
  if (!(F & FZ)) { CYCT(b_+122, b_+123); ret_effect(gb); return; } // ret nz
  CYC(b_+122, b_+123);
  CYC(b_+123, b_+125); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+128); alu_bit(gb, 7, A);
  if (!(F & FZ)) { CYCT(b_+128, b_+130); goto subid80State1; } // jr nz
  CYC(b_+128, b_+130);
  CALL_C(b_+130, objectSetPriorityRelativeToLink_withTerrainEffects_hook, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+133);
  CALL_C(b_+133, interactionAnimate_hook, SYM(interactionAnimate), b_+136);
  CYC(b_+136, b_+138); C = 0x20;
  CALL_C(b_+138, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+141);
  CYC(b_+141, b_+143); E = INTERACTION_BASE + OBJ_VAR39;
  if (F & FC) { CYCT(b_+143, b_+145); goto linkIsClose; } // jr c
  CYC(b_+143, b_+145);

  // Link not close
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+147); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+147, b_+148); ret_effect(gb); return; } // ret z
  CYC(b_+147, b_+148);
  CYC(b_+148, b_+149); alu_xor(gb, A);
  CYC(b_+149, b_+150); mem_wr(gb, DE, A);
  CYC(b_+150, b_+152); A = 0x03;
  CYC(b_+152, b_+155); TAIL(interactionSetAnimation); // jp

linkIsClose:
  CYC(b_+155, b_+156); A = mem_rd(gb, DE);
  CYC(b_+156, b_+157); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+157, b_+159); goto afterTalkedCheck; } // jr nz
  CYC(b_+157, b_+159);
  CYC(b_+159, b_+160); A = alu_inc8(gb, A);
  CYC(b_+160, b_+161); mem_wr(gb, DE, A);
  CYC(b_+161, b_+163); A = 0x01;
  CYC(b_+163, b_+166); TAIL(interactionSetAnimation); // jp

afterTalkedCheck:
  CYC(b_+166, b_+168); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+168, b_+169); A = mem_rd(gb, DE);
  CYC(b_+169, b_+170); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+170, b_+171); ret_effect(gb); return; } // ret z
  CYC(b_+170, b_+171);

  // Link talked to the scrub
  CALL_C(b_+171, interactionIncState_hook, SYM(interactionIncState), b_+174);
  CYC(b_+174, b_+176); A = 0x02;
  CALL_C(b_+176, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+179);
  CYC(b_+179, b_+181); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+181, b_+182); A = mem_rd(gb, DE);
  CYC(b_+182, b_+185); SET_HL(b_+369); // @offerItemTextIndices
  CYC(b_+185, b_+186); interactionCodece_addAToHl(gb, b_+186);
  CYC(b_+186, b_+187); C = mem_rd(gb, HL);
  CYC(b_+187, b_+189); B = 0x45; // >TX_4500
  CYC(b_+189, b_+192); TAIL(showTextNonExitable); // jp

// Subid $80: the bush above the scrub
subid80State1:
  CYC(b_+192, b_+194); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+194, b_+195); A = mem_rd(gb, DE);
  CYC(b_+195, b_+196); H = A;
  CYC(b_+196, b_+198); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+198, b_+199); A = mem_rd(gb, HL);
  CYC(b_+199, b_+201); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+201, b_+202); mem_wr(gb, DE, A);
  CYC(b_+202, b_+204); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+204, b_+205); B = mem_rd(gb, HL);
  CYC(b_+205, b_+207); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+207, b_+208); A = mem_rd(gb, HL);
  CYC(b_+208, b_+211); SET_HL(b_+378); // @bushYOffsets
  CYC(b_+211, b_+212); interactionCodece_addAToHl(gb, b_+212);
  CYC(b_+212, b_+214); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+214, b_+215); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+215, b_+216); alu_add(gb, B);
  CYC(b_+216, b_+217); mem_wr(gb, DE, A);
  RET(b_+217); return;

state2:
  CALL_C(b_+218, interactionAnimate_hook, SYM(interactionAnimate), b_+221);
  CYC(b_+221, b_+224); A = mem_rd(gb, wTextIsActive);
  CYC(b_+224, b_+226); alu_and(gb, 0x7f);
  if (!(F & FZ)) { CYCT(b_+226, b_+227); ret_effect(gb); return; } // ret nz
  CYC(b_+226, b_+227);

  // Link just finished talking to the scrub
  CYC(b_+227, b_+230); A = mem_rd(gb, wSelectedTextOption);
  CYC(b_+230, b_+232); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+232, b_+234); goto label_0b_103; } // jr z
  CYC(b_+232, b_+234);
  CYC(b_+234, b_+236); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+236, b_+238); A = 0x01;
  CYC(b_+238, b_+239); mem_wr(gb, DE, A);
  CYC(b_+239, b_+240); alu_xor(gb, A);
  CYC(b_+240, b_+243); mem_wr(gb, wTextIsActive, A);
  CYC(b_+243, b_+245); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+245, b_+246); mem_wr(gb, DE, A);
  CYC(b_+246, b_+247); A = alu_dec8(gb, A);
  CYC(b_+247, b_+250); mem_wr(gb, wSelectedTextOption, A);
  CYC(b_+250, b_+252); A = 0x04;
  CYC(b_+252, b_+255); TAIL(interactionSetAnimation); // jp

label_0b_103:
  CYC(b_+255, b_+258); A = mem_rd(gb, wSelectedTextOption);
  CYC(b_+258, b_+259); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+259, b_+261); goto agreedToBuy; } // jr z
  CYC(b_+259, b_+261);

  // Declined to buy
  CYC(b_+261, b_+264); SET_BC((SYM(interactionCodeb6__state4) + 23)); // TX_4506
  CYC(b_+264, b_+266); goto showTextLabel; // jr $4ac8

agreedToBuy:
  CYC(b_+266, b_+268); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+268, b_+269); A = mem_rd(gb, DE);
  CYC(b_+269, b_+272); SET_HL(b_+381); // @rupeeValues
  CYC(b_+272, b_+273); interactionCodece_addAToHl(gb, b_+273);
  CYC(b_+273, b_+274); A = mem_rd(gb, HL);
  CYC(b_+274, b_+276); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+276, b_+277); mem_wr(gb, DE, A);
  CALL_C(b_+277, cpRupeeValue_hook, SYM(cpRupeeValue), b_+280);
  if (F & FZ) { CYCT(b_+280, b_+282); goto enoughRupees; } // jr z
  CYC(b_+280, b_+282);

  // Not enough rupees
  CYC(b_+282, b_+285); SET_BC((SYM(interactionCodeb6__state4) + 24)); // TX_4507
  CYC(b_+285, b_+287); goto showTextLabel; // jr $4ac8

enoughRupees:
  CYC(b_+287, b_+289); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+289, b_+290); A = mem_rd(gb, DE);
  CYC(b_+290, b_+293); SET_HL(b_+390); // @treasuresToSell
  CYC(b_+293, b_+294); interactionCodece_addDoubleIndex(gb, b_+294);
  CYC(b_+294, b_+295); A = mem_rd(gb, HL);
  CYC(b_+295, b_+297); alu_cp(gb, 0x03); // TREASURE_BOMBS
  if (F & FZ) { CYCT(b_+297, b_+299); goto giveBombs; } // jr z
  CYC(b_+297, b_+299);
  CYC(b_+299, b_+301); alu_cp(gb, 0x20); // TREASURE_EMBER_SEEDS
  if (!(F & FZ)) { CYCT(b_+301, b_+303); goto giveShield; } // jr nz
  CYC(b_+301, b_+303);

  // giveEmberSeeds (falls through, never a goto target)
  CYC(b_+303, b_+306); A = mem_rd(gb, wSeedSatchelLevel);
  CYC(b_+306, b_+309); SET_BC(b_+365); // @maxSatchelCapacities-1
  CALL_C(b_+309, addAToBc_hook, 0x006d, b_+312);
  CYC(b_+312, b_+313); A = mem_rd(gb, BC);
  CYC(b_+313, b_+314); C = A;
  CYC(b_+314, b_+317); A = mem_rd(gb, wNumEmberSeeds);
  CYC(b_+317, b_+318); alu_cp(gb, C);
  if (!(F & FZ)) { CYCT(b_+318, b_+320); goto giveTreasure; } // jr nz
  CYC(b_+318, b_+320);
  CYC(b_+320, b_+322); goto alreadyHaveTreasure; // jr $4ab0

giveBombs:
  CYC(b_+322, b_+325); SET_BC(wNumBombs);
  CYC(b_+325, b_+326); A = mem_rd(gb, BC);
  CYC(b_+326, b_+327); C = alu_inc8(gb, C);
  CYC(b_+327, b_+328); E = A;
  CYC(b_+328, b_+329); A = mem_rd(gb, BC);
  CYC(b_+329, b_+330); alu_cp(gb, E);
  if (!(F & FZ)) { CYCT(b_+330, b_+332); goto giveTreasure; } // jr nz
  CYC(b_+330, b_+332);
  CYC(b_+332, b_+334); goto alreadyHaveTreasure; // jr $4ab0

giveShield:
  CALL_C(b_+334, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+337);
  if (!(F & FC)) { CYCT(b_+337, b_+339); goto giveTreasure; } // jr nc
  CYC(b_+337, b_+339);

alreadyHaveTreasure:
  CYC(b_+339, b_+342); SET_BC((SYM(interactionCodeb6__state4) + 25)); // TX_4508
  CYC(b_+342, b_+344); goto showTextLabel; // jr $4ac8

giveTreasure:
  CYC(b_+344, b_+345); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+345, b_+346); C = mem_rd(gb, HL);
  CALL_C(b_+346, giveTreasure_hook, SYM(giveTreasure), b_+349);
  CYC(b_+349, b_+351); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+351, b_+352); A = mem_rd(gb, DE);
  CALL_C(b_+352, removeRupeeValue_hook, SYM(removeRupeeValue), b_+355);
  CYC(b_+355, b_+357); A = 0x5e; // SND_GETSEED
  CALL_C(b_+357, playSound_b00_hook, SYM(playSound_b00), b_+360);
  CYC(b_+360, b_+363); SET_BC((SYM(interactionCodeb6__state4) + 22)); // TX_4505

showTextLabel:
  CYC(b_+363, b_+366); TAIL(showText); // jp
}
