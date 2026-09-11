#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0c, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0c, (from), (to), true)

void scriptFunc_checkRoomFlag_hook(GB *gb);
void scriptFunc_popHlAndInc_hook(GB *gb);
void scriptCmd_disableMenu_hook(GB *gb);
void scriptCmd_enableMenu_hook(GB *gb);
void scriptFunc_setLinkCantMove_hook(GB *gb);
void func_0c_4177_hook(GB *gb);

// 0c:4103
void scriptCmd_none_hook(GB *gb) {
  CYC(0x4103, 0x4104); SET_HL(pop_effect(gb));
  CYC(0x4104, 0x4105); ret_effect(gb);
}

// 0c:4105
void scriptCmd_stopIfItemFlagSet_hook(GB *gb) {
  CYC(0x4105, 0x4107); B = 0x20;
  CYC(0x4107, 0x4109);
  scriptFunc_checkRoomFlag_hook(gb);
}

// 0c:4109
void scriptCmd_stopIfRoomFlag40Set_hook(GB *gb) {
  CYC(0x4109, 0x410b); B = 0x40;
  CYC(0x410b, 0x410d);
  scriptFunc_checkRoomFlag_hook(gb);
}

// 0c:410d
void scriptCmd_stopIfRoomFlag80Set_hook(GB *gb) {
  CYC(0x410d, 0x410f); B = 0x80;
  scriptFunc_checkRoomFlag_hook(gb);
}

// 0c:410f
void scriptFunc_checkRoomFlag_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x410f, getThisRoomFlags_hook, 0x197d, 0x4112);
  CYC(0x4112, 0x4113); alu_and(gb, B);
  if (F & FZ) {
    CYCT(0x4113, 0x4116);
    scriptFunc_popHlAndInc_hook(gb);
    return;
  }
  CYC(0x4113, 0x4116);
  CYC(0x4116, 0x4117); SET_HL(pop_effect(gb));
  CYC(0x4117, 0x411a); SET_HL(0x45ef);
  CYC(0x411a, 0x411b); alu_scf(gb);
  CYC(0x411b, 0x411c); ret_effect(gb);
}

void scriptCmd_disableInput_hook(GB *gb) {
  CYC(0x4147, 0x4149); A = 0x81;
  CYC(0x4149, 0x414c); W8(wDisabledObjects) = A;
  scriptCmd_disableMenu_hook(gb);
}

void scriptCmd_disableMenu_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x414c, 0x414e); A = 0x80;
  CYC(0x414e, 0x4151); W8(wMenuDisabled) = A;
  CALL_C(0x4151, clearAllParentItems_hook, 0x2c10, 0x4154);
  CALL_C(0x4154, dropLinkHeldItem_hook, 0x2c43, 0x4157);
  CALL_C(0x4157, func_0c_4177_hook, 0x4177, 0x415a);
  scriptFunc_popHlAndInc_hook(gb);
}

void scriptFunc_popHlAndInc_hook(GB *gb) {
  CYC(0x415a, 0x415b); SET_HL(pop_effect(gb));
  CYC(0x415b, 0x415c); SET_HL(HL + 1);
  CYC(0x415c, 0x415d); alu_scf(gb);
  CYC(0x415d, 0x415e); ret_effect(gb);
}

void scriptCmd_enableInput_hook(GB *gb) {
  CYC(0x415e, 0x415f); alu_xor(gb, A);
  CYC(0x415f, 0x4162); W8(wDisabledObjects) = A;
  scriptCmd_enableMenu_hook(gb);
}

void scriptCmd_enableMenu_hook(GB *gb) {
  CYC(0x4162, 0x4163); alu_xor(gb, A);
  CYC(0x4163, 0x4166); W8(wMenuDisabled) = A;
  CYC(0x4166, 0x4168); scriptFunc_popHlAndInc_hook(gb);
}

void scriptCmd_setLinkCantMoveTo91_hook(GB *gb) {
  CYC(0x4168, 0x416a); A = 0x91;
  scriptFunc_setLinkCantMove_hook(gb);
}

void scriptFunc_setLinkCantMove_hook(GB *gb) {
  CYC(0x416a, 0x416d); W8(wDisabledObjects) = A;
  CYC(0x416d, 0x416e); SET_HL(pop_effect(gb));
  CYC(0x416e, 0x416f); SET_HL(HL + 1);
  CYC(0x416f, 0x4170); ret_effect(gb);
}

void scriptCmd_setLinkCantMoveTo00_hook(GB *gb) {
  CYC(0x4170, 0x4171); alu_xor(gb, A);
  CYC(0x4171, 0x4173); scriptFunc_setLinkCantMove_hook(gb);
}

void scriptCmd_setLinkCantMoveTo11_hook(GB *gb) {
  CYC(0x4173, 0x4175); A = 0x11;
  CYC(0x4175, 0x4177); scriptFunc_setLinkCantMove_hook(gb);
}

void func_0c_4177_hook(GB *gb) {
  CYC(0x4177, 0x4178); push_effect(gb, HL);
  CYC(0x4178, 0x417b); A = W8(wLinkObjectIndex);
  CYC(0x417b, 0x417c); H = A;
  CYC(0x417c, 0x417e); L = 0x2b;
  CYC(0x417e, 0x4180); mem_wr(gb, HL, 0x80);
  CYC(0x4180, 0x4182); L = 0x2d;
  CYC(0x4182, 0x4184); mem_wr(gb, HL, 0x00);
  CYC(0x4184, 0x4185); SET_HL(pop_effect(gb));
  CYC(0x4185, 0x4186); ret_effect(gb);
}
