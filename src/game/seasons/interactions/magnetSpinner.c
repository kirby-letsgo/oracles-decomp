#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/magnetSpinner.s.
// INTERAC_MAGNET_SPINNER

static uint16_t magnetSpinner_jump_table(GB *gb) {
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

static void magnetSpinner_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// interactionCode7b@func_758d
static void magnetSpinner_func_758d(GB *gb) {
  BASE(interactionCode7b);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+155, b_+157); A = alu_swap(gb, A);
  CYC(b_+157, b_+158); alu_rrca(gb);
  CYC(b_+158, b_+161); SET_HL(b_+207 /* @table_75c1 */);
  CYC(b_+161, b_+162); magnetSpinner_add_a_to_hl(gb, b_+162);
  CYC(b_+162, b_+164); B = 0x00;
  CYC(b_+164, b_+165); alu_add_hl(gb, BC);
  CYC(b_+165, b_+167); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+167, b_+168); A = mem_rd(gb, DE);
  CYC(b_+168, b_+169); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+169, b_+172); mem_wr(gb, w1Link_yh, A);
  CYC(b_+172, b_+173); SET_HL(HL + 1);
  CYC(b_+173, b_+175); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+175, b_+176); A = mem_rd(gb, DE);
  CYC(b_+176, b_+177); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+177, b_+180); mem_wr(gb, w1Link_xh, A);
  RET(b_+180); return;
}

// INTERAC_MAGNET_SPINNER
void s_interactionCode7b_hook(GB *gb) {
  BASE(interactionCode7b);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); H = D;
  CYC(b_+1, b_+3); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+5, b_+7); goto L_7500; }
  CYC(b_+5, b_+7);
  CYC(b_+7, b_+8); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (F & FZ) { CYCT(b_+8, b_+10); goto L_7500; }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); A = D;
  CYC(b_+11, b_+14); mem_wr(gb, wLinkRidingObject, A);
L_7500:
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); push_effect(gb, b_+18);
  do { uint16_t jt_ = (magnetSpinner_jump_table(gb));
    if (jt_ == b_+24) goto state0;
    if (jt_ == b_+38) goto state1;
    if (jt_ == b_+127) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+24, b_+26); A = 0x01;
  CYC(b_+26, b_+27); mem_wr(gb, DE, A);
  CALL_C(b_+27, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+30);
  CYC(b_+30, b_+32); A = 0x07;
  CALL_C(b_+32, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+35);
  CYC(b_+35, b_+38);
  TAIL(objectSetVisible82);
state1:
  CALL_C(b_+38, s_objectGetTileAtPosition, SYM(objectGetTileAtPosition), b_+41);
  CYC(b_+41, b_+43); mem_wr(gb, HL, 0x3f);
  CALL_C(b_+43, s_func_75e7_hook, SYM(func_75e7), b_+46);
  if (!(F & FC)) CALL_C_CC(b_+46, s_interactionAnimate, SYM(interactionAnimate), b_+49);
  else CYC(b_+46, b_+49);
  CALL_C(b_+49, s_objectPreventLinkFromPassing, SYM(objectPreventLinkFromPassing), b_+52);
  if (!(F & FC)) { RET_TAKEN(b_+52); return; }
  CYC(b_+52, b_+53);
  CYC(b_+53, b_+56); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+56, b_+57); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+57, b_+59); goto func_754e; }
  CYC(b_+57, b_+59);
  CYC(b_+59, b_+61); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+61, b_+62); A = mem_rd(gb, DE);
  CYC(b_+62, b_+63); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+63); return; }
  CYC(b_+63, b_+64);
  CYC(b_+64, b_+66); C = 0x18;
  CALL_C(b_+66, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+69);
  CYC(b_+69, b_+71); A = alu_srl(gb, A);
  CYC(b_+71, b_+73); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+73, b_+74); mem_wr(gb, DE, A);
  CYC(b_+74, b_+75); B = A;
  CYC(b_+75, b_+78); A = mem_rd(gb, w1Link_direction);
  CYC(b_+78, b_+80); alu_xor(gb, 0x02);
  CYC(b_+80, b_+81); alu_cp(gb, B);
  if (!(F & FZ)) { RET_TAKEN(b_+81); return; }
  CYC(b_+81, b_+82);
  CALL_C(b_+82, s_func_75e7_hook, SYM(func_75e7), b_+85);
  if (F & FC) { RET_TAKEN(b_+85); return; }
  CYC(b_+85, b_+86);
  CALL_C(b_+86, s_interactionIncState, SYM(interactionIncState), b_+89);
  CYC(b_+89, b_+92);
  TAIL_S(func_75e1);
func_754e:
  CYC(b_+92, b_+95); A = mem_rd(gb, wLinkRidingObject);
  CYC(b_+95, b_+96); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+96); return; }
  CYC(b_+96, b_+97);
  CYC(b_+97, b_+99); C = 0x18;
  CALL_C(b_+99, s_objectCheckLinkWithinDistance, SYM(objectCheckLinkWithinDistance), b_+102);
  CYC(b_+102, b_+104); A = alu_srl(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+104); return; }
  CYC(b_+104, b_+105);
  CYC(b_+105, b_+108); A = mem_rd(gb, wActiveTileIndex);
  CYC(b_+108, b_+110); alu_cp(gb, 0x3f);
  if (!(F & FZ)) { RET_TAKEN(b_+110); return; }
  CYC(b_+110, b_+111);
  CYC(b_+111, b_+114); A = mem_rd(gb, w1Link_state);
  CYC(b_+114, b_+116); alu_cp(gb, 0x01);
  if (!(F & FZ)) { RET_TAKEN(b_+116); return; }
  CYC(b_+116, b_+117);
  CYC(b_+117, b_+119); A = 0x02;
  CYC(b_+119, b_+122); mem_wr(gb, wLinkForceState, A);
  CYC(b_+122, b_+123); alu_xor(gb, A);
  CYC(b_+123, b_+126); mem_wr(gb, wLinkStateParameter, A);
  RET(b_+126); return;
state2:
  CALL_C(b_+127, s_func_75e1_hook, SYM(func_75e1), b_+130);
  CALL_C(b_+130, s_interactionAnimate, SYM(interactionAnimate), b_+133);
  CYC(b_+133, b_+136); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+136, b_+137); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+137, b_+139); goto func_75bb; }
  CYC(b_+137, b_+139);
  CYC(b_+139, b_+141); alu_bit(gb, 1, A);
  if (F & FZ) { CYCT(b_+141, b_+143); goto func_75bb; }
  CYC(b_+141, b_+143);
  CYC(b_+143, b_+145); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+145, b_+146); A = mem_rd(gb, DE);
  CYC(b_+146, b_+148); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(b_+148, b_+150); goto func_75a7; }
  CYC(b_+148, b_+150);
  CYC(b_+150, b_+151); alu_add(gb, A);
  CYC(b_+151, b_+152); C = A;
  CYC(b_+152, b_+154); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+154, b_+155); A = mem_rd(gb, DE);
  CYC(b_+155, b_+157); A = alu_swap(gb, A);
  CYC(b_+157, b_+158); alu_rrca(gb);
  CYC(b_+158, b_+161); SET_HL(b_+207 /* @table_75c1 */);
  CYC(b_+161, b_+162); magnetSpinner_add_a_to_hl(gb, b_+162);
  CYC(b_+162, b_+164); B = 0x00;
  CYC(b_+164, b_+165); alu_add_hl(gb, BC);
  CYC(b_+165, b_+167); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+167, b_+168); A = mem_rd(gb, DE);
  CYC(b_+168, b_+169); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+169, b_+172); mem_wr(gb, w1Link_yh, A);
  CYC(b_+172, b_+173); SET_HL(HL + 1);
  CYC(b_+173, b_+175); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+175, b_+176); A = mem_rd(gb, DE);
  CYC(b_+176, b_+177); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+177, b_+180); mem_wr(gb, w1Link_xh, A);
  RET(b_+180); return;
func_75a7:
  CYC(b_+181, b_+183); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+183, b_+184); A = mem_rd(gb, DE);
  CYC(b_+184, b_+185); A = alu_inc8(gb, A);
  CYC(b_+185, b_+187); alu_and(gb, 0x03);
  CYC(b_+187, b_+189); mem_wr(gb, hFF8B, A);
  CYC(b_+189, b_+191); C = 0x00;
  CALL_L(b_+191, magnetSpinner_func_758d, b_+194);
  CYC(b_+194, b_+196); A = mem_rd(gb, hFF8B);
  CYC(b_+196, b_+198); alu_xor(gb, 0x02);
  CYC(b_+198, b_+201); mem_wr(gb, w1Link_direction, A);
func_75bb:
  CYC(b_+201, b_+203); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+203, b_+205); A = 0x01;
  CYC(b_+205, b_+206); mem_wr(gb, DE, A);
  RET(b_+206); return;
}

void s_func_75e1_hook(GB *gb) {
  BASE(func_75e1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+2, b_+4); A = 0x14;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  RET(b_+5); return;
}

void s_func_75e7_hook(GB *gb) {
  BASE(func_75e7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); B = A;
  CYC(b_+4, b_+6); alu_and(gb, 0x80);
  if (F & FZ) { RET_TAKEN(b_+6); return; }
  CYC(b_+6, b_+7);
  CYC(b_+7, b_+8); A = B;
  CYC(b_+8, b_+10); alu_and(gb, 0x07);
  CYC(b_+10, b_+13); SET_HL(wToggleBlocksState);
  CALL_C(b_+13, s_checkFlag, SYM(checkFlag), b_+16);
  if (!(F & FZ)) { RET_TAKEN(b_+16); return; }
  CYC(b_+16, b_+17);
  CYC(b_+17, b_+18); alu_scf(gb);
  RET(b_+18); return;
}

