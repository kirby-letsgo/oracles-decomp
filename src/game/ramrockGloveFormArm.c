#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x11, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x11, (from), (to), true)

static uint16_t ramrockGloveFormArm_jump_table(GB *gb) {
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

static void ramrockGloveFormArm_addAToHl_from_rst(GB *gb, uint16_t return_address) {
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

static void ramrock35_state0func_6731(GB *gb) {
  CYC(0x6731, 0x6733); A = 0x01;
  CYC(0x6733, 0x6734); mem_wr(gb, DE, A);
  CYC(0x6734, 0x6736); E = 0xcf;
  CYC(0x6736, 0x6738); A = 0x81;
  CYC(0x6738, 0x6739); mem_wr(gb, DE, A);
  CYC(0x6739, 0x673a);
}

void partCode35_hook(GB *gb);
void func_693b_hook(GB *gb);
void state0func_6956_hook(GB *gb);
void state3func_6970_hook(GB *gb);
void state0func_6992_hook(GB *gb);
void func_69a5_hook(GB *gb);

void partCode35_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6686, 0x6688); A = 0x29; // Object.health
  CALL_C(0x6688, objectGetRelatedObject1Var_hook, 0x2160, 0x668b);
  CYC(0x668b, 0x668c); A = mem_rd(gb, HL);
  CYC(0x668c, 0x668d); alu_or(gb, A);
  if (F & FZ) { CYCT(0x668d, 0x6690); partDelete_hook(gb); return; } // jp z
  CYC(0x668d, 0x6690);
  CYC(0x6690, 0x6692); E = 0xc2; // Part.subid
  CYC(0x6692, 0x6693); A = mem_rd(gb, DE);
  CYC(0x6693, 0x6694); alu_rlca(gb);
  if (F & FC) { CYCT(0x6694, 0x6696); goto subidBit7SetArm; } // jr c
  CYC(0x6694, 0x6696);
  CYC(0x6696, 0x6699); A = mem_rd(gb, 0xcc5a); // wLinkGrabState
  CYC(0x6699, 0x669a); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(0x669a, objectPushLinkAwayOnCollision_hook, 0x230e, 0x669d);
  } else {
    CYC(0x669a, 0x669d);
  }
  CYC(0x669d, 0x669f); E = 0xc4; // Part.state
  CYC(0x669f, 0x66a0); A = mem_rd(gb, DE);
  {
    CYC(0x66a0, 0x66a1); push_effect(gb, 0x66a1);
    uint16_t target = ramrockGloveFormArm_jump_table(gb);
    if (target == 0x66d2) goto state0;
    if (target == 0x673a) goto state1;
    if (target == 0x677c) goto state2;
    if (target == 0x680f) goto state3;
    if (target == 0x686d) goto state4;
    if (target == 0x68e2) goto state5;
    goto state6;
  }

subidBit7SetArm:
  CYC(0x66af, 0x66b1); E = 0xc6;
  CYC(0x66b1, 0x66b2); A = mem_rd(gb, DE);
  CYC(0x66b2, 0x66b3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x66b3, 0x66b5); goto L_66c2; } // jr nz
  CYC(0x66b3, 0x66b5);
  CYC(0x66b5, 0x66b7); E = 0xc4;
  CYC(0x66b7, 0x66b8); A = mem_rd(gb, DE);
  CYC(0x66b8, 0x66ba); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(0x66ba, 0x66bc); goto L_66c2; } // jr z
  CYC(0x66ba, 0x66bc);
  CYC(0x66bc, 0x66be); E = 0xda;
  CYC(0x66be, 0x66bf); A = mem_rd(gb, DE);
  CYC(0x66bf, 0x66c1); alu_xor(gb, 0x80);
  CYC(0x66c1, 0x66c2); mem_wr(gb, DE, A);
L_66c2:
  CYC(0x66c2, 0x66c4); E = 0xc4;
  CYC(0x66c4, 0x66c5); A = mem_rd(gb, DE);
  {
    CYC(0x66c5, 0x66c6); push_effect(gb, 0x66c6);
    uint16_t target = ramrockGloveFormArm_jump_table(gb);
    if (target == 0x6705) goto subidBit7SetArm_state0;
    if (target == 0x6747) goto subidBit7SetArm_state1;
    if (target == 0x67f1) goto subidBit7SetArm_state2;
    if (target == 0x680f) goto state3;
    if (target == 0x68c3) goto subidBit7SetArm_state4;
    goto subidBit7SetArm_state5;
  }

state0:
  CYC(0x66d2, 0x66d5); ramrock35_state0func_6731(gb);
  CYC(0x66d5, 0x66d7); E = 0xd7;
  CYC(0x66d7, 0x66d8); A = mem_rd(gb, DE);
  CYC(0x66d8, 0x66da); E = 0xf0;
  CYC(0x66da, 0x66db); mem_wr(gb, DE, A);
  CALL_C(0x66db, state0func_6956_hook, 0x6956, 0x66de);
  CYC(0x66de, 0x66df); A = mem_rd(gb, DE);
  CYC(0x66df, 0x66e1); A = alu_swap(gb, A);
  CYC(0x66e1, 0x66e2); mem_wr(gb, DE, A);
  CYC(0x66e2, 0x66e4); alu_or(gb, 0x80);
  CYC(0x66e4, 0x66e5); mem_wr(gb, HL, A);
  CALL_C(0x66e5, state0func_6992_hook, 0x6992, 0x66e8);
  CYC(0x66e8, 0x66ea); L = 0xd6;
  CYC(0x66ea, 0x66ec); A = 0xc0;
  CYC(0x66ec, 0x66ed); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x66ed, 0x66ee); mem_wr(gb, HL, D);
  CYC(0x66ee, 0x66f0); E = 0xc2;
  CYC(0x66f0, 0x66f1); A = mem_rd(gb, DE);
  CYC(0x66f1, 0x66f3); A = alu_swap(gb, A);
  CYC(0x66f3, 0x66f6); SET_HL(0x6703);
  CYC(0x66f6, 0x66f7); ramrockGloveFormArm_addAToHl_from_rst(gb, 0x66f7);
  CYC(0x66f7, 0x66f8); A = mem_rd(gb, HL);
  CYC(0x66f8, 0x66fa); E = 0xc9;
  CYC(0x66fa, 0x66fb); mem_wr(gb, DE, A);
  CYC(0x66fb, 0x66fd); A = 0x51; // SND_THROW
  CALL_C(0x66fd, playSound_b00_hook, 0x0c98, 0x6700);
  CYC(0x6700, 0x6703); objectSetVisiblec0_hook(gb); return; // jp

subidBit7SetArm_state0:
  CYC(0x6705, 0x6708); ramrock35_state0func_6731(gb);
  CALL_C(0x6708, state0func_6956_hook, 0x6956, 0x670b);
  CALL_C(0x670b, state0func_6992_hook, 0x6992, 0x670e);
  CYC(0x670e, 0x6710); L = 0xd6;
  CYC(0x6710, 0x6712); A = 0xc0;
  CYC(0x6712, 0x6713); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6713, 0x6714); mem_wr(gb, HL, D);
  CYC(0x6714, 0x6716); L = 0xf0;
  CYC(0x6716, 0x6717); E = L;
  CYC(0x6717, 0x6718); A = mem_rd(gb, DE);
  CYC(0x6718, 0x6719); mem_wr(gb, HL, A);
  CYC(0x6719, 0x671b); A = 0x01;
  CALL_C(0x671b, partSetAnimation_hook, 0x2988, 0x671e);
  CYC(0x671e, 0x6720); E = 0xc2;
  CYC(0x6720, 0x6721); A = mem_rd(gb, DE);
  CYC(0x6721, 0x6723); alu_and(gb, 0x0f);
  CYC(0x6723, 0x6725); alu_add(gb, 0x0a);
  CYC(0x6725, 0x6727); E = 0xc6;
  CYC(0x6727, 0x6728); mem_wr(gb, DE, A);
  CYC(0x6728, 0x672a); E = 0xe4;
  CYC(0x672a, 0x672b); A = mem_rd(gb, DE);
  CYC(0x672b, 0x672d); A = (uint8_t)(A & ~(1 << 7));
  CYC(0x672d, 0x672e); mem_wr(gb, DE, A);
  CYC(0x672e, 0x6731); objectSetVisiblec1_hook(gb); return; // jp

state1:
  CYC(0x673a, 0x673c); C = 0x10;
  CALL_C(0x673c, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x673f);
  if (!(F & FZ)) { RET_TAKEN(0x673f); return; } // ret nz
  CYC(0x673f, 0x6740);
  CYC(0x6740, 0x6742); E = 0xf1;
  CYC(0x6742, 0x6743); A = mem_rd(gb, DE);
  CYC(0x6743, 0x6744); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6744, 0x6746); goto func_675e; } // jr nz
  CYC(0x6744, 0x6746);
  RET(0x6746); return; // ret

subidBit7SetArm_state1:
  CALL_C(0x6747, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x674a);
  if (!(F & FZ)) { RET_TAKEN(0x674a); return; } // ret nz
  CYC(0x674a, 0x674b);
  CYC(0x674b, 0x674d); C = 0x10;
  CALL_C(0x674d, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x6750);
  if (!(F & FZ)) { RET_TAKEN(0x6750); return; } // ret nz
  CYC(0x6750, 0x6751);
  CYC(0x6751, 0x6753); L = 0xc7;
  CYC(0x6753, 0x6754); A = mem_rd(gb, HL);
  CYC(0x6754, 0x6755); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6755, 0x6757); goto func_675e; } // jr nz
  CYC(0x6755, 0x6757);
  CYC(0x6757, 0x6758); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6758, 0x675b); SET_BC(0xfe80);
  CYC(0x675b, 0x675e); objectSetSpeedZ_hook(gb); return; // jp

func_675e:
  CYC(0x675e, 0x6760); A = 0x78;
  CYC(0x6760, 0x6762); goto L_6764; // jr

func_6762:
  CYC(0x6762, 0x6764); A = 0x14;
L_6764:
  CYC(0x6764, 0x6766); E = 0xd0;
  CYC(0x6766, 0x6767); mem_wr(gb, DE, A);
  CYC(0x6767, 0x6769); A = 0x31;
  CALL_C(0x6769, objectGetRelatedObject1Var_hook, 0x2160, 0x676c);
  CYC(0x676c, 0x676d); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x676d, 0x676f); E = 0xc4;
  CYC(0x676f, 0x6771); A = 0x03;
  CYC(0x6771, 0x6772); mem_wr(gb, DE, A);
  CALL_C(0x6772, func_693b_hook, 0x693b, 0x6775);
  CALL_C(0x6775, objectGetRelativeAngle_hook, 0x1ea4, 0x6778);
  CYC(0x6778, 0x677a); E = 0xc9;
  CYC(0x677a, 0x677b); mem_wr(gb, DE, A);
  RET(0x677b); return; // ret

state2:
  CYC(0x677c, 0x677d); E = alu_inc8(gb, E);
  CYC(0x677d, 0x677e); A = mem_rd(gb, DE);
  {
    CYC(0x677e, 0x677f); push_effect(gb, 0x677f);
    uint16_t target = ramrockGloveFormArm_jump_table(gb);
    if (target == 0x6787) goto state2_substate0;
    if (target == 0x67b7) goto state2_substate1;
    goto state2_substate2; // substate2 and substate3 both target 0x67b0
  }

state2_substate0:
  CYC(0x6787, 0x6788); H = D;
  CYC(0x6788, 0x6789); L = E;
  CYC(0x6789, 0x678a); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x678a, 0x678c); A = 0x90;
  CYC(0x678c, 0x678f); mem_wr(gb, 0xcc5b, A); // wLinkGrabState2
  CYC(0x678f, 0x6790); alu_xor(gb, A);
  CYC(0x6790, 0x6792); L = 0xca;
  CYC(0x6792, 0x6793); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(0x6793, 0x6796); mem_wr(gb, 0xd00a, A);
  CYC(0x6796, 0x6798); mem_wr(gb, HL, 0x10);
  CYC(0x6798, 0x679a); L = 0xd0;
  CYC(0x679a, 0x679c); mem_wr(gb, HL, 0x14);
  CYC(0x679c, 0x679e); L = 0xc7;
  CYC(0x679e, 0x67a0); mem_wr(gb, HL, 0x60);
  CALL_C(0x67a0, func_69a5_hook, 0x69a5, 0x67a3);
  CYC(0x67a3, 0x67a5); L = 0xb7;
  CYC(0x67a5, 0x67a7); E = 0xc2;
  CYC(0x67a7, 0x67a8); A = mem_rd(gb, DE);
  CYC(0x67a8, 0x67aa); A = alu_swap(gb, A);
  CYC(0x67aa, 0x67ad); unsetFlag_hook(gb); return; // jp

state2_dropLinkHeldItem:
  CALL_C(0x67ad, dropLinkHeldItem_hook, 0x2c43, 0x67b0);
state2_substate2:
  CYC(0x67b0, 0x67b2); A = 0xb1; // SND_BIGSWORD
  CALL_C(0x67b2, playSound_b00_hook, 0x0c98, 0x67b5);
  CYC(0x67b5, 0x67b7); goto func_675e; // jr

state2_substate1:
  CALL_C(0x67b7, func_69a5_hook, 0x69a5, 0x67ba);
  CYC(0x67ba, 0x67bb); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x67bb, 0x67bd); alu_cp(gb, 0x11);
  if (F & FZ) { CYCT(0x67bd, 0x67bf); goto state2_dropLinkHeldItem; } // jr z
  CYC(0x67bd, 0x67bf);
  CYC(0x67bf, 0x67c2); A = mem_rd(gb, 0xd221);
  CYC(0x67c2, 0x67c3); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x67c3, 0x67c5); goto state2func_67c9; } // jr nz
  CYC(0x67c3, 0x67c5);
  CYC(0x67c5, 0x67c7); E = 0xf3;
  CYC(0x67c7, 0x67c8); mem_wr(gb, DE, A);
  RET(0x67c8); return; // ret

state2func_67c9:
  CYC(0x67c9, 0x67ca); H = D;
  CYC(0x67ca, 0x67cc); L = 0xc7;
  CYC(0x67cc, 0x67cd); A = mem_rd(gb, HL);
  CYC(0x67cd, 0x67ce); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(0x67ce); return; } // ret z
  CYC(0x67ce, 0x67cf);
  CYC(0x67cf, 0x67d0); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(0x67d0, 0x67d2); goto L_67d8; } // jr nz
  CYC(0x67d0, 0x67d2);
  CYC(0x67d2, 0x67d3); L = alu_dec8(gb, L);
  CYC(0x67d3, 0x67d5); mem_wr(gb, HL, 0x14);
  CYC(0x67d5, 0x67d7); L = 0xf2;
  CYC(0x67d7, 0x67d8); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_67d8:
  CYC(0x67d8, 0x67da); L = 0xf3;
  CYC(0x67da, 0x67db); A = mem_rd(gb, HL);
  CYC(0x67db, 0x67dc); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x67dc, 0x67de); goto L_67e4; } // jr nz
  CYC(0x67dc, 0x67de);
  CYC(0x67de, 0x67e0); A = 0x71; // SND_MOVEBLOCK
  CYC(0x67e0, 0x67e1); mem_wr(gb, HL, A);
  CALL_C(0x67e1, playSound_b00_hook, 0x0c98, 0x67e4);
L_67e4:
  CYC(0x67e4, 0x67e5); H = D;
  CYC(0x67e5, 0x67e7); L = 0xc9;
  CYC(0x67e7, 0x67e8); C = mem_rd(gb, HL);
  CYC(0x67e8, 0x67ea); L = 0xd0;
  CYC(0x67ea, 0x67eb); B = mem_rd(gb, HL);
  CALL_C(0x67eb, updateLinkPositionGivenVelocity_hook, 0x231e, 0x67ee);
  CYC(0x67ee, 0x67f1); objectApplySpeed_hook(gb); return; // jp

subidBit7SetArm_state2:
  CYC(0x67f1, 0x67f3); A = 0x0b;
  CALL_C(0x67f3, objectGetRelatedObject1Var_hook, 0x2160, 0x67f6);
  CYC(0x67f6, 0x67f8); E = 0xcb;
  CYC(0x67f8, 0x67f9); A = mem_rd(gb, DE);
  CYC(0x67f9, 0x67fa); alu_sub(gb, mem_rd(gb, HL));
  CYC(0x67fa, 0x67fb); alu_cpl(gb);
  CYC(0x67fb, 0x67fc); A = alu_inc8(gb, A);
  CYC(0x67fc, 0x67fe); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(0x67fe, 0x6800); goto L_6803; } // jr c
  CYC(0x67fe, 0x6800);
  CYC(0x6800, 0x6801); A = mem_rd(gb, DE);
  CYC(0x6801, 0x6802); A = alu_inc8(gb, A);
  CYC(0x6802, 0x6803); mem_wr(gb, DE, A);
L_6803:
  CYC(0x6803, 0x6805); A = 0x04;
  CALL_C(0x6805, objectGetRelatedObject1Var_hook, 0x2160, 0x6808);
  CYC(0x6808, 0x6809); A = mem_rd(gb, HL);
  CYC(0x6809, 0x680b); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(0x680b); return; } // ret z
  CYC(0x680b, 0x680c);
  CYC(0x680c, 0x680f); goto func_675e; // jp

state3:
  CYC(0x680f, 0x6811); E = 0xc6;
  CYC(0x6811, 0x6812); A = mem_rd(gb, DE);
  CYC(0x6812, 0x6813); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6813, 0x6815); goto state3func_681a; } // jr z
  CYC(0x6813, 0x6815);
  CYC(0x6815, 0x6816); A = alu_dec8(gb, A);
  CYC(0x6816, 0x6817); mem_wr(gb, DE, A);
  CYC(0x6817, 0x681a); objectApplySpeed_hook(gb); return; // jp

state3func_681a:
  CALL_C(0x681a, func_693b_hook, 0x693b, 0x681d);
  CALL_C(0x681d, objectGetRelativeAngle_hook, 0x1ea4, 0x6820);
  CYC(0x6820, 0x6822); E = 0xc9;
  CYC(0x6822, 0x6823); mem_wr(gb, DE, A);
  CALL_C(0x6823, objectApplySpeed_hook, 0x201d, 0x6826);
  CALL_C(0x6826, state3func_6970_hook, 0x6970, 0x6829);
  if (!(F & FZ)) { RET_TAKEN(0x6829); return; } // ret nz
  CYC(0x6829, 0x682a);
  CYC(0x682a, 0x682c); E = 0xc2;
  CYC(0x682c, 0x682d); A = mem_rd(gb, DE);
  CYC(0x682d, 0x682e); alu_rlca(gb);
  if (F & FC) { CYCT(0x682e, 0x6830); goto state3func_6864; } // jr c
  CYC(0x682e, 0x6830);
  CYC(0x6830, 0x6831); H = D;
  CYC(0x6831, 0x6833); L = 0xe4;
  CYC(0x6833, 0x6835); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x6835, 0x6837); E = 0xf2;
  CYC(0x6837, 0x6838); A = mem_rd(gb, DE);
  CYC(0x6838, 0x6839); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6839, 0x683b); goto L_6850; } // jr z
  CYC(0x6839, 0x683b);
  CYC(0x683b, 0x683c); alu_xor(gb, A);
  CYC(0x683c, 0x683d); mem_wr(gb, DE, A);
  CALL_C(0x683d, func_69a5_hook, 0x69a5, 0x6840);
  CYC(0x6840, 0x6842); L = 0xab;
  CYC(0x6842, 0x6843); A = mem_rd(gb, HL);
  CYC(0x6843, 0x6844); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6844, 0x6846); goto L_6850; } // jr nz
  CYC(0x6844, 0x6846);
  CYC(0x6846, 0x6848); mem_wr(gb, HL, 0x3c);
  CYC(0x6848, 0x684a); L = 0xb5;
  CYC(0x684a, 0x684b); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x684b, 0x684d); A = 0x63; // SND_BOSS_DAMAGE
  CALL_C(0x684d, playSound_b00_hook, 0x0c98, 0x6850);
L_6850:
  CYC(0x6850, 0x6852); E = 0xc6;
  CYC(0x6852, 0x6854); A = 0x3c;
  CYC(0x6854, 0x6855); mem_wr(gb, DE, A);
  CALL_C(0x6855, func_69a5_hook, 0x69a5, 0x6858);
  CYC(0x6858, 0x685a); L = 0xb7;
  CYC(0x685a, 0x685c); E = 0xc2;
  CYC(0x685c, 0x685d); A = mem_rd(gb, DE);
  CYC(0x685d, 0x685f); A = alu_swap(gb, A);
  CALL_C(0x685f, setFlag_hook, 0x020e, 0x6862);
  CYC(0x6862, 0x6864); goto L_6867; // jr

state3func_6864:
  CALL_C(0x6864, objectSetInvisible_hook, 0x1e7b, 0x6867);
L_6867:
  CYC(0x6867, 0x6869); E = 0xc4;
  CYC(0x6869, 0x686b); A = 0x04;
  CYC(0x686b, 0x686c); mem_wr(gb, DE, A);
  RET(0x686c); return; // ret

state4:
  CYC(0x686d, 0x686e); H = D;
  CYC(0x686e, 0x6870); L = 0xe4;
  CYC(0x6870, 0x6872); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(0x6872, 0x6875);
  CALL_C(0x68d7, func_693b_hook, 0x693b, 0x68da);
  CYC(0x68da, 0x68db); H = D;
  CYC(0x68db, 0x68dd); L = 0xcb;
  CYC(0x68dd, 0x68de); mem_wr(gb, HL, B);
  CYC(0x68de, 0x68e0); L = 0xcd;
  CYC(0x68e0, 0x68e1); mem_wr(gb, HL, C);
  CYC(0x68e1, 0x68e2); // ret (returns to 0x6875, no continuation check needed here)
  CALL_C(0x6875, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x6878);
  if (!(F & FZ)) { RET_TAKEN(0x6878); return; } // ret nz
  CYC(0x6878, 0x6879);
  CALL_C(0x6879, func_69a5_hook, 0x69a5, 0x687c);
  CYC(0x687c, 0x687d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x687d, 0x687f); alu_cp(gb, 0x12);
  if (!(F & FZ)) { RET_TAKEN(0x687f); return; } // ret nz
  CYC(0x687f, 0x6880);
  CYC(0x6880, 0x6881); A = mem_rd(gb, HL);
  CYC(0x6881, 0x6883); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(0x6883, 0x6885); goto state4func_689e; } // jr nz
  CYC(0x6883, 0x6885);
  CYC(0x6885, 0x6887); E = 0xc2;
  CYC(0x6887, 0x6888); A = mem_rd(gb, DE);
  CYC(0x6888, 0x6889); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x6889, 0x688b); goto state4func_689e; } // jr z
  CYC(0x6889, 0x688b);
  CALL_C(0x688b, objectGetAngleTowardLink_hook, 0x1e9c, 0x688e);
  CYC(0x688e, 0x6890); alu_cp(gb, 0x10);
  if (!(F & FZ)) { RET_TAKEN(0x6890); return; } // ret nz
  CYC(0x6890, 0x6891);
  CYC(0x6891, 0x6894); A = mem_rd(gb, 0xd008); // w1Link.direction
  CYC(0x6894, 0x6895); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(0x6895); return; } // ret nz
  CYC(0x6895, 0x6896);
  CYC(0x6896, 0x6897); H = D;
  CYC(0x6897, 0x6899); L = 0xe4;
  CYC(0x6899, 0x689b); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(0x689b, 0x689e); objectAddToGrabbableObjectBuffer_hook(gb); return; // jp

state4func_689e:
  CYC(0x689e, 0x68a0); A = 0x6f; // SND_EXPLOSION
  CALL_C(0x68a0, playSound_b00_hook, 0x0c98, 0x68a3);
  CALL_C(0x68a3, objectGetAngleTowardLink_hook, 0x1e9c, 0x68a6);
  CYC(0x68a6, 0x68a7); H = D;
  CYC(0x68a7, 0x68a9); L = 0xc9;
  CYC(0x68a9, 0x68aa); mem_wr(gb, HL, A);
  CYC(0x68aa, 0x68ac); L = 0xc4;
  CYC(0x68ac, 0x68ae); mem_wr(gb, HL, 0x05);
  CYC(0x68ae, 0x68b0); L = 0xc6;
  CYC(0x68b0, 0x68b2); mem_wr(gb, HL, 0x02);
  CYC(0x68b2, 0x68b4); L = 0xd0;
  CYC(0x68b4, 0x68b6); mem_wr(gb, HL, 0x78);
  CALL_C(0x68b6, func_69a5_hook, 0x69a5, 0x68b9);
  CYC(0x68b9, 0x68bb); L = 0xb7;
  CYC(0x68bb, 0x68bd); E = 0xc2;
  CYC(0x68bd, 0x68be); A = mem_rd(gb, DE);
  CYC(0x68be, 0x68c0); A = alu_swap(gb, A);
  CYC(0x68c0, 0x68c3); unsetFlag_hook(gb); return; // jp

subidBit7SetArm_state4:
  CYC(0x68c3, 0x68c5); A = 0x04;
  CALL_C(0x68c5, objectGetRelatedObject1Var_hook, 0x2160, 0x68c8);
  CYC(0x68c8, 0x68c9); A = mem_rd(gb, HL);
  CYC(0x68c9, 0x68cb); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(0x68cb, 0x68cd); goto state4func_68d7; } // jr z
  CYC(0x68cb, 0x68cd);
  CYC(0x68cd, 0x68ce); E = L;
  CYC(0x68ce, 0x68cf); mem_wr(gb, DE, A);
  CYC(0x68cf, 0x68d1); L = 0xc9;
  CYC(0x68d1, 0x68d2); E = L;
  CYC(0x68d2, 0x68d3); A = mem_rd(gb, HL);
  CYC(0x68d3, 0x68d4); mem_wr(gb, DE, A);
  CYC(0x68d4, 0x68d7); objectSetVisible_hook(gb); return; // jp

state4func_68d7:
  CALL_C(0x68d7, func_693b_hook, 0x693b, 0x68da);
  CYC(0x68da, 0x68db); H = D;
  CYC(0x68db, 0x68dd); L = 0xcb;
  CYC(0x68dd, 0x68de); mem_wr(gb, HL, B);
  CYC(0x68de, 0x68e0); L = 0xcd;
  CYC(0x68e0, 0x68e1); mem_wr(gb, HL, C);
  RET(0x68e1); return; // ret

state5:
  CALL_C(0x68e2, partCommon_getTileCollisionInFront_hook, 0x4000, 0x68e5);
  if (!(F & FZ)) { CYCT(0x68e5, 0x68e7); goto state5func_68fe; } // jr nz
  CYC(0x68e5, 0x68e7);
  CALL_C(0x68e7, objectApplySpeed_hook, 0x201d, 0x68ea);
  CALL_C(0x68ea, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x68ed);
  if (!(F & FZ)) { RET_TAKEN(0x68ed); return; } // ret nz
  CYC(0x68ed, 0x68ee);
  CYC(0x68ee, 0x68f0); mem_wr(gb, HL, 0x03);
  CYC(0x68f0, 0x68f2); E = 0xd0;
  CYC(0x68f2, 0x68f3); A = mem_rd(gb, DE);
  CYC(0x68f3, 0x68f4); alu_or(gb, A);
  if (F & FZ) { CYCT(0x68f4, 0x68f7); goto state5func_68fe; } // jp z
  CYC(0x68f4, 0x68f7);
  CYC(0x68f7, 0x68f9); alu_sub(gb, 0x0a);
  if (!(F & FC)) { CYCT(0x68f9, 0x68fb); goto L_68fc; } // jr nc
  CYC(0x68f9, 0x68fb);
  CYC(0x68fb, 0x68fc); alu_xor(gb, A);
L_68fc:
  CYC(0x68fc, 0x68fd); mem_wr(gb, DE, A);
  RET(0x68fd); return; // ret

state5func_68fe:
  CYC(0x68fe, 0x68ff); H = D;
  CYC(0x68ff, 0x6901); L = 0xc4;
  CYC(0x6901, 0x6903); mem_wr(gb, HL, 0x06);
  CYC(0x6903, 0x6905); L = 0xc6;
  CYC(0x6905, 0x6907); mem_wr(gb, HL, 0x3c);
  CYC(0x6907, 0x6909); L = 0xd0;
  CYC(0x6909, 0x690b); mem_wr(gb, HL, 0x00);
  RET(0x690b); return; // ret

subidBit7SetArm_state5:
  CYC(0x690c, 0x690e); A = 0x10;
  CALL_C(0x690e, objectGetRelatedObject1Var_hook, 0x2160, 0x6911);
  CYC(0x6911, 0x6912); E = L;
  CYC(0x6912, 0x6913); A = mem_rd(gb, HL);
  CYC(0x6913, 0x6915); alu_sub(gb, 0x19);
  if (!(F & FC)) { CYCT(0x6915, 0x6917); goto L_6918; } // jr nc
  CYC(0x6915, 0x6917);
  CYC(0x6917, 0x6918); alu_xor(gb, A);
L_6918:
  CYC(0x6918, 0x6919); mem_wr(gb, DE, A);
  CYC(0x6919, 0x691b); L = 0xc4;
  CYC(0x691b, 0x691c); A = mem_rd(gb, HL);
  CYC(0x691c, 0x691e); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x691e, 0x6921); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(0x691e, 0x6921);
  CYC(0x6921, 0x6924); goto func_6762; // jp

state6:
  CALL_C(0x6924, partCommon_decCounter1IfNonzero_hook, 0x40a7, 0x6927);
  if (!(F & FZ)) { RET_TAKEN(0x6927); return; } // ret nz
  CYC(0x6927, 0x6928);
  CYC(0x6928, 0x692a); L = 0xe4;
  CYC(0x692a, 0x692c); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CALL_C(0x692c, func_69a5_hook, 0x69a5, 0x692f);
  CYC(0x692f, 0x6930); L = alu_inc8(gb, L);
  CYC(0x6930, 0x6931); A = mem_rd(gb, HL);
  CYC(0x6931, 0x6933); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(0x6933, 0x6935); goto L_6938; } // jr z
  CYC(0x6933, 0x6935);
  CYC(0x6935, 0x6937); A = 0x80;
  CYC(0x6937, 0x6938); mem_wr(gb, HL, A);
L_6938:
  CYC(0x6938, 0x693b); goto func_6762; // jp
}

void func_693b_hook(GB *gb) {
  CYC(0x693b, 0x693d); E = 0xc2; // Part.subid
  CYC(0x693d, 0x693e); A = mem_rd(gb, DE);
  CYC(0x693e, 0x6940); A = alu_swap(gb, A);
  CYC(0x6940, 0x6942); C = 0x0c;
  CYC(0x6942, 0x6943); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(0x6943, 0x6945);
  } else {
    CYC(0x6943, 0x6945);
    CYC(0x6945, 0x6947); C = 0xf4;
  }
  CYC(0x6947, 0x6949); E = 0xf0;
  CYC(0x6949, 0x694a); A = mem_rd(gb, DE);
  CYC(0x694a, 0x694b); H = A;
  CYC(0x694b, 0x694d); L = 0x8b;
  CYC(0x694d, 0x694e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x694e, 0x6950); alu_add(gb, 0x0c);
  CYC(0x6950, 0x6951); B = A;
  CYC(0x6951, 0x6952); L = alu_inc8(gb, L);
  CYC(0x6952, 0x6953); A = mem_rd(gb, HL);
  CYC(0x6953, 0x6954); alu_add(gb, C);
  CYC(0x6954, 0x6955); C = A;
  RET(0x6955); return;
}

void state0func_6956_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6956, 0x6958); E = 0xc2; // Part.subid
  CYC(0x6958, 0x6959); A = mem_rd(gb, DE);
  CYC(0x6959, 0x695b); alu_and(gb, 0x0f);
  CYC(0x695b, 0x695d); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(0x695d); return; } // ret z
  CYC(0x695d, 0x695e);
  CALL_C(0x695e, getFreePartSlot_hook, 0x3e8e, 0x6961);
  CYC(0x6961, 0x6963); A = 0x35; // PART_RAMROCK_GLOVE_FORM_ARM
  CYC(0x6963, 0x6964); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6964, 0x6965); A = mem_rd(gb, DE);
  CYC(0x6965, 0x6966); A = alu_inc8(gb, A);
  CYC(0x6966, 0x6967); mem_wr(gb, HL, A);
  CYC(0x6967, 0x6969); E = 0xf0;
  CYC(0x6969, 0x696a); L = E;
  CYC(0x696a, 0x696b); A = mem_rd(gb, DE);
  CYC(0x696b, 0x696c); mem_wr(gb, HL, A);
  CYC(0x696c, 0x696e); L = 0xc2;
  CYC(0x696e, 0x696f); E = L;
  RET(0x696f); return;
}

void state3func_6970_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6970, func_693b_hook, 0x693b, 0x6973);
  CYC(0x6973, 0x6975); E = 0x03;
  CYC(0x6975, 0x6976); H = D;
  CYC(0x6976, 0x6978); L = 0xcb;
  CYC(0x6978, 0x6979); A = E;
  CYC(0x6979, 0x697a); alu_add(gb, B);
  CYC(0x697a, 0x697b); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x697b, 0x697d); goto ret_6990; } // jr c
  CYC(0x697b, 0x697d);
  CYC(0x697d, 0x697e); alu_sub(gb, E);
  CYC(0x697e, 0x697f); alu_sub(gb, E);
  CYC(0x697f, 0x6980); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x6980, 0x6982); goto ret_6990; } // jr nc
  CYC(0x6980, 0x6982);
  CYC(0x6982, 0x6984); L = 0xcd;
  CYC(0x6984, 0x6985); A = E;
  CYC(0x6985, 0x6986); alu_add(gb, C);
  CYC(0x6986, 0x6987); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(0x6987, 0x6989); goto ret_6990; } // jr c
  CYC(0x6987, 0x6989);
  CYC(0x6989, 0x698a); alu_sub(gb, E);
  CYC(0x698a, 0x698b); alu_sub(gb, E);
  CYC(0x698b, 0x698c); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(0x698c, 0x698e); goto ret_6990; } // jr nc
  CYC(0x698c, 0x698e);
  CYC(0x698e, 0x698f); alu_xor(gb, A);
  RET(0x698f); return;
ret_6990:
  CYC(0x6990, 0x6991); alu_or(gb, D);
  RET(0x6991); return;
}

void state0func_6992_hook(GB *gb) {
  CYC(0x6992, 0x6993); push_effect(gb, HL);
  CYC(0x6993, 0x6994); A = mem_rd(gb, HL);
  CYC(0x6994, 0x6996); alu_and(gb, 0x10);
  CYC(0x6996, 0x6998); A = alu_swap(gb, A);
  CYC(0x6998, 0x699b); SET_HL(0x69a3);
  CYC(0x699b, 0x699c); ramrockGloveFormArm_addAToHl_from_rst(gb, 0x699c);
  CYC(0x699c, 0x699d); C = mem_rd(gb, HL);
  CYC(0x699d, 0x699f); B = 0xfc;
  CYC(0x699f, 0x69a0); SET_HL(pop_effect(gb));
  CYC(0x69a0, 0x69a3); objectCopyPositionWithOffset_hook(gb); return; // jp
}

void func_69a5_hook(GB *gb) {
  CYC(0x69a5, 0x69a7); E = 0xf0;
  CYC(0x69a7, 0x69a8); A = mem_rd(gb, DE);
  CYC(0x69a8, 0x69a9); H = A;
  CYC(0x69a9, 0x69ab); L = 0x82;
  RET(0x69ab); return;
}
