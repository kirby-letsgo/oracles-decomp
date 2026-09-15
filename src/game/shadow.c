#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

void partCode07_hook(GB *gb);
void shadow_initialize_hook(GB *gb);

void partCode07_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4642, 0x4644); E = 0xc4; // Part.state
  CYC(0x4644, 0x4645); A = mem_rd(gb, DE);
  CYC(0x4645, 0x4646); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4646, 0x4649); // call z
    push_effect(gb, 0x4649);
    shadow_initialize_hook(gb);
  } else {
    CYC(0x4646, 0x4649);
  }
  CYC(0x4649, 0x464b); A = 0x01; // Object.id
  CALL_C(0x464b, objectGetRelatedObject1Var_hook, 0x2160, 0x464e);
  CYC(0x464e, 0x4650); E = 0xf0; // Part.var30
  CYC(0x4650, 0x4651); A = mem_rd(gb, DE);
  CYC(0x4651, 0x4652); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(0x4652, 0x4655); partDelete_hook(gb); return; } // jp nz
  CYC(0x4652, 0x4655);
  CYC(0x4655, 0x4657); A = 0x0b; // Object.yh
  CALL_C(0x4657, objectGetRelatedObject1Var_hook, 0x2160, 0x465a);
  CYC(0x465a, 0x465c); E = 0xc3; // Part.var03
  CYC(0x465c, 0x465d); A = mem_rd(gb, DE);
  CYC(0x465d, 0x465e); B = A;
  CYC(0x465e, 0x4660); C = 0x00;
  CALL_C(0x4660, objectTakePositionWithOffset_hook, 0x2277, 0x4663);
  CYC(0x4663, 0x4664); alu_xor(gb, A);
  CYC(0x4664, 0x4665); mem_wr(gb, DE, A); // [this.zh] = 0
  CYC(0x4665, 0x4666); A = mem_rd(gb, HL); // [parent.zh]
  CYC(0x4666, 0x4667); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4667, 0x466a); objectSetInvisible_hook(gb); return; } // jp z
  CYC(0x4667, 0x466a);
  CYC(0x466a, 0x466c); E = 0xda; // Part.visible
  CYC(0x466c, 0x466d); A = mem_rd(gb, DE);
  CYC(0x466d, 0x466f); alu_xor(gb, 0x80);
  CYC(0x466f, 0x4670); mem_wr(gb, DE, A);
  CYC(0x4670, 0x4672); E = 0xc2; // Part.subid
  CYC(0x4672, 0x4673); A = mem_rd(gb, DE);
  CYC(0x4673, 0x4674); alu_add(gb, A);
  CYC(0x4674, 0x4677); SET_BC(0x468e); // @animationIndices
  CALL_C(0x4677, addDoubleIndexToBc_hook, 0x007e, 0x467a);
  CYC(0x467a, 0x467b); A = mem_rd(gb, HL); // [parent.zh]
  CYC(0x467b, 0x467d); alu_cp(gb, 0xe0);
  if (!(F & FC)) { CYCT(0x467d, 0x467f); goto setAnim; } // jr nc
  CYC(0x467d, 0x467f);
  CYC(0x467f, 0x4680); SET_BC(BC + 1);
  CYC(0x4680, 0x4682); alu_cp(gb, 0xc0);
  if (!(F & FC)) { CYCT(0x4682, 0x4684); goto setAnim; } // jr nc
  CYC(0x4682, 0x4684);
  CYC(0x4684, 0x4685); SET_BC(BC + 1);
  CYC(0x4685, 0x4687); alu_cp(gb, 0xa0);
  if (!(F & FC)) { CYCT(0x4687, 0x4689); goto setAnim; } // jr nc
  CYC(0x4687, 0x4689);
  CYC(0x4689, 0x468a); SET_BC(BC + 1);

setAnim:
  CYC(0x468a, 0x468b); A = mem_rd(gb, BC);
  CYC(0x468b, 0x468e); partSetAnimation_hook(gb); return; // jp
}

void shadow_initialize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x469a, 0x469b); A = alu_inc8(gb, A);
  CYC(0x469b, 0x469c); mem_wr(gb, DE, A); // [state] = 1
  CYC(0x469c, 0x469e); A = 0x01; // Object.id
  CALL_C(0x469e, objectGetRelatedObject1Var_hook, 0x2160, 0x46a1);
  CYC(0x46a1, 0x46a3); E = 0xf0; // Part.var30
  CYC(0x46a3, 0x46a4); A = mem_rd(gb, HL);
  CYC(0x46a4, 0x46a5); mem_wr(gb, DE, A);
  CYC(0x46a5, 0x46a8); objectSetVisible83_hook(gb); return; // jp
}
