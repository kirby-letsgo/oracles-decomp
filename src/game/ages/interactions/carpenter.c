#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode9a);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCode9a_jump_table(gb));
    if (jt_ == b_+10) { goto state0; }
    else if (jt_ == b_+339) { goto state1; }
    else if (jt_ == b_+395) { goto state2; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(b_+15, b_+17); goto initialize; } // jr z
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); A = 0x25;
  CALL_C(b_+19, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+22);
  if (!(F & FZ)) { CYCT(b_+22, b_+25); goto delete_; } // jp nz
  CYC(b_+22, b_+25);
  CALL_C(b_+25, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+28);
  if (F & FZ) { CYCT(b_+28, b_+30); goto l_50fc; } // jr z
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); A = 0x38;
  CALL_C(b_+32, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+35);
  if (F & FZ) { CYCT(b_+35, b_+37); goto delete_; } // jr z
  CYC(b_+35, b_+37);

l_50fc:
  CYC(b_+37, b_+39); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+39, b_+40); A = mem_rd(gb, DE);
  CYC(b_+40, b_+41); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+41, b_+43); goto runSubidFF; } // jr z
  CYC(b_+41, b_+43);

initialize:
  CALL_C(b_+43, interactionIncState_hook, SYM(interactionIncState), b_+46);
  CALL_C(b_+46, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+49);
  CALL_C(b_+49, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+52);
  CYC(b_+52, b_+54); A = 0x23;
  CALL_C(b_+54, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+57);
  CYC(b_+57, b_+59); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+59, b_+60); A = mem_rd(gb, DE);
  CYC(b_+60, b_+62); alu_and(gb, 0x0f);
  CYC(b_+62, b_+63); push_effect(gb, b_+63);
  do { uint16_t jt_ = (interactionCode9a_jump_table(gb));
    if (jt_ == b_+283) { goto initSubid00; }
    else if (jt_ == b_+212) { goto initSubid01; }
    else if (jt_ == b_+225) { goto initSubid02; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

runSubidFF:
  CYC(b_+83, b_+86); A = mem_rd(gb, wTmpcfc0_carpenterSearch_cfd0);
  CYC(b_+86, b_+87); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+87, b_+88); ret_effect(gb); return; } // ret z
  CYC(b_+87, b_+88);
  CYC(b_+88, b_+91); A = mem_rd(gb, wScrollMode);
  CYC(b_+91, b_+93); alu_and(gb, 0x0e);
  if (!(F & FZ)) { CYCT(b_+93, b_+94); ret_effect(gb); return; } // ret nz
  CYC(b_+93, b_+94);
  CYC(b_+94, b_+96); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+96, b_+97); A = mem_rd(gb, DE);
  CYC(b_+97, b_+98); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+98, b_+100); goto l_5140; } // jr z
  CYC(b_+98, b_+100);
  CYC(b_+100, b_+101); A = alu_dec8(gb, A);
  CYC(b_+101, b_+102); mem_wr(gb, DE, A);
  CYC(b_+102, b_+105); mem_wr(gb, wDisallowMountingCompanion, A);

l_5140:
  CYC(b_+105, b_+107); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  CYC(b_+108, b_+109); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+109, b_+111); goto substate1; } // jr z
  CYC(b_+109, b_+111);

// substate0
  CYC(b_+111, b_+114); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+114, b_+115); H = A;
  CYC(b_+115, b_+117); L = 0x0d; // SpecialObject.xh
  CYC(b_+117, b_+119); A = 0x10;
  CYC(b_+119, b_+120); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+120, b_+121); ret_effect(gb); return; } // ret c
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+123); A = 0x01;
  CYC(b_+123, b_+124); mem_wr(gb, DE, A); // [substate] = $01
  CYC(b_+124, b_+127); SET_BC(0x2307); // TX_2307
  CYC(b_+127, b_+130); showText_hook(gb); return; // jp

substate1:
  CALL_C(b_+130, retIfTextIsActive_hook, SYM(retIfTextIsActive), b_+133);
  CYC(b_+133, b_+136); A = mem_rd(gb, wSelectedTextOption);
  CYC(b_+136, b_+137); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+137, b_+139); goto option0; } // jr z
  CYC(b_+137, b_+139);
  CYC(b_+139, b_+142); SET_HL(wTmpcfc0_carpenterSearch_cfd0);
  CYC(b_+142, b_+144); B = 0x10;
  CALL_C(b_+144, clearMemory_hook, SYM(clearMemory), b_+147);

delete_:
  CYC(b_+147, b_+150); interactionDelete_hook(gb); return; // jp

option0:
  CALL_C(b_+150, resetLinkInvincibility_hook, SYM(resetLinkInvincibility), b_+153);
  CYC(b_+153, b_+155); A = 0xc4; // -60
  CYC(b_+155, b_+158); mem_wr(gb, w1Link_invincibilityCounter, A);
  CYC(b_+158, b_+160); A = 0x78;
  CYC(b_+160, b_+163); mem_wr(gb, wDisallowMountingCompanion, A);
  CYC(b_+163, b_+165); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+165, b_+166); mem_wr(gb, DE, A);
  CYC(b_+166, b_+168); A = 0x08; // ANGLE_RIGHT
  CYC(b_+168, b_+171); mem_wr(gb, w1Link_angle, A);
  CYC(b_+171, b_+173); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+173, b_+174); alu_xor(gb, A);
  CYC(b_+174, b_+175); mem_wr(gb, DE, A);
  CYC(b_+175, b_+178); A = mem_rd(gb, wLinkObjectIndex);
  CYC(b_+178, b_+179); H = A;
  CYC(b_+179, b_+181); L = 0x0d; // SpecialObject.xh
  CYC(b_+181, b_+183); mem_wr(gb, HL, 0x12);
  CYC(b_+183, b_+184); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+184, b_+185); ret_effect(gb); return; } // ret nc
  CYC(b_+184, b_+185);
  CYC(b_+185, b_+187); L = 0x01; // SpecialObject.id
  CYC(b_+187, b_+188); A = mem_rd(gb, HL);
  CYC(b_+188, b_+190); L = 0x04; // SpecialObject.state
  CYC(b_+190, b_+192); alu_cp(gb, 0x0b); // SPECIALOBJECT_RICKY
  if (!(F & FZ)) { CYCT(b_+192, b_+194); goto l_51a1; } // jr nz
  CYC(b_+192, b_+194);

// @@ricky (falls into from above when the special object is Ricky)
  CYC(b_+194, b_+195); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+195, b_+197); alu_cp(gb, 0x05);
  if (!(F & FZ)) { CYCT(b_+197, b_+198); ret_effect(gb); return; } // ret nz
  CYC(b_+197, b_+198);
  CYC(b_+198, b_+200); A = 0x03;
  CYC(b_+200, b_+201); mem_wr(gb, HL, A);
  RET(b_+201); return;

l_51a1:
  CYC(b_+202, b_+204); alu_cp(gb, 0x0c); // SPECIALOBJECT_DIMITRI
  if (!(F & FZ)) { CYCT(b_+204, b_+205); ret_effect(gb); return; } // ret nz
  CYC(b_+204, b_+205);

// dimitri
  CYC(b_+205, b_+206); A = mem_rd(gb, HL);
  CYC(b_+206, b_+208); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+208, b_+209); ret_effect(gb); return; } // ret nz
  CYC(b_+208, b_+209);
  CYC(b_+209, b_+211); mem_wr(gb, HL, 0x0d);
  RET(b_+211); return;

initSubid01:
  CYC(b_+212, b_+213); alu_xor(gb, A);
  CYC(b_+213, b_+215); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+215, b_+216); mem_wr(gb, DE, A);
  CALL_C(b_+216, objectMakeTileSolid_hook, SYM(objectMakeTileSolid), b_+219);
  CYC(b_+219, b_+221); H = 0xcf; // >wRoomLayout
  CYC(b_+221, b_+223); mem_wr(gb, HL, 0x00);
  CYC(b_+223, b_+225); goto checkDoBridgeBuildingCutscene; // jr

initSubid02:
  CYC(b_+225, b_+228); A = mem_rd(gb, wActiveRoom);
  CYC(b_+228, b_+230); alu_cp(gb, 0x25); // <ROOM_AGES_025
  if (F & FZ) { CYCT(b_+230, b_+232); goto inBridgeRoom; } // jr z
  CYC(b_+230, b_+232);
  CYC(b_+232, b_+233); A = mem_rd(gb, DE);
  CYC(b_+233, b_+235); A = alu_swap(gb, A);
  CYC(b_+235, b_+237); alu_and(gb, 0x0f);
  CYC(b_+237, b_+240); SET_HL(wAnimalCompanion);
  CYC(b_+240, b_+241); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+241, b_+243); goto delete_; } // jr nz
  CYC(b_+241, b_+243);
  CYC(b_+243, b_+244); A = mem_rd(gb, DE);
  CYC(b_+244, b_+246); alu_and(gb, 0x0f);
  CYC(b_+246, b_+249); SET_HL(wTmpcfc0_carpenterSearch_carpentersFound);
  CALL_C(b_+249, checkFlag_hook, SYM(checkFlag), b_+252);
  if (!(F & FZ)) { CYCT(b_+252, b_+254); goto delete2; } // jr nz
  CYC(b_+252, b_+254);
  CYC(b_+254, b_+256); goto checkDoBridgeBuildingCutscene; // jr (unconditional)

inBridgeRoom:
  CYC(b_+256, b_+257); A = mem_rd(gb, DE);
  CYC(b_+257, b_+259); alu_and(gb, 0x0f);
  CYC(b_+259, b_+262); SET_HL(wTmpcfc0_carpenterSearch_carpentersFound);
  CALL_C(b_+262, checkFlag_hook, SYM(checkFlag), b_+265);
  CYC(b_+265, b_+267); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+267, b_+268); mem_wr(gb, DE, A);
  if (F & FZ) { CYCT(b_+268, b_+270); goto delete2; } // jr z
  CYC(b_+268, b_+270);
  CYC(b_+270, b_+271); A = mem_rd(gb, HL);
  CYC(b_+271, b_+273); alu_cp(gb, 0x1c);
  if (!(F & FZ)) { CYCT(b_+273, b_+275); goto checkDoBridgeBuildingCutscene; } // jr nz
  CYC(b_+273, b_+275);
  CYC(b_+275, b_+277); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+277, b_+278); A = mem_rd(gb, DE);
  CYC(b_+278, b_+280); alu_add(gb, 0x04);
  CYC(b_+280, b_+281); mem_wr(gb, DE, A);
  CYC(b_+281, b_+283); goto checkDoBridgeBuildingCutscene; // jr (unconditional)

initSubid00:
  CYC(b_+283, b_+285); A = 0x03;
  CYC(b_+285, b_+287); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+287, b_+288); mem_wr(gb, DE, A);
  CYC(b_+288, b_+291); A = mem_rd(gb, wTmpcfc0_carpenterSearch_carpentersFound);
  CYC(b_+291, b_+293); alu_cp(gb, 0x1c);
  if (!(F & FZ)) { CYCT(b_+293, b_+295); goto checkDoBridgeBuildingCutscene; } // jr nz
  CYC(b_+293, b_+295);
  CYC(b_+295, b_+297); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+297, b_+299); A = 0x05;
  CYC(b_+299, b_+300); mem_wr(gb, DE, A);
  CYC(b_+300, b_+302); A = 0x58;
  CYC(b_+302, b_+304); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+304, b_+305); mem_wr(gb, DE, A);

checkDoBridgeBuildingCutscene:
  CYC(b_+305, b_+307); A = 0x25;
  CALL_C(b_+307, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+310);
  if (!(F & FZ)) { CYCT(b_+310, b_+312); goto delete2; } // jr nz
  CYC(b_+310, b_+312);

// @initSubid09 (falls into from @checkDoBridgeBuildingCutscene above)
  CALL_C(b_+312, objectMarkSolidPosition_hook, SYM(objectMarkSolidPosition), b_+315);
  CYC(b_+315, b_+317); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+317, b_+318); A = mem_rd(gb, DE);
  CYC(b_+318, b_+320); alu_and(gb, 0x0f);
  CYC(b_+320, b_+323); SET_HL(b_+501); // @animationsForBridgeBuildCutsceneStart
  CYC(b_+323, b_+324); interactionCode9a_add_a_to_hl(gb); // rst $10
  CYC(b_+324, b_+325); A = mem_rd(gb, HL);
  CALL_C(b_+325, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+328);
  CYC(b_+328, b_+330); A = 0x06;
  CALL_C(b_+330, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+333);
  CALL_C(b_+333, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+336);
  CYC(b_+336, b_+339); goto loadScript; // jp

state1:
  CYC(b_+339, b_+341); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+341, b_+342); A = mem_rd(gb, DE);
  CYC(b_+342, b_+344); alu_and(gb, 0x0f);
  CYC(b_+344, b_+345); push_effect(gb, b_+345);
  do { uint16_t jt_ = (interactionCode9a_jump_table(gb));
    if (jt_ == b_+365) { goto runSubid; }
    else if (jt_ == b_+380) { goto runSubid01; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

runSubid:
  CALL_C(b_+365, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+368);
  CYC(b_+368, b_+370); C = 0x40;
  CALL_C(b_+370, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+373);
  CALL_C(b_+373, interactionRunScript_hook, SYM(interactionRunScript), b_+376);
  if (!(F & FC)) { CYCT(b_+376, b_+377); ret_effect(gb); return; } // ret nc

delete2:
  CYC(b_+377, b_+380); interactionDelete_hook(gb); return; // jp

runSubid01:
  CYC(b_+380, b_+383); A = mem_rd(gb, wTmpcfc0_carpenterSearch_cfd0);
  CYC(b_+383, b_+385); alu_cp(gb, 0x0b);
  if (!(F & FZ)) { CYCT(b_+385, b_+386); ret_effect(gb); return; } // ret nz
  CYC(b_+385, b_+386);
  CYC(b_+386, b_+388); A = 0x3a;
  CYC(b_+388, b_+390); C = 0x55;
  CALL_C(b_+390, setTile_hook, SYM(setTile), b_+393);
  CYC(b_+393, b_+395); goto delete2; // jr

state2:
  CALL_C(b_+395, interactionAnimate_hook, SYM(interactionAnimate), b_+398);
  CYC(b_+398, b_+399); H = D;
  CYC(b_+399, b_+401); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+401, b_+403); mem_wr(gb, HL, 0x18); // ANGLE_LEFT
  CYC(b_+403, b_+405); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+405, b_+407); mem_wr(gb, HL, 0x28); // SPEED_100
  CALL_C(b_+407, objectApplySpeed_hook, SYM(objectApplySpeed), b_+410);
  CALL_C(b_+410, objectCheckWithinScreenBoundary_hook, SYM(objectCheckWithinScreenBoundary), b_+413);
  if (!(F & FC)) { CYCT(b_+413, b_+415); goto leftScreen; } // jr nc
  CYC(b_+413, b_+415);
  CYC(b_+415, b_+417); C = 0x10;
  CALL_C(b_+417, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+420);
  if (!(F & FZ)) { CYCT(b_+420, b_+421); ret_effect(gb); return; } // ret nz
  CYC(b_+420, b_+421);
  CYC(b_+421, b_+424); SET_BC(0xfe00); // -$200
  CALL_C(b_+424, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+427);
  CYC(b_+427, b_+429); A = 0x53; // SND_JUMP
  CYC(b_+429, b_+432); playSound_b00_hook(gb); return; // jp

leftScreen:
  CYC(b_+432, b_+434); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+434, b_+435); A = mem_rd(gb, DE);
  CYC(b_+435, b_+437); alu_and(gb, 0x0f);
  CYC(b_+437, b_+440); SET_HL(wTmpcfc0_carpenterSearch_carpentersFound);
  CALL_C(b_+440, setFlag_hook, SYM(setFlag), b_+443);
  CYC(b_+443, b_+444); A = mem_rd(gb, HL);
  CYC(b_+444, b_+446); alu_cp(gb, 0x1c);
  CYC(b_+446, b_+449); SET_HL(b_+461); // @warpDest
  if (F & FZ) { CYCT(b_+449, b_+452); setWarpDestVariables_hook(gb); return; } // jp z
  CYC(b_+449, b_+452);
  CYC(b_+452, b_+453); alu_xor(gb, A);
  CYC(b_+453, b_+456); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+456, b_+459); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+459, b_+461); goto delete2; // jr

loadScript:
  CYC(b_+466, b_+468); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+468, b_+469); A = mem_rd(gb, DE);
  CYC(b_+469, b_+471); alu_and(gb, 0x0f);
  CYC(b_+471, b_+474); SET_HL(b_+481); // @scriptTable
  CYC(b_+474, b_+475); interactionCode9a_add_double_index_from_rst(gb, b_+475);
  CYC(b_+475, b_+476); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+476, b_+477); H = mem_rd(gb, HL);
  CYC(b_+477, b_+478); L = A;
  CYC(b_+478, b_+481); interactionSetScript_hook(gb); return; // jp

  // @warpDest (0b:52a4): m_HardcodedWarpA ROOM_AGES_025, $00, $48, $03 -- pure ROM data referenced
  // only via SET_HL(0x52a4) above.
  // @scriptTable (0b:52b8) / @animationsForBridgeBuildCutsceneStart (0b:52cc): pure ROM data
  // referenced only via SET_HL(0x52b8) / SET_HL(0x52cc) above.
}
