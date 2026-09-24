#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/9a.s.
// INTERAC_9a

static uint16_t i9a_jump_table(GB *gb) {
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

static void i9a_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void i9a_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// func_5b49@func_5b65
static void i9a_func_5b65(GB *gb) {
  BASE(func_5b49);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+28, b_+30); alu_bit(gb, 6, A);
  CYC(b_+30, b_+32); mem_wr(gb, HL, mem_rd(gb, HL) | 0x40);
  if (F & FZ) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+35, b_+37); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);
  RET(b_+37); return;
}

// interactionCode9a@func_59ba
static void i9a_func_59ba(GB *gb) {
  BASE(interactionCode9a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+303, b_+305); B = 0x03;
L_59bc:
  CALL_C(b_+305, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+308);
  if (!(F & FZ)) { CYCT(b_+308, b_+310); goto ret; }
  CYC(b_+308, b_+310);
  CYC(b_+310, b_+311); A = mem_rd(gb, DE);
  CYC(b_+311, b_+312); SET_DE(DE + 1);
  CYC(b_+312, b_+313); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+313, b_+314); A = mem_rd(gb, DE);
  CYC(b_+314, b_+315); SET_DE(DE + 1);
  CYC(b_+315, b_+316); mem_wr(gb, HL, A);
  CYC(b_+316, b_+318); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+318, b_+319); A = mem_rd(gb, DE);
  CYC(b_+319, b_+320); SET_DE(DE + 1);
  CYC(b_+320, b_+321); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+321, b_+322); L = alu_inc8(gb, L);
  CYC(b_+322, b_+323); A = mem_rd(gb, DE);
  CYC(b_+323, b_+324); SET_DE(DE + 1);
  CYC(b_+324, b_+325); mem_wr(gb, HL, A);
  CYC(b_+325, b_+327); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+327, b_+329); mem_wr(gb, HL, 0x0a);
  CYC(b_+329, b_+330); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+330, b_+332); goto L_59bc; }
  CYC(b_+330, b_+332);
ret:
  RET(b_+332); return;
}

// INTERAC_9a
void s_interactionCode9a_hook(GB *gb) {
  BASE(interactionCode9a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i9a_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+118) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x16;
  CALL_C(b_+10, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+13);
  if (F & FZ) { CYCT(b_+13, b_+16); TAIL(interactionDelete); }
  CYC(b_+13, b_+16);
  CYC(b_+16, b_+18); A = 0x01;
  CYC(b_+18, b_+19); mem_wr(gb, DE, A);
  CYC(b_+19, b_+21); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+21, b_+22); A = mem_rd(gb, DE);
  CYC(b_+22, b_+24); alu_cp(gb, 0x02);
  if (F & FZ) { CYCT(b_+24, b_+26); goto state0_subid2; }
  CYC(b_+24, b_+26);
  CALL_C(b_+26, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+29);
  CYC(b_+29, b_+32);
  TAIL(objectSetVisible82);
state0_subid2:
  CYC(b_+32, b_+35); A = mem_rd(gb, wcdd7);
  CYC(b_+35, b_+36); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+36, b_+39); TAIL(interactionDelete); }
  CYC(b_+36, b_+39);
  CYC(b_+39, b_+40); alu_xor(gb, A);
  CYC(b_+40, b_+43); mem_wr(gb, wcdd7, A);
  CYC(b_+43, b_+45); A = 0x12;
  CALL_C(b_+45, s_unsetGlobalFlag, SYM(unsetGlobalFlag), b_+48);
  CALL_C(b_+48, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+51);
  CYC(b_+51, b_+52); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+52, b_+54); goto bit7NotSet; }
  CYC(b_+52, b_+54);
  CYC(b_+54, b_+56); A = 0x12;
  CALL_C(b_+56, s_setGlobalFlag, SYM(setGlobalFlag), b_+59);
  CYC(b_+59, b_+60); alu_xor(gb, A);
  CYC(b_+60, b_+63); SET_HL(w1Companion);
  CYC(b_+63, b_+64); mem_wr(gb, HL, A);
  CYC(b_+64, b_+66); L = 0x1a;
  CYC(b_+66, b_+67); mem_wr(gb, HL, A);
  CYC(b_+67, b_+68); push_effect(gb, DE);
  CYC(b_+68, b_+71); SET_DE(b_+345 /* @table_59e4 */);
  CALL_L(b_+71, i9a_func_59ba, b_+74);
  CYC(b_+74, b_+75); SET_DE(pop_effect(gb));
  CYC(b_+75, b_+77); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+77, b_+79); A = 0x03;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+82); A = 0xb9;
  CYC(b_+82, b_+84);
  goto L_58f0;
bit7NotSet:
  CALL_C(b_+84, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+87);
  CALL_L(b_+87, i9a_func_5b65, b_+90);
  CYC(b_+90, b_+91); C = mem_rd(gb, HL);
  CYC(b_+91, b_+93); A = 0x03;
  CYC(b_+93, b_+95); B = 0xaa;
  CALL_C(b_+95, s_getRoomFlags, SYM(getRoomFlags), b_+98);
  CYC(b_+98, b_+99); mem_wr(gb, HL, C);
  CYC(b_+99, b_+101); A = 0xb4;
L_58f0:
  CYC(b_+101, b_+102); H = D;
  CYC(b_+102, b_+104); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+104, b_+105); mem_wr(gb, HL, A);
  CYC(b_+105, b_+107); A = 0x01;
  CYC(b_+107, b_+110); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+110, b_+113); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+113, b_+115); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+115, b_+117); mem_wr(gb, HL, 0x5a);
  RET(b_+117); return;
state1:
  CYC(b_+118, b_+120); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+120, b_+121); A = mem_rd(gb, DE);
  CYC(b_+121, b_+122); push_effect(gb, b_+122);
  do { uint16_t jt_ = (i9a_jump_table(gb));
    if (jt_ == b_+130) goto state1_subid0;
    if (jt_ == b_+235) goto state1_subid1;
    if (jt_ == b_+247) goto state1_subid2;
    if (jt_ == SYM(state1_subid3) && hook_is(gb, SYM(state1_subid3), s_state1_subid3_hook)) { s_state1_subid3_hook(gb); return; }
    HANDOFF(HL);
  } while (0);
state1_subid0:
  CALL_C(b_+130, s_interactionAnimate, SYM(interactionAnimate), b_+133);
  CYC(b_+133, b_+135); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+135, b_+136); A = mem_rd(gb, DE);
  CYC(b_+136, b_+137); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+137); return; }
  CYC(b_+137, b_+138);
  CYC(b_+138, b_+139); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(b_+139, b_+142); TAIL(interactionDelete); }
  CYC(b_+139, b_+142);
  CYC(b_+142, b_+143); alu_xor(gb, A);
  CYC(b_+143, b_+144); mem_wr(gb, DE, A);
  CYC(b_+144, b_+146); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+146, b_+147); A = mem_rd(gb, DE);
  CYC(b_+147, b_+148); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+148); return; }
  CYC(b_+148, b_+149);
  CYC(b_+149, b_+150); A = alu_dec8(gb, A);
  CYC(b_+150, b_+153); SET_HL(SYM(table_5a5e));
  CYC(b_+153, b_+154); i9a_add_double_index(gb, b_+154);
  CYC(b_+154, b_+155); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+155, b_+156); H = mem_rd(gb, HL);
  CYC(b_+156, b_+157); L = A;
  CYC(b_+157, b_+158); push_effect(gb, DE);
  CYC(b_+158, b_+159); B = mem_rd(gb, HL);
  CYC(b_+159, b_+160); SET_HL(HL + 1);
L_592b:
  CYC(b_+160, b_+161); C = mem_rd(gb, HL);
  CYC(b_+161, b_+162); SET_HL(HL + 1);
  CYC(b_+162, b_+163); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+163, b_+164); push_effect(gb, BC);
  CYC(b_+164, b_+165); push_effect(gb, HL);
  CALL_C(b_+165, s_setTile, SYM(setTile), b_+168);
  CYC(b_+168, b_+169); SET_HL(pop_effect(gb));
  CYC(b_+169, b_+170); SET_BC(pop_effect(gb));
  CYC(b_+170, b_+171); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+171, b_+173); goto L_592b; }
  CYC(b_+171, b_+173);
  CYC(b_+173, b_+174); SET_DE(pop_effect(gb));
  CALL_C(b_+174, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+177);
  CYC(b_+177, b_+179); alu_and(gb, 0x03);
  CYC(b_+179, b_+181); alu_add(gb, 0x02);
  CYC(b_+181, b_+182); C = A;
  CYC(b_+182, b_+184); B = 0x04;
L_5943:
  CALL_C(b_+184, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+187);
  if (!(F & FZ)) { RET_TAKEN(b_+187); return; }
  CYC(b_+187, b_+188);
  CYC(b_+188, b_+190); mem_wr(gb, HL, 0x9a);
  CYC(b_+190, b_+191); L = alu_inc8(gb, L);
  CYC(b_+191, b_+193); mem_wr(gb, HL, 0x01);
  CYC(b_+193, b_+194); A = B;
  CYC(b_+194, b_+195); alu_add(gb, A);
  CYC(b_+195, b_+196); alu_add(gb, A);
  CYC(b_+196, b_+197); alu_add(gb, A);
  CYC(b_+197, b_+198); alu_add(gb, C);
  CYC(b_+198, b_+200); alu_and(gb, 0x1f);
  CYC(b_+200, b_+202); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(b_+202, b_+203); mem_wr(gb, HL, A);
  CALL_C(b_+203, s_getRandomNumber, SYM(getRandomNumber), b_+206);
  CYC(b_+206, b_+208); alu_and(gb, 0x03);
  CYC(b_+208, b_+209); push_effect(gb, HL);
  CYC(b_+209, b_+212); SET_HL(b_+231 /* @state1@table_5972 */);
  CYC(b_+212, b_+213); i9a_add_a_to_hl(gb, b_+213);
  CYC(b_+213, b_+214); A = mem_rd(gb, HL);
  CYC(b_+214, b_+215); SET_HL(pop_effect(gb));
  CYC(b_+215, b_+217); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+217, b_+218); mem_wr(gb, HL, A);
  CYC(b_+218, b_+221); SET_BC(0xfe80);
  CALL_C(b_+221, s_objectSetSpeedZ, SYM(objectSetSpeedZ), b_+224);
  CALL_C(b_+224, s_objectCopyPosition, SYM(objectCopyPosition), b_+227);
  CYC(b_+227, b_+228); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+228, b_+230); goto L_5943; }
  CYC(b_+228, b_+230);
  RET(b_+230); return;

state1_subid1:
  CALL_C(b_+235, s_objectApplySpeed, SYM(objectApplySpeed), b_+238);
  CYC(b_+238, b_+240); C = 0x28;
  CALL_C(b_+240, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+243);
  if (F & FZ) { CYCT(b_+243, b_+246); TAIL(interactionDelete); }
  CYC(b_+243, b_+246);
  RET(b_+246); return;
state1_subid2:
  CYC(b_+247, b_+249); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+249, b_+250); A = mem_rd(gb, DE);
  CYC(b_+250, b_+251); push_effect(gb, b_+251);
  do { uint16_t jt_ = (i9a_jump_table(gb));
    if (jt_ == b_+257) goto state1_subid2_substate0;
    if (jt_ == b_+266) goto state1_subid2_substate1;
    if (jt_ == b_+283) goto state1_subid2_substate2;
    HANDOFF(HL);
  } while (0);
state1_subid2_substate0:
  CALL_C(b_+257, s_returnIfScrollMode01Unset, SYM(returnIfScrollMode01Unset), b_+260);
  CYC(b_+260, b_+262); A = 0x01;
  CYC(b_+262, b_+264); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+264, b_+265); mem_wr(gb, DE, A);
  RET(b_+265); return;
state1_subid2_substate1:
  CYC(b_+266, b_+267); H = D;
  CYC(b_+267, b_+269); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+269, b_+270); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+270, b_+272); goto state1_subid2_substate2; }
  CYC(b_+270, b_+272);
  CYC(b_+272, b_+274); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+274, b_+275); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+275, b_+276); push_effect(gb, DE);
  CYC(b_+276, b_+279); SET_DE(b_+333 /* @table_59d8 */);
  CALL_L(b_+279, i9a_func_59ba, b_+282);
  CYC(b_+282, b_+283); SET_DE(pop_effect(gb));
state1_subid2_substate2:
  CYC(b_+283, b_+284); alu_xor(gb, A);
  CALL_C(b_+284, s_func_5a82_hook, SYM(func_5a82), b_+287);
  if (!(F & FZ)) { RET_TAKEN(b_+287); return; }
  CYC(b_+287, b_+288);
  CYC(b_+288, b_+291); SET_HL(wSeedTreeRefilledBitset);
  CYC(b_+291, b_+293); mem_wr(gb, HL, mem_rd(gb, HL) & 0xfd);
  CYC(b_+293, b_+294); alu_xor(gb, A);
  CYC(b_+294, b_+297); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+297, b_+300); mem_wr(gb, wMenuDisabled, A);
  CYC(b_+300, b_+303);
  TAIL(interactionDelete);
}

void s_state1_subid3_hook(GB *gb) {
  BASE(state1_subid3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i9a_jump_table(gb));
    if (jt_ == b_+12) goto substate0;
    if (jt_ == b_+21) goto substate1;
    if (jt_ == b_+37) goto substate2;
    if (jt_ == b_+94) goto substate3;
    HANDOFF(HL);
  } while (0);
substate0:
  CALL_C(b_+12, s_returnIfScrollMode01Unset, SYM(returnIfScrollMode01Unset), b_+15);
  CYC(b_+15, b_+17); A = 0x01;
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+19, b_+20); mem_wr(gb, DE, A);
  RET(b_+20); return;
substate1:
  CYC(b_+21, b_+22); H = D;
  CYC(b_+22, b_+24); L = INTERACTION_BASE + OBJ_USE_TEXT_ID;
  CYC(b_+24, b_+25); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { CYCT(b_+25, b_+27); goto L_5a11; }
  CYC(b_+25, b_+27);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+30, s_fadeoutToWhite, SYM(fadeoutToWhite), b_+33);
L_5a11:
  CYC(b_+33, b_+34); alu_xor(gb, A);
  CYC(b_+34, b_+37);
  TAIL_S(func_5a82);
substate2:
  CYC(b_+37, b_+40); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+40, b_+41); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+41); return; }
  CYC(b_+41, b_+42);
  CYC(b_+42, b_+44); mem_wr(gb, hSprPaletteSources, A);
  CYC(b_+44, b_+45); A = alu_dec8(gb, A);
  CYC(b_+45, b_+47); mem_wr(gb, hDirtySprPalettes, A);
  CYC(b_+47, b_+50); mem_wr(gb, wTmpcfc0 + 0x10, A);
  CALL_C(b_+50, s_interactionIncSubstate, SYM(interactionIncSubstate), b_+53);
  CYC(b_+53, b_+55); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+55, b_+57); mem_wr(gb, HL, 0x1e);
  CYC(b_+57, b_+60); SET_HL(w1Link_yh);
  CYC(b_+60, b_+62); A = 0x40;
  CYC(b_+62, b_+63); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+63, b_+64); L = alu_inc8(gb, L);
  CYC(b_+64, b_+66); mem_wr(gb, HL, 0x50);
  CYC(b_+66, b_+68); A = 0x80;
  CYC(b_+68, b_+71); mem_wr(gb, w1Link_visible, A);
  CYC(b_+71, b_+73); A = 0x02;
  CYC(b_+73, b_+76); mem_wr(gb, w1Link_direction, A);
  CALL_C(b_+76, s_setLinkForceStateToState08, SYM(setLinkForceStateToState08), b_+79);
  CYC(b_+79, b_+80); push_effect(gb, DE);
  CALL_C(b_+80, s_hideStatusBar, SYM(hideStatusBar), b_+83);
  CYC(b_+83, b_+84); SET_DE(pop_effect(gb));
  CYC(b_+84, b_+86); C = 0x02;
  CYC(b_+86, b_+89); SET_HL(SYM(loadDeathRespawnBufferPreset));
  CYC(b_+89, b_+91); E = 0x01;
  CYC(b_+91, b_+94);
  TAIL(interBankCall);
substate3:
  CALL_C(b_+94, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+97);
  if (!(F & FZ)) { RET_TAKEN(b_+97); return; }
  CYC(b_+97, b_+98);
  CYC(b_+98, b_+100); A = 0x03;
  CYC(b_+100, b_+103); mem_wr(gb, wLinkForceState, A);
  CYC(b_+103, b_+104); alu_xor(gb, A);
  CYC(b_+104, b_+107); mem_wr(gb, wLinkHealth, A);
  CYC(b_+107, b_+110);
  TAIL(interactionDelete);
}

void s_func_5a82_hook(GB *gb) {
  BASE(func_5a82);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+4); L = E;
  CYC(b_+4, b_+5); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+9); SET_HL(SYM(table_5ac4));
  CYC(b_+9, b_+10); i9a_add_double_index(gb, b_+10);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+11, b_+12); H = mem_rd(gb, HL);
  CYC(b_+12, b_+13); L = A;
  CYC(b_+13, b_+14); E = alu_inc8(gb, E);
  CYC(b_+14, b_+15); A = mem_rd(gb, DE);
  CYC(b_+15, b_+16); alu_add(gb, A);
  CYC(b_+16, b_+17); i9a_add_double_index(gb, b_+17);
  CYC(b_+17, b_+19); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+21); mem_wr(gb, DE, A);
  CYC(b_+21, b_+22); A = alu_inc8(gb, A);
  if (F & FZ) { RET_TAKEN(b_+22); return; }
  CYC(b_+22, b_+23);
  CYC(b_+23, b_+24); E = alu_inc8(gb, E);
  CYC(b_+24, b_+25); A = mem_rd(gb, DE);
  CYC(b_+25, b_+26); A = alu_inc8(gb, A);
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CYC(b_+27, b_+28); push_effect(gb, DE);
  CYC(b_+28, b_+29); D = H;
  CYC(b_+29, b_+30); E = L;
  CALL_C(b_+30, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+33);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto L_5ac0; }
  CYC(b_+33, b_+35);
  CYC(b_+35, b_+37); mem_wr(gb, HL, 0x9a);
  CYC(b_+37, b_+38); L = alu_inc8(gb, L);
  CYC(b_+38, b_+40); mem_wr(gb, HL, 0x00);
  CYC(b_+40, b_+41); L = alu_inc8(gb, L);
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+43); SET_DE(DE + 1);
  CYC(b_+43, b_+44); mem_wr(gb, HL, A);
  CYC(b_+44, b_+46); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+46, b_+47); A = mem_rd(gb, DE);
  CYC(b_+47, b_+48); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+48, b_+49); SET_DE(DE + 1);
  CYC(b_+49, b_+50); L = alu_inc8(gb, L);
  CYC(b_+50, b_+51); A = mem_rd(gb, DE);
  CYC(b_+51, b_+52); mem_wr(gb, HL, A);
  CYC(b_+52, b_+54); A = 0x6f;
  CALL_C(b_+54, s_playSound, SYM(playSound_b00), b_+57);
  CYC(b_+57, b_+59); A = 0x08;
  CALL_C(b_+59, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+62);
L_5ac0:
  CYC(b_+62, b_+63); SET_DE(pop_effect(gb));
  CYC(b_+63, b_+65); alu_or(gb, 0x01);
  RET(b_+65); return;
}

