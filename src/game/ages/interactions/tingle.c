#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x7542, 0x7544); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x7544, 0x7545); A = mem_rd(gb, DE);
  {
    CYC(0x7545, 0x7546); push_effect(gb, 0x7546);
    uint16_t target = interactionCodec8_jump_table(gb);
    if (target == 0x758f) goto state1;
    if (target == 0x75b1) goto state2;
    if (target == 0x7590) goto state3;
    if (target == 0x75be) goto state4;
    // target == 0x7550 falls through to state0
  }

  // interactionCodec8@state0
  CYC(0x7550, 0x7552); A = 0x01;
  CYC(0x7552, 0x7553); mem_wr(gb, DE, A); // [state]
  CALL_C(0x7553, interactionInitGraphics_hook, 0x15fb, 0x7556); // SWITCHES THREADS
  CALL_C(0x7556, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x7559);
  CALL_C(0x7559, objectSetVisiblec0_hook, 0x1e33, 0x755c);
  CYC(0x755c, 0x755e); A = 0x1e; // >TX_1e00
  CALL_C(0x755e, interactionSetHighTextIndex_hook, 0x253b, 0x7561);
  CYC(0x7561, 0x7563); A = 0x06;
  CALL_C(0x7563, objectSetCollideRadius_hook, 0x24a1, 0x7566);
  CYC(0x7566, 0x7569); SET_BC(0x2000); // TREASURE_EMBER_SEEDS, 0

checkNextSeed: // interactionCodec8@checkNextSeed
  CYC(0x7569, 0x756a); A = B;
  CALL_C(0x756a, checkTreasureObtained_hook, 0x1748, 0x756d);
  CYC(0x756d, 0x756f); A = 0x00;
  CYC(0x756f, 0x7570); alu_rla(gb);
  CYC(0x7570, 0x7571); alu_add(gb, C);
  CYC(0x7571, 0x7572); C = A;
  CYC(0x7572, 0x7573); B = alu_inc8(gb, B);
  CYC(0x7573, 0x7574); A = B;
  CYC(0x7574, 0x7576); alu_cp(gb, 0x25); // TREASURE_MYSTERY_SEEDS+1
  if (!(F & FZ)) { CYCT(0x7576, 0x7578); goto checkNextSeed; } // jr nz
  CYC(0x7576, 0x7578);
  CYC(0x7578, 0x7579); A = C;
  CYC(0x7579, 0x757b); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x757b, 0x757d); goto l7580; } // jr c
  CYC(0x757b, 0x757d);
  CYC(0x757d, 0x757f); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(0x757f, 0x7580); mem_wr(gb, DE, A);

l7580:
  CALL_C(0x7580, getFreePartSlot_hook, 0x3e8e, 0x7583);
  if (!(F & FZ)) { CYCT(0x7583, 0x7584); ret_effect(gb); return; } // ret nz
  CYC(0x7583, 0x7584);
  CYC(0x7584, 0x7586); mem_wr(gb, HL, 0x44); // PART_TINGLE_BALLOON
  CALL_C(0x7586, objectCopyPosition_hook, 0x2242, 0x7589);
  CYC(0x7589, 0x758b); L = 0xd6; // Part.relatedObj1
  CYC(0x758b, 0x758d); A = 0x40; // Interaction.start (used as a related-object type marker)
  CYC(0x758d, 0x758e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x758e, 0x758f); mem_wr(gb, HL, D);

state1: // interactionCodec8@state1
  CYC(0x758f, 0x7590); ret_effect(gb); return; // ret

state3: // interactionCodec8@state3
  CYC(0x7590, 0x7592); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x7592, 0x7593); A = mem_rd(gb, DE);
  CYC(0x7593, 0x7594); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x7594, 0x7597); interactionDecCounter1_hook(gb); return; } // jp nz
  CYC(0x7594, 0x7597);
  CYC(0x7597, 0x7599); C = 0x10;
  CALL_C(0x7599, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x759c);
  if (!(F & FZ)) { CYCT(0x759c, 0x759d); ret_effect(gb); return; } // ret nz
  CYC(0x759c, 0x759d);
  CYC(0x759d, 0x759f); E = INTERACTION_BASE + OBJ_PRESSED_A_BUTTON;
  CALL_C(0x759f, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x75a2);
  CYC(0x75a2, 0x75a5); SET_HL(0x7dfd); // mainScripts.tingleScript
  CALL_C(0x75a5, interactionSetScript_hook, 0x2544, 0x75a8);
  CYC(0x75a8, 0x75aa); A = 0x04;
  CYC(0x75aa, 0x75ac); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x75ac, 0x75ad); mem_wr(gb, DE, A);
  CYC(0x75ad, 0x75af); A = 0x01;
  CYC(0x75af, 0x75b1); goto setAnimation; // jr

state2: // interactionCodec8@state2
  CYC(0x75b1, 0x75b3); A = 0x03;
  CYC(0x75b3, 0x75b4); mem_wr(gb, DE, A); // [state]
  CYC(0x75b4, 0x75b6); A = 15;
  CYC(0x75b6, 0x75b8); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x75b8, 0x75b9); mem_wr(gb, DE, A);
  CYC(0x75b9, 0x75bb); A = 0x02;

setAnimation: // interactionCodec8@setAnimation
  CYC(0x75bb, 0x75be); interactionSetAnimation_hook(gb); return; // jp

state4: // interactionCodec8@state4
  CYC(0x75be, 0x75c0); A = 0x19; // TREASURE_SEED_SATCHEL
  CALL_C(0x75c0, checkTreasureObtained_hook, 0x1748, 0x75c3);
  CYC(0x75c3, 0x75c5); E = INTERACTION_BASE + OBJ_VAR3D;
  CYC(0x75c5, 0x75c6); A = alu_dec8(gb, A);
  CYC(0x75c6, 0x75c7); mem_wr(gb, DE, A);
  CALL_C(0x75c7, interactionRunScript_hook, 0x2552, 0x75ca);
  CALL_C(0x75ca, interactionAnimateAsNpc_hook, 0x26db, 0x75cd);
  CYC(0x75cd, 0x75cf); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x75cf, 0x75d0); A = mem_rd(gb, DE);
  CYC(0x75d0, 0x75d1); alu_rrca(gb);
  if (!(F & FC)) { CYCT(0x75d1, 0x75d3); goto label_0b_330; } // jr nc
  CYC(0x75d1, 0x75d3);
  CYC(0x75d3, 0x75d6); SET_BC(0xfe00); // -$200
  CALL_C(0x75d6, objectSetSpeedZ_hook, 0x239d, 0x75d9);
  CYC(0x75d9, 0x75dc); SET_BC(0xe800);
  CALL_C(0x75dc, objectCreateSparkle_hook, 0x3e28, 0x75df);
  CYC(0x75df, 0x75e1); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x75e1, 0x75e3); mem_wr(gb, HL, 0x10);
  CYC(0x75e3, 0x75e6); SET_BC(0xf008);
  CALL_C(0x75e6, objectCreateSparkle_hook, 0x3e28, 0x75e9);
  CYC(0x75e9, 0x75eb); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x75eb, 0x75ed); mem_wr(gb, HL, 0x10);
  CYC(0x75ed, 0x75f0); SET_BC(0xf0f8);
  CALL_C(0x75f0, objectCreateSparkle_hook, 0x3e28, 0x75f3);
  CYC(0x75f3, 0x75f5); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x75f5, 0x75f7); mem_wr(gb, HL, 0x10);

label_0b_330: // interactionCodec8@label_0b_330
  CYC(0x75f7, 0x75f9); C = 0x20;
  CALL_C(0x75f9, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x75fc);
  if (!(F & FZ)) { CYCT(0x75fc, 0x75fd); ret_effect(gb); return; } // ret nz
  CYC(0x75fc, 0x75fd);
  CYC(0x75fd, 0x75ff); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(0x75ff, 0x7600); A = mem_rd(gb, DE);
  CYC(0x7600, 0x7601); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x7601, 0x7602); ret_effect(gb); return; } // ret nc
  CYC(0x7601, 0x7602);
  CYC(0x7602, 0x7603); alu_xor(gb, A);
  CYC(0x7603, 0x7605); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x7605, 0x7606); mem_wr(gb, DE, A);
  CYC(0x7606, 0x7608); A = 0x01;
  CYC(0x7608, 0x760a); goto setAnimation; // jr
}
