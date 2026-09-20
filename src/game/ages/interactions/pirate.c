#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodec4), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodec4), (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/pirate.s (interactionCodec4 / INTERAC_PIRATE),
// bank 0x0b. Distinct from src/game/pirateShip.c and src/game/pirateShipInteraction.c
// (INTERAC_PIRATE_SHIP / pirateShip.s) and src/game/pirateCaptain.c (pirateCaptain.s) --
// verified via this file's own disassembly path.
//
// Variables:
//   var3f: Push counter for subid 4 (tokay eyeball is inserted when it reached 0)

static uint16_t interactionCodec4_jump_table(GB *gb) {
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

static void interactionCodec4_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCodec4@resetPushCounter (0b:7440): true local, always called with a genuine return
// expected, never dispatches into another hook -- a plain helper with no push/ret_effect at its
// own boundary is correct and safe (matches src/game/ball.c's func_6b00 precedent). No CALL_C
// inside, so it needs no sp0_.
static void interactionCodec4_resetPushCounter(GB *gb) {
  BASE(interactionCodec4);
  CYC(b_+214, b_+216); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+216, b_+218); A = 10;
  CYC(b_+218, b_+219); mem_wr(gb, DE, A);
  CYC(b_+219, SYM(interactionCodec5)); return; // ret
}

// interactionCodec4@checkCenteredWithLink (0b:73eb): single caller (@state3), tail-jumps into the
// already-hooked objectCheckCenteredWithLink, so (like interactionCode8a_checkEssenceObtained in
// src/game/remoteMakuCutscene.c) it takes an explicit return_address and pushes it at entry; its
// early "ret nz" paths pop that same value via ret_effect for consistency.
static void interactionCodec4_checkCenteredWithLink(GB *gb, uint16_t return_address) {
  BASE(interactionCodec4);
  push_effect(gb, return_address);
  CYC(b_+129, b_+132); A = W8(wLinkDeathTrigger);
  CYC(b_+132, b_+133); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+133, b_+134); ret_effect(gb); return; } // ret nz
  CYC(b_+133, b_+134);
  CYC(b_+134, b_+137); A = W8(wLinkPushingDirection);
  CYC(b_+137, b_+138); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+138, b_+139); ret_effect(gb); return; } // ret nz
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+142); A = W8(wGameKeysPressed);
  CYC(b_+142, b_+144); alu_and(gb, 0x03); // BTN_A | BTN_B
  if (!(F & FZ)) { CYCT(b_+144, b_+145); ret_effect(gb); return; } // ret nz
  CYC(b_+144, b_+145);
  CYC(b_+145, b_+147); B = 0x05;
  CYC(b_+147, b_+150); objectCheckCenteredWithLink_hook(gb); // jp
}

// ==================================================================================================
// INTERAC_PIRATE
// ==================================================================================================
void interactionCodec4_hook(GB *gb) {
  BASE(interactionCodec4);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCodec4_jump_table(gb);
    if (target == b_+87) goto state1;
    if (target == b_+115) goto state2;
    if (target == b_+150) goto state3;
    if (target == b_+207) goto state4;
    // target == 0x7378 falls through to state0
  }

  // interactionCodec4@state0
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  {
    CYC(b_+17, b_+18); push_effect(gb, b_+18);
    uint16_t target = interactionCodec4_jump_table(gb);
    if (target == b_+48) goto subid4Init;
    // target == 0x7386 falls through to subid0Init/1Init/2Init/3Init (aliased)
  }

  // interactionCodec4@subid0Init / @subid1Init / @subid2Init / @subid3Init
  CYC(b_+28, b_+29); A = mem_rd(gb, DE);
  CYC(b_+29, b_+32); SET_HL(b_+77); // @scriptTable
  CYC(b_+32, b_+33); interactionCodec4_add_double_index(gb, b_+33);
  CYC(b_+33, b_+34); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+34, b_+35); H = mem_rd(gb, HL);
  CYC(b_+35, b_+36); L = A;
  CALL_C(b_+36, interactionSetScript_hook, SYM(interactionSetScript), b_+39);
  CALL_C(b_+39, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+42); // SWITCHES THREADS
  CALL_C(b_+42, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+45);
  CYC(b_+45, b_+48); interactionIncState_hook(gb); return; // jp

subid4Init: // interactionCodec4@subid4Init
  CALL_C(b_+48, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+51);
  CYC(b_+51, b_+53); alu_and(gb, 0x80); // ROOMFLAG_80
  if (!(F & FZ)) { CYCT(b_+53, b_+56); interactionDelete_hook(gb); return; } // jp nz
  CYC(b_+53, b_+56);
  CYC(b_+56, b_+59); interactionCodec4_resetPushCounter(gb); // call
  CYC(b_+59, b_+61); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+61, b_+63); A = 0x03;
  CYC(b_+63, b_+64); mem_wr(gb, DE, A);
  CYC(b_+64, b_+66); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+70); SET_HL(b_+77); // @scriptTable
  CYC(b_+70, b_+71); interactionCodec4_add_double_index(gb, b_+71);
  CYC(b_+71, b_+72); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+72, b_+73); H = mem_rd(gb, HL);
  CYC(b_+73, b_+74); L = A;
  CYC(b_+74, b_+77); interactionSetScript_hook(gb); return; // jp

state1: // interactionCodec4@state1
  CYC(b_+87, b_+90); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(b_+90, b_+92); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+92, b_+95); goto jump; } // jp nz
  CYC(b_+92, b_+95);
  CALL_C(b_+95, interactionRunScript_hook, SYM(interactionRunScript), b_+98);
  CYC(b_+98, b_+101); npcFaceLinkAndAnimate_hook(gb); return; // jp

jump: // interactionCodec4@jump
  CYC(b_+101, b_+103); A = 0x02;
  CALL_C(b_+103, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+106);
  CYC(b_+106, b_+109); SET_BC(0xfe00); // -$200
  CALL_C(b_+109, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+112);
  CYC(b_+112, b_+115); interactionIncState_hook(gb); return; // jp

state2: // interactionCodec4@state2
  CYC(b_+115, b_+117); C = 0x28;
  CALL_C(b_+117, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+120);
  if (!(F & FZ)) { CYCT(b_+120, b_+121); ret_effect(gb); return; } // ret nz
  CYC(b_+120, b_+121);
  CYC(b_+121, b_+124); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(b_+124, b_+126); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02); // set 1,(hl)
  CYC(b_+126, b_+129); interactionAnimate_hook(gb); return; // jp

state3: // interactionCodec4@state3
  CALL_C(b_+150, objectCheckCollidedWithLink_notDead_hook, SYM(objectCheckCollidedWithLink_notDead), b_+153);
  if (!(F & FC)) { CYCT(b_+153, b_+156); interactionCodec4_resetPushCounter(gb); } else { CYC(b_+153, b_+156); } // call nc
  CYC(b_+156, b_+159); interactionCodec4_checkCenteredWithLink(gb, b_+159); // call
  if (!(F & FC)) { CYCT(b_+159, b_+162); interactionCodec4_resetPushCounter(gb); } else { CYC(b_+159, b_+162); } // call nc
  CYC(b_+162, b_+163); H = D;
  CYC(b_+163, b_+165); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+165, b_+166); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+166, b_+168); goto state4; } // jr nz
  CYC(b_+166, b_+168);
  CYC(b_+168, b_+170); A = 0x4f; // TREASURE_TOKAY_EYEBALL
  CALL_C(b_+170, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+173);
  if (F & FC) { CYCT(b_+173, b_+175); goto haveEyeball; } // jr c
  CYC(b_+173, b_+175);
  CYC(b_+175, b_+178); SET_BC((SYM(clearParts) + 8)); // TX_360d
  CALL_C(b_+178, showText_hook, SYM(showText), b_+181);
  CYC(b_+181, b_+183); interactionCodec4_resetPushCounter(gb); return; // jr

haveEyeball: // interactionCodec4@haveEyeball
  CALL_C(b_+183, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+186);
  if (!(F & FC)) { CYCT(b_+186, b_+188); interactionCodec4_resetPushCounter(gb); return; } // jr nc
  CYC(b_+186, b_+188);
  CYC(b_+188, b_+190); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(b_+190, b_+193); W8(wDisabledObjects) = A;
  CYC(b_+193, b_+196); W8(wMenuDisabled) = A;
  CYC(b_+196, b_+198); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+198, playSound_b00_hook, SYM(playSound_b00), b_+201);
  CYC(b_+201, b_+204); SET_HL((SYM(interactionCoded8__subid1Script) + 52)); // mainScripts.pirateSubid4Script_insertEyeball
  CALL_C(b_+204, interactionSetScript_hook, SYM(interactionSetScript), b_+207);

state4: // interactionCodec4@state4
  CALL_C(b_+207, interactionRunScript_hook, SYM(interactionRunScript), b_+210);
  if (!(F & FC)) { CYCT(b_+210, b_+211); ret_effect(gb); return; } // ret nc
  CYC(b_+210, b_+211);
  CYC(b_+211, b_+214); interactionDelete_hook(gb); return; // jp
}
