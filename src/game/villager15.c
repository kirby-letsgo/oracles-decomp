#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_MALE_VILLAGER), bank 0x15.

void villager_setLinkYToVar39_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(0x582c, 0x582f); SET_HL(w1Link_yh);
  CYC(0x582f, 0x5831); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(0x5831, 0x5832); A = mem_rd(gb, DE);
  CYC(0x5832, 0x5833); mem_wr(gb, HL, A);
  RET(0x5833); return;
}

// Creates a ball object for the purpose of a cutscene.
void villager_createBallAccessory_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5834, getFreeInteractionSlot_hook, 0x3aef, 0x5837);
  if (!(F & FZ)) { RET_TAKEN(0x5837); return; }
  CYC(0x5837, 0x5838);
  CYC(0x5838, 0x583a); mem_wr(gb, HL, 0x63); // INTERAC_ACCESSORY
  CYC(0x583a, 0x583b); L = alu_inc8(gb, L);
  CYC(0x583b, 0x583d); mem_wr(gb, HL, 0x3f);
  CYC(0x583d, 0x583e); L = alu_inc8(gb, L);
  CYC(0x583e, 0x5840); mem_wr(gb, HL, 0x01);
  CYC(0x5840, 0x5842); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(0x5842, 0x5844); mem_wr(gb, HL, 0x40); // Interaction.start
  CYC(0x5844, 0x5845); L = alu_inc8(gb, L);
  CYC(0x5845, 0x5846); mem_wr(gb, HL, D);
  RET(0x5846); return;
}

// Creates an actual ball that can be thrown by the villagers.
void villager_createBall_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x5847, 0x584a); SET_BC(0x9500); // INTERAC_BALL, $00
  CALL_C(0x584a, objectCreateInteraction_hook, 0x24c5, 0x584d);
  if (!(F & FZ)) { RET_TAKEN(0x584d); return; }
  CYC(0x584d, 0x584e);
  CYC(0x584e, 0x5851); SET_BC(0x4a3c);
  CYC(0x5851, 0x5854); interactionHSetPosition_hook(gb); return; // jp
}
