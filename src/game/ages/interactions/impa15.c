#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

void impa_moveLinkUp32Frames_hook(GB *gb);
void impa_moveLinkRight8Frames_hook(GB *gb);
void impa_restoreNormalSpriteSheet_hook(GB *gb);
void impa_showZeldaKidnappedTextNonExitable_hook(GB *gb);

static void impa_setLinkAnimState0b(GB *gb) {
  BASE(impa_moveLinkRight8Frames);
  CYC(b_+10, b_+12); A = 0x0b;
  CYC(b_+12, b_+15); mem_wr(gb, wLinkForceState, A);
  RET(b_+15);
}

void impa_moveLinkUp32Frames_hook(GB *gb) {
  BASE(impa_moveLinkUp32Frames);
  CYC(b_+0, b_+2); A = 0x20;
  CYC(b_+2, b_+5); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+5, b_+6); alu_xor(gb, A);
  CYC(b_+6, b_+9); mem_wr(gb, w1Link_angle, A);
  CYC(b_+9, b_+12); mem_wr(gb, w1Link_direction, A);
  CYC(b_+12, b_+14);
  impa_setLinkAnimState0b(gb);
}

void impa_moveLinkRight8Frames_hook(GB *gb) {
  BASE(impa_moveLinkRight8Frames);
  CYC(b_+0, b_+2); A = 0x08;
  CYC(b_+2, b_+5); mem_wr(gb, wLinkStateParameter, A);
  CYC(b_+5, b_+7); A = 0x08;
  CYC(b_+7, b_+10); mem_wr(gb, w1Link_angle, A);
  impa_setLinkAnimState0b(gb);
}

void impa_restoreNormalSpriteSheet_hook(GB *gb) {
  BASE(impa_restoreNormalSpriteSheet);
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_VAR3B;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+5); E = INTERACTION_BASE + OBJ_OAM_TILE_INDEX_BASE;
  CYC(b_+5, b_+6); mem_wr(gb, DE, A);
  CYC(b_+6, b_+8); E = INTERACTION_BASE + OBJ_OAM_FLAGS;
  CYC(b_+8, b_+10); A = 0x02;
  CYC(b_+10, b_+11); mem_wr(gb, DE, A);
  RET(b_+11);
}

void impa_showZeldaKidnappedTextNonExitable_hook(GB *gb) {
  BASE(impa_showZeldaKidnappedTextNonExitable);
  CYC(b_+0, b_+3); SET_BC(0x0131);
  CYC(b_+3, b_+6); showTextNonExitable_hook(gb);
}
