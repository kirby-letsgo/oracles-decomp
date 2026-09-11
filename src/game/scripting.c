#include "game/game.h"
#include "game/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, 0x0c, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, 0x0c, (from), (to), true)

void scriptFunc_checkRoomFlag_hook(GB *gb);

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
    scriptFunc_popHlAndInc(gb);
    return;
  }
  CYC(0x4113, 0x4116);
  CYC(0x4116, 0x4117); SET_HL(pop_effect(gb));
  CYC(0x4117, 0x411a); SET_HL(0x45ef);
  CYC(0x411a, 0x411b); alu_scf(gb);
  CYC(0x411b, 0x411c); ret_effect(gb);
}
