#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/sokra.s.
// INTERAC_SOKRA

static uint16_t sokra_jump_table(GB *gb) {
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

// interactionCode27@state1@func_5bae
static void sokra_state1_func_5bae(GB *gb) {
  BASE(interactionCode27);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+178, b_+180); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+180, b_+182); A = 0x01;
  CYC(b_+182, b_+183); mem_wr(gb, DE, A);
  RET(b_+183); return;
}

// INTERAC_SOKRA
void s_interactionCode27_hook(GB *gb) {
  BASE(interactionCode27);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (sokra_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+127) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CYC(b_+11, b_+13); A = 0x28;
  CALL_C(b_+13, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+16);
  if (!(F & FZ)) { CYCT(b_+16, b_+19); TAIL(interactionDelete); }
  CYC(b_+16, b_+19);
  CYC(b_+19, b_+21); A = 0x52;
  CALL_C(b_+21, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+24);
  CALL_C(b_+24, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+27);
  CYC(b_+27, b_+29); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+29, b_+30); A = mem_rd(gb, DE);
  CYC(b_+30, b_+31); push_effect(gb, b_+31);
  do { uint16_t jt_ = (sokra_jump_table(gb));
    if (jt_ == b_+46) goto state0_subid0;
    if (jt_ == b_+71) goto state0_subid1;
    if (jt_ == b_+102) goto state0_subid2;
    HANDOFF(HL);
  } while (0);
state0_runScriptSetVisible:
  CALL_C(b_+37, s_interactionRunScript, SYM(interactionRunScript), b_+40);
  CALL_C(b_+40, s_interactionRunScript, SYM(interactionRunScript), b_+43);
  CYC(b_+43, b_+46);
  TAIL(objectSetVisiblec2);
state0_subid0:
  CYC(b_+46, b_+49); A = mem_rd(gb, wObtainedSeasons);
  CYC(b_+49, b_+51); alu_and(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+51, b_+53); goto L_5b3b; }
  CYC(b_+51, b_+53);
  CYC(b_+53, b_+55); A = 0x71;
  CALL_C(b_+55, s_getARoomFlags, SYM(getARoomFlags), b_+58);
  CYC(b_+58, b_+60); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+60, b_+63); TAIL(interactionDelete); }
  CYC(b_+60, b_+63);
L_5b3b:
  CYC(b_+63, b_+66); SET_HL(SYM(sokraScript_inVillage));
  CALL_C(b_+66, s_interactionSetScript, SYM(interactionSetScript), b_+69);
  CYC(b_+69, b_+71);
  goto state0_runScriptSetVisible;
state0_subid1:
  CYC(b_+71, b_+74); A = mem_rd(gb, wObtainedSeasons);
  CYC(b_+74, b_+76); alu_and(gb, 0x08);
  if (F & FZ) { CYCT(b_+76, b_+78); goto L_5b59; }
  CYC(b_+76, b_+78);
  CALL_C(b_+78, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+81);
  CYC(b_+81, b_+83); alu_bit(gb, 6, A);
  if (!(F & FZ)) { CYCT(b_+83, b_+85); goto L_5b59; }
  CYC(b_+83, b_+85);
  CYC(b_+85, b_+88); SET_HL(SYM(sokraScript_easternSuburbsPortal));
  CALL_C(b_+88, s_interactionSetScript, SYM(interactionSetScript), b_+91);
  CYC(b_+91, b_+93);
  goto state0_runScriptSetVisible;
L_5b59:
  CYC(b_+93, b_+96); SET_HL(SYM(objectData7e4a));
  CALL_C(b_+96, s_parseGivenObjectData, SYM(parseGivenObjectData_b00), b_+99);
  CYC(b_+99, b_+102);
  TAIL(interactionDelete);
state0_subid2:
  CALL_C(b_+102, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+105);
  CYC(b_+105, b_+108); A = mem_rd(gb, wObtainedSeasons);
  CYC(b_+108, b_+110); alu_and(gb, 0x02);
  if (F & FZ) { CYCT(b_+110, b_+112); goto L_5b71; }
  CYC(b_+110, b_+112);
  CYC(b_+112, b_+114); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+114, b_+117);
  TAIL(interactionDelete);
L_5b71:
  CYC(b_+117, b_+119); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  CYC(b_+119, b_+122); SET_HL(SYM(sokraScript_needSummerForD3));
  CALL_C(b_+122, s_interactionSetScript, SYM(interactionSetScript), b_+125);
  CYC(b_+125, b_+127);
  goto state0_runScriptSetVisible;
state1:
  CYC(b_+127, b_+129); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+131); push_effect(gb, b_+131);
  do { uint16_t jt_ = (sokra_jump_table(gb));
    if (jt_ == b_+137) goto state1_subid0;
    if (jt_ == b_+197) goto state1_subid1;
    if (jt_ == b_+225) goto state1_subid2;
    HANDOFF(HL);
  } while (0);
state1_subid0:
  CALL_C(b_+137, s_interactionRunScript, SYM(interactionRunScript), b_+140);
  CALL_C(b_+140, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+143);
  CYC(b_+143, b_+145); A = 0x19;
  CALL_C(b_+145, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+148);
  if (!(F & FC)) { RET_TAKEN(b_+148); return; }
  CYC(b_+148, b_+149);
  CALL_C(b_+149, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+152);
  CYC(b_+152, b_+154); alu_bit(gb, 6, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+154, b_+156); goto L_5b9f; }
  CYC(b_+154, b_+156);
  CYC(b_+156, b_+158); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+158, b_+159); A = mem_rd(gb, DE);
  CYC(b_+159, b_+160); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+160, b_+162); goto L_5bb4; }
  CYC(b_+160, b_+162);
  RET(b_+162); return;
L_5b9f:
  CYC(b_+163, b_+166); A = mem_rd(gb, w1Link_xh);
  CYC(b_+166, b_+168); alu_cp(gb, 0x78);
  if (F & FC) { RET_TAKEN(b_+168); return; }
  CYC(b_+168, b_+169);
  CYC(b_+169, b_+172); A = mem_rd(gb, w1Link_yh);
  CYC(b_+172, b_+174); alu_cp(gb, 0x3c);
  if (F & FC) { RET_TAKEN(b_+174); return; }
  CYC(b_+174, b_+175);
  CYC(b_+175, b_+177); alu_cp(gb, 0x60);
  if (!(F & FC)) { RET_TAKEN(b_+177); return; }
  CYC(b_+177, b_+178);
  CYC(b_+178, b_+180); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+180, b_+182); A = 0x01;
  CYC(b_+182, b_+183); mem_wr(gb, DE, A);
  RET(b_+183); return;
L_5bb4:
  CYC(b_+184, b_+187); A = mem_rd(gb, wFrameCounter);
  CYC(b_+187, b_+189); alu_and(gb, 0x3f);
  if (!(F & FZ)) { RET_TAKEN(b_+189); return; }
  CYC(b_+189, b_+190);
  CYC(b_+190, b_+192); B = 0xf4;
  CYC(b_+192, b_+194); C = 0xfa;
  CYC(b_+194, b_+197);
  TAIL(objectCreateFloatingMusicNote);
state1_subid1:
  CALL_C(b_+197, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+200);
  CALL_C(b_+200, s_interactionRunScript, SYM(interactionRunScript), b_+203);
  if (F & FC) { CYCT(b_+203, b_+206); TAIL(interactionDelete); }
  CYC(b_+203, b_+206);
  CALL_C(b_+206, s_checkInteractionSubstate, SYM(checkInteractionSubstate), b_+209);
  if (!(F & FZ)) { RET_TAKEN(b_+209); return; }
  CYC(b_+209, b_+210);
  CYC(b_+210, b_+213); A = mem_rd(gb, w1Link_xh);
  CYC(b_+213, b_+215); alu_cp(gb, 0x18);
  if (F & FC) { RET_TAKEN(b_+215); return; }
  CYC(b_+215, b_+216);
  CALL_C(b_+216, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+219);
  CALL_L(b_+219, sokra_state1_func_5bae, b_+222);
  CYC(b_+222, b_+225);
  TAIL_S(beginJump);
state1_subid2:
  CYC(b_+225, b_+228); A = mem_rd(gb, wDisabledObjects);
  CYC(b_+228, b_+230); alu_and(gb, 0x01);
  if (F & FZ) CALL_C_CC(b_+230, s_createSokraSnore_hook, SYM(createSokraSnore), b_+233);
  else CYC(b_+230, b_+233);
  CALL_C(b_+233, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+236);
  CYC(b_+236, b_+239);
  TAIL(interactionRunScript);
}

