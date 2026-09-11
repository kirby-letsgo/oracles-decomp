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
void scriptFunc_4310_hook(GB *gb);
void scriptFunc_getTextIndex_hook(GB *gb);

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

void scriptCmd_checkCounter2ZeroAndReset_hook(GB *gb) {
  CYC(0x4244, 0x4245); SET_HL(pop_effect(gb));
  CYC(0x4245, 0x4247); E = 0x47;
  CYC(0x4247, 0x4248); A = mem_rd(gb, DE);
  CYC(0x4248, 0x4249); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(0x4249, 0x424a); ret_effect(gb); return; }
  CYC(0x4249, 0x424a);
  CYC(0x424a, 0x424b); SET_HL(HL + 1);
  CYC(0x424b, 0x424c); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x424c, 0x424d); mem_wr(gb, DE, A);
  CYC(0x424d, 0x424e); ret_effect(gb);
}

void scriptCmd_setCollideRadii_hook(GB *gb) {
  CYC(0x424e, 0x424f); SET_HL(pop_effect(gb));
  CYC(0x424f, 0x4250); SET_HL(HL + 1);
  CYC(0x4250, 0x4251); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4251, 0x4253); E = 0x66;
  CYC(0x4253, 0x4254); mem_wr(gb, DE, A);
  CYC(0x4254, 0x4255); E = alu_inc8(gb, E);
  CYC(0x4255, 0x4256); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4256, 0x4257); mem_wr(gb, DE, A);
  CYC(0x4257, 0x4258); ret_effect(gb);
}

void scriptCmd_writeInteractionByte_hook(GB *gb) {
  CYC(0x4258, 0x4259); SET_HL(pop_effect(gb));
  CYC(0x4259, 0x425a); SET_HL(HL + 1);
  CYC(0x425a, 0x425b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x425b, 0x425c); E = A;
  CYC(0x425c, 0x425d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x425d, 0x425e); mem_wr(gb, DE, A);
  CYC(0x425e, 0x425f); ret_effect(gb);
}

void scriptCmd_addinteractionByte_hook(GB *gb) {
  CYC(0x425f, 0x4260); SET_HL(pop_effect(gb));
  CYC(0x4260, 0x4261); SET_HL(HL + 1);
  CYC(0x4261, 0x4262); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4262, 0x4263); E = A;
  CYC(0x4263, 0x4264); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4264, 0x4265); B = A;
  CYC(0x4265, 0x4266); A = mem_rd(gb, DE);
  CYC(0x4266, 0x4267); alu_add(gb, B);
  CYC(0x4267, 0x4268); mem_wr(gb, DE, A);
  CYC(0x4268, 0x4269); alu_scf(gb);
  CYC(0x4269, 0x426a); ret_effect(gb);
}

void scriptCmd_getRandomBits_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x426a, 0x426b); SET_HL(pop_effect(gb));
  CYC(0x426b, 0x426c); SET_HL(HL + 1);
  CALL_C(0x426c, getRandomNumber_hook, 0x043e, 0x426f);
  CYC(0x426f, 0x4270); B = A;
  CYC(0x4270, 0x4271); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4271, 0x4272); E = A;
  CYC(0x4272, 0x4273); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4273, 0x4274); alu_and(gb, B);
  CYC(0x4274, 0x4275); mem_wr(gb, DE, A);
  CYC(0x4275, 0x4276); ret_effect(gb);
}

void scriptCmd_turnToFaceLink_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4298, objectGetAngleTowardEnemyTarget_hook, 0x1e94, 0x429b);
  CYC(0x429b, 0x429d); alu_add(gb, 0x04);
  CYC(0x429d, 0x429f); alu_and(gb, 0x18);
  CYC(0x429f, 0x42a1); A = alu_swap(gb, A);
  CYC(0x42a1, 0x42a2); alu_rlca(gb);
  CALL_C(0x42a2, interactionSetAnimation_hook, 0x262e, 0x42a5);
  CYC(0x42a5, 0x42a8); scriptFunc_popHlAndInc_hook(gb);
}

void scriptCmd_setAngleAndExtra_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42a8, 0x42a9); SET_HL(pop_effect(gb));
  CYC(0x42a9, 0x42aa); SET_HL(HL + 1);
  CYC(0x42aa, 0x42ab); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x42ab, 0x42ad); E = 0x49;
  CYC(0x42ad, 0x42ae); mem_wr(gb, DE, A);
  CALL_C(0x42ae, convertAngleDeToDirection_hook, 0x26f8, 0x42b1);
  CYC(0x42b1, 0x42b2); push_effect(gb, HL);
  CALL_C(0x42b2, interactionSetAnimation_hook, 0x262e, 0x42b5);
  CYC(0x42b5, 0x42b6); SET_HL(pop_effect(gb));
  CYC(0x42b6, 0x42b7); alu_scf(gb);
  CYC(0x42b7, 0x42b8); ret_effect(gb);
}

void scriptCmd_runGenericNpc_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42b8, 0x42b9); SET_HL(pop_effect(gb));
  CYC(0x42b9, 0x42ba); SET_HL(HL + 1);
  CALL_C(0x42ba, scriptFunc_getTextIndex_hook, 0x42c8, 0x42bd);
  CYC(0x42bd, 0x42be); A = C;
  CYC(0x42be, 0x42c0); E = 0x72;
  CYC(0x42c0, 0x42c1); mem_wr(gb, DE, A);
  CYC(0x42c1, 0x42c2); A = B;
  CYC(0x42c2, 0x42c3); E = alu_inc8(gb, E);
  CYC(0x42c3, 0x42c4); mem_wr(gb, DE, A);
  CYC(0x42c4, 0x42c7); SET_HL(0x45f0);
  CYC(0x42c7, 0x42c8); ret_effect(gb);
}

void scriptFunc_getTextIndex_hook(GB *gb) {
  CYC(0x42c8, 0x42ca); E = 0x70;
  CYC(0x42ca, 0x42cb); A = mem_rd(gb, DE);
  CYC(0x42cb, 0x42cc); alu_or(gb, A);
  if (F & FZ) { CYCT(0x42cc, 0x42ce); goto use_script_text_index; }
  CYC(0x42cc, 0x42ce);
  CYC(0x42ce, 0x42d0); E = 0x73;
  CYC(0x42d0, 0x42d1); A = mem_rd(gb, DE);
  CYC(0x42d1, 0x42d2); B = A;
  CYC(0x42d2, 0x42d4); goto load_text_index;
use_script_text_index:
  CYC(0x42d4, 0x42d5); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x42d5, 0x42d6); B = A;
load_text_index:
  CYC(0x42d6, 0x42d7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x42d7, 0x42d8); C = A;
  CYC(0x42d8, 0x42d9); ret_effect(gb);
}

void scriptCmd_showText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42d9, 0x42da); SET_HL(pop_effect(gb));
  CYC(0x42da, 0x42db); SET_HL(HL + 1);
  CALL_C(0x42db, scriptFunc_getTextIndex_hook, 0x42c8, 0x42de);
  CYC(0x42de, 0x42df); push_effect(gb, HL);
  CALL_C(0x42df, showText_hook, 0x1872, 0x42e2);
  CYC(0x42e2, 0x42e3); SET_HL(pop_effect(gb));
  CYC(0x42e3, 0x42e4); ret_effect(gb);
}

void scriptCmd_showTextDifferentForLinked_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42e4, 0x42e5); SET_HL(pop_effect(gb));
  CYC(0x42e5, 0x42e6); SET_HL(HL + 1);
  CYC(0x42e6, 0x42e7); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x42e7, 0x42e8); B = A;
  CALL_C(0x42e8, checkIsLinkedGame_hook, 0x1992, 0x42eb);
  if (!(F & FZ)) {
    CYCT(0x42eb, 0x42ed);
    goto linked;
  }
  CYC(0x42eb, 0x42ed);
  CYC(0x42ed, 0x42ee); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x42ee, 0x42ef); SET_HL(HL + 1);
  CYC(0x42ef, 0x42f1); goto show_text;
linked:
  CYC(0x42f1, 0x42f2); SET_HL(HL + 1);
  CYC(0x42f2, 0x42f3); A = mem_rd(gb, HL); SET_HL(HL + 1);
show_text:
  CYC(0x42f3, 0x42f4); C = A;
  CYC(0x42f4, 0x42f5); push_effect(gb, HL);
  CALL_C(0x42f5, showText_hook, 0x1872, 0x42f8);
  CYC(0x42f8, 0x42f9); SET_HL(pop_effect(gb));
  CYC(0x42f9, 0x42fa); ret_effect(gb);
}

void scriptCmd_showTextNonExitable_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x42fa, 0x42fb); SET_HL(pop_effect(gb));
  CYC(0x42fb, 0x42fc); SET_HL(HL + 1);
  CALL_C(0x42fc, scriptFunc_getTextIndex_hook, 0x42c8, 0x42ff);
  CYC(0x42ff, 0x4300); push_effect(gb, HL);
  CALL_C(0x4300, showTextNonExitable_hook, 0x186e, 0x4303);
  CYC(0x4303, 0x4304); SET_HL(pop_effect(gb));
  CYC(0x4304, 0x4305); ret_effect(gb);
}

void scriptCmd_waitForText_hook(GB *gb) {
  CYC(0x4305, 0x4306); SET_HL(pop_effect(gb));
  CYC(0x4306, 0x4309); A = W8(wTextIsActive);
  CYC(0x4309, 0x430a); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x430a, 0x430b); ret_effect(gb);
    return;
  }
  CYC(0x430a, 0x430b);
  CYC(0x430b, 0x430c); SET_HL(HL + 1);
  CYC(0x430c, 0x430d); ret_effect(gb);
}

void scriptCmd_setCounter1_hook(GB *gb) {
  CYC(0x430d, 0x430e); SET_HL(pop_effect(gb));
  CYC(0x430e, 0x430f); SET_HL(HL + 1);
  CYC(0x430f, 0x4310); A = mem_rd(gb, HL); SET_HL(HL + 1);
  scriptFunc_4310_hook(gb);
}

void scriptFunc_4310_hook(GB *gb) {
  CYC(0x4310, 0x4312); E = 0x46;
  CYC(0x4312, 0x4313); mem_wr(gb, DE, A);
  CYC(0x4313, 0x4314); alu_xor(gb, A);
  CYC(0x4314, 0x4315); ret_effect(gb);
}

void scriptCmd_cpLinkX_hook(GB *gb) {
  CYC(0x4315, 0x4316); SET_HL(pop_effect(gb));
  CYC(0x4316, 0x4317); SET_HL(HL + 1);
  CYC(0x4317, 0x4318); push_effect(gb, HL);
  CYC(0x4318, 0x431a); E = 0x4d;
  CYC(0x431a, 0x431b); A = mem_rd(gb, DE);
  CYC(0x431b, 0x431e); SET_HL(w1Link_xh);
  CYC(0x431e, 0x431f); alu_cp(gb, mem_rd(gb, HL));
  CYC(0x431f, 0x4320); SET_HL(pop_effect(gb));
  CYC(0x4320, 0x4321); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4321, 0x4322); E = A;
  CYC(0x4322, 0x4324); A = 0x00;
  if (!(F & FC)) {
    CYCT(0x4324, 0x4326);
    goto store_result;
  }
  CYC(0x4324, 0x4326);
  CYC(0x4326, 0x4327); A = alu_inc8(gb, A);
store_result:
  CYC(0x4327, 0x4328); mem_wr(gb, DE, A);
  CYC(0x4328, 0x4329); alu_scf(gb);
  CYC(0x4329, 0x432a); ret_effect(gb);
}

void scriptCmd_shakeScreen_hook(GB *gb) {
  CYC(0x432a, 0x432b); SET_HL(pop_effect(gb));
  CYC(0x432b, 0x432c); SET_HL(HL + 1);
  CYC(0x432c, 0x432d); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x432d, 0x4330); W8(wScreenShakeCounterX) = A;
  CYC(0x4330, 0x4331); ret_effect(gb);
}

void scriptCmd_writeMemory_hook(GB *gb) {
  CYC(0x4331, 0x4332); SET_HL(pop_effect(gb));
  CYC(0x4332, 0x4333); SET_HL(HL + 1);
  CYC(0x4333, 0x4334); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4334, 0x4335); C = A;
  CYC(0x4335, 0x4336); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4336, 0x4337); B = A;
  CYC(0x4337, 0x4338); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4338, 0x4339); mem_wr(gb, BC, A);
  CYC(0x4339, 0x433a); alu_scf(gb);
  CYC(0x433a, 0x433b); ret_effect(gb);
}

void scriptCmd_checkPaletteFadeDone_hook(GB *gb) {
  CYC(0x433b, 0x433c); SET_HL(pop_effect(gb));
  CYC(0x433c, 0x433f); A = W8(wPaletteThread_mode);
  CYC(0x433f, 0x4340); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4340, 0x4341); ret_effect(gb);
    return;
  }
  CYC(0x4340, 0x4341);
  CYC(0x4341, 0x4342); SET_HL(HL + 1);
  CYC(0x4342, 0x4343); ret_effect(gb);
}

void scriptCmd_checkCFC0Bit_hook(GB *gb) {
  CYC(0x4343, 0x4344); SET_HL(pop_effect(gb));
  CYC(0x4344, 0x4345); A = mem_rd(gb, HL);
  CYC(0x4345, 0x4347); alu_and(gb, 0x07);
  CYC(0x4347, 0x434a); SET_BC(0x00f8);
  CYC(0x434a, 0x434b); alu_add(gb, C);
  CYC(0x434b, 0x434c); C = A;
  CYC(0x434c, 0x434d); A = mem_rd(gb, BC);
  CYC(0x434d, 0x434e); B = A;
  CYC(0x434e, 0x4351); A = mem_rd(gb, 0xcfc0);
  CYC(0x4351, 0x4352); alu_and(gb, B);
  if (F & FZ) {
    CYCT(0x4352, 0x4353); ret_effect(gb);
    return;
  }
  CYC(0x4352, 0x4353);
  CYC(0x4353, 0x4354); SET_HL(HL + 1);
  CYC(0x4354, 0x4355); ret_effect(gb);
}

void scriptCmd_xorCFC0Bit_hook(GB *gb) {
  CYC(0x4355, 0x4356); SET_HL(pop_effect(gb));
  CYC(0x4356, 0x4357); A = mem_rd(gb, HL);
  CYC(0x4357, 0x4359); alu_and(gb, 0x07);
  CYC(0x4359, 0x435c); SET_BC(0x00f8);
  CYC(0x435c, 0x435d); alu_add(gb, C);
  CYC(0x435d, 0x435e); C = A;
  CYC(0x435e, 0x435f); A = mem_rd(gb, BC);
  CYC(0x435f, 0x4360); B = A;
  CYC(0x4360, 0x4363); A = mem_rd(gb, 0xcfc0);
  CYC(0x4363, 0x4364); alu_xor(gb, B);
  CYC(0x4364, 0x4367); mem_wr(gb, 0xcfc0, A);
  CYC(0x4367, 0x4368); SET_HL(HL + 1);
  CYC(0x4368, 0x4369); ret_effect(gb);
}

void scriptCmd_jumpIfNoEnemies_hook(GB *gb) {
  CYC(0x4369, 0x436a); SET_HL(pop_effect(gb));
  CYC(0x436a, 0x436d); A = W8(wNumEnemies);
  CYC(0x436d, 0x436e); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x436e, 0x4371);
    scriptFunc_add3ToHl_hook(gb);
    return;
  }
  CYC(0x436e, 0x4371);
  CYC(0x4371, 0x4372); SET_HL(HL + 1);
  CYC(0x4372, 0x4375); scriptFunc_jump_hook(gb);
}

void scriptCmd_jumpIfC6xxSet_hook(GB *gb) {
  CYC(0x4375, 0x4376); SET_HL(pop_effect(gb));
  CYC(0x4376, 0x4377); SET_HL(HL + 1);
  CYC(0x4377, 0x4379); B = 0xc6;
  CYC(0x4379, 0x437a); C = mem_rd(gb, HL);
  CYC(0x437a, 0x437b); SET_HL(HL + 1);
  CYC(0x437b, 0x437c); A = mem_rd(gb, BC);
  CYC(0x437c, 0x437d); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x437d, 0x4380);
    scriptFunc_add3ToHl_hook(gb);
    return;
  }
  CYC(0x437d, 0x4380);
  CYC(0x4380, 0x4381); SET_HL(HL + 1);
  CYC(0x4381, 0x4384); scriptFunc_jump_hook(gb);
}

void scriptCmd_playSound_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4384, 0x4385); SET_HL(pop_effect(gb));
  CYC(0x4385, 0x4386); SET_HL(HL + 1);
  CYC(0x4386, 0x4387); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4387, 0x4388); push_effect(gb, HL);
  CALL_C(0x4388, playSound_b00_hook, 0x0c98, 0x438b);
  CYC(0x438b, 0x438c); SET_HL(pop_effect(gb));
  CYC(0x438c, 0x438d); ret_effect(gb);
}

void scriptCmd_updateLinkLocalRespawnPosition_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x438d, updateLinkLocalRespawnPosition_hook, 0x113a, 0x4390);
  CYC(0x4390, 0x4391); SET_HL(pop_effect(gb));
  CYC(0x4391, 0x4392); SET_HL(HL + 1);
  CYC(0x4392, 0x4393); ret_effect(gb);
}

void scriptCmd_jumpIfLinkVariableNe_hook(GB *gb) {
  CYC(0x4393, 0x4394); SET_HL(pop_effect(gb));
  CYC(0x4394, 0x4395); SET_HL(HL + 1);
  CYC(0x4395, 0x4396); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4396, 0x4398); D = 0xd0;
  CYC(0x4398, 0x4399); E = A;
  CYC(0x4399, 0x439a); A = mem_rd(gb, DE);
  CYC(0x439a, 0x439b); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x439b, 0x439d);
    CYC(0x43a1, 0x43a4); SET_BC(0x0003);
    CYC(0x43a4, 0x43a5); alu_add_hl(gb, BC);
    CYC(0x43a5, 0x43a7); A = H8(hActiveObject);
    CYC(0x43a7, 0x43a8); D = A;
    CYC(0x43a8, 0x43a9); ret_effect(gb);
    return;
  }
  CYC(0x439b, 0x439d);
  CYC(0x439d, 0x439e); SET_HL(HL + 1);
  CYC(0x439e, 0x43a1); scriptFunc_jump_hook(gb);
}

static void scriptCmd_compareMemoryThenJump(GB *gb) {
  CYC(0x43b0, 0x43b1); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x43b1, 0x43b4);
    scriptFunc_add3ToHl_scf_hook(gb);
    return;
  }
  CYC(0x43b1, 0x43b4);
  CYC(0x43b4, 0x43b5); SET_HL(HL + 1);
  CYC(0x43b5, 0x43b8); scriptFunc_jump_scf_hook(gb);
}

void scriptCmd_jumpIfMemoryEq_hook(GB *gb) {
  CYC(0x43a9, 0x43aa); SET_HL(pop_effect(gb));
  CYC(0x43aa, 0x43ab); SET_HL(HL + 1);
  CYC(0x43ab, 0x43ac); C = mem_rd(gb, HL);
  CYC(0x43ac, 0x43ad); SET_HL(HL + 1);
  CYC(0x43ad, 0x43ae); B = mem_rd(gb, HL);
  CYC(0x43ae, 0x43af); SET_HL(HL + 1);
  CYC(0x43af, 0x43b0); A = mem_rd(gb, BC);
  scriptCmd_compareMemoryThenJump(gb);
}

void scriptCmd_jumpIfInteractionByteEq_hook(GB *gb) {
  CYC(0x43b8, 0x43b9); SET_HL(pop_effect(gb));
  CYC(0x43b9, 0x43ba); SET_HL(HL + 1);
  CYC(0x43ba, 0x43bb); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43bb, 0x43bc); E = A;
  CYC(0x43bc, 0x43bd); A = mem_rd(gb, DE);
  CYC(0x43bd, 0x43bf);
  scriptCmd_compareMemoryThenJump(gb);
}

void scriptCmd_jumpIfRoomFlagSet_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x43bf, 0x43c0); SET_HL(pop_effect(gb));
  CYC(0x43c0, 0x43c1); SET_HL(HL + 1);
  CYC(0x43c1, 0x43c2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43c2, 0x43c3); B = A;
  CYC(0x43c3, 0x43c4); push_effect(gb, HL);
  CALL_C(0x43c4, getThisRoomFlags_hook, 0x197d, 0x43c7);
  CYC(0x43c7, 0x43c8); alu_and(gb, B);
  if (!(F & FZ)) {
    CYCT(0x43c8, 0x43ca);
    CYC(0x43cf, 0x43d0); SET_HL(pop_effect(gb));
    CYC(0x43d0, 0x43d3); scriptFunc_jump_scf_hook(gb);
    return;
  }
  CYC(0x43c8, 0x43ca);
  CYC(0x43ca, 0x43cb); SET_HL(pop_effect(gb));
  CYC(0x43cb, 0x43cc); SET_HL(HL + 1);
  CYC(0x43cc, 0x43cd); SET_HL(HL + 1);
  CYC(0x43cd, 0x43ce); alu_scf(gb);
  CYC(0x43ce, 0x43cf); ret_effect(gb);
}

void scriptCmd_orRoomFlags_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x43d3, 0x43d4); SET_HL(pop_effect(gb));
  CYC(0x43d4, 0x43d5); SET_HL(HL + 1);
  CYC(0x43d5, 0x43d6); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43d6, 0x43d7); B = A;
  CYC(0x43d7, 0x43d8); push_effect(gb, HL);
  CALL_C(0x43d8, getThisRoomFlags_hook, 0x197d, 0x43db);
  CYC(0x43db, 0x43dc); alu_or(gb, B);
  CYC(0x43dc, 0x43dd); mem_wr(gb, HL, A);
  CYC(0x43dd, 0x43de); SET_HL(pop_effect(gb));
  CYC(0x43de, 0x43df); ret_effect(gb);
}

void scriptCmd_checkSomething_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x43df, 0x43e1); E = 0x71;
  CALL_C(0x43e1, objectAddToAButtonSensitiveObjectList_hook, 0x1b2c, 0x43e4);
  CYC(0x43e4, 0x43e5); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(0x43e5, 0x43e7);
    CYC(0x43e8, 0x43e9); ret_effect(gb);
    return;
  }
  CYC(0x43e5, 0x43e7);
  CYC(0x43e7, 0x43e8); SET_HL(HL + 1);
  CYC(0x43e8, 0x43e9); ret_effect(gb);
}

void scriptCmd_showLoadedText_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x43e9, 0x43eb); E = 0x72;
  CYC(0x43eb, 0x43ec); A = mem_rd(gb, DE);
  CYC(0x43ec, 0x43ed); C = A;
  CYC(0x43ed, 0x43ee); E = alu_inc8(gb, E);
  CYC(0x43ee, 0x43ef); A = mem_rd(gb, DE);
  CYC(0x43ef, 0x43f0); B = A;
  CALL_C(0x43f0, showText_hook, 0x1872, 0x43f3);
  CYC(0x43f3, 0x43f4); SET_HL(pop_effect(gb));
  CYC(0x43f4, 0x43f5); SET_HL(HL + 1);
  CYC(0x43f5, 0x43f6); ret_effect(gb);
}

void scriptCmd_setTextID_hook(GB *gb) {
  CYC(0x43f6, 0x43f7); SET_HL(pop_effect(gb));
  CYC(0x43f7, 0x43f8); SET_HL(HL + 1);
  CYC(0x43f8, 0x43f9); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43f9, 0x43fb); E = 0x72;
  CYC(0x43fb, 0x43fc); mem_wr(gb, DE, A);
  CYC(0x43fc, 0x43fd); E = alu_inc8(gb, E);
  CYC(0x43fd, 0x43fe); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x43fe, 0x43ff); mem_wr(gb, DE, A);
  CYC(0x43ff, 0x4400); alu_scf(gb);
  CYC(0x4400, 0x4401); ret_effect(gb);
}

void scriptCmd_setMusic_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4401, 0x4402); SET_HL(pop_effect(gb));
  CYC(0x4402, 0x4403); SET_HL(HL + 1);
  CYC(0x4403, 0x4404); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4404, 0x4406); alu_cp(gb, 0xff);
  if (!(F & FZ)) {
    CYCT(0x4406, 0x4408);
    goto set_music;
  }
  CYC(0x4406, 0x4408);
  CYC(0x4408, 0x440b); A = W8(wActiveMusic2);
set_music:
  CYC(0x440b, 0x440e); W8(wActiveMusic) = A;
  CYC(0x440e, 0x440f); push_effect(gb, HL);
  CALL_C(0x440f, playSound_b00_hook, 0x0c98, 0x4412);
  CYC(0x4412, 0x4413); SET_HL(pop_effect(gb));
  CYC(0x4413, 0x4414); ret_effect(gb);
}

void scriptCmd_orMemory_hook(GB *gb) {
  CYC(0x4414, 0x4415); SET_HL(pop_effect(gb));
  CYC(0x4415, 0x4416); SET_HL(HL + 1);
  CYC(0x4416, 0x4417); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4417, 0x4418); C = A;
  CYC(0x4418, 0x4419); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4419, 0x441a); B = A;
  CYC(0x441a, 0x441b); A = mem_rd(gb, BC);
  CYC(0x441b, 0x441c); alu_or(gb, mem_rd(gb, HL));
  CYC(0x441c, 0x441d); mem_wr(gb, BC, A);
  CYC(0x441d, 0x441e); SET_HL(HL + 1);
  CYC(0x441e, 0x441f); alu_scf(gb);
  CYC(0x441f, 0x4420); ret_effect(gb);
}

void scriptCmd_spawnItem_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4420, 0x4421); SET_HL(pop_effect(gb));
  CYC(0x4421, 0x4422); E = mem_rd(gb, HL);
  CYC(0x4422, 0x4423); SET_HL(HL + 1);
  CYC(0x4423, 0x4424); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4424, 0x4425); B = A;
  CYC(0x4425, 0x4426); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4426, 0x4427); C = A;
  CYC(0x4427, 0x4428); push_effect(gb, HL);
  CALL_C(0x4428, getFreeInteractionSlot_hook, 0x3aef, 0x442b);
  if (!(F & FZ)) {
    CYCT(0x442b, 0x442e);
    scriptFunc_restoreActiveObject_hook(gb);
    return;
  }
  CYC(0x442b, 0x442e);
  CYC(0x442e, 0x4430); mem_wr(gb, HL, 0x60);
  CYC(0x4430, 0x4431); L = alu_inc8(gb, L);
  CYC(0x4431, 0x4432); mem_wr(gb, HL, B);
  CYC(0x4432, 0x4433); L = alu_inc8(gb, L);
  CYC(0x4433, 0x4434); mem_wr(gb, HL, C);
  CYC(0x4434, 0x4435); A = E;
  CYC(0x4435, 0x4437); alu_cp(gb, 0xde);
  if (F & FZ) {
    CYCT(0x4437, 0x4439);
    goto create_link_item;
  }
  CYC(0x4437, 0x4439);
  CALL_C(0x4439, objectCopyPosition_hook, 0x2242, 0x443c);
  CYC(0x443c, 0x443f); scriptFunc_restoreActiveObject_hook(gb);
  return;
create_link_item:
  CYC(0x443f, 0x4441); E = 0x46;
  CYC(0x4441, 0x4443); A = 0x03;
  CYC(0x4443, 0x4444); mem_wr(gb, DE, A);
  CYC(0x4444, 0x4447); SET_DE(w1Link_yh);
  CALL_C(0x4447, objectCopyPosition_rawAddress_hook, 0x2247, 0x444a);
  CYC(0x444a, 0x444d); scriptFunc_restoreActiveObject_hook(gb);
}

void scriptCmd_df_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x444d, 0x444e); SET_HL(pop_effect(gb));
  CYC(0x444e, 0x444f); SET_HL(HL + 1);
  CYC(0x444f, 0x4450); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CALL_C(0x4450, checkTreasureObtained_hook, 0x1748, 0x4453);
  CYC(0x4453, 0x4456); mem_wr(gb, 0xcfc1, A);
  if (!(F & FC)) {
    CYCT(0x4456, 0x4458);
    goto skip_jump;
  }
  CYC(0x4456, 0x4458);
  CYC(0x4458, 0x445b); scriptFunc_jump_hook(gb);
  return;
skip_jump:
  CYC(0x445b, 0x445c); SET_HL(HL + 1);
  CYC(0x445c, 0x445d); SET_HL(HL + 1);
  CYC(0x445d, 0x445e); ret_effect(gb);
}

void scriptCmd_jumpIfSomething_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x445e, 0x445f); SET_HL(pop_effect(gb));
  CYC(0x445f, 0x4460); SET_HL(HL + 1);
  CYC(0x4460, 0x4462); A = 0x41;
  CALL_C(0x4462, checkTreasureObtained_hook, 0x1748, 0x4465);
  if (!(F & FC)) {
    CYCT(0x4465, 0x4467);
    goto skip_first_argument;
  }
  CYC(0x4465, 0x4467);
  CYC(0x4467, 0x4468); B = A;
  CYC(0x4468, 0x4469); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4469, 0x446a); A = alu_dec8(gb, A);
  CYC(0x446a, 0x446b); alu_cp(gb, B);
  if (!(F & FZ)) {
    CYCT(0x446b, 0x446d);
    goto skip_remaining_arguments;
  }
  CYC(0x446b, 0x446d);
  CYC(0x446d, 0x4470); scriptFunc_jump_hook(gb);
  return;
skip_first_argument:
  CYC(0x4470, 0x4471); SET_HL(HL + 1);
skip_remaining_arguments:
  CYC(0x4471, 0x4472); SET_HL(HL + 1);
  CYC(0x4472, 0x4473); SET_HL(HL + 1);
  CYC(0x4473, 0x4474); ret_effect(gb);
}

void scriptCmd_setLinkCantMove_hook(GB *gb) {
  CYC(0x4474, 0x4475); SET_HL(pop_effect(gb));
  CYC(0x4475, 0x4476); SET_HL(HL + 1);
  CYC(0x4476, 0x4477); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4477, 0x447a); W8(wDisabledObjects) = A;
  CYC(0x447a, 0x447b); ret_effect(gb);
}

void scriptCmd_checkCounter2Zero_hook(GB *gb) {
  CYC(0x447b, 0x447c); SET_HL(pop_effect(gb));
  CYC(0x447c, 0x447e); E = 0x47;
  CYC(0x447e, 0x447f); A = mem_rd(gb, DE);
  CYC(0x447f, 0x4480); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4480, 0x4481); ret_effect(gb);
    return;
  }
  CYC(0x4480, 0x4481);
  CYC(0x4481, 0x4482); SET_HL(HL + 1);
  CYC(0x4482, 0x4483); ret_effect(gb);
}

void scriptCmd_setTile_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x4486, 0x4487); C = A;
  CYC(0x4487, 0x4488); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4488, 0x4489); push_effect(gb, HL);
  CALL_C(0x4489, setTile_hook, 0x3a9c, 0x448c);
  CYC(0x448c, 0x448d); SET_HL(pop_effect(gb));
  CYC(0x448d, 0x448e); alu_scf(gb);
  CYC(0x448e, 0x448f); ret_effect(gb);
}

void scriptCmd_setTile_hook(GB *gb) {
  CYC(0x4483, 0x4484); SET_HL(pop_effect(gb));
  CYC(0x4484, 0x4485); SET_HL(HL + 1);
  CYC(0x4485, 0x4486); A = mem_rd(gb, HL); SET_HL(HL + 1);
  scriptCmd_setTile_body_hook(gb);
}

void scriptCmd_setTileHere_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x448f, 0x4490); SET_HL(pop_effect(gb));
  CYC(0x4490, 0x4491); SET_HL(HL + 1);
  CALL_C(0x4491, objectGetShortPosition_hook, 0x2096, 0x4494);
  CYC(0x4494, 0x4496);
  scriptCmd_setTile_body_hook(gb);
}

void scriptCmd_callScript_hook(GB *gb) {
  CYC(0x4496, 0x4497); SET_HL(pop_effect(gb));
  CYC(0x4497, 0x4498); SET_HL(HL + 1);
  CYC(0x4498, 0x4499); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4499, 0x449a); C = A;
  CYC(0x449a, 0x449b); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x449b, 0x449c); B = A;
  CYC(0x449c, 0x449e); E = 0x75;
  CYC(0x449e, 0x449f); A = L;
  CYC(0x449f, 0x44a0); mem_wr(gb, DE, A);
  CYC(0x44a0, 0x44a1); E = alu_inc8(gb, E);
  CYC(0x44a1, 0x44a2); A = H;
  CYC(0x44a2, 0x44a3); mem_wr(gb, DE, A);
  CYC(0x44a3, 0x44a4); L = C;
  CYC(0x44a4, 0x44a5); H = B;
  CYC(0x44a5, 0x44a6); ret_effect(gb);
}

void scriptCmd_ret_hook(GB *gb) {
  CYC(0x44a6, 0x44a7); SET_HL(pop_effect(gb));
  CYC(0x44a7, 0x44a9); E = 0x75;
  CYC(0x44a9, 0x44aa); A = mem_rd(gb, DE);
  CYC(0x44aa, 0x44ab); L = A;
  CYC(0x44ab, 0x44ac); E = alu_inc8(gb, E);
  CYC(0x44ac, 0x44ad); A = mem_rd(gb, DE);
  CYC(0x44ad, 0x44ae); H = A;
  CYC(0x44ae, 0x44af); ret_effect(gb);
}

void scriptCmd_jumpIfCBA5Eq_hook(GB *gb) {
  CYC(0x44b3, 0x44b4); SET_HL(pop_effect(gb));
  CYC(0x44b4, 0x44b5); SET_HL(HL + 1);
  CYC(0x44b5, 0x44b8); A = W8(wSelectedTextOption);
  CYC(0x44b8, 0x44b9); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x44b9, 0x44bb);
    CYC(0x44af, 0x44b0); SET_HL(HL + 1);
    CYC(0x44b0, 0x44b3); scriptFunc_jump_scf_hook(gb);
    return;
  }
  CYC(0x44b9, 0x44bb);
  CYC(0x44bb, 0x44be); scriptFunc_add3ToHl_scf_hook(gb);
}

void scriptCmd_jumpRandom_hook(GB *gb) {
  CYC(0x44be, 0x44bf); SET_HL(pop_effect(gb));
  CYC(0x44bf, 0x44c0); SET_HL(HL + 1);
  CYC(0x44c0, 0x44c3); scriptFunc_jump_scf_hook(gb);
}

void scriptCmd_jumpIfMemorySet_hook(GB *gb) {
  CYC(0x44cc, 0x44cd); SET_HL(pop_effect(gb));
  CYC(0x44cd, 0x44ce); SET_HL(HL + 1);
  CYC(0x44ce, 0x44cf); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x44cf, 0x44d0); B = mem_rd(gb, HL);
  CYC(0x44d0, 0x44d1); C = A;
  CYC(0x44d1, 0x44d2); SET_HL(HL + 1);
  CYC(0x44d2, 0x44d3); A = mem_rd(gb, BC);
  CYC(0x44d3, 0x44d4); alu_and(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x44d4, 0x44d7);
    scriptFunc_add3ToHl_hook(gb);
    return;
  }
  CYC(0x44d4, 0x44d7);
  CYC(0x44d7, 0x44d8); SET_HL(HL + 1);
  CYC(0x44d8, 0x44db); scriptFunc_jump_scf_hook(gb);
}

void scriptCmd_writeC6xx_hook(GB *gb) {
  CYC(0x44db, 0x44dc); SET_HL(pop_effect(gb));
  CYC(0x44dc, 0x44dd); SET_HL(HL + 1);
  CYC(0x44dd, 0x44df); B = 0xc6;
  CYC(0x44df, 0x44e0); C = mem_rd(gb, HL);
  CYC(0x44e0, 0x44e1); SET_HL(HL + 1);
  CYC(0x44e1, 0x44e2); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x44e2, 0x44e3); mem_wr(gb, BC, A);
  CYC(0x44e3, 0x44e4); ret_effect(gb);
}

void scriptCmd_checkCollidedWithLink_body_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x44f0, func_0c_4177_hook, 0x4177, 0x44f3);
  CYC(0x44f3, 0x44f4); SET_HL(HL + 1);
  CYC(0x44f4, 0x44f5); ret_effect(gb);
}

void scriptCmd_checkCollidedWithLink_ignoreZ_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x44e4, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x44e7);
  CYC(0x44e7, 0x44e8); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(0x44e8, 0x44e9); ret_effect(gb);
    return;
  }
  CYC(0x44e8, 0x44e9);
  CYC(0x44e9, 0x44eb);
  scriptCmd_checkCollidedWithLink_body_hook(gb);
}

void scriptCmd_checkCollidedWithLink_onGround_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x44eb, objectCheckCollidedWithLink_onGround_hook, 0x1c35, 0x44ee);
  CYC(0x44ee, 0x44ef); SET_HL(pop_effect(gb));
  if (!(F & FC)) {
    CYCT(0x44ef, 0x44f0); ret_effect(gb);
    return;
  }
  CYC(0x44ef, 0x44f0);
  scriptCmd_checkCollidedWithLink_body_hook(gb);
}

void scriptCmd_checkAButton_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x44f5, 0x44f7); E = 0x71;
  CYC(0x44f7, 0x44f8); A = mem_rd(gb, DE);
  CYC(0x44f8, 0x44f9); alu_or(gb, A);
  CYC(0x44f9, 0x44fa); SET_HL(pop_effect(gb));
  if (F & FZ) {
    CYCT(0x44fa, 0x44fb); ret_effect(gb);
    return;
  }
  CYC(0x44fa, 0x44fb);
  CYC(0x44fb, 0x44fc); alu_xor(gb, A);
  CYC(0x44fc, 0x44fd); mem_wr(gb, DE, A);
  CALL_C(0x44fd, func_0c_4177_hook, 0x4177, 0x4500);
  CYC(0x4500, 0x4501); SET_HL(HL + 1);
  CYC(0x4501, 0x4502); alu_scf(gb);
  CYC(0x4502, 0x4503); ret_effect(gb);
}

void scriptCmd_checkNoEnemies_hook(GB *gb) {
  CYC(0x4503, 0x4504); SET_HL(pop_effect(gb));
  CYC(0x4504, 0x4507); A = mem_rd(gb, 0xcdd1);
  CYC(0x4507, 0x4508); alu_or(gb, A);
  if (!(F & FZ)) {
    CYCT(0x4508, 0x4509); ret_effect(gb);
    return;
  }
  CYC(0x4508, 0x4509);
  CYC(0x4509, 0x450a); SET_HL(HL + 1);
  CYC(0x450a, 0x450b); ret_effect(gb);
}

void scriptCmd_checkFlagSet_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(0x450b, 0x450c); SET_HL(pop_effect(gb));
  CYC(0x450c, 0x450d); push_effect(gb, HL);
  CYC(0x450d, 0x450e); SET_HL(HL + 1);
  CYC(0x450e, 0x450f); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x450f, 0x4510); B = A;
  CYC(0x4510, 0x4511); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4511, 0x4512); H = mem_rd(gb, HL);
  CYC(0x4512, 0x4513); L = A;
  CYC(0x4513, 0x4514); A = B;
  CALL_C(0x4514, checkFlag_hook, 0x0205, 0x4517);
  CYC(0x4517, 0x4518); SET_HL(pop_effect(gb));
  if (F & FZ) {
    CYCT(0x4518, 0x4519); ret_effect(gb);
    return;
  }
  CYC(0x4518, 0x4519);
  CYC(0x4519, 0x451c); SET_BC(0x0004);
  CYC(0x451c, 0x451d); alu_add_hl(gb, BC);
  CYC(0x451d, 0x451e); alu_scf(gb);
  CYC(0x451e, 0x451f); ret_effect(gb);
}

void scriptCmd_checkInteractionByteEq_hook(GB *gb) {
  CYC(0x451f, 0x4520); SET_HL(pop_effect(gb));
  CYC(0x4520, 0x4521); push_effect(gb, HL);
  CYC(0x4521, 0x4522); SET_HL(HL + 1);
  CYC(0x4522, 0x4523); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4523, 0x4524); E = A;
  CYC(0x4524, 0x4525); A = mem_rd(gb, DE);
  CYC(0x4525, 0x4526); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4526, 0x4528);
    goto matched;
  }
  CYC(0x4526, 0x4528);
  CYC(0x4528, 0x4529); SET_HL(pop_effect(gb));
  CYC(0x4529, 0x452a); alu_xor(gb, A);
  CYC(0x452a, 0x452b); ret_effect(gb);
  return;
matched:
  CYC(0x452b, 0x452c); SET_BC(pop_effect(gb));
  CYC(0x452c, 0x452d); SET_HL(HL + 1);
  CYC(0x452d, 0x452e); ret_effect(gb);
}

void scriptCmd_checkMemoryEq_hook(GB *gb) {
  CYC(0x452e, 0x452f); SET_HL(pop_effect(gb));
  CYC(0x452f, 0x4530); push_effect(gb, HL);
  CYC(0x4530, 0x4531); SET_HL(HL + 1);
  CYC(0x4531, 0x4532); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4532, 0x4533); C = A;
  CYC(0x4533, 0x4534); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(0x4534, 0x4535); B = A;
  CYC(0x4535, 0x4536); A = mem_rd(gb, BC);
  CYC(0x4536, 0x4537); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x4537, 0x4539);
    goto matched;
  }
  CYC(0x4537, 0x4539);
  CYC(0x4539, 0x453a); SET_HL(pop_effect(gb));
  CYC(0x453a, 0x453b); alu_xor(gb, A);
  CYC(0x453b, 0x453c); ret_effect(gb);
  return;
matched:
  CYC(0x453c, 0x453d); SET_BC(pop_effect(gb));
  CYC(0x453d, 0x453e); SET_HL(HL + 1);
  CYC(0x453e, 0x453f); ret_effect(gb);
}

void scriptCmd_checkHeartDisplayUpdated_hook(GB *gb) {
  CYC(0x453f, 0x4540); SET_HL(pop_effect(gb));
  CYC(0x4540, 0x4543); A = mem_rd(gb, 0xcbe4);
  CYC(0x4543, 0x4544); B = A;
  CYC(0x4544, 0x4547); A = mem_rd(gb, 0xc6aa);
  CYC(0x4547, 0x4548); alu_cp(gb, B);
  if (F & FZ) {
    CYCT(0x4548, 0x454a);
    goto updated;
  }
  CYC(0x4548, 0x454a);
  CYC(0x454a, 0x454b); alu_xor(gb, A);
  CYC(0x454b, 0x454c); ret_effect(gb);
  return;
updated:
  CYC(0x454c, 0x454d); SET_HL(HL + 1);
  CYC(0x454d, 0x454e); alu_scf(gb);
  CYC(0x454e, 0x454f); ret_effect(gb);
}

void scriptCmd_checkRupeeDisplayUpdated_hook(GB *gb) {
  CYC(0x454f, 0x4552); SET_HL(0xc6ad);
  CYC(0x4552, 0x4555); A = mem_rd(gb, 0xcbe5);
  CYC(0x4555, 0x4556); alu_cp(gb, mem_rd(gb, HL));
  if (!(F & FZ)) {
    CYCT(0x4556, 0x4558);
    goto not_updated;
  }
  CYC(0x4556, 0x4558);
  CYC(0x4558, 0x4559); L = alu_inc8(gb, L);
  CYC(0x4559, 0x455c); A = mem_rd(gb, 0xcbe6);
  CYC(0x455c, 0x455d); alu_cp(gb, mem_rd(gb, HL));
  if (F & FZ) {
    CYCT(0x455d, 0x4560);
    scriptFunc_popHlAndInc_hook(gb);
    return;
  }
  CYC(0x455d, 0x4560);
not_updated:
  CYC(0x4560, 0x4561); SET_HL(pop_effect(gb));
  CYC(0x4561, 0x4562); alu_xor(gb, A);
  CYC(0x4562, 0x4563); ret_effect(gb);
}

void scriptCmd_checkNotCollidedWithLink_ignoreZ_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x4563, objectCheckCollidedWithLink_ignoreZ_hook, 0x1c6f, 0x4566);
  CYC(0x4566, 0x4567); SET_HL(pop_effect(gb));
  if (F & FC) {
    CYCT(0x4567, 0x4569);
    goto collided;
  }
  CYC(0x4567, 0x4569);
  CYC(0x4569, 0x456a); SET_HL(HL + 1);
  CYC(0x456a, 0x456b); ret_effect(gb);
  return;
collided:
  CYC(0x456b, 0x456c); alu_xor(gb, A);
  CYC(0x456c, 0x456d); ret_effect(gb);
}

void scriptCmd_createPuff_hook(GB *gb) {
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(0x456d, objectCreatePuff_hook, 0x24c1, 0x4570);
  CYC(0x4570, 0x4571); SET_HL(pop_effect(gb));
  CYC(0x4571, 0x4572); SET_HL(HL + 1);
  CYC(0x4572, 0x4573); ret_effect(gb);
}
