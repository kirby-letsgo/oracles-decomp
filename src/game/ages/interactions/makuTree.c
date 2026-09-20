#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x66a1, 0x66a3); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x66a3, 0x66a4); A = mem_rd(gb, DE);
  {
    CYC(0x66a4, 0x66a5); push_effect(gb, 0x66a5);
    uint16_t target = makuTree_jump_table(gb);
    if (target == 0x66c4) goto subid01;
    if (target == 0x6701) goto subid03;
    if (target == 0x6721) goto subid04;
    if (target == 0x672b) goto subid06;
  }

  // interactionCode87@subid00
  CALL_C(0x66b3, checkInteractionState_hook, 0x23fe, 0x66b6);
  if (!(F & FZ)) { CYCT(0x66b6, 0x66b8); goto runScriptAndAnimate; } // jr nz
  CYC(0x66b6, 0x66b8);
  CYC(0x66b8, 0x66b9); alu_xor(gb, A);
  CYC(0x66b9, 0x66bb); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x66bb, 0x66bc); mem_wr(gb, DE, A);
  CYC(0x66bc, 0x66bf); push_effect(gb, 0x66bf); goto initSubid00; // call
afterInitSubid00:
  CYC(0x66bf, 0x66c2); push_effect(gb, 0x66c2); goto initializeMakuTree; // call
afterSpawnFlower_subid00:
  CYC(0x66c2, 0x66c4); goto runScriptAndAnimate; // jr

subid01:
  CALL_C(0x66c4, checkInteractionState_hook, 0x23fe, 0x66c7);
  if (!(F & FZ)) { CYCT(0x66c7, 0x66c9); goto runScriptAndAnimate; } // jr nz
  CYC(0x66c7, 0x66c9);
  CYC(0x66c9, 0x66cc); push_effect(gb, 0x66cc); goto initializeMakuTree; // call

afterSpawnFlower_subid0102:
  CALL_C(0x66cc, interactionRunScript_hook, 0x2552, 0x66cf);
  CYC(0x66cf, 0x66d1); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x66d1, 0x66d2); A = mem_rd(gb, DE);
  CYC(0x66d2, 0x66d3); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x66d3, 0x66d5); goto runScriptAndAnimate; } // jr nz
  CYC(0x66d3, 0x66d5);

  // Subid 1 only: make Link move right/up to approach the maku tree, starting the
  // "maku tree disappearance" cutscene
  CYC(0x66d5, 0x66d7); A = 0x8f; // PALH_8f
  CALL_C(0x66d7, loadPaletteHeader_hook, 0x050b, 0x66da);
  CYC(0x66da, 0x66dd); SET_HL(0x66ea); // @simulatedInput
  CYC(0x66dd, 0x66df); A = 0x0a; // bank of @simulatedInput
  CYC(0x66df, 0x66e0); push_effect(gb, DE);
  CALL_C(0x66e0, setSimulatedInputAddress_hook, 0x2a1d, 0x66e3);
  CYC(0x66e3, 0x66e4); SET_DE(pop_effect(gb));
  CYC(0x66e4, 0x66e5); alu_xor(gb, A);
  CYC(0x66e5, 0x66e8); W8(w1Link_direction) = A;
  CYC(0x66e8, 0x66ea); goto runScriptAndAnimate; // jr

runScriptAndAnimate:
  CALL_C(0x66fb, interactionRunScript_hook, 0x2552, 0x66fe);
  CYC(0x66fe, 0x6701); interactionAnimate_hook(gb); return; // jp

subid03:
  CALL_C(0x6701, checkInteractionState_hook, 0x23fe, 0x6704);
  if (!(F & FZ)) { CYCT(0x6704, 0x6706); goto runScriptAndAnimate; } // jr nz
  CYC(0x6704, 0x6706);
  CYC(0x6706, 0x6708); B = 0x01;
  CYC(0x6708, 0x670b); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(0x670b, 0x670d); alu_cp(gb, 0x03);
  if (F & FZ) { CYCT(0x670d, 0x670f); goto subid03Continue; } // jr z
  CYC(0x670d, 0x670f);
  CALL_C(0x670f, interactionLoadExtraGraphics_hook, 0x2781, 0x6712);
  CYC(0x6712, 0x6714); B = 0x00;

subid03Continue:
  CYC(0x6714, 0x6715); A = B;
  CALL_C(0x6715, interactionSetAnimation_hook, 0x262e, 0x6718);
  CALL_C(0x6718, interactionInitGraphics_hook, 0x15fb, 0x671b);
  CYC(0x671b, 0x671e); push_effect(gb, 0x671e); goto loadScript; // call
  // interactionCode87@subid03 resumes at 0x6721 via loadScript's own exit check

subid04:
  CALL_C(0x6721, checkInteractionState_hook, 0x23fe, 0x6724);
  if (!(F & FZ)) { CYCT(0x6724, 0x6726); goto runScriptAndAnimate; } // jr nz
  CYC(0x6724, 0x6726);
  CYC(0x6726, 0x6729); push_effect(gb, 0x6729); goto initializeMakuTree; // call
afterSpawnFlower_subid0405:
  CYC(0x6729, 0x672b); goto runScriptAndAnimate; // jr

subid06:
  CALL_C(0x672b, checkInteractionState_hook, 0x23fe, 0x672e);
  if (!(F & FZ)) { CYCT(0x672e, 0x6730); goto runScriptAndAnimate; } // jr nz
  CYC(0x672e, 0x6730);
  CYC(0x6730, 0x6732); A = 0x13; // GLOBALFLAG_SAW_TWINROVA_BEFORE_ENDGAME
  CALL_C(0x6732, checkGlobalFlag_hook, 0x31f3, 0x6735);
  if (!(F & FZ)) { CYCT(0x6735, 0x6738); goto initializeMakuTree; } // jp nz
  CYC(0x6735, 0x6738);
  CYC(0x6738, 0x673b); SET_HL(w1Link_direction);
  CYC(0x673b, 0x673d); mem_wr(gb, HL, 0x00);
  CALL_C(0x673d, setLinkForceStateToState08_hook, 0x2aad, 0x6740);
  CYC(0x6740, 0x6743); push_effect(gb, 0x6743); goto initGraphicsAndIncState; // call
afterInitGraphicsAndIncState_subid06:
  CYC(0x6743, 0x6746); push_effect(gb, 0x6746); goto setVisibleAndSpawnFlower; // call

afterSpawnFlower_subid06:
  CYC(0x6746, 0x6748); B = 0x00;
  CYC(0x6748, 0x674b); SET_HL(0x7794); // mainScripts.makuTree_subid06Script_part1
  CYC(0x674b, 0x674d); A = 0x35; // GLOBALFLAG_GOT_MAKU_SEED
  CYC(0x674d, 0x674e); push_effect(gb, HL);
  CALL_C(0x674e, checkGlobalFlag_hook, 0x31f3, 0x6751);
  CYC(0x6751, 0x6752); SET_HL(pop_effect(gb));
  if (F & FZ) { CYCT(0x6752, 0x6754); goto subid06SetScript; } // jr z
  CYC(0x6752, 0x6754);
  CYC(0x6754, 0x6756); B = 0x04;
  CYC(0x6756, 0x6759); SET_HL(0x7798); // mainScripts.makuTree_subid06Script_part2

subid06SetScript:
  CALL_C(0x6759, interactionSetScript_hook, 0x2544, 0x675c);
  CYC(0x675c, 0x675e); A = 0x05; // >TX_0500
  CALL_C(0x675e, interactionSetHighTextIndex_hook, 0x253b, 0x6761);
  CYC(0x6761, 0x6762); A = B;
  CALL_C(0x6762, interactionSetAnimation_hook, 0x262e, 0x6765);
  CYC(0x6765, 0x6768); goto runScriptAndAnimate; // jp

initSubid00:
  CYC(0x6768, 0x676b); A = W8(wMakuTreeState);
  {
    CYC(0x676b, 0x676c); push_effect(gb, 0x676c);
    uint16_t target = makuTree_jump_table(gb);
    if (target == 0x6799) goto state02;
    if (target == 0x679d) goto state03;
    if (target == 0x67a2) goto state04;
    if (target == 0x67a7) goto state05;
    if (target == 0x67ac) goto state06;
    if (target == 0x67b1) goto state07;
    if (target == 0x67b6) goto state08;
    if (target == 0x67bb) goto state09;
    if (target == 0x67c0) goto state0a;
    if (target == 0x67c5) goto state0b;
    if (target == 0x67ca) goto state0c;
    if (target == 0x67cf) goto state0d;
    if (target == 0x67d4) goto state0e;
    if (target == 0x67d8) goto state0f;
    if (target == 0x67dd) goto state10;
    if (target == 0x67ec) goto state01;
  }

  // interactionCode87@state00
  CYC(0x678e, 0x6790); A = 0x0c; // GLOBALFLAG_0c
  CALL_C(0x6790, checkGlobalFlag_hook, 0x31f3, 0x6793);
  if (!(F & FZ)) { CYCT(0x6793, 0x6795); goto initSubid00Ret; } // jr nz
  CYC(0x6793, 0x6795);
  CYC(0x6795, 0x6797); A = 0x01;
  CYC(0x6797, 0x6799); goto runSubidCode; // jr

state02:
  CYC(0x6799, 0x679b); A = 0x02;
  CYC(0x679b, 0x679d); goto runSubidCode; // jr

state03:
  CYC(0x679d, 0x67a0); SET_BC(0x0200); // ldbc $02,<TX_0500
  CYC(0x67a0, 0x67a2); goto runSubid0ScriptMode; // jr

state04:
  CYC(0x67a2, 0x67a5); SET_BC(0x0003); // ldbc $00,<TX_0503
  CYC(0x67a5, 0x67a7); goto runSubid0ScriptMode; // jr

state05:
  CYC(0x67a7, 0x67aa); SET_BC(0x0005); // ldbc $00,<TX_0505
  CYC(0x67aa, 0x67ac); goto runSubid0ScriptMode; // jr

state06:
  CYC(0x67ac, 0x67af); SET_BC(0x0007); // ldbc $00,<TX_0507
  CYC(0x67af, 0x67b1); goto runSubid0ScriptMode; // jr

state07:
  CYC(0x67b1, 0x67b4); SET_BC(0x0409); // ldbc $04,<TX_0509
  CYC(0x67b4, 0x67b6); goto runSubid0ScriptMode; // jr

state08:
  CYC(0x67b6, 0x67b9); SET_BC(0x040b); // ldbc $04,<TX_050b
  CYC(0x67b9, 0x67bb); goto runSubid0ScriptMode; // jr

state09:
  CYC(0x67bb, 0x67be); SET_BC(0x020d); // ldbc $02,<TX_050d
  CYC(0x67be, 0x67c0); goto runSubid0ScriptMode; // jr

state0a:
  CYC(0x67c0, 0x67c3); SET_BC(0x0010); // ldbc $00,<TX_0510
  CYC(0x67c3, 0x67c5); goto runSubid0ScriptMode; // jr

state0b:
  CYC(0x67c5, 0x67c8); SET_BC(0x0512); // ldbc $05,<TX_0512
  CYC(0x67c8, 0x67ca); goto runSubid0ScriptMode; // jr

state0c:
  CYC(0x67ca, 0x67cd); SET_BC(0x0414); // ldbc $04,<TX_0514
  CYC(0x67cd, 0x67cf); goto runSubid0ScriptMode; // jr

state0d:
  CYC(0x67cf, 0x67d2); SET_BC(0x0016); // ldbc $00,<TX_0516
  CYC(0x67d2, 0x67d4); goto runSubid0ScriptMode; // jr

state0e:
  CYC(0x67d4, 0x67d6); A = 0x06;
  CYC(0x67d6, 0x67d8); goto runSubidCode; // jr

state0f:
  CYC(0x67d8, 0x67db); SET_BC(0x0018); // ldbc $00,<TX_0518
  CYC(0x67db, 0x67dd); goto runSubid0ScriptMode; // jr

state10:
  CALL_C(0x67dd, checkIsLinkedGame_hook, 0x1992, 0x67e0);
  if (F & FZ) { CYCT(0x67e0, 0x67e2); goto state10Linked; } // jr z
  CYC(0x67e0, 0x67e2);
  CYC(0x67e2, 0x67e5); SET_BC(0x001a); // ldbc $00,<TX_051a
  CYC(0x67e5, 0x67e7); goto runSubid0ScriptMode; // jr

state10Linked:
  CYC(0x67e7, 0x67ea); SET_BC(0x011c); // ldbc $01,<TX_051c
  CYC(0x67ea, 0x67ec); goto runSubid0ScriptMode; // jr

state01:
  CYC(0x67ec, 0x67ed); SET_AF(pop_effect(gb));
  CYC(0x67ed, 0x67f0); interactionDelete_hook(gb); return; // jp

runSubidCode:
  CYC(0x67f0, 0x67f2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x67f2, 0x67f3); mem_wr(gb, DE, A);
  CYC(0x67f3, 0x67f4); SET_AF(pop_effect(gb));
  CYC(0x67f4, 0x67f7); interactionCode87_hook(gb); return; // jp

runSubid0ScriptMode:
  CYC(0x67f7, 0x67f8); H = D;
  CYC(0x67f8, 0x67fa); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x67fa, 0x67fb); mem_wr(gb, HL, B);
  CYC(0x67fb, 0x67fc); L = alu_inc8(gb, L);
  CYC(0x67fc, 0x67fd); mem_wr(gb, HL, C);

initSubid00Ret:
  RET(0x67fd);
  if (gb->pc == 0x66bf && gb->sp == sp0_) goto afterInitSubid00;
  return; // ret

initializeMakuTree:
  CYC(0x67fe, 0x6801); push_effect(gb, 0x6801); goto initGraphicsAndLoadScript; // call

setVisibleAndSpawnFlower:
  CALL_C(0x6801, objectSetVisible83_hook, 0x1e72, 0x6804);
  CALL_C(0x6804, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6807);
  CYC(0x6807, 0x680a); goto spawnMakuFlower; // jp

initGraphicsAndIncState:
  CYC(0x680a, 0x680d); push_effect(gb, 0x680d); goto initGraphics; // call

afterInitGraphics_viaIncState:
  CYC(0x680d, 0x6810); interactionIncState_hook(gb);
  if (gb->pc == 0x6743 && gb->sp == sp0_) goto afterInitGraphicsAndIncState_subid06;
  return; // jp

initGraphicsAndLoadScript:
  CYC(0x6810, 0x6813); push_effect(gb, 0x6813); goto initGraphics; // call

afterInitGraphics_viaLoadScript:
  CYC(0x6813, 0x6815); goto loadScript; // jr

// interactionCode87@initGraphics is called two ways: from @initGraphicsAndIncState's return
// address 0x680d, pushed at sp0_-2 since that caller was itself entered via subid06's own
// push_effect; and from @initGraphicsAndLoadScript's return address 0x6813, pushed at sp0_-4
// since that caller was entered via @initializeMakuTree's push, itself entered via subid00,
// subid01, or subid04's own outer push. Each edge's sp check must match its own nesting depth,
// not sp0_ uniformly.
initGraphics:
  CALL_C(0x6815, interactionLoadExtraGraphics_hook, 0x2781, 0x6818);
  CYC(0x6818, 0x681b); interactionInitGraphics_hook(gb);
  if (gb->pc == 0x680d && gb->sp == (uint16_t)(sp0_ - 2)) goto afterInitGraphics_viaIncState;
  if (gb->pc == 0x6813 && gb->sp == (uint16_t)(sp0_ - 4)) goto afterInitGraphics_viaLoadScript;
  return; // jp

loadScript:
  CYC(0x681b, 0x681d); A = 0x05; // >TX_0500
  CALL_C(0x681d, interactionSetHighTextIndex_hook, 0x253b, 0x6820);
  CYC(0x6820, 0x6822); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6822, 0x6823); A = mem_rd(gb, DE);
  CYC(0x6823, 0x6826); SET_HL(0x6847); // @scriptTable
  CYC(0x6826, 0x6827); makuTree_add_double_index(gb, 0x6827);
  CYC(0x6827, 0x6828); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6828, 0x6829); H = mem_rd(gb, HL);
  CYC(0x6829, 0x682a); L = A;
  CALL_C(0x682a, interactionSetScript_hook, 0x2544, 0x682d);
  CYC(0x682d, 0x6830); interactionIncState_hook(gb);
  if (gb->pc == 0x671e && gb->sp == sp0_) { CYC(0x671e, 0x6721); goto setVisibleAndSpawnFlower; }
  // 0x6801 is pushed by @initializeMakuTree at sp0_-2, since one outer push from subid00, subid01,
  // or subid04 is already outstanding -- see @initGraphics's comment for the full nesting chain.
  if (gb->pc == 0x6801 && gb->sp == (uint16_t)(sp0_ - 2)) goto setVisibleAndSpawnFlower;
  return; // jp

spawnMakuFlower:
  CALL_C(0x6830, getFreeInteractionSlot_hook, 0x3aef, 0x6833);
  if (!(F & FZ)) {
    RET_TAKEN(0x6833);
    if (gb->pc == 0x66c2 && gb->sp == sp0_) goto afterSpawnFlower_subid00;
    if (gb->pc == 0x66cc && gb->sp == sp0_) goto afterSpawnFlower_subid0102;
    if (gb->pc == 0x6729 && gb->sp == sp0_) goto afterSpawnFlower_subid0405;
    if (gb->pc == 0x6746 && gb->sp == sp0_) goto afterSpawnFlower_subid06;
    return;
  } // ret nz
  CYC(0x6833, 0x6834);
  CYC(0x6834, 0x6836); mem_wr(gb, HL, 0x86); // INTERAC_MAKU_FLOWER
  CYC(0x6836, 0x6838); L = INTERACTION_BASE + OBJ_RELATED2; // Interaction.relatedObj2
  CYC(0x6838, 0x683a); A = INTERACTION_BASE; // Interaction.start
  CYC(0x683a, 0x683b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x683b, 0x683c); mem_wr(gb, HL, D);
  CYC(0x683c, 0x683e); E = INTERACTION_BASE + OBJ_RELATED1; // Interaction.relatedObj1
  CYC(0x683e, 0x6840); A = INTERACTION_BASE; // Interaction.start
  CYC(0x6840, 0x6841); mem_wr(gb, DE, A);
  CYC(0x6841, 0x6842); E = alu_inc8(gb, E);
  CYC(0x6842, 0x6843); A = H;
  CYC(0x6843, 0x6844); mem_wr(gb, DE, A);
  CYC(0x6844, 0x6847); objectCopyPosition_hook(gb);
  if (gb->pc == 0x66c2 && gb->sp == sp0_) goto afterSpawnFlower_subid00;
  if (gb->pc == 0x66cc && gb->sp == sp0_) goto afterSpawnFlower_subid0102;
  if (gb->pc == 0x6729 && gb->sp == sp0_) goto afterSpawnFlower_subid0405;
  if (gb->pc == 0x6746 && gb->sp == sp0_) goto afterSpawnFlower_subid06;
  return; // jp
}
