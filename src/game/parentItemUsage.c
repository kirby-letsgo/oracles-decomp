#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

void initializeParentItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4980, 0x4981); A = C;
  CYC(0x4981, 0x4983); alu_and(gb, 0xf0);
  CYC(0x4983, 0x4984); A = alu_inc8(gb, A);
  CYC(0x4984, 0x4986); L = 0x00;
  CYC(0x4986, 0x4987); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4987, 0x4988); mem_wr(gb, HL, E);
  CYC(0x4988, 0x4989); L = alu_inc8(gb, L);
  CYC(0x4989, 0x498a); L = alu_inc8(gb, L);
  CYC(0x498a, 0x498b); mem_wr(gb, HL, D);
  CYC(0x498b, 0x498c); ret_effect(gb);
}

void checkShopInput_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x49da, 0x49dd); A = W8(wLinkGrabState);
  CYC(0x49dd, 0x49de); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x49de, 0x49df); ret_effect(gb); return;
  }
  CYC(0x49de, 0x49df);
  CYC(0x49df, 0x49e2); A = W8(wGameKeysJustPressed);
  CYC(0x49e2, 0x49e4); alu_and(gb, 0x03);
  if (F & FZ) {
    CYCT(0x49e4, 0x49e5); ret_effect(gb); return;
  }
  CYC(0x49e4, 0x49e5);
  CALL_C(0x49e5, checkGrabbableObjects_hook, 0x1c97, 0x49e8);
  if (!(F & FC)) {
    CYCT(0x49e8, 0x49e9); ret_effect(gb); return;
  }
  CYC(0x49e8, 0x49e9);
  CYC(0x49e9, 0x49eb); A = 0x83;
  CYC(0x49eb, 0x49ee); W8(wLinkGrabState) = A;
  CYC(0x49ee, 0x49ef); ret_effect(gb);
}

void clearParentItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4a42, clearLinkUsingItem1, 0x5450, 0x4a45);
  CALL_C(0x4a45, itemEnableLinkTurning, 0x5479, 0x4a48);
  CALL_C(0x4a48, itemEnableLinkMovement, 0x5466, 0x4a4b);
  CYC(0x4a4b, 0x4a4d); E = 0x00;
  CYC(0x4a4d, 0x4a50); objectDelete_de_hook(gb);
}

void clearParentItemH_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4a50, 0x4a51); push_effect(gb, DE);
  CYC(0x4a51, 0x4a52); D = H;
  CALL_C(0x4a52, clearParentItem_hook, 0x4a42, 0x4a55);
  CYC(0x4a55, 0x4a56); SET_DE(pop_effect(gb));
  CYC(0x4a56, 0x4a57); ret_effect(gb);
}
