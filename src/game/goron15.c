#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void writeFlagsTocddb_hook(GB *gb);

void goron_putLinkInState08_hook(GB *gb);
void goron_findInteractionWithID_hook(GB *gb);

static void goron_addAToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) { burn_rom(gb, 0x00, 0x0012, 0x0013, false); burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H); burn_rom(gb, 0x00, 0x0014, 0x0015, false); }
  else burn_rom(gb, 0x00, 0x0012, 0x0013, true);
  ret_effect(gb);
}

static void goron_addDoubleIndexToHl_from_rst(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void goron_setLinkPositionAndDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x634b, 0x634e); SET_HL(0xd008);
  CYC(0x634e, 0x634f); mem_wr(gb, HL, A);
  CYC(0x634f, 0x6351); L = 0x0b;
  CYC(0x6351, 0x6352); mem_wr(gb, HL, B);
  CYC(0x6352, 0x6354); L = 0x0d;
  CYC(0x6354, 0x6355); mem_wr(gb, HL, C);
  goron_putLinkInState08_hook(gb);
}

void goron_putLinkInState08_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6355, putLinkOnGround_hook, 0x2a8c, 0x6358);
  CYC(0x6358, 0x635b); setLinkForceStateToState08_hook(gb);
}

static void goron_showText_differentForPresent_add20(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  CYC(0x63a6, 0x63a7); A = C;
  CYC(0x63a7, 0x63a9); alu_add(gb, 0x20);
  CYC(0x63a9, 0x63aa); C = A;
  CYC(0x63aa, 0x63ab); ret_effect(gb);
}

void goron_showText_differentForPresent_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6398, 0x6399); C = A;
  CYC(0x6399, 0x639c); A = mem_rd(gb, 0xcc34);
  CYC(0x639c, 0x639e); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x639e, 0x63a1); goron_showText_differentForPresent_add20(gb, 0x63a1); }
  else CYC(0x639e, 0x63a1);
  CYC(0x63a1, 0x63a3); B = 0x24;
  CYC(0x63a3, 0x63a6); showText_hook(gb);
}

void goron_decideTextToShow_differentForLinkedInPast_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x63ab, 0x63ac); C = A;
  CALL_C(0x63ac, checkIsLinkedGame_hook, 0x1992, 0x63af);
  if (!(F & FZ)) { CYCT(0x63af, 0x63b1); goto linked; }
  CYC(0x63af, 0x63b1);
  CYC(0x63b1, 0x63b4); A = mem_rd(gb, 0xcc34);
  CYC(0x63b4, 0x63b6); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x63b6, 0x63b8); goto showPresentText; }
  CYC(0x63b6, 0x63b8);
  CYC(0x63b8, 0x63ba); goto showText;

showPresentText:
  CYC(0x63ba, 0x63bb); A = C;
  CYC(0x63bb, 0x63bd); alu_add(gb, 0x10);
  CYC(0x63bd, 0x63be); C = A;

showText:
  CYC(0x63be, 0x63c0); B = 0x24;
  CYC(0x63c0, 0x63c3); showText_hook(gb); return;

linked:
  CYC(0x63c3, 0x63c6); A = mem_rd(gb, 0xcc34);
  CYC(0x63c6, 0x63c8); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x63c8, 0x63ca); goto showPresentText; }
  CYC(0x63c8, 0x63ca);
  CYC(0x63ca, 0x63cb); A = C;
  CYC(0x63cb, 0x63cd); alu_add(gb, 0x20);
  CYC(0x63cd, 0x63ce); C = A;
  CYC(0x63ce, 0x63d0); goto showText;
}

static void goron_showText_differentForPast_add0c(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  CYC(0x63de, 0x63df); A = C;
  CYC(0x63df, 0x63e1); alu_add(gb, 0x0c);
  CYC(0x63e1, 0x63e2); C = A;
  CYC(0x63e2, 0x63e3); ret_effect(gb);
}

void goron_showText_differentForPast_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x63d0, 0x63d1); C = A;
  CYC(0x63d1, 0x63d4); A = mem_rd(gb, 0xcc34);
  CYC(0x63d4, 0x63d6); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x63d6, 0x63d9); goron_showText_differentForPast_add0c(gb, 0x63d9); }
  else CYC(0x63d6, 0x63d9);
  CYC(0x63d9, 0x63db); B = 0x24;
  CYC(0x63db, 0x63de); showText_hook(gb);
}

void goron_showTextForGoronWorriedAboutElder_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x63e3, 0x63e5); A = 0x2f;
  CALL_C(0x63e5, checkGlobalFlag_hook, 0x31f3, 0x63e8);
  if (!(F & FZ)) { CYCT(0x63e8, 0x63ea); goto L_63ee; }
  CYC(0x63e8, 0x63ea);
  CYC(0x63ea, 0x63ec); C = 0x79;
  CYC(0x63ec, 0x63ee); goto L_63f0;

L_63ee:
  CYC(0x63ee, 0x63f0); C = 0x7a;
L_63f0:
  CYC(0x63f0, 0x63f2); B = 0x24;
  CYC(0x63f2, 0x63f5); showText_hook(gb);
}

void goron_showTextForSubid05_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x63f5, 0x63f7); E = 0x43;
  CYC(0x63f7, 0x63f8); A = mem_rd(gb, DE);
  CYC(0x63f8, 0x63fa); alu_cp(gb, 0x03);
  if (!(F & FC)) { CYCT(0x63fa, 0x63fc); goto threeOrHigher; }
  CYC(0x63fa, 0x63fc);
  CYC(0x63fc, 0x63fe); A = 0x2f;
  CALL_C(0x63fe, checkGlobalFlag_hook, 0x31f3, 0x6401);
  CYC(0x6401, 0x6403); B = 0x00;
  if (F & FZ) { CYCT(0x6403, 0x6405); goto L_6407; }
  CYC(0x6403, 0x6405);
  CYC(0x6405, 0x6407); B = 0x01;

L_6407:
  CYC(0x6407, 0x6409); E = 0x43;
  CYC(0x6409, 0x640a); A = mem_rd(gb, DE);
  CYC(0x640a, 0x640b); alu_rlca(gb);
  CYC(0x640b, 0x640d); goto showTextTail;

threeOrHigher:
  CYC(0x640d, 0x640f); B = 0x03;

showTextTail:
  CYC(0x640f, 0x6410); alu_add(gb, B);
  CYC(0x6410, 0x6413); SET_HL(0x641a);
  CYC(0x6413, 0x6414); goron_addAToHl_from_rst(gb, 0x6414);
  CYC(0x6414, 0x6416); B = 0x24;
  CYC(0x6416, 0x6417); C = mem_rd(gb, HL);
  CYC(0x6417, 0x641a); showText_hook(gb);
}

static void goron_determineTextForGenericNpc_getGameState(GB *gb, uint16_t return_address, uint16_t sp0_) {
  push_effect(gb, return_address);
  CYC(0x6429, 0x642c); A = mem_rd(gb, wTilesetFlags);
  CYC(0x642c, 0x642e); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x642e, 0x6430); goto inPresent; }
  CYC(0x642e, 0x6430);

  CYC(0x6430, 0x6432); A = 0x14;
  CALL_C(0x6432, checkGlobalFlag_hook, 0x31f3, 0x6435);
  if (!(F & FZ)) { CYCT(0x6435, 0x6437); goto val02; }
  CYC(0x6435, 0x6437);
  CYC(0x6437, 0x6439); A = 0x2f;
  CALL_C(0x6439, checkGlobalFlag_hook, 0x31f3, 0x643c);
  if (!(F & FZ)) { CYCT(0x643c, 0x643e); goto val01; }
  CYC(0x643c, 0x643e);
  CYC(0x643e, 0x6440); goto val00;

inPresent:
  CYC(0x6440, 0x6442); A = 0x14;
  CALL_C(0x6442, checkGlobalFlag_hook, 0x31f3, 0x6445);
  if (!(F & FZ)) { CYCT(0x6445, 0x6447); goto val03; }
  CYC(0x6445, 0x6447);
  CYC(0x6447, 0x6449); A = 0x1a;
  CALL_C(0x6449, checkGlobalFlag_hook, 0x31f3, 0x644c);
  if (!(F & FZ)) { CYCT(0x644c, 0x644e); goto val02; }
  CYC(0x644c, 0x644e);
  CYC(0x644e, 0x6450); A = 0x03;
  CYC(0x6450, 0x6453); SET_HL(wEssencesObtained);
  CALL_C(0x6453, checkFlag_hook, 0x0205, 0x6456);
  if (!(F & FZ)) { CYCT(0x6456, 0x6458); goto val01; }
  CYC(0x6456, 0x6458);

val00:
  CYC(0x6458, 0x6459); alu_xor(gb, A);
  CYC(0x6459, 0x645b); goto writeVal;

val01:
  CYC(0x645b, 0x645d); A = 0x01;
  CYC(0x645d, 0x645f); goto writeVal;

val02:
  CYC(0x645f, 0x6461); A = 0x02;
  CYC(0x6461, 0x6463); goto writeVal;

val03:
  CYC(0x6463, 0x6465); A = 0x03;

writeVal:
  CYC(0x6465, 0x6467); E = 0x7b;
  CYC(0x6467, 0x6468); mem_wr(gb, DE, A);
  CYC(0x6468, 0x6469); ret_effect(gb);
}

void goron_determineTextForGenericNpc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6423, 0x6426); goron_determineTextForGenericNpc_getGameState(gb, 0x6426, sp0_);
  CYC(0x6426, 0x6429);

  CYC(0x6469, 0x646b); E = 0x42;
  CYC(0x646b, 0x646c); A = mem_rd(gb, DE);
  CYC(0x646c, 0x646e); alu_sub(gb, 0x0c);
  CYC(0x646e, 0x6471); SET_HL(0x6493);
  CYC(0x6471, 0x6472); goron_addAToHl_from_rst(gb, 0x6472);
  CYC(0x6472, 0x6473); A = mem_rd(gb, HL);
  CYC(0x6473, 0x6474); goron_addAToHl_from_rst(gb, 0x6474);
  CYC(0x6474, 0x6476); E = 0x43;
  CYC(0x6476, 0x6477); A = mem_rd(gb, DE);
  CYC(0x6477, 0x6478); alu_rlca(gb);
  CYC(0x6478, 0x6479); goron_addDoubleIndexToHl_from_rst(gb, 0x6479);
  CYC(0x6479, 0x647b); E = 0x7b;
  CYC(0x647b, 0x647c); A = mem_rd(gb, DE);
  CYC(0x647c, 0x647d); goron_addAToHl_from_rst(gb, 0x647d);
  CYC(0x647d, 0x647e); A = mem_rd(gb, HL);
  CYC(0x647e, 0x6480); E = 0x72;
  CYC(0x6480, 0x6481); mem_wr(gb, DE, A);
  CYC(0x6481, 0x6482); B = A;
  CYC(0x6482, 0x6483); E = alu_inc8(gb, E);
  CYC(0x6483, 0x6485); A = 0x31;
  CYC(0x6485, 0x6486); mem_wr(gb, DE, A);
  CYC(0x6486, 0x6487); A = B;
  CYC(0x6487, 0x6489); alu_cp(gb, 0x27);
  if (!(F & FZ)) { CYCT(0x6489, 0x648a); ret_effect(gb); return; }
  CYC(0x6489, 0x648a);
  CALL_C(0x648a, checkIsLinkedGame_hook, 0x1992, 0x648d);
  if (!(F & FZ)) { CYCT(0x648d, 0x648e); ret_effect(gb); return; }
  CYC(0x648d, 0x648e);
  CYC(0x648e, 0x6490); A = 0xff;
  CYC(0x6490, 0x6491); E = alu_dec8(gb, E);
  CYC(0x6491, 0x6492); mem_wr(gb, DE, A);
  CYC(0x6492, 0x6493); ret_effect(gb);
}

void goron_checkShouldBeNapping_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x64f6, 0x64f9); SET_BC(0x1818);
  CALL_C(0x64f9, objectSetCollideRadii_hook, 0x24a9, 0x64fc);
  CALL_C(0x64fc, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x64ff);
  CYC(0x64ff, 0x6500); alu_ccf(gb);
  CALL_C(0x6500, writeFlagsTocddb_hook, 0x5118, 0x6503);
  CYC(0x6503, 0x6506); SET_BC(0x0606);
  CYC(0x6506, 0x6509); objectSetCollideRadii_hook(gb);
}

void goron_faceDown_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6509, 0x650a); H = D;
  CYC(0x650a, 0x650c); L = 0x6b;
  CYC(0x650c, 0x650e); mem_wr(gb, HL, 0x00);
  CYC(0x650e, 0x6510); L = 0x49;
  CYC(0x6510, 0x6512); mem_wr(gb, HL, 0x10);
  CYC(0x6512, 0x6514); L = 0x7f;
  CYC(0x6514, 0x6516); mem_wr(gb, HL, 0x00);
  CYC(0x6516, 0x6518); A = 0x02;
  CYC(0x6518, 0x651b); interactionSetAnimation_hook(gb);
}

void goron_setAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x651b, 0x651c); H = D;
  CYC(0x651c, 0x651e); L = 0x7f;
  CYC(0x651e, 0x6520); mem_wr(gb, HL, 0x01);
  CYC(0x6520, 0x6523); interactionSetAnimation_hook(gb);
}

void goron_reverseWalkingDirection_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x653f, 0x6540); H = D;
  CYC(0x6540, 0x6542); L = 0x7c;
  CYC(0x6542, 0x6544); mem_wr(gb, HL, 0x80);
  CYC(0x6544, 0x6545); L = alu_inc8(gb, L);
  CYC(0x6545, 0x6547); mem_wr(gb, HL, 0x00);
  CYC(0x6547, 0x6549); L = 0x49;
  CYC(0x6549, 0x654a); A = mem_rd(gb, HL);
  CYC(0x654a, 0x654c); alu_xor(gb, 0x10);
  CYC(0x654c, 0x654d); mem_wr(gb, HL, A);
  CYC(0x654d, 0x654f); L = 0x7e;
  CYC(0x654f, 0x6550); A = mem_rd(gb, HL);
  CYC(0x6550, 0x6552); alu_xor(gb, 0x02);
  CYC(0x6552, 0x6553); mem_wr(gb, HL, A);
  CYC(0x6553, 0x6556); interactionSetAnimation_hook(gb);
}

void goron_refreshWalkingAnimation_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6556, 0x6558); E = 0x7e;
  CYC(0x6558, 0x6559); A = mem_rd(gb, DE);
  CYC(0x6559, 0x655c); interactionSetAnimation_hook(gb);
}

void goron_setSpeedToMoveDown_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x655c, 0x655d); H = D;
  CYC(0x655d, 0x655f); L = 0x50;
  CYC(0x655f, 0x6561); mem_wr(gb, HL, 0x28);
  CYC(0x6561, 0x6563); L = 0x49;
  CYC(0x6563, 0x6565); mem_wr(gb, HL, 0x10);
  CYC(0x6565, 0x6567); A = 0x02;
  CYC(0x6567, 0x656a); goron_setAnimation_hook(gb);
}

static void goron_cpLinkY_writeResult(GB *gb) {
  CYC(0x6573, 0x6574); H = D;
  CYC(0x6574, 0x6576); L = 0x4b;
  CYC(0x6576, 0x6577); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x6577, 0x657a); writeFlagsTocddb_hook(gb);
}

void goron_cpLinkY_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x656a, 0x656b); alu_xor(gb, A);
  CYC(0x656b, 0x656e); SET_HL(0xd00b);
  CYC(0x656e, 0x656f); alu_add(gb, mem_rd(gb, HL));
  CYC(0x656f, 0x6571); goron_cpLinkY_writeResult(gb);
}

void goron_cpYTo60_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6571, 0x6573); A = 0x60;
  goron_cpLinkY_writeResult(gb);
}

static void goron_checkReachedLinkHorizontally_writeResult(GB *gb) {
  CYC(0x6584, 0x6585); H = D;
  CYC(0x6585, 0x6587); L = 0x4d;
  CYC(0x6587, 0x6588); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x6588, 0x658b); writeFlagsTocddb_hook(gb);
}

void goron_checkReachedLinkHorizontally_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x657a, 0x657c); A = 0xf2;
  CYC(0x657c, 0x657f); SET_HL(0xd00d);
  CYC(0x657f, 0x6580); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6580, 0x6582); goron_checkReachedLinkHorizontally_writeResult(gb);
}

void goron_cpXTo48_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6582, 0x6584); A = 0x48;
  goron_checkReachedLinkHorizontally_writeResult(gb);
}

void goron_checkLinkApproachedWithBombFlower_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x658b, 0x658d); A = 0x49;
  CALL_C(0x658d, checkTreasureObtained_hook, 0x1748, 0x6590);
  CALL_C(0x6590, writeFlagsTocddb_hook, 0x5118, 0x6593);
  if (!(F & FC)) { CYCT(0x6593, 0x6594); ret_effect(gb); return; }
  CYC(0x6593, 0x6594);
  CYC(0x6594, 0x6595); H = D;
  CYC(0x6595, 0x6597); L = 0x4b;
  CYC(0x6597, 0x6598); A = mem_rd(gb, HL);
  CYC(0x6598, 0x659a); mem_wr(gb, HL, 0x88);
  CYC(0x659a, 0x659b); push_effect(gb, AF);
  CYC(0x659b, 0x659d); L = 0x4d;
  CYC(0x659d, 0x659e); A = mem_rd(gb, HL);
  CYC(0x659e, 0x65a0); mem_wr(gb, HL, 0x58);
  CYC(0x65a0, 0x65a1); push_effect(gb, AF);
  CYC(0x65a1, 0x65a4); SET_BC(0x1808);
  CALL_C(0x65a4, objectSetCollideRadii_hook, 0x24a9, 0x65a7);
  CALL_C(0x65a7, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x65aa);
  CALL_C(0x65aa, writeFlagsTocddb_hook, 0x5118, 0x65ad);
  CYC(0x65ad, 0x65b0); SET_BC(0x0606);
  CALL_C(0x65b0, objectSetCollideRadii_hook, 0x24a9, 0x65b3);
  CYC(0x65b3, 0x65b4); SET_AF(pop_effect(gb));
  CYC(0x65b4, 0x65b5); H = D;
  CYC(0x65b5, 0x65b7); L = 0x4d;
  CYC(0x65b7, 0x65b8); mem_wr(gb, HL, A);
  CYC(0x65b8, 0x65b9); SET_AF(pop_effect(gb));
  CYC(0x65b9, 0x65bb); L = 0x4b;
  CYC(0x65bb, 0x65bc); mem_wr(gb, HL, A);
  CYC(0x65bc, 0x65bd); ret_effect(gb);
}

void goron_decMovementCounter_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65bd, 0x65be); H = D;
  CYC(0x65be, 0x65c0); L = 0x7c;
  CALL_C(0x65c0, decHlRef16WithCap_hook, 0x0237, 0x65c3);
  CYC(0x65c3, 0x65c6); writeFlagsTocddb_hook(gb);
}

void goron_initCountersForBombFlowerExplosion_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65c6, 0x65c7); H = D;
  CYC(0x65c7, 0x65c9); L = 0x7c;
  CYC(0x65c9, 0x65cb); mem_wr(gb, HL, 0x5a);
  CYC(0x65cb, 0x65cc); L = alu_inc8(gb, L);
  CYC(0x65cc, 0x65ce); mem_wr(gb, HL, 0x00);
  CYC(0x65ce, 0x65d0); L = 0x7e;
  CYC(0x65d0, 0x65d2); mem_wr(gb, HL, 0x01);
  CYC(0x65d2, 0x65d3); ret_effect(gb);
}

void goron_countdownToPlayRockSoundAndShakeScreen_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65d3, 0x65d4); H = D;
  CYC(0x65d4, 0x65d6); L = 0x7e;
  CYC(0x65d6, 0x65d7); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x65d7, 0x65d8); ret_effect(gb); return; }
  CYC(0x65d7, 0x65d8);
  CYC(0x65d8, 0x65da); mem_wr(gb, HL, 0x05);
  CYC(0x65da, 0x65dc); A = 0xa5;
  CALL_C(0x65dc, playSound_b00_hook, 0x0c98, 0x65df);
  CYC(0x65df, 0x65e1); A = 0x04;
  CYC(0x65e1, 0x65e4); setScreenShakeCounter_hook(gb);
}

void goron_createFallingRockSpawner_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65e4, 0x65e6); B = 0x92;
  CYC(0x65e6, 0x65e9); objectCreateInteractionWithSubid00_hook(gb);
}

static void goron_clearRockBarrier_clearRow_body(GB *gb, uint16_t sp0_) {
  CYC(0x65f8, 0x65fa); A = 0x05;
L_nextTile:
  CYC(0x65fa, 0x65fc); hram_wr(gb, 0x8b, A);
  CYC(0x65fc, 0x65fd); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x65fd, 0x65fe); push_effect(gb, BC);
  CYC(0x65fe, 0x65ff); push_effect(gb, HL);
  CALL_C(0x65ff, setTile_hook, 0x3a9c, 0x6602);
  CYC(0x6602, 0x6603); SET_HL(pop_effect(gb));
  CYC(0x6603, 0x6604); SET_BC(pop_effect(gb));
  CYC(0x6604, 0x6605); C = alu_inc8(gb, C);
  CYC(0x6605, 0x6607); A = hram_rd(gb, 0x8b);
  CYC(0x6607, 0x6608); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(0x6608, 0x660a); goto L_nextTile; }
  CYC(0x6608, 0x660a);
  CYC(0x660a, 0x660b); ret_effect(gb);
}

static void goron_clearRockBarrier_clearRow_from_call(GB *gb, uint16_t return_address, uint16_t sp0_) {
  push_effect(gb, return_address);
  goron_clearRockBarrier_clearRow_body(gb, sp0_);
}

void goron_clearRockBarrier_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x65e9, 0x65ec); SET_HL(0x660b);
  CYC(0x65ec, 0x65ee); C = 0x31;
  CYC(0x65ee, 0x65f1); goron_clearRockBarrier_clearRow_from_call(gb, 0x65f1, sp0_);
  CYC(0x65f1, 0x65f3); C = 0x41;
  CYC(0x65f3, 0x65f6); goron_clearRockBarrier_clearRow_from_call(gb, 0x65f6, sp0_);
  CYC(0x65f6, 0x65f8); C = 0x51;
  goron_clearRockBarrier_clearRow_body(gb, sp0_);
}

static void goron_createRockDebris_body(GB *gb, uint16_t sp0_) {
  CALL_C(0x6622, getRandomNumber_hook, 0x043e, 0x6625);
  CYC(0x6625, 0x6627); alu_and(gb, 0x01);
  CYC(0x6627, 0x6629); hram_wr(gb, 0x8d, A);
  CYC(0x6629, 0x662a); alu_xor(gb, A);
L_nextRock:
  CYC(0x662a, 0x662c); hram_wr(gb, 0x8b, A);
  CALL_C(0x662c, getFreeInteractionSlot_hook, 0x3aef, 0x662f);
  if (!(F & FZ)) { CYCT(0x662f, 0x6631); goto end; }
  CYC(0x662f, 0x6631);
  CYC(0x6631, 0x6633); mem_wr(gb, HL, 0x92);
  CYC(0x6633, 0x6634); L = alu_inc8(gb, L);
  CYC(0x6634, 0x6636); mem_wr(gb, HL, 0x02);
  CYC(0x6636, 0x6637); L = alu_inc8(gb, L);
  CYC(0x6637, 0x6639); mem_wr(gb, HL, 0x01);
  CYC(0x6639, 0x663b); L = 0x46;
  CYC(0x663b, 0x663d); A = hram_rd(gb, 0x8d);
  CYC(0x663d, 0x663e); mem_wr(gb, HL, A);
  CYC(0x663e, 0x6640); L = 0x49;
  CYC(0x6640, 0x6642); A = hram_rd(gb, 0x8b);
  CYC(0x6642, 0x6643); mem_wr(gb, HL, A);
  CALL_C(0x6643, objectCopyPositionWithOffset_hook, 0x225a, 0x6646);
  CYC(0x6646, 0x6648); A = hram_rd(gb, 0x8b);
  CYC(0x6648, 0x6649); A = alu_inc8(gb, A);
  CYC(0x6649, 0x664b); alu_cp(gb, 0x04);
  if (!(F & FZ)) { CYCT(0x664b, 0x664d); goto L_nextRock; }
  CYC(0x664b, 0x664d);
end:
  CYC(0x664d, 0x664f); A = 0xa5;
  CYC(0x664f, 0x6652); playSound_b00_hook(gb);
}

void goron_createRockDebrisToLeft_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x661a, 0x661d); SET_BC(0xf6fa);
  CYC(0x661d, 0x661f); goron_createRockDebris_body(gb, sp0_);
}

void goron_createRockDebrisToRight_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x661f, 0x6622); SET_BC(0xf606);
  goron_createRockDebris_body(gb, sp0_);
}

void goron_tryTakeEmberSeedsAndBombs_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6652, 0x6654); A = 0x19;
  CALL_C(0x6654, checkTreasureObtained_hook, 0x1748, 0x6657);
  if (!(F & FC)) { CYCT(0x6657, 0x6659); goto dontGiveItems; }
  CYC(0x6657, 0x6659);
  CYC(0x6659, 0x665b); A = 0x20;
  CALL_C(0x665b, checkTreasureObtained_hook, 0x1748, 0x665e);
  if (!(F & FC)) { CYCT(0x665e, 0x6660); goto dontGiveItems; }
  CYC(0x665e, 0x6660);
  CYC(0x6660, 0x6662); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(0x6662, 0x6664); goto dontGiveItems; }
  CYC(0x6662, 0x6664);
  CYC(0x6664, 0x6665); push_effect(gb, AF);
  CYC(0x6665, 0x6667); A = 0x03;
  CALL_C(0x6667, checkTreasureObtained_hook, 0x1748, 0x666a);
  if (!(F & FC)) { CYCT(0x666a, 0x666c); goto popAndDontGiveItems; }
  CYC(0x666a, 0x666c);
  CYC(0x666c, 0x666e); alu_cp(gb, 0x20);
  if (F & FC) { CYCT(0x666e, 0x6670); goto popAndDontGiveItems; }
  CYC(0x666e, 0x6670);
  CYC(0x6670, 0x6672); alu_sub(gb, 0x20);
  CYC(0x6672, 0x6673); alu_daa(gb);
  CYC(0x6673, 0x6676); mem_wr(gb, 0xc6b0, A);
  CYC(0x6676, 0x6677); SET_AF(pop_effect(gb));
  CYC(0x6677, 0x6679); alu_sub(gb, 0x20);
  CYC(0x6679, 0x667a); alu_daa(gb);
  CYC(0x667a, 0x667d); mem_wr(gb, 0xc6b9, A);
  CALL_C(0x667d, setStatusBarNeedsRefreshBit1_hook, 0x17d8, 0x6680);
  CYC(0x6680, 0x6681); alu_xor(gb, A);
  CYC(0x6681, 0x6684); writeFlagsTocddb_hook(gb); return;

popAndDontGiveItems:
  CYC(0x6684, 0x6685); SET_AF(pop_effect(gb));

dontGiveItems:
  CYC(0x6685, 0x6686); alu_or(gb, D);
  CYC(0x6686, 0x6689); writeFlagsTocddb_hook(gb);
}

void goron_deleteTreasure_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x674e, 0x6750); B = 0x60;
  CALL_C(0x6750, goron_findInteractionWithID_hook, 0x6767, 0x6753);
  CYC(0x6753, 0x6755); L = 0x44;
  CYC(0x6755, 0x6757); mem_wr(gb, HL, 0x04);
  CYC(0x6757, 0x6758); ret_effect(gb);
}

void goron_findInteractionWithID_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6767, 0x676a); SET_HL(0xd240);
loop:
  CYC(0x676a, 0x676b); A = mem_rd(gb, HL);
  CYC(0x676b, 0x676c); alu_or(gb, A);
  if (F & FZ) { CYCT(0x676c, 0x676e); goto next; }
  CYC(0x676c, 0x676e);
  CYC(0x676e, 0x676f); L = alu_inc8(gb, L);
  CYC(0x676f, 0x6770); A = mem_rd(gb, HL); SET_HL(HL - 1);
  CYC(0x6770, 0x6771); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(0x6771, 0x6773); goto next; }
  CYC(0x6771, 0x6773);
  CYC(0x6773, 0x6774); alu_xor(gb, A);
  CYC(0x6774, 0x6775); ret_effect(gb); return;

next:
  CYC(0x6775, 0x6776); H = alu_inc8(gb, H);
  CYC(0x6776, 0x6777); A = H;
  CYC(0x6777, 0x6779); alu_cp(gb, 0xe0);
  if (F & FC) { CYCT(0x6779, 0x677b); goto loop; }
  CYC(0x6779, 0x677b);
  CYC(0x677b, 0x677c); alu_or(gb, H);
  CYC(0x677c, 0x677d); ret_effect(gb);
}

void goron_checkLinkNotInAir_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x67bd, 0x67c0); A = mem_rd(gb, 0xcc5c);
  CYC(0x67c0, 0x67c2); alu_bit(gb, 7, A);
  CYC(0x67c2, 0x67c5); writeFlagsTocddb_hook(gb);
}

void goron_checkGracefulGoronQuestStatus_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x686d, 0x686f); A = 0x5a;
  CALL_C(0x686f, checkTreasureObtained_hook, 0x1748, 0x6872);
  if (!(F & FC)) { CYCT(0x6872, 0x6874); goto noLavaJuice; }
  CYC(0x6872, 0x6874);
  CYC(0x6874, 0x6876); A = 0x45;
  CALL_C(0x6876, checkTreasureObtained_hook, 0x1748, 0x6879);
  if (!(F & FC)) { CYCT(0x6879, 0x687b); goto noMermaidKey; }
  CYC(0x6879, 0x687b);
  CYC(0x687b, 0x687c); alu_xor(gb, A);
  CYC(0x687c, 0x687e); goto writeByte;

noLavaJuice:
  CYC(0x687e, 0x6880); A = 0x02;
  CYC(0x6880, 0x6882); goto writeByte;

noMermaidKey:
  CYC(0x6882, 0x6884); A = 0x01;

writeByte:
  CYC(0x6884, 0x6886); E = 0x7e;
  CYC(0x6886, 0x6887); mem_wr(gb, DE, A);
  CYC(0x6887, 0x6888); ret_effect(gb);
}

void goron_showTextForClairvoyantGoron_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6888, 0x688a); B = 0x00;
  CYC(0x688a, 0x688d); A = mem_rd(gb, 0xc6bf);
  CYC(0x688d, 0x688f); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(0x688f, 0x6891); goto finishedRollingRidgeSidequest; }
  CYC(0x688f, 0x6891);
  CYC(0x6891, 0x6894); SET_HL(0x68dc);

nextTreasure:
  CYC(0x6894, 0x6895); B = alu_inc8(gb, B);
  CYC(0x6895, 0x6896); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(0x6896, checkTreasureObtained_hook, 0x1748, 0x6899);
  if (F & FC) { CYCT(0x6899, 0x689b); goto finishedRollingRidgeSidequest; }
  CYC(0x6899, 0x689b);
  CYC(0x689b, 0x689c); A = B;
  CYC(0x689c, 0x689e); alu_cp(gb, 0x08);
  if (!(F & FZ)) { CYCT(0x689e, 0x68a0); goto nextTreasure; }
  CYC(0x689e, 0x68a0);

finishedRollingRidgeSidequest:
  CYC(0x68a0, 0x68a1); A = B;
  CYC(0x68a1, 0x68a3); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x68a3, 0x68a5); goto L_68b0; }
  CYC(0x68a3, 0x68a5);
  CYC(0x68a5, 0x68a7); A = 0x5a;
  CALL_C(0x68a7, checkTreasureObtained_hook, 0x1748, 0x68aa);
  if (!(F & FC)) { CYCT(0x68aa, 0x68ac); goto showTipForItem; }
  CYC(0x68aa, 0x68ac);
  CYC(0x68ac, 0x68ae); B = 0x09;
  CYC(0x68ae, 0x68b0); goto showTipForItem;

L_68b0:
  CYC(0x68b0, 0x68b2); alu_cp(gb, 0x05);
  if (F & FC) { CYCT(0x68b2, 0x68b4); goto showTipForItem; }
  CYC(0x68b2, 0x68b4);
  CYC(0x68b4, 0x68b5); push_effect(gb, BC);
  CYC(0x68b5, 0x68b7); A = 0x03;
  CYC(0x68b7, 0x68b9); B = 0x3e;
  CALL_C(0x68b9, getRoomFlags_hook, 0x198a, 0x68bc);
  CYC(0x68bc, 0x68bd); SET_BC(pop_effect(gb));
  CYC(0x68bd, 0x68bf); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x68bf, 0x68c1); goto showTipForItem; }
  CYC(0x68bf, 0x68c1);
  CYC(0x68c1, 0x68c3); B = 0x0a;

showTipForItem:
  CYC(0x68c3, 0x68c6); A = mem_rd(gb, 0xcc34);
  CYC(0x68c6, 0x68c8); alu_and(gb, 0x80);
  if (F & FZ) { CYCT(0x68c8, 0x68ca); goto present; }
  CYC(0x68c8, 0x68ca);

  CYC(0x68ca, 0x68cc); A = 0x43;
  CYC(0x68cc, 0x68cd); alu_add(gb, B);
  CYC(0x68cd, 0x68cf); B = 0x31;
  CYC(0x68cf, 0x68d0); C = A;
  CYC(0x68d0, 0x68d3); showText_hook(gb); return;

present:
  CYC(0x68d3, 0x68d5); A = 0x4f;
  CYC(0x68d5, 0x68d6); alu_add(gb, B);
  CYC(0x68d6, 0x68d8); B = 0x31;
  CYC(0x68d8, 0x68d9); C = A;
  CYC(0x68d9, 0x68dc); showText_hook(gb);
}
