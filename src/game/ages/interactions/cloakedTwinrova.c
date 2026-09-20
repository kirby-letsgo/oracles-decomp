#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

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
  uint16_t sp0_ = gb->sp;
  CYC(0x6b58, 0x6b5a); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6b5a, 0x6b5b); A = mem_rd(gb, DE);
  {
    CYC(0x6b5b, 0x6b5c); push_effect(gb, 0x6b5c);
    uint16_t target = interactionCode8d_jump_table(gb);
    if (target == 0x6b96) goto state1;
    // target == 0x6b60 falls through to state0
  }

  // interactionCode8d@state0
  CYC(0x6b60, 0x6b62); A = 0x01;
  CYC(0x6b62, 0x6b63); mem_wr(gb, DE, A);
  CALL_C(0x6b63, interactionInitGraphics_hook, 0x15fb, 0x6b66);
  CALL_C(0x6b66, objectSetVisiblec2_hook, 0x1e45, 0x6b69);
  CYC(0x6b69, 0x6b6b); A = 0x28; // >TX_2800
  CALL_C(0x6b6b, interactionSetHighTextIndex_hook, 0x253b, 0x6b6e);
  CYC(0x6b6e, 0x6b70); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6b70, 0x6b71); A = mem_rd(gb, DE);
  {
    CYC(0x6b71, 0x6b72); push_effect(gb, 0x6b72);
    uint16_t target = interactionCode8d_jump_table(gb);
    if (target == 0x6b89) goto initSubid1;
    if (target == 0x6b83) goto initSubid2;
    // target == 0x6b78 falls through to initSubid0
  }

  // interactionCode8d@initSubid0
  CYC(0x6b78, 0x6b7a); A = 0x03;
  CALL_C(0x6b7a, interactionSetAnimation_hook, 0x262e, 0x6b7d);
  CYC(0x6b7d, 0x6b80); SET_BC(0x4088);
  CALL_C(0x6b80, interactionSetPosition_hook, 0x2773, 0x6b83);

initSubid2:
  CYC(0x6b83, 0x6b86); push_effect(gb, 0x6b86); goto loadScript; // call
afterLoadScript:
  CYC(0x6b86, 0x6b89); objectSetInvisible_hook(gb); return; // jp

initSubid1:
  CYC(0x6b89, 0x6b8c); SET_BC(0x4050);
  CALL_C(0x6b8c, interactionSetPosition_hook, 0x2773, 0x6b8f);
  CYC(0x6b8f, 0x6b91); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6b91, 0x6b93); mem_wr(gb, HL, 30);
  CYC(0x6b93, 0x6b96); objectSetInvisible_hook(gb); return; // jp

state1:
  CYC(0x6b96, 0x6b98); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6b98, 0x6b99); A = mem_rd(gb, DE);
  {
    CYC(0x6b99, 0x6b9a); push_effect(gb, 0x6b9a);
    uint16_t target = interactionCode8d_jump_table(gb);
    if (target == 0x6bb8) goto runSubid1;
    // target == 0x6ba0 (subid0 and subid2 alias) falls through to runSubid0
  }

  // interactionCode8d@runSubid0 / @runSubid2
  CALL_C(0x6ba0, interactionRunScript_hook, 0x2552, 0x6ba3);
  if (!(F & FC)) { CYCT(0x6ba3, 0x6ba6); interactionAnimate_hook(gb); return; } // jp nc
  CYC(0x6ba3, 0x6ba6);
  CALL_C(0x6ba6, objectCreatePuff_hook, 0x24c1, 0x6ba9);
  CYC(0x6ba9, 0x6bab); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6bab, 0x6bac); A = mem_rd(gb, DE);
  CYC(0x6bac, 0x6bad); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6bad, 0x6baf); goto L_6bb5; } // jr z
  CYC(0x6bad, 0x6baf);
  CYC(0x6baf, 0x6bb2); SET_BC(0x9302); // INTERAC_TWINROVA, subid 2
  CALL_C(0x6bb2, objectCreateInteraction_hook, 0x24c5, 0x6bb5);

L_6bb5:
  CYC(0x6bb5, 0x6bb8); interactionDelete_hook(gb); return; // jp

runSubid1:
  CALL_C(0x6bb8, interactionAnimate_hook, 0x261b, 0x6bbb);
  CYC(0x6bbb, 0x6bbd); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6bbd, 0x6bbe); A = mem_rd(gb, DE);
  {
    CYC(0x6bbe, 0x6bbf); push_effect(gb, 0x6bbf);
    uint16_t target = interactionCode8d_jump_table(gb);
    if (target == 0x6beb) goto subid1Substate1;
    if (target == 0x6bfa) goto subid1Substate2;
    if (target == 0x6c10) goto subid1Substate3;
    // target == 0x6bc7 falls through to subid1Substate0
  }

  // interactionCode8d@subid1Substate0
  CALL_C(0x6bc7, interactionDecCounter1_hook, 0x23cc, 0x6bca);
  if (!(F & FZ)) { RET_TAKEN(0x6bca); return; } // ret nz
  CYC(0x6bca, 0x6bcb);
  CYC(0x6bcb, 0x6bcd); mem_wr(gb, HL, 20);
  CYC(0x6bcd, 0x6bcf); A = 0x21; // MUS_DISASTER
  CALL_C(0x6bcf, playSound_b00_hook, 0x0c98, 0x6bd2);
  CALL_C(0x6bd2, objectSetVisible_hook, 0x1e84, 0x6bd5);
  CALL_C(0x6bd5, fadeinFromBlack_hook, 0x32e6, 0x6bd8);
  CYC(0x6bd8, 0x6bda); A = 0x06;
  CYC(0x6bda, 0x6bdd); W8(wDirtyFadeSprPalettes) = A;
  CYC(0x6bdd, 0x6be0); W8(wFadeSprPaletteSources) = A;
  CYC(0x6be0, 0x6be2); A = 0x03;
  CYC(0x6be2, 0x6be5); W8(wDirtyFadeBgPalettes) = A;
  CYC(0x6be5, 0x6be8); W8(wFadeBgPaletteSources) = A;
  CYC(0x6be8, 0x6beb); interactionIncSubstate_hook(gb); return; // jp

subid1Substate1:
  CALL_C(0x6beb, interactionDecCounter1IfPaletteNotFading_hook, 0x2744, 0x6bee);
  if (!(F & FZ)) { RET_TAKEN(0x6bee); return; } // ret nz
  CYC(0x6bee, 0x6bef);
  CYC(0x6bef, 0x6bf1); mem_wr(gb, HL, 20);
  CALL_C(0x6bf1, interactionIncSubstate_hook, 0x23e5, 0x6bf4);
  CYC(0x6bf4, 0x6bf7); SET_BC(0x2808); // TX_2808
  CYC(0x6bf7, 0x6bfa); showText_hook(gb); return; // jp

subid1Substate2:
  CALL_C(0x6bfa, interactionDecCounter1IfTextNotActive_hook, 0x273c, 0x6bfd);
  if (!(F & FZ)) { RET_TAKEN(0x6bfd); return; } // ret nz
  CYC(0x6bfd, 0x6bfe);
  CYC(0x6bfe, 0x6c00); A = 0xd2; // SND_LIGHTNING
  CALL_C(0x6c00, playSound_b00_hook, 0x0c98, 0x6c03);
  CYC(0x6c03, 0x6c06); SET_HL(wGenericCutscene_cbb3);
  CYC(0x6c06, 0x6c08); mem_wr(gb, HL, 0x00);
  CYC(0x6c08, 0x6c0b); SET_HL(wGenericCutscene_cbba);
  CYC(0x6c0b, 0x6c0d); mem_wr(gb, HL, 0xff);
  CYC(0x6c0d, 0x6c10); interactionIncSubstate_hook(gb); return; // jp

subid1Substate3:
  CYC(0x6c10, 0x6c13); SET_HL(wGenericCutscene_cbb3);
  CYC(0x6c13, 0x6c15); B = 0x02;
  CALL_C(0x6c15, flashScreen_hook, 0x2d73, 0x6c18);
  if (F & FZ) { RET_TAKEN(0x6c18); return; } // ret z
  CYC(0x6c18, 0x6c19);
  CYC(0x6c19, 0x6c1b); A = 0x02;
  CYC(0x6c1b, 0x6c1e); W8(wGenericCutscene_cbb8) = A;
  CYC(0x6c1e, 0x6c20); A = 0x08; // CUTSCENE_BLACK_TOWER_EXPLANATION
  CYC(0x6c20, 0x6c23); W8(wCutsceneTrigger) = A;
  CYC(0x6c23, 0x6c26); interactionDelete_hook(gb); return; // jp

// interactionCode8d@loadScript
loadScript:
  CYC(0x6c26, 0x6c28); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6c28, 0x6c29); A = mem_rd(gb, DE);
  CYC(0x6c29, 0x6c2c); SET_HL(0x6c33); // @scriptTable
  CYC(0x6c2c, 0x6c2d); interactionCode8d_add_double_index(gb, 0x6c2d);
  CYC(0x6c2d, 0x6c2e); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6c2e, 0x6c2f); H = mem_rd(gb, HL);
  CYC(0x6c2f, 0x6c30); L = A;
  CYC(0x6c30, 0x6c33); interactionSetScript_hook(gb);
  if (gb->pc == 0x6b86 && gb->sp == sp0_) goto afterLoadScript;
  return; // jp
}
