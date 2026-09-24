#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/common/parts/itemDrop.s.
// PART_ITEM_DROP
// Variables:
// relatedObj1: ?
// var30: ID of relatedObj1
// var31/var32: Position to move towards?
// var33: Counter until collisions are enabled (fairy only)
// var34: Set to $01 when it hits water in a sidescrolling area

static void itemDrop_add_a_to_hl(GB *gb, uint16_t return_address) {
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

void s_itemDrop_pullOreChunksWithMagnetGloves_hook(GB *gb) {
  BASE(itemDrop_pullOreChunksWithMagnetGloves);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = PART_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); alu_sub(gb, 0x0c);
  CYC(b_+5, b_+7); alu_cp(gb, 0x03);
  if (!(F & FC)) { RET_TAKEN(b_+7); return; }
  CYC(b_+7, b_+8);
  CYC(b_+8, b_+11); A = mem_rd(gb, wMagnetGloveState);
  CYC(b_+11, b_+12); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+12); return; }
  CYC(b_+12, b_+13);
  CALL_C(b_+13, s_objectGetAngleTowardLink, SYM(objectGetAngleTowardLink), b_+16);
  CYC(b_+16, b_+17); C = A;
  CYC(b_+17, b_+18); H = D;
  CYC(b_+18, b_+20); L = PART_BASE + OBJ_YH;
  CYC(b_+20, b_+23); A = mem_rd(gb, w1Link_yh);
  CYC(b_+23, b_+24); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+24, b_+26); goto L_4450; }
  CYC(b_+24, b_+26);
  CYC(b_+26, b_+27); alu_cpl(gb);
  CYC(b_+27, b_+28); A = alu_inc8(gb, A);
L_4450:
  CYC(b_+28, b_+29); B = A;
  CYC(b_+29, b_+31); L = PART_BASE + OBJ_XH;
  CYC(b_+31, b_+34); A = mem_rd(gb, w1Link_xh);
  CYC(b_+34, b_+35); alu_sub(gb, mem_rd(gb, HL));
  if (!(F & FC)) { CYCT(b_+35, b_+37); goto L_445b; }
  CYC(b_+35, b_+37);
  CYC(b_+37, b_+38); alu_cpl(gb);
  CYC(b_+38, b_+39); A = alu_inc8(gb, A);
L_445b:
  CYC(b_+39, b_+40); alu_cp(gb, B);
  if (!(F & FC)) { CYCT(b_+40, b_+42); goto L_445f; }
  CYC(b_+40, b_+42);
  CYC(b_+42, b_+43); A = B;
L_445f:
  CYC(b_+43, b_+45); alu_and(gb, 0xf0);
  CYC(b_+45, b_+47); A = alu_swap(gb, A);
  CYC(b_+47, b_+49); alu_bit(gb, 3, A);
  if (F & FZ) { CYCT(b_+49, b_+51); goto L_4469; }
  CYC(b_+49, b_+51);
  CYC(b_+51, b_+53); A = 0x07;
L_4469:
  CYC(b_+53, b_+56); SET_HL(SYM(itemDrop_magnetGlovePullSpeed));
  CYC(b_+56, b_+57); itemDrop_add_a_to_hl(gb, b_+57);
  CYC(b_+57, b_+58); B = mem_rd(gb, HL);
  s_itemDrop_applySpeed(gb); return; // falls through
}

