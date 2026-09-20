#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(interactionCode9d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+134, b_+137); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+137, b_+139); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+139, b_+140); ret_effect(gb); return; } // ret z
  CYC(b_+139, b_+140);
  CYC(b_+140, b_+143); A = mem_rd(gb, wFrameCounter);
  CYC(b_+143, b_+145); alu_and(gb, 0x0f);
  if (!(F & FZ)) { CYCT(b_+145, b_+146); ret_effect(gb); return; } // ret nz
  CYC(b_+145, b_+146);
  CALL_C(b_+146, getRandomNumber_hook, SYM(getRandomNumber), b_+149);
  CYC(b_+149, b_+151); alu_and(gb, 0x01);
  CYC(b_+151, b_+154); SET_BC(0xf808);
  CYC(b_+154, b_+157); objectCreateFloatingMusicNote_hook(gb); return; // jp
}

// ==================================================================================================
// INTERAC_TOKKEY
// ==================================================================================================
void interactionCode9d_hook(GB *gb) {
  BASE(interactionCode9d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCode9d_jump_table(gb));
    if (jt_ == b_+14) { goto state0; }
    else if (jt_ == b_+34) { goto state1; }
    else if (jt_ == b_+96) { goto state2; }
    else if (jt_ == b_+110) { goto state3; }
    else if (jt_ == b_+102) { goto state4; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

state0:
  CALL_C(b_+14, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+17);
  CYC(b_+17, b_+19); A = 0x2c;
  CALL_C(b_+19, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+22);
  CYC(b_+22, b_+25); SET_HL((SYM(interactiond7_makuSeed__state4Substate3__unlinkedGame) + 14)); // mainScripts.tokkeyScript
  CALL_C(b_+25, interactionSetScript_hook, SYM(interactionSetScript), b_+28);
  CALL_C(b_+28, objectSetVisible82_hook, SYM(objectSetVisible82), b_+31);
  CYC(b_+31, b_+34); interactionIncState_hook(gb); return; // jp

state1:
  CYC(b_+34, b_+37); A = mem_rd(gb, wTmpcfc0_genericCutscene_state);
  CYC(b_+37, b_+39); alu_bit(gb, 0, A);
  if (F & FZ) { CYCT(b_+39, b_+41); goto runScript; } // jr z
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+44); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+44, b_+46); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+46, b_+48); goto runScript; } // jr nz
  CYC(b_+46, b_+48);
  CALL_C(b_+48, checkLinkCollisionsEnabled_hook, SYM(checkLinkCollisionsEnabled), b_+51);
  if (!(F & FC)) { CYCT(b_+51, b_+52); ret_effect(gb); return; } // ret nc
  CYC(b_+51, b_+52);
  CYC(b_+52, b_+55); A = mem_rd(gb, wActiveTilePos);
  CYC(b_+55, b_+57); alu_cp(gb, 0x32);
  if (F & FZ) { CYCT(b_+57, b_+59); goto l_5519; } // jr z
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+62); SET_BC(0x2c05); // TX_2c05
  CYC(b_+62, b_+65); showText_hook(gb); return; // jp

l_5519:
  CYC(b_+65, b_+67); A = 0x3c; // 60
  CYC(b_+67, b_+70); SET_BC(0xf810);
  CALL_C(b_+70, objectCreateExclamationMark_hook, SYM(objectCreateExclamationMark), b_+73);
  CYC(b_+73, b_+76); SET_HL((SYM(interactiond7_essence) + 4)); // mainScripts.tokkeyScript_justHeardTune
  CALL_C(b_+76, interactionSetScript_hook, SYM(interactionSetScript), b_+79);
  CYC(b_+79, b_+82); interactionIncState_hook(gb); return; // jp

runScript:
  CYC(b_+82, b_+84); C = 0x20;
  CALL_C(b_+84, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+87);
  CALL_C(b_+87, interactionRunScript_hook, SYM(interactionRunScript), b_+90);
  if (F & FC) { CYCT(b_+90, b_+93); interactionDelete_hook(gb); return; } // jp c
  CYC(b_+90, b_+93);
  CYC(b_+93, b_+96); npcFaceLinkAndAnimate_hook(gb); return; // jp

state2:
  CALL_C(b_+96, interactionCode9d_checkCreateMusicNote_hook, b_+134, b_+99);
  CALL_C(b_+99, interactionAnimate_hook, SYM(interactionAnimate), b_+102);
  // falls through into @state4 (also a direct jump-table target)

state4:
  CALL_C(b_+102, interactionRunScript_hook, SYM(interactionRunScript), b_+105);
  CYC(b_+105, b_+107); C = 0x20;
  CYC(b_+107, b_+110); objectUpdateSpeedZ_paramC_hook(gb); return; // jp

state3:
  CALL_C(b_+110, interactionCode9d_checkCreateMusicNote_hook, b_+134, b_+113);
  CALL_C(b_+113, interactionRunScript_hook, SYM(interactionRunScript), b_+116);
  CALL_C(b_+116, interactionAnimate_hook, SYM(interactionAnimate), b_+119);
  CALL_C(b_+119, interactionAnimate_hook, SYM(interactionAnimate), b_+122);
  CYC(b_+122, b_+124); C = 0x60;
  CALL_C(b_+124, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+127);
  if (!(F & FZ)) { CYCT(b_+127, b_+128); ret_effect(gb); return; } // ret nz
  CYC(b_+127, b_+128);
  CYC(b_+128, b_+131); SET_BC(0xfe00); // -$200
  CYC(b_+131, b_+134); objectSetSpeedZ_hook(gb); return; // jp
}
