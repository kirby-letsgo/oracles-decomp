#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t fallingFire_jump_table(GB *gb) {
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

static void fallingFire_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void func_5535_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5535, 0x5536); H = D;
  CYC(0x5536, 0x5537); L = E;
  CYC(0x5537, 0x5538); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5538, 0x553a); L = 0xe4; // Part.collisionType
  CYC(0x553a, 0x553c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x553c, 0x553f); objectSetVisible81_hook(gb); return; // jp
}

void func_553f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x553f, 0x5541); E = 0x87;
  CYC(0x5541, 0x5542); A = mem_rd(gb, DE);
  CYC(0x5542, 0x5543); A = alu_inc8(gb, A);
  CYC(0x5543, 0x5545); alu_and(gb, 0x03);
  CYC(0x5545, 0x5546); mem_wr(gb, DE, A);
  CYC(0x5546, 0x5549); SET_HL(0x554f); // table_554f
  CYC(0x5549, 0x554a); fallingFire_addAToHl_from_rst(gb, 0x554a);
  CYC(0x554a, 0x554c); E = 0xc6; // Part.counter1
  CYC(0x554c, 0x554d); A = mem_rd(gb, HL);
  CYC(0x554d, 0x554e); mem_wr(gb, DE, A);
  RET(0x554e); return; // ret
}

void partCode23_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x54de, 0x54e0); E = 0xc2; // Part.subid
  CYC(0x54e0, 0x54e1); A = mem_rd(gb, DE);
  CYC(0x54e1, 0x54e3); E = 0xc4; // Part.state
  {
    CYC(0x54e3, 0x54e4); push_effect(gb, 0x54e4);
    uint16_t target = fallingFire_jump_table(gb);
    if (target == 0x54ea) goto subid0;
    if (target == 0x54f9) goto subid1;
    goto subid2;
  }

subid0:
  CYC(0x54ea, 0x54eb); A = mem_rd(gb, DE);
  CYC(0x54eb, 0x54ec); alu_or(gb, A);
  if (F & FZ) { CYCT(0x54ec, 0x54ee); goto func_54f6; } // jr z
  CYC(0x54ec, 0x54ee);
  CALL_C(0x54ee, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x54f1);
  if (!(F & FZ)) { RET_TAKEN(0x54f1); return; } // ret nz
  CYC(0x54f1, 0x54f2);
  CYC(0x54f2, 0x54f4); mem_wr(gb, HL, 0x78);
  CYC(0x54f4, 0x54f6); goto L_5504; // jr

func_54f6:
  CYC(0x54f6, 0x54f7); A = alu_inc8(gb, A);
  CYC(0x54f7, 0x54f8); mem_wr(gb, DE, A);
  RET(0x54f8); return; // ret

subid1:
  CYC(0x54f9, 0x54fa); A = mem_rd(gb, DE);
  CYC(0x54fa, 0x54fb); alu_or(gb, A);
  if (F & FZ) { CYCT(0x54fb, 0x54fd); goto func_54f6; } // jr z
  CYC(0x54fb, 0x54fd);
  CALL_C(0x54fd, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x5500);
  if (!(F & FZ)) { RET_TAKEN(0x5500); return; } // ret nz
  CYC(0x5500, 0x5501);
  CALL_C(0x5501, func_553f_hook, 0x553f, 0x5504);

L_5504:
  CALL_C(0x5504, getFreePartSlot_hook, 0x3e8e, 0x5507);
  if (!(F & FZ)) { RET_TAKEN(0x5507); return; } // ret nz
  CYC(0x5507, 0x5508);
  CYC(0x5508, 0x550a); mem_wr(gb, HL, 0x23); // PART_FALLING_FIRE
  CYC(0x550a, 0x550b); L = alu_inc8(gb, L);
  CYC(0x550b, 0x550d); mem_wr(gb, HL, 0x02);
  CYC(0x550d, 0x550f); L = 0xf0; // Part.var30
  CYC(0x550f, 0x5510); E = L;
  CYC(0x5510, 0x5511); A = mem_rd(gb, DE);
  CYC(0x5511, 0x5512); mem_wr(gb, HL, A);
  CYC(0x5512, 0x5515); objectCopyPosition_hook(gb); return; // jp

subid2:
  CYC(0x5515, 0x5516); A = mem_rd(gb, DE);
  CYC(0x5516, 0x5517); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5517, 0x5519); func_5535_hook(gb); return; } // jr z
  CYC(0x5517, 0x5519);
  CYC(0x5519, 0x551a); H = D;
  CYC(0x551a, 0x551c); L = 0xcb; // Part.yh
  CYC(0x551c, 0x551d); A = mem_rd(gb, HL);
  CYC(0x551d, 0x551f); alu_cp(gb, 0xb0);
  if (!(F & FC)) { CYCT(0x551f, 0x5522); partDelete_hook(gb); return; } // jp nc
  CYC(0x551f, 0x5522);
  CYC(0x5522, 0x5524); L = 0xd0; // Part.speed
  CYC(0x5524, 0x5526); E = 0xca;
  CALL_C(0x5526, add16BitRefs_hook, 0x23a7, 0x5529);
  CYC(0x5529, 0x552a); L = alu_dec8(gb, L);
  CYC(0x552a, 0x552b); A = mem_rd(gb, HL);
  CYC(0x552b, 0x552d); alu_add(gb, 0x10);
  CYC(0x552d, 0x552e); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x552e, 0x552f); A = mem_rd(gb, HL);
  CYC(0x552f, 0x5531); alu_adc(gb, 0x00);
  CYC(0x5531, 0x5532); mem_wr(gb, HL, A);
  CYC(0x5532, 0x5535); partAnimate_hook(gb); return; // jp
}
