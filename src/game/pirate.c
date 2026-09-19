#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  CYC(0x7440, 0x7442); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7442, 0x7444); A = 10;
  CYC(0x7444, 0x7445); mem_wr(gb, DE, A);
  CYC(0x7445, 0x7446); return; // ret
}

// interactionCodec4@checkCenteredWithLink (0b:73eb): single caller (@state3), tail-jumps into the
// already-hooked objectCheckCenteredWithLink, so (like interactionCode8a_checkEssenceObtained in
// src/game/remoteMakuCutscene.c) it takes an explicit return_address and pushes it at entry; its
// early "ret nz" paths pop that same value via ret_effect for consistency.
static void interactionCodec4_checkCenteredWithLink(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  CYC(0x73eb, 0x73ee); A = W8(wLinkDeathTrigger);
  CYC(0x73ee, 0x73ef); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x73ef, 0x73f0); ret_effect(gb); return; } // ret nz
  CYC(0x73ef, 0x73f0);
  CYC(0x73f0, 0x73f3); A = W8(wLinkPushingDirection);
  CYC(0x73f3, 0x73f4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x73f4, 0x73f5); ret_effect(gb); return; } // ret nz
  CYC(0x73f4, 0x73f5);
  CYC(0x73f5, 0x73f8); A = W8(wGameKeysPressed);
  CYC(0x73f8, 0x73fa); alu_and(gb, 0x03); // BTN_A | BTN_B
  if (!(F & FZ)) { CYCT(0x73fa, 0x73fb); ret_effect(gb); return; } // ret nz
  CYC(0x73fa, 0x73fb);
  CYC(0x73fb, 0x73fd); B = 0x05;
  CYC(0x73fd, 0x7400); objectCheckCenteredWithLink_hook(gb); // jp
}

// ==================================================================================================
// INTERAC_PIRATE
// ==================================================================================================
void interactionCodec4_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x736a, 0x736c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x736c, 0x736d); A = mem_rd(gb, DE);
  {
    CYC(0x736d, 0x736e); push_effect(gb, 0x736e);
    uint16_t target = interactionCodec4_jump_table(gb);
    if (target == 0x73c1) goto state1;
    if (target == 0x73dd) goto state2;
    if (target == 0x7400) goto state3;
    if (target == 0x7439) goto state4;
    // target == 0x7378 falls through to state0
  }

  // interactionCodec4@state0
  CYC(0x7378, 0x737a); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x737a, 0x737b); A = mem_rd(gb, DE);
  {
    CYC(0x737b, 0x737c); push_effect(gb, 0x737c);
    uint16_t target = interactionCodec4_jump_table(gb);
    if (target == 0x739a) goto subid4Init;
    // target == 0x7386 falls through to subid0Init/1Init/2Init/3Init (aliased)
  }

  // interactionCodec4@subid0Init / @subid1Init / @subid2Init / @subid3Init
  CYC(0x7386, 0x7387); A = mem_rd(gb, DE);
  CYC(0x7387, 0x738a); SET_HL(0x73b7); // @scriptTable
  CYC(0x738a, 0x738b); interactionCodec4_add_double_index(gb, 0x738b);
  CYC(0x738b, 0x738c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x738c, 0x738d); H = mem_rd(gb, HL);
  CYC(0x738d, 0x738e); L = A;
  CALL_C(0x738e, interactionSetScript_hook, 0x2544, 0x7391);
  CALL_C(0x7391, interactionInitGraphics_hook, 0x15fb, 0x7394); // SWITCHES THREADS
  CALL_C(0x7394, objectSetVisiblec2_hook, 0x1e45, 0x7397);
  CYC(0x7397, 0x739a); interactionIncState_hook(gb); return; // jp

subid4Init: // interactionCodec4@subid4Init
  CALL_C(0x739a, getThisRoomFlags_hook, 0x197d, 0x739d);
  CYC(0x739d, 0x739f); alu_and(gb, 0x80); // ROOMFLAG_80
  if (!(F & FZ)) { CYCT(0x739f, 0x73a2); interactionDelete_hook(gb); return; } // jp nz
  CYC(0x739f, 0x73a2);
  CYC(0x73a2, 0x73a5); interactionCodec4_resetPushCounter(gb); // call
  CYC(0x73a5, 0x73a7); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x73a7, 0x73a9); A = 0x03;
  CYC(0x73a9, 0x73aa); mem_wr(gb, DE, A);
  CYC(0x73aa, 0x73ac); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x73ac, 0x73ad); A = mem_rd(gb, DE);
  CYC(0x73ad, 0x73b0); SET_HL(0x73b7); // @scriptTable
  CYC(0x73b0, 0x73b1); interactionCodec4_add_double_index(gb, 0x73b1);
  CYC(0x73b1, 0x73b2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x73b2, 0x73b3); H = mem_rd(gb, HL);
  CYC(0x73b3, 0x73b4); L = A;
  CYC(0x73b4, 0x73b7); interactionSetScript_hook(gb); return; // jp

state1: // interactionCodec4@state1
  CYC(0x73c1, 0x73c4); A = W8(wTmpcfc0_genericCutscene_state);
  CYC(0x73c4, 0x73c6); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(0x73c6, 0x73c9); goto jump; } // jp nz
  CYC(0x73c6, 0x73c9);
  CALL_C(0x73c9, interactionRunScript_hook, 0x2552, 0x73cc);
  CYC(0x73cc, 0x73cf); npcFaceLinkAndAnimate_hook(gb); return; // jp

jump: // interactionCodec4@jump
  CYC(0x73cf, 0x73d1); A = 0x02;
  CALL_C(0x73d1, interactionSetAnimation_hook, 0x262e, 0x73d4);
  CYC(0x73d4, 0x73d7); SET_BC(0xfe00); // -$200
  CALL_C(0x73d7, objectSetSpeedZ_hook, 0x239d, 0x73da);
  CYC(0x73da, 0x73dd); interactionIncState_hook(gb); return; // jp

state2: // interactionCodec4@state2
  CYC(0x73dd, 0x73df); C = 0x28;
  CALL_C(0x73df, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x73e2);
  if (!(F & FZ)) { CYCT(0x73e2, 0x73e3); ret_effect(gb); return; } // ret nz
  CYC(0x73e2, 0x73e3);
  CYC(0x73e3, 0x73e6); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x73e6, 0x73e8); mem_wr(gb, HL, mem_rd(gb, HL) | 0x02); // set 1,(hl)
  CYC(0x73e8, 0x73eb); interactionAnimate_hook(gb); return; // jp

state3: // interactionCodec4@state3
  CALL_C(0x7400, objectCheckCollidedWithLink_notDead_hook, 0x1c2e, 0x7403);
  if (!(F & FC)) { CYCT(0x7403, 0x7406); interactionCodec4_resetPushCounter(gb); } else { CYC(0x7403, 0x7406); } // call nc
  CYC(0x7406, 0x7409); interactionCodec4_checkCenteredWithLink(gb, 0x7409); // call
  if (!(F & FC)) { CYCT(0x7409, 0x740c); interactionCodec4_resetPushCounter(gb); } else { CYC(0x7409, 0x740c); } // call nc
  CYC(0x740c, 0x740d); H = D;
  CYC(0x740d, 0x740f); L = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x740f, 0x7410); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x7410, 0x7412); goto state4; } // jr nz
  CYC(0x7410, 0x7412);
  CYC(0x7412, 0x7414); A = 0x4f; // TREASURE_TOKAY_EYEBALL
  CALL_C(0x7414, checkTreasureObtained_hook, 0x1748, 0x7417);
  if (F & FC) { CYCT(0x7417, 0x7419); goto haveEyeball; } // jr c
  CYC(0x7417, 0x7419);
  CYC(0x7419, 0x741c); SET_BC(0x360d); // TX_360d
  CALL_C(0x741c, showText_hook, 0x1872, 0x741f);
  CYC(0x741f, 0x7421); interactionCodec4_resetPushCounter(gb); return; // jr

haveEyeball: // interactionCodec4@haveEyeball
  CALL_C(0x7421, checkLinkCollisionsEnabled_hook, 0x1d32, 0x7424);
  if (!(F & FC)) { CYCT(0x7424, 0x7426); interactionCodec4_resetPushCounter(gb); return; } // jr nc
  CYC(0x7424, 0x7426);
  CYC(0x7426, 0x7428); A = 0x81; // DISABLE_ALL_BUT_INTERACTIONS | DISABLE_LINK
  CYC(0x7428, 0x742b); W8(wDisabledObjects) = A;
  CYC(0x742b, 0x742e); W8(wMenuDisabled) = A;
  CYC(0x742e, 0x7430); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(0x7430, playSound_b00_hook, 0x0c98, 0x7433);
  CYC(0x7433, 0x7436); SET_HL(0x7ddd); // mainScripts.pirateSubid4Script_insertEyeball
  CALL_C(0x7436, interactionSetScript_hook, 0x2544, 0x7439);

state4: // interactionCodec4@state4
  CALL_C(0x7439, interactionRunScript_hook, 0x2552, 0x743c);
  if (!(F & FC)) { CYCT(0x743c, 0x743d); ret_effect(gb); return; } // ret nc
  CYC(0x743c, 0x743d);
  CYC(0x743d, 0x7440); interactionDelete_hook(gb); return; // jp
}
