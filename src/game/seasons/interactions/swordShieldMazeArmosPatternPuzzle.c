#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/swordShieldMazeArmosPatternPuzzle.s.
// INTERAC_D8_ARMOS_PATTERN_PUZZLE

static uint16_t swordShieldMazeArmosPatternPuzzle_jump_table(GB *gb) {
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

static void swordShieldMazeArmosPatternPuzzle_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void swordShieldMazeArmosPatternPuzzle_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode67@func_5775
static void swordShieldMazeArmosPatternPuzzle_func_5775(GB *gb) {
  BASE(interactionCode67);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+169, b_+171); A = 0xa2;
  CALL_C(b_+171, s_setTile, SYM(setTile), b_+174);
  CYC(b_+174, b_+176); A = 0x62;
  CYC(b_+176, b_+179);
  TAIL(playSound_b00);
}

// INTERAC_D8_ARMOS_PATTERN_PUZZLE
void s_interactionCode67_hook(GB *gb) {
  BASE(interactionCode67);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (swordShieldMazeArmosPatternPuzzle_jump_table(gb));
    if (jt_ == b_+14) goto state0;
    if (jt_ == b_+33) goto state1;
    if (jt_ == b_+107) goto state2;
    if (jt_ == b_+126) goto state3;
    if (jt_ == b_+288) goto state4;
    HANDOFF(HL);
  } while (0);
state0:
  CALL_C(b_+14, s_getThisRoomFlags, SYM(getThisRoomFlags), b_+17);
  CYC(b_+17, b_+19); alu_bit(gb, 5, mem_rd(gb, HL));
  if (!(F & FZ)) { CYCT(b_+19, b_+22); TAIL(interactionDelete); }
  CYC(b_+19, b_+22);
  CYC(b_+22, b_+24); A = 0x0a;
  CALL_C(b_+24, s_objectSetCollideRadius, SYM(objectSetCollideRadius), b_+27);
  CYC(b_+27, b_+29); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+29, b_+30); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+30, b_+33);
  TAIL(interactionInitGraphics);
state1:
  CALL_C(b_+33, s_objectCheckCollidedWithLink_notDead, SYM(objectCheckCollidedWithLink_notDead), b_+36);
  if (!(F & FC)) { RET_TAKEN(b_+36); return; }
  CYC(b_+36, b_+37);
  CALL_C(b_+37, s_getRandomNumber, SYM(getRandomNumber), b_+40);
  CYC(b_+40, b_+42); alu_and(gb, 0x0f);
  CYC(b_+42, b_+45); SET_HL(b_+91 /* @table_5727 */);
  CYC(b_+45, b_+46); swordShieldMazeArmosPatternPuzzle_add_a_to_hl(gb, b_+46);
  CYC(b_+46, b_+47); A = mem_rd(gb, HL);
  CYC(b_+47, b_+49); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+49, b_+50); mem_wr(gb, DE, A);
  CYC(b_+50, b_+53); SET_HL(b_+83 /* @table_571f */);
  CYC(b_+53, b_+54); swordShieldMazeArmosPatternPuzzle_add_double_index(gb, b_+54);
  CYC(b_+54, b_+55); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+55, b_+56); H = mem_rd(gb, HL);
  CYC(b_+56, b_+57); L = A;
  CALL_C(b_+57, s_interactionSetScript, SYM(interactionSetScript), b_+60);
  CALL_C(b_+60, s_interactionIncState, SYM(interactionIncState), b_+63);
  CYC(b_+63, b_+65); A = 0x81;
  CYC(b_+65, b_+68); mem_wr(gb, wDisabledObjects, A);
  CALL_C(b_+68, s_objectSetVisible82, SYM(objectSetVisible82), b_+71);
  CALL_C(b_+71, s_setCameraFocusedObject, SYM(setCameraFocusedObject), b_+74);
  CALL_C(b_+74, s_func_57f3_hook, SYM(func_57f3), b_+77);
  CYC(b_+77, b_+79); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+79, b_+80); mem_wr(gb, DE, A);
  CYC(b_+80, b_+83);
  TAIL(objectCreatePuff);

state2:
  CYC(b_+107, b_+110); A = mem_rd(gb, wFrameCounter);
  CYC(b_+110, b_+111); alu_rrca(gb);
  if (!(F & FC)) { CYCT(b_+111, b_+113); goto L_5744; }
  CYC(b_+111, b_+113);
  CYC(b_+113, b_+115); A = 0x80;
  CYC(b_+115, b_+116); H = D;
  CYC(b_+116, b_+118); L = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+118, b_+119); alu_xor(gb, mem_rd(gb, HL));
  CYC(b_+119, b_+120); mem_wr(gb, HL, A);
L_5744:
  CALL_C(b_+120, s_interactionAnimate, SYM(interactionAnimate), b_+123);
  CYC(b_+123, b_+126);
  TAIL(interactionRunScript);
state3:
  CYC(b_+126, b_+128); E = INTERACTION_BASE + OBJ_VISIBLE;
  CYC(b_+128, b_+129); alu_xor(gb, A);
  CYC(b_+129, b_+130); mem_wr(gb, DE, A);
  CALL_C(b_+130, s_func_57f3_hook, SYM(func_57f3), b_+133);
  CYC(b_+133, b_+134); B = A;
  CYC(b_+134, b_+136); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+136, b_+137); A = mem_rd(gb, DE);
  CYC(b_+137, b_+138); alu_cp(gb, B);
  if (F & FZ) { RET_TAKEN(b_+138); return; }
  CYC(b_+138, b_+139);
  CYC(b_+139, b_+141); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+145); SET_HL(b_+206 /* @table_579a */);
  CYC(b_+145, b_+146); swordShieldMazeArmosPatternPuzzle_add_double_index(gb, b_+146);
  CYC(b_+146, b_+147); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+147, b_+148); H = mem_rd(gb, HL);
  CYC(b_+148, b_+149); L = A;
  CYC(b_+149, b_+151); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(b_+151, b_+152); A = mem_rd(gb, DE);
  CYC(b_+152, b_+153); swordShieldMazeArmosPatternPuzzle_add_a_to_hl(gb, b_+153);
  CYC(b_+153, b_+154); A = mem_rd(gb, HL);
  CYC(b_+154, b_+155); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+155, b_+157); goto func_5792; }
  CYC(b_+155, b_+157);
  CYC(b_+157, b_+159); alu_cp(gb, 0x1c);
  if (F & FZ) { CYCT(b_+159, b_+161); goto func_577f; }
  CYC(b_+159, b_+161);
  CYC(b_+161, b_+162); C = A;
  CYC(b_+162, b_+163); A = mem_rd(gb, DE);
  CYC(b_+163, b_+164); A = alu_inc8(gb, A);
  CYC(b_+164, b_+165); mem_wr(gb, DE, A);
  CYC(b_+165, b_+166); A = B;
  CYC(b_+166, b_+168); E = INTERACTION_BASE + OBJ_VAR39;
  CYC(b_+168, b_+169); mem_wr(gb, DE, A);
  CYC(b_+169, b_+171); A = 0xa2;
  CALL_C(b_+171, s_setTile, SYM(setTile), b_+174);
  CYC(b_+174, b_+176); A = 0x62;
  CYC(b_+176, b_+179);
  TAIL(playSound_b00);
func_577f:
  CYC(b_+179, b_+181); C = 0x1c;
  CALL_L(b_+181, swordShieldMazeArmosPatternPuzzle_func_5775, b_+184);
  CALL_C(b_+184, s_interactionIncState, SYM(interactionIncState), b_+187);
  CYC(b_+187, b_+189); A = 0x4d;
  CALL_C(b_+189, s_playSound, SYM(playSound_b00), b_+192);
  CYC(b_+192, b_+195); SET_HL(SYM(d8ArmosScript_giveKey));
  CYC(b_+195, b_+198);
  TAIL(interactionSetScript);
func_5792:
  CYC(b_+198, b_+200); A = 0x5a;
  CALL_C(b_+200, s_playSound, SYM(playSound_b00), b_+203);
  CYC(b_+203, b_+206);
  TAIL(interactionDelete);

state4:
  CALL_C(b_+288, s_interactionRunScript, SYM(interactionRunScript), b_+291);
  if (F & FC) { CYCT(b_+291, b_+294); TAIL(interactionDelete); }
  CYC(b_+291, b_+294);
  RET(b_+294); return;
}

void s_func_57f3_hook(GB *gb) {
  BASE(func_57f3);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(w1Link_yh);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+4, b_+6); alu_add(gb, 0x04);
  CYC(b_+6, b_+8); alu_and(gb, 0xf0);
  CYC(b_+8, b_+9); B = A;
  CYC(b_+9, b_+10); L = alu_inc8(gb, L);
  CYC(b_+10, b_+11); A = mem_rd(gb, HL);
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+15); alu_and(gb, 0x0f);
  CYC(b_+15, b_+16); alu_or(gb, B);
  RET(b_+16); return;
}

