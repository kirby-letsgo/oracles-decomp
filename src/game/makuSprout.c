#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t makuSprout_jump_table(GB *gb) {
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

static void makuSprout_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_MAKU_SPROUT
//
// interactionCode88@initGraphics 0x692b-0x6931 is dead code -- no call, jump, or fallthrough
// reaches it from anywhere in this routine, matching its own disasm comment marking it unused --
// so its bytes are omitted here.
//
// Nested manual call chain: subid0/subid1/subid2 each call @initializeMakuSprout at the top
// level, so initializeMakuSprout is entered with zero outstanding pushes. But initializeMakuSprout
// itself calls @loadScriptAndInitGraphics before its own call returns, so loadScriptAndInitGraphics
// is entered one level deeper -- one push already outstanding from initializeMakuSprout's own
// call -- and its resume check below must compare against sp0_-2, not bare sp0_.
void interactionCode88_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6855, 0x6857); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6857, 0x6858); A = mem_rd(gb, DE);
  {
    CYC(0x6858, 0x6859); push_effect(gb, 0x6859);
    uint16_t target = makuSprout_jump_table(gb);
    if (target == 0x687b) goto subid1;
    if (target == 0x6888) goto subid2;
  }

  // interactionCode88@subid0
  CALL_C(0x685f, checkInteractionState_hook, 0x23fe, 0x6862);
  if (!(F & FZ)) { CYCT(0x6862, 0x6864); goto subid0State1; } // jr nz
  CYC(0x6862, 0x6864);
  CYC(0x6864, 0x6866); A = 0x01;
  CYC(0x6866, 0x6868); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x6868, 0x6869); mem_wr(gb, DE, A);
  CYC(0x6869, 0x686c); push_effect(gb, 0x686c); goto initSubid0; // call
afterInitSubid0:
  CYC(0x686c, 0x686f); push_effect(gb, 0x686f); goto initializeMakuSprout; // call

subid0State1:
  CALL_C(0x686f, interactionAnimateAsNpc_hook, 0x26db, 0x6872);
  CYC(0x6872, 0x6874); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(0x6874, 0x6875); A = mem_rd(gb, DE);
  CYC(0x6875, 0x6877); alu_and(gb, 0x8f);
  CYC(0x6877, 0x6878); mem_wr(gb, DE, A);
  CYC(0x6878, 0x687b); interactionRunScript_hook(gb); return; // jp

subid1:
  CALL_C(0x687b, checkInteractionState_hook, 0x23fe, 0x687e);
  if (!(F & FZ)) { CYCT(0x687e, 0x6880); goto subid1State1; } // jr nz
  CYC(0x687e, 0x6880);
  CYC(0x6880, 0x6883); push_effect(gb, 0x6883); goto initializeMakuSprout; // call
afterInitializeMakuSprout_subid1:
  CALL_C(0x6883, interactionRunScript_hook, 0x2552, 0x6886);

subid1State1:
  CYC(0x6886, 0x6888); goto subid0State1; // jr

subid2:
  CALL_C(0x6888, checkInteractionState_hook, 0x23fe, 0x688b);
  if (!(F & FZ)) { CYCT(0x688b, 0x688d); goto subid2State1; } // jr nz
  CYC(0x688b, 0x688d);
  CYC(0x688d, 0x6890); push_effect(gb, 0x6890); goto initializeMakuSprout; // call
afterInitializeMakuSprout_subid2:
  CYC(0x6890, 0x6892); A = 0x01;
  CYC(0x6892, 0x6895); interactionSetAnimation_hook(gb); return; // jp

subid2State1:
  CALL_C(0x6895, checkInteractionSubstate_hook, 0x2403, 0x6898);
  if (!(F & FZ)) { CYCT(0x6898, 0x689b); interactionAnimate_hook(gb); return; } // jp nz
  CYC(0x6898, 0x689b);
  CYC(0x689b, 0x689e); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x689e, 0x68a0); alu_cp(gb, 0x06);
  if (!(F & FZ)) { RET_TAKEN(0x68a0); return; } // ret nz
  CYC(0x68a0, 0x68a1);
  CALL_C(0x68a1, interactionIncSubstate_hook, 0x23e5, 0x68a4);
  CYC(0x68a4, 0x68a7); objectSetVisible82_hook(gb); return; // jp

initSubid0:
  CYC(0x68a7, 0x68aa); A = W8(wMakuTreeState);
  {
    CYC(0x68aa, 0x68ab); push_effect(gb, 0x68ab);
    uint16_t target = makuSprout_jump_table(gb);
    if (target == 0x6924) goto state00;
    if (target == 0x68cd) goto state01;
    if (target == 0x68d1) goto state03;
    if (target == 0x68d6) goto state06;
    if (target == 0x68db) goto state07;
    if (target == 0x68e0) goto state08;
    if (target == 0x68e5) goto state09;
    if (target == 0x68ea) goto state0a;
    if (target == 0x68ef) goto state0b;
    if (target == 0x68f4) goto state0c;
    if (target == 0x68f9) goto state0d;
    if (target == 0x68fe) goto state0e;
    if (target == 0x6903) goto state0f;
    if (target == 0x6908) goto state10;
    HANDOFF(target);
  }

state01: // also @state02
  CYC(0x68cd, 0x68cf); A = 0x01;
  CYC(0x68cf, 0x68d1); goto runSubidCode; // jr

state03: // also @state04, @state05
  CYC(0x68d1, 0x68d4); SET_BC(0x0170); // ldbc $01, <TX_0570
  CYC(0x68d4, 0x68d6); goto runSubid0ScriptMode; // jr

state06:
  CYC(0x68d6, 0x68d9); SET_BC(0x0076); // ldbc $00, <TX_0576
  CYC(0x68d9, 0x68db); goto runSubid0ScriptMode; // jr

state07:
  CYC(0x68db, 0x68de); SET_BC(0x0078); // ldbc $00, <TX_0578
  CYC(0x68de, 0x68e0); goto runSubid0ScriptMode; // jr

state08:
  CYC(0x68e0, 0x68e3); SET_BC(0x027a); // ldbc $02, <TX_057a
  CYC(0x68e3, 0x68e5); goto runSubid0ScriptMode; // jr

state09:
  CYC(0x68e5, 0x68e8); SET_BC(0x017c); // ldbc $01, <TX_057c
  CYC(0x68e8, 0x68ea); goto runSubid0ScriptMode; // jr

state0a:
  CYC(0x68ea, 0x68ed); SET_BC(0x017e); // ldbc $01, <TX_057e
  CYC(0x68ed, 0x68ef); goto runSubid0ScriptMode; // jr

state0b:
  CYC(0x68ef, 0x68f2); SET_BC(0x0080); // ldbc $00, <TX_0580
  CYC(0x68f2, 0x68f4); goto runSubid0ScriptMode; // jr

state0c:
  CYC(0x68f4, 0x68f7); SET_BC(0x0082); // ldbc $00, <TX_0582
  CYC(0x68f7, 0x68f9); goto runSubid0ScriptMode; // jr

state0d:
  CYC(0x68f9, 0x68fc); SET_BC(0x0184); // ldbc $01, <TX_0584
  CYC(0x68fc, 0x68fe); goto runSubid0ScriptMode; // jr

state0e:
  CYC(0x68fe, 0x6901); SET_BC(0x0186); // ldbc $01, <TX_0586
  CYC(0x6901, 0x6903); goto runSubid0ScriptMode; // jr

state0f:
  CYC(0x6903, 0x6906); SET_BC(0x0288); // ldbc $02, <TX_0588
  CYC(0x6906, 0x6908); goto runSubid0ScriptMode; // jr

state10:
  CALL_C(0x6908, checkIsLinkedGame_hook, 0x1992, 0x690b);
  if (F & FZ) { CYCT(0x690b, 0x690d); goto state10Linked; } // jr z
  CYC(0x690b, 0x690d);
  CYC(0x690d, 0x6910); SET_BC(0x008a); // ldbc $00, <TX_058a
  CYC(0x6910, 0x6912); goto runSubid0ScriptMode; // jr

state10Linked:
  CYC(0x6912, 0x6915); SET_BC(0x018c); // ldbc $01, <TX_058c
  CYC(0x6915, 0x6917); goto runSubid0ScriptMode; // jr

runSubidCode:
  CYC(0x6917, 0x6919); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6919, 0x691a); mem_wr(gb, DE, A);
  CYC(0x691a, 0x691b); SET_AF(pop_effect(gb));
  CYC(0x691b, 0x691e); interactionCode88_hook(gb); return; // jp

runSubid0ScriptMode:
  CYC(0x691e, 0x691f); H = D;
  CYC(0x691f, 0x6921); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x6921, 0x6922); mem_wr(gb, HL, B);
  CYC(0x6922, 0x6923); L = alu_inc8(gb, L);
  CYC(0x6923, 0x6924); mem_wr(gb, HL, C);

state00:
  RET(0x6924);
  if (gb->pc == 0x686c && gb->sp == sp0_) goto afterInitSubid0;
  return; // ret

initializeMakuSprout:
  CYC(0x6925, 0x6928); push_effect(gb, 0x6928); goto loadScriptAndInitGraphics; // call
afterLoadScriptAndInitGraphics:
  CYC(0x6928, 0x692b); interactionSetAlwaysUpdateBit_hook(gb);
  if (gb->pc == 0x686f && gb->sp == sp0_) goto subid0State1;
  if (gb->pc == 0x6883 && gb->sp == sp0_) goto afterInitializeMakuSprout_subid1;
  if (gb->pc == 0x6890 && gb->sp == sp0_) goto afterInitializeMakuSprout_subid2;
  return; // jp

loadScriptAndInitGraphics:
  CALL_C(0x6931, interactionInitGraphics_hook, 0x15fb, 0x6934);
  CYC(0x6934, 0x6936); A = 0x05; // >TX_0500
  CALL_C(0x6936, interactionSetHighTextIndex_hook, 0x253b, 0x6939);
  CYC(0x6939, 0x693b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x693b, 0x693c); A = mem_rd(gb, DE);
  CYC(0x693c, 0x693f); SET_HL(0x6949); // @scriptTable
  CYC(0x693f, 0x6940); makuSprout_add_double_index(gb, 0x6940);
  CYC(0x6940, 0x6941); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6941, 0x6942); H = mem_rd(gb, HL);
  CYC(0x6942, 0x6943); L = A;
  CALL_C(0x6943, interactionSetScript_hook, 0x2544, 0x6946);
  CYC(0x6946, 0x6949); interactionIncState_hook(gb);
  if (gb->pc == 0x6928 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterLoadScriptAndInitGraphics;
  return; // jp
}
