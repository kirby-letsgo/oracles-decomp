#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/items/magnetBall.s.
// In common folder because Ages has a stub

static uint16_t magnetBall_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

static void magnetBall_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void magnetBall_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// itemCode29@preventLinkFromPassing
static void magnetBall_preventLinkFromPassing(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+952, b_+955); A = mem_rd(gb, wLinkInAir);
  CYC(b_+955, b_+956); alu_rlca(gb);
  if (F & FC) { RET_TAKEN(b_+956); return; }
  CYC(b_+956, b_+957);
  CYC(b_+957, b_+960); A = mem_rd(gb, w1Link_state);
  CYC(b_+960, b_+962); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+962); return; }
  CYC(b_+962, b_+963);
  CALL_C(b_+963, s_objectCheckCollidedWithLink_ignoreZ, SYM(objectCheckCollidedWithLink_ignoreZ), b_+966);
  if (!(F & FC)) { RET_TAKEN(b_+966); return; }
  CYC(b_+966, b_+967);
  CYC(b_+967, b_+970); A = mem_rd(gb, w1Link_yh);
  CYC(b_+970, b_+971); B = A;
  CYC(b_+971, b_+974); A = mem_rd(gb, w1Link_xh);
  CYC(b_+974, b_+975); C = A;
  CALL_C(b_+975, s_objectCheckContainsPoint, SYM(objectCheckContainsPoint), b_+978);
  if (F & FC) { CYCT(b_+978, b_+980); goto L_5a83; }
  CYC(b_+978, b_+980);
  CALL_C(b_+980, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+983);
  CYC(b_+983, b_+984); C = A;
  CYC(b_+984, b_+986); B = 0x78;
  CYC(b_+986, b_+989);
  TAIL(updateLinkPositionGivenVelocity);
L_5a83:
  CALL_C(b_+989, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+992);
  CYC(b_+992, b_+993); C = A;
  CYC(b_+993, b_+995); B = 0x14;
  CYC(b_+995, b_+998);
  TAIL(updateLinkPositionGivenVelocity);
}

// itemCode29@savePositionInStaticObjects
static void magnetBall_savePositionInStaticObjects(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+898, s_objectCheckIsOnHazard, SYM(objectCheckIsOnHazard), b_+901);
  if (F & FC) { RET_TAKEN(b_+901); return; }
  CYC(b_+901, b_+902);
  CYC(b_+902, b_+904); E = 0x0b;
  CYC(b_+904, b_+905); A = mem_rd(gb, DE);
  CYC(b_+905, b_+906); B = A;
  CYC(b_+906, b_+908); E = 0x0d;
  CYC(b_+908, b_+909); A = mem_rd(gb, DE);
  CYC(b_+909, b_+910); C = A;
  CYC(b_+910, b_+912); E = 0x16;
  CYC(b_+912, b_+913); A = mem_rd(gb, DE);
  CYC(b_+913, b_+914); L = A;
  CYC(b_+914, b_+915); E = alu_inc8(gb, E);
  CYC(b_+915, b_+916); A = mem_rd(gb, DE);
  CYC(b_+916, b_+917); H = A;
  CYC(b_+917, b_+918); push_effect(gb, BC);
  CYC(b_+918, b_+921); SET_BC(0x0004);
  CYC(b_+921, b_+922); alu_add_hl(gb, BC);
  CYC(b_+922, b_+923); SET_BC(pop_effect(gb));
  CYC(b_+923, b_+924); A = B;
  CYC(b_+924, b_+926); alu_cp(gb, 0x18);
  if (!(F & FC)) { CYCT(b_+926, b_+928); goto L_5a48; }
  CYC(b_+926, b_+928);
  CYC(b_+928, b_+930); A = 0x18;
L_5a48:
  CYC(b_+930, b_+932); alu_cp(gb, 0x99);
  if (F & FC) { CYCT(b_+932, b_+934); goto L_5a4e; }
  CYC(b_+932, b_+934);
  CYC(b_+934, b_+936); A = 0x98;
L_5a4e:
  CYC(b_+936, b_+937); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+937, b_+938); A = C;
  CYC(b_+938, b_+940); alu_cp(gb, 0x18);
  if (!(F & FC)) { CYCT(b_+940, b_+942); goto L_5a56; }
  CYC(b_+940, b_+942);
  CYC(b_+942, b_+944); A = 0x18;
L_5a56:
  CYC(b_+944, b_+946); alu_cp(gb, 0xd9);
  if (F & FC) { CYCT(b_+946, b_+948); goto L_5a5c; }
  CYC(b_+946, b_+948);
  CYC(b_+948, b_+950); A = 0xd8;
L_5a5c:
  CYC(b_+950, b_+951); mem_wr(gb, HL, A);
  RET(b_+951); return;
}

// itemCode29@saveInitialPositionInStaticObjects
static void magnetBall_saveInitialPositionInStaticObjects(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+889, b_+891); E = 0x30;
  CYC(b_+891, b_+892); A = mem_rd(gb, DE);
  CYC(b_+892, b_+893); B = A;
  CYC(b_+893, b_+894); E = alu_inc8(gb, E);
  CYC(b_+894, b_+895); A = mem_rd(gb, DE);
  CYC(b_+895, b_+896); C = A;
  CYC(b_+896, b_+898);
  goto L_5a34;

L_5a34:
  CYC(b_+910, b_+912); E = 0x16;
  CYC(b_+912, b_+913); A = mem_rd(gb, DE);
  CYC(b_+913, b_+914); L = A;
  CYC(b_+914, b_+915); E = alu_inc8(gb, E);
  CYC(b_+915, b_+916); A = mem_rd(gb, DE);
  CYC(b_+916, b_+917); H = A;
  CYC(b_+917, b_+918); push_effect(gb, BC);
  CYC(b_+918, b_+921); SET_BC(0x0004);
  CYC(b_+921, b_+922); alu_add_hl(gb, BC);
  CYC(b_+922, b_+923); SET_BC(pop_effect(gb));
  CYC(b_+923, b_+924); A = B;
  CYC(b_+924, b_+926); alu_cp(gb, 0x18);
  if (!(F & FC)) { CYCT(b_+926, b_+928); goto L_5a48; }
  CYC(b_+926, b_+928);
  CYC(b_+928, b_+930); A = 0x18;
L_5a48:
  CYC(b_+930, b_+932); alu_cp(gb, 0x99);
  if (F & FC) { CYCT(b_+932, b_+934); goto L_5a4e; }
  CYC(b_+932, b_+934);
  CYC(b_+934, b_+936); A = 0x98;
L_5a4e:
  CYC(b_+936, b_+937); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+937, b_+938); A = C;
  CYC(b_+938, b_+940); alu_cp(gb, 0x18);
  if (!(F & FC)) { CYCT(b_+940, b_+942); goto L_5a56; }
  CYC(b_+940, b_+942);
  CYC(b_+942, b_+944); A = 0x18;
L_5a56:
  CYC(b_+944, b_+946); alu_cp(gb, 0xd9);
  if (F & FC) { CYCT(b_+946, b_+948); goto L_5a5c; }
  CYC(b_+946, b_+948);
  CYC(b_+948, b_+950); A = 0xd8;
L_5a5c:
  CYC(b_+950, b_+951); mem_wr(gb, HL, A);
  RET(b_+951); return;
}

// itemCode29@blockWallFlameShooter
static void magnetBall_blockWallFlameShooter(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+879, b_+880); push_effect(gb, HL);
  CYC(b_+880, b_+882); L = ENEMY_BASE + OBJ_ZH;
  CYC(b_+882, b_+884); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) CALL_C_CC(b_+884, s_preventObjectHFromPassingObjectD, SYM(preventObjectHFromPassingObjectD), b_+887);
  else CYC(b_+884, b_+887);
  CYC(b_+887, b_+888); SET_HL(pop_effect(gb));
  RET(b_+888); return;
}

// itemCode29@convertAngleTo0To3
static void magnetBall_convertAngleTo0To3(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+853, b_+855); E = 0x09;
  CYC(b_+855, b_+856); A = mem_rd(gb, DE);
  CYC(b_+856, b_+858); alu_add(gb, 0x04);
  CYC(b_+858, b_+859); alu_add(gb, A);
  CYC(b_+859, b_+861); A = alu_swap(gb, A);
  CYC(b_+861, b_+863); alu_and(gb, 0x03);
  RET(b_+863); return;
}

// itemCode29@retCIfTileAtRelativePositionHLIsTopOfCliff
static void magnetBall_retCIfTileAtRelativePositionHLIsTopOfCliff(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+749, b_+751); E = 0x0b;
  CYC(b_+751, b_+752); A = mem_rd(gb, DE);
  CYC(b_+752, b_+753); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+753, b_+754); SET_HL(HL + 1);
  CYC(b_+754, b_+755); B = A;
  CYC(b_+755, b_+757); E = 0x0d;
  CYC(b_+757, b_+758); A = mem_rd(gb, DE);
  CYC(b_+758, b_+759); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+759, b_+760); SET_HL(HL + 1);
  CYC(b_+760, b_+761); C = A;
  CYC(b_+761, b_+762); push_effect(gb, HL);
  CALL_C(b_+762, s_getTileAtPosition, SYM(getTileAtPosition), b_+765);
  CYC(b_+765, b_+766); SET_HL(pop_effect(gb));
  CYC(b_+766, b_+768); alu_sub(gb, 0xb0);
  CYC(b_+768, b_+770); alu_cp(gb, 0x04);
  RET(b_+770); return;
}

// itemCode29@checkBallShouldBeDroppedOffCliff
static void magnetBall_checkBallShouldBeDroppedOffCliff(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_L(b_+771, magnetBall_convertAngleTo0To3, b_+774);
  CYC(b_+774, b_+775); alu_add(gb, A);
  CYC(b_+775, b_+778); SET_HL(b_+601 /* @positionOf2TilesJustAheadOfBall */);
  CYC(b_+778, b_+779); magnetBall_add_double_index(gb, b_+779);
  CALL_L(b_+779, magnetBall_retCIfTileAtRelativePositionHLIsTopOfCliff, b_+782);
  if (!(F & FC)) { RET_TAKEN(b_+782); return; }
  CYC(b_+782, b_+783);
  CALL_L(b_+783, magnetBall_retCIfTileAtRelativePositionHLIsTopOfCliff, b_+786);
  if (!(F & FC)) { RET_TAKEN(b_+786); return; }
  CYC(b_+786, b_+787);
  CYC(b_+787, b_+789); alu_add(gb, 0x02);
  CYC(b_+789, b_+791); alu_and(gb, 0x03);
  CYC(b_+791, b_+793); A = alu_swap(gb, A);
  CYC(b_+793, b_+794); alu_rrca(gb);
  CYC(b_+794, b_+795); B = A;
  CYC(b_+795, b_+797); E = 0x09;
  CYC(b_+797, b_+798); A = mem_rd(gb, DE);
  CYC(b_+798, b_+799); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+799); return; }
  CYC(b_+799, b_+800);
  CYC(b_+800, b_+802); A = alu_sra(gb, A);
  CYC(b_+802, b_+805); SET_HL(b_+837 /* @cliffDropData */);
  CYC(b_+805, b_+806); magnetBall_add_a_to_hl(gb, b_+806);
  CYC(b_+806, b_+807); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+807, b_+809); E = 0x08;
  CYC(b_+809, b_+810); mem_wr(gb, DE, A);
  CYC(b_+810, b_+811); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+811, b_+813); E = 0x10;
  CYC(b_+813, b_+814); mem_wr(gb, DE, A);
  CYC(b_+814, b_+815); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+815, b_+817); E = 0x14;
  CYC(b_+817, b_+818); mem_wr(gb, DE, A);
  CYC(b_+818, b_+819); E = alu_inc8(gb, E);
  CYC(b_+819, b_+820); A = mem_rd(gb, HL);
  CYC(b_+820, b_+821); mem_wr(gb, DE, A);
  CYC(b_+821, b_+822); alu_xor(gb, A);
  CYC(b_+822, b_+823); H = D;
  CYC(b_+823, b_+825); L = 0x32;
  CYC(b_+825, b_+826); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+826, b_+827); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+827, b_+828); mem_wr(gb, HL, A);
  CYC(b_+828, b_+830); L = 0x06;
  CYC(b_+830, b_+832); mem_wr(gb, HL, 0x02);
  CYC(b_+832, b_+834); L = 0x04;
  CYC(b_+834, b_+836); mem_wr(gb, HL, 0x02);
  RET(b_+836); return;
}

// itemCode29@keepVar34LessThan40_decVar34IfNot0
static void magnetBall_keepVar34LessThan40_decVar34IfNot0(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto keepVar34LessThan40_decVar34IfNot0;
L_5982:
  CYC(b_+732, b_+733); H = D;
  CYC(b_+733, b_+734); A = mem_rd(gb, HL);
  CYC(b_+734, b_+736); alu_cp(gb, 0x40);
  if (F & FC) { CYCT(b_+736, b_+738); goto L_598a; }
  CYC(b_+736, b_+738);
  CYC(b_+738, b_+740); A = 0x40;
L_598a:
  CYC(b_+740, b_+741); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+741); return; }
  CYC(b_+741, b_+742);
  CYC(b_+742, b_+743); A = alu_dec8(gb, A);
  CYC(b_+743, b_+744); mem_wr(gb, HL, A);
  RET(b_+744); return;
keepVar34LessThan40_decVar34IfNot0:
  CYC(b_+745, b_+747); L = 0x34;
  CYC(b_+747, b_+749);
  goto L_5982;
}

// itemCode29@keepVar33LessThan40_decVar33IfNot0
static void magnetBall_keepVar33LessThan40_decVar33IfNot0(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+730, b_+732); L = 0x33;
  CYC(b_+732, b_+733); H = D;
  CYC(b_+733, b_+734); A = mem_rd(gb, HL);
  CYC(b_+734, b_+736); alu_cp(gb, 0x40);
  if (F & FC) { CYCT(b_+736, b_+738); goto L_598a; }
  CYC(b_+736, b_+738);
  CYC(b_+738, b_+740); A = 0x40;
L_598a:
  CYC(b_+740, b_+741); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+741); return; }
  CYC(b_+741, b_+742);
  CYC(b_+742, b_+743); A = alu_dec8(gb, A);
  CYC(b_+743, b_+744); mem_wr(gb, HL, A);
  RET(b_+744); return;
}

// itemCode29@incVar34IfNotff
static void magnetBall_incVar34IfNotff(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+723, b_+724); H = D;
  CYC(b_+724, b_+726); L = 0x34;
  CYC(b_+726, b_+727); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+727); return; }
  CYC(b_+727, b_+728);
  CYC(b_+728, b_+729); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+729); return;
}

// itemCode29@incVar33IfNotff
static void magnetBall_incVar33IfNotff(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+716, b_+717); H = D;
  CYC(b_+717, b_+719); L = 0x33;
  CYC(b_+719, b_+720); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+720); return; }
  CYC(b_+720, b_+721);
  CYC(b_+721, b_+722); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  RET(b_+722); return;
}

// itemCode29@applySpeedBasedOnVar34
static void magnetBall_applySpeedBasedOnVar34(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto applySpeedBasedOnVar34;
L_5950:
  CYC(b_+682, b_+683); A = mem_rd(gb, DE);
  CYC(b_+683, b_+685); alu_cp(gb, 0x40);
  CYC(b_+685, b_+687); B = 0x78;
  if (!(F & FC)) { CYCT(b_+687, b_+689); goto loadBIntoItemSpeed; }
  CYC(b_+687, b_+689);
  CYC(b_+689, b_+691); alu_and(gb, 0x38);
  CYC(b_+691, b_+693); A = alu_swap(gb, A);
  CYC(b_+693, b_+694); alu_rlca(gb);
  CYC(b_+694, b_+697); SET_HL(b_+708 /* @magnetBallSpeeds */);
  CYC(b_+697, b_+698); magnetBall_add_a_to_hl(gb, b_+698);
  CYC(b_+698, b_+699); B = mem_rd(gb, HL);
loadBIntoItemSpeed:
  CYC(b_+699, b_+700); A = B;
  CYC(b_+700, b_+702); E = 0x10;
  CYC(b_+702, b_+703); mem_wr(gb, DE, A);
  RET(b_+703); return;
applySpeedBasedOnVar34:
  CYC(b_+704, b_+706); E = 0x34;
  CYC(b_+706, b_+708);
  goto L_5950;
}

// itemCode29@loadBIntoItemSpeed
static void magnetBall_loadBIntoItemSpeed(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+699, b_+700); A = B;
  CYC(b_+700, b_+702); E = 0x10;
  CYC(b_+702, b_+703); mem_wr(gb, DE, A);
  RET(b_+703); return;
}

// itemCode29@applySpeedBasedOnVar33
static void magnetBall_applySpeedBasedOnVar33(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+680, b_+682); E = 0x33;
  CYC(b_+682, b_+683); A = mem_rd(gb, DE);
  CYC(b_+683, b_+685); alu_cp(gb, 0x40);
  CYC(b_+685, b_+687); B = 0x78;
  if (!(F & FC)) { CYCT(b_+687, b_+689); goto loadBIntoItemSpeed; }
  CYC(b_+687, b_+689);
  CYC(b_+689, b_+691); alu_and(gb, 0x38);
  CYC(b_+691, b_+693); A = alu_swap(gb, A);
  CYC(b_+693, b_+694); alu_rlca(gb);
  CYC(b_+694, b_+697); SET_HL(b_+708 /* @magnetBallSpeeds */);
  CYC(b_+697, b_+698); magnetBall_add_a_to_hl(gb, b_+698);
  CYC(b_+698, b_+699); B = mem_rd(gb, HL);
loadBIntoItemSpeed:
  CYC(b_+699, b_+700); A = B;
  CYC(b_+700, b_+702); E = 0x10;
  CYC(b_+702, b_+703); mem_wr(gb, DE, A);
  RET(b_+703); return;
}

// itemCode29@applySpeedIfNoCollision
static void magnetBall_applySpeedIfNoCollision(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+617, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+620);
  CYC(b_+620, b_+623); SET_HL(SYM(hazardCollisionTable));
  CALL_C(b_+623, s_lookupCollisionTable, SYM(lookupCollisionTable), b_+626);
  if (!(F & FC)) { RET_TAKEN(b_+626); return; }
  CYC(b_+626, b_+627);
  CALL_C(b_+627, s_objectGetPosition, SYM(objectGetPosition), b_+630);
  CYC(b_+630, b_+632); A = 0x05;
  CYC(b_+632, b_+633); alu_add(gb, B);
  CYC(b_+633, b_+634); B = A;
  CALL_C(b_+634, s_checkTileCollisionAt_allowHoles, SYM(checkTileCollisionAt_allowHoles), b_+637);
  if (!(F & FC)) { RET_TAKEN(b_+637); return; }
  CYC(b_+637, b_+638);
  CYC(b_+638, b_+640); B = 0x14;
  CALL_L(b_+640, magnetBall_loadBIntoItemSpeed, b_+643);
  CYC(b_+643, b_+645); E = 0x09;
  CYC(b_+645, b_+646); alu_xor(gb, A);
  CYC(b_+646, b_+647); mem_wr(gb, DE, A);
  CYC(b_+647, b_+650);
  TAIL(objectApplySpeed);
}

// itemCode29@checkRelativeHLTileCollision_allowHoles
static void magnetBall_checkRelativeHLTileCollision_allowHoles(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+583, b_+585); E = 0x0b;
  CYC(b_+585, b_+586); A = mem_rd(gb, DE);
  CYC(b_+586, b_+587); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+587, b_+588); SET_HL(HL + 1);
  CYC(b_+588, b_+589); B = A;
  CYC(b_+589, b_+591); E = 0x0d;
  CYC(b_+591, b_+592); A = mem_rd(gb, DE);
  CYC(b_+592, b_+593); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+593, b_+594); SET_HL(HL + 1);
  CYC(b_+594, b_+595); C = A;
  CYC(b_+595, b_+596); push_effect(gb, HL);
  CALL_C(b_+596, s_checkTileCollisionAt_allowHoles, SYM(checkTileCollisionAt_allowHoles), b_+599);
  CYC(b_+599, b_+600); SET_HL(pop_effect(gb));
  RET(b_+600); return;
}

// itemCode29@checkBallShouldBeDroppedOffCliffOrLeavingRoom
static void magnetBall_checkBallShouldBeDroppedOffCliffOrLeavingRoom(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+541, b_+543); A = alu_srl(gb, A);
  CYC(b_+543, b_+546); SET_HL(b_+601 /* @positionOf2TilesJustAheadOfBall */);
  CYC(b_+546, b_+547); magnetBall_add_a_to_hl(gb, b_+547);
  CALL_L(b_+547, magnetBall_checkRelativeHLTileCollision_allowHoles, b_+550);
  if (F & FC) { CYCT(b_+550, b_+552); goto L_58dd; }
  CYC(b_+550, b_+552);
  CALL_L(b_+552, magnetBall_checkRelativeHLTileCollision_allowHoles, b_+555);
  if (F & FC) { CYCT(b_+555, b_+557); goto L_58dd; }
  CYC(b_+555, b_+557);
  CYC(b_+557, b_+558); H = D;
  CYC(b_+558, b_+560); L = 0x24;
  CYC(b_+560, b_+562); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+562, s_objectApplySpeed, SYM(objectApplySpeed), b_+565);
  CYC(b_+565, b_+567);
  goto preventFromLeavingRoom;
L_58dd:
  CALL_L(b_+567, magnetBall_checkBallShouldBeDroppedOffCliff, b_+570);
  CYC(b_+570, b_+572); E = 0x09;
  CYC(b_+572, b_+573); A = mem_rd(gb, DE);
  CYC(b_+573, b_+575); alu_bit(gb, 3, A);
  CYC(b_+575, b_+577); E = 0x33;
  if (F & FZ) { CYCT(b_+577, b_+579); goto L_58ea; }
  CYC(b_+577, b_+579);
  CYC(b_+579, b_+580); E = alu_inc8(gb, E);
L_58ea:
  CYC(b_+580, b_+581); alu_xor(gb, A);
  CYC(b_+581, b_+582); mem_wr(gb, DE, A);
  RET(b_+582); return;

preventFromLeavingRoom:
  CYC(b_+650, b_+653); SET_BC(0xa8e8);
  CYC(b_+653, b_+655); E = 0x08;
  CYC(b_+655, b_+656); H = D;
  CYC(b_+656, b_+658); L = 0x0b;
  CYC(b_+658, b_+659); A = E;
  CYC(b_+659, b_+660); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+660, b_+662); goto L_593d; }
  CYC(b_+660, b_+662);
  CYC(b_+662, b_+663); mem_wr(gb, HL, A);
L_593d:
  CYC(b_+663, b_+664); A = B;
  CYC(b_+664, b_+665); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+665, b_+667); goto L_5942; }
  CYC(b_+665, b_+667);
  CYC(b_+667, b_+668); mem_wr(gb, HL, A);
L_5942:
  CYC(b_+668, b_+670); L = 0x0d;
  CYC(b_+670, b_+671); A = E;
  CYC(b_+671, b_+672); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+672, b_+674); goto L_5949; }
  CYC(b_+672, b_+674);
  CYC(b_+674, b_+675); mem_wr(gb, HL, A);
L_5949:
  CYC(b_+675, b_+676); A = C;
  CYC(b_+676, b_+677); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+677); return; }
  CYC(b_+677, b_+678);
  CYC(b_+678, b_+679); mem_wr(gb, HL, A);
  RET(b_+679); return;
}

// itemCode29@loadBIntoAngleIfLinkLeftOfBallElseLoadC
static void magnetBall_loadBIntoAngleIfLinkLeftOfBallElseLoadC(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  goto loadBIntoAngleIfLinkLeftOfBallElseLoadC;
L_58b1:
  CYC(b_+523, b_+524); H = D;
  CYC(b_+524, b_+525); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+525, b_+526); A = B;
  if (F & FC) { CYCT(b_+526, b_+528); goto L_58b7; }
  CYC(b_+526, b_+528);
  CYC(b_+528, b_+529); A = C;
L_58b7:
  CYC(b_+529, b_+531); L = 0x09;
  CYC(b_+531, b_+532); mem_wr(gb, HL, A);
  RET(b_+532); return;
loadBIntoAngleIfLinkLeftOfBallElseLoadC:
  CYC(b_+533, b_+535); A = mem_rd(gb, hFF8C);
  CYC(b_+535, b_+537); L = 0x0d;
  CYC(b_+537, b_+539); E = 0x34;
  CYC(b_+539, b_+541);
  goto L_58b1;
}

// itemCode29@loadBIntoAngleIfLinkAboveBallElseLoadC
static void magnetBall_loadBIntoAngleIfLinkAboveBallElseLoadC(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+517, b_+519); A = mem_rd(gb, hFF8D);
  CYC(b_+519, b_+521); L = 0x0b;
  CYC(b_+521, b_+523); E = 0x33;
  CYC(b_+523, b_+524); H = D;
  CYC(b_+524, b_+525); alu_cp(gb, mem_rd(gb, HL));
  CYC(b_+525, b_+526); A = B;
  if (F & FC) { CYCT(b_+526, b_+528); goto L_58b7; }
  CYC(b_+526, b_+528);
  CYC(b_+528, b_+529); A = C;
L_58b7:
  CYC(b_+529, b_+531); L = 0x09;
  CYC(b_+531, b_+532); mem_wr(gb, HL, A);
  RET(b_+532); return;
}

// itemCode29@horizontalAttractBall
static void magnetBall_horizontalAttractBall(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+487, b_+489); B = 0x18;
  CYC(b_+489, b_+491); C = 0x08;
  CALL_L(b_+491, magnetBall_loadBIntoAngleIfLinkLeftOfBallElseLoadC, b_+494);
  if (F & FZ) { RET_TAKEN(b_+494); return; }
  CYC(b_+494, b_+495);
  CYC(b_+495, b_+497);
  goto checkBallShouldBeDroppedOffCliffOrLeavingRoom;

checkBallShouldBeDroppedOffCliffOrLeavingRoom:
  CYC(b_+541, b_+543); A = alu_srl(gb, A);
  CYC(b_+543, b_+546); SET_HL(b_+601 /* @positionOf2TilesJustAheadOfBall */);
  CYC(b_+546, b_+547); magnetBall_add_a_to_hl(gb, b_+547);
  CALL_L(b_+547, magnetBall_checkRelativeHLTileCollision_allowHoles, b_+550);
  if (F & FC) { CYCT(b_+550, b_+552); goto L_58dd; }
  CYC(b_+550, b_+552);
  CALL_L(b_+552, magnetBall_checkRelativeHLTileCollision_allowHoles, b_+555);
  if (F & FC) { CYCT(b_+555, b_+557); goto L_58dd; }
  CYC(b_+555, b_+557);
  CYC(b_+557, b_+558); H = D;
  CYC(b_+558, b_+560); L = 0x24;
  CYC(b_+560, b_+562); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+562, s_objectApplySpeed, SYM(objectApplySpeed), b_+565);
  CYC(b_+565, b_+567);
  goto preventFromLeavingRoom;
L_58dd:
  CALL_L(b_+567, magnetBall_checkBallShouldBeDroppedOffCliff, b_+570);
  CYC(b_+570, b_+572); E = 0x09;
  CYC(b_+572, b_+573); A = mem_rd(gb, DE);
  CYC(b_+573, b_+575); alu_bit(gb, 3, A);
  CYC(b_+575, b_+577); E = 0x33;
  if (F & FZ) { CYCT(b_+577, b_+579); goto L_58ea; }
  CYC(b_+577, b_+579);
  CYC(b_+579, b_+580); E = alu_inc8(gb, E);
L_58ea:
  CYC(b_+580, b_+581); alu_xor(gb, A);
  CYC(b_+581, b_+582); mem_wr(gb, DE, A);
  RET(b_+582); return;

preventFromLeavingRoom:
  CYC(b_+650, b_+653); SET_BC(0xa8e8);
  CYC(b_+653, b_+655); E = 0x08;
  CYC(b_+655, b_+656); H = D;
  CYC(b_+656, b_+658); L = 0x0b;
  CYC(b_+658, b_+659); A = E;
  CYC(b_+659, b_+660); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+660, b_+662); goto L_593d; }
  CYC(b_+660, b_+662);
  CYC(b_+662, b_+663); mem_wr(gb, HL, A);
L_593d:
  CYC(b_+663, b_+664); A = B;
  CYC(b_+664, b_+665); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+665, b_+667); goto L_5942; }
  CYC(b_+665, b_+667);
  CYC(b_+667, b_+668); mem_wr(gb, HL, A);
L_5942:
  CYC(b_+668, b_+670); L = 0x0d;
  CYC(b_+670, b_+671); A = E;
  CYC(b_+671, b_+672); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+672, b_+674); goto L_5949; }
  CYC(b_+672, b_+674);
  CYC(b_+674, b_+675); mem_wr(gb, HL, A);
L_5949:
  CYC(b_+675, b_+676); A = C;
  CYC(b_+676, b_+677); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+677); return; }
  CYC(b_+677, b_+678);
  CYC(b_+678, b_+679); mem_wr(gb, HL, A);
  RET(b_+679); return;
}

// itemCode29@verticalAttractBall
static void magnetBall_verticalAttractBall(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+477, b_+479); B = 0x00;
  CYC(b_+479, b_+481); C = 0x10;
  CALL_L(b_+481, magnetBall_loadBIntoAngleIfLinkAboveBallElseLoadC, b_+484);
  if (F & FZ) { RET_TAKEN(b_+484); return; }
  CYC(b_+484, b_+485);
  CYC(b_+485, b_+487);
  goto checkBallShouldBeDroppedOffCliffOrLeavingRoom;

checkBallShouldBeDroppedOffCliffOrLeavingRoom:
  CYC(b_+541, b_+543); A = alu_srl(gb, A);
  CYC(b_+543, b_+546); SET_HL(b_+601 /* @positionOf2TilesJustAheadOfBall */);
  CYC(b_+546, b_+547); magnetBall_add_a_to_hl(gb, b_+547);
  CALL_L(b_+547, magnetBall_checkRelativeHLTileCollision_allowHoles, b_+550);
  if (F & FC) { CYCT(b_+550, b_+552); goto L_58dd; }
  CYC(b_+550, b_+552);
  CALL_L(b_+552, magnetBall_checkRelativeHLTileCollision_allowHoles, b_+555);
  if (F & FC) { CYCT(b_+555, b_+557); goto L_58dd; }
  CYC(b_+555, b_+557);
  CYC(b_+557, b_+558); H = D;
  CYC(b_+558, b_+560); L = 0x24;
  CYC(b_+560, b_+562); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+562, s_objectApplySpeed, SYM(objectApplySpeed), b_+565);
  CYC(b_+565, b_+567);
  goto preventFromLeavingRoom;
L_58dd:
  CALL_L(b_+567, magnetBall_checkBallShouldBeDroppedOffCliff, b_+570);
  CYC(b_+570, b_+572); E = 0x09;
  CYC(b_+572, b_+573); A = mem_rd(gb, DE);
  CYC(b_+573, b_+575); alu_bit(gb, 3, A);
  CYC(b_+575, b_+577); E = 0x33;
  if (F & FZ) { CYCT(b_+577, b_+579); goto L_58ea; }
  CYC(b_+577, b_+579);
  CYC(b_+579, b_+580); E = alu_inc8(gb, E);
L_58ea:
  CYC(b_+580, b_+581); alu_xor(gb, A);
  CYC(b_+581, b_+582); mem_wr(gb, DE, A);
  RET(b_+582); return;

preventFromLeavingRoom:
  CYC(b_+650, b_+653); SET_BC(0xa8e8);
  CYC(b_+653, b_+655); E = 0x08;
  CYC(b_+655, b_+656); H = D;
  CYC(b_+656, b_+658); L = 0x0b;
  CYC(b_+658, b_+659); A = E;
  CYC(b_+659, b_+660); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+660, b_+662); goto L_593d; }
  CYC(b_+660, b_+662);
  CYC(b_+662, b_+663); mem_wr(gb, HL, A);
L_593d:
  CYC(b_+663, b_+664); A = B;
  CYC(b_+664, b_+665); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+665, b_+667); goto L_5942; }
  CYC(b_+665, b_+667);
  CYC(b_+667, b_+668); mem_wr(gb, HL, A);
L_5942:
  CYC(b_+668, b_+670); L = 0x0d;
  CYC(b_+670, b_+671); A = E;
  CYC(b_+671, b_+672); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+672, b_+674); goto L_5949; }
  CYC(b_+672, b_+674);
  CYC(b_+674, b_+675); mem_wr(gb, HL, A);
L_5949:
  CYC(b_+675, b_+676); A = C;
  CYC(b_+676, b_+677); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+677); return; }
  CYC(b_+677, b_+678);
  CYC(b_+678, b_+679); mem_wr(gb, HL, A);
  RET(b_+679); return;
}

// itemCode29@checkLinkInBallsPosition
static void magnetBall_checkLinkInBallsPosition(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+412, b_+414); A = mem_rd(gb, hFF8D);
  CYC(b_+414, b_+415); B = A;
  CYC(b_+415, b_+417); A = mem_rd(gb, hFF8C);
  CYC(b_+417, b_+418); C = A;
  CYC(b_+418, b_+421);
  TAIL(objectCheckContainsPoint);
}

// itemCode29@setAngleAndCheckBallShouldBeDroppedOffCliffOrLeavingRoom
static void magnetBall_setAngleAndCheckBallShouldBeDroppedOffCliffOrLeavingRoom(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+406, b_+408); E = 0x09;
  CYC(b_+408, b_+409); mem_wr(gb, DE, A);
  CYC(b_+409, b_+412);
  goto checkBallShouldBeDroppedOffCliffOrLeavingRoom;

checkBallShouldBeDroppedOffCliffOrLeavingRoom:
  CYC(b_+541, b_+543); A = alu_srl(gb, A);
  CYC(b_+543, b_+546); SET_HL(b_+601 /* @positionOf2TilesJustAheadOfBall */);
  CYC(b_+546, b_+547); magnetBall_add_a_to_hl(gb, b_+547);
  CALL_L(b_+547, magnetBall_checkRelativeHLTileCollision_allowHoles, b_+550);
  if (F & FC) { CYCT(b_+550, b_+552); goto L_58dd; }
  CYC(b_+550, b_+552);
  CALL_L(b_+552, magnetBall_checkRelativeHLTileCollision_allowHoles, b_+555);
  if (F & FC) { CYCT(b_+555, b_+557); goto L_58dd; }
  CYC(b_+555, b_+557);
  CYC(b_+557, b_+558); H = D;
  CYC(b_+558, b_+560); L = 0x24;
  CYC(b_+560, b_+562); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+562, s_objectApplySpeed, SYM(objectApplySpeed), b_+565);
  CYC(b_+565, b_+567);
  goto preventFromLeavingRoom;
L_58dd:
  CALL_L(b_+567, magnetBall_checkBallShouldBeDroppedOffCliff, b_+570);
  CYC(b_+570, b_+572); E = 0x09;
  CYC(b_+572, b_+573); A = mem_rd(gb, DE);
  CYC(b_+573, b_+575); alu_bit(gb, 3, A);
  CYC(b_+575, b_+577); E = 0x33;
  if (F & FZ) { CYCT(b_+577, b_+579); goto L_58ea; }
  CYC(b_+577, b_+579);
  CYC(b_+579, b_+580); E = alu_inc8(gb, E);
L_58ea:
  CYC(b_+580, b_+581); alu_xor(gb, A);
  CYC(b_+581, b_+582); mem_wr(gb, DE, A);
  RET(b_+582); return;

preventFromLeavingRoom:
  CYC(b_+650, b_+653); SET_BC(0xa8e8);
  CYC(b_+653, b_+655); E = 0x08;
  CYC(b_+655, b_+656); H = D;
  CYC(b_+656, b_+658); L = 0x0b;
  CYC(b_+658, b_+659); A = E;
  CYC(b_+659, b_+660); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+660, b_+662); goto L_593d; }
  CYC(b_+660, b_+662);
  CYC(b_+662, b_+663); mem_wr(gb, HL, A);
L_593d:
  CYC(b_+663, b_+664); A = B;
  CYC(b_+664, b_+665); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+665, b_+667); goto L_5942; }
  CYC(b_+665, b_+667);
  CYC(b_+667, b_+668); mem_wr(gb, HL, A);
L_5942:
  CYC(b_+668, b_+670); L = 0x0d;
  CYC(b_+670, b_+671); A = E;
  CYC(b_+671, b_+672); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+672, b_+674); goto L_5949; }
  CYC(b_+672, b_+674);
  CYC(b_+674, b_+675); mem_wr(gb, HL, A);
L_5949:
  CYC(b_+675, b_+676); A = C;
  CYC(b_+676, b_+677); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+677); return; }
  CYC(b_+677, b_+678);
  CYC(b_+678, b_+679); mem_wr(gb, HL, A);
  RET(b_+679); return;
}

// itemCode29@mainStateBody
static void magnetBall_mainStateBody(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+73, b_+74); H = D;
  CYC(b_+74, b_+76); L = 0x03;
  CYC(b_+76, b_+77); A = mem_rd(gb, HL);
  CYC(b_+77, b_+78); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+78, b_+80); goto L_56fa; }
  CYC(b_+78, b_+80);
  CYC(b_+80, b_+82); L = 0x24;
  CYC(b_+82, b_+84); mem_wr(gb, HL, mem_rd(gb, HL) & 0x7f);
L_56fa:
  CALL_L(b_+84, magnetBall_savePositionInStaticObjects, b_+87);
  CALL_L(b_+87, magnetBall_preventLinkFromPassing, b_+90);
  CYC(b_+90, b_+93); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+93, b_+94); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+94, b_+97); goto slowDownCheckCliffHolesAndRoomBoundary; }
  CYC(b_+94, b_+97);
  CYC(b_+97, b_+99); B = 0x0c;
  CALL_C(b_+99, s_objectCheckCenteredWithLink, SYM(objectCheckCenteredWithLink), b_+102);
  if (!(F & FC)) { CYCT(b_+102, b_+105); goto slowDownCheckCliffHolesAndRoomBoundary; }
  CYC(b_+102, b_+105);
  CALL_C(b_+105, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+108);
  CYC(b_+108, b_+110); alu_add(gb, 0x04);
  CYC(b_+110, b_+111); alu_add(gb, A);
  CYC(b_+111, b_+113); A = alu_swap(gb, A);
  CYC(b_+113, b_+115); alu_and(gb, 0x03);
  CYC(b_+115, b_+117); alu_xor(gb, 0x02);
  CYC(b_+117, b_+118); B = A;
  CYC(b_+118, b_+121); A = mem_rd(gb, w1Link_direction);
  CYC(b_+121, b_+122); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+122, b_+125); goto slowDownCheckCliffHolesAndRoomBoundary; }
  CYC(b_+122, b_+125);
  CYC(b_+125, b_+127); E = 0x10;
  CYC(b_+127, b_+129); A = 0x28;
  CYC(b_+129, b_+130); mem_wr(gb, DE, A);
  CYC(b_+130, b_+132); E = 0x32;
  CYC(b_+132, b_+133); A = mem_rd(gb, DE);
  CYC(b_+133, b_+134); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+134, b_+136); goto L_5740; }
  CYC(b_+134, b_+136);
  CYC(b_+136, b_+137); E = alu_inc8(gb, E);
  CYC(b_+137, b_+138); A = mem_rd(gb, DE);
  CYC(b_+138, b_+140); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+140, b_+143); goto slowDownCheckCliffHolesAndRoomBoundary; }
  CYC(b_+140, b_+143);
  CYC(b_+143, b_+144); E = alu_inc8(gb, E);
  CYC(b_+144, b_+145); A = mem_rd(gb, DE);
  CYC(b_+145, b_+147); alu_cp(gb, 0x10);
  if (!(F & FC)) { CYCT(b_+147, b_+150); goto slowDownCheckCliffHolesAndRoomBoundary; }
  CYC(b_+147, b_+150);
  CYC(b_+150, b_+152); E = 0x32;
  CYC(b_+152, b_+153); alu_xor(gb, A);
  CYC(b_+153, b_+154); mem_wr(gb, DE, A);
L_5740:
  CYC(b_+154, b_+157); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+157, b_+159); alu_bit(gb, 1, A);
  if (!(F & FZ)) { CYCT(b_+159, b_+161); goto repelBall; }
  CYC(b_+159, b_+161);
  CYC(b_+161, b_+164); A = mem_rd(gb, w1Link_direction);
  CYC(b_+164, b_+167); SET_HL(b_+469 /* @linksRelativePositionForFF8D_FF8CbasedOnLinkDirection */);
  CYC(b_+167, b_+168); magnetBall_add_double_index(gb, b_+168);
  CYC(b_+168, b_+171); A = mem_rd(gb, w1Link_yh);
  CYC(b_+171, b_+172); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+172, b_+174); mem_wr(gb, hFF8D, A);
  CYC(b_+174, b_+175); SET_HL(HL + 1);
  CYC(b_+175, b_+178); A = mem_rd(gb, w1Link_xh);
  CYC(b_+178, b_+179); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+179, b_+181); mem_wr(gb, hFF8C, A);
  CYC(b_+181, b_+182); push_effect(gb, BC);
  CALL_L(b_+182, magnetBall_checkLinkInBallsPosition, b_+185);
  CYC(b_+185, b_+186); SET_BC(pop_effect(gb));
  if (F & FC) { CYCT(b_+186, b_+189); goto moveBallCheckCliffsAndRoomBoundary; }
  CYC(b_+186, b_+189);
  CYC(b_+189, b_+191); alu_bit(gb, 0, B);
  if (!(F & FZ)) { CYCT(b_+191, b_+193); goto L_5779; }
  CYC(b_+191, b_+193);
  CALL_L(b_+193, magnetBall_horizontalAttractBall, b_+196);
  CYC(b_+196, b_+198); E = 0x04;
  CYC(b_+198, b_+199); A = mem_rd(gb, DE);
  CYC(b_+199, b_+201); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+201); return; }
  CYC(b_+201, b_+202);
  CALL_L(b_+202, magnetBall_incVar33IfNotff, b_+205);
  CALL_L(b_+205, magnetBall_applySpeedBasedOnVar33, b_+208);
  CYC(b_+208, b_+211);
  goto verticalAttractBall;
L_5779:
  CALL_L(b_+211, magnetBall_verticalAttractBall, b_+214);
  CYC(b_+214, b_+216); E = 0x04;
  CYC(b_+216, b_+217); A = mem_rd(gb, DE);
  CYC(b_+217, b_+219); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+219); return; }
  CYC(b_+219, b_+220);
  CALL_L(b_+220, magnetBall_incVar34IfNotff, b_+223);
  CALL_L(b_+223, magnetBall_applySpeedBasedOnVar34, b_+226);
  CYC(b_+226, b_+229);
  goto horizontalAttractBall;
repelBall:
  CYC(b_+229, b_+232); A = mem_rd(gb, w1Link_yh);
  CYC(b_+232, b_+234); mem_wr(gb, hFF8D, A);
  CYC(b_+234, b_+237); A = mem_rd(gb, w1Link_xh);
  CYC(b_+237, b_+239); mem_wr(gb, hFF8C, A);
  CYC(b_+239, b_+241); alu_bit(gb, 0, B);
  if (!(F & FZ)) { CYCT(b_+241, b_+243); goto L_57ab; }
  CYC(b_+241, b_+243);
  CALL_L(b_+243, magnetBall_horizontalAttractBall, b_+246);
  CYC(b_+246, b_+248); E = 0x04;
  CYC(b_+248, b_+249); A = mem_rd(gb, DE);
  CYC(b_+249, b_+251); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+251); return; }
  CYC(b_+251, b_+252);
  CALL_L(b_+252, magnetBall_incVar33IfNotff, b_+255);
  CALL_L(b_+255, magnetBall_applySpeedBasedOnVar33, b_+258);
  CYC(b_+258, b_+261);
  goto verticalRepelBall;
L_57ab:
  CALL_L(b_+261, magnetBall_verticalAttractBall, b_+264);
  CYC(b_+264, b_+266); E = 0x04;
  CYC(b_+266, b_+267); A = mem_rd(gb, DE);
  CYC(b_+267, b_+269); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+269); return; }
  CYC(b_+269, b_+270);
  CALL_L(b_+270, magnetBall_incVar34IfNotff, b_+273);
  CALL_L(b_+273, magnetBall_applySpeedBasedOnVar34, b_+276);
  CYC(b_+276, b_+279);
  goto horizontalRepelBall;
slowDownCheckCliffHolesAndRoomBoundary:
  CYC(b_+279, b_+281); E = 0x33;
  CYC(b_+281, b_+282); A = mem_rd(gb, DE);
  CYC(b_+282, b_+283); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+283, b_+285); goto L_57d7; }
  CYC(b_+283, b_+285);
  CYC(b_+285, b_+287); E = 0x32;
  CYC(b_+287, b_+289); A = 0x01;
  CYC(b_+289, b_+290); mem_wr(gb, DE, A);
  CALL_L(b_+290, magnetBall_keepVar33LessThan40_decVar33IfNot0, b_+293);
  CALL_L(b_+293, magnetBall_keepVar33LessThan40_decVar33IfNot0, b_+296);
  CALL_L(b_+296, magnetBall_applySpeedBasedOnVar33, b_+299);
  CYC(b_+299, b_+301); E = 0x09;
  CYC(b_+301, b_+302); A = mem_rd(gb, DE);
  CALL_L(b_+302, magnetBall_checkBallShouldBeDroppedOffCliffOrLeavingRoom, b_+305);
L_57d7:
  CYC(b_+305, b_+307); E = 0x34;
  CYC(b_+307, b_+308); A = mem_rd(gb, DE);
  CYC(b_+308, b_+309); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+309, b_+311); goto L_57f1; }
  CYC(b_+309, b_+311);
  CYC(b_+311, b_+313); E = 0x32;
  CYC(b_+313, b_+315); A = 0x01;
  CYC(b_+315, b_+316); mem_wr(gb, DE, A);
  CALL_L(b_+316, magnetBall_keepVar34LessThan40_decVar34IfNot0, b_+319);
  CALL_L(b_+319, magnetBall_keepVar34LessThan40_decVar34IfNot0, b_+322);
  CALL_L(b_+322, magnetBall_applySpeedBasedOnVar34, b_+325);
  CYC(b_+325, b_+327); E = 0x09;
  CYC(b_+327, b_+328); A = mem_rd(gb, DE);
  CALL_L(b_+328, magnetBall_checkBallShouldBeDroppedOffCliffOrLeavingRoom, b_+331);
L_57f1:
  CALL_C(b_+331, s_objectCheckIsOnHazard, SYM(objectCheckIsOnHazard), b_+334);
  if (F & FC) { CYCT(b_+334, b_+337); goto ballDropped_positionResetOnRoomReentry; }
  CYC(b_+334, b_+337);
  RET(b_+337); return;
ballDropped_positionResetOnRoomReentry:
  CYC(b_+338, b_+340); mem_wr(gb, hFF8B, A);
  CALL_L(b_+340, magnetBall_saveInitialPositionInStaticObjects, b_+343);
  CYC(b_+343, b_+345); A = mem_rd(gb, hFF8B);
  CYC(b_+345, b_+346); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(b_+346, b_+349); TAIL(objectReplaceWithSplash); }
  CYC(b_+346, b_+349);
  CYC(b_+349, b_+352);
  TAIL(objectReplaceWithFallingDownHoleInteraction);
moveBallCheckCliffsAndRoomBoundary:
  CYC(b_+352, b_+353); alu_xor(gb, A);
  CYC(b_+353, b_+355); E = 0x33;
  CYC(b_+355, b_+356); mem_wr(gb, DE, A);
  CYC(b_+356, b_+358); E = 0x34;
  CYC(b_+358, b_+359); mem_wr(gb, DE, A);
  CYC(b_+359, b_+362); A = mem_rd(gb, wLinkAngle);
  CYC(b_+362, b_+364); alu_cp(gb, 0xff);
  if (F & FZ) { RET_TAKEN(b_+364); return; }
  CYC(b_+364, b_+365);
  CYC(b_+365, b_+368); A = mem_rd(gb, wGameKeysPressed);
  CYC(b_+368, b_+369); B = A;
  CYC(b_+369, b_+371); alu_bit(gb, 6, B);
  if (F & FZ) { CYCT(b_+371, b_+373); goto L_5822; }
  CYC(b_+371, b_+373);
  CYC(b_+373, b_+375); A = 0x00;
  CALL_L(b_+375, magnetBall_setAngleAndCheckBallShouldBeDroppedOffCliffOrLeavingRoom, b_+378);
  CYC(b_+378, b_+380);
  goto L_582b;
L_5822:
  CYC(b_+380, b_+382); alu_bit(gb, 7, B);
  if (F & FZ) { CYCT(b_+382, b_+384); goto L_582b; }
  CYC(b_+382, b_+384);
  CYC(b_+384, b_+386); A = 0x10;
  CALL_L(b_+386, magnetBall_setAngleAndCheckBallShouldBeDroppedOffCliffOrLeavingRoom, b_+389);
L_582b:
  CYC(b_+389, b_+392); A = mem_rd(gb, wGameKeysPressed);
  CYC(b_+392, b_+393); B = A;
  CYC(b_+393, b_+395); alu_bit(gb, 4, B);
  if (F & FZ) { CYCT(b_+395, b_+397); goto L_5837; }
  CYC(b_+395, b_+397);
  CYC(b_+397, b_+399); A = 0x08;
  CYC(b_+399, b_+401);
  goto setAngleAndCheckBallShouldBeDroppedOffCliffOrLeavingRoom;
L_5837:
  CYC(b_+401, b_+403); alu_bit(gb, 5, B);
  CYC(b_+403, b_+405); A = 0x18;
  if (F & FZ) { RET_TAKEN(b_+405); return; }
  CYC(b_+405, b_+406);
setAngleAndCheckBallShouldBeDroppedOffCliffOrLeavingRoom:
  CYC(b_+406, b_+408); E = 0x09;
  CYC(b_+408, b_+409); mem_wr(gb, DE, A);
  CYC(b_+409, b_+412);
  goto checkBallShouldBeDroppedOffCliffOrLeavingRoom;

verticalAttractBall:
  CYC(b_+477, b_+479); B = 0x00;
  CYC(b_+479, b_+481); C = 0x10;
  CALL_L(b_+481, magnetBall_loadBIntoAngleIfLinkAboveBallElseLoadC, b_+484);
  if (F & FZ) { RET_TAKEN(b_+484); return; }
  CYC(b_+484, b_+485);
  CYC(b_+485, b_+487);
  goto checkBallShouldBeDroppedOffCliffOrLeavingRoom;
horizontalAttractBall:
  CYC(b_+487, b_+489); B = 0x18;
  CYC(b_+489, b_+491); C = 0x08;
  CALL_L(b_+491, magnetBall_loadBIntoAngleIfLinkLeftOfBallElseLoadC, b_+494);
  if (F & FZ) { RET_TAKEN(b_+494); return; }
  CYC(b_+494, b_+495);
  CYC(b_+495, b_+497);
  goto checkBallShouldBeDroppedOffCliffOrLeavingRoom;
verticalRepelBall:
  CYC(b_+497, b_+499); B = 0x10;
  CYC(b_+499, b_+501); C = 0x00;
  CALL_L(b_+501, magnetBall_loadBIntoAngleIfLinkAboveBallElseLoadC, b_+504);
  if (F & FZ) { RET_TAKEN(b_+504); return; }
  CYC(b_+504, b_+505);
  CYC(b_+505, b_+507);
  goto checkBallShouldBeDroppedOffCliffOrLeavingRoom;
horizontalRepelBall:
  CYC(b_+507, b_+509); B = 0x08;
  CYC(b_+509, b_+511); C = 0x18;
  CALL_L(b_+511, magnetBall_loadBIntoAngleIfLinkLeftOfBallElseLoadC, b_+514);
  if (F & FZ) { RET_TAKEN(b_+514); return; }
  CYC(b_+514, b_+515);
  CYC(b_+515, b_+517);
  goto checkBallShouldBeDroppedOffCliffOrLeavingRoom;

checkBallShouldBeDroppedOffCliffOrLeavingRoom:
  CYC(b_+541, b_+543); A = alu_srl(gb, A);
  CYC(b_+543, b_+546); SET_HL(b_+601 /* @positionOf2TilesJustAheadOfBall */);
  CYC(b_+546, b_+547); magnetBall_add_a_to_hl(gb, b_+547);
  CALL_L(b_+547, magnetBall_checkRelativeHLTileCollision_allowHoles, b_+550);
  if (F & FC) { CYCT(b_+550, b_+552); goto L_58dd; }
  CYC(b_+550, b_+552);
  CALL_L(b_+552, magnetBall_checkRelativeHLTileCollision_allowHoles, b_+555);
  if (F & FC) { CYCT(b_+555, b_+557); goto L_58dd; }
  CYC(b_+555, b_+557);
  CYC(b_+557, b_+558); H = D;
  CYC(b_+558, b_+560); L = 0x24;
  CYC(b_+560, b_+562); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CALL_C(b_+562, s_objectApplySpeed, SYM(objectApplySpeed), b_+565);
  CYC(b_+565, b_+567);
  goto preventFromLeavingRoom;
L_58dd:
  CALL_L(b_+567, magnetBall_checkBallShouldBeDroppedOffCliff, b_+570);
  CYC(b_+570, b_+572); E = 0x09;
  CYC(b_+572, b_+573); A = mem_rd(gb, DE);
  CYC(b_+573, b_+575); alu_bit(gb, 3, A);
  CYC(b_+575, b_+577); E = 0x33;
  if (F & FZ) { CYCT(b_+577, b_+579); goto L_58ea; }
  CYC(b_+577, b_+579);
  CYC(b_+579, b_+580); E = alu_inc8(gb, E);
L_58ea:
  CYC(b_+580, b_+581); alu_xor(gb, A);
  CYC(b_+581, b_+582); mem_wr(gb, DE, A);
  RET(b_+582); return;

preventFromLeavingRoom:
  CYC(b_+650, b_+653); SET_BC(0xa8e8);
  CYC(b_+653, b_+655); E = 0x08;
  CYC(b_+655, b_+656); H = D;
  CYC(b_+656, b_+658); L = 0x0b;
  CYC(b_+658, b_+659); A = E;
  CYC(b_+659, b_+660); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+660, b_+662); goto L_593d; }
  CYC(b_+660, b_+662);
  CYC(b_+662, b_+663); mem_wr(gb, HL, A);
L_593d:
  CYC(b_+663, b_+664); A = B;
  CYC(b_+664, b_+665); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+665, b_+667); goto L_5942; }
  CYC(b_+665, b_+667);
  CYC(b_+667, b_+668); mem_wr(gb, HL, A);
L_5942:
  CYC(b_+668, b_+670); L = 0x0d;
  CYC(b_+670, b_+671); A = E;
  CYC(b_+671, b_+672); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+672, b_+674); goto L_5949; }
  CYC(b_+672, b_+674);
  CYC(b_+674, b_+675); mem_wr(gb, HL, A);
L_5949:
  CYC(b_+675, b_+676); A = C;
  CYC(b_+676, b_+677); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { RET_TAKEN(b_+677); return; }
  CYC(b_+677, b_+678);
  CYC(b_+678, b_+679); mem_wr(gb, HL, A);
  RET(b_+679); return;
}

// ITEM_MAGNET_BALL
// Variables:
// var03: Disables collisions and uses custom code to prevent enemies passing
// through it (for wall flame shooters)
// var30/var31: Set to initial yh,xh to reset ball's position in wStaticObjects
// if ball fell in hole
// var32: boolean that restricts friction from its max of SPEED_300 to SPEED_100
// var33: vertical friction - the higher, the faster
// var34: horizontal friction
void s_itemCode29_hook(GB *gb) {
  BASE(itemCode29);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0x04;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (magnetBall_jump_table(gb));
    if (jt_ == b_+10) goto unitialized;
    if (jt_ == b_+58) goto mainState;
    if (jt_ == b_+421) goto fallingDownCliff;
    HANDOFF(HL);
  } while (0);
unitialized:
  CYC(b_+10, b_+12); A = 0x01;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  CYC(b_+13, b_+14); H = D;
  CYC(b_+14, b_+16); L = 0x10;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x0a);
  CYC(b_+18, b_+20); L = 0x0b;
  CYC(b_+20, b_+21); A = mem_rd(gb, HL);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+24); L = 0x0d;
  CYC(b_+24, b_+25); A = mem_rd(gb, HL);
  CYC(b_+25, b_+27); L = 0x31;
  CYC(b_+27, b_+28); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+28, b_+29); mem_wr(gb, HL, B);
  CALL_C(b_+29, s_itemLoadAttributesAndGraphics, SYM(itemLoadAttributesAndGraphics), b_+32);
  CYC(b_+32, b_+33); alu_xor(gb, A);
  CALL_C(b_+33, s_itemSetAnimation, SYM(itemSetAnimation), b_+36);
  CALL_C(b_+36, s_objectSetVisiblec3, SYM(objectSetVisiblec3), b_+39);
  CYC(b_+39, b_+42); A = mem_rd(gb, wActiveGroup);
  CYC(b_+42, b_+44); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(b_+44, b_+46); goto mainState; }
  CYC(b_+44, b_+46);
  CYC(b_+46, b_+49); A = mem_rd(gb, wActiveRoom);
  CYC(b_+49, b_+51); alu_cp(gb, 0x94);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto mainState; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); E = 0x03;
  CYC(b_+55, b_+57); A = 0x01;
  CYC(b_+57, b_+58); mem_wr(gb, DE, A);
mainState:
  CALL_L(b_+58, magnetBall_mainStateBody, b_+61);
  CALL_L(b_+61, magnetBall_applySpeedIfNoCollision, b_+64);
  CYC(b_+64, b_+66); E = 0x24;
  CYC(b_+66, b_+67); A = mem_rd(gb, DE);
  CYC(b_+67, b_+69); alu_bit(gb, 7, A);
  if (!(F & FZ)) { RET_TAKEN(b_+69); return; }
  CYC(b_+69, b_+70);
  CYC(b_+70, b_+73);
  goto blockAllWallFlameShooters;

fallingDownCliff:
  CYC(b_+421, b_+422); H = D;
  CYC(b_+422, b_+424); L = 0x24;
  CYC(b_+424, b_+426); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  CYC(b_+426, b_+428); L = 0x08;
  CYC(b_+428, b_+429); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+429, b_+430); mem_wr(gb, HL, A);
  CALL_C(b_+430, s_objectApplySpeed, SYM(objectApplySpeed), b_+433);
  CYC(b_+433, b_+435); C = 0x20;
  CALL_C(b_+435, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+438);
  if (!(F & FZ)) { RET_TAKEN(b_+438); return; }
  CYC(b_+438, b_+439);
  CYC(b_+439, b_+441); A = 0x77;
  CALL_C(b_+441, s_playSound, SYM(playSound_b00), b_+444);
  CYC(b_+444, b_+445); H = D;
  CYC(b_+445, b_+447); L = 0x06;
  CYC(b_+447, b_+448); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+448, b_+450); goto L_5875; }
  CYC(b_+448, b_+450);
  CYC(b_+450, b_+453); SET_BC(0xff20);
  CYC(b_+453, b_+455); L = 0x14;
  CYC(b_+455, b_+456); mem_wr(gb, HL, C);
  CYC(b_+456, b_+457); L = alu_inc8(gb, L);
  CYC(b_+457, b_+458); mem_wr(gb, HL, B);
  CYC(b_+458, b_+460); L = 0x10;
  CYC(b_+460, b_+462); mem_wr(gb, HL, 0x14);
  RET(b_+462); return;
L_5875:
  CYC(b_+463, b_+465); A = 0x01;
  CYC(b_+465, b_+467); E = 0x04;
  CYC(b_+467, b_+468); mem_wr(gb, DE, A);
  RET(b_+468); return;

blockAllWallFlameShooters:
  CYC(b_+864, b_+867); SET_HL(w1ReservedInteraction0_var3f + 0x01);
L_5a09:
  CYC(b_+867, b_+868); A = mem_rd(gb, HL);
  CYC(b_+868, b_+869); alu_or(gb, A);
  if (!(F & FZ)) CALL_L_CC(b_+869, magnetBall_blockWallFlameShooter, b_+872);
  else CYC(b_+869, b_+872);
  CYC(b_+872, b_+873); H = alu_inc8(gb, H);
  CYC(b_+873, b_+874); A = H;
  CYC(b_+874, b_+876); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(b_+876, b_+878); goto L_5a09; }
  CYC(b_+876, b_+878);
  RET(b_+878); return;
}

