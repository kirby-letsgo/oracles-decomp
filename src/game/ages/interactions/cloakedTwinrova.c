#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(interactionCode8d), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(interactionCode8d), (from), (to), true)

static uint16_t interactionCode8d_jump_table(GB *gb) {
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

static void interactionCode8d_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_CLOAKED_TWINROVA
void interactionCode8d_hook(GB *gb) {
  BASE(interactionCode8d);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  {
    CYC(b_+3, b_+4); push_effect(gb, b_+4);
    uint16_t target = interactionCode8d_jump_table(gb);
    if (target == b_+62) goto state1;
    // target == 0x6b60 falls through to state0
  }

  // interactionCode8d@state0
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_C(b_+11, interactionInitGraphics_hook, SYM(interactionInitGraphics), b_+14);
  CALL_C(b_+14, objectSetVisiblec2_hook, SYM(objectSetVisiblec2), b_+17);
  CYC(b_+17, b_+19); A = 0x28; // >TX_2800
  CALL_C(b_+19, interactionSetHighTextIndex_hook, SYM(interactionSetHighTextIndex), b_+22);
  CYC(b_+22, b_+24); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  {
    CYC(b_+25, b_+26); push_effect(gb, b_+26);
    uint16_t target = interactionCode8d_jump_table(gb);
    if (target == b_+49) goto initSubid1;
    if (target == b_+43) goto initSubid2;
    // target == 0x6b78 falls through to initSubid0
  }

  // interactionCode8d@initSubid0
  CYC(b_+32, b_+34); A = 0x03;
  CALL_C(b_+34, interactionSetAnimation_hook, SYM(interactionSetAnimation), b_+37);
  CYC(b_+37, b_+40); SET_BC((SYM(interactionCode79__state0) + 27));
  CALL_C(b_+40, interactionSetPosition_hook, SYM(interactionSetPosition), b_+43);

initSubid2:
  CYC(b_+43, b_+46); push_effect(gb, b_+46); goto loadScript; // call
afterLoadScript:
  CYC(b_+46, b_+49); objectSetInvisible_hook(gb); return; // jp

initSubid1:
  CYC(b_+49, b_+52); SET_BC((SYM(interactionCode78__tileReplacement) + 27));
  CALL_C(b_+52, interactionSetPosition_hook, SYM(interactionSetPosition), b_+55);
  CYC(b_+55, b_+57); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+57, b_+59); mem_wr(gb, HL, 30);
  CYC(b_+59, b_+62); objectSetInvisible_hook(gb); return; // jp

state1:
  CYC(b_+62, b_+64); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+64, b_+65); A = mem_rd(gb, DE);
  {
    CYC(b_+65, b_+66); push_effect(gb, b_+66);
    uint16_t target = interactionCode8d_jump_table(gb);
    if (target == b_+96) goto runSubid1;
    // target == 0x6ba0 (subid0 and subid2 alias) falls through to runSubid0
  }

  // interactionCode8d@runSubid0 / @runSubid2
  CALL_C(b_+72, interactionRunScript_hook, SYM(interactionRunScript), b_+75);
  if (!(F & FC)) { CYCT(b_+75, b_+78); interactionAnimate_hook(gb); return; } // jp nc
  CYC(b_+75, b_+78);
  CALL_C(b_+78, objectCreatePuff_hook, SYM(objectCreatePuff), b_+81);
  CYC(b_+81, b_+83); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+83, b_+84); A = mem_rd(gb, DE);
  CYC(b_+84, b_+85); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+85, b_+87); goto L_6bb5; } // jr z
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+90); SET_BC(0x9302); // INTERAC_TWINROVA, subid 2
  CALL_C(b_+90, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+93);

L_6bb5:
  CYC(b_+93, b_+96); interactionDelete_hook(gb); return; // jp

runSubid1:
  CALL_C(b_+96, interactionAnimate_hook, SYM(interactionAnimate), b_+99);
  CYC(b_+99, b_+101); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+101, b_+102); A = mem_rd(gb, DE);
  {
    CYC(b_+102, b_+103); push_effect(gb, b_+103);
    uint16_t target = interactionCode8d_jump_table(gb);
    if (target == b_+147) goto subid1Substate1;
    if (target == b_+162) goto subid1Substate2;
    if (target == b_+184) goto subid1Substate3;
    // target == 0x6bc7 falls through to subid1Substate0
  }

  // interactionCode8d@subid1Substate0
  CALL_C(b_+111, interactionDecCounter1_hook, SYM(interactionDecCounter1), b_+114);
  if (!(F & FZ)) { RET_TAKEN(b_+114); return; } // ret nz
  CYC(b_+114, b_+115);
  CYC(b_+115, b_+117); mem_wr(gb, HL, 20);
  CYC(b_+117, b_+119); A = 0x21; // MUS_DISASTER
  CALL_C(b_+119, playSound_b00_hook, SYM(playSound_b00), b_+122);
  CALL_C(b_+122, objectSetVisible_hook, SYM(objectSetVisible), b_+125);
  CALL_C(b_+125, fadeinFromBlack_hook, SYM(fadeinFromBlack), b_+128);
  CYC(b_+128, b_+130); A = 0x06;
  CYC(b_+130, b_+133); W8(wDirtyFadeSprPalettes) = A;
  CYC(b_+133, b_+136); W8(wFadeSprPaletteSources) = A;
  CYC(b_+136, b_+138); A = 0x03;
  CYC(b_+138, b_+141); W8(wDirtyFadeBgPalettes) = A;
  CYC(b_+141, b_+144); W8(wFadeBgPaletteSources) = A;
  CYC(b_+144, b_+147); interactionIncSubstate_hook(gb); return; // jp

subid1Substate1:
  CALL_C(b_+147, interactionDecCounter1IfPaletteNotFading_hook, SYM(interactionDecCounter1IfPaletteNotFading), b_+150);
  if (!(F & FZ)) { RET_TAKEN(b_+150); return; } // ret nz
  CYC(b_+150, b_+151);
  CYC(b_+151, b_+153); mem_wr(gb, HL, 20);
  CALL_C(b_+153, interactionIncSubstate_hook, SYM(interactionIncSubstate), b_+156);
  CYC(b_+156, b_+159); SET_BC((SYM(objectCreateFloatingMusicNote) + 10)); // TX_2808
  CYC(b_+159, b_+162); showText_hook(gb); return; // jp

subid1Substate2:
  CALL_C(b_+162, interactionDecCounter1IfTextNotActive_hook, SYM(interactionDecCounter1IfTextNotActive), b_+165);
  if (!(F & FZ)) { RET_TAKEN(b_+165); return; } // ret nz
  CYC(b_+165, b_+166);
  CYC(b_+166, b_+168); A = 0xd2; // SND_LIGHTNING
  CALL_C(b_+168, playSound_b00_hook, SYM(playSound_b00), b_+171);
  CYC(b_+171, b_+174); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+174, b_+176); mem_wr(gb, HL, 0x00);
  CYC(b_+176, b_+179); SET_HL(wGenericCutscene_cbba);
  CYC(b_+179, b_+181); mem_wr(gb, HL, 0xff);
  CYC(b_+181, b_+184); interactionIncSubstate_hook(gb); return; // jp

subid1Substate3:
  CYC(b_+184, b_+187); SET_HL(wGenericCutscene_cbb3);
  CYC(b_+187, b_+189); B = 0x02;
  CALL_C(b_+189, flashScreen_hook, SYM(flashScreen), b_+192);
  if (F & FZ) { RET_TAKEN(b_+192); return; } // ret z
  CYC(b_+192, b_+193);
  CYC(b_+193, b_+195); A = 0x02;
  CYC(b_+195, b_+198); W8(wGenericCutscene_cbb8) = A;
  CYC(b_+198, b_+200); A = 0x08; // CUTSCENE_BLACK_TOWER_EXPLANATION
  CYC(b_+200, b_+203); W8(wCutsceneTrigger) = A;
  CYC(b_+203, b_+206); interactionDelete_hook(gb); return; // jp

// interactionCode8d@loadScript
loadScript:
  CYC(b_+206, b_+208); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+208, b_+209); A = mem_rd(gb, DE);
  CYC(b_+209, b_+212); SET_HL(b_+219); // @scriptTable
  CYC(b_+212, b_+213); interactionCode8d_add_double_index(gb, b_+213);
  CYC(b_+213, b_+214); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(b_+214, b_+215); H = mem_rd(gb, HL);
  CYC(b_+215, b_+216); L = A;
  CYC(b_+216, b_+219); interactionSetScript_hook(gb);
  if (gb->pc == b_+46 && gb->sp == sp0_) goto afterLoadScript;
  return; // jp
}
