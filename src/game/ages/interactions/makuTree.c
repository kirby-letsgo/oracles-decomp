#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t makuTree_jump_table(GB *gb) {
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

static void makuTree_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_MAKU_TREE
void interactionCode87_hook(GB *gb) {
  BASE(interactionCode87);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = makuTree_jump_table(gb);
    if (target == b_+35) goto subid01;
    if (target == b_+96) goto subid03;
    if (target == b_+128) goto subid04;
    if (target == b_+138) goto subid06;
  }

  // interactionCode87@subid00
  CALL_C(b_+18, checkInteractionState_hook, SYM(checkInteractionState), b_+21);
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto runScriptAndAnimate; } // jr nz
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CYC(b_+24, b_+26); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+30); push_effect(gb, b_+30); goto initSubid00; // call
afterInitSubid00:
  CYC(b_+30, b_+33); push_effect(gb, b_+33); goto initializeMakuTree; // call
afterSpawnFlower_subid00:
  CYC(b_+33, b_+35); goto runScriptAndAnimate; // jr

subid01:
  CALL_C(b_+35, checkInteractionState_hook, SYM(checkInteractionState), b_+38);
  if (!(F & FZ)) { CYCT(b_+38, b_+40); goto runScriptAndAnimate; } // jr nz
  CYC(b_+38, b_+40);
  CYC(b_+40, b_+43); push_effect(gb, b_+43); goto initializeMakuTree; // call

afterSpawnFlower_subid0102:
  CALL_C(b_+43, interactionRunScript_hook, SYM(interactionRunScript), b_+46);
  CYC(b_+46, b_+48); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+48, b_+49); A = mem_rd(gb, DE);
  CYC(b_+49, b_+50); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+50, b_+52); goto runScriptAndAnimate; } // jr nz
  CYC(b_+50, b_+52);

  // Subid 1 only: make Link move right/up to approach the maku tree, starting the
  // "maku tree disappearance" cutscene
  CYC(b_+52, b_+54); A = 0x8f; // PALH_8f
  CALL_C(b_+54, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+57);
  CYC(b_+57, b_+60); SET_HL(b_+73); // @simulatedInput
  CYC(b_+60, b_+62); A = 0x0a; // bank of @simulatedInput
  CYC(b_+62, b_+63); push_effect(gb, DE);
  CALL_C(b_+63, setSimulatedInputAddress_hook, SYM(setSimulatedInputAddress), b_+66);
  CYC(b_+66, b_+67); SET_DE(pop_effect(gb));
  CYC(b_+67, b_+68); alu_xor(gb, A);
  CYC(b_+68, b_+71); W8(w1Link_direction) = A;
  CYC(b_+71, b_+73); goto runScriptAndAnimate; // jr

runScriptAndAnimate:
  CALL_C(b_+90, interactionRunScript_hook, SYM(interactionRunScript), b_+93);
  CYC(b_+93, b_+96); interactionAnimate_hook(gb); return; // jp

subid03:
  CALL_C(b_+96, checkInteractionState_hook, SYM(checkInteractionState), b_+99);
  if (!(F & FZ)) { CYCT(b_+99, b_+101); goto runScriptAndAnimate; } // jr nz
  CYC(b_+99, b_+101);
  CYC(b_+101, b_+103); B = 0x01;
  CYC(b_+103, b_+106); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+106, b_+108); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(b_+108, b_+110); goto subid03Continue; } // jr z
  CYC(b_+108, b_+110);
  CALL_C(b_+110, interactionLoadExtraGraphics_hook, SYM(interactionLoadExtraGraphics), b_+113);
  CYC(b_+113, b_+115); B = 0x00;

subid03Continue:
  CYC(b_+115, b_+116); A = B;
  CALL_C(b_+116, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+119);
  CALL_C(b_+119, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+122);
  CYC(b_+122, b_+125); push_effect(gb, b_+125); goto loadScript; // call
  // interactionCode87@subid03 resumes at 0x6721 via loadScript's own exit check

subid04:
  CALL_C(b_+128, checkInteractionState_hook, SYM(checkInteractionState), b_+131);
  if (!(F & FZ)) { CYCT(b_+131, b_+133); goto runScriptAndAnimate; } // jr nz
  CYC(b_+131, b_+133);
  CYC(b_+133, b_+136); push_effect(gb, b_+136); goto initializeMakuTree; // call
afterSpawnFlower_subid0405:
  CYC(b_+136, b_+138); goto runScriptAndAnimate; // jr

subid06:
  CALL_C(b_+138, checkInteractionState_hook, SYM(checkInteractionState), b_+141);
  if (!(F & FZ)) { CYCT(b_+141, b_+143); goto runScriptAndAnimate; } // jr nz
  CYC(b_+141, b_+143);
  CYC(b_+143, b_+145); A = 0x13; // GLOBALFLAG_SAW_TWINROVA_BEFORE_ENDGAME
  CALL_C(b_+145, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+148);
  if (!(F & FZ)) { CYCT(b_+148, b_+151); goto initializeMakuTree; } // jp nz
  CYC(b_+148, b_+151);
  CYC(b_+151, b_+154); SET_HL(w1Link_direction);
  CYC(b_+154, b_+156); mem_wr(gb, HL, 0x00);
  CALL_C(b_+156, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+159);
  CYC(b_+159, b_+162); push_effect(gb, b_+162); goto initGraphicsAndIncState; // call
afterInitGraphicsAndIncState_subid06:
  CYC(b_+162, b_+165); push_effect(gb, b_+165); goto setVisibleAndSpawnFlower; // call

afterSpawnFlower_subid06:
  CYC(b_+165, b_+167); B = 0x00;
  CYC(b_+167, b_+170); SET_HL((SYM(presetInteractionAnglesAndCounters__data4) + 7)); // mainScripts.makuTree_subid06Script_part1
  CYC(b_+170, b_+172); A = 0x35; // GLOBALFLAG_GOT_MAKU_SEED
  CYC(b_+172, b_+173); push_effect(gb, HL);
  CALL_C(b_+173, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+176);
  CYC(b_+176, b_+177); SET_HL(pop_effect(gb));
  if (F & FZ) { CYCT(b_+177, b_+179); goto subid06SetScript; } // jr z
  CYC(b_+177, b_+179);
  CYC(b_+179, b_+181); B = 0x04;
  CYC(b_+181, b_+184); SET_HL((SYM(presetInteractionAnglesAndCounters__data4) + 11)); // mainScripts.makuTree_subid06Script_part2

subid06SetScript:
  CALL_C(b_+184, interactionSetScript_hook, SYM(interactionSetScript), b_+187);
  CYC(b_+187, b_+189); A = 0x05; // >TX_0500
  CALL_C(b_+189, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+192);
  CYC(b_+192, b_+193); A = B;
  CALL_C(b_+193, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+196);
  CYC(b_+196, b_+199); goto runScriptAndAnimate; // jp

initSubid00:
  CYC(b_+199, b_+202); A = W8(wMakuTreeState);
  {
    CYC(b_+202, b_+203); push_effect(gb, b_+203);
    uint16_t target = makuTree_jump_table(gb);
    if (target == b_+248) goto state02;
    if (target == b_+252) goto state03;
    if (target == b_+257) goto state04;
    if (target == b_+262) goto state05;
    if (target == b_+267) goto state06;
    if (target == b_+272) goto state07;
    if (target == b_+277) goto state08;
    if (target == b_+282) goto state09;
    if (target == b_+287) goto state0a;
    if (target == b_+292) goto state0b;
    if (target == b_+297) goto state0c;
    if (target == b_+302) goto state0d;
    if (target == b_+307) goto state0e;
    if (target == b_+311) goto state0f;
    if (target == b_+316) goto state10;
    if (target == b_+331) goto state01;
  }

  // interactionCode87@state00
  CYC(b_+237, b_+239); A = 0x0c; // GLOBALFLAG_0c
  CALL_C(b_+239, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+242);
  if (!(F & FZ)) { CYCT(b_+242, b_+244); goto initSubid00Ret; } // jr nz
  CYC(b_+242, b_+244);
  CYC(b_+244, b_+246); A = 0x01;
  CYC(b_+246, b_+248); goto runSubidCode; // jr

state02:
  CYC(b_+248, b_+250); A = 0x02;
  CYC(b_+250, b_+252); goto runSubidCode; // jr

state03:
  CYC(b_+252, b_+255); SET_BC(0x0200); // ldbc $02,<TX_0500
  CYC(b_+255, b_+257); goto runSubid0ScriptMode; // jr

state04:
  CYC(b_+257, b_+260); SET_BC(0x0003); // ldbc $00,<TX_0503
  CYC(b_+260, b_+262); goto runSubid0ScriptMode; // jr

state05:
  CYC(b_+262, b_+265); SET_BC(0x0005); // ldbc $00,<TX_0505
  CYC(b_+265, b_+267); goto runSubid0ScriptMode; // jr

state06:
  CYC(b_+267, b_+270); SET_BC(0x0007); // ldbc $00,<TX_0507
  CYC(b_+270, b_+272); goto runSubid0ScriptMode; // jr

state07:
  CYC(b_+272, b_+275); SET_BC(0x0409); // ldbc $04,<TX_0509
  CYC(b_+275, b_+277); goto runSubid0ScriptMode; // jr

state08:
  CYC(b_+277, b_+280); SET_BC(0x040b); // ldbc $04,<TX_050b
  CYC(b_+280, b_+282); goto runSubid0ScriptMode; // jr

state09:
  CYC(b_+282, b_+285); SET_BC(0x020d); // ldbc $02,<TX_050d
  CYC(b_+285, b_+287); goto runSubid0ScriptMode; // jr

state0a:
  CYC(b_+287, b_+290); SET_BC(0x0010); // ldbc $00,<TX_0510
  CYC(b_+290, b_+292); goto runSubid0ScriptMode; // jr

state0b:
  CYC(b_+292, b_+295); SET_BC(0x0512); // ldbc $05,<TX_0512
  CYC(b_+295, b_+297); goto runSubid0ScriptMode; // jr

state0c:
  CYC(b_+297, b_+300); SET_BC(0x0414); // ldbc $04,<TX_0514
  CYC(b_+300, b_+302); goto runSubid0ScriptMode; // jr

state0d:
  CYC(b_+302, b_+305); SET_BC(0x0016); // ldbc $00,<TX_0516
  CYC(b_+305, b_+307); goto runSubid0ScriptMode; // jr

state0e:
  CYC(b_+307, b_+309); A = 0x06;
  CYC(b_+309, b_+311); goto runSubidCode; // jr

state0f:
  CYC(b_+311, b_+314); SET_BC(0x0018); // ldbc $00,<TX_0518
  CYC(b_+314, b_+316); goto runSubid0ScriptMode; // jr

state10:
  CALL_C(b_+316, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+319);
  if (F & FZ) { CYCT(b_+319, b_+321); goto state10Linked; } // jr z
  CYC(b_+319, b_+321);
  CYC(b_+321, b_+324); SET_BC(0x001a); // ldbc $00,<TX_051a
  CYC(b_+324, b_+326); goto runSubid0ScriptMode; // jr

state10Linked:
  CYC(b_+326, b_+329); SET_BC(0x011c); // ldbc $01,<TX_051c
  CYC(b_+329, b_+331); goto runSubid0ScriptMode; // jr

state01:
  CYC(b_+331, b_+332); SET_AF(pop_effect(gb));
  CYC(b_+332, b_+335); interactionDelete_hook(gb); return; // jp

runSubidCode:
  CYC(b_+335, b_+337); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+337, b_+338); mem_wr(gb, DE, A);
  CYC(b_+338, b_+339); SET_AF(pop_effect(gb));
  CYC(b_+339, b_+342); interactionCode87_hook(gb); return; // jp

runSubid0ScriptMode:
  CYC(b_+342, b_+343); H = D;
  CYC(b_+343, b_+345); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+345, b_+346); mem_wr(gb, HL, B);
  CYC(b_+346, b_+347); L = alu_inc8(gb, L);
  CYC(b_+347, b_+348); mem_wr(gb, HL, C);

initSubid00Ret:
  RET(b_+348);
  if (gb->pc == b_+30 && gb->sp == sp0_) goto afterInitSubid00;
  return; // ret

initializeMakuTree:
  CYC(b_+349, b_+352); push_effect(gb, b_+352); goto initGraphicsAndLoadScript; // call

setVisibleAndSpawnFlower:
  CALL_C(b_+352, objectSetVisible83_hook, SYM(objectSetVisible83), b_+355);
  CALL_C(b_+355, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+358);
  CYC(b_+358, b_+361); goto spawnMakuFlower; // jp

initGraphicsAndIncState:
  CYC(b_+361, b_+364); push_effect(gb, b_+364); goto initGraphics; // call

afterInitGraphics_viaIncState:
  CYC(b_+364, b_+367); interactionIncState_hook(gb);
  if (gb->pc == b_+162 && gb->sp == sp0_) goto afterInitGraphicsAndIncState_subid06;
  return; // jp

initGraphicsAndLoadScript:
  CYC(b_+367, b_+370); push_effect(gb, b_+370); goto initGraphics; // call

afterInitGraphics_viaLoadScript:
  CYC(b_+370, b_+372); goto loadScript; // jr

// interactionCode87@initGraphics is called two ways: from @initGraphicsAndIncState's return
// address 0x680d, pushed at sp0_-2 since that caller was itself entered via subid06's own
// push_effect; and from @initGraphicsAndLoadScript's return address 0x6813, pushed at sp0_-4
// since that caller was entered via @initializeMakuTree's push, itself entered via subid00,
// subid01, or subid04's own outer push. Each edge's sp check must match its own nesting depth,
// not sp0_ uniformly.
initGraphics:
  CALL_C(b_+372, interactionLoadExtraGraphics_hook, SYM(interactionLoadExtraGraphics), b_+375);
  CYC(b_+375, b_+378); interactionInitGraphics_hook(gb);
  if (gb->pc == b_+364 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterInitGraphics_viaIncState;
  if (gb->pc == b_+370 && gb->sp == (uint16_t)(sp0_ - 4)) goto afterInitGraphics_viaLoadScript;
  return; // jp

loadScript:
  CYC(b_+378, b_+380); A = 0x05; // >TX_0500
  CALL_C(b_+380, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+383);
  CYC(b_+383, b_+385); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+385, b_+386); A = mem_rd(gb, DE);
  CYC(b_+386, b_+389); SET_HL(b_+422); // @scriptTable
  CYC(b_+389, b_+390); makuTree_add_double_index(gb, b_+390);
  CYC(b_+390, b_+391); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+391, b_+392); H = mem_rd(gb, HL);
  CYC(b_+392, b_+393); L = A;
  CALL_C(b_+393, interactionSetScript_hook, SYM(interactionSetScript), b_+396);
  CYC(b_+396, b_+399); interactionIncState_hook(gb);
  if (gb->pc == b_+125 && gb->sp == sp0_) { CYC(b_+125, b_+128); goto setVisibleAndSpawnFlower; }
  // 0x6801 is pushed by @initializeMakuTree at sp0_-2, since one outer push from subid00, subid01,
  // or subid04 is already outstanding -- see @initGraphics's comment for the full nesting chain.
  if (gb->pc == b_+352 && gb->sp == (uint16_t)(sp0_ - 2)) goto setVisibleAndSpawnFlower;
  return; // jp

spawnMakuFlower:
  CALL_C(b_+399, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+402);
  if (!(F & FZ)) {
    RET_TAKEN(b_+402);
    if (gb->pc == b_+33 && gb->sp == sp0_) goto afterSpawnFlower_subid00;
    if (gb->pc == b_+43 && gb->sp == sp0_) goto afterSpawnFlower_subid0102;
    if (gb->pc == b_+136 && gb->sp == sp0_) goto afterSpawnFlower_subid0405;
    if (gb->pc == b_+165 && gb->sp == sp0_) goto afterSpawnFlower_subid06;
    return;
  } // ret nz
  CYC(b_+402, b_+403);
  CYC(b_+403, b_+405); mem_wr(gb, HL, 0x86); // INTERAC_MAKU_FLOWER
  CYC(b_+405, b_+407); L = INTERACTION_BASE + OBJ_RELATED2; // Interaction.relatedObj2
  CYC(b_+407, b_+409); A = INTERACTION_BASE; // Interaction.start
  CYC(b_+409, b_+410); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+410, b_+411); mem_wr(gb, HL, D);
  CYC(b_+411, b_+413); E = INTERACTION_BASE + OBJ_RELATED1; // Interaction.relatedObj1
  CYC(b_+413, b_+415); A = INTERACTION_BASE; // Interaction.start
  CYC(b_+415, b_+416); mem_wr(gb, DE, A);
  CYC(b_+416, b_+417); E = alu_inc8(gb, E);
  CYC(b_+417, b_+418); A = H;
  CYC(b_+418, b_+419); mem_wr(gb, DE, A);
  CYC(b_+419, b_+422); objectCopyPosition_hook(gb);
  if (gb->pc == b_+33 && gb->sp == sp0_) goto afterSpawnFlower_subid00;
  if (gb->pc == b_+43 && gb->sp == sp0_) goto afterSpawnFlower_subid0102;
  if (gb->pc == b_+136 && gb->sp == sp0_) goto afterSpawnFlower_subid0405;
  if (gb->pc == b_+165 && gb->sp == sp0_) goto afterSpawnFlower_subid06;
  return; // jp
}
