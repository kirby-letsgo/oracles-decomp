#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_PIRATIAN), bank 0x15.

// Applies the jump with gravity $30; sets var3d once the piratian has landed.
void s_piratian_waitUntilJumpDone_hook(GB *gb) {
  BASE(piratian_waitUntilJumpDone);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); C = 0x30;
  CALL_C(b_+2, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+7); H = D;
  CYC(b_+7, b_+9); L = 0x7d; // Interaction.var3d
  CYC(b_+9, b_+11); mem_wr(gb, HL, 0x01);
  RET(b_+11); return;
}

static void piratian_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

// TX_3axx from the subid's pair in @subidTable, the second one when D6 is done (a = 1).
void s_showPiratianTextBasedOnD6Done_hook(GB *gb) {
  BASE(showPiratianTextBasedOnD6Done);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); E = 0x42; // Interaction.subid
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+6); alu_add(gb, B);
  CYC(b_+6, b_+9); SET_HL(b_+14); // @subidTable-2
  CYC(b_+9, b_+10); piratian_add_a_to_hl(gb, b_+10);
  CYC(b_+10, b_+12); B = 0x3a;
  CYC(b_+12, b_+13); C = mem_rd(gb, HL);
  CYC(b_+13, b_+16);
  TAIL(showText);
}

// Tile a at the row of the piratian's x (its high nibble as the position).
void s_piratian_replaceTileAtPiratian_hook(GB *gb) {
  BASE(piratian_replaceTileAtPiratian);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); E = 0x4d; // Interaction.xh
  CYC(b_+3, b_+4); A = mem_rd(gb, DE);
  CYC(b_+4, b_+6); alu_and(gb, 0xf0);
  CYC(b_+6, b_+8); A = alu_swap(gb, A);
  CYC(b_+8, b_+9); C = A;
  CYC(b_+9, b_+10); A = B;
  CYC(b_+10, b_+13);
  TAIL(setTile);
}

void s_headToPirateShip_hook(GB *gb) {
  BASE(headToPirateShip);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x13; // GLOBALFLAG_PIRATES_LEFT_FOR_SHIP
  CALL_C(b_+2, setGlobalFlag_hook, SYM(setGlobalFlag), b_+5);
  CYC(b_+5, b_+8); SET_HL(b_+16); // @warpDestVariables
  CALL_C(b_+8, setWarpDestVariables_hook, SYM(setWarpDestVariables), b_+11);
  CYC(b_+11, b_+13); A = 0x8d;
  CYC(b_+13, b_+16);
  TAIL(playSound_b00);
}

void s_piratesDeparting_spawnPirateFromShip_hook(GB *gb) {
  BASE(piratesDeparting_spawnPirateFromShip);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x40); // INTERAC_PIRATIAN
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x0c);
  CYC(b_+9, b_+11); L = 0x4b; // Interaction.yh
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x28);
  CYC(b_+13, b_+15); L = 0x4d; // Interaction.xh
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x78);
  RET(b_+17); return;
}

void s_linkedGame_spawnAmbi_hook(GB *gb) {
  BASE(linkedGame_spawnAmbi);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0xb8); // INTERAC_AMBI
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x03);
  CYC(b_+9, b_+11); L = 0x4b; // Interaction.yh
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0x88);
  CYC(b_+13, b_+15); L = 0x4d; // Interaction.xh
  CYC(b_+15, b_+17); mem_wr(gb, HL, 0x50);
  RET(b_+17); return;
}

void s_piratianCaptain_simulatedInput_hook(GB *gb) {
  BASE(piratianCaptain_simulatedInput);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(b_+8); // @simulatedInput
  CYC(b_+3, b_+5); A = 0x15;
  CYC(b_+5, b_+8);
  TAIL(setSimulatedInputAddress);
}

void s_piratianCaptain_setLinkInvisible_hook(GB *gb) {
  BASE(piratianCaptain_setLinkInvisible);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_visible);
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  RET(b_+5); return;
}

void s_piratianCaptain_setInvisible_hook(GB *gb) {
  BASE(piratianCaptain_setInvisible);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = 0x5a; // Interaction.visible
  CYC(b_+3, b_+5); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
  RET(b_+5); return;
}

void s_pirateCaptain_freezeLinkForCutscene_hook(GB *gb) {
  BASE(pirateCaptain_freezeLinkForCutscene);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, setLinkForceStateToState08_hook, SYM(setLinkForceStateToState08), b_+3);
  CYC(b_+3, b_+6); SET_HL(w1Link_direction);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x01);
  RET(b_+8); return;
}

// var39 = 1 or 2 by which side of Link the object is, when within 12 pixels horizontally and
// Link is at y $38 or below.
void s_seasonsFunc_15_5a70_hook(GB *gb) {
  BASE(seasonsFunc_15_5a70);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x4d; // Interaction.xh
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+6); SET_HL(w1Link_xh);
  CYC(b_+6, b_+7); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+7, b_+8); B = A;
  CYC(b_+8, b_+10); alu_add(gb, 0x0c);
  CYC(b_+10, b_+12); alu_cp(gb, 0x18);
  if (!(F & FC)) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CYC(b_+13, b_+16); A = mem_rd(gb, w1Link_yh);
  CYC(b_+16, b_+18); alu_cp(gb, 0x38);
  if (F & FC) { RET_TAKEN(b_+18); return; }
  CYC(b_+18, b_+19);
  CYC(b_+19, b_+20); A = B;
  CYC(b_+20, b_+21); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+21, b_+23); A = 0x01;
  if (!(F & FC)) { CYCT(b_+23, b_+25); goto store; }
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); A = alu_inc8(gb, A);
store:
  CYC(b_+26, b_+28); E = 0x79; // Interaction.var39
  CYC(b_+28, b_+29); mem_wr(gb, DE, A);
  RET(b_+29); return;
}
