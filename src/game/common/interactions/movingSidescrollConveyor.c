#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// Bare-global, implemented in src/game/movingSidescrollPlatform.c; shared by both interactions'
// updateState jump tables (both @-locals dispatch their last entry into it identically).
void movingPlatform_stateC_hook(GB *gb);

static uint16_t interactionCodea2_jump_table(GB *gb) {
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

static void interactionCodea2_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// ==================================================================================================
// INTERAC_MOVING_SIDESCROLL_CONVEYOR
// ==================================================================================================
// interactionCodea2@updateState is inlined below (single caller): entered by a genuine `call`
// from this routine's own body, its branches all funnel into @applySpeed (a real "ret nz" or a
// tail `jp updateLinkPositionGivenVelocity` whose own ret consumes the same manually-pushed
// return address), or into the shared movingPlatform_stateC_hook -- and this routine still has
// "jp sidescrollingPlatformCommon" to run afterward, so the resume is verified like CALL_C at the
// shared `afterUpdateState` label (same idiom as movingSidescrollPlatform.c's interactionCodea1).
void interactionCodea2_hook(GB *gb) {
  BASE(interactionCodea2);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionAnimate_hook, SYM(interactionAnimate), b_+3);
  CALL_C(b_+3, sidescrollPlatform_checkLinkOnPlatform_hook, SYM(sidescrollPlatform_checkLinkOnPlatform), b_+6);
  if (!(F & FZ)) { CALL_C_CC(b_+6, sidescrollPlatform_updateLinkKnockbackForConveyor_hook, SYM(sidescrollPlatform_updateLinkKnockbackForConveyor), b_+9); } else { CYC(b_+6, b_+9); } // call nz
  CYC(b_+9, b_+12); push_effect(gb, b_+12);

  CYC(b_+15, b_+17); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+17, b_+18); A = mem_rd(gb, DE);
  CYC(b_+18, b_+20); alu_sub(gb, 0x08);
  if (F & FC) { CYCT(b_+20, b_+22); goto state0To7; } // jr c
  CYC(b_+20, b_+22);
  CYC(b_+22, b_+23); push_effect(gb, b_+23);
  do { uint16_t jt_ = (interactionCodea2_jump_table(gb));
    if (jt_ == b_+59) { goto state8; }
    else if (jt_ == b_+73) { goto state9; }
    else if (jt_ == b_+87) { goto stateA; }
    else if (jt_ == b_+115) { goto stateB; }
    else if (jt_ == SYM(movingPlatform_stateC)) { movingPlatform_stateC_hook(gb); goto afterUpdateState; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0To7:
  CYC(b_+33, b_+36); SET_HL((SYM(interactionCoded8__subid5Script) + 27)); // bank0e.movingSidescrollConveyorScriptTable
  CALL_C(b_+36, objectLoadMovementScript_hook, SYM(objectLoadMovementScript), b_+39);
  CALL_C(b_+39, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+42);
  CYC(b_+42, b_+43); H = D;
  CYC(b_+43, b_+45); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+45, b_+47); mem_wr(gb, HL, 0x08);
  CYC(b_+47, b_+48); L = alu_inc8(gb, L);
  CYC(b_+48, b_+50); mem_wr(gb, HL, 0x0c);
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CALL_C(b_+53, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+56);
  CYC(b_+56, b_+59); objectSetVisible82_hook(gb); goto afterUpdateState; // jp

state8:
  CYC(b_+59, b_+61); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+63); H = D;
  CYC(b_+63, b_+65); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+65, b_+66); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+66, b_+68); goto applySpeed; } // jr c
  CYC(b_+66, b_+68);
  CYC(b_+68, b_+69); A = mem_rd(gb, DE);
  CYC(b_+69, b_+70); mem_wr(gb, HL, A);
  CYC(b_+70, b_+73); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateState; // jp

state9:
  CYC(b_+73, b_+75); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+75, b_+76); A = mem_rd(gb, DE);
  CYC(b_+76, b_+77); H = D;
  CYC(b_+77, b_+79); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+79, b_+80); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+80, b_+82); goto applySpeed; } // jr c
  CYC(b_+80, b_+82);
  CYC(b_+82, b_+83); A = mem_rd(gb, HL);
  CYC(b_+83, b_+84); mem_wr(gb, DE, A);
  CYC(b_+84, b_+87); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateState; // jp

stateA:
  CYC(b_+87, b_+89); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+89, b_+90); A = mem_rd(gb, DE);
  CYC(b_+90, b_+91); H = D;
  CYC(b_+91, b_+93); L = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+93, b_+94); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+94, b_+96); goto l_5939; } // jr nc
  CYC(b_+94, b_+96);
  CYC(b_+96, b_+98); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+98, b_+99); B = mem_rd(gb, HL);
  CYC(b_+99, b_+101); C = 0x10; // ANGLE_DOWN
  CYC(b_+101, b_+104); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+104, b_+105); alu_cp(gb, D);
  if (F & FZ) { CALL_C_CC(b_+105, updateLinkPositionGivenVelocity_hook, SYM(updateLinkPositionGivenVelocity), b_+108); } else { CYC(b_+105, b_+108); } // call z
  CYCT(b_+108, b_+110); goto applySpeed; // jr

l_5939:
  CYC(b_+110, b_+111); A = mem_rd(gb, HL);
  CYC(b_+111, b_+112); mem_wr(gb, DE, A);
  CYC(b_+112, b_+115); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateState; // jp

stateB:
  CYC(b_+115, b_+117); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+117, b_+118); A = mem_rd(gb, DE);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+121, b_+122); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+122, b_+124); goto applySpeed; } // jr c
  CYC(b_+122, b_+124);
  CYC(b_+124, b_+125); A = mem_rd(gb, DE);
  CYC(b_+125, b_+126); mem_wr(gb, HL, A);
  CYC(b_+126, b_+129); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateState; // jp

applySpeed:
  CALL_C(b_+129, objectApplySpeed_hook, SYM(objectApplySpeed), b_+132);
  CYC(b_+132, b_+135); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+135, b_+136); alu_cp(gb, D);
  if (!(F & FZ)) { CYCT(b_+136, b_+137); ret_effect(gb); return; } // ret nz
  CYC(b_+136, b_+137);
  CYC(b_+137, b_+139); E = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+139, b_+140); A = mem_rd(gb, DE);
  CYC(b_+140, b_+141); alu_rrca(gb);
  CYC(b_+141, b_+142); alu_rrca(gb);
  CYC(b_+142, b_+143); B = A;
  CYC(b_+143, b_+145); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+147); alu_add(gb, B);
  CYC(b_+147, b_+150); SET_HL(b_+157); // @directions
  CYC(b_+150, b_+151); interactionCodea2_addDoubleIndex(gb, b_+151);
  CYC(b_+151, b_+152); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+152, b_+153); C = A;
  CYC(b_+153, b_+154); B = mem_rd(gb, HL);
  CYC(b_+154, b_+157); updateLinkPositionGivenVelocity_hook(gb); goto afterUpdateState; // jp

afterUpdateState:
  if (!(gb->pc == b_+12 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(b_+12, b_+15); TAIL(sidescrollingPlatformCommon); // jp
}
