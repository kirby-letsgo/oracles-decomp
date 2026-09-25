#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/blainoScript.s.
// INTERAC_BLAINO_SCRIPT

static uint16_t blainoScript_jump_table(GB *gb) {
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

// interactionCode5a@checkOutsideRing
static void blainoScript_checkOutsideRing(GB *gb) {
  BASE(interactionCode5a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+136, b_+137); A = mem_rd(gb, HL); SET_HL(HL + 1);
  CYC(b_+137, b_+139); alu_sub(gb, 0x16);
  CYC(b_+139, b_+141); alu_cp(gb, 0x4c);
  if (!(F & FC)) { RET_TAKEN(b_+141); return; }
  CYC(b_+141, b_+142);
  CYC(b_+142, b_+143); L = alu_inc8(gb, L);
  CYC(b_+143, b_+144); A = mem_rd(gb, HL);
  CYC(b_+144, b_+146); alu_sub(gb, 0x22);
  CYC(b_+146, b_+148); alu_cp(gb, 0x5c);
  RET(b_+148); return;
}

// interactionCode5a@checkFightDone
static void blainoScript_checkFightDone(GB *gb) {
  BASE(interactionCode5a);
  uint16_t sp0_ = cpu_sp(gb); (void)sp0_;
  CYC(b_+95, b_+98); SET_HL(wInventoryB);
  CYC(b_+98, b_+101); A = mem_rd(gb, wLinkPlayingInstrument);
  CYC(b_+101, b_+102); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+102, b_+103); L = alu_inc8(gb, L);
  CYC(b_+103, b_+104); alu_or(gb, mem_rd(gb, HL));
  CYC(b_+104, b_+106); A = 0x03;
  if (!(F & FZ)) { CYCT(b_+106, b_+108); goto L_7f13; }
  CYC(b_+106, b_+108);
  CYC(b_+108, b_+110); A = 0x0b;
  CALL_C(b_+110, s_objectGetRelatedObject1Var, SYM(objectGetRelatedObject1Var), b_+113);
  CALL_L(b_+113, blainoScript_checkOutsideRing, b_+116);
  CYC(b_+116, b_+118); A = 0x01;
  if (!(F & FC)) { CYCT(b_+118, b_+120); goto L_7f13; }
  CYC(b_+118, b_+120);
  CYC(b_+120, b_+123); SET_HL(w1Link_yh);
  CALL_L(b_+123, blainoScript_checkOutsideRing, b_+126);
  CYC(b_+126, b_+128); A = 0x02;
  if (!(F & FC)) { CYCT(b_+128, b_+130); goto L_7f13; }
  CYC(b_+128, b_+130);
  CYC(b_+130, b_+131); alu_xor(gb, A);
L_7f13:
  CYC(b_+131, b_+134); mem_wr(gb, wShootingGalleryccd5, A);
  CYC(b_+134, b_+135); alu_or(gb, A);
  RET(b_+135); return;
}

// INTERAC_BLAINO_SCRIPT
void s_interactionCode5a_hook(GB *gb) {
  BASE(interactionCode5a);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (blainoScript_jump_table(gb));
    if (jt_ == b_+12) goto state0;
    if (jt_ == b_+29) goto state1;
    if (jt_ == b_+56) goto state2;
    if (jt_ == SYM(interactionRunScript) && hook_is(gb, SYM(interactionRunScript), s_interactionRunScript)) { s_interactionRunScript(gb); return; }
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+12, b_+14); A = 0x01;
  CYC(b_+14, b_+15); mem_wr(gb, DE, A);
  CALL_C(b_+15, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+18);
  CYC(b_+18, b_+20); A = 0x23;
  CALL_C(b_+20, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+23);
  CYC(b_+23, b_+26); SET_HL(SYM(blainoScript));
  CALL_C(b_+26, s_interactionSetScript, SYM(interactionSetScript), b_+29);
state1:
  CALL_C(b_+29, s_interactionRunScript, SYM(interactionRunScript), b_+32);
  if (!(F & FC)) { RET_TAKEN(b_+32); return; }
  CYC(b_+32, b_+33);
  CYC(b_+33, b_+34); H = D;
  CYC(b_+34, b_+36); L = INTERACTION_BASE + OBJ_ENABLED;
  CYC(b_+36, b_+38); mem_wr(gb, HL, 0x01);
  CYC(b_+38, b_+40); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+40, b_+42); mem_wr(gb, HL, 0x02);
  CYC(b_+42, b_+44); A = 0x02;
  CYC(b_+44, b_+47); mem_wr(gb, wShootingGalleryBallStatus, A);
  CYC(b_+47, b_+48); alu_xor(gb, A);
  CYC(b_+48, b_+51); mem_wr(gb, wShootingGalleryccd5, A);
  CYC(b_+51, b_+52); A = alu_inc8(gb, A);
  CYC(b_+52, b_+55); mem_wr(gb, wInBoxingMatch, A);
  RET(b_+55); return;
state2:
  CALL_L(b_+56, blainoScript_checkFightDone, b_+59);
  if (F & FZ) { RET_TAKEN(b_+59); return; }
  CYC(b_+59, b_+60);
  CALL_C(b_+60, s_restartSound, SYM(restartSound), b_+63);
  CALL_C(b_+63, s_interactionSetAlwaysUpdateBit, SYM(interactionSetAlwaysUpdateBit), b_+66);
  CYC(b_+66, b_+68); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+68, b_+70); mem_wr(gb, HL, 0x03);
  CYC(b_+70, b_+72); A = 0x03;
  CYC(b_+72, b_+75); mem_wr(gb, wShootingGalleryBallStatus, A);
  CYC(b_+75, b_+76); alu_xor(gb, A);
  CYC(b_+76, b_+79); mem_wr(gb, wLinkPlayingInstrument, A);
  CALL_C(b_+79, s_resetLinkInvincibility, SYM(resetLinkInvincibility), b_+82);
  CYC(b_+82, b_+83); alu_xor(gb, A);
  CYC(b_+83, b_+86); mem_wr(gb, wInBoxingMatch, A);
  CYC(b_+86, b_+89); SET_HL(SYM(blainoFightDoneScript));
  CALL_C(b_+89, s_interactionSetScript, SYM(interactionSetScript), b_+92);
  CYC(b_+92, b_+95);
  TAIL(interactionRunScript);
}
