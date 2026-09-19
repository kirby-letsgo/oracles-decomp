#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_SOLDIER), bank 0x15.

void soldierSetSimulatedInputToEscortLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x59f3, 0x59f4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x59f4, 0x59f6); goto exit_palace; } // jr nz
  CYC(0x59f4, 0x59f6);
  CYC(0x59f6, 0x59f9); A = W8(w1Link_xh);
  CYC(0x59f9, 0x59fb); B = 0x60;
  CYC(0x59fb, 0x59fd); alu_sub(gb, 0x50);
  if (!(F & FC)) { CYCT(0x59fd, 0x59ff); goto have_x_delta; } // jr nc
  CYC(0x59fd, 0x59ff);
  CYC(0x59ff, 0x5a00); alu_cpl(gb);
  CYC(0x5a00, 0x5a01); A = alu_inc8(gb, A);
  CYC(0x5a01, 0x5a03); B = 0x50;

have_x_delta:
  CYC(0x5a03, 0x5a04); C = A;
  CYC(0x5a04, 0x5a05); push_effect(gb, DE);
  CYC(0x5a05, 0x5a08); SET_HL(0x51e8); // agesInteractionsBank09.linkEnterPalaceSimulatedInput
  CYC(0x5a08, 0x5a0a); A = 0x09; // :agesInteractionsBank09.linkEnterPalaceSimulatedInput
  CALL_C(0x5a0a, setSimulatedInputAddress_hook, 0x2a1d, 0x5a0d);
  CYC(0x5a0d, 0x5a0e); SET_DE(pop_effect(gb));
  CYC(0x5a0e, 0x5a0f); A = C;
  CYC(0x5a0f, 0x5a10); alu_rra(gb);
  CYC(0x5a10, 0x5a11); alu_add(gb, C);
  CYC(0x5a11, 0x5a14); mem_wr(gb, wSimulatedInputCounter, A);
  CYC(0x5a14, 0x5a15); A = B;
  CYC(0x5a15, 0x5a18); mem_wr(gb, wSimulatedInputValue, A);
  CYC(0x5a18, 0x5a19); alu_xor(gb, A);
  CYC(0x5a19, 0x5a1c); mem_wr(gb, wDisabledObjects, A);
  RET(0x5a1c); return;

exit_palace:
  CYC(0x5a1d, 0x5a1e); push_effect(gb, DE);
  CYC(0x5a1e, 0x5a21); SET_HL(0x51ed); // agesInteractionsBank09.linkExitPalaceSimulatedInput
  CYC(0x5a21, 0x5a23); A = 0x09; // :agesInteractionsBank09.linkExitPalaceSimulatedInput
  CALL_C(0x5a23, setSimulatedInputAddress_hook, 0x2a1d, 0x5a26);
  CYC(0x5a26, 0x5a27); SET_DE(pop_effect(gb));
  RET(0x5a27); return;
}

void soldierGiveMysterySeeds_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5a28, 0x5a2a); A = 0x24; // TREASURE_MYSTERY_SEEDS
  CYC(0x5a2a, 0x5a2c); C = 0x00;
  CYC(0x5a2c, 0x5a2f); giveTreasure_hook(gb); return; // jp
}

void soldierUpdateMinimap_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5a2f, 0x5a32); SET_HL(0x5945); // bank1.checkUpdateDungeonMinimap
  CYC(0x5a32, 0x5a34); E = 0x01; // target bank
  CYC(0x5a34, 0x5a37); interBankCall_hook(gb); return; // jp
}

static void soldier15_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0x00, 0x0012, 0x0013, false);
  burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
}

void soldierGetRandomVar32Val_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CALL_C(0x5a37, getRandomNumber_hook, 0x043e, 0x5a3a);
  CYC(0x5a3a, 0x5a3c); alu_and(gb, 0x03);
  CYC(0x5a3c, 0x5a3f); SET_HL(0x5a49); // @data
  CYC(0x5a3f, 0x5a40); soldier15_addAToHl_from_rst(gb, 0x5a40);
  CYC(0x5a40, 0x5a41); A = mem_rd(gb, HL);
  CYC(0x5a41, 0x5a43); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x5a43, 0x5a44); mem_wr(gb, DE, A);
  CYC(0x5a44, 0x5a46); A = 0x59;
  CYC(0x5a46, 0x5a47); E = alu_inc8(gb, E);
  CYC(0x5a47, 0x5a48); mem_wr(gb, DE, A);
  RET(0x5a48); return;
}

void soldierSetTextToShow_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x5a4d, 0x5a4f); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x5a4f, 0x5a50); A = mem_rd(gb, DE);
  CYC(0x5a50, 0x5a53); SET_HL(0x5a5d); // @soldierTextIndices
  CYC(0x5a53, 0x5a54); soldier15_addAToHl_from_rst(gb, 0x5a54);
  CYC(0x5a54, 0x5a55); A = mem_rd(gb, HL);
  CYC(0x5a55, 0x5a57); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x5a57, 0x5a58); mem_wr(gb, DE, A);
  CYC(0x5a58, 0x5a5a); A = 0x59; // >TX_5900
  CYC(0x5a5a, 0x5a5b); E = alu_inc8(gb, E);
  CYC(0x5a5b, 0x5a5c); mem_wr(gb, DE, A);
  RET(0x5a5c); return;
}
