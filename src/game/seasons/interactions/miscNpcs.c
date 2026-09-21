#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/miscNpcs.s, bank $08.

// The shared "run the script, then animate" state of the Horon Village NPCs: an NPC whose
// Interaction.var03 has bit 7 set animates on its own, the others face Link.
void s_miscNPC_state1_hook(GB *gb) {
  BASE(miscNPC_state1);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CALL_C(b_+0, interactionRunScript_hook, SYM(interactionRunScript), b_+3);
  CYC(b_+3, b_+5); E = 0x43; // Interaction.var03
  CYC(b_+5, b_+6); A = mem_rd(gb, DE);
  CYC(b_+6, b_+8); alu_and(gb, 0x80);
  if (!(F & FZ)) { CYCT(b_+8, b_+11); TAIL(interactionAnimateAsNpc); }
  CYC(b_+8, b_+11);
  CYC(b_+11, b_+14);
  TAIL(npcFaceLinkAndAnimate);
}
