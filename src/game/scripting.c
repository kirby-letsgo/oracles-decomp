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
void scriptFunc_setState_hook(GB *gb);
void scriptFunc_restoreActiveObject_hook(GB *gb);
void scriptFunc_loadBcAndDe_hook(GB *gb);
void scriptFunc_initializeObject_hook(GB *gb);

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

void scriptCmd_setState_hook(GB *gb) {
  CYC(0x4186, 0x4187); SET_HL(pop_effect(gb));
  CYC(0x4187, 0x4188); SET_HL(HL + 1);
  CYC(0x4188, 0x418a); E = 0x44;
  scriptFunc_setState_hook(gb);
}

void scriptFunc_setState_hook(GB *gb) {
  CYC(0x418a, 0x418b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x418b, 0x418d); alu_cp(gb, 0xff);
  if (F & FZ) { CYCT(0x418d, 0x418f); goto increment_state; }
  CYC(0x418d, 0x418f);
  CYC(0x418f, 0x4190); mem_wr(gb, DE, A);
  CYC(0x4190, 0x4191); alu_xor(gb, A);
  CYC(0x4191, 0x4192); ret_effect(gb);
  return;
increment_state:
  CYC(0x4192, 0x4193); A = mem_rd(gb, DE);
  CYC(0x4193, 0x4194); A = alu_inc8(gb, A);
  CYC(0x4194, 0x4195); mem_wr(gb, DE, A);
  CYC(0x4195, 0x4196); alu_xor(gb, A);
  CYC(0x4196, 0x4197); ret_effect(gb);
}

void scriptCmd_setSubstate_hook(GB *gb) {
  CYC(0x4197, 0x4198); SET_HL(pop_effect(gb));
  CYC(0x4198, 0x4199); SET_HL(HL + 1);
  CYC(0x4199, 0x419b); E = 0x45;
  CYC(0x419b, 0x419d); scriptFunc_setState_hook(gb);
}

void scriptCmd_spawnInteraction_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x41ba, 0x41bb); SET_HL(pop_effect(gb));
  CYC(0x41bb, 0x41bc); SET_HL(HL + 1);
  CALL_C(0x41bc, scriptFunc_loadBcAndDe_hook, 0x41cf, 0x41bf);
  CYC(0x41bf, 0x41c0); push_effect(gb, HL);
  CALL_C(0x41c0, getFreeInteractionSlot_hook, 0x3aef, 0x41c3);
  if (!(F & FZ)) { CYCT(0x41c3, 0x41c5); scriptFunc_restoreActiveObject_hook(gb); return; }
  CYC(0x41c3, 0x41c5);
  CYC(0x41c5, 0x41c7); A = 0x4b;
  CALL_C(0x41c7, scriptFunc_initializeObject_hook, 0x41d8, 0x41ca);
  scriptFunc_restoreActiveObject_hook(gb);
}

void scriptFunc_restoreActiveObject_hook(GB *gb) {
  CYC(0x41ca, 0x41cc); A = H8(hActiveObject);
  CYC(0x41cc, 0x41cd); D = A;
  CYC(0x41cd, 0x41ce); SET_HL(pop_effect(gb));
  CYC(0x41ce, 0x41cf); ret_effect(gb);
}

void scriptFunc_loadBcAndDe_hook(GB *gb) {
  CYC(0x41cf, 0x41d0); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x41d0, 0x41d1); B = A;
  CYC(0x41d1, 0x41d2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x41d2, 0x41d3); C = A;
  CYC(0x41d3, 0x41d4); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x41d4, 0x41d5); D = A;
  CYC(0x41d5, 0x41d6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x41d6, 0x41d7); E = A;
  CYC(0x41d7, 0x41d8); ret_effect(gb);
}

void scriptFunc_initializeObject_hook(GB *gb) {
  CYC(0x41d8, 0x41d9); mem_wr(gb, HL, B);
  CYC(0x41d9, 0x41da); L = alu_inc8(gb, L);
  CYC(0x41da, 0x41db); mem_wr(gb, HL, C);
  CYC(0x41db, 0x41dc); L = alu_inc8(gb, L);
  CYC(0x41dc, 0x41dd); L = A;
  CYC(0x41dd, 0x41de); mem_wr(gb, HL, D);
  CYC(0x41de, 0x41df); L = alu_inc8(gb, L);
  CYC(0x41df, 0x41e0); L = alu_inc8(gb, L);
  CYC(0x41e0, 0x41e1); mem_wr(gb, HL, E);
  CYC(0x41e1, 0x41e2); ret_effect(gb);
}

void scriptCmd_spawnEnemy_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x41e2, 0x41e3); SET_HL(pop_effect(gb));
  CYC(0x41e3, 0x41e4); SET_HL(HL + 1);
  CALL_C(0x41e4, scriptFunc_loadBcAndDe_hook, 0x41cf, 0x41e7);
  CYC(0x41e7, 0x41e8); push_effect(gb, HL);
  CALL_C(0x41e8, getFreeEnemySlot_hook, 0x2e27, 0x41eb);
  if (!(F & FZ)) { CYCT(0x41eb, 0x41ed); scriptFunc_restoreActiveObject_hook(gb); return; }
  CYC(0x41eb, 0x41ed);
  CYC(0x41ed, 0x41ef); A = 0x8b;
  CALL_C(0x41ef, scriptFunc_initializeObject_hook, 0x41d8, 0x41f2);
  CYC(0x41f2, 0x41f4); scriptFunc_restoreActiveObject_hook(gb);
}

void scriptCmd_setCoords_hook(GB *gb) {
  CYC(0x421b, 0x421c); SET_HL(pop_effect(gb));
  CYC(0x421c, 0x421d); SET_HL(HL + 1);
  CYC(0x421d, 0x421e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x421e, 0x421f); B = A;
  CYC(0x421f, 0x4220); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4220, 0x4221); C = A;
  CYC(0x4221, 0x4222); push_effect(gb, HL);
  CYC(0x4222, 0x4223); H = D;
  CYC(0x4223, 0x4225); L = 0x4b;
  CYC(0x4225, 0x4226); mem_wr(gb, HL, B);
  CYC(0x4226, 0x4228); L = 0x4d;
  CYC(0x4228, 0x4229); mem_wr(gb, HL, C);
  CYC(0x4229, 0x422a); SET_HL(pop_effect(gb));
  CYC(0x422a, 0x422b); ret_effect(gb);
}

void scriptCmd_setAngle_hook(GB *gb) {
  CYC(0x422b, 0x422c); SET_HL(pop_effect(gb));
  CYC(0x422c, 0x422d); SET_HL(HL + 1);
  CYC(0x422d, 0x422e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x422e, 0x4230); E = 0x49;
  CYC(0x4230, 0x4231); mem_wr(gb, DE, A);
  CYC(0x4231, 0x4232); ret_effect(gb);
}

void scriptCmd_setSpeed_hook(GB *gb) {
  CYC(0x4232, 0x4233); SET_HL(pop_effect(gb));
  CYC(0x4233, 0x4234); SET_HL(HL + 1);
  CYC(0x4234, 0x4235); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4235, 0x4237); E = 0x50;
  CYC(0x4237, 0x4238); mem_wr(gb, DE, A);
  CYC(0x4238, 0x4239); ret_effect(gb);
}

void scriptCmd_setZSpeed_hook(GB *gb) {
  CYC(0x4239, 0x423a); SET_HL(pop_effect(gb));
  CYC(0x423a, 0x423b); SET_HL(HL + 1);
  CYC(0x423b, 0x423d); E = 0x54;
  CYC(0x423d, 0x423e); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x423e, 0x423f); mem_wr(gb, DE, A);
  CYC(0x423f, 0x4240); E = alu_inc8(gb, E);
  CYC(0x4240, 0x4241); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4241, 0x4242); mem_wr(gb, DE, A);
  CYC(0x4242, 0x4243); alu_scf(gb);
  CYC(0x4243, 0x4244); ret_effect(gb);
}
