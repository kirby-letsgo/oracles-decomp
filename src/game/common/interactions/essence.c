#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode7f_jump_table(GB *gb) {
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

static void interaction7f_subid00_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void interaction7f_subid00_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
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

void interaction7f_subid01_hook(GB *gb) {
  BASE(interaction7f_subid01);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+6); objectPreventLinkFromPassing_hook(gb); return; } // jp nz
  CYC(b_+3, b_+6);
  CYC(b_+6, b_+8); A = 0x01;
  CYC(b_+8, b_+9); mem_wr(gb, DE, A);
  CYC(b_+9, b_+12); SET_BC(0x060a);
  CALL_C(b_+12, objectSetCollideRadii_hook, SYM(objectSetCollideRadii), b_+15);
  CALL_C(b_+15, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+18);
  CYC(b_+18, b_+19); H = alu_dec8(gb, H);
  CYC(b_+19, b_+21); mem_wr(gb, HL, 0x0f);
  CALL_C(b_+21, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+24);
  CYC(b_+24, b_+27); TAIL(objectSetVisible83); // jp
}

// interaction7f_subid02@copyEssencePosition: reached by one genuine call, from @state1's
// return address 0x479b below; never separately hooked.
void interaction7f_subid02_hook(GB *gb) {
  BASE(interaction7f_subid02);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } // jr nz
  CYC(b_+3, b_+5);

  // interaction7f_subid02@state0
  CYC(b_+5, b_+7); A = 0x01;
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+14); TAIL(objectSetVisible82); // jp

state1:
  CYC(b_+14, b_+17); push_effect(gb, b_+17); goto copyEssencePosition;
afterCopy:
  CALL_C(b_+17, interactionAnimate_hook, SYM(interactionAnimate), b_+20);
  CYC(b_+20, b_+21); H = D;
  CYC(b_+21, b_+23); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+25); return; } // ret z
  CYC(b_+25, b_+26);
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x00);
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+30, b_+32); A = 0x80;
  CYC(b_+32, b_+33); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  RET(b_+34); return; // ret

copyEssencePosition:
  CYC(b_+35, b_+37); A = 0x00; // Object.enabled
  CALL_C(b_+37, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+40);
  CYC(b_+40, b_+43); objectTakePosition_hook(gb);
  if (gb->pc == b_+17 && gb->sp == sp0_) goto afterCopy;
  return;
}

void interaction7f_subid00_hook(GB *gb) {
  BASE(interaction7f_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode7f_jump_table(gb);
    if (target == b_+20) goto state0;
    if (target == b_+140) goto state1;
    if (target == b_+247) goto state2;
    if (target == b_+268) goto state3;
    if (target == b_+287) goto state4;
    if (target == b_+356) goto state5;
    if (target == b_+368) goto state6;
    goto state7;
  }

state0:
  CYC(b_+20, b_+22); A = 0x01;
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CALL_C(b_+23, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+26);
  CYC(b_+26, b_+28); A = 0x04;
  CALL_C(b_+28, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+31);
  CYC(b_+31, b_+34); SET_BC((SYM(interaction97_subid01__positions) + 298)); // INTERAC_ESSENCE, $01
  CALL_C(b_+34, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+37);
  CALL_C(b_+37, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+40);
  CYC(b_+40, b_+42); alu_and(gb, 0x20);
  if (!(F & FZ)) { CYCT(b_+42, b_+45); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+42, b_+45);
  CYC(b_+45, b_+48); SET_HL(w1ReservedInteraction1);
  CYC(b_+48, b_+50); B = 0x40;
  CALL_C(b_+50, clearMemory_hook, SYM(clearMemory), b_+53);
  CYC(b_+53, b_+56); SET_HL(w1ReservedInteraction1);
  CYC(b_+56, b_+58); mem_wr(gb, HL, 0x81);
  CYC(b_+58, b_+59); L = alu_inc8(gb, L);
  CYC(b_+59, b_+61); mem_wr(gb, HL, 0x7f); // INTERAC_ESSENCE
  CYC(b_+61, b_+62); L = alu_inc8(gb, L);
  CYC(b_+62, b_+64); mem_wr(gb, HL, 0x02);
  CALL_C(b_+64, objectCopyPosition_hook, SYM(objectCopyPosition), b_+67);
  CYC(b_+67, b_+69); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+69, b_+71); A = H8(hActiveObjectType);
  CYC(b_+71, b_+72); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+72, b_+74); A = H8(hActiveObject);
  CYC(b_+74, b_+75); mem_wr(gb, HL, A);
  CYC(b_+75, b_+76); H = D;
  CYC(b_+76, b_+78); L = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+78, b_+80); mem_wr(gb, HL, (uint8_t)-0x10);
  CYC(b_+80, b_+83); A = W8(wDungeonIndex);
  CYC(b_+83, b_+84); A = alu_dec8(gb, A);
  CYC(b_+84, b_+86); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+86, b_+88); goto afterDungeonOverride; } // jr nz
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+90); A = 0x05;

afterDungeonOverride:
  CYC(b_+90, b_+92); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+92, b_+93); mem_wr(gb, HL, A);
  CYC(b_+93, b_+94); B = A;
  CYC(b_+94, b_+95); alu_add(gb, A);
  CYC(b_+95, b_+96); alu_add(gb, B);
  CYC(b_+96, b_+99); SET_HL(b_+116); // interaction7f_subid00@essenceOamData
  CYC(b_+99, b_+100); interaction7f_subid00_addAToHl_from_rst(gb, b_+100);
  CYC(b_+100, b_+102); E = INTERACTION_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+102, b_+103); A = mem_rd(gb, DE);
  CYC(b_+103, b_+104); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+104, b_+105); SET_HL(HL + 1); // inc hl
  CYC(b_+105, b_+106); mem_wr(gb, DE, A);
  CYC(b_+106, b_+107); E = alu_dec8(gb, E);
  CYC(b_+107, b_+108); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+108, b_+109); mem_wr(gb, DE, A);
  CYC(b_+109, b_+110); A = mem_rd(gb, HL);
  CALL_C(b_+110, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+113);
  CYC(b_+113, b_+116); TAIL(objectSetVisible81); // jp

state1:
  CYC(b_+140, b_+143); A = W8(wFrameCounter);
  CYC(b_+143, b_+145); alu_and(gb, 0x03);
  if (!(F & FZ)) { RET_TAKEN(b_+145); return; } // ret nz
  CYC(b_+145, b_+146);
  CYC(b_+146, b_+147); H = D;
  CYC(b_+147, b_+149); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+149, b_+150); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+150, b_+151); A = mem_rd(gb, HL);
  CYC(b_+151, b_+153); alu_and(gb, 0x0f);
  CYC(b_+153, b_+156); SET_HL(b_+231); // interaction7f_subid00@essenceFloatOffsets
  CYC(b_+156, b_+157); interaction7f_subid00_addAToHl_from_rst(gb, b_+157);
  CYC(b_+157, b_+158); A = mem_rd(gb, HL);
  CYC(b_+158, b_+160); alu_add(gb, 0xf0);
  CYC(b_+160, b_+162); E = INTERACTION_BASE + OBJ_ZH;
  CYC(b_+162, b_+163); mem_wr(gb, DE, A);
  CYC(b_+163, b_+166); A = W8(wLinkInAir);
  CYC(b_+166, b_+167); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+167); return; } // ret nz
  CYC(b_+167, b_+168);
  CYC(b_+168, b_+171); A = W8(wLinkGrabState);
  CYC(b_+171, b_+172); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+172); return; } // ret nz
  CYC(b_+172, b_+173);
  CYC(b_+173, b_+175); B = 0x04;
  CALL_C(b_+175, objectCheckCenteredWithLink_hook, SYM(objectCheckCenteredWithLink), b_+178);
  if (!(F & FC)) { RET_TAKEN(b_+178); return; } // ret nc
  CYC(b_+178, b_+179);
  CYC(b_+179, b_+181); C = 0x14;
  CALL_C(b_+181, objectCheckLinkWithinDistance_hook, SYM(objectCheckLinkWithinDistance), b_+184);
  if (!(F & FC)) { RET_TAKEN(b_+184); return; } // ret nc
  CYC(b_+184, b_+185);
  CYC(b_+185, b_+187); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+187); return; } // ret nz
  CYC(b_+187, b_+188);
  CALL_C(b_+188, clearAllParentItems_hook, SYM(clearAllParentItems), b_+191);
  CYC(b_+191, b_+193); A = 0x81;
  CYC(b_+193, b_+196); W8(wDisabledObjects) = A;
  CYC(b_+196, b_+199); W8(wDisableLinkCollisionsAndMenu) = A;
  CYC(b_+199, b_+202); SET_HL(w1Link_direction);
  CYC(b_+202, b_+204); mem_wr(gb, HL, 0x00); // DIR_UP
  CALL_C(b_+204, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+207);
  CYC(b_+207, b_+208); H = D;
  CYC(b_+208, b_+210); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+210, b_+211); mem_wr(gb, HL, A);
  CYC(b_+211, b_+213); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+213, b_+215); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+215, b_+217); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+217, b_+218); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+218, darkenRoom_hook, SYM(darkenRoom), b_+221);
  CYC(b_+221, b_+223); A = 0x77; // SND_DROPESSENCE
  CALL_C(b_+223, playSound_b00_hook, SYM(playSound_b00), b_+226);
  CYC(b_+226, b_+228); A = 0xfc; // SNDCTRL_SLOW_FADEOUT
  CYC(b_+228, b_+231); TAIL(playSound_b00); // jp

state2:
  CALL_C(b_+247, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+250);
  CYC(b_+250, b_+252); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+252, b_+253); mem_wr(gb, DE, A);
  CALL_C(b_+253, objectApplySpeed_hook, SYM(objectApplySpeed), b_+256);
  CALL_C(b_+256, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+259);
  if (!(F & FC)) { RET_TAKEN(b_+259); return; } // ret nc
  CYC(b_+259, b_+260);
  CYC(b_+260, b_+262); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_X;
  CYC(b_+262, b_+264); A = 0x06;
  CYC(b_+264, b_+265); mem_wr(gb, DE, A);
  CYC(b_+265, b_+268); TAIL(interactionIncState); // jp

state3:
  CYC(b_+268, b_+270); C = 0x08;
  CALL_C(b_+270, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+273);
  if (F & FZ) { CYCT(b_+273, b_+275); goto afterSpeedZCheck; } // jr z
  CYC(b_+273, b_+275);
  CALL_C(b_+275, objectCheckCollidedWithLink_notDeadAndNotGrabbing_hook, SYM(objectCheckCollidedWithLink_notDeadAndNotGrabbing), b_+278);
  if (!(F & FC)) { RET_TAKEN(b_+278); return; } // ret nc
  CYC(b_+278, b_+279);

afterSpeedZCheck:
  CYC(b_+279, b_+280); H = D;
  CYC(b_+280, b_+282); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+282, b_+284); mem_wr(gb, HL, 30);
  CYC(b_+284, b_+287); TAIL(interactionIncState); // jp

state4:
  CALL_C(b_+287, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+290);
  if (!(F & FZ)) { RET_TAKEN(b_+290); return; } // ret nz
  CYC(b_+290, b_+291);
  CYC(b_+291, b_+293); A = 0x04; // LINK_STATE_04
  CYC(b_+293, b_+296); W8(wLinkForceState) = A;
  CYC(b_+296, b_+298); A = 0x01;
  CYC(b_+298, b_+301); W8(wcc50) = A;
  CALL_C(b_+301, interactionIncState_hook, SYM(interactionIncState), b_+304);
  CYC(b_+304, b_+307); A = W8(w1Link_yh);
  CYC(b_+307, b_+309); alu_sub(gb, 0x0e);
  CYC(b_+309, b_+311); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+311, b_+312); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+312, b_+313); L = alu_inc8(gb, L);
  CYC(b_+313, b_+316); A = W8(w1Link_xh);
  CYC(b_+316, b_+317); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+317, b_+318); L = alu_inc8(gb, L);
  CYC(b_+318, b_+319); alu_xor(gb, A);
  CYC(b_+319, b_+320); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+320, b_+321); mem_wr(gb, HL, A);
  CYC(b_+321, b_+323); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+323, b_+324); A = mem_rd(gb, HL);
  CYC(b_+324, b_+327); SET_HL(b_+348); // interaction7f_subid00@getEssenceTextTable
  CYC(b_+327, b_+328); interaction7f_subid00_addAToHl_from_rst(gb, b_+328);
  CYC(b_+328, b_+330); B = 0x00; // >TX_0000
  CYC(b_+330, b_+331); C = mem_rd(gb, HL);
  CALL_C(b_+331, showText_hook, SYM(showText), b_+334);
  CALL_C(b_+334, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+337);
  CYC(b_+337, b_+339); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 5)); // ROOMFLAG_BIT_ITEM
  CYC(b_+339, b_+341); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+341, b_+342); A = mem_rd(gb, DE);
  CYC(b_+342, b_+343); C = A;
  CYC(b_+343, b_+345); A = 0x40; // TREASURE_ESSENCE
  CYC(b_+345, b_+348); TAIL(giveTreasure); // jp

state5:
  CALL_C(b_+356, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+359);
  CALL_C(b_+359, interactionIncState_hook, SYM(interactionIncState), b_+362);
  CYC(b_+362, b_+365); SET_HL((SYM(interactionCode91__subid00__state1) + 31)); // mainScripts.essenceScript_essenceGetCutscene
  CYC(b_+365, b_+368); TAIL(interactionSetScript); // jp

state6:
  CALL_C(b_+368, interactionRunScript_hook, SYM(interactionRunScript), b_+371);
  if (!(F & FC)) { RET_TAKEN(b_+371); return; } // ret nc
  CYC(b_+371, b_+372);
  CALL_C(b_+372, interactionIncState_hook, SYM(interactionIncState), b_+375);
  CYC(b_+375, b_+377); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+377, b_+379); mem_wr(gb, HL, 30);

state7:
  CALL_C(b_+379, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+382);
  if (!(F & FZ)) { RET_TAKEN(b_+382); return; } // ret nz
  CYC(b_+382, b_+383);
  CYC(b_+383, b_+385); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+385, b_+386); A = mem_rd(gb, HL);
  CYC(b_+386, b_+387); alu_add(gb, A);
  CYC(b_+387, b_+390); SET_HL(b_+419); // interaction7f_subid00@essenceWarps
  CYC(b_+390, b_+391); interaction7f_subid00_addDoubleIndexToHl_from_rst(gb, b_+391);
  CYC(b_+391, b_+392); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+392, b_+395); W8(wWarpDestGroup) = A;
  CYC(b_+395, b_+396); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+396, b_+399); W8(wWarpDestRoom) = A;
  CYC(b_+399, b_+400); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+400, b_+403); W8(wWarpDestPos) = A;
  CYC(b_+403, b_+404); A = mem_rd(gb, HL);
  CYC(b_+404, b_+407); W8(wWarpTransition) = A;
  CYC(b_+407, b_+409); A = 0x83;
  CYC(b_+409, b_+412); W8(wWarpTransition2) = A;
  CYC(b_+412, b_+413); alu_xor(gb, A);
  CYC(b_+413, b_+416); W8(wActiveMusic) = A;
  CYC(b_+416, b_+419); TAIL(clearStaticObjects); // jp
}

// INTERAC_ESSENCE
void interactionCode7f_hook(GB *gb) {
  BASE(interactionCode7f);
  CYC(b_+0, b_+3); A = W8(wLinkDeathTrigger);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  {
    CYC(b_+8, b_+9); push_effect(gb, b_+9);
    uint16_t target = interactionCode7f_jump_table(gb);
    if (target == SYM(interaction7f_subid00)) { interaction7f_subid00_hook(gb); return; }
    if (target == SYM(interaction7f_subid01)) { interaction7f_subid01_hook(gb); return; }
    TAIL(interaction7f_subid02);
  }
}
