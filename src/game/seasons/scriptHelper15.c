#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/scripts/common/scriptHelper.s and the "Generic" section of
// scripts/seasons/scriptHelper.s, bank 0x15: helpers shared by several scripts.

// A boss death explosion part at the object.
void s_createBossDeathExplosion_hook(GB *gb) {
  BASE(createBossDeathExplosion);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, getFreePartSlot_hook, SYM(getFreePartSlot), b_+3);
  if (!(F & FZ)) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); mem_wr(gb, HL, 0x04); // PART_BOSS_DEATH_EXPLOSION
  CYC(b_+6, b_+9);
  TAIL(objectCopyPosition);
}

void s_faceOppositeDirectionAsLink_hook(GB *gb) {
  BASE(faceOppositeDirectionAsLink);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_direction);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+6); alu_xor(gb, 0x02);
  CYC(b_+6, b_+9);
  TAIL(interactionSetAnimation);
}

void s_linkedScript_giveRing_hook(GB *gb) {
  BASE(linkedScript_giveRing);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); B = A;
  CYC(b_+1, b_+3); C = 0x00;
  CYC(b_+3, b_+6);
  TAIL(giveRingToLink);
}

// Link becomes the cutscene special object, subid 8.
void s_playLinkCutscene2_hook(GB *gb) {
  BASE(playLinkCutscene2);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x08; // SPECIALOBJECT_LINK_CUTSCENE
  CALL_C(b_+2, setLinkIDOverride_hook, SYM(setLinkIDOverride), b_+5);
  CYC(b_+5, b_+7); L = 0x02; // w1Link.subid
  CYC(b_+7, b_+9); mem_wr(gb, HL, 0x08);
  RET(b_+9); return;
}

void s_forceLinkState8AndSetDirection_hook(GB *gb) {
  BASE(forceLinkState8AndSetDirection);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_direction);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  CYC(b_+4, b_+7);
  TAIL(setLinkForceStateToState08);
}
