#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode56), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode56), (from), (to), true)

static uint16_t veranSpiderweb_jump_table(GB *gb) {
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

void partCode56_hook(GB *gb);

void partCode56_hook(GB *gb) {
  BASE(partCode56);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  if (F & FZ) { CYCT(b_+0, b_+2); goto normalStatus; } // jr z
  CYC(b_+0, b_+2);
  CYC(b_+2, b_+4); E = 0xea; // Part.var2a
  CYC(b_+4, b_+5); A = mem_rd(gb, DE);
  CYC(b_+5, b_+7); alu_cp(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); goto normalStatus; } // jr nz
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+12); SET_HL(w1Link_var31);
  CYC(b_+12, b_+14); mem_wr(gb, HL, 0x10);
  CYC(b_+14, b_+16); L = 0x30;
  CYC(b_+16, b_+18); mem_wr(gb, HL, 0x00);
  CYC(b_+18, b_+20); L = 0x24;
  CYC(b_+20, b_+22); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) & ~(1 << 7)));
  CYC(b_+22, b_+25); SET_BC(0xfa00);
  CALL_C(b_+25, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+28);
  CYC(b_+28, b_+29); H = D;
  CYC(b_+29, b_+31); L = 0xf0; // Part.var30
  CYC(b_+31, b_+33); mem_wr(gb, HL, 0x01);
  CYC(b_+33, b_+35); L = 0xc4; // Part.state
  CYC(b_+35, b_+36); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+36, b_+37); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+37, b_+39); goto normalStatus; } // jr nz
  CYC(b_+37, b_+39);
  CYC(b_+39, b_+40); L = alu_inc8(gb, L);
  CYC(b_+40, b_+42); A = 0x01;
  CYC(b_+42, b_+43); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);

normalStatus:
  CYC(b_+44, b_+46); E = 0xc2; // Part.subid
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+49); E = 0xc4; // Part.state
  {
    CYC(b_+49, b_+50); push_effect(gb, b_+50);
    uint16_t target = veranSpiderweb_jump_table(gb);
    if (target == b_+58) goto subid0;
    if (target == b_+145) goto subid1;
    if (target == b_+372) goto subid2;
    goto subid3;
  }

subid0:
  CYC(b_+58, b_+59); A = mem_rd(gb, DE);
  CYC(b_+59, b_+60); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+60, b_+62); goto func_7c2e; } // jr z
  CYC(b_+60, b_+62);
  CALL_C(b_+62, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+65);
  if (!(F & FZ)) { CYCT(b_+65, b_+67); goto L_7c05; } // jr nz
  CYC(b_+65, b_+67);
  CYC(b_+67, b_+69); mem_wr(gb, HL, 0x04);
  CALL_C(b_+69, getFreePartSlot_hook, SYM(getFreePartSlot), b_+72);
  if (!(F & FZ)) { CYCT(b_+72, b_+74); goto L_7c05; } // jr nz
  CYC(b_+72, b_+74);
  CYC(b_+74, b_+76); mem_wr(gb, HL, 0x56); // PART_VERAN_SPIDERWEB
  CYC(b_+76, b_+77); L = alu_inc8(gb, L);
  CYC(b_+77, b_+79); mem_wr(gb, HL, 0x02);
  CYC(b_+79, b_+81); L = 0xd6; // Part.relatedObj1
  CYC(b_+81, b_+82); E = L;
  CYC(b_+82, b_+83); A = mem_rd(gb, DE);
  CYC(b_+83, b_+84); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+84, b_+85); E = alu_inc8(gb, E);
  CYC(b_+85, b_+86); A = mem_rd(gb, DE);
  CYC(b_+86, b_+87); mem_wr(gb, HL, A);
  CALL_C(b_+87, objectCopyPosition_hook, SYM(objectCopyPosition), b_+90);
L_7c05:
  CYC(b_+90, b_+92); A = 0x02;
  CALL_C(b_+92, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+95);
  CYC(b_+95, b_+96); A = mem_rd(gb, HL);
  CYC(b_+96, b_+97); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+97, b_+100); goto func_7c28; } // jp nz
  CYC(b_+97, b_+100);
  CYC(b_+100, b_+101); C = H;
  CYC(b_+101, b_+103); A = hram_rd(gb, 0xaa); // hCameraY
  CYC(b_+103, b_+104); B = A;
  CYC(b_+104, b_+106); E = 0xcf; // Part.zh
  CYC(b_+106, b_+107); A = mem_rd(gb, DE);
  CYC(b_+107, b_+109); alu_sub(gb, 0x04);
  CYC(b_+109, b_+110); mem_wr(gb, DE, A);
  CYC(b_+110, b_+111); H = D;
  CYC(b_+111, b_+113); L = 0xcb; // Part.yh
  CYC(b_+113, b_+114); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+114, b_+115); alu_sub(gb, B);
  CYC(b_+115, b_+117); alu_cp(gb, 0xb0);
  if (F & FC) { RET_TAKEN(b_+117); return; } // ret c
  CYC(b_+117, b_+118);
  CYC(b_+118, b_+119); H = C;
  CYC(b_+119, b_+121); L = 0xb8; // Enemy.animParameter?
  CYC(b_+121, b_+122); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+122, b_+125); partDelete_hook(gb); return; // jp

func_7c28:
  CALL_C(b_+125, objectCreatePuff_hook, SYM(objectCreatePuff), b_+128);
  CYC(b_+128, b_+131); partDelete_hook(gb); return; // jp

func_7c2e:
  CYC(b_+131, b_+132); H = D;
  CYC(b_+132, b_+133); L = E; // Part.state
  CYC(b_+133, b_+134); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+134, b_+136); L = 0xc6; // Part.counter1
  CYC(b_+136, b_+137); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+137, objectSetVisible80_hook, SYM(objectSetVisible80), b_+140);
beamSound:
  CYC(b_+140, b_+142); A = 0xbb; // SND_BEAM2
  CYC(b_+142, b_+145); playSound_b00_hook(gb); return; // jp

subid1:
  CYC(b_+145, b_+147); A = 0x02;
  CALL_C(b_+147, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+150);
  CYC(b_+150, b_+151); A = mem_rd(gb, HL);
  CYC(b_+151, b_+152); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+152, b_+154); goto func_7c28; } // jr nz
  CYC(b_+152, b_+154);
  CYC(b_+154, b_+156); L = 0xad; // Part.var2d?
  CYC(b_+156, b_+157); A = mem_rd(gb, HL);
  CYC(b_+157, b_+158); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+158, b_+160); goto func_7c28; } // jr nz
  CYC(b_+158, b_+160);
  CYC(b_+160, b_+162); E = 0xc4; // Part.state
  CYC(b_+162, b_+163); A = mem_rd(gb, DE);
  {
    CYC(b_+163, b_+164); push_effect(gb, b_+164);
    uint16_t target = veranSpiderweb_jump_table(gb);
    if (target == b_+174) goto subid1_state0;
    if (target == b_+231) goto subid1_state1;
    if (target == b_+286) goto subid1_state2;
    if (target == b_+294) goto subid1_state3;
    goto subid1_state4;
  }

subid1_state0:
  CYC(b_+174, b_+175); H = D;
  CYC(b_+175, b_+176); L = E; // Part.state
  CYC(b_+176, b_+177); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+177, b_+179); L = 0xc6; // Part.counter1
  CYC(b_+179, b_+181); mem_wr(gb, HL, 0x01);
  CYC(b_+181, b_+182); L = alu_inc8(gb, L);
  CYC(b_+182, b_+184); mem_wr(gb, HL, 0x05);
  CYC(b_+184, b_+186); L = 0xe4; // Part.collisionType
  CYC(b_+186, b_+188); mem_wr(gb, HL, (uint8_t)(mem_rd(gb, HL) | (1 << 7)));
  CYC(b_+188, b_+190); L = 0xd0; // Part.speed
  CYC(b_+190, b_+192); mem_wr(gb, HL, 0x50);
  CYC(b_+192, b_+194); L = 0xf1; // Part.speedZ
  CYC(b_+194, b_+196); E = 0xcb; // Part.yh
  CYC(b_+196, b_+197); A = mem_rd(gb, DE);
  CYC(b_+197, b_+199); alu_add(gb, 0x10);
  CYC(b_+199, b_+200); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+200, b_+201); mem_wr(gb, DE, A);
  CYC(b_+201, b_+203); E = 0xcd; // Part.xh
  CYC(b_+203, b_+204); A = mem_rd(gb, DE);
  CYC(b_+204, b_+205); mem_wr(gb, HL, A);
  CALL_C(b_+205, objectGetAngleTowardLink_hook, SYM(objectGetAngleTowardLink), b_+208);
  CYC(b_+208, b_+210); alu_cp(gb, 0x0e);
  CYC(b_+210, b_+212); B = 0x0c;
  if (F & FC) { CYCT(b_+212, b_+214); goto L_7c89; } // jr c
  CYC(b_+212, b_+214);
  CYC(b_+214, b_+216); B = 0x10;
  CYC(b_+216, b_+218); alu_cp(gb, 0x13);
  if (F & FC) { CYCT(b_+218, b_+220); goto L_7c89; } // jr c
  CYC(b_+218, b_+220);
  CYC(b_+220, b_+222); B = 0x14;
L_7c89:
  CYC(b_+222, b_+224); E = 0xc9; // Part.angle
  CYC(b_+224, b_+225); A = B;
  CYC(b_+225, b_+226); mem_wr(gb, DE, A);
  CALL_C(b_+226, objectSetVisible81_hook, SYM(objectSetVisible81), b_+229);
  CYC(b_+229, b_+231); goto beamSound; // jr

subid1_state1:
  CALL_C(b_+231, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+234);
  if (!(F & FZ)) { CYCT(b_+234, b_+236); goto L_7cb0; } // jr nz
  CYC(b_+234, b_+236);
  CYC(b_+236, b_+238); mem_wr(gb, HL, 0x08); // Part.counter1
  CYC(b_+238, b_+239); L = alu_inc8(gb, L);
  CYC(b_+239, b_+240); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // Part.counter2
  if (F & FZ) { CYCT(b_+240, b_+242); goto L_7cb6; } // jr z
  CYC(b_+240, b_+242);
  CALL_C(b_+242, getFreePartSlot_hook, SYM(getFreePartSlot), b_+245);
  if (!(F & FZ)) { CYCT(b_+245, b_+247); goto L_7cb0; } // jr nz
  CYC(b_+245, b_+247);
  CYC(b_+247, b_+249); mem_wr(gb, HL, 0x56); // PART_VERAN_SPIDERWEB
  CYC(b_+249, b_+250); L = alu_inc8(gb, L);
  CYC(b_+250, b_+252); mem_wr(gb, HL, 0x03);
  CYC(b_+252, b_+254); L = 0xd6; // Part.relatedObj1
  CYC(b_+254, b_+256); A = 0xc0;
  CYC(b_+256, b_+257); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+257, b_+258); mem_wr(gb, HL, D);
  CALL_C(b_+258, objectCopyPosition_hook, SYM(objectCopyPosition), b_+261);
L_7cb0:
  CALL_C(b_+261, partCommon_checkTileCollisionOrOutOfBounds_hook, SYM(partCommon_checkTileCollisionOrOutOfBounds), b_+264);
  if (!(F & FC)) { CYCT(b_+264, b_+267); objectApplySpeed_hook(gb); return; } // jp nc
  CYC(b_+264, b_+267);
L_7cb6:
  CYC(b_+267, b_+268); H = D;
  CYC(b_+268, b_+270); L = 0xc4; // Part.state
  CYC(b_+270, b_+271); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+271, b_+273); L = 0xc6; // Part.counter1
  CYC(b_+273, b_+275); mem_wr(gb, HL, 0x1e);
  CYC(b_+275, b_+277); L = 0xd0; // Part.speed
  CYC(b_+277, b_+279); mem_wr(gb, HL, 0x3c);
  CYC(b_+279, b_+281); L = 0xc9; // Part.angle
  CYC(b_+281, b_+282); A = mem_rd(gb, HL);
  CYC(b_+282, b_+284); alu_xor(gb, 0x10);
  CYC(b_+284, b_+285); mem_wr(gb, HL, A);
  RET(b_+285); return; // ret

subid1_state2:
  CALL_C(b_+286, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+289);
  if (!(F & FZ)) { RET_TAKEN(b_+289); return; } // ret nz
  CYC(b_+289, b_+290);
  CYC(b_+290, b_+292); L = 0xc4; // Part.state
  CYC(b_+292, b_+293); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+293); return; // ret

subid1_state3:
  CALL_C(b_+294, objectApplySpeed_hook, SYM(objectApplySpeed), b_+297);
  CYC(b_+297, b_+299); E = 0xf0; // Part.var30
  CYC(b_+299, b_+300); A = mem_rd(gb, DE);
  CYC(b_+300, b_+301); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+301, b_+303); goto L_7ce3; } // jr z
  CYC(b_+301, b_+303);
  CYC(b_+303, b_+306); SET_BC(0xfa00);
  CYC(b_+306, b_+309); SET_HL(w1Link);
  CALL_C(b_+309, objectCopyPositionWithOffset_hook, SYM(objectCopyPositionWithOffset), b_+312);
L_7ce3:
  CYC(b_+312, b_+313); H = D;
  CYC(b_+313, b_+315); L = 0xf1; // Part.speedZ
  CYC(b_+315, b_+317); E = 0xcb; // Part.yh
  CYC(b_+317, b_+318); A = mem_rd(gb, DE);
  CYC(b_+318, b_+319); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+319, b_+321); alu_add(gb, 0x02);
  CYC(b_+321, b_+323); alu_cp(gb, 0x05);
  if (!(F & FC)) { RET_TAKEN(b_+323); return; } // ret nc
  CYC(b_+323, b_+324);
  CYC(b_+324, b_+326); L = 0xf2; // Part.speedZ+1
  CYC(b_+326, b_+328); E = 0xcd; // Part.xh
  CYC(b_+328, b_+329); A = mem_rd(gb, DE);
  CYC(b_+329, b_+330); alu_sub(gb, mem_rd(gb, HL));
  CYC(b_+330, b_+332); alu_add(gb, 0x02);
  CYC(b_+332, b_+334); alu_cp(gb, 0x05);
  if (!(F & FC)) { RET_TAKEN(b_+334); return; } // ret nc
  CYC(b_+334, b_+335);
  CYC(b_+335, b_+337); A = 0x38;
  CALL_C(b_+337, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+340);
  CYC(b_+340, b_+341); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+341, b_+343); E = 0xf0; // Part.var30
  CYC(b_+343, b_+344); A = mem_rd(gb, DE);
  CYC(b_+344, b_+345); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+345, b_+348); partDelete_hook(gb); return; } // jp z
  CYC(b_+345, b_+348);
  CYC(b_+348, b_+350); L = 0x86; // Enemy.var06?
  CYC(b_+350, b_+352); mem_wr(gb, HL, 0x08);
  CYC(b_+352, b_+353); H = D;
  CYC(b_+353, b_+355); L = 0xc4; // Part.state
  CYC(b_+355, b_+356); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  RET(b_+356); return; // ret

subid1_state4:
  CYC(b_+357, b_+360); SET_HL(w1Link_substate);
  CYC(b_+360, b_+361); A = mem_rd(gb, HL);
  CYC(b_+361, b_+363); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+363, b_+366); partDelete_hook(gb); return; } // jp z
  CYC(b_+363, b_+366);
  CYC(b_+366, b_+369); SET_BC((SYM(loadUncompressedGfxHeader) + 38));
  CYC(b_+369, b_+372); objectTakePositionWithOffset_hook(gb); return; // jp

subid2:
  CYC(b_+372, b_+373); A = mem_rd(gb, DE);
  CYC(b_+373, b_+374); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+374, b_+376); goto func_7d39; } // jr z
  CYC(b_+374, b_+376);
  CYC(b_+376, b_+378); A = 0x1a;
  CALL_C(b_+378, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+381);
  CYC(b_+381, b_+383); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+383, b_+385); goto subid2_delete; } // jr z
  CYC(b_+383, b_+385);
  CYC(b_+385, b_+387); L = 0x8f; // Enemy.var0f?
  CYC(b_+387, b_+388); B = mem_rd(gb, HL);
  CYC(b_+388, b_+389); B = alu_dec8(gb, B);
  CYC(b_+389, b_+391); E = 0xcf; // Part.zh
  CYC(b_+391, b_+392); A = mem_rd(gb, DE);
  CYC(b_+392, b_+393); A = alu_dec8(gb, A);
  CYC(b_+393, b_+394); alu_cp(gb, B);
  if (F & FC) { RET_TAKEN(b_+394); return; } // ret c
  CYC(b_+394, b_+395);
subid2_delete:
  CYC(b_+395, b_+398); partDelete_hook(gb); return; // jp

func_7d39:
  CYC(b_+398, b_+399); A = alu_inc8(gb, A);
  CYC(b_+399, b_+400); mem_wr(gb, DE, A);
  CYC(b_+400, b_+401); A = alu_inc8(gb, A);
  CALL_C(b_+401, partSetAnimation_hook, SYM(partSetAnimation), b_+404);
  CYC(b_+404, b_+407); objectSetVisible80_hook(gb); return; // jp

subid3:
  CYC(b_+407, b_+408); A = mem_rd(gb, DE);
  CYC(b_+408, b_+409); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+409, b_+411); goto func_7d59; } // jr z
  CYC(b_+409, b_+411);
  CYC(b_+411, b_+413); A = 0x01;
  CALL_C(b_+413, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+416);
  CYC(b_+416, b_+417); A = mem_rd(gb, HL);
  CYC(b_+417, b_+419); alu_cp(gb, 0x56);
  if (!(F & FZ)) { CYCT(b_+419, b_+421); goto subid3_delete; } // jr nz
  CYC(b_+419, b_+421);
  CYC(b_+421, b_+423); L = 0xcb; // Part.yh
  CYC(b_+423, b_+424); E = L;
  CYC(b_+424, b_+425); A = mem_rd(gb, DE);
  CYC(b_+425, b_+426); alu_cp(gb, mem_rd(gb, HL));
  if (F & FC) { RET_TAKEN(b_+426); return; } // ret c
  CYC(b_+426, b_+427);
subid3_delete:
  CYC(b_+427, b_+430); partDelete_hook(gb); return; // jp

func_7d59:
  CYC(b_+430, b_+431); A = alu_inc8(gb, A);
  CYC(b_+431, b_+432); mem_wr(gb, DE, A);
  CYC(b_+432, b_+434); A = 0x09;
  CALL_C(b_+434, objectGetRelatedObject1Var_hook, SYM(objectGetRelatedObject1Var), b_+437);
  CYC(b_+437, b_+438); A = mem_rd(gb, HL);
  CYC(b_+438, b_+440); alu_sub(gb, 0x0c);
  CYC(b_+440, b_+441); alu_rrca(gb);
  CYC(b_+441, b_+442); alu_rrca(gb);
  CYC(b_+442, b_+443); A = alu_inc8(gb, A);
  CALL_C(b_+443, partSetAnimation_hook, SYM(partSetAnimation), b_+446);
  CYC(b_+446, SYM(partCode57)); objectSetVisible83_hook(gb); return; // jp
}
