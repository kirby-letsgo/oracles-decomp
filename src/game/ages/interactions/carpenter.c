#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCode9a_jump_table(GB *gb) {
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

static void interactionCode9a_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void interactionCode9a_add_double_index_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_CARPENTER
//
// Variables:
//   var3f: Nonzero if the carpenter has returned to the boss
// ==================================================================================================
void interactionCode9a_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x50d7, 0x50d9); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x50d9, 0x50da); A = mem_rd(gb, DE);
  CYC(0x50da, 0x50db); push_effect(gb, 0x50db);
  switch (interactionCode9a_jump_table(gb)) {
    case 0x50e1: goto state0;
    case 0x522a: goto state1;
    case 0x5262: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x50e1, 0x50e3); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x50e3, 0x50e4); A = mem_rd(gb, DE);
  CYC(0x50e4, 0x50e6); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(0x50e6, 0x50e8); goto initialize; } // jr z
  CYC(0x50e6, 0x50e8);
  CYC(0x50e8, 0x50ea); A = 0x25;
  CALL_C(0x50ea, checkGlobalFlag_hook, 0x31f3, 0x50ed);
  if (!(F & FZ)) { CYCT(0x50ed, 0x50f0); goto delete_; } // jp nz
  CYC(0x50ed, 0x50f0);
  CALL_C(0x50f0, checkIsLinkedGame_hook, 0x1992, 0x50f3);
  if (F & FZ) { CYCT(0x50f3, 0x50f5); goto l_50fc; } // jr z
  CYC(0x50f3, 0x50f5);
  CYC(0x50f5, 0x50f7); A = 0x38;
  CALL_C(0x50f7, checkGlobalFlag_hook, 0x31f3, 0x50fa);
  if (F & FZ) { CYCT(0x50fa, 0x50fc); goto delete_; } // jr z
  CYC(0x50fa, 0x50fc);

l_50fc:
  CYC(0x50fc, 0x50fe); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x50fe, 0x50ff); A = mem_rd(gb, DE);
  CYC(0x50ff, 0x5100); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x5100, 0x5102); goto runSubidFF; } // jr z
  CYC(0x5100, 0x5102);

initialize:
  CALL_C(0x5102, interactionIncState_hook, 0x23e0, 0x5105);
  CALL_C(0x5105, interactionInitGraphics_hook, 0x15fb, 0x5108);
  CALL_C(0x5108, objectSetVisiblec2_hook, 0x1e45, 0x510b);
  CYC(0x510b, 0x510d); A = 0x23;
  CALL_C(0x510d, interactionSetHighTextIndex_hook, 0x253b, 0x5110);
  CYC(0x5110, 0x5112); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5112, 0x5113); A = mem_rd(gb, DE);
  CYC(0x5113, 0x5115); alu_and(gb, 0x0f);
  CYC(0x5115, 0x5116); push_effect(gb, 0x5116);
  switch (interactionCode9a_jump_table(gb)) {
    case 0x51f2: goto initSubid00;
    case 0x51ab: goto initSubid01;
    case 0x51b8: goto initSubid02;
    default: hook_continue(gb, HL, sp0_); return;
  }

runSubidFF:
  CYC(0x512a, 0x512d); A = mem_rd(gb, wTmpcfc0_carpenterSearch_cfd0);
  CYC(0x512d, 0x512e); alu_or(gb, A);
  if (F & FZ) { CYCT(0x512e, 0x512f); ret_effect(gb); return; } // ret z
  CYC(0x512e, 0x512f);
  CYC(0x512f, 0x5132); A = mem_rd(gb, wScrollMode);
  CYC(0x5132, 0x5134); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(0x5134, 0x5135); ret_effect(gb); return; } // ret nz
  CYC(0x5134, 0x5135);
  CYC(0x5135, 0x5137); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x5137, 0x5138); A = mem_rd(gb, DE);
  CYC(0x5138, 0x5139); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5139, 0x513b); goto l_5140; } // jr z
  CYC(0x5139, 0x513b);
  CYC(0x513b, 0x513c); A = alu_dec8(gb, A);
  CYC(0x513c, 0x513d); mem_wr(gb, DE, A);
  CYC(0x513d, 0x5140); mem_wr(gb, wDisallowMountingCompanion, A);

l_5140:
  CYC(0x5140, 0x5142); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5142, 0x5143); A = mem_rd(gb, DE);
  CYC(0x5143, 0x5144); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x5144, 0x5146); goto substate1; } // jr z
  CYC(0x5144, 0x5146);

// substate0
  CYC(0x5146, 0x5149); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x5149, 0x514a); H = A;
  CYC(0x514a, 0x514c); L = 0x0d; // SpecialObject.xh
  CYC(0x514c, 0x514e); A = 0x10;
  CYC(0x514e, 0x514f); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x514f, 0x5150); ret_effect(gb); return; } // ret c
  CYC(0x514f, 0x5150);
  CYC(0x5150, 0x5152); A = 0x01;
  CYC(0x5152, 0x5153); mem_wr(gb, DE, A); // [substate] = $01
  CYC(0x5153, 0x5156); SET_BC(0x2307); // TX_2307
  CYC(0x5156, 0x5159); showText_hook(gb); return; // jp

substate1:
  CALL_C(0x5159, retIfTextIsActive_hook, 0x1859, 0x515c);
  CYC(0x515c, 0x515f); A = mem_rd(gb, wSelectedTextOption);
  CYC(0x515f, 0x5160); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x5160, 0x5162); goto option0; } // jr z
  CYC(0x5160, 0x5162);
  CYC(0x5162, 0x5165); SET_HL(wTmpcfc0_carpenterSearch_cfd0);
  CYC(0x5165, 0x5167); B = 0x10;
  CALL_C(0x5167, clearMemory_hook, 0x046f, 0x516a);

delete_:
  CYC(0x516a, 0x516d); interactionDelete_hook(gb); return; // jp

option0:
  CALL_C(0x516d, resetLinkInvincibility_hook, 0x2ba9, 0x5170);
  CYC(0x5170, 0x5172); A = 0xc4; // -60
  CYC(0x5172, 0x5175); mem_wr(gb, w1Link_invincibilityCounter, A);
  CYC(0x5175, 0x5177); A = 0x78;
  CYC(0x5177, 0x517a); mem_wr(gb, wDisallowMountingCompanion, A);
  CYC(0x517a, 0x517c); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x517c, 0x517d); mem_wr(gb, DE, A);
  CYC(0x517d, 0x517f); A = 0x08; // ANGLE_RIGHT
  CYC(0x517f, 0x5182); mem_wr(gb, w1Link_angle, A);
  CYC(0x5182, 0x5184); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x5184, 0x5185); alu_xor(gb, A);
  CYC(0x5185, 0x5186); mem_wr(gb, DE, A);
  CYC(0x5186, 0x5189); A = mem_rd(gb, wLinkObjectIndex);
  CYC(0x5189, 0x518a); H = A;
  CYC(0x518a, 0x518c); L = 0x0d; // SpecialObject.xh
  CYC(0x518c, 0x518e); mem_wr(gb, HL, 0x12);
  CYC(0x518e, 0x518f); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x518f, 0x5190); ret_effect(gb); return; } // ret nc
  CYC(0x518f, 0x5190);
  CYC(0x5190, 0x5192); L = 0x01; // SpecialObject.id
  CYC(0x5192, 0x5193); A = mem_rd(gb, HL);
  CYC(0x5193, 0x5195); L = 0x04; // SpecialObject.state
  CYC(0x5195, 0x5197); alu_cp(gb, 0x0b); // SPECIALOBJECT_RICKY
  if (!(F & FZ)) { CYCT(0x5197, 0x5199); goto l_51a1; } // jr nz
  CYC(0x5197, 0x5199);

// @@ricky (falls into from above when the special object is Ricky)
  CYC(0x5199, 0x519a); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x519a, 0x519c); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(0x519c, 0x519d); ret_effect(gb); return; } // ret nz
  CYC(0x519c, 0x519d);
  CYC(0x519d, 0x519f); A = 0x03;
  CYC(0x519f, 0x51a0); mem_wr(gb, HL, A);
  RET(0x51a0); return;

l_51a1:
  CYC(0x51a1, 0x51a3); alu_cp(gb, 0x0c); // SPECIALOBJECT_DIMITRI
  if (!(F & FZ)) { CYCT(0x51a3, 0x51a4); ret_effect(gb); return; } // ret nz
  CYC(0x51a3, 0x51a4);

// dimitri
  CYC(0x51a4, 0x51a5); A = mem_rd(gb, HL);
  CYC(0x51a5, 0x51a7); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x51a7, 0x51a8); ret_effect(gb); return; } // ret nz
  CYC(0x51a7, 0x51a8);
  CYC(0x51a8, 0x51aa); mem_wr(gb, HL, 0x0d);
  RET(0x51aa); return;

initSubid01:
  CYC(0x51ab, 0x51ac); alu_xor(gb, A);
  CYC(0x51ac, 0x51ae); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x51ae, 0x51af); mem_wr(gb, DE, A);
  CALL_C(0x51af, objectMakeTileSolid_hook, 0x20b2, 0x51b2);
  CYC(0x51b2, 0x51b4); H = 0xcf; // >wRoomLayout
  CYC(0x51b4, 0x51b6); mem_wr(gb, HL, 0x00);
  CYC(0x51b6, 0x51b8); goto checkDoBridgeBuildingCutscene; // jr

initSubid02:
  CYC(0x51b8, 0x51bb); A = mem_rd(gb, wActiveRoom);
  CYC(0x51bb, 0x51bd); alu_cp(gb, 0x25); // <ROOM_AGES_025
  if (F & FZ) { CYCT(0x51bd, 0x51bf); goto inBridgeRoom; } // jr z
  CYC(0x51bd, 0x51bf);
  CYC(0x51bf, 0x51c0); A = mem_rd(gb, DE);
  CYC(0x51c0, 0x51c2); A = alu_swap(gb, A);
  CYC(0x51c2, 0x51c4); alu_and(gb, 0x0f);
  CYC(0x51c4, 0x51c7); SET_HL(wAnimalCompanion);
  CYC(0x51c7, 0x51c8); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x51c8, 0x51ca); goto delete_; } // jr nz
  CYC(0x51c8, 0x51ca);
  CYC(0x51ca, 0x51cb); A = mem_rd(gb, DE);
  CYC(0x51cb, 0x51cd); alu_and(gb, 0x0f);
  CYC(0x51cd, 0x51d0); SET_HL(wTmpcfc0_carpenterSearch_carpentersFound);
  CALL_C(0x51d0, checkFlag_hook, 0x0205, 0x51d3);
  if (!(F & FZ)) { CYCT(0x51d3, 0x51d5); goto delete2; } // jr nz
  CYC(0x51d3, 0x51d5);
  CYC(0x51d5, 0x51d7); goto checkDoBridgeBuildingCutscene; // jr (unconditional)

inBridgeRoom:
  CYC(0x51d7, 0x51d8); A = mem_rd(gb, DE);
  CYC(0x51d8, 0x51da); alu_and(gb, 0x0f);
  CYC(0x51da, 0x51dd); SET_HL(wTmpcfc0_carpenterSearch_carpentersFound);
  CALL_C(0x51dd, checkFlag_hook, 0x0205, 0x51e0);
  CYC(0x51e0, 0x51e2); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x51e2, 0x51e3); mem_wr(gb, DE, A);
  if (F & FZ) { CYCT(0x51e3, 0x51e5); goto delete2; } // jr z
  CYC(0x51e3, 0x51e5);
  CYC(0x51e5, 0x51e6); A = mem_rd(gb, HL);
  CYC(0x51e6, 0x51e8); alu_cp(gb, 0x1c);
  if (!(F & FZ)) { CYCT(0x51e8, 0x51ea); goto checkDoBridgeBuildingCutscene; } // jr nz
  CYC(0x51e8, 0x51ea);
  CYC(0x51ea, 0x51ec); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x51ec, 0x51ed); A = mem_rd(gb, DE);
  CYC(0x51ed, 0x51ef); alu_add(gb, 0x04);
  CYC(0x51ef, 0x51f0); mem_wr(gb, DE, A);
  CYC(0x51f0, 0x51f2); goto checkDoBridgeBuildingCutscene; // jr (unconditional)

initSubid00:
  CYC(0x51f2, 0x51f4); A = 0x03;
  CYC(0x51f4, 0x51f6); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x51f6, 0x51f7); mem_wr(gb, DE, A);
  CYC(0x51f7, 0x51fa); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CYC(0x51fa, 0x51fc); alu_cp(gb, 0x1c);
  if (!(F & FZ)) { CYCT(0x51fc, 0x51fe); goto checkDoBridgeBuildingCutscene; } // jr nz
  CYC(0x51fc, 0x51fe);
  CYC(0x51fe, 0x5200); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5200, 0x5202); A = 0x05;
  CYC(0x5202, 0x5203); mem_wr(gb, DE, A);
  CYC(0x5203, 0x5205); A = 0x58;
  CYC(0x5205, 0x5207); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x5207, 0x5208); mem_wr(gb, DE, A);

checkDoBridgeBuildingCutscene:
  CYC(0x5208, 0x520a); A = 0x25;
  CALL_C(0x520a, checkGlobalFlag_hook, 0x31f3, 0x520d);
  if (!(F & FZ)) { CYCT(0x520d, 0x520f); goto delete2; } // jr nz
  CYC(0x520d, 0x520f);

// @initSubid09 (falls into from @checkDoBridgeBuildingCutscene above)
  CALL_C(0x520f, objectMarkSolidPosition_hook, 0x24f0, 0x5212);
  CYC(0x5212, 0x5214); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5214, 0x5215); A = mem_rd(gb, DE);
  CYC(0x5215, 0x5217); alu_and(gb, 0x0f);
  CYC(0x5217, 0x521a); SET_HL(0x52cc); // @animationsForBridgeBuildCutsceneStart
  CYC(0x521a, 0x521b); interactionCode9a_add_a_to_hl(gb); // rst $10
  CYC(0x521b, 0x521c); A = mem_rd(gb, HL);
  CALL_C(0x521c, interactionSetAnimation_hook, 0x262e, 0x521f);
  CYC(0x521f, 0x5221); A = 0x06;
  CALL_C(0x5221, objectSetCollideRadius_hook, 0x24a1, 0x5224);
  CALL_C(0x5224, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x5227);
  CYC(0x5227, 0x522a); goto loadScript; // jp

state1:
  CYC(0x522a, 0x522c); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x522c, 0x522d); A = mem_rd(gb, DE);
  CYC(0x522d, 0x522f); alu_and(gb, 0x0f);
  CYC(0x522f, 0x5230); push_effect(gb, 0x5230);
  switch (interactionCode9a_jump_table(gb)) {
    case 0x5244: goto runSubid;
    case 0x5253: goto runSubid01;
    default: hook_continue(gb, HL, sp0_); return;
  }

runSubid:
  CALL_C(0x5244, interactionAnimateAsNpc_hook, 0x26db, 0x5247);
  CYC(0x5247, 0x5249); C = 0x40;
  CALL_C(0x5249, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x524c);
  CALL_C(0x524c, interactionRunScript_hook, 0x2552, 0x524f);
  if (!(F & FC)) { CYCT(0x524f, 0x5250); ret_effect(gb); return; } // ret nc

delete2:
  CYC(0x5250, 0x5253); interactionDelete_hook(gb); return; // jp

runSubid01:
  CYC(0x5253, 0x5256); A = mem_rd(gb, wTmpcfc0_carpenterSearch_cfd0);
  CYC(0x5256, 0x5258); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(0x5258, 0x5259); ret_effect(gb); return; } // ret nz
  CYC(0x5258, 0x5259);
  CYC(0x5259, 0x525b); A = 0x3a;
  CYC(0x525b, 0x525d); C = 0x55;
  CALL_C(0x525d, setTile_hook, 0x3a9c, 0x5260);
  CYC(0x5260, 0x5262); goto delete2; // jr

state2:
  CALL_C(0x5262, interactionAnimate_hook, 0x261b, 0x5265);
  CYC(0x5265, 0x5266); H = D;
  CYC(0x5266, 0x5268); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x5268, 0x526a); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(0x526a, 0x526c); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x526c, 0x526e); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(0x526e, objectApplySpeed_hook, 0x201d, 0x5271);
  CALL_C(0x5271, objectCheckWithinScreenBoundary_hook, 0x2184, 0x5274);
  if (!(F & FC)) { CYCT(0x5274, 0x5276); goto leftScreen; } // jr nc
  CYC(0x5274, 0x5276);
  CYC(0x5276, 0x5278); C = 0x10;
  CALL_C(0x5278, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x527b);
  if (!(F & FZ)) { CYCT(0x527b, 0x527c); ret_effect(gb); return; } // ret nz
  CYC(0x527b, 0x527c);
  CYC(0x527c, 0x527f); SET_BC(0xfe00); // -$200
  CALL_C(0x527f, objectSetSpeedZ_hook, 0x239d, 0x5282);
  CYC(0x5282, 0x5284); A = 0x53; // SND_JUMP
  CYC(0x5284, 0x5287); playSound_b00_hook(gb); return; // jp

leftScreen:
  CYC(0x5287, 0x5289); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5289, 0x528a); A = mem_rd(gb, DE);
  CYC(0x528a, 0x528c); alu_and(gb, 0x0f);
  CYC(0x528c, 0x528f); SET_HL(wTmpcfc0_carpenterSearch_carpentersFound);
  CALL_C(0x528f, setFlag_hook, 0x020e, 0x5292);
  CYC(0x5292, 0x5293); A = mem_rd(gb, HL);
  CYC(0x5293, 0x5295); alu_cp(gb, 0x1c);
  CYC(0x5295, 0x5298); SET_HL(0x52a4); // @warpDest
  if (F & FZ) { CYCT(0x5298, 0x529b); setWarpDestVariables_hook(gb); return; } // jp z
  CYC(0x5298, 0x529b);
  CYC(0x529b, 0x529c); alu_xor(gb, A);
  CYC(0x529c, 0x529f); mem_wr(gb, wMenuDisabled, A);
  CYC(0x529f, 0x52a2); mem_wr(gb, wDisabledObjects, A);
  CYC(0x52a2, 0x52a4); goto delete2; // jr

loadScript:
  CYC(0x52a9, 0x52ab); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x52ab, 0x52ac); A = mem_rd(gb, DE);
  CYC(0x52ac, 0x52ae); alu_and(gb, 0x0f);
  CYC(0x52ae, 0x52b1); SET_HL(0x52b8); // @scriptTable
  CYC(0x52b1, 0x52b2); interactionCode9a_add_double_index_from_rst(gb, 0x52b2);
  CYC(0x52b2, 0x52b3); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x52b3, 0x52b4); H = mem_rd(gb, HL);
  CYC(0x52b4, 0x52b5); L = A;
  CYC(0x52b5, 0x52b8); interactionSetScript_hook(gb); return; // jp

  // @warpDest (0b:52a4): m_HardcodedWarpA ROOM_AGES_025, $00, $48, $03 -- pure ROM data referenced
  // only via SET_HL(0x52a4) above.
  // @scriptTable (0b:52b8) / @animationsForBridgeBuildCutsceneStart (0b:52cc): pure ROM data
  // referenced only via SET_HL(0x52b8) / SET_HL(0x52cc) above.
}
