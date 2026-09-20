#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, SYMBANK(partCode09), (from), (to), false)
#define CYCT(from, to) burn_rom(gb, SYMBANK(partCode09), (from), (to), true)

void partCode09_hook(GB *gb);
void button_state0_hook(GB *gb);
void button_updateTileBeforeDeletion_hook(GB *gb);

void partCode09_hook(GB *gb) {
  BASE(partCode09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = 0xc4; // Part.state
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+4, b_+7); // call z
    button_state0_hook(gb);
  } else {
    CYC(b_+4, b_+7);
  }

  CYC(b_+7, b_+10); A = mem_rd(gb, wccb1); // wccb1
  CYC(b_+10, b_+11); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+11); return; } // ret nz
  CYC(b_+11, b_+12);
  CYC(b_+12, b_+15); SET_HL(w1Link); // w1Link
  CALL_C(b_+15, checkObjectsCollided_hook, SYM(checkObjectsCollided), b_+18);
  if (F & FC) { CYCT(b_+18, b_+20); goto linkTouchedButton; } // jr c
  CYC(b_+18, b_+20);
  CALL_C(b_+20, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+23);
  CYC(b_+23, b_+25); alu_sub(gb, 0x0c); // TILEINDEX_BUTTON
  CYC(b_+25, b_+27); alu_cp(gb, 0x02);
  if (!(F & FC)) { CYCT(b_+27, b_+29); goto somethingOnButton; } // jr nc
  CYC(b_+27, b_+29);
  CALL_C(b_+29, partCommon_decCounter1IfNonzero_hook, SYM(partCommon_decCounter1IfNonzero), b_+32);
  if (!(F & FZ)) { RET_TAKEN(b_+32); return; } // ret nz
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+35); L = 0xf0; // Part.var30
  CYC(b_+35, b_+37); alu_bit(gb, 0, mem_rd(gb, HL));
  if (F & FZ) { RET_TAKEN(b_+37); return; } // ret z
  CYC(b_+37, b_+38);
  CYC(b_+38, b_+40); E = 0xf0; // Part.var30
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+42); alu_or(gb, A);
  if (F & FZ) { RET_TAKEN(b_+42); return; } // ret z
  CYC(b_+42, b_+43);
  CALL_C(b_+43, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+46);
  CYC(b_+46, b_+47); C = A;
  CYC(b_+47, b_+49); A = 0x0c; // TILEINDEX_BUTTON
  CALL_C(b_+49, setTile_hook, SYM(setTile), b_+52);
  CYC(b_+52, b_+54); E = 0xc3; // Part.var03
  CYC(b_+54, b_+55); A = mem_rd(gb, DE);
  CYC(b_+55, b_+58); SET_HL(wActiveTriggers); // wActiveTriggers
  CALL_C(b_+58, unsetFlag_hook, SYM(unsetFlag), b_+61);
  CYC(b_+61, b_+63); E = 0xf0; // Part.var30
  CYC(b_+63, b_+64); alu_xor(gb, A);
  CYC(b_+64, b_+65); mem_wr(gb, DE, A);
  CYC(b_+65, b_+67); A = 0x87; // SND_SPLASH
  CYC(b_+67, b_+70); playSound_b00_hook(gb); return; // jp

somethingOnButton:
  CYC(b_+70, b_+71); H = D;
  CYC(b_+71, b_+73); L = 0xc2; // Part.subid
  CYC(b_+73, b_+75); alu_bit(gb, 7, mem_rd(gb, HL));
  if (F & FZ) { CYCT(b_+75, b_+77); goto delete; } // jr z
  CYC(b_+75, b_+77);
  CYC(b_+77, b_+79); L = 0xf0; // Part.var30
  CYC(b_+79, b_+81); alu_bit(gb, 0, mem_rd(gb, HL));
  if (!(F & FZ)) { RET_TAKEN(b_+81); return; } // ret nz
  CYC(b_+81, b_+82);
  CYC(b_+82, b_+84); L = 0xc6; // Part.counter1
  CYC(b_+84, b_+86); mem_wr(gb, HL, 0x1c);
  CALL_C(b_+86, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+89);
  CYC(b_+89, b_+90); C = A;
  CYC(b_+90, b_+92); B = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(b_+92, setTileInRoomLayoutBuffer_hook, SYM(setTileInRoomLayoutBuffer), b_+95);
  CYC(b_+95, b_+97); goto setTriggerAndPlaySound; // jr

delete:
  CYC(b_+97, b_+100); push_effect(gb, b_+100); button_updateTileBeforeDeletion_hook(gb);
  CYC(b_+100, b_+103); partDelete_hook(gb); return; // jp

linkTouchedButton:
  CYC(b_+103, b_+106); A = mem_rd(gb, w1Link_zh); // w1Link.zh
  CYC(b_+106, b_+107); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+107); return; } // ret nz
  CYC(b_+107, b_+108);

  CYC(b_+108, b_+110); E = 0xc2; // Part.subid
  CYC(b_+110, b_+111); A = mem_rd(gb, DE);
  CYC(b_+111, b_+112); alu_rlca(gb);
  if (!(F & FC)) { CYCT(b_+112, b_+114); goto delete; } // jr nc
  CYC(b_+112, b_+114);

  CYC(b_+114, b_+116); E = 0xf0; // Part.var30
  CYC(b_+116, b_+117); A = mem_rd(gb, DE);
  CYC(b_+117, b_+118); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+118); return; } // ret nz
  CYC(b_+118, b_+119);
  CALL_C(b_+119, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+122);
  CYC(b_+122, b_+123); C = A;
  CYC(b_+123, b_+125); A = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(b_+125, setTile_hook, SYM(setTile), b_+128);

setTriggerAndPlaySound:
  CYC(b_+128, b_+130); E = 0xc3; // Part.var03
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+134); SET_HL(wActiveTriggers); // wActiveTriggers
  CALL_C(b_+134, setFlag_hook, SYM(setFlag), b_+137);
  CYC(b_+137, b_+139); E = 0xf0; // Part.var30
  CYC(b_+139, b_+141); A = 0x01;
  CYC(b_+141, b_+142); mem_wr(gb, DE, A);
  CYC(b_+142, b_+144); A = 0x87; // SND_SPLASH
  CYC(b_+144, b_+147); playSound_b00_hook(gb); return; // jp
}

void button_state0_hook(GB *gb) {
  BASE(partCode09);
  CYC(b_+165, b_+166); H = D;
  CYC(b_+166, b_+167); L = E;
  CYC(b_+167, b_+168); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // [state] = 1
  CYC(b_+168, b_+170); L = 0xc2; // Part.subid
  CYC(b_+170, b_+171); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+171, b_+173); alu_and(gb, 0x07);
  CYC(b_+173, b_+174); mem_wr(gb, HL, A); SET_HL(HL - 1); // [var03]
  CYC(b_+174, SYM(partCode0b)); return; // ret
}

void button_updateTileBeforeDeletion_hook(GB *gb) {
  BASE(partCode09);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+147, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+150);
  CYC(b_+150, b_+151); C = A;
  CYC(b_+151, b_+153); B = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(b_+153, setTileInRoomLayoutBuffer_hook, SYM(setTileInRoomLayoutBuffer), b_+156);
  CALL_C(b_+156, objectGetTileAtPosition_hook, SYM(objectGetTileAtPosition), b_+159);
  CYC(b_+159, b_+161); alu_cp(gb, 0x0c); // TILEINDEX_BUTTON
  if (F & FZ) { CYCT(b_+161, b_+163); goto dupCheckButtonPushed; } // jr z
  CYC(b_+161, b_+163);
  CYC(b_+163, b_+165); goto dupSetTriggerAndPlaySound; // jr

dupCheckButtonPushed:
  CYC(b_+114, b_+116); E = 0xf0; // Part.var30
  CYC(b_+116, b_+117); A = mem_rd(gb, DE);
  CYC(b_+117, b_+118); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+118); return; } // ret nz
  CYC(b_+118, b_+119);
  CALL_C(b_+119, objectGetShortPosition_hook, SYM(objectGetShortPosition), b_+122);
  CYC(b_+122, b_+123); C = A;
  CYC(b_+123, b_+125); A = 0x0d; // TILEINDEX_PRESSED_BUTTON
  CALL_C(b_+125, setTile_hook, SYM(setTile), b_+128);

dupSetTriggerAndPlaySound:
  CYC(b_+128, b_+130); E = 0xc3; // Part.var03
  CYC(b_+130, b_+131); A = mem_rd(gb, DE);
  CYC(b_+131, b_+134); SET_HL(wActiveTriggers); // wActiveTriggers
  CALL_C(b_+134, setFlag_hook, SYM(setFlag), b_+137);
  CYC(b_+137, b_+139); E = 0xf0; // Part.var30
  CYC(b_+139, b_+141); A = 0x01;
  CYC(b_+141, b_+142); mem_wr(gb, DE, A);
  CYC(b_+142, b_+144); A = 0x87; // SND_SPLASH
  CYC(b_+144, b_+147); playSound_b00_hook(gb); return; // jp
}
