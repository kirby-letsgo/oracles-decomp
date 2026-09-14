#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x10, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x10, (from), (to), true)

// object_code/ages/interactions/knowItAllBird.s (INTERAC_KNOW_IT_ALL_BIRD), bank $10.

static void knowItAllBird_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

// 10:7e3f
void interactionCodee3__decVar36_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7e3f, 0x7e40); H = D;  // ld h,d
  CYC(0x7e40, 0x7e42); L = 0x76;  // ld l,Interaction.var36
  CYC(0x7e42, 0x7e43); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  RET(0x7e43); return;  // ret
}

// 10:7e4b
void interactionCodee3__beginJump_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x7e4b, 0x7e4e); SET_BC(0xff40);  // ld bc,-$c0
  CYC(0x7e4e, 0x7e51); objectSetSpeedZ_hook(gb); return;  // jp objectSetSpeedZ
}

void interactionCodee3_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x7da8, checkInteractionState_hook, 0x23fe, 0x7dab);
  if (!(F & FZ)) { CYCT(0x7dab, 0x7dad); goto state1; } CYC(0x7dab, 0x7dad);  // jr nz,@state1
state0:
  CYC(0x7dad, 0x7daf); A = 0x01;  // ld a,$01
  CYC(0x7daf, 0x7db0); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(0x7db0, interactionInitGraphics_hook, 0x15fb, 0x7db3);
  CYC(0x7db3, 0x7db6); SET_HL(0x7f75);  // ld hl,mainScripts.knowItAllBirdScript
  CALL_C(0x7db6, interactionSetScript_hook, 0x2544, 0x7db9);
  CALL_C(0x7db9, getRandomNumber_noPreserveVars_hook, 0x0453, 0x7dbc);
  CYC(0x7dbc, 0x7dbe); alu_and(gb, 0x01);  // and $01
  CYC(0x7dbe, 0x7dc0); E = INTERACTION_BASE + OBJ_DIRECTION;  // ld e,Interaction.direction
  CYC(0x7dc0, 0x7dc1); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(0x7dc1, interactionSetAnimation_hook, 0x262e, 0x7dc4);
  CALL_C(0x7dc4, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7dc7);
  CYC(0x7dc7, 0x7dc9); L = INTERACTION_BASE + OBJ_VAR36;  // ld l,Interaction.var36
  CYC(0x7dc9, 0x7dcb); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(0x7dcb, 0x7dce); push_effect(gb, 0x7dce); interactionCodee3__beginJump_hook(gb);  // call @beginJump
  // beginJump tail-jumps into objectSetSpeedZ_hook, whose own ret pops 0x7dce and
  // resumes execution right here once it returns.
  CYC(0x7dce, 0x7dd0); L = INTERACTION_BASE + OBJ_SUBID;  // ld l,Interaction.subid
  CYC(0x7dd0, 0x7dd1); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x7dd1, 0x7dd3); L = INTERACTION_BASE + OBJ_TEXT_ID;  // ld l,Interaction.textID
  CYC(0x7dd3, 0x7dd4); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x7dd4, 0x7dd7); SET_HL(0x7de4);  // ld hl,@oamFlagsTable
  CYC(0x7dd7, 0x7dd8);
  knowItAllBird_addAToHl_from_rst(gb, 0x7dd8);  // rst_addAToHl
  CYC(0x7dd8, 0x7dd9); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x7dd9, 0x7ddb); E = INTERACTION_BASE + OBJ_OAM_FLAGS;  // ld e,Interaction.oamFlags
  CYC(0x7ddb, 0x7ddc); mem_wr(gb, DE, A);  // ld (de),a
  CYC(0x7ddc, 0x7dde); A = 0x32;  // ld a,>TX_3200
  CALL_C(0x7dde, interactionSetHighTextIndex_hook, 0x253b, 0x7de1);
  CYC(0x7de1, 0x7de4); objectSetVisible82_hook(gb); return;  // jp objectSetVisible82

state1:
  CALL_C(0x7dee, interactionRunScript_hook, 0x2552, 0x7df1);
  CALL_C(0x7df1, checkInteractionSubstate_hook, 0x2403, 0x7df4);
  if (!(F & FZ)) { CYCT(0x7df4, 0x7df6); goto substate1; } CYC(0x7df4, 0x7df6);  // jr nz,@substate1
substate0:
  CYC(0x7df6, 0x7df8); E = INTERACTION_BASE + OBJ_VAR37;  // ld e,Interaction.var37
  CYC(0x7df8, 0x7df9); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(0x7df9, 0x7dfa); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(0x7dfa, 0x7dfc); goto label_10_337; } CYC(0x7dfa, 0x7dfc);  // jr z,@label_10_337
  CALL_C(0x7dfc, interactionIncSubstate_hook, 0x23e5, 0x7dff);
  CYC(0x7dff, 0x7e01); L = INTERACTION_BASE + OBJ_DIRECTION;  // ld l,Interaction.direction
  CYC(0x7e01, 0x7e02); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x7e02, 0x7e04); alu_add(gb, 0x02);  // add $02
  CYC(0x7e04, 0x7e07); interactionSetAnimation_hook(gb); return;  // jp interactionSetAnimation
label_10_337:
  CYC(0x7e07, 0x7e0a); push_effect(gb, 0x7e0a); interactionCodee3__decVar36_hook(gb);  // call @decVar36
  if (!(F & FZ)) { CYCT(0x7e0a, 0x7e0c); goto animate; } CYC(0x7e0a, 0x7e0c);  // jr nz,@animate
  CYC(0x7e0c, 0x7e0e); L = INTERACTION_BASE + OBJ_VAR36;  // ld l,Interaction.var36
  CYC(0x7e0e, 0x7e10); mem_wr(gb, HL, 30);  // ld (hl),30
  CALL_C(0x7e10, getRandomNumber_hook, 0x043e, 0x7e13);
  CYC(0x7e13, 0x7e15); alu_and(gb, 0x07);  // and $07
  if (!(F & FZ)) { CYCT(0x7e15, 0x7e17); goto animate; } CYC(0x7e15, 0x7e17);  // jr nz,@animate
  CYC(0x7e17, 0x7e19); L = INTERACTION_BASE + OBJ_DIRECTION;  // ld l,Interaction.direction
  CYC(0x7e19, 0x7e1a); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x7e1a, 0x7e1c); alu_xor(gb, 0x01);  // xor $01
  CYC(0x7e1c, 0x7e1d); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x7e1d, 0x7e20); interactionSetAnimation_hook(gb); return;  // jp interactionSetAnimation
animate:
  CYC(0x7e20, 0x7e23); interactionAnimateAsNpc_hook(gb); return;  // jp interactionAnimateAsNpc
substate1:
  CALL_C(0x7e23, interactionAnimate_hook, 0x261b, 0x7e26);
  CYC(0x7e26, 0x7e27); H = D;  // ld h,d
  CYC(0x7e27, 0x7e29); L = INTERACTION_BASE + OBJ_VAR37;  // ld l,Interaction.var37
  CYC(0x7e29, 0x7e2a); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x7e2a, 0x7e2b); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(0x7e2b, 0x7e2e); goto updateSpeedZ; } CYC(0x7e2b, 0x7e2e);  // jp nz,@updateSpeedZ
  CYC(0x7e2e, 0x7e30); L = INTERACTION_BASE + OBJ_VAR36;  // ld l,Interaction.var36
  CYC(0x7e30, 0x7e32); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(0x7e32, 0x7e34); L = INTERACTION_BASE + OBJ_SUBSTATE;  // ld l,Interaction.substate
  CYC(0x7e34, 0x7e35); mem_wr(gb, HL, A);  // ld (hl),a  (a==0 here)
  CYC(0x7e35, 0x7e37); L = INTERACTION_BASE + OBJ_Z;  // ld l,Interaction.z
  CYC(0x7e37, 0x7e38); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ld (hl+),a
  CYC(0x7e38, 0x7e39); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(0x7e39, 0x7e3b); L = INTERACTION_BASE + OBJ_DIRECTION;  // ld l,Interaction.direction
  CYC(0x7e3b, 0x7e3c); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(0x7e3c, 0x7e3f); interactionSetAnimation_hook(gb); return;  // jp interactionSetAnimation
updateSpeedZ:
  CYC(0x7e44, 0x7e46); C = 0x20;  // ld c,$20
  CALL_C(0x7e46, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x7e49);
  if (!(F & FZ)) { RET_TAKEN(0x7e49); return; } CYC(0x7e49, 0x7e4a);  // ret nz
  CYC(0x7e4a, 0x7e4b); H = D;  // ld h,d
  interactionCodee3__beginJump_hook(gb); return;  // falls into @beginJump
}
