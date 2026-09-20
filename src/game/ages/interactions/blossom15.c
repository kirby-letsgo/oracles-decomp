#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void setNextChildStage_hook(GB *gb);
void setc6e2Bit_hook(GB *gb);
void checkc6e2BitSet_hook(GB *gb);
void blossom_checkHasRupees_hook(GB *gb);
void blossom_addValueToChildStatus_hook(GB *gb);
void blossom_decideInitialChildStatus_hook(GB *gb);
void blossom_openNameEntryMenu_hook(GB *gb);

void setNextChildStage_hook(GB *gb) {
  CYC(0x4fe1, 0x4fe4); SET_HL(0xc6e1);
  CYC(0x4fe4, 0x4fe5); mem_wr(gb, HL, A);
  RET(0x4fe5);
}

void setc6e2Bit_hook(GB *gb) {
  CYC(0x4fe6, 0x4fe9); SET_HL(0xc6e2);
  CYC(0x4fe9, 0x4fec); setFlag_hook(gb);
}

void checkc6e2BitSet_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x4fec, 0x4fef); SET_HL(0xc6e2);
  CALL_C(0x4fef, checkFlag_hook, 0x0205, 0x4ff2);
  CYC(0x4ff2, 0x4ff4); A = 0x01;
  if (!(F & FZ)) {
    CYCT(0x4ff4, 0x4ff6);
    goto setResult;
  }
  CYC(0x4ff4, 0x4ff6);
  CYC(0x4ff6, 0x4ff7); alu_xor(gb, A);

setResult:
  CYC(0x4ff7, 0x4ff9); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x4ff9, 0x4ffa); mem_wr(gb, DE, A);
  RET(0x4ffa);
}

void blossom_checkHasRupees_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x4ffb, cpRupeeValue_hook, 0x1765, 0x4ffe);
  CYC(0x4ffe, 0x5000); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(0x5000, 0x5001); mem_wr(gb, DE, A);
  RET(0x5001);
}

void blossom_addValueToChildStatus_hook(GB *gb) {
  CYC(0x5002, 0x5005); SET_HL(0xc60f);
  CYC(0x5005, 0x5006); alu_add(gb, mem_rd(gb, HL));
  CYC(0x5006, 0x5007); mem_wr(gb, HL, A);
  RET(0x5007);
}

void blossom_decideInitialChildStatus_hook(GB *gb) {
  CYC(0x5008, 0x500b); SET_HL(0xc609);
  CYC(0x500b, 0x500d); B = 0x00;

nextChar:
  CYC(0x500d, 0x500e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x500e, 0x500f); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x500f, 0x5011);
    goto parsedName;
  }
  CYC(0x500f, 0x5011);
  CYC(0x5011, 0x5013); alu_and(gb, 0x0f);
  CYC(0x5013, 0x5014); alu_add(gb, B);
  CYC(0x5014, 0x5015); B = A;
  CYC(0x5015, 0x5017);
  goto nextChar;

parsedName:
  CYC(0x5017, 0x5018); A = B;

reduceLoop:
  CYC(0x5018, 0x501a); alu_sub(gb, 0x03);
  if (!(F & FC)) {
    CYCT(0x501a, 0x501c);
    goto reduceLoop;
  }
  CYC(0x501a, 0x501c);
  CYC(0x501c, 0x501e); alu_add(gb, 0x04);
  CYC(0x501e, 0x5021); mem_wr(gb, 0xc60f, A);
  RET(0x5021);
}

void blossom_openNameEntryMenu_hook(GB *gb) {
  CYC(0x5022, 0x5024); A = 0x07;
  CYC(0x5024, 0x5027); openMenu_hook(gb);
}
