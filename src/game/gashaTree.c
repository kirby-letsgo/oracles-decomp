#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t gashaTree_jump_table(GB *gb) {
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

static void gashaTree_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001e, 0x001f, false);
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void partCode17_hook(GB *gb);
void gashaTree_func_4fb2_hook(GB *gb);
void func_5010_hook(GB *gb);

void partCode17_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x4f59, 0x4f5b); goto normalStatus; } // jr z
  CYC(0x4f59, 0x4f5b);
  CYC(0x4f5b, 0x4f5d); E = 0xc2; // Part.subid
  CYC(0x4f5d, 0x4f5e); A = mem_rd(gb, DE);
  CYC(0x4f5e, 0x4f5f); alu_add(gb, A);
  CYC(0x4f5f, 0x4f62); SET_HL(0x501e); // table_501e
  CYC(0x4f62, 0x4f63); gashaTree_addDoubleIndexToHl_from_rst(gb, 0x4f63);
  CYC(0x4f63, 0x4f65); E = 0xea; // Part.var2a
  CYC(0x4f65, 0x4f66); A = mem_rd(gb, DE);
  CYC(0x4f66, 0x4f68); alu_and(gb, 0x1f);
  CALL_C(0x4f68, checkFlag_hook, 0x0205, 0x4f6b);
  if (F & FZ) { CYCT(0x4f6b, 0x4f6d); goto normalStatus; } // jr z
  CYC(0x4f6b, 0x4f6d);
  CALL_C(0x4f6d, checkLinkVulnerable_hook, 0x1d28, 0x4f70);
  if (!(F & FC)) { CYCT(0x4f70, 0x4f72); goto normalStatus; } // jr nc
  CYC(0x4f70, 0x4f72);
  CYC(0x4f72, 0x4f73); H = D;
  CYC(0x4f73, 0x4f75); L = 0xc4; // Part.state
  CYC(0x4f75, 0x4f77); mem_wr(gb, HL, 0x02);
  CYC(0x4f77, 0x4f79); L = 0xe4; // Part.collisionType
  CYC(0x4f79, 0x4f7b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x4f7b, 0x4f7d); L = 0xc2; // Part.subid
  CYC(0x4f7d, 0x4f7e); A = mem_rd(gb, HL);
  CYC(0x4f7e, 0x4f7f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4f7f, 0x4f81); goto normalStatus; } // jr z
  CYC(0x4f7f, 0x4f81);
  CYC(0x4f81, 0x4f83); A = 0x2a;
  CALL_C(0x4f83, objectGetRelatedObject1Var_hook, 0x2160, 0x4f86);
  CYC(0x4f86, 0x4f88); mem_wr(gb, HL, 0xff);

normalStatus:
  CYC(0x4f88, 0x4f8a); E = 0xc4; // Part.state
  CYC(0x4f8a, 0x4f8b); A = mem_rd(gb, DE);
  {
    CYC(0x4f8b, 0x4f8c); push_effect(gb, 0x4f8c);
    uint16_t target = gashaTree_jump_table(gb);
    if (target == 0x4f92) goto state0;
    if (target == 0x4fab) goto state1;
    goto state2;
  }

state0:
  CYC(0x4f92, 0x4f94); A = 0x01;
  CYC(0x4f94, 0x4f95); mem_wr(gb, DE, A);
  CYC(0x4f95, 0x4f97); A = 0x26;
  CALL_C(0x4f97, objectGetRelatedObject1Var_hook, 0x2160, 0x4f9a);
  CYC(0x4f9a, 0x4f9c); E = 0xe6; // Part.collisionRadiusY
  CYC(0x4f9c, 0x4f9d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4f9d, 0x4f9e); mem_wr(gb, DE, A);
  CYC(0x4f9e, 0x4f9f); E = alu_inc8(gb, E);
  CYC(0x4f9f, 0x4fa0); A = mem_rd(gb, HL);
  CYC(0x4fa0, 0x4fa1); mem_wr(gb, DE, A); // collisionRadiusX
  CALL_C(0x4fa1, objectTakePosition_hook, 0x2274, 0x4fa4);
  CYC(0x4fa4, 0x4fa6); E = 0xf0; // Part.var30
  CYC(0x4fa6, 0x4fa8); L = 0x41;
  CYC(0x4fa8, 0x4fa9); A = mem_rd(gb, HL);
  CYC(0x4fa9, 0x4faa); mem_wr(gb, DE, A);
  RET(0x4faa); return; // ret

state1:
  CYC(0x4fab, 0x4fae); gashaTree_func_4fb2_hook(gb);
  if (F & FZ) { RET_TAKEN(0x4fae); return; } // ret z
  CYC(0x4fae, 0x4faf);
  CYC(0x4faf, 0x4fb2); partDelete_hook(gb); return; // jp

state2:
  CYC(0x4fbc, 0x4fbf); gashaTree_func_4fb2_hook(gb);
  if (!(F & FZ)) { CYCT(0x4fbf, 0x4fc2); partDelete_hook(gb); return; } // jp nz
  CYC(0x4fbf, 0x4fc2);
  CYC(0x4fc2, 0x4fc4); E = 0xc5; // Part.substate
  CYC(0x4fc4, 0x4fc5); A = mem_rd(gb, DE);
  {
    CYC(0x4fc5, 0x4fc6); push_effect(gb, 0x4fc6);
    uint16_t target = gashaTree_jump_table(gb);
    if (target == 0x4fcc) goto substate0;
    if (target == 0x4fec) goto substate1;
    goto substate2;
  }

substate0:
  CYC(0x4fcc, 0x4fcd); H = D;
  CYC(0x4fcd, 0x4fce); L = E;
  CYC(0x4fce, 0x4fcf); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x4fcf, 0x4fd1); L = 0xd0; // Part.speed
  CYC(0x4fd1, 0x4fd3); mem_wr(gb, HL, 0x28); // SPEED_100
  CYC(0x4fd3, 0x4fd5); A = 0x1a;
  CALL_C(0x4fd5, objectGetRelatedObject1Var_hook, 0x2160, 0x4fd8);
  CYC(0x4fd8, 0x4fda); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 6)));
  CYC(0x4fda, 0x4fdc); E = 0xc2; // Part.subid
  CYC(0x4fdc, 0x4fdd); A = mem_rd(gb, DE);
  CYC(0x4fdd, 0x4fde); alu_or(gb, A);
  CYC(0x4fde, 0x4fe0); A = 0x10;
  if (!(F & FZ)) {
    CALL_C_CC(0x4fe0, objectGetAngleTowardLink_hook, 0x1e9c, 0x4fe3);
  } else {
    CYC(0x4fe0, 0x4fe3);
  }
  CYC(0x4fe3, 0x4fe5); E = 0xc9; // Part.angle
  CYC(0x4fe5, 0x4fe6); mem_wr(gb, DE, A);
  CYC(0x4fe6, 0x4fe9); SET_BC(0xfec0); // -0x140
  CYC(0x4fe9, 0x4fec); objectSetSpeedZ_hook(gb); return; // jp

substate1:
  CYC(0x4fec, 0x4fee); C = 0x18;
  CALL_C(0x4fee, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x4ff1);
  if (F & FZ) { CYCT(0x4ff1, 0x4ff3); goto L_4ffe; } // jr z
  CYC(0x4ff1, 0x4ff3);
  CALL_C(0x4ff3, objectApplySpeed_hook, 0x201d, 0x4ff6);
  CYC(0x4ff6, 0x4ff8); A = 0x00;
  CALL_C(0x4ff8, objectGetRelatedObject1Var_hook, 0x2160, 0x4ffb);
  CYC(0x4ffb, 0x4ffe); objectCopyPosition_hook(gb); return; // jp

L_4ffe:
  CYC(0x4ffe, 0x5000); E = 0xc5; // Part.substate
  CYC(0x5000, 0x5002); A = 0x02;
  CYC(0x5002, 0x5003); mem_wr(gb, DE, A);

substate2:
  CYC(0x5003, 0x5005); C = 0x18;
  CALL_C(0x5005, objectUpdateSpeedZAndBounce_hook, 0x2370, 0x5008);
  if (!(F & FC)) { CYCT(0x5008, 0x500a); func_5010_hook(gb); return; } // jr nc
  CYC(0x5008, 0x500a);
  CALL_C(0x500a, func_5010_hook, 0x5010, 0x500d);
  CYC(0x500d, 0x5010); partDelete_hook(gb); return; // jp
}

void gashaTree_func_4fb2_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4fb2, 0x4fb4); A = 0x01;
  CALL_C(0x4fb4, objectGetRelatedObject1Var_hook, 0x2160, 0x4fb7);
  CYC(0x4fb7, 0x4fb9); E = 0xf0; // Part.var30
  CYC(0x4fb9, 0x4fba); A = mem_rd(gb, DE);
  CYC(0x4fba, 0x4fbb); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x4fbb, 0x4fbc); return; // ret
}

void func_5010_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5010, objectCheckTileCollision_allowHoles_hook, 0x14c7, 0x5013);
  if (!(F & FC)) {
    CALL_C_CC(0x5013, objectApplySpeed_hook, 0x201d, 0x5016);
  } else {
    CYC(0x5013, 0x5016);
  }
  CYC(0x5016, 0x5018); A = 0x00;
  CALL_C(0x5018, objectGetRelatedObject1Var_hook, 0x2160, 0x501b);
  CYC(0x501b, 0x501e); objectCopyPosition_hook(gb); return; // jp
}
