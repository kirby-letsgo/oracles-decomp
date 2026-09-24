#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/swordShieldMazeFreezingLavaEvent.s.
// INTERAC_D8_FREEZING_LAVA_EVENT

static uint16_t swordShieldMazeFreezingLavaEvent_jump_table(GB *gb) {
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

static void swordShieldMazeFreezingLavaEvent_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void swordShieldMazeFreezingLavaEvent_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_D8_FREEZING_LAVA_EVENT
void s_interactionCode69_hook(GB *gb) {
  BASE(interactionCode69);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (swordShieldMazeFreezingLavaEvent_jump_table(gb));
    if (jt_ == b_+14) goto subid0;
    if (jt_ == b_+288) goto subid1;
    if (jt_ == b_+361) goto subid2;
    if (jt_ == b_+386) goto subid3;
    if (jt_ == b_+431) goto subid4;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (swordShieldMazeFreezingLavaEvent_jump_table(gb));
    if (jt_ == b_+26) goto subid0_state0;
    if (jt_ == b_+128) goto subid0_state1;
    if (jt_ == b_+163) goto subid0_state2;
    if (jt_ == b_+207) goto subid0_state3;
    HANDOFF(HL);
  } while (0);
subid0_state0:
  CYC(b_+26, b_+28); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+30); push_effect(gb, b_+30);
  do { uint16_t jt_ = (swordShieldMazeFreezingLavaEvent_jump_table(gb));
    if (jt_ == b_+34) goto subid0_state0_substate0;
    if (jt_ == b_+93) goto subid0_state0_substate1;
    HANDOFF(HL);
  } while (0);
subid0_state0_substate0:
  CALL_C(b_+34, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+37);
  CYC(b_+37, b_+39); alu_bit(gb, 6, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+39, b_+42); TAIL(interactionDelete); }
  CYC(b_+39, b_+42);
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+47); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+51); SET_HL(b_+89 /* @subid0@state0@table_5976 */);
  CYC(b_+51, b_+52); swordShieldMazeFreezingLavaEvent_add_a_to_hl(gb, b_+52);
  CYC(b_+52, b_+53); B = mem_rd(gb, HL);
  CALL_C(b_+53, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+56);
  CYC(b_+56, b_+57); L = B;
  CYC(b_+57, b_+59); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+59, b_+62); TAIL(interactionDelete); }
  CYC(b_+59, b_+62);
  CALL_C(b_+62, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+65);
  CYC(b_+65, b_+67); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+67, b_+69); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+69, b_+71); A = 0x01;
  CYC(b_+71, b_+72); mem_wr(gb, DE, A);
  CYC(b_+72, b_+74); A = 0xf0;
  CALL_C(b_+74, s_playSound, SYM(playSound_b00), b_+77);
  CYC(b_+77, b_+79); A = 0xff;
  CYC(b_+79, b_+82); mem_wr(gb, wActiveMusic, A);
  CYC(b_+82, b_+84); A = 0x80;
  CYC(b_+84, b_+87); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+87, b_+89);
  goto subid0_state0_substate1;

subid0_state0_substate1:
  CALL_C(b_+93, s_func_5ae0_hook, SYM(func_5ae0), b_+96);
  CYC(b_+96, b_+99); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+99, b_+100); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+100); return; }
  CYC(b_+100, b_+101);
  CYC(b_+101, b_+103); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+103, b_+105); A = 0x01;
  CYC(b_+105, b_+106); mem_wr(gb, DE, A);
  CYC(b_+106, b_+107); alu_xor(gb, A);
  CYC(b_+107, b_+108); E = alu_inc8(gb, E);
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CALL_C(b_+109, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+112);
  if (!(F & FZ)) { CYCT(b_+112, b_+115); TAIL(interactionDelete); }
  CYC(b_+112, b_+115);
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x69);
  CYC(b_+117, b_+118); L = alu_inc8(gb, L);
  CYC(b_+118, b_+120); mem_wr(gb, HL, 0x01);
  CYC(b_+120, b_+122); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+122, b_+123); A = mem_rd(gb, DE);
  CYC(b_+123, b_+125); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+125, b_+128);
  TAIL(setShortPosition);
subid0_state1:
  CYC(b_+128, b_+131); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+131, b_+132); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+132); return; }
  CYC(b_+132, b_+133);
  CYC(b_+133, b_+135); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+135, b_+137); A = 0x02;
  CYC(b_+137, b_+138); mem_wr(gb, DE, A);
  CYC(b_+138, b_+140); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+140, b_+141); A = mem_rd(gb, DE);
  CYC(b_+141, b_+144); SET_HL(SYM(table_5b0d));
  CYC(b_+144, b_+145); swordShieldMazeFreezingLavaEvent_add_double_index(gb, b_+145);
  CYC(b_+145, b_+147); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+147, b_+148); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+148, b_+149); mem_wr(gb, DE, A);
  CYC(b_+149, b_+150); E = alu_inc8(gb, E);
  CYC(b_+150, b_+151); A = mem_rd(gb, HL);
  CYC(b_+151, b_+152); mem_wr(gb, DE, A);
  CYC(b_+152, b_+153); H = D;
  CYC(b_+153, b_+155); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+155, b_+157); mem_wr(gb, HL, 0x14);
  CYC(b_+157, b_+158); L = alu_inc8(gb, L);
  CYC(b_+158, b_+160); mem_wr(gb, HL, 0x03);
  CALL_C(b_+160, s_fastFadeoutToWhite, SYM(fastFadeoutToWhite), b_+163);
subid0_state2:
  CYC(b_+163, b_+165); A = 0x3c;
  CALL_C(b_+165, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+168);
  CALL_C(b_+168, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+171);
  if (!(F & FZ)) { RET_TAKEN(b_+171); return; }
  CYC(b_+171, b_+172);
  CYC(b_+172, b_+174); mem_wr(gb, HL, 0x14);
  CYC(b_+174, b_+175); L = alu_inc8(gb, L);
  CYC(b_+175, b_+176); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+176, b_+179); TAIL(fastFadeoutToWhite); }
  CYC(b_+176, b_+179);
  CYC(b_+179, b_+181); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+181, b_+182); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+182, s_clearPaletteFadeVariablesAndRefreshPalettes, SYM(clearPaletteFadeVariablesAndRefreshPalettes), b_+185);
  CALL_C(b_+185, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+188);
  if (!(F & FZ)) { CYCT(b_+188, b_+190); goto subid0_state3; }
  CYC(b_+188, b_+190);
  CYC(b_+190, b_+192); mem_wr(gb, HL, 0x69);
  CYC(b_+192, b_+193); L = alu_inc8(gb, L);
  CYC(b_+193, b_+195); mem_wr(gb, HL, 0x04);
  CYC(b_+195, b_+197); E = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+197, b_+198); L = E;
  CYC(b_+198, b_+199); A = mem_rd(gb, DE);
  CYC(b_+199, b_+200); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+200, b_+201); E = alu_inc8(gb, E);
  CYC(b_+201, b_+202); A = mem_rd(gb, DE);
  CYC(b_+202, b_+203); mem_wr(gb, HL, A);
  CYC(b_+203, b_+205); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+205, b_+207); mem_wr(gb, HL, 0x84);
subid0_state3:
  CYC(b_+207, b_+209); A = 0x3c;
  CALL_C(b_+209, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+212);
  CYC(b_+212, b_+215); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+215, b_+216); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+216); return; }
  CYC(b_+216, b_+217);
  CALL_C(b_+217, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+220);
  if (!(F & FZ)) { RET_TAKEN(b_+220); return; }
  CYC(b_+220, b_+221);
  CYC(b_+221, b_+223); mem_wr(gb, HL, 0x08);
  CYC(b_+223, b_+225); A = 0x7a;
  CALL_C(b_+225, s_playSound, SYM(playSound_b00), b_+228);
  CYC(b_+228, b_+230); B = 0xd6;
  CALL_C(b_+230, s_func_5af7_hook, SYM(func_5af7), b_+233);
  if (!(F & FZ)) { RET_TAKEN(b_+233); return; }
  CYC(b_+233, b_+234);
  CYC(b_+234, b_+237);
  TAIL(interactionDelete);
func_5a0a:
  CYC(b_+237, b_+240); A = mem_rd(gb, wDungeonFloor);
  CYC(b_+240, b_+241); A = alu_inc8(gb, A);
  CYC(b_+241, b_+244); mem_wr(gb, wDungeonFloor, A);
  CALL_C(b_+244, s_getActiveRoomFromDungeonMapPosition, SYM(getActiveRoomFromDungeonMapPosition), b_+247);
  CYC(b_+247, b_+250); mem_wr(gb, wWarpDestRoom, A);
  CYC(b_+250, b_+253); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+253, b_+256); mem_wr(gb, wWarpDestPos, A);
  CYC(b_+256, b_+259); A = mem_rd(gb, wActiveGroup);
  CYC(b_+259, b_+261); alu_or(gb, 0x80);
  CYC(b_+261, b_+264); mem_wr(gb, wWarpDestGroup, A);
  CYC(b_+264, b_+265); alu_xor(gb, A);
  CYC(b_+265, b_+268); mem_wr(gb, wWarpTransition, A);
  CYC(b_+268, b_+270); A = 0x03;
  CYC(b_+270, b_+273); mem_wr(gb, wWarpTransition2, A);
  CALL_C(b_+273, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+276);
  CYC(b_+276, b_+278); mem_wr(gb, HL, mem_rd(gb, HL) & 0xef);
  CYC(b_+278, b_+279); alu_xor(gb, A);
  CYC(b_+279, b_+282); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+282, b_+285); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+285, b_+288);
  TAIL(interactionDelete);
subid1:
  CYC(b_+288, b_+290); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+290, b_+291); A = mem_rd(gb, DE);
  CYC(b_+291, b_+292); push_effect(gb, b_+292);
  do { uint16_t jt_ = (swordShieldMazeFreezingLavaEvent_jump_table(gb));
    if (jt_ == b_+296) goto subid1_state0;
    if (jt_ == b_+327) goto subid1_state1;
    HANDOFF(HL);
  } while (0);
subid1_state0:
  CYC(b_+296, b_+298); A = 0x01;
  CYC(b_+298, b_+299); mem_wr(gb, DE, A);
  CALL_C(b_+299, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+302);
  CALL_C(b_+302, s_objectGetZAboveScreen, SYM(objectGetZAboveScreen), b_+305);
  CYC(b_+305, b_+307); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+307, b_+308); mem_wr(gb, DE, A);
  CALL_C(b_+308, s_objectSetVisiblec0, SYM(objectSetVisiblec0), b_+311);
  CALL_C(b_+311, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+314);
  if (!(F & FZ)) { RET_TAKEN(b_+314); return; }
  CYC(b_+314, b_+315);
  CYC(b_+315, b_+317); mem_wr(gb, HL, 0x69);
  CYC(b_+317, b_+318); L = alu_inc8(gb, L);
  CYC(b_+318, b_+320); mem_wr(gb, HL, 0x02);
  CYC(b_+320, b_+322); E = INTERACTION_BASE + OBJ_VAR19;
  CYC(b_+322, b_+323); A = H;
  CYC(b_+323, b_+324); mem_wr(gb, DE, A);
  CYC(b_+324, b_+327);
  TAIL(objectCopyPosition);
subid1_state1:
  CYC(b_+327, b_+329); E = INTERACTION_BASE + OBJ_VAR19;
  CYC(b_+329, b_+330); A = mem_rd(gb, DE);
  CYC(b_+330, b_+331); H = A;
  CYC(b_+331, b_+333); L = INTERACTION_BASE + OBJ_Y;
  CYC(b_+333, b_+334); E = L;
  CYC(b_+334, b_+336); B = 0x06;
  CALL_C(b_+336, s_copyMemoryReverse, SYM(copyMemoryReverse), b_+339);
  CYC(b_+339, b_+341); C = 0x08;
  CALL_C(b_+341, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+344);
  if (!(F & FZ)) { RET_TAKEN(b_+344); return; }
  CYC(b_+344, b_+345);
  CYC(b_+345, b_+348); SET_BC(0x6903);
  CALL_C(b_+348, s_objectCreateInteraction, SYM(objectCreateInteraction), b_+351);
  if (!(F & FZ)) { CYCT(b_+351, b_+353); goto L_5a83; }
  CYC(b_+351, b_+353);
  CYC(b_+353, b_+355); A = 0x01;
  CYC(b_+355, b_+358); mem_wr(gb, wTmpcfc0, A);
L_5a83:
  CYC(b_+358, b_+361);
  TAIL(interactionDelete);
subid2:
  CYC(b_+361, b_+363); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+363, b_+364); A = mem_rd(gb, DE);
  CYC(b_+364, b_+365); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+365, b_+367); goto L_5a95; }
  CYC(b_+365, b_+367);
  CYC(b_+367, b_+369); A = 0x01;
  CYC(b_+369, b_+370); mem_wr(gb, DE, A);
  CALL_C(b_+370, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+373);
  CALL_C(b_+373, s_objectSetVisible83, SYM(objectSetVisible83), b_+376);
L_5a95:
  CYC(b_+376, b_+379); A = mem_rd(gb, wTmpcfc0);
  CYC(b_+379, b_+380); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+380, b_+383); TAIL(interactionDelete); }
  CYC(b_+380, b_+383);
  CYC(b_+383, b_+386);
  TAIL(interactionAnimate);
subid3:
  CYC(b_+386, b_+388); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+388, b_+389); A = mem_rd(gb, DE);
  CYC(b_+389, b_+390); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+390, b_+392); goto L_5ab3; }
  CYC(b_+390, b_+392);
  CYC(b_+392, b_+394); A = 0x01;
  CYC(b_+394, b_+395); mem_wr(gb, DE, A);
  CALL_C(b_+395, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+398);
  CALL_C(b_+398, s_objectSetVisible83, SYM(objectSetVisible83), b_+401);
  CYC(b_+401, b_+403); A = 0x5c;
  CALL_C(b_+403, s_playSound, SYM(playSound_b00), b_+406);
L_5ab3:
  CALL_C(b_+406, s_interactionAnimate, SYM(interactionAnimate), b_+409);
  CYC(b_+409, b_+411); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+411, b_+412); A = mem_rd(gb, DE);
  CYC(b_+412, b_+413); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+413); return; }
  CYC(b_+413, b_+414);
  CYC(b_+414, b_+416); A = 0xff;
  CYC(b_+416, b_+419); mem_wr(gb, wTmpcfc0, A);
  CALL_C(b_+419, s_objectGetShortPosition, SYM(objectGetShortPosition), b_+422);
  CYC(b_+422, b_+423); C = A;
  CYC(b_+423, b_+425); A = 0xd5;
  CALL_C(b_+425, s_setTile, SYM(setTile), b_+428);
  CYC(b_+428, b_+431);
  TAIL(interactionDelete);
subid4:
  CYC(b_+431, b_+434); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+434, b_+435); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+435); return; }
  CYC(b_+435, b_+436);
  CALL_C(b_+436, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+439);
  if (!(F & FZ)) { RET_TAKEN(b_+439); return; }
  CYC(b_+439, b_+440);
  CYC(b_+440, b_+442); mem_wr(gb, HL, 0x08);
  CYC(b_+442, b_+444); B = 0xd7;
  CALL_C(b_+444, s_func_5af7_hook, SYM(func_5af7), b_+447);
  if (!(F & FZ)) { RET_TAKEN(b_+447); return; }
  CYC(b_+447, b_+448);
  CYC(b_+448, b_+451);
  goto func_5a0a;
}

void s_func_5ae0_hook(GB *gb) {
  BASE(func_5ae0);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1ReservedInteraction0_var3f + 0x01);
L_5ae3:
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (!(F & FZ)) CALL_C_CC(b_+5, s_func_5aef_hook, SYM(func_5aef), b_+8);
  else CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); H = alu_inc8(gb, H);
  CYC(b_+9, b_+10); A = H;
  CYC(b_+10, b_+12); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+12, b_+14); goto L_5ae3; }
  CYC(b_+12, b_+14);
  RET(b_+14); return;
}

void s_func_5aef_hook(GB *gb) {
  BASE(func_5aef);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); L = ENEMY_BASE + OBJ_VISIBLE;
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+6); L = ENEMY_BASE + OBJ_ENABLED;
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  RET(b_+7); return;
}

void s_func_5af7_hook(GB *gb) {
  BASE(func_5af7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_RELATED2;
  CYC(b_+3, b_+4); E = L;
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); H = mem_rd(gb, HL);
  CYC(b_+6, b_+7); L = A;
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+9); return; }
  CYC(b_+9, b_+10);
  CYC(b_+10, b_+11); C = A;
  CYC(b_+11, b_+12); A = L;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+14); E = alu_inc8(gb, E);
  CYC(b_+14, b_+15); A = H;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CYC(b_+16, b_+17); A = B;
  CALL_C(b_+17, s_setTile, SYM(setTile), b_+20);
  CYC(b_+20, b_+21); alu_or(gb, D);
  RET(b_+21); return;
}

