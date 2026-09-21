#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCodee3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+151, b_+152); H = D;  // ld h,d
  CYC(b_+152, b_+154); L = 0x76;  // ld l,Interaction.var36
  CYC(b_+154, b_+155); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));  // dec (hl)
  RET(b_+155); return;  // ret
}

// 10:7e4b
void interactionCodee3__beginJump_hook(GB *gb) {
  BASE(interactionCodee3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+163, b_+166); SET_BC(0xff40);  // ld bc,-$c0
  CYC(b_+166, b_+169); TAIL(objectSetSpeedZ);  // jp objectSetSpeedZ
}

void interactionCodee3_hook(GB *gb) {
  BASE(interactionCodee3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, checkInteractionState_hook, SYM(checkInteractionState), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+5); goto state1; } CYC(b_+3, b_+5);  // jr nz,@state1
state0:
  CYC(b_+5, b_+7); A = 0x01;  // ld a,$01
  CYC(b_+7, b_+8); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CYC(b_+11, b_+14); SET_HL((SYM(interactionCodee6__checkLinkWithinRange) + 144));  // ld hl,mainScripts.knowItAllBirdScript
  CALL_C(b_+14, interactionSetScript_hook, SYM(interactionSetScript), b_+17);
  CALL_C(b_+17, getRandomNumber_noPreserveVars_hook, SYM(getRandomNumber_noPreserveVars), b_+20);
  CYC(b_+20, b_+22); alu_and(gb, 0x01);  // and $01
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_DIRECTION;  // ld e,Interaction.direction
  CYC(b_+24, b_+25); mem_wr(gb, DE, A);  // ld (de),a
  CALL_C(b_+25, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+28);
  CALL_C(b_+28, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+31);
  CYC(b_+31, b_+33); L = INTERACTION_BASE + OBJ_VAR36;  // ld l,Interaction.var36
  CYC(b_+33, b_+35); mem_wr(gb, HL, 30);  // ld (hl),30
  CYC(b_+35, b_+38); push_effect(gb, b_+38); interactionCodee3__beginJump_hook(gb);  // call @beginJump
  // beginJump tail-jumps into objectSetSpeedZ_hook, whose own ret pops 0x7dce and
  // resumes execution right here once it returns.
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_SUBID;  // ld l,Interaction.subid
  CYC(b_+40, b_+41); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+41, b_+43); L = INTERACTION_BASE + OBJ_TEXT_ID;  // ld l,Interaction.textID
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+44, b_+47); SET_HL(b_+60);  // ld hl,@oamFlagsTable
  CYC(b_+47, b_+48);
  knowItAllBird_addAToHl_from_rst(gb, b_+48);  // rst_addAToHl
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_OAM_FLAGS;  // ld e,Interaction.oamFlags
  CYC(b_+51, b_+52); mem_wr(gb, DE, A);  // ld (de),a
  CYC(b_+52, b_+54); A = 0x32;  // ld a,>TX_3200
  CALL_C(b_+54, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+57);
  CYC(b_+57, b_+60); TAIL(objectSetVisible82);  // jp objectSetVisible82

state1:
  CALL_C(b_+70, interactionRunScript_hook, SYM(interactionRunScript), b_+73);
  CALL_C(b_+73, checkInteractionSubstate_hook, SYM(checkInteractionSubstate), b_+76);
  if (!(F & FZ)) { CYCT(b_+76, b_+78); goto substate1; } CYC(b_+76, b_+78);  // jr nz,@substate1
substate0:
  CYC(b_+78, b_+80); E = INTERACTION_BASE + OBJ_VAR37;  // ld e,Interaction.var37
  CYC(b_+80, b_+81); A = mem_rd(gb, DE);  // ld a,(de)
  CYC(b_+81, b_+82); alu_or(gb, A);  // or a
  if ((F & FZ)) { CYCT(b_+82, b_+84); goto label_10_337; } CYC(b_+82, b_+84);  // jr z,@label_10_337
  CALL_C(b_+84, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+87);
  CYC(b_+87, b_+89); L = INTERACTION_BASE + OBJ_DIRECTION;  // ld l,Interaction.direction
  CYC(b_+89, b_+90); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+90, b_+92); alu_add(gb, 0x02);  // add $02
  CYC(b_+92, b_+95); TAIL(interactionSetAnimation);  // jp interactionSetAnimation
label_10_337:
  CYC(b_+95, b_+98); push_effect(gb, b_+98); interactionCodee3__decVar36_hook(gb);  // call @decVar36
  if (!(F & FZ)) { CYCT(b_+98, b_+100); goto animate; } CYC(b_+98, b_+100);  // jr nz,@animate
  CYC(b_+100, b_+102); L = INTERACTION_BASE + OBJ_VAR36;  // ld l,Interaction.var36
  CYC(b_+102, b_+104); mem_wr(gb, HL, 30);  // ld (hl),30
  CALL_C(b_+104, getRandomNumber_hook, SYM(getRandomNumber), b_+107);
  CYC(b_+107, b_+109); alu_and(gb, 0x07);  // and $07
  if (!(F & FZ)) { CYCT(b_+109, b_+111); goto animate; } CYC(b_+109, b_+111);  // jr nz,@animate
  CYC(b_+111, b_+113); L = INTERACTION_BASE + OBJ_DIRECTION;  // ld l,Interaction.direction
  CYC(b_+113, b_+114); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+114, b_+116); alu_xor(gb, 0x01);  // xor $01
  CYC(b_+116, b_+117); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+117, b_+120); TAIL(interactionSetAnimation);  // jp interactionSetAnimation
animate:
  CYC(b_+120, b_+123); TAIL(interactionAnimateAsNpc);  // jp interactionAnimateAsNpc
substate1:
  CALL_C(b_+123, interactionAnimate_hook, SYM(interactionAnimate), b_+126);
  CYC(b_+126, b_+127); H = D;  // ld h,d
  CYC(b_+127, b_+129); L = INTERACTION_BASE + OBJ_VAR37;  // ld l,Interaction.var37
  CYC(b_+129, b_+130); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+130, b_+131); alu_or(gb, A);  // or a
  if (!(F & FZ)) { CYCT(b_+131, b_+134); goto updateSpeedZ; } CYC(b_+131, b_+134);  // jp nz,@updateSpeedZ
  CYC(b_+134, b_+136); L = INTERACTION_BASE + OBJ_VAR36;  // ld l,Interaction.var36
  CYC(b_+136, b_+138); mem_wr(gb, HL, 60);  // ld (hl),60
  CYC(b_+138, b_+140); L = INTERACTION_BASE + OBJ_SUBSTATE;  // ld l,Interaction.substate
  CYC(b_+140, b_+141); mem_wr(gb, HL, A);  // ld (hl),a  (a==0 here)
  CYC(b_+141, b_+143); L = INTERACTION_BASE + OBJ_Z;  // ld l,Interaction.z
  CYC(b_+143, b_+144); mem_wr(gb, HL, A); SET_HL(HL + 1);  // ld (hl+),a
  CYC(b_+144, b_+145); mem_wr(gb, HL, A);  // ld (hl),a
  CYC(b_+145, b_+147); L = INTERACTION_BASE + OBJ_DIRECTION;  // ld l,Interaction.direction
  CYC(b_+147, b_+148); A = mem_rd(gb, HL);  // ld a,(hl)
  CYC(b_+148, b_+151); TAIL(interactionSetAnimation);  // jp interactionSetAnimation
updateSpeedZ:
  CYC(b_+156, b_+158); C = 0x20;  // ld c,$20
  CALL_C(b_+158, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+161);
  if (!(F & FZ)) { RET_TAKEN(b_+161); return; } CYC(b_+161, b_+162);  // ret nz
  CYC(b_+162, b_+163); H = D;  // ld h,d
  TAIL(interactionCodee3__beginJump);  // falls into @beginJump
}
