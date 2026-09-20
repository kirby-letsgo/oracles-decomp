#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(enemyCode64);
  uint16_t sp0_ = gb->sp;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); alu_sub(gb, 0x03); // ENEMYSTATUS_NO_HEALTH
  if (F & FC) { RET_TAKEN(b_+4); return; } // ret c
  CYC(b_+4, b_+5);
  if (F & FZ) { CYCT(b_+5, b_+8); enemyDie_hook(gb); return; } // jp z
  CYC(b_+5, b_+8);
  CYC(b_+8, b_+9); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+9, b_+12); ecom_updateKnockback_b0e_hook(gb); return; } // jp nz
  CYC(b_+9, b_+12);
  RET(b_+12); return; // ret

normalStatus:
  CYC(b_+13, b_+15); E = ENEMY_BASE + OBJ_STATE;
  CYC(b_+15, b_+16); A = mem_rd(gb, DE);
  {
    CYC(b_+16, b_+17); push_effect(gb, b_+17);
    uint16_t target = linkMimic_jump_table(gb);
    if (target == b_+35) goto state_uninitialized;
    if (target == SYM(armMimic_state_stub)) { armMimic_state_stub_hook(gb); return; }
    if (target == SYM(armMimic_state_switchHook)) { armMimic_state_switchHook_hook(gb); return; }
    if (target == SYM(ecom_blownByGaleSeedState_b0e)) { ecom_blownByGaleSeedState_b0e_hook(gb); return; }
    if (target == SYM(linkMimic_state8)) { linkMimic_state8_hook(gb); return; }
    HANDOFF(target);
  }

state_uninitialized:
  CYC(b_+35, b_+37); A = 0x82; // PALH_82
  CALL_C(b_+37, loadPaletteHeader_hook, SYM(loadPaletteHeader), b_+40);
  CALL_C(b_+40, armMimic_uninitialized_hook, SYM(armMimic_uninitialized), b_+43);
  CYC(b_+43, b_+46); objectSetVisible83_hook(gb); return; // jp
}

// 0e:6128, bare global; jump-table target from enemyCode64. Falls into (via unconditional jr)
// armMimic_state8.
void linkMimic_state8_hook(GB *gb) {
  BASE(linkMimic_state8);
  CYC(b_+0, b_+3); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+4); return; } // ret nz
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); armMimic_state8_hook(gb); return; // jr
}
