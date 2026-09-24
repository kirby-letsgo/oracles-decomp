#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/zeldaVillagersRoom.s.
// INTERAC_ZELDA_VILLAGERS_ROOM

static void zeldaVillagersRoom_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// INTERAC_ZELDA_VILLAGERS_ROOM
void s_interactionCodec4_hook(GB *gb) {
  BASE(interactionCodec4);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkZeldaVillagersSeenButNoMakuSeed_hook, SYM(checkZeldaVillagersSeenButNoMakuSeed), b_+3);
  CYC(b_+3, b_+5); A = 0x00;
  if (!(F & FZ)) { CYCT(b_+5, b_+7); goto L_7a41; }
  CYC(b_+5, b_+7);
  CALL_C(b_+7, s_checkGotMakuSeedDidNotSeeZeldaKidnapped_body_hook, SYM(checkGotMakuSeedDidNotSeeZeldaKidnapped_body), b_+10);
  if (F & FZ) { CYCT(b_+10, b_+13); TAIL(interactionDelete); }
  CYC(b_+10, b_+13);
  CYC(b_+13, b_+15); A = 0x01;
L_7a41:
  CYC(b_+15, b_+18); SET_HL(SYM(zeldaVillagersRoom_interactionsTableLookup));
  CYC(b_+18, b_+19); zeldaVillagersRoom_add_double_index(gb, b_+19);
  CYC(b_+19, b_+20); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+20, b_+21); H = mem_rd(gb, HL);
  CYC(b_+21, b_+22); L = A;
  CYC(b_+22, b_+23); B = mem_rd(gb, HL);
  CYC(b_+23, b_+24); L = alu_inc8(gb, L);
  CYC(b_+24, b_+25); push_effect(gb, DE);
  CYC(b_+25, b_+26); D = H;
  CYC(b_+26, b_+27); E = L;
L_7a4d:
  CALL_C(b_+27, s_getFreeInteractionSlot, SYM(getFreeInteractionSlot), b_+30);
  if (!(F & FZ)) { CYCT(b_+30, b_+32); goto L_7a64; }
  CYC(b_+30, b_+32);
  CYC(b_+32, b_+33); A = mem_rd(gb, DE);
  CYC(b_+33, b_+34); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+34, b_+35); SET_DE(DE + 1);
  CYC(b_+35, b_+36); A = mem_rd(gb, DE);
  CYC(b_+36, b_+37); mem_wr(gb, HL, A);
  CYC(b_+37, b_+38); SET_DE(DE + 1);
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_YH;
  CYC(b_+40, b_+41); A = mem_rd(gb, DE);
  CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+42, b_+43); SET_DE(DE + 1);
  CYC(b_+43, b_+44); L = alu_inc8(gb, L);
  CYC(b_+44, b_+45); A = mem_rd(gb, DE);
  CYC(b_+45, b_+46); mem_wr(gb, HL, A);
  CYC(b_+46, b_+47); SET_DE(DE + 1);
  CYC(b_+47, b_+48); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(b_+48, b_+50); goto L_7a4d; }
  CYC(b_+48, b_+50);
L_7a64:
  CYC(b_+50, b_+51); SET_DE(pop_effect(gb));
  CYC(b_+51, b_+54);
  TAIL(interactionDelete);
}

void s_checkZeldaVillagersSeenButNoMakuSeed_hook(GB *gb) {
  BASE(checkZeldaVillagersSeenButNoMakuSeed);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x19;
  CALL_C(b_+6, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+9);
  if (F & FZ) { CYCT(b_+9, b_+12); goto checkZeldaVillagersSeen; }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  RET(b_+13); return;
checkZeldaVillagersSeen:
  CYC(b_+14, b_+16); A = 0x1e;
  CYC(b_+16, b_+19);
  TAIL(checkGlobalFlag);
}

void s_checkGotMakuSeedDidNotSeeZeldaKidnapped_body_hook(GB *gb) {
  BASE(checkGotMakuSeedDidNotSeeZeldaKidnapped_body);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, s_checkIsLinkedGame, SYM(checkIsLinkedGame), b_+3);
  if (F & FZ) { RET_TAKEN(b_+3); return; }
  CYC(b_+3, b_+4);
  CYC(b_+4, b_+6); A = 0x1f;
  CALL_C(b_+6, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+9);
  if (F & FZ) { CYCT(b_+9, b_+12); goto checkGotMakuSeed; }
  CYC(b_+9, b_+12);
  CYC(b_+12, b_+13); alu_xor(gb, A);
  RET(b_+13); return;
checkGotMakuSeed:
  CYC(b_+14, b_+16); A = 0x19;
  CYC(b_+16, b_+19);
  TAIL(checkGlobalFlag);
}

