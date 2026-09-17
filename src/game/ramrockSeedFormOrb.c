#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t ramrockSeedFormOrb_jump_table(GB *gb) {
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

// PART_RAMROCK_SEED_FORM_ORB
void partCode4f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x7a5f, 0x7a61); E = 0xc4; // Part.state
  CYC(0x7a61, 0x7a62); A = mem_rd(gb, DE);
  {
    CYC(0x7a62, 0x7a63); push_effect(gb, 0x7a63);
    uint16_t target = ramrockSeedFormOrb_jump_table(gb);
    if (target == 0x7a78) goto state1;
    if (target == 0x7a99) goto state2;
    goto state0;
  }

state0:
  CYC(0x7a69, 0x7a6b); A = 0x01;
  CYC(0x7a6b, 0x7a6c); mem_wr(gb, DE, A); // Part.state
  CYC(0x7a6c, 0x7a6d); A = alu_inc8(gb, A);
  CALL_C(0x7a6d, partSetAnimation_hook, 0x2988, 0x7a70);
  CYC(0x7a70, 0x7a72); E = 0xc6; // Part.counter1
  CYC(0x7a72, 0x7a74); A = 0x28;
  CYC(0x7a74, 0x7a75); mem_wr(gb, DE, A);
  CYC(0x7a75, 0x7a78); objectSetVisible80_hook(gb); return; // jp

state1:
  CALL_C(0x7a78, partAnimate_hook, 0x2978, 0x7a7b);
  CYC(0x7a7b, 0x7a7d); A = 0x02; // Object.state
  CALL_C(0x7a7d, objectGetRelatedObject1Var_hook, 0x2160, 0x7a80);
  CYC(0x7a80, 0x7a81); A = mem_rd(gb, HL);
  CYC(0x7a81, 0x7a83); alu_cp(gb, 0x0f);
  if (!(F & FZ)) { CYCT(0x7a83, 0x7a85); goto delete; } // jr nz
  CYC(0x7a83, 0x7a85);
  CALL_C(0x7a85, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7a88);
  if (!(F & FZ)) { RET_TAKEN(0x7a88); return; } // ret nz
  CYC(0x7a88, 0x7a89);
  CALL_C(0x7a89, objectGetAngleTowardLink_hook, 0x1e9c, 0x7a8c);
  CYC(0x7a8c, 0x7a8e); E = 0xc9; // Part.angle
  CYC(0x7a8e, 0x7a8f); mem_wr(gb, DE, A);
  CYC(0x7a8f, 0x7a91); A = 0x50;
  CYC(0x7a91, 0x7a93); E = 0xd0; // Part.speed
  CYC(0x7a93, 0x7a94); mem_wr(gb, DE, A);
  CYC(0x7a94, 0x7a96); E = 0xc4; // Part.state
  CYC(0x7a96, 0x7a98); A = 0x02;
  CYC(0x7a98, 0x7a99); mem_wr(gb, DE, A);

state2:
  CALL_C(0x7a99, partAnimate_hook, 0x2978, 0x7a9c);
  CALL_C(0x7a9c, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x7a9f);
  if (!(F & FZ)) { CYCT(0x7a9f, 0x7aa1); goto func_7aa9; } // jr nz
  CYC(0x7a9f, 0x7aa1);
  CYC(0x7aa1, 0x7aa3); mem_wr(gb, HL, 0x0a);
  CALL_C(0x7aa3, objectGetAngleTowardLink_hook, 0x1e9c, 0x7aa6);
  CYC(0x7aa6, 0x7aa9); objectNudgeAngleTowards_hook(gb); return; // jp

func_7aa9:
  CALL_C(0x7aa9, objectApplySpeed_hook, 0x201d, 0x7aac);
  CALL_C(0x7aac, objectCheckWithinScreenBoundary_hook, 0x2184, 0x7aaf);
  if (F & FC) { RET_TAKEN(0x7aaf); return; } // ret c
  CYC(0x7aaf, 0x7ab0);

delete:
  CYC(0x7ab0, 0x7ab3); partDelete_hook(gb); return; // jp
}
