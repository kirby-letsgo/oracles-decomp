#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/makuCutscenes.s.
// INTERAC_MAKU_CUTSCENES

static uint16_t makuCutscenes_jump_table(GB *gb) {
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

static void makuCutscenes_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode22@func_561f
static void makuCutscenes_func_561f(GB *gb) {
  BASE(interactionCode22);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+311, b_+314); SET_BC(b_+343 /* @table_563f */);
  CALL_C(b_+314, s_addDoubleIndexToBc, SYM(addDoubleIndexToBc), b_+317);
  CYC(b_+317, b_+319); A = 0x04;
L_5627:
  CYC(b_+319, b_+321); mem_wr(gb, hFF8B, A);
  CALL_C(b_+321, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+324);
  if (!(F & FZ)) { RET_TAKEN(b_+324); return; }
  CYC(b_+324, b_+325);
  CYC(b_+325, b_+327); mem_wr(gb, HL, 0x05);
  CYC(b_+327, b_+329); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+329, b_+330); A = mem_rd(gb, BC);
  CYC(b_+330, b_+331); mem_wr(gb, HL, A);
  CYC(b_+331, b_+332); SET_BC(BC + 1);
  CYC(b_+332, b_+334); L = INTERACTION_BASE + OBJ_XH;
  CYC(b_+334, b_+335); A = mem_rd(gb, BC);
  CYC(b_+335, b_+336); mem_wr(gb, HL, A);
  CYC(b_+336, b_+337); SET_BC(BC + 1);
  CYC(b_+337, b_+339); A = mem_rd(gb, hFF8B);
  CYC(b_+339, b_+340); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+340, b_+342); goto L_5627; }
  CYC(b_+340, b_+342);
  RET(b_+342); return;
}

// interactionCode22@func_5610
static void makuCutscenes_func_5610(GB *gb) {
  BASE(interactionCode22);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+296, b_+297); push_effect(gb, AF);
  CYC(b_+297, b_+300); SET_HL(wc6e5);
  CYC(b_+300, b_+302); mem_wr(gb, HL, 0x00);
L_5616:
  CYC(b_+302, b_+303); alu_add(gb, A);
  if (!(F & FC)) { CYCT(b_+303, b_+305); goto L_561a; }
  CYC(b_+303, b_+305);
  CYC(b_+305, b_+306); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
L_561a:
  CYC(b_+306, b_+307); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+307, b_+309); goto L_5616; }
  CYC(b_+307, b_+309);
  CYC(b_+309, b_+310); SET_AF(pop_effect(gb));
  RET(b_+310); return;
}

// interactionCode22@func_55f5
static void makuCutscenes_func_55f5(GB *gb) {
  BASE(interactionCode22);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+269, b_+271); A = 0x40;
  CALL_C(b_+271, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+274);
  if (!(F & FC)) { CYCT(b_+274, b_+276); goto noEssence; }
  CYC(b_+274, b_+276);
  CYC(b_+276, b_+278); E = INTERACTION_BASE + OBJ_VAR3E;
  CYC(b_+278, b_+279); mem_wr(gb, DE, A);
  CALL_L(b_+279, makuCutscenes_func_5610, b_+282);
  CYC(b_+282, b_+283); C = A;
  CYC(b_+283, b_+285); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+285, b_+286); A = mem_rd(gb, DE);
  CYC(b_+286, b_+289); SET_HL(SYM(bitTable));
  CYC(b_+289, b_+290); alu_add(gb, L);
  CYC(b_+290, b_+291); L = A;
  CYC(b_+291, b_+292); A = C;
  CYC(b_+292, b_+293); alu_and(gb, mem_rd(gb, HL));
  RET(b_+293); return;
noEssence:
  CYC(b_+294, b_+295); alu_xor(gb, A);
  RET(b_+295); return;
}

// interactionCode22@setScript
static void makuCutscenes_setScript(GB *gb) {
  BASE(interactionCode22);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+118, b_+120); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+120, b_+122); A = 0x01;
  CYC(b_+122, b_+123); mem_wr(gb, DE, A);
  CYC(b_+123, b_+125); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+125, b_+126); A = mem_rd(gb, DE);
  CYC(b_+126, b_+129); SET_HL(b_+359 /* @scriptTable */);
  CYC(b_+129, b_+130); makuCutscenes_add_double_index(gb, b_+130);
  CYC(b_+130, b_+131); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+131, b_+132); H = mem_rd(gb, HL);
  CYC(b_+132, b_+133); L = A;
  CYC(b_+133, b_+136);
  TAIL(interactionSetScript);
}

// INTERAC_MAKU_CUTSCENES
void s_interactionCode22_hook(GB *gb) {
  BASE(interactionCode22);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (makuCutscenes_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+136) goto runScript;
    if (jt_ == b_+143) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+15); alu_cp(gb, 0x08);
  if (F & FZ) { CYCT(b_+15, b_+17); goto outsideTempleOfWinter; }
  CYC(b_+15, b_+17);
  CYC(b_+17, b_+19); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(b_+19, b_+21); goto haveWinterSeason; }
  CYC(b_+19, b_+21);
  if (!(F & FC)) { CYCT(b_+21, b_+23); goto atMakuTreeGate; }
  CYC(b_+21, b_+23);
  CALL_C(b_+23, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+26);
  CYC(b_+26, b_+28); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+28, b_+31); TAIL(interactionDelete); }
  CYC(b_+28, b_+31);
  CALL_L(b_+31, makuCutscenes_func_55f5, b_+34);
  if (F & FZ) { CYCT(b_+34, b_+37); TAIL(interactionDelete); }
  CYC(b_+34, b_+37);
  CALL_C(b_+37, s_returnIfScrollMode01Unset, SYM(returnIfScrollMode01Unset), b_+40);
  CALL_L(b_+40, makuCutscenes_setScript, b_+43);
  CALL_C(b_+43, s_interactionRunScript, SYM(interactionRunScript), b_+46);
  CALL_C(b_+46, s_interactionRunScript, SYM(interactionRunScript), b_+49);
  CYC(b_+49, b_+51); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+51, b_+52); A = mem_rd(gb, DE);
  CYC(b_+52, b_+54); alu_cp(gb, 0x07);
  if (F & FZ) { CYCT(b_+54, b_+56); goto outsideDungeon8; }
  CYC(b_+54, b_+56);
  CALL_C(b_+56, s_setMakuTreeStageAndMapText_hook, SYM(setMakuTreeStageAndMapText), b_+59);
  CYC(b_+59, b_+61);
  goto runScript;
outsideDungeon8:
  CYC(b_+61, b_+64); SET_HL(wMakuMapTextPresent);
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x16);
  CYC(b_+66, b_+68);
  goto runScript;
outsideTempleOfWinter:
  CALL_C(b_+68, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+71);
  CYC(b_+71, b_+73); alu_and(gb, 0x40);
  if (!(F & FZ)) { CYCT(b_+73, b_+76); TAIL(interactionDelete); }
  CYC(b_+73, b_+76);
  CYC(b_+76, b_+78); A = 0x07;
  CALL_C(b_+78, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+81);
  if (!(F & FC)) { CYCT(b_+81, b_+84); TAIL(interactionDelete); }
  CYC(b_+81, b_+84);
  CYC(b_+84, b_+87); A = mem_rd(gb, wObtainedSeasons);
  CYC(b_+87, b_+88); alu_add(gb, A);
  if (F & FZ) { CYCT(b_+88, b_+91); TAIL(interactionDelete); }
  CYC(b_+88, b_+91);
haveWinterSeason:
  CALL_L(b_+91, makuCutscenes_setScript, b_+94);
  CALL_C(b_+94, s_interactionRunScript, SYM(interactionRunScript), b_+97);
  CALL_C(b_+97, s_interactionRunScript, SYM(interactionRunScript), b_+100);
  CALL_C(b_+100, s_setMakuTreeStageAndMapText_hook, SYM(setMakuTreeStageAndMapText), b_+103);
  CYC(b_+103, b_+105);
  goto runScript;
atMakuTreeGate:
  CALL_C(b_+105, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+108);
  CYC(b_+108, b_+110); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+110, b_+113); TAIL(interactionDelete); }
  CYC(b_+110, b_+113);
  CALL_L(b_+113, makuCutscenes_setScript, b_+116);
  CYC(b_+116, b_+118);
  goto runScript;

runScript:
  CALL_C(b_+136, s_interactionRunScript, SYM(interactionRunScript), b_+139);
  if (F & FC) { CYCT(b_+139, b_+142); TAIL(interactionDelete); }
  CYC(b_+139, b_+142);
  RET(b_+142); return;
state2:
  CYC(b_+143, b_+145); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+147); push_effect(gb, b_+147);
  do { uint16_t jt_ = (makuCutscenes_jump_table(gb));
    if (jt_ == b_+151) goto substate0;
    if (jt_ == b_+208) goto substate1;
    HANDOFF(HL);
  } while (0);
substate0:
  CYC(b_+151, b_+154); SET_HL(b_+253 /* @tileChangeTable */);
  CYC(b_+154, b_+156); B = 0x04;
L_5584:
  CYC(b_+156, b_+157); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+157, b_+159); mem_wr(gb, hFF8C, A);
  CYC(b_+159, b_+160); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+160, b_+162); mem_wr(gb, hFF8F, A);
  CYC(b_+162, b_+163); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+163, b_+165); mem_wr(gb, hFF8E, A);
  CYC(b_+165, b_+166); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+166, b_+167); push_effect(gb, HL);
  CYC(b_+167, b_+168); push_effect(gb, BC);
  CALL_C(b_+168, s_setInterleavedTile, SYM(setInterleavedTile), b_+171);
  CYC(b_+171, b_+172); SET_BC(pop_effect(gb));
  CYC(b_+172, b_+173); SET_HL(pop_effect(gb));
  CYC(b_+173, b_+174); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+174, b_+176); goto L_5584; }
  CYC(b_+174, b_+176);
  CYC(b_+176, b_+178); A = mem_rd(gb, hActiveObject);
  CYC(b_+178, b_+179); D = A;
  CYC(b_+179, b_+181); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+181, b_+183); A = 0x01;
  CYC(b_+183, b_+184); mem_wr(gb, DE, A);
  CYC(b_+184, b_+186); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+186, b_+188); A = 0x1e;
  CYC(b_+188, b_+189); mem_wr(gb, DE, A);
  CYC(b_+189, b_+190); alu_xor(gb, A);
  CALL_L(b_+190, makuCutscenes_func_561f, b_+193);
  CYC(b_+193, b_+195); A = 0x73;
  CALL_C(b_+195, s_playSound, SYM(playSound_b00), b_+198);
rumble:
  CYC(b_+198, b_+200); A = 0x06;
  CALL_C(b_+200, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+203);
  CYC(b_+203, b_+205); A = 0x70;
  CYC(b_+205, b_+208);
  TAIL(playSound_b00);
substate1:
  CALL_C(b_+208, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+211);
  if (!(F & FZ)) { RET_TAKEN(b_+211); return; }
  CYC(b_+211, b_+212);
  CYC(b_+212, b_+215); SET_HL(b_+253 /* @tileChangeTable */);
  CYC(b_+215, b_+217); B = 0x04;
L_55c1:
  CYC(b_+217, b_+218); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+218, b_+219); C = A;
  CYC(b_+219, b_+220); A = mem_rd(gb, HL);
  CYC(b_+220, b_+221); push_effect(gb, HL);
  CYC(b_+221, b_+222); push_effect(gb, BC);
  CALL_C(b_+222, s_setTile, SYM(setTile), b_+225);
  CYC(b_+225, b_+226); SET_BC(pop_effect(gb));
  CYC(b_+226, b_+227); SET_HL(pop_effect(gb));
  CYC(b_+227, b_+228); SET_HL(HL + 1);
  CYC(b_+228, b_+229); SET_HL(HL + 1);
  CYC(b_+229, b_+230); SET_HL(HL + 1);
  CYC(b_+230, b_+231); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+231, b_+233); goto L_55c1; }
  CYC(b_+231, b_+233);
  CYC(b_+233, b_+235); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+235, b_+237); A = 0x01;
  CYC(b_+237, b_+238); mem_wr(gb, DE, A);
  CYC(b_+238, b_+239); alu_xor(gb, A);
  CYC(b_+239, b_+240); E = alu_inc8(gb, E);
  CYC(b_+240, b_+241); mem_wr(gb, DE, A);
  CYC(b_+241, b_+243); A = 0x04;
  CALL_L(b_+243, makuCutscenes_func_561f, b_+246);
  CYC(b_+246, b_+248); A = 0x73;
  CALL_C(b_+248, s_playSound, SYM(playSound_b00), b_+251);
  CYC(b_+251, b_+253);
  goto rumble;
}

