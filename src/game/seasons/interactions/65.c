#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/65.s.
// INTERAC_D6_CRYSTAL_TRAP_ROOM

static uint16_t i65_jump_table(GB *gb) {
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

static void i65_add_a_to_hl(GB *gb, uint16_t return_address) {
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

static void i65_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// interactionCode65@func_51c0
static void i65_func_51c0(GB *gb) {
  BASE(interactionCode65);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+96, b_+98); B = 0x20;
L_51c2:
  CYC(b_+98, b_+99); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+99, b_+100); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+100, b_+102); goto L_51c2; }
  CYC(b_+100, b_+102);
  RET(b_+102); return;
}

// INTERAC_D6_CRYSTAL_TRAP_ROOM
void s_interactionCode65_hook(GB *gb) {
  BASE(interactionCode65);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_returnIfScrollMode01Unset, SYM(returnIfScrollMode01Unset), b_+3);
  CALL_C(b_+3, s_func_5258_hook, SYM(func_5258), b_+6);
  if (!(F & FZ)) { CYCT(b_+6, b_+9); TAIL(interactionDelete); }
  CYC(b_+6, b_+9);
  CYC(b_+9, b_+11); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+11, b_+12); A = mem_rd(gb, DE);
  CYC(b_+12, b_+13); push_effect(gb, b_+13);
  do { uint16_t jt_ = (i65_jump_table(gb));
    if (jt_ == b_+19) goto state0;
    if (jt_ == b_+103) goto state1;
    if (jt_ == b_+180) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+19, b_+21); A = 0x01;
  CYC(b_+21, b_+22); mem_wr(gb, DE, A);
  CALL_C(b_+22, s_objectSetReservedBit1, SYM(objectSetReservedBit1), b_+25);
  CYC(b_+25, b_+27); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+27, b_+28); A = mem_rd(gb, DE);
  CYC(b_+28, b_+29); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+29, b_+31); goto notSubId0; }
  CYC(b_+29, b_+31);
  CYC(b_+31, b_+33); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(b_+33, b_+35); A = 0x78;
  CYC(b_+35, b_+36); mem_wr(gb, DE, A);
  CYC(b_+36, b_+38); A = 0x02;
  CYC(b_+38, b_+40); mem_wr(gb, IO_SVBK, A);
  CYC(b_+40, b_+42); A = 0x80;
  CYC(b_+42, b_+45); SET_HL(w2TmpGfxBuffer);
  CALL_L(b_+45, i65_func_51c0, b_+48);
  CYC(b_+48, b_+51); SET_HL(w2TmpGfxBuffer + 0xa0);
  CALL_L(b_+51, i65_func_51c0, b_+54);
  CYC(b_+54, b_+56); A = 0x0b;
  CYC(b_+56, b_+59); SET_HL(w2TmpAttrBuffer);
  CALL_L(b_+59, i65_func_51c0, b_+62);
  CYC(b_+62, b_+65); SET_HL(w2TmpAttrBuffer + 0xa0);
  CALL_L(b_+65, i65_func_51c0, b_+68);
  CYC(b_+68, b_+69); alu_xor(gb, A);
  CYC(b_+69, b_+71); mem_wr(gb, IO_SVBK, A);
  CALL_C(b_+71, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+74);
  if (!(F & FZ)) { RET_TAKEN(b_+74); return; }
  CYC(b_+74, b_+75);
  CYC(b_+75, b_+77); mem_wr(gb, HL, 0x65);
  CYC(b_+77, b_+78); L = alu_inc8(gb, L);
  CYC(b_+78, b_+80); mem_wr(gb, HL, 0x01);
  CALL_C(b_+80, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+83);
  if (!(F & FZ)) { RET_TAKEN(b_+83); return; }
  CYC(b_+83, b_+84);
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x65);
  CYC(b_+86, b_+87); L = alu_inc8(gb, L);
  CYC(b_+87, b_+89); mem_wr(gb, HL, 0x02);
  RET(b_+89); return;
notSubId0:
  CYC(b_+90, b_+92); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+92, b_+94); A = 0x02;
  CYC(b_+94, b_+95); mem_wr(gb, DE, A);
  RET(b_+95); return;

state1:
  CYC(b_+103, b_+104); alu_xor(gb, A);
  CYC(b_+104, b_+107); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+107, b_+109); A = 0x3c;
  CYC(b_+109, b_+112); mem_wr(gb, wScreenShakeCounterX, A);
  CALL_C(b_+112, s_interactionDecCounter1, SYM(interactionDecCounter1), b_+115);
  if (!(F & FZ)) { RET_TAKEN(b_+115); return; }
  CYC(b_+115, b_+116);
  CYC(b_+116, b_+118); mem_wr(gb, HL, 0x78);
  CYC(b_+118, b_+120); A = 0x01;
  CYC(b_+120, b_+123); mem_wr(gb, wDisableScreenTransitions, A);
  CYC(b_+123, b_+126); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+126, b_+127); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CALL_C(b_+127, s_func_5261_hook, SYM(func_5261), b_+130);
  CALL_C(b_+130, s_func_545a_hook, SYM(func_545a), b_+133);
  CALL_C(b_+133, s_func_52d9_hook, SYM(func_52d9), b_+136);
  CALL_C(b_+136, s_func_537e_hook, SYM(func_537e), b_+139);
  CYC(b_+139, b_+140); alu_xor(gb, A);
  CYC(b_+140, b_+142); mem_wr(gb, IO_SVBK, A);
  CYC(b_+142, b_+144); A = mem_rd(gb, hActiveObject);
  CYC(b_+144, b_+145); D = A;
  CYC(b_+145, b_+147); A = 0x70;
  CALL_C(b_+147, s_playSound, SYM(playSound_b00), b_+150);
  CYC(b_+150, b_+152); A = 0x0f;
  CYC(b_+152, b_+155); mem_wr(gb, wScreenShakeCounterY, A);
  CYC(b_+155, b_+158); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+158, b_+160); alu_cp(gb, 0x09);
  if (F & FC) { RET_TAKEN(b_+160); return; }
  CYC(b_+160, b_+161);
  CALL_C(b_+161, s_func_5258_hook, SYM(func_5258), b_+164);
  if (!(F & FZ)) { CYCT(b_+164, b_+167); TAIL(interactionDelete); }
  CYC(b_+164, b_+167);
  CYC(b_+167, b_+169); A = 0x11;
  CYC(b_+169, b_+172); mem_wr(gb, wLinkForceState, A);
  CYC(b_+172, b_+174); A = 0x81;
  CYC(b_+174, b_+177); mem_wr(gb, wcc50, A);
  CYC(b_+177, b_+180);
  TAIL(interactionDelete);
state2:
  CALL_C(b_+180, s_func_5258_hook, SYM(func_5258), b_+183);
  if (!(F & FZ)) { CYCT(b_+183, b_+186); TAIL(interactionDelete); }
  CYC(b_+183, b_+186);
  CYC(b_+186, b_+189); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+189, b_+191); alu_cp(gb, 0x09);
  if (F & FZ) { CYCT(b_+191, b_+193); TAIL_S(func_524d); }
  CYC(b_+191, b_+193);
  CYC(b_+193, b_+196); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+196, b_+198); C = 0x08;
  CALL_C(b_+198, s_multiplyAByC, SYM(multiplyAByC), b_+201);
  CYC(b_+201, b_+202); A = L;
  CYC(b_+202, b_+204); alu_add(gb, 0x10);
  CYC(b_+204, b_+205); B = A;
  CYC(b_+205, b_+208); SET_HL(w2TmpGfxBuffer + 0x0b);
  CYC(b_+208, b_+209); A = mem_rd(gb, HL);
  CYC(b_+209, b_+210); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+210, b_+212); goto L_5235; }
  CYC(b_+210, b_+212);
  CYC(b_+212, b_+213); mem_wr(gb, HL, B);
L_5235:
  CYC(b_+213, b_+216); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+216, b_+217); B = A;
  CYC(b_+217, b_+219); A = 0x15;
  CYC(b_+219, b_+220); alu_sub(gb, B);
  CYC(b_+220, b_+222); C = 0x08;
  CALL_C(b_+222, s_multiplyAByC, SYM(multiplyAByC), b_+225);
  CYC(b_+225, b_+226); A = L;
  CYC(b_+226, b_+228); alu_sub(gb, 0x0e);
  CYC(b_+228, b_+229); B = A;
  CYC(b_+229, b_+232); SET_HL(w2TmpGfxBuffer + 0x0b);
  CYC(b_+232, b_+233); A = mem_rd(gb, HL);
  CYC(b_+233, b_+234); alu_cp(gb, B);
  if (F & FC) { RET_TAKEN(b_+234); return; }
  CYC(b_+234, b_+235);
  CYC(b_+235, b_+236); mem_wr(gb, HL, B);
  RET(b_+236); return;
}

void s_func_524d_hook(GB *gb) {
  BASE(func_524d);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x08;
  CALL_C(b_+2, s_setScreenShakeCounter, SYM(setScreenShakeCounter), b_+5);
  CYC(b_+5, b_+7); A = 0x58;
  CYC(b_+7, b_+10); mem_wr(gb, w1Link_yh, A);
  RET(b_+10); return;
}

void s_func_5258_hook(GB *gb) {
  BASE(func_5258);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wActiveRoom);
  CYC(b_+3, b_+5); alu_cp(gb, 0xc5);
  if (F & FZ) { RET_TAKEN(b_+5); return; }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); alu_cp(gb, 0xc6);
  RET(b_+8); return;
}

void s_func_5261_hook(GB *gb) {
  BASE(func_5261);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x02;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+7); A = mem_rd(gb, wScreenOffsetX);
  CYC(b_+7, b_+8); alu_cpl(gb);
  CYC(b_+8, b_+9); A = alu_inc8(gb, A);
  CYC(b_+9, b_+11); A = alu_swap(gb, A);
  CYC(b_+11, b_+12); alu_rlca(gb);
  CYC(b_+12, b_+14); mem_wr(gb, hFF8B, A);
  CYC(b_+14, b_+15); alu_xor(gb, A);
  CALL_C(b_+15, s_func_5293_hook, SYM(func_5293), b_+18);
  CYC(b_+18, b_+20); A = 0x04;
  CALL_C(b_+20, s_func_5293_hook, SYM(func_5293), b_+23);
  CYC(b_+23, b_+25); A = 0x08;
  CALL_C(b_+25, s_func_5293_hook, SYM(func_5293), b_+28);
  CYC(b_+28, b_+30); A = 0x0c;
  CALL_C(b_+30, s_func_5293_hook, SYM(func_5293), b_+33);
  CYC(b_+33, b_+35); A = 0x10;
  CALL_C(b_+35, s_func_5293_hook, SYM(func_5293), b_+38);
  CYC(b_+38, b_+40); A = 0x14;
  CALL_C(b_+40, s_func_5293_hook, SYM(func_5293), b_+43);
  CYC(b_+43, b_+45); A = 0x18;
  CALL_C(b_+45, s_func_5293_hook, SYM(func_5293), b_+48);
  CYC(b_+48, b_+50); A = 0x1c;
  s_func_5293_hook(gb); return; // falls through
}

void s_func_5293_hook(GB *gb) {
  BASE(func_5293);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); SET_HL(SYM(table_52a6));
  CYC(b_+3, b_+4); i65_add_a_to_hl(gb, b_+4);
  CYC(b_+4, b_+5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+5, b_+6); D = mem_rd(gb, HL);
  CYC(b_+6, b_+7); E = A;
  CYC(b_+7, b_+8); SET_HL(HL + 1);
  CYC(b_+8, b_+9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+9, b_+10); H = mem_rd(gb, HL);
  CYC(b_+10, b_+11); L = A;
  CYC(b_+11, b_+13); A = mem_rd(gb, hFF8B);
  CYC(b_+13, b_+14); C = A;
  CYC(b_+14, b_+16); B = 0x00;
  CYC(b_+16, b_+17); alu_add_hl(gb, BC);
  CYC(b_+17, b_+19);
  TAIL_S(func_52c6);
}

void s_func_52c6_hook(GB *gb) {
  BASE(func_52c6);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x20;
L_52c8:
  CYC(b_+2, b_+3); A = mem_rd(gb, HL);
  CYC(b_+3, b_+4); mem_wr(gb, DE, A);
  CYC(b_+4, b_+5); SET_DE(DE + 1);
  CYC(b_+5, b_+6); L = alu_inc8(gb, L);
  CYC(b_+6, b_+7); A = L;
  CYC(b_+7, b_+9); alu_and(gb, 0x1f);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto L_52d5; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+12); A = L;
  CYC(b_+12, b_+14); alu_sub(gb, 0x20);
  CYC(b_+14, b_+15); L = A;
L_52d5:
  CYC(b_+15, b_+16); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+16, b_+18); goto L_52c8; }
  CYC(b_+16, b_+18);
  RET(b_+18); return;
}

void s_func_52d9_hook(GB *gb) {
  BASE(func_52d9);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); push_effect(gb, DE);
  CYC(b_+1, b_+4); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+4, b_+5); alu_add(gb, A);
  CYC(b_+5, b_+8); SET_HL(SYM(table_5326));
  CYC(b_+8, b_+9); i65_add_double_index(gb, b_+9);
  CYC(b_+9, b_+10); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+10, b_+11); D = mem_rd(gb, HL);
  CYC(b_+11, b_+12); E = A;
  CYC(b_+12, b_+13); SET_HL(HL + 1);
  CYC(b_+13, b_+14); push_effect(gb, HL);
  CYC(b_+14, b_+17); SET_HL(w2TmpAttrBuffer);
  CYC(b_+17, b_+19); B = 0x05;
  CYC(b_+19, b_+21); C = 0x02;
  CALL_C(b_+21, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+24);
  CYC(b_+24, b_+25); SET_HL(pop_effect(gb));
  CYC(b_+25, b_+26); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+26, b_+27); D = mem_rd(gb, HL);
  CYC(b_+27, b_+28); E = A;
  CYC(b_+28, b_+31); SET_HL(w2TmpGfxBuffer);
  CYC(b_+31, b_+33); B = 0x05;
  CYC(b_+33, b_+35); C = 0x02;
  CALL_C(b_+35, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+38);
  CYC(b_+38, b_+41); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+41, b_+42); alu_add(gb, A);
  CYC(b_+42, b_+45); SET_HL(SYM(table_5352));
  CYC(b_+45, b_+46); i65_add_double_index(gb, b_+46);
  CYC(b_+46, b_+47); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+47, b_+48); D = mem_rd(gb, HL);
  CYC(b_+48, b_+49); E = A;
  CYC(b_+49, b_+50); SET_HL(HL + 1);
  CYC(b_+50, b_+51); push_effect(gb, HL);
  CYC(b_+51, b_+54); SET_HL(w2TmpAttrBuffer + 0x60);
  CYC(b_+54, b_+56); B = 0x05;
  CYC(b_+56, b_+58); C = 0x02;
  CALL_C(b_+58, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+61);
  CYC(b_+61, b_+62); SET_HL(pop_effect(gb));
  CYC(b_+62, b_+63); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+63, b_+64); D = mem_rd(gb, HL);
  CYC(b_+64, b_+65); E = A;
  CYC(b_+65, b_+68); SET_HL(w2TmpGfxBuffer + 0x60);
  CYC(b_+68, b_+70); B = 0x05;
  CYC(b_+70, b_+72); C = 0x02;
  CALL_C(b_+72, s_queueDmaTransfer, SYM(queueDmaTransfer), b_+75);
  CYC(b_+75, b_+76); SET_DE(pop_effect(gb));
  RET(b_+76); return;
}

void s_func_537e_hook(GB *gb) {
  BASE(func_537e);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 0, A);
  if (!(F & FZ)) { CYCT(b_+7, b_+9); TAIL_S(func_53a1); }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); A = alu_srl(gb, A);
  CYC(b_+11, b_+13); A = alu_swap(gb, A);
  CYC(b_+13, b_+14); L = A;
  CYC(b_+14, b_+16); A = 0x0f;
  CALL_C(b_+16, s_func_53bb_hook, SYM(func_53bb), b_+19);
  CYC(b_+19, b_+22); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+22, b_+24); A = alu_srl(gb, A);
  CYC(b_+24, b_+25); B = A;
  CYC(b_+25, b_+27); A = 0x0a;
  CYC(b_+27, b_+28); alu_sub(gb, B);
  CYC(b_+28, b_+30); A = alu_swap(gb, A);
  CYC(b_+30, b_+31); L = A;
  CYC(b_+31, b_+33); A = 0x0f;
  CYC(b_+33, b_+35);
  TAIL_S(func_53bb);
}

void s_func_53a1_hook(GB *gb) {
  BASE(func_53a1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = alu_inc8(gb, A);
  CYC(b_+1, b_+3); A = alu_srl(gb, A);
  CYC(b_+3, b_+5); A = alu_swap(gb, A);
  CYC(b_+5, b_+6); L = A;
  CYC(b_+6, b_+8); A = 0x0c;
  CALL_C(b_+8, s_func_53bb_hook, SYM(func_53bb), b_+11);
  CYC(b_+11, b_+14); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+14, b_+15); A = alu_inc8(gb, A);
  CYC(b_+15, b_+17); A = alu_srl(gb, A);
  CYC(b_+17, b_+18); B = A;
  CYC(b_+18, b_+20); A = 0x0a;
  CYC(b_+20, b_+21); alu_sub(gb, B);
  CYC(b_+21, b_+23); A = alu_swap(gb, A);
  CYC(b_+23, b_+24); L = A;
  CYC(b_+24, b_+26); A = 0x03;
  s_func_53bb_hook(gb); return; // falls through
}

void s_func_53bb_hook(GB *gb) {
  BASE(func_53bb);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = A;
  CYC(b_+1, b_+3); B = 0x10;
  CYC(b_+3, b_+5); H = 0xce;
L_53c0:
  CYC(b_+5, b_+6); A = mem_rd(gb, HL);
  CYC(b_+6, b_+7); alu_or(gb, E);
  CYC(b_+7, b_+8); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+8, b_+9); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto L_53c0; }
  CYC(b_+9, b_+11);
  RET(b_+11); return;
}

void s_func_53c7_hook(GB *gb) {
  BASE(func_53c7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+7); alu_bit(gb, 0, A);
  if (!(F & FZ)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+10); A = alu_srl(gb, A);
  CYC(b_+10, b_+12); A = alu_swap(gb, A);
  CYC(b_+12, b_+13); L = A;
  CYC(b_+13, b_+15); A = 0xb0;
  CALL_C(b_+15, s_func_53e7_hook, SYM(func_53e7), b_+18);
  CYC(b_+18, b_+21); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+21, b_+23); A = alu_srl(gb, A);
  CYC(b_+23, b_+24); B = A;
  CYC(b_+24, b_+26); A = 0x0a;
  CYC(b_+26, b_+27); alu_sub(gb, B);
  CYC(b_+27, b_+29); A = alu_swap(gb, A);
  CYC(b_+29, b_+30); L = A;
  CYC(b_+30, b_+32); A = 0xb2;
  s_func_53e7_hook(gb); return; // falls through
}

void s_func_53e7_hook(GB *gb) {
  BASE(func_53e7);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); B = 0x10;
  CYC(b_+2, b_+4); H = 0xcf;
L_53eb:
  CYC(b_+4, b_+5); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+5, b_+6); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+6, b_+8); goto L_53eb; }
  CYC(b_+6, b_+8);
  RET(b_+8); return;
}

// $02: D6 wall-closing room
void s_roomTileChangesAfterLoad02_body_hook(GB *gb) {
  BASE(roomTileChangesAfterLoad02_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_func_537e_hook, SYM(func_537e), b_+3);
  CALL_C(b_+3, s_func_53c7_hook, SYM(func_53c7), b_+6);
  CYC(b_+6, b_+9); SET_HL(w3VramTiles);
  CYC(b_+9, b_+12); SET_DE(w3TileMappingData + 0xc0);
  CALL_C(b_+12, s_func_5440_hook, SYM(func_5440), b_+15);
  CYC(b_+15, b_+18); SET_HL(w3VramTiles + 0x20);
  CYC(b_+18, b_+21); SET_DE(w3TileMappingData + 0xe0);
  CALL_C(b_+21, s_func_5440_hook, SYM(func_5440), b_+24);
  CYC(b_+24, b_+27); SET_HL(w3VramAttributes);
  CYC(b_+27, b_+30); SET_DE(w3TileMappingData + 0x4c0);
  CALL_C(b_+30, s_func_5440_hook, SYM(func_5440), b_+33);
  CYC(b_+33, b_+36); SET_HL(w3VramAttributes + 0x20);
  CYC(b_+36, b_+39); SET_DE(w3TileMappingData + 0x4e0);
  CALL_C(b_+39, s_func_5440_hook, SYM(func_5440), b_+42);
  CYC(b_+42, b_+45); SET_HL(w3VramTiles + 0x280);
  CYC(b_+45, b_+48); SET_DE(w3TileMappingData + 0x100);
  CALL_C(b_+48, s_func_5440_hook, SYM(func_5440), b_+51);
  CYC(b_+51, b_+54); SET_HL(w3VramTiles + 0x2a0);
  CYC(b_+54, b_+57); SET_DE(w3TileMappingData + 0x120);
  CALL_C(b_+57, s_func_5440_hook, SYM(func_5440), b_+60);
  CYC(b_+60, b_+63); SET_HL(w3Filler2 + 0x80);
  CYC(b_+63, b_+66); SET_DE(w3TileMappingData + 0x500);
  CALL_C(b_+66, s_func_5440_hook, SYM(func_5440), b_+69);
  CYC(b_+69, b_+72); SET_HL(w3Filler2 + 0xa0);
  CYC(b_+72, b_+75); SET_DE(w3TileMappingData + 0x520);
  CALL_C(b_+75, s_func_5440_hook, SYM(func_5440), b_+78);
  CYC(b_+78, b_+80);
  TAIL_S(func_545a);
}

void s_func_5440_hook(GB *gb) {
  BASE(func_5440);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); A = 0x03;
  CYC(b_+2, b_+4); mem_wr(gb, IO_SVBK, A);
  CYC(b_+4, b_+5); push_effect(gb, DE);
  CYC(b_+5, b_+8); SET_DE(wTmpVramBuffer);
  CYC(b_+8, b_+10); B = 0x20;
  CALL_C(b_+10, s_copyMemory, SYM(copyMemory), b_+13);
  CYC(b_+13, b_+14); SET_DE(pop_effect(gb));
  CYC(b_+14, b_+16); A = 0x02;
  CYC(b_+16, b_+18); mem_wr(gb, IO_SVBK, A);
  CYC(b_+18, b_+21); SET_HL(wTmpVramBuffer);
  CYC(b_+21, b_+23); B = 0x20;
  CYC(b_+23, b_+26);
  TAIL(copyMemory);
}

void s_func_545a_hook(GB *gb) {
  BASE(func_545a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+3); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+4); return; }
  CYC(b_+4, b_+5);
  CYC(b_+5, b_+6); push_effect(gb, DE);
  CYC(b_+6, b_+7); push_effect(gb, HL);
  CYC(b_+7, b_+10); SET_HL(w3TileMappingData + 0xc0);
  CYC(b_+10, b_+13); SET_DE(wTmpVramBuffer);
  CYC(b_+13, b_+15); B = 0x40;
  CYC(b_+15, b_+17); C = 0x02;
  CALL_C(b_+17, s_func_553a_hook, SYM(func_553a), b_+20);
  CYC(b_+20, b_+23); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+23, b_+26); SET_HL(SYM(table_5544));
  CYC(b_+26, b_+27); i65_add_double_index(gb, b_+27);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+28, b_+29); D = mem_rd(gb, HL);
  CYC(b_+29, b_+30); E = A;
  CYC(b_+30, b_+33); SET_HL(wTmpVramBuffer);
  CYC(b_+33, b_+35); B = 0x40;
  CYC(b_+35, b_+37); C = 0x03;
  CALL_C(b_+37, s_func_553a_hook, SYM(func_553a), b_+40);
  CYC(b_+40, b_+43); SET_HL(w3TileMappingData + 0x100);
  CYC(b_+43, b_+46); SET_DE(wTmpVramBuffer);
  CYC(b_+46, b_+48); B = 0x40;
  CYC(b_+48, b_+50); C = 0x02;
  CALL_C(b_+50, s_func_553a_hook, SYM(func_553a), b_+53);
  CYC(b_+53, b_+56); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+56, b_+59); SET_HL(SYM(table_5558));
  CYC(b_+59, b_+60); i65_add_double_index(gb, b_+60);
  CYC(b_+60, b_+61); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+61, b_+62); D = mem_rd(gb, HL);
  CYC(b_+62, b_+63); E = A;
  CYC(b_+63, b_+66); SET_HL(wTmpVramBuffer);
  CYC(b_+66, b_+68); B = 0x40;
  CYC(b_+68, b_+70); C = 0x03;
  CALL_C(b_+70, s_func_553a_hook, SYM(func_553a), b_+73);
  CYC(b_+73, b_+76); SET_HL(w3TileMappingData + 0x4c0);
  CYC(b_+76, b_+79); SET_DE(wTmpVramBuffer);
  CYC(b_+79, b_+81); B = 0x40;
  CYC(b_+81, b_+83); C = 0x02;
  CALL_C(b_+83, s_func_553a_hook, SYM(func_553a), b_+86);
  CYC(b_+86, b_+89); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+89, b_+92); SET_HL(SYM(table_5544));
  CYC(b_+92, b_+93); i65_add_double_index(gb, b_+93);
  CYC(b_+93, b_+94); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+94, b_+95); E = A;
  CYC(b_+95, b_+96); A = mem_rd(gb, HL);
  CYC(b_+96, b_+98); alu_add(gb, 0x04);
  CYC(b_+98, b_+99); D = A;
  CYC(b_+99, b_+102); SET_HL(wTmpVramBuffer);
  CYC(b_+102, b_+104); B = 0x40;
  CYC(b_+104, b_+106); C = 0x03;
  CALL_C(b_+106, s_func_553a_hook, SYM(func_553a), b_+109);
  CYC(b_+109, b_+112); SET_HL(w3TileMappingData + 0x500);
  CYC(b_+112, b_+115); SET_DE(wTmpVramBuffer);
  CYC(b_+115, b_+117); B = 0x40;
  CYC(b_+117, b_+119); C = 0x02;
  CALL_C(b_+119, s_func_553a_hook, SYM(func_553a), b_+122);
  CYC(b_+122, b_+125); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+125, b_+128); SET_HL(SYM(table_5558));
  CYC(b_+128, b_+129); i65_add_double_index(gb, b_+129);
  CYC(b_+129, b_+130); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+130, b_+131); E = A;
  CYC(b_+131, b_+132); A = mem_rd(gb, HL);
  CYC(b_+132, b_+134); alu_add(gb, 0x04);
  CYC(b_+134, b_+135); D = A;
  CYC(b_+135, b_+138); SET_HL(wTmpVramBuffer);
  CYC(b_+138, b_+140); B = 0x40;
  CYC(b_+140, b_+142); C = 0x03;
  CALL_C(b_+142, s_func_553a_hook, SYM(func_553a), b_+145);
  CYC(b_+145, b_+147); A = 0x03;
  CYC(b_+147, b_+149); mem_wr(gb, IO_SVBK, A);
  CYC(b_+149, b_+152); SET_HL(w3VramTiles);
  CYC(b_+152, b_+154); A = 0x80;
  CALL_C(b_+154, s_func_552a_hook, SYM(func_552a), b_+157);
  CYC(b_+157, b_+160); SET_HL(w3VramAttributes);
  CYC(b_+160, b_+162); A = 0x0b;
  CALL_C(b_+162, s_func_552a_hook, SYM(func_552a), b_+165);
  CYC(b_+165, b_+168); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+168, b_+169); C = A;
  CYC(b_+169, b_+171); B = 0x00;
  CYC(b_+171, b_+173); A = 0x16;
  CYC(b_+173, b_+174); alu_sub(gb, C);
  CYC(b_+174, b_+175); C = A;
  CYC(b_+175, b_+177); A = 0x20;
  CALL_C(b_+177, s_multiplyAByC, SYM(multiplyAByC), b_+180);
  CYC(b_+180, b_+181); C = L;
  CYC(b_+181, b_+182); B = H;
  CYC(b_+182, b_+185); SET_HL(w3VramTiles);
  CYC(b_+185, b_+186); alu_add_hl(gb, BC);
  CYC(b_+186, b_+188); A = 0x80;
  CYC(b_+188, b_+189); push_effect(gb, HL);
  CALL_C(b_+189, s_func_552a_hook, SYM(func_552a), b_+192);
  CYC(b_+192, b_+193); SET_HL(pop_effect(gb));
  CYC(b_+193, b_+196); SET_BC(0x0400);
  CYC(b_+196, b_+197); alu_add_hl(gb, BC);
  CYC(b_+197, b_+199); A = 0x0b;
  CALL_C(b_+199, s_func_552a_hook, SYM(func_552a), b_+202);
  CYC(b_+202, b_+203); alu_xor(gb, A);
  CYC(b_+203, b_+205); mem_wr(gb, IO_SVBK, A);
  CYC(b_+205, b_+206); SET_HL(pop_effect(gb));
  CYC(b_+206, b_+207); SET_DE(pop_effect(gb));
  RET(b_+207); return;
}

void s_func_552a_hook(GB *gb) {
  BASE(func_552a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); E = A;
  CYC(b_+1, b_+4); A = mem_rd(gb, wTmpcfc0 + 0x10);
  CYC(b_+4, b_+5); C = A;
  CYC(b_+5, b_+6); A = E;
L_5530:
  CYC(b_+6, b_+8); B = 0x20;
L_5532:
  CYC(b_+8, b_+9); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+9, b_+10); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+10, b_+12); goto L_5532; }
  CYC(b_+10, b_+12);
  CYC(b_+12, b_+13); C = alu_dec8(gb, C);
  if (!(F & FZ)) { CYCT(b_+13, b_+15); goto L_5530; }
  CYC(b_+13, b_+15);
  RET(b_+15); return;
}

void s_func_553a_hook(GB *gb) {
  BASE(func_553a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+1); A = C;
  CYC(b_+1, b_+3); mem_wr(gb, IO_SVBK, A);
  CALL_C(b_+3, s_copyMemory, SYM(copyMemory), b_+6);
  CYC(b_+6, b_+7); alu_xor(gb, A);
  CYC(b_+7, b_+9); mem_wr(gb, IO_SVBK, A);
  RET(b_+9); return;
}

