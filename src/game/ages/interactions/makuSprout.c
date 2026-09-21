#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode88);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = makuSprout_jump_table(gb);
    if (target == b_+38) goto subid1;
    if (target == b_+51) goto subid2;
  }

  // interactionCode88@subid0
  CALL_C(b_+10, checkInteractionState_hook, SYM(checkInteractionState), b_+13);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto subid0State1; } // jr nz
  CYC(b_+13, b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  CYC(b_+20, b_+23); push_effect(gb, b_+23); goto initSubid0; // call
afterInitSubid0:
  CYC(b_+23, b_+26); push_effect(gb, b_+26); goto initializeMakuSprout; // call

subid0State1:
  CALL_C(b_+26, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+29);
  CYC(b_+29, b_+31); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+31, b_+32); A = mem_rd(gb, DE);
  CYC(b_+32, b_+34); alu_and(gb, 0x8f);
  CYC(b_+34, b_+35); mem_wr(gb, DE, A);
  CYC(b_+35, b_+38); TAIL(interactionRunScript); // jp

subid1:
  CALL_C(b_+38, checkInteractionState_hook, SYM(checkInteractionState), b_+41);
  if (!(F & FZ)) { CYCT(b_+41, b_+43); goto subid1State1; } // jr nz
  CYC(b_+41, b_+43);
  CYC(b_+43, b_+46); push_effect(gb, b_+46); goto initializeMakuSprout; // call
afterInitializeMakuSprout_subid1:
  CALL_C(b_+46, interactionRunScript_hook, SYM(interactionRunScript), b_+49);

subid1State1:
  CYC(b_+49, b_+51); goto subid0State1; // jr

subid2:
  CALL_C(b_+51, checkInteractionState_hook, SYM(checkInteractionState), b_+54);
  if (!(F & FZ)) { CYCT(b_+54, b_+56); goto subid2State1; } // jr nz
  CYC(b_+54, b_+56);
  CYC(b_+56, b_+59); push_effect(gb, b_+59); goto initializeMakuSprout; // call
afterInitializeMakuSprout_subid2:
  CYC(b_+59, b_+61); A = 0x01;
  CYC(b_+61, b_+64); TAIL(interactionSetAnimation); // jp

subid2State1:
  CALL_C(b_+64, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+67);
  if (!(F & FZ)) { CYCT(b_+67, b_+70); interactionAnimate_hook(gb); return; } // jp nz
  CYC(b_+67, b_+70);
  CYC(b_+70, b_+73); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+73, b_+75); alu_cp(gb, 0x06);
  if (!(F & FZ)) { RET_TAKEN(b_+75); return; } // ret nz
  CYC(b_+75, b_+76);
  CALL_C(b_+76, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+79);
  CYC(b_+79, b_+82); TAIL(objectSetVisible82); // jp

initSubid0:
  CYC(b_+82, b_+85); A = W8(wMakuTreeState);
  {
    CYC(b_+85, b_+86); push_effect(gb, b_+86);
    uint16_t target = makuSprout_jump_table(gb);
    if (target == b_+207) goto state00;
    if (target == b_+120) goto state01;
    if (target == b_+124) goto state03;
    if (target == b_+129) goto state06;
    if (target == b_+134) goto state07;
    if (target == b_+139) goto state08;
    if (target == b_+144) goto state09;
    if (target == b_+149) goto state0a;
    if (target == b_+154) goto state0b;
    if (target == b_+159) goto state0c;
    if (target == b_+164) goto state0d;
    if (target == b_+169) goto state0e;
    if (target == b_+174) goto state0f;
    if (target == b_+179) goto state10;
    HANDOFF(target);
  }

state01: // also @state02
  CYC(b_+120, b_+122); A = 0x01;
  CYC(b_+122, b_+124); goto runSubidCode; // jr

state03: // also @state04, @state05
  CYC(b_+124, b_+127); SET_BC(0x0170); // ldbc $01, <TX_0570
  CYC(b_+127, b_+129); goto runSubid0ScriptMode; // jr

state06:
  CYC(b_+129, b_+132); SET_BC(0x0076); // ldbc $00, <TX_0576
  CYC(b_+132, b_+134); goto runSubid0ScriptMode; // jr

state07:
  CYC(b_+134, b_+137); SET_BC(0x0078); // ldbc $00, <TX_0578
  CYC(b_+137, b_+139); goto runSubid0ScriptMode; // jr

state08:
  CYC(b_+139, b_+142); SET_BC(0x027a); // ldbc $02, <TX_057a
  CYC(b_+142, b_+144); goto runSubid0ScriptMode; // jr

state09:
  CYC(b_+144, b_+147); SET_BC(0x017c); // ldbc $01, <TX_057c
  CYC(b_+147, b_+149); goto runSubid0ScriptMode; // jr

state0a:
  CYC(b_+149, b_+152); SET_BC(0x017e); // ldbc $01, <TX_057e
  CYC(b_+152, b_+154); goto runSubid0ScriptMode; // jr

state0b:
  CYC(b_+154, b_+157); SET_BC(0x0080); // ldbc $00, <TX_0580
  CYC(b_+157, b_+159); goto runSubid0ScriptMode; // jr

state0c:
  CYC(b_+159, b_+162); SET_BC(0x0082); // ldbc $00, <TX_0582
  CYC(b_+162, b_+164); goto runSubid0ScriptMode; // jr

state0d:
  CYC(b_+164, b_+167); SET_BC(0x0184); // ldbc $01, <TX_0584
  CYC(b_+167, b_+169); goto runSubid0ScriptMode; // jr

state0e:
  CYC(b_+169, b_+172); SET_BC(0x0186); // ldbc $01, <TX_0586
  CYC(b_+172, b_+174); goto runSubid0ScriptMode; // jr

state0f:
  CYC(b_+174, b_+177); SET_BC(0x0288); // ldbc $02, <TX_0588
  CYC(b_+177, b_+179); goto runSubid0ScriptMode; // jr

state10:
  CALL_C(b_+179, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+182);
  if (F & FZ) { CYCT(b_+182, b_+184); goto state10Linked; } // jr z
  CYC(b_+182, b_+184);
  CYC(b_+184, b_+187); SET_BC(0x008a); // ldbc $00, <TX_058a
  CYC(b_+187, b_+189); goto runSubid0ScriptMode; // jr

state10Linked:
  CYC(b_+189, b_+192); SET_BC(0x018c); // ldbc $01, <TX_058c
  CYC(b_+192, b_+194); goto runSubid0ScriptMode; // jr

runSubidCode:
  CYC(b_+194, b_+196); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+196, b_+197); mem_wr(gb, DE, A);
  CYC(b_+197, b_+198); SET_AF(pop_effect(gb));
  CYC(b_+198, b_+201); TAIL(interactionCode88); // jp

runSubid0ScriptMode:
  CYC(b_+201, b_+202); H = D;
  CYC(b_+202, b_+204); L = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+204, b_+205); mem_wr(gb, HL, B);
  CYC(b_+205, b_+206); L = alu_inc8(gb, L);
  CYC(b_+206, b_+207); mem_wr(gb, HL, C);

state00:
  RET(b_+207);
  if (gb->pc == b_+23 && gb->sp == sp0_) goto afterInitSubid0;
  return; // ret

initializeMakuSprout:
  CYC(b_+208, b_+211); push_effect(gb, b_+211); goto loadScriptAndInitGraphics; // call
afterLoadScriptAndInitGraphics:
  CYC(b_+211, b_+214); interactionSetAlwaysUpdateBit_hook(gb);
  if (gb->pc == b_+26 && gb->sp == sp0_) goto subid0State1;
  if (gb->pc == b_+46 && gb->sp == sp0_) goto afterInitializeMakuSprout_subid1;
  if (gb->pc == b_+59 && gb->sp == sp0_) goto afterInitializeMakuSprout_subid2;
  return; // jp

loadScriptAndInitGraphics:
  CALL_C(b_+220, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+223);
  CYC(b_+223, b_+225); A = 0x05; // >TX_0500
  CALL_C(b_+225, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+228);
  CYC(b_+228, b_+230); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+230, b_+231); A = mem_rd(gb, DE);
  CYC(b_+231, b_+234); SET_HL(b_+244); // @scriptTable
  CYC(b_+234, b_+235); makuSprout_add_double_index(gb, b_+235);
  CYC(b_+235, b_+236); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+236, b_+237); H = mem_rd(gb, HL);
  CYC(b_+237, b_+238); L = A;
  CALL_C(b_+238, interactionSetScript_hook, SYM(interactionSetScript), b_+241);
  CYC(b_+241, b_+244); interactionIncState_hook(gb);
  if (gb->pc == b_+211 && gb->sp == (uint16_t)(sp0_ - 2)) goto afterLoadScriptAndInitGraphics;
  return; // jp
}
