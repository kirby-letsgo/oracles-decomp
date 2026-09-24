#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/sunkenCityBullies.s.
// INTERAC_SUNKEN_CITY_BULLIES

static uint16_t sunkenCityBullies_jump_table(GB *gb) {
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

static void sunkenCityBullies_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void sunkenCityBullies_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_SUNKEN_CITY_BULLIES
void s_interactionCode76_hook(GB *gb) {
  BASE(interactionCode76);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (sunkenCityBullies_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == b_+211) goto state1;
    if (jt_ == b_+151) goto state2;
    if (jt_ == b_+200) goto state3;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CYC(b_+15, b_+17); A = 0x28;
  CALL_C(b_+17, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+20);
  if (!(F & FZ)) { CYCT(b_+20, b_+23); goto delete; }
  CYC(b_+20, b_+23);
  CYC(b_+23, b_+26); A = mem_rd(gb, wDimitriState);
  CYC(b_+26, b_+28); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+28, b_+30); goto func_7375; }
  CYC(b_+28, b_+30);
  CYC(b_+30, b_+32); A = 0x03;
  CYC(b_+32, b_+33); mem_wr(gb, DE, A);
  CALL_C(b_+33, s_func_745b_hook, SYM(func_745b), b_+36);
  CYC(b_+36, b_+38); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+38, b_+39); A = mem_rd(gb, DE);
  CYC(b_+39, b_+41); alu_and(gb, 0x1f);
  CYC(b_+41, b_+43); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+43, b_+44); A = mem_rd(gb, DE);
  CYC(b_+44, b_+46); alu_and(gb, 0x1f);
  CYC(b_+46, b_+47); C = A;
  CYC(b_+47, b_+50); SET_HL(SYM(table_748f));
  CYC(b_+50, b_+51); sunkenCityBullies_add_double_index(gb, b_+51);
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+53, b_+54); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+54, b_+55); mem_wr(gb, DE, A);
  CYC(b_+55, b_+57); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+57, b_+58); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+58, b_+59); mem_wr(gb, DE, A);
  CYC(b_+59, b_+60); A = C;
  CYC(b_+60, b_+63); SET_HL(SYM(table_7483));
  CYC(b_+63, b_+64); sunkenCityBullies_add_double_index(gb, b_+64);
  CYC(b_+64, b_+65); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+65, b_+66); H = mem_rd(gb, HL);
  CYC(b_+66, b_+67); L = A;
  CALL_C(b_+67, s_interactionSetScript, SYM(interactionSetScript), b_+70);
  CYC(b_+70, b_+71); A = C;
  CYC(b_+71, b_+74); SET_HL(SYM(table_7495));
  CYC(b_+74, b_+75); sunkenCityBullies_add_a_to_hl(gb, b_+75);
  CYC(b_+75, b_+76); A = mem_rd(gb, HL);
  CYC(b_+76, b_+79);
  TAIL(interactionSetAnimation);
func_7375:
  CYC(b_+79, b_+82); SET_HL(w1Companion_id);
  CYC(b_+82, b_+83); A = mem_rd(gb, HL);
  CYC(b_+83, b_+85); alu_cp(gb, 0x0c);
  if (!(F & FZ)) { CYCT(b_+85, b_+87); goto delete; }
  CYC(b_+85, b_+87);
  CYC(b_+87, b_+90); A = mem_rd(gb, wAnimalCompanion);
  CYC(b_+90, b_+92); alu_cp(gb, 0x0c);
  if (F & FZ) { CYCT(b_+92, b_+94); goto delete; }
  CYC(b_+92, b_+94);
  CYC(b_+94, b_+97); A = mem_rd(gb, wDimitriState);
  CYC(b_+97, b_+99); alu_bit(gb, 5, A);
  if (F & FZ) { CYCT(b_+99, b_+101); goto delete; }
  CYC(b_+99, b_+101);
  CYC(b_+101, b_+103); alu_bit(gb, 4, A);
  if (!(F & FZ)) { CYCT(b_+103, b_+105); goto delete; }
  CYC(b_+103, b_+105);
  CALL_C(b_+105, s_func_745b_hook, SYM(func_745b), b_+108);
  CYC(b_+108, b_+110); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+113); alu_and(gb, 0x1f);
  CYC(b_+113, b_+114); C = A;
  CYC(b_+114, b_+117); SET_HL(SYM(table_7489));
  CYC(b_+117, b_+118); sunkenCityBullies_add_double_index(gb, b_+118);
  CYC(b_+118, b_+120); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+120, b_+121); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+121, b_+122); mem_wr(gb, DE, A);
  CYC(b_+122, b_+124); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+124, b_+125); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+125, b_+126); mem_wr(gb, DE, A);
  CYC(b_+126, b_+127); A = C;
  CYC(b_+127, b_+130); SET_HL(SYM(table_7477));
  CYC(b_+130, b_+131); sunkenCityBullies_add_double_index(gb, b_+131);
  CYC(b_+131, b_+132); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+132, b_+133); H = mem_rd(gb, HL);
  CYC(b_+133, b_+134); L = A;
  CALL_C(b_+134, s_interactionSetScript, SYM(interactionSetScript), b_+137);
  CYC(b_+137, b_+139); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+139, b_+140); A = mem_rd(gb, DE);
  CYC(b_+140, b_+142); alu_and(gb, 0x1f);
  if (F & FZ) CALL_C_CC(b_+142, s_func_743e_hook, SYM(func_743e), b_+145);
  else CYC(b_+142, b_+145);
  CYC(b_+145, b_+147); A = 0x78;
  CYC(b_+147, b_+150); mem_wr(gb, wInstrumentsDisabledCounter, A);
  RET(b_+150); return;
state2:
  CYC(b_+151, b_+153); C = 0x40;
  CALL_C(b_+153, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+156);
  if (F & FZ) { CYCT(b_+156, b_+158); goto L_73c6; }
  CYC(b_+156, b_+158);
  CYC(b_+158, b_+160); A = 0x01;
L_73c6:
  CYC(b_+160, b_+162); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+162, b_+163); mem_wr(gb, DE, A);
  CALL_C(b_+163, s_interactionAnimate, SYM(interactionAnimate), b_+166);
  CALL_C(b_+166, s_objectSetPriorityRelativeToLink_withTerrainEffects, SYM(objectSetPriorityRelativeToLink_withTerrainEffects), b_+169);
  CALL_C(b_+169, s_interactionRunScript, SYM(interactionRunScript), b_+172);
  CYC(b_+172, b_+174); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+174, b_+175); A = mem_rd(gb, DE);
  CYC(b_+175, b_+177); alu_bit(gb, 7, A);
  if (F & FZ) { RET_TAKEN(b_+177); return; }
  CYC(b_+177, b_+178);
  CYC(b_+178, b_+180); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+180, b_+181); A = mem_rd(gb, DE);
  CYC(b_+181, b_+183); alu_and(gb, 0x1f);
  CYC(b_+183, b_+185); alu_cp(gb, 0x01);
  if (!(F & FZ)) { CYCT(b_+185, b_+187); goto delete; }
  CYC(b_+185, b_+187);
  CYC(b_+187, b_+188); alu_xor(gb, A);
  CYC(b_+188, b_+191); mem_wr(gb, wTextIsActive, A);
  CYC(b_+191, b_+194); mem_wr(gb, wDisabledObjects, A);
  CYC(b_+194, b_+197); mem_wr(gb, wMenuDisabled, A);
delete:
  CYC(b_+197, b_+200);
  TAIL(interactionDelete);
state3:
  CYC(b_+200, b_+203); A = mem_rd(gb, wScrollMode);
  CYC(b_+203, b_+205); alu_and(gb, 0x0e);
  if (!(F & FZ)) { RET_TAKEN(b_+205); return; }
  CYC(b_+205, b_+206);
  CALL_C(b_+206, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+209);
  CYC(b_+209, b_+211);
  goto L_7411;
state1:
  CYC(b_+211, b_+214); A = mem_rd(gb, wScrollMode);
  CYC(b_+214, b_+216); alu_and(gb, 0x0e);
  if (!(F & FZ)) { RET_TAKEN(b_+216); return; }
  CYC(b_+216, b_+217);
  CALL_C(b_+217, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+220);
  CYC(b_+220, b_+222); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+222, b_+223); A = mem_rd(gb, DE);
  CYC(b_+223, b_+225); alu_and(gb, 0x1f);
  if (F & FZ) CALL_C_CC(b_+225, s_func_743e_hook, SYM(func_743e), b_+228);
  else CYC(b_+225, b_+228);
  CYC(b_+228, b_+231); A = mem_rd(gb, wDimitriState);
  CYC(b_+231, b_+233); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+233, b_+235); TAIL_S(func_742a); }
  CYC(b_+233, b_+235);
L_7411:
  CYC(b_+235, b_+238); A = mem_rd(gb, wPaletteThread_mode);
  CYC(b_+238, b_+239); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+239); return; }
  CYC(b_+239, b_+240);
  CYC(b_+240, b_+242); C = 0x40;
  CALL_C(b_+242, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+245);
  if (F & FZ) { CYCT(b_+245, b_+247); goto L_7424; }
  CYC(b_+245, b_+247);
  CYC(b_+247, b_+249); A = 0xc0;
  CYC(b_+249, b_+251); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+251, b_+252); mem_wr(gb, DE, A);
  CYC(b_+252, b_+254); A = 0x01;
L_7424:
  CYC(b_+254, b_+256); E = INTERACTION_BASE + OBJ_VAR37;
  CYC(b_+256, b_+257); mem_wr(gb, DE, A);
  CYC(b_+257, b_+260);
  TAIL(interactionRunScript);
}

void s_func_742a_hook(GB *gb) {
  BASE(func_742a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+4); A = 0x02;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CYC(b_+5, b_+7); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+7, b_+8); A = mem_rd(gb, DE);
  CYC(b_+8, b_+10); alu_and(gb, 0x1f);
  CYC(b_+10, b_+13); SET_HL(SYM(table_747d));
  CYC(b_+13, b_+14); sunkenCityBullies_add_double_index(gb, b_+14);
  CYC(b_+14, b_+15); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+15, b_+16); H = mem_rd(gb, HL);
  CYC(b_+16, b_+17); L = A;
  CYC(b_+17, b_+20);
  TAIL(interactionSetScript);
}

void s_func_743e_hook(GB *gb) {
  BASE(func_743e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); alu_xor(gb, A);
  CYC(b_+1, b_+3); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+7); SET_HL(wNumBombs);
  CYC(b_+7, b_+8); A = mem_rd(gb, HL);
  CYC(b_+8, b_+9); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+9, b_+11); goto L_744e; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+15); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+15, b_+16); mem_wr(gb, DE, A);
L_744e:
  CYC(b_+16, b_+18); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+18, b_+19); A = mem_rd(gb, DE);
  CYC(b_+19, b_+20); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+20); return; }
  CYC(b_+20, b_+21);
  CYC(b_+21, b_+22); alu_xor(gb, A);
  CYC(b_+22, b_+23); mem_wr(gb, DE, A);
  CYC(b_+23, b_+24); alu_xor(gb, A);
  CYC(b_+24, b_+25); mem_wr(gb, HL, A);
  CALL_C(b_+25, s_setStatusBarNeedsRefreshBit1, SYM(setStatusBarNeedsRefreshBit1), b_+28);
  RET(b_+28); return;
}

void s_func_745b_hook(GB *gb) {
  BASE(func_745b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+3);
  CALL_C(b_+3, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+6);
  CALL_C(b_+6, s_interactionAnimateAsNpc, SYM(interactionAnimateAsNpc), b_+9);
  CYC(b_+9, b_+10); H = D;
  CYC(b_+10, b_+12); L = INTERACTION_BASE + OBJ_COLLISION_RADIUS_Y;
  CYC(b_+12, b_+14); A = 0x06;
  CYC(b_+14, b_+15); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+15, b_+17); A = 0x06;
  CYC(b_+17, b_+18); mem_wr(gb, HL, A);
  CYC(b_+18, b_+20); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(b_+20, b_+22); A = 0x32;
  CYC(b_+22, b_+23); mem_wr(gb, HL, A);
  CYC(b_+23, b_+25); A = 0x21;
  CYC(b_+25, b_+28);
  TAIL(interactionSetHighTextIndex);
}

