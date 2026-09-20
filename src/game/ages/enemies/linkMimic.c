#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0e, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0e, (from), (to), true)

void linkMimic_state8_hook(GB *gb);
void armMimic_uninitialized_hook(GB *gb);
void armMimic_state_stub_hook(GB *gb);
void armMimic_state_switchHook_hook(GB *gb);
void armMimic_state8_hook(GB *gb);

static uint16_t linkMimic_jump_table(GB *gb) {
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

// ==================================================================================================
// ENEMY_LINK_MIMIC
//
// Shares code with ENEMY_ARM_MIMIC (armMimic.c).
// ==================================================================================================
void enemyCode64_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(0x60fa, 0x60fc); goto normalStatus; } // jr z
  CYC(0x60fa, 0x60fc);
  CYC(0x60fc, 0x60fe); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(0x60fe); return; } // ret c
  CYC(0x60fe, 0x60ff);
  if (F & FZ) { CYCT(0x60ff, 0x6102); enemyDie_hook(gb); return; } // jp z
  CYC(0x60ff, 0x6102);
  CYC(0x6102, 0x6103); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6103, 0x6106); ecom_updateKnockback_b0e_hook(gb); return; } // jp nz
  CYC(0x6103, 0x6106);
  RET(0x6106); return; // ret

normalStatus:
  CYC(0x6107, 0x6109); E = ENEMY_BASE + OBJ_STATE;
  CYC(0x6109, 0x610a); A = mem_rd(gb, DE);
  {
    CYC(0x610a, 0x610b); push_effect(gb, 0x610b);
    uint16_t target = linkMimic_jump_table(gb);
    if (target == 0x611d) goto state_uninitialized;
    if (target == 0x6173) { armMimic_state_stub_hook(gb); return; }
    if (target == 0x6167) { armMimic_state_switchHook_hook(gb); return; }
    if (target == 0x44ac) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == 0x6128) { linkMimic_state8_hook(gb); return; }
    HANDOFF(target);
  }

state_uninitialized:
  CYC(0x611d, 0x611f); A = 0x82; // PALH_82
  CALL_C(0x611f, loadPaletteHeader_hook, 0x050b, 0x6122);
  CALL_C(0x6122, armMimic_uninitialized_hook, 0x6155, 0x6125);
  CYC(0x6125, 0x6128); objectSetVisible83_hook(gb); return; // jp
}

// 0e:6128, bare global; jump-table target from enemyCode64. Falls into (via unconditional jr)
// armMimic_state8.
void linkMimic_state8_hook(GB *gb) {
  CYC(0x6128, 0x612b); A = mem_rd(gb, wDisabledObjects);
  CYC(0x612b, 0x612c); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x612c); return; } // ret nz
  CYC(0x612c, 0x612d);
  CYC(0x612d, 0x612f); armMimic_state8_hook(gb); return; // jr
}
