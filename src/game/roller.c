#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4115, retIfTextIsActive_hook, 0x1859, 0x4118);
  CYC(0x4118, 0x411a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x411a, 0x411b); A = mem_rd(gb, DE);
  {
    CYC(0x411b, 0x411c); push_effect(gb, 0x411c);
    uint16_t target = interactionCode7a_jump_table(gb);
    if (target == 0x4122) goto state0;
    if (target == 0x414c) goto state1;
    goto state2;
  }

state0:
  CYC(0x4122, 0x4124); A = 0x01;
  CYC(0x4124, 0x4125); mem_wr(gb, DE, A);
  CALL_C(0x4125, interactionInitGraphics_hook, 0x15fb, 0x4128);
  CYC(0x4128, 0x4129); H = D;
  CYC(0x4129, 0x412b); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x412b, 0x412c); A = mem_rd(gb, HL);
  CYC(0x412c, 0x412e); alu_add(gb, 0x02);
  CYC(0x412e, 0x4130); A = alu_swap(gb, A);
  CYC(0x4130, 0x4131); alu_rrca(gb);
  CYC(0x4131, 0x4133); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x4133, 0x4134); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4134, 0x4136); A = 0x06;
  CYC(0x4136, 0x4137); mem_wr(gb, HL, A);
  CYC(0x4137, 0x4139); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x4139, 0x413b); mem_wr(gb, HL, 0x14); // SPEED_80
  CYC(0x413b, 0x413d); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x413d, 0x413f); mem_wr(gb, HL, 30);
  CYC(0x413f, 0x4141); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x4141, 0x4143); mem_wr(gb, HL, 60);
  CYC(0x4143, 0x4145); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x4145, 0x4146); A = mem_rd(gb, HL);
  CYC(0x4146, 0x4148); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x4148, 0x4149); mem_wr(gb, HL, A);
  CALL_C(0x4149, objectSetVisible83_hook, 0x1e72, 0x414c);

state1:
  CYC(0x414c, 0x414f); push_effect(gb, 0x414f); goto preventLinkFromPassing;
afterPrevent1:
  if (F & FC) { CYCT(0x414f, 0x4151); goto movingTowardRoller; } // jr c
  CYC(0x414f, 0x4151);

notPushingAgainstRoller:
  CYC(0x4151, 0x4152); H = D;
  CYC(0x4152, 0x4154); L = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(0x4154, 0x4156); mem_wr(gb, HL, 30);

moveTowardOriginalPosition:
  CYC(0x4156, 0x4157); H = D;
  CYC(0x4157, 0x4159); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4159, 0x415b); mem_wr(gb, HL, 30);
  CYC(0x415b, 0x415d); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x415d, 0x415e); B = mem_rd(gb, HL);
  CYC(0x415e, 0x4160); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x4160, 0x4161); A = mem_rd(gb, HL);
  CYC(0x4161, 0x4162); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(0x4162); return; } // ret z
  CYC(0x4162, 0x4163);
  CYC(0x4163, 0x4165); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x4165, 0x4166); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x4166); return; } // ret nz
  CYC(0x4166, 0x4167);
  CYC(0x4167, 0x4168); alu_cp(gb, B);
  CYC(0x4168, 0x416b); SET_BC(0x0008);
  if (!(F & FC)) { CYCT(0x416b, 0x416d); goto moveRollerInDirection; } // jr nc
  CYC(0x416b, 0x416d);
  CYC(0x416d, 0x4170); SET_BC(0x0118);
  CYC(0x4170, 0x4172); goto moveRollerInDirection; // jr

movingTowardRoller:
  CYC(0x4172, 0x4173); H = D;
  CYC(0x4173, 0x4175); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(0x4175, 0x4176); A = mem_rd(gb, HL);
  CYC(0x4176, 0x4178); alu_sub(gb, 0x02);
  CYC(0x4178, 0x4179); B = A;
  CYC(0x4179, 0x417a); C = B;
  CYC(0x417a, 0x417c); C = alu_sla(gb, C);
  CYC(0x417c, 0x417d); C = alu_inc8(gb, C);
  CYC(0x417d, 0x417f); L = INTERACTION_BASE + OBJ_YH;
  CYC(0x417f, 0x4182); A = W8(w1Link_yh);
  CYC(0x4182, 0x4183); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x4183, 0x4184); alu_add(gb, B);
  CYC(0x4184, 0x4185); alu_cp(gb, C);
  if (!(F & FC)) { CYCT(0x4185, 0x4187); goto notPushingAgainstRoller; } // jr nc
  CYC(0x4185, 0x4187);
  CYC(0x4187, 0x418a); A = W8(wLinkAngle);
  CYC(0x418a, 0x418c); alu_cp(gb, 0x08);
  CYC(0x418c, 0x418f); SET_BC(0x0008);
  if (F & FZ) { CYCT(0x418f, 0x4198); goto afterAngleCheck; } // jr z
  CYC(0x418f, 0x4191);
  CYC(0x4191, 0x4193); alu_cp(gb, 0x18);
  CYC(0x4193, 0x4196); SET_BC(0x0118);
  if (!(F & FZ)) { CYCT(0x4196, 0x4198); goto notPushingAgainstRoller; } // jr nz
  CYC(0x4196, 0x4198);

afterAngleCheck:
  CYC(0x4198, 0x419a); A = 0x01;
  CYC(0x419a, 0x419d); W8(wForceLinkPushAnimation) = A;
  CYC(0x419d, 0x41a0); A = W8(wBraceletGrabbingNothing);
  CYC(0x41a0, 0x41a2); alu_and(gb, 0x03);
  CYC(0x41a2, 0x41a4); A = alu_swap(gb, A);
  CYC(0x41a4, 0x41a5); alu_rrca(gb);
  CYC(0x41a5, 0x41a6); alu_cp(gb, C);
  if (F & FZ) { CYCT(0x41a6, 0x41a8); goto pushingAgainstRoller; } // jr z
  CYC(0x41a6, 0x41a8);
  CYC(0x41a8, 0x41ab); SET_HL(wInventoryB);
  CYC(0x41ab, 0x41ad); A = 0x14; // ITEM_BRACELET
  CYC(0x41ad, 0x41ae); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x41ae, 0x41b0); goto notPushingAgainstRoller; } // jr z
  CYC(0x41ae, 0x41b0);
  CYC(0x41b0, 0x41b1); SET_HL(HL + 1);
  CYC(0x41b1, 0x41b2); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x41b2, 0x41b4); goto notPushingAgainstRoller; } // jr z
  CYC(0x41b2, 0x41b4);
  CYC(0x41b4, 0x41b7); A = W8(wBraceletGrabbingNothing);
  CYC(0x41b7, 0x41b8); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x41b8, 0x41ba); goto notPushingAgainstRoller; } // jr nz
  CYC(0x41b8, 0x41ba);
  CYC(0x41ba, 0x41bd); A = W8(wLinkInAir);
  CYC(0x41bd, 0x41be); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x41be, 0x41c0); goto notPushingAgainstRoller; } // jr nz
  CYC(0x41be, 0x41c0);
  CYC(0x41c0, 0x41c1); H = D;
  CYC(0x41c1, 0x41c3); L = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(0x41c3, 0x41c4); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x41c4, 0x41c6); goto moveTowardOriginalPosition; } // jr nz
  CYC(0x41c4, 0x41c6);
  CALL_C(0x41c6, showInfoTextForRoller_hook, 0x1298, 0x41c9);
  CYC(0x41c9, 0x41cb); goto notPushingAgainstRoller; // jr

pushingAgainstRoller:
  CYC(0x41cb, 0x41cd); A = 60;
  CYC(0x41cd, 0x41cf); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x41cf, 0x41d0); mem_wr(gb, DE, A);
  CYC(0x41d0, 0x41d3); push_effect(gb, 0x41d3); goto checkRollerCanBePushed;
afterCheckPush:
  if (!(F & FZ)) { CYCT(0x41d3, 0x41d6); goto notPushingAgainstRoller; } // jp nz
  CYC(0x41d3, 0x41d6);
  CALL_C(0x41d6, interactionDecCounter1_hook, 0x23cc, 0x41d9);
  if (!(F & FZ)) { RET_TAKEN(0x41d9); return; } // ret nz
  CYC(0x41d9, 0x41da);

moveRollerInDirection:
  CYC(0x41da, 0x41dc); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x41dc, 0x41dd); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x41dd, 0x41df); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x41df, 0x41e0); mem_wr(gb, HL, C);
  CYC(0x41e0, 0x41e2); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x41e2, 0x41e3); A = mem_rd(gb, HL);
  CYC(0x41e3, 0x41e4); alu_add(gb, A);
  CYC(0x41e4, 0x41e5); alu_add(gb, B);
  CALL_C(0x41e5, interactionSetAnimation_hook, 0x262e, 0x41e8);
  CYC(0x41e8, 0x41eb); SET_HL(wInformativeTextsShown);
  CYC(0x41eb, 0x41ed); mem_wr(gb, HL, mem_rd(gb, HL) | (1 << 6));

state2:
  CALL_C(0x41ed, objectApplySpeed_hook, 0x201d, 0x41f0);
  CALL_C(0x41f0, interactionAnimate_hook, 0x261b, 0x41f3);
  CALL_C(0x41f3, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x41f6);
  if (!(F & FC)) { CYCT(0x41f6, 0x41fb); goto afterCollisionCheck; } // jr nc
  CYC(0x41f6, 0x41f8);
  CYC(0x41f8, 0x41fb); push_effect(gb, 0x41fb); goto updateLinkPositionWhileRollerMoving;

afterCollisionCheck:
  CYC(0x41fb, 0x41fc); H = D;
  CYC(0x41fc, 0x41fe); L = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x41fe, 0x41ff); A = mem_rd(gb, HL);
  CYC(0x41ff, 0x4200); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4200, 0x4202); goto rollerSound; } // jr z
  CYC(0x4200, 0x4202);
  CYC(0x4202, 0x4203); A = alu_inc8(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x4203); return; } // ret nz
  CYC(0x4203, 0x4204);
  CYC(0x4204, 0x4206); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x4206, 0x4208); mem_wr(gb, HL, 30);
  CYC(0x4208, 0x4209); L = alu_inc8(gb, L);
  CYC(0x4209, 0x420b); mem_wr(gb, HL, 60);
  CYC(0x420b, 0x420d); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x420d, 0x420f); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(0x420f); return; // ret

rollerSound:
  CYC(0x420f, 0x4211); mem_wr(gb, HL, 0x01);
  CYC(0x4211, 0x4213); A = 0x82; // SND_ROLLER
  CYC(0x4213, 0x4216); playSound_b00_hook(gb); return; // jp

// interactionCode7a@updateLinkPositionWhileRollerMoving / @squashLink: reached only by
// one genuine call, from @state2's return address 0x41fb above; never separately hooked.
updateLinkPositionWhileRollerMoving:
  CYC(0x4216, 0x4219); A = W8(w1Link_adjacentWallsBitset);
  CYC(0x4219, 0x421b); alu_cp(gb, 0x53);
  if (F & FZ) { CYCT(0x421b, 0x421d); goto squashLink; } // jr z
  CYC(0x421b, 0x421d);
  CYC(0x421d, 0x421f); alu_cp(gb, 0xac);
  if (F & FZ) { CYCT(0x421f, 0x4221); goto squashLink; } // jr z
  CYC(0x421f, 0x4221);
  CYC(0x4221, 0x4223); alu_cp(gb, 0x33);
  if (F & FZ) { CYCT(0x4223, 0x4225); goto squashLink; } // jr z
  CYC(0x4223, 0x4225);
  CYC(0x4225, 0x4227); alu_cp(gb, 0xc3);
  if (F & FZ) { CYCT(0x4227, 0x4229); goto squashLink; } // jr z
  CYC(0x4227, 0x4229);
  CYC(0x4229, 0x422b); alu_cp(gb, 0xcc);
  if (F & FZ) { CYCT(0x422b, 0x422d); goto squashLink; } // jr z
  CYC(0x422b, 0x422d);
  CYC(0x422d, 0x422f); alu_cp(gb, 0x3c);
  if (F & FZ) { CYCT(0x422f, 0x4231); goto squashLink; } // jr z
  CYC(0x422f, 0x4231);
  CYC(0x4231, 0x4234); push_effect(gb, 0x4234); goto preventLinkFromPassing;
afterPrevent2:
  CYC(0x4234, 0x4237); A = W8(w1Link_adjacentWallsBitset);
  CYC(0x4237, 0x4239); alu_and(gb, 0x0f);
  if (F & FZ) {
    RET_TAKEN(0x4239);
    if (gb->pc == 0x41fb && gb->sp == sp0_) goto afterCollisionCheck;
    return;
  } // ret z
  CYC(0x4239, 0x423a);
  CALL_C(0x423a, objectGetAngleTowardLink_hook, 0x1e9c, 0x423d);
  CYC(0x423d, 0x423f); alu_cp(gb, 0x10);
  CYC(0x423f, 0x4241); C = 0x08;
  if (F & FC) { CYCT(0x4241, 0x4245); goto afterCTable; } // jr c
  CYC(0x4241, 0x4243);
  CYC(0x4243, 0x4245); C = 0x18;

afterCTable:
  CYC(0x4245, 0x4247); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x4247, 0x4248); A = mem_rd(gb, DE);
  CYC(0x4248, 0x4249); alu_cp(gb, C);
  if (!(F & FZ)) {
    RET_TAKEN(0x4249);
    if (gb->pc == 0x41fb && gb->sp == sp0_) goto afterCollisionCheck;
    return;
  } // ret nz
  CYC(0x4249, 0x424a);
  CYC(0x424a, 0x424c); B = 0x28; // SPEED_100
  CYC(0x424c, 0x424f); updateLinkPositionGivenVelocity_hook(gb);
  if (gb->pc == 0x41fb && gb->sp == sp0_) goto afterCollisionCheck;
  return; // jp

squashLink:
  CYC(0x424f, 0x4252); A = W8(w1Link_state);
  CYC(0x4252, 0x4254); alu_cp(gb, 0x01); // LINK_STATE_NORMAL
  if (!(F & FZ)) {
    RET_TAKEN(0x4254);
    if (gb->pc == 0x41fb && gb->sp == sp0_) goto afterCollisionCheck;
    return;
  } // ret nz
  CYC(0x4254, 0x4255);
  CYC(0x4255, 0x4257); A = 0x11; // LINK_STATE_SQUISHED
  CYC(0x4257, 0x425a); W8(wLinkForceState) = A;
  CYC(0x425a, 0x425b); alu_xor(gb, A);
  CYC(0x425b, 0x425e); W8(wcc50) = A;
  RET(0x425e);
  if (gb->pc == 0x41fb && gb->sp == sp0_) goto afterCollisionCheck;
  return; // ret

// interactionCode7a@checkRollerCanBePushed / @nextTile: zflag set if nothing blocks the
// roller in direction c. Reached only by one genuine call, from @pushingAgainstRoller's
// return address 0x41d3 above; never separately hooked.
checkRollerCanBePushed:
  CYC(0x425f, 0x4260); push_effect(gb, BC);
  CYC(0x4260, 0x4262); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x4262, 0x4263); A = mem_rd(gb, DE);
  CYC(0x4263, 0x4265); alu_add(gb, 0x02);
  CYC(0x4265, 0x4267); H8(hFF8B) = A;
  CYC(0x4267, 0x4269); A = alu_swap(gb, A);
  CYC(0x4269, 0x426a); alu_rrca(gb);
  CYC(0x426a, 0x426b); B = A;
  CYC(0x426b, 0x426d); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x426d, 0x426e); A = mem_rd(gb, DE);
  CYC(0x426e, 0x426f); alu_sub(gb, B);
  CYC(0x426f, 0x4271); alu_add(gb, 0x08);
  CYC(0x4271, 0x4273); alu_and(gb, 0xf0);
  CYC(0x4273, 0x4274); B = A;
  CYC(0x4274, 0x4276); A = 0x08;
  CYC(0x4276, 0x4278); L = 0x01;
  CYC(0x4278, 0x4279); alu_cp(gb, C);
  if (F & FZ) { CYCT(0x4279, 0x427b); goto afterSign; } // jr z
  CYC(0x4279, 0x427b);
  CYC(0x427b, 0x427d); L = 0xff;

afterSign:
  CYC(0x427d, 0x427f); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x427f, 0x4280); A = mem_rd(gb, DE);
  CYC(0x4280, 0x4282); A = alu_swap(gb, A);
  CYC(0x4282, 0x4283); alu_add(gb, L);
  CYC(0x4283, 0x4285); alu_and(gb, 0x0f);
  CYC(0x4285, 0x4286); alu_or(gb, B);
  CYC(0x4286, 0x4287); SET_BC(pop_effect(gb));
  CYC(0x4287, 0x4288); L = A;
  CYC(0x4288, 0x428a); H = 0xce; // >wRoomCollisions
  CYC(0x428a, 0x428c); A = H8(hFF8B);
  CYC(0x428c, 0x428d); E = A;

nextTile:
  CYC(0x428d, 0x428e); A = mem_rd(gb, HL);
  CYC(0x428e, 0x4290); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(0x4290, 0x4292); goto loopIncrement; } // jr nc
  CYC(0x4290, 0x4292);
  CYC(0x4292, 0x4293); alu_or(gb, A);
  if (!(F & FZ)) {
    RET_TAKEN(0x4293);
    if (gb->pc == 0x41d3 && gb->sp == sp0_) goto afterCheckPush;
    return;
  } // ret nz
  CYC(0x4293, 0x4294);

loopIncrement:
  CYC(0x4294, 0x4295); A = L;
  CYC(0x4295, 0x4297); alu_add(gb, 0x10);
  CYC(0x4297, 0x4298); L = A;
  CYC(0x4298, 0x4299); E = alu_dec8(gb, E);
  if (!(F & FZ)) { CYCT(0x4299, 0x429b); goto nextTile; } // jr nz
  CYC(0x4299, 0x429b);
  CYC(0x429b, 0x429c); alu_xor(gb, A);
  RET(0x429c);
  if (gb->pc == 0x41d3 && gb->sp == sp0_) goto afterCheckPush;
  return;

// interactionCode7a@preventLinkFromPassing: sets carry via objectPreventLinkFromPassing if
// Link is pushing against the roller. Reached by two genuine calls: @state1's return address
// 0x414f above, pushed at sp0_ since @state1 itself has no outer push; and
// @updateLinkPositionWhileRollerMoving's return address 0x4234 above, pushed at sp0_-2 since
// that caller was itself entered via its own push_effect from @state2 -- the sp comparison
// must match each edge's own push depth, not sp0_ uniformly. Has both literal ret exits and a
// tail-jump-into-external-hook exit, so every exit needs the same two-way resume check.
preventLinkFromPassing:
  CYC(0x429d, 0x42a0); A = W8(w1Link_collisionType);
  CYC(0x42a0, 0x42a2); alu_bit(gb, 7, A);
  if (F & FZ) {
    RET_TAKEN(0x42a2);
    if (gb->pc == 0x414f && gb->sp == sp0_) goto afterPrevent1;
    if (gb->pc == 0x4234 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterPrevent2;
    return;
  }
  CYC(0x42a2, 0x42a3);
  CYC(0x42a3, 0x42a6); A = W8(w1Link_state);
  CYC(0x42a6, 0x42a8); alu_cp(gb, 0x01);
  if (!(F & FZ)) {
    RET_TAKEN(0x42a8);
    if (gb->pc == 0x414f && gb->sp == sp0_) goto afterPrevent1;
    if (gb->pc == 0x4234 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterPrevent2;
    return;
  }
  CYC(0x42a8, 0x42a9);
  CYC(0x42a9, 0x42ac); objectPreventLinkFromPassing_hook(gb); // jp
  if (gb->pc == 0x414f && gb->sp == sp0_) goto afterPrevent1;
  if (gb->pc == 0x4234 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterPrevent2;
  return;
}
