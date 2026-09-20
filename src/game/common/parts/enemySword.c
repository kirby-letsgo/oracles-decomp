#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static void enemySword_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void enemySword_func_5273_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x5273, 0x5275); A = 0x01;
  CALL_C(0x5275, objectGetRelatedObject1Var_hook, 0x2160, 0x5278);
  CYC(0x5278, 0x527a); E = 0xf0; // Part.var30
  CYC(0x527a, 0x527b); A = mem_rd(gb, DE);
  CYC(0x527b, 0x527c); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x527c, 0x527d); return; } // ret nz
  CYC(0x527c, 0x527d);
  CYC(0x527d, 0x527f); L = 0xb0;
  CYC(0x527f, 0x5281); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x5281, 0x5283); goto L_5294; } // jr nz
  CYC(0x5281, 0x5283);
  CYC(0x5283, 0x5285); L = 0xa9;
  CYC(0x5285, 0x5286); A = mem_rd(gb, HL);
  CYC(0x5286, 0x5287); alu_or(gb, A);
  if (F & FZ) { CYCT(0x5287, 0x5289); goto L_5294; } // jr z
  CYC(0x5287, 0x5289);
  CYC(0x5289, 0x528b); L = 0xae;
  CYC(0x528b, 0x528c); A = mem_rd(gb, HL);
  CYC(0x528c, 0x528d); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x528d, 0x528f); goto L_5294; } // jr nz
  CYC(0x528d, 0x528f);
  CYC(0x528f, 0x5291); L = 0xbf;
  CYC(0x5291, 0x5293); alu_bit(gb, 1, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x5293, 0x5294); return; } // ret z
  CYC(0x5293, 0x5294);

L_5294:
  CYC(0x5294, 0x5296); E = 0xe4; // Part.collisionType
  CYC(0x5296, 0x5297); A = mem_rd(gb, DE);
  CYC(0x5297, 0x5299); A = (uint8_t)(A & ~(1 << 7));
  CYC(0x5299, 0x529a); mem_wr(gb, DE, A);
  CYC(0x529a, 0x529b); alu_xor(gb, A);
  CYC(0x529b, 0x529c); return; // ret
}

void partCode1d_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(0x51e6, 0x51e8); goto normalStatus; } // jr z
  CYC(0x51e6, 0x51e8);
  CYC(0x51e8, 0x51ea); E = 0xea; // Part.var2a
  CYC(0x51ea, 0x51eb); A = mem_rd(gb, DE);
  CYC(0x51eb, 0x51ed); alu_cp(gb, 0x80);
  if (F & FZ) { CYCT(0x51ed, 0x51ef); goto normalStatus; } // jr z
  CYC(0x51ed, 0x51ef);
  CYC(0x51ef, 0x51f1); alu_cp(gb, 0x8a);
  if (F & FZ) { CYCT(0x51f1, 0x51f3); goto normalStatus; } // jr z
  CYC(0x51f1, 0x51f3);
  CYC(0x51f3, 0x51f5); A = 0x2b;
  CALL_C(0x51f5, objectGetRelatedObject1Var_hook, 0x2160, 0x51f8);
  CYC(0x51f8, 0x51f9); A = mem_rd(gb, HL);
  CYC(0x51f9, 0x51fa); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x51fa, 0x51fc); goto L_5200; } // jr nz
  CYC(0x51fa, 0x51fc);
  CYC(0x51fc, 0x51fe); E = 0xeb; // Part.invincibilityCounter
  CYC(0x51fe, 0x51ff); A = mem_rd(gb, DE);
  CYC(0x51ff, 0x5200); mem_wr(gb, HL, A);

L_5200:
  CYC(0x5200, 0x5202); E = 0xec;
  CYC(0x5202, 0x5203); A = mem_rd(gb, DE);
  CYC(0x5203, 0x5204); L = alu_inc8(gb, L);
  CYC(0x5204, 0x5205); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x5205, 0x5207); E = 0xed; // Part.knockbackCounter
  CYC(0x5207, 0x5208); A = mem_rd(gb, DE);
  CYC(0x5208, 0x5209); mem_wr(gb, HL, A);

normalStatus:
  CYC(0x5209, 0x520b); E = 0xc4; // Part.state
  CYC(0x520b, 0x520c); A = mem_rd(gb, DE);
  CYC(0x520c, 0x520d); alu_or(gb, A);
  if (F & FZ) { CYCT(0x520d, 0x520f); goto func_5261; } // jr z
  CYC(0x520d, 0x520f);
  CYC(0x520f, 0x5210); H = D;
  CYC(0x5210, 0x5212); L = 0xe4; // Part.collisionType
  CYC(0x5212, 0x5214); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x5214, 0x5217); enemySword_func_5273_hook(gb);
  if (!(F & FZ)) { CYCT(0x5217, 0x521a); partDelete_hook(gb); return; } // jp nz
  CYC(0x5217, 0x521a);

func_521a:
  CYC(0x521a, 0x521c); L = 0x8b;
  CYC(0x521c, 0x521d); B = mem_rd(gb, HL);
  CYC(0x521d, 0x521f); L = 0x8d;
  CYC(0x521f, 0x5220); C = mem_rd(gb, HL);
  CYC(0x5220, 0x5222); L = 0x89;
  CYC(0x5222, 0x5223); A = mem_rd(gb, HL);
  CYC(0x5223, 0x5225); alu_add(gb, 0x04);
  CYC(0x5225, 0x5227); alu_and(gb, 0x18);
  CYC(0x5227, 0x5228); alu_rrca(gb);
  CYC(0x5228, 0x522a); H8(hFF8B) = A;
  CYC(0x522a, 0x522c); L = 0xa1;
  CYC(0x522c, 0x522d); alu_add(gb, mem_rd(gb, HL));
  CYC(0x522d, 0x522e); alu_add(gb, mem_rd(gb, HL));
  CYC(0x522e, 0x5231); SET_HL(0x524d);
  CYC(0x5231, 0x5232); enemySword_addAToHl_from_rst(gb, 0x5232);
  CYC(0x5232, 0x5234); E = 0xcb; // Part.yh
  CYC(0x5234, 0x5235); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5235, 0x5236); alu_add(gb, B);
  CYC(0x5236, 0x5237); mem_wr(gb, DE, A);
  CYC(0x5237, 0x5239); E = 0xcd; // Part.xh
  CYC(0x5239, 0x523a); A = mem_rd(gb, HL);
  CYC(0x523a, 0x523b); alu_add(gb, C);
  CYC(0x523b, 0x523c); mem_wr(gb, DE, A);
  CYC(0x523c, 0x523e); A = H8(hFF8B);
  CYC(0x523e, 0x523f); alu_rrca(gb);
  CYC(0x523f, 0x5241); alu_and(gb, 0x02);
  CYC(0x5241, 0x5244); SET_HL(0x525d);
  CYC(0x5244, 0x5245); enemySword_addAToHl_from_rst(gb, 0x5245);
  CYC(0x5245, 0x5247); E = 0xe6; // Part.collisionRadiusY
  CYC(0x5247, 0x5248); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x5248, 0x5249); mem_wr(gb, DE, A);
  CYC(0x5249, 0x524a); E = alu_inc8(gb, E);
  CYC(0x524a, 0x524b); A = mem_rd(gb, HL);
  CYC(0x524b, 0x524c); mem_wr(gb, DE, A);
  RET(0x524c); return; // ret

func_5261:
  CYC(0x5261, 0x5262); H = D;
  CYC(0x5262, 0x5263); L = E;
  CYC(0x5263, 0x5264); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x5264, 0x5266); L = 0xfe;
  CYC(0x5266, 0x5268); mem_wr(gb, HL, 0x04);
  CYC(0x5268, 0x526a); A = 0x01;
  CALL_C(0x526a, objectGetRelatedObject1Var_hook, 0x2160, 0x526d);
  CYC(0x526d, 0x526f); E = 0xf0; // Part.var30
  CYC(0x526f, 0x5270); A = mem_rd(gb, HL);
  CYC(0x5270, 0x5271); mem_wr(gb, DE, A);
  CYC(0x5271, 0x5273); goto func_521a; // jr
}
