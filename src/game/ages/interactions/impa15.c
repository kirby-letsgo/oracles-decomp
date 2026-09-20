#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x15, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x15, (from), (to), true)

void impa_moveLinkUp32Frames_hook(GB *gb);
void impa_moveLinkRight8Frames_hook(GB *gb);
void impa_restoreNormalSpriteSheet_hook(GB *gb);
void impa_showZeldaKidnappedTextNonExitable_hook(GB *gb);

static void impa_setLinkAnimState0b(GB *gb) {
  CYC(0x52fa, 0x52fc); A = 0x0b;
  CYC(0x52fc, 0x52ff); mem_wr(gb, wLinkForceState, A);
  RET(0x52ff);
}

void impa_moveLinkUp32Frames_hook(GB *gb) {
  CYC(0x52e2, 0x52e4); A = 0x20;
  CYC(0x52e4, 0x52e7); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x52e7, 0x52e8); alu_xor(gb, A);
  CYC(0x52e8, 0x52eb); mem_wr(gb, w1Link_angle, A);
  CYC(0x52eb, 0x52ee); mem_wr(gb, w1Link_direction, A);
  CYC(0x52ee, 0x52f0);
  impa_setLinkAnimState0b(gb);
}

void impa_moveLinkRight8Frames_hook(GB *gb) {
  CYC(0x52f0, 0x52f2); A = 0x08;
  CYC(0x52f2, 0x52f5); mem_wr(gb, wLinkStateParameter, A);
  CYC(0x52f5, 0x52f7); A = 0x08;
  CYC(0x52f7, 0x52fa); mem_wr(gb, w1Link_angle, A);
  impa_setLinkAnimState0b(gb);
}

void impa_restoreNormalSpriteSheet_hook(GB *gb) {
  CYC(0x5300, 0x5302); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(0x5302, 0x5303); A = mem_rd(gb, DE);
  CYC(0x5303, 0x5305); E = INTERACTION_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(0x5305, 0x5306); mem_wr(gb, DE, A);
  CYC(0x5306, 0x5308); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(0x5308, 0x530a); A = 0x02;
  CYC(0x530a, 0x530b); mem_wr(gb, DE, A);
  RET(0x530b);
}

void impa_showZeldaKidnappedTextNonExitable_hook(GB *gb) {
  CYC(0x530c, 0x530f); SET_BC(0x0131);
  CYC(0x530f, 0x5312); showTextNonExitable_hook(gb);
}
