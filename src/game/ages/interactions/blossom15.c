#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(setNextChildStage), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(setNextChildStage), (from), (to), true)

void setNextChildStage_hook(GB *gb);
void setc6e2Bit_hook(GB *gb);
void checkc6e2BitSet_hook(GB *gb);
void blossom_checkHasRupees_hook(GB *gb);
void blossom_addValueToChildStatus_hook(GB *gb);
void blossom_decideInitialChildStatus_hook(GB *gb);
void blossom_openNameEntryMenu_hook(GB *gb);

void setNextChildStage_hook(GB *gb) {
  BASE(setNextChildStage);
  CYC(b_+0, b_+3); SET_HL(wNextChildStage);
  CYC(b_+3, b_+4); mem_wr(gb, HL, A);
  RET(b_+4);
}

void setc6e2Bit_hook(GB *gb) {
  BASE(setc6e2Bit);
  CYC(b_+0, b_+3); SET_HL(wc6e2);
  CYC(b_+3, SYM(checkc6e2BitSet)); setFlag_hook(gb);
}

void checkc6e2BitSet_hook(GB *gb) {
  BASE(checkc6e2BitSet);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+3); SET_HL(wc6e2);
  CALL_C(b_+3, checkFlag_hook, SYM(checkFlag), b_+6);
  CYC(b_+6, b_+8); A = 0x01;
  if (!(F & FZ)) {
    CYCT(b_+8, b_+10);
    goto setResult;
  }
  CYC(b_+8, b_+10);
  CYC(b_+10, b_+11); alu_xor(gb, A);

setResult:
  CYC(b_+11, b_+13); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  RET(b_+14);
}

void blossom_checkHasRupees_hook(GB *gb) {
  BASE(blossom_checkHasRupees);
  uint16_t sp0_ = gb->sp;
  CALL_C(b_+0, cpRupeeValue_hook, SYM(cpRupeeValue), b_+3);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_VAR3C;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  RET(b_+6);
}

void blossom_addValueToChildStatus_hook(GB *gb) {
  BASE(blossom_addValueToChildStatus);
  CYC(b_+0, b_+3); SET_HL(wChildStatus);
  CYC(b_+3, b_+4); alu_add(gb, mem_rd(gb, HL));
  CYC(b_+4, b_+5); mem_wr(gb, HL, A);
  RET(b_+5);
}

void blossom_decideInitialChildStatus_hook(GB *gb) {
  BASE(blossom_decideInitialChildStatus);
  CYC(b_+0, b_+3); SET_HL(wKidName);
  CYC(b_+3, b_+5); B = 0x00;

nextChar:
  CYC(b_+5, b_+6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+6, b_+7); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+7, b_+9);
    goto parsedName;
  }
  CYC(b_+7, b_+9);
  CYC(b_+9, b_+11); alu_and(gb, 0x0f);
  CYC(b_+11, b_+12); alu_add(gb, B);
  CYC(b_+12, b_+13); B = A;
  CYC(b_+13, b_+15);
  goto nextChar;

parsedName:
  CYC(b_+15, b_+16); A = B;

reduceLoop:
  CYC(b_+16, b_+18); alu_sub(gb, 0x03);
  if (!(F & FC)) {
    CYCT(b_+18, b_+20);
    goto reduceLoop;
  }
  CYC(b_+18, b_+20);
  CYC(b_+20, b_+22); alu_add(gb, 0x04);
  CYC(b_+22, b_+25); mem_wr(gb, wChildStatus, A);
  RET(b_+25);
}

void blossom_openNameEntryMenu_hook(GB *gb) {
  BASE(blossom_openNameEntryMenu);
  CYC(b_+0, b_+2); A = 0x07;
  CYC(b_+2, SYM(veranFaceCutsceneScript_b15)); openMenu_hook(gb);
}
