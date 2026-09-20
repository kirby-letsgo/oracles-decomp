#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void oldMan_takeRupees_hook(GB *gb);
void oldMan_giveRupees_hook(GB *gb);
void oldManGiveShieldUpgradeToLink_hook(GB *gb);
void oldManWarpLinkToLibrary_hook(GB *gb);
void oldManSetAnimationToVar38_hook(GB *gb);
void label_15_097_hook(GB *gb);

static void oldMan_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void oldMan_takeRupees_hook(GB *gb) {
  CYC(0x5039, 0x503c); SET_HL(0xc6ad);
  CYC(0x503c, 0x503d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x503d, 0x503e); alu_or(gb, mem_rd(gb, HL));
  CYC(0x503e, 0x5040); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(0x5040, 0x5041); mem_wr(gb, DE, A);
  if (F & FZ) {
    RET_TAKEN(0x5041); return;
  }
  CYC(0x5041, 0x5042);
  CYC(0x5042, 0x5044); A = 0x01;
  CYC(0x5044, 0x5045); mem_wr(gb, DE, A);
  CYC(0x5045, 0x5047); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5047, 0x5048); A = mem_rd(gb, DE);
  CYC(0x5048, 0x504b); SET_HL(0x505d);
  CYC(0x504b, 0x504c); oldMan_addAToHl_from_rst(gb, 0x504c);
  CYC(0x504c, 0x504d); A = mem_rd(gb, HL);
  CYC(0x504d, 0x5050); removeRupeeValue_hook(gb);
}

void oldMan_giveRupees_hook(GB *gb) {
  CYC(0x5050, 0x5052); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x5052, 0x5053); A = mem_rd(gb, DE);
  CYC(0x5053, 0x5056); SET_HL(0x505d);
  CYC(0x5056, 0x5057); oldMan_addAToHl_from_rst(gb, 0x5057);
  CYC(0x5057, 0x5058); C = mem_rd(gb, HL);
  CYC(0x5058, 0x505a); A = 0x28;
  CYC(0x505a, 0x505d); giveTreasure_hook(gb);
}

void oldManGiveShieldUpgradeToLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d15, 0x5d17); A = 0x01;
  CALL_C(0x5d17, checkTreasureObtained_hook, 0x1748, 0x5d1a);
  if (F & FC) { CYCT(0x5d1a, 0x5d1c); goto afterShieldCheck; } // jr c
  CYC(0x5d1a, 0x5d1c);
  CYC(0x5d1c, 0x5d1f); A = W8(wShieldLevel);
afterShieldCheck:
  CYC(0x5d1f, 0x5d21); alu_cp(gb, 0x03);
  if (F & FC) { CYCT(0x5d21, 0x5d23); goto haveSubidLevel; } // jr c
  CYC(0x5d21, 0x5d23);
  CYC(0x5d23, 0x5d25); A = 0x02;
haveSubidLevel:
  CYC(0x5d25, 0x5d26); C = A;
  CALL_C(0x5d26, getFreeInteractionSlot_hook, 0x3aef, 0x5d29);
  if (!(F & FZ)) { RET_TAKEN(0x5d29); return; } // ret nz
  CYC(0x5d29, 0x5d2a);
  CYC(0x5d2a, 0x5d2c); mem_wr(gb, HL, 0x60);
  CYC(0x5d2c, 0x5d2d); L = alu_inc8(gb, L);
  CYC(0x5d2d, 0x5d2f); mem_wr(gb, HL, 0x01);
  CYC(0x5d2f, 0x5d30); L = alu_inc8(gb, L);
  CYC(0x5d30, 0x5d31); mem_wr(gb, HL, C);
  CYC(0x5d31, 0x5d32); push_effect(gb, DE);
  CYC(0x5d32, 0x5d35); SET_DE(w1Link_yh);
  CALL_C(0x5d35, objectCopyPosition_rawAddress_hook, 0x2247, 0x5d38);
  CYC(0x5d38, 0x5d39); SET_DE(pop_effect(gb));
  RET(0x5d39); return; // ret
}

void oldManWarpLinkToLibrary_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5d3a, 0x5d3d); SET_HL(0x5d45);
  CALL_C(0x5d3d, setWarpDestVariables_hook, 0x1997, 0x5d40);
  CYC(0x5d40, 0x5d42); A = 0x8d;
  CYC(0x5d42, 0x5d45); playSound_b00_hook(gb); return; // jp
}

void oldManSetAnimationToVar38_hook(GB *gb) {
  CYC(0x5d4a, 0x5d4c); E = 0x78;
  label_15_097_hook(gb); return;
}

void label_15_097_hook(GB *gb) {
  CYC(0x5d4c, 0x5d4d); A = mem_rd(gb, DE);
  CYC(0x5d4d, 0x5d50); interactionSetAnimation_hook(gb); return; // jp
}
