#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCodec8), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCodec8), (from), (to), true)

// ref/oracles-disasm/object_code/ages/interactions/tingle.s (interactionCodec8 /
// INTERAC_TINGLE), bank 0x0b.
//
// Variables:
//   var3d: Satchel level (minus one); used by script.
//   var3e: Nonzero if Link has 3 seed types or more
//   var3f: Signal for the script, set to 1 when his "kooloo-limpah" animation ends

static uint16_t interactionCodec8_jump_table(GB *gb) {
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

// ==================================================================================================
// INTERAC_TINGLE
// ==================================================================================================
void interactionCodec8_hook(GB *gb) {
  BASE(interactionCodec8);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCodec8_jump_table(gb);
    if (target == b_+77) goto state1;
    if (target == b_+111) goto state2;
    if (target == b_+78) goto state3;
    if (target == b_+124) goto state4;
    // target == 0x7550 falls through to state0
  }

  // interactionCodec8@state0
  CYC(b_+14, b_+16); A = 0x01;
  CYC(b_+16, b_+17); mem_wr(gb, DE, A); // [state]
  CALL_C(b_+17, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+20); // SWITCHES THREADS
  CALL_C(b_+20, interactionSetAlwaysUpdateBit_hook, SYM(interactionSetAlwaysUpdateBit), b_+23);
  CALL_C(b_+23, objectSetVisiblec0_hook, SYM(objectSetVisiblec0), b_+26);
  CYC(b_+26, b_+28); A = 0x1e; // >TX_1e00
  CALL_C(b_+28, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+31);
  CYC(b_+31, b_+33); A = 0x06;
  CALL_C(b_+33, objectSetCollideRadius_hook, SYM(objectSetCollideRadius), b_+36);
  CYC(b_+36, b_+39); SET_BC((SYM(objectCheckCenteredWithLink) + 18)); // TREASURE_EMBER_SEEDS, 0

checkNextSeed: // interactionCodec8@checkNextSeed
  CYC(b_+39, b_+40); A = B;
  CALL_C(b_+40, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+43);
  CYC(b_+43, b_+45); A = 0x00;
  CYC(b_+45, b_+46); alu_rla(gb);
  CYC(b_+46, b_+47); alu_add(gb, C);
  CYC(b_+47, b_+48); C = A;
  CYC(b_+48, b_+49); B = alu_inc8(gb, B);
  CYC(b_+49, b_+50); A = B;
  CYC(b_+50, b_+52); alu_cp(gb, 0x25); // TREASURE_MYSTERY_SEEDS+1
  if (!(F & FZ)) { CYCT(b_+52, b_+54); goto checkNextSeed; } // jr nz
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+55); A = C;
  CYC(b_+55, b_+57); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(b_+57, b_+59); goto l7580; } // jr c
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+61, b_+62); mem_wr(gb, DE, A);

l7580:
  CALL_C(b_+62, getFreePartSlot_hook, SYM(getFreePartSlot), b_+65);
  if (!(F & FZ)) { CYCT(b_+65, b_+66); ret_effect(gb); return; } // ret nz
  CYC(b_+65, b_+66);
  CYC(b_+66, b_+68); mem_wr(gb, HL, 0x44); // PART_TINGLE_BALLOON
  CALL_C(b_+68, objectCopyPosition_hook, SYM(objectCopyPosition), b_+71);
  CYC(b_+71, b_+73); L = 0xd6; // Part.relatedObj1
  CYC(b_+73, b_+75); A = 0x40; // Interaction.start (used as a related-object type marker)
  CYC(b_+75, b_+76); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+76, b_+77); mem_wr(gb, HL, D);

state1: // interactionCodec8@state1
  CYC(b_+77, b_+78); ret_effect(gb); return; // ret

state3: // interactionCodec8@state3
  CYC(b_+78, b_+80); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+80, b_+81); A = mem_rd(gb, DE);
  CYC(b_+81, b_+82); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+82, b_+85); interactionDecCounter1_hook(gb); return; } // jp nz
  CYC(b_+82, b_+85);
  CYC(b_+85, b_+87); C = 0x10;
  CALL_C(b_+87, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+90);
  if (!(F & FZ)) { CYCT(b_+90, b_+91); ret_effect(gb); return; } // ret nz
  CYC(b_+90, b_+91);
  CYC(b_+91, b_+93); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(b_+93, objectAddToAButtonSensitiveObjectList_hook, SYM(objectAddToAButtonSensitiveObjectList), b_+96);
  CYC(b_+96, b_+99); SET_HL((SYM(interactionCoded8__subid1Script) + 84)); // mainScripts.tingleScript
  CALL_C(b_+99, interactionSetScript_hook, SYM(interactionSetScript), b_+102);
  CYC(b_+102, b_+104); A = 0x04;
  CYC(b_+104, b_+106); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+106, b_+107); mem_wr(gb, DE, A);
  CYC(b_+107, b_+109); A = 0x01;
  CYC(b_+109, b_+111); goto setAnimation; // jr

state2: // interactionCodec8@state2
  CYC(b_+111, b_+113); A = 0x03;
  CYC(b_+113, b_+114); mem_wr(gb, DE, A); // [state]
  CYC(b_+114, b_+116); A = 15;
  CYC(b_+116, b_+118); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+118, b_+119); mem_wr(gb, DE, A);
  CYC(b_+119, b_+121); A = 0x02;

setAnimation: // interactionCodec8@setAnimation
  CYC(b_+121, b_+124); interactionSetAnimation_hook(gb); return; // jp

state4: // interactionCodec8@state4
  CYC(b_+124, b_+126); A = 0x19; // TREASURE_SEED_SATCHEL
  CALL_C(b_+126, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+129);
  CYC(b_+129, b_+131); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(b_+131, b_+132); A = alu_dec8(gb, A);
  CYC(b_+132, b_+133); mem_wr(gb, DE, A);
  CALL_C(b_+133, interactionRunScript_hook, SYM(interactionRunScript), b_+136);
  CALL_C(b_+136, interactionAnimateAsNpc_hook, SYM(interactionAnimateAsNpc), b_+139);
  CYC(b_+139, b_+141); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+143); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+143, b_+145); goto label_0b_330; } // jr nc
  CYC(b_+143, b_+145);
  CYC(b_+145, b_+148); SET_BC(0xfe00); // -$200
  CALL_C(b_+148, objectSetSpeedZ_hook, SYM(objectSetSpeedZ), b_+151);
  CYC(b_+151, b_+154); SET_BC(0xe800);
  CALL_C(b_+154, objectCreateSparkle_hook, SYM(objectCreateSparkle), b_+157);
  CYC(b_+157, b_+159); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+159, b_+161); mem_wr(gb, HL, 0x10);
  CYC(b_+161, b_+164); SET_BC(0xf008);
  CALL_C(b_+164, objectCreateSparkle_hook, SYM(objectCreateSparkle), b_+167);
  CYC(b_+167, b_+169); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x10);
  CYC(b_+171, b_+174); SET_BC(0xf0f8);
  CALL_C(b_+174, objectCreateSparkle_hook, SYM(objectCreateSparkle), b_+177);
  CYC(b_+177, b_+179); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+179, b_+181); mem_wr(gb, HL, 0x10);

label_0b_330: // interactionCodec8@label_0b_330
  CYC(b_+181, b_+183); C = 0x20;
  CALL_C(b_+183, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+186);
  if (!(F & FZ)) { CYCT(b_+186, b_+187); ret_effect(gb); return; } // ret nz
  CYC(b_+186, b_+187);
  CYC(b_+187, b_+189); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+189, b_+190); A = mem_rd(gb, DE);
  CYC(b_+190, b_+191); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+191, b_+192); ret_effect(gb); return; } // ret nc
  CYC(b_+191, b_+192);
  CYC(b_+192, b_+193); alu_xor(gb, A);
  CYC(b_+193, b_+195); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+195, b_+196); mem_wr(gb, DE, A);
  CYC(b_+196, b_+198); A = 0x01;
  CYC(b_+198, SYM(interactionCodec9)); goto setAnimation; // jr
}
