#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t beam_jump_table(GB *gb) {
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

static void beam_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  }
  ret_effect(gb);
}

void func_5758_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x5758, objectApplyComponentSpeed_hook, 0x2008, 0x575b);
  CYC(0x575b, 0x575d); E = 0xc2; // Part.subid
  CYC(0x575d, 0x575e); A = mem_rd(gb, DE);
  CYC(0x575e, 0x575f); B = A;
  CYC(0x575f, 0x5762); A = mem_rd(gb, wFrameCounter);
  CYC(0x5762, 0x5763); alu_and(gb, B);
  if (F & FZ) { CYCT(0x5763, 0x5766); objectSetVisible81_hook(gb); return; } // jp z
  CYC(0x5763, 0x5766);
  CYC(0x5766, 0x5769); objectSetInvisible_hook(gb); return; // jp
}

void partCode29_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x5705, 0x5707); goto normalStatus; } // jr z
  CYC(0x5705, 0x5707);
  CYC(0x5707, 0x5709); E = 0xea; // Part.var2a
  CYC(0x5709, 0x570a); A = mem_rd(gb, DE);
  CYC(0x570a, 0x570c); alu_cp(gb, 0x83);
  if (F & FZ) { CYCT(0x570c, 0x570f); partDelete_hook(gb); return; } // jp z
  CYC(0x570c, 0x570f);

normalStatus:
  CYC(0x570f, 0x5711); E = 0xc4; // Part.state
  CYC(0x5711, 0x5712); A = mem_rd(gb, DE);
  {
    CYC(0x5712, 0x5713); push_effect(gb, 0x5713);
    uint16_t target = beam_jump_table(gb);
    if (target == 0x5719) goto state0;
    if (target == 0x5747) goto state1;
    goto state2;
  }

state0:
  CYC(0x5719, 0x571a); H = D;
  CYC(0x571a, 0x571b); L = E;
  CYC(0x571b, 0x571c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x571c, 0x571e); L = 0xc6; // Part.counter1
  CYC(0x571e, 0x5720); mem_wr(gb, HL, 0x02);
  CYC(0x5720, 0x5722); L = 0xc9; // Part.angle
  CYC(0x5722, 0x5723); C = mem_rd(gb, HL);
  CYC(0x5723, 0x5725); B = 0x50;
  CYC(0x5725, 0x5727); A = 0x04;
  CALL_C(0x5727, objectSetComponentSpeedByScaledVelocity_hook, 0x214c, 0x572a);
  CYC(0x572a, 0x572c); E = 0xc9; // Part.angle
  CYC(0x572c, 0x572d); A = mem_rd(gb, DE);
  CYC(0x572d, 0x572f); alu_and(gb, 0x0f);
  CYC(0x572f, 0x5732); SET_HL(0x5737); // table_5737
  CYC(0x5732, 0x5733); beam_addAToHl_from_rst(gb, 0x5733);
  CYC(0x5733, 0x5734); A = mem_rd(gb, HL);
  CYC(0x5734, 0x5737); partSetAnimation_hook(gb); return; // jp

state1:
  CALL_C(0x5747, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x574a);
  if (!(F & FZ)) { CYCT(0x574a, 0x574c); func_5758_hook(gb); return; } // jr nz
  CYC(0x574a, 0x574c);
  CYC(0x574c, 0x574d); L = E;
  CYC(0x574d, 0x574e); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));

state2:
  CALL_C(0x574e, func_5758_hook, 0x5758, 0x5751);
  CALL_C(0x5751, partCommon_checkTileCollisionOrOutOfBounds_hook, 0x4072, 0x5754);
  if (F & FC) { CYCT(0x5754, 0x5757); partDelete_hook(gb); return; } // jp c
  CYC(0x5754, 0x5757);
  RET(0x5757); return; // ret
}
