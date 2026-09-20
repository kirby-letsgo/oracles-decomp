#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interaction6e_subid00), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interaction6e_subid00), (from), (to), true)

static uint16_t nayruSavedCutscene_jump_table(GB *gb) {
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

static void nayruSavedCutscene_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Nayru waking up after being freed from possession
void interaction6e_subid00_hook(GB *gb) {
  BASE(interaction6e_subid00);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = nayruSavedCutscene_jump_table(gb);
    if (target == b_+82) goto state1;
    if (target == b_+96) goto state2;
  }

  CALL_C(b_+8, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+11);
  CALL_C(b_+11, interactionIncState_hook, SYM(interactionIncState), b_+14);
  CYC(b_+14, b_+16); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x58);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x78);
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+24, b_+26); mem_wr(gb, HL, 0x0a); // SPEED_40
  CYC(b_+26, b_+28); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+28, b_+30); A = 0x01;
  CYC(b_+30, b_+31); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+31, b_+32); mem_wr(gb, HL, A);
  CYC(b_+32, b_+35); W8(wLoadedTreeGfxIndex) = A;
  CYC(b_+35, b_+38); SET_HL(w1Link_direction); // w1Link.direction
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x00); // DIR_UP
  CYC(b_+40, b_+42); L = 0x0b; // <w1Link.yh
  CYC(b_+42, b_+44); mem_wr(gb, HL, 0x64);
  CYC(b_+44, b_+46); L = 0x0d; // <w1Link.xh
  CYC(b_+46, b_+48); mem_wr(gb, HL, 0x78);
  CYC(b_+48, b_+51); SET_HL(wTmpcfc0_armosStatue_killedArmosPositions); // wTmpcfc0.genericCutscene.cfd0
  CYC(b_+51, b_+53); B = 0x10;
  CALL_C(b_+53, clearMemory_hook, SYM(clearMemory), b_+56);
  CALL_C(b_+56, setCameraFocusedObjectToLink_hook, SYM(setCameraFocusedObjectToLink), b_+59);
  CALL_C(b_+59, resetCamera_hook, SYM(resetCamera), b_+62);
  CYC(b_+62, b_+64); A = H8(hActiveObject);
  CYC(b_+64, b_+65); D = A;
  CALL_C(b_+65, fadeinFromWhite_hook, SYM(fadeinFromWhite), b_+68);
  CYC(b_+68, b_+70); A = 0x0a;
  CALL_C(b_+70, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+73);
  CALL_C(b_+73, objectSetVisible82_hook, SYM(objectSetVisible82), b_+76);
  CYC(b_+76, b_+79); SET_HL((SYM(miscPuzzles_subid1a) + 19)); // mainScripts.interaction6e_subid00Script
  CYC(b_+79, b_+82); interactionSetScript_hook(gb); return; // jp

state1:
  CALL_C(b_+82, interactionRunScript_hook, SYM(interactionRunScript), b_+85);
  if (!(F & FC)) { CYCT(b_+85, b_+88); interactionAnimate_hook(gb); return; } // jp nc
  CYC(b_+85, b_+88);
  CALL_C(b_+88, interactionIncState_hook, SYM(interactionIncState), b_+91);
  CYC(b_+91, b_+93); A = 0x04;
  CYC(b_+93, b_+96); fadeoutToWhiteWithDelay_hook(gb); return; // jp

state2:
  CYC(b_+96, b_+99); A = W8(wPaletteThread_mode);
  CYC(b_+99, b_+100); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+100); return; } // ret nz
  CYC(b_+100, b_+101);
  CYC(b_+101, b_+103); A = 0x19; // GLOBALFLAG_BEAT_POSSESSED_NAYRU
  CALL_C(b_+103, setGlobalFlag_hook, SYM(setGlobalFlag), b_+106);
  CYC(b_+106, b_+108); A = 0x0c; // CUTSCENE_NAYRU_WARP_TO_MAKU_TREE
  CYC(b_+108, b_+111); W8(wCutsceneTrigger) = A;
  CYC(b_+111, SYM(interaction6e_subid01)); interactionDelete_hook(gb); return; // jp
}

// Queen Ambi
void interaction6e_subid01_hook(GB *gb) {
  BASE(interaction6e_subid01);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = nayruSavedCutscene_jump_table(gb);
    if (target == b_+37) goto state1;
    if (target == b_+62) goto state2;
    if (target == b_+90) goto state3;
    if (target == b_+146) goto state4;
  }

  CALL_C(b_+12, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+15);
  CALL_C(b_+15, interactionIncState_hook, SYM(interactionIncState), b_+18);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+27); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(b_+27, b_+28); mem_wr(gb, HL, A);
  CALL_C(b_+28, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+31);
  CYC(b_+31, b_+34); SET_HL((SYM(miscPuzzles_subid1d) + 31)); // mainScripts.interaction6e_subid01Script_part1
  CYC(b_+34, b_+37); interactionSetScript_hook(gb); return; // jp

state1:
  CYC(b_+37, b_+39); C = 0x30;
  CALL_C(b_+39, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+42);
  if (!(F & FZ)) { RET_TAKEN(b_+42); return; } // ret nz
  CYC(b_+42, b_+43);
  CALL_C(b_+43, interactionRunScript_hook, SYM(interactionRunScript), b_+46);
  if (!(F & FC)) { CYCT(b_+46, b_+48); goto animate; } // jr nc
  CYC(b_+46, b_+48);
  CALL_C(b_+48, interactionIncState_hook, SYM(interactionIncState), b_+51);
  CYC(b_+51, b_+53); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+53, b_+55); mem_wr(gb, HL, 0xf4); // 244
  // Use "direction" variable temporarily as "animation"
  CYC(b_+55, b_+57); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x05);

animate:
  CYC(b_+59, b_+62); interactionAnimate_hook(gb); return; // jp

state2:
  CALL_C(b_+62, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+65);
  if (F & FZ) { CYCT(b_+65, b_+67); goto l559f; } // jr z
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+68); A = mem_rd(gb, HL);
  CYC(b_+68, b_+70); alu_cp(gb, 0xc1);
  if (!(F & FC)) { CYCT(b_+70, b_+72); goto animate; } // jr nc
  CYC(b_+70, b_+72);
  CYC(b_+72, b_+74); alu_and(gb, 0x1f);
  if (!(F & FZ)) { RET_TAKEN(b_+74); return; } // ret nz
  CYC(b_+74, b_+75);
  CYC(b_+75, b_+77); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+77, b_+78); A = mem_rd(gb, HL);
  CYC(b_+78, b_+80); alu_xor(gb, 0x02);
  CYC(b_+80, b_+81); mem_wr(gb, HL, A);
  CYC(b_+81, b_+83); goto setAnimation; // jr

l559f:
  CYC(b_+83, b_+84); L = E;
  CYC(b_+84, b_+85); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+85, b_+87); A = 0x06;

setAnimation:
  CYC(b_+87, b_+90); interactionSetAnimation_hook(gb); return; // jp

state3:
  CYC(b_+90, b_+93); A = W8(wTmpcfc0_genericCutscene_cfd0);
  CYC(b_+93, b_+95); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+95, b_+97); goto l55bc; } // jr z
  CYC(b_+95, b_+97);
  CYC(b_+97, b_+100); A = W8(wFrameCounter);
  CYC(b_+100, b_+101); alu_rrca(gb);
  if (F & FC) { RET_TAKEN(b_+101); return; } // ret c
  CYC(b_+101, b_+102);
  CYC(b_+102, b_+104); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+104, b_+105); A = mem_rd(gb, DE);
  CYC(b_+105, b_+106); A = alu_inc8(gb, A);
  CYC(b_+106, b_+108); alu_and(gb, 0x01);
  CYC(b_+108, b_+110); alu_add(gb, 0x78);
  CYC(b_+110, b_+111); mem_wr(gb, DE, A);
  RET(b_+111); return; // ret

l55bc:
  CALL_C(b_+112, interactionIncState_hook, SYM(interactionIncState), b_+115);
  CYC(b_+115, b_+117); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+117, b_+119); mem_wr(gb, HL, 0x78);
  CYC(b_+119, b_+121); L = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+121, b_+123); A = 0x06;
  CYC(b_+123, b_+124); mem_wr(gb, HL, A); SET_HL(HL - 1); // ldd (hl),a
  CYC(b_+124, b_+125); mem_wr(gb, HL, A);
  CYC(b_+125, b_+128); SET_HL((SYM(miscPuzzles_subid1f__state1) + 4)); // mainScripts.interaction6e_subid01Script_part2
  CALL_C(b_+128, interactionSetScript_hook, SYM(interactionSetScript), b_+131);
  CYC(b_+131, b_+133); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+133, playSound_b00_hook, SYM(playSound_b00), b_+136);
  CYC(b_+136, b_+138); A = 0x21; // MUS_DISASTER
  CALL_C(b_+138, playSound_b00_hook, SYM(playSound_b00), b_+141);
  CYC(b_+141, b_+143); A = 0x04;
  CYC(b_+143, b_+146); fadeinFromWhiteWithDelay_hook(gb); return; // jp

state4:
  CYC(b_+146, b_+149); A = W8(wPaletteThread_mode);
  CYC(b_+149, b_+150); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+150, b_+152); goto l55ed; } // jr nz
  CYC(b_+150, b_+152);
  CYC(b_+152, b_+154); C = 0x30;
  CALL_C(b_+154, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+157);
  if (!(F & FZ)) { RET_TAKEN(b_+157); return; } // ret nz
  CYC(b_+157, b_+158);
  CALL_C(b_+158, interactionRunScript_hook, SYM(interactionRunScript), b_+161);

l55ed:
  CYC(b_+161, SYM(interaction6e_subid02)); interactionAnimate_hook(gb); return; // jp
}

// Ghost Veran
void interaction6e_subid02_hook(GB *gb) {
  BASE(interaction6e_subid02);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  {
    CYC(b_+1, b_+2); push_effect(gb, b_+2);
    uint16_t target = nayruSavedCutscene_jump_table(gb);
    if (target == b_+42) goto state1;
    if (target == b_+89) goto state2;
    if (target == b_+142) goto state3;
  }

  CALL_C(b_+10, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+13);
  CALL_C(b_+13, interactionIncState_hook, SYM(interactionIncState), b_+16);
  CYC(b_+16, b_+18); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+18, b_+20); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(b_+20, b_+22); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+22, b_+24); mem_wr(gb, HL, 0x0e);
  CYC(b_+24, b_+26); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+26, b_+28); mem_wr(gb, HL, 0x48);
  CYC(b_+28, b_+31); SET_BC(b_+28); // TX_560c
  CALL_C(b_+31, showText_hook, SYM(showText), b_+34);
  CYC(b_+34, b_+36); A = 0xf0; // SNDCTRL_STOPMUSIC
  CALL_C(b_+36, playSound_b00_hook, SYM(playSound_b00), b_+39);
  CYC(b_+39, b_+42); objectSetVisible81_hook(gb); return; // jp

state1:
  CYC(b_+42, b_+44); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+47); alu_cp(gb, 0x48);
  CYC(b_+47, b_+49); A = 0xa4; // SND_BEAM
  if (F & FZ) { CALL_C_CC(b_+49, playSound_b00_hook, SYM(playSound_b00), b_+52); } else { CYC(b_+49, b_+52); } // call z
  CALL_C(b_+52, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+55);
  if (!(F & FZ)) { CYCT(b_+55, b_+57); goto applySpeedAndAnimate; } // jr nz
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0xac);
  CYC(b_+59, b_+61); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+61, b_+62); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+62, b_+64); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x16);
  // Link moves up, while facing down
  CYC(b_+66, b_+69); SET_HL(w1Link_direction); // w1Link.direction
  CYC(b_+69, b_+71); mem_wr(gb, HL, 0x02); // DIR_DOWN
  CYC(b_+71, b_+72); L = alu_inc8(gb, L);
  CYC(b_+72, b_+74); mem_wr(gb, HL, 0x00); // ANGLE_UP
  CYC(b_+74, b_+76); A = 0x0b; // LINK_STATE_FORCE_MOVEMENT
  CYC(b_+76, b_+79); W8(wLinkForceState) = A;
  CYC(b_+79, b_+81); A = 0x04;
  CYC(b_+81, b_+84); W8(wLinkStateParameter) = A;
  CYC(b_+84, b_+86); A = 0xc9; // SND_CIRCLING
  CALL_C(b_+86, playSound_b00_hook, SYM(playSound_b00), b_+89);

state2:
  CALL_C(b_+89, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+92);
  if (F & FZ) { CYCT(b_+92, b_+94); goto beginPossessingAmbi; } // jr z
  CYC(b_+92, b_+94);
  CYC(b_+94, b_+95); A = mem_rd(gb, HL);
  CYC(b_+95, b_+96); push_effect(gb, AF);
  CYC(b_+96, b_+98); alu_cp(gb, 0x56);
  CYC(b_+98, b_+100); A = 0xc9; // SND_CIRCLING
  if (F & FZ) { CALL_C_CC(b_+100, playSound_b00_hook, SYM(playSound_b00), b_+103); } else { CYC(b_+100, b_+103); } // call z
  CYC(b_+103, b_+104); SET_AF(pop_effect(gb));
  CYC(b_+104, b_+105); alu_rrca(gb);
  CYC(b_+105, b_+107); E = INTERACTION_BASE + OBJ_ANGLE;
  if (!(F & FC)) { CYCT(b_+107, b_+109); goto l5662; } // jr nc
  CYC(b_+107, b_+109);
  CYC(b_+109, b_+110); A = mem_rd(gb, DE);
  CYC(b_+110, b_+111); A = alu_dec8(gb, A);
  CYC(b_+111, b_+113); alu_and(gb, 0x1f);
  CYC(b_+113, b_+114); mem_wr(gb, DE, A);

l5662:
  CYC(b_+114, b_+116); A = 0x10;
  CYC(b_+116, b_+119); SET_BC((SYM(interaction97_subid01__positions) + 161));
  CALL_C(b_+119, objectSetPositionInCircleArc_hook, SYM(objectSetPositionInCircleArc), b_+122);
  CYC(b_+122, b_+125); interactionAnimate_hook(gb); return; // jp

beginPossessingAmbi:
  CYC(b_+125, b_+127); mem_wr(gb, HL, 0x50);
  CYC(b_+127, b_+128); L = E;
  CYC(b_+128, b_+129); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]++
  CYC(b_+129, b_+131); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+131, b_+133); mem_wr(gb, HL, 0x05); // SPEED_20
  CYC(b_+133, b_+135); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x10); // ANGLE_DOWN
  CYC(b_+137, b_+139); A = 0x63; // SND_BOSS_DAMAGE
  CALL_C(b_+139, playSound_b00_hook, SYM(playSound_b00), b_+142);

state3:
  CALL_C(b_+142, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+145);
  if (!(F & FZ)) { CYCT(b_+145, b_+147); goto l568d; } // jr nz
  CYC(b_+145, b_+147);
  CYC(b_+147, b_+149); A = 0x07;
  CYC(b_+149, b_+152); W8(wTmpcfc0_genericCutscene_cfd0) = A;
  CYC(b_+152, b_+154); A = 0x04;
  CYC(b_+154, b_+157); interactionDelete_hook(gb); return; // jp

l568d:
  CYC(b_+157, b_+159); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+159, b_+160); A = mem_rd(gb, HL);
  CYC(b_+160, b_+162); alu_xor(gb, 0x80);
  CYC(b_+162, b_+163); mem_wr(gb, HL, A);

applySpeedAndAnimate:
  CALL_C(b_+163, objectApplySpeed_hook, SYM(objectApplySpeed), b_+166);
  CYC(b_+166, SYM(interaction6e_subid03)); interactionAnimate_hook(gb); return; // jp
}

void interaction6e_runScriptAndAnimate_hook(GB *gb) {
  BASE(interaction6e_runScriptAndAnimate);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, SYM(interaction6e_initRalph)); interactionAnimate_hook(gb); return; // jp
}

void interaction6e_initRalph_hook(GB *gb) {
  BASE(interaction6e_initRalph);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, interactionIncState_hook, SYM(interactionIncState), b_+6);
  CYC(b_+6, b_+8); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+8, b_+10); mem_wr(gb, HL, 0x50); // SPEED_200
  CALL_C(b_+10, objectSetVisible82_hook, SYM(objectSetVisible82), b_+13);
  CYC(b_+13, b_+16); SET_HL((SYM(miscPuzzles_subid1f__state2) + 10)); // mainScripts.interaction6e_subid03Script
  CYC(b_+16, SYM(interaction6e_subid04)); interactionSetScript_hook(gb); return; // jp
}

// Ralph
void interaction6e_subid03_hook(GB *gb) {
  BASE(interaction6e_subid03);
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+2, SYM(interaction6e_runScriptAndAnimate)); interaction6e_initRalph_hook(gb); return; } // jr z
  CYC(b_+2, SYM(interaction6e_runScriptAndAnimate));
  interaction6e_runScriptAndAnimate_hook(gb); return;
}

// Guards that run into the room
void interaction6e_subid04_hook(GB *gb) {
  BASE(interaction6e_subid04);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+1); A = mem_rd(gb, DE);
  CYC(b_+1, b_+2); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+2, b_+4); interaction6e_runScriptAndAnimate_hook(gb); return; } // jr nz
  CYC(b_+2, b_+4);
  CALL_C(b_+4, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+7);
  CALL_C(b_+7, interactionIncState_hook, SYM(interactionIncState), b_+10);
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x3c); // SPEED_180
  CYC(b_+14, b_+16); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0xb0);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x78);
  CALL_C(b_+22, objectSetVisible82_hook, SYM(objectSetVisible82), b_+25);
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+31); SET_HL(b_+38); // @scriptTable
  CYC(b_+31, b_+32); nayruSavedCutscene_add_double_index(gb, b_+32);
  CYC(b_+32, b_+33); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+33, b_+34); H = mem_rd(gb, HL);
  CYC(b_+34, b_+35); L = A;
  CYC(b_+35, b_+38); interactionSetScript_hook(gb); return; // jp
}

// INTERAC_NAYRU_SAVED_CUTSCENE
void interactionCode6e_hook(GB *gb) {
  BASE(interactionCode6e);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_STATE;
  {
    CYC(b_+5, b_+6); push_effect(gb, b_+6);
    uint16_t target = nayruSavedCutscene_jump_table(gb);
    if (target == SYM(interaction6e_subid00)) { interaction6e_subid00_hook(gb); return; }
    if (target == SYM(interaction6e_subid01)) { interaction6e_subid01_hook(gb); return; }
    if (target == SYM(interaction6e_subid02)) { interaction6e_subid02_hook(gb); return; }
    if (target == SYM(interaction6e_subid03)) { interaction6e_subid03_hook(gb); return; }
    interaction6e_subid04_hook(gb); return; // target == 0x56b6
  }
}
