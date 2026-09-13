#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

void itemIndexToBit_hook(GB *gb);
void andHlWithGameKeysPressed_hook(GB *gb);

void setLinkUsingItem1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5444, itemIndexToBit_hook, 0x548c, 0x5447);
  CYC(0x5447, 0x5449); A = alu_swap(gb, A);
  CYC(0x5449, 0x544a); alu_or(gb, mem_rd(gb, HL));
  CYC(0x544a, 0x544d); SET_HL(wLinkUsingItem1);
  CYC(0x544d, 0x544e); alu_or(gb, mem_rd(gb, HL));
  CYC(0x544e, 0x544f); mem_wr(gb, HL, A);
  CYC(0x544f, 0x5450); ret_effect(gb);
}

void clearLinkUsingItem1_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5450, itemIndexToBit_hook, 0x548c, 0x5453);
  CYC(0x5453, 0x5455); A = alu_swap(gb, A);
  CYC(0x5455, 0x5456); alu_or(gb, mem_rd(gb, HL));
  CYC(0x5456, 0x5457); alu_cpl(gb);
  CYC(0x5457, 0x545a); SET_HL(wLinkUsingItem1);
  CYC(0x545a, 0x545b); alu_and(gb, mem_rd(gb, HL));
  CYC(0x545b, 0x545c); mem_wr(gb, HL, A);
  CYC(0x545c, 0x545d); ret_effect(gb);
}

void itemDisableLinkMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x545d, itemIndexToBit_hook, 0x548c, 0x5460);
  CYC(0x5460, 0x5463); SET_HL(wLinkImmobilized);
  CYC(0x5463, 0x5464); alu_or(gb, mem_rd(gb, HL));
  CYC(0x5464, 0x5465); mem_wr(gb, HL, A);
  CYC(0x5465, 0x5466); ret_effect(gb);
}

void itemEnableLinkMovement_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5466, itemIndexToBit_hook, 0x548c, 0x5469);
  CYC(0x5469, 0x546c); SET_HL(wLinkImmobilized);
  CYC(0x546c, 0x546d); alu_cpl(gb);
  CYC(0x546d, 0x546e); alu_and(gb, mem_rd(gb, HL));
  CYC(0x546e, 0x546f); mem_wr(gb, HL, A);
  CYC(0x546f, 0x5470); ret_effect(gb);
}

void itemDisableLinkTurning_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5470, itemIndexToBit_hook, 0x548c, 0x5473);
  CYC(0x5473, 0x5476); SET_HL(wLinkTurningDisabled);
  CYC(0x5476, 0x5477); alu_or(gb, mem_rd(gb, HL));
  CYC(0x5477, 0x5478); mem_wr(gb, HL, A);
  CYC(0x5478, 0x5479); ret_effect(gb);
}

void itemEnableLinkTurning_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5479, itemIndexToBit_hook, 0x548c, 0x547c);
  CYC(0x547c, 0x547f); SET_HL(wLinkTurningDisabled);
  CYC(0x547f, 0x5480); alu_cpl(gb);
  CYC(0x5480, 0x5481); alu_and(gb, mem_rd(gb, HL));
  CYC(0x5481, 0x5482); mem_wr(gb, HL, A);
  CYC(0x5482, 0x5483); ret_effect(gb);
}

void setCc95Bit_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x5483, itemIndexToBit_hook, 0x548c, 0x5486);
  CYC(0x5486, 0x5489); SET_HL(wcc95);
  CYC(0x5489, 0x548a); alu_or(gb, mem_rd(gb, HL));
  CYC(0x548a, 0x548b); mem_wr(gb, HL, A);
  CYC(0x548b, 0x548c); ret_effect(gb);
}

void itemIndexToBit_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x548c, 0x548d); A = D;
  CYC(0x548d, 0x548f); alu_sub(gb, 0xd2);
  CYC(0x548f, 0x5492); SET_HL(0x00f8);
  CYC(0x5492, 0x5493); alu_add(gb, L);
  CYC(0x5493, 0x5494); L = A;
  CYC(0x5494, 0x5495); A = mem_rd(gb, HL);
  CYC(0x5495, 0x5496); ret_effect(gb);
}

void parentItemCheckButtonPressed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5496, 0x5497); H = D;
  CYC(0x5497, 0x5499); L = 0x03;
  andHlWithGameKeysPressed_hook(gb);
}

void andHlWithGameKeysPressed_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5499, 0x549c); A = W8(wGameKeysPressed);
  CYC(0x549c, 0x549d); alu_and(gb, mem_rd(gb, HL));
  CYC(0x549d, 0x549e); ret_effect(gb);
}
