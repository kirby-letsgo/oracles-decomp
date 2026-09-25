#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/74.s.
// INTERAC_74

static uint16_t i74_jump_table(GB *gb) {
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

static void i74_add_a_to_hl(GB *gb, uint16_t return_address) {
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

// interactionCode74@func_7283
static void i74_func_7283(GB *gb) {
  BASE(interactionCode74);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+158, b_+160); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+160, b_+161); A = mem_rd(gb, DE);
  CYC(b_+161, b_+162); alu_or(gb, A);
  RET(b_+162); return;
}

// interactionCode74@func_7268
static void i74_func_7268(GB *gb) {
  BASE(interactionCode74);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CALL_C(b_+131, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+134);
  if (!(F & FZ)) { RET_TAKEN(b_+134); return; }
  CYC(b_+134, b_+135);
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x74);
  CYC(b_+137, b_+138); L = alu_inc8(gb, L);
  CYC(b_+138, b_+139); mem_wr(gb, HL, E);
  CYC(b_+139, b_+141); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+143); alu_add(gb, B);
  CYC(b_+143, b_+144); L = E;
  CYC(b_+144, b_+145); mem_wr(gb, HL, A);
  CYC(b_+145, b_+147); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+147, b_+148); A = mem_rd(gb, DE);
  CYC(b_+148, b_+149); alu_add(gb, C);
  CYC(b_+149, b_+150); L = E;
  CYC(b_+150, b_+151); mem_wr(gb, HL, A);
  RET(b_+151); return;
}

// interactionCode74@func_725b
static void i74_func_725b(GB *gb) {
  BASE(interactionCode74);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+118, b_+121); SET_BC(SYM(func_3211));
  CYC(b_+121, b_+123); E = 0x0a;
  CALL_L(b_+123, i74_func_7268, b_+126);
  CYC(b_+126, b_+129); SET_BC(0x3018);
  CYC(b_+129, b_+131); E = 0x0b;
  CALL_C(b_+131, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+134);
  if (!(F & FZ)) { RET_TAKEN(b_+134); return; }
  CYC(b_+134, b_+135);
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x74);
  CYC(b_+137, b_+138); L = alu_inc8(gb, L);
  CYC(b_+138, b_+139); mem_wr(gb, HL, E);
  CYC(b_+139, b_+141); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+143); alu_add(gb, B);
  CYC(b_+143, b_+144); L = E;
  CYC(b_+144, b_+145); mem_wr(gb, HL, A);
  CYC(b_+145, b_+147); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+147, b_+148); A = mem_rd(gb, DE);
  CYC(b_+148, b_+149); alu_add(gb, C);
  CYC(b_+149, b_+150); L = E;
  CYC(b_+150, b_+151); mem_wr(gb, HL, A);
  RET(b_+151); return;
}

// interactionCode74@func_7208
static void i74_func_7208(GB *gb) {
  BASE(interactionCode74);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+37, b_+39); A = 0x01;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+42); A = 0x57;
  CALL_C(b_+42, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+45);
  CALL_C(b_+45, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+48);
  CYC(b_+48, b_+51);
  TAIL(objectSetVisible80);
}

// INTERAC_74
void s_interactionCode74_hook(GB *gb) {
  BASE(interactionCode74);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (i74_jump_table(gb));
    if (jt_ == b_+30) goto subid0;
    if (jt_ == b_+78) goto subid2;
    if (jt_ == b_+95) goto subid4;
    if (jt_ == b_+152) goto subid6;
    if (jt_ == b_+163) goto subidC;
    HANDOFF(HL);
  } while (0);
subid0:
  CALL_L(b_+30, i74_func_7283, b_+33);
  if (!(F & FZ)) { CYCT(b_+33, b_+35); goto func_7218; }
  CYC(b_+33, b_+35);
func_7208:
  CYC(b_+35, b_+37); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+37, b_+39); A = 0x01;
  CYC(b_+39, b_+40); mem_wr(gb, DE, A);
  CYC(b_+40, b_+42); A = 0x57;
  CALL_C(b_+42, s_loadPaletteHeader, SYM(loadPaletteHeader), b_+45);
  CALL_C(b_+45, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+48);
  CYC(b_+48, b_+51);
  TAIL(objectSetVisible80);
func_7218:
  CYC(b_+51, b_+53); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+53, b_+54); A = mem_rd(gb, DE);
  CYC(b_+54, b_+57); SET_HL(b_+68 /* @table_7229 */);
  CYC(b_+57, b_+58); i74_add_a_to_hl(gb, b_+58);
  CYC(b_+58, b_+61); A = mem_rd(gb, wTmpcbbf);
  CYC(b_+61, b_+62); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+62, b_+64); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+64, b_+65); mem_wr(gb, DE, A);
  CYC(b_+65, b_+68);
  TAIL(interactionAnimate);

subid2:
  CALL_L(b_+78, i74_func_7283, b_+81);
  if (!(F & FZ)) { RET_TAKEN(b_+81); return; }
  CYC(b_+81, b_+82);
  CYC(b_+82, b_+84); A = 0x17;
  CALL_C(b_+84, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+87);
  if (!(F & FZ)) { CYCT(b_+87, b_+90); TAIL(interactionDelete); }
  CYC(b_+87, b_+90);
  CALL_L(b_+90, i74_func_725b, b_+93);
  CYC(b_+93, b_+95);
  goto func_7208;
subid4:
  CALL_L(b_+95, i74_func_7283, b_+98);
  if (!(F & FZ)) { CYCT(b_+98, b_+101); TAIL(interactionAnimate); }
  CYC(b_+98, b_+101);
  CYC(b_+101, b_+103); A = 0x17;
  CALL_C(b_+103, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+106);
  if (F & FZ) { CYCT(b_+106, b_+109); TAIL(interactionDelete); }
  CYC(b_+106, b_+109);
  CALL_L(b_+109, i74_func_7208, b_+112);
  CYC(b_+112, b_+114); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+114, b_+115); A = mem_rd(gb, DE);
  CYC(b_+115, b_+117); alu_cp(gb, 0x04);
  if (!(F & FZ)) { RET_TAKEN(b_+117); return; }
  CYC(b_+117, b_+118);
  CYC(b_+118, b_+121); SET_BC(SYM(func_3211));
  CYC(b_+121, b_+123); E = 0x0a;
  CALL_L(b_+123, i74_func_7268, b_+126);
  CYC(b_+126, b_+129); SET_BC(0x3018);
  CYC(b_+129, b_+131); E = 0x0b;
  CALL_C(b_+131, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+134);
  if (!(F & FZ)) { RET_TAKEN(b_+134); return; }
  CYC(b_+134, b_+135);
  CYC(b_+135, b_+137); mem_wr(gb, HL, 0x74);
  CYC(b_+137, b_+138); L = alu_inc8(gb, L);
  CYC(b_+138, b_+139); mem_wr(gb, HL, E);
  CYC(b_+139, b_+141); E = INTERACTION_BASE + OBJ_YH;
  CYC(b_+141, b_+142); A = mem_rd(gb, DE);
  CYC(b_+142, b_+143); alu_add(gb, B);
  CYC(b_+143, b_+144); L = E;
  CYC(b_+144, b_+145); mem_wr(gb, HL, A);
  CYC(b_+145, b_+147); E = INTERACTION_BASE + OBJ_XH;
  CYC(b_+147, b_+148); A = mem_rd(gb, DE);
  CYC(b_+148, b_+149); alu_add(gb, C);
  CYC(b_+149, b_+150); L = E;
  CYC(b_+150, b_+151); mem_wr(gb, HL, A);
  RET(b_+151); return;
subid6:
  CALL_L(b_+152, i74_func_7283, b_+155);
  if (!(F & FZ)) { RET_TAKEN(b_+155); return; }
  CYC(b_+155, b_+156);
  CYC(b_+156, b_+158);
  goto func_7208;

subidC:
  CALL_L(b_+163, i74_func_7283, b_+166);
  if (!(F & FZ)) { CYCT(b_+166, b_+169); TAIL(interactionAnimate); }
  CYC(b_+166, b_+169);
  CYC(b_+169, b_+171); A = 0x16;
  CALL_C(b_+171, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+174);
  if (!(F & FZ)) { CYCT(b_+174, b_+177); TAIL(interactionDelete); }
  CYC(b_+174, b_+177);
  CYC(b_+177, b_+180);
  goto func_7208;
}

