#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t detectionHelper_jump_table(GB *gb) {
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

static void detectionHelper_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

void partCode0e_hook(GB *gb);
void detectionHelper_spawnCollisionHelper_hook(GB *gb);

void partCode0e_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (!(F & FZ)) { CYCT(0x48b8, 0x48bb); partDelete_hook(gb); return; } // jp nz
  CYC(0x48b8, 0x48bb);
  CYC(0x48bb, 0x48bd); E = 0xc2; // Part.subid
  CYC(0x48bd, 0x48be); A = mem_rd(gb, DE);
  CYC(0x48be, 0x48c0); E = 0xc4; // Part.state
  {
    CYC(0x48c0, 0x48c1); push_effect(gb, 0x48c1);
    uint16_t target = detectionHelper_jump_table(gb);
    if (target == 0x48c9) goto subid0;
    if (target == 0x492d) goto subid1;
    goto subid2; // subid2 and subid3 alias the same address
  }

subid0:
  CYC(0x48c9, 0x48ca); A = mem_rd(gb, DE);
  CYC(0x48ca, 0x48cb); alu_or(gb, A);
  if (F & FZ) { CYCT(0x48cb, 0x48cd); goto subid0_state0; } // jr z
  CYC(0x48cb, 0x48cd);

  CYC(0x48cd, 0x48cf); A = 0x00; // Object.enabled
  CALL_C(0x48cf, objectGetRelatedObject1Var_hook, 0x2160, 0x48d2);
  CYC(0x48d2, 0x48d3); A = mem_rd(gb, HL);
  CYC(0x48d3, 0x48d4); alu_or(gb, A);
  if (F & FZ) { CYCT(0x48d4, 0x48d7); partDelete_hook(gb); return; } // jp z
  CYC(0x48d4, 0x48d7);
  CYC(0x48d7, 0x48d9); E = 0xc9; // Part.angle
  CYC(0x48d9, 0x48da); A = L;
  CYC(0x48da, 0x48dc); alu_or(gb, 0x09); // Object.angle
  CYC(0x48dc, 0x48dd); L = A;
  CYC(0x48dd, 0x48de); A = mem_rd(gb, HL);
  CYC(0x48de, 0x48df); mem_wr(gb, DE, A);
  CALL_C(0x48df, objectTakePosition_hook, 0x2274, 0x48e2);
  CALL_C(0x48e2, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x48e5);
  if (!(F & FZ)) { CYCT(0x48e5, 0x48e7); goto L_48f2; } // jr nz
  CYC(0x48e5, 0x48e7);
  CYC(0x48e7, 0x48e9); mem_wr(gb, HL, 0x0f); // [counter1]
  CYC(0x48e9, 0x48eb); E = 0xc9; // Part.angle
  CYC(0x48eb, 0x48ec); A = mem_rd(gb, DE);
  CYC(0x48ec, 0x48ed); B = A;
  CYC(0x48ed, 0x48ef); E = 0x01;
  CYC(0x48ef, 0x48f2); detectionHelper_spawnCollisionHelper_hook(gb);

L_48f2:
  CYC(0x48f2, 0x48f3); H = D;
  CYC(0x48f3, 0x48f5); L = 0xc7; // Part.counter2
  CYC(0x48f5, 0x48f6); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(0x48f6); return; } // ret nz
  CYC(0x48f6, 0x48f7);
  CYC(0x48f7, 0x48f9); mem_wr(gb, HL, 0x06); // [counter2]
  CYC(0x48f9, 0x48fb); L = 0xc3; // Part.var03
  CYC(0x48fb, 0x48fc); A = mem_rd(gb, HL);
  CYC(0x48fc, 0x48fd); A = alu_inc8(gb, A);
  CYC(0x48fd, 0x48ff); alu_and(gb, 0x03);
  CYC(0x48ff, 0x4900); mem_wr(gb, HL, A);
  CYC(0x4900, 0x4901); C = A;
  CYC(0x4901, 0x4903); L = 0xc9; // Part.angle
  CYC(0x4903, 0x4904); B = mem_rd(gb, HL);
  CYC(0x4904, 0x4906); E = 0x02;
  CYC(0x4906, 0x4909); detectionHelper_spawnCollisionHelper_hook(gb);
  CYC(0x4909, 0x490b); E = 0x03;
  detectionHelper_spawnCollisionHelper_hook(gb); return; // falls through

subid0_state0:
  CYC(0x4924, 0x4925); H = D;
  CYC(0x4925, 0x4926); L = E;
  CYC(0x4926, 0x4927); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [subid]
  CYC(0x4927, 0x4929); L = 0xc6; // Part.counter1
  CYC(0x4929, 0x492a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x492a, 0x492b); L = alu_inc8(gb, L);
  CYC(0x492b, 0x492c); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [counter2]
  RET(0x492c); return; // ret

subid1:
  CYC(0x492d, 0x492e); A = mem_rd(gb, DE);
  CYC(0x492e, 0x492f); alu_or(gb, A);
  if (F & FZ) { CYCT(0x492f, 0x4931); goto subid1_state0; } // jr z
  CYC(0x492f, 0x4931);

subid1_state1:
  CALL_C(0x4931, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x4934);
  if (F & FC) { CYCT(0x4934, 0x4936); goto sawLink; } // jr c
  CYC(0x4934, 0x4936);
  CALL_C(0x4936, objectApplyComponentSpeed_hook, 0x2008, 0x4939);
  CALL_C(0x4939, objectCheckSimpleCollision_hook, 0x1487, 0x493c);
  if (F & FZ) { RET_TAKEN(0x493c); return; } // ret z
  CYC(0x493c, 0x493d);
  CYC(0x493d, 0x493f); goto delete; // jr

sawLink:
  CYC(0x493f, 0x4941); A = 0x3b; // Object.var3b
  CALL_C(0x4941, objectGetRelatedObject1Var_hook, 0x2160, 0x4944);
  CYC(0x4944, 0x4946); mem_wr(gb, HL, 0xff);

delete:
  CYC(0x4946, 0x4949); partDelete_hook(gb); return; // jp

subid1_state0:
  CYC(0x4949, 0x494a); A = alu_inc8(gb, A);
  CYC(0x494a, 0x494b); mem_wr(gb, DE, A);
  CYC(0x494b, 0x494d); E = 0xc9; // Part.angle
  CYC(0x494d, 0x494e); A = mem_rd(gb, DE);
  CYC(0x494e, 0x4950); alu_add(gb, 0x04);
  CYC(0x4950, 0x4952); alu_and(gb, 0x08);
  CYC(0x4952, 0x4953); alu_rrca(gb);
  CYC(0x4953, 0x4954); alu_rrca(gb);
  CYC(0x4954, 0x4957); SET_HL(0x4962); // @collisionRadii
  CYC(0x4957, 0x4958); detectionHelper_addAToHl_from_rst(gb, 0x4958);
  CYC(0x4958, 0x495a); E = 0xe6; // Part.collisionRadiusY
  CYC(0x495a, 0x495b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x495b, 0x495c); mem_wr(gb, DE, A);
  CYC(0x495c, 0x495d); E = alu_inc8(gb, E);
  CYC(0x495d, 0x495e); A = mem_rd(gb, HL);
  CYC(0x495e, 0x495f); mem_wr(gb, DE, A);
  CYC(0x495f, 0x4962); goto initSpeed; // jp

subid2:
  CYC(0x4966, 0x4967); A = mem_rd(gb, DE);
  CYC(0x4967, 0x4968); alu_or(gb, A);
  if (F & FZ) { CYCT(0x4968, 0x496a); goto subid2_state0; } // jr z
  CYC(0x4968, 0x496a);

  CALL_C(0x496a, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x496d);
  if (!(F & FZ)) { CYCT(0x496d, 0x496f); goto subid1_state1; } // jr nz
  CYC(0x496d, 0x496f);
  CYC(0x496f, 0x4971); goto delete; // jr

subid2_state0:
  CYC(0x4971, 0x4972); H = D;
  CYC(0x4972, 0x4973); L = E;
  CYC(0x4973, 0x4974); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state]
  CYC(0x4974, 0x4976); L = 0xc6; // Part.counter1
  CYC(0x4976, 0x4978); mem_wr(gb, HL, 0x04);
  CYC(0x4978, 0x497a); L = 0xc3; // Part.var03
  CYC(0x497a, 0x497b); A = mem_rd(gb, HL);
  CYC(0x497b, 0x497c); A = alu_inc8(gb, A);
  CYC(0x497c, 0x497d); alu_add(gb, A);
  CYC(0x497d, 0x497e); L = alu_dec8(gb, L);
  CYC(0x497e, 0x4980); alu_bit(gb, 0, mem_rd(gb, HL)); // [subid]
  if (!(F & FZ)) { CYCT(0x4980, 0x4982); goto L_4984; } // jr nz
  CYC(0x4980, 0x4982);
  CYC(0x4982, 0x4983); alu_cpl(gb);
  CYC(0x4983, 0x4984); A = alu_inc8(gb, A);

L_4984:
  CYC(0x4984, 0x4986); L = 0xc9; // Part.angle
  CYC(0x4986, 0x4987); alu_add(gb, mem_rd(gb, HL));
  CYC(0x4987, 0x4989); alu_and(gb, 0x1f);
  CYC(0x4989, 0x498a); mem_wr(gb, HL, A);

initSpeed:
  CYC(0x498a, 0x498b); H = D;
  CYC(0x498b, 0x498d); L = 0xc9; // Part.angle
  CYC(0x498d, 0x498e); C = mem_rd(gb, HL);
  CYC(0x498e, 0x4990); B = 0x64; // SPEED_280
  CYC(0x4990, 0x4992); A = 0x04;
  CYC(0x4992, 0x4995); objectSetComponentSpeedByScaledVelocity_hook(gb); return; // jp
}

void detectionHelper_spawnCollisionHelper_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x490b, getFreePartSlot_hook, 0x3e8e, 0x490e);
  if (!(F & FZ)) { CYCT(0x490e, 0x490f); return; } // ret nz
  CYC(0x490e, 0x490f);
  CYC(0x490f, 0x4911); mem_wr(gb, HL, 0x0e); // PART_DETECTION_HELPER
  CYC(0x4911, 0x4912); L = alu_inc8(gb, L);
  CYC(0x4912, 0x4913); mem_wr(gb, HL, E);
  CYC(0x4913, 0x4914); L = alu_inc8(gb, L);
  CYC(0x4914, 0x4915); mem_wr(gb, HL, C);
  CALL_C(0x4915, objectCopyPosition_hook, 0x2242, 0x4918);
  CYC(0x4918, 0x491a); L = 0xc9; // Part.angle
  CYC(0x491a, 0x491b); mem_wr(gb, HL, B);
  CYC(0x491b, 0x491d); L = 0xd6; // Part.relatedObj1
  CYC(0x491d, 0x491e); E = L;
  CYC(0x491e, 0x491f); A = mem_rd(gb, DE);
  CYC(0x491f, 0x4920); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4920, 0x4921); E = alu_inc8(gb, E);
  CYC(0x4921, 0x4922); A = mem_rd(gb, DE);
  CYC(0x4922, 0x4923); mem_wr(gb, HL, A);
  CYC(0x4923, 0x4924); return; // ret
}
