#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/fickleGirl.s.
// INTERAC_FICKLE_GIRL

static uint16_t fickleGirl_jump_table(GB *gb) {
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

static void fickleGirl_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode2e@func_5efa
static void fickleGirl_func_5efa(GB *gb) {
  BASE(interactionCode2e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+96, s_func_5f70_hook, SYM(func_5f70), b_+99);
  CYC(b_+99, b_+102);
  TAIL(interactionPushLinkAwayAndUpdateDrawPriority);
}

// interactionCode2e@func_5f65
static void fickleGirl_func_5f65(GB *gb) {
  BASE(interactionCode2e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+203, b_+205); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+205, b_+206); A = mem_rd(gb, DE);
  CYC(b_+206, b_+208); alu_and(gb, 0x01);
  CALL_C(b_+208, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+211);
  CALL_L(b_+211, fickleGirl_func_5efa, b_+214);
  s_func_5f70_hook(gb); return; // falls through
}

// interactionCode2e@func_5f4e
static void fickleGirl_func_5f4e(GB *gb) {
  BASE(interactionCode2e);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+180, b_+181); push_effect(gb, BC);
  CYC(b_+181, b_+183); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+183, b_+184); A = mem_rd(gb, DE);
  CYC(b_+184, b_+185); alu_rrca(gb);
  CYC(b_+185, b_+187); C = 0xf8;
  CYC(b_+187, b_+189); A = 0x1c;
  if (!(F & FC)) { CYCT(b_+189, b_+191); goto L_5f5d; }
  CYC(b_+189, b_+191);
  CYC(b_+191, b_+193); C = 0x0a;
  CYC(b_+193, b_+195); A = 0x06;
L_5f5d:
  CYC(b_+195, b_+196); mem_wr(gb, HL, A);
  CYC(b_+196, b_+198); B = 0x02;
  CALL_C(b_+198, s_objectCopyPositionWithOffset, SYM(objectCopyPositionWithOffset), b_+201);
  CYC(b_+201, b_+202); SET_BC(pop_effect(gb));
  RET(b_+202); return;
}

// getSunkenCityNPCVisibleSubId@main
static void fickleGirl_main(GB *gb) {
  BASE(getSunkenCityNPCVisibleSubId);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+14, b_+16); A = 0x28;
  CALL_C(b_+16, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+19);
  CYC(b_+19, b_+21); B = 0x04;
  if (!(F & FZ)) { CYCT(b_+21, b_+23); goto xorARet; }
  CYC(b_+21, b_+23);
  CYC(b_+23, b_+25); A = 0x1f;
  CALL_C(b_+25, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+28);
  CYC(b_+28, b_+30); B = 0x03;
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto xorARet; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+34); A = 0x40;
  CALL_C(b_+34, s_checkTreasureObtained, SYM(checkTreasureObtained), b_+37);
  CYC(b_+37, b_+39); B = 0x00;
  if (!(F & FC)) { CYCT(b_+39, b_+41); goto xorARet; }
  CYC(b_+39, b_+41);
  CYC(b_+41, b_+42); C = A;
  CALL_C(b_+42, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+45);
  if (F & FZ) { CYCT(b_+45, b_+47); goto L_5895; }
  CYC(b_+45, b_+47);
L_5895:
  CYC(b_+47, b_+48); A = C;
  CALL_C(b_+48, s_getHighestSetBit, SYM(getHighestSetBit), b_+51);
  CYC(b_+51, b_+53); B = 0x02;
  CYC(b_+53, b_+55); alu_cp(gb, 0x07);
  if (!(F & FC)) { RET_TAKEN(b_+55); return; }
  CYC(b_+55, b_+56);
  CYC(b_+56, b_+57); B = alu_dec8(gb, B);
  CYC(b_+57, b_+58); A = C;
  CYC(b_+58, b_+60); alu_and(gb, 0x08);
  if (!(F & FZ)) { CYCT(b_+60, b_+62); goto xorARet; }
  CYC(b_+60, b_+62);
  CYC(b_+62, b_+63); B = alu_dec8(gb, B);
xorARet:
  CYC(b_+63, b_+64); alu_xor(gb, A);
  RET(b_+64); return;
}

// INTERAC_FICKLE_GIRL
void s_interactionCode2e_hook(GB *gb) {
  BASE(interactionCode2e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (fickleGirl_jump_table(gb));
    if (jt_ == b_+8) goto state0;
    if (jt_ == b_+72) goto state1;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+8, b_+10); A = 0x01;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  CALL_L(b_+11, fickleGirl_main, b_+14);
  CYC(b_+14, b_+16); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+16, b_+17); A = mem_rd(gb, DE);
  CYC(b_+17, b_+18); alu_cp(gb, B);
  if (!(F & FZ)) { CYCT(b_+18, b_+21); TAIL(interactionDelete); }
  CYC(b_+18, b_+21);
  CALL_C(b_+21, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+24);
  CYC(b_+24, b_+27); A = mem_rd(gb, wActiveRoom);
  CYC(b_+27, b_+29); alu_cp(gb, 0x6d);
  if (F & FZ) { CYCT(b_+29, b_+31); goto L_5ec1; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); A = 0x01;
  CYC(b_+33, b_+35); E = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CALL_C(b_+36, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+39);
L_5ec1:
  CYC(b_+39, b_+41); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+41, b_+42); A = mem_rd(gb, DE);
  CYC(b_+42, b_+45); SET_HL(SYM(table_5f7e));
  CYC(b_+45, b_+46); fickleGirl_add_double_index(gb, b_+46);
  CYC(b_+46, b_+47); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+47, b_+48); H = mem_rd(gb, HL);
  CYC(b_+48, b_+49); L = A;
  CALL_C(b_+49, s_interactionSetScript, SYM(interactionSetScript), b_+52);
  CALL_C(b_+52, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+55);
  if (!(F & FZ)) { CYCT(b_+55, b_+57); goto L_5ee0; }
  CYC(b_+55, b_+57);
  CYC(b_+57, b_+59); mem_wr(gb, HL, 0x8e);
  CYC(b_+59, b_+60); L = alu_inc8(gb, L);
  CYC(b_+60, b_+62); mem_wr(gb, HL, 0x00);
  CYC(b_+62, b_+64); L = 0x57;
  CYC(b_+64, b_+65); mem_wr(gb, HL, D);
  CYC(b_+65, b_+67); L = INTERACTION_BASE + OBJ_ANGLE;
  CALL_L(b_+67, fickleGirl_func_5f4e, b_+70);
L_5ee0:
  CYC(b_+70, b_+72);
  goto var03_00;
state1:
  CALL_C(b_+72, s_interactionRunScript, SYM(interactionRunScript), b_+75);
  CYC(b_+75, b_+77); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+77, b_+78); A = mem_rd(gb, DE);
  CYC(b_+78, b_+79); push_effect(gb, b_+79);
  do { uint16_t jt_ = (fickleGirl_jump_table(gb));
    if (jt_ == b_+87) goto var03_00;
    if (jt_ == b_+102) goto var03_01;
    if (jt_ == b_+124) goto var03_02;
    HANDOFF(HL);
  } while (0);
var03_00:
  CALL_C(b_+87, s_interactionAnimate, SYM(interactionAnimate), b_+90);
  CYC(b_+90, b_+92); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+92, b_+93); A = mem_rd(gb, DE);
  CYC(b_+93, b_+94); A = alu_inc8(gb, A);
  if (!(F & FZ)) { CYCT(b_+94, b_+96); goto pushLinkAwayUpdateDrawPriority; }
  CYC(b_+94, b_+96);
  CALL_C(b_+96, s_func_5f70_hook, SYM(func_5f70), b_+99);
pushLinkAwayUpdateDrawPriority:
  CYC(b_+99, b_+102);
  TAIL(interactionPushLinkAwayAndUpdateDrawPriority);
var03_01:
  CALL_C(b_+102, s_interactionAnimate, SYM(interactionAnimate), b_+105);
  CYC(b_+105, b_+107); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+107, b_+108); A = mem_rd(gb, DE);
  CYC(b_+108, b_+109); alu_or(gb, A);
  if (F & FZ) { CYCT(b_+109, b_+111); goto pushLinkAwayUpdateDrawPriority; }
  CYC(b_+109, b_+111);
  CALL_L(b_+111, fickleGirl_func_5f65, b_+114);
  CALL_C(b_+114, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+117);
  CYC(b_+117, b_+119); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(b_+119, b_+121); goto func_5f3b; }
  CYC(b_+119, b_+121);
  CYC(b_+121, b_+122); A = alu_inc8(gb, A);
  CYC(b_+122, b_+124);
  goto func_5f3b;
var03_02:
  CALL_C(b_+124, s_interactionAnimate, SYM(interactionAnimate), b_+127);
  CYC(b_+127, b_+129); E = INTERACTION_BASE + OBJ_ANIM_PARAMETER;
  CYC(b_+129, b_+130); A = mem_rd(gb, DE);
  CYC(b_+130, b_+132); alu_cp(gb, 0x02);
  if (!(F & FZ)) { CYCT(b_+132, b_+134); goto pushLinkAwayUpdateDrawPriority; }
  CYC(b_+132, b_+134);
  CALL_L(b_+134, fickleGirl_func_5f65, b_+137);
  CALL_C(b_+137, s_getFreePartSlot, SYM(getFreePartSlot), b_+140);
  if (!(F & FZ)) { CYCT(b_+140, b_+142); goto L_5f32; }
  CYC(b_+140, b_+142);
  CYC(b_+142, b_+144); mem_wr(gb, HL, 0x32);
  CYC(b_+144, b_+145); L = alu_inc8(gb, L);
  CYC(b_+145, b_+147); mem_wr(gb, HL, 0x01);
  CYC(b_+147, b_+149); L = PART_BASE + OBJ_ANGLE;
  CALL_L(b_+149, fickleGirl_func_5f4e, b_+152);
L_5f32:
  CALL_C(b_+152, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+155);
  CYC(b_+155, b_+157); alu_and(gb, 0x03);
  CYC(b_+157, b_+159); alu_sub(gb, 0x02);
  if (F & FC) { RET_TAKEN(b_+159); return; }
  CYC(b_+159, b_+160);
  CYC(b_+160, b_+161); A = alu_inc8(gb, A);
func_5f3b:
  CYC(b_+161, b_+162); B = A;
L_5f3c:
  CALL_C(b_+162, s_getFreePartSlot, SYM(getFreePartSlot), b_+165);
  if (!(F & FZ)) { RET_TAKEN(b_+165); return; }
  CYC(b_+165, b_+166);
  CYC(b_+166, b_+168); mem_wr(gb, HL, 0x32);
  CYC(b_+168, b_+169); L = alu_inc8(gb, L);
  CYC(b_+169, b_+171); mem_wr(gb, HL, 0x00);
  CYC(b_+171, b_+173); L = PART_BASE + OBJ_ANGLE;
  CALL_L(b_+173, fickleGirl_func_5f4e, b_+176);
  CYC(b_+176, b_+177); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+177, b_+179); goto L_5f3c; }
  CYC(b_+177, b_+179);
  RET(b_+179); return;
}

void s_func_5f70_hook(GB *gb) {
  BASE(func_5f70);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR36;
  CYC(b_+2, b_+4); A = 0x01;
  CYC(b_+4, b_+5); mem_wr(gb, DE, A);
  CALL_C(b_+5, s_getRandomNumber_noPreserveVars, SYM(getRandomNumber_noPreserveVars), b_+8);
  CYC(b_+8, b_+10); alu_and(gb, 0x03);
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(b_+12, b_+13); mem_wr(gb, DE, A);
  RET(b_+13); return;
}

