#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(movingPlatform_stateC), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(movingPlatform_stateC), (from), (to), true)

static uint16_t interactionCodea1_jump_table(GB *gb) {
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

static void interactionCodea1_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:58c4, bare-global (no leading @ in the .s source). Reached by the last entry of
// interactionCodea1@updateSubid's jump table (see interactionCodea1_hook below).
void movingPlatform_stateC_hook(GB *gb) {
  BASE(movingPlatform_stateC);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+3);
  if (!(F & FZ)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nz
  CYC(b_+3, b_+4);
  CYC(b_+4, SYM(interactionCodea2)); sidescrollPlatformFunc_5bfc_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_MOVING_SIDESCROLL_PLATFORM
// ==================================================================================================
// interactionCodea1@updateSubid is inlined below (single caller, "callers: 1"): it's entered by a
// genuine `call` from this routine's own body, and every one of its branches either performs a
// real "ret" (movingPlatform_stateC's "ret nz") or tail-jumps into an already-hooked leaf whose own
// ret consumes the very same manually-pushed return address (0x581d) -- and this routine still has
// more work to do afterward ("jp sidescrollingPlatformCommon"), so the resume is verified exactly
// like CALL_C does, at the single shared `afterUpdateSubid` label (same idiom as
// src/game/decoration.c's @isSymmetryCityRoom).
void interactionCodea1_hook(GB *gb) {
  BASE(interactionCodea1);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, sidescrollPlatform_checkLinkOnPlatform_hook, SYM(sidescrollPlatform_checkLinkOnPlatform), b_+3);
  CYC(b_+3, b_+6); push_effect(gb, b_+6);

  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+14); alu_sub(gb, 0x08);
  if (F & FC) { CYCT(b_+14, b_+16); goto state0To7; } // jr c
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+17); push_effect(gb, b_+17);
  do { uint16_t jt_ = (interactionCodea1_jump_table(gb));
    if (jt_ == b_+69) { goto state8; }
    else if (jt_ == b_+86) { goto state9; }
    else if (jt_ == b_+115) { goto stateA; }
    else if (jt_ == b_+144) { goto stateB; }
    else if (jt_ == SYM(movingPlatform_stateC)) { movingPlatform_stateC_hook(gb); goto afterUpdateSubid; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0To7:
  CYC(b_+27, b_+30); SET_HL((SYM(interactionCoded8__subid4Script) + 51)); // bank0e.movingSidescrollPlatformScriptTable
  CALL_C(b_+30, objectLoadMovementScript_hook, SYM(objectLoadMovementScript), b_+33);
  CALL_C(b_+33, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+36);
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+42); SET_HL(b_+59); // @collisionRadii
  CYC(b_+42, b_+43); interactionCodea1_addDoubleIndex(gb, b_+43);
  CYC(b_+43, b_+45); E = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+45, b_+46); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+46, b_+47); mem_wr(gb, DE, A);
  CYC(b_+47, b_+48); E = alu_inc8(gb, E);
  CYC(b_+48, b_+49); A = mem_rd(gb, HL);
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+52); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+52, b_+53); A = mem_rd(gb, DE);
  CALL_C(b_+53, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+56);
  CYC(b_+56, b_+59); objectSetVisible82_hook(gb); goto afterUpdateSubid; // jp

state8:
  CYC(b_+69, b_+71); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+71, b_+72); A = mem_rd(gb, DE);
  CYC(b_+72, b_+73); H = D;
  CYC(b_+73, b_+75); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+75, b_+76); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+76, b_+78); goto l_5868; } // jr nc
  CYC(b_+76, b_+78);
  CYC(b_+78, b_+81); objectApplySpeed_hook(gb); goto afterUpdateSubid; // jp

l_5868:
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+83); mem_wr(gb, HL, A);
  CYC(b_+83, b_+86); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateSubid; // jp

state9:
  CYC(b_+86, b_+88); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+90); H = D;
  CYC(b_+90, b_+92); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+92, b_+93); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+93, b_+95); goto l_5885; } // jr nc
  CYC(b_+93, b_+95);
  CYC(b_+95, b_+97); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+97, b_+98); B = mem_rd(gb, HL);
  CYC(b_+98, b_+100); C = 0x08; // ANGLE_RIGHT
  CYC(b_+100, b_+103); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+103, b_+104); alu_cp(gb, D);
  if (F & FZ) { CALL_C_CC(b_+104, updateLinkPositionGivenVelocity_hook, SYM(updateLinkPositionGivenVelocity), b_+107); } else { CYC(b_+104, b_+107); } // call z
  CYC(b_+107, b_+110); objectApplySpeed_hook(gb); goto afterUpdateSubid; // jp

l_5885:
  CYC(b_+110, b_+111); A = mem_rd(gb, HL);
  CYC(b_+111, b_+112); mem_wr(gb, DE, A);
  CYC(b_+112, b_+115); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateSubid; // jp

stateA:
  CYC(b_+115, b_+117); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+117, b_+118); A = mem_rd(gb, DE);
  CYC(b_+118, b_+119); H = D;
  CYC(b_+119, b_+121); L = INTERACTION_BASE + OBJ_VAR32;
  CYC(b_+121, b_+122); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+122, b_+124); goto l_58a2; } // jr nc
  CYC(b_+122, b_+124);
  CYC(b_+124, b_+126); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+126, b_+127); B = mem_rd(gb, HL);
  CYC(b_+127, b_+129); C = 0x10; // ANGLE_DOWN
  CYC(b_+129, b_+132); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+132, b_+133); alu_cp(gb, D);
  if (F & FZ) { CALL_C_CC(b_+133, updateLinkPositionGivenVelocity_hook, SYM(updateLinkPositionGivenVelocity), b_+136); } else { CYC(b_+133, b_+136); } // call z
  CYC(b_+136, b_+139); objectApplySpeed_hook(gb); goto afterUpdateSubid; // jp

l_58a2:
  CYC(b_+139, b_+140); A = mem_rd(gb, HL);
  CYC(b_+140, b_+141); mem_wr(gb, DE, A);
  CYC(b_+141, b_+144); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateSubid; // jp

stateB:
  CYC(b_+144, b_+146); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(b_+146, b_+147); A = mem_rd(gb, DE);
  CYC(b_+147, b_+148); H = D;
  CYC(b_+148, b_+150); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+150, b_+151); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+151, b_+153); goto l_58bf; } // jr nc
  CYC(b_+151, b_+153);
  CYC(b_+153, b_+155); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+155, b_+156); B = mem_rd(gb, HL);
  CYC(b_+156, b_+158); C = 0x18; // ANGLE_LEFT
  CYC(b_+158, b_+161); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+161, b_+162); alu_cp(gb, D);
  if (F & FZ) { CALL_C_CC(b_+162, updateLinkPositionGivenVelocity_hook, SYM(updateLinkPositionGivenVelocity), b_+165); } else { CYC(b_+162, b_+165); } // call z
  CYC(b_+165, b_+168); objectApplySpeed_hook(gb); goto afterUpdateSubid; // jp

l_58bf:
  CYC(b_+168, b_+169); A = mem_rd(gb, DE);
  CYC(b_+169, b_+170); mem_wr(gb, HL, A);
  CYC(b_+170, SYM(movingPlatform_stateC)); sidescrollPlatformFunc_5bfc_hook(gb); goto afterUpdateSubid; // jp

afterUpdateSubid:
  if (!(gb->pc == b_+6 && gb->sp == sp0_)) { hook_continue(gb, gb->pc, sp0_); return; }
  CYC(b_+6, b_+9); sidescrollingPlatformCommon_hook(gb); return; // jp
}

// 0b:7fa1, bare-global. Genuinely dead code -- "Garbage function here (partial repeat of the
// above function)" per ref/oracles-disasm/code/ages/garbage/bank0bEnd.s -- with zero callers and
// call/jump targets (0x258f, 0x3b5c) that land mid-instruction inside unrelated routines, so
// neither has a known _hook. Ported byte-for-byte via the raw interpreter primitives.
void func_7fa1_hook(GB *gb) {
  BASE(func_7fa1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); push_effect(gb, b_+3); asm_call(gb, (SYM(_interactionSaveScriptAddress) + 7), b_+3);
  if (!(F & FC)) { CYCT(b_+3, b_+4); ret_effect(gb); return; } // ret nc
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+7); hook_continue(gb, (SYM(updateInteractions__next) + 12), sp0_); return; // jp
}
