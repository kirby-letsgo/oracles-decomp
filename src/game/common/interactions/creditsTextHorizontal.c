#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0b, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0b, (from), (to), true)

static uint16_t interactionCodeae_jump_table(GB *gb) {
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

static void interactionCodeae_addAToHl(GB *gb, uint16_t return_address) {
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

static void interactionCodeae_addDoubleIndex(GB *gb, uint16_t return_address) {
  push_effect(gb, return_address);
  burn_rom(gb, 0x00, 0x0018, 0x0019, false); push_effect(gb, BC);
  burn_rom(gb, 0x00, 0x0019, 0x001a, false); C = A;
  burn_rom(gb, 0x00, 0x001a, 0x001c, false); B = 0x00;
  burn_rom(gb, 0x00, 0x001c, 0x001d, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001d, 0x001e, false); alu_add_hl(gb, BC);
  burn_rom(gb, 0x00, 0x001e, 0x001f, false); SET_BC(pop_effect(gb));
  burn_rom(gb, 0x00, 0x001f, 0x0020, false); ret_effect(gb);
}

// Mutually chained via straight fallthrough (creditsTextHorizontal_6537 -> _6554 -> _6559) and,
// on the "still empty" path, _6559 tail-jumps back into _6537. All three are bare-global.
void creditsTextHorizontal_6537_hook(GB *gb);
void creditsTextHorizontal_6554_hook(GB *gb);
void creditsTextHorizontal_6559_hook(GB *gb);

// 0b:6537, called from creditsTextHorizontal_6559 (tail `jp`) and interactionCodeae@substate0.
void creditsTextHorizontal_6537_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CALL_C(0x6537, getFreeInteractionSlot_hook, 0x3aef, 0x653a);
  if (!(F & FZ)) { CYCT(0x653a, 0x653c); goto l_6550; } // jr nz
  CYC(0x653a, 0x653c);
  CYC(0x653c, 0x653e); mem_wr(gb, HL, 0xae); // INTERAC_CREDITS_TEXT_HORIZONTAL
  CYC(0x653e, 0x653f); L = alu_inc8(gb, L);
  CYC(0x653f, 0x6541); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x6541, 0x6542); A = mem_rd(gb, DE);
  CYC(0x6542, 0x6543); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a ; [child.subid]
  CYC(0x6543, 0x6545); mem_wr(gb, HL, 0x01); // [child.var03]
  CYC(0x6545, 0x6547); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6547, 0x6548); E = L;
  CYC(0x6548, 0x6549); A = mem_rd(gb, DE);
  CYC(0x6549, 0x654a); E = alu_inc8(gb, E);
  CYC(0x654a, 0x654b); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x654b, 0x654c); A = mem_rd(gb, DE); // [counter2]
  CYC(0x654c, 0x654d); mem_wr(gb, HL, A);
  CALL_C(0x654d, objectCopyPosition_hook, 0x2242, 0x6550);

l_6550:
  CYC(0x6550, 0x6551); H = D;
  CYC(0x6551, 0x6553); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x6553, 0x6554); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  creditsTextHorizontal_6554_hook(gb); return; // falls through
}

// 0b:6554, called from interactionCodeae@subsubstate1; falls into (and out of)
// creditsTextHorizontal_6537.
void creditsTextHorizontal_6554_hook(GB *gb) {
  CYC(0x6554, 0x6556); L = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(0x6556, 0x6557); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6557, 0x6558); H = mem_rd(gb, HL);
  CYC(0x6558, 0x6559); L = A;
  creditsTextHorizontal_6559_hook(gb); return; // falls through
}

// 0b:6559, called from interactionCodeae@state0.
// @param[in] hl  Script pointer.
void creditsTextHorizontal_6559_hook(GB *gb) {
  CYC(0x6559, 0x655a); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x655a, 0x655c); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x655c, 0x655d); mem_wr(gb, DE, A);
  CYC(0x655d, 0x655e); E = alu_inc8(gb, E);
  CYC(0x655e, 0x655f); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x655f, 0x6560); mem_wr(gb, DE, A); // [var31]
  CYC(0x6560, 0x6561); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6561, 0x6563); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6563, 0x6564); mem_wr(gb, DE, A);
  CYC(0x6564, 0x6565); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6565, 0x6567); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x6567, 0x6568); mem_wr(gb, DE, A);
  CYC(0x6568, 0x656a); E = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(0x656a, 0x656b); A = L;
  CYC(0x656b, 0x656c); mem_wr(gb, DE, A);
  CYC(0x656c, 0x656d); E = alu_inc8(gb, E);
  CYC(0x656d, 0x656e); A = H;
  CYC(0x656e, 0x656f); mem_wr(gb, DE, A);
  CYC(0x656f, 0x6571); E = INTERACTION_BASE + 0x31; // Interaction.var31
  CYC(0x6571, 0x6572); A = mem_rd(gb, DE);
  CYC(0x6572, 0x6573); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6573, 0x6574); ret_effect(gb); return; } // ret nz
  CYC(0x6573, 0x6574);
  CYC(0x6574, 0x6575); E = alu_dec8(gb, E);
  CYC(0x6575, 0x6576); A = mem_rd(gb, DE); // [var30]
  CYC(0x6576, 0x6577); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6577, 0x6578); ret_effect(gb); return; } // ret nz
  CYC(0x6577, 0x6578);
  CYC(0x6578, 0x657b); creditsTextHorizontal_6537_hook(gb); return; // jp
}

// 0b:657b, called from interactionCodeae@state1 (tail `jp`).
void horizontalCreditsText_var03Nonzero_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x657b, 0x657e); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x657e, 0x657f); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x657f, 0x6580); ret_effect(gb); return; } // ret nz
  CYC(0x657f, 0x6580);
  CYC(0x6580, 0x6582); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6582, 0x6583); A = mem_rd(gb, DE);
  CYC(0x6583, 0x6584); push_effect(gb, 0x6584);
  switch (interactionCodeae_jump_table(gb)) {
    case 0x6588: goto substate0;
    case 0x65a5: goto substate1;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x6588, 0x6589); H = D;
  CYC(0x6589, 0x658b); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x658b, 0x658c); mem_wr(gb, HL, alu_dec8(gb, mem_rd(gb, HL))); // dec (hl)
  if (!(F & FZ)) { CYCT(0x658c, 0x658e); goto applySpeed; } // jr nz
  CYC(0x658c, 0x658e);
  CALL_C(0x658e, interactionIncSubstate_hook, 0x23e5, 0x6591);
  CYC(0x6591, 0x6593); B = 0xa0;
  CYC(0x6593, 0x6595); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6595, 0x6596); A = mem_rd(gb, HL);
  CYC(0x6596, 0x6597); alu_or(gb, A);
  if (F & FZ) { CYCT(0x6597, 0x6599); goto l_659b; } // jr z
  CYC(0x6597, 0x6599);
  CYC(0x6599, 0x659b); B = 0x50;

l_659b:
  CYC(0x659b, 0x659d); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x659d, 0x659e); mem_wr(gb, HL, B);
  RET(0x659e); return;

applySpeed:
  CALL_C(0x659f, objectApplySpeed_hook, 0x201d, 0x65a2);
  CYC(0x65a2, 0x65a5); objectApplySpeed_hook(gb); return; // jp

substate1:
  CYC(0x65a5, 0x65a7); E = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x65a7, 0x65a8); A = mem_rd(gb, DE);
  CYC(0x65a8, 0x65a9); A = alu_inc8(gb, A);
  if (F & FZ) { CYCT(0x65a9, 0x65aa); ret_effect(gb); return; } // ret z
  CYC(0x65a9, 0x65aa);
  CALL_C(0x65aa, interactionDecCounter1_hook, 0x23cc, 0x65ad);
  if (F & FZ) { CYCT(0x65ad, 0x65b0); interactionDelete_hook(gb); return; } // jp z
  CYC(0x65ad, 0x65b0);
  RET(0x65b0); return;
}

// ==================================================================================================
// INTERAC_CREDITS_TEXT_HORIZONTAL
//
// Variables:
//   var03: ?
//   var30: ?
//   var31: ?
//   var32: ?
//   var33: ?
// ==================================================================================================
void interactionCodeae_hook(GB *gb) {
  uint16_t sp0_ = gb->sp;
  CYC(0x63dd, 0x63df); E = INTERACTION_BASE + OBJ_STATE;
  CYC(0x63df, 0x63e0); A = mem_rd(gb, DE);
  CYC(0x63e0, 0x63e1); push_effect(gb, 0x63e1);
  switch (interactionCodeae_jump_table(gb)) {
    case 0x63e5: goto state0;
    case 0x6434: goto state1;
    default: hook_continue(gb, HL, sp0_); return;
  }

state0:
  CYC(0x63e5, 0x63e7); A = 0x01;
  CYC(0x63e7, 0x63e8); mem_wr(gb, DE, A); // [state]
  CYC(0x63e8, 0x63ea); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x63ea, 0x63eb); A = mem_rd(gb, DE);
  CYC(0x63eb, 0x63ec); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x63ec, 0x63ee); goto var03Nonzero; } // jr nz
  CYC(0x63ec, 0x63ee);
  CYC(0x63ee, 0x63ef); H = D;
  CYC(0x63ef, 0x63f1); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x63f1, 0x63f2); A = mem_rd(gb, HL);
  CYC(0x63f2, 0x63f5); SET_HL(0x65c1); // horizontalCreditsText_scriptTable
  CYC(0x63f5, 0x63f6); interactionCodeae_addDoubleIndex(gb, 0x63f6);
  CYC(0x63f6, 0x63f7); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x63f7, 0x63f8); H = mem_rd(gb, HL);
  CYC(0x63f8, 0x63f9); L = A;
  CALL_C(0x63f9, creditsTextHorizontal_6559_hook, 0x6559, 0x63fc);
  CYC(0x63fc, 0x63fe); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x63fe, 0x63ff); A = mem_rd(gb, DE);
  CYC(0x63ff, 0x6402); SET_HL(0x65b1); // horizontalCreditsText_65b1
  CYC(0x6402, 0x6403); interactionCodeae_addDoubleIndex(gb, 0x6403);
  CYC(0x6403, 0x6404); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6404, 0x6406); E = INTERACTION_BASE + OBJ_VAR32;
  CYC(0x6406, 0x6407); mem_wr(gb, DE, A);
  CYC(0x6407, 0x6408); A = mem_rd(gb, HL); SET_HL(HL + 1); // ldi a,(hl)
  CYC(0x6408, 0x640a); E = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x640a, 0x640b); mem_wr(gb, DE, A);
  RET(0x640b); return;

var03Nonzero:
  CALL_C(0x640c, interactionInitGraphics_hook, 0x15fb, 0x640f);
  CYC(0x640f, 0x6410); H = D;
  CYC(0x6410, 0x6412); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x6412, 0x6414); mem_wr(gb, HL, 0x14);
  CYC(0x6414, 0x6416); L = INTERACTION_BASE + OBJ_SPEED;
  CYC(0x6416, 0x6418); mem_wr(gb, HL, 0x50); // SPEED_200
  CYC(0x6418, 0x641a); L = INTERACTION_BASE + OBJ_COUNTER2;
  CYC(0x641a, 0x641b); A = mem_rd(gb, HL);
  CALL_C(0x641b, interactionSetAnimation_hook, 0x262e, 0x641e);
  CYC(0x641e, 0x641f); H = D;
  CYC(0x641f, 0x6421); L = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6421, 0x6422); A = mem_rd(gb, HL);
  CYC(0x6422, 0x6423); alu_or(gb, A);
  CYC(0x6423, 0x6426); SET_BC(0xf018);
  if (F & FZ) { CYCT(0x6426, 0x6428); goto l_642b; } // jr z
  CYC(0x6426, 0x6428);
  CYC(0x6428, 0x642b); SET_BC(0x0008);

l_642b:
  CYC(0x642b, 0x642d); L = INTERACTION_BASE + OBJ_XH;
  CYC(0x642d, 0x642e); mem_wr(gb, HL, B);
  CYC(0x642e, 0x6430); L = INTERACTION_BASE + OBJ_ANGLE;
  CYC(0x6430, 0x6431); mem_wr(gb, HL, C);
  CYC(0x6431, 0x6434); objectSetVisible82_hook(gb); return; // jp

state1:
  CYC(0x6434, 0x6436); A = 0x01;
  CYC(0x6436, 0x6437); mem_wr(gb, DE, A); // [state]
  CYC(0x6437, 0x6439); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6439, 0x643a); A = mem_rd(gb, DE);
  CYC(0x643a, 0x643b); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x643b, 0x643e); horizontalCreditsText_var03Nonzero_hook(gb); return; } // jp nz
  CYC(0x643b, 0x643e);
  CYC(0x643e, 0x6441); A = mem_rd(gb, wPaletteThread_mode);
  CYC(0x6441, 0x6442); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x6442, 0x6443); ret_effect(gb); return; } // ret nz
  CYC(0x6442, 0x6443);
  CYC(0x6443, 0x6445); E = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x6445, 0x6446); A = mem_rd(gb, DE);
  CYC(0x6446, 0x6447); push_effect(gb, 0x6447);
  switch (interactionCodeae_jump_table(gb)) {
    case 0x644d: goto substate0;
    case 0x646c: goto substate1;
    case 0x652b: goto substate2;
    default: hook_continue(gb, HL, sp0_); return;
  }

substate0:
  CYC(0x644d, 0x644e); H = D;
  CYC(0x644e, 0x6450); L = INTERACTION_BASE + 0x30; // Interaction.var30
  CALL_C(0x6450, decHlRef16WithCap_hook, 0x0237, 0x6453);
  if (!(F & FZ)) { CYCT(0x6453, 0x6454); ret_effect(gb); return; } // ret nz
  CYC(0x6453, 0x6454);
  CALL_C(0x6454, creditsTextHorizontal_6537_hook, 0x6537, 0x6457);
  // falls through into @func_6457

func_6457:
  CYC(0x6457, 0x6459); E = INTERACTION_BASE + 0x30; // Interaction.var30
  CYC(0x6459, 0x645a); A = mem_rd(gb, DE);
  CYC(0x645a, 0x645b); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x645b, 0x645c); ret_effect(gb); return; } // ret nc
  CYC(0x645b, 0x645c);
  CYC(0x645c, 0x645e); B = 0x01;
  CYC(0x645e, 0x645f); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x645f, 0x6461); goto l_6463; } // jr nc
  CYC(0x645f, 0x6461);
  CYC(0x6461, 0x6463); B = 0x02;

l_6463:
  CYC(0x6463, 0x6464); H = D;
  CYC(0x6464, 0x6466); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x6466, 0x6468); mem_wr(gb, HL, 0xb4); // 180
  CYC(0x6468, 0x646a); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x646a, 0x646b); mem_wr(gb, HL, B);
  RET(0x646b); return;

substate1:
  CYC(0x646c, 0x646e); E = INTERACTION_BASE + OBJ_VAR33;
  CYC(0x646e, 0x646f); A = mem_rd(gb, DE);
  CYC(0x646f, 0x6470); push_effect(gb, 0x6470);
  switch (interactionCodeae_jump_table(gb)) {
    case 0x6478: goto subsubstate0;
    case 0x6481: goto subsubstate1;
    case 0x64cf: goto subsubstate2;
    case 0x64db: goto subsubstate3;
    default: hook_continue(gb, HL, sp0_); return;
  }

subsubstate0:
  CALL_C(0x6478, interactionDecCounter1_hook, 0x23cc, 0x647b);
  if (!(F & FZ)) { CYCT(0x647b, 0x647c); ret_effect(gb); return; } // ret nz
  CYC(0x647b, 0x647c);
  CYC(0x647c, 0x647d); H = D;
  CYC(0x647d, 0x647f); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(0x647f, 0x6480); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(0x6480); return;

subsubstate1:
  CYC(0x6481, 0x6484); A = mem_rd(gb, wFrameCounter);
  CYC(0x6484, 0x6486); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x6486, 0x6487); ret_effect(gb); return; } // ret nz
  CYC(0x6486, 0x6487);
  CYC(0x6487, 0x6488); H = D;
  CYC(0x6488, 0x648a); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x648a, 0x648b); A = mem_rd(gb, HL);
  CYC(0x648b, 0x648d); alu_cp(gb, 0x10);
  if (!(F & FZ)) { CYCT(0x648d, 0x648f); goto label_0b_234; } // jr nz
  CYC(0x648d, 0x648f);
  CYC(0x648f, 0x6491); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(0x6491, 0x6492); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x6492, 0x6494); L = INTERACTION_BASE + OBJ_SCRIPT_PTR;
  CYC(0x6494, 0x6495); A = mem_rd(gb, HL);
  CYC(0x6495, 0x6497); alu_sub(gb, 0x03);
  CYC(0x6497, 0x6498); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x6498, 0x6499); A = mem_rd(gb, HL);
  CYC(0x6499, 0x649b); alu_sbc(gb, 0x00);
  CYC(0x649b, 0x649c); mem_wr(gb, HL, A);
  CALL_C(0x649c, creditsTextHorizontal_6554_hook, 0x6554, 0x649f);
  CYC(0x649f, 0x64a0); H = D;
  CYC(0x64a0, 0x64a2); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x64a2, 0x64a4); mem_wr(gb, HL, 0x1e);
  RET(0x64a4); return;

label_0b_234:
  CYC(0x64a5, 0x64a7); A = hram_rd(gb, R_SVBK);
  CYC(0x64a7, 0x64a8); push_effect(gb, AF); // push af
  CYC(0x64a8, 0x64aa); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x64aa, 0x64ab); A = mem_rd(gb, HL);
  CYC(0x64ab, 0x64ac); B = A;
  CYC(0x64ac, 0x64ae); A = 0x04; // :w4TileMap
  CYC(0x64ae, 0x64b0); hram_wr(gb, R_SVBK, A);
  CYC(0x64b0, 0x64b1); A = B;
  CYC(0x64b1, 0x64b4); SET_HL(w4TileMap);
  CYC(0x64b4, 0x64b5); interactionCodeae_addDoubleIndex(gb, 0x64b5);
  CYC(0x64b5, 0x64b7); B = 0x30;

loop:
  CYC(0x64b7, 0x64b8); alu_xor(gb, A);
  CYC(0x64b8, 0x64b9); mem_wr(gb, HL, A); SET_HL(HL + 1); // ldi (hl),a
  CYC(0x64b9, 0x64ba); mem_wr(gb, HL, A);
  CYC(0x64ba, 0x64bc); A = 0x1f;
  CYC(0x64bc, 0x64bd); interactionCodeae_addAToHl(gb, 0x64bd);
  CYC(0x64bd, 0x64be); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x64be, 0x64c0); goto loop; } // jr nz
  CYC(0x64be, 0x64c0);
  CYC(0x64c0, 0x64c1); push_effect(gb, DE); // push de
  CYC(0x64c1, 0x64c3); A = 0x09; // UNCMP_GFXH_09
  CALL_C(0x64c3, loadUncompressedGfxHeader_hook, 0x05da, 0x64c6);
  CYC(0x64c6, 0x64c7); SET_DE(pop_effect(gb)); // pop de
  CYC(0x64c7, 0x64c8); SET_AF(pop_effect(gb)); // pop af
  CYC(0x64c8, 0x64ca); hram_wr(gb, R_SVBK, A);
  CYC(0x64ca, 0x64cb); H = D;
  CYC(0x64cb, 0x64cd); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x64cd, 0x64ce); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  RET(0x64ce); return;

subsubstate2:
  CALL_C(0x64cf, interactionDecCounter1_hook, 0x23cc, 0x64d2);
  if (!(F & FZ)) { CYCT(0x64d2, 0x64d3); ret_effect(gb); return; } // ret nz
  CYC(0x64d2, 0x64d3);
  CYC(0x64d3, 0x64d5); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(0x64d5, 0x64d6); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL))); // inc (hl)
  CYC(0x64d6, 0x64d8); L = INTERACTION_BASE + OBJ_COUNTER1;
  CYC(0x64d8, 0x64da); mem_wr(gb, HL, 0x10);
  RET(0x64da); return;

subsubstate3:
  CYC(0x64db, 0x64de); A = mem_rd(gb, wFrameCounter);
  CYC(0x64de, 0x64e0); alu_and(gb, 0x03);
  if (!(F & FZ)) { CYCT(0x64e0, 0x64e1); ret_effect(gb); return; } // ret nz
  CYC(0x64e0, 0x64e1);
  CALL_C(0x64e1, interactionDecCounter1_hook, 0x23cc, 0x64e4);
  if (!(F & FZ)) { CYCT(0x64e4, 0x64e6); goto label_0b_236; } // jr nz
  CYC(0x64e4, 0x64e6);
  CYC(0x64e6, 0x64e7); alu_xor(gb, A);
  CYC(0x64e7, 0x64e9); L = INTERACTION_BASE + OBJ_SUBSTATE;
  CYC(0x64e9, 0x64ea); mem_wr(gb, HL, A);
  CYC(0x64ea, 0x64ec); L = INTERACTION_BASE + OBJ_VAR33;
  CYC(0x64ec, 0x64ed); mem_wr(gb, HL, A);
  CYC(0x64ed, 0x64f0); goto func_6457; // jp

label_0b_236:
  CYC(0x64f0, 0x64f1); push_effect(gb, DE); // push de
  CYC(0x64f1, 0x64f3); A = hram_rd(gb, R_SVBK);
  CYC(0x64f3, 0x64f4); push_effect(gb, AF); // push af
  CYC(0x64f4, 0x64f5); A = mem_rd(gb, HL); // [counter1]
  CYC(0x64f5, 0x64f6); B = A;
  CYC(0x64f6, 0x64f7); A = B;
  CYC(0x64f7, 0x64fa); SET_HL(w4TileMap);
  CYC(0x64fa, 0x64fb); interactionCodeae_addDoubleIndex(gb, 0x64fb);
  CYC(0x64fb, 0x64fc); A = B;
  CYC(0x64fc, 0x64ff); SET_DE(w3VramTiles);
  CALL_C(0x64ff, addDoubleIndexToDe_hook, 0x0072, 0x6502);
  CYC(0x6502, 0x6504); B = 0x30;

tileLoop:
  CYC(0x6504, 0x6505); push_effect(gb, BC); // push bc
  CYC(0x6505, 0x6507); A = 0x03; // :w3VramTiles
  CYC(0x6507, 0x6509); hram_wr(gb, R_SVBK, A);
  CYC(0x6509, 0x650a); A = mem_rd(gb, DE);
  CYC(0x650a, 0x650b); B = A;
  CYC(0x650b, 0x650c); SET_DE(DE + 1); // inc de
  CYC(0x650c, 0x650d); A = mem_rd(gb, DE);
  CYC(0x650d, 0x650e); C = A;
  CYC(0x650e, 0x6510); A = 0x04; // :w4TileMap
  CYC(0x6510, 0x6512); hram_wr(gb, R_SVBK, A);
  CYC(0x6512, 0x6513); mem_wr(gb, HL, B);
  CYC(0x6513, 0x6514); SET_HL(HL + 1); // inc hl
  CYC(0x6514, 0x6515); mem_wr(gb, HL, C);
  CYC(0x6515, 0x6517); A = 0x1f;
  CYC(0x6517, 0x6518); C = A;
  CYC(0x6518, 0x6519); interactionCodeae_addAToHl(gb, 0x6519);
  CYC(0x6519, 0x651a); A = C;
  CALL_C(0x651a, addAToDe_hook, 0x0068, 0x651d);
  CYC(0x651d, 0x651e); SET_BC(pop_effect(gb)); // pop bc
  CYC(0x651e, 0x651f); B = alu_dec8(gb, B);
  if (!(F & FZ)) { CYCT(0x651f, 0x6521); goto tileLoop; } // jr nz
  CYC(0x651f, 0x6521);
  CYC(0x6521, 0x6523); A = 0x09; // UNCMP_GFXH_09
  CALL_C(0x6523, loadUncompressedGfxHeader_hook, 0x05da, 0x6526);
  CYC(0x6526, 0x6527); SET_AF(pop_effect(gb)); // pop af
  CYC(0x6527, 0x6529); hram_wr(gb, R_SVBK, A);
  CYC(0x6529, 0x652a); SET_DE(pop_effect(gb)); // pop de
  RET(0x652a); return;

substate2:
  CALL_C(0x652b, interactionDecCounter1_hook, 0x23cc, 0x652e);
  if (!(F & FZ)) { CYCT(0x652e, 0x652f); ret_effect(gb); return; } // ret nz
  CYC(0x652e, 0x652f);
  CYC(0x652f, 0x6532); SET_HL(wTmpcfc0_genericCutscene_cfdf);
  CYC(0x6532, 0x6534); mem_wr(gb, HL, 0xff);
  CYC(0x6534, 0x6537); interactionDelete_hook(gb); return; // jp
}
