#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

#define zoraAnimationTable 0x615e
#define zoraScriptTable 0x61e0
#define zoraTextTable 0x6255
#define zoraTextIndices 0x6281

void zora_commonInit__afterCall60d7_hook(GB *gb);
void zora_subid0B__afterCall611f_hook(GB *gb);
void zora_subid12__afterCall6193_hook(GB *gb);
void zora_subid0E__afterCall6206_hook(GB *gb);
void zora_subid1B__afterCall6236_hook(GB *gb);
void zora_commonInit_hook(GB *gb);
void zora_commonInitWithScript_hook(GB *gb);
void zora_getWorldState_hook(GB *gb);

static uint16_t zora_jump_table(GB *gb) {
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

static void zora_add_a_to_hl(GB *gb) {
  burn_rom(gb, 0x00, 0x0010, 0x0011, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0011, 0x0012, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0012, 0x0013, false);
    burn_rom(gb, 0x00, 0x0013, 0x0014, false); H = alu_inc8(gb, H);
    burn_rom(gb, 0x00, 0x0014, 0x0015, false); ret_effect(gb);
  } else {
    burn_rom(gb, 0x00, 0x0012, 0x0013, true); ret_effect(gb);
  }
}

static void zora_add_double_index(GB *gb, uint16_t return_address) {
  CYC(0x61a2, 0x61a3); push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

void zora_commonInit__afterCall60d7_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x60d7, interactionSetAlwaysUpdateBit_hook, 0x2701, 0x60da);
  CALL_C(0x60da, interactionIncState_hook, 0x23e0, 0x60dd);
  CYC(0x60dd, 0x60df); L = INTERACTION_BASE + OBJ_TEXT_ID + 1;
  CYC(0x60df, 0x60e1); mem_wr(gb, HL, 0x34);
  CYC(0x60e1, 0x60e4); objectSetVisiblec2_hook(gb);
}

void zora_commonInit_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x60d4, interactionInitGraphics_hook, 0x15fb, 0x60d7);
  zora_commonInit__afterCall60d7_hook(gb);
}

void zora_commonInitWithScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x60d1, interactionSetScript_hook, 0x2544, 0x60d4);
  zora_commonInit_hook(gb);
}

void zora_getWorldState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6267, 0x6269); A = 0x27;
  CALL_C(0x6269, checkGlobalFlag_hook, 0x31f3, 0x626c);
  CYC(0x626c, 0x626e); B = 0;
  if (F & FZ) {
    CYCT(0x626e, 0x626f); ret_effect(gb); return;
  }
  CYC(0x626e, 0x626f);
  CYC(0x626f, 0x6271); A = 0x30;
  CALL_C(0x6271, checkGlobalFlag_hook, 0x31f3, 0x6274);
  CYC(0x6274, 0x6276); B = 1;
  if (F & FZ) {
    CYCT(0x6276, 0x6277); ret_effect(gb); return;
  }
  CYC(0x6276, 0x6277);
  CYC(0x6277, 0x627a); A = mem_rd(gb, wEssencesObtained);
  CYC(0x627a, 0x627c); alu_bit(gb, 6, A);
  CYC(0x627c, 0x627e); B = 2;
  if (!(F & FC)) {
    CYCT(0x627e, 0x627f); ret_effect(gb); return;
  }
  CYC(0x627e, 0x627f);
  CYC(0x627f, 0x6280); B = alu_inc8(gb, B);
  CYC(0x6280, 0x6281); ret_effect(gb);
}

void zora_subid0B__afterCall611f_hook(GB *gb) {
  CYC(0x611f, 0x6121); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6121, 0x6123); mem_wr(gb, HL, 0x1e);
  CYC(0x6123, 0x6125); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6125, 0x6126); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x6126, 0x6128); alu_sub(gb, 0x0a);
  CYC(0x6128, 0x612a); A = alu_swap(gb, A);
  CYC(0x612a, 0x612b); alu_rrca(gb);
  CYC(0x612b, 0x612c); mem_wr(gb, HL, A);
  CYC(0x612c, 0x612d); ret_effect(gb);
}

void zora_subid0B_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6110, 0x6112); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6112, 0x6113); A = mem_rd(gb, DE);
  CYC(0x6113, 0x6114); push_effect(gb, 0x6114);
  switch (zora_jump_table(gb)) {
    case 0x611c: goto state0;
    case 0x612d: goto state1;
    case 0x6138: goto state2;
    case 0x616e: goto state3;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x611c, zora_commonInit_hook, 0x60d4, 0x611f);
  zora_subid0B__afterCall611f_hook(gb);
  return;

state1:
  CALL_C(0x612d, interactionDecCounter1_hook, 0x23cc, 0x6130);
  if (!(F & FZ)) {
    CYCT(0x6130, 0x6131); ret_effect(gb); return;
  }
  CYC(0x6130, 0x6131);
  CYC(0x6131, 0x6133); mem_wr(gb, HL, 0x78);
  CYC(0x6133, 0x6134); L = alu_inc8(gb, L);
  CYC(0x6134, 0x6135); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(0x6135, 0x6138); interactionIncState_hook(gb);
  return;

state2:
  CALL_C(0x6138, interactionDecCounter1_hook, 0x23cc, 0x613b);
  if (!(F & FZ)) {
    CYCT(0x613b, 0x613d); goto decrement_counter2;
  }
  CYC(0x613b, 0x613d);
  CYC(0x613d, 0x613f); mem_wr(gb, HL, 0x5a);
  CYC(0x613f, 0x6141); A = 2;
  CALL_C(0x6141, interactionSetAnimation_hook, 0x262e, 0x6144);
  CYC(0x6144, 0x6147); interactionIncState_hook(gb);
  return;

decrement_counter2:
  CYC(0x6147, 0x6148); L = alu_inc8(gb, L);
  CYC(0x6148, 0x6149); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL)));
  if (!(F & FZ)) {
    CYCT(0x6149, 0x614a); ret_effect(gb); return;
  }
  CYC(0x6149, 0x614a);
  CYC(0x614a, 0x614c); mem_wr(gb, HL, 0x14);
  CYC(0x614c, 0x614e); L = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x614e, 0x614f); A = mem_rd(gb, HL);
  CYC(0x614f, 0x6150); A = alu_inc8(gb, A);
  CYC(0x6150, 0x6152); alu_and(gb, 7);
  CYC(0x6152, 0x6153); mem_wr(gb, HL, A);
  CYC(0x6153, 0x6155); L = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6155, 0x6156); alu_add(gb, mem_rd(gb, HL));
  CYC(0x6156, 0x6159); SET_HL(zoraAnimationTable);
  CYC(0x6159, 0x615a); push_effect(gb, 0x615a);
  zora_add_a_to_hl(gb);
  CYC(0x615a, 0x615b); A = mem_rd(gb, HL);
  CYC(0x615b, 0x615e); interactionSetAnimation_hook(gb);
  return;

state3:
  CALL_C(0x616e, interactionDecCounter1_hook, 0x23cc, 0x6171);
  if (!(F & FZ)) {
    CYCT(0x6171, 0x6173); goto update_speed_z;
  }
  CYC(0x6171, 0x6173);
  CYC(0x6173, 0x6176); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x6176, 0x6178); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);

update_speed_z:
  CYC(0x6178, 0x617a); C = 0x20;
  CALL_C(0x617a, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x617d);
  if (!(F & FZ)) {
    CYCT(0x617d, 0x617e); ret_effect(gb); return;
  }
  CYC(0x617d, 0x617e);
  CYC(0x617e, 0x6180); L = INTERACTION_BASE + OBJ_SPEED_Z;
  CYC(0x6180, 0x6182); A = 0x80;
  CYC(0x6182, 0x6183); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(0x6183, 0x6185); mem_wr(gb, HL, 0xfe);
  CYC(0x6185, 0x6186); ret_effect(gb);
}

void zora_subid0D_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x60e4, checkInteractionState_hook, 0x23fe, 0x60e7);
  if (F & FZ) {
    CYCT(0x60e7, 0x60e9); goto state0;
  }
  CYC(0x60e7, 0x60e9);

  CYC(0x60e9, 0x60eb); C = 0x20;
  CALL_C(0x60eb, objectUpdateSpeedZ_paramC_hook, 0x1f46, 0x60ee);
  if (!(F & FZ)) {
    CYCT(0x60ee, 0x60ef); ret_effect(gb); return;
  }
  CYC(0x60ee, 0x60ef);
  CALL_C(0x60ef, interactionRunScript_hook, 0x2552, 0x60f2);
  if (!(F & FC)) {
    CYCT(0x60f2, 0x60f4); goto animate;
  }
  CYC(0x60f2, 0x60f4);
  CYC(0x60f4, 0x60f7); SET_HL(wTmpcfc0_genericCutscene_state);
  CYC(0x60f7, 0x60f9); mem_wr(gb, HL, mem_rd(gb, HL) | 0x80);

animate:
  CYC(0x60f9, 0x60fc); interactionAnimate_hook(gb);
  return;

state0:
  CYC(0x60fc, 0x60fe); E = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x60fe, 0x6100); A = 0x3c;
  CYC(0x6100, 0x6101); mem_wr(gb, DE, A);
  CYC(0x6101, 0x6103); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6103, 0x6104); A = mem_rd(gb, DE);
  CYC(0x6104, 0x6106); alu_cp(gb, 0x0c);
  CYC(0x6106, 0x6109); SET_HL(0x7bf2);
  if (F & FZ) {
    CYCT(0x6109, 0x610b); zora_commonInitWithScript_hook(gb); return;
  }
  CYC(0x6109, 0x610b);
  CYC(0x610b, 0x610e); SET_HL(0x7c0a);
  CYC(0x610e, 0x6110); zora_commonInitWithScript_hook(gb);
}

void zora_subid12__afterCall6193_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6193, objectSetVisible82_hook, 0x1e69, 0x6196);
  CALL_C(0x6196, interactionIncState_hook, 0x23e0, 0x6199);
  CYC(0x6199, 0x619b); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x619b, 0x619c); A = mem_rd(gb, DE);
  CYC(0x619c, 0x619e); alu_sub(gb, 0x10);
  CYC(0x619e, 0x619f); B = A;
  CYC(0x619f, 0x61a2); SET_HL(zoraScriptTable);
  zora_add_double_index(gb, 0x61a3);
  CYC(0x61a3, 0x61a4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x61a4, 0x61a5); H = mem_rd(gb, HL);
  CYC(0x61a5, 0x61a6); L = A;
  CALL_C(0x61a6, interactionSetScript_hook, 0x2544, 0x61a9);
  CYC(0x61a9, 0x61aa); A = B;
  CYC(0x61aa, 0x61ab); push_effect(gb, 0x61ab);
  switch (zora_jump_table(gb)) {
    case 0x61b1: goto subid10;
    case 0x61c9: goto subid11;
    case 0x61d1: goto subid12;
    default: hook_continue(gb, HL, sp0_); return;
  }

subid10:
  CALL_C(0x61b1, getThisRoomFlags_hook, 0x197d, 0x61b4);
  CYC(0x61b4, 0x61b6); alu_and(gb, 0x20);
  if (!(F & FZ)) {
    CYCT(0x61b6, 0x61b9); interactionDelete_hook(gb); return;
  }
  CYC(0x61b6, 0x61b9);
  CYC(0x61b9, 0x61bc); A = mem_rd(gb, wEssencesObtained);
  CYC(0x61bc, 0x61be); alu_bit(gb, 6, A);
  if (F & FZ) {
    CYCT(0x61be, 0x61c1); interactionDelete_hook(gb); return;
  }
  CYC(0x61be, 0x61c1);
  CYC(0x61c1, 0x61c3); A = 3;
  CALL_C(0x61c3, interactionSetAnimation_hook, 0x262e, 0x61c6);
  CYC(0x61c6, 0x61c9); interactionIncState_hook(gb);
  return;

subid11:
  CALL_C(0x61c9, checkIsLinkedGame_hook, 0x1992, 0x61cc);
  if (!(F & FZ)) {
    CYCT(0x61cc, 0x61cf); interactionDelete_hook(gb); return;
  }
  CYC(0x61cc, 0x61cf);
  CYC(0x61cf, 0x61d1); goto delete_if_flag_set;

subid12:
  CALL_C(0x61d1, checkIsLinkedGame_hook, 0x1992, 0x61d4);
  if (F & FZ) {
    CYCT(0x61d4, 0x61d7); interactionDelete_hook(gb); return;
  }
  CYC(0x61d4, 0x61d7);

delete_if_flag_set:
  CALL_C(0x61d7, getThisRoomFlags_hook, 0x197d, 0x61da);
  CYC(0x61da, 0x61dc); alu_and(gb, 0x40);
  if (!(F & FZ)) {
    CYCT(0x61dc, 0x61df); interactionDelete_hook(gb); return;
  }
  CYC(0x61dc, 0x61df);
  CYC(0x61df, 0x61e0); ret_effect(gb);
}

void zora_subid12_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6186, 0x6188); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x6188, 0x6189); A = mem_rd(gb, DE);
  CYC(0x6189, 0x618a); push_effect(gb, 0x618a);
  switch (zora_jump_table(gb)) {
    case 0x6190: goto state0;
    case 0x61e6: goto state1;
    case 0x61ef: goto state2;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CALL_C(0x6190, interactionInitGraphics_hook, 0x15fb, 0x6193);
  zora_subid12__afterCall6193_hook(gb);
  return;

state1:
  CALL_C(0x61e6, interactionRunScript_hook, 0x2552, 0x61e9);
  if (F & FC) {
    CYCT(0x61e9, 0x61ec); interactionDelete_hook(gb); return;
  }
  CYC(0x61e9, 0x61ec);
  CYC(0x61ec, 0x61ef); npcFaceLinkAndAnimate_hook(gb);
  return;

state2:
  CALL_C(0x61ef, interactionRunScript_hook, 0x2552, 0x61f2);
  if (F & FC) {
    CYCT(0x61f2, 0x61f5); interactionDelete_hook(gb); return;
  }
  CYC(0x61f2, 0x61f5);
  CYC(0x61f5, 0x61f8); interactionAnimate_hook(gb);
}

void zora_subid0E__afterCall6206_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6206, interactionIncState_hook, 0x23e0, 0x6209);
  CYC(0x6209, 0x620b); L = INTERACTION_BASE + OBJ_TEXT_ID + 1;
  CYC(0x620b, 0x620d); mem_wr(gb, HL, 0x34);
  CYC(0x620d, 0x620f); A = 0x30;
  CALL_C(0x620f, checkGlobalFlag_hook, 0x31f3, 0x6212);
  CYC(0x6212, 0x6214); A = 0x33;
  if (F & FZ) {
    CYCT(0x6214, 0x6216); goto set_text_id;
  }
  CYC(0x6214, 0x6216);
  CYC(0x6216, 0x6218); A = 0x34;

set_text_id:
  CYC(0x6218, 0x621a); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x621a, 0x621b); mem_wr(gb, DE, A);
  CYC(0x621b, 0x621c); alu_xor(gb, A);
  CALL_C(0x621c, interactionSetAnimation_hook, 0x262e, 0x621f);
  CALL_C(0x621f, objectSetVisiblec2_hook, 0x1e45, 0x6222);
  CYC(0x6222, 0x6225); SET_HL(0x7c21);
  CYC(0x6225, 0x6228); interactionSetScript_hook(gb);
}

void zora_subid0E_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x61f8, checkInteractionState_hook, 0x23fe, 0x61fb);
  if (F & FZ) {
    CYCT(0x61fb, 0x61fd); goto state0;
  }
  CYC(0x61fb, 0x61fd);

  CALL_C(0x61fd, interactionRunScript_hook, 0x2552, 0x6200);
  CYC(0x6200, 0x6203); interactionAnimateAsNpc_hook(gb);
  return;

state0:
  CALL_C(0x6203, interactionInitGraphics_hook, 0x15fb, 0x6206);
  zora_subid0E__afterCall6206_hook(gb);
}

void zora_subid1B__afterCall6236_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x6236, 0x6238); A = 0x30;
  CALL_C(0x6238, checkGlobalFlag_hook, 0x31f3, 0x623b);
  CYC(0x623b, 0x623d); B = 0;
  if (F & FZ) {
    CYCT(0x623d, 0x623f); goto select_text;
  }
  CYC(0x623d, 0x623f);
  CYC(0x623f, 0x6240); B = alu_inc8(gb, B);

select_text:
  CYC(0x6240, 0x6242); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6242, 0x6243); A = mem_rd(gb, DE);
  CYC(0x6243, 0x6245); alu_sub(gb, 0x13);
  CYC(0x6245, 0x6246); alu_add(gb, A);
  CYC(0x6246, 0x6247); alu_add(gb, B);
  CYC(0x6247, 0x624a); SET_HL(zoraTextTable);
  CYC(0x624a, 0x624b); push_effect(gb, 0x624b);
  zora_add_a_to_hl(gb);
  CYC(0x624b, 0x624d); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x624d, 0x624e); A = mem_rd(gb, HL);
  CYC(0x624e, 0x624f); mem_wr(gb, DE, A);
  CYC(0x624f, 0x6252); SET_HL(0x45f0);
  CYC(0x6252, 0x6255); interactionSetScript_hook(gb);
}

void zora_subid1B_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6228, checkInteractionState_hook, 0x23fe, 0x622b);
  if (F & FZ) {
    CYCT(0x622b, 0x622d); goto state0;
  }
  CYC(0x622b, 0x622d);

  CALL_C(0x622d, interactionRunScript_hook, 0x2552, 0x6230);
  CYC(0x6230, 0x6233); npcFaceLinkAndAnimate_hook(gb);
  return;

state0:
  CALL_C(0x6233, zora_commonInit_hook, 0x60d4, 0x6236);
  zora_subid1B__afterCall6236_hook(gb);
}

void zora_subid0F_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x60a3, checkInteractionState_hook, 0x23fe, 0x60a6);
  if (F & FZ) {
    CYCT(0x60a6, 0x60a8); goto state0;
  }
  CYC(0x60a6, 0x60a8);

  CALL_C(0x60a8, zora_getWorldState_hook, 0x6267, 0x60ab);
  CYC(0x60ab, 0x60ad); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x60ad, 0x60ae); A = mem_rd(gb, DE);
  CYC(0x60ae, 0x60af); alu_add(gb, A);
  CYC(0x60af, 0x60b0); alu_add(gb, A);
  CYC(0x60b0, 0x60b1); alu_add(gb, B);
  CYC(0x60b1, 0x60b4); SET_HL(zoraTextIndices);
  CYC(0x60b4, 0x60b5); push_effect(gb, 0x60b5);
  zora_add_a_to_hl(gb);
  CYC(0x60b5, 0x60b7); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x60b7, 0x60b8); A = mem_rd(gb, HL);
  CYC(0x60b8, 0x60b9); mem_wr(gb, DE, A);
  CALL_C(0x60b9, interactionRunScript_hook, 0x2552, 0x60bc);
  CYC(0x60bc, 0x60bf); npcFaceLinkAndAnimate_hook(gb);
  return;

state0:
  CALL_C(0x60bf, zora_getWorldState_hook, 0x6267, 0x60c2);
  CYC(0x60c2, 0x60c3); A = B;
  CYC(0x60c3, 0x60c4); alu_or(gb, A);
  CYC(0x60c4, 0x60c6); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x60c6, 0x60c7); A = mem_rd(gb, DE);
  if (!(F & FZ)) {
    CYCT(0x60c7, 0x60c9); goto initialize;
  }
  CYC(0x60c7, 0x60c9);
  CYC(0x60c9, 0x60cb); alu_cp(gb, 6);
  if (!(F & FC)) {
    CYCT(0x60cb, 0x60ce); interactionDelete_hook(gb); return;
  }
  CYC(0x60cb, 0x60ce);

initialize:
  CYC(0x60ce, 0x60d1); SET_HL(0x45f0);
  zora_commonInitWithScript_hook(gb);
}

void zora_subid00_hook(GB *gb) {
  zora_subid0F_hook(gb);
}

void zora_subid0A_hook(GB *gb) {
  zora_subid0B_hook(gb);
}

void zora_subid0C_hook(GB *gb) {
  zora_subid0D_hook(gb);
}

void zora_subid10_hook(GB *gb) {
  zora_subid12_hook(gb);
}

void zora_subid13_hook(GB *gb) {
  zora_subid1B_hook(gb);
}
