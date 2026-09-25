#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/holly.s.
// INTERAC_HOLLY

// INTERAC_HOLLY
void s_interactionCode70_hook(GB *gb) {
  BASE(interactionCode70);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+4, b_+6); goto subid1; }
  CYC(b_+4, b_+6);
  CALL_C(b_+6, s_checkInteractionState, SYM(checkInteractionState), b_+9);
  if (!(F & FZ)) { CYCT(b_+9, b_+11); goto state1; }
  CYC(b_+9, b_+11);
  CYC(b_+11, b_+13); A = 0x01;
  CYC(b_+13, b_+14); mem_wr(gb, DE, A);
  CYC(b_+14, b_+17); A = mem_rd(gb, wWarpDestPos);
  CYC(b_+17, b_+19); alu_cp(gb, 0x04);
  CYC(b_+19, b_+22); SET_HL(SYM(hollyScript_enteredFromChimney));
  if (F & FZ) { CYCT(b_+22, b_+24); goto setScript; }
  CYC(b_+22, b_+24);
  CYC(b_+24, b_+27); SET_HL(SYM(hollyScript_enteredNormally));
setScript:
  CALL_C(b_+27, s_interactionSetScript, SYM(interactionSetScript), b_+30);
  CALL_C(b_+30, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+33);
  CYC(b_+33, b_+36);
  TAIL(objectSetVisiblec2);
state1:
  CALL_C(b_+36, s_interactionRunScript, SYM(interactionRunScript), b_+39);
  CYC(b_+39, b_+41); C = 0x0e;
  CALL_C(b_+41, s_objectUpdateSpeedZ_paramC, SYM(objectUpdateSpeedZ_paramC), b_+44);
  CYC(b_+44, b_+47);
  TAIL(npcFaceLinkAndAnimate);
subid1:
  CALL_C(b_+47, s_returnIfScrollMode01Unset, SYM(returnIfScrollMode01Unset), b_+50);
  CALL_C(b_+50, s_interactionDeleteAndRetIfEnabled02, SYM(interactionDeleteAndRetIfEnabled02), b_+53);
  CYC(b_+53, b_+55); A = 0xd9;
  CALL_C(b_+55, s_findTileInRoom, SYM(findTileInRoom), b_+58);
  if (!(F & FZ)) { CYCT(b_+58, b_+60); goto L_6c84; }
  CYC(b_+58, b_+60);
  CYC(b_+60, b_+62); B = 0x00;
L_6c78:
  CYC(b_+62, b_+63); B = alu_inc8(gb, B);
  CYC(b_+63, b_+64); L = alu_dec8(gb, L);
  CALL_C(b_+64, s_backwardsSearch, SYM(backwardsSearch), b_+67);
  if (F & FZ) { CYCT(b_+67, b_+69); goto L_6c78; }
  CYC(b_+67, b_+69);
  CYC(b_+69, b_+70); A = B;
  CYC(b_+70, b_+72); alu_cp(gb, 0x04);
  if (F & FZ) { CYCT(b_+72, b_+74); goto L_6c89; }
  CYC(b_+72, b_+74);
L_6c84:
  CYC(b_+74, b_+76); A = 0x25;
  CYC(b_+76, b_+79);
  TAIL(setGlobalFlag);
L_6c89:
  CYC(b_+79, b_+81); A = 0x25;
  CYC(b_+81, b_+84);
  TAIL(unsetGlobalFlag);
}
