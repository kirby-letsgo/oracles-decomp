#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

static uint16_t bipin_jump_table(GB *gb) {
  burn_rom(gb, 0, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0, 0x0004, 0x0006, false);
    burn_rom(gb, 0, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else {
    burn_rom(gb, 0, 0x0004, 0x0006, true);
  }
  burn_rom(gb, 0, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0, 0x000a, 0x000b, false);
  return HL;
}

static void bipin_add_a_to_hl(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0, 0x10, 0x11, false); alu_add(gb, L);
  burn_rom(gb, 0, 0x11, 0x12, false); L = A;
  if (!(F & FC)) {
    burn_rom(gb, 0, 0x12, 0x13, true); ret_effect(gb);
    return;
  }
  burn_rom(gb, 0, 0x12, 0x13, false);
  burn_rom(gb, 0, 0x13, 0x14, false); H = alu_inc8(gb, H);
  burn_rom(gb, 0, 0x14, 0x15, false); ret_effect(gb);
}

static void bipin_add_double_index(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0, 0x18, 0x19, false); push_effect(gb, BC);
  burn_rom(gb, 0, 0x19, 0x1a, false); C = A;
  burn_rom(gb, 0, 0x1a, 0x1c, false); B = 0;
  burn_rom(gb, 0, 0x1c, 0x1d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x1d, 0x1e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0, 0x1e, 0x1f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0, 0x1f, 0x20, false); ret_effect(gb);
}

static void bipin_label_006(GB *gb) {
  BASE(label_0b_006);
  CYC(b_+0, b_+3); A = mem_rd(gb, wChildStatus);
  for (;;) {
    CYC(b_+3, b_+4); alu_cp(gb, mem_rd(gb, HL));
    if (!(F & FC)) {
      CYCT(b_+4, b_+6);
      break;
    }
    CYC(b_+4, b_+6);
    CYC(b_+6, b_+7); SET_HL(HL + 1);
    CYC(b_+7, b_+8); SET_HL(HL + 1);
    CYC(b_+8, b_+10);
  }
  CYC(b_+10, b_+11); SET_HL(HL + 1);
  CYC(b_+11, b_+12); A = mem_rd(gb, HL);
  CYC(b_+12, b_+15); mem_wr(gb, wChildPersonality, A);
  CYC(b_+15, b_+16); ret_effect(gb);
}

void label_0b_006_hook(GB *gb) {
  bipin_label_006(gb);
}

void decideInitialChildPersonality_hook(GB *gb) {
  BASE(decideInitialChildPersonality);
  CYC(b_+0, b_+3); SET_HL(SYM(initialChildPersonalityTable));
  CYC(b_+3, b_+5);
  bipin_label_006(gb);
}

void decideFinalChildPersonality_hook(GB *gb) {
  BASE(decideFinalChildPersonality);
  CYC(b_+0, b_+3); A = mem_rd(gb, wChildPersonality);
  CYC(b_+3, b_+4); alu_add(gb, A);
  CYC(b_+4, b_+5); B = A;
  CYC(b_+5, b_+6); alu_add(gb, A);
  CYC(b_+6, b_+7); alu_add(gb, B);
  CYC(b_+7, b_+10); SET_HL(SYM(finalChildPersonalityTable));
  CYC(b_+10, b_+11); bipin_add_a_to_hl(gb, SYM(label_0b_006));
  bipin_label_006(gb);
}

void initializeChildOnGameStart_hook(GB *gb) {
  BASE(initializeChildOnGameStart);
  CYC(b_+0, b_+3); SET_HL(wChildStatus);
  CYC(b_+3, b_+4); A = mem_rd(gb, HL);
  CYC(b_+4, b_+5); alu_or(gb, A);
  if (F & FZ) {
    CYCT(b_+5, b_+6); ret_effect(gb);
    return;
  }
  CYC(b_+5, b_+6);
  CYC(b_+6, b_+8); A = 5;
  CYC(b_+8, b_+10); L = GV(0xe0, 0xda);
  CYC(b_+10, b_+11); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+11, b_+12); mem_wr(gb, HL, A); SET_HL(HL + 1);
  TAIL(decideInitialChildPersonality);
}

void childSetVar38ToNumEssencesObtained_hook(GB *gb) {
  BASE(childSetVar38ToNumEssencesObtained);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x40;
  CALL_C(b_+2, checkTreasureObtained_hook, SYM(checkTreasureObtained), b_+5);
  if (F & FC) {
    CYCT(b_+5, b_+7);
  } else {
    CYC(b_+5, b_+7);
    CYC(b_+7, b_+8); alu_xor(gb, A);
  }
  CYC(b_+8, b_+9); H = D;
  CYC(b_+9, b_+11); L = 0x78;
  CYC(b_+11, b_+13); mem_wr(gb, HL, 0);
  for (;;) {
    CYC(b_+13, b_+14); alu_add(gb, A);
    if (!(F & FC)) {
      CYCT(b_+14, b_+16);
    } else {
      CYC(b_+14, b_+16);
      CYC(b_+16, b_+17); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    }
    CYC(b_+17, b_+18); alu_or(gb, A);
    if (!(F & FZ)) {
      CYCT(b_+18, b_+20);
      continue;
    }
    CYC(b_+18, b_+20); RET(b_+20);
    return;
  }
}

static void bipin_goto_next_state(GB *gb) {
  BASE(interactionCodeac);
  CYC(b_+57, b_+60); A = mem_rd(gb, wNextChildStage);
  CYC(b_+60, b_+63); mem_wr(gb, wChildStage, A);
  CYC(b_+63, b_+65); alu_cp(gb, 4);
  if (F & FZ) {
    CYCT(b_+65, b_+68); decideInitialChildPersonality_hook(gb);
    return;
  }
  CYC(b_+65, b_+68);
  CYC(b_+68, b_+70); alu_cp(gb, 7);
  if (F & FZ) {
    CYCT(b_+70, b_+73); decideFinalChildPersonality_hook(gb);
    return;
  }
  CYC(b_+70, b_+73);
  CYC(b_+73, b_+74); ret_effect(gb);
}

void interactionCodeac__gotoNextState_hook(GB *gb) {
  bipin_goto_next_state(gb);
}

static void bipin_need_essences(GB *gb, uint16_t from, uint8_t amount) {
  BANKOF(decideInitialChildPersonality);
  CYC(from, from + 2); E = 0x78;
  CYC(from + 2, from + 4); A = mem_rd(gb, DE);
  CYC(from + 4, from + 6); alu_cp(gb, amount);
  if (F & FC) {
    CYCT(from + 6, from + 7); ret_effect(gb);
    return;
  }
  CYC(from + 6, from + 7);
  CYC(from + 7, from + 9);
  bipin_goto_next_state(gb);
}

void interactionCodeac__need2Essences_hook(GB *gb) { bipin_need_essences(gb, SYM(interactionCodeac__need2Essences), 2); }
void interactionCodeac__gotoNextState_2_hook(GB *gb) {
  BASE(interactionCodeac);
  CYC(b_+82, b_+85);
  bipin_goto_next_state(gb);
}
void interactionCodeac__need4Essences_hook(GB *gb) { bipin_need_essences(gb, SYM(interactionCodeac__need4Essences), 4); }
void interactionCodeac__need6Essences_hook(GB *gb) { bipin_need_essences(gb, SYM(interactionCodeac__need6Essences), 6); }

static void bipin_check_update_state(GB *gb) {
  BASE(interactionCodeac);
  CYC(b_+25, b_+28); A = mem_rd(gb, wSeedTreeRefilledBitset);
  CYC(b_+28, b_+30); alu_bit(gb, 1, A);
  if (F & FZ) {
    CYCT(b_+30, b_+31); ret_effect(gb);
    return;
  }
  CYC(b_+30, b_+31);
  CYC(b_+31, b_+34); SET_HL(wNextChildStage);
  CYC(b_+34, b_+35); A = mem_rd(gb, HL);
  CYC(b_+35, b_+36); A = mem_rd(gb, HL);
  CYC(b_+36, b_+37); push_effect(gb, b_+37);
  do { uint16_t jt_ = (bipin_jump_table(gb));
    if (jt_ == b_+57) { bipin_goto_next_state(gb); return; }
    else if (jt_ == b_+74) { interactionCodeac__need2Essences_hook(gb); return; }
    else if (jt_ == b_+82) { interactionCodeac__gotoNextState_2_hook(gb); return; }
    else if (jt_ == b_+84) { interactionCodeac__need4Essences_hook(gb); return; }
    else if (jt_ == b_+92) { interactionCodeac__need6Essences_hook(gb); return; }
    else { return; }
  } while (0);
}

void interactionCodeac__checkUpdateState_hook(GB *gb) {
  bipin_check_update_state(gb);
}

void spawnBipinBlossomFamilyObjects_hook(GB *gb) {
  BASE(spawnBipinBlossomFamilyObjects);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); E = 0x42;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  CYC(b_+4, b_+7); SET_HL(b_+58);
  if (F & FZ) {
    CYCT(b_+7, b_+9);
  } else {
    CYC(b_+7, b_+9);
    CYC(b_+9, b_+12); SET_HL(b_+120);
  }
  CYC(b_+12, b_+15); A = mem_rd(gb, wChildStage);
  CYC(b_+15, b_+17); alu_cp(gb, 4);
  if (F & FC) {
    CYCT(b_+17, b_+19);
  } else {
    CYC(b_+17, b_+19);
    CYC(b_+19, b_+20); bipin_add_double_index(gb, b_+20);
    CYC(b_+20, b_+21); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(b_+21, b_+22); H = mem_rd(gb, HL);
    CYC(b_+22, b_+23); L = A;
    CYC(b_+23, b_+26); A = mem_rd(gb, wChildPersonality);
  }
  CYC(b_+26, b_+27); bipin_add_double_index(gb, b_+27);
  CYC(b_+27, b_+28); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+28, b_+29); B = mem_rd(gb, HL);
  CYC(b_+29, b_+30); C = A;
  for (;;) {
    CYC(b_+30, b_+31); A = mem_rd(gb, BC);
    CYC(b_+31, b_+32); alu_or(gb, A);
    if (F & FZ) {
      CYCT(b_+32, b_+33); ret_effect(gb);
      return;
    }
    CYC(b_+32, b_+33);
    CALL_C(b_+33, getFreeInteractionSlot_hook, SYM(getFreeInteractionSlot), b_+36);
    if (!(F & FZ)) {
      CYCT(b_+36, b_+37); ret_effect(gb);
      return;
    }
    CYC(b_+36, b_+37);
    CYC(b_+37, b_+38); A = mem_rd(gb, BC);
    CYC(b_+38, b_+39); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+39, b_+40); SET_BC(BC + 1);
    CYC(b_+40, b_+41); A = mem_rd(gb, BC);
    CYC(b_+41, b_+42); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+42, b_+43); SET_BC(BC + 1);
    CYC(b_+43, b_+44); A = mem_rd(gb, BC);
    CYC(b_+44, b_+45); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(b_+45, b_+46); SET_BC(BC + 1);
    CYC(b_+46, b_+48); L = 0x4b;
    CYC(b_+48, b_+49); A = mem_rd(gb, BC);
    CYC(b_+49, b_+50); mem_wr(gb, HL, A);
    CYC(b_+50, b_+51); SET_BC(BC + 1);
    CYC(b_+51, b_+53); L = 0x4d;
    CYC(b_+53, b_+54); A = mem_rd(gb, BC);
    CYC(b_+54, b_+55); mem_wr(gb, HL, A);
    CYC(b_+55, b_+56); SET_BC(BC + 1);
    CYC(b_+56, b_+59);
  }
}

void interactionCodeac_hook(GB *gb) {
  BASE(interactionCodeac);
  uint16_t sp0_ = gb->sp;
  CYC(b_+0, b_+2); A = 0x14;
  CALL_C(b_+2, checkGlobalFlag_hook, SYM(checkGlobalFlag), b_+5);
  if (!(F & FZ)) {
    CYCT(b_+5, b_+8); interactionDelete_hook(gb);
    return;
  }
  CYC(b_+5, b_+8);
  CALL_C(b_+8, childSetVar38ToNumEssencesObtained_hook, SYM(childSetVar38ToNumEssencesObtained), b_+11);
  CYC(b_+11, b_+14); push_effect(gb, b_+14);
  bipin_check_update_state(gb);
  CALL_C(b_+14, spawnBipinBlossomFamilyObjects_hook, SYM(spawnBipinBlossomFamilyObjects), b_+17);
  CYC(b_+17, b_+20); SET_HL(wSeedTreeRefilledBitset);
  CYC(b_+20, b_+22); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x02);
  CYC(b_+22, b_+25); interactionDelete_hook(gb);
}
