#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t book_of_seals_jump_table(GB *gb) {
  burn_rom(gb, 0, 0, 1, false); alu_add(gb, A);
  burn_rom(gb, 0, 1, 2, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 2, 3, false); alu_add(gb, L);
  burn_rom(gb, 0, 3, 4, false); L = A;
  if (F & FC) { burn_rom(gb, 0, 4, 6, false); burn_rom(gb, 0, 6, 7, false); H = alu_inc8(gb, H); }
  else burn_rom(gb, 0, 4, 6, true);
  burn_rom(gb, 0, 7, 8, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 8, 9, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 9, 10, false); L = A;
  burn_rom(gb, 0, 10, 11, false);
  return HL;
}

static void book_add_a_to_hl(GB *gb, uint16_t ra) {
  push_effect(gb, ra); burn_rom(gb,0,0x10,0x11,false); alu_add(gb,L); burn_rom(gb,0,0x11,0x12,false); L=A;
  if (F&FC) { burn_rom(gb,0,0x12,0x13,false); burn_rom(gb,0,0x13,0x14,false); H=alu_inc8(gb,H); burn_rom(gb,0,0x14,0x15,false); }
  else burn_rom(gb,0,0x12,0x13,true); ret_effect(gb);
}

void interactionCodeb4__func_69ce_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x69ce,0x69d0); E=0x42; CYC(0x69d0,0x69d1); A=mem_rd(gb,DE); CYC(0x69d1,0x69d2); C=A;
  CYC(0x69d2,0x69d5); SET_HL(0x6a05); CYC(0x69d5,0x69d6); book_add_a_to_hl(gb,0x69d6);
  CYC(0x69d6,0x69d8); A=mem_rd(gb,HL); CYC(0x69d7,0x69d8); book_add_a_to_hl(gb,0x69d8);
  CYC(0x69d8,0x69da); E=0x58; CYC(0x69da,0x69db); A=L; CYC(0x69db,0x69dd); mem_wr(gb,DE,A);
  CYC(0x69dc,0x69dd); E=alu_inc8(gb,E); CYC(0x69dd,0x69de); A=H; CYC(0x69de,0x69e0); mem_wr(gb,DE,A); CYC(0x69df,0x69e0); ret_effect(gb);
}

void interactionCodeb4__state0_hook(GB *gb) {
  uint16_t sp0_=gb->sp; CYC(0x68c5,0x68c7); A=1; CYC(0x68c7,0x68c8); mem_wr(gb,DE,A); CYC(0x68c8,0x68ca); A=6;
  CALL_C(0x68ca,objectSetCollideRadius_hook,0x24a1,0x68cd); CALL_C(0x68cd,interactionInitGraphics_hook,0x15fb,0x68d0);
  CALL_C(0x68d0,interactionSetAlwaysUpdateBit_hook,0x2701,0x68d3); CYC(0x68d3,0x68d5); A=0x12; CALL_C(0x68d5,interactionSetHighTextIndex_hook,0x253b,0x68d8);
  CYC(0x68d8,0x68da); E=0x71; CALL_C(0x68da,objectAddToAButtonSensitiveObjectList_hook,0x1b2c,0x68dd); CYC(0x68dd,0x68df); E=0x42; CYC(0x68df,0x68e0); A=mem_rd(gb,DE); CYC(0x68e0,0x68e1); alu_or(gb,A);
  if (!(F&FZ)) { CYCT(0x68e1,0x68e2); ret_effect(gb); return; } CYC(0x68e1,0x68e2); CYC(0x68e2,0x68e5); SET_HL(0xcfd0); CYC(0x68e5,0x68e7); B=0x10;
  if (F&FZ) CALL_C_CC(0x68e7,clearMemory_hook,0x046f,0x68ea); else CYC(0x68e7,0x68ea); CALL_C(0x68ea,getThisRoomFlags_hook,0x197d,0x68ed); CYC(0x68ed,0x68ef); alu_bit(gb,6,A);
  if (!(F&FZ)) { CYCT(0x68ef,0x68f0); ret_effect(gb); return; } CYC(0x68ef,0x68f0); CALL_C(0x68f0,interactionIncState_hook,0x23e0,0x68f3); CYC(0x68f3,0x68f6); SET_HL(0x7d38); CYC(0x68f6,0x68f9); interactionSetScript_hook(gb); return;
}

void interactionCodeb4__activatedBook_hook(GB *gb) {
  uint16_t sp0_=gb->sp; CYC(0x6907,0x6909); A=1; CALL_C(0x6909,interactionSetAnimation_hook,0x262e,0x690c); CALL_C(0x690c,interactionCodeb4__func_69ce_hook,0x69ce,0x690f);
  CYC(0x690f,0x6910); A=C; CYC(0x6910,0x6913); mem_wr(gb,0xcfd0,A); CYC(0x6913,0x6916); SET_HL(0x6932); CYC(0x6916,0x6917); book_add_a_to_hl(gb,0x6917); CYC(0x6917,0x6919); C=mem_rd(gb,HL); CYC(0x6918,0x691a); B=0x12; CALL_C(0x691a,showText_hook,0x1872,0x691d);
  CYC(0x691d,0x6921); A=0x81; CYC(0x691f,0x6923); mem_wr(gb,0xcc02,A); CYC(0x6922,0x6926); mem_wr(gb,0xcc8a,A); CYC(0x6925,0x6927); H=D; CYC(0x6926,0x6928); L=0x43; CYC(0x6928,0x692a); A=0xd3; CYC(0x692a,0x692c); mem_wr(gb,HL,A); CYC(0x692b,0x692d); A=3; CYC(0x692d,0x692f); mem_wr(gb,HL,A); CYC(0x692e,0x692f); L=alu_inc8(gb,L); CYC(0x692f,0x6932); mem_wr(gb,HL,2); CYC(0x6931,0x6932); ret_effect(gb);
}

void interactionCodeb4__state1_hook(GB *gb) {
  uint16_t sp0_=gb->sp; CYC(0x68f9,0x68fa); E=alu_inc8(gb,E); CYC(0x68fa,0x68fb); A=mem_rd(gb,DE); CYC(0x68fb,0x68fc); alu_or(gb,A); if (F&FZ) CALL_ROM_CC(0x68fc,0x69e0); else CYC(0x68fc,0x68ff); CALL_C(0x68ff,objectSetPriorityRelativeToLink_withTerrainEffects_hook,0x22e0,0x6902); CYC(0x6902,0x6904); E=0x71; CYC(0x6904,0x6905); A=mem_rd(gb,DE); CYC(0x6905,0x6906); alu_or(gb,A); if (F&FZ) { CYCT(0x6906,0x6907); ret_effect(gb); return; } CYC(0x6906,0x6907); interactionCodeb4__activatedBook_hook(gb); return;
}

void interactionCodeb4__state2_hook(GB *gb) {
  uint16_t sp0_=gb->sp; CYC(0x6938,0x6939); E=alu_inc8(gb,E); CYC(0x6939,0x693a); A=mem_rd(gb,DE); CYC(0x693a,0x693b); alu_or(gb,A); if (F&FZ) CALL_ROM_CC(0x693b,0x69e0); else CYC(0x693b,0x693e); CALL_C(0x693e,interactionRunScript_hook,0x2552,0x6941); if (!(F&FC)) { CYCT(0x6941,0x6942); ret_effect(gb); return; } CYC(0x6941,0x6942); CALL_C(0x6942,objectSetPriorityRelativeToLink_withTerrainEffects_hook,0x22e0,0x6945); CYC(0x6945,0x6947); A=0x4d; CALL_C(0x6947,playSound_b00_hook,0x0c98,0x694a); CYC(0x694a,0x694c); A=0x55; CALL_C(0x694c,loseTreasure_hook,0x1733,0x694f); CYC(0x694f,0x6952); interactionCodeb4__activatedBook_hook(gb); return;
}

void interactionCodeb4__state3_hook(GB *gb) {
  uint16_t sp0_=gb->sp; CALL_C(0x6951,retIfTextIsActive_hook,0x1859,0x6954); CALL_ROM(0x6954,0x6964); CYC(0x6957,0x695a); A=mem_rd(gb,0xcc8a); CYC(0x695a,0x695b); alu_or(gb,A); if (!(F&FZ)) { CYCT(0x695b,0x695c); ret_effect(gb); return; } CYC(0x695b,0x695c); CYC(0x695c,0x695e); E=0x43; CYC(0x695e,0x6960); A=0xf4; CYC(0x6960,0x6961); mem_wr(gb,DE,A); CYC(0x6961,0x6965); interactionCodeb4__func_69ce_hook(gb); return;
}

void interactionCodeb4__state4_hook(GB *gb) {
  uint16_t sp0_=gb->sp; CALL_C(0x6996,objectSetPriorityRelativeToLink_withTerrainEffects_hook,0x22e0,0x6999); CYC(0x6999,0x699b); E=0x71; CYC(0x699b,0x699c); A=mem_rd(gb,DE); CYC(0x699c,0x699d); alu_or(gb,A); if (F&FZ) goto after_prompt; CYC(0x699d,0x699f); alu_xor(gb,A); CYC(0x69a0,0x69a2); mem_wr(gb,DE,A); CYC(0x69a1,0x69a3); E=0x42; CYC(0x69a3,0x69a4); A=mem_rd(gb,DE); CYC(0x69a4,0x69a7); SET_HL(0x6932); CYC(0x69a7,0x69a8); book_add_a_to_hl(gb,0x69a8); CYC(0x69a8,0x69aa); C=mem_rd(gb,HL); CYC(0x69a9,0x69ab); B=0x12; CYC(0x69ab,0x69af); showText_hook(gb); return;
after_prompt:
  CYC(0x69ae,0x69b1); SET_HL(0xcfd0); CYC(0x69b1,0x69b3); E=0x42; CYC(0x69b3,0x69b4); A=mem_rd(gb,DE); CYC(0x69b4,0x69b5); alu_cp(gb,mem_rd(gb,HL)); if (F&FZ) { CYCT(0x69b5,0x69b6); ret_effect(gb); return; } CYC(0x69b5,0x69b6); CALL_C(0x69b6,retIfTextIsActive_hook,0x1859,0x69b9); CALL_ROM(0x69b9,0x6964); CYC(0x69bc,0x69c0); A=mem_rd(gb,0xcc8a); CYC(0x69bf,0x69c0); alu_or(gb,A); if (!(F&FZ)) { CYCT(0x69c0,0x69c1); ret_effect(gb); return; } CYC(0x69c0,0x69c1); CALL_C(0x69c1,interactionSetAnimation_hook,0x262e,0x69c4); CYC(0x69c4,0x69c6); E=0x44; CYC(0x69c6,0x69c8); A=1; CYC(0x69c8,0x69ca); mem_wr(gb,DE,A); CYC(0x69c9,0x69cb); E=0x71; CYC(0x69cb,0x69cc); alu_xor(gb,A); CYC(0x69cc,0x69ce); mem_wr(gb,DE,A); CYC(0x69cd,0x69ce); ret_effect(gb);
}

void interactionCodeb4_hook(GB *gb) {
  uint16_t sp0_=gb->sp; CYC(0x68b7,0x68b9); E=0x44; CYC(0x68b9,0x68bb); A=mem_rd(gb,DE); CYC(0x68ba,0x68bb); push_effect(gb,0x68bb); switch(book_of_seals_jump_table(gb)) { case 0x68c5: interactionCodeb4__state0_hook(gb); return; case 0x68f9: interactionCodeb4__state1_hook(gb); return; case 0x6938: interactionCodeb4__state2_hook(gb); return; case 0x6951: interactionCodeb4__state3_hook(gb); return; case 0x6996: interactionCodeb4__state4_hook(gb); return; default: hook_continue(gb,HL,sp0_); return; }
}
