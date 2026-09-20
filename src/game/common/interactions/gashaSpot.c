#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodeb6_jump_table(GB *gb) {
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

static void interactionCodeb6_addAToHl(GB *gb, uint16_t return_address) {
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

static void interactionCodeb6_addDoubleIndex(GB *gb, uint16_t return_address) {
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
// INTERAC_GASHA_SPOT
// ==================================================================================================
void interactionCodeb6_hook(GB *gb) {
  BASE(interactionCodeb6);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodeb6_jump_table(gb));
    if (jt_ == b_+20) { goto state0; }
    else if (jt_ == b_+134) { goto state1; }
    else if (jt_ == b_+158) { goto state2; }
    else if (jt_ == b_+207) { goto state3; }
    else if (jt_ == b_+250) { goto state4; }
    else if (jt_ == b_+283) { goto state5; }
    else if (jt_ == b_+467) { goto state6; }
    else if (jt_ == b_+622) { goto state7; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+20, b_+22); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+22, b_+23); A = mem_rd(gb, DE);
  CYC(b_+23, b_+24); E = alu_inc8(gb, E);
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+28); SET_HL(wGashaSpotsPlantedBitset);
  CALL_C(b_+28, checkFlag_hook, SYM(checkFlag), b_+31);
  if (!(F & FZ)) { CYCT(b_+31, b_+33); goto seedPlanted; } // jr nz
  CYC(b_+31, b_+33);
  CYC(b_+33, b_+35); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+37, b_+39); goto l_442b; } // jr nz
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); A = 0x28; // DISCOVERY_RING
  CALL_C(b_+41, cpActiveRing_hook, SYM(cpActiveRing), b_+44);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); goto l_442b; } // jr nz
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+48); A = 0xa2; // SND_COMPASS
  CYC(b_+48, b_+50); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+50, b_+51); mem_wr(gb, DE, A);
  CALL_C(b_+51, playSound_b00_hook, SYM(playSound_b00), b_+54);

l_442b:
  CALL_C(b_+54, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+57);
  CYC(b_+57, b_+59); alu_cp(gb, 0xd2); // TILEINDEX_SOFT_SOIL
  if (!(F & FZ)) { CYCT(b_+59, b_+60); ret_effect(gb); return; } // ret nz
  CYC(b_+59, b_+60);

  // @unearthed
  CALL_C(b_+60, interactionIncState_hook, SYM(interactionIncState), b_+63);
  CYC(b_+63, b_+65); A = 0x0a;
  CALL_C(b_+65, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+68);
  CYC(b_+68, b_+70); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+70, b_+73); objectAddToAButtonSensitiveObjectList_hook(gb); return; // jp

seedPlanted:
  CYC(b_+73, b_+74); A = mem_rd(gb, DE);
  CYC(b_+74, b_+77); SET_HL(wGashaSpotKillCounters);
  CYC(b_+77, b_+78); interactionCodeb6_addAToHl(gb, b_+78);
  CYC(b_+78, b_+79); A = mem_rd(gb, HL);
  CYC(b_+79, b_+81); alu_cp(gb, 40);
  if (F & FC) { CYCT(b_+81, b_+83); goto delete_; } // jr c
  CYC(b_+81, b_+83);

  // @killedEnoughEnemies
  CALL_C(b_+83, getFreePartSlot_hook, SYM(getFreePartSlot), b_+86);
  if (!(F & FZ)) { CYCT(b_+86, b_+87); ret_effect(gb); return; } // ret nz
  CYC(b_+86, b_+87);
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x17); // PART_GASHA_TREE
  CYC(b_+89, b_+90); L = alu_inc8(gb, L);
  CYC(b_+90, b_+92); mem_wr(gb, HL, 0x01);
  CYC(b_+92, b_+94); L = 0xd6; // Part.relatedObj1
  CYC(b_+94, b_+96); A = INTERACTION_BASE; // Interaction.start
  CYC(b_+96, b_+97); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+97, b_+98); mem_wr(gb, HL, D);
  CYC(b_+98, b_+99); H = D;
  CYC(b_+99, b_+101); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+101, b_+103); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+103, b_+104); A = mem_rd(gb, DE);
  CYC(b_+104, b_+105); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+105, b_+107); alu_add(gb, 0xf8);
  CYC(b_+107, b_+108); mem_wr(gb, DE, A);
  CYC(b_+108, b_+110); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+112, b_+114); alu_add(gb, 0x08);
  CYC(b_+114, b_+115); mem_wr(gb, DE, A);
  CYC(b_+115, b_+117); A = 0x04;
  CALL_C(b_+117, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+120);
  CYC(b_+120, b_+122); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+122, b_+124); mem_wr(gb, HL, 0x03);
  CYC(b_+124, b_+126); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+126, b_+128); mem_wr(gb, HL, 0x0a);
  CALL_C(b_+128, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+131);
  CYC(b_+131, b_+134); objectSetVisible83_hook(gb); return; // jp

state1:
  CYC(b_+134, b_+136); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+136, b_+137); A = mem_rd(gb, DE);
  CYC(b_+137, b_+138); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+138, b_+139); ret_effect(gb); return; } // ret z
  CYC(b_+138, b_+139);

  // @pressedAButton
  CYC(b_+139, b_+140); alu_xor(gb, A);
  CYC(b_+140, b_+141); mem_wr(gb, DE, A);
  CYC(b_+141, b_+144); SET_BC(0x3509); // TX_3509
  CYC(b_+144, b_+147); A = mem_rd(gb, wNumGashaSeeds);
  CYC(b_+147, b_+148); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+148, b_+150); goto l_4490; } // jr z
  CYC(b_+148, b_+150);
  CALL_C(b_+150, interactionIncState_hook, SYM(interactionIncState), b_+153);
  CYC(b_+153, b_+155); C = 0x00; // <TX_3500

l_4490:
  CYC(b_+155, b_+158); showText_hook(gb); return; // jp

state2:
  CYC(b_+158, b_+161); A = mem_rd(gb, wSelectedTextOption);
  CYC(b_+161, b_+162); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+162, b_+164); goto l_449d; } // jr z
  CYC(b_+162, b_+164);
  CYC(b_+164, b_+166); A = 0x01;
  CYC(b_+166, b_+167); mem_wr(gb, DE, A); // [state]
  RET(b_+167); return;

l_449d:
  CALL_C(b_+168, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+171);
  CYC(b_+171, b_+172); C = L;
  CYC(b_+172, b_+174); A = 0xf5; // TILEINDEX_SOFT_SOIL_PLANTED
  CALL_C(b_+174, setTile_hook, SYM(setTile), b_+177);
  CYC(b_+177, b_+179); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+179, b_+180); A = mem_rd(gb, DE);
  CYC(b_+180, b_+183); SET_HL(wGashaSpotsPlantedBitset);
  CALL_C(b_+183, setFlag_hook, SYM(setFlag), b_+186);
  CYC(b_+186, b_+187); A = mem_rd(gb, DE);
  CYC(b_+187, b_+189); L = 0x4f; // wGashaSpotKillCounters low byte
  CYC(b_+189, b_+190); interactionCodeb6_addAToHl(gb, b_+190);
  CYC(b_+190, b_+192); mem_wr(gb, HL, 0x00);
  CYC(b_+192, b_+194); L = 0xbe; // wNumGashaSeeds low byte
  CYC(b_+194, b_+195); A = mem_rd(gb, HL);
  CYC(b_+195, b_+197); alu_sub(gb, 0x01);
  CYC(b_+197, b_+198); alu_daa(gb);
  CYC(b_+198, b_+199); mem_wr(gb, HL, A);
  CYC(b_+199, b_+201); A = 0x5e; // SND_GETSEED
  CALL_C(b_+201, playSound_b00_hook, SYM(playSound_b00), b_+204);

delete_:
  CYC(b_+204, b_+207); interactionDelete_hook(gb); return; // jp

state3:
  CYC(b_+207, b_+209); E = INTERACTION_BASE + OBJ_VAR2A;
  CYC(b_+209, b_+210); A = mem_rd(gb, DE);
  CYC(b_+210, b_+212); alu_cp(gb, 0xff);
  if (!(F & FZ)) { CYCT(b_+212, b_+213); ret_effect(gb); return; } // ret nz
  CYC(b_+212, b_+213);
  CYC(b_+213, b_+215); A = 0x80; // DISABLE_ALL_BUT_INTERACTIONS
  CYC(b_+215, b_+218); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+218, b_+221); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+221, b_+222); H = D;
  CYC(b_+222, b_+224); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+224, b_+226); mem_wr(gb, HL, 0x04);
  CYC(b_+226, b_+228); A = 0x06;
  CALL_C(b_+228, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+231);
  CYC(b_+231, b_+234); SET_BC(0xfec0); // -$140
  CALL_C(b_+234, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+237);
  CYC(b_+237, b_+239); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+239, b_+241); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(b_+241, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+244);
  CYC(b_+244, b_+246); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+246, b_+247); mem_wr(gb, DE, A);
  CYC(b_+247, b_+250); objectSetVisible80_hook(gb); return; // jp

state4:
  CYC(b_+250, b_+253); A = mem_rd(gb, wLinkDeathTrigger);
  CYC(b_+253, b_+254); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+254, b_+257); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+254, b_+257);
  CALL_C(b_+257, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+260);
  if (F & FC) { CYCT(b_+260, b_+262); goto l_4503; } // jr c
  CYC(b_+260, b_+262);
  CALL_C(b_+262, objectApplySpeed_hook, SYM(objectApplySpeed), b_+265);
  CYC(b_+265, b_+267); C = 0x20;
  CYC(b_+267, b_+270); objectUpdateSpeedZ_paramC_hook(gb); return; // jp

l_4503:
  CALL_C(b_+270, interactionIncState_hook, SYM(interactionIncState), b_+273);
  CYC(b_+273, b_+275); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+275, b_+277); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CYC(b_+277, b_+280); SET_BC(0x3501); // TX_3501
  CYC(b_+280, b_+283); showText_hook(gb); return; // jp

state5:
  CYC(b_+283, b_+286); SET_HL(wGashaSpotFlags);
  CYC(b_+286, b_+288); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+288, b_+290); goto l_451d; } // jr nz
  CYC(b_+288, b_+290);
  CYC(b_+290, b_+292); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 0))); // set 0,(hl)
  CYC(b_+292, b_+294); B = 0x04; // GASHATREASURE_TIER3_RING
  CYCT(b_+294, b_+296); goto spawnTreasure; // jr

l_451d:
  CYC(b_+296, b_+298); C = 0x00;
  CYC(b_+298, b_+301); SET_HL(wGashaMaturity + 1);
  CYC(b_+301, b_+302); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+302, b_+304); A = alu_srl(gb, A);
  if (!(F & FZ)) { CYCT(b_+304, b_+306); goto l_4533; } // jr nz
  CYC(b_+304, b_+306);
  CYC(b_+306, b_+307); A = mem_rd(gb, HL);
  CYC(b_+307, b_+308); alu_rra(gb);
  CYC(b_+308, b_+311); SET_HL(b_+774); // @gashaMaturityValues

l_452c:
  CYC(b_+311, b_+312); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+312, b_+314); goto l_4533; } // jr nc
  CYC(b_+312, b_+314);
  CYC(b_+314, b_+315); SET_HL(HL + 1); // inc hl
  CYC(b_+315, b_+316); C = alu_inc8(gb, C);
  CYCT(b_+316, b_+318); goto l_452c; // jr

l_4533:
  CYC(b_+318, b_+320); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+320, b_+321); A = mem_rd(gb, DE);
  CYC(b_+321, b_+324); SET_HL(b_+799); // @gashaSpotRanks
  CYC(b_+324, b_+325); interactionCodeb6_addAToHl(gb, b_+325);
  CYC(b_+325, b_+326); A = mem_rd(gb, HL);
  CYC(b_+326, b_+327); interactionCodeb6_addAToHl(gb, b_+327);
  CYC(b_+327, b_+328); A = C;
  CYC(b_+328, b_+329); alu_add(gb, A);
  CYC(b_+329, b_+330); C = A;
  CYC(b_+330, b_+331); alu_add(gb, A);
  CYC(b_+331, b_+332); alu_add(gb, A);
  CYC(b_+332, b_+333); alu_add(gb, C);
  CYC(b_+333, b_+334); interactionCodeb6_addAToHl(gb, b_+334);
  CALL_C(b_+334, getRandomIndexFromProbabilityDistribution_hook, SYM(getRandomIndexFromProbabilityDistribution), b_+337);
  CYC(b_+337, b_+338); A = B;
  CYC(b_+338, b_+340); alu_cp(gb, 0x06); // GASHATREASURE_POTION
  if (!(F & FZ)) { CYCT(b_+340, b_+342); goto notPotion; } // jr nz
  CYC(b_+340, b_+342);
  CYC(b_+342, b_+344); A = 0x2f; // TREASURE_POTION
  CALL_C(b_+344, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+347);
  if (!(F & FC)) { CYCT(b_+347, b_+349); goto decGashaMaturity; } // jr nc
  CYC(b_+347, b_+349);
  CYC(b_+349, b_+352); SET_HL(wLinkMaxHealth);
  CYC(b_+352, b_+353); A = mem_rd(gb, HL); SET_HL(HL - 1); // ldd a,(hl)
  CYC(b_+353, b_+354); mem_wr(gb, HL, A);
  CYCT(b_+354, b_+356); goto decGashaMaturity; // jr

notPotion:
  CYC(b_+356, b_+358); alu_cp(gb, 0x00); // GASHATREASURE_HEART_PIECE
  if (!(F & FZ)) { CYCT(b_+358, b_+360); goto decGashaMaturity; } // jr nz
  CYC(b_+358, b_+360);
  CYC(b_+360, b_+363); SET_HL(wGashaSpotFlags);
  CYC(b_+363, b_+365); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+365, b_+367); goto l_4565; } // jr z
  CYC(b_+365, b_+367);
  CYC(b_+367, b_+368); B = alu_inc8(gb, B);

l_4565:
  CYC(b_+368, b_+370); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 1))); // set 1,(hl)

decGashaMaturity:
  CYC(b_+370, b_+373); SET_HL(wGashaMaturity);
  CYC(b_+373, b_+374); A = mem_rd(gb, HL);
  CYC(b_+374, b_+376); alu_sub(gb, 0xc8); // 200
  CYC(b_+376, b_+377); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+377, b_+378); A = mem_rd(gb, HL);
  CYC(b_+378, b_+380); alu_sbc(gb, 0x00);
  CYC(b_+380, b_+381); mem_wr(gb, HL, A);
  if (!(F & FC)) { CYCT(b_+381, b_+383); goto spawnTreasure; } // jr nc
  CYC(b_+381, b_+383);
  CYC(b_+383, b_+384); alu_xor(gb, A);
  CYC(b_+384, b_+385); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+385, b_+386); mem_wr(gb, HL, A);

spawnTreasure:
  CYC(b_+386, b_+387); A = B;
  CYC(b_+387, b_+389); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+389, b_+390); mem_wr(gb, DE, A);
  CYC(b_+390, b_+393); SET_HL(b_+779); // @gashaTreasures
  CYC(b_+393, b_+394); interactionCodeb6_addDoubleIndex(gb, b_+394);
  CYC(b_+394, b_+395); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+395, b_+396); C = mem_rd(gb, HL);
  CYC(b_+396, b_+398); alu_cp(gb, 0x2d); // TREASURE_RING
  if (!(F & FZ)) { CYCT(b_+398, b_+400); goto l_4588; } // jr nz
  CYC(b_+398, b_+400);
  CALL_C(b_+400, getRandomRingOfGivenTier_hook, SYM(getRandomRingOfGivenTier), b_+403);

l_4588:
  CYC(b_+403, b_+404); B = A;
  CALL_C(b_+404, giveTreasure_hook, SYM(giveTreasure), b_+407);
  CYC(b_+407, b_+410); SET_HL(wLinkForceState);
  CYC(b_+410, b_+412); A = 0x04; // LINK_STATE_04
  CYC(b_+412, b_+413); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+413, b_+415); mem_wr(gb, HL, 0x01); // [wcc50]
  CYC(b_+415, b_+418); SET_HL(w1Link_yh);
  CYC(b_+418, b_+421); SET_BC(0xf300);
  CALL_C(b_+421, objectTakePositionWithOffset_hook, SYM(objectTakePositionWithOffset), b_+424);
  CALL_C(b_+424, interactionIncState_hook, SYM(interactionIncState), b_+427);
  CYC(b_+427, b_+429); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+429, b_+431); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7))); // set 7,(hl)
  CYC(b_+431, b_+433); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+433, b_+434); A = mem_rd(gb, DE);
  CYC(b_+434, b_+436); alu_cp(gb, 0x00); // GASHATREASURE_HEART_PIECE
  CYC(b_+436, b_+438); A = 0x4c; // SND_GETITEM
  if (!(F & FZ)) { CALL_C_CC(b_+438, playSound_b00_hook, SYM(playSound_b00), b_+441); } else { CYC(b_+438, b_+441); } // call nz
  CALL_C(b_+441, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+444);
  CYC(b_+444, b_+446); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+446, b_+447); A = mem_rd(gb, DE);
  CYC(b_+447, b_+450); SET_HL(b_+457); // @lowTextIndices
  CYC(b_+450, b_+451); interactionCodeb6_addAToHl(gb, b_+451);
  CYC(b_+451, b_+452); C = mem_rd(gb, HL);
  CYC(b_+452, b_+454); B = 0x35; // >TX_3500
  CYC(b_+454, b_+457); showText_hook(gb); return; // jp

state6:
  CYC(b_+467, b_+470); SET_HL(wNumRupees);
  CYC(b_+470, b_+473); A = mem_rd(gb, wDisplayedRupees);
  CYC(b_+473, b_+474); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+474, b_+475); ret_effect(gb); return; } // ret nz
  CYC(b_+474, b_+475);
  CYC(b_+475, b_+476); L = alu_inc8(gb, L);
  CYC(b_+476, b_+479); A = mem_rd(gb, wDisplayedRupees + 1);
  CYC(b_+479, b_+480); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+480, b_+481); ret_effect(gb); return; } // ret nz
  CYC(b_+480, b_+481);
  CYC(b_+481, b_+483); L = 0xaa; // wLinkHealth low byte
  CYC(b_+483, b_+486); A = mem_rd(gb, wDisplayedHearts);
  CYC(b_+486, b_+487); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+487, b_+488); ret_effect(gb); return; } // ret nz
  CYC(b_+487, b_+488);
  CYC(b_+488, b_+490); A = 0x91; // SND_FAIRYCUTSCENE
  CALL_C(b_+490, playSound_b00_hook, SYM(playSound_b00), b_+493);
  CYC(b_+493, b_+495); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+495, b_+496); A = mem_rd(gb, DE);
  CYC(b_+496, b_+499); SET_HL(b_+1186); // @gfxHeaderToLoadWhenTreeDisappears
  CYC(b_+499, b_+500); interactionCodeb6_addAToHl(gb, b_+500);
  CYC(b_+500, b_+501); A = mem_rd(gb, HL);
  CYC(b_+501, b_+502); push_effect(gb, AF); // push af
  CYC(b_+502, b_+504); alu_sub(gb, 0x39); // GFXH_GASHA_TREE_DISAPPEARED - OBJ_GFXH_04
  CYC(b_+504, b_+507); mem_wr(gb, wLoadedTreeGfxActive, A);
  CYC(b_+507, b_+509); A = 0x3d; // GFXH_GASHA_TREE_DISAPPEARED
  CALL_C(b_+509, loadGfxHeader_hook, SYM(loadGfxHeader), b_+512);
  CYC(b_+512, b_+513); SET_AF(pop_effect(gb)); // pop af
  CYC(b_+513, b_+515); alu_cp(gb, 0x3d);
  if (!(F & FZ)) { CALL_C_CC(b_+515, loadGfxHeader_hook, SYM(loadGfxHeader), b_+518); } else { CYC(b_+515, b_+518); } // call nz
  CYC(b_+518, b_+520); A = H8(hActiveObject); // ldh a,($ffaf)
  CYC(b_+520, b_+521); D = A;
  CYC(b_+521, b_+522); H = D;
  CYC(b_+522, b_+524); L = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+524, b_+526); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+526, b_+527); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+527, b_+528); mem_wr(gb, DE, A);
  CYC(b_+528, b_+530); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+530, b_+531); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+531, b_+532); mem_wr(gb, DE, A);
  CYC(b_+532, b_+534); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+534, b_+536); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7))); // res 7,(hl)
  CALL_C(b_+536, objectGetTileCollisions_hook, SYM(objectGetTileCollisions), b_+539);
  CYC(b_+539, b_+540); alu_xor(gb, A);
  CYC(b_+540, b_+541); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+541, b_+542); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+542, b_+543); A = L;
  CYC(b_+543, b_+545); alu_sub(gb, 0x10);
  CYC(b_+545, b_+546); L = A;
  CYC(b_+546, b_+547); alu_xor(gb, A);
  CYC(b_+547, b_+548); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+548, b_+549); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+549, b_+550); H = A;
  CYC(b_+550, b_+551); E = L;
  CYC(b_+551, b_+552); A = L;
  CYC(b_+552, b_+554); alu_and(gb, 0xf0);
  CYC(b_+554, b_+555); L = A;
  CYC(b_+555, b_+556); A = E;
  CYC(b_+556, b_+558); alu_and(gb, 0x0f);
  CYC(b_+558, b_+560); L = alu_sla(gb, L);
  CYC(b_+560, b_+562); H = alu_rl(gb, H);
  CYC(b_+562, b_+563); alu_add(gb, L);
  CYC(b_+563, b_+564); L = A;
  CYC(b_+564, b_+566); L = alu_sla(gb, L);
  CYC(b_+566, b_+568); H = alu_rl(gb, H);
  CYC(b_+568, b_+571); SET_BC(w3VramTiles);
  CYC(b_+571, b_+572); alu_add_hl(gb, BC);
  CYC(b_+572, b_+574); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+574, b_+575); A = L;
  CYC(b_+575, b_+576); mem_wr(gb, DE, A);
  CYC(b_+576, b_+577); E = alu_inc8(gb, E);
  CYC(b_+577, b_+578); A = H;
  CYC(b_+578, b_+579); mem_wr(gb, DE, A);
  CYC(b_+579, b_+582); SET_BC(0x0400);
  CYC(b_+582, b_+583); alu_add_hl(gb, BC);
  CYC(b_+583, b_+585); A = hram_rd(gb, R_SVBK);
  CYC(b_+585, b_+586); push_effect(gb, AF); // push af
  CYC(b_+586, b_+588); A = 0x03; // :w3VramAttributes
  CYC(b_+588, b_+590); hram_wr(gb, R_SVBK, A);
  CYC(b_+590, b_+592); B = 0x04;

l_4645:
  CYC(b_+592, b_+594); C = 0x04;
  CYC(b_+594, b_+595); push_effect(gb, BC); // push bc

l_4648:
  CYC(b_+595, b_+596); A = mem_rd(gb, HL);
  CYC(b_+596, b_+598); alu_and(gb, 0xf0);
  CYC(b_+598, b_+600); alu_or(gb, 0x04);
  CYC(b_+600, b_+601); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+601, b_+602); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+602, b_+604); goto l_4648; } // jr nz
  CYC(b_+602, b_+604);
  CYC(b_+604, b_+607); SET_BC(0x001c);
  CYC(b_+607, b_+608); alu_add_hl(gb, BC);
  CYC(b_+608, b_+609); SET_BC(pop_effect(gb)); // pop bc
  CYC(b_+609, b_+610); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+610, b_+612); goto l_4645; } // jr nz
  CYC(b_+610, b_+612);
  CYC(b_+612, b_+613); SET_AF(pop_effect(gb)); // pop af
  CYC(b_+613, b_+615); hram_wr(gb, R_SVBK, A);
  CALL_C(b_+615, interactionIncState_hook, SYM(interactionIncState), b_+618);
  CYC(b_+618, b_+620); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+620, b_+622); mem_wr(gb, HL, 0x08);

state7:
  CYC(b_+622, b_+623); H = D;
  CYC(b_+623, b_+625); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+625, b_+626); A = mem_rd(gb, HL);
  CYC(b_+626, b_+627); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+627, b_+629); goto counter1Done; } // jr z
  CYC(b_+627, b_+629);
  CYC(b_+629, b_+630); A = alu_dec8(gb, A);
  CYC(b_+630, b_+631); mem_wr(gb, HL, A);
  if (!(F & FZ)) { CYCT(b_+631, b_+632); ret_effect(gb); return; } // ret nz
  CYC(b_+631, b_+632);

counter1Done:
  CYC(b_+632, b_+634); A = 0x08;
  CYC(b_+634, b_+635); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+635, b_+636); A = mem_rd(gb, HL);
  CYC(b_+636, b_+637); A = alu_inc8(gb, A);
  CYC(b_+637, b_+638); mem_wr(gb, HL, A);
  CYC(b_+638, b_+640); alu_cp(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+640, b_+642); goto counter2Done; } // jr nc
  CYC(b_+640, b_+642);
  CYC(b_+642, b_+645); SET_HL(b_+1064); // @treeDisappearanceFrames - 1
  CYC(b_+645, b_+646); interactionCodeb6_addAToHl(gb, b_+646);
  CYC(b_+646, b_+647); A = mem_rd(gb, HL);
  CYC(b_+647, b_+648); interactionCodeb6_addAToHl(gb, b_+648);
  CYC(b_+648, b_+650); E = INTERACTION_BASE + OBJ_VAR3A;
  CYC(b_+650, b_+651); A = mem_rd(gb, DE);
  CYC(b_+651, b_+652); C = A;
  CYC(b_+652, b_+653); E = alu_inc8(gb, E);
  CYC(b_+653, b_+654); A = mem_rd(gb, DE);
  CYC(b_+654, b_+655); D = A;
  CYC(b_+655, b_+656); E = C;
  CYC(b_+656, b_+657); push_effect(gb, HL); // push hl
  CYC(b_+657, b_+658); push_effect(gb, DE); // push de
  CYC(b_+658, b_+659); SET_HL(pop_effect(gb)); // pop hl
  CYC(b_+659, b_+660); SET_DE(pop_effect(gb)); // pop de
  CYC(b_+660, b_+662); A = hram_rd(gb, R_SVBK);
  CYC(b_+662, b_+663); push_effect(gb, AF); // push af
  CYC(b_+663, b_+665); A = 0x03; // :w3VramTiles
  CYC(b_+665, b_+667); hram_wr(gb, R_SVBK, A);
  CYC(b_+667, b_+669); B = 0x04;

l_4692:
  CYC(b_+669, b_+671); C = 0x04;
  CYC(b_+671, b_+672); push_effect(gb, BC); // push bc

l_4695:
  CYC(b_+672, b_+673); A = mem_rd(gb, DE);
  CYC(b_+673, b_+674); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+674, b_+675); SET_DE(DE + 1); // inc de
  CYC(b_+675, b_+676); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+676, b_+678); goto l_4695; } // jr nz
  CYC(b_+676, b_+678);
  CYC(b_+678, b_+681); SET_BC(0x001c);
  CYC(b_+681, b_+682); alu_add_hl(gb, BC);
  CYC(b_+682, b_+683); SET_BC(pop_effect(gb)); // pop bc
  CYC(b_+683, b_+684); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+684, b_+686); goto l_4692; } // jr nz
  CYC(b_+684, b_+686);
  CYC(b_+686, b_+687); SET_AF(pop_effect(gb)); // pop af
  CYC(b_+687, b_+689); hram_wr(gb, R_SVBK, A);
  CYC(b_+689, b_+691); A = 0x29; // UNCMP_GFXH_29
  CALL_C(b_+691, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+694);
  CYC(b_+694, b_+696); A = H8(hActiveObject); // ldh a,($ffaf)
  CYC(b_+696, b_+697); D = A;
  CYC(b_+697, b_+699); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+699, b_+700); A = mem_rd(gb, DE);
  CYC(b_+700, b_+702); alu_add(gb, 0x1f); // UNCMP_GFXH_20 - 1
  CALL_C(b_+702, loadUncompressedGfxHeader_hook, SYM(loadUncompressedGfxHeader), b_+705);
  CALL_C(b_+705, reloadTileMap_hook, SYM(reloadTileMap), b_+708);
  CYC(b_+708, b_+710); A = H8(hActiveObject); // ldh a,($ffaf)
  CYC(b_+710, b_+711); D = A;
  RET(b_+711); return;

counter2Done:
  CYC(b_+712, b_+713); alu_xor(gb, A);
  CYC(b_+713, b_+716); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+716, b_+719); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+719, b_+721); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+721, b_+722); A = mem_rd(gb, DE);
  CYC(b_+722, b_+725); SET_HL(wGashaSpotsPlantedBitset);
  CALL_C(b_+725, unsetFlag_hook, SYM(unsetFlag), b_+728);
  CYC(b_+728, b_+730); A = 0x4e; // TILEINDEX_GASHA_TREE_TL
  CALL_C(b_+730, findTileInRoom_hook, SYM(findTileInRoom), b_+733);
  if (!(F & FZ)) { CYCT(b_+733, b_+734); ret_effect(gb); return; } // ret nz
  CYC(b_+733, b_+734);
  CYC(b_+734, b_+736); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+736, b_+737); A = mem_rd(gb, DE);
  CYC(b_+737, b_+740); SET_BC(b_+758); // @tileReplacements
  CALL_C(b_+740, addAToBc_hook, 0x006d, b_+743);
  CYC(b_+743, b_+744); A = mem_rd(gb, BC);
  CYC(b_+744, b_+745); B = A;
  CYC(b_+745, b_+746); A = B;
  CYC(b_+746, b_+747); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+747, b_+748); mem_wr(gb, HL, A);
  CYC(b_+748, b_+750); A = 0x0f;
  CYC(b_+750, b_+751); alu_add(gb, L);
  CYC(b_+751, b_+752); L = A;
  CYC(b_+752, b_+753); A = B;
  CYC(b_+753, b_+754); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+754, b_+755); mem_wr(gb, HL, A);
  CYC(b_+755, b_+758); interactionDelete_hook(gb); return; // jp
}
