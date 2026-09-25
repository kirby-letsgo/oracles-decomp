#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/kingMoblin.s.
// INTERAC_KING_MOBLIN

static uint16_t kingMoblin_jump_table(GB *gb) {
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

static void kingMoblin_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode95@state1@func_557c
static void kingMoblin_state1_func_557c(GB *gb) {
  BASE(interactionCode95);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+347, b_+348); H = D;
  CYC(b_+348, b_+350); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+350, b_+351); A = mem_rd(gb, HL);
  CYC(b_+351, b_+353); alu_cp(gb, 0x70);
  if (!(F & FZ)) { RET_TAKEN(b_+353); return; }
  CYC(b_+353, b_+354);
  CYC(b_+354, b_+356); mem_wr(gb, HL, 0x00);
  CYC(b_+356, b_+359);
  TAIL(playSound_b00);
}

// interactionCode96@state1@subid2
static void kingMoblin_state1_subid2(GB *gb) {
  BASE(interactionCode96);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+363, s_interactionAnimate, SYM(interactionAnimate), b_+366);
  CALL_C(b_+366, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+369);
  if (!(F & FZ)) { CYCT(b_+369, b_+371); goto L_5743; }
  CYC(b_+369, b_+371);
  CALL_C(b_+371, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+374);
  if (!(F & FZ)) { RET_TAKEN(b_+374); return; }
  CYC(b_+374, b_+375);
  CYC(b_+375, b_+377); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+377, b_+379); mem_wr(gb, HL, 0x50);
  CYC(b_+379, b_+382);
  TAIL(interactionIncSubstate);
L_5743:
  CALL_C(b_+382, s_interactionAnimate, SYM(interactionAnimate), b_+385);
  CALL_L(b_+385, kingMoblin_state1_func_557c, b_+388);
  CALL_C(b_+388, s_objectApplySpeed, SYM(objectApplySpeed), b_+391);
  CALL_C(b_+391, s_objectCheckWithinScreenBoundary, SYM(objectCheckWithinScreenBoundary), b_+394);
  if (F & FC) { RET_TAKEN(b_+394); return; }
  CYC(b_+394, b_+395);
  CYC(b_+395, b_+398);
  TAIL(interactionDelete);
}

// interactionCode95@state1@subid0@func_5547
static void kingMoblin_state1_subid0_func_5547(GB *gb) {
  BASE(interactionCode95);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+294, b_+296); mem_wr(gb, hFF8B, A);
  CALL_C(b_+296, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+299);
  if (!(F & FZ)) { RET_TAKEN(b_+299); return; }
  CYC(b_+299, b_+300);
  CYC(b_+300, b_+302); mem_wr(gb, HL, 0x9f);
  CYC(b_+302, b_+304); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+304, b_+306); A = mem_rd(gb, hFF8B);
  CYC(b_+306, b_+307); mem_wr(gb, HL, A);
  CYC(b_+307, b_+310);
  TAIL(objectCopyPositionWithOffset);
}

// interactionCode95@state1@subid0@func_5517
static void kingMoblin_state1_subid0_func_5517(GB *gb) {
  BASE(interactionCode95);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+246, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+249);
  if (!(F & FZ)) { RET_TAKEN(b_+249); return; }
  CYC(b_+249, b_+250);
  CYC(b_+250, b_+252); mem_wr(gb, HL, 0x97);
  CYC(b_+252, b_+255); SET_BC(0x0c02);
  CALL_C(b_+255, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+258);
  CYC(b_+258, b_+260); E = 0x57;
  CYC(b_+260, b_+261); L = E;
  CYC(b_+261, b_+262); A = mem_rd(gb, DE);
  CYC(b_+262, b_+263); mem_wr(gb, HL, A);
  RET(b_+263); return;
}

// INTERAC_KING_MOBLIN
void s_interactionCode95_hook(GB *gb) {
  BASE(interactionCode95);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (kingMoblin_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+154) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+13, b_+14); A = mem_rd(gb, DE);
  CYC(b_+14, b_+15); push_effect(gb, b_+15);
  do { uint16_t jt_ = (kingMoblin_jump_table(gb));
    if (jt_ == b_+27) goto state0_subid0;
    if (jt_ == b_+40) goto state0_subid1;
    if (jt_ == b_+119) goto state0_subid3;
    if (jt_ == b_+113) goto state0_subid4;
    if (jt_ == b_+125) goto state0_subid5;
    HANDOFF(HL);
  } while (0);
state0_subid0:
  CYC(b_+27, b_+29); A = 0x16;
  CALL_C(b_+29, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+32);
  if (F & FZ) { CYCT(b_+32, b_+35); TAIL(interactionDelete); }
  CYC(b_+32, b_+35);
  CYC(b_+35, b_+37); A = 0x2d;
  CALL_C(b_+37, s_setGlobalFlag, SYM(setGlobalFlag), b_+40);
state0_subid1:
  CALL_C(b_+40, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+43);
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+45, b_+47); A = 0x80;
  CYC(b_+47, b_+48); mem_wr(gb, DE, A);
  CYC(b_+48, b_+50); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+52, b_+55); TAIL(objectSetVisible80); }
  CYC(b_+52, b_+55);
  CALL_C(b_+55, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+58);
  CYC(b_+58, b_+59); B = A;
  CYC(b_+59, b_+60); alu_xor(gb, A);
  CYC(b_+60, b_+62); B = alu_sla(gb, B);
  CYC(b_+62, b_+64); alu_adc(gb, 0x00);
  CYC(b_+64, b_+66); B = alu_sla(gb, B);
  CYC(b_+66, b_+68); alu_adc(gb, 0x00);
  CYC(b_+68, b_+71); SET_HL(SYM(table_55bf));
  CYC(b_+71, b_+72); kingMoblin_add_double_index(gb, b_+72);
  CYC(b_+72, b_+73); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+73, b_+74); H = mem_rd(gb, HL);
  CYC(b_+74, b_+75); L = A;
  CALL_C(b_+75, s_interactionSetScript, SYM(interactionSetScript), b_+78);
  CALL_C(b_+78, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+81);
  if (!(F & FZ)) { CYCT(b_+81, b_+83); goto L_547d; }
  CYC(b_+81, b_+83);
  CYC(b_+83, b_+85); mem_wr(gb, HL, 0x96);
  CYC(b_+85, b_+86); L = alu_inc8(gb, L);
  CYC(b_+86, b_+88); mem_wr(gb, HL, 0x01);
  CYC(b_+88, b_+90); E = 0x57;
  CYC(b_+90, b_+91); A = H;
  CYC(b_+91, b_+92); mem_wr(gb, DE, A);
L_547d:
  CALL_L(b_+92, kingMoblin_state1_subid0_func_5517, b_+95);
  CYC(b_+95, b_+98); SET_HL(SYM(objectData7ea0));
  CALL_C(b_+98, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+101);
  CALL_C(b_+101, s_objectSetVisible83, SYM(objectSetVisible83), b_+104);
  CYC(b_+104, b_+105); alu_xor(gb, A);
  CYC(b_+105, b_+108); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CYC(b_+108, b_+111); mem_wr(gb, wTmpcfc0 + 0x11, A);
  CYC(b_+111, b_+113);
  goto state1;
state0_subid4:
  CYC(b_+113, b_+116); SET_HL(SYM(script73cd));
  CALL_C(b_+116, s_interactionSetScript, SYM(interactionSetScript), b_+119);
state0_subid3:
  CALL_C(b_+119, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+122);
  CYC(b_+122, b_+125);
  TAIL(interactionAnimateAsNpc);
state0_subid5:
  CYC(b_+125, b_+128); SET_HL(SYM(script73d8));
  CALL_C(b_+128, s_interactionSetScript, SYM(interactionSetScript), b_+131);
  CYC(b_+131, b_+133); A = 0x01;
  CYC(b_+133, b_+136); mem_wr(gb, wLoadedTreeGfxIndex, A);
  CYC(b_+136, b_+138); A = 0x95;
  CYC(b_+138, b_+141); mem_wr(gb, wInteractionIDToLoadExtraGfx, A);
  CYC(b_+141, b_+143); A = 0x05;
  CYC(b_+143, b_+146); mem_wr(gb, wcc1e, A);
  CALL_C(b_+146, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+149);
  CALL_C(b_+149, s_objectSetVisible81, SYM(objectSetVisible81), b_+152);
  CYC(b_+152, b_+154);
  goto state1;
state1:
  CYC(b_+154, b_+156); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+156, b_+157); A = mem_rd(gb, DE);
  CYC(b_+157, b_+158); push_effect(gb, b_+158);
  do { uint16_t jt_ = (kingMoblin_jump_table(gb));
    if (jt_ == b_+170) goto state1_subid0;
    if (jt_ == b_+332) goto state1_subid1;
    if (jt_ == b_+335) goto state1_subid2;
    if (jt_ == b_+384) goto state1_subid3;
    if (jt_ == b_+359) goto state1_subid4;
    if (jt_ == b_+387) goto state1_subid5;
    HANDOFF(HL);
  } while (0);
state1_subid0:
  CYC(b_+170, b_+172); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+172, b_+173); A = mem_rd(gb, DE);
  CYC(b_+173, b_+174); push_effect(gb, b_+174);
  do { uint16_t jt_ = (kingMoblin_jump_table(gb));
    if (jt_ == b_+178) goto state1_subid0_substate0;
    if (jt_ == b_+264) goto state1_subid0_substate1;
    HANDOFF(HL);
  } while (0);
state1_subid0_substate0:
  CYC(b_+178, b_+181); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+181, b_+182); A = mem_rd(gb, HL);
  CYC(b_+182, b_+184); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+184, b_+186); goto state1_subid0_func_54ec; }
  CYC(b_+184, b_+186);
  CYC(b_+186, b_+187); H = D;
  CYC(b_+187, b_+189); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+189, b_+191); mem_wr(gb, HL, 0x01);
  CYC(b_+191, b_+193); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+193, b_+195); mem_wr(gb, HL, 0x00);
  CYC(b_+195, b_+197); A = 0x39;
  CALL_C(b_+197, s_playSound, SYM(playSound_b00), b_+200);
  CYC(b_+200, b_+203);
  TAIL(interactionAnimateAsNpc);
state1_subid0_func_54ec:
  CYC(b_+203, b_+204); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+204, b_+207); TAIL(interactionDelete); }
  CYC(b_+204, b_+207);
  CALL_C(b_+207, s_interactionRunScript, SYM(interactionRunScript), b_+210);
  CALL_C(b_+210, s_interactionAnimate, SYM(interactionAnimate), b_+213);
  CALL_C(b_+213, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+216);
  CYC(b_+216, b_+218); E = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+218, b_+219); A = mem_rd(gb, DE);
  CYC(b_+219, b_+220); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+220, b_+222); goto state1_subid0_func_550e; }
  CYC(b_+220, b_+222);
  CYC(b_+222, b_+224); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+224, b_+225); A = mem_rd(gb, DE);
  CYC(b_+225, b_+226); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+226); return; }
  CYC(b_+226, b_+227);
  CYC(b_+227, b_+230); SET_HL(wTmpcfc0);
  CYC(b_+230, b_+232); mem_wr(gb, HL, 0x01);
  CYC(b_+232, b_+233); H = D;
  CYC(b_+233, b_+235); L = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+235, b_+236); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+236); return;
state1_subid0_func_550e:
  CYC(b_+237, b_+239); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+239, b_+240); A = mem_rd(gb, DE);
  CYC(b_+240, b_+241); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+241); return; }
  CYC(b_+241, b_+242);
  CYC(b_+242, b_+244); E = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+244, b_+245); alu_xor(gb, A);
  CYC(b_+245, b_+246); mem_wr(gb, DE, A);
  CALL_C(b_+246, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+249);
  if (!(F & FZ)) { RET_TAKEN(b_+249); return; }
  CYC(b_+249, b_+250);
  CYC(b_+250, b_+252); mem_wr(gb, HL, 0x97);
  CYC(b_+252, b_+255); SET_BC(0x0c02);
  CALL_C(b_+255, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+258);
  CYC(b_+258, b_+260); E = 0x57;
  CYC(b_+260, b_+261); L = E;
  CYC(b_+261, b_+262); A = mem_rd(gb, DE);
  CYC(b_+262, b_+263); mem_wr(gb, HL, A);
  RET(b_+263); return;
state1_subid0_substate1:
  CYC(b_+264, b_+266); E = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+266, b_+267); A = mem_rd(gb, DE);
  CYC(b_+267, b_+268); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+268, b_+270); goto state1_subid0_func_5557; }
  CYC(b_+268, b_+270);
  CYC(b_+270, b_+272); A = 0x01;
  CYC(b_+272, b_+273); mem_wr(gb, DE, A);
  CYC(b_+273, b_+275); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+275, b_+276); A = mem_rd(gb, DE);
  CYC(b_+276, b_+278); alu_sub(gb, 0x20);
  CYC(b_+278, b_+279); B = A;
  CYC(b_+279, b_+281); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+281, b_+282); A = mem_rd(gb, DE);
  CYC(b_+282, b_+283); C = A;
  CYC(b_+283, b_+285); A = 0x50;
  CALL_L(b_+285, kingMoblin_state1_subid0_func_5547, b_+288);
  CYC(b_+288, b_+291); SET_HL(SYM(kingMoblinScript_trapLinkInBombedHouse));
  CYC(b_+291, b_+294);
  TAIL(interactionSetScript);

state1_subid0_func_5557:
  CALL_C(b_+310, s_interactionRunScript, SYM(interactionRunScript), b_+313);
  if (F & FC) { CYCT(b_+313, b_+316); TAIL(interactionDelete); }
  CYC(b_+313, b_+316);
  CALL_C(b_+316, s_objectSetPriorityRelativeToLink_withTerrainEffects, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+319);
  CYC(b_+319, b_+321); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+321, b_+322); A = mem_rd(gb, DE);
  CYC(b_+322, b_+323); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+323); return; }
  CYC(b_+323, b_+324);
  CALL_C(b_+324, s_interactionAnimate, SYM(interactionAnimate), b_+327);
  CALL_C(b_+327, s_interactionAnimate, SYM(interactionAnimate), b_+330);
  CYC(b_+330, b_+332);
  goto state1_func_557c;
state1_subid1:
  CYC(b_+332, b_+335);
  kingMoblin_state1_subid2(gb); return;
state1_subid2:
  CALL_C(b_+335, s_interactionAnimate, SYM(interactionAnimate), b_+338);
  CYC(b_+338, b_+341); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+341, b_+342); A = mem_rd(gb, HL);
  CYC(b_+342, b_+343); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+343); return; }
  CYC(b_+343, b_+344);
  CYC(b_+344, b_+347);
  TAIL(interactionDelete);
state1_func_557c:
  CYC(b_+347, b_+348); H = D;
  CYC(b_+348, b_+350); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+350, b_+351); A = mem_rd(gb, HL);
  CYC(b_+351, b_+353); alu_cp(gb, 0x70);
  if (!(F & FZ)) { RET_TAKEN(b_+353); return; }
  CYC(b_+353, b_+354);
  CYC(b_+354, b_+356); mem_wr(gb, HL, 0x00);
  CYC(b_+356, b_+359);
  TAIL(playSound_b00);
state1_subid4:
  CALL_C(b_+359, s_interactionRunScript, SYM(interactionRunScript), b_+362);
  if (F & FC) { CYCT(b_+362, b_+365); TAIL(interactionDelete); }
  CYC(b_+362, b_+365);
  CYC(b_+365, b_+368); SET_HL(wTmpcfc0);
  CYC(b_+368, b_+370); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+370, b_+372); goto state1_subid3; }
  CYC(b_+370, b_+372);
  CYC(b_+372, b_+375); A = mem_rd(gb, wFrameCounter);
  CYC(b_+375, b_+377); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+377, b_+379); goto state1_subid3; }
  CYC(b_+377, b_+379);
  CYC(b_+379, b_+381); A = 0x70;
  CALL_C(b_+381, s_playSound, SYM(playSound_b00), b_+384);
state1_subid3:
  CYC(b_+384, b_+387);
  TAIL(interactionAnimateAsNpc);
state1_subid5:
  CALL_C(b_+387, s_interactionRunScript, SYM(interactionRunScript), b_+390);
  if (F & FC) { CYCT(b_+390, b_+393); TAIL(interactionDelete); }
  CYC(b_+390, b_+393);
  CYC(b_+393, b_+395); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+395, b_+396); A = mem_rd(gb, DE);
  CYC(b_+396, b_+397); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+397, b_+399); goto L_55bc; }
  CYC(b_+397, b_+399);
  CYC(b_+399, b_+402); A = mem_rd(gb, wFrameCounter);
  CYC(b_+402, b_+404); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+404, b_+406); goto L_55bc; }
  CYC(b_+404, b_+406);
  CYC(b_+406, b_+408); A = 0x70;
  CALL_C(b_+408, s_playSound, SYM(playSound_b00), b_+411);
L_55bc:
  CYC(b_+411, b_+414);
  TAIL(interactionAnimate);
}
