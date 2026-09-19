#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCode9d_jump_table(GB *gb) {
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

// 0b:555e, called from interactionCode9d@state2 and @state3.
void interactionCode9d_checkCreateMusicNote_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x555e, 0x5561); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x5561, 0x5563); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(0x5563, 0x5564); ret_effect(gb); return; } // ret z
  CYC(0x5563, 0x5564);
  CYC(0x5564, 0x5567); A = mem_rd(gb, wFrameCounter);
  CYC(0x5567, 0x5569); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x5569, 0x556a); ret_effect(gb); return; } // ret nz
  CYC(0x5569, 0x556a);
  CALL_C(0x556a, getRandomNumber_hook, 0x043e, 0x556d);
  CYC(0x556d, 0x556f); alu_and(gb, 0x01);
  CYC(0x556f, 0x5572); SET_BC(0xf808);
  CYC(0x5572, 0x5575); objectCreateFloatingMusicNote_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_TOKKEY
// ==================================================================================================
void interactionCode9d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x54d8, 0x54da); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x54da, 0x54db); A = mem_rd(gb, DE);
  CYC(0x54db, 0x54dc); push_effect(gb, 0x54dc);
  switch (interactionCode9d_jump_table(gb)) {
    case 0x54e6: goto state0;
    case 0x54fa: goto state1;
    case 0x5538: goto state2;
    case 0x5546: goto state3;
    case 0x553e: goto state4;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x54e6, interactionInitGraphics_hook, 0x15fb, 0x54e9);
  CYC(0x54e9, 0x54eb); A = 0x2c;
  CALL_C(0x54eb, interactionSetHighTextIndex_hook, 0x253b, 0x54ee);
  CYC(0x54ee, 0x54f1); SET_HL(0x7b9d); // mainScripts.tokkeyScript
  CALL_C(0x54f1, interactionSetScript_hook, 0x2544, 0x54f4);
  CALL_C(0x54f4, objectSetVisible82_hook, 0x1e69, 0x54f7);
  CYC(0x54f7, 0x54fa); interactionIncState_hook(gb); return; // jp

state1:
  CYC(0x54fa, 0x54fd); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(0x54fd, 0x54ff); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(0x54ff, 0x5501); goto runScript; } // jr z
  CYC(0x54ff, 0x5501);
  CYC(0x5501, 0x5504); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(0x5504, 0x5506); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(0x5506, 0x5508); goto runScript; } // jr nz
  CYC(0x5506, 0x5508);
  CALL_C(0x5508, checkLinkCollisionsEnabled_hook, 0x1d32, 0x550b);
  if (!(F & FC)) { CYCT(0x550b, 0x550c); ret_effect(gb); return; } // ret nc
  CYC(0x550b, 0x550c);
  CYC(0x550c, 0x550f); A = mem_rd(gb, wActiveTilePos);
  CYC(0x550f, 0x5511); alu_cp(gb, 0x32);
  if (F & FZ) { CYCT(0x5511, 0x5513); goto l_5519; } // jr z
  CYC(0x5511, 0x5513);
  CYC(0x5513, 0x5516); SET_BC(0x2c05); // TX_2c05
  CYC(0x5516, 0x5519); showText_hook(gb); return; // jp

l_5519:
  CYC(0x5519, 0x551b); A = 0x3c; // 60
  CYC(0x551b, 0x551e); SET_BC(0xf810);
  CALL_C(0x551e, objectCreateExclamationMark_hook, 0x27e0, 0x5521);
  CYC(0x5521, 0x5524); SET_HL(0x7bb0); // mainScripts.tokkeyScript_justHeardTune
  CALL_C(0x5524, interactionSetScript_hook, 0x2544, 0x5527);
  CYC(0x5527, 0x552a); interactionIncState_hook(gb); return; // jp

runScript:
  CYC(0x552a, 0x552c); C = 0x20;
  CALL_C(0x552c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x552f);
  CALL_C(0x552f, interactionRunScript_hook, 0x2552, 0x5532);
  if (F & FC) { CYCT(0x5532, 0x5535); interactionDelete_hook(gb); return; } // jp c
  CYC(0x5532, 0x5535);
  CYC(0x5535, 0x5538); npcFaceLinkAndAnimate_hook(gb); return; // jp

state2:
  CALL_C(0x5538, interactionCode9d_checkCreateMusicNote_hook, 0x555e, 0x553b);
  CALL_C(0x553b, interactionAnimate_hook, 0x261b, 0x553e);
  // falls through into @state4 (also a direct jump-table target)

state4:
  CALL_C(0x553e, interactionRunScript_hook, 0x2552, 0x5541);
  CYC(0x5541, 0x5543); C = 0x20;
  CYC(0x5543, 0x5546); objectUpdateSpeedZ_paramC_hook(gb); return; // jp

state3:
  CALL_C(0x5546, interactionCode9d_checkCreateMusicNote_hook, 0x555e, 0x5549);
  CALL_C(0x5549, interactionRunScript_hook, 0x2552, 0x554c);
  CALL_C(0x554c, interactionAnimate_hook, 0x261b, 0x554f);
  CALL_C(0x554f, interactionAnimate_hook, 0x261b, 0x5552);
  CYC(0x5552, 0x5554); C = 0x60;
  CALL_C(0x5554, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x5557);
  if (!(F & FZ)) { CYCT(0x5557, 0x5558); ret_effect(gb); return; } // ret nz
  CYC(0x5557, 0x5558);
  CYC(0x5558, 0x555b); SET_BC(0xfe00); // -$200
  CYC(0x555b, 0x555e); objectSetSpeedZ_hook(gb); return; // jp
}
