#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/seasons/scriptHelper.s (INTERAC_SEASON_SPIRITS_SCRIPTS), bank 0x15.

// An energy swirl going into the fairy in relatedObj1.
void s_seasonsSpirit_createSwirl_hook(GB *gb) {
  BASE(seasonsSpirit_createSwirl);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x57; // Interaction.relatedObj1+1
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); L = 0x4b; // Interaction.yh
  CYC(b_+6, b_+7); B = mem_rd(gb, HL);
  CYC(b_+7, b_+9); L = 0x4d; // Interaction.xh
  CYC(b_+9, b_+10); C = mem_rd(gb, HL);
  CYC(b_+10, b_+12); A = 0x6e;
  CYC(b_+12, b_+15);
  TAIL(createEnergySwirlGoingIn);
}

// spawnSeasonsSpiritSubId01 from its +2: the seasons fairy (subid = var03, var03 = b) at
// ($18,$70), remembered in relatedObj1.
static void spirit_spawn_fairy(GB *gb) {
  BASE(spawnSeasonsSpiritSubId01);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+2, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+5);
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); mem_wr(gb, HL, 0x50); // INTERAC_SEASONS_FAIRY
  CYC(b_+8, b_+10); E = 0x43; // Interaction.var03
  CYC(b_+10, b_+11); A = mem_rd(gb, DE);
  CYC(b_+11, b_+12); L = alu_inc8(gb, L);
  CYC(b_+12, b_+13); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+13, b_+14); mem_wr(gb, HL, B);
  CYC(b_+14, b_+16); L = 0x4b; // Interaction.yh
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x18);
  CYC(b_+18, b_+20); L = 0x4d; // Interaction.xh
  CYC(b_+20, b_+22); mem_wr(gb, HL, 0x70);
  CYC(b_+22, b_+24); E = 0x57; // Interaction.relatedObj1+1
  CYC(b_+24, b_+25); A = H;
  CYC(b_+25, b_+26); mem_wr(gb, DE, A);
  RET(b_+26); return;
}

void s_spawnSeasonsSpiritSubId00_hook(GB *gb) {
  BASE(spawnSeasonsSpiritSubId00);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x00;
  CYCT(b_+2, b_+4); spirit_spawn_fairy(gb); return;
}

void s_spawnSeasonsSpiritSubId01_hook(GB *gb) {
  BASE(spawnSeasonsSpiritSubId01);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x01;
  spirit_spawn_fairy(gb); return;
}

// var3f = 0 with fewer than four seasons, 1 with all four in room $f5, else 2.
void s_seasonsSpirits_checkPostSeasonGetText_hook(GB *gb) {
  BASE(seasonsSpirits_checkPostSeasonGetText);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wObtainedSeasons);
  CYC(b_+3, b_+4); alu_add(gb, A);
  CALL_C(b_+4, getNumSetBits_hook, SYM(getNumSetBits), b_+7);
  CYC(b_+7, b_+8); H = D;
  CYC(b_+8, b_+10); L = 0x7f; // Interaction.var3f
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x00);
  CYC(b_+12, b_+14); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+14); return; }
  CYC(b_+14, b_+15);
  CYC(b_+15, b_+16); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+16, b_+19); A = mem_rd(gb, wActiveRoom);
  CYC(b_+19, b_+21); alu_cp(gb, 0xf5); // ROOM_SEASONS_5f5
  if (F & FZ) { RET_TAKEN(b_+21); return; }
  CYC(b_+21, b_+22);
  CYC(b_+22, b_+23); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+23); return;
}
