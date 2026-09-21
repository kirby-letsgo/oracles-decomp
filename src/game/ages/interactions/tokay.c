#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t tokay_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0004, 0x0006, false); burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void tokay_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); }
  else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

static void tokay_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void tokayState1_hook(GB *gb);
void tokayRunSubid00_hook(GB *gb);
void tokayThiefSubstate0_hook(GB *gb);
void tokayInitHeldItem_hook(GB *gb);
void tokayInitAccessory_hook(GB *gb);
void tokayThief_countdownToStealNextItem_hook(GB *gb);
void tokayThiefSubstate1_hook(GB *gb);
void tokayThiefSubstate2_hook(GB *gb);
void tokayThief_jump_hook(GB *gb);
void tokayThiefSubstate3_hook(GB *gb);
void tokayThiefSubstate4_hook(GB *gb);
void tokayThiefSubstate5_hook(GB *gb);
void tokayThiefSubstate6_hook(GB *gb);
void tokayRunSubid05_hook(GB *gb);
void tokayRunSubid06_hook(GB *gb);
void tokayRunSubid0b_hook(GB *gb);
void tokayRunSubid0c_hook(GB *gb);
void wildTokayParticipantSubstate0_hook(GB *gb);
void wildTokayParticipantSubstate1_hook(GB *gb);
void wildTokayParticipantSubstate2_hook(GB *gb);
void wildTokayParticipant_checkGrabMeat_hook(GB *gb);
void tokayInitMeatAccessory_hook(GB *gb);
void tokayRunSubid0d_hook(GB *gb);
void tokayRunSubid0e_hook(GB *gb);
void tokayRunSubid0f_hook(GB *gb);
void tokayRunSubid10_hook(GB *gb);
void tokayRunSubid1e_hook(GB *gb);
void tokayRunSubid12_hook(GB *gb);
void tokayRunSubid1a_hook(GB *gb);
void tokayRunStinkBagCutscene_hook(GB *gb);
void tokayLoadScript_hook(GB *gb);

// interactionCode48@initSubid0d: the past NPC running the wild Tokay game; also called from
// @initSubid19. Ends in tokayLoadScript, whose ret pops the caller's address.
static void tokay_init_subid0d(GB *gb) {
  BASE(interactionCode48__initSubid);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+408, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+411);
  alu_bit(gb, 6, A);
  CYC(b_+411, b_+413);
  if (F & FZ) CYCT(b_+413, b_+415);
  else {
    CYC(b_+413, b_+415);
    A = 0x81;
    CYC(b_+415, b_+417);
    CYC(b_+417, b_+420); mem_wr(gb, wDisabledObjects, A);
    CYC(b_+420, b_+423); mem_wr(gb, wMenuDisabled, A);
    SET_HL(w1Link_yh);
    CYC(b_+423, b_+426);
    CYC(b_+426, b_+428); mem_wr(gb, HL, 0x48);
    L = w1Link_xh & 0xff;
    CYC(b_+428, b_+430);
    CYC(b_+430, b_+432); mem_wr(gb, HL, 0x50);
    alu_xor(gb, A);
    L = w1Link_direction & 0xff;
    CYC(b_+432, b_+435);
    CYC(b_+435, b_+436); mem_wr(gb, HL, A);
  }
  H = D;
  L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+436, b_+439);
  CYC(b_+439, b_+441); mem_wr(gb, HL, 0x03);
  CYC(b_+441, b_+444);
  tokayLoadScript_hook(gb);
}

// interactionCode48@initSubid: per-subid initialisation of the Tokay interaction, reached
// through the 32-entry jump table; the caller pushed the return address each path pops.
static void tokay_init_subid(GB *gb) {
  BASE(interactionCode48__initSubid);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (tokay_jump_table(gb));
    if (jt_ == b_+68) { goto subid00; }
    else if (jt_ == b_+72) { goto subid01; }
    else if (jt_ == b_+76) { goto subid02; }
    else if (jt_ == b_+124) { goto subid1d; }
    else if (jt_ == b_+167) { goto subid07; }
    else if (jt_ == b_+173) { goto subid06; }
    else if (jt_ == b_+263) { goto subid11; }
    else if (jt_ == b_+281) { goto subid1e; }
    else if (jt_ == b_+290) { goto subid0f; }
    else if (jt_ == b_+294) { goto subid10; }
    else if (jt_ == b_+316) { goto subid0e; }
    else if (jt_ == b_+321) { goto subid05; }
    else if (jt_ == b_+333) { goto subid0b; }
    else if (jt_ == b_+363) { goto subid0c; }
    else if (jt_ == b_+408) { tokay_init_subid0d(gb); return; }
    else if (jt_ == b_+444) { goto subid12; }
    else if (jt_ == b_+467) { goto subid19; }
    else if (jt_ == b_+473) { goto subid1a; }
    else if (jt_ == b_+486) { goto subid1c; }
    else if (jt_ == b_+494) { goto subid1b; }
    else if (jt_ == b_+495) { goto subid1f; }
    else { HANDOFF(jt_); }
  } while (0);
subid00:
  A = 0x01;
  CYC(b_+68, b_+70);
  CYCT(b_+70, b_+72);
  goto initLinkRobberyTokay;
subid01:
  A = 0x03;
  CYC(b_+72, b_+74);
  CYCT(b_+74, b_+76);
  goto initLinkRobberyTokay;
subid02:
  CALL_C(b_+76, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+79);
  alu_bit(gb, 6, A);
  CYC(b_+79, b_+81);
  if (!(F & FZ)) { CYCT(b_+81, b_+84); goto deleteSelf; }
  CYC(b_+81, b_+84);
  alu_xor(gb, A);
  CYC(b_+84, b_+85);
  CALL_C(b_+85, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+88);
  CALL_C(b_+88, tokayLoadScript_hook, SYM(tokayLoadScript), b_+91);
  A = 0x08;
  CYC(b_+91, b_+93);
  CALL_C(b_+93, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+96);
  L = w1Link_subid & 0xff;
  CYC(b_+96, b_+98);
  CYC(b_+98, b_+100); mem_wr(gb, HL, 0x07);
  E = INTERACTION_BASE + OBJ_VAR38;
  A = 0x46;
  CYC(b_+100, b_+104);
  CYC(b_+104, b_+105); mem_wr(gb, DE, A);
  A = 0xf0;
  CYC(b_+105, b_+107);
  CYC(b_+107, b_+110);
  TAIL(playSound_b00);
initLinkRobberyTokay:
  CALL_C(b_+110, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+113);
  CALL_C(b_+113, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+116);
  alu_bit(gb, 6, A);
  CYC(b_+116, b_+118);
  if (!(F & FZ)) { CYCT(b_+118, b_+121); goto deleteSelf; }
  CYC(b_+118, b_+121);
  CYC(b_+121, b_+124);
  TAIL(tokayLoadScript);
subid1d:
  CALL_C(b_+124, tokayLoadScript_hook, SYM(tokayLoadScript), b_+127);
  CALL_C(b_+127, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+130);
  alu_bit(gb, 6, A);
  A = 0x02;
  CYC(b_+130, b_+134);
  if (!(F & FZ)) { CYCT(b_+134, b_+137); interactionSetAnimation_hook(gb); return; }
  CYC(b_+134, b_+137);
  B = 0x14;
  CYC(b_+137, b_+139);
  CYC(b_+139, b_+142); A = mem_rd(gb, wShieldLevel);
  alu_cp(gb, 0x02);
  CYC(b_+142, b_+144);
  if (F & FC) CYCT(b_+144, b_+146);
  else {
    CYC(b_+144, b_+146);
    B = 0x15;
    CYC(b_+146, b_+148);
  }
  A = B;
  E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+148, b_+151);
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CALL_C(b_+152, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+155);
  if (!(F & FZ)) { CYCT(b_+155, b_+156); ret_effect(gb); return; }
  CYC(b_+155, b_+156);
  L = alu_inc8(gb, L);
  CYC(b_+156, b_+157);
  CYC(b_+157, b_+158); mem_wr(gb, HL, B);
  L = alu_dec8(gb, L);
  CYC(b_+158, b_+159);
  CALL_C(b_+159, tokayInitAccessory_hook, SYM(tokayInitAccessory), b_+162);
  A = 0x06;
  CYC(b_+162, b_+164);
  CYC(b_+164, b_+167);
  TAIL(interactionSetAnimation);
subid07:
  CALL_C(b_+167, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+170);
  if (!(F & FZ)) { CYCT(b_+170, b_+173); interactionDelete_hook(gb); return; }
  CYC(b_+170, b_+173);
subid06:
  CALL_C(b_+173, tokayLoadScript_hook, SYM(tokayLoadScript), b_+176);
  H = D;
  L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+176, b_+179);
  CYC(b_+179, b_+180); A = mem_rd(gb, HL);
  alu_sub(gb, 0x06);
  SET_BC(SYM(tokayIslandStolenItems));
  CYC(b_+180, b_+185);
  CALL_C(b_+185, addAToBc_hook, SYM(addAToBc), b_+188);
  CYC(b_+188, b_+189); A = mem_rd(gb, BC);
  L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+189, b_+191);
  CYC(b_+191, b_+192); mem_wr(gb, HL, A);
  C = 0x00;
  CYC(b_+192, b_+194);
  CALL_C(b_+194, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+197);
  alu_bit(gb, 6, A);
  CYC(b_+197, b_+199);
  if (F & FZ) { CYCT(b_+199, b_+201); goto endLoop; }
  CYC(b_+199, b_+201);
  C = alu_inc8(gb, C);
  B = 0x09;
  CYC(b_+201, b_+204);
  for (;;) {
    A = B;
    A = alu_dec8(gb, A);
    SET_HL(SYM(tokayIslandStolenItems));
    CYC(b_+204, b_+209);
    CYC(b_+209, b_+210); tokay_add_a_to_hl(gb, b_+210);
    CYC(b_+210, b_+211); A = mem_rd(gb, HL);
    alu_cp(gb, 0x01);
    CYC(b_+211, b_+213);
    if (F & FZ) CYCT(b_+213, b_+215);
    else {
      CYC(b_+213, b_+215);
      CALL_C(b_+215, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+218);
      if (!(F & FC)) { CYCT(b_+218, b_+221); goto endLoop; }
      CYC(b_+218, b_+221);
    }
    B = alu_dec8(gb, B);
    CYC(b_+221, b_+222);
    if (B) { CYCT(b_+222, b_+224); continue; }
    CYC(b_+222, b_+224);
    break;
  }
  C = alu_inc8(gb, C);
  CYC(b_+224, b_+225);
endLoop:
  A = C;
  E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+225, b_+228);
  CYC(b_+228, b_+229); mem_wr(gb, DE, A);
  alu_or(gb, A);
  CYC(b_+229, b_+230);
  if (!(F & FZ)) {
    CYCT(b_+230, b_+232);
    A = 0x02;
    CYC(b_+258, b_+260);
    CYC(b_+260, b_+263);
    TAIL(interactionSetAnimation);
  }
  CYC(b_+230, b_+232);
  A = 0x06;
  CYC(b_+232, b_+234);
  CALL_C(b_+234, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+237);
  E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+237, b_+239);
  CYC(b_+239, b_+240); A = mem_rd(gb, DE);
  B = 0x0a;
  alu_cp(gb, 0x07);
  CYC(b_+240, b_+244);
  if (F & FZ) CYCT(b_+244, b_+246);
  else {
    CYC(b_+244, b_+246);
    B = 0x0b;
    CYC(b_+246, b_+248);
  }
  H = D;
  L = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+248, b_+251);
  CYC(b_+251, b_+252); mem_wr(gb, HL, B);
  alu_sub(gb, 0x06);
  B = A;
  CYC(b_+252, b_+255);
  CYC(b_+255, b_+258);
  TAIL(tokayInitHeldItem);
subid11:
  CALL_C(b_+263, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+266);
  alu_bit(gb, 7, A);
  CYC(b_+266, b_+268);
  if (F & FZ) { CYCT(b_+268, b_+270); goto subid0e; }
  CYC(b_+268, b_+270);
  E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+270, b_+272);
  CYC(b_+272, b_+273); A = mem_rd(gb, DE);
  alu_add(gb, 0x10);
  CYC(b_+273, b_+275);
  CYC(b_+275, b_+276); mem_wr(gb, DE, A);
  CALL_C(b_+276, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+279);
  CYCT(b_+279, b_+281);
  goto subid0e;
subid1e:
  CALL_C(b_+281, objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+284);
  H = wRoomLayout >> 8;
  CYC(b_+284, b_+286);
  CYC(b_+286, b_+288); mem_wr(gb, HL, 0x00);
  CYCT(b_+288, b_+290);
  goto subid0e;
subid0f:
  A = 0x01;
  CYC(b_+290, b_+292);
  CYCT(b_+292, b_+294);
  goto eatDimitri;
subid10:
  alu_xor(gb, A);
  CYC(b_+294, b_+295);
eatDimitri:
  CALL_C(b_+295, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+298);
  SET_HL(wDimitriState);
  CYC(b_+298, b_+301);
  CYC(b_+301, b_+303); alu_bit(gb, 1, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+303, b_+305); goto deleteSelf; }
  CYC(b_+303, b_+305);
  L = wEssencesObtained & 0xff;
  CYC(b_+305, b_+307);
  CYC(b_+307, b_+309); alu_bit(gb, 2, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+309, b_+311); goto deleteSelf; }
  CYC(b_+309, b_+311);
  E = INTERACTION_BASE + OBJ_SPEED;
  A = 0x50;
  CYC(b_+311, b_+315);
  CYC(b_+315, b_+316); mem_wr(gb, DE, A);
subid0e:
  A = 0x06;
  CYC(b_+316, b_+318);
  CALL_C(b_+318, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+321);
subid05:
  CALL_C(b_+321, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+324);
  CALL_C(b_+324, tokayLoadScript_hook, SYM(tokayLoadScript), b_+327);
  CYC(b_+327, b_+330);
  TAIL(tokayState1);
deleteSelf:
  CYC(b_+330, b_+333);
  TAIL(interactionDelete);
subid0b:
  CALL_C(b_+333, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+336);
  if (F & FZ) { CYCT(b_+336, b_+339); interactionDelete_hook(gb); return; }
  CYC(b_+336, b_+339);
  A = 0x15;
  CYC(b_+339, b_+341);
  CALL_C(b_+341, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+344);
  if (F & FC) { CYCT(b_+344, b_+347); interactionDelete_hook(gb); return; }
  CYC(b_+344, b_+347);
  CALL_C(b_+347, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+350);
  alu_bit(gb, 7, A);
  CYC(b_+350, b_+352);
  if (!(F & FZ)) { CYCT(b_+352, b_+355); interactionDelete_hook(gb); return; }
  CYC(b_+352, b_+355);
  A = 0x01;
  CYC(b_+355, b_+357);
  CYC(b_+357, b_+360); mem_wr(gb, wDiggingUpEnemiesForbidden, A);
  CYC(b_+360, b_+363);
  TAIL(tokayLoadScript);
subid0c:
  H = D;
  CYC(b_+363, b_+364);
  CYC(b_+364, b_+367); A = mem_rd(gb, wTmpcfc0_wildTokay_cfdf);
  alu_or(gb, A);
  CYC(b_+367, b_+368);
  if (F & FZ) CYCT(b_+368, b_+370);
  else {
    CYC(b_+368, b_+370);
    L = INTERACTION_BASE + OBJ_OAM_FLAGS;
    CYC(b_+370, b_+372);
    CYC(b_+372, b_+374); mem_wr(gb, HL, 0x02);
  }
  L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+374, b_+376);
  CYC(b_+376, b_+378); mem_wr(gb, HL, 0x10);
  L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+378, b_+380);
  CYC(b_+380, b_+381); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+381, b_+383);
  CYC(b_+383, b_+384); A = mem_rd(gb, HL);
  alu_cp(gb, 0x88);
  CYC(b_+384, b_+386);
  if (F & FZ) CYCT(b_+386, b_+388);
  else {
    CYC(b_+386, b_+388);
    L = INTERACTION_BASE + OBJ_DIRECTION;
    CYC(b_+388, b_+390);
    CYC(b_+390, b_+391); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  }
  CYC(b_+391, b_+394); A = mem_rd(gb, wWildTokayGameLevel);
  SET_HL(b_+403);
  CYC(b_+394, b_+397);
  CYC(b_+397, b_+398); tokay_add_a_to_hl(gb, b_+398);
  CYC(b_+398, b_+399); A = mem_rd(gb, HL);
  E = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+399, b_+401);
  CYC(b_+401, b_+402); mem_wr(gb, DE, A);
  CYC(b_+402, b_+403); ret_effect(gb);
  return;
subid12:
  E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+444, b_+446);
  CYC(b_+446, b_+447); A = mem_rd(gb, DE);
  alu_sub(gb, 0x12);
  SET_HL(b_+460);
  CYC(b_+447, b_+452);
  CYC(b_+452, b_+453); tokay_add_a_to_hl(gb, b_+453);
  E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+453, b_+455);
  CYC(b_+455, b_+456); A = mem_rd(gb, HL);
  CYC(b_+456, b_+457); mem_wr(gb, DE, A);
  CYC(b_+457, b_+460);
  TAIL(tokayLoadScript);
subid19:
  CYC(b_+467, b_+470); push_effect(gb, b_+470);
  tokay_init_subid0d(gb);
  CYC(b_+470, b_+473);
  TAIL(tokayLoadScript);
subid1a:
  E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  A = 0x02;
  CYC(b_+473, b_+477);
  CYC(b_+477, b_+478); mem_wr(gb, DE, A);
  E = INTERACTION_BASE + OBJ_ANIM_COUNTER;
  A = 0x01;
  CYC(b_+478, b_+482);
  CYC(b_+482, b_+483); mem_wr(gb, DE, A);
  CYC(b_+483, b_+486);
  TAIL(interactionAnimate);
subid1c:
  A = 0x09;
  CYC(b_+486, b_+488);
  CALL_C(b_+488, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+491);
  CALL_C(b_+491, tokayInitMeatAccessory_hook, SYM(tokayInitMeatAccessory), b_+494);
subid1b:
  CYC(b_+494, b_+495); ret_effect(gb);
  return;
subid1f:
  E = INTERACTION_BASE + OBJ_TEXT_ID;
  A = 0x6c;
  CYC(b_+495, b_+499);
  CYC(b_+499, b_+500); mem_wr(gb, DE, A);
  CYC(b_+500, b_+503);
  tokayLoadScript_hook(gb);
}

void interactionCode48_hook(GB *gb) {
  BASE(interactionCode48);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x44;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  if (tokay_jump_table(gb) != b_+8) { HANDOFF(HL); }
  CYC(b_+8, b_+10); A = 1;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+19); A = 0x0a;
  CALL_C(b_+19, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+22);
  CYC(b_+22, b_+25); push_effect(gb, b_+25);
  tokay_init_subid(gb);
  CYC(b_+25, b_+27); E = 0x40;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (F & FZ) { CYC(b_+29, b_+32); RET(b_+32); return; }
  CYCT(b_+29, b_+32); objectMarkSolidPosition_hook(gb);
}

void tokayState1_hook(GB *gb) {
  BASE(tokayState1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  uint8_t subid = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  tokay_jump_table(gb);
  switch (subid) {
    case 0: case 1: case 2: case 3: case 4: tokayRunSubid00_hook(gb); return;
    case 5: tokayRunSubid05_hook(gb); return;
    case 6: case 7: case 8: case 9: case 10: case 29: tokayRunSubid06_hook(gb); return;
    case 11: tokayRunSubid0b_hook(gb); return;
    case 12: tokayRunSubid0c_hook(gb); return;
    case 13: case 25: tokayRunSubid0d_hook(gb); return;
    case 14: case 17: tokayRunSubid0e_hook(gb); return;
    case 15: tokayRunSubid0f_hook(gb); return;
    case 16: tokayRunSubid10_hook(gb); return;
    case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 31: tokayRunSubid12_hook(gb); return;
    case 26: case 27: case 28: tokayRunSubid1a_hook(gb); return;
    case 30: tokayRunSubid1e_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void tokayRunSubid00_hook(GB *gb) {
  BASE(tokayRunSubid00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  tokay_jump_table(gb);
  switch (substate) {
    case 0: tokayThiefSubstate0_hook(gb); return;
    case 1: tokayThiefSubstate1_hook(gb); return;
    case 2: tokayThiefSubstate2_hook(gb); return;
    case 3: tokayThiefSubstate3_hook(gb); return;
    case 4: tokayThiefSubstate4_hook(gb); return;
    case 5: tokayThiefSubstate5_hook(gb); return;
    case 6: tokayThiefSubstate6_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void tokayThiefSubstate0_hook(GB *gb) {
  BASE(tokayThiefSubstate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_cp(gb, 2);
  if (F & FZ) CALL_C_CC(b_+5, tokayThief_countdownToStealNextItem_hook, SYM(tokayThief_countdownToStealNextItem), b_+8); else CYC(b_+5, b_+8);
  CYC(b_+8, b_+10); E = 0x79;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) CALL_C_CC(b_+12, interactionAnimateBasedOnSpeed_hook, SYM(interactionAnimateBasedOnSpeed), b_+15); else CYC(b_+12, b_+15);
  CALL_C(b_+15, interactionRunScript_hook, SYM(interactionRunScript), b_+18);
  if (!(F & FC)) { CYCT(b_+18, b_+19); return; } CYC(b_+18, b_+19);
  CYC(b_+19, b_+21); A = 5;
  CALL_C(b_+21, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+24);
  CALL_C(b_+24, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+27);
  CYC(b_+27, b_+29); L = 0x42;
  CYC(b_+29, b_+30); A = mem_rd(gb, HL);
  CYC(b_+30, b_+31); B = A;
  CYC(b_+31, b_+32); alu_or(gb, A);
  if (F & FZ) { CYC(b_+32, b_+34); CYC(b_+34, b_+36); A = 0x4c; CALL_C(b_+36, playSound_b00_hook, SYM(playSound_b00), b_+39); CYC(b_+39, b_+40); H = D; }
  else CYCT(b_+32, b_+34);
  CYC(b_+40, b_+42); L = 0x46;
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x5a);
  TAIL(tokayInitHeldItem);
}

void tokayInitHeldItem_hook(GB *gb) {
  BASE(tokayInitHeldItem);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); L = alu_inc8(gb, L);
  CYC(b_+5, b_+6); A = B;
  CYC(b_+6, b_+9); SET_BC(SYM(tokayItemGraphics));
  CALL_C(b_+9, addAToBc_hook, 0x006d, b_+12);
  CYC(b_+12, b_+13); A = mem_rd(gb, BC);
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL - 1);
  TAIL(tokayInitAccessory);
}

void tokayInitAccessory_hook(GB *gb) {
  BASE(tokayInitAccessory);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); mem_wr(gb, HL, 0x63);
  CYC(b_+2, b_+4); L = 0x56;
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x40);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+8); mem_wr(gb, HL, D);
  CYC(b_+8, b_+9); ret_effect(gb);
}

void tokayThief_countdownToStealNextItem_hook(GB *gb) {
  BASE(tokayThief_countdownToStealNextItem);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x78;
  CYC(b_+3, b_+4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+4, b_+5); ret_effect(gb); return; } CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); mem_wr(gb, HL, 0x0a);
  CYC(b_+7, b_+9); L = 0x7a;
  CYC(b_+9, b_+10); A = mem_rd(gb, HL);
  CYC(b_+10, b_+12); alu_cp(gb, 9);
  if (F & FZ) { CYCT(b_+12, b_+13); ret_effect(gb); return; } CYC(b_+12, b_+13);
  CYC(b_+13, b_+14); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+14, b_+17); SET_HL(SYM(tokayIslandStolenItems));
  CYC(b_+17, b_+18); tokay_add_a_to_hl(gb, b_+18);
  CYC(b_+18, b_+19); A = mem_rd(gb, HL);
  CYC(b_+19, b_+21); alu_cp(gb, 0x19);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto lose; } CYC(b_+21, b_+23);
  CALL_C(b_+23, loseTreasure_hook, SYM(loseTreasure), b_+26);
  CYC(b_+26, b_+28); A = 0x20;
  CALL_C(b_+28, loseTreasure_hook, SYM(loseTreasure), b_+31);
  CYC(b_+31, b_+33); A = 0x24;
lose:
  CALL_C(b_+33, loseTreasure_hook, SYM(loseTreasure), b_+36);
  CYC(b_+36, b_+38); A = 0x75;
  CYC(b_+38, b_+41); playSound_b00_hook(gb);
}

void tokayThiefSubstate1_hook(GB *gb) {
  BASE(tokayThiefSubstate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); L = 0x42;
  CYC(b_+6, b_+7); A = mem_rd(gb, HL);
  CYC(b_+7, b_+9); A = alu_swap(gb, A);
  CYC(b_+9, b_+11); alu_add(gb, 0x14);
  CYC(b_+11, b_+13); L = 0x46;
  CYC(b_+13, b_+14); mem_wr(gb, HL, A);
  CYC(b_+14, b_+17); interactionIncSubstate_hook(gb);
}

void tokayThiefSubstate2_hook(GB *gb) {
  BASE(tokayThiefSubstate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimate3Times_hook, SYM(interactionAnimate3Times), b_+3);
  CALL_C(b_+3, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+7); ret_effect(gb); return; } CYC(b_+6, b_+7);
  CYC(b_+7, b_+9); L = 0x49;
  CYC(b_+9, b_+11); mem_wr(gb, HL, 6);
  CYC(b_+11, b_+13); L = 0x50;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x64);
  TAIL(tokayThief_jump);
}

void tokayThief_jump_hook(GB *gb) {
  BASE(tokayThief_jump);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+3);
  CYC(b_+3, b_+6); SET_BC(0xfe40);
  CALL_C(b_+6, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+9);
  CYC(b_+9, b_+11); A = 5;
  CALL_C(b_+11, specialObjectSetAnimation_hook, SYM(specialObjectSetAnimation), b_+14);
  CYC(b_+14, b_+16); E = 0x60;
  CYC(b_+16, b_+18); A = 1;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CALL_C(b_+19, specialObjectAnimate_hook, SYM(specialObjectAnimate), b_+22);
  CYC(b_+22, b_+24); A = 0x53;
  CYC(b_+24, b_+27); playSound_b00_hook(gb);
}

void tokayThiefSubstate3_hook(GB *gb) {
  BASE(tokayThiefSubstate3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x20;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { CYCT(b_+5, b_+8); objectApplySpeed_hook(gb); return; } CYC(b_+5, b_+8);
  CALL_C(b_+8, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+11);
  CYC(b_+11, b_+13); L = 0x46;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 6);
  CYC(b_+15, b_+17); A = 5;
  CYC(b_+17, b_+20); interactionSetAnimation_hook(gb);
}

void tokayThiefSubstate4_hook(GB *gb) {
  BASE(tokayThiefSubstate4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); tokayThief_jump_hook(gb);
}

void tokayThiefSubstate5_hook(GB *gb) {
  BASE(tokayThiefSubstate5);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CALL_C(b_+3, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+6);
  if (F & FC) { CYCT(b_+6, b_+8); CYC(b_+26, b_+28); C = 0x20; CYC(b_+28, b_+31); objectUpdateSpeedZ_paramC_hook(gb); return; }
  CYC(b_+6, b_+8);
  CYC(b_+8, b_+10); E = 0x42;
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+13); alu_cp(gb, 3);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); CYC(b_+23, b_+26); interactionDelete_hook(gb); return; }
  CYC(b_+13, b_+15);
  CALL_C(b_+15, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+18);
  CYC(b_+18, b_+20); L = 0x46;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x3c);
  CYC(b_+22, b_+23); ret_effect(gb);
}

void tokayThiefSubstate6_hook(GB *gb) {
  BASE(tokayThiefSubstate6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+5); alu_xor(gb, A);
  CYC(b_+5, b_+8); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+8, b_+11); mem_wr(gb, wUseSimulatedInput, A);
  CYC(b_+11, b_+14); mem_wr(gb, wMenuDisabled, A);
  CALL_C(b_+14, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+17);
  CYC(b_+17, b_+19); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | 0x40));
  CYC(b_+19, b_+22); A = mem_rd(gb, wActiveMusic2);
  CYC(b_+22, b_+25); mem_wr(gb, wActiveMusic, A);
  CALL_C(b_+25, playSound_b00_hook, SYM(playSound_b00), b_+28);
  CALL_C(b_+28, setDeathRespawnPoint_hook, SYM(setDeathRespawnPoint), b_+31);
  CYC(b_+31, b_+34); interactionDelete_hook(gb);
}

void tokayRunSubid05_hook(GB *gb) {
  BASE(tokayRunSubid05);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) { CYCT(b_+3, b_+6); interactionDelete_hook(gb); return; } CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); E = 0x7f;
  CYC(b_+8, b_+9); A = mem_rd(gb, DE);
  CYC(b_+9, b_+10); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+10, b_+13); npcFaceLinkAndAnimate_hook(gb); return; } CYC(b_+10, b_+13);
  CALL_C(b_+13, tokayRunStinkBagCutscene_hook, SYM(tokayRunStinkBagCutscene), b_+16);
  CALL_C(b_+16, interactionAnimate_hook, SYM(interactionAnimate), b_+19);
  CYC(b_+19, b_+22); objectSetPriorityRelativeToLink_withTerrainEffects_hook(gb);
}

void tokayRunSubid06_hook(GB *gb) {
  BASE(tokayRunSubid06);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+5); E = 0x7b;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+10); interactionAnimateAsNpc_hook(gb); return; }
  CYC(b_+7, b_+10); npcFaceLinkAndAnimate_hook(gb);
}

void tokayRunSubid0b_hook(GB *gb) {
  BASE(tokayRunSubid0b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  if (F & FC) { CYCT(b_+3, b_+6); interactionDelete_hook(gb); return; }
  CYC(b_+3, b_+6); interactionAnimateBasedOnSpeed_hook(gb);
}

void tokayRunSubid0c_hook(GB *gb) {
  BASE(tokayRunSubid0c);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  tokay_jump_table(gb);
  if (substate == 0) wildTokayParticipantSubstate0_hook(gb);
  else if (substate == 1) wildTokayParticipantSubstate1_hook(gb);
  else if (substate == 2) wildTokayParticipantSubstate2_hook(gb);
  else HANDOFF(HL);
}

void wildTokayParticipantSubstate0_hook(GB *gb) {
  BASE(wildTokayParticipantSubstate0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, wildTokayParticipant_checkGrabMeat_hook, SYM(wildTokayParticipant_checkGrabMeat), SYM(wildTokayParticipantSubstate2));
  TAIL(wildTokayParticipantSubstate2);
}

void wildTokayParticipantSubstate2_hook(GB *gb) {
  BASE(wildTokayParticipantSubstate2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, objectApplySpeed_hook, SYM(objectApplySpeed), b_+3);
  CYC(b_+3, b_+5); E = 0x4b;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_add(gb, 8);
  CYC(b_+8, b_+10); alu_cp(gb, 0x90);
  if (F & FC) { CYCT(b_+10, b_+13); interactionAnimateBasedOnSpeed_hook(gb); return; } CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); E = 0x7c;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  CYC(b_+16, b_+17); alu_or(gb, A);
  if (F & FZ) { CYC(b_+17, b_+19); CYC(b_+19, b_+21); A = 0xff; CYC(b_+21, b_+24); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A); CYC(b_+24, b_+26); interactionDelete_hook(gb); return; }
  CYCT(b_+17, b_+19);
  CYC(b_+26, b_+28); E = 0x59;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); push_effect(gb, DE);
  CYC(b_+30, b_+31); D = A;
  CALL_C(b_+31, objectDelete_de_hook, SYM(objectDelete_de), b_+34);
  CYC(b_+34, b_+35); SET_DE(pop_effect(gb));
  CYC(b_+35, b_+37); E = 0x5c;
  CYC(b_+37, b_+38); A = mem_rd(gb, DE);
  CYC(b_+38, b_+40); alu_cp(gb, 2);
  if (F & FZ) { CYC(b_+40, b_+42); CYC(b_+42, b_+44); A = 1; CYC(b_+44, b_+47); mem_wr(gb, wTmpcfc0_genericCutscene_cfde, A); }
  else CYCT(b_+40, b_+42);
  CYC(b_+47, b_+50); interactionDelete_hook(gb);
}

void wildTokayParticipant_checkGrabMeat_hook(GB *gb) {
  BASE(wildTokayParticipant_checkGrabMeat);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1ReservedItemC);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; } CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); A = mem_rd(gb, wLinkGrabState);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+10); ret_effect(gb); return; } CYC(b_+9, b_+10);
  CYC(b_+10, b_+12); A = 0x0a;
  CYC(b_+12, b_+15); SET_HL(w1ReservedItemC_yh);
  CYC(b_+15, b_+16); B = mem_rd(gb, HL);
  CYC(b_+16, b_+18); L = 0x0d;
  CYC(b_+18, b_+19); C = mem_rd(gb, HL);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = 0x4b;
  CALL_C(b_+22, checkObjectIsCloseToPosition_b00_hook, SYM(checkObjectIsCloseToPosition_b00), b_+25);
  if (!(F & FC)) { CYCT(b_+25, b_+26); ret_effect(gb); return; } CYC(b_+25, b_+26);
  CALL_C(b_+26, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+29);
  CYC(b_+29, b_+31); L = 0x7c;
  CYC(b_+31, b_+32); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+32, b_+34); L = 0x46;
  CYC(b_+34, b_+36); mem_wr(gb, HL, 6);
  CYC(b_+36, b_+38); A = 7;
  CYC(b_+38, b_+40); L = 0x48;
  CYC(b_+40, b_+41); alu_add(gb, mem_rd(gb, HL));
  CALL_C(b_+41, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+44);
  CYC(b_+44, b_+45); push_effect(gb, DE);
  CYC(b_+45, b_+48); SET_DE(w1ReservedItemC);
  CALL_C(b_+48, objectDelete_de_hook, SYM(objectDelete_de), b_+51);
  CYC(b_+51, b_+54); SET_HL(wTmpcfc0_goronDance_roundIndex);
  CYC(b_+54, b_+55); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+55, b_+56); E = mem_rd(gb, HL);
  CYC(b_+56, b_+57); D = A;
  CALL_C(b_+57, objectDelete_de_hook, SYM(objectDelete_de), b_+60);
  CYC(b_+60, b_+61); SET_DE(pop_effect(gb));
  CYC(b_+61, b_+63); A = 0x6c;
  CALL_C(b_+63, playSound_b00_hook, SYM(playSound_b00), SYM(tokayInitMeatAccessory));
  TAIL(tokayInitMeatAccessory);
}

void tokayInitMeatAccessory_hook(GB *gb) {
  BASE(tokayInitMeatAccessory);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x63);
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x73);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+11, b_+13); L = 0x56;
  CYC(b_+13, b_+15); mem_wr(gb, HL, 0x40);
  CYC(b_+15, b_+16); L = alu_inc8(gb, L);
  CYC(b_+16, b_+17); mem_wr(gb, HL, D);
  CYC(b_+17, b_+19); E = 0x59;
  CYC(b_+19, b_+20); A = H;
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); ret_effect(gb);
}

void wildTokayParticipantSubstate1_hook(GB *gb) {
  BASE(wildTokayParticipantSubstate1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); interactionIncSubstate_hook(gb);
}

void tokayRunSubid0d_hook(GB *gb) {
  BASE(tokayRunSubid0d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  tokay_jump_table(gb);
  if (substate == 0) {
    CYC(b_+8, b_+11); A = mem_rd(gb, wPaletteThread_mode);
    CYC(b_+11, b_+12); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+12, b_+13); ret_effect(gb); return; } CYC(b_+12, b_+13);
    CALL_C(b_+13, interactionRunScript_hook, SYM(interactionRunScript), b_+16);
    if (!(F & FC)) { CYCT(b_+16, b_+19); interactionAnimateAsNpc_hook(gb); return; } CYC(b_+16, b_+19);
    CALL_C(b_+19, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+22);
    if (!(F & FZ)) { CYCT(b_+22, b_+23); ret_effect(gb); return; } CYC(b_+22, b_+23);
    CYC(b_+23, b_+25); mem_wr(gb, HL, 0x70);
    CALL_C(b_+25, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+28);
    CYC(b_+28, b_+30); A = 0xfb;
    CALL_C(b_+30, playSound_b00_hook, SYM(playSound_b00), b_+33);
    CYC(b_+33, b_+36); fadeoutToWhite_hook(gb);
    return;
  }
  if (substate == 1) {
    CYC(b_+36, b_+39); A = mem_rd(gb, wPaletteThread_mode);
    CYC(b_+39, b_+40); alu_or(gb, A);
    if (!(F & FZ)) { CYCT(b_+40, b_+41); ret_effect(gb); return; } CYC(b_+40, b_+41);
    CYC(b_+41, b_+42); push_effect(gb, DE);
    CALL_C(b_+42, clearAllItemsAndPutLinkOnGround_hook, SYM(clearAllItemsAndPutLinkOnGround), b_+45);
    CYC(b_+45, b_+46); SET_DE(pop_effect(gb));
    CYC(b_+46, b_+48); E = 0x42;
    CYC(b_+48, b_+49); A = mem_rd(gb, DE);
    CYC(b_+49, b_+51); alu_cp(gb, 0x19);
    if (F & FZ) { CYC(b_+51, b_+53); CYC(b_+53, b_+55); A = 1; CYC(b_+55, b_+58); mem_wr(gb, wTmpcfc0_wildTokay_inPresent, A); }
    else CYCT(b_+51, b_+53);
    CYC(b_+58, b_+61); interactionDelete_hook(gb);
    return;
  }
  HANDOFF(HL);
}

void tokayRunSubid0f_hook(GB *gb) {
  BASE(tokayRunSubid0f);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wScrollMode);
  CYC(b_+3, b_+5); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+5, b_+6); ret_effect(gb); return; } CYC(b_+5, b_+6);
  TAIL(tokayRunSubid10);
}

void tokayRunSubid10_hook(GB *gb) {
  BASE(tokayRunSubid10);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, w1Companion_var3e);
  CYC(b_+3, b_+5); alu_and(gb, 4);
  if (F & FZ) { CYC(b_+5, b_+7); tokayRunSubid0e_hook(gb); return; }
  CYCT(b_+5, b_+7);
  CALL_C((SYM(tokayRunSubid0e) + 3), interactionRunScript_hook, SYM(interactionRunScript), (SYM(tokayRunSubid0e) + 6));
  if (!(F & FC)) { CYCT((SYM(tokayRunSubid0e) + 6), (SYM(tokayRunSubid0e) + 7)); ret_effect(gb); return; } CYC((SYM(tokayRunSubid0e) + 6), (SYM(tokayRunSubid0e) + 7));
  CYC((SYM(tokayRunSubid0e) + 7), (SYM(tokayRunSubid0e) + 10)); interactionDelete_hook(gb);
}

void tokayRunSubid0e_hook(GB *gb) {
  BASE(tokayRunSubid0e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+3);
  CALL_C(b_+3, interactionRunScript_hook, SYM(interactionRunScript), b_+6);
  if (!(F & FC)) { CYCT(b_+6, b_+7); ret_effect(gb); return; } CYC(b_+6, b_+7);
  CYC(b_+7, b_+10); interactionDelete_hook(gb);
}

void tokayRunSubid1e_hook(GB *gb) {
  BASE(tokayRunSubid1e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x10;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  CALL_C(b_+5, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+8);
  CALL_C(b_+8, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+11);
  CYC(b_+11, b_+13); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+13, b_+16); interactionRunScript_hook(gb); return; } CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); C = 0x18;
  CALL_C(b_+18, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+21);
  if (!(F & FC)) { CYCT(b_+21, b_+22); ret_effect(gb); return; } CYC(b_+21, b_+22);
  CYC(b_+22, b_+24); E = 0x71;
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);
  CYC(b_+25, b_+28); interactionRunScript_hook(gb);
}

void tokayRunSubid12_hook(GB *gb) {
  BASE(tokayRunSubid12);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+6); npcFaceLinkAndAnimate_hook(gb);
}

void tokayRunSubid1a_hook(GB *gb) {
  BASE(tokayRunSubid1a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0_wildTokay_inPresent);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+4, b_+5); ret_effect(gb); return; } CYC(b_+4, b_+5);
  CYC(b_+5, b_+8); interactionDelete_hook(gb);
}

// tokayRunStinkBagCutscene@initJumpVariables: loads angle, speedZ and gravity for jump var3b
// from @jumpPaths.
static void tokay_init_jump_variables(GB *gb) {
  BASE(tokayRunStinkBagCutscene__initJumpVariables);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  H = D;
  L = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+0, b_+3);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  alu_add(gb, A);
  SET_BC(SYM(tokayRunStinkBagCutscene__jumpPaths));
  CYC(b_+4, b_+8);
  CALL_C(b_+8, addDoubleIndexToBc_hook, SYM(addDoubleIndexToBc), b_+11);
  CYC(b_+11, b_+12); A = mem_rd(gb, BC);
  SET_BC(BC + 1);
  L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+12, b_+15);
  CYC(b_+15, b_+16); mem_wr(gb, HL, A);
  CYC(b_+16, b_+17); A = mem_rd(gb, BC);
  SET_BC(BC + 1);
  L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(b_+17, b_+20);
  CYC(b_+20, b_+21); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+21, b_+22); A = mem_rd(gb, BC);
  SET_BC(BC + 1);
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+24); mem_wr(gb, HL, A);
  CYC(b_+24, b_+25); A = mem_rd(gb, BC);
  L = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CYC(b_+28, b_+29); ret_effect(gb);
}

void tokayRunStinkBagCutscene_hook(GB *gb) {
  BASE(tokayRunStinkBagCutscene);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x45;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  uint8_t substate = A;
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  tokay_jump_table(gb);
  if (substate == 0 || substate == 2) {
    if (substate == 0) { CYC(b_+10, b_+11); H = D; CYC(b_+11, b_+13); L = 0x50; CYC(b_+13, b_+15); mem_wr(gb, HL, 0x78); }
    if (substate == 2) { CYC(b_+136, b_+137); H = D; CYC(b_+137, b_+139); L = 0x7b; CYC(b_+139, b_+140); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); CYC(b_+140, b_+141); A = mem_rd(gb, HL); CYC(b_+141, b_+143); alu_cp(gb, 6); if (!(F & FC)) { CYC(b_+143, b_+145); CYC(b_+145, b_+147); mem_wr(gb, HL, 0); } else CYCT(b_+143, b_+145); CYC(b_+147, b_+150); }
    CYC(b_+15, b_+16); H = D; CYC(b_+16, b_+18); L = 0x4b; CYC(b_+18, b_+19); A = mem_rd(gb, HL); CYC(b_+19, b_+21); L = 0x79; CYC(b_+21, b_+22); mem_wr(gb, HL, A);
    CYC(b_+22, b_+24); L = 0x4d; CYC(b_+24, b_+25); A = mem_rd(gb, HL); CYC(b_+25, b_+27); L = 0x7a; CYC(b_+27, b_+28); mem_wr(gb, HL, A);
    CYC(b_+28, b_+29); H = D; CYC(b_+29, b_+31); L = 0x45; CYC(b_+31, b_+33); mem_wr(gb, HL, 1); CYC(b_+33, b_+35); L = 0x7e; CYC(b_+35, b_+37); mem_wr(gb, HL, 1);
    CYC(b_+37, b_+40); push_effect(gb, b_+40); tokay_init_jump_variables(gb); CYC(b_+40, b_+42); A = 0x53; CYC(b_+42, b_+45); playSound_b00_hook(gb); return;
  }
  if (substate == 1) {
    CYC(b_+98, b_+100); E = 0x7c; CYC(b_+100, b_+101); A = mem_rd(gb, DE); CYC(b_+101, b_+102); C = A;
    CALL_C(b_+102, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+105);
    if (!(F & FZ)) { CYCT(b_+105, b_+108); objectApplySpeed_hook(gb); return; } CYC(b_+105, b_+108);
    CALL_C(b_+108, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+111);
    CYC(b_+111, b_+113); L = 0x7b; CYC(b_+113, b_+114); A = mem_rd(gb, HL); CYC(b_+114, b_+116); alu_cp(gb, 5);
    if (!(F & FZ)) { CYCT(b_+116, b_+117); ret_effect(gb); return; } CYC(b_+116, b_+117);
    CYC(b_+117, b_+119); L = 0x4a; CYC(b_+119, b_+121); mem_wr(gb, HL, 0); CYC(b_+121, b_+122); L = alu_inc8(gb, L); CYC(b_+122, b_+124); mem_wr(gb, HL, 0x28);
    CYC(b_+124, b_+126); L = 0x4c; CYC(b_+126, b_+128); mem_wr(gb, HL, 0); CYC(b_+128, b_+129); L = alu_inc8(gb, L); CYC(b_+129, b_+131); mem_wr(gb, HL, 0x48); CYC(b_+131, b_+133); L = 0x7e; CYC(b_+133, b_+135); mem_wr(gb, HL, 0); CYC(b_+135, b_+136); ret_effect(gb); return;
  }
  HANDOFF(HL);
}

void tokayLoadScript_hook(GB *gb) {
  BASE(tokayLoadScript);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(SYM(tokayScriptTable));
  CYC(b_+6, b_+7); tokay_add_double_index(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); interactionSetScript_hook(gb);
}
