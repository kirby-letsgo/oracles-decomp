#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/ages/scriptHelper.s (INTERAC_MALE_VILLAGER), bank 0x15.

void villager_setLinkYToVar39_hook(GB *gb) {
  BASE(villager_setLinkYToVar39);
  uint16_t sp0_ = gb->sp;
  (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+7); mem_wr(gb, HL, A);
  RET(b_+7); return;
}

// Creates a ball object for the purpose of a cutscene.
void villager_createBallAccessory_hook(GB *gb) {
  BASE(villager_createBallAccessory);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x63); // INTERAC_ACCESSORY
  CYC(b_+6, b_+7); L = alu_inc8(gb, L);
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x3f);
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+12); mem_wr(gb, HL, 0x01);
  CYC(b_+12, b_+14); L = INTERACTION_BASE + OBJ_RELATED1;
  CYC(b_+14, b_+16); mem_wr(gb, HL, 0x40); // Interaction.start
  CYC(b_+16, b_+17); L = alu_inc8(gb, L);
  CYC(b_+17, b_+18); mem_wr(gb, HL, D);
  RET(b_+18); return;
}

// Creates an actual ball that can be thrown by the villagers.
void villager_createBall_hook(GB *gb) {
  BASE(villager_createBall);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_BC(0x9500); // INTERAC_BALL, $00
  CALL_C(b_+3, objectCreateInteraction_hook, SYM(objectCreateInteraction), b_+6);
  if (!(F & FZ)) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+10); SET_BC((SYM(group3ObjectDataTable) + 257));
  CYC(b_+10, b_+13); interactionHSetPosition_hook(gb); return; // jp
}
