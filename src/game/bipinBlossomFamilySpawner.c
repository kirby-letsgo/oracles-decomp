#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

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
  CYC(0x4178, 0x417b); A = mem_rd(gb, wChildStatus);
  for (;;) {
    CYC(0x417b, 0x417c); alu_cp(gb, mem_rd(gb, HL));
    if (!(F & FC)) {
      CYCT(0x417c, 0x417e);
      break;
    }
    CYC(0x417c, 0x417e);
    CYC(0x417e, 0x417f); SET_HL(HL + 1);
    CYC(0x417f, 0x4180); SET_HL(HL + 1);
    CYC(0x4180, 0x4182);
  }
  CYC(0x4182, 0x4183); SET_HL(HL + 1);
  CYC(0x4183, 0x4184); A = mem_rd(gb, HL);
  CYC(0x4184, 0x4187); mem_wr(gb, wChildPersonality, A);
  CYC(0x4187, 0x4188); ret_effect(gb);
}

void label_0b_006_hook(GB *gb) {
  bipin_label_006(gb);
}

void decideInitialChildPersonality_hook(GB *gb) {
  CYC(0x4168, 0x416b); SET_HL(0x4188);
  CYC(0x416b, 0x416d);
  bipin_label_006(gb);
}

void decideFinalChildPersonality_hook(GB *gb) {
  CYC(0x416d, 0x4170); A = mem_rd(gb, wChildPersonality);
  CYC(0x4170, 0x4171); alu_add(gb, A);
  CYC(0x4171, 0x4172); B = A;
  CYC(0x4172, 0x4173); alu_add(gb, A);
  CYC(0x4173, 0x4174); alu_add(gb, B);
  CYC(0x4174, 0x4177); SET_HL(0x418e);
  CYC(0x4177, 0x4178); bipin_add_a_to_hl(gb, 0x4178);
  bipin_label_006(gb);
}

void initializeChildOnGameStart_hook(GB *gb) {
  CYC(0x415c, 0x415f); SET_HL(0xc60f);
  CYC(0x415f, 0x4160); A = mem_rd(gb, HL);
  CYC(0x4160, 0x4161); alu_or(gb, A);
  if (F & FZ) {
    CYCT(0x4161, 0x4162); ret_effect(gb);
    return;
  }
  CYC(0x4161, 0x4162);
  CYC(0x4162, 0x4164); A = 5;
  CYC(0x4164, 0x4166); L = 0xe0;
  CYC(0x4166, 0x4167); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x4167, 0x4168); mem_wr(gb, HL, A); SET_HL(HL + 1);
  decideInitialChildPersonality_hook(gb);
}

void childSetVar38ToNumEssencesObtained_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41a0, 0x41a2); A = 0x40;
  CALL_C(0x41a2, checkTreasureObtained_hook, 0x1748, 0x41a5);
  if (F & FC) {
    CYCT(0x41a5, 0x41a7);
  } else {
    CYC(0x41a5, 0x41a7);
    CYC(0x41a7, 0x41a8); alu_xor(gb, A);
  }
  CYC(0x41a8, 0x41a9); H = D;
  CYC(0x41a9, 0x41ab); L = 0x78;
  CYC(0x41ab, 0x41ad); mem_wr(gb, HL, 0);
  for (;;) {
    CYC(0x41ad, 0x41ae); alu_add(gb, A);
    if (!(F & FC)) {
      CYCT(0x41ae, 0x41b0);
    } else {
      CYC(0x41ae, 0x41b0);
      CYC(0x41b0, 0x41b1); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
    }
    CYC(0x41b1, 0x41b2); alu_or(gb, A);
    if (!(F & FZ)) {
      CYCT(0x41b2, 0x41b4);
      continue;
    }
    CYC(0x41b2, 0x41b4); ret_effect(gb);
    return;
  }
}

static void bipin_goto_next_state(GB *gb) {
  CYC(0x4131, 0x4134); A = mem_rd(gb, wNextChildStage);
  CYC(0x4134, 0x4137); mem_wr(gb, wChildStage, A);
  CYC(0x4137, 0x4139); alu_cp(gb, 4);
  if (F & FZ) {
    CYCT(0x4139, 0x413c); decideInitialChildPersonality_hook(gb);
    return;
  }
  CYC(0x4139, 0x413c);
  CYC(0x413c, 0x413e); alu_cp(gb, 7);
  if (F & FZ) {
    CYCT(0x413e, 0x4141); decideFinalChildPersonality_hook(gb);
    return;
  }
  CYC(0x413e, 0x4141);
  CYC(0x4141, 0x4142); ret_effect(gb);
}

void interactionCodeac__gotoNextState_hook(GB *gb) {
  bipin_goto_next_state(gb);
}

static void bipin_need_essences(GB *gb, uint16_t from, uint8_t amount) {
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

void interactionCodeac__need2Essences_hook(GB *gb) { bipin_need_essences(gb, 0x4142, 2); }
void interactionCodeac__gotoNextState_2_hook(GB *gb) {
  CYC(0x414a, 0x414d);
  bipin_goto_next_state(gb);
}
void interactionCodeac__need4Essences_hook(GB *gb) { bipin_need_essences(gb, 0x414c, 4); }
void interactionCodeac__need6Essences_hook(GB *gb) { bipin_need_essences(gb, 0x4154, 6); }

static void bipin_check_update_state(GB *gb) {
  CYC(0x4111, 0x4114); A = mem_rd(gb, wSeedTreeRefilledBitset);
  CYC(0x4114, 0x4116); alu_bit(gb, 1, A);
  if (F & FZ) {
    CYCT(0x4116, 0x4117); ret_effect(gb);
    return;
  }
  CYC(0x4116, 0x4117);
  CYC(0x4117, 0x411a); SET_HL(wNextChildStage);
  CYC(0x411a, 0x411b); A = mem_rd(gb, HL);
  CYC(0x411b, 0x411c); A = mem_rd(gb, HL);
  CYC(0x411c, 0x411d); push_effect(gb, 0x411d);
  switch (bipin_jump_table(gb)) {
    case 0x4131: bipin_goto_next_state(gb); return;
    case 0x4142: interactionCodeac__need2Essences_hook(gb); return;
    case 0x414a: interactionCodeac__gotoNextState_2_hook(gb); return;
    case 0x414c: interactionCodeac__need4Essences_hook(gb); return;
    case 0x4154: interactionCodeac__need6Essences_hook(gb); return;
    default: return;
  }
}

void interactionCodeac__checkUpdateState_hook(GB *gb) {
  bipin_check_update_state(gb);
}

void spawnBipinBlossomFamilyObjects_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x41b5, 0x41b7); E = 0x42;
  CYC(0x41b7, 0x41b8); A = mem_rd(gb, DE);
  CYC(0x41b8, 0x41b9); alu_or(gb, A);
  CYC(0x41b9, 0x41bc); SET_HL(0x41ef);
  if (F & FZ) {
    CYCT(0x41bc, 0x41be);
  } else {
    CYC(0x41bc, 0x41be);
    CYC(0x41be, 0x41c1); SET_HL(0x422d);
  }
  CYC(0x41c1, 0x41c4); A = mem_rd(gb, wChildStage);
  CYC(0x41c4, 0x41c6); alu_cp(gb, 4);
  if (F & FC) {
    CYCT(0x41c6, 0x41c8);
  } else {
    CYC(0x41c6, 0x41c8);
    CYC(0x41c8, 0x41c9); bipin_add_double_index(gb, 0x41c9);
    CYC(0x41c9, 0x41ca); A = mem_rd(gb, HL); SET_HL(HL + 1);
    CYC(0x41ca, 0x41cb); H = mem_rd(gb, HL);
    CYC(0x41cb, 0x41cc); L = A;
    CYC(0x41cc, 0x41cf); A = mem_rd(gb, wChildPersonality);
  }
  CYC(0x41cf, 0x41d0); bipin_add_double_index(gb, 0x41d0);
  CYC(0x41d0, 0x41d1); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x41d1, 0x41d2); B = mem_rd(gb, HL);
  CYC(0x41d2, 0x41d3); C = A;
  for (;;) {
    CYC(0x41d3, 0x41d4); A = mem_rd(gb, BC);
    CYC(0x41d4, 0x41d5); alu_or(gb, A);
    if (F & FZ) {
      CYCT(0x41d5, 0x41d6); ret_effect(gb);
      return;
    }
    CYC(0x41d5, 0x41d6);
    CALL_C(0x41d6, getFreeInteractionSlot_hook, 0x3aef, 0x41d9);
    if (!(F & FZ)) {
      CYCT(0x41d9, 0x41da); ret_effect(gb);
      return;
    }
    CYC(0x41d9, 0x41da);
    CYC(0x41da, 0x41db); A = mem_rd(gb, BC);
    CYC(0x41db, 0x41dc); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x41dc, 0x41dd); SET_BC(BC + 1);
    CYC(0x41dd, 0x41de); A = mem_rd(gb, BC);
    CYC(0x41de, 0x41df); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x41df, 0x41e0); SET_BC(BC + 1);
    CYC(0x41e0, 0x41e1); A = mem_rd(gb, BC);
    CYC(0x41e1, 0x41e2); mem_wr(gb, HL, A); SET_HL(HL + 1);
    CYC(0x41e2, 0x41e3); SET_BC(BC + 1);
    CYC(0x41e3, 0x41e5); L = 0x4b;
    CYC(0x41e5, 0x41e6); A = mem_rd(gb, BC);
    CYC(0x41e6, 0x41e7); mem_wr(gb, HL, A);
    CYC(0x41e7, 0x41e8); SET_BC(BC + 1);
    CYC(0x41e8, 0x41ea); L = 0x4d;
    CYC(0x41ea, 0x41eb); A = mem_rd(gb, BC);
    CYC(0x41eb, 0x41ec); mem_wr(gb, HL, A);
    CYC(0x41ec, 0x41ed); SET_BC(BC + 1);
    CYC(0x41ed, 0x41f0);
  }
}

void interactionCodeac_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x40f8, 0x40fa); A = 0x14;
  CALL_C(0x40fa, checkGlobalFlag_hook, 0x31f3, 0x40fd);
  if (!(F & FZ)) {
    CYCT(0x40fd, 0x4100); interactionDelete_hook(gb);
    return;
  }
  CYC(0x40fd, 0x4100);
  CALL_C(0x4100, childSetVar38ToNumEssencesObtained_hook, 0x41a0, 0x4103);
  CYC(0x4103, 0x4106); push_effect(gb, 0x4106);
  bipin_check_update_state(gb);
  CALL_C(0x4106, spawnBipinBlossomFamilyObjects_hook, 0x41b5, 0x4109);
  CYC(0x4109, 0x410c); SET_HL(0xcc4d);
  CYC(0x410c, 0x410e); mem_wr(gb, HL, mem_rd(gb, HL) & ~0x02);
  CYC(0x410e, 0x4111); interactionDelete_hook(gb);
}
