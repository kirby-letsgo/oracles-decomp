#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

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
  BASE(partCode35);
  CYC(b_+171, b_+173); A = 0x01;
  CYC(b_+173, b_+174); mem_wr(gb, DE, A);
  CYC(b_+174, b_+176); E = 0xcf;
  CYC(b_+176, b_+178); A = 0x81;
  CYC(b_+178, b_+179); mem_wr(gb, DE, A);
  CYC(b_+179, b_+180);
}

void partCode35_hook(GB *gb);
void func_693b_hook(GB *gb);
void state0func_6956_hook(GB *gb);
void state3func_6970_hook(GB *gb);
void state0func_6992_hook(GB *gb);
void func_69a5_hook(GB *gb);

void partCode35_hook(GB *gb) {
  BASE(partCode35);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x29; // Object.health
  CALL_C(b_+2, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+5);
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+7, b_+10); partDelete_hook(gb); return; } // jp z
  CYC(b_+7, b_+10);
  CYC(b_+10, b_+12); E = 0xc2; // Part.subid
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); alu_rlca(gb);
  if (F & FC) { CYCT(b_+14, b_+16); goto subidBit7SetArm; } // jr c
  CYC(b_+14, b_+16);
  CYC(b_+16, b_+19); A = mem_rd(gb, wLinkGrabState); // wLinkGrabState
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (F & FZ) {
    CALL_C_CC(b_+20, objectPushLinkAwayOnCollision_hook, SYM(objectPushLinkAwayOnCollision), b_+23);
  } else {
    CYC(b_+20, b_+23);
  }
  CYC(b_+23, b_+25); E = 0xc4; // Part.state
  CYC(b_+25, b_+26); A = mem_rd(gb, DE);
  {
    CYC(b_+26, b_+27); push_effect(gb, b_+27);
    uint16_t target = ramrockGloveFormArm_jump_table(gb);
    if (target == b_+76) goto state0;
    if (target == b_+180) goto state1;
    if (target == b_+246) goto state2;
    if (target == b_+393) goto state3;
    if (target == b_+487) goto state4;
    if (target == b_+604) goto state5;
    goto state6;
  }

subidBit7SetArm:
  CYC(b_+41, b_+43); E = 0xc6;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+45); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+45, b_+47); goto L_66c2; } // jr nz
  CYC(b_+45, b_+47);
  CYC(b_+47, b_+49); E = 0xc4;
  CYC(b_+49, b_+50); A = mem_rd(gb, DE);
  CYC(b_+50, b_+52); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+52, b_+54); goto L_66c2; } // jr z
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); E = 0xda;
  CYC(b_+56, b_+57); A = mem_rd(gb, DE);
  CYC(b_+57, b_+59); alu_xor(gb, 0x80);
  CYC(b_+59, b_+60); mem_wr(gb, DE, A);
L_66c2:
  CYC(b_+60, b_+62); E = 0xc4;
  CYC(b_+62, b_+63); A = mem_rd(gb, DE);
  {
    CYC(b_+63, b_+64); push_effect(gb, b_+64);
    uint16_t target = ramrockGloveFormArm_jump_table(gb);
    if (target == b_+127) goto subidBit7SetArm_state0;
    if (target == b_+193) goto subidBit7SetArm_state1;
    if (target == b_+363) goto subidBit7SetArm_state2;
    if (target == b_+393) goto state3;
    if (target == b_+573) goto subidBit7SetArm_state4;
    goto subidBit7SetArm_state5;
  }

state0:
  CYC(b_+76, b_+79); ramrock35_state0func_6731(gb);
  CYC(b_+79, b_+81); E = 0xd7;
  CYC(b_+81, b_+82); A = mem_rd(gb, DE);
  CYC(b_+82, b_+84); E = 0xf0;
  CYC(b_+84, b_+85); mem_wr(gb, DE, A);
  CALL_C(b_+85, state0func_6956_hook, SYM(state0func_6956), b_+88);
  CYC(b_+88, b_+89); A = mem_rd(gb, DE);
  CYC(b_+89, b_+91); A = alu_swap(gb, A);
  CYC(b_+91, b_+92); mem_wr(gb, DE, A);
  CYC(b_+92, b_+94); alu_or(gb, 0x80);
  CYC(b_+94, b_+95); mem_wr(gb, HL, A);
  CALL_C(b_+95, state0func_6992_hook, SYM(state0func_6992), b_+98);
  CYC(b_+98, b_+100); L = 0xd6;
  CYC(b_+100, b_+102); A = 0xc0;
  CYC(b_+102, b_+103); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+103, b_+104); mem_wr(gb, HL, D);
  CYC(b_+104, b_+106); E = 0xc2;
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+109); A = alu_swap(gb, A);
  CYC(b_+109, b_+112); SET_HL(b_+125);
  CYC(b_+112, b_+113); ramrockGloveFormArm_addAToHl_from_rst(gb, b_+113);
  CYC(b_+113, b_+114); A = mem_rd(gb, HL);
  CYC(b_+114, b_+116); E = 0xc9;
  CYC(b_+116, b_+117); mem_wr(gb, DE, A);
  CYC(b_+117, b_+119); A = 0x51; // SND_THROW
  CALL_C(b_+119, playSound_b00_hook, SYM(playSound_b00), b_+122);
  CYC(b_+122, b_+125); TAIL(objectSetVisiblec0); // jp

subidBit7SetArm_state0:
  CYC(b_+127, b_+130); ramrock35_state0func_6731(gb);
  CALL_C(b_+130, state0func_6956_hook, SYM(state0func_6956), b_+133);
  CALL_C(b_+133, state0func_6992_hook, SYM(state0func_6992), b_+136);
  CYC(b_+136, b_+138); L = 0xd6;
  CYC(b_+138, b_+140); A = 0xc0;
  CYC(b_+140, b_+141); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+141, b_+142); mem_wr(gb, HL, D);
  CYC(b_+142, b_+144); L = 0xf0;
  CYC(b_+144, b_+145); E = L;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+147); mem_wr(gb, HL, A);
  CYC(b_+147, b_+149); A = 0x01;
  CALL_C(b_+149, partSetAnimation_hook, SYM(partSetAnimation), b_+152);
  CYC(b_+152, b_+154); E = 0xc2;
  CYC(b_+154, b_+155); A = mem_rd(gb, DE);
  CYC(b_+155, b_+157); alu_and(gb, 0x0f);
  CYC(b_+157, b_+159); alu_add(gb, 0x0a);
  CYC(b_+159, b_+161); E = 0xc6;
  CYC(b_+161, b_+162); mem_wr(gb, DE, A);
  CYC(b_+162, b_+164); E = 0xe4;
  CYC(b_+164, b_+165); A = mem_rd(gb, DE);
  CYC(b_+165, b_+167); A = (uint8_t)(A & ~(1 << 7));
  CYC(b_+167, b_+168); mem_wr(gb, DE, A);
  CYC(b_+168, b_+171); TAIL(objectSetVisiblec1); // jp

state1:
  CYC(b_+180, b_+182); C = 0x10;
  CALL_C(b_+182, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+185);
  if (!(F & FZ)) { RET_TAKEN(b_+185); return; } // ret nz
  CYC(b_+185, b_+186);
  CYC(b_+186, b_+188); E = 0xf1;
  CYC(b_+188, b_+189); A = mem_rd(gb, DE);
  CYC(b_+189, b_+190); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+190, b_+192); goto func_675e; } // jr nz
  CYC(b_+190, b_+192);
  RET(b_+192); return; // ret

subidBit7SetArm_state1:
  CALL_C(b_+193, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+196);
  if (!(F & FZ)) { RET_TAKEN(b_+196); return; } // ret nz
  CYC(b_+196, b_+197);
  CYC(b_+197, b_+199); C = 0x10;
  CALL_C(b_+199, objectUpdateSpeedZ_paramC_hook, SYM(objectUpdateSpeedZ_paramC), b_+202);
  if (!(F & FZ)) { RET_TAKEN(b_+202); return; } // ret nz
  CYC(b_+202, b_+203);
  CYC(b_+203, b_+205); L = 0xc7;
  CYC(b_+205, b_+206); A = mem_rd(gb, HL);
  CYC(b_+206, b_+207); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+207, b_+209); goto func_675e; } // jr nz
  CYC(b_+207, b_+209);
  CYC(b_+209, b_+210); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+210, b_+213); SET_BC(0xfe80);
  CYC(b_+213, b_+216); TAIL(objectSetSpeedZ); // jp

func_675e:
  CYC(b_+216, b_+218); A = 0x78;
  CYC(b_+218, b_+220); goto L_6764; // jr

func_6762:
  CYC(b_+220, b_+222); A = 0x14;
L_6764:
  CYC(b_+222, b_+224); E = 0xd0;
  CYC(b_+224, b_+225); mem_wr(gb, DE, A);
  CYC(b_+225, b_+227); A = 0x31;
  CALL_C(b_+227, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+230);
  CYC(b_+230, b_+231); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+231, b_+233); E = 0xc4;
  CYC(b_+233, b_+235); A = 0x03;
  CYC(b_+235, b_+236); mem_wr(gb, DE, A);
  CALL_C(b_+236, func_693b_hook, SYM(func_693b), b_+239);
  CALL_C(b_+239, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+242);
  CYC(b_+242, b_+244); E = 0xc9;
  CYC(b_+244, b_+245); mem_wr(gb, DE, A);
  RET(b_+245); return; // ret

state2:
  CYC(b_+246, b_+247); E = alu_inc8(gb, E);
  CYC(b_+247, b_+248); A = mem_rd(gb, DE);
  {
    CYC(b_+248, b_+249); push_effect(gb, b_+249);
    uint16_t target = ramrockGloveFormArm_jump_table(gb);
    if (target == b_+257) goto state2_substate0;
    if (target == b_+305) goto state2_substate1;
    goto state2_substate2; // substate2 and substate3 both target 0x67b0
  }

state2_substate0:
  CYC(b_+257, b_+258); H = D;
  CYC(b_+258, b_+259); L = E;
  CYC(b_+259, b_+260); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+260, b_+262); A = 0x90;
  CYC(b_+262, b_+265); mem_wr(gb, wLinkGrabState2, A); // wLinkGrabState2
  CYC(b_+265, b_+266); alu_xor(gb, A);
  CYC(b_+266, b_+268); L = 0xca;
  CYC(b_+268, b_+269); mem_wr(gb, HL, A); SET_HL(HL - 1);
  CYC(b_+269, b_+272); mem_wr(gb, w1Link_y, A);
  CYC(b_+272, b_+274); mem_wr(gb, HL, 0x10);
  CYC(b_+274, b_+276); L = 0xd0;
  CYC(b_+276, b_+278); mem_wr(gb, HL, 0x14);
  CYC(b_+278, b_+280); L = 0xc7;
  CYC(b_+280, b_+282); mem_wr(gb, HL, 0x60);
  CALL_C(b_+282, func_69a5_hook, SYM(func_69a5), b_+285);
  CYC(b_+285, b_+287); L = 0xb7;
  CYC(b_+287, b_+289); E = 0xc2;
  CYC(b_+289, b_+290); A = mem_rd(gb, DE);
  CYC(b_+290, b_+292); A = alu_swap(gb, A);
  CYC(b_+292, b_+295); TAIL(unsetFlag); // jp

state2_dropLinkHeldItem:
  CALL_C(b_+295, dropLinkHeldItem_hook, SYM(dropLinkHeldItem), b_+298);
state2_substate2:
  CYC(b_+298, b_+300); A = 0xb1; // SND_BIGSWORD
  CALL_C(b_+300, playSound_b00_hook, SYM(playSound_b00), b_+303);
  CYC(b_+303, b_+305); goto func_675e; // jr

state2_substate1:
  CALL_C(b_+305, func_69a5_hook, SYM(func_69a5), b_+308);
  CYC(b_+308, b_+309); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+309, b_+311); alu_cp(gb, 0x11);
  if (F & FZ) { CYCT(b_+311, b_+313); goto state2_dropLinkHeldItem; } // jr z
  CYC(b_+311, b_+313);
  CYC(b_+313, b_+316); A = mem_rd(gb, w1ParentItem2_animParameter);
  CYC(b_+316, b_+317); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+317, b_+319); goto state2func_67c9; } // jr nz
  CYC(b_+317, b_+319);
  CYC(b_+319, b_+321); E = 0xf3;
  CYC(b_+321, b_+322); mem_wr(gb, DE, A);
  RET(b_+322); return; // ret

state2func_67c9:
  CYC(b_+323, b_+324); H = D;
  CYC(b_+324, b_+326); L = 0xc7;
  CYC(b_+326, b_+327); A = mem_rd(gb, HL);
  CYC(b_+327, b_+328); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+328); return; } // ret z
  CYC(b_+328, b_+329);
  CYC(b_+329, b_+330); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+330, b_+332); goto L_67d8; } // jr nz
  CYC(b_+330, b_+332);
  CYC(b_+332, b_+333); L = alu_dec8(gb, L);
  CYC(b_+333, b_+335); mem_wr(gb, HL, 0x14);
  CYC(b_+335, b_+337); L = 0xf2;
  CYC(b_+337, b_+338); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_67d8:
  CYC(b_+338, b_+340); L = 0xf3;
  CYC(b_+340, b_+341); A = mem_rd(gb, HL);
  CYC(b_+341, b_+342); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+342, b_+344); goto L_67e4; } // jr nz
  CYC(b_+342, b_+344);
  CYC(b_+344, b_+346); A = 0x71; // SND_MOVEBLOCK
  CYC(b_+346, b_+347); mem_wr(gb, HL, A);
  CALL_C(b_+347, playSound_b00_hook, SYM(playSound_b00), b_+350);
L_67e4:
  CYC(b_+350, b_+351); H = D;
  CYC(b_+351, b_+353); L = 0xc9;
  CYC(b_+353, b_+354); C = mem_rd(gb, HL);
  CYC(b_+354, b_+356); L = 0xd0;
  CYC(b_+356, b_+357); B = mem_rd(gb, HL);
  CALL_C(b_+357, updateLinkPositionGivenVelocity_hook, SYM(updateLinkPositionGivenVelocity), b_+360);
  CYC(b_+360, b_+363); TAIL(objectApplySpeed); // jp

subidBit7SetArm_state2:
  CYC(b_+363, b_+365); A = 0x0b;
  CALL_C(b_+365, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+368);
  CYC(b_+368, b_+370); E = 0xcb;
  CYC(b_+370, b_+371); A = mem_rd(gb, DE);
  CYC(b_+371, b_+372); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+372, b_+373); alu_cpl(gb);
  CYC(b_+373, b_+374); A = alu_inc8(gb, A);
  CYC(b_+374, b_+376); alu_cp(gb, 0x10);
  if (F & FC) { CYCT(b_+376, b_+378); goto L_6803; } // jr c
  CYC(b_+376, b_+378);
  CYC(b_+378, b_+379); A = mem_rd(gb, DE);
  CYC(b_+379, b_+380); A = alu_inc8(gb, A);
  CYC(b_+380, b_+381); mem_wr(gb, DE, A);
L_6803:
  CYC(b_+381, b_+383); A = 0x04;
  CALL_C(b_+383, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+386);
  CYC(b_+386, b_+387); A = mem_rd(gb, HL);
  CYC(b_+387, b_+389); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+389); return; } // ret z
  CYC(b_+389, b_+390);
  CYC(b_+390, b_+393); goto func_675e; // jp

state3:
  CYC(b_+393, b_+395); E = 0xc6;
  CYC(b_+395, b_+396); A = mem_rd(gb, DE);
  CYC(b_+396, b_+397); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+397, b_+399); goto state3func_681a; } // jr z
  CYC(b_+397, b_+399);
  CYC(b_+399, b_+400); A = alu_dec8(gb, A);
  CYC(b_+400, b_+401); mem_wr(gb, DE, A);
  CYC(b_+401, b_+404); TAIL(objectApplySpeed); // jp

state3func_681a:
  CALL_C(b_+404, func_693b_hook, SYM(func_693b), b_+407);
  CALL_C(b_+407, objectGetRelativeAngle_hook, SYM(objectGetRelativeAngle), b_+410);
  CYC(b_+410, b_+412); E = 0xc9;
  CYC(b_+412, b_+413); mem_wr(gb, DE, A);
  CALL_C(b_+413, objectApplySpeed_hook, SYM(objectApplySpeed), b_+416);
  CALL_C(b_+416, state3func_6970_hook, SYM(state3func_6970), b_+419);
  if (!(F & FZ)) { RET_TAKEN(b_+419); return; } // ret nz
  CYC(b_+419, b_+420);
  CYC(b_+420, b_+422); E = 0xc2;
  CYC(b_+422, b_+423); A = mem_rd(gb, DE);
  CYC(b_+423, b_+424); alu_rlca(gb);
  if (F & FC) { CYCT(b_+424, b_+426); goto state3func_6864; } // jr c
  CYC(b_+424, b_+426);
  CYC(b_+426, b_+427); H = D;
  CYC(b_+427, b_+429); L = 0xe4;
  CYC(b_+429, b_+431); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+431, b_+433); E = 0xf2;
  CYC(b_+433, b_+434); A = mem_rd(gb, DE);
  CYC(b_+434, b_+435); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+435, b_+437); goto L_6850; } // jr z
  CYC(b_+435, b_+437);
  CYC(b_+437, b_+438); alu_xor(gb, A);
  CYC(b_+438, b_+439); mem_wr(gb, DE, A);
  CALL_C(b_+439, func_69a5_hook, SYM(func_69a5), b_+442);
  CYC(b_+442, b_+444); L = 0xab;
  CYC(b_+444, b_+445); A = mem_rd(gb, HL);
  CYC(b_+445, b_+446); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+446, b_+448); goto L_6850; } // jr nz
  CYC(b_+446, b_+448);
  CYC(b_+448, b_+450); mem_wr(gb, HL, 0x3c);
  CYC(b_+450, b_+452); L = 0xb5;
  CYC(b_+452, b_+453); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+453, b_+455); A = 0x63; // SND_BOSS_DAMAGE
  CALL_C(b_+455, playSound_b00_hook, SYM(playSound_b00), b_+458);
L_6850:
  CYC(b_+458, b_+460); E = 0xc6;
  CYC(b_+460, b_+462); A = 0x3c;
  CYC(b_+462, b_+463); mem_wr(gb, DE, A);
  CALL_C(b_+463, func_69a5_hook, SYM(func_69a5), b_+466);
  CYC(b_+466, b_+468); L = 0xb7;
  CYC(b_+468, b_+470); E = 0xc2;
  CYC(b_+470, b_+471); A = mem_rd(gb, DE);
  CYC(b_+471, b_+473); A = alu_swap(gb, A);
  CALL_C(b_+473, setFlag_hook, SYM(setFlag), b_+476);
  CYC(b_+476, b_+478); goto L_6867; // jr

state3func_6864:
  CALL_C(b_+478, objectSetInvisible_hook, SYM(objectSetInvisible), b_+481);
L_6867:
  CYC(b_+481, b_+483); E = 0xc4;
  CYC(b_+483, b_+485); A = 0x04;
  CYC(b_+485, b_+486); mem_wr(gb, DE, A);
  RET(b_+486); return; // ret

state4:
  CYC(b_+487, b_+488); H = D;
  CYC(b_+488, b_+490); L = 0xe4;
  CYC(b_+490, b_+492); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+492, b_+495);
  CALL_C(b_+593, func_693b_hook, SYM(func_693b), b_+596);
  CYC(b_+596, b_+597); H = D;
  CYC(b_+597, b_+599); L = 0xcb;
  CYC(b_+599, b_+600); mem_wr(gb, HL, B);
  CYC(b_+600, b_+602); L = 0xcd;
  CYC(b_+602, b_+603); mem_wr(gb, HL, C);
  CYC(b_+603, b_+604); // ret (returns to 0x6875, no continuation check needed here)
  CALL_C(b_+495, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+498);
  if (!(F & FZ)) { RET_TAKEN(b_+498); return; } // ret nz
  CYC(b_+498, b_+499);
  CALL_C(b_+499, func_69a5_hook, SYM(func_69a5), b_+502);
  CYC(b_+502, b_+503); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+503, b_+505); alu_cp(gb, 0x12);
  if (!(F & FZ)) { RET_TAKEN(b_+505); return; } // ret nz
  CYC(b_+505, b_+506);
  CYC(b_+506, b_+507); A = mem_rd(gb, HL);
  CYC(b_+507, b_+509); alu_bit(gb, 5, A);
  if (!(F & FZ)) { CYCT(b_+509, b_+511); goto state4func_689e; } // jr nz
  CYC(b_+509, b_+511);
  CYC(b_+511, b_+513); E = 0xc2;
  CYC(b_+513, b_+514); A = mem_rd(gb, DE);
  CYC(b_+514, b_+515); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+515, b_+517); goto state4func_689e; } // jr z
  CYC(b_+515, b_+517);
  CALL_C(b_+517, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+520);
  CYC(b_+520, b_+522); alu_cp(gb, 0x10);
  if (!(F & FZ)) { RET_TAKEN(b_+522); return; } // ret nz
  CYC(b_+522, b_+523);
  CYC(b_+523, b_+526); A = mem_rd(gb, w1Link_direction); // w1Link.direction
  CYC(b_+526, b_+527); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+527); return; } // ret nz
  CYC(b_+527, b_+528);
  CYC(b_+528, b_+529); H = D;
  CYC(b_+529, b_+531); L = 0xe4;
  CYC(b_+531, b_+533); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+533, b_+536); TAIL(objectAddToGrabbableObjectBuffer); // jp

state4func_689e:
  CYC(b_+536, b_+538); A = 0x6f; // SND_EXPLOSION
  CALL_C(b_+538, playSound_b00_hook, SYM(playSound_b00), b_+541);
  CALL_C(b_+541, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+544);
  CYC(b_+544, b_+545); H = D;
  CYC(b_+545, b_+547); L = 0xc9;
  CYC(b_+547, b_+548); mem_wr(gb, HL, A);
  CYC(b_+548, b_+550); L = 0xc4;
  CYC(b_+550, b_+552); mem_wr(gb, HL, 0x05);
  CYC(b_+552, b_+554); L = 0xc6;
  CYC(b_+554, b_+556); mem_wr(gb, HL, 0x02);
  CYC(b_+556, b_+558); L = 0xd0;
  CYC(b_+558, b_+560); mem_wr(gb, HL, 0x78);
  CALL_C(b_+560, func_69a5_hook, SYM(func_69a5), b_+563);
  CYC(b_+563, b_+565); L = 0xb7;
  CYC(b_+565, b_+567); E = 0xc2;
  CYC(b_+567, b_+568); A = mem_rd(gb, DE);
  CYC(b_+568, b_+570); A = alu_swap(gb, A);
  CYC(b_+570, b_+573); TAIL(unsetFlag); // jp

subidBit7SetArm_state4:
  CYC(b_+573, b_+575); A = 0x04;
  CALL_C(b_+575, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+578);
  CYC(b_+578, b_+579); A = mem_rd(gb, HL);
  CYC(b_+579, b_+581); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+581, b_+583); goto state4func_68d7; } // jr z
  CYC(b_+581, b_+583);
  CYC(b_+583, b_+584); E = L;
  CYC(b_+584, b_+585); mem_wr(gb, DE, A);
  CYC(b_+585, b_+587); L = 0xc9;
  CYC(b_+587, b_+588); E = L;
  CYC(b_+588, b_+589); A = mem_rd(gb, HL);
  CYC(b_+589, b_+590); mem_wr(gb, DE, A);
  CYC(b_+590, b_+593); TAIL(objectSetVisible); // jp

state4func_68d7:
  CALL_C(b_+593, func_693b_hook, SYM(func_693b), b_+596);
  CYC(b_+596, b_+597); H = D;
  CYC(b_+597, b_+599); L = 0xcb;
  CYC(b_+599, b_+600); mem_wr(gb, HL, B);
  CYC(b_+600, b_+602); L = 0xcd;
  CYC(b_+602, b_+603); mem_wr(gb, HL, C);
  RET(b_+603); return; // ret

state5:
  CALL_C(b_+604, partCommon_getTileCollisionInFront_hook, SYM(partCommon_getTileCollisionInFront), b_+607);
  if (!(F & FZ)) { CYCT(b_+607, b_+609); goto state5func_68fe; } // jr nz
  CYC(b_+607, b_+609);
  CALL_C(b_+609, objectApplySpeed_hook, SYM(objectApplySpeed), b_+612);
  CALL_C(b_+612, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+615);
  if (!(F & FZ)) { RET_TAKEN(b_+615); return; } // ret nz
  CYC(b_+615, b_+616);
  CYC(b_+616, b_+618); mem_wr(gb, HL, 0x03);
  CYC(b_+618, b_+620); E = 0xd0;
  CYC(b_+620, b_+621); A = mem_rd(gb, DE);
  CYC(b_+621, b_+622); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+622, b_+625); goto state5func_68fe; } // jp z
  CYC(b_+622, b_+625);
  CYC(b_+625, b_+627); alu_sub(gb, 0x0a);
  if (!(F & FC)) { CYCT(b_+627, b_+629); goto L_68fc; } // jr nc
  CYC(b_+627, b_+629);
  CYC(b_+629, b_+630); alu_xor(gb, A);
L_68fc:
  CYC(b_+630, b_+631); mem_wr(gb, DE, A);
  RET(b_+631); return; // ret

state5func_68fe:
  CYC(b_+632, b_+633); H = D;
  CYC(b_+633, b_+635); L = 0xc4;
  CYC(b_+635, b_+637); mem_wr(gb, HL, 0x06);
  CYC(b_+637, b_+639); L = 0xc6;
  CYC(b_+639, b_+641); mem_wr(gb, HL, 0x3c);
  CYC(b_+641, b_+643); L = 0xd0;
  CYC(b_+643, b_+645); mem_wr(gb, HL, 0x00);
  RET(b_+645); return; // ret

subidBit7SetArm_state5:
  CYC(b_+646, b_+648); A = 0x10;
  CALL_C(b_+648, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+651);
  CYC(b_+651, b_+652); E = L;
  CYC(b_+652, b_+653); A = mem_rd(gb, HL);
  CYC(b_+653, b_+655); alu_sub(gb, 0x19);
  if (!(F & FC)) { CYCT(b_+655, b_+657); goto L_6918; } // jr nc
  CYC(b_+655, b_+657);
  CYC(b_+657, b_+658); alu_xor(gb, A);
L_6918:
  CYC(b_+658, b_+659); mem_wr(gb, DE, A);
  CYC(b_+659, b_+661); L = 0xc4;
  CYC(b_+661, b_+662); A = mem_rd(gb, HL);
  CYC(b_+662, b_+664); alu_cp(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+664, b_+667); objectApplySpeed_hook(gb); return; } // jp nz
  CYC(b_+664, b_+667);
  CYC(b_+667, b_+670); goto func_6762; // jp

state6:
  CALL_C(b_+670, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+673);
  if (!(F & FZ)) { RET_TAKEN(b_+673); return; } // ret nz
  CYC(b_+673, b_+674);
  CYC(b_+674, b_+676); L = 0xe4;
  CYC(b_+676, b_+678); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CALL_C(b_+678, func_69a5_hook, SYM(func_69a5), b_+681);
  CYC(b_+681, b_+682); L = alu_inc8(gb, L);
  CYC(b_+682, b_+683); A = mem_rd(gb, HL);
  CYC(b_+683, b_+685); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(b_+685, b_+687); goto L_6938; } // jr z
  CYC(b_+685, b_+687);
  CYC(b_+687, b_+689); A = 0x80;
  CYC(b_+689, b_+690); mem_wr(gb, HL, A);
L_6938:
  CYC(b_+690, b_+693); goto func_6762; // jp
}

void func_693b_hook(GB *gb) {
  BASE(func_693b);
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+7); C = 0x0c;
  CYC(b_+7, b_+8); alu_rrca(gb);
  if (!(F & FC)) {
    CYCT(b_+8, b_+10);
  } else {
    CYC(b_+8, b_+10);
    CYC(b_+10, b_+12); C = 0xf4;
  }
  CYC(b_+12, b_+14); E = 0xf0;
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); H = A;
  CYC(b_+16, b_+18); L = 0x8b;
  CYC(b_+18, b_+19); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+19, b_+21); alu_add(gb, 0x0c);
  CYC(b_+21, b_+22); B = A;
  CYC(b_+22, b_+23); L = alu_inc8(gb, L);
  CYC(b_+23, b_+24); A = mem_rd(gb, HL);
  CYC(b_+24, b_+25); alu_add(gb, C);
  CYC(b_+25, b_+26); C = A;
  RET(b_+26); return;
}

void state0func_6956_hook(GB *gb) {
  BASE(state0func_6956);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc2; // Part.subid
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_and(gb, 0x0f);
  CYC(b_+5, b_+7); alu_cp(gb, 0x02);
  if (F & FZ) { RET_TAKEN(b_+7); return; } // ret z
  CYC(b_+7, b_+8);
  CALL_C(b_+8, getFreePartSlot_hook, SYM(getFreePartSlot), b_+11);
  CYC(b_+11, b_+13); A = 0x35; // PART_RAMROCK_GLOVE_FORM_ARM
  CYC(b_+13, b_+14); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); A = alu_inc8(gb, A);
  CYC(b_+16, b_+17); mem_wr(gb, HL, A);
  CYC(b_+17, b_+19); E = 0xf0;
  CYC(b_+19, b_+20); L = E;
  CYC(b_+20, b_+21); A = mem_rd(gb, DE);
  CYC(b_+21, b_+22); mem_wr(gb, HL, A);
  CYC(b_+22, b_+24); L = 0xc2;
  CYC(b_+24, b_+25); E = L;
  RET(b_+25); return;
}

void state3func_6970_hook(GB *gb) {
  BASE(state3func_6970);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, func_693b_hook, SYM(func_693b), b_+3);
  CYC(b_+3, b_+5); E = 0x03;
  CYC(b_+5, b_+6); H = D;
  CYC(b_+6, b_+8); L = 0xcb;
  CYC(b_+8, b_+9); A = E;
  CYC(b_+9, b_+10); alu_add(gb, B);
  CYC(b_+10, b_+11); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+11, b_+13); goto ret_6990; } // jr c
  CYC(b_+11, b_+13);
  CYC(b_+13, b_+14); alu_sub(gb, E);
  CYC(b_+14, b_+15); alu_sub(gb, E);
  CYC(b_+15, b_+16); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+16, b_+18); goto ret_6990; } // jr nc
  CYC(b_+16, b_+18);
  CYC(b_+18, b_+20); L = 0xcd;
  CYC(b_+20, b_+21); A = E;
  CYC(b_+21, b_+22); alu_add(gb, C);
  CYC(b_+22, b_+23); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { CYCT(b_+23, b_+25); goto ret_6990; } // jr c
  CYC(b_+23, b_+25);
  CYC(b_+25, b_+26); alu_sub(gb, E);
  CYC(b_+26, b_+27); alu_sub(gb, E);
  CYC(b_+27, b_+28); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+28, b_+30); goto ret_6990; } // jr nc
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+31); alu_xor(gb, A);
  RET(b_+31); return;
ret_6990:
  CYC(b_+32, b_+33); alu_or(gb, D);
  RET(b_+33); return;
}

void state0func_6992_hook(GB *gb) {
  BASE(state0func_6992);
  CYC(b_+0, b_+1); push_effect(gb, HL);
  CYC(b_+1, b_+2); A = mem_rd(gb, HL);
  CYC(b_+2, b_+4); alu_and(gb, 0x10);
  CYC(b_+4, b_+6); A = alu_swap(gb, A);
  CYC(b_+6, b_+9); SET_HL(b_+17);
  CYC(b_+9, b_+10); ramrockGloveFormArm_addAToHl_from_rst(gb, b_+10);
  CYC(b_+10, b_+11); C = mem_rd(gb, HL);
  CYC(b_+11, b_+13); B = 0xfc;
  CYC(b_+13, b_+14); SET_HL(pop_effect(gb));
  CYC(b_+14, b_+17); TAIL(objectCopyPositionWithOffset); // jp
}

void func_69a5_hook(GB *gb) {
  BASE(func_69a5);
  CYC(b_+0, b_+2); E = 0xf0;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); H = A;
  CYC(b_+4, b_+6); L = 0x82;
  RET(b_+6); return;
}
