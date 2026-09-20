#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t interactionCodead_jump_table(GB *gb) {
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

static void interactionCodead_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// 0b:63ba, called from zelda_state0@commonInit.
void zelda_loadScript_hook(GB *gb) {
  BASE(zelda_loadScript);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(b_+13); // @scriptTable
  CYC(b_+6, b_+7); interactionCodead_addDoubleIndex(gb, b_+7);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+8, b_+9); H = mem_rd(gb, HL);
  CYC(b_+9, b_+10); L = A;
  CYC(b_+10, b_+13); interactionSetScript_hook(gb); return; // jp
}

// 0b:6381, falls into from zelda_state0@commonInit's own tail call chain (see below).
void zelda_state1_hook(GB *gb) {
  BASE(zelda_state1);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodead_jump_table(gb));
    if (jt_ == b_+26) { goto animateAndRunScript; }
    else if (jt_ == b_+32) { goto runSubid2; }
    else if (jt_ == b_+42) { goto runSubid4; }
    else if (jt_ == b_+51) { goto faceLinkAndRunScript; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

animateAndRunScript:
  CALL_C(b_+26, interactionAnimate_hook, SYM(interactionAnimate), b_+29);
  CYC(b_+29, b_+32); interactionRunScript_hook(gb); return; // jp

runSubid2:
  CYC(b_+32, b_+34); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+34, b_+35); A = mem_rd(gb, DE);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (F & FZ) { CALL_C_CC(b_+36, interactionAnimate_hook, SYM(interactionAnimate), b_+39); } else { CYC(b_+36, b_+39); } // call z
  CYC(b_+39, b_+42); interactionRunScript_hook(gb); return; // jp

runSubid4:
  CALL_C(b_+42, interactionRunScript_hook, SYM(interactionRunScript), b_+45);
  if (!(F & FC)) { CYCT(b_+45, b_+48); interactionAnimateBasedOnSpeed_hook(gb); return; } // jp nc
  CYC(b_+45, b_+48);
  CYC(b_+48, b_+51); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; // jp

faceLinkAndRunScript:
  CALL_C(b_+51, interactionRunScript_hook, SYM(interactionRunScript), b_+54);
  CYC(b_+54, b_+57); npcFaceLinkAndAnimate_hook(gb); return; // jp
}

// 0b:62b1, called from interactionCodead@state0 (top-level jump table).
void zelda_state0_hook(GB *gb) {
  BASE(zelda_state0);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x01;
  CYC(b_+2, b_+3); mem_wr(gb, DE, A); // [state]
  CALL_C(b_+3, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+6);
  CALL_C(b_+6, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+9);
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (interactionCodead_jump_table(gb));
    if (jt_ == b_+175) { goto initSubid00; }
    else if (jt_ == b_+205) { goto commonInit; }
    else if (jt_ == b_+69) { goto initSubid03; }
    else if (jt_ == b_+35) { goto initSubid04; }
    else if (jt_ == b_+202) { goto commonInitWithExtraGraphics; }
    else if (jt_ == b_+83) { goto initSubid07; }
    else if (jt_ == b_+123) { goto initSubid08; }
    else if (jt_ == b_+149) { goto initSubid0a; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);

initSubid04:
  CALL_C(b_+35, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+38);
  if (F & FZ) { CYCT(b_+38, b_+41); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp z
  CYC(b_+38, b_+41);
  CYC(b_+41, b_+43); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(b_+43, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+46);
  if (!(F & FC)) { CYCT(b_+46, b_+49); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp nc
  CYC(b_+46, b_+49);
  CYC(b_+49, b_+51); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(b_+51, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+54);
  if (!(F & FZ)) { CYCT(b_+54, b_+57); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp nz
  CYC(b_+54, b_+57);
  CYC(b_+57, b_+58); H = D;
  CYC(b_+58, b_+60); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(b_+62, b_+64); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x08);
  CYC(b_+66, b_+69); goto commonInit; // jp

initSubid03:
  CYC(b_+69, b_+72); SET_BC((SYM(interactionCodeb6__treeDisappearanceFrames) + 2));
  CALL_C(b_+72, interactionSetPosition_hook, SYM(interactionSetPosition), b_+75);
  CYC(b_+75, b_+77); A = 0x01;
  CALL_C(b_+77, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+80);
  CYC(b_+80, b_+83); goto commonInit; // jp

initSubid07:
  CYC(b_+83, b_+85); A = 0x38; // GLOBALFLAG_GOT_RING_FROM_ZELDA
  CALL_C(b_+85, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+88);
  if (F & FZ) { CYCT(b_+88, b_+91); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp z
  CYC(b_+88, b_+91);
  CYC(b_+91, b_+93); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(b_+93, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+96);
  if (F & FC) { CYCT(b_+96, b_+99); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp c
  CYC(b_+96, b_+99);
  CYC(b_+99, b_+101); A = 0x11; // GLOBALFLAG_SAVED_NAYRU
  CALL_C(b_+101, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+104);
  CYC(b_+104, b_+106); A = 0x06; // <TX_0606
  if (!(F & FZ)) { CYCT(b_+106, b_+108); goto actAsGenericNpc; } // jr nz
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+110); A = 0x05; // <TX_0605

actAsGenericNpc:
  CYC(b_+110, b_+112); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(b_+112, b_+113); mem_wr(gb, DE, A);
  CYC(b_+113, b_+114); E = alu_inc8(gb, E);
  CYC(b_+114, b_+116); A = 0x06; // >TX_0600
  CYC(b_+116, b_+117); mem_wr(gb, DE, A);
  CYC(b_+117, b_+120); SET_HL((SYM(interactionCodeb6__state6) + 40)); // mainScripts.genericNpcScript
  CYC(b_+120, b_+123); interactionSetScript_hook(gb); return; // jp

initSubid08:
  CALL_C(b_+123, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+126);
  if (F & FZ) { CYCT(b_+126, b_+129); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp z
  CYC(b_+126, b_+129);
  CYC(b_+129, b_+131); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(b_+131, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+134);
  if (F & FZ) { CYCT(b_+134, b_+137); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp z
  CYC(b_+134, b_+137);
  CYC(b_+137, b_+139); A = 0x3a; // GLOBALFLAG_FLAME_OF_DESPAIR_LIT
  CALL_C(b_+139, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+142);
  if (!(F & FZ)) { CYCT(b_+142, b_+145); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp nz
  CYC(b_+142, b_+145);
  CYC(b_+145, b_+147); A = 0x0b; // <TX_060b
  CYCT(b_+147, b_+149); goto actAsGenericNpc; // jr

initSubid0a:
  CALL_C(b_+149, checkIsLinkedGame_hook, SYM(checkIsLinkedGame), b_+152);
  if (F & FZ) { CYCT(b_+152, b_+155); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp z
  CYC(b_+152, b_+155);
  CYC(b_+155, b_+157); A = 0x36; // TREASURE_MAKU_SEED
  CALL_C(b_+157, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+160);
  if (!(F & FC)) { CYCT(b_+160, b_+163); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp nc
  CYC(b_+160, b_+163);
  CYC(b_+163, b_+165); A = 0x33; // GLOBALFLAG_PRE_BLACK_TOWER_CUTSCENE_DONE
  CALL_C(b_+165, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+168);
  if (!(F & FZ)) { CYCT(b_+168, b_+171); interactionDeleteAndUnmarkSolidPosition_hook(gb); return; } // jp nz
  CYC(b_+168, b_+171);
  CYC(b_+171, b_+173); A = 0x0a; // <TX_060a
  CYCT(b_+173, b_+175); goto actAsGenericNpc; // jr

initSubid00:
  CALL_C(b_+175, getThisRoomFlags_hook, SYM(getThisRoomFlags), b_+178);
  CYC(b_+178, b_+180); alu_bit(gb, 7, A);
  if (F & FZ) { CYCT(b_+180, b_+182); goto commonInitWithExtraGraphics; } // jr z
  CYC(b_+180, b_+182);
  CYC(b_+182, b_+184); A = 0x01;
  CYC(b_+184, b_+187); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+187, b_+190); A = mem_rd(gb, wActiveMusic);
  CYC(b_+190, b_+191); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+191, b_+193); goto commonInitWithExtraGraphics; } // jr z
  CYC(b_+191, b_+193);
  CYC(b_+193, b_+194); alu_xor(gb, A);
  CYC(b_+194, b_+197); mem_wr(gb, wActiveMusic, A);
  CYC(b_+197, b_+199); A = 0x38; // MUS_ZELDA_SAVED
  CALL_C(b_+199, playSound_b00_hook, SYM(playSound_b00), b_+202);

commonInitWithExtraGraphics:
  CALL_C(b_+202, interactionLoadExtraGraphics_hook, SYM(interactionLoadExtraGraphics), b_+205);

commonInit:
  CALL_C(b_+205, zelda_loadScript_hook, SYM(zelda_loadScript), SYM(zelda_state1));
  zelda_state1_hook(gb); return;
}

// ==================================================================================================
// INTERAC_ZELDA
// ==================================================================================================
void interactionCodead_hook(GB *gb) {
  BASE(interactionCodead);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (interactionCodead_jump_table(gb));
    if (jt_ == SYM(zelda_state0)) { zelda_state0_hook(gb); return; }
    else if (jt_ == SYM(zelda_state1)) { zelda_state1_hook(gb); return; }
    else { hook_continue(gb, HL, sp0_); return; }
  } while (0);
}
