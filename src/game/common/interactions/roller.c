#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCode7a_jump_table(GB *gb) {
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

// INTERAC_ROLLER
void interactionCode7a_hook(GB *gb) {
  BASE(interactionCode7a);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  {
    CYC(b_+6, b_+7); push_effect(gb, b_+7);
    uint16_t target = interactionCode7a_jump_table(gb);
    if (target == b_+13) goto state0;
    if (target == b_+55) goto state1;
    goto state2;
  }

state0:
  CYC(b_+13, b_+15); A = 0x01;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
  CALL_C(b_+16, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+19);
  CYC(b_+19, b_+20); H = D;
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+22, b_+23); A = mem_rd(gb, HL);
  CYC(b_+23, b_+25); alu_add(gb, 0x02);
  CYC(b_+25, b_+27); A = alu_swap(gb, A);
  CYC(b_+27, b_+28); alu_rrca(gb);
  CYC(b_+28, b_+30); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+31, b_+33); A = 0x06;
  CYC(b_+33, b_+34); mem_wr(gb, HL, A);
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 30);
  CYC(b_+42, b_+44); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+44, b_+46); mem_wr(gb, HL, 60);
  CYC(b_+46, b_+48); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+51); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+51, b_+52); mem_wr(gb, HL, A);
  CALL_C(b_+52, objectSetVisible83_hook, SYM(objectSetVisible83), b_+55);

state1:
  CYC(b_+55, b_+58); push_effect(gb, b_+58); goto preventLinkFromPassing;
afterPrevent1:
  if (F & FC) { CYCT(b_+58, b_+60); goto movingTowardRoller; } // jr c
  CYC(b_+58, b_+60);

notPushingAgainstRoller:
  CYC(b_+60, b_+61); H = D;
  CYC(b_+61, b_+63); L = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(b_+63, b_+65); mem_wr(gb, HL, 30);

moveTowardOriginalPosition:
  CYC(b_+65, b_+66); H = D;
  CYC(b_+66, b_+68); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 30);
  CYC(b_+70, b_+72); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+72, b_+73); B = mem_rd(gb, HL);
  CYC(b_+73, b_+75); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(b_+75, b_+76); A = mem_rd(gb, HL);
  CYC(b_+76, b_+77); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+77); return; } // ret z
  CYC(b_+77, b_+78);
  CYC(b_+78, b_+80); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+80, b_+81); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+81); return; } // ret nz
  CYC(b_+81, b_+82);
  CYC(b_+82, b_+83); alu_cp(gb, B);
  CYC(b_+83, b_+86); SET_BC(0x0008);
  if (!(F & FC)) { CYCT(b_+86, b_+88); goto moveRollerInDirection; } // jr nc
  CYC(b_+86, b_+88);
  CYC(b_+88, b_+91); SET_BC(0x0118);
  CYC(b_+91, b_+93); goto moveRollerInDirection; // jr

movingTowardRoller:
  CYC(b_+93, b_+94); H = D;
  CYC(b_+94, b_+96); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+96, b_+97); A = mem_rd(gb, HL);
  CYC(b_+97, b_+99); alu_sub(gb, 0x02);
  CYC(b_+99, b_+100); B = A;
  CYC(b_+100, b_+101); C = B;
  CYC(b_+101, b_+103); C = alu_sla(gb, C);
  CYC(b_+103, b_+104); C = alu_inc8(gb, C);
  CYC(b_+104, b_+106); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+106, b_+109); A = W8(w1Link_yh);
  CYC(b_+109, b_+110); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+110, b_+111); alu_add(gb, B);
  CYC(b_+111, b_+112); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(b_+112, b_+114); goto notPushingAgainstRoller; } // jr nc
  CYC(b_+112, b_+114);
  CYC(b_+114, b_+117); A = W8(wLinkAngle);
  CYC(b_+117, b_+119); alu_cp(gb, 0x08);
  CYC(b_+119, b_+122); SET_BC(0x0008);
  if (F & FZ) { CYCT(b_+122, b_+131); goto afterAngleCheck; } // jr z
  CYC(b_+122, b_+124);
  CYC(b_+124, b_+126); alu_cp(gb, 0x18);
  CYC(b_+126, b_+129); SET_BC(0x0118);
  if (!(F & FZ)) { CYCT(b_+129, b_+131); goto notPushingAgainstRoller; } // jr nz
  CYC(b_+129, b_+131);

afterAngleCheck:
  CYC(b_+131, b_+133); A = 0x01;
  CYC(b_+133, b_+136); W8(wForceLinkPushAnimation) = A;
  CYC(b_+136, b_+139); A = W8(wBraceletGrabbingNothing);
  CYC(b_+139, b_+141); alu_and(gb, 0x03);
  CYC(b_+141, b_+143); A = alu_swap(gb, A);
  CYC(b_+143, b_+144); alu_rrca(gb);
  CYC(b_+144, b_+145); alu_cp(gb, C);
  if (F & FZ) { CYCT(b_+145, b_+147); goto pushingAgainstRoller; } // jr z
  CYC(b_+145, b_+147);
  CYC(b_+147, b_+150); SET_HL(wInventoryB);
  CYC(b_+150, b_+152); A = 0x14; // ITEM_BRACELET
  CYC(b_+152, b_+153); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+153, b_+155); goto notPushingAgainstRoller; } // jr z
  CYC(b_+153, b_+155);
  CYC(b_+155, b_+156); SET_HL(HL + 1);
  CYC(b_+156, b_+157); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+157, b_+159); goto notPushingAgainstRoller; } // jr z
  CYC(b_+157, b_+159);
  CYC(b_+159, b_+162); A = W8(wBraceletGrabbingNothing);
  CYC(b_+162, b_+163); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+163, b_+165); goto notPushingAgainstRoller; } // jr nz
  CYC(b_+163, b_+165);
  CYC(b_+165, b_+168); A = W8(wLinkInAir);
  CYC(b_+168, b_+169); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+169, b_+171); goto notPushingAgainstRoller; } // jr nz
  CYC(b_+169, b_+171);
  CYC(b_+171, b_+172); H = D;
  CYC(b_+172, b_+174); L = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(b_+174, b_+175); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+175, b_+177); goto moveTowardOriginalPosition; } // jr nz
  CYC(b_+175, b_+177);
  CALL_C(b_+177, showInfoTextForRoller_hook, SYM(showInfoTextForRoller), b_+180);
  CYC(b_+180, b_+182); goto notPushingAgainstRoller; // jr

pushingAgainstRoller:
  CYC(b_+182, b_+184); A = 60;
  CYC(b_+184, b_+186); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(b_+186, b_+187); mem_wr(gb, DE, A);
  CYC(b_+187, b_+190); push_effect(gb, b_+190); goto checkRollerCanBePushed;
afterCheckPush:
  if (!(F & FZ)) { CYCT(b_+190, b_+193); goto notPushingAgainstRoller; } // jp nz
  CYC(b_+190, b_+193);
  CALL_C(b_+193, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+196);
  if (!(F & FZ)) { RET_TAKEN(b_+196); return; } // ret nz
  CYC(b_+196, b_+197);

moveRollerInDirection:
  CYC(b_+197, b_+199); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+199, b_+200); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+200, b_+202); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+202, b_+203); mem_wr(gb, HL, C);
  CYC(b_+203, b_+205); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+205, b_+206); A = mem_rd(gb, HL);
  CYC(b_+206, b_+207); alu_add(gb, A);
  CYC(b_+207, b_+208); alu_add(gb, B);
  CALL_C(b_+208, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+211);
  CYC(b_+211, b_+214); SET_HL(wInformativeTextsShown);
  CYC(b_+214, b_+216); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 6));

state2:
  CALL_C(b_+216, objectApplySpeed_hook, SYM(objectApplySpeed), b_+219);
  CALL_C(b_+219, interactionAnimate_hook, SYM(interactionAnimate), b_+222);
  CALL_C(b_+222, objectCheckCollidedWithLink_ignoreZ_hook, SYM(objectCheckCollidedWithLink_ignoreZ), b_+225);
  if (!(F & FC)) { CYCT(b_+225, b_+230); goto afterCollisionCheck; } // jr nc
  CYC(b_+225, b_+227);
  CYC(b_+227, b_+230); push_effect(gb, b_+230); goto updateLinkPositionWhileRollerMoving;

afterCollisionCheck:
  CYC(b_+230, b_+231); H = D;
  CYC(b_+231, b_+233); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+233, b_+234); A = mem_rd(gb, HL);
  CYC(b_+234, b_+235); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+235, b_+237); goto rollerSound; } // jr z
  CYC(b_+235, b_+237);
  CYC(b_+237, b_+238); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+238); return; } // ret nz
  CYC(b_+238, b_+239);
  CYC(b_+239, b_+241); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+241, b_+243); mem_wr(gb, HL, 30);
  CYC(b_+243, b_+244); L = alu_inc8(gb, L);
  CYC(b_+244, b_+246); mem_wr(gb, HL, 60);
  CYC(b_+246, b_+248); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+248, b_+250); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+250); return; // ret

rollerSound:
  CYC(b_+250, b_+252); mem_wr(gb, HL, 0x01);
  CYC(b_+252, b_+254); A = 0x82; // SND_ROLLER
  CYC(b_+254, b_+257); playSound_b00_hook(gb); return; // jp

// interactionCode7a@updateLinkPositionWhileRollerMoving / @squashLink: reached only by
// one genuine call, from @state2's return address 0x41fb above; never separately hooked.
updateLinkPositionWhileRollerMoving:
  CYC(b_+257, b_+260); A = W8(w1Link_adjacentWallsBitset);
  CYC(b_+260, b_+262); alu_cp(gb, 0x53);
  if (F & FZ) { CYCT(b_+262, b_+264); goto squashLink; } // jr z
  CYC(b_+262, b_+264);
  CYC(b_+264, b_+266); alu_cp(gb, 0xac);
  if (F & FZ) { CYCT(b_+266, b_+268); goto squashLink; } // jr z
  CYC(b_+266, b_+268);
  CYC(b_+268, b_+270); alu_cp(gb, 0x33);
  if (F & FZ) { CYCT(b_+270, b_+272); goto squashLink; } // jr z
  CYC(b_+270, b_+272);
  CYC(b_+272, b_+274); alu_cp(gb, 0xc3);
  if (F & FZ) { CYCT(b_+274, b_+276); goto squashLink; } // jr z
  CYC(b_+274, b_+276);
  CYC(b_+276, b_+278); alu_cp(gb, 0xcc);
  if (F & FZ) { CYCT(b_+278, b_+280); goto squashLink; } // jr z
  CYC(b_+278, b_+280);
  CYC(b_+280, b_+282); alu_cp(gb, 0x3c);
  if (F & FZ) { CYCT(b_+282, b_+284); goto squashLink; } // jr z
  CYC(b_+282, b_+284);
  CYC(b_+284, b_+287); push_effect(gb, b_+287); goto preventLinkFromPassing;
afterPrevent2:
  CYC(b_+287, b_+290); A = W8(w1Link_adjacentWallsBitset);
  CYC(b_+290, b_+292); alu_and(gb, 0x0f);
  if (F & FZ) {
    RET_TAKEN(b_+292);
    if (gb->pc == b_+230 && gb->sp == sp0_) goto afterCollisionCheck;
    return;
  } // ret z
  CYC(b_+292, b_+293);
  CALL_C(b_+293, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+296);
  CYC(b_+296, b_+298); alu_cp(gb, 0x10);
  CYC(b_+298, b_+300); C = 0x08;
  if (F & FC) { CYCT(b_+300, b_+304); goto afterCTable; } // jr c
  CYC(b_+300, b_+302);
  CYC(b_+302, b_+304); C = 0x18;

afterCTable:
  CYC(b_+304, b_+306); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+306, b_+307); A = mem_rd(gb, DE);
  CYC(b_+307, b_+308); alu_cp(gb, C);
  if (!(F & FZ)) {
    RET_TAKEN(b_+308);
    if (gb->pc == b_+230 && gb->sp == sp0_) goto afterCollisionCheck;
    return;
  } // ret nz
  CYC(b_+308, b_+309);
  CYC(b_+309, b_+311); B = 0x28; // SPEED_100
  CYC(b_+311, b_+314); updateLinkPositionGivenVelocity_hook(gb);
  if (gb->pc == b_+230 && gb->sp == sp0_) goto afterCollisionCheck;
  return; // jp

squashLink:
  CYC(b_+314, b_+317); A = W8(w1Link_state);
  CYC(b_+317, b_+319); alu_cp(gb, 0x01); // LINK_STATE_NORMAL
  if (!(F & FZ)) {
    RET_TAKEN(b_+319);
    if (gb->pc == b_+230 && gb->sp == sp0_) goto afterCollisionCheck;
    return;
  } // ret nz
  CYC(b_+319, b_+320);
  CYC(b_+320, b_+322); A = 0x11; // LINK_STATE_SQUISHED
  CYC(b_+322, b_+325); W8(wLinkForceState) = A;
  CYC(b_+325, b_+326); alu_xor(gb, A);
  CYC(b_+326, b_+329); W8(wcc50) = A;
  RET(b_+329);
  if (gb->pc == b_+230 && gb->sp == sp0_) goto afterCollisionCheck;
  return; // ret

// interactionCode7a@checkRollerCanBePushed / @nextTile: zflag set if nothing blocks the
// roller in direction c. Reached only by one genuine call, from @pushingAgainstRoller's
// return address 0x41d3 above; never separately hooked.
checkRollerCanBePushed:
  CYC(b_+330, b_+331); push_effect(gb, BC);
  CYC(b_+331, b_+333); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+333, b_+334); A = mem_rd(gb, DE);
  CYC(b_+334, b_+336); alu_add(gb, 0x02);
  CYC(b_+336, b_+338); H8(hFF8B) = A;
  CYC(b_+338, b_+340); A = alu_swap(gb, A);
  CYC(b_+340, b_+341); alu_rrca(gb);
  CYC(b_+341, b_+342); B = A;
  CYC(b_+342, b_+344); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+344, b_+345); A = mem_rd(gb, DE);
  CYC(b_+345, b_+346); alu_sub(gb, B);
  CYC(b_+346, b_+348); alu_add(gb, 0x08);
  CYC(b_+348, b_+350); alu_and(gb, 0xf0);
  CYC(b_+350, b_+351); B = A;
  CYC(b_+351, b_+353); A = 0x08;
  CYC(b_+353, b_+355); L = 0x01;
  CYC(b_+355, b_+356); alu_cp(gb, C);
  if (F & FZ) { CYCT(b_+356, b_+358); goto afterSign; } // jr z
  CYC(b_+356, b_+358);
  CYC(b_+358, b_+360); L = 0xff;

afterSign:
  CYC(b_+360, b_+362); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+362, b_+363); A = mem_rd(gb, DE);
  CYC(b_+363, b_+365); A = alu_swap(gb, A);
  CYC(b_+365, b_+366); alu_add(gb, L);
  CYC(b_+366, b_+368); alu_and(gb, 0x0f);
  CYC(b_+368, b_+369); alu_or(gb, B);
  CYC(b_+369, b_+370); SET_BC(pop_effect(gb));
  CYC(b_+370, b_+371); L = A;
  CYC(b_+371, b_+373); H = 0xce; // >wRoomCollisions
  CYC(b_+373, b_+375); A = H8(hFF8B);
  CYC(b_+375, b_+376); E = A;

nextTile:
  CYC(b_+376, b_+377); A = mem_rd(gb, HL);
  CYC(b_+377, b_+379); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+379, b_+381); goto loopIncrement; } // jr nc
  CYC(b_+379, b_+381);
  CYC(b_+381, b_+382); alu_or(gb, A);
  if (!(F & FZ)) {
    RET_TAKEN(b_+382);
    if (gb->pc == b_+190 && gb->sp == sp0_) goto afterCheckPush;
    return;
  } // ret nz
  CYC(b_+382, b_+383);

loopIncrement:
  CYC(b_+383, b_+384); A = L;
  CYC(b_+384, b_+386); alu_add(gb, 0x10);
  CYC(b_+386, b_+387); L = A;
  CYC(b_+387, b_+388); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(b_+388, b_+390); goto nextTile; } // jr nz
  CYC(b_+388, b_+390);
  CYC(b_+390, b_+391); alu_xor(gb, A);
  RET(b_+391);
  if (gb->pc == b_+190 && gb->sp == sp0_) goto afterCheckPush;
  return;

// interactionCode7a@preventLinkFromPassing: sets carry via objectPreventLinkFromPassing if
// Link is pushing against the roller. Reached by two genuine calls: @state1's return address
// 0x414f above, pushed at sp0_ since @state1 itself has no outer push; and
// @updateLinkPositionWhileRollerMoving's return address 0x4234 above, pushed at sp0_-2 since
// that caller was itself entered via its own push_effect from @state2 -- the sp comparison
// must match each edge's own push depth, not sp0_ uniformly. Has both literal ret exits and a
// tail-jump-into-external-hook exit, so every exit needs the same two-way resume check.
preventLinkFromPassing:
  CYC(b_+392, b_+395); A = W8(w1Link_collisionType);
  CYC(b_+395, b_+397); alu_bit(gb, 7, A);
  if (F & FZ) {
    RET_TAKEN(b_+397);
    if (gb->pc == b_+58 && gb->sp == sp0_) goto afterPrevent1;
    if (gb->pc == b_+287 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterPrevent2;
    return;
  }
  CYC(b_+397, b_+398);
  CYC(b_+398, b_+401); A = W8(w1Link_state);
  CYC(b_+401, b_+403); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    RET_TAKEN(b_+403);
    if (gb->pc == b_+58 && gb->sp == sp0_) goto afterPrevent1;
    if (gb->pc == b_+287 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterPrevent2;
    return;
  }
  CYC(b_+403, b_+404);
  CYC(b_+404, b_+407); objectPreventLinkFromPassing_hook(gb); // jp
  if (gb->pc == b_+58 && gb->sp == sp0_) goto afterPrevent1;
  if (gb->pc == b_+287 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterPrevent2;
  return;
}
