#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t switch_hook_parent_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A); burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L); burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H); } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1); burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A; burn_rom(gb, 0, 10, 11, false); return HL;
}

void parentItemCode_switchHook_hook(GB *gb) {
  BASE(parentItemCode_switchHook);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0,b_+2); E=4; CYC(b_+2,b_+3); A=mem_rd(gb,DE); CYC(b_+3,b_+4); push_effect(gb,b_+4);
  do { uint16_t jt_ = (switch_hook_parent_jump_table(gb));
    if (jt_ == b_+8) { break; }
    else if (jt_ == b_+54) { goto state1; }
    else { hook_continue(gb,HL,sp0_); return; }
  } while (0);
  CYC(b_+8,b_+11); A=W8(wLinkObjectIndex); CYC(b_+11,b_+12); alu_rrca(gb);
  if (F&FC) { CYCT(b_+12,b_+15); clearParentItem_hook(gb); return; } CYC(b_+12,b_+15);
  CYC(b_+15,b_+18); A=W8(wLinkInAir); CYC(b_+18,b_+19); alu_or(gb,A);
  if (!(F&FZ)) { CYCT(b_+19,b_+22); clearParentItem_hook(gb); return; } CYC(b_+19,b_+22);
  CALL_C(b_+22,isLinkInHole_hook,SYM(isLinkInHole),b_+25); if (F&FC) { CYCT(b_+25,b_+28); clearParentItem_hook(gb); return; } CYC(b_+25,b_+28);
  CALL_C(b_+28,updateLinkDirectionFromAngle_hook,SYM(updateLinkDirectionFromAngle),b_+31);
  CALL_C(b_+31,clearVariousLinkVariables_hook,SYM(clearVariousLinkVariables),b_+34);
  CYC(b_+34,b_+35); H=D; CYC(b_+35,b_+37); L=0; CYC(b_+37,b_+39); mem_wr(gb,HL,0xff);
  CALL_C(b_+39,parentItemLoadAnimationAndIncState_hook,SYM(parentItemLoadAnimationAndIncState),b_+42);
  CALL_C(b_+42,itemCreateChild_hook,SYM(itemCreateChild),b_+45);
  CALL_C(b_+45,isLinkUnderwater_hook,SYM(isLinkUnderwater),b_+48);
  if (F&FZ) { CYCT(b_+48,b_+49); ret_effect(gb); return; } CYC(b_+48,b_+49);
  CYC(b_+49,b_+51); A=0x2e; CYC(b_+51,b_+54); specialObjectSetAnimationWithLinkData_hook(gb); return;
state1:
  CYC(b_+54,b_+57); A=W8(w1WeaponItem_var2f); CYC(b_+57,b_+58); alu_or(gb,A);
  if (F&FZ) { CYCT(b_+58,b_+61); clearParentItem_hook(gb); return; } CYC(b_+58,b_+61);
  CYC(b_+61,b_+64); W8(wDisallowMountingCompanion)=A;
  CALL_C(b_+64,clearVariousLinkVariables_hook,SYM(clearVariousLinkVariables),b_+67);
  CYC(b_+67,b_+70); SET_HL(w1Link_var2a); CYC(b_+70,b_+71); A=mem_rd(gb,HL); CYC(b_+71,b_+73); L=(uint8_t)w1Link_knockbackCounter; CYC(b_+73,b_+74); alu_or(gb,mem_rd(gb,HL));
  if (F&FZ) { CYCT(b_+74,b_+75); ret_effect(gb); return; } CYC(b_+74,b_+75);
  CYC(b_+75,b_+78); SET_HL(w1WeaponItem_var2f); CYC(b_+78,b_+80); mem_wr(gb,HL,mem_rd(gb,HL)|(1<<5)); CYC(b_+80,b_+81); ret_effect(gb);
}
