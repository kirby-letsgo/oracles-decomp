#include "game/game.h"
#include "game/gen.h"
#include "game/seasons/gen.h"

#undef CYC
#undef CYCT
#define CYC(from, to) burn_rom(gb, bk_, (from), (to), false)
#define CYCT(from, to) burn_rom(gb, bk_, (from), (to), true)

// ref/oracles-disasm/object_code/seasons/interactions/linkedMasterDiver.s.
// INTERAC_LINKED_MASTER_DIVER

static uint16_t linkedMasterDiver_jump_table(GB *gb) {
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

// INTERAC_LINKED_MASTER_DIVER
void s_interactionCodecd_hook(GB *gb) {
  BASE(interactionCodecd);
  uint16_t sp0_ = gb->sp; (void)sp0_;
  CYC(b_+0, b_+2); E = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+2, b_+3); A = mem_rd(gb, DE);
  CYC(b_+3, b_+4); push_effect(gb, b_+4);
  do { uint16_t jt_ = (linkedMasterDiver_jump_table(gb));
    if (jt_ == b_+10) goto state0;
    if (jt_ == b_+154) goto state1;
    if (jt_ == b_+165) goto state2;
    HANDOFF(HL);
  } while (0);
state0:
  CYC(b_+10, b_+12); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+12, b_+13); A = mem_rd(gb, DE);
  CYC(b_+13, b_+14); push_effect(gb, b_+14);
  do { uint16_t jt_ = (linkedMasterDiver_jump_table(gb));
    if (jt_ == b_+20) goto subid0;
    if (jt_ == b_+101) goto subid1;
    if (jt_ == b_+136) goto subid2;
    HANDOFF(HL);
  } while (0);
subid0:
  CYC(b_+20, b_+22); A = 0x28;
  CALL_C(b_+22, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+25);
  if (F & FZ) { CYCT(b_+25, b_+28); TAIL(interactionDelete); }
  CYC(b_+25, b_+28);
  CALL_C(b_+28, s_interactionInitGraphics, SYM(interactionInitGraphics), b_+31);
  CALL_C(b_+31, s_interactionIncState, SYM(interactionIncState), b_+34);
  CYC(b_+34, b_+36); A = 0x4c;
  CALL_C(b_+36, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+39);
  CYC(b_+39, b_+41); A = 0x5d;
  CALL_C(b_+41, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+44);
  if (F & FZ) { CYCT(b_+44, b_+47); goto notDoneDiverSecret; }
  CYC(b_+44, b_+47);
  CYC(b_+47, b_+50); SET_HL(b_+157);
  CYC(b_+50, b_+52);
  goto setScript;
notDoneDiverSecret:
  CYC(b_+52, b_+54); A = 0x07;
  CYC(b_+54, b_+56); B = 0xea;
  CALL_C(b_+56, s_getRoomFlags, SYM(getRoomFlags), b_+59);
  CYC(b_+59, b_+61); alu_and(gb, 0x40);
  if (F & FZ) { CYCT(b_+61, b_+63); goto L_7e15; }
  CYC(b_+61, b_+63);
  CYC(b_+63, b_+65); mem_wr(gb, HL, mem_rd(gb, HL) & 0xbf);
  CYC(b_+65, b_+68); SET_HL(b_+121);
  CYC(b_+68, b_+70);
  goto setScript;
L_7e15:
  CYC(b_+70, b_+72); A = 0x53;
  CALL_C(b_+72, s_checkGlobalFlag, SYM(checkGlobalFlag), b_+75);
  CYC(b_+75, b_+78); SET_HL(SYM(masterDiverScript_beginningSecret));
  if (F & FZ) { CYCT(b_+78, b_+80); goto setScript; }
  CYC(b_+78, b_+80);
  CYC(b_+80, b_+83); SET_HL(b_+19);
setScript:
  CALL_C(b_+83, s_interactionSetScript, SYM(interactionSetScript), b_+86);
  CYC(b_+86, b_+87); alu_xor(gb, A);
  CYC(b_+87, b_+90); SET_HL(wTmpcfc0 + 0x10);
  CYC(b_+90, b_+91); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+91, b_+92); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+92, b_+93); mem_wr(gb, HL, A); SET_HL(HL + 1);
  CYC(b_+93, b_+95); A = 0x02;
  CALL_C(b_+95, s_interactionSetAnimation, SYM(interactionSetAnimation), b_+98);
  CYC(b_+98, b_+101);
  TAIL(interactionAnimateAsNpc);
subid1:
  CYC(b_+101, b_+104); SET_HL(wTmpcfc0 + 0x11);
  CYC(b_+104, b_+105); A = mem_rd(gb, HL);
  CYC(b_+105, b_+106); alu_or(gb, A);
  if (!(F & FZ)) { CYCT(b_+106, b_+109); TAIL(interactionDelete); }
  CYC(b_+106, b_+109);
  CYC(b_+109, b_+110); mem_wr(gb, HL, alu_inc8(gb, mem_rd(gb, HL)));
  CYC(b_+110, b_+111); H = D;
  CYC(b_+111, b_+113); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+113, b_+115); mem_wr(gb, HL, 0x02);
  CYC(b_+115, b_+117); A = 0x2e;
  CALL_C(b_+117, s_unsetGlobalFlag, SYM(unsetGlobalFlag), b_+120);
  CYC(b_+120, b_+122); A = 0x4c;
  CALL_C(b_+122, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+125);
  CYC(b_+125, b_+128); SET_HL(b_+64);
  CALL_C(b_+128, s_interactionSetScript, SYM(interactionSetScript), b_+131);
  CALL_C(b_+131, s_objectSetReservedBit1, SYM(objectSetReservedBit1), b_+134);
  CYC(b_+134, b_+136);
  goto state2;
subid2:
  CYC(b_+136, b_+137); H = D;
  CYC(b_+137, b_+139); L = INTERACTION_BASE + OBJ_STATE;
  CYC(b_+139, b_+141); mem_wr(gb, HL, 0x02);
  CYC(b_+141, b_+143); A = 0x4c;
  CALL_C(b_+143, s_interactionSetHighTextIndex, SYM(interactionSetHighTextIndex), b_+146);
  CYC(b_+146, b_+149); SET_HL(b_+160);
  CALL_C(b_+149, s_interactionSetScript, SYM(interactionSetScript), b_+152);
  CYC(b_+152, b_+154);
  goto state2;
state1:
  CALL_C(b_+154, s_interactionRunScript, SYM(interactionRunScript), b_+157);
  CYC(b_+157, b_+159); E = INTERACTION_BASE + OBJ_VAR3F;
  CYC(b_+159, b_+160); A = mem_rd(gb, DE);
  CYC(b_+160, b_+161); alu_or(gb, A);
  if (!(F & FZ)) { RET_TAKEN(b_+161); return; }
  CYC(b_+161, b_+162);
  CYC(b_+162, b_+165);
  TAIL(interactionAnimateAsNpc);
state2:
  CYC(b_+165, b_+167); E = INTERACTION_BASE + OBJ_SUBID;
  CYC(b_+167, b_+168); A = mem_rd(gb, DE);
  CYC(b_+168, b_+169); A = alu_dec8(gb, A);
  if (!(F & FZ)) { CYCT(b_+169, b_+171); goto L_7e82; }
  CYC(b_+169, b_+171);
  CYC(b_+171, b_+174); SET_HL(SYM(func_79df));
  CYC(b_+174, b_+176); E = 0x0f;
  CALL_C(b_+176, s_interBankCall, SYM(interBankCall), b_+179);
L_7e82:
  CALL_C(b_+179, s_interactionRunScript, SYM(interactionRunScript), b_+182);
  if (F & FC) { CYCT(b_+182, b_+185); TAIL(interactionDelete); }
  CYC(b_+182, b_+185);
  RET(b_+185); return;
}
