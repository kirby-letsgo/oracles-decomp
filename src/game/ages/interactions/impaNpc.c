#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x09, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x09, (from), (to), true)

static uint16_t impa_npc_jump_table(GB *gb) {
  burn_rom(gb, 0x00, 0x0000, 0x0001, false); alu_add(gb, A);
  burn_rom(gb, 0x00, 0x0001, 0x0002, false); SET_HL(pop_effect(gb));
  burn_rom(gb, 0x00, 0x0002, 0x0003, false); alu_add(gb, L);
  burn_rom(gb, 0x00, 0x0003, 0x0004, false); L = A;
  if (F & FC) {
    burn_rom(gb, 0x00, 0x0004, 0x0006, false);
    burn_rom(gb, 0x00, 0x0006, 0x0007, false); H = alu_inc8(gb, H);
  } else burn_rom(gb, 0x00, 0x0004, 0x0006, true);
  burn_rom(gb, 0x00, 0x0007, 0x0008, false); A = mem_rd(gb, HL); SET_HL(HL + 1);
  burn_rom(gb, 0x00, 0x0008, 0x0009, false); H = mem_rd(gb, HL);
  burn_rom(gb, 0x00, 0x0009, 0x000a, false); L = A;
  burn_rom(gb, 0x00, 0x000a, 0x000b, false);
  return HL;
}

void impaNpc_subid00_hook(GB *gb);
void impaNpc_setScriptAndInitialize_hook(GB *gb);
void impaNpc_determineTextAndPositionInHouse_hook(GB *gb);
void impaNpc_subid01_hook(GB *gb);
void impaNpc_setTextIndexAndLoadGenericNpcScript_hook(GB *gb);
void impaNpc_subid02_hook(GB *gb);
void impaNpc_runScriptAndFaceLink_hook(GB *gb);
void impaNpc_subid03_hook(GB *gb);
void impaNpc_faceLinkIfClose_hook(GB *gb);
void getImpaNpcState_hook(GB *gb);

static void impa_set_text_and_position(GB *gb) {
  CYC(0x64d4, 0x64d6); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x64d6, 0x64d7); mem_wr(gb, DE, A);
  CYC(0x64d7, 0x64d9); E = INTERACTION_BASE + OBJ_YH;
  CYC(0x64d9, 0x64da); A = B;
  CYC(0x64da, 0x64db); mem_wr(gb, DE, A);
  CYC(0x64db, 0x64dd); E = INTERACTION_BASE + OBJ_XH;
  CYC(0x64dd, 0x64de); A = C;
  CYC(0x64de, 0x64df); mem_wr(gb, DE, A);
  CYC(0x64df, 0x64e1); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x64e1, 0x64e3); A = 2;
  CYC(0x64e3, 0x64e4); mem_wr(gb, DE, A);
  CYC(0x64e4, 0x64e7); SET_HL(0x45f0);
  CYC(0x64e7, 0x64e8); alu_xor(gb, A);
  CYC(0x64e8, 0x64e9); ret_effect(gb);
}

void interactionCode4f_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x642e, 0x6430); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(0x6430, 0x6431); A = mem_rd(gb, DE);
  CYC(0x6431, 0x6432); push_effect(gb, 0x6432);
  switch (impa_npc_jump_table(gb)) {
    case 0x643a: impaNpc_subid00_hook(gb); return;
    case 0x64e9: impaNpc_subid01_hook(gb); return;
    case 0x650e: impaNpc_subid02_hook(gb); return;
    case 0x6529: impaNpc_subid03_hook(gb); return;
    default: HANDOFF(HL);
  }
}

void impaNpc_subid00_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x643a, checkInteractionState_hook, 0x23fe, 0x643d);
  if (F & FZ) { CYCT(0x643d, 0x643f); goto state0; }
  CYC(0x643d, 0x643f);
state1:
  CALL_C(0x643f, interactionRunScript_hook, 0x2552, 0x6442);
  CYC(0x6442, 0x6444); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6444, 0x6445); A = mem_rd(gb, DE);
  CYC(0x6445, 0x6446); A = alu_dec8(gb, A);
  if (F & FZ) { CYCT(0x6446, 0x6448); goto animate; }
  CYC(0x6446, 0x6448);
  CYC(0x6448, 0x644a); alu_cp(gb, 9);
  if (!(F & FZ)) CALL_C_CC(0x644a, impaNpc_faceLinkIfClose_hook, 0x653b, 0x644d);
  else CYC(0x644a, 0x644d);
animate:
  CYC(0x644d, 0x6450); interactionAnimateAsNpc_hook(gb);
  return;
state0:
  CYC(0x6450, 0x6453); SET_HL(wRoomLayout + 0x22);
  CYC(0x6453, 0x6455); mem_wr(gb, HL, 0x45);
  CALL_C(0x6455, getImpaNpcState_hook, 0x655a, 0x6458);
  CYC(0x6458, 0x645a); alu_bit(gb, 7, B);
  if (!(F & FZ)) { CYCT(0x645a, 0x645d); interactionDelete_hook(gb); return; }
  CYC(0x645a, 0x645d);
  CALL_C(0x645d, checkIsLinkedGame_hook, 0x1992, 0x6460);
  if (F & FZ) { CYCT(0x6460, 0x6462); goto choose; }
  CYC(0x6460, 0x6462);
  CYC(0x6462, 0x6464); A = 9;
choose:
  CYC(0x6464, 0x6465); alu_add(gb, B);
  CALL_C(0x6465, impaNpc_determineTextAndPositionInHouse_hook, 0x6481, 0x6468);
  impaNpc_setScriptAndInitialize_hook(gb);
}

void impaNpc_setScriptAndInitialize_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6468, interactionSetScript_hook, 0x2544, 0x646b);
  CALL_C(0x646b, interactionInitGraphics_hook, 0x15fb, 0x646e);
  CALL_C(0x646e, interactionIncState_hook, 0x23e0, 0x6471);
  CYC(0x6471, 0x6473); L = INTERACTION_BASE + OBJ_TEXT_ID + 1;
  CYC(0x6473, 0x6475); mem_wr(gb, HL, 1);
  CALL_C(0x6475, objectMarkSolidPosition_hook, 0x24f0, 0x6478);
  CYC(0x6478, 0x647a); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x647a, 0x647b); A = mem_rd(gb, DE);
  CALL_C(0x647b, interactionSetAnimation_hook, 0x262e, 0x647e);
  CYC(0x647e, 0x6481); objectSetVisiblec2_hook(gb);
}

void impaNpc_determineTextAndPositionInHouse_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6481, 0x6483); E = INTERACTION_BASE + OBJ_VAR03;
  CYC(0x6483, 0x6484); mem_wr(gb, DE, A);
  CYC(0x6484, 0x6485); push_effect(gb, 0x6485);
  switch (impa_npc_jump_table(gb)) {
    case 0x64a9:
      CYC(0x64a9, 0x64aa); SET_HL(pop_effect(gb));
      CYC(0x64aa, 0x64ad); interactionDelete_hook(gb);
      return;
    case 0x64ad:
      CYC(0x64ad, 0x64b0); SET_BC(0x3838);
      CYC(0x64b0, 0x64b2); A = 0x20;
      CYC(0x64b2, 0x64b4); impa_set_text_and_position(gb);
      return;
    case 0x64b4:
      CYC(0x64b4, 0x64b7); SET_BC(0x4828);
      CYC(0x64b7, 0x64b9); A = 0x21;
      CYC(0x64b9, 0x64bc); push_effect(gb, 0x64bc); impa_set_text_and_position(gb);
      CYC(0x64bc, 0x64bd); mem_wr(gb, DE, A);
      CYC(0x64bd, 0x64c0); SET_HL(0x64e1);
      CYC(0x64c0, 0x64c1); ret_effect(gb);
      return;
    case 0x64c1:
      CYC(0x64c1, 0x64c4); SET_BC(0x2868);
      CYC(0x64c4, 0x64c6); A = 0x22;
      CYC(0x64c6, 0x64c8); impa_set_text_and_position(gb);
      return;
    case 0x64c8:
      CYC(0x64c8, 0x64cb); SET_BC(0x2868);
      CYC(0x64cb, 0x64cd); A = 0x2c;
      CYC(0x64cd, 0x64cf); impa_set_text_and_position(gb);
      return;
    case 0x64cf:
      CYC(0x64cf, 0x64d2); SET_BC(0x2868);
      CYC(0x64d2, 0x64d4); A = 0x23;
      impa_set_text_and_position(gb);
      return;
    default: HANDOFF(HL);
  }
}

void impaNpc_subid01_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x64e9, checkInteractionState_hook, 0x23fe, 0x64ec);
  if (!(F & FZ)) { CYCT(0x64ec, 0x64ee); impaNpc_runScriptAndFaceLink_hook(gb); return; }
  CYC(0x64ec, 0x64ee);
  CALL_C(0x64ee, getImpaNpcState_hook, 0x655a, 0x64f1);
  CYC(0x64f1, 0x64f2); A = B;
  CYC(0x64f2, 0x64f4); alu_cp(gb, 7);
  if (!(F & FZ)) { CYCT(0x64f4, 0x64f7); interactionDelete_hook(gb); return; }
  CYC(0x64f4, 0x64f7);
  CALL_C(0x64f7, checkIsLinkedGame_hook, 0x1992, 0x64fa);
  CYC(0x64fa, 0x64fc); A = 0x2b;
  if (F & FZ) { CYCT(0x64fc, 0x64fe); impaNpc_setTextIndexAndLoadGenericNpcScript_hook(gb); return; }
  CYC(0x64fc, 0x64fe);
  CYC(0x64fe, 0x6500); A = 0x2e;
  impaNpc_setTextIndexAndLoadGenericNpcScript_hook(gb);
}

void impaNpc_setTextIndexAndLoadGenericNpcScript_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x6500, 0x6502); E = INTERACTION_BASE + OBJ_TEXT_ID;
  CYC(0x6502, 0x6503); mem_wr(gb, DE, A);
  CYC(0x6503, 0x6505); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x6505, 0x6507); A = 2;
  CYC(0x6507, 0x6508); mem_wr(gb, DE, A);
  CYC(0x6508, 0x650b); SET_HL(0x45f0);
  CYC(0x650b, 0x650e); impaNpc_setScriptAndInitialize_hook(gb);
}

void impaNpc_subid02_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x650e, checkInteractionState_hook, 0x23fe, 0x6511);
  if (!(F & FZ)) { CYCT(0x6511, 0x6513); impaNpc_runScriptAndFaceLink_hook(gb); return; }
  CYC(0x6511, 0x6513);
  CALL_C(0x6513, getImpaNpcState_hook, 0x655a, 0x6516);
  CYC(0x6516, 0x6517); A = B;
  CYC(0x6517, 0x6519); alu_cp(gb, 8);
  if (!(F & FZ)) { CYCT(0x6519, 0x651c); interactionDelete_hook(gb); return; }
  CYC(0x6519, 0x651c);
  CYC(0x651c, 0x651e); A = 0x2f;
  CYC(0x651e, 0x6520); impaNpc_setTextIndexAndLoadGenericNpcScript_hook(gb);
}

void impaNpc_runScriptAndFaceLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6520, interactionRunScript_hook, 0x2552, 0x6523);
  CALL_C(0x6523, impaNpc_faceLinkIfClose_hook, 0x653b, 0x6526);
  CYC(0x6526, 0x6529); interactionAnimateAsNpc_hook(gb);
}

void impaNpc_subid03_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x6529, checkInteractionState_hook, 0x23fe, 0x652c);
  if (!(F & FZ)) { CYCT(0x652c, 0x652e); impaNpc_runScriptAndFaceLink_hook(gb); return; }
  CYC(0x652c, 0x652e);
  CALL_C(0x652e, getImpaNpcState_hook, 0x655a, 0x6531);
  CYC(0x6531, 0x6532); A = B;
  CYC(0x6532, 0x6534); alu_cp(gb, 6);
  if (!(F & FZ)) { CYCT(0x6534, 0x6537); interactionDelete_hook(gb); return; }
  CYC(0x6534, 0x6537);
  CYC(0x6537, 0x6539); A = 0x23;
  CYC(0x6539, 0x653b); impaNpc_setTextIndexAndLoadGenericNpcScript_hook(gb);
}

void impaNpc_faceLinkIfClose_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x653b, 0x653d); C = 0x28;
  CALL_C(0x653d, objectCheckLinkWithinDistance_hook, 0x1fa2, 0x6540);
  if (!(F & FC)) { CYCT(0x6540, 0x6542); goto no_change; }
  CYC(0x6540, 0x6542);
  CALL_C(0x6542, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x6545);
  CYC(0x6545, 0x6547); alu_add(gb, 4);
  CYC(0x6547, 0x6549); alu_and(gb, 0x18);
  CYC(0x6549, 0x654b); A = alu_swap(gb, A);
  CYC(0x654b, 0x654c); alu_rlca(gb);
  CYC(0x654c, 0x654e); goto update;
no_change:
  CYC(0x654e, 0x6550); E = INTERACTION_BASE + OBJ_VAR38;
  CYC(0x6550, 0x6551); A = mem_rd(gb, DE);
update:
  CYC(0x6551, 0x6552); H = D;
  CYC(0x6552, 0x6554); L = INTERACTION_BASE + OBJ_DIRECTION;
  CYC(0x6554, 0x6555); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) { CYCT(0x6555, 0x6556); ret_effect(gb); return; }
  CYC(0x6555, 0x6556);
  CYC(0x6556, 0x6557); mem_wr(gb, HL, A);
  CYC(0x6557, 0x655a); interactionSetAnimation_hook(gb);
}

void getImpaNpcState_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x655a, 0x655c); A = 0x14;
  CALL_C(0x655c, checkGlobalFlag_hook, 0x31f3, 0x655f);
  CYC(0x655f, 0x6561); B = 0xff;
  if (!(F & FZ)) { CYCT(0x6561, 0x6562); ret_effect(gb); return; }
  CYC(0x6561, 0x6562);
  CYC(0x6562, 0x6563); B = alu_inc8(gb, B);
  CYC(0x6563, 0x6566); A = mem_rd(gb, wGroup0RoomFlags + 0x83);
  CYC(0x6566, 0x6567); alu_rlca(gb);
  if (!(F & FC)) { CYCT(0x6567, 0x6568); ret_effect(gb); return; }
  CYC(0x6567, 0x6568);
  CYC(0x6568, 0x656a); A = 0x11;
  CALL_C(0x656a, checkTreasureObtained_hook, 0x1748, 0x656d);
  CYC(0x656d, 0x656f); B = 1;
  if (!(F & FC)) { CYCT(0x656f, 0x6570); ret_effect(gb); return; }
  CYC(0x656f, 0x6570);
  CYC(0x6570, 0x6572); A = 0x11;
  CALL_C(0x6572, checkGlobalFlag_hook, 0x31f3, 0x6575);
  if (!(F & FZ)) { CYCT(0x6575, 0x6577); goto saved_nayru; }
  CYC(0x6575, 0x6577);
  CYC(0x6577, 0x6579); A = 0x38;
  CALL_C(0x6579, checkGlobalFlag_hook, 0x31f3, 0x657c);
  CYC(0x657c, 0x657e); B = 4;
  if (!(F & FZ)) { CYCT(0x657e, 0x657f); ret_effect(gb); return; }
  CYC(0x657e, 0x657f);
  CYC(0x657f, 0x6581); A = 0x40;
  CALL_C(0x6581, checkTreasureObtained_hook, 0x1748, 0x6584);
  CYC(0x6584, 0x6586); alu_bit(gb, 2, A);
  CYC(0x6586, 0x6588); B = 2;
  if (F & FZ) { CYCT(0x6588, 0x6589); ret_effect(gb); return; }
  CYC(0x6588, 0x6589);
  CYC(0x6589, 0x658a); B = alu_inc8(gb, B);
  CYC(0x658a, 0x658b); ret_effect(gb);
  return;
saved_nayru:
  CYC(0x658b, 0x658d); A = 0x36;
  CALL_C(0x658d, checkTreasureObtained_hook, 0x1748, 0x6590);
  CYC(0x6590, 0x6592); B = 5;
  if (!(F & FC)) { CYCT(0x6592, 0x6593); ret_effect(gb); return; }
  CYC(0x6592, 0x6593);
  CYC(0x6593, 0x6595); A = 0x33;
  CALL_C(0x6595, checkGlobalFlag_hook, 0x31f3, 0x6598);
  CYC(0x6598, 0x659a); B = 6;
  if (F & FZ) { CYCT(0x659a, 0x659b); ret_effect(gb); return; }
  CYC(0x659a, 0x659b);
  CYC(0x659b, 0x659d); A = 0x3a;
  CALL_C(0x659d, checkGlobalFlag_hook, 0x31f3, 0x65a0);
  CYC(0x65a0, 0x65a2); B = 7;
  if (F & FZ) { CYCT(0x65a2, 0x65a3); ret_effect(gb); return; }
  CYC(0x65a2, 0x65a3);
  CYC(0x65a3, 0x65a4); B = alu_inc8(gb, B);
  CYC(0x65a4, 0x65a5); ret_effect(gb);
}
