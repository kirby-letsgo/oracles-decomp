#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/subrosianShop.s.
// INTERAC_SUBROSIAN_SHOP
// Variables:
// var37 - wram variable to check for number of 2nd currency
// var38 - number of 2nd currency required
// var39 - ore chunk cost

static uint16_t subrosianShop_jump_table(GB *gb) {
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

static void subrosianShop_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void subrosianShop_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_SUBROSIAN_SHOP
// Variables:
// var37 - wram variable to check for number of 2nd currency
// var38 - number of 2nd currency required
// var39 - ore chunk cost
void s_interactionCode81_hook(GB *gb) {
  BASE(interactionCode81);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (subrosianShop_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == b_+203) goto state1;
    if (jt_ == b_+237) goto state2;
    if (jt_ == b_+267) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_or(gb, 0x80);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
func_7742:
  CYC(b_+21, b_+23); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+23, b_+24); A = mem_rd(gb, DE);
  CYC(b_+24, b_+26); alu_cp(gb, 0x0a);
  if (F & FZ) { CYCT(b_+26, b_+28); goto shield; }
  CYC(b_+26, b_+28);
  CYC(b_+28, b_+30); alu_cp(gb, 0x0d);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto func_7770; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+35); A = mem_rd(gb, wEssencesObtained);
  CYC(b_+35, b_+37); alu_bit(gb, 2, A);
  if (F & FZ) { CYCT(b_+37, b_+39); goto func_776b; }
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+41); A = 0x53;
  CALL_C(b_+41, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+44);
  if (F & FC) { CYCT(b_+44, b_+46); goto func_776b; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+48);
  goto func_7770;
shield:
  CYC(b_+48, b_+51); A = mem_rd(gb, wShieldLevel);
  CYC(b_+51, b_+53); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+53, b_+55); goto func_776b; }
  CYC(b_+53, b_+55);
  CYC(b_+55, b_+57); A = 0x01;
  CALL_C(b_+57, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+60);
  if (!(F & FC)) { CYCT(b_+60, b_+62); goto func_7770; }
  CYC(b_+60, b_+62);
func_776b:
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  CYC(b_+63, b_+64); A = alu_inc8(gb, A);
  CYC(b_+64, b_+65); mem_wr(gb, DE, A);
  CYC(b_+65, b_+67);
  goto func_7770;
func_7770:
  CYC(b_+67, b_+68); A = mem_rd(gb, DE);
  CYC(b_+68, b_+69); alu_add(gb, A);
  CYC(b_+69, b_+72); SET_HL(b_+113 /* @table_779e */);
  CYC(b_+72, b_+73); subrosianShop_add_double_index(gb, b_+73);
  CYC(b_+73, b_+76); A = mem_rd(gb, wBoughtSubrosianShopItems);
  CYC(b_+76, b_+77); alu_and(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+77, b_+79); goto func_7799; }
  CYC(b_+77, b_+79);
  CYC(b_+79, b_+80); SET_HL(HL + 1);
  CYC(b_+80, b_+82); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+82, b_+84); B = 0x03;
L_7781:
  CYC(b_+84, b_+85); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+85, b_+86); mem_wr(gb, DE, A);
  CYC(b_+86, b_+87); E = alu_inc8(gb, E);
  CYC(b_+87, b_+88); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+88, b_+90); goto L_7781; }
  CYC(b_+88, b_+90);
  CALL_C(b_+90, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+93);
  CYC(b_+93, b_+95); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+95, b_+97); A = 0x06;
  CYC(b_+97, b_+98); mem_wr(gb, DE, A);
  CYC(b_+98, b_+99); E = alu_inc8(gb, E);
  CYC(b_+99, b_+100); mem_wr(gb, DE, A);
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+102, s_objectAddToAButtonSensitiveObjectList, SYM(objectAddToAButtonSensitiveObjectList), b_+105);
  CYC(b_+105, b_+108);
  TAIL(objectSetVisible82);
func_7799:
  CYC(b_+108, b_+109); A = mem_rd(gb, DE);
  CYC(b_+109, b_+110); A = alu_inc8(gb, A);
  CYC(b_+110, b_+111); mem_wr(gb, DE, A);
  CYC(b_+111, b_+113);
  goto func_7742;

state1:
  CALL_C(b_+203, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+206);
  CYC(b_+206, b_+208); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CYC(b_+208, b_+209); A = mem_rd(gb, DE);
  CYC(b_+209, b_+210); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+210); return; }
  CYC(b_+210, b_+211);
  CYC(b_+211, b_+212); alu_xor(gb, A);
  CYC(b_+212, b_+213); mem_wr(gb, DE, A);
  CYC(b_+213, b_+215); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+215, b_+216); mem_wr(gb, DE, A);
  CALL_C(b_+216, s_func_7931_hook, SYM(func_7931), b_+219);
  CYC(b_+219, b_+221); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+221, b_+223); A = 0x02;
  CYC(b_+223, b_+224); mem_wr(gb, DE, A);
  CYC(b_+224, b_+226); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+226, b_+227); A = mem_rd(gb, DE);
  CYC(b_+227, b_+230); SET_HL(SYM(table_7994));
  CYC(b_+230, b_+231); subrosianShop_add_double_index(gb, b_+231);
  CYC(b_+231, b_+232); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+232, b_+233); H = mem_rd(gb, HL);
  CYC(b_+233, b_+234); L = A;
  CYC(b_+234, b_+237);
  TAIL(interactionSetScript);
state2:
  CALL_C(b_+237, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+240);
  CALL_C(b_+240, s_interactionRunScript, SYM(interactionRunScript), b_+243);
  if (!(F & FC)) { RET_TAKEN(b_+243); return; }
  CYC(b_+243, b_+244);
  CYC(b_+244, b_+246); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+246, b_+247); A = mem_rd(gb, DE);
  CYC(b_+247, b_+249); alu_bit(gb, 7, A);
  CYC(b_+249, b_+251); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+251, b_+253); A = 0x01;
  CYC(b_+253, b_+254); mem_wr(gb, DE, A);
  if (!(F & FZ)) { RET_TAKEN(b_+254); return; }
  CYC(b_+254, b_+255);
  CYC(b_+255, b_+257); A = 0x03;
  CYC(b_+257, b_+258); mem_wr(gb, DE, A);
  CYC(b_+258, b_+259); E = alu_inc8(gb, E);
  CYC(b_+259, b_+260); alu_xor(gb, A);
  CYC(b_+260, b_+261); mem_wr(gb, DE, A);
  CYC(b_+261, b_+263); A = 0x80;
  CYC(b_+263, b_+266); mem_wr(gb, wMenuDisabled, A);
  RET(b_+266); return;
state3:
  CYC(b_+267, b_+269); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+269, b_+270); A = mem_rd(gb, DE);
  CYC(b_+270, b_+271); push_effect(gb, b_+271);
  do { uint16_t jt_ = (subrosianShop_jump_table(gb));
    if (jt_ == b_+281) goto substate0;
    if (jt_ == b_+403) goto substate1;
    if (jt_ == b_+416) goto substate2;
    if (jt_ == b_+462) goto substate3;
    if (jt_ == b_+495) goto substate4;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+281, s_objectSetVisible80, SYM(objectSetVisible80), b_+284);
  CYC(b_+284, b_+287); A = mem_rd(gb, wInShop);
  CYC(b_+287, b_+288); A = alu_dec8(gb, A);
  CYC(b_+288, b_+291); mem_wr(gb, wInShop, A);
  CALL_C(b_+291, s_func_7973_hook, SYM(func_7973), b_+294);
  CYC(b_+294, b_+296); A = 0x04;
  CYC(b_+296, b_+299); mem_wr(gb, wLinkForceState, A);
  CYC(b_+299, b_+301); A = 0x01;
  CYC(b_+301, b_+304); mem_wr(gb, wcc50, A);
  CYC(b_+304, b_+305); H = D;
  CYC(b_+305, b_+307); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+307, b_+310); A = mem_rd(gb, w1Link_yh);
  CYC(b_+310, b_+312); alu_sub(gb, 0x0e);
  CYC(b_+312, b_+313); mem_wr(gb, HL, A);
  CYC(b_+313, b_+315); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+315, b_+318); A = mem_rd(gb, w1Link_xh);
  CYC(b_+318, b_+319); mem_wr(gb, HL, A);
  CYC(b_+319, b_+321); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+321, b_+323); A = 0x80;
  CYC(b_+323, b_+324); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+324, b_+326); mem_wr(gb, HL, 0x04);
  CYC(b_+326, b_+328); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+328, b_+330); A = 0x01;
  CYC(b_+330, b_+331); mem_wr(gb, HL, A);
  CYC(b_+331, b_+334); SET_HL(wStatusBarNeedsRefresh);
  CYC(b_+334, b_+336); mem_wr(gb, HL, mem_rd(gb, HL) | 0x04);
  CYC(b_+336, b_+338); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+338, b_+339); A = mem_rd(gb, DE);
  CYC(b_+339, b_+341); alu_cp(gb, 0x01);
  if (F & FZ) { CYCT(b_+341, b_+343); goto func_78ac; }
  CYC(b_+341, b_+343);
  CYC(b_+343, b_+346); SET_HL(b_+173 /* @table_77da */);
  CYC(b_+346, b_+347); subrosianShop_add_double_index(gb, b_+347);
  CYC(b_+347, b_+348); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+348, b_+349); C = mem_rd(gb, HL);
  CYC(b_+349, b_+351); alu_cp(gb, 0x2d);
  if (!(F & FZ)) { CYCT(b_+351, b_+353); goto L_7891; }
  CYC(b_+351, b_+353);
  CALL_C(b_+353, s_getRandomRingOfGivenTier, SYM(getRandomRingOfGivenTier), b_+356);
L_7891:
  CALL_C(b_+356, s_giveTreasure, SYM(giveTreasure), b_+359);
  CYC(b_+359, b_+361); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+361, b_+362); A = mem_rd(gb, DE);
  CYC(b_+362, b_+365); SET_HL(b_+388 /* @table_78b1 */);
  CYC(b_+365, b_+366); subrosianShop_add_a_to_hl(gb, b_+366);
  CYC(b_+366, b_+367); C = mem_rd(gb, HL);
  CYC(b_+367, b_+369); B = 0x00;
  CALL_C(b_+369, s_showText, SYM(showText), b_+372);
  CYC(b_+372, b_+374); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+374, b_+375); A = mem_rd(gb, DE);
  CYC(b_+375, b_+377); alu_cp(gb, 0x04);
  if (F & FZ) { RET_TAKEN(b_+377); return; }
  CYC(b_+377, b_+378);
  CYC(b_+378, b_+380); A = 0x4c;
  CYC(b_+380, b_+383);
  TAIL(playSound_b00);
func_78ac:
  CYC(b_+383, b_+384); H = D;
  CYC(b_+384, b_+386); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+386, b_+387); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+387); return;

substate1:
  CALL_C(b_+403, s_retIfTextIsActive, SYM(retIfTextIsActive), b_+406);
  CYC(b_+406, b_+407); alu_xor(gb, A);
  CYC(b_+407, b_+410); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+410, b_+413); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+413, b_+416);
  TAIL(interactionDelete);
substate2:
  CALL_C(b_+416, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+419);
  if (F & FZ) { CYCT(b_+419, b_+421); goto L_78de; }
  CYC(b_+419, b_+421);
  CYC(b_+421, b_+422); L = alu_inc8(gb, L);
  CYC(b_+422, b_+423); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+423); return; }
  CYC(b_+423, b_+424);
  CYC(b_+424, b_+426); mem_wr(gb, HL, 0x04);
  CYC(b_+426, b_+428); A = 0x01;
  CYC(b_+428, b_+430); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+430, b_+431); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+431, b_+432); mem_wr(gb, HL, A);
  RET(b_+432); return;
L_78de:
  CYC(b_+433, b_+435); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+435, b_+437); A = 0x0a;
  CYC(b_+437, b_+438); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+438, b_+439); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+439, b_+441); mem_wr(gb, HL, 0x0c);
  CYC(b_+441, b_+443); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+443, b_+445); mem_wr(gb, HL, 0x03);
  CYC(b_+445, b_+447); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+447, b_+449); mem_wr(gb, HL, 0x1e);
  CYC(b_+449, b_+451); A = 0x08;
  CALL_C(b_+451, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+454);
  CYC(b_+454, b_+456); A = 0xbc;
  CALL_C(b_+456, s_playSound, SYM(playSound_b00), b_+459);
  CYC(b_+459, b_+462);
  TAIL(fadeoutToWhite);
substate3:
  CALL_C(b_+462, s_interactionAnimate, SYM(interactionAnimate), b_+465);
  CYC(b_+465, b_+468); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+468, b_+469); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+469); return; }
  CYC(b_+469, b_+470);
  CALL_C(b_+470, s_interactionDecCounter2, SYM(interactionDecCounter2), b_+473);
  if (!(F & FZ)) { RET_TAKEN(b_+473); return; }
  CYC(b_+473, b_+474);
  CYC(b_+474, b_+476); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+476, b_+477); mem_wr(gb, HL, A);
  CYC(b_+477, b_+479); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+479, b_+481); mem_wr(gb, HL, 0x04);
  CYC(b_+481, b_+484); SET_HL(wMaxBombs);
  CYC(b_+484, b_+485); A = mem_rd(gb, HL);
  CYC(b_+485, b_+487); alu_add(gb, 0x20);
  CYC(b_+487, b_+488); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+488, b_+489); mem_wr(gb, HL, A);
  CALL_C(b_+489, s_setStatusBarNeedsRefreshBit1, SYM(setStatusBarNeedsRefreshBit1), b_+492);
  CYC(b_+492, b_+495);
  TAIL(fadeinFromWhite);
substate4:
  CYC(b_+495, b_+498); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+498, b_+499); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+499); return; }
  CYC(b_+499, b_+500);
  CYC(b_+500, b_+501); alu_xor(gb, A);
  CYC(b_+501, b_+504); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+504, b_+507); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+507, b_+510); SET_BC(0x2b0e);
  CALL_C(b_+510, s_showText, SYM(showText), b_+513);
  CYC(b_+513, b_+516);
  TAIL(interactionDelete);
}

void s_func_7931_hook(GB *gb) {
  BASE(func_7931);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+2, b_+3); alu_xor(gb, A);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+6); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+6, b_+7); A = mem_rd(gb, DE);
  CYC(b_+7, b_+8); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+8, b_+10); TAIL_S(buyingRibbon); }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+14, b_+16); goto L_794b; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+19); H = 0xc6;
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_cp(gb, B);
  if (F & FC) { CYCT(b_+24, b_+26); goto L_7950; }
  CYC(b_+24, b_+26);
L_794b:
  CYC(b_+26, b_+28); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+28, b_+30); A = 0x01;
  CYC(b_+30, b_+31); mem_wr(gb, DE, A);
L_7950:
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+33, b_+34); A = mem_rd(gb, DE);
  CALL_C(b_+34, s_cpOreChunkValue_hook, SYM(cpOreChunkValue), b_+37);
  CYC(b_+37, b_+40); SET_HL(wTextNumberSubstitution);
  CYC(b_+40, b_+41); mem_wr(gb, HL, C);
  CYC(b_+41, b_+42); L = alu_inc8(gb, L);
  CYC(b_+42, b_+43); mem_wr(gb, HL, B);
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+45, b_+47); alu_xor(gb, 0x01);
  if (F & FZ) { CYCT(b_+47, b_+49); goto L_7965; }
  CYC(b_+47, b_+49);
  CYC(b_+49, b_+50); C = A;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_and(gb, C);
L_7965:
  CYC(b_+52, b_+53); mem_wr(gb, DE, A);
  RET(b_+53); return;
}

void s_buyingRibbon_hook(GB *gb) {
  BASE(buyingRibbon);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x45;
  CALL_C(b_+2, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+5);
  if (!(F & FC)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  RET(b_+11); return;
}

void s_func_7973_hook(GB *gb) {
  BASE(func_7973);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); A = 0xff;
  CYC(b_+7, b_+10); mem_wr(gb, wStatusBarNeedsRefresh, A);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+14, b_+16); goto L_798d; }
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+19); H = 0xc6;
  CYC(b_+19, b_+20); E = alu_inc8(gb, E);
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); C = A;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+24); alu_sub(gb, C);
  CYC(b_+24, b_+25); alu_daa(gb);
  CYC(b_+25, b_+26); mem_wr(gb, HL, A);
L_798d:
  CYC(b_+26, b_+28); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); C = A;
  CYC(b_+30, b_+33);
  TAIL_S(removeOreChunkValue);
}

