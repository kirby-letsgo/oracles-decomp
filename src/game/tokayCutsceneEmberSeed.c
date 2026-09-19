#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0a, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0a, (from), (to), true)

static uint16_t interactionCode8f_jump_table(GB *gb) {
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

// INTERAC_TOKAY_CUTSCENE_EMBER_SEED
void interactionCode8f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6c5a, 0x6c5c); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6c5c, 0x6c5d); A = mem_rd(gb, DE);
  {
    CYC(0x6c5d, 0x6c5e); push_effect(gb, 0x6c5e);
    uint16_t target = interactionCode8f_jump_table(gb);
    if (target == 0x6c78) goto state1;
    if (target == 0x6c8a) goto state2;
    if (target == 0x6ca4) goto state3;
  }

  // interactionCode8f@state0
  CYC(0x6c66, 0x6c68); A = 0x01;
  CYC(0x6c68, 0x6c69); mem_wr(gb, DE, A);
  CYC(0x6c69, 0x6c6c); SET_BC(0xff00);
  CALL_C(0x6c6c, objectSetSpeedZ_hook, 0x239d, 0x6c6f);
  CALL_C(0x6c6f, interactionInitGraphics_hook, 0x15fb, 0x6c72);
  CALL_C(0x6c72, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x6c75);
  CYC(0x6c75, 0x6c78); objectSetVisible80_hook(gb); return; // jp

state1:
  CYC(0x6c78, 0x6c7a); C = 0x10;
  CALL_C(0x6c7a, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6c7d);
  if (!(F & FZ)) { RET_TAKEN(0x6c7d); return; } // ret nz
  CYC(0x6c7d, 0x6c7e);
  CALL_C(0x6c7e, objectSetInvisible_hook, 0x1e7b, 0x6c81);
  CYC(0x6c81, 0x6c84); A = W8(wTextIsActive);
  CYC(0x6c84, 0x6c85); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x6c85); return; } // ret z
  CYC(0x6c85, 0x6c86);
  CYC(0x6c86, 0x6c88); L = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6c88, 0x6c89); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(0x6c89);
  return; // ret

state2:
  CALL_C(0x6c8a, retIfTextIsActive_hook, 0x1859, 0x6c8d);
  CALL_C(0x6c8d, interactionIncState_hook, 0x23e0, 0x6c90);
  CYC(0x6c90, 0x6c92); L = INTERACTION_BASE + OBJ_OAM_FLAGS_BACKUP;
  CYC(0x6c92, 0x6c94); A = 0x0a;
  CYC(0x6c94, 0x6c95); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6c95, 0x6c96); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6c96, 0x6c98); mem_wr(gb, HL, 0x06); // [oamTileIndexBase] = $06
  CYC(0x6c98, 0x6c9a); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6c9a, 0x6c9c); mem_wr(gb, HL, 0x3a); // 58
  CYC(0x6c9c, 0x6c9e); A = 0x0b;
  CALL_C(0x6c9e, interactionSetAnimation_hook, 0x262e, 0x6ca1);
  CYC(0x6ca1, 0x6ca4); objectSetVisible_hook(gb); return; // jp

state3:
  CALL_C(0x6ca4, interactionAnimate_hook, 0x261b, 0x6ca7);
  CALL_C(0x6ca7, interactionDecCounter1_hook, 0x23cc, 0x6caa);
  if (!(F & FZ)) { RET_TAKEN(0x6caa); return; } // ret nz
  CYC(0x6caa, 0x6cab);
  CYC(0x6cab, 0x6cae); interactionDelete_hook(gb); return; // jp
}
