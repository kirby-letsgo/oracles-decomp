#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x06, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x06, (from), (to), true)

static uint16_t switch_hook_parent_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A); burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L); burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H); } else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1); burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A; burn_rom(gb, 0, 10, 11, false); return HL;
}

void parentItemCode_switchHook_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4b16,0x4b18); E=4; CYC(0x4b18,0x4b19); A=mem_rd(gb,DE); CYC(0x4b19,0x4b1a); push_effect(gb,0x4b1a);
  switch (switch_hook_parent_jump_table(gb)) { case 0x4b1e: break; case 0x4b4c: goto state1; default: hook_continue(gb,HL,sp0_); return; }
  CYC(0x4b1e,0x4b21); A=W8(wLinkObjectIndex); CYC(0x4b21,0x4b22); alu_rrca(gb);
  if (F&FC) { CYCT(0x4b22,0x4b25); clearParentItem_hook(gb); return; } CYC(0x4b22,0x4b25);
  CYC(0x4b25,0x4b28); A=W8(wLinkInAir); CYC(0x4b28,0x4b29); alu_or(gb,A);
  if (!(F&FZ)) { CYCT(0x4b29,0x4b2c); clearParentItem_hook(gb); return; } CYC(0x4b29,0x4b2c);
  CALL_C(0x4b2c,isLinkInHole_hook,0x54d8,0x4b2f); if (F&FC) { CYCT(0x4b2f,0x4b32); clearParentItem_hook(gb); return; } CYC(0x4b2f,0x4b32);
  CALL_C(0x4b32,updateLinkDirectionFromAngle_hook,0x2b64,0x4b35);
  CALL_C(0x4b35,clearVariousLinkVariables_hook,0x2c9b,0x4b38);
  CYC(0x4b38,0x4b39); H=D; CYC(0x4b39,0x4b3b); L=0; CYC(0x4b3b,0x4b3d); mem_wr(gb,HL,0xff);
  CALL_C(0x4b3d,parentItemLoadAnimationAndIncState_hook,0x5378,0x4b40);
  CALL_C(0x4b40,itemCreateChild_hook,0x53dd,0x4b43);
  CALL_C(0x4b43,isLinkUnderwater_hook,0x54d2,0x4b46);
  if (F&FZ) { CYCT(0x4b46,0x4b47); ret_effect(gb); return; } CYC(0x4b46,0x4b47);
  CYC(0x4b47,0x4b49); A=0x2e; CYC(0x4b49,0x4b4c); specialObjectSetAnimationWithLinkData_hook(gb); return;
state1:
  CYC(0x4b4c,0x4b4f); A=W8(w1WeaponItem_var2f); CYC(0x4b4f,0x4b50); alu_or(gb,A);
  if (F&FZ) { CYCT(0x4b50,0x4b53); clearParentItem_hook(gb); return; } CYC(0x4b50,0x4b53);
  CYC(0x4b53,0x4b56); W8(wDisallowMountingCompanion)=A;
  CALL_C(0x4b56,clearVariousLinkVariables_hook,0x2c9b,0x4b59);
  CYC(0x4b59,0x4b5c); SET_HL(w1Link_var2a); CYC(0x4b5c,0x4b5d); A=mem_rd(gb,HL); CYC(0x4b5d,0x4b5f); L=(uint8_t)w1Link_knockbackCounter; CYC(0x4b5f,0x4b60); alu_or(gb,mem_rd(gb,HL));
  if (F&FZ) { CYCT(0x4b60,0x4b61); ret_effect(gb); return; } CYC(0x4b60,0x4b61);
  CYC(0x4b61,0x4b64); SET_HL(w1WeaponItem_var2f); CYC(0x4b64,0x4b66); mem_wr(gb,HL,mem_rd(gb,HL)|(1<<5)); CYC(0x4b66,0x4b67); ret_effect(gb);
}
